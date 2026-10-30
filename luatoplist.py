import re

LUA_FILE = "gamesheet.lua" 
OUTPUT_PLIST = "PD-gameSheet.plist"
SPRITE_SIZE = 16

with open(LUA_FILE, "r", encoding="utf-8") as f:
    lua_code = f.read()

# 1. Считываем соответствие id -> имя из M.sprites
names_by_id = {}
sprites_match = re.search(r"M\.sprites\s*=\s*\{(.*?)\n\s*\}", lua_code, re.DOTALL)
if sprites_match:
    for line in sprites_match.group(1).splitlines():
        m = re.search(r"([A-Za-z0-9_]+)\s*=\s*(\d+)", line)
        if m:
            name, idx = m.group(1), int(m.group(2))
            names_by_id[idx] = name.lower() + ".png"

# 2. Вырезаем секцию frames
frames_section = lua_code.split("M.spriteOptions")[1].split("M.sprites")[0]
frames_raw = re.findall(r"\{([^{}]+)\}", frames_section)

def eval_expr(expr):
    expr = expr.strip().replace("SPRITE_SIZE", str(SPRITE_SIZE))
    return int(eval(expr))

frames_data = []
for idx, frame_str in enumerate(frames_raw, start=1):
    vals = {}
    for line in frame_str.splitlines():
        line = line.strip().rstrip(",")
        if "=" in line:
            k, v = line.split("=", 1)
            k = k.strip()
            if k == "heigth":  # исправление опечатки в исходном lua
                k = "height"
            vals[k] = eval_expr(v)
    
    if "x" in vals and "y" in vals and "width" in vals and "height" in vals:
        sprite_name = names_by_id.get(idx, f"sprite_{idx}.png")
        frames_data.append((sprite_name, vals["x"], vals["y"], vals["width"], vals["height"]))

# 3. Собираем plist XML без конфликтов с f-strings
lines = [
    '<?xml version="1.0" encoding="UTF-8"?>',
    '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://apple.com">',
    '<plist version="1.0">',
    '<dict>',
    '    <key>frames</key>',
    '    <dict>'
]

for name, x, y, w, h in frames_data:
    frame_box = "{{" + str(x) + "," + str(y) + "},{" + str(w) + "," + str(h) + "}}"
    source_box = "{{0,0},{" + str(w) + "," + str(h) + "}}"
    size_box = "{" + str(w) + "," + str(h) + "}"
    
    lines.append('        <key>' + name + '</key>')
    lines.append('        <dict>')
    lines.append('            <key>frame</key>')
    lines.append('            <string>' + frame_box + '</string>')
    lines.append('            <key>offset</key>')
    lines.append('            <string>{0,0}</string>')
    lines.append('            <key>rotated</key>')
    lines.append('            <false/>')
    lines.append('            <key>sourceColorRect</key>')
    lines.append('            <string>' + source_box + '</string>')
    lines.append('            <key>sourceSize</key>')
    lines.append('            <string>' + size_box + '</string>')
    lines.append('        </dict>')

lines.extend([
    '    </dict>',
    '    <key>metadata</key>',
    '    <dict>',
    '        <key>format</key>',
    '        <integer>2</integer>',
    '        <key>realTextureFileName</key>',
    '        <string>PD-gameSheet.png</string>',
    '        <key>size</key>',
    '        <string>{512,512}</string>',
    '        <key>textureFileName</key>',
    '        <string>PD-gameSheet.png</string>',
    '    </dict>',
    '</dict>',
    '</plist>'
])

with open(OUTPUT_PLIST, "w", encoding="utf-8") as f:
    f.write("\n".join(lines))

print(f"Успешно сгенерировано {len(frames_data)} спрайтов в {OUTPUT_PLIST}!")
