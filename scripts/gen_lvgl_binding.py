import json
import os
from datetime import date
import sys
from colorama import init, Fore, Style
import fnmatch

# 配置路径
script_dir = os.path.dirname(os.path.abspath(__file__))
input_json_file = '../LvglPlatform/lvgl/scripts/gen_json/output/lvgl.json'
output_c_file = 'src/lv_bindings.c'

configuration_path = ''
export_functions_path = os.path.join(script_dir, "export_functions.txt")
blacklist_functions_path = os.path.join(script_dir, "blacklist_functions.txt")
export_macros_path = os.path.join(script_dir, "export_macros.txt")
blacklist_macros_path = os.path.join(script_dir, "blacklist_macros.txt")

extract_funcs_from = None
print_all_info = False
print_macro_info = False
# 目标代码的固定部分
HEADER_CODE = r"""
/**
 * @file lv_bindings.c
 * @brief 将 LVGL 绑定到 JerryScript 的实现文件，此文件使用脚本`gen_lvgl_binding.py`自动生成。
 * @author Sab1e
 * @date """ + date.today().strftime("%Y-%m-%d") + r"""
 */
// Application System header files
#include "lv_bindings.h"
#include "lv_bindings_misc.h"
#include "script_engine_core.h"
// Third party header files
#include "jerryscript.h"
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>

/********************************** 错误处理辅助函数 **********************************/
static jerry_value_t throw_error(const char* message) {
    jerry_value_t error_obj = jerry_error_sz(JERRY_ERROR_TYPE, (const jerry_char_t*)message);
    return jerry_throw_value(error_obj, true);
}

/********************************** 宏定义处理辅助函数 **********************************/

static void lvgl_binding_set_enum(jerry_value_t global, const char* key, int32_t val) {
    jerry_value_t jkey = jerry_string_sz(key);
    jerry_value_t jval = jerry_number(val);
    jerry_value_free(jerry_object_set(global, jkey, jval));
    jerry_value_free(jkey);
    jerry_value_free(jval);
}

"""

INIT_FUNCTION_CODE = r"""
/********************************** 初始化 LVGL 绑定系统 **********************************/
/**
 * @brief 初始化回调系统，注册 LVGL 对象删除事件处理函数，并注册 LVGL 函数
 */
void lv_binding_init() {
    lv_binding_jerryscript_register_functions(lvgl_binding_funcs, sizeof(lvgl_binding_funcs) / sizeof(LVBindingJerryscriptFuncEntry_t));
    lv_bindings_misc_init();
    register_lvgl_enums();
}
"""

def parse_type(type_info):
    """解析类型信息，返回C类型字符串和类型信息字典"""
    if not type_info:
        return ('void', {'is_void': True})

    if type_info.get('json_type') == 'macro':
        return (type_info['name'], {'is_macro': True, 'initializer': type_info.get('initializer', '')})

    if type_info.get('json_type') == 'ret_type':
        return parse_type(type_info.get('type'))

    if type_info.get('json_type') == 'pointer':
        base_type, base_info = parse_type(type_info.get('type'))
        quals = ' '.join(type_info.get('quals', []))

        # 特殊处理 lv_font_t*
        if base_type == 'lv_font_t':
            return (f"{quals} lv_font_t*", {'is_font_pointer': True})

        if quals:
            return (f"{quals} {base_type}*", {'is_pointer': True, 'base_type': base_info})
        return (f"{base_type}*", {'is_pointer': True, 'base_type': base_info})

    if 'name' in type_info:
        # 特殊处理 lv_font_t 非指针情况
        if type_info['name'] == 'lv_font_t':
            return ('lv_font_t', {'is_font': True})
        # 特殊处理 lv_event_t 指针
        if type_info['name'] == 'lv_event_t' and type_info.get('json_type') == 'pointer':
            return ('lv_event_t*', {'is_event_pointer': True})

        quals = ' '.join(type_info.get('quals', []))
        type_name = type_info['name']
        type_info_dict = {'type_name': type_name}

        if type_info.get('json_type') == 'primitive_type':
            type_info_dict['is_primitive'] = True
        elif type_info.get('json_type') == 'stdlib_type':
            type_info_dict['is_stdlib'] = True

        if quals:
            return (f"{quals} {type_name}", type_info_dict)
        return (type_name, type_info_dict)

    return ('void', {'is_void': True})

def is_macro_type(type_info):
    """检查是否是宏定义类型"""
    return isinstance(type_info, dict) and type_info.get('is_macro', False)

def is_void_type(type_str):
    """检查是否是void类型"""
    return 'void' in type_str and '*' not in type_str

def is_lv_color_t(type_str):
    """检查是否是 lv_color_t 类型"""
    return 'lv_color_t' in type_str

def is_void_pointer(type_str):
    """检查是否是void指针类型"""
    return 'void*' in type_str.replace(' ', '')

def is_lv_obj_pointer(type_str):
    """检查是否是lv_obj_t指针类型"""
    return type_str.replace(' ', '') == 'lv_obj_t*'

def is_object_pointer(type_str):
    """检查是否是对象指针类型"""
    return type_str.endswith('*') and ('lv_' in type_str or 'obj' in type_str)

def is_string_pointer(type_str):
    """检查是否是字符串指针类型"""
    normalized = type_str.replace(' ', '')
    return normalized in ['char*', 'constchar*', 'const char*', 'char const*']

def is_number_type(type_str):
    """检查是否是数字类型"""
    number_types = ['int', 'float', 'double', 'uint8_t', 'uint16_t', 'uint32_t',
                   'int8_t', 'int16_t', 'int32_t', 'size_t', 'bool', 'short',
                   'long', 'unsigned', 'signed', 'uintptr_t']
    return any(num_type in type_str for num_type in number_types)

def is_lvgl_value_type(type_str):
    """检查是否是LVGL值类型（非指针）"""
    return 'lv_' in type_str and '*' not in type_str

def is_enum_type(type_name, typedefs_data):
    """检查是否是枚举类型"""
    for enum in typedefs_data.get('enums', []):
        if enum['name'] == type_name:
            return True
    return False

def is_typedef_convertible_to_basic(typedef_name, typedefs_data):
    """检查typedef是否可以转换为基本类型"""
    if is_enum_type(typedef_name, typedefs_data):
        return True

    for typedef in typedefs_data.get('typedefs', []):
        if typedef['name'] == typedef_name:
            base_type, type_info = parse_type(typedef['type'])

            if type_info.get('is_pointer') and type_info['base_type'].get('is_void'):
                return True

            if type_info.get('is_primitive') or type_info.get('is_stdlib'):
                return True

    return False

def get_typedef_base_type(typedef_name, typedefs_data):
    """获取typedef对应的基础类型"""
    if is_enum_type(typedef_name, typedefs_data):
        return 'int'

    for typedef in typedefs_data.get('typedefs', []):
        if typedef['name'] == typedef_name:
            base_type, type_info = parse_type(typedef['type'])

            if type_info.get('is_pointer') and type_info['base_type'].get('is_void'):
                return 'uintptr_t'

            if type_info.get('is_primitive') or type_info.get('is_stdlib'):
                return base_type

    return None

def generate_void_pointer_arg_parsing(index, name):
    """生成void指针类型参数解析代码，支持字符串、对象和数值"""
    return fr"""    // void*/字符串 类型参数，支持null
    void* {name} = NULL;
    char* {name}_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[{index}]) && !jerry_value_is_null(args[{index}])) {{
        if (jerry_value_is_string(args[{index}])) {{
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t {name}_len = jerry_string_size(args[{index}], JERRY_ENCODING_UTF8);
            {name}_str = (char*)malloc({name}_len + 1);
            if (!{name}_str) {{
                return throw_error("Failed to allocate memory for string argument");
            }}
            jerry_string_to_buffer(args[{index}], JERRY_ENCODING_UTF8, (jerry_char_t*){name}_str, {name}_len);
            {name}_str[{name}_len] = '\0';
            {name} = (void*){name}_str;
        }}
        else if (jerry_value_is_object(args[{index}])) {{
            // 尝试从对象获取指针
            jerry_value_t ptr_prop = jerry_string_sz("__ptr");
            jerry_value_t ptr_val = jerry_object_get(args[{index}], ptr_prop);
            jerry_value_free(ptr_prop);

            if (jerry_value_is_number(ptr_val)) {{
                uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(ptr_val);
                {name} = (void*)ptr_num;
            }}
            jerry_value_free(ptr_val);
        }}
        else if (jerry_value_is_number(args[{index}])) {{
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[{index}]);
            {name} = (void*)ptr_num;
        }}
        else {{
            if ({name}_str) free({name}_str);
            return throw_error("Argument {index} must be string, object or number");
        }}
    }}

    // 注意：需要在函数末尾添加 free({name}_str);
"""

def generate_generic_pointer_arg_parsing(index, name, type_str):
    """生成通用指针类型参数解析代码，支持null"""
    return fr"""    // 通用指针类型: {type_str}，支持null
    void* {name} = NULL;
    if (!jerry_value_is_undefined(args[{index}]) && !jerry_value_is_null(args[{index}])) {{
        if (jerry_value_is_object(args[{index}])) {{
            // 尝试从对象获取指针
            jerry_value_t ptr_prop = jerry_string_sz("__ptr");
            jerry_value_t ptr_val = jerry_object_get(args[{index}], ptr_prop);
            jerry_value_free(ptr_prop);

            if (jerry_value_is_number(ptr_val)) {{
                uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(ptr_val);
                {name} = (void*)ptr_num;
            }}
            jerry_value_free(ptr_val);
        }}
        else if (jerry_value_is_number(args[{index}])) {{
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[{index}]);
            {name} = (void*)ptr_num;
        }}
        else {{
            return throw_error("Argument {index} must be object, number or null for {type_str}");
        }}
    }}

"""

def generate_object_arg_parsing(index, name):
    """生成对象类型参数解析代码，支持null"""
    return fr"""    // 对象类型参数，支持null
    void* {name} = NULL;
    if (!jerry_value_is_undefined(args[{index}]) && !jerry_value_is_null(args[{index}])) {{
        jerry_value_t js_{name} = args[{index}];
        if (!jerry_value_is_object(js_{name})) {{
            return throw_error("Argument {index} must be an object or null");
        }}

        jerry_value_t {name}_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t {name}_ptr_val = jerry_object_get(js_{name}, {name}_ptr_prop);
        jerry_value_free({name}_ptr_prop);

        if (!jerry_value_is_number({name}_ptr_val)) {{
            jerry_value_free({name}_ptr_val);
            return throw_error("Invalid __ptr property");
        }}

        uintptr_t {name}_ptr = (uintptr_t)jerry_value_as_number({name}_ptr_val);
        jerry_value_free({name}_ptr_val);
        {name} = (void*){name}_ptr;
    }}

"""

def generate_string_arg_parsing(index, name):
    """生成字符串类型参数解析代码，支持null"""
    return fr"""    // 字符串类型参数，支持null
    char* {name} = NULL;
    if (!jerry_value_is_undefined(args[{index}]) && !jerry_value_is_null(args[{index}])) {{
        jerry_value_t js_{name} = args[{index}];
        if (!jerry_value_is_string(js_{name})) {{
            return throw_error("Argument {index} must be a string or null");
        }}

        jerry_size_t {name}_len = jerry_string_size(js_{name}, JERRY_ENCODING_UTF8);
        {name} = (char*)malloc({name}_len + 1);
        if (!{name}) {{
            return throw_error("Out of memory");
        }}
        jerry_string_to_buffer(js_{name}, JERRY_ENCODING_UTF8, (jerry_char_t*){name}, {name}_len);
        {name}[{name}_len] = '\0';
    }}

"""

def generate_number_arg_parsing(index, name, type_str):
    """生成数字类型参数解析代码"""
    return fr"""    jerry_value_t js_{name} = args[{index}];
    if (!jerry_value_is_number(js_{name})) {{
        return throw_error("Argument {index} must be a number");
    }}

    {type_str} {name} = ({type_str})jerry_value_as_number(js_{name});

"""

def generate_bool_arg_parsing(index, name):
    """生成布尔类型参数解析代码"""
    return fr"""    jerry_value_t js_{name} = args[{index}];
    if (!jerry_value_is_boolean(js_{name})) {{
        return throw_error("Argument {index} must be a boolean");
    }}

    bool {name} = jerry_value_to_boolean(js_{name});

"""

def generate_arg_parsing(index, name, arg_type, type_info, typedefs_data):
    """根据类型信息生成参数解析代码"""
    type_str = arg_type.replace(' ', '')
    var_name = f"arg_{name}"  # 添加arg_前缀避免冲突

    # 1. 特殊处理lv_font_t指针
    if type_info.get('is_font_pointer'):
        return fr"""    // lv_font_t* 类型参数处理
    const lv_font_t* {var_name} = NULL;
    if (!jerry_value_is_undefined(args[{index}]) && !jerry_value_is_null(args[{index}])) {{
        jerry_value_t js_{var_name} = args[{index}];
        if (!jerry_value_is_object(js_{var_name})) {{
            return throw_error("Argument {index} must be a font object or null");
        }}

        // 检查类型标记
        jerry_value_t type_prop = jerry_string_sz("__type");
        jerry_value_t type_val = jerry_object_get(js_{var_name}, type_prop);
        jerry_value_free(type_prop);

        jerry_size_t type_len = jerry_string_size(type_val, JERRY_ENCODING_UTF8);
        char type_str[32];
        jerry_string_to_buffer(type_val, JERRY_ENCODING_UTF8, (jerry_char_t*)type_str, type_len);
        type_str[type_len] = '\0';
        jerry_value_free(type_val);

        if (strcmp(type_str, "lv_font") != 0) {{
            return throw_error("Argument {index} must be a font object");
        }}

        // 获取字体指针
        jerry_value_t ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t ptr_val = jerry_object_get(js_{var_name}, ptr_prop);
        jerry_value_free(ptr_prop);

        if (!jerry_value_is_number(ptr_val)) {{
            jerry_value_free(ptr_val);
            return throw_error("Invalid font pointer");
        }}

        uintptr_t ptr = (uintptr_t)jerry_value_as_number(ptr_val);
        jerry_value_free(ptr_val);
        {var_name} = (const lv_font_t*)ptr;
    }}

"""
    # 特殊处理lv_event_t指针
    if type_info.get('is_event_pointer'):
        return fr"""    // lv_event_t* 类型参数处理
    lv_event_t* {var_name} = NULL;
    if (!jerry_value_is_undefined(args[{index}]) && !jerry_value_is_null(args[{index}])) {{
        jerry_value_t js_{var_name} = args[{index}];
        if (!jerry_value_is_object(js_{var_name})) {{
            return throw_error("Argument {index} must be an event object");
        }}

        // 检查类型标记
        jerry_value_t type_prop = jerry_string_sz("__type");
        jerry_value_t type_val = jerry_object_get(js_{var_name}, type_prop);
        jerry_value_free(type_prop);

        jerry_size_t type_len = jerry_string_size(type_val, JERRY_ENCODING_UTF8);
        char type_str[32];
        jerry_string_to_buffer(type_val, JERRY_ENCODING_UTF8, (jerry_char_t*)type_str, type_len);
        type_str[type_len] = '\0';
        jerry_value_free(type_val);

        if (strcmp(type_str, "lv_event") != 0) {{
            return throw_error("Argument {index} must be an event object");
        }}

        // 获取事件指针
        jerry_value_t ptr_prop = jerry_string_sz("__event_ptr");
        jerry_value_t ptr_val = jerry_object_get(js_{var_name}, ptr_prop);
        jerry_value_free(ptr_prop);

        if (!jerry_value_is_number(ptr_val)) {{
            jerry_value_free(ptr_val);
            return throw_error("Invalid event pointer");
        }}

        uintptr_t ptr = (uintptr_t)jerry_value_as_number(ptr_val);
        jerry_value_free(ptr_val);
        {var_name} = (lv_event_t*)ptr;
    }}

"""
    # 2. 特殊处理lv_obj_t指针
    if is_lv_obj_pointer(type_str):
        return generate_object_arg_parsing(index, var_name)

    # 3. 处理基本类型
    if is_void_type(type_str):
        return f"    // void 类型跳过解析\n"

    # 4. 处理void指针
    if is_void_pointer(type_str):
        return generate_void_pointer_arg_parsing(index, var_name)

    # 5. 处理对象指针
    if is_object_pointer(type_str):
        return generate_object_arg_parsing(index, var_name)

    # 6. 处理字符串指针
    if is_string_pointer(type_str):
        return generate_string_arg_parsing(index, var_name)

    # 7. 处理lv_color_t
    if is_lv_color_t(type_str):
        return f"    lv_color_t {var_name} = js_to_lv_color(args[{index}]);\n\n"

    # 8. 处理布尔类型
    if type_str == 'bool' or (is_typedef_convertible_to_basic(type_info.get('type_name', ''), typedefs_data)
                             and get_typedef_base_type(type_info['type_name'], typedefs_data) == 'bool'):
        return fr"""    // 布尔类型参数: {name}
    bool {var_name} = false;
    if (!jerry_value_is_undefined(args[{index}])) {{
        if (jerry_value_is_boolean(args[{index}])) {{
            {var_name} = jerry_value_to_boolean(args[{index}]);
        }}
        else if (jerry_value_is_number(args[{index}])) {{
            {var_name} = (jerry_value_as_number(args[{index}]) != 0);
        }}
        else {{
            return throw_error("Argument {index} must be boolean or number for {type_str}");
        }}
    }}

"""

    # 9. 处理数字类型
    if is_number_type(type_str):
        return generate_number_arg_parsing(index, var_name, type_str)

    # 10. 检查typedef类型
    type_name = type_info.get('type_name', '')
    if type_name:
        base_type = get_typedef_base_type(type_name, typedefs_data)
        if base_type:
            if base_type == 'bool':
                return generate_bool_arg_parsing(index, var_name)
            elif is_number_type(base_type):
                return generate_number_arg_parsing(index, var_name, base_type)

    # 11. 默认处理为通用指针
    return generate_generic_pointer_arg_parsing(index, var_name, type_str)

def find_real_function_definition(func_name, data):
    """
    查找函数的真实定义，处理宏定义的情况
    返回 (真实函数名, 函数定义, is_macro)
    """
    # 首先检查是否是宏定义
    for macro in data.get('macros', []):
        if macro['name'] == func_name and 'initializer' in macro:
            # 提取宏定义的底层函数名
            real_func_name = macro['initializer'].strip()
            if real_func_name.endswith(';'):
                real_func_name = real_func_name[:-1].strip()

            # 查找底层函数定义
            for func in data.get('functions', []):
                if func['name'] == real_func_name:
                    return (real_func_name, func, True)

            # 如果没有找到函数定义，尝试查找函数指针
            for func_ptr in data.get('function_pointers', []):
                if func_ptr['name'] == real_func_name:
                    return (real_func_name, func_ptr, True)

    # 如果不是宏定义，直接查找函数定义
    for func in data.get('functions', []):
        if func['name'] == func_name:
            return (func_name, func, False)

    # 查找函数指针定义
    for func_ptr in data.get('function_pointers', []):
        if func_ptr['name'] == func_name:
            return (func_name, func_ptr, False)

    return (None, None, False)

def generate_binding_function(func, typedefs_data, export_functions):
    """生成绑定函数实现（完整支持字符串/对象/数值参数）"""
    func_name = func['name']

    # 查找真实函数定义
    real_func_name, real_func_def, is_macro = find_real_function_definition(func_name, typedefs_data)

    # 调试信息：显示原始查找结果
    if print_all_info:
        if is_macro:
            print(f"{Fore.CYAN}[调试] 函数 {func_name} 是宏，底层实现为 {real_func_name}{Style.RESET_ALL}")
        elif real_func_def:
            print(f"{Fore.CYAN}[调试] 函数 {func_name} 是普通函数{Style.RESET_ALL}")
        else:
            print(f"{Fore.YELLOW}[调试] 未找到函数 {func_name} 的定义{Style.RESET_ALL}")

    # 如果函数名在导出列表中，尝试强制查找宏定义
    if func_name in export_functions and not is_macro:
        # 查找是否有同名的宏定义
        for macro in typedefs_data.get('macros', []):
            if macro['name'] == func_name and 'initializer' in macro:
                # 提取宏定义的底层函数名
                real_func_name = macro['initializer'].strip()
                if real_func_name.endswith(';'):
                    real_func_name = real_func_name[:-1].strip()

                # 查找底层函数定义
                for func_def in typedefs_data.get('functions', []):
                    if func_def['name'] == real_func_name:
                        real_func_def = func_def
                        is_macro = True
                        if print_all_info:
                            print(f"{Fore.CYAN}[调试] 强制将 {func_name} 作为宏处理，底层实现为 {real_func_name}{Style.RESET_ALL}")
                        break
                else:
                    # 查找函数指针定义
                    for func_ptr in typedefs_data.get('function_pointers', []):
                        if func_ptr['name'] == real_func_name:
                            real_func_def = func_ptr
                            is_macro = True
                            if print_all_info:
                                print(f"{Fore.CYAN}[调试] 强制将 {func_name} 作为宏处理，底层实现为函数指针 {real_func_name}{Style.RESET_ALL}")
                            break

    if not real_func_def:
        print(f"{Fore.RED}[错误] 未找到函数 {func_name} 的定义，无法生成绑定{Style.RESET_ALL}")
        return ""

    # 解析返回类型
    return_type = 'void'
    return_type_info = {'is_void': True}
    if 'type' in real_func_def:
        return_type, return_type_info = parse_type(real_func_def['type'])

    # 解析参数
    args = real_func_def.get('args', [])
    if len(args) == 1 and is_void_type(parse_type(args[0].get('type', {}))[0]):
        args = []

    docstring = real_func_def.get('docstring', '').strip() or f"{func_name} function"

    # 生成函数头
    code = f"""
/**
 * @brief {docstring}
 */
static jerry_value_t js_{func_name}(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {{
"""

    # 参数检查
    if args:
        code += f"    // 参数数量检查\n"
        code += f"    if (argc < {len(args)}) {{\n"
        code += f"        return throw_error(\"{func_name}: Insufficient arguments\");\n"
        code += f"    }}\n\n"

    # 参数解析
    string_vars = []  # 记录需要释放的字符串变量
    arg_var_names = []  # 用于函数调用的参数名

    for i, arg in enumerate(args):
        arg_name = arg.get('name', f"arg{i}")
        arg_type = 'void'
        arg_type_info = {'is_void': True}

        if 'type' in arg:
            arg_type, arg_type_info = parse_type(arg['type'])

        # 特殊处理lv_event_get_user_data函数的参数
        if func_name == 'lv_event_get_user_data' and i == 0:
            arg_type = 'lv_event_t*'
            arg_type_info = {'is_event_pointer': True}

        code += f"    // 解析参数: {arg_name} ({arg_type})\n"

        # 为变量名添加前缀
        var_name = f"arg_{arg_name}"

        # 特殊处理字符串指针
        if is_string_pointer(arg_type):
            str_var = f"{var_name}_str"
            string_vars.append(str_var)
            code += fr"""
    char* {str_var} = NULL;
    const char* {var_name} = NULL;
    if (!jerry_value_is_undefined(args[{i}]) && !jerry_value_is_null(args[{i}])) {{
        if (!jerry_value_is_string(args[{i}])) {{
            return throw_error("Argument {i} must be a string");
        }}
        jerry_size_t {var_name}_len = jerry_string_size(args[{i}], JERRY_ENCODING_UTF8);
        {str_var} = (char*)malloc({var_name}_len + 1);
        jerry_string_to_buffer(args[{i}], JERRY_ENCODING_UTF8, (jerry_char_t*){str_var}, {var_name}_len);
        {str_var}[{var_name}_len] = '\0';
        {var_name} = {str_var};
    }}

"""
        else:
            # 其他类型使用通用解析
            code += generate_arg_parsing(i, arg_name, arg_type, arg_type_info, typedefs_data)

        arg_var_names.append(var_name)

    # 函数调用
    code += "    // 调用底层函数\n"

    if return_type == 'void':
        code += f"    {real_func_name}({', '.join(arg_var_names)});\n"
    else:
        code += f"    {return_type} ret_value = {real_func_name}({', '.join(arg_var_names)});\n\n"
        code += "    // 处理返回值\n"

        # 使用临时变量存储返回值
        code += "    jerry_value_t js_result;\n"

        if is_void_pointer(return_type):
            code += "    // 包装为通用指针对象\n"
            code += "    js_result = jerry_object();\n"
            code += "    jerry_value_t ptr_key = jerry_string_sz(\"__ptr\");\n"
            code += "    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);\n"
            code += "    jerry_value_t type_key = jerry_string_sz(\"__type\");\n"
            code += "    jerry_value_t type = jerry_string_sz(\"void*\");\n"
            code += "    jerry_value_free(jerry_object_set(js_result, ptr_key, ptr));\n"
            code += "    jerry_value_free(jerry_object_set(js_result, type_key, type));\n"
            code += "    jerry_value_free(ptr_key);\n"
            code += "    jerry_value_free(ptr);\n"
            code += "    jerry_value_free(type_key);\n"
            code += "    jerry_value_free(type);\n"
        elif is_lv_color_t(return_type):
            code += "    // 转换为JS颜色对象\n"
            code += "    js_result = lv_color_to_js(ret_value);\n"
        elif is_lv_obj_pointer(return_type):
            code += "    // 包装为LVGL对象\n"
            code += "    js_result = jerry_object();\n"
            code += "    jerry_value_t ptr_key = jerry_string_sz(\"__ptr\");\n"
            code += "    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);\n"
            code += "    jerry_value_t cls_key = jerry_string_sz(\"__class\");\n"
            code += "    jerry_value_t cls = jerry_string_sz(\"lv_obj\");\n"
            code += "    jerry_value_free(jerry_object_set(js_result, ptr_key, ptr));\n"
            code += "    jerry_value_free(jerry_object_set(js_result, cls_key, cls));\n"
            code += "    jerry_value_free(ptr_key);\n"
            code += "    jerry_value_free(ptr);\n"
            code += "    jerry_value_free(cls_key);\n"
            code += "    jerry_value_free(cls);\n"
        elif is_string_pointer(return_type):
            code += "    if (ret_value == NULL) {\n"
            code += "        js_result = jerry_string_sz(\"\");\n"
            code += "    } else {\n"
            code += "        js_result = jerry_string_sz((const jerry_char_t*)ret_value);\n"
            code += "    }\n"
        elif is_number_type(return_type) or is_lvgl_value_type(return_type):
            code += "    js_result = jerry_number(ret_value);\n"
        elif return_type == 'bool':
            code += "    js_result = jerry_boolean(ret_value);\n"
        else:
            # 默认处理为通用指针
            code += "    js_result = jerry_object();\n"
            code += "    jerry_value_t ptr_key = jerry_string_sz(\"__ptr\");\n"
            code += "    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);\n"
            code += "    jerry_value_free(jerry_object_set(js_result, ptr_key, ptr));\n"
            code += "    jerry_value_free(ptr);\n"
            code += "    jerry_value_free(ptr_key);\n"

    # 释放临时字符串内存（确保在函数调用之后）
    if string_vars:
        code += "\n    // 释放临时字符串内存\n"
        for var in string_vars:
            code += f"    if ({var}) free({var});\n"

    # 添加返回值
    if return_type == 'void':
        code += "\n    return jerry_undefined();\n"
    else:
        code += "\n    return js_result;\n"

    code += "}\n"

    # 打印成功信息
    if is_macro:
        print(f"{Fore.GREEN}[成功] 生成宏函数绑定: {func_name} -> {real_func_name}{Style.RESET_ALL}")
    else:
        print(f"{Fore.GREEN}[成功] 生成函数绑定: {func_name}{Style.RESET_ALL}")

    return code

def generate_native_funcs_list(functions, macro_alias_map=None):
    """生成原生函数列表数组，支持宏别名（旧API映射到新API）"""
    entries = []
    added_names = set()

    for func in functions:
        func_name = func['name']
        # 支持宏别名
        if macro_alias_map and func_name in macro_alias_map:
            real_func_name = macro_alias_map[func_name]
            # 旧API条目
            if func_name not in added_names:
                entries.append(f'    {{ "{func_name}", js_{real_func_name} }}')
                added_names.add(func_name)
            # 新API条目
            if real_func_name not in added_names:
                entries.append(f'    {{ "{real_func_name}", js_{real_func_name} }}')
                added_names.add(real_func_name)
        else:
            if func_name not in added_names:
                entries.append(f'    {{ "{func_name}", js_{func_name} }}')
                added_names.add(func_name)

    entries_str = ',\n'.join(entries)
    return f"""
const LVBindingJerryscriptFuncEntry_t lvgl_binding_funcs[] = {{
{entries_str}
}};

const unsigned int lvgl_binding_funcs_count = {len(entries)};
"""

def load_export_macros(file_path):
    """从文件加载宏导出列表"""
    patterns = []
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    patterns.append(line)
        print(f"{Fore.GREEN}[加载] 从 {file_path} 读取了 {len(patterns)} 个导出宏模式{Style.RESET_ALL}")
    else:
        print(f"{Fore.YELLOW}[提示] 未找到宏导出列表文件: {file_path}{Style.RESET_ALL}")
    return patterns

def load_blacklist_macros(file_path):
    """从文件加载宏黑名单"""
    patterns = []
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    patterns.append(line)
        print(f"{Fore.GREEN}[加载] 从 {file_path} 读取了 {len(patterns)} 个黑名单宏模式{Style.RESET_ALL}")
    else:
        print(f"{Fore.YELLOW}[提示] 未找到宏黑名单文件: {file_path}{Style.RESET_ALL}")
    return patterns

def load_export_functions(file_path):
    """加载导出函数列表（支持通配符）"""
    patterns = []
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    patterns.append(line)
        print(f"{Fore.GREEN}[加载] 从 {file_path} 读取了 {len(patterns)} 个导出函数模式{Style.RESET_ALL}")
    else:
        print(f"{Fore.YELLOW}[警告] 未找到函数导出列表文件: {file_path}{Style.RESET_ALL}")
    return patterns

def load_blacklist_functions(file_path):
    """加载黑名单函数列表（支持通配符）"""
    patterns = []
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    patterns.append(line)
        print(f"{Fore.GREEN}[加载] 从 {file_path} 读取了 {len(patterns)} 个黑名单函数模式{Style.RESET_ALL}")
    else:
        print(f"{Fore.YELLOW}[提示] 未找到黑名单函数列表文件: {file_path}{Style.RESET_ALL}")
    return patterns

def is_function_matched(func_name, patterns):
    """检查函数名是否匹配任意模式"""
    for pattern in patterns:
        if fnmatch.fnmatchcase(func_name, pattern):
            return True
    return False

def extract_lvgl_functions_from_file(file_path):
    """从文件中提取所有以lv_开头并以(结尾的函数名"""
    lvgl_functions = set()

    # 定义警告函数列表
    warning_functions = {
        'lv_obj_add_event_cb',
    }

    if not os.path.exists(file_path):
        print(f"{Fore.YELLOW}[警告] 未找到文件: {file_path}{Style.RESET_ALL}")
        return lvgl_functions

    # 支持.js文件
    if not file_path.endswith('.js'):
        print(f"{Fore.YELLOW}[警告] 仅支持.js文件: {file_path}{Style.RESET_ALL}")
        return lvgl_functions

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

            # 使用正则表达式匹配lv_开头并以(结尾的函数调用
            import re
            pattern = r'\blv_\w+\('
            matches = re.findall(pattern, content)

            # 提取函数名（去掉左括号）
            for match in matches:
                func_name = match[:-1]
                lvgl_functions.add(func_name)

                # 检查是否是警告函数
                if func_name in warning_functions:
                    print(f"{Fore.RED}[安全警告] 检测到潜在风险函数调用: {func_name}{Style.RESET_ALL}")

        print(f"{Fore.GREEN}[提取完成] 从 {file_path} 中提取到 {len(lvgl_functions)} 个lvgl函数{Style.RESET_ALL}")
        return lvgl_functions

    except Exception as e:
        print(f"{Fore.RED}[错误] 处理文件 {file_path} 时出错: {str(e)}{Style.RESET_ALL}")
        return lvgl_functions

def generate_enum_binding(enums, macros=None, export_macros=None, blacklist_macros=None):
    """生成枚举绑定代码，处理enums和macros，直接注册到全局作用域"""
    lines = []
    lines.append("static void register_lvgl_enums(void) {")
    lines.append("    jerry_value_t global = jerry_current_realm();")

    # 1. 首先处理所有枚举值（保持不变）
    for enum in enums or []:
        if enum is None:
            continue
        for member in enum.get('members', []) or []:
            if member is None:
                continue
            name = member.get('name')
            value = member.get('value')
            if name and value is not None:
                try:
                    if isinstance(value, str):
                        value = value.strip()
                        if value.startswith('0x'):
                            val = int(value, 16)
                        elif value.startswith('0'):
                            val = int(value, 8)
                        else:
                            val = int(value)
                    else:
                        val = int(value)
                    lines.append(f'    lvgl_binding_set_enum(global, "{name}", {val});')
                except (ValueError, TypeError) as e:
                    print(f"{Fore.YELLOW}[警告] 无法解析枚举值 {name}={value}: {e}{Style.RESET_ALL}")
                    continue

    if macros:
        # 特殊处理LV_SYMBOL_开头的宏（总是包含）
        for macro in macros:
            if not macro or not isinstance(macro, dict):
                continue

            macro_name = macro.get('name')
            if not macro_name:
                continue

            # 跳过带参数的宏
            if macro.get('params'):
                if print_macro_info:
                    print(f"{Fore.BLUE}[跳过] 带参数的宏 {macro_name}{Style.RESET_ALL}")
                continue

            # 检查是否在黑名单中
            if is_function_matched(macro_name, blacklist_macros):
                if print_macro_info:
                    print(f"{Fore.RED}[黑名单] 跳过宏 {macro_name}{Style.RESET_ALL}")
                continue

            # 特殊处理LV_SYMBOL_开头的宏
            if macro_name.startswith('LV_SYMBOL_'):
                lines.append(f"#ifdef {macro_name}")
                lines.append(f'    jerry_value_t {macro_name}_str = jerry_string_sz("{macro_name}");')
                lines.append(f"    jerry_value_t {macro_name}_val = jerry_string_sz({macro_name});")
                lines.append(f'    jerry_value_free(jerry_object_set(global, {macro_name}_str, {macro_name}_val));')
                lines.append(f"    jerry_value_free({macro_name}_str);")
                lines.append(f"    jerry_value_free({macro_name}_val);")
                lines.append("#else")
                lines.append(f'    #pragma message("WARNING: Macro {macro_name} is not defined")')
                lines.append("#endif")
                continue

            # 检查是否在导出列表中
            if is_function_matched(macro_name, export_macros):
                lines.append(f"#ifdef {macro_name}")
                lines.append(f'    lvgl_binding_set_enum(global, "{macro_name}", {macro_name});')
                lines.append("#else")
                lines.append(f'    #pragma message("WARNING: Macro {macro_name} is not defined")')
                lines.append("#endif")
            elif print_macro_info:
                print(f"{Fore.BLUE}[跳过] 宏 {macro_name} 不在导出列表中{Style.RESET_ALL}")

    lines.append("    jerry_value_free(global);")
    lines.append("}")
    return "\n".join(lines)

def main():
    # 如果指定了--extract-funcs-from参数，先执行提取功能
    if extract_funcs_from:
        extracted_funcs = extract_lvgl_functions_from_file(extract_funcs_from)
        if extracted_funcs:
            # 加载现有的导出函数列表
            existing_funcs = set()
            if os.path.exists(export_functions_path):
                with open(export_functions_path, 'r', encoding='utf-8') as f:
                    existing_funcs = {line.strip() for line in f if line.strip() and not line.startswith('#')}

            # 找出需要添加的新函数
            new_funcs = extracted_funcs - existing_funcs

            if new_funcs:
                # 追加新函数到文件
                with open(export_functions_path, 'a', encoding='utf-8') as f:
                    for func in sorted(new_funcs):
                        f.write(f"{func}\n")
                print(f"{Fore.GREEN}[更新完成] 向 {export_functions_path} 中添加了 {len(new_funcs)} 个新函数{Style.RESET_ALL}")
            else:
                print(f"{Fore.GREEN}[无需更新] 没有发现新的lvgl函数需要添加{Style.RESET_ALL}")

    # 从同级目录读取lvgl.json
    if not os.path.exists(input_json_file):
        print(f"{Fore.RED}[错误] 找不到输入文件: {input_json_file}{Style.RESET_ALL}")
        return

    with open(input_json_file, 'r') as f:
        data = json.load(f)

    # 生成绑定函数
    binding_code = ""
    EXPORT_FUNCTION_PATTERNS = load_export_functions(export_functions_path)
    BLACKLIST_FUNCTION_PATTERNS = load_blacklist_functions(blacklist_functions_path)
    EXPORT_MACROS = load_export_macros(export_macros_path)
    BLACKLIST_MACROS = load_blacklist_macros(blacklist_macros_path)

    # 收集所有需要导出的函数（匹配白名单且不在黑名单中的函数）
    exported_funcs = []
    matched_funcs = set()
    missing_funcs = set(EXPORT_FUNCTION_PATTERNS)  # 初始化包含所有导出模式

    # 1. 首先处理所有函数
    for func in data.get('functions', []):
        func_name = func['name']
        if func_name in matched_funcs:
            continue

        # 检查是否在导出模式中
        if not is_function_matched(func_name, EXPORT_FUNCTION_PATTERNS):
            continue

        # 检查是否在黑名单中
        if is_function_matched(func_name, BLACKLIST_FUNCTION_PATTERNS):
            print(f"{Fore.RED}[黑名单] 跳过函数 {func_name}{Style.RESET_ALL}")
            missing_funcs.discard(func_name)
            continue

        exported_funcs.append(func)
        matched_funcs.add(func_name)
        missing_funcs.discard(func_name)  # 从缺失列表中移除

    # 2. 处理宏定义（作为函数导出）
    for macro in data.get('macros', []):
        macro_name = macro.get('name')
        if not macro_name or macro_name in matched_funcs:
            continue

        # 检查是否在导出模式中
        if not is_function_matched(macro_name, EXPORT_FUNCTION_PATTERNS):
            continue

        # 检查是否在黑名单中
        if is_function_matched(macro_name, BLACKLIST_FUNCTION_PATTERNS):
            print(f"{Fore.RED}[黑名单] 跳过宏 {macro_name}{Style.RESET_ALL}")
            missing_funcs.discard(macro_name)
            continue

        # 检查是否有initializer（函数型宏）
        if 'initializer' in macro:
            # 创建一个伪函数定义
            pseudo_func = {
                'name': macro_name,
                'type': {'name': 'void'},  # 默认类型，实际会从底层函数获取
                'args': [],
                'docstring': f"Macro wrapper for {macro_name}"
            }
            exported_funcs.append(pseudo_func)
            matched_funcs.add(macro_name)
            missing_funcs.discard(macro_name)
        else:
            print(f"{Fore.YELLOW}[警告] 宏 {macro_name} 没有initializer，无法作为函数处理{Style.RESET_ALL}")
            missing_funcs.discard(macro_name)

    # 打印未生成的函数
    if missing_funcs:
        print(f"\n{Fore.YELLOW}[警告] 以下函数在export_functions.txt中指定但未生成:{Style.RESET_ALL}")
        for func in sorted(missing_funcs):
            print(f"  - {func} (未在lvgl.json中找到定义)")

    if not exported_funcs:
        print(f"{Fore.RED}[错误] 没有找到匹配的导出函数，请检查 export_functions.txt 文件。匹配模式: {EXPORT_FUNCTION_PATTERNS}{Style.RESET_ALL}")
        return

    # 处理宏别名映射
    macro_alias_map = {}
    for macro in data.get('macros', []):
        macro_name = macro.get('name')
        initializer = macro.get('initializer', '').strip() if macro.get('initializer') else None
        # 只处理无参数宏，且initializer是另一个API名
        if macro_name and initializer and initializer.startswith('lv_') and macro_name != initializer:
            macro_alias_map[macro_name] = initializer

    # 只生成新API的声明和实现
    real_func_names = set()
    for func in exported_funcs:
        func_name = func['name']
        if macro_alias_map.get(func_name):
            real_func_names.add(macro_alias_map[func_name])
        else:
            real_func_names.add(func_name)

    # 生成函数声明（只用新API名）
    func_decls = ''.join([
        f"static jerry_value_t js_{name}(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);\n"
        for name in sorted(real_func_names)
    ])

    # 生成函数实现（只用新API名）
    binding_code = ""
    for name in sorted(real_func_names):
        # 优先用新API名查找真实定义
        real_func = None
        # 先查新API名
        for func in exported_funcs:
            # 如果是宏别名，func['name']是旧API，macro_alias_map[func['name']]是新API
            if macro_alias_map.get(func['name'], None) == name:
                real_func = func
                break
            # 如果本身就是新API
            if func['name'] == name:
                real_func = func
                break
        # 生成实现体，函数名始终用新API名
        if real_func:
            # 复制一份func并改名为新API名
            func_for_impl = dict(real_func)
            func_for_impl['name'] = name
            binding_code += generate_binding_function(func_for_impl, data, EXPORT_FUNCTION_PATTERNS)
            binding_code += "\n\n"
        else:
            print(f"{Fore.RED}[警告] 未找到 {name} 的函数定义，无法生成实现体{Style.RESET_ALL}")

    # 生成函数列表，传入宏别名映射
    func_list = generate_native_funcs_list(exported_funcs, macro_alias_map)

    # 从JSON数据中获取enums和macros
    enums = data.get('enums', [])
    macros = data.get('macros', [])

    # 生成枚举绑定
    enum_binding = generate_enum_binding(
        enums,
        macros,
        EXPORT_MACROS,
        BLACKLIST_MACROS
    )

    # 构建完整的C代码
    output = (
        HEADER_CODE +
        "// 函数声明\n" +
        func_decls + "\n" +
        "// 函数实现\n" +
        binding_code +
        func_list + "\n" +
        enum_binding + "\n" +
        INIT_FUNCTION_CODE
    )

    # 输出到.c文件
    os.makedirs(os.path.dirname(output_c_file), exist_ok=True)
    with open(output_c_file, "w", encoding="utf-8") as f:
        f.write(output)
    print(f"{Fore.GREEN}✅ 生成C代码已写入: {output_c_file}{Style.RESET_ALL}")

if __name__ == "__main__":
    for arg in sys.argv:
        if arg.startswith('--json-file='):
            try:
                input_json_file = arg.split('=', 1)[1]
            except:
                print("{Fore.RED}[错误] lvgl.json文件不存在。")
                exit()
        elif arg.startswith('--output-c-path='):
            output_c_file = arg.split('=', 1)[1]
            output_c_file = output_c_file+"/lv_bindings.c"
        elif arg.startswith('--extract-funcs-from='):
            extract_funcs_from = arg.split('=', 1)[1]
        elif arg.startswith('--cfg-path='):
            configuration_path = arg.split('=', 1)[1]
            export_functions_path = os.path.join(script_dir, configuration_path+"/export_functions.txt")
            blacklist_functions_path = os.path.join(script_dir, configuration_path+"/blacklist_functions.txt")
            export_macros_path = os.path.join(script_dir, configuration_path+"/export_macros.txt")
            blacklist_macros_path = os.path.join(script_dir, configuration_path+"/blacklist_macros.txt")
    main()
