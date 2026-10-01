from __future__ import annotations

import inspect
from pathlib import Path
import unittest

from sni.core.config import validate_config
from sni.core.ir import ApiStatus
from sni.core.lvgl_model import LVGLModel
from sni.core.resolver import PRIMITIVE_SNI, TypeResolver
from sni.core.selection import select_apis
from sni.render.lv_api import render_api


def scalar(name: str, kind: str = "stdlib_type") -> dict:
    return {"name": name, "json_type": kind, "quals": []}


def pointer(node: dict) -> dict:
    return {"type": node, "json_type": "pointer", "quals": []}


def function(name: str, return_type: dict, args: list[tuple[str, dict]]) -> dict:
    return {
        "name": name,
        "type": {"json_type": "ret_type", "type": return_type},
        "args": [
            {"json_type": "arg", "name": arg_name, "type": arg_type}
            for arg_name, arg_type in args
        ],
        "docstring": f"Documentation for {name}.",
    }


def resolved_fixture(full_types: bool = False):
    object_ptr = pointer(scalar("lv_obj_t", "lvgl_type"))
    functions = [
            function("lv_obj_create", pointer(scalar("lv_obj_t", "lvgl_type")), [("parent", object_ptr)]),
            function("lv_obj_set_short", scalar("void", "primitive_type"), [("obj", object_ptr), ("value", scalar("short"))]),
            function("lv_obj_get_short", scalar("short"), [("obj", object_ptr)]),
            function("lv_obj_blocked", scalar("void", "primitive_type"), [("obj", object_ptr)]),
            function("lv_obj_special", scalar("void", "primitive_type"), [("obj", object_ptr), ("value", scalar("int"))]),
            function("lv_obj_unselected", scalar("void", "primitive_type"), [("obj", object_ptr)]),
        ]
    method_names = [
        "lv_obj_set_short",
        "lv_obj_get_short",
        "lv_obj_blocked",
        "lv_obj_special",
    ]
    declarations = {}
    enums = []
    structures = []
    forward_decls = [
        {"name": "lv_obj_t", "json_type": "forward_decl", "type": scalar("struct", "primitive_type")}
    ]
    if full_types:
        pointer_const_point = {"type": scalar("lv_point_t", "lvgl_type"), "json_type": "pointer", "quals": ["const"]}
        typed_function_name = "lv_obj_set_resolved_types"
        functions.append(
            function(
                typed_function_name,
                scalar("void", "primitive_type"),
                [
                    ("obj", object_ptr),
                    ("flag", scalar("bool")),
                    ("signed8", scalar("int8_t")),
                    ("unsigned8", scalar("uint8_t")),
                    ("signed16", scalar("int16_t")),
                    ("unsigned16", scalar("uint16_t")),
                    ("signed32", scalar("int32_t")),
                    ("unsigned32", scalar("uint32_t")),
                    ("single", scalar("float")),
                    ("wide", scalar("double")),
                    ("mode", scalar("lv_test_mode_t", "lvgl_type")),
                    ("text", pointer(scalar("char"))),
                    ("point", pointer_const_point),
                    ("child", object_ptr),
                    ("timer", pointer(scalar("lv_timer_t", "lvgl_type"))),
                ],
            )
        )
        method_names.append(typed_function_name)
        enums.append({"name": "lv_test_mode_t", "json_type": "enum", "members": []})
        structures.append(
            {
                "name": "lv_point_t",
                "json_type": "struct",
                "fields": [
                    {"name": "x", "type": scalar("int16_t"), "bitsize": None},
                    {"name": "y", "type": scalar("int16_t"), "bitsize": None},
                ],
            }
        )
        forward_decls.append(
            {"name": "lv_timer_t", "json_type": "forward_decl", "type": scalar("struct", "primitive_type")}
        )
        declarations["lv_timer_t"] = {
            "kind": "managed_resource",
            "category": "pure_managed",
            "creator": "lv_timer_create",
        }
    data = {
        "functions": functions,
        "enums": enums,
        "structures": structures,
        "unions": [],
        "typedefs": [],
        "forward_decls": forward_decls,
        "function_pointers": [],
        "variables": [],
        "macros": [],
        "exported_constants": [],
    }
    config_data = {
        "schema_version": 1,
        "api_selection": {
            "classes": {
                "obj": {
                    "c_type": "lv_obj_t",
                    "constructor": "lv_obj_create",
                    "base": None,
                    "methods": method_names,
                    "static_methods": [],
                    "constants": [],
                }
            },
            "scan": {
                "function": {"whitelist": [], "blacklist": ["lv_obj_blocked"]},
                "constant": {"whitelist": [], "blacklist": []},
            },
        },
        "function_overrides": {},
        "api_rejection_reasons": {},
        "type_declarations": declarations,
        "special_conversions": {},
        "special_bindings": {
            "apis": {"lv_obj_special": "sni_api_custom_special"},
            "constructors": {},
            "properties": [],
            "class_extensions": {},
            "type_dependencies": {},
        },
        "runtime_type_ids": {"tree_dependent": [], "hybrid": [], "pure_managed": [], "legacy_handles": []},
    }
    model = LVGLModel(data)
    config = validate_config(config_data, Path("fixture.json"), {"sni_api_custom_special"})
    selection = select_apis(data, config.data)
    ir = TypeResolver(model, config.data, selection).resolve_all()
    return data, ir


class ResolvedApiRendererTests(unittest.TestCase):
    def test_renderer_interface_accepts_only_ir_and_version(self) -> None:
        self.assertEqual(list(inspect.signature(render_api).parameters), ["surface", "lvgl_version"])

    def test_short_mapping_flows_from_resolver_through_generated_conversion(self) -> None:
        self.assertEqual(PRIMITIVE_SNI["short"], "SNI_T_INT16")
        _, ir = resolved_fixture()
        short_use = next(
            use for use in ir.uses
            if use.function == "lv_obj_set_short" and use.name == "value"
        )
        return_use = next(use for use in ir.uses if use.function == "lv_obj_get_short" and use.position == "return")
        self.assertEqual(short_use.sni_type, "SNI_T_INT16")
        self.assertEqual(return_use.sni_type, "SNI_T_INT16")

        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("arg_value = sni_tb_js2c_int32(args_p[0]);", source)
        self.assertIn("sni_tb_c2js(&result, SNI_T_INT16)", source)
        self.assertNotIn("SNI_T_INT32", source)

    def test_resolved_conversion_plans_cover_primitive_enum_string_object_and_resources(self) -> None:
        _, ir = resolved_fixture(full_types=True)
        function_name = "lv_obj_set_resolved_types"
        expected = {
            "flag": "SNI_T_BOOL",
            "signed8": "SNI_T_INT8",
            "unsigned8": "SNI_T_UINT8",
            "signed16": "SNI_T_INT16",
            "unsigned16": "SNI_T_UINT16",
            "signed32": "SNI_T_INT32",
            "unsigned32": "SNI_T_UINT32",
            "single": "SNI_T_FLOAT",
            "wide": "SNI_T_DOUBLE",
            "mode": "SNI_T_INT32",
            "text": "SNI_T_STRING",
            "point": "SNI_V_LV_POINT",
            "child": "SNI_H_LV_OBJ",
            "timer": "SNI_H_LV_TIMER",
        }
        uses = {
            use.name: use
            for use in ir.api_surface.uses
            if use.function == function_name and use.position != "return"
        }
        self.assertEqual({name: uses[name].sni_type for name in expected}, expected)
        self.assertEqual(uses["point"].argument_mode, "value_pointer")
        self.assertFalse(uses["point"].copy_back)
        self.assertEqual(uses["timer"].lifecycle_class, "controlled_resource")

        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("sni_tb_js2c_boolean(args_p[0])", source)
        self.assertIn("sni_tb_js2c_string(args_p[10])", source)
        self.assertIn("SNI_V_LV_POINT", source)
        self.assertIn("SNI_H_LV_TIMER", source)

    def test_renderer_consumes_final_selected_surface_and_excludes_rejections(self) -> None:
        metadata, ir = resolved_fixture()
        self.assertIn("lv_obj_unselected", {item["name"] for item in metadata["functions"]})
        self.assertTrue(any(api.name == "lv_obj_blocked" and api.status == ApiStatus.EXCLUDED_BLACKLIST for api in ir.apis))
        self.assertNotIn("lv_obj_blocked", {api.name for api in ir.api_surface.apis})
        self.assertNotIn("lv_obj_blocked", {use.function for use in ir.api_surface.uses})

        source, result = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("sni_api_lv_obj_get_short", source)
        self.assertNotIn("lv_obj_blocked", source)
        self.assertNotIn("lv_obj_unselected", source)
        self.assertNotIn("SNI_API_SURFACE_BEGIN\nlv_obj_blocked", source)
        self.assertIn("{.name = \"special\", .handler = sni_api_custom_special}", source)
        self.assertNotIn("jerry_value_t sni_api_lv_obj_special(", source)
        self.assertEqual(set(result["api_names"]), set(ir.api_surface.names))


if __name__ == "__main__":
    unittest.main()
