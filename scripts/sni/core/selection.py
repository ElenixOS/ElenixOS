"""API selection using the same selectors and property discovery as the old emitter."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

from .ir import ApiRecord, ApiStatus
from ..render import lv_api


@dataclass
class SelectedAPI:
    name: str
    item: dict[str, Any]
    class_name: str | None
    selection_kinds: list[str] = field(default_factory=list)
    special_binding: str | None = None
    is_constructor: bool = False


@dataclass
class SelectionResult:
    classes: list[Any]
    functions: dict[str, SelectedAPI]
    constants: list[str]
    properties: dict[tuple[str, str], tuple[str | None, str | None]]
    messages: list[dict[str, str]]
    pre_rejected: list[ApiRecord] = field(default_factory=list)
    candidate_names: list[str] = field(default_factory=list)


def _special_map(config: dict[str, Any]) -> dict[str, str]:
    special = config.get("special_bindings", {})
    return {**special.get("apis", {}), **special.get("constructors", {})}


def select_apis(lvgl_data: dict[str, Any], config: dict[str, Any]) -> SelectionResult:
    api_config = config["api_selection"]
    filters = lv_api.parse_api_filters(api_config)
    classes_by_name = lv_api.parse_api_classes(api_config)
    classes = lv_api.topo_sort_classes(classes_by_name)
    function_index = lv_api.build_function_index(lvgl_data)
    selected: dict[str, SelectedAPI] = {}
    props: dict[tuple[str, str], tuple[str | None, str | None]] = {}
    messages: list[dict[str, str]] = []
    pre_rejected: dict[str, ApiRecord] = {}
    candidate_names: set[str] = set()
    special_by_api = _special_map(config)
    rejection_reasons = config.get("api_rejection_reasons", {})
    special_properties = config.get("special_bindings", {}).get("properties", [])
    class_ctor: dict[str, bool] = {}

    def add(item: dict[str, Any], class_name: str | None, kind: str, is_ctor: bool = False) -> None:
        name = str(item.get("name", "")).strip()
        if not name:
            return
        candidate_names.add(name)
        if lv_api.is_matched(name, filters["function"]["blacklist"]):
            reason = rejection_reasons.get(name, "excluded by scan.function.blacklist")
            messages.append({"severity": "info", "code": "API_BLACKLISTED", "message": f"{name}: {reason}"})
            pre_rejected[name] = ApiRecord(name, ApiStatus.REJECTED, class_name, kind, reason=reason)
            return
        entry = selected.get(name)
        if entry is None:
            entry = SelectedAPI(name, item, class_name)
            selected[name] = entry
        elif entry.item is not item and entry.item != item:
            messages.append({"severity": "error", "code": "DUPLICATE_API_IDENTITY", "message": f"{name} resolves to conflicting declarations"})
        if kind not in entry.selection_kinds:
            entry.selection_kinds.append(kind)
        entry.is_constructor = entry.is_constructor or is_ctor
        entry.special_binding = special_by_api.get(name)

    for cls in classes:
        class_ctor[cls.name] = cls.constructor is not None
        if cls.constructor:
            ctor = lv_api.require_class_function(function_index, cls.name, cls.constructor, f"classes.{cls.name}.constructor")
            add(ctor, cls.name, "constructor", True)
            for selector in cls.methods:
                for item in lv_api.resolve_class_selector_items(function_index, cls.name, selector, f"classes.{cls.name}.methods"):
                    add(item, cls.name, "method")
        for selector in cls.static_methods:
            for item in lv_api.resolve_class_selector_items(function_index, cls.name, selector, f"classes.{cls.name}.static_methods"):
                add(item, cls.name, "static_method")

        for prop in lv_api.discover_class_properties(cls, function_index, filters):
            getter = prop.getter
            setter = prop.setter
            # Match the legacy emitter's this-only getter / this+value setter validation.
            getter_args = lv_api.normalize_args(function_index[getter].get("args", [])) if getter else []
            getter_return = lv_api.normalize_type_key(lv_api.normalize_c_type(function_index[getter].get("type"))) if getter else "void"
            valid_getter = getter if getter and len(getter_args) == 1 and getter_return != "void" else None
            valid_setter = setter if setter and len(lv_api.normalize_args(function_index[setter].get("args", []))) == 2 else None
            if valid_getter:
                add(function_index[getter], cls.name, "property_getter")
            if valid_setter:
                add(function_index[setter], cls.name, "property_setter")
            if valid_getter or valid_setter:
                props[(cls.name, prop.name)] = (valid_getter, valid_setter)

    # Global constants use the existing scan selectors and class-specific constants remain validated by the emitter.
    const_filter = filters["constant"]
    constant_names = sorted(lv_api.build_constant_index(lvgl_data))
    selected_constants = [
        name
        for name in constant_names
        if lv_api.include_by_filter(name, const_filter["whitelist"], const_filter["blacklist"], lv_api.FilterStats())
    ]

    for cls_name, extras in config.get("special_bindings", {}).get("class_extensions", {}).get("methods", {}).items():
        for item in extras:
            messages.append({"severity": "info", "code": "SPECIAL_INJECTED", "message": f"{cls_name}.{item['name']} uses injected special binding {item['binding']}"})

    candidate_names.update(selected)
    return SelectionResult(classes, selected, selected_constants, props, messages, list(pre_rejected.values()), sorted(candidate_names))
