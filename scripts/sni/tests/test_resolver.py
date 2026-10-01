from __future__ import annotations

import unittest

from sni.core.ir import ApiStatus, Representation
from sni.core.lvgl_model import LVGLModel, parse_type_node
from sni.core.resolver import TypeResolver
from sni.core.selection import SelectedAPI, SelectionResult


def scalar(name: str, kind: str = "stdlib_type", quals: list[str] | None = None) -> dict:
    return {"name": name, "json_type": kind, "quals": quals or []}


def pointer(base: dict, quals: list[str] | None = None) -> dict:
    return {"type": base, "json_type": "pointer", "quals": quals or []}


def fixture() -> LVGLModel:
    return LVGLModel(
        {
            "functions": [],
            "enums": [{"name": "lv_mode_t", "json_type": "enum", "type": scalar("int", "primitive_type"), "members": []}],
            "structures": [
                {"name": "lv_point_t", "json_type": "struct", "fields": [
                    {"name": "x", "type": scalar("int16_t"), "bitsize": None},
                    {"name": "y", "type": scalar("int16_t"), "bitsize": None},
                ]},
                {"name": "lv_box_t", "json_type": "struct", "fields": [
                    {"name": "origin", "type": scalar("lv_point_t", "lvgl_type"), "bitsize": None},
                    {"name": "mode", "type": scalar("lv_mode_t", "lvgl_type"), "bitsize": None},
                ]},
                {"name": "lv_pointer_value_t", "json_type": "struct", "fields": [
                    {"name": "data", "type": pointer(scalar("uint8_t")), "bitsize": None},
                ]},
                {"name": "lv_bits_t", "json_type": "struct", "fields": [
                    {"name": "flags", "type": scalar("uint32_t"), "bitsize": "3"},
                ]},
                {"name": "lv_obj_t", "json_type": "forward_decl", "fields": []},
                {"name": "lv_timer_t", "json_type": "forward_decl", "fields": []},
                {"name": "lv_chart_series_t", "json_type": "forward_decl", "fields": []},
                {"name": "lv_external_t", "json_type": "forward_decl", "fields": []},
            ],
            "unions": [{"name": "lv_payload_t", "json_type": "union", "fields": []}],
            "typedefs": [
                {"name": "lv_mode_alias_t", "json_type": "typedef", "type": scalar("lv_mode_t", "lvgl_type")},
                {"name": "lv_nested_alias_t", "json_type": "typedef", "type": scalar("lv_mode_alias_t", "lvgl_type")},
            ],
            "function_pointers": [{"name": "lv_callback_t", "json_type": "function_pointer", "type": scalar("void", "primitive_type"), "args": []}],
            "forward_decls": [],
            "variables": [],
            "macros": [],
        }
    )


def empty_selection() -> SelectionResult:
    return SelectionResult([], {}, [], {}, [])


class TypeResolverTests(unittest.TestCase):
    def setUp(self) -> None:
        self.model = fixture()
        self.config = {"type_declarations": {}, "special_conversions": {}, "special_bindings": {}}

    def test_primitive_and_enum_resolve_to_basic_values(self) -> None:
        resolver = TypeResolver(self.model, self.config, empty_selection())
        self.assertEqual(resolver.resolve_type("int32_t").representation, Representation.PRIMITIVE)
        self.assertEqual(resolver.resolve_type("lv_mode_t").representation, Representation.ENUM)

    def test_nested_typedef_aliases_reach_canonical_enum(self) -> None:
        canonical, category, chain = self.model.resolve_declaration("lv_nested_alias_t")
        self.assertEqual(canonical, "lv_mode_t")
        self.assertEqual(category.value, "enum")
        self.assertEqual(chain, ["lv_nested_alias_t", "lv_mode_alias_t"])
        self.assertEqual(TypeResolver(self.model, self.config, empty_selection()).resolve_type("lv_nested_alias_t").representation, Representation.ENUM)

    def test_simple_and_nested_value_objects_are_inferred(self) -> None:
        resolver = TypeResolver(self.model, self.config, empty_selection())
        self.assertEqual(resolver.resolve_type("lv_point_t").representation, Representation.VALUE_OBJECT)
        box = resolver.resolve_type("lv_box_t")
        self.assertEqual(box.representation, Representation.VALUE_OBJECT)
        self.assertIn("lv_point_t", box.dependencies)

    def test_pointer_member_and_union_are_rejected(self) -> None:
        resolver = TypeResolver(self.model, self.config, empty_selection())
        pointer_struct = resolver.resolve_type("lv_pointer_value_t")
        self.assertEqual(pointer_struct.representation, Representation.REJECTED)
        self.assertIn("pointer member", pointer_struct.rejection_reason)
        self.assertEqual(resolver.resolve_type("lv_payload_t").representation, Representation.REJECTED)

    def test_callback_typedef_is_special_required(self) -> None:
        info = TypeResolver(self.model, self.config, empty_selection()).resolve_type("lv_callback_t")
        self.assertEqual(info.representation, Representation.SPECIAL_REQUIRED)

    def test_array_node_keeps_element_type_and_shape(self) -> None:
        node = {"name": "lv_point_t", "json_type": "array", "dim": None, "quals": ["const"]}
        use = parse_type_node(node).use_site
        self.assertEqual(use.base_name, "lv_point_t")
        self.assertEqual(use.array_shape, (None,))
        self.assertEqual(use.pointer_depth, 0)
        self.assertTrue(use.is_const)
        selected = SelectedAPI("lv_use_array", {"name": "lv_use_array", "type": scalar("void", "primitive_type"), "args": [{"name": "points", "type": node}]}, "fixture", ["method"])
        result = TypeResolver(self.model, self.config, SelectionResult([], {selected.name: selected}, [], {}, [])).resolve_all()
        self.assertEqual(result.apis[0].status, ApiStatus.REJECTED)
        self.assertIn("array shape", result.apis[0].reason)

    def test_explicit_value_object_allows_reviewed_bitfield_layout(self) -> None:
        config = {**self.config, "type_declarations": {"lv_bits_t": {"kind": "value_object", "reason": "Reviewed bitfield layout."}}}
        info = TypeResolver(self.model, config, empty_selection()).resolve_type("lv_bits_t")
        self.assertEqual(info.representation, Representation.VALUE_OBJECT)
        self.assertEqual(info.resolution_source, "explicit")

    def test_unknown_type_stays_unresolved(self) -> None:
        selected = SelectedAPI("lv_use_unknown", {"name": "lv_use_unknown", "type": scalar("lv_missing_t", "lvgl_type"), "args": []}, "fixture", ["static_method"])
        selection = SelectionResult([], {selected.name: selected}, [], {}, [])
        ir = TypeResolver(self.model, self.config, selection).resolve_all()
        self.assertEqual(ir.apis[0].status, ApiStatus.CANDIDATE)
        self.assertTrue(any(item["code"] == "UNRESOLVED_TYPE" for item in ir.diagnostics))

    def test_explicit_override_redundancy_and_unused_diagnostics(self) -> None:
        config = {
            **self.config,
            "type_declarations": {
                "lv_point_t": {"kind": "value_object", "reason": "Manual declaration."},
                "lv_bits_t": {"kind": "value_object", "reason": "Reviewed layout."},
                "lv_unused_t": {"kind": "unknown", "reason": "TODO"},
            },
        }
        ir = TypeResolver(self.model, config, empty_selection()).resolve_all()
        codes = {item["code"] for item in ir.diagnostics}
        self.assertIn("REDUNDANT_EXPLICIT_DECLARATION", codes)
        self.assertIn("UNUSED_EXPLICIT_DECLARATION", codes)

    def test_object_tree_constructor_and_ordinary_getter_are_nodes(self) -> None:
        funcs = {
            "lv_button_create": {"name": "lv_button_create", "type": pointer(scalar("lv_obj_t", "lvgl_type")), "args": [{"name": "parent", "type": pointer(scalar("lv_obj_t", "lvgl_type"))}]},
            "lv_obj_get_child": {"name": "lv_obj_get_child", "type": pointer(scalar("lv_obj_t", "lvgl_type")), "args": [{"name": "obj", "type": pointer(scalar("lv_obj_t", "lvgl_type"))}]},
        }
        apis = {
            name: SelectedAPI(name, item, "obj", ["constructor"] if name.endswith("create") else ["static_method"], is_constructor=name.endswith("create"))
            for name, item in funcs.items()
        }
        ir = TypeResolver(self.model, self.config, SelectionResult([], apis, [], {}, [])).resolve_all()
        self.assertEqual(ir.apis[0].status, ApiStatus.ACCEPTED)
        self.assertTrue(all(use.sni_type == "SNI_H_LV_OBJ" for use in ir.uses if use.use_site.base_name == "lv_obj_t"))

    def test_pure_managed_constructor_allowed_but_getter_rejected(self) -> None:
        config = {
            **self.config,
            "type_declarations": {"lv_timer_t": {"kind": "managed_resource", "category": "pure_managed", "creator": "lv_timer_create"}},
        }
        ctor = SelectedAPI("lv_timer_create", {"name": "lv_timer_create", "type": pointer(scalar("lv_timer_t", "lvgl_type")), "args": []}, "timer", ["constructor"], is_constructor=True)
        getter = SelectedAPI("lv_timer_get_next", {"name": "lv_timer_get_next", "type": pointer(scalar("lv_timer_t", "lvgl_type")), "args": []}, "timer", ["static_method"])
        result = TypeResolver(self.model, config, SelectionResult([], {ctor.name: ctor, getter.name: getter}, [], {}, [])).resolve_all()
        statuses = {item.name: item.status for item in result.apis}
        self.assertEqual(statuses["lv_timer_create"], ApiStatus.ACCEPTED)
        self.assertEqual(statuses["lv_timer_get_next"], ApiStatus.REJECTED)

    def test_tree_dependent_creator_requires_parent_node(self) -> None:
        config = {
            **self.config,
            "type_declarations": {"lv_chart_series_t": {"kind": "managed_resource", "category": "tree_dependent", "creator": "lv_chart_add_series"}},
        }
        item = {"name": "lv_chart_add_series", "type": pointer(scalar("lv_chart_series_t", "lvgl_type")), "args": [{"name": "chart", "type": pointer(scalar("lv_obj_t", "lvgl_type"))}]}
        selected = SelectedAPI(item["name"], item, "chart", ["method"])
        result = TypeResolver(self.model, config, SelectionResult([], {selected.name: selected}, [], {}, [])).resolve_all()
        self.assertEqual(result.apis[0].status, ApiStatus.ACCEPTED)

    def test_tree_dependent_getter_keeps_category_when_parent_is_known(self) -> None:
        config = {
            **self.config,
            "type_declarations": {"lv_chart_series_t": {"kind": "managed_resource", "category": "tree_dependent", "creator": "lv_chart_add_series"}},
        }
        item = {"name": "lv_chart_get_series_next", "type": pointer(scalar("lv_chart_series_t", "lvgl_type")), "args": [{"name": "chart", "type": pointer(scalar("lv_obj_t", "lvgl_type"))}]}
        selected = SelectedAPI(item["name"], item, "chart", ["method"])
        result = TypeResolver(self.model, config, SelectionResult([], {selected.name: selected}, [], {}, [])).resolve_all()
        self.assertEqual(result.apis[0].status, ApiStatus.ACCEPTED)
        self.assertEqual(result.uses[0].sni_type, "SNI_H_LV_CHART_SERIES")

    def test_unmanaged_native_pointer_is_rejected(self) -> None:
        item = {"name": "lv_external_get", "type": pointer(scalar("lv_external_t", "lvgl_type")), "args": []}
        selected = SelectedAPI(item["name"], item, "fixture", ["static_method"])
        result = TypeResolver(self.model, self.config, SelectionResult([], {selected.name: selected}, [], {}, [])).resolve_all()
        self.assertEqual(result.apis[0].status, ApiStatus.REJECTED)


if __name__ == "__main__":
    unittest.main()
