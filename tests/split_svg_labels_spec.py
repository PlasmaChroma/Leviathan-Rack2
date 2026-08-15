#!/usr/bin/env python3

from __future__ import annotations

import importlib.util
from pathlib import Path
import tempfile
import xml.etree.ElementTree as ET


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "split_svg_labels", ROOT / "tools" / "split_svg_labels.py"
)
assert SPEC and SPEC.loader
splitter = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(splitter)


def by_id(root: ET.Element, wanted: str) -> ET.Element:
    return next(elem for elem in root.iter() if elem.attrib.get("id") == wanted)


def main() -> int:
    fixture = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100">
  <g id="panel_art">
    <rect id="generic" width="100" height="100" fill="#c04080"/>
    <g id="glass_input">
      <path id="input" d="M 1 1 H 40 V 40 H 1 Z" style="opacity:0.33;fill:#5740bf;fill-opacity:1"/>
      <path id="highlight" d="M 1 1 H 40" fill="#ffffff"/>
    </g>
    <g id="glass_output">
      <rect id="output" x="50" y="50" width="40" height="40" fill="rgb(28, 204, 217)"/>
    </g>
  </g>
  <g id="labels"><text>TEST</text></g>
</svg>"""
    with tempfile.TemporaryDirectory(prefix="split-svg-spec-") as directory:
        source = Path(directory) / "fixture.svg"
        source.write_text(fixture, encoding="utf-8")
        panel, _ = splitter.split_svg(
            source_path=source,
            label_id="labels",
            panel_suffix=".panel",
            labels_suffix=".labels",
            overwrite=True,
            cleanup=True,
            strip_panel_text=True,
            outline_label_text=False,
            inkscape_path=None,
            inkscape_timeout_sec=1.0,
        )

        source_root = ET.parse(source).getroot()
        panel_root = ET.parse(panel).getroot()
        checks = {
            "master preview pigment remains colored": "fill:#5740bf" in by_id(source_root, "input").attrib["style"],
            "semantic style pigment becomes neutral": "fill:#000000" in by_id(panel_root, "input").attrib["style"],
            "semantic attribute pigment becomes neutral": by_id(panel_root, "output").attrib["fill"] == "#000000",
            "achromatic highlight is preserved": by_id(panel_root, "highlight").attrib["fill"] == "#ffffff",
            "generic authored pigment is preserved": by_id(panel_root, "generic").attrib["fill"] == "#c04080",
            "semantic identity is preserved": by_id(panel_root, "glass_input").attrib["data-theme-runtime-substrate"] == "neutral",
            "labels are absent from panel": all(elem.attrib.get("id") != "labels" for elem in panel_root.iter()),
        }

    for name, passed in checks.items():
        print(f"[{'PASS' if passed else 'FAIL'}] {name}")
    failed = sum(not passed for passed in checks.values())
    print(f"Split SVG labels spec: {len(checks) - failed}/{len(checks)} passed")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
