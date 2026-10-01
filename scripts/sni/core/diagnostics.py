"""Structured binding analysis derived from the typed SNI IR."""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any

from .config import BindingConfig
from .ir import ApiStatus, BindingIR, ir_to_dict
from .selection import SelectionResult


def old_surface_from_generated(path: Path) -> set[str] | None:
    if not path.exists():
        return None
    text = path.read_text(encoding="utf-8")
    match = re.search(r"/\* SNI_API_SURFACE_BEGIN\s*(.*?)\s*SNI_API_SURFACE_END \*/", text, re.S)
    if not match:
        return None
    return {line.strip() for line in match.group(1).splitlines() if line.strip()}


def build_analysis(
    ir: BindingIR,
    selection: SelectionResult,
    config: BindingConfig,
    api_output_path: Path,
) -> dict[str, Any]:
    data = ir_to_dict(ir)
    old_surface = old_surface_from_generated(api_output_path)
    current_candidates = set(ir.selected_names)
    current_surface = set(ir.accepted_names) | set(ir.special_names)
    referenced = [item for item in data["types"] if item["referenced_by"]]
    inferred = [item for item in referenced if item["resolution_source"] in {"builtin", "inferred"}]
    explicit = [item for item in referenced if item["resolution_source"] == "explicit"]
    resources = [item for item in referenced if item["sni_representation"] == "managed_resource"]
    unresolved = [item for item in data["types"] if item["sni_representation"] == "unknown"]
    unsupported = [item for item in data["types"] if item["sni_representation"] == "rejected"]
    special_required = [item for item in data["types"] if item["sni_representation"] == "special_required"]
    status_counts = {
        status.value: sum(api.status.value == status.value for api in ir.apis)
        for status in ApiStatus
    }
    attention_statuses = {
        ApiStatus.REJECTED_UNSUPPORTED_TYPE.value,
        ApiStatus.REJECTED_SPECIAL_REQUIRED.value,
        ApiStatus.REJECTED_LIFECYCLE.value,
        ApiStatus.REJECTED_UNRESOLVED.value,
    }
    accepted_generic = status_counts[ApiStatus.ACCEPTED_GENERIC.value]
    accepted_special = status_counts[ApiStatus.ACCEPTED_SPECIAL.value]
    needs_attention = sum(status_counts[status] for status in attention_statuses)

    special_mappings = []
    special = config.special_bindings
    for group in ("apis", "constructors"):
        special_mappings.extend(
            {"api": api, "binding": binding, "kind": group[:-1] if group.endswith("s") else group}
            for api, binding in special.get(group, {}).items()
        )
    special_mappings.extend(
        {"api": item["function"], "binding": item["binding"], "kind": f"property_{item['accessor']}", "class": item["class"], "property": item["property"]}
        for item in special.get("properties", [])
    )

    api_counts = {
        "selected": len(ir.selected_names),
        "exported": accepted_generic + accepted_special,
        "accepted_generic": accepted_generic,
        "accepted_special": accepted_special,
        "blacklisted": status_counts[ApiStatus.EXCLUDED_BLACKLIST.value],
        "unsupported": status_counts[ApiStatus.REJECTED_UNSUPPORTED_TYPE.value],
        "special_required": status_counts[ApiStatus.REJECTED_SPECIAL_REQUIRED.value],
        "lifecycle_rejected": status_counts[ApiStatus.REJECTED_LIFECYCLE.value],
        "unresolved": status_counts[ApiStatus.REJECTED_UNRESOLVED.value],
        "needs_attention": needs_attention,
        "status_counts": status_counts,
    }

    delta = None
    if old_surface is not None:
        changed = (old_surface & current_candidates) - current_surface
        changed_by_status = {
            status.value: sorted(name for name in changed if any(api["name"] == name and api["status"] == status.value for api in data["apis"]))
            for status in (
                ApiStatus.EXCLUDED_BLACKLIST,
                ApiStatus.REJECTED_UNSUPPORTED_TYPE,
                ApiStatus.REJECTED_SPECIAL_REQUIRED,
                ApiStatus.REJECTED_LIFECYCLE,
                ApiStatus.REJECTED_UNRESOLVED,
            )
        }
        delta = {
            "baseline_available": True,
            "newly_selected": sorted(current_surface - old_surface),
            "removed": sorted(old_surface - current_candidates),
            "status_changed": sorted(changed),
            "status_changed_to": changed_by_status,
        }

    data["analysis"] = {
        "api_counts": api_counts,
        "type_counts": {
            "referenced": len(referenced),
            "automatically_inferred": len(inferred),
            "explicit": len(explicit),
            "managed_resources": len(resources),
            "unsupported": len(unsupported),
            "special_required": len(special_required),
            "unresolved": len(unresolved),
        },
        "referenced_types": [item["name"] for item in referenced],
        "automatically_inferred_types": [item["name"] for item in inferred],
        "explicit_types": [item["name"] for item in explicit],
        "managed_resources": resources,
        "unsupported_types": unsupported,
        "special_required_types": special_required,
        "special_mappings": sorted(special_mappings, key=lambda item: (item["api"], item["kind"], item["binding"])),
        "unresolved_types": unresolved,
        "surface_delta": delta,
    }
    return data
