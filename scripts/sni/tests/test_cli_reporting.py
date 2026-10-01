from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from sni.core.pipeline import PipelineError, _clang_format_20


REPOSITORY_ROOT = Path(__file__).resolve().parents[4]
GENERATOR = REPOSITORY_ROOT / "ElenixOS" / "scripts" / "sni" / "generate_sni.py"
CONFIG = REPOSITORY_ROOT / "ElenixOS" / "scripts" / "sni" / "config" / "sni_lvgl_bindings.json"
GENERATED_ROOT = REPOSITORY_ROOT / "ElenixOS" / "src" / "script_engine" / "sni"


def run_cli(*arguments: str, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    process_env = os.environ.copy()
    if env:
        process_env.update(env)
    return subprocess.run(
        [sys.executable, str(GENERATOR), *arguments],
        cwd=REPOSITORY_ROOT,
        capture_output=True,
        text=True,
        check=False,
        env=process_env,
    )


class SNICommandReportingTests(unittest.TestCase):
    def test_json_contract_for_analysis_validation_dump_and_config_update(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            config_copy = Path(folder) / "bindings.json"
            shutil.copyfile(CONFIG, config_copy)
            commands = [
                ("analyze",),
                ("validate",),
                ("dump-ir",),
                ("update-config", "--config", str(config_copy)),
            ]
            expected_totals = {"analyze": 7, "validate": 7, "dump-ir": 8, "update-config": 10}
            for command in commands:
                with self.subTest(command=command[0]):
                    process = run_cli(*command, "--format", "json")
                    self.assertEqual(process.returncode, 0, process.stderr)
                    self.assertEqual(process.stderr, "")
                    result = json.loads(process.stdout)
                    self.assertEqual(result["command"], command[0])
                    self.assertIn("success", result)
                    self.assertIn("progress", result)
                    self.assertIn("summary", result)
                    self.assertIn("diagnostics", result)
                    self.assertIn("result", result)
                    self.assertTrue(result["progress"])
                    self.assertTrue(all(item["total"] == expected_totals[command[0]] for item in result["progress"]))
                    self.assertEqual([item["index"] for item in result["progress"]], list(range(1, len(result["progress"]) + 1)))

    def test_dump_ir_file_output_keeps_console_short_and_writes_full_snapshot(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            for output_format in ("text", "json"):
                with self.subTest(format=output_format):
                    output_file = Path(folder) / f"sni-ir-{output_format}.json"
                    process = run_cli(
                        "dump-ir",
                        "--output-file",
                        str(output_file),
                        "--quiet",
                        "--format",
                        output_format,
                    )
                    self.assertEqual(process.returncode, 0, process.stderr)
                    self.assertEqual(process.stderr, "")
                    self.assertTrue(output_file.is_file())
                    snapshot = json.loads(output_file.read_text(encoding="utf-8"))
                    self.assertIn("apis", snapshot)
                    self.assertIn("types", snapshot)
                    self.assertIn("uses", snapshot)
                    self.assertIn("diagnostics", snapshot)
                    self.assertEqual(len(snapshot["apis"]), 845)
                    if output_format == "text":
                        self.assertIn(str(output_file.resolve()), process.stdout)
                        self.assertNotIn('"apis": [', process.stdout)
                    else:
                        response = json.loads(process.stdout)
                        self.assertEqual(Path(response["result"]["file"]).resolve(), output_file.resolve())
                        self.assertNotIn("ir", response["result"])
                        self.assertEqual(len(response["progress"]), 9)
                        self.assertTrue(all(item["total"] == 9 for item in response["progress"]))

    def test_category_and_api_type_drilldowns(self) -> None:
        blacklist = run_cli("analyze", "--category", "blacklist", "--quiet")
        self.assertEqual(blacklist.returncode, 0, blacklist.stderr)
        self.assertIn("Blacklisted", blacklist.stdout)
        self.assertIn("Intentionally excluded", blacklist.stdout)
        self.assertIn("Needs attention 112", blacklist.stdout)
        self.assertNotIn("API_BLACKLISTED", blacklist.stdout + blacklist.stderr)

        api = run_cli("analyze", "--api", "lv_timer_get_next", "--quiet")
        self.assertEqual(api.returncode, 0, api.stderr)
        self.assertIn("Status: REJECTED_LIFECYCLE", api.stdout)
        self.assertIn("Pure Managed lv_timer_t", api.stdout)

        type_result = run_cli("analyze", "--type", "lv_grad_dsc_t", "--quiet")
        self.assertEqual(type_result.returncode, 0, type_result.stderr)
        self.assertIn("Status: REJECTED_UNSUPPORTED_TYPE", type_result.stdout)
        self.assertIn("array", type_result.stdout)
        self.assertIn("lv_obj_get_style_bg_grad", type_result.stdout)

    def test_no_color_and_json_stdout_are_clean(self) -> None:
        process = run_cli("analyze", "--no-color", "--details", env={"NO_COLOR": "1"})
        self.assertEqual(process.returncode, 0, process.stderr)
        self.assertNotIn("\x1b[", process.stdout + process.stderr)
        machine = run_cli("analyze", "--format", "json", "--verbose")
        self.assertEqual(machine.returncode, 0, machine.stderr)
        json.loads(machine.stdout)
        self.assertNotIn("[1/", machine.stdout)
        self.assertEqual(machine.stderr, "")
        global_flags = run_cli("--quiet", "--format", "json", "analyze")
        self.assertEqual(global_flags.returncode, 0, global_flags.stderr)
        json.loads(global_flags.stdout)

    def test_unresolved_type_is_one_error_and_fails_validation_progress(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            metadata = root / "lvgl.json"
            metadata.write_text(
                json.dumps({
                    "functions": [{
                        "name": "lv_test_unknown",
                        "type": {"type": {"name": "void", "json_type": "primitive_type"}, "json_type": "ret_type"},
                        "args": [{"name": "value", "type": {"name": "lv_new_type_t", "json_type": "lvgl_type"}}],
                    }],
                    "structures": [], "unions": [], "typedefs": [], "enums": [],
                    "function_pointers": [], "forward_decls": [], "variables": [], "macros": [],
                }),
                encoding="utf-8",
            )
            config = root / "bindings.json"
            config.write_text(json.dumps({
                "schema_version": 1,
                "api_selection": {
                    "classes": {"static": {"c_type": "lv_static_t", "constructor": None, "base": None, "methods": [], "static_methods": ["lv_test_unknown"], "constants": []}},
                    "scan": {"function": {"blacklist": [], "whitelist": []}, "constant": {"whitelist": ["*"], "blacklist": []}},
                },
            }), encoding="utf-8")
            process = run_cli("validate", "--lvgl-json", str(metadata), "--config", str(config), "--format", "json")
            self.assertEqual(process.returncode, 1, process.stderr)
            self.assertEqual(process.stderr, "")
            result = json.loads(process.stdout)
        self.assertFalse(result["success"])
        validation_stage = next(item for item in result["progress"] if item["label"] == "Validating bindings")
        self.assertEqual(validation_stage["status"], "FAILED")
        self.assertEqual([item["code"] for item in result["diagnostics"]], ["UNRESOLVED_TYPE"])
        self.assertEqual(result["summary"]["unresolved"], 1)

    def test_generation_preserves_checked_in_artifact_bytes(self) -> None:
        try:
            _clang_format_20()
        except PipelineError as exc:
            self.skipTest(str(exc))
        targets = {
            "sni_type_ids.h": GENERATED_ROOT / "sni_type_ids.h",
            "sni_lv_types.c": GENERATED_ROOT / "sni_gen" / "sni_lv_types.c",
            "sni_api_lv.c": GENERATED_ROOT / "sni_api" / "lv" / "sni_api_lv.c",
        }
        expected = {name: path.read_bytes() for name, path in targets.items()}
        with tempfile.TemporaryDirectory() as folder:
            process = run_cli("generate", "--output-dir", folder, "--format", "json")
            self.assertEqual(process.returncode, 0, process.stderr)
            result = json.loads(process.stdout)
            self.assertTrue(result["success"])
            generated = {path.name: path.read_bytes() for path in Path(folder).iterdir()}
        self.assertEqual(generated, expected)


if __name__ == "__main__":
    unittest.main()
