import json
import sys
from pathlib import Path

# Относительные пути от корня проекта
BASE_DIR = Path(__file__).resolve().parent
CONTENT_DIR = BASE_DIR / "Content"

JSON_FILE = CONTENT_DIR / "PD-gameSheet.json"
PLIST_FILE = CONTENT_DIR / "PD-gameSheet.plist"

def main():
    if not JSON_FILE.exists():
        print(f"Ошибка: Файл '{JSON_FILE}' не найден!", file=sys.stderr)
        sys.exit(1)

    with open(JSON_FILE, "r", encoding="utf-8") as f:
        data = json.load(f)

    meta = data.get("meta", {})
    slices = meta.get("slices", [])
    texture_name = meta.get("image", "PD-gameSheet.png")
    
    # Размеры текстуры из метаданных
    size = meta.get("size", {"w": 500, "h": 500})
    tex_w = size.get("w", 500)
    tex_h = size.get("h", 500)

    # Заголовок XML Plist (формат Cocos2d/Axmol format 2)
    lines = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://apple.com">',
        '<plist version="1.0">',
        '<dict>',
        '    <key>frames</key>',
        '    <dict>'
    ]

    count = 0
    for item in slices:
        raw_name = item.get("name", "").strip()
        if not raw_name:
            continue

        # Приводим к единому формату имен с расширением .png
        frame_name = raw_name if raw_name.endswith(".png") else f"{raw_name}.png"

        # Извлекаем координаты прямоугольника слайса
        keys = item.get("keys", [])
        if not keys:
            continue

        bounds = keys[0].get("bounds", {})
        x = bounds.get("x", 0)
        y = bounds.get("y", 0)
        w = bounds.get("w", 0)
        h = bounds.get("h", 0)

        frame_box = "{{" + str(x) + "," + str(y) + "},{" + str(w) + "," + str(h) + "}}"
        source_box = "{{0,0},{" + str(w) + "," + str(h) + "}}"
        size_box = "{" + str(w) + "," + str(h) + "}"

        lines.append('        <key>' + frame_name + '</key>')
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
        count += 1

    lines.extend([
        '    </dict>',
        '    <key>metadata</key>',
        '    <dict>',
        '        <key>format</key>',
        '        <integer>2</integer>',
        '        <key>realTextureFileName</key>',
        '        <string>' + texture_name + '</string>',
        '        <key>size</key>',
        '        <string>{' + str(tex_w) + ',' + str(tex_h) + '}</string>',
        '        <key>textureFileName</key>',
        '        <string>' + texture_name + '</string>',
        '    </dict>',
        '</dict>',
        '</plist>'
    ])

    with open(PLIST_FILE, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

    print(f"Готово! Обработано {count} спрайтов.")
    print(f"Файл сохранен: {PLIST_FILE}")

if __name__ == "__main__":
    main()
