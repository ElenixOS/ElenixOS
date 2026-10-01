"""Shared progress, terminal, and JSON presentation for every SNI command."""

from __future__ import annotations

import json
import os
import shutil
import sys
import textwrap
from contextlib import contextmanager
from dataclasses import dataclass
from typing import Any, Iterator

from ..core.ir import ApiStatus, Diagnostic, Severity
from ..core.progress import ProgressEvent
from ..core.result import CommandResult

API_CATEGORY_STATUSES = {"generic", "special", "blacklist", "unsupported", "special-required", "lifecycle", "unresolved"}
DIAGNOSTIC_CATEGORIES = {"warnings", "errors", "resolution", "configuration", "validation"}


@dataclass
class _Stage:
    label: str
    status: str = "OK"

    def warning(self) -> None:
        self.status = "WARNING"

    def failure(self) -> None:
        self.status = "FAILED"


class ProgressReporter:
    """Collect stable stage events; subclasses decide where results are rendered."""

    def __init__(self, stages: list[str], quiet: bool = False, verbose: bool = False):
        self.stages = list(stages)
        self.quiet = quiet
        self.verbose = verbose
        self.current = "Starting"
        self.index = 0
        self.progress_events: list[ProgressEvent] = []

    @contextmanager
    def stage(self, label: str) -> Iterator[_Stage]:
        self.index += 1
        self.current = label
        stage = _Stage(label)
        try:
            yield stage
        except BaseException:
            stage.status = "FAILED"
            raise
        finally:
            event = ProgressEvent(self.index, len(self.stages), label, stage.status)
            self.progress_events.append(event)
            self._render_stage(event)

    def _render_stage(self, event: ProgressEvent) -> None:
        raise NotImplementedError

    def detail(self, message: str) -> None:
        if self.verbose and not self.quiet:
            self._write_detail(message)

    def _write_detail(self, message: str) -> None:
        raise NotImplementedError

    def report(self, result: CommandResult, view: dict[str, Any] | None = None) -> None:
        raise NotImplementedError


class TerminalReporter(ProgressReporter):
    def __init__(self, stages: list[str], quiet: bool = False, verbose: bool = False, no_color: bool = False):
        super().__init__(stages, quiet=quiet, verbose=verbose)
        self.color_enabled = (
            not no_color
            and "NO_COLOR" not in os.environ
            and "CI" not in os.environ
            and sys.stdout.isatty()
        )

    def _paint(self, value: str, code: str) -> str:
        if not self.color_enabled:
            return value
        return f"\033[{code}m{value}\033[0m"

    def _render_stage(self, event: ProgressEvent) -> None:
        if self.quiet:
            return
        total = event.total
        label_width = max((len(label) for label in self.stages), default=len(event.label))
        state = event.status
        style = "32" if state == "OK" else "33" if state == "WARNING" else "31"
        prefix = f"[{event.index}/{total}] {event.label:<{label_width}}"
        print(f"{prefix}  {self._paint(state, style)}", file=sys.stderr)

    def _write_detail(self, message: str) -> None:
        print(f"  {message}", file=sys.stderr)

    def report(self, result: CommandResult, view: dict[str, Any] | None = None) -> None:
        view = view or {}
        if result.command == "analyze":
            self._report_analysis(result, view)
        elif result.command == "validate":
            self._report_validation(result, view)
        elif result.command == "generate":
            self._report_generation(result)
        elif result.command == "update-config":
            self._report_config_update(result)
        elif result.command == "dump-ir":
            self._report_dump_ir(result)
        category = view.get("category")
        if result.command in {"generate", "update-config"}:
            if category in API_CATEGORY_STATUSES:
                analysis = result.result.get("analysis")
                if analysis:
                    self._report_analysis_details(analysis, category, result.diagnostics)
            elif category in DIAGNOSTIC_CATEGORIES:
                self._report_analysis_details(result.result.get("analysis", {}), category, result.diagnostics)
            elif view.get("details"):
                self._report_diagnostic_details(result.diagnostics, include_info=True)

    def _report_analysis(self, result: CommandResult, view: dict[str, Any]) -> None:
        data = result.result
        analysis = data["analysis"]
        counts = analysis["analysis"]["api_counts"]
        type_counts = analysis["analysis"]["type_counts"]
        print("SNI / LVGL Binding Analysis")
        print("\nAPI Surface")
        statuses = [
            ("Generic", counts["accepted_generic"], "Automatically generated"),
            ("Special binding", counts["accepted_special"], "Explicit special implementation"),
            ("Blacklisted", counts["blacklisted"], "Intentionally excluded"),
            ("Unsupported type", counts["unsupported"], "No safe generic type representation"),
            ("Special binding required", counts["special_required"], "Signature needs a special wrapper"),
            ("Lifecycle rejected", counts["lifecycle_rejected"], "Violates SNI lifecycle rules"),
            ("Unresolved", counts["unresolved"], "Missing binding decision"),
            ("Total candidates", counts["selected"], ""),
        ]
        self._table(("Status", "Count", "Meaning"), statuses)
        print(
            f"Exported {counts['exported']}  |  Excluded {counts['blacklisted']}  |  "
            f"Needs attention {counts['needs_attention']}"
        )
        print("\nTypes")
        self._table(
            ("Result", "Count"),
            [
                ("Inferred", type_counts["automatically_inferred"]),
                ("Explicit", type_counts["explicit"]),
                ("Managed resources", type_counts["managed_resources"]),
                ("Unsupported", type_counts["unsupported"]),
                ("Special required", type_counts["special_required"]),
                ("Unresolved", type_counts["unresolved"]),
                ("Referenced", type_counts["referenced"]),
            ],
        )
        delta = analysis["analysis"].get("surface_delta")
        if delta:
            print("\nSurface Changes")
            changes = [
                ("Newly selected", len(delta["newly_selected"])),
                ("Removed", len(delta["removed"])),
                ("Status changed", len(delta["status_changed"])),
            ]
            if any(count for _, count in changes):
                self._table(("Change", "Count"), changes)
            else:
                print("No API surface changes from the generated baseline.")

        filter_category = view.get("category")
        if view.get("api_detail"):
            self._report_api_detail(view["api_detail"])
        elif view.get("type_detail"):
            self._report_type_detail(view["type_detail"])
        elif view.get("details") or filter_category:
            self._report_analysis_details(analysis, filter_category, result.diagnostics)
        elif counts["needs_attention"]:
            print(f"\nNeeds attention: {counts['needs_attention']} API candidates")
            print("Run with --details for grouped diagnostics.")
        else:
            print("\nAll selected bindings are resolved.")

    def _report_analysis_details(self, analysis: dict[str, Any], category: str | None, diagnostics: list[Diagnostic]) -> None:
        if category in {"warnings", "errors", "resolution", "configuration", "validation"}:
            matches = [
                item
                for item in diagnostics
                if (category == "warnings" and item.severity in {Severity.WARNING, Severity.INFO})
                or (category == "errors" and item.severity == Severity.ERROR)
                or item.category == category
            ]
            if matches:
                self._report_diagnostic_details(matches, include_info=category == "warnings")
            else:
                print(f"No diagnostics in category '{category}'.")
            return
        apis = analysis["apis"]
        types = analysis["types"]
        categories = [category] if category else ["unsupported", "special-required", "lifecycle", "unresolved", "warnings"]
        if "unsupported" in categories:
            rows = [
                (item["name"], item["rejection_reason"], len(item["referenced_by"]))
                for item in types
                if item["sni_representation"] == "rejected" and item["referenced_by"]
            ]
            self._report_group("Unsupported Types", ("Type", "Reason", "APIs"), rows, show_empty=category == "unsupported")
            if self.verbose:
                for item in types:
                    if item["sni_representation"] == "rejected" and item["referenced_by"]:
                        self.detail(f"{item['name']} affects: {', '.join(item['referenced_by'])}")
        status_categories = {
            "special-required": (ApiStatus.REJECTED_SPECIAL_REQUIRED.value, "Special Binding Required"),
            "lifecycle": (ApiStatus.REJECTED_LIFECYCLE.value, "Lifecycle Rejections"),
            "unresolved": (ApiStatus.REJECTED_UNRESOLVED.value, "Unresolved APIs"),
            "blacklist": (ApiStatus.EXCLUDED_BLACKLIST.value, "Blacklisted APIs"),
            "generic": (ApiStatus.ACCEPTED_GENERIC.value, "Generic APIs"),
            "special": (ApiStatus.ACCEPTED_SPECIAL.value, "Special APIs"),
        }
        for selected_category, (status, title) in status_categories.items():
            if selected_category not in categories:
                continue
            if selected_category == "unresolved":
                entries = [item for item in types if item["sni_representation"] == "unknown" and item["referenced_by"]]
                self._report_group(
                    "Unresolved Types",
                    ("Type", "Reason", "APIs"),
                    [(item["name"], item["rejection_reason"], ", ".join(item["referenced_by"])) for item in entries],
                    show_empty=category == "unresolved",
                )
                continue
            entries = [item for item in apis if item["status"] == status]
            rows = [(item["name"], item["reason"] or item.get("special_binding") or "") for item in entries]
            self._report_group(title, ("API", "Reason"), rows, show_empty=category == selected_category)
        if "warnings" in categories:
            visible = [item for item in diagnostics if item.severity in {Severity.WARNING, Severity.INFO}]
            self._report_diagnostic_details(visible, include_info=True)

    def _report_api_detail(self, api: dict[str, Any]) -> None:
        print(f"\n{api['name']}")
        print(f"Status: {api['status']}")
        uses = [item for item in api["uses"]]
        return_use = next((item for item in uses if item["position"] == "return"), None)
        print(f"Return type: {return_use['spelling'] if return_use else 'unknown'}")
        params = [item for item in uses if item["position"].startswith("parameter:")]
        if params:
            print("Parameters:")
            for item in params:
                name = item["position"].split(":", 2)[-1]
                print(f"  {name}: {item['spelling']} ({item['status']})")
        reasons = api.get("issues") or ([{"reason": api["reason"], "position": "", "type": "", "reason_code": api["reason_code"]}] if api.get("reason") else [])
        if reasons:
            print("Reasons:")
            for item in reasons:
                where = f" at {item['position']} ({item['type']})" if item.get("position") else ""
                print(f"  - {item['reason']}{where}")
        action = _suggested_action(api["status"])
        if action:
            print(f"Suggested action: {action}")

    def _report_type_detail(self, item: dict[str, Any]) -> None:
        print(f"\n{item['name']}")
        print(f"Status: {_type_status(item)}")
        print(f"Representation: {item['sni_representation']}")
        reason = item.get("rejection_reason") or item.get("inference_reason")
        if reason:
            print("Reasons:")
            for value in reason.split("; "):
                print(f"  - {value}")
        if item.get("referenced_by"):
            print("Affected APIs:")
            for name in item["referenced_by"]:
                print(f"  - {name}")
        if item.get("fields"):
            print("Fields:")
            for field in item["fields"]:
                if field.get("unsupported_reason") or field.get("bitsize") is not None or field.get("array_shape") or field.get("pointer_depth") or field.get("function_pointer"):
                    facts = []
                    if field.get("unsupported_reason"):
                        facts.append(field["unsupported_reason"])
                    if field.get("array_shape"):
                        facts.append(f"array {field['array_shape']}")
                    if field.get("bitsize") is not None:
                        facts.append(f"bitfield ({field['bitsize']} bits)")
                    if field.get("pointer_depth"):
                        facts.append(f"pointer depth {field['pointer_depth']}")
                    if field.get("function_pointer"):
                        facts.append("callback")
                    print(f"  - {field['name']}: {field['type']} ({', '.join(facts)})")

    def _report_validation(self, result: CommandResult, view: dict[str, Any]) -> None:
        summary = result.summary
        print("Validation Summary")
        self._table(
            ("Result", "Count"),
            [
                ("Errors", summary["errors"]),
                ("Warnings", summary["warnings"]),
                ("Unresolved bindings", summary["unresolved"]),
                ("Unused configuration", summary["unused_configuration"]),
                ("API candidates needing attention", summary["needs_attention"]),
                ("Blacklisted APIs", summary["blacklisted"]),
            ],
        )
        print("\nValidation successful." if result.success else "\nValidation failed.")
        if result.success and summary["needs_attention"]:
            print(
                f"{summary['needs_attention']} API candidates are not exported yet; "
                f"{summary['blacklisted']} are intentionally blacklisted."
            )
        if not result.success or view.get("details") or view.get("category"):
            selected_category = view.get("category")
            entries = [item for item in result.diagnostics if selected_category is None or _diagnostic_matches(item, selected_category)]
            self._report_diagnostic_details(entries, include_info=view.get("details", False))

    def _report_generation(self, result: CommandResult) -> None:
        if result.success:
            artifacts = result.result.get("artifacts", [])
            print("Generated Artifacts")
            self._table(("Artifact", "Status"), [(item["name"], item["status"]) for item in artifacts])
            print("\nGeneration completed successfully.")
            print(
                f"Exported {result.summary['exported']} APIs; "
                f"{result.summary['needs_attention']} candidates need attention; "
                f"{result.summary['blacklisted']} are intentionally blacklisted."
            )
        else:
            print(f"Generation failed during {result.phase or 'unknown phase'}.")
            self._report_diagnostic_details(result.diagnostics, include_info=False)

    def _report_config_update(self, result: CommandResult) -> None:
        items = result.result.get("added_unknown_types", [])
        print("Configuration Update")
        if items:
            self._table(("Type", "Added as"), [(name, "unknown") for name in items])
            print(f"\n{len(items)} entries added. Review them before validation can succeed.")
        else:
            print("No new unresolved types to add.")

    def _report_dump_ir(self, result: CommandResult) -> None:
        snapshot = dict(result.result.get("ir", {}))
        snapshot["diagnostics"] = [item.to_dict() for item in result.diagnostics]
        print(json.dumps(snapshot, ensure_ascii=False, sort_keys=True, indent=2))

    def _report_diagnostic_details(self, diagnostics: list[Diagnostic], include_info: bool = False) -> None:
        visible = [item for item in diagnostics if item.severity == Severity.ERROR or item.severity == Severity.WARNING or include_info]
        if not visible:
            return
        for severity in (Severity.ERROR, Severity.WARNING, Severity.INFO):
            group = [item for item in visible if item.severity == severity]
            if not group:
                continue
            title = {Severity.ERROR: "Errors", Severity.WARNING: "Warnings", Severity.INFO: "Information"}[severity]
            print(f"\n{title}")
            for item in group:
                subject = f" [{item.subject_kind} {item.subject}]" if item.subject else ""
                print(f"  {item.code}{subject}: {item.reason}")
                if item.references:
                    print(f"    Referenced by: {', '.join(item.references)}")
                if item.suggested_action:
                    print(f"    Action: {item.suggested_action}")

    def _report_group(self, title: str, headers: tuple[str, ...], rows: list[tuple[Any, ...]], show_empty: bool = False) -> None:
        if not rows and not show_empty:
            return
        print(f"\n{title}")
        self._table(headers, rows)

    def _table(self, headers: tuple[str, ...], rows: list[tuple[Any, ...]]) -> None:
        if not rows:
            normalized = [["(none)", *("" for _ in headers[1:])]]
        else:
            normalized = [[str(value) for value in row] for row in rows]
        available = max(50, shutil.get_terminal_size((120, 24)).columns - 2)
        widths = []
        for index, header in enumerate(headers):
            natural = max(len(header), *(len(row[index]) for row in normalized))
            if header.lower() in {"reason", "meaning"}:
                natural = min(natural, max(28, min(64, available - 32)))
            widths.append(natural)

        def border(left: str, junction: str, right: str) -> str:
            segments = ["─" * (width + 2) for width in widths]
            return "  " + left + junction.join(segments) + right

        def format_row(values: list[str]) -> str:
            cells = [f" {values[index].ljust(widths[index])} " for index in range(len(headers))]
            return "  │" + "│".join(cells) + "│"

        print(border("┌", "┬", "┐"))
        print(format_row([header for header in headers]))
        print(border("├", "┼", "┤"))
        for data_row in normalized:
            cells = [textwrap.wrap(value, width=width) or [""] for value, width in zip(data_row, widths)]
            height = max(len(cell) for cell in cells)
            for line_index in range(height):
                values = [cell[line_index] if line_index < len(cell) else "" for cell in cells]
                print(format_row(values))
        print(border("└", "┴", "┘"))


class JsonReporter(ProgressReporter):
    def __init__(self, stages: list[str], verbose: bool = False):
        super().__init__(stages, quiet=True, verbose=verbose)

    def _render_stage(self, event: ProgressEvent) -> None:
        return

    def _write_detail(self, message: str) -> None:
        return

    def report(self, result: CommandResult, view: dict[str, Any] | None = None) -> None:
        json.dump(result.to_dict(self.progress_events), sys.stdout, ensure_ascii=False, sort_keys=True, indent=2)
        sys.stdout.write("\n")


def _type_status(item: dict[str, Any]) -> str:
    return item["status"]


def _suggested_action(status: str) -> str | None:
    return {
        "REJECTED_UNSUPPORTED_TYPE": "Review the layout and add a safe explicit value-object declaration or a special binding.",
        "REJECTED_SPECIAL_REQUIRED": "Provide a deliberate special binding or exclude this API.",
        "REJECTED_LIFECYCLE": "Exclude this API or provide a deliberate special binding.",
        "REJECTED_UNRESOLVED": "Add a deliberate type binding, then run validate.",
        "EXCLUDED_BLACKLIST": "This API is intentionally excluded by configuration.",
    }.get(status)


def _diagnostic_matches(item: Diagnostic, category: str) -> bool:
    return item.category == category or item.status == category.upper().replace("-", "_") or item.code.lower().find(category.replace("-", "_")) >= 0
