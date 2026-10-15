#!/usr/bin/env python3
"""Rebuild Bifurx split SVGs, panel rasters, and the repository anchor atlas."""

from __future__ import annotations

import argparse
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-root', type=Path, default=ROOT,
                        help='repository root or isolated preview directory (default: this repository)')
    parser.add_argument('--colors', type=int, default=256,
                        help='palette size 16..256; 0 for full color without PNG optimization tools')
    parser.add_argument('--native-resolution', action='store_true')
    parser.add_argument('--inkscape-path')
    args = parser.parse_args()
    if args.colors != 0 and not 16 <= args.colors <= 256:
        parser.error('--colors must be 0 or between 16 and 256')

    # Check dependencies before altering runtime assets.
    try:
        import PIL
        import numpy
        from split_svg_labels import find_inkscape
        inkscape = find_inkscape(args.inkscape_path)
        if args.colors:
            for tool in ('pngquant', 'optipng'):
                if not shutil.which(tool):
                    raise RuntimeError(f'{tool} is required; install it or use --colors 0')
    except (ImportError, RuntimeError) as error:
        parser.error(f'{error}; see doc/Bifurx/artwork-pipeline.md')

    output = args.output_root.resolve()
    if output != ROOT:
        # Preserve the full SVG union, including other Pro modules, for atlas generation.
        (output / 'res').mkdir(parents=True, exist_ok=True)
        for source in (ROOT / 'res').glob('*.svg'):
            shutil.copyfile(source, output / 'res' / source.name)
    master = output / 'res/bifurx.svg'

    def run(tool: str, *arguments: str) -> None:
        subprocess.run([sys.executable, str(ROOT / 'tools' / tool), *arguments],
                       cwd=ROOT, check=True)

    run('split_svg_labels.py', str(master), '--overwrite', '--inkscape-path', inkscape)
    raster_options = ['--output-dir', str(output / 'res/bifurx'), '--colors', str(args.colors)]
    if args.native_resolution:
        raster_options.append('--native-resolution')
    run('convert_bifurx_rasters.py',
        '--light-top', str(ROOT / 'artwork/Bifurx/Bifurx-LT-source.png'),
        '--dark-top', str(ROOT / 'artwork/Bifurx/Bifurx-DT-source.png'), *raster_options)
    run('composite_bifurx_bottom.py', '--master-svg', str(master),
        '--labels-svg', str(output / 'res/bifurx.theme-text-input.svg'),
        str(output / 'res/bifurx.theme-text-output.svg'),
        '--inkscape-path', inkscape, *raster_options)
    (output / 'src').mkdir(parents=True, exist_ok=True)
    run('generate_panel_anchor_atlas.py', '--repo-root', str(output))
    print(f'Bifurx assets rebuilt in {output}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
