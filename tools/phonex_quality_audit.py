#!/usr/bin/env python3
"""Render and measure deterministic PHONEX quality artifacts."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import re
import shutil
import subprocess
import sys
import wave
from array import array
from dataclasses import dataclass
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT_ROOT = REPO_ROOT / "build" / "phonex-quality"
FIXED_SEED = 109793
FFT_SIZE = 2048
MAX_WINDOWS = 32

VOWELS = ("AA", "AE", "AH", "AO", "AW", "AY", "EH", "ER", "EY",
          "IH", "IY", "OW", "OY", "UH", "UW")
CONSONANTS = ("B", "CH", "D", "DH", "F", "G", "HH", "JH", "K", "L",
              "M", "N", "NG", "P", "R", "S", "SH", "T", "TH", "V",
              "W", "Y", "Z", "ZH")


@dataclass(frozen=True)
class RenderCase:
    key: str
    group: str
    label: str
    args: tuple[str, ...]


def slug(text: str) -> str:
    value = re.sub(r"[^a-z0-9]+", "-", text.lower()).strip("-")
    return value or "unnamed"


def load_bank() -> list[tuple[int, str, str]]:
    bank: list[tuple[int, str, str]] = []
    path = REPO_ROOT / "tools" / "phonex_rom" / "bundled_phrases.tsv"
    with path.open(encoding="utf-8") as handle:
        for raw in handle:
            if not raw.strip() or raw.startswith("#"):
                continue
            index, name, phones = raw.rstrip("\n").split("\t")
            bank.append((int(index), name, phones))
    if [item[0] for item in bank] != list(range(64)):
        raise RuntimeError("bundled phrase bank must contain contiguous indices 0..63")
    return bank


def build_cases() -> list[RenderCase]:
    bank = load_bank()
    cases: list[RenderCase] = []
    for index, name, _phones in bank:
        cases.append(RenderCase(
            f"bank-{index:02d}-{slug(name)}", "bank", name, (str(index),)))

    core = (("hello", "HELLO", ("36",)),
            ("speak", "SPEAK", ("38",)),
            ("robot", "ROBOT", ("47",)),
            ("computer", "COMPUTER", ("45",)),
            ("one-two-three", "ONE TWO THREE", ("--text", "ONE TWO THREE")),
            ("leviathan", "LEVIATHAN", ("62",)),
            ("phonex", "PHONEX", ("63",)))
    for key, label, args in core:
        cases.append(RenderCase(f"core-{key}", "core", label, args))

    for phone in VOWELS:
        script = f"B {phone}1 T"
        cases.append(RenderCase(f"vowel-{phone.lower()}", "vowels", script,
                                ("--phones", script)))
    for phone in CONSONANTS:
        script = f"AA1 {phone} AA"
        cases.append(RenderCase(f"consonant-{phone.lower()}", "consonants", script,
                                ("--phones", script)))
    for phone in (*VOWELS, *CONSONANTS):
        script = f"{phone}1 SIL" if phone in VOWELS else f"{phone} SIL"
        cases.append(RenderCase(f"isolated-{phone.lower()}", "isolated", script,
                                ("--phones", script)))

    contrasts = {
        "fricative-s": "AA1 S AA",
        "fricative-sh": "AA1 SH AA",
        "fricative-f": "AA1 F AA",
        "fricative-th": "AA1 TH AA",
        "stop-p": "AA1 P AA", "stop-t": "AA1 T AA", "stop-k": "AA1 K AA",
        "stop-b": "AA1 B AA", "stop-d": "AA1 D AA", "stop-g": "AA1 G AA",
        "nasal-m": "AA1 M AA", "nasal-n": "AA1 N AA", "nasal-ng": "AA1 NG AA",
        "liquid-l": "AA1 L AA", "liquid-r": "AA1 R AA",
        "glide-w": "AA1 W AA", "glide-y": "AA1 Y AA",
    }
    for key, script in contrasts.items():
        cases.append(RenderCase(f"contrast-{key}", "contrasts", script,
                                ("--phones", script)))

    for index, text in enumerate(("HELLO COMPUTER", "SPEAK AGAIN", "READY TO START",
                                  "VOLTAGE SIGNAL")):
        cases.append(RenderCase(f"typed-{index:02d}-{slug(text)}", "typed", text,
                                ("--text", text)))
    for index, text in enumerate(("MAKE", "YELLOW", "THOSE", "PLAYED", "SYNTHESIZER")):
        cases.append(RenderCase(f"g2p-{index:02d}-{slug(text)}", "g2p", text,
                                ("--text", text)))

    probes = ("zero-voiced", "zero-unvoiced", "level-sweep",
              "forced-voiced-unvoiced")
    for probe in probes:
        cases.append(RenderCase(f"probe-{probe}-filtered", "probes", probe,
                                ("--probe", probe)))
    for probe in ("zero-voiced", "zero-unvoiced"):
        cases.append(RenderCase(f"probe-{probe}-raw", "probes", f"{probe} raw",
                                ("--probe", probe, "--raw")))
    for frequency in (500, 1000, 2000, 3000, 3800):
        arguments = ("--probe", "reconstruction-sine", "--probe-frequency", str(frequency))
        cases.append(RenderCase(f"reconstruction-{frequency}-filtered", "reconstruction",
                                f"{frequency} Hz filtered", arguments))
        cases.append(RenderCase(f"reconstruction-{frequency}-raw", "reconstruction",
                                f"{frequency} Hz raw", (*arguments, "--raw")))

    variants = (("hello-8k", "HELLO internal 8 kHz", ("36", "--internal-rate", "8000")),
                ("hello-96k", "HELLO host 96 kHz", ("36", "--sample-rate", "96000")),
                ("hello-192k", "HELLO host 192 kHz", ("36", "--sample-rate", "192000")),
                ("hello-raw", "HELLO raw hold", ("36", "--raw")),
                ("hello-forced-unvoiced", "HELLO forced unvoiced",
                 ("36", "--forced-unvoiced", "--excite-blend", "1")))
    for key, label, args in variants:
        cases.append(RenderCase(f"variant-{key}", "variants", label, args))
    return cases


def parse_renderer_output(output: str) -> dict[str, str]:
    line = next((line for line in output.splitlines()
                 if line.startswith("PHONEX_RENDER\t")), "")
    if not line:
        raise RuntimeError(f"renderer did not emit metadata: {output!r}")
    result: dict[str, str] = {}
    for field in line.split("\t")[1:]:
        key, separator, value = field.partition("=")
        if separator:
            result[key] = value
    required = {"frames", "silence_frames", "unvoiced_frames", "voiced_frames",
                "max_abs_k", "sample_rate", "internal_rate", "reconstruction",
                "forced_excitation", "excite_blend", "output_stage", "filter_order"}
    if not required.issubset(result):
        raise RuntimeError(f"renderer metadata missing {sorted(required - result.keys())}")
    return result


def render(renderer: Path, case: RenderCase, output: Path,
           renderer_args: list[str]) -> dict[str, str]:
    command = [str(renderer), str(output), *case.args, *renderer_args,
               "--seed", str(FIXED_SEED)]
    completed = subprocess.run(command, cwd=REPO_ROOT, check=True, text=True,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return parse_renderer_output(completed.stdout)


def fft(values: list[complex]) -> None:
    count = len(values)
    j = 0
    for i in range(1, count):
        bit = count >> 1
        while j & bit:
            j ^= bit
            bit >>= 1
        j ^= bit
        if i < j:
            values[i], values[j] = values[j], values[i]
    length = 2
    while length <= count:
        angle = -2.0 * math.pi / length
        root = complex(math.cos(angle), math.sin(angle))
        half = length // 2
        for start in range(0, count, length):
            factor = 1.0 + 0.0j
            for offset in range(half):
                even = values[start + offset]
                odd = values[start + offset + half] * factor
                values[start + offset] = even + odd
                values[start + offset + half] = even - odd
                factor *= root
        length *= 2


def spectrum_metrics(samples: list[float], sample_rate: int) -> dict[str, float]:
    active = [index for index, value in enumerate(samples) if abs(value) > 1.0 / 32768.0]
    if not active:
        return {"centroid_hz": 0.0, "band_0_1k": 0.0, "band_1_3k": 0.0,
                "band_3_6k": 0.0, "band_6k_nyquist": 0.0}
    signal = samples[active[0]:active[-1] + 1]
    if len(signal) <= FFT_SIZE:
        starts = [0]
    else:
        available = len(signal) - FFT_SIZE
        windows = min(MAX_WINDOWS, 1 + available // (FFT_SIZE // 2))
        starts = [round(i * available / max(1, windows - 1)) for i in range(windows)]
    powers = [0.0] * (FFT_SIZE // 2 + 1)
    window = [0.5 - 0.5 * math.cos(2.0 * math.pi * i / (FFT_SIZE - 1))
              for i in range(FFT_SIZE)]
    for start in starts:
        block = signal[start:start + FFT_SIZE]
        if len(block) < FFT_SIZE:
            block = block + [0.0] * (FFT_SIZE - len(block))
        values = [complex(block[i] * window[i], 0.0) for i in range(FFT_SIZE)]
        fft(values)
        for index in range(len(powers)):
            powers[index] += values[index].real ** 2 + values[index].imag ** 2
    total = sum(powers) or 1.0
    bin_hz = sample_rate / FFT_SIZE
    centroid = sum(index * bin_hz * power for index, power in enumerate(powers)) / total
    def band(low: float, high: float) -> float:
        return sum(power for index, power in enumerate(powers)
                   if low <= index * bin_hz < high) / total
    nyquist = sample_rate * 0.5 + bin_hz
    result = {"centroid_hz": centroid,
            "band_0_1k": band(0.0, 1000.0),
            "band_1_3k": band(1000.0, 3000.0),
            "band_3_6k": band(3000.0, 6000.0),
            "band_6k_nyquist": band(6000.0, nyquist)}
    for frequency in (500, 1000, 2000, 3000, 3800):
        tone = sum(power for index, power in enumerate(powers)
                   if abs(index * bin_hz - frequency) <= 100.0)
        image = sum(power for index, power in enumerate(powers)
                    if abs(index * bin_hz - (10000.0 - frequency)) <= 100.0
                    or abs(index * bin_hz - (10000.0 + frequency)) <= 100.0)
        result[f"tone_power_{frequency}"] = tone
        result[f"image_power_{frequency}"] = image
    return result


def measure(path: Path) -> dict[str, str]:
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    with wave.open(str(path), "rb") as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() != 2:
            raise RuntimeError(f"expected mono 16-bit PCM: {path}")
        sample_rate = wav.getframerate()
        raw = wav.readframes(wav.getnframes())
    pcm = array("h")
    pcm.frombytes(raw)
    if sys.byteorder != "little":
        pcm.byteswap()
    samples = [value / 32768.0 for value in pcm]
    count = len(samples)
    peak = max((abs(value) for value in samples), default=0.0)
    rms = math.sqrt(sum(value * value for value in samples) / max(1, count))
    dc = sum(samples) / max(1, count)
    differences = sorted(abs(samples[i] - samples[i - 1]) for i in range(1, count))
    p999 = differences[min(len(differences) - 1, int(0.999 * len(differences)))] if differences else 0.0
    result = {"sha256": digest, "sample_count": str(count),
              "duration_seconds": format(count / sample_rate, ".9g"),
              "peak": format(peak, ".9g"), "peak_dbfs": format(20.0 * math.log10(max(peak, 1e-12)), ".9g"),
              "rms": format(rms, ".9g"), "rms_dbfs": format(20.0 * math.log10(max(rms, 1e-12)), ".9g"),
              "dc": format(dc, ".9g"), "clipped_samples": str(sum(abs(value) >= 32767 / 32768 for value in samples)),
              "max_step": format(max(differences, default=0.0), ".9g"),
              "p999_step": format(p999, ".9g"),
              "limiter_region_samples": str(sum(abs(value) > 0.9 for value in samples)),
              "limiter_region_duty": format(
                  sum(abs(value) > 0.9 for value in samples) / max(1, count), ".9g")}
    result.update({key: format(value, ".9g")
                   for key, value in spectrum_metrics(samples, sample_rate).items()})
    return result


def measure_level_sweep(path: Path) -> tuple[list[dict[str, str]], float]:
    with wave.open(str(path), "rb") as wav:
        sample_rate = wav.getframerate()
        pcm = array("h")
        pcm.frombytes(wav.readframes(wav.getnframes()))
    if sys.byteorder != "little":
        pcm.byteswap()
    samples = [value / 32768.0 for value in pcm]
    rows: list[dict[str, str]] = []
    rms_values: list[float] = []
    for level in range(1, 11):
        segment_start = int(((level - 1) * 0.12 + 0.04) * sample_rate)
        segment_end = int(level * 0.12 * sample_rate)
        segment = samples[segment_start:segment_end]
        rms = math.sqrt(sum(value * value for value in segment) / max(1, len(segment)))
        rms_values.append(rms)
        rows.append({"encoded_energy": format(level * 0.1, ".9g"),
                     "output_rms": format(rms, ".9g")})
    energies = [level * 0.1 for level in range(1, 11)]
    energy_mean = sum(energies) / len(energies)
    rms_mean = sum(rms_values) / len(rms_values)
    numerator = sum((energy - energy_mean) * (rms - rms_mean)
                    for energy, rms in zip(energies, rms_values))
    denominator = math.sqrt(sum((energy - energy_mean) ** 2 for energy in energies)
                            * sum((rms - rms_mean) ** 2 for rms in rms_values))
    return rows, numerator / denominator if denominator > 0.0 else 0.0


def write_tsv(path: Path, rows: list[dict[str, str]], columns: list[str]) -> None:
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=columns, delimiter="\t",
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--renderer", type=Path,
                        default=REPO_ROOT / "build" / "tools" / "phonex_render")
    parser.add_argument("--corpus-report", type=Path,
                        default=REPO_ROOT / "build" / "tools" / "phonex_corpus_report")
    parser.add_argument("--tag", required=True,
                        help="immutable output tag under build/phonex-quality")
    parser.add_argument("--output-root", type=Path, default=DEFAULT_OUTPUT_ROOT)
    parser.add_argument("--verify-determinism", action="store_true")
    parser.add_argument("--renderer-arg", action="append", default=[],
                        help="argument appended to every renderer invocation")
    parser.add_argument("--group", action="append", default=[],
                        help="render only the named group; repeat as needed")
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]*", args.tag):
        parser.error("tag must contain only letters, digits, dot, underscore, and dash")
    renderer = args.renderer.resolve()
    if not renderer.is_file():
        parser.error(f"renderer not found: {renderer}")
    corpus_report = args.corpus_report.resolve()
    if not corpus_report.is_file():
        parser.error(f"corpus report tool not found: {corpus_report}")
    output_root = args.output_root.resolve()
    final_dir = output_root / args.tag
    staging_dir = output_root / f".{args.tag}.staging"
    if final_dir.exists() or staging_dir.exists():
        parser.error(f"tag already exists or has a partial staging directory: {args.tag}")
    staging_dir.mkdir(parents=True)

    cases = build_cases()
    if args.group:
        cases = [case for case in cases if case.group in set(args.group)]
        if not cases:
            parser.error("group filter selected no render cases")
    manifest_rows: list[dict[str, str]] = []
    metric_rows: list[dict[str, str]] = []
    try:
        report = subprocess.run([str(corpus_report)], cwd=REPO_ROOT, check=True,
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        (staging_dir / "post-tms-frames.tsv").write_bytes(report.stdout)
        for number, case in enumerate(cases, 1):
            group_dir = staging_dir / "audio" / case.group
            group_dir.mkdir(parents=True, exist_ok=True)
            relative = Path("audio") / case.group / f"{case.key}.wav"
            output = staging_dir / relative
            metadata = render(renderer, case, output, args.renderer_arg)
            metrics = measure(output)
            if args.verify_determinism:
                verification = staging_dir / ".determinism.wav"
                render(renderer, case, verification, args.renderer_arg)
                if hashlib.sha256(verification.read_bytes()).hexdigest() != metrics["sha256"]:
                    raise RuntimeError(f"non-deterministic render: {case.key}")
                verification.unlink()
            manifest_rows.append({"key": case.key, "group": case.group,
                                  "label": case.label, "source_args": " ".join(case.args),
                                  **metadata, "path": relative.as_posix()})
            metric_rows.append({"key": case.key, "group": case.group,
                                "path": relative.as_posix(), **metrics})
            print(f"[{number:03d}/{len(cases):03d}] {case.key}")

        blind_dir = staging_dir / "blind" / "core"
        blind_dir.mkdir(parents=True)
        answers: list[dict[str, str]] = []
        for case in (case for case in cases if case.group == "core"):
            blind_id = hashlib.sha256(f"{FIXED_SEED}:{case.key}".encode()).hexdigest()[:12]
            source = staging_dir / "audio" / case.group / f"{case.key}.wav"
            destination = blind_dir / f"{blind_id}.wav"
            shutil.copyfile(source, destination)
            answers.append({"blind_id": blind_id, "key": case.key, "answer": case.label})

        manifest_columns = ["key", "group", "label", "source_args", "path", "name", "frames",
                            "silence_frames", "unvoiced_frames", "voiced_frames", "max_abs_k",
                            "sample_rate", "internal_rate", "reconstruction",
                            "forced_excitation", "excite_blend", "output_stage"]
        metric_columns = ["key", "group", "path", "sha256", "sample_count", "duration_seconds",
                          "peak", "peak_dbfs", "rms", "rms_dbfs", "dc", "clipped_samples",
                          "max_step", "p999_step", "limiter_region_samples",
                          "limiter_region_duty", "centroid_hz", "band_0_1k", "band_1_3k",
                          "band_3_6k", "band_6k_nyquist"]
        manifest_columns.append("filter_order")
        for frequency in (500, 1000, 2000, 3000, 3800):
            metric_columns.extend((f"tone_power_{frequency}", f"image_power_{frequency}"))
        write_tsv(staging_dir / "manifest.tsv", manifest_rows, manifest_columns)
        write_tsv(staging_dir / "metrics.tsv", metric_rows, metric_columns)
        sweep_path = staging_dir / "audio" / "probes" / "probe-level-sweep-filtered.wav"
        sweep_correlation = None
        if sweep_path.is_file():
            sweep_rows, sweep_correlation = measure_level_sweep(sweep_path)
            write_tsv(staging_dir / "level-sweep.tsv", sweep_rows,
                      ["encoded_energy", "output_rms"])
        write_tsv(staging_dir / "blind" / "core-answer-key.tsv", answers,
                  ["blind_id", "key", "answer"])
        summary = {"schema": 1, "tag": args.tag, "seed": FIXED_SEED,
                   "render_count": len(cases), "determinism_verified": args.verify_determinism,
                   "renderer_args": args.renderer_arg,
                   "level_sweep_energy_rms_correlation": sweep_correlation,
                   "groups": {group: sum(case.group == group for case in cases)
                              for group in sorted({case.group for case in cases})}}
        (staging_dir / "summary.json").write_text(
            json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        staging_dir.rename(final_dir)
    except Exception:
        print(f"Partial artifacts retained at {staging_dir}", file=sys.stderr)
        raise
    print(f"PHONEX quality audit complete: {final_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
