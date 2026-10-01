"""Cross-check selected bindings, type admission, and explicit configuration."""

from __future__ import annotations

from typing import Any

from .ir import ApiStatus, BindingIR
from .lvgl_model import LVGLModel, parse_type_node
from .selection import SelectionResult


def validate_bindings(
    config: dict[str, Any],
    model: LVGLModel,
    selection: SelectionResult,
    ir: BindingIR,
    special_ids: set[str],
) -> list[dict[str, str]]:
    diagnostics: list[dict[str, str]] = []
    api_names = set(selection.functions)
    special = config.get("special_bindings", {})
    for group in ("apis", "constructors"):
        for api_name, binding in special.get(group, {}).items():
            if api_name not in model.functions:
                diagnostics.append({"severity": "error", "code": "UNKNOWN_SPECIAL_API", "message": f"special_bindings.{group} references missing LVGL function {api_name}"})
            elif api_name not in api_names:
                diagnostics.append({"severity": "warning", "code": "UNUSED_SPECIAL_BINDING", "message": f"special binding {api_name} -> {binding} is not used by selected APIs"})
            if binding not in special_ids:
                diagnostics.append({"severity": "error", "code": "MISSING_SPECIAL_BINDING", "message": f"special binding {binding} for {api_name} is not implemented in SNI C sources"})

    for item in special.get("properties", []):
        if item["function"] not in model.functions:
            diagnostics.append({"severity": "error", "code": "UNKNOWN_SPECIAL_PROPERTY_API", "message": f"special property mapping references missing function {item['function']}"})
        elif item["function"] not in api_names:
            diagnostics.append({"severity": "warning", "code": "UNUSED_SPECIAL_PROPERTY", "message": f"special property mapping for {item['class']}.{item['property']} is unused"})
        if item["binding"] not in special_ids:
            diagnostics.append({"severity": "error", "code": "MISSING_SPECIAL_BINDING", "message": f"special property binding {item['binding']} is not implemented"})

    class_extensions = special.get("class_extensions", {})
    for kind in ("methods", "properties"):
        for class_name, items in class_extensions.get(kind, {}).items():
            if class_name not in config["api_selection"]["classes"]:
                diagnostics.append({"severity": "error", "code": "UNKNOWN_EXTENSION_CLASS", "message": f"special class extension references missing class {class_name}"})
            for item in items:
                for binding in (item.get("binding"), item.get("getter"), item.get("setter")):
                    if binding and binding not in special_ids:
                        diagnostics.append({"severity": "error", "code": "MISSING_SPECIAL_BINDING", "message": f"class extension references unimplemented wrapper {binding}"})

    for type_name, declaration in config.get("type_declarations", {}).items():
        if declaration.get("kind") == "managed_resource":
            creator = declaration["creator"]
            function = model.functions.get(creator)
            if function is None:
                diagnostics.append({"severity": "error", "code": "UNKNOWN_RESOURCE_CREATOR", "message": f"managed resource {type_name} references missing creator {creator}"})
                continue
            if creator not in api_names:
                diagnostics.append({"severity": "warning", "code": "UNUSED_RESOURCE_CREATOR", "message": f"managed resource creator {creator} is not selected"})
            if declaration["category"] == "pure_managed":
                selected = selection.functions.get(creator)
                if selected is not None and not selected.is_constructor:
                    diagnostics.append({"severity": "error", "code": "INVALID_PURE_MANAGED_CREATOR", "message": f"Pure Managed creator {creator} must be configured as an SNI constructor"})
            elif declaration["category"] == "tree_dependent":
                args = function.get("args", [])
                has_parent = any(
                    (lambda use: use.base_name == "lv_obj_t" and use.pointer_depth == 1)(parse_type_node(arg.get("type")).use_site)
                    for arg in args if isinstance(arg, dict)
                )
                if not has_parent and creator in api_names and not selection.functions[creator].special_binding:
                    diagnostics.append({"severity": "error", "code": "TREE_RESOURCE_WITHOUT_PARENT", "message": f"Tree-Dependent creator {creator} needs an lv_obj_t * parent or a configured special binding"})

    for api in ir.apis:
        if api.status == ApiStatus.CANDIDATE:
            diagnostics.append({"severity": "error", "code": "UNRESOLVED_API", "message": f"{api.name} remains a candidate because one or more types are unresolved"})

    diagnostics.extend(ir.diagnostics)
    # Keep diagnostics deterministic and remove duplicate checks emitted by both stages.
    unique = {(item.get("severity", ""), item.get("code", ""), item.get("message", "")): item for item in diagnostics}
    ir.diagnostics = sorted(unique.values(), key=lambda item: (item.get("severity", ""), item.get("code", ""), item.get("message", "")))
    return ir.diagnostics


def has_errors(diagnostics: list[dict[str, Any]]) -> bool:
    return any(item.get("severity") == "error" for item in diagnostics)
