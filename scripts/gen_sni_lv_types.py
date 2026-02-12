#!/usr/bin/env python3
import json
import sys
import datetime

# 类型映射字典
type_mapping = {
    'int': 'SNI_T_INT32',
    'int32_t': 'SNI_T_INT32',
    'uint32_t': 'SNI_T_UINT32',
    'int16_t': 'SNI_T_INT32',
    'uint16_t': 'SNI_T_UINT32',
    'int8_t': 'SNI_T_INT32',
    'uint8_t': 'SNI_T_UINT32',
    'float': 'SNI_T_DOUBLE',
    'double': 'SNI_T_DOUBLE',
    'bool': 'SNI_T_BOOL',
}


def add_sni_value_type(sni_types_path, sni_v_name):
    """
    Add a SNI_V_* type to sni_types.h file
    Returns True if added, False if already exists
    """
    try:
        with open(sni_types_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File {sni_types_path} not found", file=sys.stderr)
        sys.exit(1)

    # Check if the type already exists
    if sni_v_name in content:
        return False

    # Find __SNI_VALUE_END position
    end_marker = '__SNI_VALUE_END'
    end_pos = content.find(end_marker)
    if end_pos == -1:
        print(f"Error: Cannot find {end_marker} in {sni_types_path}", file=sys.stderr)
        sys.exit(1)

    # Find line before __SNI_VALUE_END to get correct indentation
    # Look for last newline before __SNI_VALUE_END
    newline_pos = content.rfind('\n', 0, end_pos)
    if newline_pos == -1:
        # No newline found, use default tab
        indent = '\t'
    else:
        # Get indentation from previous line
        # Find first SNI_V_* type after __SNI_VALUE_START
        import re
        # Look for __SNI_VALUE_START followed by newline and then SNI_V_*
        pattern = r'__SNI_VALUE_START,\s*\n([ \t]*)SNI_V_[A-Z0-9_]+,'
        match = re.search(pattern, content[:end_pos])
        if match:
            # Use same indentation as first SNI_V_* type
            indent = match.group(1)
        else:
            # Use default tab
            indent = '\t'

    # Insert the new type before __SNI_VALUE_END with proper indentation
    # Find the line before __SNI_VALUE_END
    before_end = content.rfind('\n', 0, end_pos)
    if before_end == -1:
        # No newline found, just insert before __SNI_VALUE_END
        insert_pos = end_pos
    else:
        # Insert after the last newline before __SNI_VALUE_END
        insert_pos = before_end + 1

    # Insert new type with proper indentation
    insert_text = f"{indent}{sni_v_name},\n"
    new_content = content[:insert_pos] + insert_text + content[insert_pos:]

    # Write back to file
    try:
        with open(sni_types_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Added '{sni_v_name}' to {sni_types_path}")
        return True
    except Exception as e:
        print(f"Error writing to file: {e}", file=sys.stderr)
        sys.exit(1)


def extract_sni_value_types(sni_types_path):
    """
    Extract SNI_V_* types from sni_types.h file
    Returns a set of SNI_V_* type names
    """
    try:
        with open(sni_types_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File {sni_types_path} not found", file=sys.stderr)
        sys.exit(1)

    # Find the section between __SNI_VALUE_START and __SNI_VALUE_END
    start_marker = '__SNI_VALUE_START'
    end_marker = '__SNI_VALUE_END'

    start_pos = content.find(start_marker)
    if start_pos == -1:
        print(f"Error: Cannot find {start_marker} in {sni_types_path}", file=sys.stderr)
        sys.exit(1)

    end_pos = content.find(end_marker, start_pos)
    if end_pos == -1:
        print(f"Error: Cannot find {end_marker} in {sni_types_path}", file=sys.stderr)
        sys.exit(1)

    # Extract the section
    section = content[start_pos:end_pos]

    # Find all SNI_V_* types
    import re
    pattern = r'SNI_V_[A-Z0-9_]+'
    matches = re.findall(pattern, section)

    return set(matches)


def find_underlying_type(type_name, type_definitions, visited=None):
    """
    Find the underlying type of a given type name by recursively looking up typedefs
    Returns the SNI type if found in type_mapping, None otherwise
    """
    if visited is None:
        visited = set()

    # Check if we've already visited this type (avoid infinite recursion)
    if type_name in visited:
        return None
    visited.add(type_name)

    # Check if type is already in type_mapping
    if type_name in type_mapping:
        return type_mapping[type_name]

    # Check if type exists in type_definitions
    if type_name not in type_definitions:
        return None

    type_def = type_definitions[type_name]

    # Check if it's a typedef with a type field
    if 'type' in type_def:
        underlying_type = type_def['type']
        # Handle case where type is a dict
        if isinstance(underlying_type, dict):
            if 'name' in underlying_type:
                underlying_type = underlying_type['name']
            else:
                return None
        # Recursively find the underlying type
        return find_underlying_type(underlying_type, type_definitions, visited)

    return None


def generate_sni_lv_types(lv_types_json, lvgl_json, output_file, sni_types_path):
    """
    Generate sni_lv_types.c file
    """
    # Extract SNI_V_* types from sni_types.h
    sni_value_types = extract_sni_value_types(sni_types_path)

    # Load lv_types_output.json
    try:
        with open(lv_types_json, 'r', encoding='utf-8') as f:
            types_data = json.load(f)
    except FileNotFoundError:
        print(f"Error: File {lv_types_json} not found", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON in {lv_types_json}", file=sys.stderr)
        sys.exit(1)

    # Load lvgl.json
    try:
        with open(lvgl_json, 'r', encoding='utf-8') as f:
            lvgl_data = json.load(f)
    except FileNotFoundError:
        print(f"Error: File {lvgl_json} not found", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON in {lvgl_json}", file=sys.stderr)
        sys.exit(1)

    # Collect type definitions from lvgl.json
    type_definitions = {}
    # Check structures
    if 'structures' in lvgl_data:
        for struct in lvgl_data['structures']:
            if 'name' in struct:
                type_definitions[struct['name']] = struct
    # Check unions
    if 'unions' in lvgl_data:
        for union in lvgl_data['unions']:
            if 'name' in union:
                type_definitions[union['name']] = union
    # Check typedefs
    if 'typedefs' in lvgl_data:
        for typedef in lvgl_data['typedefs']:
            if 'name' in typedef:
                type_definitions[typedef['name']] = typedef

    # Extract value types from lv_types_output.json
    value_types = []
    if 'types' in types_data:
        for item in types_data['types']:
            if 'name' in item and item.get('object_type', '') == 'value':
                value_types.append(item['name'])

    # Generate code
    code = []

    # Add header
    code.append("/**")
    code.append(" * @file sni_lv_types.c")
    code.append(" * @brief LVGL 类型表")
    code.append(" * @author Auto-generated")
    code.append(f" * @date {datetime.datetime.now().strftime('%Y-%m-%d')}")
    code.append(" */")
    code.append("")
    code.append("#include \"sni_lv_types.h\"")
    code.append("")
    code.append("/* Includes ---------------------------------------------------*/")
    code.append("#include <stdio.h>")
    code.append("#include <stdlib.h>")
    code.append("#include \"sni_types.h\"")
    code.append("#include \"sni_type_bridge.h\"")
    code.append("#include \"lvgl.h\"")
    code.append("")

    # Generate property tables
    code.append("/************************** 属性表 **************************/")
    code.append("")

    property_tables = []
    value_obj_descriptions = []

    for type_name in value_types:
        # Skip if type not found in lvgl.json
        if type_name not in type_definitions:
            continue

        # Get type definition
        type_def = type_definitions[type_name]

        # Generate property table name
        base_name = type_name[:-2] if type_name.endswith('_t') else type_name
        prop_table_name = f"{base_name}_props"
        property_tables.append(prop_table_name)

        # Generate property table
        code.append(f"const sni_val_prop_t {prop_table_name}[] = {{")

        fields = []
        # Check if type has fields
        if 'fields' in type_def:
            fields = type_def['fields']
        elif 'type' in type_def and type_def['type'] in type_definitions:
            # If it's a typedef, check the underlying type
            underlying_type = type_definitions[type_def['type']]
            if 'fields' in underlying_type:
                fields = underlying_type['fields']

        for field in fields:
            if 'name' not in field:
                continue

            field_name = field['name']
            field_type = field.get('type', '')

            # Handle case where type is a dict
            if isinstance(field_type, dict):
                # Try to get type from dict
                if 'name' in field_type:
                    field_type = field_type['name']
                else:
                    field_type = ''

            # Find underlying type and map to SNI type
            sni_type = find_underlying_type(field_type, type_definitions)
            if sni_type is None:
                print(f"Error: Cannot find underlying type for field '{field_name}' with type '{field_type}' in type '{type_name}'", file=sys.stderr)
                sys.exit(1)

            # Generate field entry
            code.append("    {")
            code.append(f"        .name = \"{field_name}\",")
            code.append(f"        .type = {sni_type},")
            code.append(f"        .offset = offsetof({type_name}, {field_name}),")
            code.append("    },")

        code.append("};")
        code.append("")

        # Generate value object description
        value_obj_name = f"{base_name}_prop"
        sni_v_name = f"SNI_V_{base_name.upper()}"
        prop_count = len(fields)

        # Check if SNI_V_* type is defined in sni_types.h
        # If not, add it automatically
        if sni_v_name not in sni_value_types:
            add_sni_value_type(sni_types_path, sni_v_name)
            sni_value_types.add(sni_v_name)

        code.append(f"const sni_val_obj_t {value_obj_name} = {{")
        code.append(f"    .type = {sni_v_name},")
        code.append(f"    .prop_count = {prop_count},")
        code.append(f"    .props = {prop_table_name},")
        code.append("};")
        code.append("")

        value_obj_descriptions.append(value_obj_name)

    # Generate registry
    code.append("/************************** 注册表 **************************/")
    code.append("")
    code.append("const size_t sni_lv_types_count = sizeof(sni_lv_types) / sizeof(sni_lv_types[0]);")
    code.append("")
    code.append("const sni_val_obj_t *sni_lv_types[] = {")

    for value_obj in value_obj_descriptions:
        code.append(f"    &{value_obj},")

    code.append("};")
    code.append("")

    # Generate initialization function
    code.append("/************************** 初始化函数 **************************/")
    code.append("void sni_lv_types_init(void)")
    code.append("{")
    code.append("    for (size_t i = 0;")
    code.append("         i < sni_lv_types_count;")
    code.append("         i++)")
    code.append("    {")
    code.append("        sni_tb_register_val_obj(sni_lv_types[i]);")
    code.append("    }")
    code.append("}")
    code.append("")

    # Write to file
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(code))
        print(f"Generated sni_lv_types.c at {output_file}")
    except Exception as e:
        print(f"Error writing to file: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    """
    Main function
    """
    # Check if the required arguments are provided
    if len(sys.argv) != 5:
        print(f"Usage: {sys.argv[0]} <path_to_lv_types_output.json> <path_to_lvgl.json> <sni_types.h> <output_file>", file=sys.stderr)
        sys.exit(1)

    # Generate sni_lv_types.c
    generate_sni_lv_types(sys.argv[1], sys.argv[2], sys.argv[4], sys.argv[3])


if __name__ == "__main__":
    main()
