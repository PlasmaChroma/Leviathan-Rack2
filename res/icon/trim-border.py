#!/usr/bin/env python3
from PIL import Image, ImageFilter, ImageChops
from pathlib import Path
import argparse


def erode_mask(mask: Image.Image, pixels: int) -> Image.Image:
    """
    Erode a grayscale/binary mask inward by 'pixels' using repeated 3x3 min filters.
    This follows the actual shape, including angled corners.
    """
    out = mask
    for _ in range(max(0, pixels)):
        out = out.filter(ImageFilter.MinFilter(3))
    return out


def make_binary_mask(alpha: Image.Image, threshold: int) -> Image.Image:
    """
    Convert alpha to a hard binary mask:
    alpha >= threshold => 255
    alpha < threshold  => 0
    """
    return alpha.point(lambda p: 255 if p >= threshold else 0, mode="L")


def trim_shape_inward(img: Image.Image, trim_px: int, threshold: int, feather: float) -> Image.Image:
    """
    Shrink the visible shape inward by trim_px, following the alpha contour.
    """
    img = img.convert("RGBA")
    r, g, b, a = img.split()

    # Build a clean solid mask from existing alpha
    solid_mask = make_binary_mask(a, threshold)

    # Erode the visible shape inward
    eroded = erode_mask(solid_mask, trim_px)

    # Optional soft edge so the new cut doesn't look jagged
    if feather > 0:
        eroded = eroded.filter(ImageFilter.GaussianBlur(radius=feather))

    # Combine with original alpha so we preserve internal transparency behavior
    new_alpha = ImageChops.multiply(a, eroded)

    out = Image.merge("RGBA", (r, g, b, new_alpha))
    return out


def main():
    parser = argparse.ArgumentParser(
        description="Trim an image inward based on its alpha shape, respecting angled corners."
    )
    parser.add_argument(
        "input",
        nargs="?",
        default="PlasmaSwitchSmall.png",
        help="Input PNG file (default: PlasmaSwitchSmall.png)"
    )
    parser.add_argument(
        "-o", "--output",
        default="PlasmaSwitchSmall_trimmed.png",
        help="Output PNG file (default: PlasmaSwitchSmall_trimmed.png)"
    )
    parser.add_argument(
        "--trim",
        type=int,
        default=3,
        help="How many pixels to shave inward from the shape border (default: 3)"
    )
    parser.add_argument(
        "--threshold",
        type=int,
        default=8,
        help="Alpha threshold used to detect the visible shape, 0-255 (default: 8)"
    )
    parser.add_argument(
        "--feather",
        type=float,
        default=0.75,
        help="Feather radius for the new edge (default: 0.75, use 0 for hard cut)"
    )

    args = parser.parse_args()

    src = Path(args.input)
    dst = Path(args.output)

    img = Image.open(src).convert("RGBA")
    out = trim_shape_inward(
        img,
        trim_px=args.trim,
        threshold=args.threshold,
        feather=args.feather
    )

    out.save(dst)
    print(f"Saved: {dst}")
    print(f"Trimmed inward by: {args.trim}px")
    print(f"Threshold: {args.threshold}")
    print(f"Feather: {args.feather}")
    print(f"Image size preserved: {out.size[0]}x{out.size[1]}")


if __name__ == "__main__":
    main()