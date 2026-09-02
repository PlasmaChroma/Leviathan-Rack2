#!/usr/bin/env python3
"""Resize and optimize ordinary Bifurx panel raster sources for Rack.

Top-panel example:
    python3 tools/convert_bifurx_rasters.py \
        --light-top artwork/Bifurx/Bifurx-LT-source.png \
        --dark-top artwork/Bifurx/Bifurx-DT-source.png \
        --output-dir res/bifurx

For native-resolution, full-color runtime assets, append::

        --native-resolution --colors 0

Output filenames follow the selected asset flags and are written beside the
other Bifurx runtime rasters in res/bifurx by default. Generate the combined
bottom backgrounds, labels, and halos with ``composite_bifurx_bottom.py``.
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageOps


REPO_ROOT = Path(__file__).resolve().parents[1]
ASSETS = {
    "light_top": ("Bifurx-LT.png", (834, 111)),
    "dark_top": ("Bifurx-DT.png", (834, 111)),
    "light_bottom": ("Bifurx-LB.png", (834, 523)),
    "dark_bottom": ("Bifurx-DB.png", (834, 523)),
}


def convert(source: Path, destination: Path, size: tuple[int, int], colors: int) -> None:
    with Image.open(source) as opened:
        image = ImageOps.exif_transpose(opened)
        has_alpha = image.mode in {"RGBA", "LA"} or "transparency" in image.info
        image = image.convert("RGBA" if has_alpha else "RGB")
        image = ImageOps.fit(
            image,
            size,
            method=Image.Resampling.LANCZOS,
            centering=(0.5, 0.5),
        )
        if colors > 0:
            quantize_method = (
                Image.Quantize.FASTOCTREE if has_alpha else Image.Quantize.MEDIANCUT
            )
            image = image.quantize(colors=colors, method=quantize_method)
        destination.parent.mkdir(parents=True, exist_ok=True)
        image.save(destination, format="PNG", optimize=True)

    color_description = f"<= {colors} colors" if colors > 0 else "full color"
    print(
        f"{source} -> {destination} "
        f"({size[0]}x{size[1]}, {color_description})"
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    for key in ASSETS:
        parser.add_argument(
            f"--{key.replace('_', '-')}",
            type=Path,
            help=f"source image for {ASSETS[key][0]}",
        )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=REPO_ROOT / "res" / "bifurx",
        help="destination directory (default: repository res/bifurx directory)",
    )
    parser.add_argument(
        "--colors",
        type=int,
        default=96,
        help="maximum indexed-PNG palette size; 0 preserves full color (default: 96)",
    )
    parser.add_argument(
        "--native-resolution",
        action="store_true",
        help="preserve each source image's native dimensions",
    )
    args = parser.parse_args()

    requested = [(key, getattr(args, key)) for key in ASSETS if getattr(args, key)]
    if not requested:
        parser.error("provide at least one top or bottom source image")
    if args.colors != 0 and not 2 <= args.colors <= 256:
        parser.error("--colors must be 0 or between 2 and 256")

    for key, source in requested:
        if not source.is_file():
            parser.error(f"source image does not exist: {source}")
        output_name, size = ASSETS[key]
        if args.native_resolution:
            with Image.open(source) as opened:
                size = opened.size
        convert(source, args.output_dir / output_name, size, args.colors)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
