"""Cross-check selected bindings, type admission, and explicit configuration."""

from __future__ import annotations

from typing import Any

from .ir import ApiStatus, BindingIR, Diagnostic, Severity
from .lvgl_model import LVGLModel, parse_type_node
from .selection import SelectionResult


def validate_bindings(
    config: dict[str, Any],
    model: LVGLModel,
    selection: SelectionResult,
    ir: BindingIR,
    special_ids: set[str],
) -> list[Diagnostic]:
    diagnostics: list[Diagnostic] = []

    def add(
        code: str,
        severity: Severity,
        reason: str,
        *,
        subject_kind: str | None = None,
        subject: str | None = None,
        api: str | None = None,
        type_name: str | None = None,
        details: dict[str, Any] | None = None,
        suggested_action: str | None = None,
    ) -> None:
        diagnostics.append(
            Diagnostic(
                code=code,
                severity=severity,
                category="validation",
                subject_kind=subject_kind,
                subject=subject,
                api=api,
                type_name=type_name,
                reason=reason,
                details=details or {},
                suggested_action=suggested_action,
            )
        )

    api_names = set(selection.functions)
    special = config.get("special_bindings", {})
    for function_name, override in config.get("function_overrides", {}).items():
        if override.get("string_ownership") == "copy" and override.get("call", function_name) == function_name:
            add(
                "INVALID_STRING_OWNERSHIP_OVERRIDE",
                Severity.ERROR,
                f"Copying string override for {function_name} must name a different native call.",
                subject_kind="api",
                subject=function_name,
                api=function_name,
                suggested_action="Set function_overrides.call to the native function that copies the string.",
            )

        output_string = override.get("output_string")
        selected = selection.functions.get(function_name)
        if output_string is None or selected is None:
            continue
        raw_args = selected.item.get("args", [])
        args_by_name = {
            str(arg.get("name") or f"arg{index}"): (index, parse_type_node(arg.get("type")).use_site)
            for index, arg in enumerate(raw_args)
            if isinstance(arg, dict)
        }
        buffer = args_by_name.get(output_string["buffer_arg"])
        size = args_by_name.get(output_string["size_arg"])
        return_use = parse_type_node(selected.item.get("type")).use_site
        valid_buffer = bool(
            buffer
            and buffer[0] > 0
            and buffer[1].base_name == "char"
            and buffer[1].pointer_depth == 1
            and not buffer[1].is_const
            and not buffer[1].array_shape
        )
        valid_size = bool(
            size
            and size[0] > 0
            and size[1].base_name in {"uint32_t", "size_t"}
            and size[1].pointer_depth == 0
            and not size[1].array_shape
        )
        valid_method = "method" in selected.selection_kinds and "static_method" not in selected.selection_kinds
        valid_return = return_use.base_name == "void" and return_use.pointer_depth == 0 and not return_use.array_shape
        if not (valid_buffer and valid_size and valid_method and valid_return):
            add(
                "INVALID_OUTPUT_STRING_OVERRIDE",
                Severity.ERROR,
                f"output_string override for {function_name} must target a mutable char * buffer and uint32_t/size_t size on an instance method returning void.",
                subject_kind="api",
                subject=function_name,
                api=function_name,
                details={"buffer_valid": valid_buffer, "size_valid": valid_size, "instance_method": valid_method, "void_return": valid_return},
                suggested_action="Correct the output_string arguments or remove the override.",
            )

    for group in ("apis", "constructors"):
        for api_name, binding in special.get(group, {}).items():
            if api_name not in model.functions:
                add(
                    "UNKNOWN_SPECIAL_API",
                    Severity.ERROR,
                    f"special_bindings.{group} references missing LVGL function {api_name}",
                    subject_kind="api",
                    subject=api_name,
                    api=api_name,
                    suggested_action="Correct or remove the mapping in the binding configuration.",
                )
            elif api_name not in api_names:
                add(
                    "UNUSED_SPECIAL_BINDING",
                    Severity.WARNING,
                    f"Special binding {api_name} -> {binding} is not used by selected APIs.",
                    subject_kind="api",
                    subject=api_name,
                    api=api_name,
                    details={"binding": binding},
                    suggested_action="Remove the mapping or update the API selector.",
                )
            if binding not in special_ids:
                add(
                    "MISSING_SPECIAL_BINDING",
                    Severity.ERROR,
                    f"Special binding {binding} for {api_name} is not implemented in SNI C sources.",
                    subject_kind="api",
                    subject=api_name,
                    api=api_name,
                    details={"binding": binding},
                    suggested_action="Implement the wrapper or correct the configured binding ID.",
                )

    for item in special.get("properties", []):
        function = item["function"]
        if function not in model.functions:
            add(
                "UNKNOWN_SPECIAL_PROPERTY_API",
                Severity.ERROR,
                f"Special property mapping references missing function {function}.",
                subject_kind="api",
                subject=function,
                api=function,
                suggested_action="Correct or remove the property mapping.",
            )
        elif function not in api_names:
            add(
                "UNUSED_SPECIAL_PROPERTY",
                Severity.WARNING,
                f"Special property mapping for {item['class']}.{item['property']} is unused.",
                subject_kind="api",
                subject=function,
                api=function,
                details={"class": item["class"], "property": item["property"]},
                suggested_action="Remove the mapping or update the class property selectors.",
            )
        if item["binding"] not in special_ids:
            add(
                "MISSING_SPECIAL_BINDING",
                Severity.ERROR,
                f"Special property binding {item['binding']} is not implemented.",
                subject_kind="api",
                subject=function,
                api=function,
                details={"binding": item["binding"]},
                suggested_action="Implement the wrapper or correct the configured binding ID.",
            )

    class_extensions = special.get("class_extensions", {})
    for kind in ("methods", "properties"):
        for class_name, items in class_extensions.get(kind, {}).items():
            if class_name not in config["api_selection"]["classes"]:
                add(
                    "UNKNOWN_EXTENSION_CLASS",
                    Severity.ERROR,
                    f"Special class extension references missing class {class_name}.",
                    subject_kind="class",
                    subject=class_name,
                    suggested_action="Add the class to api_selection or remove the extension.",
                )
            for item in items:
                for binding in (item.get("binding"), item.get("getter"), item.get("setter")):
                    if binding and binding not in special_ids:
                        add(
                            "MISSING_SPECIAL_BINDING",
                            Severity.ERROR,
                            f"Class extension references unimplemented wrapper {binding}.",
                            subject_kind="class",
                            subject=class_name,
                            details={"binding": binding, "extension_kind": kind},
                            suggested_action="Implement the wrapper or correct the extension configuration.",
                        )

    for type_name, declaration in config.get("type_declarations", {}).items():
        if declaration.get("kind") != "managed_resource":
            continue
        creator = declaration["creator"]
        function = model.functions.get(creator)
        if function is None:
            add(
                "UNKNOWN_RESOURCE_CREATOR",
                Severity.ERROR,
                f"Managed resource {type_name} references missing creator {creator}.",
                subject_kind="type",
                subject=type_name,
                type_name=type_name,
                details={"creator": creator},
                suggested_action="Correct the creator name or remove the resource declaration.",
            )
            continue
        if creator not in api_names:
            add(
                "UNUSED_RESOURCE_CREATOR",
                Severity.WARNING,
                f"Managed resource creator {creator} is not selected.",
                subject_kind="api",
                subject=creator,
                api=creator,
                type_name=type_name,
                suggested_action="Update the API selectors or remove the unused resource declaration.",
            )
        if declaration["category"] == "pure_managed":
            selected = selection.functions.get(creator)
            if selected is not None and not selected.is_constructor:
                add(
                    "INVALID_PURE_MANAGED_CREATOR",
                    Severity.ERROR,
                    f"Pure Managed creator {creator} must be configured as an SNI constructor.",
                    subject_kind="api",
                    subject=creator,
                    api=creator,
                    type_name=type_name,
                    suggested_action="Configure the creator as the corresponding class constructor.",
                )
        elif declaration["category"] == "tree_dependent":
            args = function.get("args", [])
            has_parent = any(
                (lambda use: use.base_name == "lv_obj_t" and use.pointer_depth == 1)(parse_type_node(arg.get("type")).use_site)
                for arg in args
                if isinstance(arg, dict)
            )
            if not has_parent and creator in api_names and not selection.functions[creator].special_binding:
                add(
                    "TREE_RESOURCE_WITHOUT_PARENT",
                    Severity.ERROR,
                    f"Tree-Dependent creator {creator} needs an lv_obj_t * parent or a configured special binding.",
                    subject_kind="api",
                    subject=creator,
                    api=creator,
                    type_name=type_name,
                    suggested_action="Add the object parent argument or provide a deliberate special binding.",
                )

    # Type-level unresolved diagnostics already carry every referencing API. Add an API issue
    # only when the resolver cannot point to a missing type (for example, a missing SNI mapping).
    for api in ir.apis:
        if api.status != ApiStatus.REJECTED_UNRESOLVED:
            continue
        missing_mapping = next((issue for issue in api.issues if issue["reason_code"] == "NO_SNI_MAPPING"), None)
        if not missing_mapping:
            continue
        add(
            "NO_SNI_MAPPING",
            Severity.ERROR,
            api.reason,
            subject_kind="api",
            subject=api.name,
            api=api.name,
            suggested_action="Add a safe SNI conversion or a deliberate special binding.",
        )

    diagnostics.extend(ir.diagnostics)
    unique: dict[tuple[str, str, str | None, str | None, str | None, str | None, str], Diagnostic] = {}
    for item in diagnostics:
        key = (
            item.severity.value,
            item.code,
            item.subject,
            item.api,
            item.type_name,
            item.parameter_position,
            item.reason,
        )
        unique[key] = item
    ir.diagnostics = sorted(unique.values(), key=lambda item: (item.severity.value, item.code, item.subject or "", item.api or "", item.type_name or ""))
    return ir.diagnostics


def has_errors(diagnostics: list[Diagnostic]) -> bool:
    return any(item.severity == Severity.ERROR for item in diagnostics)
