#!/usr/bin/env python3
"""Bake the default Magitek2 radial shadow into padded PNG siblings."""

from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
ICON_DIR = ROOT / "res" / "icon"
BODY_SIZE = 256
PADDING = 42
CANVAS_SIZE = BODY_SIZE + 2 * PADDING

# Match Magitek2JackShadow in logical coordinates. The source body occupies
# 24.5 px inside the 32.5 px procedural framebuffer, inset by 4 px.
LOGICAL_CANVAS = 32.5
SHADOW_CENTER = (LOGICAL_CANVAS * 0.5 + 1.6, LOGICAL_CANVAS * 0.5 + 2.5)
OUTER_RADIUS = LOGICAL_CANVAS * 0.43
INNER_RADIUS = OUTER_RADIUS * 0.48
INNER_ALPHA = 138


def shadow_layer() -> Image.Image:
    scale = CANVAS_SIZE / LOGICAL_CANVAS
    cx, cy = (SHADOW_CENTER[0] * scale, SHADOW_CENTER[1] * scale)
    inner = INNER_RADIUS * scale
    outer = OUTER_RADIUS * scale
    pixels = bytearray(CANVAS_SIZE * CANVAS_SIZE * 4)
    for y in range(CANVAS_SIZE):
        dy = y + 0.5 - cy
        for x in range(CANVAS_SIZE):
            dx = x + 0.5 - cx
            distance = (dx * dx + dy * dy) ** 0.5
            if distance <= inner:
                alpha = INNER_ALPHA
            elif distance < outer:
                alpha = round(INNER_ALPHA * (outer - distance) / (outer - inner))
            else:
                alpha = 0
            pixels[(y * CANVAS_SIZE + x) * 4 + 3] = alpha
    return Image.frombytes("RGBA", (CANVAS_SIZE, CANVAS_SIZE), bytes(pixels))


def bake(source_name: str, output_name: str, shadow: Image.Image) -> None:
    body = Image.open(ICON_DIR / source_name).convert("RGBA")
    if body.size != (BODY_SIZE, BODY_SIZE):
        raise ValueError(f"{source_name}: expected {BODY_SIZE}x{BODY_SIZE}, got {body.size}")
    output = shadow.copy()
    output.alpha_composite(body, (PADDING, PADDING))
    output.save(ICON_DIR / output_name, optimize=True)


def main() -> None:
    shadow = shadow_layer()
    bake("magitek2_input_rackfinal_256.png", "magitek2_input_shadow_340.png", shadow)
    bake("magitek2_output_rackfinal_256.png", "magitek2_output_shadow_340.png", shadow)


if __name__ == "__main__":
    main()
