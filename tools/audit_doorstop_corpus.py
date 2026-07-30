#!/usr/bin/env python3
"""Build source-balanced acoustic targets for the Doorstop reference model.

This is an offline analysis tool.  It treats the reviewed strike onsets in
``doorstop_reference_manifest.json`` as authoritative, measures every strike,
then takes a median within each recording before deriving corpus ranges.  A
recording with fifteen strikes therefore has the same influence as a recording
with one strike.

When model WAVs are supplied, the tool evaluates the *population* of renders:

* containment: how often individual seeds remain in a plausible real range;
* coverage: how much of the real 10th-to-90th percentile corridor is spanned;
* median bias: whether the whole population is displaced from the corpus.

The resulting ranges are descriptive engineering targets, not proof that every
recording is clean or that every seed must imitate one particular doorstop.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import re
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable

import numpy as np
from scipy import ndimage, signal

import analyze_doorstop_reference as reference_analysis


RATE = reference_analysis.TARGET_RATE
MAX_HIT_SECONDS = reference_analysis.MAX_HIT_SECONDS
WINDOWS = (
    ("onset_0_30ms", 0.000, 0.030),
    ("early_30_100ms", 0.030, 0.100),
    ("body_100_300ms", 0.100, 0.300),
    ("tail_300_1000ms", 0.300, 1.000),
)
WINDOW_BANDS = (
    ("low_body", 70.0, 180.0),
    ("body", 180.0, 500.0),
    ("metal", 500.0, 1200.0),
    ("upper", 1200.0, 3000.0),
    ("air", 3000.0, 10_000.0),
)
MODEL_SEED_PATTERN = re.compile(r"seed(?P<seed>\d+)", re.IGNORECASE)
MODEL_VELOCITY_PATTERN = re.compile(
    r"(?:^|[-_])v(?:elocity)?(?P<velocity>\d+(?:\.\d+)?)",
    re.IGNORECASE,
)

# These features describe the sound strongly enough to steer model selection
# without giving multiple votes to overlapping bands such as <180 and 70-180.
POPULATION_FEATURES = (
    "spectral_centroid_hz",
    "centroid_descent_hz",
    "macro_20_70",
    "low_body_70_180",
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


@dataclass
class AuditedHit:
    group: str
    source: str
    hit_index: int
    onset_seconds: float
    duration_seconds: float
    velocity: float | None
    seed: int | None
    spectral_centroid_hz: float
    centroid_descent_hz: float
    macro_20_70: float
    low_body_70_180: float
    body_180_500: float
    metal_500_1200: float
    upper_1200_3000: float
    air_3000_10000: float
    lobe_count: float
    pulse_rate_hz: float
    boing_modulation_depth: float
    t20_seconds: float
    ridge_count: float
    ridge_median_q: float
    ridge_density_per_khz: float
    spectral_flatness: float
    pre_hit_noise_rms: float
    onset_snr_db: float
    onset_offset_ms: float
    clipped_fraction: float
    tail_to_peak_rms: float
    quality_flags: str
    quality_score: float
    onset_0_30ms_centroid_hz: float
    onset_0_30ms_low_body: float
    onset_0_30ms_body: float
    onset_0_30ms_metal: float
    onset_0_30ms_upper: float
    onset_0_30ms_air: float
    early_30_100ms_centroid_hz: float
    early_30_100ms_low_body: float
    early_30_100ms_body: float
    early_30_100ms_metal: float
    early_30_100ms_upper: float
    early_30_100ms_air: float
    body_100_300ms_centroid_hz: float
    body_100_300ms_low_body: float
    body_100_300ms_body: float
    body_100_300ms_metal: float
    body_100_300ms_upper: float
    body_100_300ms_air: float
    tail_300_1000ms_centroid_hz: float
    tail_300_1000ms_low_body: float
    tail_300_1000ms_body: float
    tail_300_1000ms_metal: float
    tail_300_1000ms_upper: float
    tail_300_1000ms_air: float


def rms(audio: np.ndarray) -> float:
    return float(np.sqrt(np.mean(np.asarray(audio, dtype=np.float64) ** 2))) if len(audio) else 0.0


def spectrum_features(audio: np.ndarray) -> dict[str, float]:
    """Measure a short, exact time region without STFT time smearing."""
    values = np.asarray(audio, dtype=np.float64)
    if len(values) < 8:
        return {
            "centroid_hz": 0.0,
            **{name: 0.0 for name, _, _ in WINDOW_BANDS},
        }
    values = values - np.mean(values)
    windowed = values * signal.windows.hann(len(values), sym=False)
    transform_size = max(4096, 1 << int(math.ceil(math.log2(len(values)))))
    power = np.abs(np.fft.rfft(windowed, n=transform_size)) ** 2
    frequencies = np.fft.rfftfreq(transform_size, 1.0 / RATE)
    useful = (frequencies >= 20.0) & (frequencies < 10_000.0)
    total = float(np.sum(power[useful])) + 1e-30
    result = {
        "centroid_hz": float(np.sum(frequencies[useful] * power[useful]) / total)
    }
    for name, low, high in WINDOW_BANDS:
        mask = (frequencies >= low) & (frequencies < high)
        result[name] = float(np.sum(power[mask]) / total)
    return result


def ridge_features(audio: np.ndarray) -> dict[str, float]:
    """Estimate stable spectral ridge density, Q, and between-ridge floor."""
    values = np.asarray(audio[: int(1.0 * RATE)], dtype=np.float64)
    if len(values) < 256:
        return {
            "ridge_count": 0.0,
            "ridge_median_q": 0.0,
            "ridge_density_per_khz": 0.0,
            "spectral_flatness": 0.0,
        }
    values -= np.mean(values)
    frequencies, psd = signal.welch(
        values,
        fs=RATE,
        window="hann",
        nperseg=min(4096, len(values)),
        noverlap=min(3072, max(0, len(values) // 2)),
        scaling="spectrum",
    )
    selection = (frequencies >= 70.0) & (frequencies <= 6000.0)
    selected_psd = np.maximum(psd[selection], 1e-30)
    selected_frequencies = frequencies[selection]
    log_power = 10.0 * np.log10(selected_psd)
    broad_envelope = ndimage.gaussian_filter1d(log_power, sigma=10.0)
    residual = log_power - broad_envelope
    peaks, _ = signal.find_peaks(
        residual,
        prominence=3.0,
        distance=max(1, int(35.0 / (RATE / min(4096, len(values))))),
    )
    q_values: list[float] = []
    if len(peaks):
        # Peaks were selected against the local envelope, so their width must
        # be measured in that same residual. Measuring them in absolute power
        # can produce zero-width results on a steep global spectral slope.
        widths, _, _, _ = signal.peak_widths(residual, peaks, rel_height=0.5)
        bin_hz = float(np.median(np.diff(selected_frequencies)))
        for peak, width in zip(peaks, widths):
            bandwidth = max(bin_hz, float(width) * bin_hz)
            q_values.append(float(selected_frequencies[peak] / bandwidth))
    flat_band = (selected_frequencies >= 180.0) & (selected_frequencies <= 5000.0)
    flat_power = selected_psd[flat_band]
    flatness = (
        float(np.exp(np.mean(np.log(flat_power))) / np.mean(flat_power))
        if len(flat_power) and np.mean(flat_power) > 0.0
        else 0.0
    )
    bandwidth_khz = (6000.0 - 70.0) / 1000.0
    return {
        "ridge_count": float(len(peaks)),
        "ridge_median_q": float(np.median(q_values)) if q_values else 0.0,
        "ridge_density_per_khz": float(len(peaks) / bandwidth_khz),
        "spectral_flatness": flatness,
    }


def boing_modulation_depth(audio: np.ndarray) -> float:
    """Measure fast lobe contrast after removing the overall decay envelope."""
    values = np.asarray(audio, dtype=np.float64)
    if len(values) < int(0.20 * RATE):
        return 0.0
    normalized = values / max(float(np.max(np.abs(values))), 1e-12)
    envelope = np.abs(signal.hilbert(normalized))
    fast_filter = signal.butter(2, 90.0, fs=RATE, output="sos")
    slow_filter = signal.butter(2, 8.0, fs=RATE, output="sos")
    fast = np.maximum(signal.sosfiltfilt(fast_filter, envelope), 0.0)
    slow = np.maximum(signal.sosfiltfilt(slow_filter, fast), 1e-9)
    start = int(0.035 * RATE)
    end = min(len(values), int(1.0 * RATE))
    active = slow[start:end] >= 0.08 * float(np.max(slow[start:end]))
    ratios = fast[start:end][active] / slow[start:end][active]
    if len(ratios) < 16:
        return 0.0
    q10, q90 = np.quantile(ratios, [0.10, 0.90])
    return float((q90 - q10) / max(q90 + q10, 1e-9))


def estimate_onset_offset(
    recording: np.ndarray, onset_sample: int, pre_noise_rms: float
) -> float:
    radius = int(0.060 * RATE)
    start = max(0, onset_sample - radius)
    end = min(len(recording), onset_sample + radius)
    region = np.asarray(recording[start:end], dtype=np.float64)
    if len(region) < int(0.010 * RATE):
        return 0.0
    frame = max(16, int(0.004 * RATE))
    hop = max(1, int(0.001 * RATE))
    curve, times = reference_analysis.frame_rms(
        region.astype(np.float32), frame, hop
    )
    local_peak = float(np.max(curve)) if len(curve) else 0.0
    threshold = max(pre_noise_rms * 5.0, local_peak * 0.08, 1e-8)
    crossings = np.flatnonzero(curve >= threshold)
    if not len(crossings):
        return 0.0
    detected = start + float(times[crossings[0]]) * RATE
    return 1000.0 * (detected - onset_sample) / RATE


def quality_features(
    recording: np.ndarray,
    segment: np.ndarray,
    onset_sample: int,
    limited_by_next_hit: bool,
) -> dict[str, float | str]:
    pre_start = max(0, onset_sample - int(0.250 * RATE))
    guard = int(0.010 * RATE)
    pre_end = max(pre_start, onset_sample - guard)
    pre_noise = rms(recording[pre_start:pre_end])
    onset_level = rms(segment[: min(len(segment), int(0.250 * RATE))])
    snr_db = 20.0 * math.log10((onset_level + 1e-12) / (pre_noise + 1e-12))
    offset_ms = estimate_onset_offset(recording, onset_sample, pre_noise)
    clipped = float(np.mean(np.abs(segment) >= 0.999)) if len(segment) else 0.0

    frame_curve, _ = reference_analysis.frame_rms(
        np.asarray(segment, dtype=np.float32), int(0.020 * RATE), int(0.005 * RATE)
    )
    peak_frame = float(np.max(frame_curve)) if len(frame_curve) else 0.0
    tail_frames = max(1, int(0.100 / 0.005))
    tail_level = float(np.median(frame_curve[-tail_frames:])) if len(frame_curve) else 0.0
    tail_ratio = tail_level / max(peak_frame, 1e-12)

    flags: list[str] = []
    penalty = 0.0
    if snr_db < 20.0:
        flags.append("low_snr")
        penalty += min(0.35, (20.0 - snr_db) / 40.0)
    # The reviewed marker denotes physical contact, while a low-frequency
    # spring can take several tens of milliseconds to reach 8% of peak RMS.
    # Only a larger disagreement is useful as an onset-review prompt.
    if abs(offset_ms) > 45.0:
        flags.append("onset_review")
        penalty += min(0.25, (abs(offset_ms) - 45.0) / 100.0)
    if clipped > 0.001:
        flags.append("clipping")
        penalty += min(0.40, clipped * 50.0)
    if tail_ratio > 0.15:
        flags.append("overlap_risk" if limited_by_next_hit else "active_tail")
        penalty += 0.10 if limited_by_next_hit else 0.03
    return {
        "pre_hit_noise_rms": pre_noise,
        "onset_snr_db": snr_db,
        "onset_offset_ms": offset_ms,
        "clipped_fraction": clipped,
        "tail_to_peak_rms": tail_ratio,
        "quality_flags": ",".join(flags),
        "quality_score": max(0.0, 1.0 - penalty),
    }


def audited_hit(
    segment: np.ndarray,
    group: str,
    source: str,
    hit_index: int,
    onset: float,
    recording: np.ndarray | None = None,
    onset_sample: int = 0,
    limited_by_next_hit: bool = False,
    velocity: float | None = None,
    seed: int | None = None,
) -> AuditedHit:
    base = reference_analysis.extract_features(
        segment, group, source, hit_index, onset, velocity, seed
    )
    ridges = ridge_features(segment)
    if recording is None:
        quality: dict[str, float | str] = {
            "pre_hit_noise_rms": 0.0,
            "onset_snr_db": 0.0,
            "onset_offset_ms": 0.0,
            "clipped_fraction": float(np.mean(np.abs(segment) >= 0.999)),
            "tail_to_peak_rms": 0.0,
            "quality_flags": "",
            "quality_score": 1.0,
        }
    else:
        quality = quality_features(
            recording, segment, onset_sample, limited_by_next_hit
        )

    window_values: dict[str, float] = {}
    for window_name, start_seconds, end_seconds in WINDOWS:
        start = min(len(segment), int(start_seconds * RATE))
        end = min(len(segment), int(end_seconds * RATE))
        measured = spectrum_features(segment[start:end])
        for feature, value in measured.items():
            window_values[f"{window_name}_{feature}"] = value

    return AuditedHit(
        group=group,
        source=source,
        hit_index=hit_index,
        onset_seconds=onset,
        duration_seconds=len(segment) / RATE,
        velocity=velocity,
        seed=seed,
        spectral_centroid_hz=base.spectral_centroid_hz,
        centroid_descent_hz=base.centroid_descent_hz,
        macro_20_70=base.macro_20_70,
        low_body_70_180=base.low_body_70_180,
        body_180_500=base.body_180_500,
        metal_500_1200=base.metal_500_1200,
        upper_1200_3000=base.upper_1200_3000,
        air_3000_10000=base.air_3000_10000,
        lobe_count=float(base.lobe_count),
        pulse_rate_hz=base.pulse_rate_hz,
        boing_modulation_depth=boing_modulation_depth(segment),
        t20_seconds=base.t20_seconds,
        **ridges,
        **quality,
        **window_values,
    )


def load_reference_hits(manifest_path: Path) -> tuple[list[AuditedHit], list[dict]]:
    root = Path.cwd()
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    rows: list[AuditedHit] = []
    sources: list[dict] = []
    for recording_entry in manifest["recordings"]:
        path = root / recording_entry["path"]
        recording, rate = reference_analysis.read_audio(path)
        onsets = [float(value) for value in recording_entry["onsets"]]
        sources.append(
            {
                "source": path.name,
                "path": str(path),
                "decoded_sample_rate": rate,
                "reviewed_hit_count": len(onsets),
            }
        )
        for hit_index, onset in enumerate(onsets):
            next_onset = (
                onsets[hit_index + 1] if hit_index + 1 < len(onsets) else math.inf
            )
            natural_duration = MAX_HIT_SECONDS
            limited_by_next = math.isfinite(next_onset) and (
                next_onset - onset - 0.10 < natural_duration
            )
            duration = min(
                natural_duration, max(0.20, next_onset - onset - 0.10)
            )
            start = max(0, int(onset * rate))
            end = min(len(recording), start + int(duration * rate))
            rows.append(
                audited_hit(
                    recording[start:end],
                    "reference",
                    path.name,
                    hit_index,
                    onset,
                    recording=recording,
                    onset_sample=start,
                    limited_by_next_hit=limited_by_next,
                )
            )
    return rows, sources


def load_model_hits(paths: Iterable[Path]) -> list[AuditedHit]:
    rows: list[AuditedHit] = []
    for path in sorted(paths):
        recording, rate = reference_analysis.read_audio(path)
        active = np.flatnonzero(np.abs(recording) > 1e-7)
        start = int(active[0]) if len(active) else 0
        segment = recording[start : start + int(MAX_HIT_SECONDS * rate)]
        seed_match = MODEL_SEED_PATTERN.search(path.stem)
        # Variant names may themselves contain a version token such as
        # ``rack-v2``. The render velocity is the final ``-v...`` token.
        velocity_matches = list(MODEL_VELOCITY_PATTERN.finditer(path.stem))
        velocity_match = velocity_matches[-1] if velocity_matches else None
        rows.append(
            audited_hit(
                segment,
                "model",
                path.name,
                0,
                start / rate,
                velocity=(
                    float(velocity_match.group("velocity"))
                    if velocity_match
                    else None
                ),
                seed=int(seed_match.group("seed")) if seed_match else None,
            )
        )
    return rows


def numeric_fields() -> tuple[str, ...]:
    excluded = {
        "group",
        "source",
        "hit_index",
        "onset_seconds",
        "duration_seconds",
        "velocity",
        "seed",
        "pre_hit_noise_rms",
        "onset_snr_db",
        "onset_offset_ms",
        "clipped_fraction",
        "tail_to_peak_rms",
        "quality_flags",
        "quality_score",
    }
    return tuple(name for name in AuditedHit.__dataclass_fields__ if name not in excluded)


def valid_values(rows: Iterable[AuditedHit], field: str) -> np.ndarray:
    values = np.array([float(getattr(row, field)) for row in rows], dtype=float)
    values = values[np.isfinite(values)]
    if field in {
        "pulse_rate_hz",
        "t20_seconds",
        "ridge_median_q",
        "lobe_count",
        "boing_modulation_depth",
    }:
        values = values[values > 0.0]
    return values


def source_medians(
    rows: list[AuditedHit], fields: Iterable[str]
) -> dict[str, dict[str, float]]:
    grouped: dict[str, list[AuditedHit]] = {}
    for row in rows:
        grouped.setdefault(row.source, []).append(row)
    result: dict[str, dict[str, float]] = {}
    for source, source_rows in grouped.items():
        result[source] = {}
        for field in fields:
            values = valid_values(source_rows, field)
            result[source][field] = (
                float(np.median(values)) if len(values) else float("nan")
            )
        result[source]["quality_score"] = float(
            np.median([row.quality_score for row in source_rows])
        )
    return result


def target_ranges(
    medians: dict[str, dict[str, float]], fields: Iterable[str]
) -> dict[str, dict[str, float]]:
    result: dict[str, dict[str, float]] = {}
    for field in fields:
        values = np.array(
            [features[field] for features in medians.values()], dtype=float
        )
        values = values[np.isfinite(values)]
        if not len(values):
            continue
        result[field] = {
            "q10": float(np.quantile(values, 0.10)),
            "median": float(np.median(values)),
            "q90": float(np.quantile(values, 0.90)),
            "minimum": float(np.min(values)),
            "maximum": float(np.max(values)),
            "source_count": float(len(values)),
        }
    return result


def population_score(
    model_rows: list[AuditedHit],
    ranges: dict[str, dict[str, float]],
) -> dict:
    scores: dict[str, dict[str, float]] = {}
    for field in POPULATION_FEATURES:
        if field not in ranges:
            continue
        values = valid_values(model_rows, field)
        if not len(values):
            continue
        target = ranges[field]
        low = target["q10"]
        center = target["median"]
        high = target["q90"]
        span = max(high - low, abs(center) * 0.10, 1e-9)
        model_low = float(np.quantile(values, 0.10))
        model_center = float(np.median(values))
        model_high = float(np.quantile(values, 0.90))
        intersection = max(0.0, min(high, model_high) - max(low, model_low))
        containment = float(np.mean((values >= low) & (values <= high)))
        expanded_low = low - 0.5 * span
        expanded_high = high + 0.5 * span
        outlier_rate = float(
            np.mean((values < expanded_low) | (values > expanded_high))
        )
        scores[field] = {
            "reference_q10": low,
            "reference_median": center,
            "reference_q90": high,
            "model_q10": model_low,
            "model_median": model_center,
            "model_q90": model_high,
            "containment": containment,
            "coverage": min(1.0, intersection / span),
            "median_bias_corridors": (model_center - center) / span,
            "expanded_outlier_rate": outlier_rate,
        }

    containment_values = [item["containment"] for item in scores.values()]
    coverage_values = [item["coverage"] for item in scores.values()]
    bias_values = [abs(item["median_bias_corridors"]) for item in scores.values()]
    outlier_values = [item["expanded_outlier_rate"] for item in scores.values()]
    seeds = sorted({row.seed for row in model_rows if row.seed is not None})
    velocities = sorted(
        {row.velocity for row in model_rows if row.velocity is not None}
    )
    return {
        "render_count": len(model_rows),
        "unique_seed_count": len(seeds),
        "seeds": seeds,
        "velocities": velocities,
        "aggregate": {
            "mean_containment": float(np.mean(containment_values)),
            "mean_coverage": float(np.mean(coverage_values)),
            "mean_absolute_median_bias_corridors": float(np.mean(bias_values)),
            "mean_expanded_outlier_rate": float(np.mean(outlier_values)),
        },
        "features": scores,
    }


def normalized_source_matrix(
    medians: dict[str, dict[str, float]],
) -> tuple[list[str], np.ndarray]:
    fields = (
        "spectral_centroid_hz",
        "low_body_70_180",
        "body_180_500",
        "metal_500_1200",
        "upper_1200_3000",
        "t20_seconds",
    )
    names = sorted(medians)
    matrix = np.array(
        [[medians[name][field] for field in fields] for name in names], dtype=float
    )
    for column in range(matrix.shape[1]):
        values = matrix[:, column]
        finite = np.isfinite(values)
        replacement = float(np.median(values[finite])) if np.any(finite) else 0.0
        values[~finite] = replacement
        q10, q90 = np.quantile(values, [0.10, 0.90])
        matrix[:, column] = (values - np.median(values)) / max(q90 - q10, 1e-9)
    return names, matrix


def choose_representatives(
    rows: list[AuditedHit], medians: dict[str, dict[str, float]]
) -> list[dict]:
    names, matrix = normalized_source_matrix(medians)
    centroid_column = matrix[:, 0]
    t20_column = matrix[:, 5]
    center_distance = np.linalg.norm(matrix, axis=1)
    roles = {
        "dark": int(np.argmin(centroid_column)),
        "balanced": int(np.argmin(center_distance)),
        "bright": int(np.argmax(centroid_column)),
        "long_tail": int(np.argmax(t20_column)),
    }
    grouped: dict[str, list[AuditedHit]] = {}
    for row in rows:
        grouped.setdefault(row.source, []).append(row)
    representatives: list[dict] = []
    for role, source_index in roles.items():
        source = names[source_index]
        source_rows = grouped[source]
        feature_names = (
            "spectral_centroid_hz",
            "low_body_70_180",
            "body_180_500",
            "metal_500_1200",
            "upper_1200_3000",
            "t20_seconds",
        )
        source_center = np.array(
            [medians[source][field] for field in feature_names], dtype=float
        )
        scales = np.maximum(np.abs(source_center), 1e-6)
        distances = []
        for row in source_rows:
            vector = np.array(
                [float(getattr(row, field)) for field in feature_names], dtype=float
            )
            distances.append(float(np.linalg.norm((vector - source_center) / scales)))
        hit = source_rows[int(np.argmin(distances))]
        representatives.append(
            {
                "role": role,
                "source": source,
                "hit_index": hit.hit_index,
                "onset_seconds": hit.onset_seconds,
                "quality_score": hit.quality_score,
                "quality_flags": hit.quality_flags,
            }
        )
    return representatives


def write_csv(path: Path, rows: list[dict], fieldnames: list[str]) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def write_report(
    path: Path,
    reference_rows: list[AuditedHit],
    medians: dict[str, dict[str, float]],
    ranges: dict[str, dict[str, float]],
    representatives: list[dict],
    model_score: dict | None,
) -> None:
    lines = [
        "# Doorstop corpus audit",
        "",
        f"Measured {len(reference_rows)} reviewed strikes from {len(medians)} recordings.",
        "Target corridors are the 10th, 50th, and 90th percentiles of per-recording",
        "medians. Each physical recording gets one vote; repeated strikes improve",
        "its estimate but do not increase its weight.",
        "",
        "## Population targets",
        "",
        "| Feature | P10 | Median | P90 |",
        "|---|---:|---:|---:|",
    ]
    for field in POPULATION_FEATURES:
        if field not in ranges:
            continue
        target = ranges[field]
        lines.append(
            f"| {field} | {target['q10']:.4g} | "
            f"{target['median']:.4g} | {target['q90']:.4g} |"
        )
    lines.extend(
        [
            "",
            "## Recording medians",
            "",
            "| Recording | Centroid Hz | 70–180 | 180–500 | 500–1200 | 1.2–3k | T20 s | Quality |",
            "|---|---:|---:|---:|---:|---:|---:|---:|",
        ]
    )
    for source, values in sorted(medians.items()):
        lines.append(
            f"| {source} | {values['spectral_centroid_hz']:.4g} | "
            f"{values['low_body_70_180']:.3f} | "
            f"{values['body_180_500']:.3f} | "
            f"{values['metal_500_1200']:.3f} | "
            f"{values['upper_1200_3000']:.3f} | "
            f"{values['t20_seconds']:.3f} | "
            f"{values['quality_score']:.2f} |"
        )
    lines.extend(
        [
            "",
            "## Suggested listening fixtures",
            "",
            "| Role | Recording | Hit | Onset s | Quality note |",
            "|---|---|---:|---:|---|",
        ]
    )
    for item in representatives:
        lines.append(
            f"| {item['role']} | {item['source']} | {item['hit_index']} | "
            f"{item['onset_seconds']:.3f} | {item['quality_flags'] or 'clean'} |"
        )

    if model_score:
        aggregate = model_score["aggregate"]
        lines.extend(
            [
                "",
                "## Model population",
                "",
                f"Measured {model_score['render_count']} renders across "
                f"{model_score['unique_seed_count']} parsed seeds.",
                "",
                f"- Mean containment inside real P10–P90: {aggregate['mean_containment']:.1%}",
                f"- Mean coverage of the real P10–P90 corridor: {aggregate['mean_coverage']:.1%}",
                f"- Mean absolute median bias: {aggregate['mean_absolute_median_bias_corridors']:.3f} corridors",
                f"- Mean expanded-range outlier rate: {aggregate['mean_expanded_outlier_rate']:.1%}",
                "",
                "| Feature | Real P10–P90 | Model P10–P90 | Containment | Coverage | Bias |",
                "|---|---:|---:|---:|---:|---:|",
            ]
        )
        for field, score in model_score["features"].items():
            lines.append(
                f"| {field} | {score['reference_q10']:.4g}–{score['reference_q90']:.4g} | "
                f"{score['model_q10']:.4g}–{score['model_q90']:.4g} | "
                f"{score['containment']:.1%} | {score['coverage']:.1%} | "
                f"{score['median_bias_corridors']:+.2f} |"
            )

    lines.extend(
        [
            "",
            "## Interpretation",
            "",
            "Containment is not expected to be 100%: specimen variation is desirable.",
            "Low coverage means the seed mapping is too narrow; low containment with a",
            "large median bias means the model family is systematically misplaced.",
            "Quality flags are review prompts, not automatic exclusions. Recording-chain",
            "noise and edited tails can otherwise masquerade as physical spring behavior.",
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
        "--model-dir",
        type=Path,
        help="optional directory containing a batch of model WAV renders",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("build/doorstop-corpus-audit"),
    )
    args = parser.parse_args()

    reference_rows, sources = load_reference_hits(args.manifest)
    fields = numeric_fields()
    medians = source_medians(reference_rows, fields)
    ranges = target_ranges(medians, fields)
    representatives = choose_representatives(reference_rows, medians)

    model_rows: list[AuditedHit] = []
    if args.model_dir:
        model_paths = list(args.model_dir.glob("*.wav"))
        if not model_paths:
            parser.error(f"no model WAV files found in {args.model_dir}")
        model_rows = load_model_hits(model_paths)
    model_score = population_score(model_rows, ranges) if model_rows else None

    args.output_dir.mkdir(parents=True, exist_ok=True)
    all_rows = reference_rows + model_rows
    write_csv(
        args.output_dir / "hits.csv",
        [asdict(row) for row in all_rows],
        list(AuditedHit.__dataclass_fields__),
    )
    range_rows = [{"feature": name, **values} for name, values in ranges.items()]
    write_csv(
        args.output_dir / "target_ranges.csv",
        range_rows,
        ["feature", "q10", "median", "q90", "minimum", "maximum", "source_count"],
    )
    payload = {
        "method": {
            "source_balance": "median within source, then percentiles across sources",
            "target_corridor": "10th to 90th percentile of source medians",
            "time_windows_seconds": {
                name: [start, end] for name, start, end in WINDOWS
            },
            "ridge_analysis": "70-6000 Hz Welch spectrum; peaks 3 dB above smoothed envelope",
            "quality_flags": "advisory only; no source is automatically excluded",
        },
        "sources": sources,
        "reference_hit_count": len(reference_rows),
        "source_medians": medians,
        "target_ranges": ranges,
        "representatives": representatives,
        "model_population": model_score,
    }
    (args.output_dir / "audit.json").write_text(
        json.dumps(payload, indent=2, allow_nan=False) + "\n",
        encoding="utf-8",
    )
    if model_score:
        (args.output_dir / "model_population_score.json").write_text(
            json.dumps(model_score, indent=2, allow_nan=False) + "\n",
            encoding="utf-8",
        )
    write_report(
        args.output_dir / "report.md",
        reference_rows,
        medians,
        ranges,
        representatives,
        model_score,
    )
    print(
        f"Audited {len(reference_rows)} recorded hits from {len(medians)} sources"
        + (
            f" and {len(model_rows)} model renders"
            if model_rows
            else ""
        )
        + f"; wrote {args.output_dir / 'report.md'}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
