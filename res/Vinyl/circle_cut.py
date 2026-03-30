#!/usr/bin/env python3
from pathlib import Path
from PIL import Image, ImageDraw
import sys


def circular_alpha_cut(
    input_path: str,
    output_path: str | None = None,
    extra_cut: int = 0,
) -> Path:
    in_path = Path(input_path)
    img = Image.open(in_path).convert("RGBA")
    w, h = img.size

    if w != h:
        raise ValueError(f"Image must be square. Got {w}x{h}.")

    if extra_cut < 0:
        raise ValueError("extra_cut must be >= 0.")

    size = w

    # Shrink the preserved circle inward by extra_cut pixels on all sides
    left = extra_cut
    top = extra_cut
    right = size - 1 - extra_cut
    bottom = size - 1 - extra_cut

    if left >= right or top >= bottom:
        raise ValueError(
            f"extra_cut={extra_cut} is too large for image size {size}x{size}."
        )

    # White inside circle, black outside
    mask = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(mask)
    draw.ellipse((left, top, right, bottom), fill=255)

    # Apply mask to alpha
    r, g, b, a = img.split()
    new_alpha = Image.composite(a, Image.new("L", (size, size), 0), mask)
    out = Image.merge("RGBA", (r, g, b, new_alpha))

    if output_path is None:
        output_path = in_path.with_name(f"{in_path.stem}_circle_cut.png")

    out_path = Path(output_path)
    out.save(out_path)
    return out_path


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python circle_cut.py input_image [output_image] [extra_cut_pixels]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = None
    extra_cut = 0

    if len(sys.argv) >= 3:
        output_file = sys.argv[2]

    if len(sys.argv) >= 4:
        extra_cut = int(sys.argv[3])

    try:
        result = circular_alpha_cut(input_file, output_file, extra_cut)
        print(f"Saved: {result}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(2)