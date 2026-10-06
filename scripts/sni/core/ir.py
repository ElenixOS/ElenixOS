"""Typed, transient intermediate representation for LVGL to SNI bindings."""

from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum
from typing import Any, Optional


class CCategory(str, Enum):
    PRIMITIVE = "primitive"
    ENUM = "enum"
    TYPEDEF = "typedef"
    STRUCT = "struct"
    UNION = "union"
    FUNCTION_POINTER = "function_pointer"
    UNKNOWN = "unknown"


class Representation(str, Enum):
    PRIMITIVE = "primitive"
    ENUM = "enum"
    STRING = "string"
    VALUE_OBJECT = "value_object"
    OBJECT_TREE_NODE = "object_tree_node"
    MANAGED_RESOURCE = "managed_resource"
    UNKNOWN = "unknown"
    SPECIAL_REQUIRED = "special_required"
    REJECTED = "rejected"


class Severity(str, Enum):
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"


class ApiStatus(str, Enum):
    ACCEPTED_GENERIC = "ACCEPTED_GENERIC"
    ACCEPTED_SPECIAL = "ACCEPTED_SPECIAL"
    EXCLUDED_BLACKLIST = "EXCLUDED_BLACKLIST"
    REJECTED_UNSUPPORTED_TYPE = "REJECTED_UNSUPPORTED_TYPE"
    REJECTED_SPECIAL_REQUIRED = "REJECTED_SPECIAL_REQUIRED"
    REJECTED_LIFECYCLE = "REJECTED_LIFECYCLE"
    REJECTED_UNRESOLVED = "REJECTED_UNRESOLVED"


def type_status(representation: Representation) -> str:
    if representation == Representation.UNKNOWN:
        return ApiStatus.REJECTED_UNRESOLVED.value
    if representation == Representation.REJECTED:
        return ApiStatus.REJECTED_UNSUPPORTED_TYPE.value
    if representation == Representation.SPECIAL_REQUIRED:
        return ApiStatus.REJECTED_SPECIAL_REQUIRED.value
    return "RESOLVED"


@dataclass(frozen=True)
class Diagnostic:
    """One structured issue, independent of terminal or machine presentation."""

    code: str
    severity: Severity
    category: str
    reason: str
    status: str | None = None
    subject_kind: str | None = None
    subject: str | None = None
    api: str | None = None
    type_name: str | None = None
    parameter_position: str | None = None
    details: dict[str, Any] = field(default_factory=dict)
    references: tuple[str, ...] = ()
    suggested_action: str | None = None

    def to_dict(self) -> dict[str, Any]:
        return {
            "code": self.code,
            "category": self.category,
            "status": self.status,
            "severity": self.severity.value,
            "subject_kind": self.subject_kind,
            "subject": self.subject,
            "api": self.api,
            "type": self.type_name,
            "parameter_position": self.parameter_position,
            "reason_code": self.code,
            "reason": self.reason,
            "details": self.details,
            "references": list(self.references),
            "suggested_action": self.suggested_action,
        }


@dataclass(frozen=True)
class FieldInfo:
    name: str
    type_name: str
    bitsize: Optional[int] = None
    array_shape: tuple[Optional[int], ...] = ()
    pointer_depth: int = 0
    is_function_pointer: bool = False
    unsupported_reason: str = ""


@dataclass
class TypeInfo:
    name: str
    canonical_name: str
    category: CCategory
    typedef_chain: list[str] = field(default_factory=list)
    dependencies: list[str] = field(default_factory=list)
    fields: list[FieldInfo] = field(default_factory=list)
    representation: Representation = Representation.UNKNOWN
    resolution_source: str = "unresolved"
    inference_reason: str = ""
    rejection_reason: str = ""
    referenced_by: list[str] = field(default_factory=list)
    resource_category: Optional[str] = None


@dataclass(frozen=True)
class CUseSite:
    spelling: str
    base_name: str
    pointer_depth: int = 0
    is_const: bool = False
    array_shape: tuple[Optional[int], ...] = ()
    is_function_pointer: bool = False


@dataclass
class ApiUse:
    function: str
    position: str
    use_site: CUseSite
    sni_type: Optional[str] = None
    conversion: str = "unresolved"
    status: ApiStatus = ApiStatus.REJECTED_UNRESOLVED
    reason: str = ""
    reason_code: str = ""
    name: str | None = None
    docstring: str = ""
    js_type: str = "unknown"
    js_check: str = ""
    js2c_mode: str = "none"
    js2c_expr: str | None = None
    c2js_mode: str = "none"
    c2js_expr: str | None = None
    argument_mode: str = "value"
    allow_null: bool = False
    copy_back: bool = False
    pointee_type: str | None = None
    lifecycle_class: str = ""


@dataclass
class ApiRecord:
    name: str
    status: ApiStatus
    class_name: Optional[str] = None
    selection_kind: str = ""
    special_binding: Optional[str] = None
    reason: str = ""
    reason_code: str = ""
    issues: list[dict[str, str]] = field(default_factory=list)
    is_constructor: bool = False
    docstring: str = ""
    return_docstring: str = ""
    native_call_name: str = ""
    output_string: tuple[str, str, str] | None = None
    result_owner: str | None = None
    cleanup_parameters: list[str] = field(default_factory=list)


@dataclass(frozen=True)
class ReceiverContract:
    owner_class: str
    binding_kind: str
    receiver_index: int
    expected_receiver_type: str
    actual_receiver_type: str
    receiver_validated: bool


@dataclass(frozen=True)
class ApiExportRef:
    function: str
    special_binding: str | None = None
    receiver: ReceiverContract | None = None


@dataclass(frozen=True)
class ApiPropertyIR:
    name: str
    getter: ApiExportRef | None = None
    setter: ApiExportRef | None = None


@dataclass(frozen=True)
class ApiConstantIR:
    name: str
    value_kind: str | None
    source_kind: str
    c_expression: str | None
    source_name: str | None = None
    availability_guard: str | None = None
    initializer: str | None = None
    parameters: tuple[str, ...] | None = None


@dataclass(frozen=True)
class ApiMacroIR:
    name: str
    parameters: tuple[str, ...] | None
    initializer: str | None
    value_kind: str | None


@dataclass
class ApiClassIR:
    name: str
    c_type: str
    configured_constructor: str | None
    has_constructor: bool
    constructor_binding: str | None
    base: str | None
    methods: list[ApiExportRef] = field(default_factory=list)
    static_methods: list[ApiExportRef] = field(default_factory=list)
    properties: list[ApiPropertyIR] = field(default_factory=list)
    constants: list[ApiConstantIR] = field(default_factory=list)
    extra_methods: list[tuple[str, str]] = field(default_factory=list)
    extra_properties: list[tuple[str, str | None, str | None]] = field(default_factory=list)


@dataclass
class ResolvedApiSurface:
    """Accepted-only API payload consumed by generated C renderers."""

    apis: list[ApiRecord] = field(default_factory=list)
    uses: list[ApiUse] = field(default_factory=list)
    classes: list[ApiClassIR] = field(default_factory=list)
    root_constants: list[ApiConstantIR] = field(default_factory=list)
    macros: list[ApiMacroIR] = field(default_factory=list)
    event_assertions: list[tuple[str, str]] = field(default_factory=list)
    names: list[str] = field(default_factory=list)


@dataclass
class BindingIR:
    types: dict[str, TypeInfo] = field(default_factory=dict)
    uses: list[ApiUse] = field(default_factory=list)
    apis: list[ApiRecord] = field(default_factory=list)
    selected_names: list[str] = field(default_factory=list)
    accepted_names: list[str] = field(default_factory=list)
    special_names: list[str] = field(default_factory=list)
    rejected_names: list[str] = field(default_factory=list)
    diagnostics: list[Diagnostic] = field(default_factory=list)
    output_texts: dict[str, str] = field(default_factory=dict)
    api_surface: ResolvedApiSurface | None = None


def ir_to_dict(ir: BindingIR) -> dict[str, Any]:
    """Return stable JSON-compatible diagnostics without copying the C AST."""
    api_status_counts = {
        status.value: sum(api.status == status for api in ir.apis)
        for status in ApiStatus
    }
    types: list[dict[str, Any]] = []
    for name in sorted(ir.types):
        item = ir.types[name]
        types.append(
            {
                "name": item.name,
                "canonical_name": item.canonical_name,
                "c_category": item.category.value,
                "typedef_chain": item.typedef_chain,
                "dependencies": sorted(set(item.dependencies)),
                "fields": [
                    {
                        "name": field.name,
                        "type": field.type_name,
                        "bitsize": field.bitsize,
                        "array_shape": list(field.array_shape),
                        "pointer_depth": field.pointer_depth,
                        "function_pointer": field.is_function_pointer,
                        "unsupported_reason": field.unsupported_reason,
                    }
                    for field in item.fields
                ],
                "sni_representation": item.representation.value,
                "status": type_status(item.representation),
                "resolution_source": item.resolution_source,
                "inference_reason": item.inference_reason,
                "rejection_reason": item.rejection_reason,
                "referenced_by": sorted(set(item.referenced_by)),
                "resource_category": item.resource_category,
            }
        )
    return {
        "summary": {
            "selected_api_count": len(ir.selected_names),
            "accepted_api_count": len(ir.accepted_names),
            "special_api_count": len(ir.special_names),
            "excluded_api_count": api_status_counts[ApiStatus.EXCLUDED_BLACKLIST.value],
            "rejected_api_count": sum(
                api_status_counts[status.value]
                for status in (
                    ApiStatus.REJECTED_UNSUPPORTED_TYPE,
                    ApiStatus.REJECTED_SPECIAL_REQUIRED,
                    ApiStatus.REJECTED_LIFECYCLE,
                    ApiStatus.REJECTED_UNRESOLVED,
                )
            ),
            "candidate_api_count": len(ir.selected_names),
            "api_status_counts": api_status_counts,
        },
        "apis": [
            {
                "name": api.name,
                "status": api.status.value,
                "class": api.class_name,
                "selection_kind": api.selection_kind,
                "special_binding": api.special_binding,
                "reason": api.reason,
                "reason_code": api.reason_code,
                "issues": api.issues,
                "is_constructor": api.is_constructor,
                "docstring": api.docstring,
                "return_docstring": api.return_docstring,
                "native_call_name": api.native_call_name,
                "output_string": list(api.output_string) if api.output_string else None,
                "result_owner": api.result_owner,
                "cleanup_parameters": api.cleanup_parameters,
            }
            for api in sorted(ir.apis, key=lambda entry: (entry.name, entry.status.value))
        ],
        "types": types,
        "uses": [
            {
                "function": use.function,
                "position": use.position,
                "spelling": use.use_site.spelling,
                "base_type": use.use_site.base_name,
                "pointer_depth": use.use_site.pointer_depth,
                "const": use.use_site.is_const,
                "array_shape": list(use.use_site.array_shape),
                "conversion": use.conversion,
                "sni_type": use.sni_type,
                "status": use.status.value,
                "reason": use.reason,
                "reason_code": use.reason_code,
                "name": use.name,
                "docstring": use.docstring,
                "js_type": use.js_type,
                "js_check": use.js_check,
                "js_to_c": {"mode": use.js2c_mode, "helper": use.js2c_expr},
                "c_to_js": {"mode": use.c2js_mode, "helper": use.c2js_expr},
                "argument_mode": use.argument_mode,
                "allow_null": use.allow_null,
                "copy_back": use.copy_back,
                "pointee_type": use.pointee_type,
                "lifecycle_class": use.lifecycle_class,
            }
            for use in sorted(ir.uses, key=lambda entry: (entry.function, entry.position))
        ],
        "api_surface": {
            "names": ir.api_surface.names if ir.api_surface else [],
            "classes": [
                {
                    "name": cls.name,
                    "c_type": cls.c_type,
                    "constructor": cls.configured_constructor,
                    "has_constructor": cls.has_constructor,
                    "constructor_binding": cls.constructor_binding,
                    "base": cls.base,
                    "methods": [
                        {
                            "function": ref.function,
                            "special_binding": ref.special_binding,
                            "receiver": None if ref.receiver is None else {
                                "owner_class": ref.receiver.owner_class,
                                "binding_kind": ref.receiver.binding_kind,
                                "receiver_index": ref.receiver.receiver_index,
                                "expected_receiver_type": ref.receiver.expected_receiver_type,
                                "actual_receiver_type": ref.receiver.actual_receiver_type,
                                "receiver_validated": ref.receiver.receiver_validated,
                            },
                        }
                        for ref in cls.methods
                    ],
                    "static_methods": [
                        {"function": ref.function, "special_binding": ref.special_binding}
                        for ref in cls.static_methods
                    ],
                    "properties": [
                        {
                            "name": prop.name,
                            "getter": None if prop.getter is None else {
                                "function": prop.getter.function,
                                "special_binding": prop.getter.special_binding,
                                "receiver": None if prop.getter.receiver is None else {
                                    "owner_class": prop.getter.receiver.owner_class,
                                    "binding_kind": prop.getter.receiver.binding_kind,
                                    "receiver_index": prop.getter.receiver.receiver_index,
                                    "expected_receiver_type": prop.getter.receiver.expected_receiver_type,
                                    "actual_receiver_type": prop.getter.receiver.actual_receiver_type,
                                    "receiver_validated": prop.getter.receiver.receiver_validated,
                                },
                            },
                            "setter": None if prop.setter is None else {
                                "function": prop.setter.function,
                                "special_binding": prop.setter.special_binding,
                                "receiver": None if prop.setter.receiver is None else {
                                    "owner_class": prop.setter.receiver.owner_class,
                                    "binding_kind": prop.setter.receiver.binding_kind,
                                    "receiver_index": prop.setter.receiver.receiver_index,
                                    "expected_receiver_type": prop.setter.receiver.expected_receiver_type,
                                    "actual_receiver_type": prop.setter.receiver.actual_receiver_type,
                                    "receiver_validated": prop.setter.receiver.receiver_validated,
                                },
                            },
                        }
                        for prop in cls.properties
                    ],
                    "constants": [
                        {
                            "name": value.name,
                            "value_kind": value.value_kind,
                            "source_kind": value.source_kind,
                            "source_name": value.source_name,
                            "c_expression": value.c_expression,
                            "availability_guard": value.availability_guard,
                            "initializer": value.initializer,
                            "parameters": value.parameters,
                        }
                        for value in cls.constants
                    ],
                    "extra_methods": [list(value) for value in cls.extra_methods],
                    "extra_properties": [list(value) for value in cls.extra_properties],
                }
                for cls in (ir.api_surface.classes if ir.api_surface else [])
            ],
            "root_constants": [
                {
                    "name": value.name,
                    "value_kind": value.value_kind,
                    "source_kind": value.source_kind,
                    "source_name": value.source_name,
                    "c_expression": value.c_expression,
                    "availability_guard": value.availability_guard,
                    "initializer": value.initializer,
                    "parameters": value.parameters,
                }
                for value in (ir.api_surface.root_constants if ir.api_surface else [])
            ],
            "macros": [
                {
                    "name": macro.name,
                    "parameters": macro.parameters,
                    "initializer": macro.initializer,
                    "value_kind": macro.value_kind,
                }
                for macro in (ir.api_surface.macros if ir.api_surface else [])
            ],
            "event_assertions": [list(item) for item in (ir.api_surface.event_assertions if ir.api_surface else [])],
        },
    }
