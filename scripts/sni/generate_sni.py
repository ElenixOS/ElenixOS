#!/usr/bin/env python3
"""Single non-interactive LVGL to SNI binding generator and analysis CLI."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from sni.core.diagnostics import render_human_analysis, render_json
from sni.core.pipeline import (
    PipelineError,
    ProgressReporter,
    build_pipeline,
    default_paths,
    generate_and_write,
    write_outputs_atomically,
)
from sni.core.validation import has_errors


def add_common_arguments(parser: argparse.ArgumentParser, allow_refresh: bool = False) -> None:
    parser.add_argument("--lvgl-json", type=Path, help="LVGL gen_json metadata file")
    parser.add_argument("--config", type=Path, help="SNI binding configuration JSON")
    parser.add_argument("--lvgl-version-header", type=Path, help="LVGL version header")
    parser.add_argument("--output-dir", type=Path, help="Directory for generated SNI outputs")
    parser.add_argument("--quiet", action="store_true", help="Suppress stage progress")
    parser.add_argument("--verbose", action="store_true", help="Print detailed resolver and emitter diagnostics to stderr")
    parser.add_argument("--format", choices=("text", "json"), default="text", help="Output format; JSON mode keeps stdout machine-readable")
    if allow_refresh:
        parser.add_argument("--refresh-lvgl-json", action="store_true", help="Regenerate lvgl.json with the repository's lv_conf.h first")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Analyze, validate and generate ElenixOS SNI bindings from LVGL metadata")
    subparsers = parser.add_subparsers(dest="command", required=True)
    for command in ("analyze", "validate", "dump-ir", "update-config"):
        child = subparsers.add_parser(command)
        add_common_arguments(child)
    generate = subparsers.add_parser("generate")
    add_common_arguments(generate, allow_refresh=True)
    return parser


def resolved_paths(args: argparse.Namespace) -> dict[str, Path]:
    paths = default_paths(Path(__file__))
    if args.lvgl_json:
        paths["lvgl_json"] = args.lvgl_json.resolve()
    if args.config:
        paths["config"] = args.config.resolve()
    if args.lvgl_version_header:
        paths["version_header"] = args.lvgl_version_header.resolve()
    if args.output_dir:
        output_dir = args.output_dir.resolve()
        paths["type_ids"] = output_dir / "sni_type_ids.h"
        paths["lv_types"] = output_dir / "sni_lv_types.c"
        paths["api"] = output_dir / "sni_api_lv.c"
    return paths


def report_diagnostics(diagnostics: list[dict[str, Any]], verbose: bool) -> None:
    visible = [
        item
        for item in diagnostics
        if item.get("severity") in {"warning", "error", "info"}
        and (verbose or item.get("severity") != "info")
    ]
    limit = len(visible) if verbose else 20
    for diagnostic in visible[:limit]:
        print(f"{diagnostic['severity'].upper()} {diagnostic.get('code', '')}: {diagnostic.get('message', '')}", file=sys.stderr)
    if len(visible) > limit:
        print(f"... {len(visible) - limit} additional diagnostics are in the analysis JSON (use --verbose for details).", file=sys.stderr)


def emit_result(args: argparse.Namespace, result: dict[str, Any]) -> None:
    if args.format == "json":
        print(json.dumps(result, ensure_ascii=False, sort_keys=True, indent=2))
    else:
        for key, value in result.items():
            if isinstance(value, list):
                print(f"{key}:")
                for item in value:
                    print(f"  {item}")
            else:
                print(f"{key}: {value}")


def run(args: argparse.Namespace) -> int:
    paths = resolved_paths(args)
    if getattr(args, "refresh_lvgl_json", False) and not args.lvgl_json:
        paths["lvgl_json"] = paths["repo_root"] / "build" / "lvgl-api" / "lvgl.json"
    stages = 10 if args.command == "generate" else (8 if args.command == "update-config" else 7)
    reporter = ProgressReporter(quiet=args.quiet)
    try:
        pipeline = build_pipeline(
            paths,
            reporter,
            refresh_metadata=bool(getattr(args, "refresh_lvgl_json", False)),
            validation_stages=stages,
        )
        diagnostics = pipeline.ir.diagnostics
        report_diagnostics(diagnostics, args.verbose)
        if args.command == "analyze":
            if args.format == "json":
                print(render_json(pipeline.analysis), end="")
            else:
                print(render_human_analysis(pipeline.analysis))
            return 0

        if args.command == "dump-ir":
            print(render_json(pipeline.analysis), end="")
            return 0

        if args.command == "validate":
            errors = has_errors(diagnostics)
            if args.format == "json":
                emit_result(args, {"valid": not errors, "diagnostics": diagnostics, "summary": pipeline.analysis["analysis"]["api_counts"]})
            else:
                print("Validation failed." if errors else "Validation completed successfully.")
            return 1 if errors else 0

        if args.command == "update-config":
            config_data = pipeline.config.data
            declarations = config_data.setdefault("type_declarations", {})
            discovered = sorted(
                info.name
                for info in pipeline.ir.types.values()
                if info.representation.value == "unknown" and info.referenced_by and info.name not in declarations
            )
            for name in discovered:
                declarations[name] = {"kind": "unknown", "reason": "Awaiting human SNI binding review."}
            content = json.dumps(config_data, ensure_ascii=False, indent=2) + "\n"
            reporter.stage("Writing binding configuration")
            write_outputs_atomically({paths["config"]: content})
            if args.format == "json":
                emit_result(args, {"updated": bool(discovered), "added_unknown_types": discovered, "config": str(paths["config"])})
            else:
                print(f"Added {len(discovered)} unknown type review item(s) to {paths['config']}." if discovered else "No new unresolved types to add.")
            return 0

        if args.command == "generate":
            outputs = generate_and_write(pipeline, paths, reporter, verbose=args.verbose)
            if args.format == "json":
                emit_result(args, {"status": "success", "outputs": [str(path) for path in outputs], "analysis": pipeline.analysis["analysis"]})
            else:
                print("Generation completed successfully.")
                print(f"Generated {len(outputs)} files: " + ", ".join(path.name for path in outputs))
            return 0

        raise PipelineError("Command dispatch", f"unsupported command {args.command}")
    except PipelineError as exc:
        diagnostic_items = exc.diagnostics
        if not diagnostic_items:
            diagnostic_items = [{"severity": "error", "code": "GENERATOR_FAILURE", "message": str(exc)}]
        if args.format == "json":
            print(json.dumps({"status": "error", "phase": exc.phase, "error": str(exc), "diagnostics": diagnostic_items}, ensure_ascii=False, sort_keys=True, indent=2))
        else:
            print(f"Generation failed during {exc.phase}: {exc}", file=sys.stderr)
            report_diagnostics(diagnostic_items, True)
        return 1
    except (OSError, ValueError, SystemExit) as exc:
        if args.format == "json":
            print(json.dumps({"status": "error", "phase": reporter.current, "error": str(exc)}, ensure_ascii=False, sort_keys=True, indent=2))
        else:
            print(f"Generation failed during {reporter.current}: {exc}", file=sys.stderr)
        return 1
    except Exception as exc:
        if args.format == "json":
            print(json.dumps({"status": "error", "phase": reporter.current, "error": f"{type(exc).__name__}: {exc}"}, ensure_ascii=False, sort_keys=True, indent=2))
        else:
            print(f"Generation failed during {reporter.current}: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 1


def main() -> int:
    parser = build_parser()
    return run(parser.parse_args())


if __name__ == "__main__":
    raise SystemExit(main())
