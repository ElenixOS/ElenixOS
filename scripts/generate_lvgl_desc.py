#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate LVGL SNI class descriptor C source from JSON metadata."""

from __future__ import annotations

import argparse
import datetime
import fnmatch
import json
import re
import sys
import unicodedata
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Set, Tuple


HEADER_TEXT = """/**
 * @file sni_api_lv.c
 * @brief LVGL API 导出
 * @author Auto Generated
 * @date {date}
 */

#include "sni_api_lv.h"

#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "sni_api_export.h"
#include "elena_os_log.h"

#define LV_API_NAME "lv"

static jerry_value_t lv_api_obj;
"""


VERBOSE = False


@dataclass
class TypeBridge:
    c_type: str
    js_check: str
    js2c_mode: str
    js2c_expr: Optional[str]
    sni_type: Optional[str]
    c2js_mode: str
    c2js_expr: Optional[str]


@dataclass
class FuncArg:
    name: str
    c_type: str
    bridge: TypeBridge


@dataclass
class ApiFunction:
    name: str
    return_type: str
    return_bridge: TypeBridge
    args: List[FuncArg]


@dataclass
class ApiConstant:
    name: str
    kind: str
    value: str


@dataclass
class FilterStats:
    total: int = 0
    blacklisted: int = 0
    whitelist_miss: int = 0
    included: int = 0
    skipped_by_user: int = 0


class SkipFunctionError(Exception):
    def __init__(self, func_name: str, reason: str):
        super().__init__(reason)
        self.func_name = func_name
        self.reason = reason


def eprint(msg: str) -> None:
    print(msg, file=sys.stderr)


def vlog(msg: str) -> None:
    if VERBOSE:
        print(msg)


def velog(msg: str) -> None:
    if VERBOSE:
        print(msg, file=sys.stderr)


def load_json(path: Path, required: Optional[List[str]] = None) -> Dict[str, Any]:
    if not path.exists():
        raise SystemExit(f"[错误] 文件不存在: {path}")

    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise SystemExit(f"[错误] JSON 解析失败: {path}: {exc}") from exc

    if required:
        for key in required:
            if key not in data:
                raise SystemExit(f"[错误] JSON 缺少关键字段 '{key}': {path}")
    return data


def normalize_c_type(type_info: Optional[Dict[str, Any]]) -> str:
    if not type_info:
        return "void"

    jt = type_info.get("json_type")

    if jt == "ret_type":
        return normalize_c_type(type_info.get("type"))

    if jt == "pointer":
        base = normalize_c_type(type_info.get("type"))
        quals = " ".join(type_info.get("quals", []))
        base_part = f"{quals} {base}".strip()
        return f"{base_part}*"

    if jt == "array":
        base = normalize_c_type(type_info.get("type"))
        return f"{base}*"

    if "name" in type_info:
        quals = " ".join(type_info.get("quals", []))
        type_name = str(type_info["name"]).strip()
        return f"{quals} {type_name}".strip()

    return "void"


def normalize_type_key(c_type: str) -> str:
    text = c_type.replace("const", " ")
    text = re.sub(r"\s+", " ", text).strip()
    text = text.replace(" *", "*").replace("* ", "*")
    return text


def c_type_to_base_name(c_type: str) -> str:
    base = normalize_type_key(c_type).replace("*", "")
    if base.endswith("_t"):
        base = base[:-2]
    return base.upper()


def parse_api_filters(api_table: Dict[str, Any]) -> Dict[str, Dict[str, List[str]]]:
    result = {
        "function": {"whitelist": [], "blacklist": []},
        "constant": {"whitelist": [], "blacklist": []},
    }

    scan = api_table.get("scan", {}) if isinstance(api_table.get("scan", {}), dict) else {}
    for group in ("function", "constant"):
        group_obj = scan.get(group, {})
        if not isinstance(group_obj, dict):
            raise SystemExit(f"[错误] api-table 字段 scan.{group} 必须是对象")
        for key in ("whitelist", "blacklist"):
            value = group_obj.get(key, [])
            if not isinstance(value, list):
                raise SystemExit(f"[错误] api-table 字段 scan.{group}.{key} 必须是数组")
            result[group][key] = [str(item).strip() for item in value if str(item).strip()]
    return result


@dataclass
class ApiClass:
    name: str
    c_type: str
    constructor: Optional[str]
    base: Optional[str]
    methods: List[str]
    static_methods: List[str]
    constants: List[str]


@dataclass
class ApiProperty:
    name: str
    getter: Optional[str]
    setter: Optional[str]


def parse_api_classes(api_table: Dict[str, Any]) -> Dict[str, ApiClass]:
    classes_raw = api_table.get("classes")
    if not isinstance(classes_raw, dict) or not classes_raw:
        raise SystemExit("[错误] api-table 缺少 classes 定义")

    classes: Dict[str, ApiClass] = {}
    for class_name, cfg in classes_raw.items():
        if not isinstance(cfg, dict):
            raise SystemExit(f"[错误] classes.{class_name} 必须是对象")

        c_type = str(cfg.get("c_type", "")).strip()
        ctor_val = cfg.get("constructor", None)
        constructor = None if ctor_val is None else str(ctor_val).strip()
        base_val = cfg.get("base")
        base = None if base_val is None else str(base_val).strip()
        methods = cfg.get("methods", [])
        static_methods = cfg.get("static_methods", [])
        constants = cfg.get("constants", [])

        if not c_type:
            raise SystemExit(f"[错误] classes.{class_name}.c_type 不能为空")
        if constructor == "":
            constructor = None
        if not isinstance(methods, list) or not isinstance(static_methods, list) or not isinstance(constants, list):
            raise SystemExit(f"[错误] classes.{class_name} 中 methods/static_methods/constants 必须为数组")

        methods_norm = [str(item).strip() for item in methods if str(item).strip()]
        static_methods_norm = [str(item).strip() for item in static_methods if str(item).strip()]
        constants_norm = [str(item).strip() for item in constants if str(item).strip()]

        if constructor is None:
            if methods_norm:
                raise SystemExit(f"[错误] classes.{class_name} 为静态类(constructor=null)，methods 必须为空")
            if not static_methods_norm:
                raise SystemExit(f"[错误] classes.{class_name} 为静态类(constructor=null)，static_methods 不能为空")

        classes[class_name] = ApiClass(
            name=class_name,
            c_type=c_type,
            constructor=constructor,
            base=base if base else None,
            methods=methods_norm,
            static_methods=static_methods_norm,
            constants=constants_norm,
        )

    for class_name, cls in classes.items():
        if cls.base and cls.base not in classes:
            raise SystemExit(f"[错误] classes.{class_name}.base 指向不存在的类: {cls.base}")

    temp_mark: Set[str] = set()
    perm_mark: Set[str] = set()

    def visit(name: str) -> None:
        if name in perm_mark:
            return
        if name in temp_mark:
            raise SystemExit(f"[错误] classes 存在循环继承: {name}")

        temp_mark.add(name)
        parent = classes[name].base
        if parent:
            visit(parent)
        temp_mark.remove(name)
        perm_mark.add(name)

    for name in classes.keys():
        visit(name)

    return classes


def is_matched(name: str, patterns: List[str]) -> bool:
    for pattern in patterns:
        if fnmatch.fnmatchcase(name, pattern):
            return True
    return False


def include_by_filter(
    name: str,
    whitelist: List[str],
    blacklist: List[str],
    stats: Optional[FilterStats] = None,
) -> bool:
    if stats is not None:
        stats.total += 1
    if blacklist and is_matched(name, blacklist):
        if stats is not None:
            stats.blacklisted += 1
        return False
    if whitelist and not is_matched(name, whitelist):
        if stats is not None:
            stats.whitelist_miss += 1
        return False
    if stats is not None:
        stats.included += 1
    return True


def parse_numeric_constant(value_text: str) -> Optional[Tuple[str, str]]:
    text = str(value_text).strip()
    if not text:
        return None

    # 去掉一层外层括号
    while text.startswith("(") and text.endswith(")"):
        inner = text[1:-1].strip()
        if not inner:
            break
        text = inner

    text = text.rstrip("UuLlFf")

    if re.fullmatch(r"[-+]?0x[0-9a-fA-F]+", text):
        return ("int", str(int(text, 16)))
    if re.fullmatch(r"[-+]?\d+", text):
        return ("int", str(int(text, 10)))
    if re.fullmatch(r"[-+]?\d+\.\d*([eE][-+]?\d+)?", text) or re.fullmatch(r"[-+]?\d+[eE][-+]?\d+", text):
        return ("float", text)
    return None


def parse_string_constant(value_text: str) -> Optional[str]:
    text = str(value_text).strip()
    if not text:
        return None

    if text.startswith('"') and text.endswith('"'):
        return text
    return None


def collect_constants(
    lvgl_data: Dict[str, Any],
    filters: Dict[str, List[str]],
    stats: FilterStats,
) -> List[ApiConstant]:
    constants: Dict[str, ApiConstant] = {}

    whitelist = filters.get("whitelist", [])
    blacklist = filters.get("blacklist", [])

    for macro in lvgl_data.get("macros", []):
        name = str(macro.get("name", "")).strip()
        if not name:
            continue
        if macro.get("params"):
            continue
        if not include_by_filter(name, whitelist, blacklist, stats):
            continue

        init = macro.get("initializer")
        if init is None:
            continue

        value_text = str(init).strip()
        number = parse_numeric_constant(value_text)
        if number:
            kind, val = number
            constants[name] = ApiConstant(name=name, kind=kind, value=val)
            continue

        string_value = parse_string_constant(value_text)
        if string_value:
            constants[name] = ApiConstant(name=name, kind="string", value=string_value)

    for enum in lvgl_data.get("enums", []):
        members = enum.get("members", [])
        for member in members:
            name = str(member.get("name", "")).strip()
            if not name:
                continue
            if not include_by_filter(name, whitelist, blacklist, stats):
                continue

            value_text = member.get("value")
            number = parse_numeric_constant(str(value_text))
            if number:
                kind, val = number
                constants[name] = ApiConstant(name=name, kind=kind, value=val)

    return [constants[name] for name in sorted(constants.keys())]


def build_constant_index(lvgl_data: Dict[str, Any]) -> Dict[str, ApiConstant]:
    constants: Dict[str, ApiConstant] = {}

    for macro in lvgl_data.get("macros", []):
        name = str(macro.get("name", "")).strip()
        if not name or macro.get("params"):
            continue
        init = macro.get("initializer")
        if init is None:
            continue

        value_text = str(init).strip()
        number = parse_numeric_constant(value_text)
        if number:
            kind, val = number
            constants[name] = ApiConstant(name=name, kind=kind, value=val)
            continue

        string_value = parse_string_constant(value_text)
        if string_value:
            constants[name] = ApiConstant(name=name, kind="string", value=string_value)

    for enum in lvgl_data.get("enums", []):
        for member in enum.get("members", []):
            name = str(member.get("name", "")).strip()
            if not name:
                continue
            value_text = member.get("value")
            number = parse_numeric_constant(str(value_text))
            if number:
                kind, val = number
                constants[name] = ApiConstant(name=name, kind=kind, value=val)

    return constants


def topo_sort_classes(classes: Dict[str, ApiClass]) -> List[ApiClass]:
    ordered: List[ApiClass] = []
    temp_mark: Set[str] = set()
    perm_mark: Set[str] = set()

    def visit(name: str) -> None:
        if name in perm_mark:
            return
        if name in temp_mark:
            raise SystemExit(f"[错误] classes 存在循环继承: {name}")
        temp_mark.add(name)
        base = classes[name].base
        if base:
            visit(base)
        temp_mark.remove(name)
        perm_mark.add(name)
        ordered.append(classes[name])

    for name in classes.keys():
        visit(name)

    return ordered


def build_function_index(lvgl_data: Dict[str, Any]) -> Dict[str, Dict[str, Any]]:
    result: Dict[str, Dict[str, Any]] = {}
    for item in lvgl_data.get("functions", []):
        name = str(item.get("name", "")).strip()
        if not name:
            continue
        result[name] = item
    return result


def require_function(function_index: Dict[str, Dict[str, Any]], name: str, context: str) -> Dict[str, Any]:
    if name not in function_index:
        raise SystemExit(f"[错误] 找不到函数 {name} ({context})")
    return function_index[name]


def require_class_function(
    function_index: Dict[str, Dict[str, Any]],
    class_name: str,
    name: str,
    context: str,
) -> Dict[str, Any]:
    normalized = str(name).strip()
    candidates: List[str] = [normalized]
    if normalized and not normalized.startswith("lv_"):
        candidates.insert(0, f"lv_{class_name}_{normalized}")
        candidates.append(f"lv_{normalized}")

    seen: Set[str] = set()
    dedup_candidates: List[str] = []
    for candidate in candidates:
        if candidate and candidate not in seen:
            dedup_candidates.append(candidate)
            seen.add(candidate)

    for candidate in dedup_candidates:
        if candidate in function_index:
            return function_index[candidate]

    raise SystemExit(
        f"[错误] 找不到函数 {name} ({context})，已尝试: {', '.join(dedup_candidates)}"
    )


def sanitize_ident(text: str) -> str:
    return re.sub(r"[^a-zA-Z0-9_]", "_", text)


def discover_class_properties(
    cls: ApiClass,
    function_index: Dict[str, Dict[str, Any]],
    filters: Dict[str, Dict[str, List[str]]],
) -> List[ApiProperty]:
    if cls.constructor is None:
        return []

    func_filter = filters.get("function", {})
    blacklist = func_filter.get("blacklist", [])

    get_prefix = f"lv_{cls.name}_get_"
    set_prefix = f"lv_{cls.name}_set_"
    props: Dict[str, ApiProperty] = {}

    for func_name in sorted(function_index.keys()):
        if blacklist and is_matched(func_name, blacklist):
            continue

        if func_name.startswith(get_prefix):
            prop_name = func_name[len(get_prefix):]
            if prop_name:
                existing = props.get(prop_name)
                if existing:
                    existing.getter = func_name
                else:
                    props[prop_name] = ApiProperty(name=prop_name, getter=func_name, setter=None)
            continue

        if func_name.startswith(set_prefix):
            prop_name = func_name[len(set_prefix):]
            if prop_name:
                existing = props.get(prop_name)
                if existing:
                    existing.setter = func_name
                else:
                    props[prop_name] = ApiProperty(name=prop_name, getter=None, setter=func_name)

    return [props[name] for name in sorted(props.keys())]


def build_api_function(
    item: Dict[str, Any],
    lv_type_entries: Dict[str, str],
    pending_updates: Dict[str, str],
    context: str,
) -> ApiFunction:
    name = str(item.get("name", "")).strip()
    if not name:
        raise SystemExit(f"[错误] {context} 缺少函数名")

    ret_type = normalize_c_type(item.get("type"))
    ret_bridge = build_bridge_from_type(ret_type, lv_type_entries, pending_updates, True, f"{context} 返回值", name)

    args_raw = normalize_args(item.get("args", []))
    args: List[FuncArg] = []
    for idx, arg in enumerate(args_raw):
        arg_name = str(arg.get("name") or f"arg{idx}")
        arg_type = normalize_c_type(arg.get("type"))
        arg_bridge = build_bridge_from_type(
            arg_type,
            lv_type_entries,
            pending_updates,
            True,
            f"{context} 参数 #{idx} ({arg_name})",
            name,
        )
        args.append(FuncArg(name=arg_name, c_type=arg_type, bridge=arg_bridge))

    return ApiFunction(name=name, return_type=ret_type, return_bridge=ret_bridge, args=args)


def render_arg_conversion(lines: List[str], arg_expr: str, arg: FuncArg, c_var_name: str, allow_null: bool = False) -> None:
    # Nullable pointer handle types (SNI_H_* or SNI_T_PTR) accept JS null → C NULL
    is_nullable_ptr = (
        allow_null
        and arg.bridge.js2c_mode == "bridge"
        and arg.bridge.sni_type is not None
        and (arg.bridge.sni_type.startswith("SNI_H_") or arg.bridge.sni_type == "SNI_T_PTR")
    )
    if is_nullable_ptr:
        lines.append(f"    {arg.c_type} {c_var_name};")
        lines.append(f"    if (jerry_value_is_null({arg_expr}))")
        lines.append("    {")
        lines.append(f"        {c_var_name} = NULL;")
        lines.append("    }")
        lines.append(f"    else if (jerry_value_is_object({arg_expr}))")
        lines.append("    {")
        lines.append(f"        if (!sni_tb_js2c({arg_expr}, {arg.bridge.sni_type}, &{c_var_name}))")
        lines.append("        {")
        lines.append('            return sni_api_throw_error("Failed to convert argument");')
        lines.append("        }")
        lines.append("    }")
        lines.append("    else")
        lines.append("    {")
        lines.append('        return sni_api_throw_error("Invalid argument type");')
        lines.append("    }")
        return

    check = arg.bridge.js_check
    if check:
        lines.append(f"    if (!{check}({arg_expr}))")
        lines.append("    {")
        lines.append('        return sni_api_throw_error("Invalid argument type");')
        lines.append("    }")

    lines.append(f"    {arg.c_type} {c_var_name};")

    if arg.bridge.js2c_mode == "macro" and arg.bridge.js2c_expr:
        lines.append(f"    {c_var_name} = {arg.bridge.js2c_expr}({arg_expr});")
    elif arg.bridge.js2c_mode == "string_fn" and arg.bridge.js2c_expr:
        lines.append(f"    {c_var_name} = {arg.bridge.js2c_expr}({arg_expr});")
    elif arg.bridge.js2c_mode == "bridge" and arg.bridge.sni_type:
        lines.append(f"    if (!sni_tb_js2c({arg_expr}, {arg.bridge.sni_type}, &{c_var_name}))")
        lines.append("    {")
        lines.append('        return sni_api_throw_error("Failed to convert argument");')
        lines.append("    }")
    else:
        lines.append('    return sni_api_throw_error("Unsupported argument conversion");')


def render_return_conversion(lines: List[str], func: ApiFunction, result_var: str) -> None:
    if func.return_bridge.c2js_mode == "void":
        lines.append("    return jerry_undefined();")
        return

    if func.return_bridge.c2js_mode == "macro" and func.return_bridge.c2js_expr:
        lines.append(f"    return {func.return_bridge.c2js_expr}({result_var});")
        return

    if func.return_bridge.c2js_mode == "string" and func.return_bridge.c2js_expr:
        lines.append(f"    return {func.return_bridge.c2js_expr}({result_var});")
        return

    if func.return_bridge.c2js_mode == "bridge" and func.return_bridge.sni_type:
        lines.append(f"    return sni_tb_c2js(&{result_var}, {func.return_bridge.sni_type});")
        return

    lines.append('    return sni_api_throw_error("Unsupported return conversion");')


def render_constructor_wrapper(cls: ApiClass, ctor_func: ApiFunction) -> str:
    wrapper_name = f"sni_api_ctor_{sanitize_ident(cls.name)}"
    lines: List[str] = []

    lines.append(f"jerry_value_t {wrapper_name}(const jerry_call_info_t *call_info_p,")
    lines.append("                                const jerry_value_t args_p[],")
    lines.append("                                const jerry_length_t args_count)")
    lines.append("{")
    lines.append("    if (jerry_value_is_undefined(call_info_p->new_target))")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Constructor must be called with new");')
    lines.append("    }")
    lines.append("")
    lines.append(f"    if (args_count != {len(ctor_func.args)})")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Invalid argument count");')
    lines.append("    }")
    lines.append("")

    call_args: List[str] = []
    for idx, arg in enumerate(ctor_func.args):
        c_var = f"arg_{arg.name}"
        render_arg_conversion(lines, f"args_p[{idx}]", arg, c_var, allow_null=True)
        call_args.append(c_var)
        lines.append("")

    call_text = ", ".join(call_args)
    if ctor_func.return_bridge.sni_type is None or ctor_func.return_bridge.c2js_mode != "bridge":
        lines.append('    return sni_api_throw_error("Constructor return type must be object handle/value");')
        lines.append("}")
        return "\n".join(lines)

    lines.append(f"    {ctor_func.return_type} native_obj = {ctor_func.name}({call_text});" if call_text else f"    {ctor_func.return_type} native_obj = {ctor_func.name}();")
    lines.append(f"    if (!sni_tb_c2js_set_object(&native_obj, {ctor_func.return_bridge.sni_type}, call_info_p->this_value))")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Failed to bind native object");')
    lines.append("    }")
    lines.append("    return jerry_undefined();")
    lines.append("}")
    return "\n".join(lines)


def render_method_wrapper(cls: ApiClass, func: ApiFunction) -> str:
    wrapper_name = f"sni_api_{func.name}"
    lines: List[str] = []

    if not func.args:
        raise SystemExit(f"[错误] 实例方法 {func.name} 缺少 this 参数")

    this_arg = func.args[0]
    js_arg_count = len(func.args) - 1

    lines.append(f"jerry_value_t {wrapper_name}(const jerry_call_info_t *call_info_p,")
    lines.append("                                const jerry_value_t args_p[],")
    lines.append("                                const jerry_length_t args_count)")
    lines.append("{")
    lines.append(f"    if (args_count != {js_arg_count})")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Invalid argument count");')
    lines.append("    }")
    lines.append("")

    render_arg_conversion(lines, "call_info_p->this_value", this_arg, "self_obj")
    lines.append("")

    call_args: List[str] = ["self_obj"]
    for idx, arg in enumerate(func.args[1:]):
        c_var = f"arg_{arg.name}"
        render_arg_conversion(lines, f"args_p[{idx}]", arg, c_var)
        call_args.append(c_var)
        lines.append("")

    call_text = ", ".join(call_args)
    if func.return_bridge.c2js_mode == "void":
        lines.append(f"    {func.name}({call_text});")
        lines.append("    return jerry_undefined();")
        lines.append("}")
        return "\n".join(lines)

    lines.append(f"    {func.return_type} result = {func.name}({call_text});")
    render_return_conversion(lines, func, "result")
    lines.append("}")
    return "\n".join(lines)


def render_static_wrapper(func: ApiFunction) -> str:
    wrapper_name = f"sni_api_{func.name}"
    lines: List[str] = []

    lines.append(f"jerry_value_t {wrapper_name}(const jerry_call_info_t *call_info_p,")
    lines.append("                                const jerry_value_t args_p[],")
    lines.append("                                const jerry_length_t args_count)")
    lines.append("{")
    lines.append("    (void)call_info_p;")
    lines.append("")
    lines.append(f"    if (args_count != {len(func.args)})")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Invalid argument count");')
    lines.append("    }")
    lines.append("")

    call_args: List[str] = []
    for idx, arg in enumerate(func.args):
        c_var = f"arg_{arg.name}"
        render_arg_conversion(lines, f"args_p[{idx}]", arg, c_var)
        call_args.append(c_var)
        lines.append("")

    call_text = ", ".join(call_args)
    if func.return_bridge.c2js_mode == "void":
        lines.append(f"    {func.name}({call_text});" if call_text else f"    {func.name}();")
        lines.append("    return jerry_undefined();")
        lines.append("}")
        return "\n".join(lines)

    lines.append(f"    {func.return_type} result = {func.name}({call_text});" if call_text else f"    {func.return_type} result = {func.name}();")
    render_return_conversion(lines, func, "result")
    lines.append("}")
    return "\n".join(lines)


def render_property_getter_wrapper(cls: ApiClass, prop: ApiProperty, func: ApiFunction) -> str:
    wrapper_name = f"sni_api_prop_get_{sanitize_ident(cls.name)}_{sanitize_ident(prop.name)}"
    lines: List[str] = []

    if len(func.args) != 1:
        raise SystemExit(f"[错误] 属性 getter {func.name} 必须只有 this 参数")

    this_arg = func.args[0]

    lines.append(f"jerry_value_t {wrapper_name}(const jerry_call_info_t *call_info_p,")
    lines.append("                                const jerry_value_t args_p[],")
    lines.append("                                const jerry_length_t args_count)")
    lines.append("{")
    lines.append("    (void)args_p;")
    lines.append("    if (args_count != 0)")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Invalid argument count");')
    lines.append("    }")
    lines.append("")

    render_arg_conversion(lines, "call_info_p->this_value", this_arg, "self_obj")
    lines.append("")

    if func.return_bridge.c2js_mode == "void":
        raise SystemExit(f"[错误] 属性 getter {func.name} 不能返回 void")

    lines.append(f"    {func.return_type} result = {func.name}(self_obj);")
    render_return_conversion(lines, func, "result")
    lines.append("}")
    return "\n".join(lines)


def render_property_setter_wrapper(cls: ApiClass, prop: ApiProperty, func: ApiFunction) -> str:
    wrapper_name = f"sni_api_prop_set_{sanitize_ident(cls.name)}_{sanitize_ident(prop.name)}"
    lines: List[str] = []

    if len(func.args) != 2:
        raise SystemExit(f"[错误] 属性 setter {func.name} 必须是 this + value 两个参数")

    this_arg = func.args[0]
    value_arg = func.args[1]

    lines.append(f"jerry_value_t {wrapper_name}(const jerry_call_info_t *call_info_p,")
    lines.append("                                const jerry_value_t args_p[],")
    lines.append("                                const jerry_length_t args_count)")
    lines.append("{")
    lines.append("    if (args_count != 1)")
    lines.append("    {")
    lines.append('        return sni_api_throw_error("Invalid argument count");')
    lines.append("    }")
    lines.append("")

    render_arg_conversion(lines, "call_info_p->this_value", this_arg, "self_obj")
    lines.append("")
    render_arg_conversion(lines, "args_p[0]", value_arg, "prop_value")
    lines.append("")
    lines.append(f"    {func.name}(self_obj, prop_value);")
    lines.append("    return jerry_undefined();")
    lines.append("}")
    return "\n".join(lines)


def render_method_array(name: str, items: List[Tuple[str, str]]) -> str:
    lines = [f"const sni_method_desc_t {name}[] = {{"]
    for js_name, handler in items:
        lines.append(f"    {{.name = \"{js_name}\", .handler = {handler}}},")
    lines.append("    {.name = NULL, .handler = NULL},")
    lines.append("};")
    return "\n".join(lines)


def render_property_array(name: str, items: List[Tuple[str, Optional[str], Optional[str]]]) -> str:
    lines = [f"const sni_property_desc_t {name}[] = {{"]
    for js_name, getter, setter in items:
        getter_expr = getter if getter else "NULL"
        setter_expr = setter if setter else "NULL"
        lines.append(f"    {{.name = \"{js_name}\", .getter = {getter_expr}, .setter = {setter_expr}}},")
    lines.append("    {.name = NULL, .getter = NULL, .setter = NULL},")
    lines.append("};")
    return "\n".join(lines)


def render_constant_array(name: str, items: List[ApiConstant]) -> str:
    lines = [f"const sni_constant_desc_t {name}[] = {{"]
    for item in items:
        if item.kind == "int":
            lines.append(
                "    {"
                f".name = \"{item.name}\", .type = SNI_CONST_INT, .value.i = {item.value}"
                "},"
            )
        elif item.kind == "float":
            lines.append(
                "    {"
                f".name = \"{item.name}\", .type = SNI_CONST_FLOAT, .value.f = {item.value}"
                "},"
            )
        elif item.kind == "string":
            lines.append(
                "    {"
                f".name = \"{item.name}\", .type = SNI_CONST_STRING, .value.s = {item.value}"
                "},"
            )
    lines.append("    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},")
    lines.append("};")
    return "\n".join(lines)


def render_class_block(
    classes: List[ApiClass],
    constructor_wrapper_names: Dict[str, str],
    method_array_names: Dict[str, str],
    property_array_names: Dict[str, str],
    static_method_array_names: Dict[str, str],
    constant_array_names: Dict[str, str],
) -> str:
    lines: List[str] = []

    for cls in classes:
        class_var = f"lv_class_desc_{sanitize_ident(cls.name)}"
        base_expr = "NULL"
        if cls.base:
            base_expr = f"&lv_class_desc_{sanitize_ident(cls.base)}"

        lines.append(f"const sni_class_desc_t {class_var} = {{")
        lines.append(f"    .name = \"{cls.name}\",")
        is_static = cls.constructor is None
        lines.append(f"    .constructor = {constructor_wrapper_names[cls.name]},")
        lines.append(f"    .base_class = {'NULL' if is_static else base_expr},")
        methods_expr = "NULL" if is_static else method_array_names[cls.name]
        properties_expr = "NULL" if is_static else property_array_names[cls.name]
        lines.append(f"    .methods = {methods_expr},")
        lines.append(f"    .properties = {properties_expr},")
        lines.append(f"    .static_methods = {static_method_array_names[cls.name]},")
        lines.append(f"    .constants = {constant_array_names[cls.name]},")
        lines.append("};")
        lines.append("")

    lines.append("const sni_class_desc_t *const lv_api_classes[] = {")
    for cls in classes:
        lines.append(f"    &lv_class_desc_{sanitize_ident(cls.name)},")
    lines.append("    NULL,")
    lines.append("};")

    init_mount = """
void sni_api_lv_init(void)
{
    lv_api_obj = sni_api_build(lv_api_classes);
}

void sni_api_lv_mount(jerry_value_t realm)
{
    bool result = sni_api_mount(realm, lv_api_obj, LV_API_NAME);
    if (!result)
    {
        EOS_LOG_E("Failed to mount LVGL API");
    }
}
""".strip("\n")

    lines.append("")
    lines.append(init_mount)

    return "\n".join(lines)


def make_default_type_map() -> Dict[str, str]:
    return {
        "bool": "SNI_T_BOOL",
        "char*": "SNI_T_STRING",
        "char const*": "SNI_T_STRING",
        "const char*": "SNI_T_STRING",
        "double": "SNI_T_DOUBLE",
        "float": "SNI_T_DOUBLE",
        "int": "SNI_T_INT32",
        "int16_t": "SNI_T_INT32",
        "int32_t": "SNI_T_INT32",
        "int64_t": "SNI_T_INT32",
        "int8_t": "SNI_T_INT32",
        "long": "SNI_T_INT32",
        "short": "SNI_T_INT32",
        "size_t": "SNI_T_UINT32",
        "uint16_t": "SNI_T_UINT32",
        "uint32_t": "SNI_T_UINT32",
        "uint64_t": "SNI_T_UINT32",
        "uint8_t": "SNI_T_UINT32",
        "unsigned": "SNI_T_UINT32",
        "unsigned int": "SNI_T_UINT32",
        "unsigned long": "SNI_T_UINT32",
        "void*": "SNI_T_PTR",
    }


def load_type_entries(lv_types_data: Dict[str, Any]) -> Dict[str, str]:
    entries = lv_types_data.get("types")
    if not isinstance(entries, list):
        raise SystemExit("[错误] lv-types JSON 缺少 'types' 数组")

    result: Dict[str, str] = {}
    for item in entries:
        if not isinstance(item, dict):
            continue
        name = str(item.get("name", "")).strip()
        if not name:
            continue
        object_type = item.get("object_type")
        if object_type is None:
            result[name] = ""
        else:
            result[name] = str(object_type).strip()
    return result


def prompt_object_type(type_name: str, context: str) -> str:
    while True:
        print(f"\n[输入] 类型未定义: {type_name}")
        print(f"[位置] {context}")
        print("[选项] 1) handle_object")
        print("[选项] 2) value_object")
        print("[选项] 3) primitive(<type>) 例如 primitive(int)")
        print("[选项] 4) blacklist 当前函数并跳过")
        value = input("请输入选项编号或直接输入 object_type: ").strip()

        if value == "1" or value == "handle_object":
            return "handle_object"
        if value == "2" or value == "value_object":
            return "value_object"
        if value == "4" or value.lower() in {"blacklist", "b"}:
            return "__BLACKLIST_FUNCTION__"
        if value == "3":
            typed = input("请输入 primitive(<type>)：").strip()
            if re.fullmatch(r"primitive\([^()]+\)", typed):
                return typed
            print("[提示] primitive 输入格式错误，请重试。")
            continue
        if re.fullmatch(r"primitive\([^()]+\)", value):
            return value
        print("[提示] 非法输入，请重试。")


def update_lv_types_output(output_path: Path, updates: Dict[str, str]) -> None:
    if not updates:
        return

    if output_path.exists():
        try:
            data = json.loads(output_path.read_text(encoding="utf-8"))
        except json.JSONDecodeError as exc:
            raise SystemExit(f"[错误] 无法解析 {output_path}: {exc}") from exc
    else:
        data = {"types": []}

    if "types" not in data or not isinstance(data["types"], list):
        data["types"] = []

    index: Dict[str, Dict[str, Any]] = {}
    for item in data["types"]:
        if isinstance(item, dict) and item.get("name"):
            index[str(item["name"])]=item

    for name, object_type in updates.items():
        if name in index:
            index[name]["object_type"] = object_type
        else:
            data["types"].append({"name": name, "object_type": object_type})

    data["types"] = sorted(
        [item for item in data["types"] if isinstance(item, dict) and item.get("name")],
        key=lambda x: str(x["name"]),
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def ensure_function_blacklist_ref(api_table_data: Dict[str, Any]) -> List[str]:
    scan = api_table_data.get("scan")
    if not isinstance(scan, dict):
        scan = {}
        api_table_data["scan"] = scan

    function_cfg = scan.get("function")
    if not isinstance(function_cfg, dict):
        function_cfg = {}
        scan["function"] = function_cfg

    blacklist = function_cfg.get("blacklist")
    if not isinstance(blacklist, list):
        blacklist = []
        function_cfg["blacklist"] = blacklist

    return blacklist


def persist_api_table(api_table_path: Path, api_table_data: Dict[str, Any]) -> None:
    api_table_path.parent.mkdir(parents=True, exist_ok=True)
    api_table_path.write_text(json.dumps(api_table_data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def add_function_to_blacklist(
    func_name: str,
    runtime_blacklist: List[str],
    api_table_blacklist: List[str],
    pending_blacklist_updates: Set[str],
) -> None:
    if func_name not in runtime_blacklist:
        runtime_blacklist.append(func_name)
    if func_name not in api_table_blacklist:
        api_table_blacklist.append(func_name)
        pending_blacklist_updates.add(func_name)


def build_bridge_from_type(
    c_type: str,
    lv_type_entries: Dict[str, str],
    pending_updates: Dict[str, str],
    allow_prompt: bool,
    context: str,
    func_name: str,
) -> TypeBridge:
    normalized = normalize_type_key(c_type)
    base_name = normalized.replace("*", "")

    if normalized == "void":
        return TypeBridge(c_type, "", "none", None, None, "void", None)

    default_map = make_default_type_map()
    if normalized in default_map:
        sni_type = default_map[normalized]

        if sni_type == "SNI_T_STRING":
            return TypeBridge(c_type, "jerry_value_is_string", "string_fn", "sni_tb_js2c_string", sni_type, "string", "sni_tb_c2js_string")
        if sni_type == "SNI_T_BOOL":
            return TypeBridge(c_type, "jerry_value_is_boolean", "macro", "sni_tb_js2c_boolean", sni_type, "macro", "sni_tb_c2js_boolean")
        if sni_type == "SNI_T_DOUBLE":
            return TypeBridge(c_type, "jerry_value_is_number", "macro", "sni_tb_js2c_number", sni_type, "macro", "sni_tb_c2js_number")
        if sni_type == "SNI_T_INT32":
            return TypeBridge(c_type, "jerry_value_is_number", "macro", "sni_tb_js2c_int32", sni_type, "bridge", None)
        if sni_type == "SNI_T_UINT32":
            return TypeBridge(c_type, "jerry_value_is_number", "macro", "sni_tb_js2c_uint32", sni_type, "bridge", None)
        if sni_type == "SNI_T_PTR":
            return TypeBridge(c_type, "jerry_value_is_object", "bridge", None, sni_type, "bridge", None)

    # 先尝试用基础类型名映射，例如 typedef 的 int
    if normalized.endswith("_t") and normalized in lv_type_entries:
        object_type = lv_type_entries[normalized]
    else:
        object_type = lv_type_entries.get(base_name, lv_type_entries.get(normalized, ""))

    if object_type == "" and allow_prompt:
        prompt_key = base_name if base_name in lv_type_entries else normalized
        object_type = prompt_object_type(prompt_key, context)
        if object_type == "__BLACKLIST_FUNCTION__":
            raise SkipFunctionError(func_name, f"用户选择将函数加入黑名单: {context}")
        lv_type_entries[prompt_key] = object_type
        pending_updates[prompt_key] = object_type

    if object_type.startswith("primitive(") and object_type.endswith(")"):
        primitive_name = object_type[len("primitive("):-1].strip()
        if not primitive_name:
            raise SystemExit(f"[错误] 类型 '{normalized}' 的 object_type 无效: {object_type}")
        primitive_bridge = build_bridge_from_type(
            primitive_name,
            lv_type_entries,
            pending_updates,
            False,
            context,
            func_name,
        )
        return TypeBridge(c_type, primitive_bridge.js_check, primitive_bridge.js2c_mode, primitive_bridge.js2c_expr, primitive_bridge.sni_type, primitive_bridge.c2js_mode, primitive_bridge.c2js_expr)

    if object_type == "int":
        return TypeBridge(c_type, "jerry_value_is_number", "macro", "sni_tb_js2c_int32", "SNI_T_INT32", "bridge", None)

    if object_type == "handle_object":
        return TypeBridge(c_type, "jerry_value_is_object", "bridge", None, f"SNI_H_{c_type_to_base_name(normalized)}", "bridge", None)

    if object_type == "value_object":
        return TypeBridge(c_type, "jerry_value_is_object", "bridge", None, f"SNI_V_{c_type_to_base_name(normalized)}", "bridge", None)

    # 无 object_type 但为指针时，按 handle 兜底
    if normalized.endswith("*") and normalized.startswith("lv_"):
        return TypeBridge(c_type, "jerry_value_is_object", "bridge", None, f"SNI_H_{c_type_to_base_name(normalized)}", "bridge", None)

    # 无 object_type 的 lv_ 非指针按 value 兜底
    if normalized.startswith("lv_"):
        return TypeBridge(c_type, "jerry_value_is_object", "bridge", None, f"SNI_V_{c_type_to_base_name(normalized)}", "bridge", None)

    raise SystemExit(f"[错误] 无法映射类型: {c_type}")


def normalize_args(raw_args: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    cleaned: List[Dict[str, Any]] = []
    for item in raw_args:
        arg_type = normalize_c_type(item.get("type"))
        arg_name = item.get("name")
        if arg_type == "void" and (arg_name is None or str(arg_name).strip() == ""):
            continue
        cleaned.append(item)
    return cleaned


def collect_functions(
    lvgl_data: Dict[str, Any],
    filters: Dict[str, List[str]],
    lv_type_entries: Dict[str, str],
    pending_updates: Dict[str, str],
    stats: FilterStats,
) -> List[ApiFunction]:
    result: List[ApiFunction] = []

    whitelist = filters.get("whitelist", [])
    blacklist = filters.get("blacklist", [])

    for item in lvgl_data.get("functions", []):
        name = str(item.get("name", "")).strip()
        if not name:
            continue
        if not include_by_filter(name, whitelist, blacklist, stats):
            continue

        try:
            ret_type = normalize_c_type(item.get("type"))
            ret_bridge = build_bridge_from_type(
                ret_type,
                lv_type_entries,
                pending_updates,
                True,
                f"函数 {name} 的返回值",
                name,
            )

            args_raw = normalize_args(item.get("args", []))
            args: List[FuncArg] = []
            for idx, arg in enumerate(args_raw):
                arg_name = str(arg.get("name") or f"arg{idx}")
                arg_type = normalize_c_type(arg.get("type"))
                arg_bridge = build_bridge_from_type(
                    arg_type,
                    lv_type_entries,
                    pending_updates,
                    True,
                    f"函数 {name} 的参数 #{idx} ({arg_name})",
                    name,
                )
                args.append(FuncArg(name=arg_name, c_type=arg_type, bridge=arg_bridge))
        except SkipFunctionError:
            if name not in blacklist:
                blacklist.append(name)
            stats.blacklisted += 1
            stats.skipped_by_user += 1
            continue

        result.append(ApiFunction(name=name, return_type=ret_type, return_bridge=ret_bridge, args=args))

    return sorted(result, key=lambda x: x.name)


def entry_name(func_name: str) -> str:
    if not func_name.startswith("lv_"):
        return func_name
    rest = func_name[3:]
    parts = rest.split("_")
    if len(parts) <= 1:
        return rest
    return "_".join(parts[1:])


def render_entries(
    classes: List[ApiClass],
    class_constructors: Dict[str, Optional[ApiFunction]],
    class_methods: Dict[str, List[ApiFunction]],
    class_static_methods: Dict[str, List[ApiFunction]],
    class_properties: Dict[str, List[ApiProperty]],
    property_getters: Dict[Tuple[str, str], ApiFunction],
    property_setters: Dict[Tuple[str, str], ApiFunction],
    class_constants: Dict[str, List[ApiConstant]],
) -> str:
    wrappers: List[str] = []
    blocks: List[str] = []

    constructor_wrapper_names: Dict[str, str] = {}
    method_array_names: Dict[str, str] = {}
    property_array_names: Dict[str, str] = {}
    static_method_array_names: Dict[str, str] = {}
    constant_array_names: Dict[str, str] = {}

    for cls in classes:
        cls_id = sanitize_ident(cls.name)

        ctor_func = class_constructors[cls.name]
        if ctor_func is not None:
            ctor_wrapper = f"sni_api_ctor_{cls_id}"
            constructor_wrapper_names[cls.name] = ctor_wrapper
            wrappers.append(render_constructor_wrapper(cls, ctor_func))
        else:
            constructor_wrapper_names[cls.name] = "NULL"

        instance_items: List[Tuple[str, str]] = []
        for method_func in class_methods[cls.name]:
            wrappers.append(render_method_wrapper(cls, method_func))
            instance_items.append((entry_name(method_func.name), f"sni_api_{method_func.name}"))

        method_array_name = f"lv_class_methods_{cls_id}"
        method_array_names[cls.name] = method_array_name
        blocks.append(render_method_array(method_array_name, instance_items))

        static_items: List[Tuple[str, str]] = []
        for static_func in class_static_methods[cls.name]:
            wrappers.append(render_static_wrapper(static_func))
            static_items.append((entry_name(static_func.name), f"sni_api_{static_func.name}"))

        static_array_name = f"lv_class_static_methods_{cls_id}"
        static_method_array_names[cls.name] = static_array_name
        blocks.append(render_method_array(static_array_name, static_items))

        prop_items: List[Tuple[str, Optional[str], Optional[str]]] = []
        for prop in class_properties[cls.name]:
            getter_name = None
            setter_name = None

            if prop.getter:
                getter_func = property_getters[(cls.name, prop.name)]
                wrappers.append(render_property_getter_wrapper(cls, prop, getter_func))
                getter_name = f"sni_api_prop_get_{cls_id}_{sanitize_ident(prop.name)}"

            if prop.setter:
                setter_func = property_setters[(cls.name, prop.name)]
                wrappers.append(render_property_setter_wrapper(cls, prop, setter_func))
                setter_name = f"sni_api_prop_set_{cls_id}_{sanitize_ident(prop.name)}"

            prop_items.append((prop.name, getter_name, setter_name))

        prop_array_name = f"lv_class_properties_{cls_id}"
        property_array_names[cls.name] = prop_array_name
        blocks.append(render_property_array(prop_array_name, prop_items))

        const_array_name = f"lv_class_constants_{cls_id}"
        constant_array_names[cls.name] = const_array_name
        blocks.append(render_constant_array(const_array_name, class_constants[cls.name]))

    blocks.append(
        render_class_block(
            classes,
            constructor_wrapper_names,
            method_array_names,
            property_array_names,
            static_method_array_names,
            constant_array_names,
        )
    )

    return "\n\n".join(wrappers + blocks)


def write_output(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")


def collect_encountered_sni_types(functions: List[ApiFunction]) -> List[str]:
    encountered: set[str] = set()

    for func in functions:
        ret_sni = func.return_bridge.sni_type
        if ret_sni and (ret_sni.startswith("SNI_H_") or ret_sni.startswith("SNI_V_")):
            encountered.add(ret_sni)

        for arg in func.args:
            arg_sni = arg.bridge.sni_type
            if arg_sni and (arg_sni.startswith("SNI_H_") or arg_sni.startswith("SNI_V_")):
                encountered.add(arg_sni)

    return sorted(encountered)


def collect_encountered_sni_types_from_model(
    class_methods: Dict[str, List[ApiFunction]],
    class_static_methods: Dict[str, List[ApiFunction]],
    property_getters: Dict[Tuple[str, str], ApiFunction],
    property_setters: Dict[Tuple[str, str], ApiFunction],
) -> List[str]:
    all_funcs: List[ApiFunction] = []
    for items in class_methods.values():
        all_funcs.extend(items)
    for items in class_static_methods.values():
        all_funcs.extend(items)
    all_funcs.extend(property_getters.values())
    all_funcs.extend(property_setters.values())
    return collect_encountered_sni_types(all_funcs)


def export_encountered_sni_types(output_dir: Path, functions: List[ApiFunction]) -> Path:
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / "encountered_sni_types.txt"
    types = collect_encountered_sni_types(functions)
    lines = [f"{item}," for item in types]
    output_file.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return output_file


def print_verbose_stats(
    func_stats: FilterStats,
    const_stats: FilterStats,
) -> None:
    vlog(
        "[过滤] func: "
        f"total={func_stats.total}, included={func_stats.included}, "
        f"blacklisted={func_stats.blacklisted}, whitelist_miss={func_stats.whitelist_miss}, "
        f"skipped_by_user={func_stats.skipped_by_user}"
    )
    vlog(
        "[过滤] const: "
        f"total={const_stats.total}, included={const_stats.included}, "
        f"blacklisted={const_stats.blacklisted}, whitelist_miss={const_stats.whitelist_miss}"
    )


def visual_width(text: str) -> int:
    width = 0
    for ch in text:
        width += 2 if unicodedata.east_asian_width(ch) in ("W", "F") else 1
    return width


def pad_visual(text: str, target_width: int) -> str:
    pad = max(0, target_width - visual_width(text))
    return text + (" " * pad)


def print_summary(
    classes: List[ApiClass],
    class_constructors: Dict[str, Optional[ApiFunction]],
    class_methods: Dict[str, List[ApiFunction]],
    class_static_methods: Dict[str, List[ApiFunction]],
    class_properties: Dict[str, List[ApiProperty]],
    class_constants: Dict[str, List[ApiConstant]],
    output_path: Path,
    pending_updates: Dict[str, str],
    pending_blacklist_updates: Set[str],
    export_file: Optional[Path],
) -> None:
    class_total = len(classes)
    static_class_total = sum(1 for cls in classes if class_constructors.get(cls.name) is None)
    constructor_total = class_total - static_class_total
    dynamic_method_total = sum(len(items) for items in class_methods.values())
    static_method_total = sum(len(items) for items in class_static_methods.values())
    property_total = sum(len(items) for items in class_properties.values())
    constant_total = sum(len(items) for items in class_constants.values())

    rows: List[Tuple[str, str]] = [
        ("类", f"{class_total}个（静态类{static_class_total}个）"),
        ("动态方法", f"{dynamic_method_total}个"),
        ("静态方法", f"{static_method_total}个"),
        ("常数", f"{constant_total}个"),
        ("属性", f"{property_total}个"),
        ("构造函数", f"{constructor_total}个"),
    ]

    if pending_blacklist_updates:
        rows.append(("新增黑名单函数", f"{len(pending_blacklist_updates)}个"))
    if pending_updates:
        rows.append(("自动补全类型", f"{len(pending_updates)}个"))
    if export_file is not None:
        rows.append(("SNI 类型清单", str(export_file)))
    rows.append(("输出文件", str(output_path)))

    label_width = max(visual_width(label) for label, _ in rows)

    print("-" * 40)
    print("导出成功，统计信息：\n")
    for label, value in rows:
        print(f"{pad_visual(label, label_width)} : {value}")
    print("-" * 40)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate LVGL SNI descriptor C file")
    parser.add_argument("--api-table", required=True, help="Path to API filter table JSON")
    parser.add_argument("--output", required=True, help="Output C file path")
    parser.add_argument("--lvgl-json", required=True, help="Path to LVGL API JSON")
    parser.add_argument("--lv-types", required=True, help="Path to lv_types JSON")
    parser.add_argument(
        "--export-sni-types",
        required=False,
        help="Optional output directory for encountered SNI_H_*/SNI_V_* (one per line)",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose logs for intermediate generation details",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    global VERBOSE
    VERBOSE = bool(args.verbose)

    api_table_path = Path(args.api_table)
    output_path = Path(args.output)
    lvgl_json_path = Path(args.lvgl_json)
    lv_types_path = Path(args.lv_types)

    api_table_data = load_json(api_table_path, required=["classes"])
    lvgl_data = load_json(lvgl_json_path, required=["functions", "enums", "macros"])
    lv_types_data = load_json(lv_types_path, required=["types"])

    filters = parse_api_filters(api_table_data)
    classes_dict = parse_api_classes(api_table_data)
    classes = topo_sort_classes(classes_dict)
    lv_type_entries = load_type_entries(lv_types_data)
    function_index = build_function_index(lvgl_data)
    constant_index = build_constant_index(lvgl_data)

    func_stats = FilterStats()
    const_stats = FilterStats()
    pending_updates: Dict[str, str] = {}
    pending_blacklist_updates: Set[str] = set()

    class_methods: Dict[str, List[ApiFunction]] = {}
    class_constructors: Dict[str, Optional[ApiFunction]] = {}
    class_static_methods: Dict[str, List[ApiFunction]] = {}
    class_properties: Dict[str, List[ApiProperty]] = {}
    class_constants: Dict[str, List[ApiConstant]] = {}
    property_getters: Dict[Tuple[str, str], ApiFunction] = {}
    property_setters: Dict[Tuple[str, str], ApiFunction] = {}
    func_blacklist = filters["function"]["blacklist"]
    api_table_blacklist = ensure_function_blacklist_ref(api_table_data)

    for cls in classes:
        methods: List[ApiFunction] = []
        ctor_func: Optional[ApiFunction] = None
        method_names: Set[str] = set()

        if cls.constructor is not None:
            ctor_item = require_class_function(function_index, cls.name, cls.constructor, f"classes.{cls.name}.constructor")
            try:
                ctor_func = build_api_function(ctor_item, lv_type_entries, pending_updates, f"classes.{cls.name}.constructor")
            except SkipFunctionError as exc:
                add_function_to_blacklist(exc.func_name, func_blacklist, api_table_blacklist, pending_blacklist_updates)
                if pending_blacklist_updates:
                    persist_api_table(api_table_path, api_table_data)
                    velog(f"[统计] 已写入函数黑名单: {', '.join(sorted(pending_blacklist_updates))}")
                raise SystemExit(f"[错误] 类 {cls.name} 的构造函数被用户跳过，无法继续生成: {exc.func_name}") from exc

            for method_name in cls.methods:
                method_item = require_class_function(function_index, cls.name, method_name, f"classes.{cls.name}.methods")
                try:
                    method_func = build_api_function(method_item, lv_type_entries, pending_updates, f"classes.{cls.name}.methods")
                except SkipFunctionError as exc:
                    add_function_to_blacklist(exc.func_name, func_blacklist, api_table_blacklist, pending_blacklist_updates)
                    velog(f"[提示] 实例方法 {exc.func_name} 已被用户跳过，忽略")
                    continue

                if method_func.name not in method_names:
                    methods.append(method_func)
                    method_names.add(method_func.name)
        else:
            if cls.methods:
                raise SystemExit(f"[错误] classes.{cls.name} 是静态类，不允许配置 methods")

        static_funcs: List[ApiFunction] = []
        for static_name in cls.static_methods:
            static_item = require_class_function(function_index, cls.name, static_name, f"classes.{cls.name}.static_methods")
            try:
                static_func = build_api_function(static_item, lv_type_entries, pending_updates, f"classes.{cls.name}.static_methods")
            except SkipFunctionError as exc:
                add_function_to_blacklist(exc.func_name, func_blacklist, api_table_blacklist, pending_blacklist_updates)
                velog(f"[提示] 静态方法 {exc.func_name} 已被用户跳过，忽略")
                continue
            static_funcs.append(static_func)

        props = discover_class_properties(cls, function_index, filters)
        if cls.constructor is None:
            props = []
        validated_props: List[ApiProperty] = []

        for prop in props:
            valid_getter: Optional[str] = None
            valid_setter: Optional[str] = None

            if prop.getter:
                getter_item = require_function(function_index, prop.getter, f"classes.{cls.name}.property_getter.{prop.name}")
                try:
                    getter_func = build_api_function(
                        getter_item,
                        lv_type_entries,
                        pending_updates,
                        f"classes.{cls.name}.property_getter.{prop.name}",
                    )
                except SkipFunctionError as exc:
                    add_function_to_blacklist(exc.func_name, func_blacklist, api_table_blacklist, pending_blacklist_updates)
                    velog(f"[提示] 属性 getter {exc.func_name} 已被用户跳过，忽略")
                    getter_func = None

                if getter_func is not None and len(getter_func.args) == 1 and getter_func.return_bridge.c2js_mode != "void":
                    property_getters[(cls.name, prop.name)] = getter_func
                    valid_getter = prop.getter
                elif getter_func is not None:
                    velog(
                        f"[提示] {prop.getter} 不符合属性 getter 签名(应为 this-only 且有返回值)，已忽略"
                    )

            if prop.setter:
                setter_item = require_function(function_index, prop.setter, f"classes.{cls.name}.property_setter.{prop.name}")
                try:
                    setter_func = build_api_function(
                        setter_item,
                        lv_type_entries,
                        pending_updates,
                        f"classes.{cls.name}.property_setter.{prop.name}",
                    )
                except SkipFunctionError as exc:
                    add_function_to_blacklist(exc.func_name, func_blacklist, api_table_blacklist, pending_blacklist_updates)
                    velog(f"[提示] 属性 setter {exc.func_name} 已被用户跳过，忽略")
                    setter_func = None

                if setter_func is not None and len(setter_func.args) == 2:
                    property_setters[(cls.name, prop.name)] = setter_func
                    valid_setter = prop.setter
                elif setter_func is not None:
                    velog(
                        f"[提示] {prop.setter} 不符合属性 setter 签名(应为 this+value)，已忽略"
                    )

            if valid_getter or valid_setter:
                validated_props.append(ApiProperty(name=prop.name, getter=valid_getter, setter=valid_setter))

        const_items: List[ApiConstant] = []
        for const_name in cls.constants:
            if const_name not in constant_index:
                raise SystemExit(f"[错误] classes.{cls.name}.constants 引用未知常量: {const_name}")
            const_items.append(constant_index[const_name])

        class_constructors[cls.name] = ctor_func
        class_methods[cls.name] = methods
        class_static_methods[cls.name] = static_funcs
        class_properties[cls.name] = validated_props
        class_constants[cls.name] = const_items

    constants = collect_constants(lvgl_data, filters["constant"], const_stats)
    func_stats.included = sum(len(items) for items in class_methods.values()) + sum(len(items) for items in class_static_methods.values())
    func_stats.total = len(function_index)
    func_stats.blacklisted = 0
    func_stats.whitelist_miss = 0

    script_dir = Path(__file__).resolve().parent
    lv_types_output_path = script_dir / "lv_types_output.json"
    update_lv_types_output(lv_types_output_path, pending_updates)

    if pending_blacklist_updates:
        persist_api_table(api_table_path, api_table_data)
        velog(f"[统计] 已写入函数黑名单: {', '.join(sorted(pending_blacklist_updates))}")

    table_text = render_entries(
        classes,
        class_constructors,
        class_methods,
        class_static_methods,
        class_properties,
        property_getters,
        property_setters,
        class_constants,
    )

    generated = (
        HEADER_TEXT.format(date=datetime.date.today().isoformat())
        + "\n"
        + table_text
        + "\n"
    )
    write_output(output_path, generated)

    export_file: Optional[Path] = None
    if args.export_sni_types:
        export_dir = Path(args.export_sni_types)
        all_types = collect_encountered_sni_types_from_model(
            class_methods,
            class_static_methods,
            property_getters,
            property_setters,
        )
        export_dir.mkdir(parents=True, exist_ok=True)
        export_file = export_dir / "encountered_sni_types.txt"
        export_file.write_text("\n".join(f"{item}," for item in all_types) + "\n", encoding="utf-8")
        velog(f"[统计] 已导出遇到的 SNI_H_*/SNI_V_*: {export_file}")

    if pending_blacklist_updates:
        func_stats.skipped_by_user += len(pending_blacklist_updates)
        func_stats.blacklisted += len(pending_blacklist_updates)
    print_verbose_stats(func_stats, const_stats)
    if pending_updates:
        velog(f"[统计] 已补全类型并写入: {lv_types_output_path}")

    print_summary(
        classes,
        class_constructors,
        class_methods,
        class_static_methods,
        class_properties,
        class_constants,
        output_path,
        pending_updates,
        pending_blacklist_updates,
        export_file,
    )


if __name__ == "__main__":
    main()
