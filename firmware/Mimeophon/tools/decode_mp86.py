#!/usr/bin/env python3
"""Recover the clean 48 kHz / 6 kHz QPSK transport in the supplied MP86 WAV.

This is a forensic decoder, NOT a hardware updater. No serial/USB/network access.
Requires Python 3.10+ and numpy. It is deliberately not a noisy-recording modem:
resampled audio, unknown carrier phase and timing drift are not corrected.

python tools/decode_mp86.py input/mp86.wav --out /tmp/mp86-decoded
"""
from __future__ import annotations
import argparse
import csv
import hashlib
import json
from pathlib import Path
import struct
import sys
import wave
import zlib
import numpy as np

PREAMBLE = bytes(8) + b'\x99' * 4 + b'\xcc' * 4
PAYLOAD_BYTES = 256
PACKET_BYTES = len(PREAMBLE) + PAYLOAD_BYTES + 4

def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def decode(path: Path, expected_packets: int = 192) -> tuple[bytes, bytes, list[dict], dict]:
    source = path.read_bytes()
    with wave.open(str(path), 'rb') as w:
        if (w.getnchannels(), w.getsampwidth(), w.getframerate(), w.getcomptype()) != (1, 2, 48000, 'NONE'):
            raise ValueError('Expected uncompressed mono 16-bit PCM at 48000 Hz; do not resample the source.')
        frames = w.getnframes()
        pcm = np.frombuffer(w.readframes(frames), dtype='<i2')
    active = np.flatnonzero(pcm)
    if not len(active):
        raise ValueError('WAV contains only silence.')
    origin = int(active[0])
    if (len(pcm) - origin) % 32:
        raise ValueError('Active sample count is not an integral QPSK byte count.')
    groups = pcm[origin:].reshape(-1, 8).astype(np.float64)
    # For x = I*cos(theta) + Q*sin(theta), the complex DFT is proportional to I-jQ.
    spectrum = groups @ np.exp(-2j * np.pi * np.arange(8) / 8)
    symbols = (2 * (spectrum.real > 0) + (spectrum.imag < 0)).astype(np.uint8)
    # Independent time-domain classifier, valid at this exact clean symbol alignment.
    direct = (2 * (groups[:, 0] > 0) + (groups[:, 2] > 0)).astype(np.uint8)
    mismatch = int(np.count_nonzero(symbols != direct))
    if mismatch:
        raise ValueError(f'DFT and two-sample classifiers disagree on {mismatch} symbols.')
    stream = (symbols.reshape(-1, 4).astype(np.uint16) @ np.array([64, 16, 4, 1], dtype=np.uint16)).astype(np.uint8).tobytes()
    packets, payloads = [], []
    cursor = 0
    while True:
        start = stream.find(PREAMBLE, cursor)
        if start < 0:
            break
        end = start + PACKET_BYTES
        if end > len(stream):
            raise ValueError(f'Truncated packet at demodulated byte {start}.')
        payload = stream[start + 16:start + 272]
        stored = int.from_bytes(stream[start + 272:end], 'big')
        computed = zlib.crc32(payload) & 0xffffffff
        if stored != computed:
            raise ValueError(f'Packet {len(packets)} CRC mismatch: stored {stored:08x}, computed {computed:08x}.')
        index = len(packets)
        sample = origin + start * 32
        packets.append(dict(packet_index=index, payload_offset=index * 256,
                            flash_address=f'0x{0x08020000 + index * 256:08x}',
                            demodulated_byte_offset=start, wav_sample_offset=sample,
                            wav_time_seconds=sample / 48000,
                            stored_crc32=f'{stored:08x}', computed_crc32=f'{computed:08x}',
                            crc_pass=True, payload_sha256=sha256(payload)))
        payloads.append(payload)
        cursor = end
    if not packets or (expected_packets and len(packets) != expected_packets):
        raise ValueError(f'Expected {expected_packets or "at least one"} packets, found {len(packets)}.')
    # Any nonpacket symbols must be the documented symbol-0 lead/trailer/page pauses.
    cursor, gaps = 0, []
    for p in packets:
        start = p['demodulated_byte_offset']
        gap = stream[cursor:start]
        if any(gap):
            raise ValueError(f'Unexplained nonzero framing bytes at {cursor}:{start}.')
        gaps.append(len(gap))
        cursor = start + PACKET_BYTES
    if any(stream[cursor:]):
        raise ValueError('Unexpected nonzero trailer.')
    firmware = b''.join(payloads)
    metadata = dict(
        input_filename=path.name, input_sha256=sha256(source), input_bytes=len(source),
        wav=dict(format='PCM signed 16-bit little endian', channels=1, sample_rate_hz=48000,
                 sample_count=frames, duration_seconds=frames / 48000, leading_silence_samples=origin),
        transport=dict(modulation='QPSK', differential=False, carrier_hz=6000, samples_per_symbol=8,
                       symbol_rate_hz=6000, raw_bit_rate_bps=12000, raw_byte_rate_Bps=1500,
                       symbols=len(symbols), demodulated_bytes=len(stream),
                       preamble_hex=PREAMBLE.hex(), payload_bytes=256, crc_bytes=4,
                       crc='zlib.crc32(payload), unsigned IEEE CRC-32, big-endian stored word',
                       bit_packing='four 2-bit absolute-phase symbols per byte, most significant first',
                       packet_count=len(packets), crc_pass_count=len(packets),
                       dft_vs_time_domain_symbol_mismatches=mismatch,
                       gap_before_packets_bytes=gaps, trailer_bytes=len(stream) - cursor,
                       scrambling_observed=False, decompression_applied=False),
        firmware=dict(bytes=len(firmware), sha256=sha256(firmware),
                      base_address='0x08020000', initial_sp=f'0x{struct.unpack_from("<I", firmware)[0]:08x}',
                      reset_vector=f'0x{struct.unpack_from("<I", firmware, 4)[0]:08x}'),
        scope='Exact supplied clean digital WAV; no hardware execution or firmware installation.'
    )
    return firmware, stream, packets, metadata

def write_outputs(out: Path, firmware: bytes, stream: bytes, packets: list[dict], metadata: dict) -> None:
    for sub in ('binaries', 'protocol'):
        (out / sub).mkdir(parents=True, exist_ok=True)
    (out / 'binaries/mimeophon_mp86.bin').write_bytes(firmware)
    (out / 'protocol/demodulated_stream.bin').write_bytes(stream)
    (out / 'protocol/transport.json').write_text(json.dumps(metadata, indent=2) + '\n', encoding='utf-8')
    with (out / 'protocol/packets.csv').open('w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=list(packets[0]))
        writer.writeheader()
        writer.writerows(packets)

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('wav', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--expected-packets', type=int, default=192,
                        help='Required packet count; 0 permits any nonzero count (default 192).')
    args = parser.parse_args()
    try:
        result = decode(args.wav, args.expected_packets)
        write_outputs(args.out, *result)
    except (OSError, ValueError, wave.Error) as exc:
        print(f'Decode failed: {exc}', file=sys.stderr)
        return 1
    print(json.dumps(result[-1]['firmware'], indent=2))
    print(f'CRC PASS: {len(result[2])}/{len(result[2])}; independent demodulation agrees.')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
