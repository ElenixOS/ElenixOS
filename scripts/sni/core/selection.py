"""Resolve configured API selectors and assemble the selected API surface."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

from .constants import build_constant_index, build_macro_catalog, parse_numeric_constant, resolve_constant
from .ir import ApiClassIR, ApiConstantIR, ApiExportRef, ApiMacroIR, ApiPropertyIR, ApiRecord, ApiStatus, Diagnostic, ReceiverContract, Severity
from .lvgl_model import LVGLModel
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
    macros: list[ApiMacroIR] = field(default_factory=list)


def _special_map(config: dict[str, Any]) -> dict[str, str]:
    special = config.get("special_bindings", {})
    return {**special.get("apis", {}), **special.get("constructors", {})}


def select_apis(lvgl_data: dict[str, Any], config: dict[str, Any], model: LVGLModel | None = None) -> SelectionResult:
    model = model or LVGLModel(lvgl_data)
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

    def class_base_c_types(cls: Any) -> tuple[str, ...]:
        result: list[str] = []
        current = cls
        while current.base:
            current = classes_by_name[current.base]
            result.append(current.c_type)
        return tuple(result)

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

    def receiver_contract(cls: Any, item: dict[str, Any], binding_kind: str) -> tuple[ReceiverContract | None, Any]:
        args = item.get("args", [])
        receiver_type = args[0].get("type") if isinstance(args, list) and args and isinstance(args[0], dict) else None
        compatibility = model.check_instance_receiver(cls.c_type, receiver_type, class_base_c_types(cls))
        if not compatibility.compatible:
            return None, compatibility
        return (
            ReceiverContract(
                owner_class=cls.name,
                binding_kind=binding_kind,
                receiver_index=0,
                expected_receiver_type=compatibility.expected_type,
                actual_receiver_type=compatibility.actual_type,
                receiver_validated=True,
            ),
            compatibility,
        )

    def receiver_mismatch(
        cls: Any,
        item: dict[str, Any],
        binding_kind: str,
        compatibility: Any,
        *,
        automatic: bool,
        member_name: str | None = None,
    ) -> None:
        function_name = str(item.get("name", "")).strip()
        candidate = (
            f"{cls.name}.{lv_api.snake_to_camel(member_name)}"
            if member_name
            else f"{cls.name}.{lv_api.entry_name(function_name)}"
        )
        messages.append(
            Diagnostic(
                code="RECEIVER_TYPE_MISMATCH",
                severity=Severity.INFO if automatic else Severity.ERROR,
                category="selection",
                subject_kind="api",
                subject=function_name,
                api=function_name,
                reason=(
                    f"Function {function_name} is not eligible for {candidate}: expected receiver "
                    f"{compatibility.expected_type}, got {compatibility.actual_type}."
                ),
                details={
                    "function": function_name,
                    "candidate": candidate,
                    "owner_class": cls.name,
                    "binding_kind": binding_kind,
                    "expected_receiver_type": compatibility.expected_type,
                    "actual_receiver_type": compatibility.actual_type,
                    "inference": "automatic" if automatic else "explicit",
                },
                suggested_action=(
                    "Keep this function out of the inferred instance API, or expose it through a deliberate static/special binding."
                    if automatic
                    else "Correct the class method selector or configure a special binding with the intended receiver semantics."
                ),
            )
        )

    for cls in classes:
        class_ctor[cls.name] = cls.constructor is not None
        class_methods[cls.name] = []
        class_static_methods[cls.name] = []
        class_properties[cls.name] = []
        if cls.constructor:
            ctor = lv_api.require_class_function(function_index, cls.name, cls.constructor, f"classes.{cls.name}.constructor")
            add(ctor, cls.name, "constructor", True)
            for selector in cls.methods:
                named_items = lv_api.resolve_class_selector_name_items(function_index, cls.name, selector)
                matched_items = [
                    item
                    for item in named_items
                    if lv_api.function_args_match_selector(
                        selector,
                        item,
                        model,
                        cls.c_type,
                        class_base_c_types(cls),
                        special_receiver=str(item.get("name", "")).strip() in special_by_api,
                    )
                ]
                if not matched_items:
                    exact_mismatches = []
                    for item in named_items:
                        if str(item.get("name", "")).strip() in special_by_api:
                            continue
                        if not lv_api.function_args_match_selector(
                            selector,
                            item,
                            model,
                            cls.c_type,
                            class_base_c_types(cls),
                            skip_receiver=True,
                        ):
                            continue
                        contract, compatibility = receiver_contract(cls, item, "instance_method")
                        if contract is None:
                            exact_mismatches.append((item, compatibility))
                    for item, compatibility in exact_mismatches:
                        receiver_mismatch(cls, item, "instance_method", compatibility, automatic=False)
                    if exact_mismatches:
                        continue
                    lv_api.resolve_class_selector_items(
                        function_index,
                        cls.name,
                        selector,
                        f"classes.{cls.name}.methods",
                        model,
                        cls.c_type,
                        class_base_c_types(cls),
                    )

                for item in matched_items:
                    name = str(item.get("name", "")).strip()
                    special_binding = special_by_api.get(name)
                    method_receiver = None
                    if special_binding is None:
                        method_receiver, compatibility = receiver_contract(cls, item, "instance_method")
                        if method_receiver is None:
                            receiver_mismatch(cls, item, "instance_method", compatibility, automatic=False)
                            continue
                    add(item, cls.name, "method")
                    if name and name not in {ref.function for ref in class_methods[cls.name]} and name in selected:
                        class_methods[cls.name].append(ApiExportRef(name, special_binding, method_receiver))
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
            getter_receiver = None
            setter_receiver = None
            getter_binding = special_property_map.get((cls.name, prop.name, valid_getter, "getter")) if valid_getter else None
            setter_binding = special_property_map.get((cls.name, prop.name, valid_setter, "setter")) if valid_setter else None
            if valid_getter and getter_binding is None:
                getter_receiver, compatibility = receiver_contract(cls, function_index[valid_getter], "property_getter")
                if getter_receiver is None:
                    receiver_mismatch(
                        cls,
                        function_index[valid_getter],
                        "property_getter",
                        compatibility,
                        automatic=True,
                        member_name=prop.name,
                    )
                    valid_getter = None
            if valid_setter and setter_binding is None:
                setter_receiver, compatibility = receiver_contract(cls, function_index[valid_setter], "property_setter")
                if setter_receiver is None:
                    receiver_mismatch(
                        cls,
                        function_index[valid_setter],
                        "property_setter",
                        compatibility,
                        automatic=True,
                        member_name=prop.name,
                    )
                    valid_setter = None
            if valid_getter:
                add(function_index[getter], cls.name, "property_getter")
            if valid_setter:
                add(function_index[setter], cls.name, "property_setter")
            if valid_getter or valid_setter:
                props[(cls.name, prop.name)] = (valid_getter, valid_setter)
                getter_ref = None
                setter_ref = None
                if valid_getter:
                    getter_ref = ApiExportRef(valid_getter, getter_binding, getter_receiver)
                if valid_setter:
                    setter_ref = ApiExportRef(valid_setter, setter_binding, setter_receiver)
                class_properties[cls.name].append(ApiPropertyIR(prop.name, getter_ref, setter_ref))

    # Resolve configured class constants and global constants once, before rendering.
    const_filter = filters["constant"]
    macro_filter = filters["macro"]
    scan_config = api_config.get("scan", {})
    macro_filter_enabled = isinstance(scan_config, dict) and "macro" in scan_config
    constant_index = build_constant_index(lvgl_data)
    constant_names = sorted(constant_index)
    scalar_kinds = {"int", "float", "string"}

    def _macro_is_selected(name: str, parameters: Any, value_kind: str | None) -> bool:
        if parameters is not None or value_kind not in scalar_kinds:
            return False
        export_name = name[3:] if name.startswith("LV_") and len(name) > 3 else name
        if any(lv_api.is_matched(candidate, const_filter["blacklist"]) for candidate in (name, export_name)):
            return False
        return lv_api.include_by_filter(
            name,
            macro_filter["whitelist"],
            macro_filter["blacklist"],
            lv_api.FilterStats(),
        )

    selected_constants = []
    for name in constant_names:
        item = constant_index[name]
        if item.source_kind == "macro":
            # An explicit macro filter gates macros independently of enum constants.
            if macro_filter_enabled and _macro_is_selected(name, item.parameters, item.value_kind):
                selected_constants.append(name)
            # Older configs without scan.macro preserve their historical behavior.
            elif not macro_filter_enabled and lv_api.include_by_filter(
                name,
                const_filter["whitelist"],
                const_filter["blacklist"],
                lv_api.FilterStats(),
            ):
                selected_constants.append(name)
        elif lv_api.include_by_filter(
            name,
            const_filter["whitelist"],
            const_filter["blacklist"],
            lv_api.FilterStats(),
        ):
            selected_constants.append(name)

    macro_catalog = build_macro_catalog(lvgl_data)
    if macro_filter_enabled:
        # Keep runtime macro metadata aligned with the selected scalar macro API.
        macro_catalog = [
            macro
            for macro in macro_catalog
            if _macro_is_selected(macro.name, macro.parameters, macro.value_kind)
        ]

    candidate_names.update(selected)
    render_classes: list[ApiClassIR] = []
    constructor_bindings = special_config.get("constructors", {})
    extensions = special_config.get("class_extensions", {})
    for cls in classes:
        class_constants: list[ApiConstantIR] = []
        for name in cls.constants:
            item = constant_index.get(name) or resolve_constant(lvgl_data, name)
            if item is None:
                raise SystemExit(f"[Error] classes.{cls.name}.constants references unknown constant: {name}")
            class_constants.append(item)
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
        root_constants[export_name] = ApiConstantIR(
            name=export_name,
            value_kind=item.value_kind,
            source_kind=item.source_kind,
            c_expression=item.c_expression,
            source_name=item.source_name,
            availability_guard=item.availability_guard,
            initializer=item.initializer,
            parameters=item.parameters,
        )

    event_assertions: list[tuple[str, str]] = []
    for enum in lvgl_data.get("enums", []):
        if enum.get("name") != "lv_event_code_t":
            continue
        for member in enum.get("members", []):
            name = str(member.get("name", "")).strip()
            number = parse_numeric_constant(str(member.get("value", "")))
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
        macro_catalog,
    )
