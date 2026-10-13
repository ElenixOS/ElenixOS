"""Strict schema/version checks for the single SNI binding configuration."""

from __future__ import annotations

import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any


SCHEMA_VERSION = 1
ROOT_KEYS = {
    "$schema", "schema_version", "description", "api_selection", "function_overrides",
    "type_declarations", "special_conversions", "special_bindings", "runtime_type_ids", "api_rejection_reasons",
}
CLASS_KEYS = {"c_type", "constructor", "base", "methods", "static_methods", "constants"}


@dataclass(frozen=True)
class BindingConfig:
    path: Path
    data: dict[str, Any]

    @property
    def api_selection(self) -> dict[str, Any]:
        return self.data["api_selection"]

    @property
    def classes(self) -> dict[str, Any]:
        return self.api_selection["classes"]

    @property
    def type_declarations(self) -> dict[str, Any]:
        return self.data.get("type_declarations", {})

    @property
    def special_bindings(self) -> dict[str, Any]:
        return self.data.get("special_bindings", {})


def _expect(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def validate_config(data: Any, path: Path, special_ids: set[str] | None = None) -> BindingConfig:
    _expect(isinstance(data, dict), f"{path}: root must be an object")
    extra = set(data) - ROOT_KEYS
    missing = {"schema_version", "api_selection"} - set(data)
    _expect(not extra, f"{path}: unsupported fields: {', '.join(sorted(extra))}")
    _expect(not missing, f"{path}: missing fields: {', '.join(sorted(missing))}")
    _expect(data["schema_version"] == SCHEMA_VERSION, f"{path}: unsupported schema_version {data['schema_version']!r}")

    selection = data["api_selection"]
    _expect(isinstance(selection, dict), f"{path}: api_selection must be an object")
    _expect(set(selection) <= {"classes", "scan"}, f"{path}: api_selection has unsupported fields")
    classes = selection.get("classes")
    _expect(isinstance(classes, dict) and bool(classes), f"{path}: api_selection.classes must be a non-empty object")
    seen_selectors: set[tuple[str, str, str]] = set()
    for name, cfg in classes.items():
        _expect(bool(re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", name)), f"{path}: invalid class name {name!r}")
        _expect(isinstance(cfg, dict), f"{path}: api_selection.classes.{name} must be an object")
        _expect(not (set(cfg) - CLASS_KEYS), f"{path}: api_selection.classes.{name} has unsupported fields")
        _expect(isinstance(cfg.get("c_type"), str) and cfg["c_type"].strip(), f"{path}: class {name} requires c_type")
        for field in ("methods", "static_methods", "constants"):
            value = cfg.get(field, [])
            _expect(isinstance(value, (list, str, dict)), f"{path}: classes.{name}.{field} has invalid type")
            entries = value if isinstance(value, list) else [value]
            for entry in entries:
                pattern = entry if isinstance(entry, str) else entry.get("name", entry.get("match", entry.get("pattern", "*"))) if isinstance(entry, dict) else ""
                _expect(bool(str(pattern).strip()), f"{path}: empty selector in classes.{name}.{field}")
                if isinstance(entry, dict):
                    _expect(set(entry) <= {"name", "match", "pattern", "arg_match", "args", "arg", "exclude"}, f"{path}: unsupported selector fields in classes.{name}.{field}")
                    _expect(isinstance(entry.get("exclude", []), list) and all(isinstance(value, str) for value in entry.get("exclude", [])), f"{path}: selector exclude must be a string array")
                key = (name, field, str(pattern).strip())
                _expect(key not in seen_selectors, f"{path}: duplicate selector {pattern!r} in classes.{name}.{field}")
                seen_selectors.add(key)
        constructor = cfg.get("constructor")
        if constructor is None:
            _expect(not cfg.get("methods"), f"{path}: static class {name} cannot declare instance methods")
            _expect(bool(cfg.get("static_methods")), f"{path}: static class {name} requires static_methods")

    scan = selection.get("scan", {})
    _expect(isinstance(scan, dict), f"{path}: api_selection.scan must be an object")
    _expect(set(scan) <= {"function", "constant", "macro"}, f"{path}: api_selection.scan has unsupported fields")
    for group in scan.values():
        _expect(isinstance(group, dict) and set(group) <= {"whitelist", "blacklist"}, f"{path}: scan group must contain whitelist/blacklist only")
        for patterns in group.values():
            _expect(isinstance(patterns, list) and all(isinstance(item, str) for item in patterns), f"{path}: scan patterns must be string arrays")

    overrides = data.get("function_overrides", {})
    _expect(isinstance(overrides, dict), f"{path}: function_overrides must be an object")
    allowed_override_keys = {"call", "string_ownership", "output_string"}
    for function, override in overrides.items():
        _expect(isinstance(override, dict) and set(override) <= allowed_override_keys, f"{path}: invalid override for {function}")
        if "string_ownership" in override:
            _expect(override["string_ownership"] == "copy", f"{path}: unsupported string_ownership for {function}")
        if "output_string" in override:
            output = override["output_string"]
            _expect(isinstance(output, dict) and set(output) == {"buffer_arg", "size_arg", "js_buffer_arg"}, f"{path}: invalid output_string override for {function}")
            _expect(all(isinstance(output[key], str) and output[key] for key in output), f"{path}: output_string fields for {function} must be non-empty strings")
            _expect(output["js_buffer_arg"] == "placeholder", f"{path}: output_string.js_buffer_arg must be placeholder")

    rejection_reasons = data.get("api_rejection_reasons", {})
    _expect(isinstance(rejection_reasons, dict) and all(isinstance(name, str) and isinstance(reason, str) and reason.strip() for name, reason in rejection_reasons.items()), f"{path}: api_rejection_reasons must map API names to non-empty reasons")

    declarations = data.get("type_declarations", {})
    _expect(isinstance(declarations, dict), f"{path}: type_declarations must be an object")
    for type_name, declaration in declarations.items():
        _expect(isinstance(declaration, dict), f"{path}: type_declarations.{type_name} must be an object")
        _expect(set(declaration) <= {"kind", "category", "creator", "reason"}, f"{path}: invalid declaration fields for {type_name}")
        _expect(declaration.get("kind") in {"managed_resource", "value_object", "unknown"}, f"{path}: invalid kind for {type_name}")
        if declaration["kind"] == "managed_resource":
            _expect(declaration.get("category") in {"pure_managed", "tree_dependent", "hybrid"}, f"{path}: invalid managed resource category for {type_name}")
            _expect(isinstance(declaration.get("creator"), str) and declaration["creator"], f"{path}: managed resource {type_name} requires creator")
        elif declaration["kind"] == "unknown":
            _expect(set(declaration) <= {"kind", "reason"}, f"{path}: unknown declaration {type_name} cannot set a representation")

    conversions = data.get("special_conversions", {})
    _expect(isinstance(conversions, dict), f"{path}: special_conversions must be an object")
    _expect(all(isinstance(k, str) and isinstance(v, str) and v for k, v in conversions.items()), f"{path}: special_conversions must map type spellings to IDs")

    special = data.get("special_bindings", {})
    _expect(isinstance(special, dict) and set(special) <= {"apis", "constructors", "properties", "class_extensions", "type_dependencies"}, f"{path}: invalid special_bindings")
    for group in ("apis", "constructors"):
        entries = special.get(group, {})
        _expect(isinstance(entries, dict), f"{path}: special_bindings.{group} must be an object")
        for api, binding in entries.items():
            _expect(isinstance(api, str) and isinstance(binding, str) and binding, f"{path}: invalid special binding for {api!r}")
            if special_ids is not None:
                _expect(binding in special_ids, f"{path}: special binding {binding!r} for {api} does not exist in C sources")
    props = special.get("properties", [])
    _expect(isinstance(props, list), f"{path}: special_bindings.properties must be an array")
    for item in props:
        _expect(isinstance(item, dict) and set(item) == {"class", "property", "function", "accessor", "binding"}, f"{path}: invalid special property binding")
        _expect(item.get("class") in classes and all(isinstance(item.get(key), str) and item[key] for key in ("property", "function", "binding")), f"{path}: special property binding must reference a configured class and non-empty names")
        _expect(item.get("accessor") in {"getter", "setter"}, f"{path}: property accessor must be getter or setter")
        if special_ids is not None:
            _expect(item["binding"] in special_ids, f"{path}: special binding {item['binding']!r} does not exist in C sources")
    extensions = special.get("class_extensions", {})
    _expect(isinstance(extensions, dict) and set(extensions) <= {"methods", "properties"}, f"{path}: invalid class_extensions")
    for kind in ("methods", "properties"):
        group = extensions.get(kind, {})
        _expect(isinstance(group, dict), f"{path}: class_extensions.{kind} must be an object")
        for class_name, entries in group.items():
            _expect(class_name in classes and isinstance(entries, list), f"{path}: class_extensions.{kind}.{class_name} must reference a class and array")
            for entry in entries:
                allowed = {"name", "binding"} if kind == "methods" else {"name", "getter", "setter"}
                _expect(isinstance(entry, dict) and set(entry) == allowed and isinstance(entry.get("name"), str) and entry["name"], f"{path}: malformed {kind} extension for {class_name}")
                if kind == "methods":
                    _expect(isinstance(entry.get("binding"), str) and entry["binding"], f"{path}: special method extension requires binding ID")
                    if special_ids is not None:
                        _expect(entry["binding"] in special_ids, f"{path}: class extension references missing wrapper {entry['binding']}")
                else:
                    _expect(entry.get("getter") is None or isinstance(entry.get("getter"), str), f"{path}: special property getter must be a binding ID or null")
                    _expect(entry.get("setter") is None or isinstance(entry.get("setter"), str), f"{path}: special property setter must be a binding ID or null")
                    if special_ids is not None:
                        for binding in (entry.get("getter"), entry.get("setter")):
                            _expect(binding is None or binding in special_ids, f"{path}: class extension references missing wrapper {binding}")
                    if special_ids is not None:
                        for binding in (entry.get("getter"), entry.get("setter")):
                            _expect(binding is None or binding in special_ids, f"{path}: class extension references missing wrapper {binding}")
    dependencies = special.get("type_dependencies", {})
    _expect(isinstance(dependencies, dict), f"{path}: special_bindings.type_dependencies must be an object")
    for binding, type_names in dependencies.items():
        _expect(isinstance(binding, str) and isinstance(type_names, list) and all(isinstance(name, str) and name for name in type_names), f"{path}: invalid type dependencies for {binding}")
        if special_ids is not None:
            _expect(binding in special_ids, f"{path}: type dependencies reference unimplemented special binding {binding}")

    runtime_ids = data.get("runtime_type_ids", {})
    _expect(isinstance(runtime_ids, dict) and set(runtime_ids) <= {"tree_dependent", "hybrid", "pure_managed", "legacy_handles", "value_objects"}, f"{path}: invalid runtime_type_ids")
    all_ids: list[str] = []
    for group, values in runtime_ids.items():
        _expect(isinstance(values, list) and all(isinstance(item, str) and item.startswith("SNI_") for item in values), f"{path}: runtime_type_ids.{group} must be SNI identifier arrays")
        all_ids.extend(values)
    _expect(len(all_ids) == len(set(all_ids)), f"{path}: duplicate runtime type ID")

    return BindingConfig(path=path, data=data)


def load_config(path: Path, special_ids: set[str] | None = None) -> BindingConfig:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"Cannot read binding configuration {path}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"Invalid binding JSON {path}:{exc.lineno}:{exc.colno}: {exc.msg}") from exc
    return validate_config(data, path, special_ids)
