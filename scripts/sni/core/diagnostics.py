"""Human and machine readable analysis views for the binding IR."""

from __future__ import annotations

import json
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
    new_surface = set(ir.selected_names) - set(ir.rejected_names)
    referenced = [item for item in data["types"] if item["referenced_by"]]
    inferred = [item for item in referenced if item["resolution_source"] in {"builtin", "inferred"}]
    explicit = [item for item in referenced if item["resolution_source"] == "explicit"]
    resources = [item for item in referenced if item["sni_representation"] == "managed_resource"]
    unresolved = [item for item in data["types"] if item["sni_representation"] == "unknown"]
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
    data["analysis"] = {
        "api_counts": {
            "selected": len(ir.selected_names),
            "accepted": len(ir.accepted_names),
            "special": len(ir.special_names),
            "rejected": len(ir.rejected_names),
            "candidate": sum(item["status"] == ApiStatus.CANDIDATE.value for item in data["apis"]),
        },
        "type_counts": {
            "referenced": len(referenced),
            "automatically_inferred": len(inferred),
            "explicit": len(explicit),
            "managed_resources": len(resources),
            "unresolved": len(unresolved),
        },
        "referenced_types": [item["name"] for item in referenced],
        "automatically_inferred_types": [item["name"] for item in inferred],
        "explicit_types": [item["name"] for item in explicit],
        "managed_resources": resources,
        "special_mappings": sorted(special_mappings, key=lambda item: (item["api"], item["kind"], item["binding"])),
        "unresolved_types": unresolved,
        "surface_delta": {
            "baseline_available": old_surface is not None,
            "newly_selected": sorted(new_surface - old_surface) if old_surface is not None else [],
            "removed": sorted(old_surface - new_surface) if old_surface is not None else [],
            "rejected": sorted(ir.rejected_names),
        },
        "warnings": [item for item in ir.diagnostics if item.get("severity") in {"warning", "info"}],
        "errors": [item for item in ir.diagnostics if item.get("severity") == "error"],
    }
    return data


def render_human_analysis(analysis: dict[str, Any]) -> str:
    apis = analysis["analysis"]["api_counts"]
    types = analysis["analysis"]["type_counts"]
    lines = [
        "API surface:",
        f"  selected {apis['selected']}, accepted {apis['accepted']}, special {apis['special']}, rejected {apis['rejected']}, unresolved candidate {apis['candidate']}",
        "Types:",
        f"  referenced {types['referenced']}, inferred {types['automatically_inferred']}, explicit {types['explicit']}, managed resources {types['managed_resources']}, unresolved {types['unresolved']}",
    ]
    delta = analysis["analysis"]["surface_delta"]
    if delta["baseline_available"]:
        lines.append(f"API surface delta: +{len(delta['newly_selected'])} newly selected, -{len(delta['removed'])} removed, {len(delta['rejected'])} rejected")
    else:
        lines.append("API surface delta: baseline marker is not yet available; the next successful generate establishes it")
    unresolved = analysis["analysis"]["unresolved_types"]
    if unresolved:
        lines.append("Unresolved types:")
        lines.extend(f"  - {item['name']}: {item['rejection_reason']} (used by {', '.join(item['referenced_by'])})" for item in unresolved)
    rejected = [item for item in analysis["apis"] if item["status"] == ApiStatus.REJECTED.value]
    if rejected:
        lines.append("Rejected APIs:")
        lines.extend(f"  - {item['name']}: {item['reason']}" for item in rejected[:30])
        if len(rejected) > 30:
            lines.append(f"  - ... {len(rejected) - 30} more; use --format json for all entries")
    for diagnostic in analysis["analysis"]["warnings"]:
        lines.append(f"{diagnostic['severity'].upper()} {diagnostic['code']}: {diagnostic['message']}")
    for diagnostic in analysis["analysis"]["errors"]:
        lines.append(f"ERROR {diagnostic['code']}: {diagnostic['message']}")
    return "\n".join(lines)


def render_json(analysis: dict[str, Any]) -> str:
    return json.dumps(analysis, ensure_ascii=False, sort_keys=True, indent=2) + "\n"
