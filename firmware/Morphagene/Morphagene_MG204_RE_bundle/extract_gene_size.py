#!/usr/bin/env python3
"""Extract and characterize MG204's Gene Size lookup table."""

from __future__ import annotations

import argparse
import csv
import math
import struct
from pathlib import Path


FLASH_BASE = 0x08020000
TABLE_ADDRESS = 0x08044D10
TABLE_LENGTH = 1024


def load_table(flash_path: Path) -> tuple[float, ...]:
    flash = flash_path.read_bytes()
    offset = TABLE_ADDRESS - FLASH_BASE
    table = struct.unpack_from(f"<{TABLE_LENGTH}f", flash, offset)
    for index, actual in enumerate(table):
        expected = 2.0 ** (index / 341.0 - 3.0)
        if not math.isclose(actual, expected, rel_tol=6e-8, abs_tol=3e-8):
            raise ValueError(
                f"table[{index}]={actual!r} does not match the inferred curve "
                f"{expected!r}"
            )
    return table


def folded_span(splice_samples: int) -> float:
    span = float(splice_samples)
    while span > 576_000.0:
        span *= 0.5
    return span


def gene_samples(adc: int, splice_samples: int, table: tuple[float, ...]) -> float | None:
    if not 0 <= adc <= 4095:
        raise ValueError("ADC must be in the 12-bit range 0..4095")
    if adc <= 199:
        return None  # whole-splice endpoint mode
    index = 1073 - (adc >> 2)
    value = folded_span(splice_samples) * table[index] ** 3
    return max(value, 8.0)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("flash", type=Path)
    parser.add_argument("--csv", type=Path, default=Path("gene_size_curve.csv"))
    parser.add_argument("--splice-seconds", type=float, default=10.0)
    args = parser.parse_args()

    table = load_table(args.flash)
    splice_samples = round(args.splice_seconds * 48_000)
    with args.csv.open("w", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            ["adc", "lut_index", "lut_value", "ratio", "gene_samples", "gene_ms", "mode"]
        )
        for adc in range(4096):
            if adc <= 199:
                writer.writerow([adc, "", "", "", "", "", "whole_splice"])
                continue
            index = 1073 - (adc >> 2)
            ratio = table[index] ** 3
            samples = gene_samples(adc, splice_samples, table)
            writer.writerow(
                [adc, index, f"{table[index]:.9g}", f"{ratio:.9g}", f"{samples:.9g}", f"{samples / 48:.9g}", "gene"]
            )

    print(f"verified LUT: {TABLE_LENGTH} floats at 0x{TABLE_ADDRESS:08x}")
    print("closed form: LUT[i] = float32(2 ** (i/341 - 3))")
    print(f"wrote {args.csv} for a {args.splice_seconds:g} s splice")


if __name__ == "__main__":
    main()
