"""Conservative C type resolution and API/use-site admission."""

from __future__ import annotations

from typing import Any

from .ir import ApiClassIR, ApiRecord, ApiStatus, ApiUse, BindingIR, CCategory, CUseSite, Diagnostic, Representation, ResolvedApiSurface, Severity, TypeInfo
from .lvgl_model import BUILTIN_C_TYPES, LVGLModel, parse_type_node
from .selection import SelectionResult


PRIMITIVE_SNI = {
    "bool": "SNI_T_BOOL",
    "char": "SNI_T_INT8",
    "signed char": "SNI_T_INT8",
    "unsigned char": "SNI_T_UINT8",
    "int8_t": "SNI_T_INT8",
    "uint8_t": "SNI_T_UINT8",
    "short": "SNI_T_INT16",
    "short int": "SNI_T_INT16",
    "int16_t": "SNI_T_INT16",
    "uint16_t": "SNI_T_UINT16",
    "int": "SNI_T_INT32",
    "unsigned": "SNI_T_UINT32",
    "unsigned int": "SNI_T_UINT32",
    "long": "SNI_T_INT32",
    "long int": "SNI_T_INT32",
    "unsigned long": "SNI_T_UINT32",
    "long long": "SNI_T_INT32",
    "unsigned long long": "SNI_T_UINT32",
    "int32_t": "SNI_T_INT32",
    "uint32_t": "SNI_T_UINT32",
    "int64_t": "SNI_T_INT32",
    "uint64_t": "SNI_T_UINT32",
    "intptr_t": "SNI_T_INT32",
    "uintptr_t": "SNI_T_UINT32",
    "size_t": "SNI_T_UINT32",
    "ptrdiff_t": "SNI_T_INT32",
    "float": "SNI_T_FLOAT",
    "double": "SNI_T_DOUBLE",
}


def _sni_name(name: str) -> str:
    base = name[:-2] if name.endswith("_t") else name
    return "SNI_V_" + base.upper()


class TypeResolver:
    def __init__(self, model: LVGLModel, config: dict[str, Any], selection: SelectionResult):
        self.model = model
        self.config = config
        self.selection = selection
        self.declarations = config.get("type_declarations", {})
        self.types: dict[str, TypeInfo] = {}
        self._resolving: set[str] = set()
        self.explicit_used: set[str] = set()

    def resolve_type(self, name: str) -> TypeInfo:
        if name in self.types:
            return self.types[name]
        canonical, category, chain = self.model.resolve_declaration(name)
        explicit = self.declarations.get(name) or self.declarations.get(canonical)
        info = TypeInfo(name, canonical, category, typedef_chain=chain)
        self.types[name] = info
        if explicit:
            self.explicit_used.add(name if name in self.declarations else canonical)
            if explicit["kind"] == "unknown":
                info.representation = Representation.UNKNOWN
                info.resolution_source = "explicit"
                info.rejection_reason = explicit.get("reason", "awaiting human review")
                return info
            if explicit["kind"] == "managed_resource":
                info.representation = Representation.MANAGED_RESOURCE
                info.resource_category = explicit["category"]
                info.resolution_source = "explicit"
                info.inference_reason = f"declared {explicit['category']} resource; creator {explicit['creator']} is separately admitted"
                return info
            if explicit["kind"] == "value_object":
                info.representation = Representation.VALUE_OBJECT
                info.resolution_source = "explicit"
                info.inference_reason = explicit.get("reason", "explicit SNI value-object declaration")
                if category != CCategory.STRUCT:
                    info.representation = Representation.REJECTED
                    info.rejection_reason = "explicit value_object must refer to a declared struct"
                    return info
                if name in self._resolving:
                    info.representation = Representation.REJECTED
                    info.rejection_reason = "recursive value-object dependency"
                    return info
                self._resolving.add(name)
                info.fields = self.model.field_facts(canonical)
                for field in info.fields:
                    if field.unsupported_reason:
                        info.representation = Representation.REJECTED
                        info.rejection_reason = f"field {field.name} is an {field.unsupported_reason}"
                        break
                    child = self.resolve_type(field.type_name)
                    info.dependencies.append(child.name)
                    if field.pointer_depth or field.array_shape or field.is_function_pointer:
                        info.representation = Representation.REJECTED
                        info.rejection_reason = f"field {field.name} requires unsupported pointer, array, or callback handling"
                        break
                    if child.representation not in {Representation.PRIMITIVE, Representation.ENUM, Representation.VALUE_OBJECT}:
                        info.representation = Representation.REJECTED
                        info.rejection_reason = f"field {field.name} has non-value type {child.name}"
                        break
                self._resolving.remove(name)
                return info

        if canonical == "lv_obj_t":
            info.representation = Representation.OBJECT_TREE_NODE
            info.resolution_source = "inferred"
            info.inference_reason = "LVGL object-tree ownership supplies deletion observation independent of the API use-site pointer form"
            return info

        if category == CCategory.UNKNOWN:
            info.representation = Representation.UNKNOWN
            info.rejection_reason = f"{name} is absent from lvgl.json and has no explicit binding declaration"
            return info
        if category == CCategory.FUNCTION_POINTER:
            info.representation = Representation.SPECIAL_REQUIRED
            info.rejection_reason = "callback values require a registered special binding"
            return info
        if category == CCategory.UNION:
            info.representation = Representation.REJECTED
            info.rejection_reason = "unions are not automatically representable"
            return info
        if category == CCategory.ENUM:
            info.representation = Representation.ENUM
            info.resolution_source = "inferred"
            info.inference_reason = "LVGL enum declarations cross the SNI boundary as their integral value"
            return info
        if canonical == "void":
            info.representation = Representation.PRIMITIVE
            info.resolution_source = "builtin"
            info.inference_reason = "void denotes a function with no JavaScript result"
            return info
        if canonical in PRIMITIVE_SNI:
            info.representation = Representation.PRIMITIVE
            info.resolution_source = "builtin" if canonical in BUILTIN_C_TYPES else "inferred"
            info.inference_reason = f"canonical C scalar maps to {PRIMITIVE_SNI[canonical]}"
            return info
        if category == CCategory.STRUCT:
            if name in self._resolving:
                info.representation = Representation.REJECTED
                info.rejection_reason = "recursive struct dependency"
                return info
            self._resolving.add(name)
            info.fields = self.model.field_facts(canonical)
            eligible = bool(info.fields)
            reasons: list[str] = []
            for field in info.fields:
                if field.unsupported_reason:
                    eligible = False
                    if field.unsupported_reason == "inline struct/union":
                        reasons.append(f"inline struct/union member `{field.name}`")
                    else:
                        reasons.append(f"member `{field.name}`: {field.unsupported_reason}")
                    continue
                child = self.resolve_type(field.type_name)
                info.dependencies.append(child.name)
                if field.pointer_depth:
                    eligible = False
                    reasons.append(f"pointer member `{field.name}`")
                elif field.is_function_pointer:
                    eligible = False
                    reasons.append(f"callback member `{field.name}`")
                elif field.array_shape:
                    eligible = False
                    reasons.append(f"array {list(field.array_shape)} member `{field.name}`")
                elif field.bitsize is not None:
                    eligible = False
                    reasons.append(f"bitfield `{field.name}` needs explicit layout review")
                elif child.representation not in {Representation.PRIMITIVE, Representation.ENUM, Representation.VALUE_OBJECT}:
                    eligible = False
                    reasons.append(f"member `{field.name}` uses {child.representation.value} type `{field.type_name}`")
            self._resolving.remove(name)
            if eligible:
                info.representation = Representation.VALUE_OBJECT
                info.resolution_source = "inferred"
                info.inference_reason = "all fields recursively resolve to primitive, enum, or confirmed value-object types"
            else:
                info.representation = Representation.REJECTED
                info.rejection_reason = "; ".join(reasons) or "no public struct fields are available for generic value-object inference"
            return info
        if category == CCategory.TYPEDEF:
            # resolve_declaration() normally removes typedef wrappers; reaching this branch means the alias target is unknown.
            info.representation = Representation.UNKNOWN
            info.rejection_reason = f"typedef {name} does not resolve to a known canonical type"
            return info
        info.representation = Representation.UNKNOWN
        info.rejection_reason = f"no resolver rule for {category.value} type {name}"
        return info

    @staticmethod
    def collect_referenced_type_names(selection: SelectionResult, config: dict[str, Any]) -> list[str]:
        """Collect signature and special-wrapper type references before resolution."""
        names: set[str] = set()
        for selected in selection.functions.values():
            names.add(parse_type_node(selected.item.get("type")).use_site.base_name)
            args = selected.item.get("args", [])
            if not isinstance(args, list):
                continue
            for arg in args:
                if not isinstance(arg, dict):
                    continue
                use = parse_type_node(arg.get("type")).use_site
                if use.base_name == "void" and use.pointer_depth == 0 and not use.array_shape and not arg.get("name"):
                    continue
                names.add(use.base_name)
        for type_names in config.get("special_bindings", {}).get("type_dependencies", {}).values():
            names.update(type_names)
        return sorted(names)

    def resolve_all(self, expected_type_names: list[str] | None = None) -> BindingIR:
        ir = BindingIR()
        api_map = self.selection.functions
        properties = self.config.get("special_bindings", {}).get("properties", [])
        special_property_functions = {item["function"] for item in properties}
        for api_name in sorted(api_map):
            selected = api_map[api_name]
            is_special = selected.special_binding is not None
            override = self.config.get("function_overrides", {}).get(api_name, {})
            is_special_property_only = (
                api_name in special_property_functions
                and set(selected.selection_kinds) <= {"property_getter", "property_setter"}
            )
            uses: list[ApiUse] = []
            raw_return = selected.item.get("type")
            return_node = parse_type_node(raw_return)
            uses.append(
                self._resolve_use(
                    api_name,
                    "return",
                    return_node.use_site,
                    is_special or is_special_property_only,
                    selected.is_constructor,
                    selected.item,
                )
            )
            args = selected.item.get("args", [])
            if isinstance(args, list):
                for index, arg in enumerate(args):
                    if not isinstance(arg, dict):
                        continue
                    node = parse_type_node(arg.get("type"))
                    if node.use_site.base_name == "void" and node.use_site.pointer_depth == 0 and not node.use_site.array_shape and not arg.get("name"):
                        continue
                    arg_name = str(arg.get("name") or f"arg{index}")
                    use = self._resolve_use(
                        api_name,
                        f"parameter:{index}:{arg_name}",
                        node.use_site,
                        is_special or is_special_property_only,
                        selected.is_constructor,
                        selected.item,
                    )
                    use.name = arg_name
                    use.docstring = str(arg.get("docstring") or "").strip()
                    uses.append(use)

            statuses = {use.status for use in uses}
            failures = [use for use in uses if use.status not in {ApiStatus.ACCEPTED_GENERIC, ApiStatus.ACCEPTED_SPECIAL}]
            if failures:
                priority = (
                    ApiStatus.REJECTED_UNRESOLVED,
                    ApiStatus.REJECTED_LIFECYCLE,
                    ApiStatus.REJECTED_UNSUPPORTED_TYPE,
                    ApiStatus.REJECTED_SPECIAL_REQUIRED,
                )
                status = next((candidate for candidate in priority if candidate in statuses), failures[0].status)
                primary_issue = next(use for use in failures if use.status == status)
                reason = primary_issue.reason
                reason_code = primary_issue.reason_code
            elif is_special or is_special_property_only:
                status = ApiStatus.ACCEPTED_SPECIAL
                reason = ""
                reason_code = ""
            else:
                status = ApiStatus.ACCEPTED_GENERIC
                reason = ""
                reason_code = ""

            if selected.is_constructor and not is_special:
                for use in uses[1:]:
                    if use.sni_type == "SNI_H_LV_OBJ" and use.use_site.pointer_depth == 1:
                        use.argument_mode = "parent_handle"
                        use.allow_null = False

            return_use = uses[0]
            parameters = uses[1:]
            result_owner = None
            if return_use.lifecycle_class == "sub_resource":
                if "method" in selected.selection_kinds:
                    result_owner = "this"
                elif "static_method" in selected.selection_kinds:
                    parent = next((use.name for use in parameters if use.sni_type == "SNI_H_LV_OBJ"), None)
                    result_owner = parent

            cleanup_parameters = []
            if api_name.startswith("lv_chart_remove_"):
                cleanup_parameters = [
                    use.name for use in parameters
                    if use.name and use.lifecycle_class == "sub_resource"
                ]

            output_string_config = override.get("output_string")
            output_string = None
            if isinstance(output_string_config, dict):
                output_string = (
                    str(output_string_config.get("buffer_arg", "")),
                    str(output_string_config.get("size_arg", "")),
                    str(output_string_config.get("js_buffer_arg", "placeholder")),
                )
            special_binding = selected.special_binding
            if is_special_property_only and special_binding is None:
                special_binding = next(
                    (
                        item["binding"]
                        for item in self.config.get("special_bindings", {}).get("properties", [])
                        if item.get("function") == api_name
                    ),
                    None,
                )
            issues = [
                {
                    "position": use.position,
                    "type": use.use_site.spelling,
                    "status": use.status.value,
                    "reason_code": use.reason_code,
                    "reason": use.reason,
                }
                for use in failures
            ]
            ir.apis.append(
                ApiRecord(
                    api_name,
                    status,
                    selected.class_name,
                    ",".join(selected.selection_kinds),
                    special_binding,
                    reason,
                    reason_code,
                    issues,
                    is_constructor=selected.is_constructor,
                    docstring=str(selected.item.get("docstring") or "").strip(),
                    return_docstring=str((selected.item.get("type") or {}).get("docstring") or "").strip()
                    if isinstance(selected.item.get("type"), dict)
                    else "",
                    native_call_name=str(override.get("call", api_name)),
                    output_string=output_string,
                    result_owner=result_owner,
                    cleanup_parameters=cleanup_parameters,
                )
            )
            ir.uses.extend(uses)

        ir.apis.extend(self.selection.pre_rejected)

        for binding, type_names in self.config.get("special_bindings", {}).get("type_dependencies", {}).items():
            for type_name in type_names:
                info = self.resolve_type(type_name)
                reference = f"special:{binding}"
                if reference not in info.referenced_by:
                    info.referenced_by.append(reference)

        missing_references = sorted(set(expected_type_names or ()) - set(self.types))
        if missing_references:
            raise ValueError(f"collected signature types were not resolved: {', '.join(missing_references)}")

        # Types named by config but not used in this selected surface remain diagnostics only.
        for configured_name in self.declarations:
            if configured_name not in self.types:
                self.resolve_type(configured_name)

        ir.types = self.types
        accepted = {
            api.name: api
            for api in ir.apis
            if api.status in {ApiStatus.ACCEPTED_GENERIC, ApiStatus.ACCEPTED_SPECIAL}
        }
        accepted_names = sorted(accepted)
        ir.api_surface = ResolvedApiSurface(
            apis=[accepted[name] for name in accepted_names],
            uses=[use for use in ir.uses if use.function in accepted],
            classes=self._resolved_classes(accepted),
            root_constants=list(self.selection.root_constants),
            macros=list(self.selection.macros),
            event_assertions=list(self.selection.event_assertions),
            names=accepted_names,
        )
        ir.selected_names = sorted(set(self.selection.candidate_names) | set(api_map))
        ir.accepted_names = sorted(api.name for api in ir.apis if api.status == ApiStatus.ACCEPTED_GENERIC)
        ir.special_names = sorted(api.name for api in ir.apis if api.status == ApiStatus.ACCEPTED_SPECIAL)
        ir.rejected_names = sorted(
            api.name
            for api in ir.apis
            if api.status not in {ApiStatus.ACCEPTED_GENERIC, ApiStatus.ACCEPTED_SPECIAL}
        )
        ir.diagnostics.extend(self.selection.messages)

        for info in sorted(ir.types.values(), key=lambda entry: entry.name):
            if info.name in self.declarations and not info.referenced_by:
                ir.diagnostics.append(
                    Diagnostic(
                        code="UNUSED_EXPLICIT_DECLARATION",
                        severity=Severity.WARNING,
                        category="configuration",
                        subject_kind="type",
                        subject=info.name,
                        type_name=info.name,
                        reason=f"Explicit declaration for {info.name} is unused by selected APIs.",
                        references=tuple(info.referenced_by),
                        suggested_action="Remove the declaration if it is no longer needed.",
                    )
                )
            if info.resolution_source == "explicit" and info.representation == Representation.VALUE_OBJECT:
                inferred = self._can_infer_value_object(info.name)
                if inferred:
                    ir.diagnostics.append(
                        Diagnostic(
                            code="REDUNDANT_EXPLICIT_DECLARATION",
                            severity=Severity.INFO,
                            category="configuration",
                            subject_kind="type",
                            subject=info.name,
                            type_name=info.name,
                            reason=f"{info.name} is a value object that current LVGL facts can safely infer.",
                            suggested_action="Remove the redundant declaration if it has no review note to preserve.",
                        )
                    )
            if info.representation == Representation.UNKNOWN:
                refs = sorted(set(info.referenced_by) | {use.function for use in ir.uses if use.use_site.base_name == info.name and use.status == ApiStatus.REJECTED_UNRESOLVED})
                is_unknown_todo = self.declarations.get(info.name, {}).get("kind") == "unknown"
                if refs or is_unknown_todo:
                    ir.diagnostics.append(
                        Diagnostic(
                            code="UNRESOLVED_TYPE",
                            severity=Severity.ERROR,
                            category="resolution",
                            status=ApiStatus.REJECTED_UNRESOLVED.value,
                            subject_kind="type",
                            subject=info.name,
                            type_name=info.name,
                            reason=f"{info.name} has no explicit binding resolution.",
                            details={"rejection_reason": info.rejection_reason},
                            references=tuple(refs),
                            suggested_action="Add a deliberate type declaration or special binding, then validate again.",
                        )
                    )

        return ir

    def _resolve_use(self, function: str, position: str, use: CUseSite, special: bool, is_constructor: bool, function_item: dict[str, Any]) -> ApiUse:
        info = self.resolve_type(use.base_name)
        refs = info.referenced_by
        if function not in refs:
            refs.append(function)

        if info.canonical_name == "void" and use.pointer_depth == 0 and not use.array_shape:
            result = ApiUse(function, position, use, conversion="void", status=ApiStatus.ACCEPTED_SPECIAL if special else ApiStatus.ACCEPTED_GENERIC, reason="void has no JavaScript value" if not special else "configured special wrapper owns this signature")
            if not special:
                result.js_type = "undefined"
                result.c2js_mode = "void"
            return result

        if special and info.representation == Representation.UNKNOWN:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_UNRESOLVED, reason=info.rejection_reason, reason_code="UNRESOLVED_TYPE")
        if special:
            if position == "return" and info.representation == Representation.MANAGED_RESOURCE:
                declaration = self.declarations.get(use.base_name) or self.declarations.get(info.canonical_name, {})
                creator = declaration.get("creator")
                if info.resource_category == "pure_managed" and not (is_constructor and function == creator):
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED_LIFECYCLE, reason=f"Pure Managed {info.canonical_name} may only enter through configured creator `{creator}`.", reason_code="PURE_MANAGED_ENTRY_VIOLATION")
                if info.resource_category == "tree_dependent" and function == creator and not self._has_object_parent(function_item):
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED_LIFECYCLE, reason=f"Tree-Dependent creator {function} has no lv_obj_t * parent argument.", reason_code="TREE_RESOURCE_PARENT_MISSING")
            return ApiUse(function, position, use, conversion="special_binding", status=ApiStatus.ACCEPTED_SPECIAL, reason="the configured C wrapper owns conversion and lifecycle handling")
        if info.representation == Representation.UNKNOWN:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_UNRESOLVED, reason=info.rejection_reason, reason_code="UNRESOLVED_TYPE")
        if use.array_shape:
            conversion = self.config.get("special_conversions", {}).get(use.spelling)
            if conversion:
                return self._accepted_use(function, position, use, info, conversion_id=conversion, conversion="explicit", is_constructor=is_constructor)
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_SPECIAL_REQUIRED, reason=f"array shape {list(use.array_shape)} requires a configured special binding", reason_code="SPECIAL_BINDING_REQUIRED")
        if use.is_function_pointer or info.category == CCategory.FUNCTION_POINTER:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_SPECIAL_REQUIRED, reason="callback requires a configured special binding", reason_code="SPECIAL_BINDING_REQUIRED")
        if use.pointer_depth > 1:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_SPECIAL_REQUIRED, reason="pointer depth greater than one requires a special binding", reason_code="SPECIAL_BINDING_REQUIRED")
        if info.representation in {Representation.REJECTED, Representation.SPECIAL_REQUIRED, Representation.UNKNOWN}:
            status = {
                Representation.REJECTED: ApiStatus.REJECTED_UNSUPPORTED_TYPE,
                Representation.SPECIAL_REQUIRED: ApiStatus.REJECTED_SPECIAL_REQUIRED,
                Representation.UNKNOWN: ApiStatus.REJECTED_UNRESOLVED,
            }[info.representation]
            reason_code = {
                ApiStatus.REJECTED_UNSUPPORTED_TYPE: "UNSUPPORTED_TYPE_LAYOUT",
                ApiStatus.REJECTED_SPECIAL_REQUIRED: "SPECIAL_BINDING_REQUIRED",
                ApiStatus.REJECTED_UNRESOLVED: "UNRESOLVED_TYPE",
            }[status]
            return ApiUse(function, position, use, status=status, reason=info.rejection_reason, reason_code=reason_code)

        canonical = info.canonical_name
        sni_type = None
        conversion = info.representation.value
        if info.representation in {Representation.PRIMITIVE, Representation.ENUM}:
            if use.pointer_depth:
                conversion_id = self.config.get("special_conversions", {}).get(use.spelling)
                if conversion_id:
                    return self._accepted_use(function, position, use, info, conversion_id=conversion_id, conversion="explicit", is_constructor=is_constructor)
                if canonical == "char" and use.pointer_depth == 1:
                    sni_type, conversion = "SNI_T_STRING", "string"
                else:
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED_SPECIAL_REQUIRED, reason="primitive pointer is an output/buffer form and requires a special binding", reason_code="SPECIAL_BINDING_REQUIRED")
            else:
                sni_type = "SNI_T_INT32" if info.representation == Representation.ENUM else PRIMITIVE_SNI.get(canonical)
        elif info.representation == Representation.VALUE_OBJECT:
            sni_type = _sni_name(canonical)
        elif info.representation == Representation.OBJECT_TREE_NODE:
            sni_type = "SNI_H_LV_OBJ"
        elif info.representation == Representation.MANAGED_RESOURCE:
            sni_type = self._handle_type(canonical)
            declaration = self.declarations.get(use.base_name) or self.declarations.get(canonical, {})
            creator = declaration.get("creator")
            if position == "return":
                if info.resource_category == "pure_managed" and not (is_constructor and function == creator):
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED_LIFECYCLE, reason=f"Pure Managed {canonical} may only enter through configured creator `{creator}`.", reason_code="PURE_MANAGED_ENTRY_VIOLATION")
                if info.resource_category == "tree_dependent":
                    has_parent = self._has_object_parent(function_item)
                    if not has_parent:
                        return ApiUse(function, position, use, status=ApiStatus.REJECTED_LIFECYCLE, reason=f"Tree-Dependent result {function} has no lv_obj_t * parent argument", reason_code="TREE_RESOURCE_PARENT_MISSING")
            elif position.startswith("parameter:") and info.resource_category == "pure_managed":
                # Passing an existing, live SNI-created instance is valid; it does not create a new one.
                pass
        else:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_UNSUPPORTED_TYPE, reason=f"no generic conversion for {info.representation.value}", reason_code="UNSUPPORTED_TYPE")

        if sni_type is None:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_UNRESOLVED, reason=f"no SNI primitive mapping for {canonical}", reason_code="NO_SNI_MAPPING")
        if use.pointer_depth and info.representation not in {Representation.VALUE_OBJECT, Representation.OBJECT_TREE_NODE, Representation.MANAGED_RESOURCE} and not (info.representation == Representation.PRIMITIVE and sni_type == "SNI_T_STRING"):
            return ApiUse(function, position, use, status=ApiStatus.REJECTED_SPECIAL_REQUIRED, reason="pointer form has no safe generic SNI representation", reason_code="SPECIAL_BINDING_REQUIRED")
        return self._accepted_use(function, position, use, info, conversion_id=sni_type, conversion=conversion, is_constructor=is_constructor)

    def _accepted_use(
        self,
        function: str,
        position: str,
        use: CUseSite,
        info: TypeInfo,
        conversion_id: str | None,
        conversion: str,
        is_constructor: bool,
    ) -> ApiUse:
        """Freeze the Core's conversion decision into the resolved API-use IR."""
        result = ApiUse(
            function,
            position,
            use,
            sni_type=conversion_id,
            conversion=conversion,
            status=ApiStatus.ACCEPTED_GENERIC,
            lifecycle_class={
                "pure_managed": "controlled_resource",
                "tree_dependent": "sub_resource",
                "hybrid": "hybrid",
            }.get(info.resource_category or "", "tree_node" if info.representation == Representation.OBJECT_TREE_NODE else ""),
        )
        if conversion_id is None:
            return result

        result.js_type = self._js_type_for_sni(conversion_id, position == "return")
        if conversion_id == "SNI_T_STRING":
            result.js_check = "jerry_value_is_string"
            result.js2c_mode = "string_fn"
            result.js2c_expr = "sni_tb_js2c_string"
            result.c2js_mode = "string"
            result.c2js_expr = "sni_tb_c2js_string"
        elif conversion_id == "SNI_T_BOOL":
            result.js_check = "jerry_value_is_boolean"
            result.js2c_mode = "macro"
            result.js2c_expr = "sni_tb_js2c_boolean"
            result.c2js_mode = "macro"
            result.c2js_expr = "sni_tb_c2js_boolean"
        elif conversion_id in {"SNI_T_DOUBLE", "SNI_T_FLOAT", "SNI_T_PTR"}:
            result.js_check = "jerry_value_is_number"
            result.js2c_mode = "bridge"
            result.c2js_mode = "bridge"
        elif conversion_id in {"SNI_T_INT8", "SNI_T_INT16", "SNI_T_INT32"}:
            result.js_check = "jerry_value_is_number"
            result.js2c_mode = "macro"
            result.js2c_expr = "sni_tb_js2c_int32"
            result.c2js_mode = "bridge"
        elif conversion_id in {"SNI_T_UINT8", "SNI_T_UINT16", "SNI_T_UINT32"}:
            result.js_check = "jerry_value_is_number"
            result.js2c_mode = "macro"
            result.js2c_expr = "sni_tb_js2c_uint32"
            result.c2js_mode = "bridge"
        else:
            result.js_check = "" if conversion_id == "SNI_V_LV_COLOR" else "jerry_value_is_object"
            result.js2c_mode = "bridge"
            result.c2js_mode = "bridge"

        if conversion_id.startswith("SNI_V_") and use.pointer_depth:
            result.argument_mode = "value_pointer_output" if "_get_" in function and not use.is_const else "value_pointer"
            result.copy_back = not use.is_const
            result.pointee_type = use.base_name
        if is_constructor and use.pointer_depth and (conversion_id.startswith("SNI_H_") or conversion_id == "SNI_T_PTR"):
            result.allow_null = conversion_id != "SNI_H_LV_OBJ"
        return result

    @staticmethod
    def _js_type_for_sni(sni_type: str, return_value: bool) -> str:
        if sni_type == "SNI_T_STRING":
            return "string"
        if sni_type == "SNI_T_BOOL":
            return "boolean"
        if sni_type == "SNI_T_PTR":
            return "number"
        if sni_type in {
            "SNI_T_UINT8", "SNI_T_INT8", "SNI_T_UINT16", "SNI_T_INT16",
            "SNI_T_UINT32", "SNI_T_INT32", "SNI_T_DOUBLE", "SNI_T_FLOAT",
        }:
            return "number"
        if sni_type == "SNI_V_LV_COLOR":
            return "object" if return_value else "number"
        if sni_type.startswith("SNI_H_") or sni_type.startswith("SNI_V_"):
            return "object"
        return "unknown"

    def _resolved_classes(self, accepted: dict[str, ApiRecord]) -> list[ApiClassIR]:
        """Copy the class surface selected by Core, retaining accepted members only."""
        result: list[ApiClassIR] = []
        for source in self.selection.render_classes:
            constructor = source.configured_constructor
            constructor_api = accepted.get(constructor or "")
            if constructor_api is None:
                constructor = None
            constructor_binding = constructor_api.special_binding if constructor_api is not None else None
            result.append(
                ApiClassIR(
                    name=source.name,
                    c_type=source.c_type,
                    configured_constructor=constructor,
                    has_constructor=source.has_constructor,
                    constructor_binding=constructor_binding if constructor else None,
                    base=source.base,
                    methods=[ref for ref in source.methods if ref.function in accepted],
                    static_methods=[ref for ref in source.static_methods if ref.function in accepted],
                    properties=[
                        type(prop)(
                            prop.name,
                            prop.getter if prop.getter and prop.getter.function in accepted else None,
                            prop.setter if prop.setter and prop.setter.function in accepted else None,
                        )
                        for prop in source.properties
                        if (prop.getter and prop.getter.function in accepted)
                        or (prop.setter and prop.setter.function in accepted)
                    ],
                    constants=list(source.constants),
                    extra_methods=list(source.extra_methods),
                    extra_properties=list(source.extra_properties),
                )
            )
        return result

    def _has_object_parent(self, function_item: dict[str, Any]) -> bool:
        args = function_item.get("args", [])
        return any(
            (lambda use: use.base_name == "lv_obj_t" and use.pointer_depth == 1)(parse_type_node(arg.get("type")).use_site)
            for arg in args if isinstance(arg, dict)
        )

    def _handle_type(self, canonical: str) -> str:
        if canonical == "lv_obj_t":
            return "SNI_H_LV_OBJ"
        base = canonical[:-2] if canonical.endswith("_t") else canonical
        return "SNI_H_" + base.upper()

    def _can_infer_value_object(self, name: str) -> bool:
        if name not in self.declarations:
            return False
        probe_config = dict(self.config)
        probe_config["type_declarations"] = dict(self.declarations)
        probe_config["type_declarations"].pop(name, None)
        probe = TypeResolver(self.model, probe_config, self.selection)
        return probe.resolve_type(name).representation == Representation.VALUE_OBJECT
