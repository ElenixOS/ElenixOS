"""Conservative C type resolution and API/use-site admission."""

from __future__ import annotations

from typing import Any

from .ir import ApiRecord, ApiStatus, ApiUse, BindingIR, CCategory, CUseSite, Representation, TypeInfo
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
                    reasons.append(f"{field.unsupported_reason} member {field.name}")
                    continue
                child = self.resolve_type(field.type_name)
                info.dependencies.append(child.name)
                if field.pointer_depth:
                    eligible = False
                    reasons.append(f"pointer member {field.name}")
                elif field.is_function_pointer:
                    eligible = False
                    reasons.append(f"callback member {field.name}")
                elif field.array_shape:
                    eligible = False
                    reasons.append(f"array member {field.name}")
                elif field.bitsize is not None:
                    eligible = False
                    reasons.append(f"bitfield {field.name} needs explicit layout review")
                elif child.representation not in {Representation.PRIMITIVE, Representation.ENUM, Representation.VALUE_OBJECT}:
                    eligible = False
                    reasons.append(f"field {field.name} uses {child.representation.value} type {field.type_name}")
            self._resolving.remove(name)
            if eligible:
                info.representation = Representation.VALUE_OBJECT
                info.resolution_source = "inferred"
                info.inference_reason = "all fields recursively resolve to primitive, enum, or confirmed value-object types"
            else:
                info.representation = Representation.REJECTED
                info.rejection_reason = "; ".join(reasons) or "empty or unsupported struct layout"
            return info
        if category == CCategory.TYPEDEF:
            # resolve_declaration() normally removes typedef wrappers; reaching this branch means the alias target is unknown.
            info.representation = Representation.UNKNOWN
            info.rejection_reason = f"typedef {name} does not resolve to a known canonical type"
            return info
        info.representation = Representation.UNKNOWN
        info.rejection_reason = f"no resolver rule for {category.value} type {name}"
        return info

    def resolve_all(self) -> BindingIR:
        ir = BindingIR()
        api_map = self.selection.functions
        properties = self.config.get("special_bindings", {}).get("properties", [])
        special_property_functions = {item["function"] for item in properties}
        function_overrides = self.config.get("function_overrides", {})

        for api_name in sorted(api_map):
            selected = api_map[api_name]
            is_special = selected.special_binding is not None
            is_special_property_only = (
                api_name in special_property_functions
                and set(selected.selection_kinds) <= {"property_getter", "property_setter"}
            )
            uses: list[ApiUse] = []
            raw_return = selected.item.get("type")
            return_node = parse_type_node(raw_return)
            uses.append(self._resolve_use(api_name, "return", return_node.use_site, is_special or is_special_property_only, selected.is_constructor, selected.item))
            args = selected.item.get("args", [])
            if isinstance(args, list):
                for index, arg in enumerate(args):
                    if not isinstance(arg, dict):
                        continue
                    node = parse_type_node(arg.get("type"))
                    if node.use_site.base_name == "void" and node.use_site.pointer_depth == 0 and not node.use_site.array_shape and not arg.get("name"):
                        continue
                    uses.append(self._resolve_use(api_name, f"parameter:{index}:{arg.get('name') or f'arg{index}'}", node.use_site, is_special or is_special_property_only, selected.is_constructor, selected.item))

            reason = next((use.reason for use in uses if use.status in {ApiStatus.REJECTED, ApiStatus.CANDIDATE} and use.reason), "")
            statuses = {use.status for use in uses}
            if ApiStatus.CANDIDATE in statuses:
                status = ApiStatus.CANDIDATE
            elif ApiStatus.REJECTED in statuses:
                status = ApiStatus.REJECTED
            elif is_special or is_special_property_only:
                status = ApiStatus.SPECIAL
            else:
                status = ApiStatus.ACCEPTED
            ir.apis.append(ApiRecord(api_name, status, selected.class_name, ",".join(selected.selection_kinds), selected.special_binding, reason))
            ir.uses.extend(uses)

        ir.apis.extend(self.selection.pre_rejected)

        for binding, type_names in self.config.get("special_bindings", {}).get("type_dependencies", {}).items():
            for type_name in type_names:
                info = self.resolve_type(type_name)
                reference = f"special:{binding}"
                if reference not in info.referenced_by:
                    info.referenced_by.append(reference)

        # Types named by config but not used in this selected surface remain diagnostics only.
        for configured_name in self.declarations:
            if configured_name not in self.types:
                self.resolve_type(configured_name)

        ir.types = self.types
        ir.selected_names = sorted(set(self.selection.candidate_names) | set(api_map))
        ir.accepted_names = sorted(api.name for api in ir.apis if api.status == ApiStatus.ACCEPTED)
        ir.special_names = sorted(api.name for api in ir.apis if api.status == ApiStatus.SPECIAL)
        ir.rejected_names = sorted(api.name for api in ir.apis if api.status == ApiStatus.REJECTED)
        ir.diagnostics.extend(self.selection.messages)

        for info in sorted(ir.types.values(), key=lambda entry: entry.name):
            if info.name in self.declarations and not info.referenced_by:
                ir.diagnostics.append({"severity": "warning", "code": "UNUSED_EXPLICIT_DECLARATION", "message": f"explicit declaration for {info.name} is unused by selected APIs"})
            if info.resolution_source == "explicit" and info.representation == Representation.VALUE_OBJECT:
                inferred = self._can_infer_value_object(info.name)
                if inferred:
                    ir.diagnostics.append({"severity": "info", "code": "REDUNDANT_EXPLICIT_DECLARATION", "message": f"{info.name} is a value object that current LVGL facts can safely infer"})
            if info.representation == Representation.UNKNOWN:
                refs = sorted(set(info.referenced_by) | {use.function for use in ir.uses if use.use_site.base_name == info.name and use.status == ApiStatus.CANDIDATE})
                is_unknown_todo = self.declarations.get(info.name, {}).get("kind") == "unknown"
                if refs or is_unknown_todo:
                    ir.diagnostics.append({"severity": "error", "code": "UNRESOLVED_TYPE", "message": f"{info.name} is unresolved; referenced by {', '.join(refs)}"})
            elif info.representation == Representation.REJECTED:
                ir.diagnostics.append({"severity": "warning", "code": "UNREPRESENTABLE_TYPE", "message": f"{info.name}: {info.rejection_reason}"})

        for api in ir.apis:
            if api.status == ApiStatus.REJECTED:
                ir.diagnostics.append({"severity": "warning", "code": "API_REJECTED", "message": f"{api.name}: {api.reason}"})

        return ir

    def _resolve_use(self, function: str, position: str, use: CUseSite, special: bool, is_constructor: bool, function_item: dict[str, Any]) -> ApiUse:
        info = self.resolve_type(use.base_name)
        refs = info.referenced_by
        if function not in refs:
            refs.append(function)

        if info.canonical_name == "void" and use.pointer_depth == 0 and not use.array_shape:
            return ApiUse(function, position, use, conversion="void", status=ApiStatus.SPECIAL if special else ApiStatus.ACCEPTED, reason="void has no JavaScript value" if not special else "configured special wrapper owns this signature")

        if special and info.representation == Representation.UNKNOWN:
            return ApiUse(function, position, use, status=ApiStatus.CANDIDATE, reason=info.rejection_reason)
        if special:
            if position == "return" and info.representation == Representation.MANAGED_RESOURCE:
                declaration = self.declarations.get(use.base_name) or self.declarations.get(info.canonical_name, {})
                creator = declaration.get("creator")
                if info.resource_category == "pure_managed" and not (is_constructor and function == creator):
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=f"Pure Managed {info.canonical_name} may only enter through configured creator {creator}")
                if info.resource_category == "tree_dependent" and function == creator and not self._has_object_parent(function_item):
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=f"Tree-Dependent creator {function} has no lv_obj_t * parent argument")
            return ApiUse(function, position, use, conversion="special_binding", status=ApiStatus.SPECIAL, reason="the configured C wrapper owns conversion and lifecycle handling")
        if info.representation == Representation.UNKNOWN:
            return ApiUse(function, position, use, status=ApiStatus.CANDIDATE, reason=info.rejection_reason)
        if use.array_shape:
            conversion = self.config.get("special_conversions", {}).get(use.spelling)
            if conversion:
                return ApiUse(function, position, use, conversion=conversion, status=ApiStatus.ACCEPTED, reason="explicit use-site conversion")
            return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=f"array shape {list(use.array_shape)} requires a configured special binding")
        if use.is_function_pointer or info.category == CCategory.FUNCTION_POINTER:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason="callback requires a configured special binding")
        if use.pointer_depth > 1:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason="pointer depth greater than one requires a special binding")
        if info.representation in {Representation.REJECTED, Representation.SPECIAL_REQUIRED, Representation.UNKNOWN}:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=info.rejection_reason)

        canonical = info.canonical_name
        sni_type = None
        conversion = info.representation.value
        if info.representation in {Representation.PRIMITIVE, Representation.ENUM}:
            if use.pointer_depth:
                conversion_id = self.config.get("special_conversions", {}).get(use.spelling)
                if conversion_id:
                    return ApiUse(function, position, use, sni_type=conversion_id, conversion="explicit", status=ApiStatus.ACCEPTED, reason="explicit use-site conversion")
                if canonical == "char" and use.pointer_depth == 1:
                    sni_type, conversion = "SNI_T_STRING", "string"
                else:
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason="primitive pointer is an output/buffer form and requires a special binding")
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
                    return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=f"Pure Managed {canonical} may only enter through configured creator {creator}")
                if info.resource_category == "tree_dependent":
                    has_parent = self._has_object_parent(function_item)
                    if not has_parent:
                        return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=f"Tree-Dependent result {function} has no lv_obj_t * parent argument")
            elif position.startswith("parameter:") and info.resource_category == "pure_managed":
                # Passing an existing, live SNI-created instance is valid; it does not create a new one.
                pass
        else:
            return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason=f"no generic conversion for {info.representation.value}")

        if sni_type is None:
            return ApiUse(function, position, use, status=ApiStatus.CANDIDATE, reason=f"no SNI primitive mapping for {canonical}")
        if use.pointer_depth and info.representation not in {Representation.VALUE_OBJECT, Representation.OBJECT_TREE_NODE, Representation.MANAGED_RESOURCE} and not (info.representation == Representation.PRIMITIVE and sni_type == "SNI_T_STRING"):
            return ApiUse(function, position, use, status=ApiStatus.REJECTED, reason="pointer form has no safe generic SNI representation")
        return ApiUse(function, position, use, sni_type=sni_type, conversion=conversion, status=ApiStatus.ACCEPTED)

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


def legacy_type_maps(ir: BindingIR, model: LVGLModel, config: dict[str, Any]) -> tuple[dict[str, str], dict[str, str], dict[str, str]]:
    """Adapt the typed resolver result to the mature wrapper emitter's narrow inputs."""
    entries: dict[str, str] = {}
    lifecycle: dict[str, str] = {}
    sni_ids: dict[str, str] = {}
    for info in ir.types.values():
        name = info.name
        canonical = info.canonical_name
        if info.representation in {Representation.PRIMITIVE, Representation.ENUM}:
            target = PRIMITIVE_SNI.get(canonical, "SNI_T_INT32")
            entries[name] = {
                "SNI_T_BOOL": "bool", "SNI_T_INT8": "int8", "SNI_T_UINT8": "uint8",
                "SNI_T_INT16": "int16", "SNI_T_UINT16": "uint16", "SNI_T_INT32": "int",
                "SNI_T_UINT32": "uint", "SNI_T_FLOAT": "float", "SNI_T_DOUBLE": "double",
            }.get(target, "")
            sni_ids[name] = target
        elif info.representation == Representation.VALUE_OBJECT:
            entries[name] = "value_object"
            sni_ids[name] = _sni_name(canonical)
        elif info.representation == Representation.OBJECT_TREE_NODE:
            entries[name] = "handle_object"
            sni_ids[name] = "SNI_H_LV_OBJ"
            lifecycle[name] = "tree_node"
        elif info.representation == Representation.MANAGED_RESOURCE:
            entries[name] = "handle_object"
            sni_ids[name] = "SNI_H_" + (canonical[:-2] if canonical.endswith("_t") else canonical).upper()
            lifecycle[name] = {"pure_managed": "controlled_resource", "tree_dependent": "sub_resource", "hybrid": "hybrid"}.get(info.resource_category, "")
        else:
            entries[name] = ""
        entries.setdefault(canonical, entries[name])
        if name not in sni_ids and canonical in sni_ids:
            sni_ids[name] = sni_ids[canonical]
        if name not in lifecycle and canonical in lifecycle:
            lifecycle[name] = lifecycle[canonical]

    for use in ir.uses:
        base = use.use_site.base_name
        info = ir.types.get(base)
        if not info:
            continue
        entries.setdefault(base, "value_object" if info.representation == Representation.VALUE_OBJECT else "handle_object" if info.representation in {Representation.OBJECT_TREE_NODE, Representation.MANAGED_RESOURCE} else "")
        sni_id = use.sni_type or sni_ids.get(base)
        if sni_id:
            # Existing emitter uses the pointee spelling after stripping pointer stars and const.
            entries[base] = entries.get(base, "")
            sni_ids[base] = sni_id
            sni_ids["const " + base] = sni_id
            entries["const " + base] = entries[base]

    return entries, lifecycle, sni_ids
