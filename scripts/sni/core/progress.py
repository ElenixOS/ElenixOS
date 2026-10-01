"""Progress sink contract used by command orchestration without terminal output."""

from __future__ import annotations

from contextlib import AbstractContextManager
from dataclasses import dataclass
from typing import Protocol


@dataclass(frozen=True)
class ProgressEvent:
    index: int
    total: int
    label: str
    status: str

    def to_dict(self) -> dict[str, int | str]:
        return {"index": self.index, "total": self.total, "label": self.label, "status": self.status}


class ProgressStage(Protocol):
    def warning(self) -> None: ...

    def failure(self) -> None: ...


class ProgressReporter(Protocol):
    @property
    def current(self) -> str: ...

    def stage(self, label: str) -> AbstractContextManager[ProgressStage]: ...

    def detail(self, message: str) -> None: ...
