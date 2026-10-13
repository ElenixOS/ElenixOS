from __future__ import annotations

import io
import json
import os
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch

from sni.cli.reporters import JsonReporter, TerminalReporter
from sni.core.config import BindingConfig
from sni.core.diagnostics import build_analysis
from sni.core.ir import ApiRecord, ApiStatus, BindingIR, Diagnostic, Severity
from sni.core.result import CommandResult
from sni.core.selection import SelectionResult


class ReporterModelTests(unittest.TestCase):
    def test_api_statuses_are_mutually_exclusive_and_attention_excludes_blacklist(self) -> None:
        statuses = list(ApiStatus)
        names = [f"api_{index}" for index in range(len(statuses))]
        apis = [ApiRecord(name, status) for name, status in zip(names, statuses)]
        ir = BindingIR(
            apis=apis,
            selected_names=names,
            accepted_names=["api_0"],
            special_names=["api_1"],
            rejected_names=names[2:],
        )
        config = BindingConfig(Path("bindings.json"), {"special_bindings": {}})
        with tempfile.TemporaryDirectory() as folder:
            analysis = build_analysis(ir, SelectionResult([], {}, [], {}, []), config, Path(folder) / "sni_api_lv.c")

        counts = analysis["analysis"]["api_counts"]
        self.assertEqual(sum(counts["status_counts"].values()), len(statuses))
        self.assertEqual(counts["selected"], len(statuses))
        self.assertEqual(counts["exported"], 2)
        self.assertEqual(counts["blacklisted"], 1)
        self.assertEqual(counts["needs_attention"], 4)
        self.assertEqual(counts["selected"], counts["exported"] + counts["blacklisted"] + counts["needs_attention"])

    def test_json_reporter_emits_one_structured_diagnostic(self) -> None:
        diagnostic = Diagnostic(
            code="UNRESOLVED_TYPE",
            severity=Severity.ERROR,
            category="resolution",
            status=ApiStatus.REJECTED_UNRESOLVED.value,
            subject_kind="type",
            subject="lv_waiting_t",
            type_name="lv_waiting_t",
            reason="No explicit binding resolution exists.",
            references=("lv_use_waiting",),
            suggested_action="Add a deliberate type declaration.",
        )
        reporter = JsonReporter(["Loading"])
        output = io.StringIO()
        with redirect_stdout(output):
            reporter.report(CommandResult("validate", False, {"errors": 1}, [diagnostic], {"valid": False}))
        result = json.loads(output.getvalue())
        self.assertEqual(result["command"], "validate")
        self.assertEqual(len(result["diagnostics"]), 1)
        self.assertEqual(result["diagnostics"][0]["type"], "lv_waiting_t")
        self.assertEqual(result["diagnostics"][0]["references"], ["lv_use_waiting"])

    def test_severity_does_not_follow_api_status(self) -> None:
        warning = Diagnostic(
            code="UNUSED_SPECIAL_BINDING",
            severity=Severity.WARNING,
            category="configuration",
            reason="The configured special binding is unused.",
        )
        blacklist = ApiRecord("lv_excluded_api", ApiStatus.EXCLUDED_BLACKLIST, reason="Excluded by config")
        serialized = warning.to_dict()
        self.assertEqual(blacklist.status, ApiStatus.EXCLUDED_BLACKLIST)
        self.assertEqual(serialized["severity"], "WARNING")
        self.assertIsNone(serialized["status"])

    def test_no_color_environment_is_respected_for_terminal_output(self) -> None:
        class TTY(io.StringIO):
            def isatty(self) -> bool:
                return True

        with patch.dict(os.environ, {"NO_COLOR": ""}, clear=False), patch("sys.stdout", TTY()):
            reporter = TerminalReporter(["Loading"])
            self.assertFalse(reporter.color_enabled)
            self.assertEqual(reporter._paint("OK", "32"), "OK")

    def test_terminal_tables_have_outer_borders_for_rows_and_empty_results(self) -> None:
        reporter = TerminalReporter([], quiet=True)
        tables = []
        for rows in ([("Generic", 4)], []):
            output = io.StringIO()
            with redirect_stdout(output):
                reporter._table(("Status", "Count"), rows)
            tables.append(output.getvalue().strip())
        for table in tables:
            lines = table.splitlines()
            self.assertTrue(lines[0].lstrip().startswith("┌"))
            self.assertTrue(lines[0].rstrip().endswith("┐"))
            self.assertTrue(lines[-1].lstrip().startswith("└"))
            self.assertTrue(lines[-1].rstrip().endswith("┘"))
            self.assertTrue(any(line.lstrip().startswith("│") for line in lines[1:-1]))


if __name__ == "__main__":
    unittest.main()
