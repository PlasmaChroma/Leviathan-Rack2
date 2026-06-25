#!/usr/bin/env python3
"""
split_svg_labels.py

Split an SVG into:
  - a panel/art SVG with labels removed
  - a labels-only SVG containing only the chosen label group

Expected source convention:
  <g id="labels"> ... </g>

Example:
  python3 split_svg_labels.py res/IntegralFlux.svg
  python3 split_svg_labels.py res/IntegralFlux.svg --label-id labels
  python3 split_svg_labels.py res --recursive
"""

from __future__ import annotations

import argparse
import copy
import sys
from pathlib import Path
import xml.etree.ElementTree as ET


SVG_NS = "http://www.w3.org/2000/svg"
INKSCAPE_NS = "http://www.inkscape.org/namespaces/inkscape"
SODIPODI_NS = "http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
XLINK_NS = "http://www.w3.org/1999/xlink"

ET.register_namespace("", SVG_NS)
ET.register_namespace("inkscape", INKSCAPE_NS)
ET.register_namespace("sodipodi", SODIPODI_NS)
ET.register_namespace("xlink", XLINK_NS)


def qname(ns: str, tag: str) -> str:
    return f"{{{ns}}}{tag}"


def local_name(tag: str) -> str:
    if tag.startswith("{"):
        return tag.split("}", 1)[1]
    return tag


def get_id(elem: ET.Element) -> str | None:
    return elem.attrib.get("id")


def find_parent_and_child_by_id(
    root: ET.Element,
    wanted_id: str,
) -> tuple[ET.Element | None, ET.Element | None]:
    for parent in root.iter():
        for child in list(parent):
            if get_id(child) == wanted_id:
                return parent, child
    return None, None


def copy_svg_shell(source_root: ET.Element) -> ET.Element:
    """
    Create a new root <svg> preserving the important root attributes.
    """
    new_root = ET.Element(source_root.tag)

    preserve_attrs = [
        "width",
        "height",
        "viewBox",
        "version",
        "id",
    ]

    for attr in preserve_attrs:
        if attr in source_root.attrib:
            new_root.attrib[attr] = source_root.attrib[attr]

    # Preserve any namespaced/root metadata that may affect rendering.
    for attr, value in source_root.attrib.items():
        if attr.startswith("{") and attr not in new_root.attrib:
            new_root.attrib[attr] = value

    return new_root


def copy_defs_and_styles(source_root: ET.Element, dest_root: ET.Element) -> None:
    """
    Copy top-level <defs> and <style> nodes because path labels may depend on
    gradients, filters, CSS classes, masks, etc.
    """
    for child in list(source_root):
        name = local_name(child.tag)
        if name in {"defs", "style"}:
            dest_root.append(copy.deepcopy(child))


def remove_editor_junk(root: ET.Element) -> None:
    """
    Conservative cleanup for labels/panel outputs.
    Removes obvious non-rendering editor metadata.
    """
    junk_names = {
        "metadata",
        "namedview",
    }

    for parent in root.iter():
        for child in list(parent):
            name = local_name(child.tag)
            if name in junk_names:
                parent.remove(child)


def split_svg(
    source_path: Path,
    label_id: str,
    panel_suffix: str,
    labels_suffix: str,
    overwrite: bool,
    cleanup: bool,
) -> tuple[Path, Path]:
    tree = ET.parse(source_path)
    root = tree.getroot()

    parent, label_group = find_parent_and_child_by_id(root, label_id)

    if parent is None or label_group is None:
        raise RuntimeError(f"{source_path}: could not find group id='{label_id}'")

    stem = source_path.with_suffix("")
    panel_path = Path(f"{stem}{panel_suffix}.svg")
    labels_path = Path(f"{stem}{labels_suffix}.svg")

    if not overwrite:
        for out in [panel_path, labels_path]:
            if out.exists():
                raise RuntimeError(f"{out} already exists; pass --overwrite")

    # labels-only SVG
    labels_root = copy_svg_shell(root)
    labels_root.attrib["id"] = f"{source_path.stem}-labels"

    copy_defs_and_styles(root, labels_root)

    labels_layer = copy.deepcopy(label_group)
    labels_layer.attrib["id"] = label_id
    labels_root.append(labels_layer)

    if cleanup:
        remove_editor_junk(labels_root)

    labels_tree = ET.ElementTree(labels_root)
    ET.indent(labels_tree, space="  ")
    labels_tree.write(labels_path, encoding="utf-8", xml_declaration=True)

    # panel-only SVG
    panel_root = copy.deepcopy(root)
    panel_parent, panel_label_group = find_parent_and_child_by_id(panel_root, label_id)
    if panel_parent is None or panel_label_group is None:
        raise RuntimeError(f"{source_path}: internal error removing labels")

    panel_parent.remove(panel_label_group)

    if cleanup:
        remove_editor_junk(panel_root)

    panel_tree = ET.ElementTree(panel_root)
    ET.indent(panel_tree, space="  ")
    panel_tree.write(panel_path, encoding="utf-8", xml_declaration=True)

    return panel_path, labels_path


def collect_svg_files(path: Path, recursive: bool) -> list[Path]:
    if path.is_file():
        if path.suffix.lower() != ".svg":
            raise RuntimeError(f"{path} is not an SVG file")
        return [path]

    if path.is_dir():
        pattern = "**/*.svg" if recursive else "*.svg"
        return sorted(path.glob(pattern))

    raise RuntimeError(f"{path} does not exist")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "path",
        type=Path,
        help="SVG file or directory of SVG files",
    )
    parser.add_argument(
        "--label-id",
        default="labels",
        help="SVG group id to extract",
    )
    parser.add_argument(
        "--panel-suffix",
        default=".panel",
        help="Suffix for the labels-removed SVG",
    )
    parser.add_argument(
        "--labels-suffix",
        default=".labels",
        help="Suffix for the labels-only SVG",
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Process SVGs recursively when path is a directory",
    )
    parser.add_argument(
        "--overwrite",
        action="store_true",
        help="Overwrite generated files",
    )
    parser.add_argument(
        "--no-cleanup",
        action="store_true",
        help="Do not remove editor metadata/namedview nodes",
    )

    args = parser.parse_args()

    try:
        svg_files = collect_svg_files(args.path, args.recursive)

        # Avoid reprocessing generated outputs.
        svg_files = [
            p for p in svg_files
            if not p.name.endswith(f"{args.panel_suffix}.svg")
            and not p.name.endswith(f"{args.labels_suffix}.svg")
        ]

        if not svg_files:
            print("No SVG files found.")
            return 0

        for svg_path in svg_files:
            panel_path, labels_path = split_svg(
                source_path=svg_path,
                label_id=args.label_id,
                panel_suffix=args.panel_suffix,
                labels_suffix=args.labels_suffix,
                overwrite=args.overwrite,
                cleanup=not args.no_cleanup,
            )
            print(f"{svg_path}")
            print(f"  -> {panel_path}")
            print(f"  -> {labels_path}")

        return 0

    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
