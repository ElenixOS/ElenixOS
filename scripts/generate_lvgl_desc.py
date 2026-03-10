#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Generate LVGL SNI descriptor C source from JSON metadata."""

from __future__ import annotations

import argparse
import datetime
import fnmatch
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple


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
        "func": {"whitelist": [], "blacklist": []},
        "const": {"whitelist": [], "blacklist": []},
    }

    for group in ("func", "const"):
        group_obj = api_table.get(group, {})
        for key in ("whitelist", "blacklist"):
            value = group_obj.get(key, [])
            if not isinstance(value, list):
                raise SystemExit(f"[错误] api-table 字段 {group}.{key} 必须是数组")
            result[group][key] = [str(item).strip() for item in value if str(item).strip()]
    return result


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


def namespace_path(func_name: str) -> List[str]:
    # lv_obj_set_size -> ["obj"] + entry "set_size"
    if not func_name.startswith("lv_"):
        return []
    rest = func_name[3:]
    parts = rest.split("_")
    if len(parts) <= 1:
        return []
    return [parts[0]]


def entry_name(func_name: str) -> str:
    if not func_name.startswith("lv_"):
        return func_name
    rest = func_name[3:]
    parts = rest.split("_")
    if len(parts) <= 1:
        return rest
    return "_".join(parts[1:])


def render_wrapper(func: ApiFunction) -> str:
    lines: List[str] = []
    wrapper_name = f"sni_api_{func.name}"

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

    for idx, arg in enumerate(func.args):
        check = arg.bridge.js_check
        if check:
            lines.append(f"    if (!{check}(args_p[{idx}]))")
            lines.append("    {")
            lines.append('        return sni_api_throw_error("Invalid argument type");')
            lines.append("    }")

        lines.append(f"    {arg.c_type} {arg.name};")

        if arg.bridge.js2c_mode == "macro" and arg.bridge.js2c_expr:
            lines.append(f"    {arg.name} = {arg.bridge.js2c_expr}(args_p[{idx}]);")
        elif arg.bridge.js2c_mode == "string_fn" and arg.bridge.js2c_expr:
            lines.append(f"    {arg.name} = {arg.bridge.js2c_expr}(args_p[{idx}]);")
        elif arg.bridge.js2c_mode == "bridge" and arg.bridge.sni_type:
            lines.append(f"    if (!sni_tb_js2c(args_p[{idx}], {arg.bridge.sni_type}, &{arg.name}))")
            lines.append("    {")
            lines.append('        return sni_api_throw_error("Failed to convert argument");')
            lines.append("    }")
        else:
            lines.append('    return sni_api_throw_error("Unsupported argument conversion");')
            lines.append("}")
            return "\n".join(lines)

        lines.append("")

    call_args = ", ".join(arg.name for arg in func.args)

    if func.return_bridge.c2js_mode == "void":
        lines.append(f"    {func.name}({call_args});" if call_args else f"    {func.name}();")
        lines.append("    return jerry_undefined();")
        lines.append("}")
        return "\n".join(lines)

    lines.append(f"    {func.return_type} result = {func.name}({call_args});" if call_args else f"    {func.return_type} result = {func.name}();")

    if func.return_bridge.c2js_mode == "macro" and func.return_bridge.c2js_expr:
        lines.append(f"    return {func.return_bridge.c2js_expr}(result);")
    elif func.return_bridge.c2js_mode == "string" and func.return_bridge.c2js_expr:
        lines.append(f"    return {func.return_bridge.c2js_expr}(result);")
    elif func.return_bridge.c2js_mode == "bridge" and func.return_bridge.sni_type:
        # sni_tb_c2js expects a pointer to the C value container.
        # For handle/pointer returns this must be pointer-to-pointer (&result).
        lines.append(f"    return sni_tb_c2js(&result, {func.return_bridge.sni_type});")
    else:
        lines.append('    return sni_api_throw_error("Unsupported return conversion");')

    lines.append("}")
    return "\n".join(lines)


def render_entry_array(name: str, items: List[str]) -> str:
    lines: List[str] = [f"const sni_api_entry_t {name}[] = {{"]
    for item in items:
        lines.append(item)
    lines.append("    {.name = NULL},")
    lines.append("};")
    return "\n".join(lines)


def render_entries(functions: List[ApiFunction], constants: List[ApiConstant]) -> str:
    namespace_map: Dict[str, List[ApiFunction]] = {}
    root_funcs: List[ApiFunction] = []

    for func in functions:
        ns = namespace_path(func.name)
        if not ns:
            root_funcs.append(func)
            continue
        namespace_map.setdefault(".".join(ns), []).append(func)

    blocks: List[str] = []

    # 一级 namespace 目前按 lv_<ns>_xxx 生成
    ns_arrays: Dict[str, str] = {}
    for ns_name in sorted(namespace_map.keys()):
        arr_name = f"lv_api_ns_{ns_name.replace('.', '_')}"
        ns_arrays[ns_name] = arr_name
        items: List[str] = []
        for func in sorted(namespace_map[ns_name], key=lambda x: x.name):
            items.append(
                "    {"
                f".type = SNI_ENTRY_FUNCTION, .name = \"{entry_name(func.name)}\", "
                f".value.function = sni_api_{func.name}"
                "},"
            )
        blocks.append(render_entry_array(arr_name, items))

    root_items: List[str] = []

    for const in constants:
        if const.kind == "int":
            root_items.append(
                "    {"
                f".type = SNI_ENTRY_CONSTANT_INT, .name = \"{const.name}\", "
                f".value.constant.i = {const.value}"
                "},"
            )
        elif const.kind == "float":
            root_items.append(
                "    {"
                f".type = SNI_ENTRY_CONSTANT_FLOAT, .name = \"{const.name}\", "
                f".value.constant.f = {const.value}"
                "},"
            )
        elif const.kind == "string":
            root_items.append(
                "    {"
                f".type = SNI_ENTRY_CONSTANT_STRING, .name = \"{const.name}\", "
                f".value.constant.s = {const.value}"
                "},"
            )

    for func in sorted(root_funcs, key=lambda x: x.name):
        root_items.append(
            "    {"
            f".type = SNI_ENTRY_FUNCTION, .name = \"{entry_name(func.name)}\", "
            f".value.function = sni_api_{func.name}"
            "},"
        )

    for ns_name in sorted(ns_arrays.keys()):
        root_items.append(
            "    {"
            f".type = SNI_ENTRY_NAMESPACE, .name = \"{ns_name}\", "
            f".value.sub_entries = {ns_arrays[ns_name]}"
            "},"
        )

    blocks.append(render_entry_array("lv_api_entry", root_items))

    init_mount = """
void sni_api_lv_init(void)
{
    lv_api_obj = sni_api_build(lv_api_entry);
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

    blocks.append(init_mount)
    return "\n\n".join(blocks)


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


def export_encountered_sni_types(output_dir: Path, functions: List[ApiFunction]) -> Path:
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / "encountered_sni_types.txt"
    types = collect_encountered_sni_types(functions)
    lines = [f"{item}," for item in types]
    output_file.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return output_file


def print_stats(
    functions: List[ApiFunction],
    constants: List[ApiConstant],
    func_stats: FilterStats,
    const_stats: FilterStats,
) -> None:
    print(
        "[过滤] func: "
        f"total={func_stats.total}, included={func_stats.included}, "
        f"blacklisted={func_stats.blacklisted}, whitelist_miss={func_stats.whitelist_miss}, "
        f"skipped_by_user={func_stats.skipped_by_user}"
    )
    print(
        "[过滤] const: "
        f"total={const_stats.total}, included={const_stats.included}, "
        f"blacklisted={const_stats.blacklisted}, whitelist_miss={const_stats.whitelist_miss}"
    )
    print(f"[统计] 导出函数: {len(functions)}")
    print(f"[统计] 导出常量: {len(constants)}")


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
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    api_table_path = Path(args.api_table)
    output_path = Path(args.output)
    lvgl_json_path = Path(args.lvgl_json)
    lv_types_path = Path(args.lv_types)

    api_table_data = load_json(api_table_path, required=["func", "const"])
    lvgl_data = load_json(lvgl_json_path, required=["functions", "enums", "macros"])
    lv_types_data = load_json(lv_types_path, required=["types"])

    filters = parse_api_filters(api_table_data)
    lv_type_entries = load_type_entries(lv_types_data)

    func_stats = FilterStats()
    const_stats = FilterStats()
    pending_updates: Dict[str, str] = {}
    functions = collect_functions(
        lvgl_data,
        filters["func"],
        lv_type_entries,
        pending_updates,
        func_stats,
    )
    constants = collect_constants(lvgl_data, filters["const"], const_stats)

    script_dir = Path(__file__).resolve().parent
    lv_types_output_path = script_dir / "lv_types_output.json"
    update_lv_types_output(lv_types_output_path, pending_updates)

    wrappers = "\n\n".join(render_wrapper(func) for func in functions)
    table_text = render_entries(functions, constants)

    generated = (
        HEADER_TEXT.format(date=datetime.date.today().isoformat())
        + "\n"
        + wrappers
        + "\n\n"
        + table_text
        + "\n"
    )
    write_output(output_path, generated)

    if args.export_sni_types:
        export_dir = Path(args.export_sni_types)
        export_file = export_encountered_sni_types(export_dir, functions)
        print(f"[统计] 已导出遇到的 SNI_H_*/SNI_V_*: {export_file}")

    print_stats(functions, constants, func_stats, const_stats)
    if pending_updates:
        print(f"[统计] 已补全类型并写入: {lv_types_output_path}")
    print(f"[完成] 已生成: {output_path}")


if __name__ == "__main__":
    main()
