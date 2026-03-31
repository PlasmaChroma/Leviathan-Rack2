#!/usr/bin/env python3
from pathlib import Path
from PIL import Image, ImageDraw
import sys


TARGET_SIZE = 1024
RESAMPLE_FILTER = Image.Resampling.LANCZOS


def resize_to_target_if_needed(img: Image.Image, target_size: int = TARGET_SIZE) -> Image.Image:
    """
    Resize to target_size x target_size using high-quality Lanczos resampling
    if the image is not already that size.
    """
    if img.size == (target_size, target_size):
        return img

    return img.resize((target_size, target_size), RESAMPLE_FILTER)


def circular_alpha_cut(
    input_path: str,
    output_path: str | None = None,
    extra_cut: int = 0,
    indexed: bool = False,
    palette_colors: int = 256,
    target_size: int = TARGET_SIZE,
) -> Path:
    in_path = Path(input_path)
    img = Image.open(in_path).convert("RGBA")

    if extra_cut < 0:
        raise ValueError("extra_cut must be >= 0.")

    if not (1 <= palette_colors <= 256):
        raise ValueError("palette_colors must be between 1 and 256.")

    # First step: high-quality resize to the requested square target if needed.
    img = resize_to_target_if_needed(img, target_size)

    w, h = img.size
    if w != h:
        raise ValueError(f"Image must be square after resize. Got {w}x{h}.")

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
        suffix = "_circle_cut_indexed.png" if indexed else "_circle_cut.png"
        output_path = in_path.with_name(f"{in_path.stem}{suffix}")

    out_path = Path(output_path)

    if indexed:
        # Quantize to paletted PNG while preserving transparency reasonably well
        pal = out.quantize(colors=palette_colors, method=Image.Quantize.FASTOCTREE)
        pal.save(out_path, format="PNG", optimize=True)
    else:
        out.save(out_path, format="PNG")

    return out_path


def parse_bool(value: str) -> bool:
    value = value.strip().lower()
    if value in {"1", "true", "yes", "y", "on"}:
        return True
    if value in {"0", "false", "no", "n", "off"}:
        return False
    raise ValueError(f"Invalid boolean value: {value}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(
            "Usage: python circle_cut.py input_image [output_image] [extra_cut_pixels] [indexed:true|false] [palette_colors]"
        )
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = None
    extra_cut = 0
    indexed = False
    palette_colors = 256

    if len(sys.argv) >= 3:
        output_file = sys.argv[2]

    if len(sys.argv) >= 4:
        extra_cut = int(sys.argv[3])

    if len(sys.argv) >= 5:
        indexed = parse_bool(sys.argv[4])

    if len(sys.argv) >= 6:
        palette_colors = int(sys.argv[5])

    try:
        result = circular_alpha_cut(
            input_file,
            output_file,
            extra_cut,
            indexed,
            palette_colors,
        )
        print(f"Saved: {result}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(2)
