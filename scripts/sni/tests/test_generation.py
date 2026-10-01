from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from sni.core.config import BindingConfig, validate_config
from sni.core.ir import ApiStatus
from sni.core.lvgl_model import LVGLModel
from sni.core.pipeline import PipelineError, PipelineResult, ProgressReporter, generate_and_write, render_outputs
from sni.core.resolver import TypeResolver
from sni.core.selection import select_apis


def primitive(name: str, kind: str = "stdlib_type", quals: list[str] | None = None) -> dict:
    return {"name": name, "json_type": kind, "quals": quals or []}


def ptr(node: dict, quals: list[str] | None = None) -> dict:
    return {"type": node, "json_type": "pointer", "quals": quals or []}


class GeneratorRenderingTests(unittest.TestCase):
    def make_fixture(self, temp_root: Path) -> tuple[PipelineResult, dict[str, Path]]:
        lvgl_data = {
            "functions": [
                {
                    "name": "lv_obj_create",
                    "type": {"type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "ret_type"},
                    "args": [{"name": "parent", "type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "arg"}],
                },
                {
                    "name": "lv_obj_set_area",
                    "type": {"type": primitive("void", "primitive_type"), "json_type": "ret_type"},
                    "args": [
                        {"name": "obj", "type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "arg"},
                        {"name": "area", "type": ptr(primitive("lv_area_t", "lvgl_type", ["const"])), "json_type": "arg"},
                    ],
                },
            ],
            "structures": [{
                "name": "lv_area_t",
                "json_type": "struct",
                "fields": [
                    {"name": "x1", "type": primitive("int16_t"), "bitsize": None},
                    {"name": "y1", "type": primitive("int16_t"), "bitsize": None},
                    {"name": "x2", "type": primitive("int16_t"), "bitsize": None},
                    {"name": "y2", "type": primitive("int16_t"), "bitsize": None},
                ],
            }],
            "unions": [],
            "typedefs": [],
            "enums": [],
            "function_pointers": [],
            "forward_decls": [{"name": "lv_obj_t", "type": primitive("struct", "primitive_type"), "json_type": "forward_decl"}],
            "variables": [],
            "macros": [],
        }
        config_data = {
            "schema_version": 1,
            "api_selection": {
                "classes": {
                    "obj": {"c_type": "lv_obj_t", "constructor": "lv_obj_create", "base": None, "methods": ["lv_obj_set_area"], "static_methods": [], "constants": []}
                },
                "scan": {"function": {"blacklist": [], "whitelist": []}, "constant": {"whitelist": ["*"], "blacklist": []}},
            },
            "function_overrides": {},
            "type_declarations": {},
            "special_conversions": {},
            "special_bindings": {"apis": {}, "constructors": {}, "properties": [], "class_extensions": {}, "type_dependencies": {}},
            "runtime_type_ids": {"tree_dependent": [], "hybrid": [], "pure_managed": [], "legacy_handles": []},
        }
        model = LVGLModel(lvgl_data)
        config = validate_config(config_data, temp_root / "sni_lvgl_bindings.json", set())
        selection = select_apis(lvgl_data, config.data)
        ir = TypeResolver(model, config.data, selection).resolve_all()
        self.assertEqual({api.status for api in ir.apis}, {ApiStatus.ACCEPTED})
        paths = {
            "type_ids": temp_root / "sni_type_ids.h",
            "lv_types": temp_root / "sni_lv_types.c",
            "api": temp_root / "sni_api_lv.c",
            "version_header": temp_root / "lv_version.h",
        }
        paths["version_header"].write_text(
            "#define LVGL_VERSION_MAJOR 9\n#define LVGL_VERSION_MINOR 6\n#define LVGL_VERSION_PATCH 0\n",
            encoding="utf-8",
        )
        result = PipelineResult(model, config, selection, ir, {"analysis": {}}, set())
        return result, paths

    def test_in_memory_ir_renders_all_three_outputs(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            result, paths = self.make_fixture(root)
            outputs = render_outputs(result, paths)
            self.assertEqual(set(outputs), {paths["type_ids"], paths["lv_types"], paths["api"]})
            self.assertIn("SNI_V_LV_AREA", outputs[paths["type_ids"]])
            self.assertIn("SNI_V_LV_AREA", outputs[paths["lv_types"]])
            self.assertIn("sni_api_lv_obj_set_area", outputs[paths["api"]])
            self.assertIn("SNI_API_SURFACE_BEGIN", outputs[paths["api"]])

    def test_identical_ir_renders_byte_identical_outputs(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            result, paths = self.make_fixture(Path(folder))
            first = render_outputs(result, paths)
            second = render_outputs(result, paths)
            self.assertEqual(first, second)

    def test_validation_failure_does_not_touch_existing_generated_files(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            result, paths = self.make_fixture(root)
            sentinel = b"existing generated output\n"
            paths["type_ids"].write_bytes(sentinel)
            result.ir.diagnostics.append({"severity": "error", "code": "UNRESOLVED_TYPE", "message": "hold"})
            reporter = ProgressReporter(quiet=True)
            reporter.start(10)
            with self.assertRaises(PipelineError):
                generate_and_write(result, paths, reporter)
            self.assertEqual(paths["type_ids"].read_bytes(), sentinel)
            self.assertFalse(paths["lv_types"].exists())
            self.assertFalse(paths["api"].exists())

    def test_successful_generation_writes_and_verifies_all_outputs(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            result, paths = self.make_fixture(root)
            reporter = ProgressReporter(quiet=True)
            reporter.start(10)
            outputs = generate_and_write(result, paths, reporter)
            for path, content in outputs.items():
                self.assertEqual(path.read_text(encoding="utf-8"), content)

    def test_chart_remove_wrapper_unlinks_and_removes_tree_dependent_handle(self) -> None:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            lvgl_data = {
                "functions": [
                    {
                        "name": "lv_chart_create",
                        "type": {"type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "ret_type"},
                        "args": [{"name": "parent", "type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "arg"}],
                    },
                    {
                        "name": "lv_chart_add_series",
                        "type": {"type": ptr(primitive("lv_chart_series_t", "lvgl_type")), "json_type": "ret_type"},
                        "args": [
                            {"name": "chart", "type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "arg"},
                            {"name": "color", "type": primitive("uint32_t"), "json_type": "arg"},
                        ],
                    },
                    {
                        "name": "lv_chart_remove_series",
                        "type": {"type": primitive("void", "primitive_type"), "json_type": "ret_type"},
                        "args": [
                            {"name": "chart", "type": ptr(primitive("lv_obj_t", "lvgl_type")), "json_type": "arg"},
                            {"name": "series", "type": ptr(primitive("lv_chart_series_t", "lvgl_type")), "json_type": "arg"},
                        ],
                    },
                ],
                "structures": [],
                "unions": [],
                "typedefs": [],
                "enums": [],
                "function_pointers": [],
                "forward_decls": [
                    {"name": "lv_obj_t", "type": primitive("struct", "primitive_type"), "json_type": "forward_decl"},
                    {"name": "lv_chart_series_t", "type": primitive("struct", "primitive_type"), "json_type": "forward_decl"},
                ],
                "variables": [],
                "macros": [],
            }
            config_data = {
                "schema_version": 1,
                "api_selection": {
                    "classes": {"chart": {"c_type": "lv_obj_t", "constructor": "lv_chart_create", "base": None, "methods": ["lv_chart_add_series", "lv_chart_remove_series"], "static_methods": [], "constants": []}},
                    "scan": {"function": {"blacklist": [], "whitelist": []}, "constant": {"whitelist": ["*"], "blacklist": []}},
                },
                "type_declarations": {"lv_chart_series_t": {"kind": "managed_resource", "category": "tree_dependent", "creator": "lv_chart_add_series"}},
                "special_conversions": {},
                "special_bindings": {"apis": {}, "constructors": {}, "properties": [], "class_extensions": {}, "type_dependencies": {}},
                "runtime_type_ids": {"tree_dependent": ["SNI_H_LV_CHART_SERIES"], "hybrid": [], "pure_managed": [], "legacy_handles": []},
            }
            model = LVGLModel(lvgl_data)
            config = validate_config(config_data, root / "config.json", set())
            selection = select_apis(lvgl_data, config.data)
            ir = TypeResolver(model, config.data, selection).resolve_all()
            self.assertFalse(any(item["severity"] == "error" for item in ir.diagnostics))
            paths = {
                "type_ids": root / "sni_type_ids.h",
                "lv_types": root / "sni_lv_types.c",
                "api": root / "sni_api_lv.c",
                "version_header": root / "lv_version.h",
            }
            paths["version_header"].write_text("#define LVGL_VERSION_MAJOR 9\n#define LVGL_VERSION_MINOR 6\n#define LVGL_VERSION_PATCH 0\n", encoding="utf-8")
            pipeline = PipelineResult(model, config, selection, ir, {"analysis": {}}, set())
            api_source = render_outputs(pipeline, paths)[paths["api"]]
            self.assertIn("sni_tb_link_sub_resource(self_obj, result, SNI_H_LV_CHART_SERIES);", api_source)
            self.assertIn("sni_tb_remove_sub_resource_current(arg_series, SNI_H_LV_CHART_SERIES);", api_source)


if __name__ == "__main__":
    unittest.main()
