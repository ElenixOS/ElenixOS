#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file ttf_mgr.py
@brief Process font files, import LVGL symbol glyphs, and remap Unicode code points
"""

from fontTools.ttLib import TTFont
from fontTools.pens.ttGlyphPen import TTGlyphPen
from fontTools.ttLib.tables._g_l_y_f import Glyph
import copy
import os

# LVGL symbol font and target font
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
lvgl_font_path = os.path.normpath(os.path.join(BASE_DIR, "../../lvgl/scripts/built_in_font/FontAwesome5-Solid+Brands+Regular.woff"))
target_font_path = os.path.normpath(os.path.join(BASE_DIR, "../third_party/RemixIcon/fonts/remixicon.ttf"))
output_font_path = os.path.normpath(os.path.join(BASE_DIR, "../third_party/RemixIcon/fonts/remixicon.ttf"))

move_only = True  # True: do not import LVGL, only remap target font symbols; False: import LVGL and remap existing symbols

# LVGL symbol table example
lvgl_symbols = {
    "LV_SYMBOL_AUDIO": 0xF001,
    "LV_SYMBOL_VIDEO": 0xF008,
    "LV_SYMBOL_LIST": 0xF00B,
    "LV_SYMBOL_OK": 0xF00C,
    "LV_SYMBOL_CLOSE": 0xF00D,
    "LV_SYMBOL_POWER": 0xF011,
    "LV_SYMBOL_SETTINGS": 0xF013,
    "LV_SYMBOL_HOME": 0xF015,
    "LV_SYMBOL_DOWNLOAD": 0xF019,
    "LV_SYMBOL_DRIVE": 0xF01C,
    "LV_SYMBOL_REFRESH": 0xF021,
    "LV_SYMBOL_MUTE": 0xF026,
    "LV_SYMBOL_VOLUME_MID": 0xF027,
    "LV_SYMBOL_VOLUME_MAX": 0xF028,
    "LV_SYMBOL_IMAGE": 0xF03E,
    "LV_SYMBOL_TINT": 0xF043,
    "LV_SYMBOL_PREV": 0xF048,
    "LV_SYMBOL_PLAY": 0xF04B,
    "LV_SYMBOL_PAUSE": 0xF04C,
    "LV_SYMBOL_STOP": 0xF04D,
    "LV_SYMBOL_NEXT": 0xF051,
    "LV_SYMBOL_EJECT": 0xF052,
    "LV_SYMBOL_LEFT": 0xF053,
    "LV_SYMBOL_RIGHT": 0xF054,
    "LV_SYMBOL_PLUS": 0xF067,
    "LV_SYMBOL_MINUS": 0xF068,
    "LV_SYMBOL_EYE_OPEN": 0xF06E,
    "LV_SYMBOL_EYE_CLOSE": 0xF070,
    "LV_SYMBOL_WARNING": 0xF071,
    "LV_SYMBOL_SHUFFLE": 0xF074,
    "LV_SYMBOL_UP": 0xF077,
    "LV_SYMBOL_DOWN": 0xF078,
    "LV_SYMBOL_LOOP": 0xF079,
    "LV_SYMBOL_DIRECTORY": 0xF07B,
    "LV_SYMBOL_UPLOAD": 0xF093,
    "LV_SYMBOL_CALL": 0xF095,
    "LV_SYMBOL_CUT": 0xF0C4,
    "LV_SYMBOL_COPY": 0xF0C5,
    "LV_SYMBOL_SAVE": 0xF0C7,
    "LV_SYMBOL_BARS": 0xF0C9,
    "LV_SYMBOL_ENVELOPE": 0xF0E0,
    "LV_SYMBOL_CHARGE": 0xF0E7,
    "LV_SYMBOL_PASTE": 0xF0EA,
    "LV_SYMBOL_BELL": 0xF0F3,
    "LV_SYMBOL_KEYBOARD": 0xF11C,
    "LV_SYMBOL_GPS": 0xF124,
    "LV_SYMBOL_FILE": 0xF158,
    "LV_SYMBOL_WIFI": 0xF1EB,
    "LV_SYMBOL_BATTERY_FULL": 0xF240,
    "LV_SYMBOL_BATTERY_3": 0xF241,
    "LV_SYMBOL_BATTERY_2": 0xF242,
    "LV_SYMBOL_BATTERY_1": 0xF243,
    "LV_SYMBOL_BATTERY_EMPTY": 0xF244,
    "LV_SYMBOL_USB": 0xF287,
    "LV_SYMBOL_BLUETOOTH": 0xF293,
    "LV_SYMBOL_TRASH": 0xF2ED,
    "LV_SYMBOL_EDIT": 0xF304,
    "LV_SYMBOL_BACKSPACE": 0xF55A,
    "LV_SYMBOL_SD_CARD": 0xF7C2,
    "LV_SYMBOL_NEW_LINE": 0xF8A2
}

# Load fonts
target_font = TTFont(target_font_path)

if not move_only:
    lvgl_font = TTFont(lvgl_font_path)
    lvgl_cmap = lvgl_font.getBestCmap()
    lvgl_glyf = lvgl_font['glyf']

target_cmap = target_font.getBestCmap()
target_glyf = target_font['glyf']
target_hmtx = target_font['hmtx']

# Remap existing target-font symbols
existing_unicode = sorted(target_cmap.keys())
start_unicode = 0xE000  # custom Unicode start
unicode_mapping = {}
for u in existing_unicode:
    # If move_only is True, ignore LVGL Unicode set
    if not move_only and u in lvgl_symbols.values():
        continue
    unicode_mapping[u] = start_unicode
    start_unicode += 1

# Copy LVGL glyphs
if not move_only:
    scale = target_font['head'].unitsPerEm / lvgl_font['head'].unitsPerEm
    for name, uni in lvgl_symbols.items():
        if uni in lvgl_cmap:
            glyph_name = lvgl_cmap[uni]
            lv_glyph = lvgl_glyf[glyph_name]

            # Scale glyph
            pen = TTGlyphPen(None)
            tpen = TransformPen(pen, (scale, 0, 0, scale, 0, 0))
            lv_glyph.draw(tpen)
            scaled_glyph = pen.glyph()

            # Save to target font
            target_glyf[glyph_name] = scaled_glyph

            # Synchronize horizontal metrics
            lv_advanceWidth, lv_lsb = lvgl_font['hmtx'][glyph_name]
            target_hmtx[glyph_name] = (int(lv_advanceWidth * scale), int(lv_lsb * scale))

            # Update cmap
            target_cmap[uni] = glyph_name

# Update cmap
for old_uni, new_uni in unicode_mapping.items():
    glyph_name = target_cmap[old_uni]
    target_cmap[new_uni] = glyph_name
    del target_cmap[old_uni]

# Write cmap table back
target_font['cmap'].tables[0].cmap = target_cmap

# Save
target_font.save(output_font_path)
print("✅ Font processing completed, saved to", output_font_path)
