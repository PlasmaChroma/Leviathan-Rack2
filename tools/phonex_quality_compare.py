#!/usr/bin/env python3
"""Compare two immutable PHONEX quality-audit tags."""

from __future__ import annotations

import argparse
import csv
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_AUDITS = ROOT / "build" / "phonex-quality"
NUMERIC_FIELDS = ("peak_dbfs", "rms_dbfs", "dc", "max_step", "p999_step",
                  "centroid_hz", "band_0_1k", "band_1_3k", "band_3_6k",
                  "band_6k_nyquist")


def read_metrics(directory: Path) -> dict[str, dict[str, str]]:
    with (directory / "metrics.tsv").open(encoding="utf-8", newline="") as handle:
        rows = list(csv.DictReader(handle, delimiter="\t"))
    return {row["key"]: row for row in rows}


def read_summary(directory: Path) -> dict:
    return json.loads((directory / "summary.json").read_text(encoding="utf-8"))


def value(rows: dict[str, dict[str, str]], key: str, field: str) -> float:
    return float(rows[key][field])


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline")
    parser.add_argument("candidate")
    parser.add_argument("--audit-root", type=Path, default=DEFAULT_AUDITS)
    args = parser.parse_args()
    for tag in (args.baseline, args.candidate):
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]*", tag):
            parser.error(f"invalid tag: {tag}")
    audit_root = args.audit_root.resolve()
    baseline_dir = audit_root / args.baseline
    candidate_dir = audit_root / args.candidate
    baseline = read_metrics(baseline_dir)
    candidate = read_metrics(candidate_dir)
    if baseline.keys() != candidate.keys():
        missing = sorted(baseline.keys() - candidate.keys())
        added = sorted(candidate.keys() - baseline.keys())
        raise RuntimeError(f"render key mismatch; missing={missing}, added={added}")

    rows: list[dict[str, str]] = []
    for key in sorted(baseline):
        row = {"key": key, "group": candidate[key]["group"],
               "audio_changed": str(baseline[key]["sha256"] != candidate[key]["sha256"]).lower()}
        for field in NUMERIC_FIELDS:
            row[f"baseline_{field}"] = baseline[key][field]
            row[f"candidate_{field}"] = candidate[key][field]
            row[f"delta_{field}"] = format(
                float(candidate[key][field]) - float(baseline[key][field]), ".9g")
        rows.append(row)

    comparisons = audit_root / "comparisons"
    comparisons.mkdir(parents=True, exist_ok=True)
    stem = f"{args.baseline}--{args.candidate}"
    columns = ["key", "group", "audio_changed"]
    for field in NUMERIC_FIELDS:
        columns.extend((f"baseline_{field}", f"candidate_{field}", f"delta_{field}"))
    with (comparisons / f"{stem}.tsv").open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=columns, delimiter="\t", lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)

    candidate_summary = read_summary(candidate_dir)
    all_unclipped = all(int(row["clipped_samples"]) == 0 for row in candidate.values())
    all_bounded = all(float(row["peak"]) <= 1.0 for row in candidate.values())
    forced_rms = value(candidate, "probe-forced-voiced-unvoiced-filtered", "rms_dbfs")
    s_centroid = value(candidate, "isolated-s", "centroid_hz")
    sh_centroid = value(candidate, "isolated-sh", "centroid_hz")
    s_high = value(candidate, "isolated-s", "band_3_6k")
    sh_high = value(candidate, "isolated-sh", "band_3_6k")
    gates = {
        "determinism": bool(candidate_summary.get("determinism_verified")),
        "finite_bounded_pcm": all_bounded,
        "no_pcm_clipping": all_unclipped,
        "forced_voiced_unvoiced_is_audible": forced_rms > -80.0,
        "q1_s_centroid_above_sh": s_centroid > sh_centroid,
        "q1_s_3_6k_energy_above_sh": s_high > sh_high,
    }
    report = {
        "schema": 1,
        "baseline": args.baseline,
        "candidate": args.candidate,
        "render_count": len(rows),
        "changed_audio_count": sum(row["audio_changed"] == "true" for row in rows),
        "max_abs_peak_db_delta": max(abs(float(row["delta_peak_dbfs"])) for row in rows),
        "max_abs_rms_db_delta": max(abs(float(row["delta_rms_dbfs"])) for row in rows),
        "max_abs_centroid_delta_hz": max(abs(float(row["delta_centroid_hz"])) for row in rows),
        "measurements": {
            "forced_voiced_probe_rms_dbfs": forced_rms,
            "isolated_s_centroid_hz": s_centroid,
            "isolated_sh_centroid_hz": sh_centroid,
            "isolated_s_band_3_6k": s_high,
            "isolated_sh_band_3_6k": sh_high,
        },
        "gates": gates,
    }
    report_path = comparisons / f"{stem}.json"
    report_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2, sort_keys=True))
    print(f"Comparison written to {report_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
