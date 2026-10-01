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
            }
            for use in sorted(ir.uses, key=lambda entry: (entry.function, entry.position))
        ],
    }
