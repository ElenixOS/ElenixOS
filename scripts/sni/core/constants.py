"""Index LVGL constants and macros without evaluating macro replacements."""

from __future__ import annotations

import re
from typing import Any

from .ir import ApiConstantIR, ApiMacroIR


_IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
_INTEGER_LITERAL = re.compile(r"^[+-]?(?:0[xX][0-9a-fA-F]+|0[bB][01]+|\d+)[uUlL]*$")
_FLOAT_LITERAL = re.compile(
    r"(?i)^[+-]?(?:(?:\d+\.\d*|\.\d+|\d+[eE][+-]?\d+)(?:[fFlL])?|"
    r"0[xX](?:[0-9a-f]+\.[0-9a-f]*|[0-9a-f]*\.[0-9a-f]+|[0-9a-f]+)[pP][+-]?\d+[fFlL]?)$"
)
_TOKEN = re.compile(
    r'\s*(0[xX](?:[0-9a-fA-F]+\.[0-9a-fA-F]*|[0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+)[pP][+-]?\d+[fFlL]?|'
    r'(?:\d+\.\d*|\.\d+|\d+[eE][+-]?\d+)[fFlL]*|'
    r'0[xX][0-9a-fA-F]+[uUlL]*|0[bB][01]+[uUlL]*|\d+[uUlL]*|'
    r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|'
    r'[A-Za-z_][A-Za-z0-9_]*|'
    r'<<|>>|<=|>=|==|!=|&&|\|\||##|[()+\-*/%~!&^|?:,<>#])'
)
_OPERATORS = {
    "(", ")", "+", "-", "*", "/", "%", "~", "!", "&", "^", "|", "?", ":", ",",
    "<", ">", "<=", ">=", "==", "!=", "&&", "||", "<<", ">>",
}
_C_TYPES = {
    "bool", "char", "short", "int", "long", "float", "double", "signed", "unsigned",
    "void", "size_t", "ptrdiff_t", "intptr_t", "uintptr_t", "int8_t", "uint8_t",
    "int16_t", "uint16_t", "int32_t", "uint32_t", "int64_t", "uint64_t",
}
_INTEGER_CONSTANT_MACROS = {
    "INT8_C", "UINT8_C", "INT16_C", "UINT16_C", "INT32_C", "UINT32_C",
    "INT64_C", "UINT64_C", "INTMAX_C", "UINTMAX_C",
}


def parse_numeric_constant(value_text: str) -> tuple[str, str] | None:
    """Parse enum values and literal metadata; macro replacements are never evaluated."""
    text = str(value_text).strip()
    if not text:
        return None
    while text.startswith("(") and text.endswith(")"):
        inner = text[1:-1].strip()
        if not inner:
            break
        text = inner

    if _INTEGER_LITERAL.fullmatch(text):
        digits = re.sub(r"[uUlL]+$", "", text)
        sign = -1 if digits.startswith("-") else 1
        unsigned_digits = digits.lstrip("+-")
        if unsigned_digits.lower().startswith("0x"):
            value = sign * int(unsigned_digits, 16)
            if 0 <= value <= 0xFFFFFFFF and value > 0x7FFFFFFF:
                value -= 0x100000000
            return "int", str(value)
        if unsigned_digits.lower().startswith("0b"):
            return "int", str(sign * int(unsigned_digits[2:], 2))
        return "int", str(int(digits, 10))
    if _FLOAT_LITERAL.fullmatch(text):
        return "float", text.rstrip("fFlL")
    return None


def _numeric_literal_kind(value_text: str) -> str | None:
    text = str(value_text).strip()
    if _INTEGER_LITERAL.fullmatch(text):
        return "int"
    if _FLOAT_LITERAL.fullmatch(text):
        return "float"
    return None


def parse_string_constant(value_text: str) -> str | None:
    """Return a C string literal prefix without interpreting its contents."""
    text = str(value_text).strip()
    match = re.match(r'^"([^"\\]|\\.)*"', text)
    return match.group(0) if match else None


def _strip_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\n]*", " ", text, flags=re.DOTALL).replace("\\\n", " ").strip()


def _tokenize(expression: str) -> list[str] | None:
    text = _strip_comments(expression)
    if not text or any(char in text for char in "{};`\\\""):
        return None
    tokens: list[str] = []
    cursor = 0
    while cursor < len(text):
        match = _TOKEN.match(text, cursor)
        if not match:
            if text[cursor:].strip():
                return None
            break
        token = match.group(1)
        if token in {"#", "##"}:
            return None
        tokens.append(token)
        cursor = match.end()
    return tokens or None


def _split_macro_arguments(tokens: list[str], open_index: int) -> tuple[list[list[str]], int] | None:
    """Split a function-like macro invocation, returning arguments and closing index."""
    depth = 1
    arguments: list[list[str]] = []
    current: list[str] = []
    index = open_index + 1
    while index < len(tokens):
        token = tokens[index]
        if token == "(":
            depth += 1
            current.append(token)
        elif token == ")":
            depth -= 1
            if depth == 0:
                if current or arguments:
                    arguments.append(current)
                return arguments, index
            current.append(token)
        elif token == "," and depth == 1:
            arguments.append(current)
            current = []
        else:
            current.append(token)
        index += 1
    return None


def _expand_function_macros(
    expression: str,
    macros: dict[str, dict[str, Any]],
    visiting: set[str] | None = None,
    depth: int = 0,
) -> str | None:
    """Expand only metadata-defined function macros for type analysis, never value evaluation."""
    if depth > 16:
        return None
    visiting = visiting or set()
    tokens = _tokenize(expression)
    if tokens is None:
        return None

    result: list[str] = []
    index = 0
    while index < len(tokens):
        name = tokens[index]
        macro = macros.get(name)
        params = macro.get("params") if macro else None
        if (
            macro is not None
            and params is not None
            and index + 1 < len(tokens)
            and tokens[index + 1] == "("
        ):
            if name in visiting or not isinstance(params, list) or not all(isinstance(p, str) for p in params):
                return None
            parsed = _split_macro_arguments(tokens, index + 1)
            if parsed is None:
                return None
            arguments, close_index = parsed
            if len(arguments) != len(params):
                return None
            initializer = macro.get("initializer")
            if initializer is None:
                return None
            replacement_tokens = _tokenize(str(initializer))
            if replacement_tokens is None:
                return None
            substitutions = {
                parameter: ["(", *argument, ")"]
                for parameter, argument in zip(params, arguments)
            }
            substituted: list[str] = []
            for replacement_token in replacement_tokens:
                substituted.extend(substitutions.get(replacement_token, [replacement_token]))
            nested = _expand_function_macros(
                " ".join(substituted), macros, visiting | {name}, depth + 1
            )
            if nested is None:
                return None
            result.extend(["(", nested, ")"])
            index = close_index + 1
            continue
        result.append(name)
        index += 1
    return " ".join(result)


def _type_names(lvgl_data: dict[str, Any]) -> set[str]:
    return {
        str(item.get("name"))
        for key in ("structures", "unions", "typedefs", "enums", "function_pointers", "forward_decls")
        for item in lvgl_data.get(key, [])
        if isinstance(item, dict) and item.get("name")
    } | _C_TYPES


def _enum_constants(lvgl_data: dict[str, Any], include_names: set[str] | None = None) -> dict[str, ApiConstantIR]:
    include_names = include_names or set()
    result: dict[str, ApiConstantIR] = {}
    for enum in lvgl_data.get("enums", []):
        if not isinstance(enum, dict):
            continue
        for member in enum.get("members", []):
            if not isinstance(member, dict):
                continue
            name = str(member.get("name", "")).strip()
            if not _IDENTIFIER.fullmatch(name):
                continue
            value = parse_numeric_constant(str(member.get("value", "")))
            if value is None and name not in include_names:
                continue
            result[name] = ApiConstantIR(
                name=name,
                value_kind="int",
                source_kind="enum",
                c_expression=name,
                source_name=name,
            )
    return result


def _expression_value_kind(
    expression: str,
    enums: dict[str, ApiConstantIR],
    macros: dict[str, dict[str, Any]],
    c_functions: set[str],
    type_names: set[str],
    cache: dict[str, str | None],
    visiting: set[str],
) -> str | None:
    text = _expand_function_macros(expression, macros)
    if text is None:
        return None
    tokens = _tokenize(text)
    if tokens is None:
        return None
    if re.search(r"\(\s*(?:(?:const|volatile)\s+)*(?:void|char|[A-Za-z_]\w*)\s*\*+\s*\)", text):
        return None
    if re.search(r"(?:^|[=(,])\s*&\s*[A-Za-z_]\w*", text):
        return None

    operand_kinds: list[str] = []
    comparison_operators = {"<", ">", "<=", ">=", "==", "!=", "&&", "||", "!"}
    for index, token in enumerate(tokens):
        if token in _OPERATORS:
            continue
        if _numeric_literal_kind(token):
            operand_kinds.append(_numeric_literal_kind(token) or "int")
            continue
        if token.startswith("'"):
            operand_kinds.append("int")
            continue
        if token.startswith('"'):
            return None
        if token in {"true", "false"} or token in _INTEGER_CONSTANT_MACROS:
            operand_kinds.append("int")
            continue
        if token in c_functions or token in type_names or token == "sizeof":
            return None
        if index + 1 < len(tokens) and tokens[index + 1] == "(":
            return None
        if token in macros:
            kind = _macro_value_kind(token, macros, enums, c_functions, type_names, cache, visiting)
            if kind not in {"int", "float"}:
                return None
            operand_kinds.append(kind)
        elif token in enums:
            operand_kinds.append("int")
        else:
            return None

    if not operand_kinds or any(operator in tokens for operator in {"?", ":", ","}):
        return None
    if "float" in operand_kinds and any(operator in tokens for operator in {"%", "~", "&", "^", "|", "<<", ">>"}):
        return None
    if any(operator in tokens for operator in comparison_operators):
        return "int"
    return "float" if "float" in operand_kinds else "int"


def _macro_value_kind(
    name: str,
    macros: dict[str, dict[str, Any]],
    enums: dict[str, ApiConstantIR],
    c_functions: set[str],
    type_names: set[str],
    cache: dict[str, str | None],
    visiting: set[str],
) -> str | None:
    if name in cache:
        return cache[name]
    macro = macros.get(name)
    if macro is None or macro.get("params") is not None or name in visiting:
        return None
    initializer = macro.get("initializer")
    if initializer is None:
        cache[name] = None
        return None

    expression = str(initializer).strip()
    if _numeric_literal_kind(expression):
        kind = _numeric_literal_kind(expression)
    elif parse_string_constant(expression):
        kind = "string"
    elif expression in {"true", "false"}:
        kind = "int"
    elif _IDENTIFIER.fullmatch(_strip_comments(expression)):
        target = _strip_comments(expression)
        if target in enums:
            kind = "int"
        elif target in macros:
            kind = _macro_value_kind(target, macros, enums, c_functions, type_names, cache, visiting | {name})
        else:
            kind = None
    else:
        kind = _expression_value_kind(
            expression, enums, macros, c_functions, type_names, cache, visiting | {name}
        )
    cache[name] = kind
    return kind


def build_constant_index(lvgl_data: dict[str, Any], include_names: set[str] | None = None) -> dict[str, ApiConstantIR]:
    """Index scalar macro symbols and enum constants without evaluating macro values."""
    include_names = include_names or set()
    enums = _enum_constants(lvgl_data, include_names)
    macros = {
        str(item.get("name", "")): item
        for item in lvgl_data.get("macros", [])
        if isinstance(item, dict) and item.get("name")
    }
    c_functions = {
        str(item.get("name", "")) for item in lvgl_data.get("functions", []) if isinstance(item, dict)
    }
    type_names = _type_names(lvgl_data)
    cache: dict[str, str | None] = {}
    constants: dict[str, ApiConstantIR] = {}
    for name, macro in macros.items():
        if not _IDENTIFIER.fullmatch(name):
            continue
        kind = _macro_value_kind(name, macros, enums, c_functions, type_names, cache, set())
        constants[name] = ApiConstantIR(
            name=name,
            value_kind=kind,
            source_kind="macro",
            c_expression=name if kind else None,
            source_name=name,
            availability_guard=name,
            initializer=None if macro.get("initializer") is None else str(macro.get("initializer")),
            parameters=None if macro.get("params") is None else tuple(str(param) for param in macro.get("params", [])),
        )

    # Enum members retain precedence for legacy root aliases, while macros remain in
    # the separate catalog regardless of whether they have a scalar runtime value.
    constants.update(enums)
    return constants


def build_macro_catalog(lvgl_data: dict[str, Any]) -> list[ApiMacroIR]:
    """Return every macro definition, preserving its raw replacement and parameters."""
    value_index = build_constant_index(lvgl_data)
    result: list[ApiMacroIR] = []
    for item in lvgl_data.get("macros", []):
        if not isinstance(item, dict):
            continue
        name = str(item.get("name", "")).strip()
        if not _IDENTIFIER.fullmatch(name):
            continue
        params = item.get("params")
        result.append(
            ApiMacroIR(
                name=name,
                parameters=None if params is None else tuple(str(param) for param in params),
                initializer=None if item.get("initializer") is None else str(item.get("initializer")),
                value_kind=value_index[name].value_kind if name in value_index else None,
            )
        )
    return sorted(result, key=lambda macro: macro.name)


def resolve_macro_constant(lvgl_data: dict[str, Any], name: str) -> ApiConstantIR | None:
    """Resolve a macro symbol while retaining its original C replacement text."""
    return build_constant_index(lvgl_data).get(name)


def resolve_constant(lvgl_data: dict[str, Any], name: str) -> ApiConstantIR | None:
    return build_constant_index(lvgl_data, {name}).get(name)
