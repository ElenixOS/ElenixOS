"""End-to-end load, select, collect, resolve, validate and render orchestration."""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from ..render import lv_api
from ..render.lv_types import render_lv_types
from ..render.type_ids import build_type_ids
from .config import BindingConfig, load_config
from .diagnostics import build_analysis
from .ir import Diagnostic
from .lvgl_model import LVGLModel
from .progress import ProgressReporter
from .resolver import TypeResolver
from .selection import SelectionResult, select_apis
from .validation import has_errors, validate_bindings


class PipelineError(RuntimeError):
    def __init__(self, phase: str, message: str, diagnostics: list[Diagnostic] | None = None):
        super().__init__(message)
        self.phase = phase
        self.diagnostics = diagnostics or []


@dataclass
class PipelineResult:
    model: LVGLModel
    config: BindingConfig
    selection: SelectionResult
    ir: Any
    analysis: dict[str, Any]
    special_ids: set[str]
    artifact_status: dict[Path, str] | None = None
    emitter_diagnostics: list[str] | None = None


def scan_special_ids(source_root: Path) -> set[str]:
    if not source_root.is_dir():
        raise PipelineError("Loading SNI binding configuration", f"special API source directory not found: {source_root}")
    pattern = re.compile(r"\b(sni_api_[A-Za-z0-9_]+)\s*\(")
    result: set[str] = set()
    for path in sorted(source_root.rglob("*")):
        if path.suffix not in {".c", ".h"}:
            continue
        result.update(pattern.findall(path.read_text(encoding="utf-8", errors="replace")))
    return result


def build_pipeline(
    paths: dict[str, Path],
    reporter: ProgressReporter,
    fail_on_validation_error: bool = False,
) -> PipelineResult:
    with reporter.stage("Loading LVGL metadata"):
        try:
            lvgl_data = json.loads(paths["lvgl_json"].read_text(encoding="utf-8"))
        except OSError as exc:
            raise PipelineError(reporter.current, f"Cannot read {paths['lvgl_json']}: {exc}") from exc
        except json.JSONDecodeError as exc:
            raise PipelineError(reporter.current, f"Invalid LVGL JSON at {paths['lvgl_json']}:{exc.lineno}:{exc.colno}: {exc.msg}") from exc
        if not isinstance(lvgl_data, dict) or not isinstance(lvgl_data.get("functions"), list):
            raise PipelineError(reporter.current, "lvgl.json must contain a functions array")
        model = LVGLModel(lvgl_data)

    with reporter.stage("Loading SNI binding configuration"):
        special_ids = scan_special_ids(paths["special_api_source"])
        try:
            config = load_config(paths["config"], special_ids)
        except ValueError as exc:
            raise PipelineError(reporter.current, str(exc)) from exc

    with reporter.stage("Selecting API surface"):
        try:
            selection = select_apis(lvgl_data, config.data, model)
        except (ValueError, SystemExit) as exc:
            raise PipelineError(reporter.current, str(exc)) from exc

    with reporter.stage("Collecting referenced types"):
        referenced_type_names = TypeResolver.collect_referenced_type_names(selection, config.data)

    with reporter.stage("Resolving C types and inferring SNI representations"):
        resolver = TypeResolver(model, config.data, selection)
        ir = resolver.resolve_all(referenced_type_names)

    with reporter.stage("Validating bindings") as stage:
        diagnostics = validate_bindings(config.data, model, selection, ir, special_ids)
        if fail_on_validation_error and has_errors(diagnostics):
            stage.failure()
        elif any(item.severity.value in {"WARNING", "ERROR"} for item in diagnostics):
            stage.warning()

    with reporter.stage("Building structured analysis result"):
        analysis = build_analysis(ir, selection, config, paths["api"])
    return PipelineResult(model, config, selection, ir, analysis, special_ids)


def render_outputs(result: PipelineResult, paths: dict[str, Path], verbose: bool = False) -> dict[Path, str]:
    if result.ir.api_surface is None:
        raise PipelineError("Rendering generated sources", "Core did not produce a resolved API surface")
    type_ids_header, _, _ = build_type_ids(result.ir, result.config.data)
    lvgl_version = lv_api.load_lvgl_version(paths["version_header"])
    api_source, emitter_result = lv_api.render_api(result.ir.api_surface, lvgl_version)
    result.emitter_diagnostics = list(emitter_result.get("diagnostics", []))
    expected = set(result.ir.api_surface.names)
    emitted = set(emitter_result["api_names"])
    if expected != emitted:
        missing = sorted(expected - emitted)
        extra = sorted(emitted - expected)
        raise PipelineError("Rendering generated sources", f"API emitter selection mismatch; missing={missing[:12]}, extra={extra[:12]}")
    lv_types_source = render_lv_types(result.ir, result.model)
    outputs = {
        paths["type_ids"]: type_ids_header,
        paths["lv_types"]: lv_types_source,
        paths["api"]: api_source,
    }
    return format_generated_outputs(outputs, paths["style_file"])


def _clang_format_20() -> Path:
    candidates = [
        shutil.which("clang-format-20"),
        shutil.which("clang-format"),
    ]
    for candidate in candidates:
        if not candidate or not Path(candidate).is_file():
            continue
        result = subprocess.run([candidate, "--version"], capture_output=True, text=True, check=False)
        match = re.search(r"clang-format version (\d+)\.", result.stdout)
        if result.returncode == 0 and match and int(match.group(1)) == 20:
            return Path(candidate)
    raise PipelineError("Formatting generated sources", "clang-format 20 is required to format generated C/C++ output deterministically")


def format_generated_outputs(outputs: dict[Path, str], style_file: Path) -> dict[Path, str]:
    formatter = _clang_format_20()
    if not style_file.is_file():
        raise PipelineError("Formatting generated sources", f"project clang-format style file not found: {style_file}")
    formatted: dict[Path, str] = {}
    for path, content in outputs.items():
        command = [str(formatter), f"--style=file:{style_file}", f"--assume-filename={path}"]
        process = subprocess.run(command, input=content, capture_output=True, text=True, check=False)
        if process.returncode != 0:
            raise PipelineError("Formatting generated sources", f"clang-format failed for {path}: {process.stderr.strip()}")
        formatted[path] = process.stdout
    return formatted


def write_outputs_atomically(outputs: dict[Path, str]) -> None:
    staged: list[tuple[Path, Path]] = []
    try:
        for target, content in sorted(outputs.items(), key=lambda entry: str(entry[0])):
            target.parent.mkdir(parents=True, exist_ok=True)
            with tempfile.NamedTemporaryFile(
                mode="w", encoding="utf-8", newline="\n", prefix=f".{target.name}.", suffix=".tmp", dir=target.parent, delete=False
            ) as stream:
                temp_path = Path(stream.name)
                stream.write(content)
                stream.flush()
                os.fsync(stream.fileno())
            staged.append((temp_path, target))
        for temp_path, target in staged:
            os.replace(temp_path, target)
    except OSError:
        for temp_path, _ in staged:
            temp_path.unlink(missing_ok=True)
        raise


def verify_outputs(outputs: dict[Path, str]) -> None:
    for path, expected in outputs.items():
        if not path.is_file():
            raise PipelineError("Verifying generated artifacts", f"generated output is missing: {path}")
        actual = path.read_text(encoding="utf-8")
        if actual != expected:
            raise PipelineError("Verifying generated artifacts", f"generated output differs from rendered contents: {path}")
    api_path = next((path for path in outputs if path.name == "sni_api_lv.c"), None)
    api_text = outputs.get(api_path, "") if api_path else ""
    if "SNI_API_SURFACE_BEGIN" not in api_text or "SNI_API_SURFACE_END" not in api_text:
        raise PipelineError("Verifying generated artifacts", "generated API source is missing its API surface marker")


def generate_and_write(result: PipelineResult, paths: dict[str, Path], reporter: ProgressReporter, verbose: bool = False) -> dict[Path, str]:
    if has_errors(result.ir.diagnostics):
        raise PipelineError("Validating bindings", "unresolved types or invalid binding configuration prevent generation", result.ir.diagnostics)
    with reporter.stage("Rendering generated sources"):
        outputs = render_outputs(result, paths, verbose=verbose)
    for message in result.emitter_diagnostics or []:
        reporter.detail(message)
    result.artifact_status = {
        path: "Unchanged" if path.is_file() and path.read_text(encoding="utf-8") == content else "Updated"
        for path, content in outputs.items()
    }
    with reporter.stage("Writing output files"):
        write_outputs_atomically(outputs)
    with reporter.stage("Verifying generated artifacts"):
        verify_outputs(outputs)
    return outputs
