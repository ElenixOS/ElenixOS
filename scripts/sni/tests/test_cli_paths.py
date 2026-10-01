from __future__ import annotations

import json
import tempfile
import unittest
from argparse import Namespace
from pathlib import Path

from sni.cli.lvgl_metadata import refresh_lvgl_json
from sni.cli.paths import resolve_paths


class CliPathResolutionTests(unittest.TestCase):
    def make_project(self, root: Path) -> tuple[Path, Path]:
        script = root / "Simulator" / "ElenixOS" / "scripts" / "sni" / "generate_sni.py"
        script.parent.mkdir(parents=True)
        (root / "Simulator" / "lvgl").mkdir()
        (root / "Simulator" / "lv_conf.h").touch()
        script.touch()
        lvgl_root = root / "alternate-lvgl"
        return script, lvgl_root

    def test_explicit_lvgl_root_selects_its_metadata_and_version_header(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            script, lvgl_root = self.make_project(root)
            project_root = script.parents[3]
            stale_build_json = project_root / "build" / "lvgl-api" / "lvgl.json"
            stale_build_json.parent.mkdir(parents=True)
            stale_build_json.write_text("{}", encoding="utf-8")

            args = Namespace(
                lvgl_root=lvgl_root,
                lvgl_json=None,
                refresh_lvgl_json=False,
                lvgl_config=None,
                lvgl_version_header=None,
                config=None,
                output_dir=None,
            )
            paths = resolve_paths(args, script, project_root)

            self.assertEqual(paths["lvgl_json"], (lvgl_root / "scripts/gen_json/output/lvgl.json").resolve())
            self.assertEqual(paths["version_header"], (lvgl_root / "include/lvgl/lv_version.h").resolve())
            self.assertEqual(paths["lvgl_root"], lvgl_root.resolve())
            self.assertEqual(paths["special_api_source"], (script.parents[2] / "src/script_engine/sni/sni_api").resolve())
            self.assertEqual(paths["style_file"], (script.parents[2] / ".clang-format").resolve())

    def test_refresh_defaults_to_project_build_metadata_path(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            script, lvgl_root = self.make_project(root)
            args = Namespace(
                lvgl_root=lvgl_root,
                lvgl_json=None,
                refresh_lvgl_json=True,
                lvgl_config=None,
                lvgl_version_header=None,
                config=None,
                output_dir=None,
            )

            paths = resolve_paths(args, script, script.parents[3])

            self.assertEqual(paths["lvgl_json"], (script.parents[3] / "build/lvgl-api/lvgl.json").resolve())
            self.assertEqual(paths["lvgl_config"], (script.parents[3] / "lv_conf.h").resolve())

    def test_metadata_refresh_runs_generator_from_selected_lvgl_checkout(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            lvgl_root = root / "lvgl"
            generator = lvgl_root / "scripts/gen_json/gen_json.py"
            generator.parent.mkdir(parents=True)
            generator.write_text(
                "import argparse, json, pathlib\n"
                "parser = argparse.ArgumentParser()\n"
                "parser.add_argument('--lvgl-config')\n"
                "parser.add_argument('--output-path')\n"
                "args = parser.parse_args()\n"
                "out = pathlib.Path(args.output_path) / 'lvgl.json'\n"
                "out.parent.mkdir(parents=True, exist_ok=True)\n"
                "out.write_text(json.dumps({'macros': ['from-selected-checkout']}))\n",
                encoding="utf-8",
            )
            config = root / "lv_conf.h"
            config.write_text("/* fixture */\n", encoding="utf-8")
            output = root / "build/lvgl-api/lvgl.json"

            refresh_lvgl_json(lvgl_root, config, output)

            self.assertEqual(json.loads(output.read_text(encoding="utf-8")), {"macros": ["from-selected-checkout"]})


if __name__ == "__main__":
    unittest.main()
