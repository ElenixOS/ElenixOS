"""LVGL metadata generation adapter used by the CLI, never by SNI Core."""

from __future__ import annotations

import subprocess
import sys
import tempfile
from pathlib import Path


class MetadataRefreshError(RuntimeError):
    """Raised when the selected LVGL checkout cannot refresh its metadata."""


def refresh_lvgl_json(lvgl_root: Path, lvgl_config: Path, output_json: Path) -> None:
    generator = lvgl_root / "scripts" / "gen_json" / "gen_json.py"
    if not generator.is_file():
        raise MetadataRefreshError(f"LVGL gen_json.py not found in selected checkout: {generator}")
    if not lvgl_config.is_file():
        raise MetadataRefreshError(f"LVGL configuration header not found: {lvgl_config}")

    output_json.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix=".sni-lvgl-json-", dir=output_json.parent) as staging:
        staging_dir = Path(staging)
        generated_json = staging_dir / "lvgl.json"
        command = [
            sys.executable,
            str(generator),
            "--lvgl-config",
            str(lvgl_config),
            "--output-path",
            str(staging_dir),
        ]
        process = subprocess.run(
            command,
            cwd=lvgl_root,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
        if process.returncode != 0:
            raise MetadataRefreshError(
                f"LVGL metadata generator exited with status {process.returncode}; "
                "its output is suppressed because the upstream tool may include process environment data"
            )
        if not generated_json.is_file():
            raise MetadataRefreshError(f"LVGL metadata generator completed without writing {generated_json}")
        generated_json.replace(output_json)
