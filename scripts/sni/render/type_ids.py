"""Render deterministic generated SNI type identifiers."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from ..core.ir import BindingIR, Representation


def _managed_id(type_name: str) -> str:
    base = type_name[:-2] if type_name.endswith("_t") else type_name
    return "SNI_H_" + base.upper()


def _value_id(type_name: str) -> str:
    base = type_name[:-2] if type_name.endswith("_t") else type_name
    return "SNI_V_" + base.upper()


def build_type_ids(ir: BindingIR, config: dict[str, Any]) -> tuple[str, dict[str, str], dict[str, list[str]]]:
    runtime = config.get("runtime_type_ids", {})
    groups = {
        name: list(runtime.get(name, []))
        for name in ("tree_dependent", "hybrid", "pure_managed", "legacy_handles")
    }
    groups["tree_node"] = ["SNI_H_LV_OBJ"]

    # Ensure each explicit LVGL Managed Resource has a type ID in the correct formal range.
    category_group = {"tree_dependent": "tree_dependent", "hybrid": "hybrid", "pure_managed": "pure_managed"}
    for name, declaration in config.get("type_declarations", {}).items():
        if declaration.get("kind") != "managed_resource":
            continue
        group = category_group[declaration["category"]]
        identifier = _managed_id(name)
        if identifier not in groups[group]:
            groups[group].append(identifier)

    value_types = sorted(
        {
            _value_id(info.canonical_name)
            for info in ir.types.values()
            if info.representation == Representation.VALUE_OBJECT
        }
    )
    groups["value_objects"] = value_types

    enum_items = [
        "    __SNI_TYPE_START = 0,",
        "    SNI_T_UNKNOWN = 0,",
        "",
        "    __SNI_TYPE_NUMBER_START,",
        "    SNI_T_UINT8,",
        "    SNI_T_INT8,",
        "    SNI_T_UINT16,",
        "    SNI_T_INT16,",
        "    SNI_T_UINT32,",
        "    SNI_T_INT32,",
        "    SNI_T_DOUBLE,",
        "    SNI_T_FLOAT,",
        "    __SNI_TYPE_NUMBER_END,",
        "",
        "    SNI_T_BOOL,",
        "    SNI_T_STRING,",
        "    SNI_T_PTR,",
        "",
        "    __SNI_HANDLE_START,",
        "    SNI_H_LV_OBJ,",
        "    __SNI_HANDLE_RESOURCE_START,",
        "",
        "    __SNI_TREE_DEPENDENT_RESOURCE_START,",
    ]
    enum_items.extend(f"    {item}," for item in groups["tree_dependent"])
    enum_items += ["    __SNI_TREE_DEPENDENT_RESOURCE_END,", "", "    __SNI_HYBRID_RESOURCE_START,"]
    enum_items.extend(f"    {item}," for item in groups["hybrid"])
    enum_items += ["    __SNI_HYBRID_RESOURCE_END,", "", "    __SNI_PURE_MANAGED_RESOURCE_START,"]
    enum_items.extend(f"    {item}," for item in groups["pure_managed"])
    enum_items += ["    __SNI_PURE_MANAGED_RESOURCE_END,"]
    enum_items.extend(f"    {item}," for item in groups["legacy_handles"])
    enum_items += ["", "    __SNI_HANDLE_RESOURCE_END,", "    __SNI_HANDLE_END,", "", "    __SNI_VALUE_START,"]
    enum_items.extend(f"    {item}," for item in value_types)
    enum_items += ["    __SNI_VALUE_END,", "", "    __SNI_TYPE_MAX"]

    template_path = Path(__file__).resolve().parents[1] / "templates" / "sni_type_ids.h.in"
    template = template_path.read_text(encoding="utf-8")
    generated = template.replace("@ENUM_ITEMS@", "\n".join(enum_items))

    names: dict[str, str] = {"lv_obj_t": "SNI_H_LV_OBJ"}
    for info in ir.types.values():
        if info.representation == Representation.VALUE_OBJECT:
            identifier = _value_id(info.canonical_name)
        elif info.representation == Representation.MANAGED_RESOURCE:
            identifier = _managed_id(info.canonical_name)
        elif info.representation == Representation.OBJECT_TREE_NODE:
            identifier = "SNI_H_LV_OBJ"
        else:
            continue
        names[info.name] = identifier
        names[info.canonical_name] = identifier

    return generated, names, groups
