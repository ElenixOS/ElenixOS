#!/usr/bin/env python3
"""Single non-interactive LVGL to SNI binding generator and analysis CLI."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from sni.cli.reporters import JsonReporter, ProgressReporter, TerminalReporter
from sni.core.ir import Diagnostic, Severity
from sni.core.pipeline import PipelineError, PipelineResult, build_pipeline, default_paths, generate_and_write, write_outputs_atomically
from sni.core.result import CommandResult
from sni.core.validation import has_errors


PIPELINE_STAGES = [
    "Loading LVGL metadata",
    "Loading SNI binding configuration",
    "Selecting API surface",
    "Collecting referenced types",
    "Resolving C types and inferring SNI representations",
    "Validating bindings",
    "Building structured analysis result",
]
STAGE_PLANS = {
    "analyze": PIPELINE_STAGES,
    "validate": PIPELINE_STAGES,
    "dump-ir": PIPELINE_STAGES + ["Preparing IR snapshot"],
    "update-config": PIPELINE_STAGES + ["Finding unresolved types", "Preparing configuration changes", "Writing binding configuration"],
    "generate": PIPELINE_STAGES + ["Rendering generated sources", "Writing output files", "Verifying generated artifacts"],
}
STATUS_CATEGORIES = {
    "generic",
    "special",
    "blacklist",
    "unsupported",
    "special-required",
    "lifecycle",
    "unresolved",
}
DIAGNOSTIC_CATEGORIES = {"warnings", "errors", "resolution", "configuration", "validation"}


def _add_output_options(parser: argparse.ArgumentParser, child: bool = False) -> None:
    default = argparse.SUPPRESS if child else None
    parser.add_argument("--quiet", action="store_true", default=default, help="Suppress normal progress output")
    parser.add_argument("--verbose", action="store_true", default=default, help="Show additional stage and emitter details")
    parser.add_argument("--details", action="store_true", default=default, help="Expand grouped diagnostics")
    parser.add_argument("--format", choices=("text", "json"), default=default, help="Output format; JSON stdout is a single machine-readable document")
    parser.add_argument("--no-color", action="store_true", default=default, help="Disable terminal colors")


def add_command_arguments(parser: argparse.ArgumentParser, allow_refresh: bool = False) -> None:
    parser.add_argument("--lvgl-json", type=Path, help="LVGL gen_json metadata file")
    parser.add_argument("--config", type=Path, help="SNI binding configuration JSON")
    parser.add_argument("--lvgl-version-header", type=Path, help="LVGL version header")
    parser.add_argument("--output-dir", type=Path, help="Directory for generated SNI outputs")
    parser.add_argument(
        "--category",
        choices=sorted(STATUS_CATEGORIES | DIAGNOSTIC_CATEGORIES),
        help="Show one API status or diagnostic category",
    )
    _add_output_options(parser, child=True)
    if allow_refresh:
        parser.add_argument("--refresh-lvgl-json", action="store_true", help="Regenerate lvgl.json with the repository's lv_conf.h first")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Analyze, validate and generate ElenixOS SNI bindings from LVGL metadata")
    _add_output_options(parser)
    subparsers = parser.add_subparsers(dest="command", required=True)
    for command in ("analyze", "validate", "dump-ir", "update-config"):
        child = subparsers.add_parser(command)
        add_command_arguments(child)
    analyze = subparsers.choices["analyze"]
    drilldown = analyze.add_mutually_exclusive_group()
    drilldown.add_argument("--api", help="Show one selected API and its signature resolution")
    drilldown.add_argument("--type", dest="type_name", help="Show one resolved C type and its affected APIs")
    generate = subparsers.add_parser("generate")
    add_command_arguments(generate, allow_refresh=True)
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


def _diagnostic_counts(diagnostics: list[Diagnostic]) -> dict[str, int]:
    return {
        "errors": sum(item.severity == Severity.ERROR for item in diagnostics),
        "warnings": sum(item.severity == Severity.WARNING for item in diagnostics),
        "info": sum(item.severity == Severity.INFO for item in diagnostics),
        "unused_configuration": sum(item.code.startswith("UNUSED_") for item in diagnostics),
    }


def _analysis_summary(pipeline: PipelineResult) -> dict[str, Any]:
    counts = pipeline.analysis["analysis"]["api_counts"]
    diagnostics = _diagnostic_counts(pipeline.ir.diagnostics)
    return {
        **counts,
        **diagnostics,
        "type_counts": pipeline.analysis["analysis"]["type_counts"],
    }


def _analysis_view(pipeline: PipelineResult, args: argparse.Namespace) -> dict[str, Any]:
    analysis = pipeline.analysis
    view: dict[str, Any] = {"details": args.details or args.verbose, "category": args.category}
    if args.api:
        api = next((item for item in analysis["apis"] if item["name"] == args.api), None)
        if api is None:
            raise ValueError(f"API is not a selected candidate: {args.api}")
        view["api_detail"] = {**api, "uses": [item for item in analysis["uses"] if item["function"] == args.api]}
    if args.type_name:
        info = next(
            (item for item in analysis["types"] if item["name"] == args.type_name or item["canonical_name"] == args.type_name),
            None,
        )
        if info is None:
            raise ValueError(f"Type is not present in the selected binding analysis: {args.type_name}")
        view["type_detail"] = info
    return view


def _query_metadata(args: argparse.Namespace) -> dict[str, str]:
    query: dict[str, str] = {}
    if args.category:
        query["category"] = args.category
    if args.command == "analyze" and args.api:
        query["api"] = args.api
    if args.command == "analyze" and args.type_name:
        query["type"] = args.type_name
    return query


def _create_reporter(args: argparse.Namespace, stages: list[str]) -> ProgressReporter:
    if args.format == "json":
        return JsonReporter(stages, verbose=args.verbose)
    return TerminalReporter(stages, quiet=args.quiet, verbose=args.verbose, no_color=args.no_color)


def _failure_result(command: str, phase: str, error: str, diagnostics: list[Diagnostic] | None = None) -> CommandResult:
    items = diagnostics or [
        Diagnostic(
            code="COMMAND_FAILURE",
            severity=Severity.ERROR,
            category="runtime",
            subject_kind="command",
            subject=command,
            reason=error,
            details={"phase": phase},
        )
    ]
    return CommandResult(command, False, _diagnostic_counts(items), items, {"error": error}, phase=phase)


def _validate_category(command: str, category: str | None) -> None:
    if not category:
        return
    if command in {"analyze", "validate", "generate", "update-config"}:
        return
    raise ValueError(f"--category has no meaning for {command}")


def run(args: argparse.Namespace) -> int:
    paths = resolved_paths(args)
    if getattr(args, "refresh_lvgl_json", False) and not args.lvgl_json:
        paths["lvgl_json"] = paths["repo_root"] / "build" / "lvgl-api" / "lvgl.json"
    reporter = _create_reporter(args, STAGE_PLANS[args.command])
    try:
        _validate_category(args.command, args.category)
        pipeline = build_pipeline(
            paths,
            reporter,
            refresh_metadata=bool(getattr(args, "refresh_lvgl_json", False)),
            fail_on_validation_error=args.command in {"validate", "generate"},
        )
        summary = _analysis_summary(pipeline)
        analysis = pipeline.analysis
        command_result: CommandResult
        view: dict[str, Any] = {}
        query = _query_metadata(args)

        if args.command == "analyze":
            view = _analysis_view(pipeline, args)
            command_result = CommandResult(
                "analyze",
                True,
                summary,
                list(pipeline.ir.diagnostics),
                {"analysis": analysis, **({"query": query} if query else {})},
            )
        elif args.command == "dump-ir":
            with reporter.stage("Preparing IR snapshot"):
                ir_snapshot = analysis
            command_result = CommandResult("dump-ir", True, summary, list(pipeline.ir.diagnostics), {"ir": ir_snapshot})
        elif args.command == "validate":
            valid = not has_errors(pipeline.ir.diagnostics)
            command_result = CommandResult(
                "validate",
                valid,
                summary,
                list(pipeline.ir.diagnostics),
                {"valid": valid, "analysis": analysis, **({"query": query} if query else {})},
            )
            view = {"details": args.details or args.verbose, "category": args.category}
        elif args.command == "update-config":
            with reporter.stage("Finding unresolved types"):
                config_data = pipeline.config.data
                declarations = config_data.setdefault("type_declarations", {})
                discovered = sorted(
                    info.name
                    for info in pipeline.ir.types.values()
                    if info.representation.value == "unknown" and info.referenced_by and info.name not in declarations
                )
            with reporter.stage("Preparing configuration changes"):
                for name in discovered:
                    declarations[name] = {"kind": "unknown", "reason": "Awaiting human SNI binding review."}
                content = json.dumps(config_data, ensure_ascii=False, indent=2) + "\n"
            with reporter.stage("Writing binding configuration"):
                write_outputs_atomically({paths["config"]: content})
            command_result = CommandResult(
                "update-config",
                True,
                {**summary, "added_unknown_types": len(discovered)},
                list(pipeline.ir.diagnostics),
                {
                    "updated": bool(discovered),
                    "added_unknown_types": discovered,
                    "config": str(paths["config"]),
                    "analysis": analysis,
                    **({"query": query} if query else {}),
                },
            )
            view = {"details": args.details or args.verbose, "category": args.category}
        elif args.command == "generate":
            outputs = generate_and_write(pipeline, paths, reporter, verbose=args.verbose)
            artifacts = [
                {"path": str(path), "name": path.name, "status": (pipeline.artifact_status or {}).get(path, "Updated")}
                for path in sorted(outputs, key=lambda item: str(item))
            ]
            command_result = CommandResult(
                "generate",
                True,
                {**summary, "artifact_count": len(artifacts)},
                list(pipeline.ir.diagnostics),
                {"artifacts": artifacts, "analysis": analysis, **({"query": query} if query else {})},
            )
            view = {"details": args.details or args.verbose, "category": args.category}
        else:
            raise PipelineError("Command dispatch", f"unsupported command {args.command}")

        reporter.report(command_result, view=view)
        return 0 if command_result.success else 1
    except PipelineError as exc:
        result = _failure_result(args.command, exc.phase, str(exc), exc.diagnostics)
        reporter.report(result)
        return 1
    except (OSError, ValueError, SystemExit) as exc:
        reporter.report(_failure_result(args.command, reporter.current, str(exc)))
        return 1
    except Exception as exc:
        reporter.report(_failure_result(args.command, reporter.current, f"{type(exc).__name__}: {exc}"))
        return 1


def main() -> int:
    parser = build_parser()
    return run(parser.parse_args())


if __name__ == "__main__":
    raise SystemExit(main())
