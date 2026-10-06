from __future__ import annotations

import unittest
from pathlib import Path

from sni.core.config import validate_config
from sni.core.diagnostics import build_analysis
from sni.core.ir import ApiExportRef, Severity
from sni.core.lvgl_model import LVGLModel
from sni.core.resolver import TypeResolver
from sni.core.selection import select_apis
from sni.core.validation import validate_bindings
from sni.render.lv_api import render_api


def primitive(name: str, kind: str = "primitive_type", quals: list[str] | None = None) -> dict:
    return {"name": name, "json_type": kind, "quals": quals or []}


def pointer(node: dict, quals: list[str] | None = None) -> dict:
    return {"type": node, "json_type": "pointer", "quals": quals or []}


def function(name: str, return_type: dict, args: list[tuple[str, dict]]) -> dict:
    return {
        "name": name,
        "type": {"type": return_type, "json_type": "ret_type"},
        "json_type": "function",
        "args": [
            {"name": arg_name, "type": arg_type, "json_type": "arg"}
            for arg_name, arg_type in args
        ],
    }


def make_binding(
    functions: list[dict],
    *,
    class_name: str = "foo",
    class_c_type: str = "lv_obj_t",
    class_base: str | None = None,
    methods: list | None = None,
    typedefs: list[dict] | None = None,
    structures: list[dict] | None = None,
    special_apis: dict[str, str] | None = None,
    special_properties: list[dict] | None = None,
    special_ids: set[str] | None = None,
):
    constructor_name = f"lv_{class_name}_create"
    all_functions = [
        *(
            [
                function(
                    "lv_obj_create",
                    pointer(primitive("lv_obj_t", "lvgl_type")),
                    [("parent", pointer(primitive("lv_obj_t", "lvgl_type")))],
                )
            ]
            if class_base
            else []
        ),
        function(
            constructor_name,
            pointer(primitive("lv_obj_t", "lvgl_type")),
            [("parent", pointer(primitive("lv_obj_t", "lvgl_type")))],
        ),
        *functions,
    ]
    data = {
        "functions": all_functions,
        "structures": structures or [],
        "unions": [],
        "typedefs": typedefs or [],
        "enums": [],
        "forward_decls": [
            {"name": "lv_obj_t", "json_type": "forward_decl", "type": primitive("struct")},
            {"name": "lv_foo_native_t", "json_type": "forward_decl", "type": primitive("struct")},
            {"name": "lv_calendar_date_t", "json_type": "forward_decl", "type": primitive("struct")},
            {"name": "other_t", "json_type": "forward_decl", "type": primitive("struct")},
        ],
        "function_pointers": [],
        "variables": [],
        "macros": [],
        "exported_constants": [],
    }
    class_configs = {}
    if class_base:
        class_configs["obj"] = {
            "c_type": "lv_obj_t",
            "constructor": "lv_obj_create",
            "base": None,
            "methods": [],
            "static_methods": [],
            "constants": [],
        }
    class_configs[class_name] = {
        "c_type": class_c_type,
        "constructor": constructor_name,
        "base": class_base,
        "methods": methods or [],
        "static_methods": [],
        "constants": [],
    }
    config_data = {
        "schema_version": 1,
        "api_selection": {
            "classes": class_configs,
            "scan": {
                "function": {"whitelist": [], "blacklist": []},
                "constant": {"whitelist": [], "blacklist": []},
            },
        },
        "function_overrides": {},
        "type_declarations": {},
        "special_conversions": {},
        "special_bindings": {
            "apis": special_apis or {},
            "constructors": {},
            "properties": special_properties or [],
            "class_extensions": {},
            "type_dependencies": {},
        },
        "runtime_type_ids": {
            "tree_dependent": [],
            "hybrid": [],
            "pure_managed": [],
            "legacy_handles": [],
        },
    }
    model = LVGLModel(data)
    config = validate_config(config_data, Path("receiver-fixture.json"), special_ids or set())
    selection = select_apis(data, config.data, model)
    ir = TypeResolver(model, config.data, selection).resolve_all()
    return data, config, model, selection, ir


class ReceiverValidationTests(unittest.TestCase):
    def test_normal_getter_is_bound_with_validated_receiver(self) -> None:
        data, _, _, _, ir = make_binding(
            [function("lv_foo_get_value", primitive("int"), [("obj", pointer(primitive("lv_obj_t", "lvgl_type")))])]
        )

        self.assertIn("lv_foo_get_value", ir.api_surface.names)
        prop = next(prop for prop in ir.api_surface.classes[0].properties if prop.name == "value")
        self.assertEqual(prop.getter.receiver.owner_class, "foo")
        self.assertEqual(prop.getter.receiver.binding_kind, "property_getter")
        self.assertEqual(prop.getter.receiver.receiver_index, 0)
        self.assertTrue(prop.getter.receiver.receiver_validated)
        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("{.name = \"value\", .getter = sni_api_prop_get_foo_value", source)

    def test_configured_object_base_receiver_is_compatible_with_concrete_class(self) -> None:
        _, _, _, _, ir = make_binding(
            [function("lv_foo_get_value", primitive("int"), [("obj", pointer(primitive("lv_obj_t", "lvgl_type")))])],
            class_c_type="lv_foo_native_t",
            class_base="obj",
        )

        prop = next(prop for prop in ir.api_surface.classes[1].properties if prop.name == "value")
        self.assertTrue(prop.getter.receiver.receiver_validated)
        self.assertIn("base lv_obj_t *", prop.getter.receiver.expected_receiver_type)

    def test_normal_setter_is_bound_with_validated_receiver(self) -> None:
        _, _, _, _, ir = make_binding(
            [
                function(
                    "lv_foo_set_value",
                    primitive("void"),
                    [("obj", pointer(primitive("lv_obj_t", "lvgl_type"))), ("value", primitive("int"))],
                )
            ]
        )

        self.assertIn("lv_foo_set_value", ir.api_surface.names)
        prop = next(prop for prop in ir.api_surface.classes[0].properties if prop.name == "value")
        self.assertEqual(prop.setter.receiver.binding_kind, "property_setter")
        self.assertTrue(prop.setter.receiver.receiver_validated)
        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("{.name = \"value\", .getter = NULL, .setter = sni_api_prop_set_foo_value}", source)

    def test_receiver_mismatch_getter_is_only_an_informational_candidate(self) -> None:
        _, config, _, selection, ir = make_binding(
            [function("lv_foo_get_value", primitive("int"), [("value", pointer(primitive("other_t", "lvgl_type")))])]
        )

        self.assertNotIn("lv_foo_get_value", ir.api_surface.names)
        self.assertFalse(any(prop.name == "value" for prop in ir.api_surface.classes[0].properties))
        mismatch = next(item for item in selection.messages if item.code == "RECEIVER_TYPE_MISMATCH")
        self.assertEqual(mismatch.severity, Severity.INFO)
        self.assertEqual(mismatch.details["candidate"], "foo.value")
        self.assertEqual(mismatch.details["expected_receiver_type"], "lv_obj_t *")
        self.assertEqual(mismatch.details["actual_receiver_type"], "other_t *")
        analysis = build_analysis(ir, selection, config, Path("missing-generated-api.c"))
        self.assertEqual(analysis["analysis"]["api_counts"]["needs_attention"], 0)

    def test_receiver_mismatch_setter_is_excluded(self) -> None:
        _, _, _, selection, ir = make_binding(
            [
                function(
                    "lv_foo_set_value",
                    primitive("void"),
                    [("value", pointer(primitive("other_t", "lvgl_type"))), ("x", primitive("int"))],
                )
            ]
        )

        self.assertNotIn("lv_foo_set_value", ir.api_surface.names)
        self.assertFalse(any(prop.name == "value" for prop in ir.api_surface.classes[0].properties))
        mismatch = next(item for item in selection.messages if item.code == "RECEIVER_TYPE_MISMATCH")
        self.assertEqual(mismatch.details["binding_kind"], "property_setter")
        self.assertEqual(mismatch.severity, Severity.INFO)

    def test_real_calendar_helper_is_not_an_instance_property(self) -> None:
        function_item = function(
            "lv_calendar_get_day_name",
            pointer(primitive("char", quals=["const"])),
            [("gregorian", pointer(primitive("lv_calendar_date_t", "lvgl_type")))],
        )
        _, _, _, selection, ir = make_binding(
            [function_item],
            class_name="calendar",
            structures=[
                {
                    "name": "lv_calendar_date_t",
                    "json_type": "struct",
                    "fields": [
                        {"name": "year", "type": primitive("uint16_t"), "bitsize": None},
                        {"name": "month", "type": primitive("uint8_t"), "bitsize": None},
                        {"name": "day", "type": primitive("uint8_t"), "bitsize": None},
                    ],
                }
            ],
        )

        self.assertNotIn("lv_calendar_get_day_name", ir.api_surface.names)
        self.assertFalse(any(prop.name == "day_name" for prop in ir.api_surface.classes[0].properties))
        mismatch = next(item for item in selection.messages if item.code == "RECEIVER_TYPE_MISMATCH")
        self.assertEqual(mismatch.details["candidate"], "calendar.dayName")
        self.assertEqual(mismatch.details["expected_receiver_type"], "lv_obj_t *")
        self.assertEqual(mismatch.details["actual_receiver_type"], "lv_calendar_date_t *")

    def test_typedef_and_const_receiver_are_canonicalized_for_methods(self) -> None:
        method = function(
            "lv_foo_measure",
            primitive("int"),
            [("obj", pointer(primitive("lv_obj_alias_t", "lvgl_type", ["const"]))), ("axis", primitive("int"))],
        )
        selector = {
            "name": "lv_foo_measure",
            "arg_match": [{"index": 0, "name": "obj", "type": "lv_obj_t*"}],
        }
        _, _, _, _, ir = make_binding(
            [method],
            methods=[selector],
            typedefs=[
                {
                    "name": "lv_obj_alias_t",
                    "json_type": "typedef",
                    "type": primitive("lv_obj_t", "lvgl_type"),
                }
            ],
        )

        method_ref = next(ref for ref in ir.api_surface.classes[0].methods if ref.function == "lv_foo_measure")
        self.assertTrue(method_ref.receiver.receiver_validated)
        self.assertEqual(method_ref.receiver.expected_receiver_type, "lv_obj_t *")
        self.assertEqual(method_ref.receiver.actual_receiver_type, "const lv_obj_t *")
        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("sni_api_lv_foo_measure", source)

    def test_typedef_pointer_alias_preserves_receiver_pointer_depth(self) -> None:
        method = function(
            "lv_foo_measure",
            primitive("int"),
            [("obj", primitive("lv_obj_ptr_t", "lvgl_type")), ("axis", primitive("int"))],
        )
        _, _, _, _, ir = make_binding(
            [method],
            methods=["lv_foo_measure"],
            typedefs=[
                {
                    "name": "lv_obj_ptr_t",
                    "json_type": "typedef",
                    "type": pointer(primitive("lv_obj_t", "lvgl_type")),
                }
            ],
        )

        method_ref = next(ref for ref in ir.api_surface.classes[0].methods if ref.function == "lv_foo_measure")
        self.assertEqual(method_ref.receiver.actual_receiver_type, "lv_obj_t *")

    def test_invalid_explicit_instance_method_is_validation_error(self) -> None:
        method = function("lv_foo_get_value", primitive("int"), [("value", pointer(primitive("other_t", "lvgl_type")))])
        selectors = [
            "lv_foo_get_value",
            {"name": "lv_foo_get_value", "arg_match": [{"index": 0, "name": "value", "type": "lv_obj_t*"}]},
        ]
        for selector in selectors:
            with self.subTest(selector=selector):
                _, config, model, selection, ir = make_binding([method], methods=[selector])

                self.assertNotIn("lv_foo_get_value", ir.api_surface.names)
                mismatch = next(item for item in selection.messages if item.code == "RECEIVER_TYPE_MISMATCH")
                self.assertEqual(mismatch.severity, Severity.ERROR)
                diagnostics = validate_bindings(config.data, model, selection, ir, set())
                self.assertTrue(any(item.code == "RECEIVER_TYPE_MISMATCH" and item.severity == Severity.ERROR for item in diagnostics))

    def test_special_binding_owns_mismatched_receiver_semantics(self) -> None:
        method = function("lv_foo_get_value", primitive("int"), [("value", pointer(primitive("other_t", "lvgl_type")))])
        _, _, _, _, ir = make_binding(
            [method],
            methods=[
                {
                    "name": "lv_foo_get_value",
                    "arg_match": [{"index": 0, "name": "value", "type": "lv_obj_t*"}],
                }
            ],
            special_apis={"lv_foo_get_value": "sni_api_custom_get"},
            special_ids={"sni_api_custom_get"},
        )

        method_ref = next(ref for ref in ir.api_surface.classes[0].methods if ref.function == "lv_foo_get_value")
        self.assertEqual(method_ref.special_binding, "sni_api_custom_get")
        self.assertIsNone(method_ref.receiver)
        self.assertFalse(
            any(
                item.code == "RECEIVER_TYPE_MISMATCH" and item.severity == Severity.ERROR
                for item in ir.diagnostics
            )
        )
        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn('{.name = "getValue", .handler = sni_api_custom_get}', source)

    def test_explicit_special_property_owns_mismatched_receiver_semantics(self) -> None:
        getter = function("lv_foo_get_value", primitive("int"), [("value", pointer(primitive("other_t", "lvgl_type")))])
        _, _, _, _, ir = make_binding(
            [getter],
            special_properties=[
                {
                    "class": "foo",
                    "property": "value",
                    "function": "lv_foo_get_value",
                    "accessor": "getter",
                    "binding": "sni_api_custom_value_get",
                }
            ],
            special_ids={"sni_api_custom_value_get"},
        )

        prop = next(prop for prop in ir.api_surface.classes[0].properties if prop.name == "value")
        self.assertEqual(prop.getter.special_binding, "sni_api_custom_value_get")
        self.assertIsNone(prop.getter.receiver)
        self.assertFalse(any(item.code == "RECEIVER_TYPE_MISMATCH" for item in ir.diagnostics))
        source, _ = render_api(ir.api_surface, (9, 6, 0))
        self.assertIn("{.name = \"value\", .getter = sni_api_custom_value_get", source)

    def test_renderer_requires_receiver_contract_for_ordinary_member(self) -> None:
        method = function("lv_foo_measure", primitive("int"), [("obj", pointer(primitive("lv_obj_t", "lvgl_type")))])
        _, _, _, _, ir = make_binding([method], methods=["lv_foo_measure"])
        reference = next(ref for ref in ir.api_surface.classes[0].methods if ref.function == "lv_foo_measure")
        ir.api_surface.classes[0].methods[0] = ApiExportRef(reference.function)

        with self.assertRaisesRegex(RuntimeError, "no validated receiver contract"):
            render_api(ir.api_surface, (9, 6, 0))


if __name__ == "__main__":
    unittest.main()
