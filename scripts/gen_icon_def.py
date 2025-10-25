#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file gen_icon_def.py
@brief 从字体文件自动生成 ElenaOS 图标定义头文件，可指定 Unicode 范围

Usage:
    python gen_icon_def.py --ttf ./fonts/remixicon.ttf --out ./src/lang/fonts --start 0xE000 --end 0xEFFF

若不指定参数，将默认使用：
    ../third_party/RemixIcon/fonts/remixicon.ttf
    ../src/lang/fonts/
"""

import os
import argparse
from fontTools.ttLib import TTFont
from datetime import datetime

def normalize_name(name: str) -> str:
    """规范化符号名为 C 宏格式"""
    invalid_chars = ['-', '.', ' ', '/', '+', '(', ')', '[', ']', '{', '}', '&', '%', '$', '@', '!', '?', ':', ';', ',']
    for ch in invalid_chars:
        name = name.replace(ch, '_')
    return name.upper()

def generate_icon_header(ttf_path: str, out_dir: str, start: int = None, end: int = None):
    """生成 elena_os_icon.h 文件"""
    ttf_path = os.path.abspath(ttf_path)
    out_dir = os.path.abspath(out_dir)
    os.makedirs(out_dir, exist_ok=True)

    font_name = os.path.splitext(os.path.basename(ttf_path))[0]
    output_path = os.path.join(out_dir, "elena_os_icon.h")
    date_str = datetime.now().strftime("%Y-%m-%d")

    # 打开字体文件
    font = TTFont(ttf_path)

    # 获取 Unicode -> glyphName 映射
    unicode_map = {}
    for table in font["cmap"].tables:
        for codepoint, name in table.cmap.items():
            # 判断是否在指定范围
            if start is not None and codepoint < start:
                continue
            if end is not None and codepoint > end:
                continue
            unicode_map[codepoint] = name

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(f"""/**
 * @file elena_os_icon.h
 * @brief 图标名称映射Unicode
 *
 * 此文件由 `gen_icon_def.py` 自动生成。
 * 不要手动修改！
 * @date {date_str}
 */

#ifndef ELENA_OS_ICON_H
#define ELENA_OS_ICON_H

#ifdef __cplusplus
extern "C" {{
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * Font: {font_name} 「https://remixicon.com」
 * License: Apache License 2.0
 * Original source: https://github.com/Remix-Design/RemixIcon
 */
""")

        # 去重逻辑
        seen_macros = set()
        dup_count = 0
        for codepoint, name in sorted(unicode_map.items()):
            macro_name = normalize_name(name)
            if macro_name in seen_macros:
                dup_count += 1
                continue
            seen_macros.add(macro_name)
            f.write(f'#define RI_{macro_name} "\\u{codepoint:04X}"\n')

        f.write("""

void eos_icon_register(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_ICON_H */
""")

    print(f"✅ 成功生成: {output_path}")
    print(f"🔢 共生成 {len(seen_macros)} 个图标定义，跳过重复 {dup_count} 个。")

def main():
    BASE_DIR = os.path.dirname(os.path.abspath(__file__))
    DEFAULT_TTF = os.path.normpath(os.path.join(BASE_DIR, "../third_party/RemixIcon/fonts/remixicon.ttf"))
    DEFAULT_OUT = os.path.normpath(os.path.join(BASE_DIR, "../src/lang/fonts"))

    parser = argparse.ArgumentParser(description="Generate LVGL icon definitions from TTF.")
    parser.add_argument("--ttf", help="输入 TTF 字体路径", default=DEFAULT_TTF)
    parser.add_argument("--out", help="输出目录", default=DEFAULT_OUT)
    parser.add_argument("--start", help="Unicode 起始码 (hex)", type=lambda x: int(x, 16))
    parser.add_argument("--end", help="Unicode 结束码 (hex)", type=lambda x: int(x, 16))
    args = parser.parse_args()

    print(f"🔍 使用字体: {args.ttf}")
    print(f"📂 输出目录: {args.out}")
    if args.start is not None or args.end is not None:
        print(f"🖤 生成 Unicode 范围: {hex(args.start) if args.start else 'None'} ~ {hex(args.end) if args.end else 'None'}")

    generate_icon_header(args.ttf, args.out, args.start, args.end)

if __name__ == "__main__":
    main()
