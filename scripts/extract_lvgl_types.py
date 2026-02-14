#!/usr/bin/env python3
import json
import os
import sys


def extract_types(json_path):
    """
    Extract types from lvgl.json file
    """
    # Open and load the JSON file
    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Error: File {json_path} not found", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON in {json_path}", file=sys.stderr)
        sys.exit(1)

    # Extract types from specified sections
    types = []

    # Extract from enums
    if 'enums' in data:
        for enum in data['enums']:
            if 'name' in enum and enum['name'] is not None:
                types.append({
                    'name': enum['name'],
                    'object_type': 'int'
                })

    # Extract from structures
    if 'structures' in data:
        for struct in data['structures']:
            if 'name' in struct and struct['name'] is not None:
                types.append({
                    'name': struct['name'],
                    'object_type': ''
                })

    # Extract from unions
    if 'unions' in data:
        for union in data['unions']:
            if 'name' in union and union['name'] is not None:
                types.append({
                    'name': union['name'],
                    'object_type': ''
                })

    # Extract from typedefs
    if 'typedefs' in data:
        for typedef in data['typedefs']:
            if 'name' in typedef and typedef['name'] is not None:
                object_type = ''
                if 'type' in typedef and 'name' in typedef['type']:
                    type_name = typedef['type']['name']
                    if type_name.startswith('uint'):
                        object_type = 'uint'
                    elif type_name.startswith('int') and not type_name.startswith('uint'):
                        object_type = 'int'
                    elif type_name in ['double', 'float']:
                        object_type = 'double'
                types.append({
                    'name': typedef['name'],
                    'object_type': object_type
                })

    # Extract form forward_decls
    if 'forward_decls' in data:
        for forward_decl in data['forward_decls']:
            if 'name' in forward_decl and forward_decl['name'] is not None:
                types.append({
                    'name': forward_decl['name'],
                    'object_type': ''
                })


    # Return the result in the required format
    return {
        'types': types
    }


def main():
    """
    Main function
    """
    # Check if the path argument is provided
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path_to_lvgl.json> [output_file]", file=sys.stderr)
        sys.exit(1)

    # Extract types
    extracted_types = extract_types(sys.argv[1])

    # Output to file if specified, otherwise to stdout
    if len(sys.argv) == 3:
        output_file = sys.argv[2]
        try:
            # Check if output file exists
            if os.path.exists(output_file):
                # Read existing content
                with open(output_file, 'r', encoding='utf-8') as f:
                    existing_data = json.load(f)
                # Get existing type names to avoid duplicates
                existing_type_names = set()
                if 'types' in existing_data:
                    existing_type_names = {t['name'] for t in existing_data['types']}
                # Add only new types (not already in existing file)
                new_types = []
                for t in extracted_types['types']:
                    if t['name'] not in existing_type_names:
                        new_types.append(t)
                # Update existing types with object_type if empty
                if 'types' in existing_data:
                    for existing_type in existing_data['types']:
                        for extracted_type in extracted_types['types']:
                            if existing_type['name'] == extracted_type['name']:
                                if not existing_type.get('object_type') and extracted_type.get('object_type'):
                                    existing_type['object_type'] = extracted_type['object_type']
                # Update existing data with new types
                if 'types' not in existing_data:
                    existing_data['types'] = []
                existing_data['types'].extend(new_types)
                # Write merged data back to file
                with open(output_file, 'w', encoding='utf-8') as f:
                    json.dump(existing_data, f, indent=2)
                print(f"Added {len(new_types)} new types to {output_file}")
            else:
                # File doesn't exist, create new one
                with open(output_file, 'w', encoding='utf-8') as f:
                    json.dump(extracted_types, f, indent=2)
                print(f"Output written to {output_file}")
        except Exception as e:
            print(f"Error writing to file: {e}", file=sys.stderr)
            sys.exit(1)
    else:
        print(json.dumps(extracted_types, indent=2))


if __name__ == "__main__":
    main()
