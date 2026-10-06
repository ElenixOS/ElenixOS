"""Read the upstream gen_json schema into a small typed LVGL model."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Optional

from .ir import CCategory, CUseSite, FieldInfo


BUILTIN_C_TYPES = {
    "void", "bool", "char", "signed char", "unsigned char", "short", "short int",
    "unsigned short", "int", "unsigned", "unsigned int", "long", "long int",
    "unsigned long", "long long", "unsigned long long", "float", "double",
    "int8_t", "uint8_t", "int16_t", "uint16_t", "int32_t", "uint32_t",
    "int64_t", "uint64_t", "intptr_t", "uintptr_t", "size_t", "ptrdiff_t",
}


@dataclass(frozen=True)
class TypeNode:
    use_site: CUseSite
    category_hint: CCategory


def _quals(node: Any) -> set[str]:
    if not isinstance(node, dict):
        return set()
    value = node.get("quals", [])
    if not isinstance(value, list):
        return set()
    return {str(item).strip() for item in value if str(item).strip()}


def parse_type_node(node: Any) -> TypeNode:
    """Parse pointer, array and qualification nodes without flattening arrays to T*."""
    if not isinstance(node, dict):
        spelling = str(node or "void").strip()
        return TypeNode(CUseSite(spelling, spelling), CCategory.UNKNOWN)

    pointer_depth = 0
    array_shape: list[Optional[int]] = []
    const = False
    is_function_pointer = False
    current = node
    while isinstance(current, dict):
        const = const or "const" in _quals(current)
        kind = current.get("json_type")
        if kind == "ret_type":
            current = current.get("type")
            continue
        if kind == "pointer":
            pointer_depth += 1
            current = current.get("type")
            continue
        if kind == "array":
            dim = current.get("dim")
            try:
                array_shape.append(int(dim) if dim is not None else None)
            except (TypeError, ValueError):
                array_shape.append(None)
            # gen_json emits arrays both as {type: T, ...} and as {name: T, ...}.
            current = current.get("type") if "type" in current else {
                "name": current.get("name", "void"),
                "json_type": current.get("element_json_type", "lvgl_type"),
                "quals": current.get("quals", []),
            }
            continue
        if kind == "function_pointer":
            is_function_pointer = True
            name = str(current.get("name", "<anonymous callback>")).strip()
            break
        name = str(current.get("name", "void")).strip()
        if name:
            is_function_pointer = is_function_pointer or kind == "function_pointer"
            break
        nested = current.get("type")
        if nested is not None:
            current = nested
            continue
        name = "void"
        break

    if not isinstance(current, dict):
        name = str(current or "void").strip()
    spelling = ("const " if const else "") + name + (" *" * pointer_depth)
    if array_shape:
        spelling += " " + "".join(f"[{dim if dim is not None else ''}]" for dim in array_shape)
    hint = CCategory.FUNCTION_POINTER if is_function_pointer else CCategory.UNKNOWN
    return TypeNode(
        CUseSite(
            spelling=spelling,
            base_name=name,
            pointer_depth=pointer_depth,
            is_const=const,
            array_shape=tuple(array_shape),
            is_function_pointer=is_function_pointer,
        ),
        hint,
    )


class LVGLModel:
    """Indexed LVGL API facts; no object graph copy or persistent type inventory."""

    def __init__(self, data: dict[str, Any]):
        self.data = data
        self.functions = self._index(data.get("functions", []), "function")
        self.enums = self._index(data.get("enums", []), "enum")
        self.structures = self._index(data.get("structures", []), "struct")
        self.unions = self._index(data.get("unions", []), "union")
        self.forward_decls = self._index(data.get("forward_decls", []), "forward_decl")
        self.typedefs = self._index(data.get("typedefs", []), "typedef")
        self.function_pointers = self._index(data.get("function_pointers", []), "function_pointer")
        self.variables = self._index(data.get("variables", []), "variable")
        self.macros = self._index(data.get("macros", []), "macro")

    @staticmethod
    def _index(items: Any, kind: str) -> dict[str, dict[str, Any]]:
        if not isinstance(items, list):
            raise ValueError(f"lvgl.json field {kind}s must be an array")
        result: dict[str, dict[str, Any]] = {}
        for item in items:
            if isinstance(item, dict) and isinstance(item.get("name"), str):
                result[item["name"]] = item
        return result

    def declarations(self) -> dict[str, tuple[CCategory, dict[str, Any]]]:
        result: dict[str, tuple[CCategory, dict[str, Any]]] = {}
        for name, item in self.forward_decls.items():
            result[name] = (CCategory.STRUCT, item)
        for name, item in self.structures.items():
            result[name] = (CCategory.STRUCT, item)
        for name, item in self.unions.items():
            result[name] = (CCategory.UNION, item)
        for name, item in self.enums.items():
            result[name] = (CCategory.ENUM, item)
        for name, item in self.function_pointers.items():
            result[name] = (CCategory.FUNCTION_POINTER, item)
        for name, item in self.typedefs.items():
            result[name] = (CCategory.TYPEDEF, item)
        return result

    def resolve_declaration(self, name: str, seen: Optional[set[str]] = None) -> tuple[str, CCategory, list[str]]:
        """Follow typedef aliases to their canonical LVGL/primitive declaration."""
        seen = set() if seen is None else seen
        if name in seen:
            return name, CCategory.UNKNOWN, list(seen)
        seen.add(name)
        declarations = self.declarations()
        found = declarations.get(name)
        if found is None:
            category = CCategory.PRIMITIVE if name in BUILTIN_C_TYPES else CCategory.UNKNOWN
            return name, category, []
        category, item = found
        if category != CCategory.TYPEDEF:
            return name, category, []
        target_node = parse_type_node(item.get("type")).use_site
        canonical, target_category, chain = self.resolve_declaration(target_node.base_name, seen)
        return canonical, target_category, [name, *chain]

    def canonical_use_site(self, use: CUseSite) -> CUseSite:
        """Expand typedefs at a parsed use site while preserving pointer shape."""
        base_name = use.base_name
        pointer_depth = use.pointer_depth
        is_const = use.is_const
        array_shape = list(use.array_shape)
        is_function_pointer = use.is_function_pointer
        seen: set[str] = set()

        while base_name in self.typedefs and base_name not in seen:
            seen.add(base_name)
            target = parse_type_node(self.typedefs[base_name].get("type")).use_site
            base_name = target.base_name
            pointer_depth += target.pointer_depth
            is_const = is_const or target.is_const
            array_shape.extend(target.array_shape)
            is_function_pointer = is_function_pointer or target.is_function_pointer

        canonical_name, _, _ = self.resolve_declaration(base_name)
        canonical_spelling = ("const " if is_const else "") + canonical_name + (" *" * pointer_depth)
        if array_shape:
            canonical_spelling += " " + "".join(
                f"[{dimension if dimension is not None else ''}]" for dimension in array_shape
            )
        return CUseSite(
            spelling=canonical_spelling,
            base_name=canonical_name,
            pointer_depth=pointer_depth,
            is_const=is_const,
            array_shape=tuple(array_shape),
            is_function_pointer=is_function_pointer,
        )

    def check_instance_receiver(
        self,
        class_c_type: str,
        parameter_type: Any,
        base_c_types: tuple[str, ...] = (),
    ) -> "ReceiverCompatibility":
        """Check a C parameter against the pointer receiver implied by class.c_type.

        Class ``c_type`` names the native object type; an ordinary JS instance
        receiver is one pointer to that type. Top-level const qualification is
        intentionally ignored, matching the existing API-selector treatment.
        """
        class_name = str(class_c_type).strip()
        class_types = (class_name, *base_c_types)
        expected_bases = list(dict.fromkeys(self.resolve_declaration(name)[0] for name in class_types))
        expected_type = " or ".join(
            f"{base} *" if index == 0 else f"base {base} *"
            for index, base in enumerate(expected_bases)
        )
        parsed = parse_type_node(parameter_type).use_site
        actual = self.canonical_use_site(parsed)
        compatible = (
            actual.base_name in expected_bases
            and actual.pointer_depth == 1
            and not actual.array_shape
            and not actual.is_function_pointer
        )
        return ReceiverCompatibility(
            compatible=compatible,
            expected_type=expected_type,
            actual_type=actual.spelling,
        )

    def field_facts(self, name: str) -> list[FieldInfo]:
        item = self.structures.get(name)
        if item is None:
            return []
        fields = item.get("fields", [])
        if not isinstance(fields, list):
            return []
        result: list[FieldInfo] = []
        for field in fields:
            if not isinstance(field, dict) or not field.get("name"):
                continue
            raw_type = field.get("type")
            inline_aggregate = isinstance(raw_type, dict) and raw_type.get("json_type") in {"struct", "union"}
            node = parse_type_node(raw_type)
            result.append(
                FieldInfo(
                    name=str(field["name"]),
                    type_name=node.use_site.base_name,
                    bitsize=field.get("bitsize"),
                    array_shape=node.use_site.array_shape,
                    pointer_depth=node.use_site.pointer_depth,
                    is_function_pointer=node.use_site.is_function_pointer,
                    unsupported_reason="inline struct/union member" if inline_aggregate else "",
                )
            )
        return result


def normalize_c_type_node(node: Any) -> str:
    """Compatibility spelling for existing C emitters; preserves array shape."""
    use = parse_type_node(node).use_site
    pointer = " *" * use.pointer_depth
    return f"{'const ' if use.is_const else ''}{use.base_name}{pointer}" + (
        " " + "".join(f"[{dim if dim is not None else ''}]" for dim in use.array_shape)
        if use.array_shape
        else ""
    )


@dataclass(frozen=True)
class ReceiverCompatibility:
    compatible: bool
    expected_type: str
    actual_type: str
