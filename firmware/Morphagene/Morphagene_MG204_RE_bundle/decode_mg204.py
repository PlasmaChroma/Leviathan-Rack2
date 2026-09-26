#!/usr/bin/env python3
"""Decode a Make Noise Morphagene MG204 audio updater.

The updater uses one 6 kHz carrier cycle per symbol at 48 kHz. Each cycle's
phase encodes a dibit. The byte stream contains 256-byte records protected by
big-endian CRC-32 values and separated by sync/padding. Removing that transport
layer yields a flat Cortex-M application image based at 0x08020000.
"""

from __future__ import annotations

import argparse
import hashlib
import struct
import wave
import zlib
from pathlib import Path

import numpy as np


SYNC = bytes.fromhex("99999999cccccccc")
PHASE_TO_DIBIT = {45: 0b10, 135: 0b00, 225: 0b01, 315: 0b11}


def deframe_records(framed: bytes) -> tuple[bytes, int]:
    """Strip recurring sync/padding and verify each 256-byte record CRC.

    Each non-final record has eight zero bytes before the sync word. Every
    fourth record has fifteen additional zero bytes (23 total).
    """
    records: list[bytes] = []
    cursor = 0
    index = 0
    while cursor < len(framed):
        record = framed[cursor : cursor + 256]
        if len(record) != 256:
            raise ValueError(f"truncated record {index}")
        if cursor + 260 > len(framed):
            raise ValueError(f"missing CRC for record {index}")
        stored_crc = struct.unpack_from(">I", framed, cursor + 256)[0]
        actual_crc = zlib.crc32(record) & 0xFFFF_FFFF
        if stored_crc != actual_crc:
            raise ValueError(
                f"record {index} CRC mismatch: stored {stored_crc:08x}, "
                f"computed {actual_crc:08x}"
            )
        records.append(record)
        cursor += 260
        if cursor == len(framed):
            break
        padding_length = 23 if index % 4 == 3 else 8
        padding = framed[cursor : cursor + padding_length]
        if padding != bytes(padding_length):
            raise ValueError(f"unexpected padding after record {index}")
        cursor += padding_length
        if framed[cursor : cursor + len(SYNC)] != SYNC:
            raise ValueError(f"missing sync marker after record {index}")
        cursor += len(SYNC)
        index += 1
    return b"".join(records), len(records)


def decode(path: Path) -> tuple[bytes, bytes, bytes, dict[str, int | str]]:
    with wave.open(str(path), "rb") as wav:
        if (wav.getnchannels(), wav.getsampwidth(), wav.getframerate()) != (1, 2, 48_000):
            raise ValueError("expected mono, 16-bit, 48 kHz PCM")
        samples = np.frombuffer(wav.readframes(wav.getnframes()), dtype="<i2").astype(float)

    nonzero = np.flatnonzero(samples)
    if not len(nonzero):
        raise ValueError("WAV contains only silence")
    carrier_start = int(nonzero[0])
    if carrier_start % 8:
        raise ValueError(f"carrier begins off the 8-sample symbol grid: {carrier_start}")

    carrier = samples[carrier_start:]
    if len(carrier) % 8:
        raise ValueError("carrier length is not a whole number of symbols")

    blocks = carrier.reshape(-1, 8)
    basis = np.exp(-2j * np.pi * np.arange(8) / 8)
    phases = (np.angle(blocks @ basis, deg=True) + 360.0) % 360.0
    quantized = ((np.rint((phases - 45.0) / 90.0).astype(int) % 4) * 90 + 45)
    phase_error = np.abs(((phases - quantized + 180.0) % 360.0) - 180.0)
    if float(phase_error.max()) > 1.0:
        raise ValueError(f"unexpected carrier phase; maximum quantization error {phase_error.max():.3f} degrees")

    dibits = np.fromiter((PHASE_TO_DIBIT[int(p)] for p in quantized), dtype=np.uint8)
    bits = np.empty(len(dibits) * 2, dtype=np.uint8)
    bits[0::2] = dibits >> 1
    bits[1::2] = dibits & 1
    raw = np.packbits(bits, bitorder="big").tobytes()

    sync_offset = raw.find(SYNC)
    if sync_offset < 0:
        raise ValueError("sync marker not found")
    payload_start = sync_offset + len(SYNC)

    trailer_length = len(raw) - len(raw.rstrip(b"\0"))
    payload_end = len(raw) - trailer_length
    framed = raw[payload_start:payload_end]
    if len(framed) < 8:
        raise ValueError("decoded payload is too short")

    flash, record_count = deframe_records(framed)
    stack_pointer, reset_handler = struct.unpack_from("<II", flash)
    if not (0x2000_0000 <= stack_pointer < 0x3000_0000):
        raise ValueError(f"implausible Cortex-M initial SP: 0x{stack_pointer:08x}")
    if not (0x0800_0001 <= reset_handler < 0x0820_0000 and reset_handler & 1):
        raise ValueError(f"implausible Thumb reset handler: 0x{reset_handler:08x}")

    metadata: dict[str, int | str] = {
        "carrier_start_sample": carrier_start,
        "symbol_count": len(dibits),
        "raw_byte_count": len(raw),
        "sync_offset": sync_offset,
        "trailer_length": trailer_length,
        "framed_byte_count": len(framed),
        "record_count": record_count,
        "flash_byte_count": len(flash),
        "flash_base_address": "0x08020000",
        "initial_stack_pointer": f"0x{stack_pointer:08x}",
        "reset_handler": f"0x{reset_handler:08x}",
        "flash_sha256": hashlib.sha256(flash).hexdigest(),
    }
    return raw, framed, flash, metadata


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("wav", type=Path)
    parser.add_argument("--raw", type=Path, default=Path("mg204_qpsk_decoded.bin"))
    parser.add_argument("--framed", type=Path, default=Path("mg204_framed_records.bin"))
    parser.add_argument("--flash", type=Path, default=Path("mg204_flash_08020000.bin"))
    args = parser.parse_args()

    raw, framed, flash, metadata = decode(args.wav)
    args.raw.write_bytes(raw)
    args.framed.write_bytes(framed)
    args.flash.write_bytes(flash)
    for key, value in metadata.items():
        print(f"{key}: {value}")


if __name__ == "__main__":
    main()
