"""Resolve configured API selectors and assemble the selected API surface."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

from .ir import ApiClassIR, ApiConstantIR, ApiExportRef, ApiPropertyIR, ApiRecord, ApiStatus, Diagnostic, Severity
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
    messages: list[Diagnostic]
    pre_rejected: list[ApiRecord] = field(default_factory=list)
    candidate_names: list[str] = field(default_factory=list)
    render_classes: list[ApiClassIR] = field(default_factory=list)
    root_constants: list[ApiConstantIR] = field(default_factory=list)
    event_assertions: list[tuple[str, str]] = field(default_factory=list)


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
    messages: list[Diagnostic] = []
    pre_rejected: dict[str, ApiRecord] = {}
    candidate_names: set[str] = set()
    special_by_api = _special_map(config)
    special_config = config.get("special_bindings", {})
    special_property_map = {
        (item["class"], item["property"], item["function"], item["accessor"]): item["binding"]
        for item in special_config.get("properties", [])
    }
    rejection_reasons = config.get("api_rejection_reasons", {})
    special_properties = config.get("special_bindings", {}).get("properties", [])
    class_ctor: dict[str, bool] = {}
    class_methods: dict[str, list[ApiExportRef]] = {}
    class_static_methods: dict[str, list[ApiExportRef]] = {}
    class_properties: dict[str, list[ApiPropertyIR]] = {}

    def add(item: dict[str, Any], class_name: str | None, kind: str, is_ctor: bool = False) -> None:
        name = str(item.get("name", "")).strip()
        if not name:
            return
        candidate_names.add(name)
        if lv_api.is_matched(name, filters["function"]["blacklist"]):
            reason = rejection_reasons.get(name, "excluded by scan.function.blacklist")
            pre_rejected[name] = ApiRecord(
                name,
                ApiStatus.EXCLUDED_BLACKLIST,
                class_name,
                kind,
                reason=reason,
                reason_code="BLACKLISTED_BY_CONFIG",
            )
            return
        entry = selected.get(name)
        if entry is None:
            entry = SelectedAPI(name, item, class_name)
            selected[name] = entry
        elif entry.item is not item and entry.item != item:
            messages.append(
                Diagnostic(
                    code="DUPLICATE_API_IDENTITY",
                    severity=Severity.ERROR,
                    category="selection",
                    subject_kind="api",
                    subject=name,
                    api=name,
                    reason=f"{name} resolves to conflicting declarations",
                    suggested_action="Remove the conflicting declaration or narrow the API selector.",
                )
            )
        if kind not in entry.selection_kinds:
            entry.selection_kinds.append(kind)
        entry.is_constructor = entry.is_constructor or is_ctor
        entry.special_binding = special_by_api.get(name)

    for cls in classes:
        class_ctor[cls.name] = cls.constructor is not None
        class_methods[cls.name] = []
        class_static_methods[cls.name] = []
        class_properties[cls.name] = []
        if cls.constructor:
            ctor = lv_api.require_class_function(function_index, cls.name, cls.constructor, f"classes.{cls.name}.constructor")
            add(ctor, cls.name, "constructor", True)
            for selector in cls.methods:
                for item in lv_api.resolve_class_selector_items(function_index, cls.name, selector, f"classes.{cls.name}.methods"):
                    add(item, cls.name, "method")
                    name = str(item.get("name", "")).strip()
                    if name and name not in {ref.function for ref in class_methods[cls.name]} and name in selected:
                        class_methods[cls.name].append(ApiExportRef(name, special_by_api.get(name)))
        for selector in cls.static_methods:
            for item in lv_api.resolve_class_selector_items(function_index, cls.name, selector, f"classes.{cls.name}.static_methods"):
                add(item, cls.name, "static_method")
                name = str(item.get("name", "")).strip()
                if name and name not in {ref.function for ref in class_static_methods[cls.name]} and name in selected:
                    class_static_methods[cls.name].append(ApiExportRef(name, special_by_api.get(name)))

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
                getter_ref = None
                setter_ref = None
                if valid_getter:
                    binding = special_property_map.get((cls.name, prop.name, valid_getter, "getter"))
                    getter_ref = ApiExportRef(valid_getter, binding)
                if valid_setter:
                    binding = special_property_map.get((cls.name, prop.name, valid_setter, "setter"))
                    setter_ref = ApiExportRef(valid_setter, binding)
                class_properties[cls.name].append(ApiPropertyIR(prop.name, getter_ref, setter_ref))

    # Resolve configured class constants and global constants once, before rendering.
    const_filter = filters["constant"]
    constant_index = lv_api.build_constant_index(lvgl_data)
    constant_names = sorted(constant_index)
    selected_constants = [
        name
        for name in constant_names
        if lv_api.include_by_filter(name, const_filter["whitelist"], const_filter["blacklist"], lv_api.FilterStats())
    ]

    candidate_names.update(selected)
    render_classes: list[ApiClassIR] = []
    constructor_bindings = special_config.get("constructors", {})
    extensions = special_config.get("class_extensions", {})
    for cls in classes:
        class_constants: list[ApiConstantIR] = []
        for name in cls.constants:
            if name not in constant_index:
                raise SystemExit(f"[Error] classes.{cls.name}.constants references unknown constant: {name}")
            item = constant_index[name]
            class_constants.append(ApiConstantIR(item.name, item.kind, item.value))
        extra_methods = [
            (item["name"], item["binding"])
            for item in extensions.get("methods", {}).get(cls.name, [])
        ]
        extra_properties = [
            (item["name"], item.get("getter"), item.get("setter"))
            for item in extensions.get("properties", {}).get(cls.name, [])
        ]
        render_classes.append(
            ApiClassIR(
                name=cls.name,
                c_type=cls.c_type,
                configured_constructor=cls.constructor,
                has_constructor=cls.constructor is not None,
                constructor_binding=constructor_bindings.get(cls.constructor),
                base=cls.base,
                methods=class_methods[cls.name],
                static_methods=class_static_methods[cls.name],
                properties=class_properties[cls.name],
                constants=class_constants,
                extra_methods=extra_methods,
                extra_properties=extra_properties,
            )
        )

    root_items = [constant_index[name] for name in selected_constants]
    root_constants: dict[str, ApiConstantIR] = {}
    for item in root_items:
        export_name = item.name[3:] if item.name.startswith("LV_") and len(item.name) > 3 else item.name
        if any(lv_api.is_matched(candidate, const_filter["blacklist"]) for candidate in (item.name, export_name)):
            continue
        root_constants[export_name] = ApiConstantIR(export_name, item.kind, item.value)

    event_assertions: list[tuple[str, str]] = []
    for enum in lvgl_data.get("enums", []):
        if enum.get("name") != "lv_event_code_t":
            continue
        for member in enum.get("members", []):
            name = str(member.get("name", "")).strip()
            number = lv_api.parse_numeric_constant(str(member.get("value", "")))
            if name and number is not None and number[0] == "int":
                event_assertions.append((name, number[1]))
        break

    return SelectionResult(
        classes,
        selected,
        selected_constants,
        props,
        messages,
        list(pre_rejected.values()),
        sorted(candidate_names),
        render_classes,
        [root_constants[name] for name in sorted(root_constants)],
        event_assertions,
    )
