#!/usr/bin/env python3
"""Summarize Integral Flux CPU draw logs without inferring GPU or frame time."""

import argparse
import csv
import hashlib
import io
import json
import math
from pathlib import Path


SETTINGS = (
    "preview_render_mode", "preview_tracer_enabled", "preview_tracer_mode",
    "halo_nanovg_forced", "lumen_preview_adapter",
)
COUNTERS = (
    "preview_dirty_requests", "preview_point_rebuilds", "preview_tracer_captures",
    "preview_tracer_accepted_captures", "history_rasterizations",
    "ch1_history_rasterizations", "ch4_history_rasterizations",
    "halo_gl_surface_framebuffer_draws", "halo_nanovg_surface_draws",
    "halo_center_framebuffer_draws", "halo_cap_reflection_framebuffer_draws",
)
GAUGES = (
    "history_trails", "ch1_history_trails", "ch4_history_trails",
    "history_submitted_points", "halo_dirty_draw_count", "halo_active_draw_count",
    "halo_dragging_draw_count",
)


def percentile(values, fraction):
    """Linear interpolation between sorted observations, including endpoints."""
    ordered = sorted(values)
    position = (len(ordered) - 1) * fraction
    lower = math.floor(position)
    upper = math.ceil(position)
    return ordered[lower] + (ordered[upper] - ordered[lower]) * (position - lower)


def distribution(values):
    return {
        "samples": len(values),
        "p50": percentile(values, .5),
        "p95": percentile(values, .95),
        "p99": percentile(values, .99),
        "max": max(values),
    }


def summarize(path, start_row=0, end_row=None):
    raw = Path(path).read_bytes()
    reader = csv.DictReader(io.StringIO(raw.decode("utf-8-sig")))
    fields = reader.fieldnames or []
    required = {"row", "module_id", "instance_id", "module_widget_draw_us", "preview_draw_us"}
    if not required.issubset(fields) or len(fields) != len(set(fields)):
        raise ValueError("missing required Flux columns or duplicate column names")
    rows = []
    previous = -1
    identities = set()
    for line, row in enumerate(reader, 2):
        if None in row or any(value is None or not value.strip() for value in row.values()):
            raise ValueError(f"line {line}: incomplete/extra CSV fields; stop logging before analysis")
        try:
            number = int(row["row"])
        except ValueError as error:
            raise ValueError(f"line {line}: invalid row number") from error
        if number <= previous:
            raise ValueError(f"line {line}: row numbers must strictly increase")
        previous = number
        identities.add((row["module_id"], row["instance_id"]))
        if number < start_row or (end_row is not None and number >= end_row):
            continue
        numeric = {}
        for field, value in row.items():
            if field in ("module_id", "instance_id"):
                continue  # Preserve integer identities without floating-point rounding.
            try:
                numeric[field] = float(value)
            except ValueError as error:
                raise ValueError(f"line {line}: nonnumeric {field}") from error
            if not math.isfinite(numeric[field]) or numeric[field] < 0:
                raise ValueError(f"line {line}: non-finite or negative {field}")
            if field in COUNTERS and not numeric[field].is_integer():
                raise ValueError(f"line {line}: fractional counter {field}")
        rows.append(numeric)
    if len(identities) != 1:
        raise ValueError("expected one module instance per file; do not concatenate captures")
    if not rows:
        raise ValueError("no rows in selected interval")
    warnings = [
        "CPU submission observations only; host frame time, GPU time, source age, and capture duration are unavailable.",
        "Timing scopes are nested; do not add component percentiles or EMA timings.",
        "CSV writing/flush overhead is not fully represented by draw timers; compare equally instrumented runs.",
    ]
    settings = {field: sorted({row[field] for row in rows}) for field in SETTINGS if field in fields}
    if any(len(values) > 1 for values in settings.values()):
        warnings.append("Renderer/settings changed inside this interval; select homogeneous intervals before comparing.")
    if any(b["row"] != a["row"] + 1 for a, b in zip(rows, rows[1:])):
        warnings.append("Selected row numbers contain gaps.")
    missing = [field for field in COUNTERS + GAUGES + SETTINGS if field not in fields]
    if missing:
        warnings.append("Missing columns are unavailable, not zero; older logs may also have different attribution.")
    if "preview_tracer_accepted_captures" in fields:
        if "preview_tracer_captures" in fields and any(
            row["preview_tracer_accepted_captures"] > row["preview_tracer_captures"] for row in rows
        ):
            warnings.append("Accepted captures exceed attempts in at least one row; investigate telemetry attribution.")
    return {
        "schema_version": 1,
        "source": str(Path(path).resolve()),
        "sha256": hashlib.sha256(raw).hexdigest(),
        "module_id": next(iter(identities))[0],
        "instance_id": next(iter(identities))[1],
        "selection": {"start_row_inclusive": start_row, "end_row_exclusive": end_row,
                      "first_row": int(rows[0]["row"]), "last_row": int(rows[-1]["row"]), "rows": len(rows)},
        "settings": settings,
        "cpu_timings_us": {field: distribution([row[field] for row in rows])
                           for field in fields if field.endswith("_us") and "_ema_" not in field},
        "smoothed_timings_us": {field: distribution([row[field] for row in rows])
                                for field in fields if field.endswith("_ema_us")},
        "counters": {field: {"total": int(sum(row[field] for row in rows)),
                              "rows_with_work": sum(row[field] > 0 for row in rows)}
                     for field in COUNTERS if field in fields},
        "gauges": {field: distribution([row[field] for row in rows]) for field in GAUGES if field in fields},
        "missing_columns": missing,
        "warnings": warnings,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", type=Path)
    parser.add_argument("--start-row", type=int, default=0, help="inclusive CSV row ID; no implicit warm-up removal")
    parser.add_argument("--end-row", type=int, help="exclusive CSV row ID")
    parser.add_argument("--label", required=True, help="e.g. A-static-run1 or B-modulated-run2")
    args = parser.parse_args()
    if args.start_row < 0 or (args.end_row is not None and args.end_row <= args.start_row):
        parser.error("require 0 <= start-row < end-row")
    try:
        result = summarize(args.csv, args.start_row, args.end_row)
    except (OSError, UnicodeError, ValueError, csv.Error) as error:
        parser.error(str(error))
    result["label"] = args.label
    print(json.dumps(result, indent=2, allow_nan=False))


if __name__ == "__main__":
    main()
