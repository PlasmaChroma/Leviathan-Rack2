#!/usr/bin/env python3
"""Compare fir_capture manifests. Requires NumPy/SciPy; writes JSON and CSV.

Alignment pads the shorter equivalent FIR at INTERNAL rate, exactly matching
group delay before decimation. No fractional host-sample interpolation is used.
Audio percentages are whole-record stereo L2 or peak errors, not sample ratios.
"""
import argparse
import csv
import json
from pathlib import Path

import numpy as np
from scipy.signal import fftconvolve, freqz, windows


def equivalent(h, factor):
    result = np.ones(1)
    stride = 1
    while stride < factor:
        expanded = np.zeros((len(h) - 1) * stride + 1)
        expanded[::stride] = h
        result = np.convolve(result, expanded)
        stride *= 2
    return result


def db(value):
    return float(20 * np.log10(max(float(value), 1e-300)))


def response(h):
    # FFT sampling is faster than direct evaluation of a million frequencies.
    z = np.fft.rfft(h, 2097152)
    grid = np.linspace(0, .5, len(z))
    # Include the exact design edges separately.
    edge_f = np.array([.2, .2045, .25])
    _, edge_z = freqz(h, worN=2*np.pi*edge_f)
    f = np.r_[grid, edge_f]
    amp = np.r_[np.abs(z), np.abs(edge_z)]
    p, s = amp[f <= .2], amp[f >= .25]
    return dict(dc=float(sum(h)), passband_span_db=db(max(p)/min(p)),
                worst_stopband_db=db(max(s)), mean_stopband_power_db=db(np.sqrt(np.mean(s*s))))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('reference', type=Path)
    parser.add_argument('candidate', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    h0, h1 = [np.loadtxt(p/'coefficients.txt') for p in (args.reference, args.candidate)]
    assert len(h0) == 129 and len(h1) == 109
    assert np.array_equal(h0, h0[::-1]) and np.array_equal(h1, h1[::-1])
    published = Path(__file__).resolve().parents[2]/'doc/Vessel/vessel_audit/equiripple109.csv'
    assert np.array_equal(h1, np.loadtxt(published, skiprows=1)), 'candidate coefficients drifted'
    summary = {'reference_response': response(h0), 'candidate_response': response(h1)}
    assert summary['candidate_response']['worst_stopband_db'] < -85
    assert summary['candidate_response']['passband_span_db'] < .01
    spectral = []
    for rate in (44100, 48000, 96000):
        for factor in (1, 2, 4, 8):
            a, b = equivalent(h0, factor), equivalent(h1, factor)
            pad = (len(a)-len(b))//2
            b = np.pad(b, (pad, pad))
            f = np.unique(np.r_[np.linspace(0, rate/2, 65537), 17640, 19200, 20000])
            f = f[f <= rate/2]
            _, za = freqz(a, worN=2*np.pi*f/(rate*factor))
            _, zb = freqz(b, worN=2*np.pi*f/(rate*factor))
            common = f <= .4*rate
            audible = f <= 20000
            row = dict(host_rate=rate, factor=factor,
                       delay_reduction_host_samples=pad/factor,
                       aligned_impulse_l1_difference=float(np.sum(np.abs(a-b))),
                       passband_max_relative_amplitude_percent=float(100*np.max(np.abs(zb[common]-za[common])/np.abs(za[common]))),
                       through_20k_max_absolute_gain_difference_percent=float(100*np.max(np.abs(zb[audible]-za[audible]))))
            for hz in (18000, 19000, 20000):
                _, va = freqz(a, worN=[2*np.pi*hz/(rate*factor)])
                _, vb = freqz(b, worN=[2*np.pi*hz/(rate*factor)])
                row[f'gain_{hz}_reference_db'] = db(abs(va[0]))
                row[f'gain_{hz}_candidate_db'] = db(abs(vb[0]))
            spectral.append(row)
    summary['spectral'] = spectral
    manifests = [list(csv.DictReader((p/'manifest.csv').open())) for p in (args.reference, args.candidate)]
    assert len(manifests[0]) == 288
    assert manifests[0] == manifests[1], 'mechanics/fixture manifests differ'
    rows = []
    max_rounding = 0.
    total_error = total_signal = 0.
    for meta in manifests[0]:
        ident, factor, frames = int(meta['id']), int(meta['factor']), int(meta['frames'])
        raw0, raw1 = [np.fromfile(p/f'{ident}.raw', dtype=np.float64).reshape(-1, 2)
                      for p in (args.reference, args.candidate)]
        assert raw0.shape == (frames*factor, 2) and np.array_equal(raw0, raw1), 'physical trajectory differs'
        assert np.isfinite(raw0).all()
        a, b = equivalent(h0, factor), equivalent(h1, factor)
        outputs = []
        # Independent full-convolution oracle for BOTH production streaming FIRs.
        for directory, coeff in ((args.reference, a), (args.candidate, b)):
            actual = np.fromfile(directory/f'{ident}.out', dtype=np.float64).reshape(-1, 2)
            expected = fftconvolve(raw0, coeff[:, None], axes=0)[factor-1:frames*factor:factor]
            scale = max(float(np.max(np.abs(raw0))), 1e-300)
            rounding = float(np.max(np.abs(actual-expected)))/scale
            max_rounding = max(max_rounding, rounding)
            assert actual.shape == (frames, 2) and np.isfinite(actual).all()
            assert rounding < 2e-13, 'production FIR differs from convolution oracle'
            outputs.append(actual)
        pad = (len(a)-len(b))//2
        b = np.pad(b, (pad, pad))
        aligned = fftconvolve(raw0, b[:, None], axes=0)[factor-1:frames*factor:factor]
        if factor == 1:
            assert np.array_equal(outputs[0], outputs[1]), 'bypass changed'
            aligned = outputs[1]
        diff = aligned-outputs[0]
        error, signal = float(np.sum(diff*diff)), float(np.sum(outputs[0]*outputs[0]))
        total_error += error
        total_signal += signal
        peak = float(np.max(np.abs(outputs[0])))
        row = {k: int(meta[k]) if k != 'pitch' else float(meta[k])
               for k in ('id', 'bowl', 'mallet', 'host_rate', 'quality', 'pitch', 'rub', 'factor')}
        row.update(rms_difference_percent=100*np.sqrt(error/max(signal,1e-300)),
                   rms_difference_db=db(np.sqrt(error/max(signal,1e-300))),
                   peak_difference_percent=100*float(np.max(np.abs(diff)))/max(peak,1e-300),
                   peak_difference_physical_velocity=float(np.max(np.abs(diff))),
                   reference_rms_physical_velocity=np.sqrt(signal/(2*frames)),
                   unaligned_rms_difference_percent=100*np.linalg.norm(outputs[1]-outputs[0])/max(np.sqrt(signal),1e-300))
        # Finite-record alias estimate: Hann-window the raw pickups, project
        # above host Nyquist, filter, then decimate. This is not a perceptual
        # metric or a universal bound; cancellation between aliases can occur.
        if factor > 1:
            windowed = raw0*windows.hann(len(raw0), sym=False)[:, None]
            nfft = 1 << (len(raw0)+len(a)-2).bit_length()
            source = np.fft.rfft(windowed, n=nfft, axis=0)
            freq = np.fft.rfftfreq(nfft, 1/(float(meta['host_rate'])*factor))
            high = source.copy(); high[freq <= float(meta['host_rate'])/2] = 0
            normalizer = None
            # Keep the actual candidate output phase here. The padded version
            # above is only for equal-delay waveform comparisons.
            for name, coeff in (('reference', a), ('candidate', equivalent(h1, factor))):
                transfer = np.fft.rfft(coeff, n=nfft)[:, None]
                alias = np.fft.irfft(high*transfer, n=nfft, axis=0)[factor-1::factor]
                if normalizer is None:
                    whole = np.fft.irfft(source*transfer, n=nfft, axis=0)[factor-1::factor]
                    normalizer = max(float(np.linalg.norm(whole)),1e-300)
                row[f'{name}_alias_rms_percent'] = 100*float(np.linalg.norm(alias))/normalizer
        else:
            row['reference_alias_rms_percent'] = row['candidate_alias_rms_percent'] = 0.
        rows.append(row)
        if (ident+1)%24 == 0:
            print(f'Analyzed {ident+1}/{len(manifests[0])}', flush=True)
    summary.update(fixtures=len(rows), physical_trajectories_exact=True,
                   max_streaming_oracle_error_over_input_peak=max_rounding,
                   pooled_rms_difference_percent=100*np.sqrt(total_error/total_signal),
                   max_energy_residual=max(float(m['max_energy_residual']) for m in manifests[0]))
    summary['alias_estimate_increased_fixtures'] = sum(
        r['candidate_alias_rms_percent'] > r['reference_alias_rms_percent'] for r in rows)
    for key in ('rms_difference_percent', 'peak_difference_percent', 'reference_alias_rms_percent', 'candidate_alias_rms_percent'):
        values = [r[key] for r in rows]
        summary[key] = dict(median=float(np.median(values)), p95=float(np.percentile(values,95)),
                            maximum=max(values), worst_fixture=rows[int(np.argmax(values))]['id'])
    # Explicit screening budgets, not established thresholds of audibility.
    summary['screening'] = dict(rms_budget_percent=.1, peak_budget_percent=1.,
        exceeding_rms=[r['id'] for r in rows if r['rms_difference_percent'] > .1],
        exceeding_peak=[r['id'] for r in rows if r['peak_difference_percent'] > 1.])
    with (args.output/'audio.csv').open('w') as out:
        writer = csv.DictWriter(out, fieldnames=list(rows[0])); writer.writeheader(); writer.writerows(rows)
    (args.output/'summary.json').write_text(json.dumps(summary, indent=2, allow_nan=False)+'\n')
    print(json.dumps({k:v for k,v in summary.items() if k != 'spectral'}, indent=2))


if __name__ == '__main__':
    main()
