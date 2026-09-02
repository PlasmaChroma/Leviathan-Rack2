#!/usr/bin/env python3
"""Generate the shipped Bifurx bottom rasters from layered development assets.

The pure panel backgrounds remain under ``artwork/Bifurx``. This tool crops the
full-panel SVG labels to the exact bottom-raster bounds, builds a supersampled
multiband halo, composites in linear light, and writes the two combined runtime
PNGs under ``res/bifurx``.

Typical use::

    # Rebuild the combined panels from separate backgrounds and SVG labels.
    python3 tools/composite_bifurx_bottom.py

    # Preserve the approved panel treatment and add only static conduits.
    python3 tools/composite_bifurx_bottom.py \
        --runtime-base-dir artwork/Bifurx/runtime-base

Requires Pillow, NumPy, and Inkscape. The script discovers the installed
Windows Inkscape automatically when invoked from WSL.
"""

from __future__ import annotations

import argparse
import math
import re
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

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
SVG_UNITS_PER_MM = 100.0
CONDUIT_GROUP_ID = "plasma_conduit_anchors"
CONDUIT_STROKES = (
    (2.60, (255, 132, 20), 12 / 255.0),
    (1.85, (255, 145, 23), 20 / 255.0),
    (1.25, (255, 158, 28), 35 / 255.0),
    (0.88, (255, 171, 36), 58 / 255.0),
    (0.62, (67, 32, 3), 145 / 255.0),
    (0.49, (255, 179, 51), 232 / 255.0),
    (0.29, (255, 218, 125), 246 / 255.0),
    (0.13, (255, 252, 225), 1.0),
)
PATH_TOKEN_RE = re.compile(
    r"[MLml]|[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?"
)


def local_name(tag: str) -> str:
    return tag.rsplit("}", 1)[-1]


def parse_straight_svg_path(path_data: str, context: str) -> list[tuple[float, float]]:
    """Parse the deliberately restricted M/L conduit-anchor path grammar."""
    tokens = PATH_TOKEN_RE.findall(path_data.replace(",", " "))
    points: list[tuple[float, float]] = []
    command = ""
    cursor = 0
    while cursor < len(tokens):
        token = tokens[cursor]
        if token in {"M", "L", "m", "l"}:
            if token.islower():
                raise ValueError(f"{context}: relative SVG path commands are unsupported")
            command = token
            cursor += 1
            continue
        if command not in {"M", "L"} or cursor + 1 >= len(tokens):
            raise ValueError(f"{context}: expected an absolute M/L coordinate pair")
        points.append((float(token), float(tokens[cursor + 1])))
        cursor += 2
        # SVG treats coordinate pairs following M as implicit L commands.
        command = "L"
    if len(points) < 2:
        raise ValueError(f"{context}: conduit path must contain at least two points")
    return points


def load_conduit_segments(master_svg: Path) -> list[tuple[tuple[float, float], tuple[float, float]]]:
    root = ET.parse(master_svg).getroot()
    group = next(
        (element for element in root.iter() if element.get("id") == CONDUIT_GROUP_ID),
        None,
    )
    if group is None:
        raise ValueError(f"{master_svg}: missing #{CONDUIT_GROUP_ID}")
    if group.get("transform"):
        raise ValueError(f"{master_svg}: transformed conduit group is unsupported")

    segments: list[tuple[tuple[float, float], tuple[float, float]]] = []
    for element in group:
        if local_name(element.tag) != "path":
            continue
        if element.get("transform"):
            raise ValueError(
                f"{master_svg}: transformed conduit path {element.get('id', '<unnamed>')} is unsupported"
            )
        points = parse_straight_svg_path(
            element.get("d", ""),
            f"{master_svg}#{element.get('id', '<unnamed>')}",
        )
        segments.extend(zip(points, points[1:]))
    if not segments:
        raise ValueError(f"{master_svg}: #{CONDUIT_GROUP_ID} contains no segments")
    return segments


def draw_rounded_segment(
    draw: ImageDraw.ImageDraw,
    start: tuple[float, float],
    end: tuple[float, float],
    width: int,
    fill: int,
) -> None:
    draw.line((start, end), fill=fill, width=width)
    radius = width * 0.5
    for x, y in (start, end):
        draw.ellipse((x - radius, y - radius, x + radius, y + radius), fill=fill)


def render_conduit_masks(
    master_svg: Path,
    supersample: int,
    target_size: tuple[int, int] = SOURCE_SIZE,
) -> list[tuple[tuple[int, int, int], float, Image.Image]]:
    size = (target_size[0] * supersample, target_size[1] * supersample)
    scale_x = size[0] / SVG_CROP_WIDTH
    scale_y = size[1] / SVG_CROP_HEIGHT
    pixels_per_mm = size[0] / (SVG_CROP_WIDTH / SVG_UNITS_PER_MM)

    def transform(point: tuple[float, float]) -> tuple[float, float]:
        return (
            (point[0] - SVG_CROP_X) * scale_x,
            (point[1] - SVG_CROP_Y) * scale_y,
        )

    transformed = [
        (transform(start), transform(end))
        for start, end in load_conduit_segments(master_svg)
    ]
    layers: list[tuple[tuple[int, int, int], float, Image.Image]] = []
    for width_mm, color, opacity in CONDUIT_STROKES:
        mask = Image.new("L", size, 0)
        draw = ImageDraw.Draw(mask)
        width = max(1, round(width_mm * pixels_per_mm))
        for start, end in transformed:
            draw_rounded_segment(draw, start, end, width, 255)
        layers.append(
            (color, opacity, mask.resize(target_size, Image.Resampling.LANCZOS))
        )
    return layers


def feather_dark_conduit_bloom(
    layers: list[tuple[tuple[int, int, int], float, Image.Image]],
    target_size: tuple[int, int],
) -> list[tuple[tuple[int, int, int], float, Image.Image]]:
    """Replace visible nested glow bands with a continuous dark-panel bloom."""
    if len(layers) < 5:
        return layers
    pixels_per_mm = target_size[0] / (SVG_CROP_WIDTH / SVG_UNITS_PER_MM)
    sheath_mask = layers[4][2]
    bloom = [
        (
            (255, 132, 20),
            0.24,
            sheath_mask.filter(ImageFilter.GaussianBlur(1.05 * pixels_per_mm)),
        ),
        (
            (255, 158, 28),
            0.34,
            sheath_mask.filter(ImageFilter.GaussianBlur(0.48 * pixels_per_mm)),
        ),
        (
            (255, 171, 36),
            0.22,
            sheath_mask.filter(ImageFilter.GaussianBlur(0.20 * pixels_per_mm)),
        ),
    ]
    # Preserve the deliberate dark sheath and bright, sharply resolved core.
    return bloom + layers[4:]


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


def bake_conduits_onto_runtime_asset(
    base_path: Path,
    destination: Path,
    conduit_layers: list[tuple[tuple[int, int, int], float, Image.Image]],
) -> None:
    """Add conduits without regenerating or resampling approved panel art."""
    with Image.open(base_path) as source:
        base = source.convert("RGB")
    if base.size != RUNTIME_SIZE:
        raise ValueError(
            f"{base_path}: expected runtime size {RUNTIME_SIZE}, got {base.size}"
        )

    combined_linear = srgb_to_linear(np.asarray(base, dtype=np.float32) / 255.0)
    for conduit_color, conduit_opacity, conduit_mask in conduit_layers:
        composite_color(
            combined_linear,
            conduit_color,
            scale_mask(conduit_mask, conduit_opacity),
        )
    encoded = np.clip(
        np.rint(linear_to_srgb(combined_linear) * 255.0), 0, 255
    ).astype(np.uint8)
    destination.parent.mkdir(parents=True, exist_ok=True)
    Image.fromarray(encoded, mode="RGB").save(destination, optimize=True)
    print(f"{base_path} + conduits -> {destination} ({base.width}x{base.height})")


def downsample_mask(mask: Image.Image) -> Image.Image:
    return mask.resize(SOURCE_SIZE, Image.Resampling.LANCZOS)


def resize_linear_rgb(
    image_linear: np.ndarray,
    size: tuple[int, int],
) -> np.ndarray:
    """Resize floating-point linear RGB without an intervening gamma encode."""
    channels = []
    for channel_index in range(3):
        channel = Image.fromarray(
            image_linear[:, :, channel_index].astype(np.float32),
            mode="F",
        )
        resized = channel.resize(size, Image.Resampling.LANCZOS)
        channels.append(np.asarray(resized, dtype=np.float32))
    return np.clip(np.stack(channels, axis=2), 0.0, 1.0)


def build_combined(
    background_path: Path,
    destination: Path,
    source_mask: Image.Image,
    conduit_layers: list[tuple[tuple[int, int, int], float, Image.Image]],
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
    linear_light_resize: bool,
    output_size: tuple[int, int],
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
    # The modern panel's static conduits are baked below labels and controls.
    # Broad-to-tight ordering matches the former NanoVG framebuffer renderer.
    for conduit_color, conduit_opacity, conduit_mask in conduit_layers:
        composite_color(
            combined_linear,
            conduit_color,
            scale_mask(conduit_mask, conduit_opacity),
        )
    # Broad-to-tight passes create a feathered field with a decisive inner rim.
    composite_color(combined_linear, halo_color, scale_mask(wide_mask, wide_opacity))
    composite_color(combined_linear, halo_color, scale_mask(soft_mask, soft_opacity))
    composite_color(combined_linear, halo_color, scale_mask(tight_mask, tight_opacity))
    composite_color(combined_linear, fill_color, scale_mask(fill_mask, 1.0))

    if linear_light_resize and output_size != SOURCE_SIZE:
        combined_linear = resize_linear_rgb(combined_linear, output_size)
    combined_srgb = np.clip(linear_to_srgb(combined_linear), 0.0, 1.0)
    combined = Image.fromarray(
        np.rint(combined_srgb * 255.0).astype(np.uint8), mode="RGB"
    )
    if not linear_light_resize and output_size != SOURCE_SIZE:
        combined = combined.resize(output_size, Image.Resampling.LANCZOS)
    if colors > 0:
        combined = combined.quantize(colors=colors, method=Image.Quantize.MEDIANCUT)
    destination.parent.mkdir(parents=True, exist_ok=True)
    combined.save(destination, format="PNG", optimize=True)
    if output_size == SOURCE_SIZE:
        resize_mode = "native/no resize"
    else:
        resize_mode = (
            "linear-light Lanczos" if linear_light_resize else "sRGB Lanczos"
        )
    print(
        f"{background_path} + labels -> {destination} "
        f"({output_size[0]}x{output_size[1]}, {resize_mode})"
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--light-background",
        type=Path,
        default=(
            REPO_ROOT
            / "artwork"
            / "Bifurx"
            / "Bifurx-LB-background-output-shift-generated-v3.png"
        ),
    )
    parser.add_argument(
        "--dark-background",
        type=Path,
        default=(
            REPO_ROOT
            / "artwork"
            / "Bifurx"
            / "Bifurx-DB-background-output-shift-generated-v3.png"
        ),
    )
    parser.add_argument(
        "--labels-svg",
        type=Path,
        default=REPO_ROOT / "res" / "bifurx.theme-text.svg",
        help="runtime theme-text SVG containing the functional panel labels",
    )
    parser.add_argument(
        "--master-svg",
        type=Path,
        default=REPO_ROOT / "res" / "bifurx.svg",
        help="editable master SVG containing conduit anchors",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=REPO_ROOT / "res" / "bifurx",
    )
    parser.add_argument(
        "--runtime-base-dir",
        type=Path,
        help=(
            "bake only conduits onto existing Bifurx-LB/DB.png assets in this "
            "directory; skips background and label regeneration"
        ),
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
    parser.add_argument(
        "--linear-light-resize",
        action="store_true",
        help="resize floating-point linear RGB before encoding the runtime PNG",
    )
    parser.add_argument(
        "--native-resolution",
        action="store_true",
        help=f"write the native bottom-master size {SOURCE_SIZE[0]}x{SOURCE_SIZE[1]}",
    )
    parser.add_argument(
        "--no-bake-conduits",
        action="store_true",
        help="omit the static conduit layer (diagnostic compatibility option)",
    )
    parser.add_argument(
        "--diagnostics-dir",
        type=Path,
        help="write the broadest and narrowest conduit masks here",
    )
    args = parser.parse_args()

    sources = [args.master_svg]
    if args.runtime_base_dir:
        runtime_inputs = (
            args.runtime_base_dir / "Bifurx-LB.png",
            args.runtime_base_dir / "Bifurx-DB.png",
        )
        runtime_outputs = (
            args.output_dir / "Bifurx-LB.png",
            args.output_dir / "Bifurx-DB.png",
        )
        for source, destination in zip(runtime_inputs, runtime_outputs):
            if source.resolve() == destination.resolve():
                parser.error(
                    "--runtime-base-dir must differ from --output-dir; "
                    "in-place baking is cumulative"
                )
        sources.extend(
            runtime_inputs
        )
    else:
        sources.extend(
            (args.light_background, args.dark_background, args.labels_svg)
        )
    for source in sources:
        if not source.is_file():
            parser.error(f"source asset does not exist: {source}")
    if not 1 <= args.supersample <= 8:
        parser.error("--supersample must be between 1 and 8")
    if args.colors != 0 and not 16 <= args.colors <= 256:
        parser.error("--colors must be 0 or between 16 and 256")
    if args.runtime_base_dir and args.native_resolution:
        parser.error("--native-resolution requires the full source-compositor path")

    conduit_target_size = RUNTIME_SIZE if args.runtime_base_dir else SOURCE_SIZE
    conduit_layers = (
        []
        if args.no_bake_conduits
        else render_conduit_masks(
            args.master_svg, args.supersample, conduit_target_size
        )
    )
    if args.diagnostics_dir and conduit_layers:
        args.diagnostics_dir.mkdir(parents=True, exist_ok=True)
        conduit_layers[0][2].save(args.diagnostics_dir / "conduit-wide-mask.png")
        conduit_layers[-1][2].save(args.diagnostics_dir / "conduit-core-mask.png")
    if args.runtime_base_dir:
        if not conduit_layers:
            parser.error("--runtime-base-dir cannot be combined with --no-bake-conduits")
        bake_conduits_onto_runtime_asset(
            args.runtime_base_dir / "Bifurx-LB.png",
            args.output_dir / "Bifurx-LB.png",
            conduit_layers,
        )
        bake_conduits_onto_runtime_asset(
            args.runtime_base_dir / "Bifurx-DB.png",
            args.output_dir / "Bifurx-DB.png",
            conduit_layers,
        )
        return 0

    mask_size = (
        SOURCE_SIZE[0] * args.supersample,
        SOURCE_SIZE[1] * args.supersample,
    )
    label_mask = render_label_mask(args.labels_svg, mask_size, args.inkscape_path)
    output_size = SOURCE_SIZE if args.native_resolution else RUNTIME_SIZE
    dark_conduit_layers = feather_dark_conduit_bloom(
        conduit_layers, conduit_target_size
    )
    build_combined(
        args.light_background,
        args.output_dir / "Bifurx-LB.png",
        label_mask,
        conduit_layers,
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
        linear_light_resize=args.linear_light_resize,
        output_size=output_size,
    )
    build_combined(
        args.dark_background,
        args.output_dir / "Bifurx-DB.png",
        label_mask,
        dark_conduit_layers,
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
        linear_light_resize=args.linear_light_resize,
        output_size=output_size,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
