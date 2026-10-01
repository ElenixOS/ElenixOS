"""Command result envelope shared by CLI, CI, and future GUI consumers."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

from .ir import Diagnostic
from .progress import ProgressEvent


@dataclass
class CommandResult:
    command: str
    success: bool
    summary: dict[str, Any]
    diagnostics: list[Diagnostic] = field(default_factory=list)
    result: dict[str, Any] = field(default_factory=dict)
    phase: str | None = None

    def to_dict(self, progress: list[ProgressEvent]) -> dict[str, Any]:
        data: dict[str, Any] = {
            "command": self.command,
            "success": self.success,
            "summary": self.summary,
            "progress": [item.to_dict() for item in progress],
            "diagnostics": [item.to_dict() for item in self.diagnostics],
            "result": self.result,
        }
        if self.phase:
            data["phase"] = self.phase
        return data
