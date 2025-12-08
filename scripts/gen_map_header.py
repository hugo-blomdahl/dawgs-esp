import os

src = os.path.join("src", "map.json")
dst = os.path.join("include", "map_json.h")

with open(src, "rb") as f:
    data = f.read()

print(f"Embedding {src} ({len(data)} bytes)")

lines = []
lines.append("#pragma once\n\n")
lines.append("unsigned char map_json[] = {\n")

for i in range(0, len(data), 12):
    chunk = data[i:i+12]
    hexs = ", ".join(f"0x{b:02x}" for b in chunk)
    lines.append("    " + hexs)

    if i + 12 < len(data):
        lines[-1] += ","

    lines[-1] += "\n"

lines.append("};\n\n")
lines.append(f"unsigned int map_json_len = {len(data)};\n")

with open(dst, "w") as f:
    f.writelines(lines)

print(f"Generated: {dst}")