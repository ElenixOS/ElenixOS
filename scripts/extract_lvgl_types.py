#!/usr/bin/env python3
import json
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
                    'object_type': ''
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
                types.append({
                    'name': typedef['name'],
                    'object_type': ''
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
    result = extract_types(sys.argv[1])

    # Output to file if specified, otherwise to stdout
    if len(sys.argv) == 3:
        try:
            with open(sys.argv[2], 'w', encoding='utf-8') as f:
                json.dump(result, f, indent=2)
            print(f"Output written to {sys.argv[2]}")
        except Exception as e:
            print(f"Error writing to file: {e}", file=sys.stderr)
            sys.exit(1)
    else:
        print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
