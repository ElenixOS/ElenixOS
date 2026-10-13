"""Resolve project-specific paths at the CLI boundary."""

from __future__ import annotations

import os
from argparse import Namespace
from pathlib import Path


def _find_project_root(start: Path) -> Path:
    for candidate in (start, *start.parents):
        if (candidate / "lv_conf.h").is_file() and (candidate / "lvgl").is_dir():
            return candidate
    return start


def resolve_paths(args: Namespace, script_path: Path, working_directory: Path | None = None) -> dict[str, Path]:
    """Resolve caller/project paths before invoking the SNI Core pipeline.

    Project inputs are discovered from the working directory and conventional
    project markers. Alternate LVGL checkouts can be selected with
    ``--lvgl-root`` or ``LVGL_ROOT`` without teaching Core about the caller.
    """
    sni_root = script_path.resolve().parent
    elenix_root = sni_root.parents[1]
    cwd = Path(working_directory or Path.cwd()).expanduser().resolve()
    project_root = _find_project_root(cwd)

    explicit_lvgl_root = getattr(args, "lvgl_root", None)
    environment_lvgl_root = os.environ.get("LVGL_ROOT")
    lvgl_root = Path(explicit_lvgl_root or environment_lvgl_root or project_root / "lvgl").expanduser().resolve()

    explicit_lvgl_json = getattr(args, "lvgl_json", None)
    refreshing = bool(getattr(args, "refresh_lvgl_json", False))
    build_json = project_root / "build" / "lvgl-api" / "lvgl.json"
    checkout_json = lvgl_root / "scripts" / "gen_json" / "output" / "lvgl.json"
    if explicit_lvgl_json:
        lvgl_json = Path(explicit_lvgl_json).expanduser().resolve()
    elif refreshing:
        lvgl_json = build_json.resolve()
    elif explicit_lvgl_root or environment_lvgl_root:
        # Do not pair an alternate LVGL checkout with a stale project snapshot.
        lvgl_json = checkout_json.resolve()
    else:
        lvgl_json = (build_json if build_json.is_file() else checkout_json).resolve()

    explicit_config = getattr(args, "config", None)
    explicit_lvgl_config = getattr(args, "lvgl_config", None)
    explicit_version_header = getattr(args, "lvgl_version_header", None)
    output_dir = getattr(args, "output_dir", None)

    special_api_source = elenix_root / "src" / "script_engine" / "sni" / "sni_api"
    paths = {
        "lvgl_json": lvgl_json,
        "config": Path(explicit_config).expanduser().resolve() if explicit_config else sni_root / "config" / "sni_lvgl_bindings.json",
        "special_api_source": special_api_source,
        "style_file": elenix_root / ".clang-format",
        "version_header": (
            Path(explicit_version_header).expanduser().resolve()
            if explicit_version_header
            else lvgl_root / "include" / "lvgl" / "lv_version.h"
        ),
        "type_ids": elenix_root / "src" / "script_engine" / "sni" / "sni_type_ids.h",
        "lv_types": elenix_root / "src" / "script_engine" / "sni" / "sni_gen" / "sni_lv_types.c",
        "api": elenix_root / "src" / "script_engine" / "sni" / "sni_api" / "lv" / "sni_api_lv.c",
        "lvgl_root": lvgl_root,
        "lvgl_config": (
            Path(explicit_lvgl_config).expanduser().resolve()
            if explicit_lvgl_config
            else project_root / "lv_conf.h"
        ),
    }

    if output_dir:
        destination = Path(output_dir).expanduser().resolve()
        paths["type_ids"] = destination / "sni_type_ids.h"
        paths["lv_types"] = destination / "sni_lv_types.c"
        paths["api"] = destination / "sni_api_lv.c"
    return paths
