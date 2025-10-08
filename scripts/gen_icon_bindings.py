import re
import os
from datetime import date

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
HEADER_FILE = os.path.join(BASE_DIR, "../src/lang/fonts/elena_os_icon.h")
HEADER_FILE = os.path.normpath(HEADER_FILE)  # 规范化路径
OUTPUT_FILE = os.path.join(BASE_DIR, "../src/lang/fonts/elena_os_icon.c")

with open(HEADER_FILE, "r", encoding="utf-8") as f:
    content = f.read()

# 提取宏名称，例如 LV_SYMBOL_POWER、LV_SYMBOL_WIFI 等
symbols = re.findall(r"#define\s+(RI_[A-Z0-9_]+)\s", content)

with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
    f.write("""/**
* @file lv_bindings.c
* @brief 将 LVGL 绑定到 JerryScript 的实现文件，此文件使用脚本`gen_lvgl_binding.py`自动生成。
* @author Sab1e
* @date """ + date.today().strftime("%Y-%m-%d") + r"""
*/

#include "elena_os_icon.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "jerryscript.h"
#include "lvgl.h"

// Macros and Definitions

// Variables

// Function Implementations

""")
    f.write("void eos_icon_register(void) {\n")
    f.write("    jerry_value_t global = jerry_current_realm();\n")

    for sym in symbols:
        f.write(f"#ifdef {sym}\n")
        f.write(f"do{{\n")
        f.write(f"    jerry_value_t {sym}_str = jerry_string_sz(\"{sym}\");\n")
        f.write(f"    jerry_value_t {sym}_val = jerry_string_sz({sym});\n")
        f.write(f"    jerry_value_free(jerry_object_set(global, {sym}_str, {sym}_val));\n")
        f.write(f"    jerry_value_free({sym}_str);\n")
        f.write(f"    jerry_value_free({sym}_val);\n")
        f.write(f"}}while(0);\n")
        f.write("#else\n")
        f.write(f"    #pragma message(\"WARNING: Macro {sym} is not defined\")\n")
        f.write("#endif\n")

    f.write("    jerry_value_free(global);\n")
    f.write("}\n")

print(f"✅ Generated {OUTPUT_FILE} with {len(symbols)} symbols.")
