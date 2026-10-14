#!/usr/bin/env python3
"""Analyze a simultaneous four-channel Octavia Doorstop velocity sweep."""

from __future__ import annotations

import argparse
import array
import json
import math
import struct
import sys
from pathlib import Path


WINDOWS = (
    ("0-50ms", 0.0, 0.050),
    ("50-200ms", 0.050, 0.200),
    ("200-500ms", 0.200, 0.500),
    ("500ms-1s", 0.500, 1.000),
    ("1-2s", 1.000, 2.000),
    ("2-4s", 2.000, 4.000),
    ("total-4s", 0.0, 4.000),
)


def read_float_wav(path: Path) -> tuple[int, int, array.array]:
    with path.open("rb") as source:
        if source.read(4) != b"RIFF":
            raise ValueError("not a RIFF file")
        source.read(4)
        if source.read(4) != b"WAVE":
            raise ValueError("not a WAVE file")
        fmt = None
        fmt_chunk = None
        payload = None
        while True:
            chunk_header = source.read(8)
            if not chunk_header:
                break
            if len(chunk_header) != 8:
                raise ValueError("truncated WAV chunk header")
            chunk_id, chunk_size = struct.unpack("<4sI", chunk_header)
            chunk = source.read(chunk_size)
            if len(chunk) != chunk_size:
                raise ValueError("truncated WAV chunk")
            if chunk_size & 1:
                source.read(1)
            if chunk_id == b"fmt ":
                if len(chunk) < 16:
                    raise ValueError("truncated fmt chunk")
                fmt = struct.unpack("<HHIIHH", chunk[:16])
                fmt_chunk = chunk
            elif chunk_id == b"data":
                payload = chunk
        if fmt is None or payload is None:
            raise ValueError("WAV is missing fmt or data")

    audio_format, channels, sample_rate, _, _, bits = fmt
    ieee_float = audio_format == 3
    if audio_format == 0xFFFE and fmt_chunk is not None and len(fmt_chunk) >= 40:
        float_subformat = bytes.fromhex("0300000000001000800000aa00389b71")
        ieee_float = fmt_chunk[24:40] == float_subformat
    if not ieee_float or bits != 32:
        raise ValueError(
            f"expected IEEE float32 WAV (format 3/32-bit), got {audio_format}/{bits}"
        )
    samples = array.array("f")
    samples.frombytes(payload)
    if sys.byteorder != "little":
        samples.byteswap()
    if channels <= 0 or len(samples) % channels:
        raise ValueError("invalid interleaved sample count")
    return sample_rate, channels, samples


def rms(samples: array.array, channels: int, channel: int, start: int, end: int) -> float:
    if end <= start:
        return 0.0
    total = 0.0
    for frame in range(start, end):
        value = samples[frame * channels + channel]
        total += value * value
    return math.sqrt(total / (end - start))


def peak(samples: array.array, channels: int, channel: int, start: int, end: int) -> float:
    result = 0.0
    for frame in range(start, end):
        result = max(result, abs(samples[frame * channels + channel]))
    return result


def db_ratio(target: float, reference: float) -> float:
    return 20.0 * math.log10(max(target, 1.0e-20) / max(reference, 1.0e-20))


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Analyze A-D as simultaneous Doorstop strikes at 5, 8, 9, and 10 V."
    )
    parser.add_argument("wav", type=Path, help="Octavia IEEE-float WAV")
    parser.add_argument(
        "--labels", default="5V,8V,9V,10V", help="comma-separated channel labels"
    )
    args = parser.parse_args()

    sample_rate, channels, samples = read_float_wav(args.wav)
    labels = [label.strip() for label in args.labels.split(",") if label.strip()]
    if channels != len(labels):
        raise ValueError(f"WAV has {channels} channels but {len(labels)} labels were supplied")
    frames = len(samples) // channels
    global_peak = max(abs(value) for value in samples) if samples else 0.0
    threshold = max(0.005, global_peak * 0.01)
    onset = next(
        (
            frame
            for frame in range(frames)
            if any(abs(samples[frame * channels + channel]) >= threshold
                   for channel in range(channels))
        ),
        None,
    )
    if onset is None:
        raise ValueError("no common strike onset detected")

    metadata_path = args.wav.with_suffix(".json")
    channel_order = None
    if metadata_path.exists():
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        channel_order = metadata.get("channels")
        if channel_order != ["A", "B", "C", "D"]:
            raise ValueError(
                f"expected Octavia channel order A,B,C,D, got {channel_order!r}"
            )

    print(f"file={args.wav}")
    print(f"sampleRate={sample_rate} channels={channels} onsetFrame={onset}")
    if channel_order:
        print("octaviaChannels=" + ",".join(channel_order))

    window_rms: dict[str, list[float]] = {}
    for name, start_seconds, end_seconds in WINDOWS:
        start = min(frames, onset + round(start_seconds * sample_rate))
        end = min(frames, onset + round(end_seconds * sample_rate))
        values = [rms(samples, channels, channel, start, end) for channel in range(channels)]
        window_rms[name] = values
        rendered = " ".join(
            f"{label}={value:.6f}V" for label, value in zip(labels, values)
        )
        print(f"RMS {name:>10} {rendered}")

    total_end = min(frames, onset + round(4.0 * sample_rate))
    peaks = [peak(samples, channels, channel, onset, total_end) for channel in range(channels)]
    print("PEAK total-4s " + " ".join(
        f"{label}={value:.6f}V" for label, value in zip(labels, peaks)
    ))

    total = window_rms["total-4s"]
    comparisons = ((0, 3, "10V_vs_5V"), (2, 3, "10V_vs_9V"))
    passed = True
    for reference, target, name in comparisons:
        rms_delta = db_ratio(total[target], total[reference])
        peak_delta = db_ratio(peaks[target], peaks[reference])
        window_deltas = {
            window: db_ratio(values[target], values[reference])
            for window, values in window_rms.items()
            if window != "total-4s"
        }
        worst_window, worst_delta = min(window_deltas.items(), key=lambda item: item[1])
        comparison_passed = (
            rms_delta >= -0.10 and peak_delta >= -0.10 and worst_delta >= -0.10
        )
        passed = passed and comparison_passed
        print(
            f"COMPARE {name} rmsDelta={rms_delta:+.3f}dB "
            f"peakDelta={peak_delta:+.3f}dB "
            f"worstWindow={worst_window}:{worst_delta:+.3f}dB "
            f"result={'PASS' if comparison_passed else 'FAIL'}"
        )

    print(f"RESULT {'PASS' if passed else 'FAIL'}")
    return 0 if passed else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"analyze_doorstop_velocity_capture: {error}", file=sys.stderr)
        raise SystemExit(2)
