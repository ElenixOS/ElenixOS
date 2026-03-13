#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file gen_icon_register.py
@brief 从 elena_os_icon.h 自动生成 eos_icon_register() 的实现文件
"""

import re
import os
from datetime import date

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
HEADER_FILE = os.path.normpath(os.path.join(BASE_DIR, "../src/lang/fonts/elena_os_icon.h"))
OUTPUT_FILE = os.path.normpath(os.path.join(BASE_DIR, "../src/lang/fonts/elena_os_icon.c"))

# 读取头文件
with open(HEADER_FILE, "r", encoding="utf-8") as f:
    content = f.read()

# 提取宏名称，例如 RI_BATTERY_CHARGE_FILL
symbols = re.findall(r"#define\s+(RI_[A-Z0-9_]+)\s", content)

# 写入C文件
with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
    f.write(f"""/**
 * @file elena_os_icon.c
 * @brief 将图标宏注册到 JerryScript，全自动生成。
 * @date {date.today().strftime("%Y-%m-%d")}
 */

// Includes
#include "elena_os_icon.h"
#include "jerryscript.h"
#include <stdio.h>

// Macros and Definitions
#define BINDING_OBJ script_engine_eos_obj

// Variables
extern jerry_value_t script_engine_eos_obj;

// Function Implementations
/**
 * @brief 将所有图标宏名与Unicode值映射注册到 JerryScript 全局对象
 */
void eos_icon_register(void)
{{
    struct {{
        const char *name;
        const char *value;
    }} icons[] = {{
""")

    for sym in symbols:
        f.write(f'        #ifdef {sym}\n')
        f.write(f'        {{"{sym}", {sym}}},\n')
        f.write(f'        #endif\n')

    f.write("""    };

    size_t icon_count = sizeof(icons) / sizeof(icons[0]);
    for (size_t i = 0; i < icon_count; i++) {
        if (!icons[i].name || !icons[i].value)
            continue;
        jerry_value_t key = jerry_string_sz(icons[i].name);
        jerry_value_t val = jerry_string_sz(icons[i].value);
        jerry_value_free(jerry_object_set(BINDING_OBJ, key, val));
        jerry_value_free(key);
        jerry_value_free(val);
    }

}
""")

print(f"✅ 成功生成: {OUTPUT_FILE}")
print(f"🔢 共注册 {len(symbols)} 个图标符号。")
