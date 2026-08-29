#!/usr/bin/env python3
"""Resize and optimize Bifurx panel raster sources for Rack.

Split-bottom example:
    python3 tools/convert_bifurx_rasters.py \
        --light-bottom-background artwork/Bifurx/Bifurx-LB-background.png \
        --dark-bottom-background artwork/Bifurx/Bifurx-DB-background.png \
        --output-dir res/bifurx

Output filenames follow the selected asset flags and are written beside the
other Bifurx runtime rasters in res/bifurx by default.
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
    "light_bottom_background": ("Bifurx-LB-background.png", (834, 523)),
    "dark_bottom_background": ("Bifurx-DB-background.png", (834, 523)),
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
        quantize_method = (
            Image.Quantize.FASTOCTREE if has_alpha else Image.Quantize.MEDIANCUT
        )
        image = image.quantize(colors=colors, method=quantize_method)
        destination.parent.mkdir(parents=True, exist_ok=True)
        image.save(destination, format="PNG", optimize=True)

    print(f"{source} -> {destination} ({size[0]}x{size[1]}, <= {colors} colors)")


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
        help="maximum indexed-PNG palette size (default: 96)",
    )
    args = parser.parse_args()

    requested = [(key, getattr(args, key)) for key in ASSETS if getattr(args, key)]
    if not requested:
        parser.error("provide at least one top or bottom source image")
    if not 2 <= args.colors <= 256:
        parser.error("--colors must be between 2 and 256")

    for key, source in requested:
        if not source.is_file():
            parser.error(f"source image does not exist: {source}")
        output_name, size = ASSETS[key]
        convert(source, args.output_dir / output_name, size, args.colors)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
