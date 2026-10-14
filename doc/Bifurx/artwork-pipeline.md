# Bifurx artwork and build pipeline

The Bifurx sync includes the editable master `res/bifurx.svg`, four original
PNGs in `artwork/Bifurx`, the generated runtime resources, and all panel
generation tools. These paths are managed by the source repository's
`tools/sync_bifurx_to_pro.py`; subsequent syncs overwrite local Pro edits to
managed files. Make lasting edits in the source repository and sync again.
Pro can regenerate the assets independently without accessing that repository.

## Dependencies

Use Python 3.10 or newer and install the artwork dependencies into that same
interpreter (a virtual environment is recommended):

```sh
python -m pip install -r tools/bifurx-artwork-requirements.txt
```

Install Inkscape and the Rubik font used by the master. Ancient is referenced
by hidden title artwork; install it if editing or enabling that text. Fonts
are not bundled. Inkscape is discovered on PATH or at its usual Windows
location; `--inkscape-path` can select it explicitly. Default optimized PNG
generation also requires `pngquant` and `optipng` on PATH. `--colors 0` skips
these two tools and writes larger, full-color PNGs.

## Regenerate

From either repository root:

```sh
python tools/build_bifurx_assets.py
```

The command splits the master into panel, labels, background and semantic
theme-text SVGs; converts both top-panel masters; composites both bottom
backgrounds with the SVG labels, halos and static conduits; then generates
`src/PanelAnchorAtlas.cpp` from the repository's complete SVG union.
Defaults are the tools' 834-pixel-wide, 256-color runtime settings. Use
`--native-resolution` for full source dimensions. Regeneration can differ
from previously approved PNGs or outlined text when tool/font versions or
generation settings differ; inspect a preview before replacing shipped assets:

```sh
python tools/build_bifurx_assets.py --output-root build/bifurx-artwork-preview --colors 0
```

Edit only the master SVG, not the generated split SVGs. For SVG-only edits:

```sh
python tools/split_svg_labels.py res/bifurx.svg --overwrite
python tools/generate_panel_anchor_atlas.py --repo-root .
```

Raster conversion and composition can also run separately; their `--help`
documents color treatment, supersampling and diagnostic options. The optional
`--runtime-base-dir` compositor mode requires separately saved bottom rasters
and is cumulative; the complete pipeline always starts from original artwork.

## Build Pro

In the native MINGW64 shell on Windows, with the sibling Rack SDK available:

```sh
make -j10 plugin.dll
make -j10 dist
```

DRM is enabled by default and requires Pro's `DRM/drm.hpp`. Use `PRO_DRM=0`
on each build command for an unlicensed local test build. The existing Pro
Makefile and plugin identity remain Pro-owned. Development artwork and tools
are not needed at runtime. Sync verification runs from the source repository:

```sh
python tools/sync_bifurx_to_pro.py --check
```
