#!/usr/bin/env python3
"""Compare segmented real doorstop recordings with deterministic model renders.

Method:
  * Decode to mono and resample to 48 kHz with polyphase resampling.
  * Use the reviewed strike onsets in doorstop_reference_manifest.json.
  * Analyze at most 3.5 seconds per hit, ending 100 ms before the next strike.
  * Remove DC and peak-normalize each hit only for shape/spectral comparison.
  * Use 2048-sample Hann STFT windows with a 256-sample hop.
  * Compute centroid and band ratios from power, excluding the first 20 ms.
  * Extract an 80 Hz low-passed Hilbert envelope and find recurring lobes with
    12 ms minimum spacing and 6% prominence.
  * Report medians and interquartile ranges; no measured feature is treated as
    a shipping acceptance limit.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import re
from dataclasses import asdict, dataclass
from fractions import Fraction
from pathlib import Path
from typing import Iterable

import numpy as np
import soundfile as sf
from scipy import signal
from scipy import ndimage


TARGET_RATE = 48_000
MAX_HIT_SECONDS = 3.5
MODEL_PATTERN = re.compile(r"reference-v(?P<velocity>[\d.]+)-seed(?P<seed>\d+)")
BANDS = (
    ("macro_20_70", 20.0, 70.0),
    ("low_body_70_180", 70.0, 180.0),
    ("sub_180", 20.0, 180.0),
    ("body_180_500", 180.0, 500.0),
    ("metal_500_1200", 500.0, 1200.0),
    ("upper_1200_3000", 1200.0, 3000.0),
    ("air_3000_10000", 3000.0, 10_000.0),
)


@dataclass
class HitFeatures:
    group: str
    source: str
    hit_index: int
    onset_seconds: float
    duration_seconds: float
    peak: float
    rms: float
    spectral_centroid_hz: float
    early_centroid_hz: float
    tail_centroid_hz: float
    centroid_descent_hz: float
    macro_20_70: float
    low_body_70_180: float
    sub_180: float
    body_180_500: float
    metal_500_1200: float
    upper_1200_3000: float
    air_3000_10000: float
    lobe_count: int
    pulse_rate_hz: float
    t20_seconds: float
    spectral_peaks_hz: str
    velocity: float | None = None
    seed: int | None = None


def read_audio(path: Path) -> tuple[np.ndarray, int]:
    audio, rate = sf.read(path, dtype="float32", always_2d=True)
    mono = np.mean(audio, axis=1, dtype=np.float64).astype(np.float32)
    if rate != TARGET_RATE:
        ratio = Fraction(TARGET_RATE, int(rate)).limit_denominator()
        mono = signal.resample_poly(mono, ratio.numerator, ratio.denominator)
        rate = TARGET_RATE
    return mono, int(rate)


def frame_rms(audio: np.ndarray, frame: int, hop: int) -> tuple[np.ndarray, np.ndarray]:
    if len(audio) < frame:
        audio = np.pad(audio, (0, frame - len(audio)))
    count = 1 + (len(audio) - frame) // hop
    shape = (count, frame)
    strides = (audio.strides[0] * hop, audio.strides[0])
    frames = np.lib.stride_tricks.as_strided(audio, shape=shape, strides=strides)
    rms = np.sqrt(np.mean(frames.astype(np.float64) ** 2, axis=1))
    times = (np.arange(count) * hop + 0.5 * frame) / TARGET_RATE
    return rms, times


def safe_median(values: np.ndarray) -> float:
    finite = values[np.isfinite(values)]
    return float(np.median(finite)) if len(finite) else 0.0


def extract_features(
    audio: np.ndarray,
    group: str,
    source: str,
    hit_index: int,
    onset: float,
    velocity: float | None = None,
    seed: int | None = None,
) -> HitFeatures:
    audio = np.asarray(audio, dtype=np.float64)
    audio -= np.mean(audio)
    peak = float(np.max(np.abs(audio))) if len(audio) else 0.0
    rms_value = float(np.sqrt(np.mean(audio * audio))) if len(audio) else 0.0
    normalized = audio / max(peak, 1e-12)

    frequencies, times, stft = signal.stft(
        normalized,
        fs=TARGET_RATE,
        window="hann",
        nperseg=2048,
        noverlap=1792,
        boundary=None,
        padded=False,
    )
    power = np.abs(stft) ** 2
    time_mask = (times >= 0.020) & (times <= min(2.0, len(audio) / TARGET_RATE))
    selected = power[:, time_mask] if np.any(time_mask) else power
    spectrum = np.sum(selected, axis=1)
    spectrum_total = float(np.sum(spectrum)) + 1e-20
    centroid = float(np.sum(frequencies * spectrum) / spectrum_total)

    frame_energy = np.sum(power, axis=0) + 1e-20
    frame_centroid = np.sum(frequencies[:, None] * power, axis=0) / frame_energy
    early_centroid = safe_median(frame_centroid[(times >= 0.030) & (times < 0.180)])
    tail_centroid = safe_median(frame_centroid[(times >= 0.500) & (times < 1.200)])
    band_values: dict[str, float] = {}
    for name, low, high in BANDS:
        mask = (frequencies >= low) & (frequencies < high)
        band_values[name] = float(np.sum(spectrum[mask]) / spectrum_total)

    analytic_envelope = np.abs(signal.hilbert(normalized))
    envelope_filter = signal.butter(2, 80.0, fs=TARGET_RATE, output="sos")
    envelope = signal.sosfiltfilt(envelope_filter, analytic_envelope)
    envelope = np.maximum(envelope, 0.0)
    search_start = int(0.035 * TARGET_RATE)
    search_end = min(len(envelope), int(2.0 * TARGET_RATE))
    search = envelope[search_start:search_end]
    prominence = max(0.015, 0.06 * float(np.max(search))) if len(search) else 0.015
    lobes, _ = signal.find_peaks(
        search,
        distance=int(0.012 * TARGET_RATE),
        prominence=prominence,
    )
    lobe_times = (lobes + search_start) / TARGET_RATE
    intervals = np.diff(lobe_times)
    plausible = intervals[(intervals >= 0.012) & (intervals <= 0.080)]
    pulse_rate = 1.0 / safe_median(plausible) if len(plausible) else 0.0

    rms_curve, rms_times = frame_rms(normalized.astype(np.float32), 960, 240)
    # Bridge the deep troughs between boing lobes before measuring decay.
    # Otherwise the first swing minimum is incorrectly reported as T20.
    held_rms = ndimage.maximum_filter1d(rms_curve, size=17, mode="nearest")
    rms_peak_index = int(np.argmax(held_rms))
    t20 = 0.0
    if held_rms[rms_peak_index] > 0.0:
        threshold = held_rms[rms_peak_index] * 0.1
        below = held_rms <= threshold
        required = 20  # remain below -20 dB for 100 ms
        stable_below = np.convolve(
            below.astype(np.int16), np.ones(required, dtype=np.int16), mode="same"
        ) >= required
        candidates = np.flatnonzero(
            stable_below[rms_peak_index + required // 2 :]
        )
        if len(candidates):
            index = rms_peak_index + required // 2 + int(candidates[0])
            t20 = float(rms_times[index] - rms_times[rms_peak_index])

    peak_mask = (frequencies >= 80.0) & (frequencies <= 3000.0)
    candidate_bins, _ = signal.find_peaks(
        10.0 * np.log10(spectrum + 1e-20),
        distance=max(1, int(45.0 / (TARGET_RATE / 2048.0))),
        prominence=2.0,
    )
    candidate_bins = candidate_bins[peak_mask[candidate_bins]]
    strongest = sorted(candidate_bins, key=lambda index: spectrum[index], reverse=True)[:5]
    spectral_peaks = ",".join(f"{frequencies[index]:.1f}" for index in strongest)

    return HitFeatures(
        group=group,
        source=source,
        hit_index=hit_index,
        onset_seconds=onset,
        duration_seconds=len(audio) / TARGET_RATE,
        peak=peak,
        rms=rms_value,
        spectral_centroid_hz=centroid,
        early_centroid_hz=early_centroid,
        tail_centroid_hz=tail_centroid,
        centroid_descent_hz=early_centroid - tail_centroid,
        lobe_count=int(len(lobes)),
        pulse_rate_hz=pulse_rate,
        t20_seconds=t20,
        spectral_peaks_hz=spectral_peaks,
        velocity=velocity,
        seed=seed,
        **band_values,
    )


def reference_hits(manifest_path: Path) -> tuple[list[HitFeatures], list[dict]]:
    root = Path.cwd()
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    features: list[HitFeatures] = []
    sources: list[dict] = []
    for recording in manifest["recordings"]:
        path = root / recording["path"]
        audio, rate = read_audio(path)
        onsets = [float(value) for value in recording["onsets"]]
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        sources.append(
            {
                "path": str(path.relative_to(root)),
                "sha256": digest,
                "decoded_sample_rate": rate,
                "channels_folded_to_mono": True,
                "onsets": onsets,
            }
        )
        for index, onset in enumerate(onsets):
            next_onset = onsets[index + 1] if index + 1 < len(onsets) else math.inf
            duration = min(MAX_HIT_SECONDS, max(0.20, next_onset - onset - 0.10))
            start = max(0, int(onset * rate))
            end = min(len(audio), start + int(duration * rate))
            features.append(
                extract_features(
                    audio[start:end], "reference", path.name, index, onset
                )
            )
    return features, sources


def model_hits(paths: Iterable[Path]) -> list[HitFeatures]:
    features: list[HitFeatures] = []
    for path in sorted(paths):
        audio, rate = read_audio(path)
        active = np.flatnonzero(np.abs(audio) > 1e-7)
        start = int(active[0]) if len(active) else 0
        match = MODEL_PATTERN.search(path.stem)
        velocity = float(match.group("velocity")) if match else None
        seed = int(match.group("seed")) if match else None
        segment = audio[start : start + int(MAX_HIT_SECONDS * rate)]
        features.append(
            extract_features(
                segment,
                "model",
                path.name,
                0,
                start / rate,
                velocity,
                seed,
            )
        )
    return features


SUMMARY_FIELDS = (
    "spectral_centroid_hz",
    "centroid_descent_hz",
    "macro_20_70",
    "low_body_70_180",
    "sub_180",
    "body_180_500",
    "metal_500_1200",
    "upper_1200_3000",
    "air_3000_10000",
    "lobe_count",
    "pulse_rate_hz",
    "t20_seconds",
)


def summarize(rows: list[HitFeatures]) -> dict[str, dict[str, float]]:
    result: dict[str, dict[str, float]] = {}
    for field in SUMMARY_FIELDS:
        values = np.array([float(getattr(row, field)) for row in rows], dtype=float)
        values = values[np.isfinite(values)]
        result[field] = {
            "median": float(np.median(values)),
            "q25": float(np.quantile(values, 0.25)),
            "q75": float(np.quantile(values, 0.75)),
        }
    return result


def summarize_reference_sources(
    rows: list[HitFeatures],
) -> tuple[dict[str, dict[str, float]], dict[str, dict[str, float]]]:
    by_source: dict[str, list[HitFeatures]] = {}
    for row in rows:
        by_source.setdefault(row.source, []).append(row)
    source_values: dict[str, dict[str, float]] = {}
    for source, source_rows in by_source.items():
        source_values[source] = {
            field: float(np.median([float(getattr(row, field)) for row in source_rows]))
            for field in SUMMARY_FIELDS
        }
    balanced: dict[str, dict[str, float]] = {}
    for field in SUMMARY_FIELDS:
        values = np.array(
            [source_summary[field] for source_summary in source_values.values()]
        )
        balanced[field] = {
            "median": float(np.median(values)),
            "q25": float(np.quantile(values, 0.25)),
            "q75": float(np.quantile(values, 0.75)),
        }
    return balanced, source_values


def write_report(
    output: Path,
    reference_summary: dict[str, dict[str, float]],
    model_summary: dict[str, dict[str, float]],
    reference_count: int,
    model_count: int,
    source_values: dict[str, dict[str, float]],
) -> None:
    labels = {
        "spectral_centroid_hz": "Spectral centroid (Hz)",
        "centroid_descent_hz": "Early-to-tail centroid descent (Hz)",
        "macro_20_70": "Macro energy 20–70 Hz",
        "low_body_70_180": "Low-body energy 70–180 Hz",
        "sub_180": "Energy below 180 Hz",
        "body_180_500": "Energy 180–500 Hz",
        "metal_500_1200": "Energy 500–1200 Hz",
        "upper_1200_3000": "Energy 1.2–3 kHz",
        "air_3000_10000": "Energy 3–10 kHz",
        "lobe_count": "Envelope lobe count",
        "pulse_rate_hz": "Median lobe rate (Hz)",
        "t20_seconds": "T20 decay (s)",
    }
    lines = [
        "# Doorstop reference comparison",
        "",
        f"Compared {reference_count} recorded hits from {len(source_values)} physical recordings "
        f"with {model_count} deterministic model renders.",
        "The recording summary first takes the median within each source, then",
        "summarizes those source medians so files containing many hits do not dominate.",
        "Every hit is independently peak-normalized; energy values are fractions of STFT power.",
        "",
        "| Feature | Recordings median [IQR] | Model median [IQR] |",
        "|---|---:|---:|",
    ]
    for field in SUMMARY_FIELDS:
        real = reference_summary[field]
        model = model_summary[field]
        lines.append(
            f"| {labels[field]} | {real['median']:.4g} "
            f"[{real['q25']:.4g}, {real['q75']:.4g}] | "
            f"{model['median']:.4g} [{model['q25']:.4g}, {model['q75']:.4g}] |"
        )
    lines.extend(
        [
            "",
            "## Per-recording medians",
            "",
            "| Recording | Centroid (Hz) | 20–70 | 70–180 | 180–500 | 500–1200 | 1.2–3k | Pulse Hz |",
            "|---|---:|---:|---:|---:|---:|---:|---:|",
        ]
    )
    for source, values in source_values.items():
        lines.append(
            f"| {source} | {values['spectral_centroid_hz']:.4g} | "
            f"{values['macro_20_70']:.3f} | "
            f"{values['low_body_70_180']:.3f} | "
            f"{values['body_180_500']:.3f} | "
            f"{values['metal_500_1200']:.3f} | "
            f"{values['upper_1200_3000']:.3f} | "
            f"{values['pulse_rate_hz']:.3g} |"
        )
    lines.extend(
        [
            "",
            "These are descriptive targets, not pass/fail limits. Recording chains,",
            "background noise, MP3 coding, and unknown strike strength remain confounds.",
            "",
        ]
    )
    output.write_text("\n".join(lines), encoding="utf-8")


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
        default=Path("build/doorstop-reference-renders"),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("build/doorstop-reference-analysis"),
    )
    args = parser.parse_args()
    model_paths = list(args.model_dir.glob("*.wav"))
    if not model_paths:
        parser.error(f"no model WAV files found in {args.model_dir}")

    reference, sources = reference_hits(args.manifest)
    model = model_hits(model_paths)
    rows = reference + model
    args.output_dir.mkdir(parents=True, exist_ok=True)
    fieldnames = list(asdict(rows[0]).keys())
    with (args.output_dir / "hit_features.csv").open(
        "w", newline="", encoding="utf-8"
    ) as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(asdict(row) for row in rows)

    reference_hit_summary = summarize(reference)
    reference_summary, source_values = summarize_reference_sources(reference)
    model_summary = summarize(model)
    method = {
        "target_sample_rate": TARGET_RATE,
        "channel_handling": "arithmetic mono fold-down",
        "maximum_hit_seconds": MAX_HIT_SECONDS,
        "level_matching": "independent peak normalization after DC removal",
        "stft": "2048-sample Hann window, 256-sample hop",
        "spectral_centroid": "power-weighted mean frequency, 20 ms to 2 s",
        "band_energy": "band STFT power divided by total STFT power",
        "envelope": "Hilbert magnitude, zero-phase 2-pole 80 Hz low-pass",
        "lobe_peaks": "minimum 12 ms spacing, 6% prominence, 35 ms to 2 s",
        "t20": "first 20 ms RMS frame at -20 dB after peak; 5 ms hop",
    }
    payload = {
        "method": method,
        "sources": sources,
        "reference_hit_count": len(reference),
        "model_render_count": len(model),
        "reference_summary": reference_summary,
        "reference_hit_weighted_summary": reference_hit_summary,
        "reference_source_medians": source_values,
        "model_summary": model_summary,
    }
    (args.output_dir / "summary.json").write_text(
        json.dumps(payload, indent=2) + "\n", encoding="utf-8"
    )
    write_report(
        args.output_dir / "report.md",
        reference_summary,
        model_summary,
        len(reference),
        len(model),
        source_values,
    )
    print(
        f"Analyzed {len(reference)} recorded hits and {len(model)} model renders; "
        f"wrote {args.output_dir / 'report.md'}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
