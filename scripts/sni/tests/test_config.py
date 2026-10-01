from __future__ import annotations

import copy
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from sni.core.config import validate_config


def valid_config() -> dict:
    return {
        "schema_version": 1,
        "api_selection": {
            "classes": {
                "widget": {
                    "c_type": "lv_obj_t",
                    "constructor": "lv_widget_create",
                    "base": None,
                    "methods": ["lv_widget_set_width"],
                    "static_methods": [],
                    "constants": [],
                }
            },
            "scan": {"function": {"blacklist": [], "whitelist": []}, "constant": {"whitelist": ["*"], "blacklist": []}},
        },
        "type_declarations": {},
        "special_bindings": {"apis": {}, "constructors": {}, "properties": [], "class_extensions": {}},
        "runtime_type_ids": {},
    }


class BindingConfigTests(unittest.TestCase):
    def setUp(self) -> None:
        self.path = Path("sni_lvgl_bindings.json")

    def test_schema_version_and_shape_are_required(self) -> None:
        data = valid_config()
        self.assertEqual(validate_config(data, self.path).data["schema_version"], 1)
        data["schema_version"] = 2
        with self.assertRaisesRegex(ValueError, "unsupported schema_version"):
            validate_config(data, self.path)

    def test_unknown_root_field_is_rejected(self) -> None:
        data = valid_config()
        data["primitive_types"] = {"int": "SNI_T_INT32"}
        with self.assertRaisesRegex(ValueError, "unsupported fields"):
            validate_config(data, self.path)

    def test_duplicate_selection_is_rejected(self) -> None:
        data = valid_config()
        data["api_selection"]["classes"]["widget"]["methods"].append("lv_widget_set_width")
        with self.assertRaisesRegex(ValueError, "duplicate selector"):
            validate_config(data, self.path)

    def test_missing_special_binding_id_is_rejected(self) -> None:
        data = valid_config()
        data["special_bindings"]["apis"]["lv_widget_set_width"] = "sni_api_typo"
        with self.assertRaisesRegex(ValueError, "does not exist in C sources"):
            validate_config(data, self.path, {"sni_api_existing"})

    def test_unknown_type_is_valid_todo_but_only_as_unknown(self) -> None:
        data = valid_config()
        data["type_declarations"]["lv_waiting_t"] = {"kind": "unknown", "reason": "Review this type."}
        validate_config(data, self.path)
        data["type_declarations"]["lv_waiting_t"]["category"] = "borrowed_resource"
        with self.assertRaisesRegex(ValueError, "unknown declaration"):
            validate_config(data, self.path)

    def test_only_formal_managed_resource_categories_are_allowed(self) -> None:
        data = valid_config()
        data["type_declarations"]["lv_resource_t"] = {
            "kind": "managed_resource",
            "category": "pure_managed",
            "creator": "lv_widget_create",
        }
        validate_config(data, self.path)
        data["type_declarations"]["lv_resource_t"]["category"] = "borrowed_resource"
        with self.assertRaisesRegex(ValueError, "invalid managed resource category"):
            validate_config(data, self.path)

    def test_config_loader_does_not_mutate_input(self) -> None:
        data = valid_config()
        before = copy.deepcopy(data)
        validate_config(data, self.path)
        self.assertEqual(data, before)

    def test_update_config_adds_an_unresolved_type_review_item(self) -> None:
        generator = Path(__file__).resolve().parents[1] / "generate_sni.py"
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
            config_path = root / "sni_lvgl_bindings.json"
            config_path.write_text(json.dumps({
                "schema_version": 1,
                "api_selection": {
                    "classes": {"static": {"c_type": "lv_static_t", "constructor": None, "base": None, "methods": [], "static_methods": ["lv_test_unknown"], "constants": []}},
                    "scan": {"function": {"blacklist": [], "whitelist": []}, "constant": {"whitelist": ["*"], "blacklist": []}},
                },
            }), encoding="utf-8")
            process = subprocess.run(
                [sys.executable, str(generator), "update-config", "--lvgl-json", str(metadata), "--config", str(config_path), "--quiet", "--format", "json"],
                capture_output=True,
                text=True,
                check=False,
            )
            self.assertEqual(process.returncode, 0, process.stderr)
            result = json.loads(process.stdout)
            self.assertEqual(result["added_unknown_types"], ["lv_new_type_t"])
            updated = json.loads(config_path.read_text(encoding="utf-8"))
            self.assertEqual(updated["type_declarations"]["lv_new_type_t"]["kind"], "unknown")


if __name__ == "__main__":
    unittest.main()
