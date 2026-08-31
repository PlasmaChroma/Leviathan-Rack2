#!/usr/bin/env python3
"""
split_svg_labels.py

Split an SVG into:
  - a panel/art SVG with labels removed
  - a static labels SVG containing the chosen label group
  - when present, a theme-text SVG containing only <g id="theme_text">

By default, the panel/art SVG strips SVG text elements and the labels-only SVG
converts text to paths so runtime panels do not depend on locally installed
fonts. Pass --keep-label-text to keep font-backed text in the labels output.

Expected source convention:
  <g id="labels"> ... </g>
  <g id="labels"> ... <g id="theme_text"> ... </g> ... </g>

Example:
  python3 split_svg_labels.py res/IntegralFlux.svg
  python3 split_svg_labels.py res/IntegralFlux.svg --label-id labels
  python3 split_svg_labels.py res/IntegralFlux.svg --keep-label-text
  python3 split_svg_labels.py res --recursive
"""

from __future__ import annotations

import argparse
import copy
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
import xml.etree.ElementTree as ET


SVG_NS = "http://www.w3.org/2000/svg"
INKSCAPE_NS = "http://www.inkscape.org/namespaces/inkscape"
SODIPODI_NS = "http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
XLINK_NS = "http://www.w3.org/1999/xlink"
XML_NS = "http://www.w3.org/XML/1998/namespace"

THEME_GLASS_IDS = {"glass_input", "glass_output", "glass_text"}
THEME_SUBSTRATE_ATTR = "data-theme-runtime-substrate"
RUNTIME_ANCHOR_GROUP_IDS = {"plasma_conduit_anchors"}

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

    # Preserve root render metadata such as width, height, viewBox,
    # preserveAspectRatio, fill-rule, stroke defaults, and editor metadata.
    # The split label SVG should behave like the original full SVG root unless
    # we deliberately override an attribute below.
    for attr, value in source_root.attrib.items():
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


def strip_font_text_elements(root: ET.Element) -> int:
    """
    Remove SVG elements whose rendering depends on text/font layout.

    This runs on panel outputs and on labels outputs when Inkscape fails to
    convert some font-backed text to paths.
    """
    text_element_names = {
        "text",
        "tspan",
        "textPath",
        "flowRoot",
        "flowRegion",
        "flowPara",
        "flowSpan",
        "flowDiv",
    }

    removed = 0
    for parent in root.iter():
        for child in list(parent):
            if local_name(child.tag) in text_element_names:
                parent.remove(child)
                removed += 1
    return removed


def parse_solid_rgb(value: str) -> tuple[int, int, int] | None:
    """Parse the solid SVG colors needed by the substrate transform."""
    text = value.strip().lower()
    if text.startswith("#"):
        digits = text[1:]
        if len(digits) in {3, 4}:
            try:
                return tuple(int(digits[i] * 2, 16) for i in range(3))
            except ValueError:
                return None
        if len(digits) in {6, 8}:
            try:
                return tuple(int(digits[i:i + 2], 16) for i in (0, 2, 4))
            except ValueError:
                return None

    match = re.fullmatch(
        r"rgb\(\s*(\d{1,3})\s*[, ]\s*(\d{1,3})\s*[, ]\s*(\d{1,3})\s*\)",
        text,
    )
    if match:
        rgb = tuple(int(channel) for channel in match.groups())
        if all(0 <= channel <= 255 for channel in rgb):
            return rgb
    return None


def neutralize_chromatic_fill(value: str) -> str:
    rgb = parse_solid_rgb(value)
    if rgb is None or max(rgb) - min(rgb) <= 2:
        return value
    return "#000000"


def neutralize_style_fill(style: str) -> str:
    declarations: list[str] = []
    for declaration in style.split(";"):
        if ":" not in declaration:
            declarations.append(declaration)
            continue
        name, value = declaration.split(":", 1)
        if name.strip().lower() == "fill":
            value = neutralize_chromatic_fill(value)
        declarations.append(f"{name}:{value}")
    return ";".join(declarations)


def neutralize_semantic_panel_pigment(root: ET.Element) -> int:
    """Convert master-only preview pigment into a neutral runtime substrate."""
    transformed = 0
    for semantic_group in root.iter():
        if get_id(semantic_group) not in THEME_GLASS_IDS:
            continue
        semantic_group.attrib[THEME_SUBSTRATE_ATTR] = "neutral"
        for elem in semantic_group.iter():
            fill = elem.attrib.get("fill")
            if fill is not None:
                neutral = neutralize_chromatic_fill(fill)
                if neutral != fill:
                    elem.attrib["fill"] = neutral
                    transformed += 1
            style = elem.attrib.get("style")
            if style is not None:
                neutral_style = neutralize_style_fill(style)
                if neutral_style != style:
                    elem.attrib["style"] = neutral_style
                    transformed += 1
    return transformed


def set_style_property(style: str, name: str, value: str) -> str:
    declarations: list[str] = []
    replaced = False
    for declaration in style.split(";"):
        if ":" not in declaration:
            if declaration:
                declarations.append(declaration)
            continue
        property_name, property_value = declaration.split(":", 1)
        if property_name.strip().lower() == name.lower():
            property_value = value
            replaced = True
        declarations.append(f"{property_name}:{property_value}")
    if not replaced:
        declarations.append(f"{name}:{value}")
    return ";".join(declarations)


def hide_runtime_anchor_groups(root: ET.Element) -> int:
    """Keep authored geometry available to parsers without drawing its guides."""
    hidden = 0
    for elem in root.iter():
        if local_name(elem.tag) != "g" or get_id(elem) not in RUNTIME_ANCHOR_GROUP_IDS:
            continue
        elem.attrib["style"] = set_style_property(
            elem.attrib.get("style", ""), "display", "none")
        hidden += 1
    return hidden


def normalize_text_for_outline(root: ET.Element) -> None:
    """
    Avoid feeding pretty-printer indentation into Inkscape as real text.

    Inkscape honors xml:space="preserve" on text nodes. If an authored text
    element contains an indented <tspan>, ElementTree writes that indentation
    back out, and Inkscape can include those spaces when converting text to
    paths. That shifts centered labels because the visible word is centered
    along with invisible leading/trailing whitespace.
    """
    text_element_names = {
        "text",
        "tspan",
        "textPath",
        "flowPara",
        "flowSpan",
        "flowDiv",
    }

    for elem in root.iter():
        if local_name(elem.tag) not in text_element_names:
            continue

        elem.attrib.pop(qname(XML_NS, "space"), None)

        if elem.text is not None:
            elem.text = elem.text.strip()

        for child in list(elem):
            if child.tail is not None:
                child.tail = child.tail.strip()


def count_text_elements(root: ET.Element) -> int:
    text_element_names = {
        "text",
        "tspan",
        "textPath",
        "flowRoot",
        "flowRegion",
        "flowPara",
        "flowSpan",
        "flowDiv",
    }
    return sum(1 for elem in root.iter() if local_name(elem.tag) in text_element_names)


def find_inkscape(override: str | None) -> str:
    if override:
        return override

    candidates = [
        shutil.which("inkscape.com"),
        shutil.which("inkscape"),
        shutil.which("inkscape.exe"),
        "/mnt/c/Program Files/Inkscape/bin/inkscape.com",
        "/mnt/c/Program Files/Inkscape/bin/inkscape.exe",
        "/mnt/c/Program Files/Inkscape/inkscape.com",
        "/mnt/c/Program Files/Inkscape/inkscape.exe",
        "/mnt/c/Program Files (x86)/Inkscape/bin/inkscape.com",
        "/mnt/c/Program Files (x86)/Inkscape/bin/inkscape.exe",
        "/mnt/c/Program Files (x86)/Inkscape/inkscape.com",
        "/mnt/c/Program Files (x86)/Inkscape/inkscape.exe",
    ]
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return candidate

    raise RuntimeError(
        "could not find Inkscape; pass --inkscape-path or install inkscape/inkscape.exe"
    )


def path_for_executable(path: Path, executable: str) -> str:
    """
    Windows Inkscape launched from WSL needs Windows-style paths. Native Linux
    Inkscape should receive normal POSIX paths.
    """
    path = path.resolve()
    exe_lower = executable.lower()

    if exe_lower.endswith(".exe") or exe_lower.endswith(".com"):
        path_string = str(path)
        parts = path.parts
        if len(parts) >= 3 and parts[0] == "/" and parts[1] == "mnt" and len(parts[2]) == 1:
            drive = parts[2].upper()
            return drive + ":\\" + "\\".join(parts[3:])

        cygpath = shutil.which("cygpath")
        if cygpath:
            return subprocess.check_output(
                [cygpath, "-w", path_string],
                text=True,
            ).strip()

    return str(path)


def outline_text_with_inkscape(
    svg_path: Path,
    inkscape_path: str | None,
    inkscape_timeout_sec: float,
) -> None:
    executable = find_inkscape(inkscape_path)

    # Write next to the target so Windows Inkscape can access it reliably when
    # this script is called from WSL/MSYS path spaces.
    with tempfile.TemporaryDirectory(
        prefix=f".{svg_path.stem}.outline.",
        dir=str(svg_path.parent),
    ) as tmp_dir:
        tmp_output = Path(tmp_dir) / svg_path.name
        input_arg = path_for_executable(svg_path, executable)
        output_arg = path_for_executable(tmp_output, executable)

        command = [
            executable,
            input_arg,
            "--export-type=svg",
            "--export-plain-svg",
            "--export-text-to-path",
            f"--export-filename={output_arg}",
        ]
        try:
            result = subprocess.run(
                command,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                timeout=inkscape_timeout_sec,
            )
        except subprocess.TimeoutExpired as exc:
            raise RuntimeError(
                "Inkscape text outline timed out after "
                f"{inkscape_timeout_sec:g}s: {' '.join(command)}"
                + (f"\nstdout:\n{exc.stdout}" if exc.stdout else "")
                + (f"\nstderr:\n{exc.stderr}" if exc.stderr else "")
            )
        if result.returncode != 0:
            raise RuntimeError(
                "Inkscape text outline failed"
                + (f"\nstdout:\n{result.stdout}" if result.stdout else "")
                + (f"\nstderr:\n{result.stderr}" if result.stderr else "")
            )

        if not tmp_output.exists():
            raise RuntimeError("Inkscape text outline did not create an output SVG")

        outlined_tree = ET.parse(tmp_output)
        remaining_text = strip_font_text_elements(outlined_tree.getroot())
        if remaining_text:
            ET.indent(outlined_tree, space="  ")
            outlined_tree.write(tmp_output, encoding="utf-8", xml_declaration=True)

        # The output lives beside the target, so replace it atomically instead
        # of reopening a file Windows Inkscape has just read from a WSL mount.
        # Reopening can fail with EINVAL even after the Inkscape process exits.
        os.replace(tmp_output, svg_path)


def split_svg(
    source_path: Path,
    label_id: str,
    panel_suffix: str,
    labels_suffix: str,
    theme_text_id: str,
    theme_text_suffix: str,
    overwrite: bool,
    cleanup: bool,
    strip_panel_text: bool,
    outline_label_text: bool,
    inkscape_path: str | None,
    inkscape_timeout_sec: float,
) -> tuple[Path, Path, Path | None]:
    tree = ET.parse(source_path)
    root = tree.getroot()

    parent, label_group = find_parent_and_child_by_id(root, label_id)

    if parent is None or label_group is None:
        raise RuntimeError(f"{source_path}: could not find group id='{label_id}'")

    stem = source_path.with_suffix("")
    panel_path = Path(f"{stem}{panel_suffix}.svg")
    labels_path = Path(f"{stem}{labels_suffix}.svg")
    theme_text_path = Path(f"{stem}{theme_text_suffix}.svg")

    _, theme_text_group = find_parent_and_child_by_id(label_group, theme_text_id)

    if not overwrite:
        outputs = [panel_path, labels_path]
        if theme_text_group is not None:
            outputs.append(theme_text_path)
        for out in outputs:
            if out.exists():
                raise RuntimeError(f"{out} already exists; pass --overwrite")

    # labels-only SVG
    labels_root = copy_svg_shell(root)
    labels_root.attrib["id"] = f"{source_path.stem}-labels"

    copy_defs_and_styles(root, labels_root)

    labels_layer = copy.deepcopy(label_group)
    labels_layer.attrib["id"] = label_id
    static_theme_parent, static_theme_group = find_parent_and_child_by_id(
        labels_layer, theme_text_id)
    if static_theme_parent is not None and static_theme_group is not None:
        static_theme_parent.remove(static_theme_group)
    labels_root.append(labels_layer)

    if outline_label_text:
        normalize_text_for_outline(labels_root)

    if cleanup:
        remove_editor_junk(labels_root)

    labels_tree = ET.ElementTree(labels_root)
    ET.indent(labels_tree, space="  ")
    # ElementTree's pretty-printer adds indentation back as text/tail content.
    # Do this after indenting as well, otherwise xml:space-aware Inkscape can
    # outline that formatting whitespace and shift centered labels right.
    if outline_label_text:
        normalize_text_for_outline(labels_root)
    labels_tree.write(labels_path, encoding="utf-8", xml_declaration=True)

    if outline_label_text:
        outline_text_with_inkscape(labels_path, inkscape_path, inkscape_timeout_sec)

    generated_theme_text_path: Path | None = None
    if theme_text_group is not None:
        theme_text_root = copy_svg_shell(root)
        theme_text_root.attrib["id"] = f"{source_path.stem}-theme-text"
        copy_defs_and_styles(root, theme_text_root)

        # Preserve transforms and inherited presentation attributes from the
        # labels group while excluding every static sibling.
        theme_labels_layer = ET.Element(label_group.tag, dict(label_group.attrib))
        theme_labels_layer.attrib["id"] = label_id
        theme_labels_layer.append(copy.deepcopy(theme_text_group))
        theme_text_root.append(theme_labels_layer)

        if outline_label_text:
            normalize_text_for_outline(theme_text_root)
        if cleanup:
            remove_editor_junk(theme_text_root)

        theme_text_tree = ET.ElementTree(theme_text_root)
        ET.indent(theme_text_tree, space="  ")
        if outline_label_text:
            normalize_text_for_outline(theme_text_root)
        theme_text_tree.write(theme_text_path, encoding="utf-8", xml_declaration=True)
        if outline_label_text:
            outline_text_with_inkscape(
                theme_text_path, inkscape_path, inkscape_timeout_sec)
        generated_theme_text_path = theme_text_path

    # panel-only SVG
    panel_root = copy.deepcopy(root)
    panel_parent, panel_label_group = find_parent_and_child_by_id(panel_root, label_id)
    if panel_parent is None or panel_label_group is None:
        raise RuntimeError(f"{source_path}: internal error removing labels")

    panel_parent.remove(panel_label_group)

    neutralize_semantic_panel_pigment(panel_root)
    hide_runtime_anchor_groups(panel_root)

    if strip_panel_text:
        strip_font_text_elements(panel_root)

    if cleanup:
        remove_editor_junk(panel_root)

    panel_tree = ET.ElementTree(panel_root)
    ET.indent(panel_tree, space="  ")
    panel_tree.write(panel_path, encoding="utf-8", xml_declaration=True)

    return panel_path, labels_path, generated_theme_text_path


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
        "--theme-text-id",
        default="theme_text",
        help="Optional subgroup within labels to extract as tintable text",
    )
    parser.add_argument(
        "--theme-text-suffix",
        default=".theme-text",
        help="Suffix for the optional tintable text SVG",
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
    parser.add_argument(
        "--keep-panel-text",
        action="store_true",
        help="Do not strip font-backed SVG text elements from the panel output",
    )
    parser.add_argument(
        "--keep-label-text",
        action="store_true",
        help="Keep font-backed text in the labels output instead of converting it to paths",
    )
    parser.add_argument(
        "--inkscape-path",
        default=os.environ.get("INKSCAPE"),
        help="Path to inkscape/inkscape.exe. Defaults to PATH, INKSCAPE, or common Windows install paths.",
    )
    parser.add_argument(
        "--inkscape-timeout",
        type=float,
        default=30.0,
        help="Seconds to wait for Inkscape text outlining before failing.",
    )

    args = parser.parse_args()

    try:
        svg_files = collect_svg_files(args.path, args.recursive)

        # Avoid reprocessing generated outputs.
        svg_files = [
            p for p in svg_files
            if not p.name.endswith(f"{args.panel_suffix}.svg")
            and not p.name.endswith(f"{args.labels_suffix}.svg")
            and not p.name.endswith(f"{args.theme_text_suffix}.svg")
        ]

        if not svg_files:
            print("No SVG files found.")
            return 0

        for svg_path in svg_files:
            panel_path, labels_path, theme_text_path = split_svg(
                source_path=svg_path,
                label_id=args.label_id,
                panel_suffix=args.panel_suffix,
                labels_suffix=args.labels_suffix,
                theme_text_id=args.theme_text_id,
                theme_text_suffix=args.theme_text_suffix,
                overwrite=args.overwrite,
                cleanup=not args.no_cleanup,
                strip_panel_text=not args.keep_panel_text,
                outline_label_text=not args.keep_label_text,
                inkscape_path=args.inkscape_path,
                inkscape_timeout_sec=args.inkscape_timeout,
            )
            print(f"{svg_path}")
            print(f"  -> {panel_path}")
            print(f"  -> {labels_path}")
            if theme_text_path is not None:
                print(f"  -> {theme_text_path}")

        return 0

    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
