#!/usr/bin/env python3
"""Stage current Bifurx sources with Pro's bootstrap for an isolated DRM build.

No source synchronization, installation, or license activation is performed.
The vendored DRM header remains in Pro (or the explicitly configured directory).
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from sync_bifurx_to_pro import expected_files, generated_atlas

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "build" / "premium-validation"
PRO_FILES = ("Makefile", "plugin.json", "eula.md", "src/plugin.cpp", "src/plugin.hpp")


def write_if_changed(path: Path, data: bytes) -> None:
    if path.is_file() and path.read_bytes() == data:
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def stage(pro_root: Path, drm_dir: Path, rack_dir: Path) -> Path:
    pro_root, drm_dir, rack_dir = pro_root.resolve(), drm_dir.resolve(), rack_dir.resolve()
    # Validate everything before touching a prior build snapshot.
    for path in [*(pro_root / p for p in PRO_FILES), drm_dir / "drm.hpp", rack_dir / "plugin.mk"]:
        if not path.is_file():
            raise RuntimeError(f"Missing Premium build dependency: {path}")
    manifest = json.loads((pro_root / "plugin.json").read_text(encoding="utf-8"))
    if manifest.get("slug") != "Leviathan-Pro" or [m.get("slug") for m in manifest.get("modules", [])] != ["Bifurx"]:
        raise RuntimeError("Premium validation requires the Leviathan-Pro / Bifurx manifest")

    payload = expected_files(ROOT)
    payload.update({p: (pro_root / p).read_bytes() for p in PRO_FILES})
    # Flag/header-location changes invalidate this build's objects, never the
    # normal Leviathan build. Source changes retain normal incremental behavior.
    payload["Makefile"] += (
        b'\n# Premium validation always compiles the actual license hooks.\n'
        b'override FLAGS += -DLEVIATHAN_PRO_DRM=1 -I"$(DRM_DIR)"\n'
        b'$(OBJECTS): .premium-config.json\n'
    )
    config = {
        "proRoot": str(pro_root), "drmDirectory": str(drm_dir), "rackDirectory": str(rack_dir),
        "drmSha256": hashlib.sha256((drm_dir / "drm.hpp").read_bytes()).hexdigest(),
        "makefileSha256": hashlib.sha256(payload["Makefile"]).hexdigest(),
    }
    payload[".premium-config.json"] = (json.dumps(config, indent=2) + "\n").encode()

    output = OUTPUT.resolve()
    if output != ROOT / "build" / "premium-validation":
        raise RuntimeError("Premium staging directory must not redirect outside its dedicated build path")
    ownership = output / ".premium-files.json"
    previous = set(json.loads(ownership.read_text()) if ownership.is_file() else [])
    current = set(payload) | {"src/PanelAnchorAtlas.cpp"}
    stale = previous - current

    def target(relative: str) -> Path:
        path = output / relative
        if not path.resolve().is_relative_to(output):
            raise RuntimeError(f"Unsafe Premium staging path: {relative}")
        return path

    for relative in previous | current:
        target(relative)
    payload["src/PanelAnchorAtlas.cpp"] = generated_atlas(ROOT, output, payload, stale)
    for relative in stale:
        path = target(relative)
        if path.is_file():
            path.unlink()
    for relative, data in payload.items():
        write_if_changed(target(relative), data)
    write_if_changed(ownership, (json.dumps(sorted(current), indent=2) + "\n").encode())
    return output


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pro-root", type=Path, default=ROOT.parent / "Leviathan-Pro")
    parser.add_argument("--drm-dir", type=Path)
    parser.add_argument("--rack-dir", type=Path, default=ROOT.parent / "Rack-SDK")
    args = parser.parse_args()
    try:
        output = stage(args.pro_root, args.drm_dir or args.pro_root / "DRM", args.rack_dir)
    except (OSError, ValueError, RuntimeError) as error:
        parser.exit(1, f"error: {error}\n")
    print(f"Premium source snapshot ready: {output}")
    print("Identity: Leviathan-Pro / Bifurx; DRM enabled by Pro's Makefile")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
