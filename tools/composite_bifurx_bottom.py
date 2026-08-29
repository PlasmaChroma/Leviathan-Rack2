#!/usr/bin/env python3
"""Generate the shipped Bifurx bottom rasters from layered development assets.

The pure panel backgrounds remain under ``artwork/Bifurx``. This tool crops the
full-panel SVG labels to the exact bottom-raster bounds, builds a supersampled
multiband halo, composites in linear light, and writes the two combined runtime
PNGs under ``res/bifurx``.

Typical use::

    python3 tools/composite_bifurx_bottom.py

Requires Pillow, NumPy, and Inkscape. The script discovers the installed
Windows Inkscape automatically when invoked from WSL.
"""

from __future__ import annotations

import argparse
import math
import subprocess
import tempfile
from pathlib import Path

import numpy as np
from PIL import Image, ImageFilter

from split_svg_labels import find_inkscape, path_for_executable


REPO_ROOT = Path(__file__).resolve().parents[1]
SOURCE_SIZE = (1584, 993)
RUNTIME_SIZE = (834, 523)

# Bifurx's bottom raster occupies x=0.4..70.72 mm and starts at y=77.5 mm.
# The master SVG uses 100 user units per mm. Deriving the lower edge from the
# raster aspect ratio keeps SVG and PNG sampling on precisely the same bounds.
SVG_CROP_X = 40.0
SVG_CROP_Y = 7750.0
SVG_CROP_WIDTH = 7032.0
SVG_CROP_HEIGHT = SVG_CROP_WIDTH * SOURCE_SIZE[1] / SOURCE_SIZE[0]


def render_label_mask(
    labels_svg: Path,
    size: tuple[int, int],
    inkscape_path: str | None,
) -> Image.Image:
    executable = find_inkscape(inkscape_path)
    with tempfile.TemporaryDirectory(
        prefix=".bifurx-label-mask.", dir=str(labels_svg.parent)
    ) as temporary_dir:
        output = Path(temporary_dir) / "labels.png"
        # Exporting a custom area has different Y-axis behavior across
        # Inkscape versions. Render the complete page, then crop it in Pillow's
        # unambiguous top-left coordinate system.
        full_width = math.ceil(size[0] * 7112.0004 / SVG_CROP_WIDTH)
        full_height = math.ceil(full_width * 12850.0 / 7112.0004)
        command = [
            executable,
            path_for_executable(labels_svg, executable),
            "--export-type=png",
            "--export-area-page",
            f"--export-width={full_width}",
            f"--export-height={full_height}",
            "--export-background-opacity=0",
            f"--export-filename={path_for_executable(output, executable)}",
        ]
        result = subprocess.run(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=90.0,
        )
        if result.returncode != 0 or not output.is_file():
            details = "\n".join(part for part in (result.stdout, result.stderr) if part)
            raise RuntimeError(f"Inkscape label rendering failed\n{details}")
        with Image.open(output) as rendered:
            page_mask = rendered.convert("RGBA").getchannel("A")
            scale_x = page_mask.width / 7112.0004
            scale_y = page_mask.height / 12850.0
            crop_box = (
                SVG_CROP_X * scale_x,
                SVG_CROP_Y * scale_y,
                (SVG_CROP_X + SVG_CROP_WIDTH) * scale_x,
                (SVG_CROP_Y + SVG_CROP_HEIGHT) * scale_y,
            )
            label_mask = page_mask.transform(
                size,
                Image.Transform.EXTENT,
                crop_box,
                Image.Resampling.BICUBIC,
            )
            if label_mask.getbbox() is None:
                raise RuntimeError("rendered Bifurx label crop is empty")
            return label_mask


def scale_mask(mask: Image.Image, opacity: float) -> np.ndarray:
    values = np.asarray(mask, dtype=np.float32) / 255.0
    return np.clip(values * opacity, 0.0, 1.0)


def srgb_to_linear(values: np.ndarray) -> np.ndarray:
    return np.where(
        values <= 0.04045,
        values / 12.92,
        ((values + 0.055) / 1.055) ** 2.4,
    )


def linear_to_srgb(values: np.ndarray) -> np.ndarray:
    return np.where(
        values <= 0.0031308,
        values * 12.92,
        1.055 * np.maximum(values, 0.0) ** (1.0 / 2.4) - 0.055,
    )


def composite_color(
    destination_linear: np.ndarray,
    color: tuple[int, int, int],
    alpha: np.ndarray,
) -> None:
    color_srgb = np.asarray(color, dtype=np.float32) / 255.0
    color_linear = srgb_to_linear(color_srgb)
    alpha_3d = alpha[:, :, None]
    destination_linear *= 1.0 - alpha_3d
    destination_linear += color_linear * alpha_3d


def downsample_mask(mask: Image.Image) -> Image.Image:
    return mask.resize(SOURCE_SIZE, Image.Resampling.LANCZOS)


def build_combined(
    background_path: Path,
    destination: Path,
    source_mask: Image.Image,
    supersample: int,
    fill_color: tuple[int, int, int],
    halo_color: tuple[int, int, int],
    fill_expand_px: float,
    tight_radius_px: float,
    soft_radius_px: float,
    wide_radius_px: float,
    tight_opacity: float,
    soft_opacity: float,
    wide_opacity: float,
    colors: int,
) -> None:
    tight_kernel_radius = max(1, round(tight_radius_px * supersample))
    tight_kernel_size = tight_kernel_radius * 2 + 1
    tight_mask = source_mask.filter(ImageFilter.MaxFilter(tight_kernel_size))
    tight_mask = tight_mask.filter(ImageFilter.GaussianBlur(0.65 * supersample))
    soft_mask = source_mask.filter(
        ImageFilter.GaussianBlur(soft_radius_px * supersample)
    )
    wide_mask = source_mask.filter(
        ImageFilter.GaussianBlur(wide_radius_px * supersample)
    )

    if fill_expand_px > 0.0:
        fill_kernel_radius = max(1, round(fill_expand_px * supersample))
        fill_mask = source_mask.filter(
            ImageFilter.MaxFilter(fill_kernel_radius * 2 + 1)
        )
    else:
        fill_mask = source_mask

    fill_mask = downsample_mask(fill_mask)
    tight_mask = downsample_mask(tight_mask)
    soft_mask = downsample_mask(soft_mask)
    wide_mask = downsample_mask(wide_mask)

    with Image.open(background_path) as opened:
        background = opened.convert("RGB")
    if background.size != SOURCE_SIZE:
        raise ValueError(
            f"{background_path} is {background.size}, expected full-resolution {SOURCE_SIZE}"
        )

    background_srgb = np.asarray(background, dtype=np.float32) / 255.0
    combined_linear = srgb_to_linear(background_srgb)
    # Broad-to-tight passes create a feathered field with a decisive inner rim.
    composite_color(combined_linear, halo_color, scale_mask(wide_mask, wide_opacity))
    composite_color(combined_linear, halo_color, scale_mask(soft_mask, soft_opacity))
    composite_color(combined_linear, halo_color, scale_mask(tight_mask, tight_opacity))
    composite_color(combined_linear, fill_color, scale_mask(fill_mask, 1.0))

    combined_srgb = np.clip(linear_to_srgb(combined_linear), 0.0, 1.0)
    combined = Image.fromarray(
        np.rint(combined_srgb * 255.0).astype(np.uint8), mode="RGB"
    )
    combined = combined.resize(RUNTIME_SIZE, Image.Resampling.LANCZOS)
    if colors > 0:
        combined = combined.quantize(colors=colors, method=Image.Quantize.MEDIANCUT)
    destination.parent.mkdir(parents=True, exist_ok=True)
    combined.save(destination, format="PNG", optimize=True)
    print(f"{background_path} + labels -> {destination} ({RUNTIME_SIZE[0]}x{RUNTIME_SIZE[1]})")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--light-background",
        type=Path,
        default=REPO_ROOT / "artwork" / "Bifurx" / "Bifurx-LB-background.png",
    )
    parser.add_argument(
        "--dark-background",
        type=Path,
        default=REPO_ROOT / "artwork" / "Bifurx" / "Bifurx-DB-background.png",
    )
    parser.add_argument(
        "--labels-svg",
        type=Path,
        default=REPO_ROOT / "res" / "bifurx.labels.svg",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=REPO_ROOT / "res" / "bifurx",
    )
    parser.add_argument("--inkscape-path", help="override the Inkscape executable")
    parser.add_argument(
        "--supersample",
        type=int,
        default=2,
        help="label-mask supersampling factor (default: 2)",
    )
    parser.add_argument(
        "--colors",
        type=int,
        default=0,
        help="indexed-PNG palette size; 0 preserves full RGB (default: 0)",
    )
    args = parser.parse_args()

    for source in (args.light_background, args.dark_background, args.labels_svg):
        if not source.is_file():
            parser.error(f"source asset does not exist: {source}")
    if not 1 <= args.supersample <= 8:
        parser.error("--supersample must be between 1 and 8")
    if args.colors != 0 and not 16 <= args.colors <= 256:
        parser.error("--colors must be 0 or between 16 and 256")

    mask_size = (
        SOURCE_SIZE[0] * args.supersample,
        SOURCE_SIZE[1] * args.supersample,
    )
    label_mask = render_label_mask(args.labels_svg, mask_size, args.inkscape_path)
    build_combined(
        args.light_background,
        args.output_dir / "Bifurx-LB.png",
        label_mask,
        args.supersample,
        fill_color=(18, 20, 25),
        halo_color=(248, 248, 244),
        # At Rack's 100% scale a thin dark glyph and bright inner rim collapse
        # toward the rim color. Give the light-panel glyph a decisive core and
        # retain the broader feather at lower strength.
        fill_expand_px=1.5,
        tight_radius_px=5.0,
        soft_radius_px=10.0,
        wide_radius_px=24.0,
        tight_opacity=0.48,
        soft_opacity=0.28,
        wide_opacity=0.13,
        colors=args.colors,
    )
    build_combined(
        args.dark_background,
        args.output_dir / "Bifurx-DB.png",
        label_mask,
        args.supersample,
        fill_color=(238, 240, 244),
        halo_color=(2, 4, 8),
        fill_expand_px=1.5,
        tight_radius_px=5.0,
        soft_radius_px=10.0,
        wide_radius_px=24.0,
        tight_opacity=0.48,
        soft_opacity=0.28,
        wide_opacity=0.13,
        colors=args.colors,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
