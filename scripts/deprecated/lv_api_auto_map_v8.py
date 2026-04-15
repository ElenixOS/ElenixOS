#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file lv_api_auto_map_v8.py
@brief 自动从 lv_api_map_v8.h 读取 API 映射 (#define old new)，并批量替换指定目录下的所有 C 代码文件内容。
"""

import os
import re

# ===== 配置区 =====
MAP_FILE = "lv_api_map_v8.h"  # 映射表文件路径
SRC_DIR = "../src"               # 要处理的源代码目录
FILE_EXTS = [".c", ".h", ".cpp", ".hpp"]  # 要替换的文件类型
# =================

# 解析 define 映射
def parse_define_map(map_file):
    mapping = {}
    with open(map_file, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            # 匹配 "#define lv_xxx lv_yyy"
            m = re.match(r"#define\s+(lv_\w+)\s+(lv_\w+)", line)
            if m:
                old, new = m.groups()
                if old != new:
                    mapping[old] = new
    return mapping

# 替换函数
def replace_in_file(filepath, mapping):
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()

    original = content
    for old, new in mapping.items():
        # 用 \b 保证只替换完整符号名
        content = re.sub(rf"\b{old}\b", new, content)

    if content != original:
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"✅ Replaced in: {filepath}")

# 遍历目录
def process_dir(src_dir, mapping):
    for root, _, files in os.walk(src_dir):
        for name in files:
            if any(name.endswith(ext) for ext in FILE_EXTS):
                replace_in_file(os.path.join(root, name), mapping)

# 主入口
if __name__ == "__main__":
    if not os.path.exists(MAP_FILE):
        print(f"❌ 找不到映射文件: {MAP_FILE}")
        exit(1)

    print(f"📖 正在解析 {MAP_FILE} ...")
    mapping = parse_define_map(MAP_FILE)
    print(f"共解析出 {len(mapping)} 条映射。")

    print(f"🔧 开始处理目录: {SRC_DIR}")
    process_dir(SRC_DIR, mapping)

    print("✨ 完成全部替换！")
