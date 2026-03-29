#!/usr/bin/env python3
from pathlib import Path
from PIL import Image, ImageDraw
import sys


def circular_alpha_cut(input_path: str, output_path: str | None = None) -> Path:
    in_path = Path(input_path)
    img = Image.open(in_path).convert("RGBA")
    w, h = img.size

    if w != h:
        raise ValueError(f"Image must be square. Got {w}x{h}.")

    size = w
    radius = size / 2.0
    cx = cy = radius

    # Build an 8-bit mask: white inside circle, black outside
    mask = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(mask)

    # Strict inscribed circle touching the square edges
    # Using exact bounds for the full image area
    draw.ellipse((0, 0, size - 1, size - 1), fill=255)

    # Apply mask as alpha
    r, g, b, a = img.split()
    new_alpha = Image.new("L", (size, size), 0)
    new_alpha.paste(a)
    new_alpha = Image.composite(new_alpha, Image.new("L", (size, size), 0), mask)

    out = Image.merge("RGBA", (r, g, b, new_alpha))

    if output_path is None:
        output_path = in_path.with_name(f"{in_path.stem}_circle_cut.png")

    out_path = Path(output_path)
    out.save(out_path)
    return out_path


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python circle_cut.py input_image [output_image]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    try:
        result = circular_alpha_cut(input_file, output_file)
        print(f"Saved: {result}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(2)