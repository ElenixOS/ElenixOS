from fontTools.ttLib import TTFont

# 打开字体文件
font = TTFont("remixicon.ttf")

# 获取 Unicode -> glyphName 映射
unicode_map = {}
for table in font['cmap'].tables:
    for codepoint, name in table.cmap.items():
        unicode_map[codepoint] = name

# 生成宏定义
with open("remixicon_symbols.h", "w", encoding="utf-8") as f:
    f.write("/* Auto generated file */\n\n")
    for codepoint, name in unicode_map.items():
        # 规范化 glyph 名称
        macro_name = name.upper().replace("-", "_")
        f.write(f'#define RI_{macro_name} "\\u{codepoint:04X}"\n')
