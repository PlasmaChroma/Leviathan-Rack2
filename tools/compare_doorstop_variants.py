#!/usr/bin/env python3
"""Compare Doorstop analysis variants and package matched listening fixtures."""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path

import numpy as np
import soundfile as sf

import audit_doorstop_corpus as corpus
import analyze_doorstop_reference as reference_analysis


CRITICAL_MATERIAL_FEATURES = (
    "spectral_centroid_hz",
    "body_180_500",
    "metal_500_1200",
    "upper_1200_3000",
    "air_3000_10000",
    "lobe_count",
    "pulse_rate_hz",
    "boing_modulation_depth",
    "t20_seconds",
    "ridge_count",
    "ridge_median_q",
    "spectral_flatness",
)
BALANCE_FEATURES = (
    "spectral_centroid_hz",
    "low_body_70_180",
    "body_180_500",
    "metal_500_1200",
    "upper_1200_3000",
    "air_3000_10000",
)
LISTENING_VELOCITY = 0.75
LISTENING_TARGET_RMS = 10.0 ** (-18.0 / 20.0)


def screening_score(score: dict) -> float:
    """A coarse sorter, deliberately not an automatic listening verdict."""
    aggregate = score["aggregate"]
    median_term = math.exp(
        -aggregate["mean_absolute_median_bias_corridors"]
    )
    return (
        0.30 * aggregate["mean_containment"]
        + 0.25 * aggregate["mean_coverage"]
        + 0.25 * median_term
        + 0.20 * (1.0 - aggregate["mean_expanded_outlier_rate"])
    )


def feature_distance(
    row: corpus.AuditedHit, ranges: dict[str, dict[str, float]]
) -> float:
    total = 0.0
    for field in BALANCE_FEATURES:
        target = ranges[field]
        span = max(target["q90"] - target["q10"], abs(target["median"]) * 0.1, 1e-9)
        normalized = (float(getattr(row, field)) - target["median"]) / span
        total += normalized * normalized
    return math.sqrt(total)


def select_common_fixtures(
    rows: list[corpus.AuditedHit],
    ranges: dict[str, dict[str, float]],
) -> list[dict]:
    choices = [
        row
        for row in rows
        if row.velocity is not None
        and abs(row.velocity - LISTENING_VELOCITY) < 1e-6
        and row.seed is not None
    ]
    if not choices:
        choices = [row for row in rows if row.seed is not None]
    if not choices:
        return []
    dark = min(choices, key=lambda row: row.spectral_centroid_hz)
    bright = max(choices, key=lambda row: row.spectral_centroid_hz)
    balanced = min(choices, key=lambda row: feature_distance(row, ranges))
    return [
        {
            "role": "dark",
            "seed": dark.seed,
            "velocity": dark.velocity,
            "selection_centroid_hz": dark.spectral_centroid_hz,
        },
        {
            "role": "balanced",
            "seed": balanced.seed,
            "velocity": balanced.velocity,
            "selection_centroid_hz": balanced.spectral_centroid_hz,
        },
        {
            "role": "bright",
            "seed": bright.seed,
            "velocity": bright.velocity,
            "selection_centroid_hz": bright.spectral_centroid_hz,
        },
    ]


def load_listening_audio(
    path: Path,
) -> tuple[np.ndarray, int, float, float]:
    audio, rate = reference_analysis.read_audio(path)
    audio = np.asarray(audio, dtype=np.float64)
    audio -= np.mean(audio)
    start = min(len(audio), int(0.050 * rate))
    end = min(len(audio), int(2.500 * rate))
    region = audio[start:end] if end > start else audio
    measured_rms = corpus.rms(region)
    peak = float(np.max(np.abs(audio))) if len(audio) else 0.0
    return audio, rate, measured_rms, peak


def package_listening_set(
    output_dir: Path,
    root: Path,
    variants: list[str],
    rows_by_variant: dict[str, list[corpus.AuditedHit]],
    fixtures: list[dict],
) -> list[dict]:
    listening_dir = output_dir / "listening"
    listening_dir.mkdir(parents=True, exist_ok=True)
    # This directory is a generated artifact. Remove only files matching this
    # tool's own fixture naming convention so obsolete rank numbers or removed
    # variants cannot remain in an audition after a new comparison.
    for stale_fixture in listening_dir.glob("[0-9][0-9]-*__*.wav"):
        stale_fixture.unlink()
    packaged: list[dict] = []
    playlist: list[str] = []
    for role_index, fixture in enumerate(fixtures, start=1):
        seed = fixture["seed"]
        velocity = fixture["velocity"]
        group: list[dict] = []
        for variant_index, variant in enumerate(variants, start=1):
            matching = [
                row
                for row in rows_by_variant[variant]
                if row.seed == seed
                and row.velocity is not None
                and velocity is not None
                and abs(row.velocity - velocity) < 1e-6
            ]
            if not matching:
                continue
            source = root / variant / matching[0].source
            audio, rate, measured_rms, peak = load_listening_audio(source)
            group.append(
                {
                    "variant_index": variant_index,
                    "variant": variant,
                    "source": source,
                    "audio": audio,
                    "rate": rate,
                    "measured_rms": measured_rms,
                    "peak": peak,
                }
            )
        # Use one group target rather than independently hitting the peak
        # ceiling. This preserves exact loudness matching even when a sparse
        # diagnostic stem has a much larger crest factor than the full mix.
        safe_targets = [
            0.95 * item["measured_rms"] / max(item["peak"], 1e-12)
            for item in group
        ]
        group_target_rms = min([LISTENING_TARGET_RMS, *safe_targets])
        for item in group:
            variant_index = item["variant_index"]
            variant = item["variant"]
            source = item["source"]
            gain = group_target_rms / max(item["measured_rms"], 1e-12)
            audio = (item["audio"] * gain).astype(np.float32)
            rate = item["rate"]
            output_name = (
                f"{role_index:02d}-{fixture['role']}__"
                f"{variant_index:02d}-{variant}.wav"
            )
            sf.write(
                listening_dir / output_name,
                audio,
                rate,
                subtype="FLOAT",
            )
            playlist.append(output_name)
            packaged.append(
                {
                    "role": fixture["role"],
                    "variant": variant,
                    "seed": seed,
                    "velocity": velocity,
                    "source": str(source),
                    "output": str(listening_dir / output_name),
                    "linear_gain": gain,
                    "group_target_rms": group_target_rms,
                }
            )
    (listening_dir / "playlist.m3u").write_text(
        "#EXTM3U\n" + "\n".join(playlist) + "\n",
        encoding="utf-8",
    )
    lines = [
        "# Doorstop matched listening set",
        "",
        "Each group uses the same specimen seed and strike velocity across every",
        "variant. Files are DC-corrected and exactly RMS-matched within each group",
        "over 50 ms–2.5 s. The common level is lowered as needed to keep every",
        "peak at or below 0.95. Listen for material and motion rather than loudness.",
        "",
        "| Group | Variant | Seed | Velocity | File |",
        "|---|---|---:|---:|---|",
    ]
    for item in packaged:
        filename = Path(item["output"]).name
        lines.append(
            f"| {item['role']} | {item['variant']} | {item['seed']} | "
            f"{item['velocity']:.2f} | [{filename}]({filename}) |"
        )
    lines.extend(
        [
            "",
            "For refinement listening, compare current, spring-forward, then",
            "spring-refined. Spring-only and modes-only remain diagnostic stems.",
            "The refined candidate deliberately protects the repeated boing",
            "contrast even when that is not favored by averaged spectrum alone.",
            "",
        ]
    )
    (listening_dir / "README.md").write_text(
        "\n".join(lines), encoding="utf-8"
    )
    return packaged


def write_report(
    path: Path,
    ordered_variants: list[str],
    scores: dict[str, dict],
    fixtures: list[dict],
) -> None:
    lines = [
        "# Doorstop variant screening",
        "",
        "This report ranks objective corpus proximity. It does not decide which",
        "candidate has the most credible material; use the matched listening set",
        "for that decision.",
        "",
        "## Population summary",
        "",
        "| Variant | Screen | Containment | Coverage | Median bias | Outliers |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for variant in ordered_variants:
        score = scores[variant]
        aggregate = score["aggregate"]
        lines.append(
            f"| {variant} | {score['screening_score']:.3f} | "
            f"{aggregate['mean_containment']:.1%} | "
            f"{aggregate['mean_coverage']:.1%} | "
            f"{aggregate['mean_absolute_median_bias_corridors']:.3f} | "
            f"{aggregate['mean_expanded_outlier_rate']:.1%} |"
        )
    lines.extend(
        [
            "",
            "## Material-defining features",
            "",
            "| Feature | Real P10–P90 | "
            + " | ".join(ordered_variants)
            + " |",
            "|---|---:|" + "---:|" * len(ordered_variants),
        ]
    )
    for field in CRITICAL_MATERIAL_FEATURES:
        first = scores[ordered_variants[0]]["features"][field]
        cells = [
            f"{first['reference_q10']:.4g}–{first['reference_q90']:.4g}"
        ]
        for variant in ordered_variants:
            feature = scores[variant]["features"][field]
            cells.append(
                f"{feature['model_q10']:.4g}–{feature['model_q90']:.4g}"
            )
        lines.append(f"| {field} | " + " | ".join(cells) + " |")
    lines.extend(
        [
            "",
            "## Matched fixture seeds",
            "",
            "| Role | Seed | Velocity | Current centroid Hz |",
            "|---|---:|---:|---:|",
        ]
    )
    for fixture in fixtures:
        lines.append(
            f"| {fixture['role']} | {fixture['seed']} | "
            f"{fixture['velocity']:.2f} | "
            f"{fixture['selection_centroid_hz']:.1f} |"
        )
    lines.extend(
        [
            "",
            "The screening score is only a sorter: 30% containment, 25% coverage,",
            "25% median proximity, and 20% expanded-range outlier control. A",
            "higher score cannot establish that a synthesized object sounds like",
            "ordinary spring steel.",
            "",
        ]
    )
    path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--manifest",
        type=Path,
        default=Path("tools/doorstop_reference_manifest.json"),
    )
    parser.add_argument(
        "--variant-root",
        type=Path,
        default=Path("build/doorstop-variant-renders"),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("build/doorstop-variant-analysis"),
    )
    args = parser.parse_args()

    variant_dirs = sorted(
        path for path in args.variant_root.iterdir() if path.is_dir()
    )
    if not variant_dirs:
        parser.error(f"no variant directories found in {args.variant_root}")

    reference_rows, _ = corpus.load_reference_hits(args.manifest)
    fields = corpus.numeric_fields()
    medians = corpus.source_medians(reference_rows, fields)
    ranges = corpus.target_ranges(medians, fields)
    rows_by_variant: dict[str, list[corpus.AuditedHit]] = {}
    scores: dict[str, dict] = {}
    for variant_dir in variant_dirs:
        paths = sorted(variant_dir.glob("*.wav"))
        if not paths:
            continue
        rows = corpus.load_model_hits(paths)
        rows_by_variant[variant_dir.name] = rows
        score = corpus.population_score(rows, ranges)
        score["screening_score"] = screening_score(score)
        scores[variant_dir.name] = score
    if "current" not in rows_by_variant:
        parser.error("the variant set must contain a current baseline")

    ordered_variants = sorted(
        scores, key=lambda name: scores[name]["screening_score"], reverse=True
    )
    fixtures = select_common_fixtures(rows_by_variant["current"], ranges)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    packaged = package_listening_set(
        args.output_dir,
        args.variant_root,
        ordered_variants,
        rows_by_variant,
        fixtures,
    )
    payload = {
        "warning": "screening rank is not a perceptual material verdict",
        "ordered_variants": ordered_variants,
        "scores": scores,
        "fixtures": fixtures,
        "listening_files": packaged,
    }
    (args.output_dir / "variant_scores.json").write_text(
        json.dumps(payload, indent=2, allow_nan=False) + "\n",
        encoding="utf-8",
    )
    write_report(
        args.output_dir / "report.md",
        ordered_variants,
        scores,
        fixtures,
    )
    print(
        f"Compared {len(scores)} variants; objective order: "
        + ", ".join(ordered_variants)
        + f"; wrote {args.output_dir / 'report.md'}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
