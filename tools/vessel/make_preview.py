#!/usr/bin/env python3
"""Offline bandlimited preview of Vessel's internal-rate float stereo WAV.

Requires NumPy. This tool is for listening fixtures, not the future Rack
resampler. Optional peak normalization is applied only to the saved preview.
"""
import argparse
import json
import struct
from pathlib import Path

import numpy as np


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--output-rate', type=int, default=48000)
    parser.add_argument('--normalize', action='store_true')
    parser.add_argument('--report', type=Path)
    args = parser.parse_args()
    data = args.source.read_bytes()
    if data[:4] != b'RIFF' or data[8:16] != b'WAVEfmt ' or data[36:40] != b'data':
        raise ValueError('expected WAV written by tools/vessel/render.cpp')
    size, fmt, channels, rate, byte_rate, align, bits = struct.unpack_from('<IHHIIHH', data, 16)
    count = struct.unpack_from('<I', data, 40)[0]
    if (size, fmt, channels, align, bits) != (16, 3, 2, 8, 32) or len(data) != count+44:
        raise ValueError('expected complete float32 stereo WAV')
    if byte_rate != rate*8 or args.output_rate < 32000 or rate % args.output_rate:
        raise ValueError('preview requires an integer input/output rate ratio and output >= 32 kHz')
    audio = np.frombuffer(data, dtype='<f4', offset=44).reshape(-1, 2).astype(np.float64)
    if not np.isfinite(audio).all():
        raise ValueError('source contains nonfinite audio')
    factor = rate // args.output_rate
    if factor < 2:
        raise ValueError('source should be an oversampled mechanical render')
    taps = 256*factor+1
    center = (taps-1)//2
    sample_index = np.arange(taps)-center
    cutoff = .44*args.output_rate/rate
    kernel = 2*cutoff*np.sinc(2*cutoff*sample_index)*np.blackman(taps)
    kernel /= kernel.sum()
    # Measure this saved offline filter rather than inferring rejection from a
    # tap count. This does not measure aliasing created inside the contact loop.
    response = np.abs(np.fft.rfft(kernel, 262144))
    frequencies = np.fft.rfftfreq(262144, 1/rate)
    stopband = float(20*np.log10(np.max(response[frequencies >= args.output_rate/2])))
    passband = response[frequencies <= .40*args.output_rate]
    passband_span = float(20*np.log10(np.max(passband)/np.min(passband)))
    if stopband > -70:
        raise ValueError(f'offline preview filter rejection too low: {stopband:.2f} dB')
    nfft = 1 << (len(audio)+taps-2).bit_length()
    transformed = np.fft.rfft(audio, n=nfft, axis=0)
    transformed *= np.fft.rfft(kernel, n=nfft)[:, None]
    filtered = np.fft.irfft(transformed, n=nfft, axis=0)
    # Offline removal of linear-phase delay uses future samples; the production
    # adapter must instead expose/handle its actual causal latency.
    output = filtered[center:center+len(audio):factor]
    peak = float(np.max(np.abs(output)))
    gain = .8/peak if args.normalize and peak > 0 else 1.0
    output = (output*gain).astype('<f4')
    encoded = output.tobytes()
    header = b'RIFF'+struct.pack('<I', 36+len(encoded))+b'WAVEfmt '
    header += struct.pack('<IHHIIHH', 16, 3, 2, args.output_rate, args.output_rate*8, 8, 32)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(header+b'data'+struct.pack('<I', len(encoded))+encoded)
    result = {'scope': 'offline listening preview; normalization does not affect mechanics',
              'input_rate_hz': rate, 'output_rate_hz': args.output_rate, 'filter_taps': taps,
              'offline_delay_removed_seconds': center/rate,
              'measured_filter_stopband_dB': stopband,
              'measured_filter_passband_span_dB_to_0_40_output_rate': passband_span,
              'raw_filtered_peak_m_per_s': peak, 'preview_gain': gain,
              'preview_peak': float(np.max(np.abs(output)))}
    text = json.dumps(result, indent=2, allow_nan=False)+'\n'
    if args.report:
        args.report.write_text(text, encoding='utf-8')
    print(text, end='')


if __name__ == '__main__':
    main()
