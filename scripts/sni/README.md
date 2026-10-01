# LVGL to SNI generator

`generate_sni.py` is the single generator entry point. It reads LVGL declaration facts from `lvgl.json` and ElenixOS binding choices from `config/sni_lvgl_bindings.json`. The resolver builds a typed IR in memory; validation must finish before the renderers write any generated file.

## Inputs and outputs

The binding file uses schema version 1. Its sections contain API selectors and filters, inheritance and constructors, signature overrides, special wrapper mappings, special wrapper type dependencies, and explicit resource declarations. It does not contain an inventory of LVGL primitive types, enums, struct fields, or inferred value objects.

The selected Managed Resource declarations are:

- `lv_timer_t`: Pure Managed; only `lv_timer_create` may introduce a new instance.
- `lv_chart_series_t`, `lv_chart_cursor_t`, and `lv_event_dsc_t`: Tree-Dependent.

`SNI_H_LV_OBJ` is inferred from LVGL object-tree semantics. A pointer to a Value Object remains a Value Object use-site conversion. Other structs become Value Objects only when every field recursively resolves to a primitive, enum, or confirmed Value Object. Pointers, callbacks, unions, arrays, bitfields, and unsupported layouts fail closed unless an explicit safe conversion is configured.

`special_bindings` maps selected C APIs and class properties to wrapper IDs implemented under `src/script_engine/sni/sni_api/`. The generator checks each ID against those C sources. `type_dependencies` names LVGL types used by special C wrappers but absent from generic API signatures.

Successful generation writes:

- `src/script_engine/sni/sni_type_ids.h`
- `src/script_engine/sni/sni_gen/sni_lv_types.c`
- `src/script_engine/sni/sni_api/lv/sni_api_lv.c`

`sni_types.h` keeps runtime structures and includes `sni_type_ids.h`.

## Commands

Run from the Simulator repository root:

```bash
python3 ElenixOS/scripts/sni/generate_sni.py analyze
python3 ElenixOS/scripts/sni/generate_sni.py analyze --format json
python3 ElenixOS/scripts/sni/generate_sni.py validate
python3 ElenixOS/scripts/sni/generate_sni.py dump-ir
python3 ElenixOS/scripts/sni/generate_sni.py update-config
python3 ElenixOS/scripts/sni/generate_sni.py generate
```

All commands use the same progress, result, diagnostic, and presentation layer. The core resolver and renderers return structured data; only `sni/cli/reporters.py` writes human output. The CLI renderer uses the Python standard library, so running SNI commands does not require Rich or another terminal package.

The API result status is independent of diagnostic severity:

- `ACCEPTED_GENERIC` and `ACCEPTED_SPECIAL` are exported APIs.
- `EXCLUDED_BLACKLIST` is an intentional exclusion and is not a warning or generation failure.
- `REJECTED_UNSUPPORTED_TYPE`, `REJECTED_SPECIAL_REQUIRED`, and `REJECTED_LIFECYCLE` explain why an API is not exported.
- `REJECTED_UNRESOLVED` means the binding decision is missing and blocks validation and generation.

The default analysis is summary-first. `--details` groups unsupported APIs by C type and shows special-required and lifecycle groups. Use `--category blacklist` to list intentional exclusions, `--api lv_timer_get_next` to inspect one API, or `--type lv_grad_dsc_t` to inspect a C type and its affected APIs. Shared output options are `--quiet`, `--verbose`, `--details`, `--format text|json`, and `--no-color`; they work before or after the subcommand. `NO_COLOR` is also honored.

Every command supports `--format json`. Stdout is one JSON document with the command, success state, summary, progress events, structured diagnostics, and command result. JSON mode suppresses human progress, table markup, and colors. `dump-ir` keeps its full IR in `result.ir`.

To regenerate metadata from the current project configuration, use `generate --refresh-lvgl-json`; it invokes LVGL's `gen_json.py` with the repository's `lv_conf.h` and writes the metadata under `build/lvgl-api/`. The LVGL generator's Doxygen helper and Python requirements must be installed for this refresh step. Existing metadata can be passed with `--lvgl-json PATH`.

Generation formats all C outputs with the repository's `.clang-format`; `clang-format` 20 must be available.

`update-config` is the only command that persists newly discovered unresolved types, as `{ "kind": "unknown" }` review items. Their presence makes validation and generation fail until a human resolves them. `analyze`, `validate`, and `generate` never edit the binding configuration.

Text-mode progress goes to stderr and command summaries go to stdout. A progress event is emitted only when its stage completes; stage totals are declared per command, and failure/warning states are recorded. Non-TTY and CI output stays static. JSON mode records those stage results in its `progress` array without writing progress text to stderr. The IR retains each type's canonical name, typedef chain, dependencies, representation, resolution source, inference reason, rejection reason, fields, and API references.

## Validation

```bash
PYTHONPATH=ElenixOS/scripts python3 -m unittest discover -s ElenixOS/scripts/sni/tests -v
python3 ElenixOS/scripts/sni/generate_sni.py validate
python3 ElenixOS/scripts/sni/generate_sni.py generate
git diff --check
```

The checked-in LVGL metadata is an input snapshot. Refresh it explicitly when changing LVGL or the Simulator's `lv_conf.h`.
