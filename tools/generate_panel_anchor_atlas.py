#!/usr/bin/env python3
"""Generate static panel SVG anchor metadata for PanelSvgUtils.

The atlas is an optimization only. Runtime lookup validates each SVG's size/hash
before using generated records; stale or missing records fall back to SVG parsing.
"""

from __future__ import annotations

import argparse
import pathlib
import re
from dataclasses import dataclass

FNV64_OFFSET = 14695981039346656037
FNV64_PRIME = 1099511628211

TAG_RE = re.compile(r"<(rect|circle|ellipse)\b[^>]*\bid\s*=\s*\"([^\"]+)\"[^>]*>", re.IGNORECASE)
ATTR_RE = re.compile(r"\b([A-Za-z_:][-A-Za-z0-9_:.]*)\s*=\s*\"([^\"]+)\"")
SVG_TAG_RE = re.compile(r"<svg\b[^>]*>", re.IGNORECASE)


def fnv1a64(data: bytes) -> int:
    h = FNV64_OFFSET
    for b in data:
        h ^= b
        h = (h * FNV64_PRIME) & 0xFFFFFFFFFFFFFFFF
    return h


def parse_float(text: str) -> float | None:
    try:
        return float(text)
    except ValueError:
        return None


@dataclass(frozen=True)
class SvgRecord:
    path: str
    size: int
    fnv64: int
    unit_to_mm_scale: float


@dataclass(frozen=True)
class AnchorRecord:
    svg_index: int
    anchor_id: str
    flags: int
    cx: float
    cy: float
    x: float
    y: float
    width: float
    height: float
    radius: float


FLAG_CENTER = 1 << 0
FLAG_RECT = 1 << 1
FLAG_RADIUS = 1 << 2


def cpp_string(s: str) -> str:
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'


def cpp_float(v: float) -> str:
    if v == 0.0:
        return "0.f"
    text = f"{v:.9g}"
    if "e" not in text.lower() and "." not in text:
        text += ".f"
    else:
        text += "f"
    return text


def parse_svg_user_unit_to_mm_scale(text: str) -> float:
    match = SVG_TAG_RE.search(text)
    if not match:
        return 0.01
    attrs = {m.group(1).lower(): m.group(2) for m in ATTR_RE.finditer(match.group(0))}
    view_box_text = attrs.get("viewbox", "")
    width_text = attrs.get("width", "")
    try:
        view_box = [float(value) for value in re.split(r"[\s,]+", view_box_text.strip())]
    except ValueError:
        return 0.01
    width_match = re.fullmatch(
        r"\s*([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\s*mm\s*",
        width_text,
        re.IGNORECASE,
    )
    if len(view_box) < 4 or view_box[2] <= 0.0 or not width_match:
        return 0.01
    width_mm = float(width_match.group(1))
    return width_mm / view_box[2] if width_mm > 0.0 else 0.01


def scan_svg(path: pathlib.Path, rel_path: str, svg_index: int) -> tuple[SvgRecord, list[AnchorRecord]]:
    data = path.read_bytes()
    text = data.decode("utf-8", errors="ignore")
    svg_record = SvgRecord(
        rel_path,
        len(data),
        fnv1a64(data),
        parse_svg_user_unit_to_mm_scale(text),
    )
    anchors: list[AnchorRecord] = []

    for match in TAG_RE.finditer(text):
        tag_name = match.group(1).lower()
        anchor_id = match.group(2)
        attrs = {m.group(1).lower(): m.group(2) for m in ATTR_RE.finditer(match.group(0))}
        flags = 0
        cx = cy = x = y = width = height = radius = 0.0

        if tag_name in ("circle", "ellipse"):
            parsed_cx = parse_float(attrs.get("cx", ""))
            parsed_cy = parse_float(attrs.get("cy", ""))
            if parsed_cx is not None and parsed_cy is not None:
                flags |= FLAG_CENTER
                cx = parsed_cx
                cy = parsed_cy
            if tag_name == "circle":
                parsed_r = parse_float(attrs.get("r", ""))
                if parsed_r is not None:
                    flags |= FLAG_RADIUS
                    radius = parsed_r
        elif tag_name == "rect":
            parsed_x = parse_float(attrs.get("x", ""))
            parsed_y = parse_float(attrs.get("y", ""))
            parsed_w = parse_float(attrs.get("width", ""))
            parsed_h = parse_float(attrs.get("height", ""))
            if None not in (parsed_x, parsed_y, parsed_w, parsed_h):
                flags |= FLAG_RECT
                x = float(parsed_x)
                y = float(parsed_y)
                width = float(parsed_w)
                height = float(parsed_h)
                flags |= FLAG_CENTER
                cx = x + 0.5 * width
                cy = y + 0.5 * height

        if flags:
            anchors.append(AnchorRecord(svg_index, anchor_id, flags, cx, cy, x, y, width, height, radius))

    return svg_record, anchors


def generate(repo_root: pathlib.Path, output: pathlib.Path) -> None:
    res_dir = repo_root / "res"
    # Keep generated indices stable across case-sensitive and case-insensitive
    # filesystems (the resource tree contains mixed-case module names).
    svg_paths = sorted(
        (p for p in res_dir.glob("*.svg") if p.is_file()),
        key=lambda p: p.name.lower(),
    )
    svg_records: list[SvgRecord] = []
    anchors: list[AnchorRecord] = []

    for path in svg_paths:
        rel = path.relative_to(repo_root).as_posix()
        svg_record, svg_anchors = scan_svg(path, rel, len(svg_records))
        svg_records.append(svg_record)
        anchors.extend(svg_anchors)

    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w", encoding="utf-8", newline="\n") as f:
        f.write("// Generated by tools/generate_panel_anchor_atlas.py. Do not edit by hand.\n")
        f.write("#include \"PanelAnchorAtlas.hpp\"\n\n")
        f.write("#include <algorithm>\n")
        f.write("#include <cstdint>\n")
        f.write("#include <fstream>\n")
        f.write("#include <limits>\n")
        f.write("#include <sstream>\n")
        f.write("#include <string>\n\n")
        f.write("#include <unordered_map>\n")
        f.write("#include <vector>\n\n")
        f.write("namespace panel_svg {\nnamespace {\n\n")
        f.write("constexpr uint32_t kAnchorHasCenter = 1u << 0;\n")
        f.write("constexpr uint32_t kAnchorHasRect = 1u << 1;\n")
        f.write("constexpr uint32_t kAnchorHasRadius = 1u << 2;\n")
        f.write("constexpr uint64_t kFnv64Offset = 14695981039346656037ull;\n")
        f.write("constexpr uint64_t kFnv64Prime = 1099511628211ull;\n\n")
        f.write("struct SvgAtlasRecord { const char* path; uint64_t size; uint64_t fnv64; float unitToMmScale; };\n")
        f.write("struct AnchorAtlasRecord { uint16_t svgIndex; const char* id; uint32_t flags; float cx; float cy; float x; float y; float width; float height; float radius; };\n\n")
        f.write("static const SvgAtlasRecord kSvgAtlas[] = {\n")
        for rec in svg_records:
            f.write(
                f"\t{{{cpp_string(rec.path)}, {rec.size}ull, "
                f"0x{rec.fnv64:016x}ull, {cpp_float(rec.unit_to_mm_scale)}}},\n"
            )
        f.write("};\n\n")
        f.write("static const AnchorAtlasRecord kAnchorAtlas[] = {\n")
        for rec in anchors:
            f.write(
                "\t{" +
                f"{rec.svg_index}, {cpp_string(rec.anchor_id)}, {rec.flags}u, " +
                f"{cpp_float(rec.cx)}, {cpp_float(rec.cy)}, {cpp_float(rec.x)}, {cpp_float(rec.y)}, " +
                f"{cpp_float(rec.width)}, {cpp_float(rec.height)}, {cpp_float(rec.radius)}" +
                "},\n"
            )
        f.write("};\n\n")
        f.write(R'''std::string normalizePath(std::string path) {
	std::replace(path.begin(), path.end(), '\\', '/');
	return path;
}

bool pathMatches(const std::string& requested, const char* atlasPath) {
	const std::string normalized = normalizePath(requested);
	const std::string atlas = normalizePath(atlasPath ? atlasPath : "");
	if (normalized == atlas) {
		return true;
	}
	if (normalized.size() <= atlas.size()) {
		return false;
	}
	const size_t offset = normalized.size() - atlas.size();
	return normalized[offset - 1u] == '/' && normalized.compare(offset, atlas.size(), atlas) == 0;
}

bool readFileBytes(const std::string& path, std::string* out) {
	if (!out) {
		return false;
	}
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.good()) {
		return false;
	}
	std::ostringstream buffer;
	buffer << file.rdbuf();
	*out = buffer.str();
	return true;
}

uint64_t fnv1a64(const std::string& bytes) {
	uint64_t h = kFnv64Offset;
	for (unsigned char c : bytes) {
		h ^= uint64_t(c);
		h *= kFnv64Prime;
	}
	return h;
}

int findSvgIndex(const std::string& svgPath) {
	for (size_t i = 0; i < sizeof(kSvgAtlas) / sizeof(kSvgAtlas[0]); ++i) {
		if (pathMatches(svgPath, kSvgAtlas[i].path)) {
			return int(i);
		}
	}
	return -1;
}

bool atlasRecordIsCurrent(const std::string& svgPath, int svgIndex) {
	if (svgIndex < 0 || size_t(svgIndex) >= sizeof(kSvgAtlas) / sizeof(kSvgAtlas[0])) {
		return false;
	}
	static int8_t validation[sizeof(kSvgAtlas) / sizeof(kSvgAtlas[0])] = {};
	int8_t& cached = validation[size_t(svgIndex)];
	if (cached != 0) {
		return cached > 0;
	}

	std::string bytes;
	if (!readFileBytes(svgPath, &bytes)) {
		cached = -1;
		return false;
	}
	const SvgAtlasRecord& rec = kSvgAtlas[size_t(svgIndex)];
	const bool valid = bytes.size() == rec.size && fnv1a64(bytes) == rec.fnv64;
	cached = valid ? 1 : -1;
	return valid;
}

const std::vector<std::unordered_map<std::string, const AnchorAtlasRecord*>>& anchorIndexBySvg() {
	static std::vector<std::unordered_map<std::string, const AnchorAtlasRecord*>> index;
	if (!index.empty()) {
		return index;
	}
	index.resize(sizeof(kSvgAtlas) / sizeof(kSvgAtlas[0]));
	for (const AnchorAtlasRecord& rec : kAnchorAtlas) {
		const size_t svgIndex = size_t(rec.svgIndex);
		if (svgIndex >= index.size()) {
			continue;
		}
		index[svgIndex].emplace(rec.id ? rec.id : "", &rec);
	}
	return index;
}

} // namespace

bool lookupPanelAnchor(const std::string& svgPath, const std::string& elementId, PanelAnchorLookupResult* out) {
	if (!out) {
		return false;
	}
	*out = PanelAnchorLookupResult{};
	const int svgIndex = findSvgIndex(svgPath);
	if (svgIndex < 0 || !atlasRecordIsCurrent(svgPath, svgIndex)) {
		return false;
	}
	const auto& index = anchorIndexBySvg();
	const size_t indexPos = size_t(svgIndex);
	if (indexPos >= index.size()) {
		return false;
	}
	const auto& byId = index[indexPos];
	const auto found = byId.find(elementId);
	if (found == byId.end() || !found->second) {
		return false;
	}
	const AnchorAtlasRecord& rec = *found->second;
	out->found = true;
	out->hasCenter = (rec.flags & kAnchorHasCenter) != 0u;
	out->hasRect = (rec.flags & kAnchorHasRect) != 0u;
	out->hasRadius = (rec.flags & kAnchorHasRadius) != 0u;
	out->unitToMmScale = kSvgAtlas[indexPos].unitToMmScale;
	out->cx = rec.cx;
	out->cy = rec.cy;
	out->x = rec.x;
	out->y = rec.y;
	out->width = rec.width;
	out->height = rec.height;
	out->radius = rec.radius;
	return true;
}

PanelAnchorAtlasStatus getPanelAnchorAtlasStatus(const std::string& svgPath) {
	const int svgIndex = findSvgIndex(svgPath);
	if (svgIndex < 0) {
		return PanelAnchorAtlasStatus::Missing;
	}
	return atlasRecordIsCurrent(svgPath, svgIndex)
		? PanelAnchorAtlasStatus::Valid
		: PanelAnchorAtlasStatus::StaleOrUnreadable;
}

} // namespace panel_svg
''')

    print(f"Wrote {output} with {len(svg_records)} SVG records and {len(anchors)} anchors")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo-root", default=".")
    parser.add_argument("--output", default="src/PanelAnchorAtlas.cpp")
    args = parser.parse_args()
    repo_root = pathlib.Path(args.repo_root).resolve()
    output = (repo_root / args.output).resolve()
    generate(repo_root, output)


if __name__ == "__main__":
    main()
