#!/usr/bin/env python3
import argparse
import csv
import os
from typing import Callable, Dict, Iterable, List


NUMERIC_COLUMNS = {
    "row",
    "module_id",
    "instance_id",
    "publish_seq",
    "render_mode",
    "row_count",
    "scope_bin_count",
    "stereo",
    "live_mode",
    "msg_changed",
    "range_changed",
    "vertical_changed",
    "full_history_rebuild",
    "visible_shift_rows",
    "rebuild_start",
    "rebuild_end",
    "draw_from_history",
    "row_texture_uploads",
    "field_draws",
    "fallback_renderer",
    "density_pct",
    "rack_zoom",
    "total_us",
    "validate_clear_us",
    "snapshot_us",
    "setup_us",
    "live_ingest_us",
    "geometry_us",
    "gl_draw_us",
    "row_upload_us",
    "field_draw_us",
    "gl_state_us",
    "framebuffer_size_us",
    "viewport_us",
    "resource_validate_us",
    "transparent_clear_us",
    "scissor_setup_us",
    "scoped_clear_us",
}


SUMMARY_COLUMNS = [
    "total_us",
    "validate_clear_us",
    "framebuffer_size_us",
    "viewport_us",
    "resource_validate_us",
    "transparent_clear_us",
    "scissor_setup_us",
    "scoped_clear_us",
    "snapshot_us",
    "setup_us",
    "live_ingest_us",
    "geometry_us",
    "gl_draw_us",
    "row_upload_us",
    "field_draw_us",
    "gl_state_us",
]


WORST_COLUMNS = [
    "row",
    "publish_seq",
    "rack_zoom",
    "total_us",
    "validate_clear_us",
    "framebuffer_size_us",
    "viewport_us",
    "resource_validate_us",
    "transparent_clear_us",
    "scoped_clear_us",
    "setup_us",
    "geometry_us",
    "gl_draw_us",
    "row_upload_us",
    "field_draw_us",
    "gl_state_us",
    "full_history_rebuild",
    "visible_shift_rows",
    "rebuild_start",
    "rebuild_end",
    "row_texture_uploads",
    "field_draws",
    "msg_changed",
]


def load_rows(path: str) -> List[Dict[str, float]]:
    rows: List[Dict[str, float]] = []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        for raw in reader:
            row: Dict[str, float] = {}
            for key, value in raw.items():
                if key in NUMERIC_COLUMNS:
                    try:
                        row[key] = float(value)
                    except (TypeError, ValueError):
                        row[key] = 0.0
            rows.append(row)
    return rows


def percentile(values: Iterable[float], pct: float) -> float:
    ordered = sorted(values)
    if not ordered:
        return 0.0
    index = round((pct / 100.0) * (len(ordered) - 1))
    index = max(0, min(len(ordered) - 1, int(index)))
    return ordered[index]


def fmt_value(key: str, value: float) -> str:
    integerish = {
        "row",
        "module_id",
        "instance_id",
        "publish_seq",
        "render_mode",
        "row_count",
        "scope_bin_count",
        "stereo",
        "live_mode",
        "msg_changed",
        "range_changed",
        "vertical_changed",
        "full_history_rebuild",
        "visible_shift_rows",
        "rebuild_start",
        "rebuild_end",
        "draw_from_history",
        "row_texture_uploads",
        "field_draws",
        "fallback_renderer",
    }
    if key in integerish:
        return str(int(value))
    return f"{value:.1f}"


def summarize_column(rows: List[Dict[str, float]], column: str) -> str:
    values = [row[column] for row in rows if column in row]
    if not values:
        return ""
    return (
        f"{column}: min={min(values):.1f} p50={percentile(values, 50):.1f} "
        f"p90={percentile(values, 90):.1f} p95={percentile(values, 95):.1f} "
        f"p99={percentile(values, 99):.1f} max={max(values):.1f}"
    )


def summarize_group(rows: List[Dict[str, float]], label: str, predicate: Callable[[Dict[str, float]], bool]) -> str:
    group = [row for row in rows if predicate(row)]
    if not group:
        return f"{label}: n=0"
    totals = [row.get("total_us", 0.0) for row in group]
    return (
        f"{label}: n={len(group)} total p50={percentile(totals, 50):.1f} "
        f"p95={percentile(totals, 95):.1f} max={max(totals):.1f}"
    )


def print_worst(rows: List[Dict[str, float]], column: str, count: int) -> None:
    if not rows or column not in rows[0]:
        return
    print(f"\nWorst by {column}:")
    for row in sorted(rows, key=lambda r: r.get(column, 0.0), reverse=True)[:count]:
        parts = []
        for key in WORST_COLUMNS:
            if key in row:
                parts.append(f"{key}={fmt_value(key, row[key])}")
        print(", ".join(parts))


def analyze(path: str, worst_count: int) -> None:
    rows = load_rows(path)
    print(f"\n== {path} ==")
    print(f"rows: {len(rows)}")
    if not rows:
        return

    for column in SUMMARY_COLUMNS:
        line = summarize_column(rows, column)
        if line:
            print(line)

    if "rack_zoom" in rows[0]:
        zooms = [row["rack_zoom"] for row in rows]
        print(f"rack_zoom: min={min(zooms):.3f} max={max(zooms):.3f}")
    if "stereo" in rows[0]:
        print(f"stereo rows: {sum(1 for row in rows if row.get('stereo', 0.0) != 0.0)}")

    groups = [
        ("msg_changed", lambda r: r.get("msg_changed", 0.0) == 1.0),
        ("msg_unchanged", lambda r: r.get("msg_changed", 0.0) == 0.0),
        ("uploads", lambda r: r.get("row_texture_uploads", 0.0) > 0.0),
        ("no_uploads", lambda r: r.get("row_texture_uploads", 0.0) == 0.0),
        ("full_rebuild", lambda r: r.get("full_history_rebuild", 0.0) == 1.0),
        ("validate_clear_gt_180", lambda r: r.get("validate_clear_us", 0.0) > 180.0),
        ("resource_validate_gt_100", lambda r: r.get("resource_validate_us", 0.0) > 100.0),
        ("transparent_clear_gt_100", lambda r: r.get("transparent_clear_us", 0.0) > 100.0),
        ("scoped_clear_gt_100", lambda r: r.get("scoped_clear_us", 0.0) > 100.0),
        ("geometry_gt_100", lambda r: r.get("geometry_us", 0.0) > 100.0),
        ("gl_draw_gt_300", lambda r: r.get("gl_draw_us", 0.0) > 300.0),
        ("row_upload_gt_100", lambda r: r.get("row_upload_us", 0.0) > 100.0),
        ("field_draw_gt_250", lambda r: r.get("field_draw_us", 0.0) > 250.0),
        ("gl_state_gt_100", lambda r: r.get("gl_state_us", 0.0) > 100.0),
    ]
    print("\nGroups:")
    for label, predicate in groups:
        print(summarize_group(rows, label, predicate))

    print_worst(rows, "total_us", worst_count)
    for column in [
        "validate_clear_us",
        "resource_validate_us",
        "transparent_clear_us",
        "scoped_clear_us",
        "gl_draw_us",
        "field_draw_us",
        "gl_state_us",
    ]:
        if column in rows[0]:
            print_worst(rows, column, min(8, worst_count))


def main() -> int:
    parser = argparse.ArgumentParser(description="Summarize TD.Scope draw CSV logs.")
    parser.add_argument("paths", nargs="+", help="CSV log path(s)")
    parser.add_argument("--worst", type=int, default=15, help="Number of worst rows to print")
    args = parser.parse_args()

    for path in args.paths:
        if not os.path.exists(path):
            raise SystemExit(f"Missing file: {path}")
        analyze(path, args.worst)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
