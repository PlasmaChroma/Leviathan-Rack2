# 109-tap FIR validation — 2026-10-09

The 109-tap candidate is available for offline experiments through
`VESSEL_EXPERIMENTAL_FIR109`. The production default remains the optimized
129-tap filter. Define the flag consistently across every translation unit;
it changes fixed storage sizes. The Makefile provides separate experiment
executables so normal plugin objects are not mixed with candidate objects.

**Decision: retain as an experiment, do not promote yet.** Measured bowl-signal
differences are small, but whole-module savings are modest and uneven, and the
44.1 kHz upper-treble difference needs an explicit sound decision. Small measured
fixture errors are evidence for those fixtures, not proof of inaudibility.

The candidate uses the audit's exact DC-normalized equiripple coefficients.
Both filters use the retained four-accumulator SSE2 kernel. Tail-history sizing
now derives from the selected tap count; the 129-tap result is unchanged.

## What changed numerically

We captured 288 one-second stereo physical-pickup fixtures: two bowls, four
mallets, 44.1/48/96 kHz hosts, all three quality policies, 261.625565/2000 Hz
pitches, and strike-only/strike-plus-rub operation. Each launches at velocity
0.1, retriggers at velocity 1 halfway through, and releases rubbing at 0.75 s.
High-pitch rate fallback is retained and the actual factor is recorded per case.
These captures exercise the internal physical engine and production streaming
FIR, before module output gain. They do not represent every possible control
trajectory, long decay, descriptor, or external feedback patch.

For each fixture, both executables produced **exactly identical internal-rate
physical pickup samples** and identical ledger summaries. The largest recorded
step energy residual was 8.93e-15 J; there were no solver faults or nonfinite
resets. Output differences therefore arise in the observation filter.

| Delay-aligned difference | Median fixture | 95th percentile | Worst fixture |
| --- | ---: | ---: | ---: |
| RMS error / reference RMS | 0.00618% | 0.02816% | 0.04165% |
| Peak error / reference peak | 0.00769% | 0.03044% | 0.05209% |

Pooling all fixture sample energies gives **0.00903% RMS error**. This weights
louder fixtures more heavily; the worst-fixture result is also reported to
avoid hiding quiet cases. The worst RMS/peak fixture is Metal/Felt, 2000 Hz,
48 kHz host, Reference quality, strike-only (fixture 130). Its RMS error is
-67.61 dB relative to its reference signal; peak physical-velocity error is
1.4293e-6 m/s.

Percentages use whole-record stereo energy or peak normalization. They are not
per-sample ratios near zero, a percentage of audibility, or percentages of
unchanged samples. The screening budgets were 0.1% RMS and 1% peak; all cases
passed. Those are explicit experiment budgets, not established psychoacoustic
thresholds or universal signal bounds.

### Alignment and independent implementation tolerance

The analysis constructs each cascade's equivalent FIR. It pads the shorter
filter symmetrically at the **internal sample rate** before convolution, giving
both filters exactly the same group delay before downsampling. This avoids
fractional host-sample interpolation, including the 7.5-sample difference at
4×. All fixture samples, including startup, enter the audio comparison; no
cross-correlation fitting or gain normalization minimizes the residual.

Separately, independent full convolution checks the **unaligned production
streaming output** of each executable. The largest error is 1.52e-15 of raw
input peak, against a 2e-13 acceptance limit. The branch-wrapped scalar oracle
also tests production ring storage, stereo isolation, cadence and reset across
factors 1/2/4/8, including inputs scaled from 1e-100 to 1e100. These are floating
point implementation tolerances; they do not describe the 109-versus-129 sound
difference. Factor-1 bypass outputs are exactly equal.

## Response and the important 44.1 kHz exception

Stage measurements use a 1,048,577-point frequency grid plus exact band edges.
Passband is 0–0.2 cycles/input sample; stopband is 0.25–0.5.

| Stage metric | Original 129 | Candidate 109 |
| --- | ---: | ---: |
| Passband span | 0.002465 dB | 0.002018 dB |
| Worst stopband amplitude | -75.29 dB | -88.23 dB |
| Mean stopband power | -90.65 dB | -91.26 dB |

Across cascades, the measured maximum relative amplitude change within
0–0.4 times the host rate is 0.02394% at 2×, 0.03945% at 4×, and 0.05506%
at 8×. These are dense-grid observations, not interval-certified bounds.

**At a 44.1 kHz host, 20 kHz is outside that passband.** At 2×, original gain
there is -7.8227 dB and candidate gain is -6.4163 dB: a **1.4064 dB / 17.58%
amplitude increase for a 20 kHz tone relative to the original filtered tone**.
The absolute transfer-gain difference through 20 kHz peaks near 7.14 percentage
points. The 4× result is essentially the same. The small bowl-fixture errors
must not be generalized to arbitrary upper-treble-rich inputs.

At a 48 kHz host, the 20 kHz difference is approximately 0.0060 dB at 2× and
0.0081 dB at 4×. At 96 kHz it is approximately 0.0020 dB at 2×. Bypass has no
filter difference. Full response results, including synthetic 8× cases, are
in the [machine-readable results](vessel_fir109_results.json).

An input-independent peak bound can also be derived from the L1 norm of the
aligned equivalent-filter difference: approximately 0.0965 times **raw input
peak** for factors 2/4/8, up to coefficient arithmetic rounding. This loose
bound applies to arbitrary bounded input, not relative output error. It is much
larger than the measured bowl residuals, illustrating why a fixture maximum
cannot be promoted into a universal guarantee.

### Aliasing estimate

For each finite fixture, the analysis Hann-windows the raw pickups, projects
their spectrum above host Nyquist, applies each filter, and decimates that
high-band component. The reference filtered, windowed signal supplies the
common RMS denominator. This is a finite-record spectral estimate with window
dependence and possible cancellation between folded components, not a
perceptual metric or live-patch alias bound.

The maximum estimated alias RMS is 0.000014999% of reference signal RMS for
129 taps (-136.48 dB), and 0.000016208% for 109 taps (-135.81 dB); these maxima
occur in different fixtures. The candidate estimate increases in 69 of 288
cases. Better worst-stopband rejection does not imply lower leakage at every
frequency or for every bowl spectrum. Production folded-tone tests also pass
the existing -70 dB requirement at factors 2/4/8.

## Delay and transition behavior

| Factor | Original tagged delay | Candidate tagged delay | Reduction |
| --- | ---: | ---: | ---: |
| 1 | 0 | 0 | 0 |
| 2 | 31.5 | 26.5 | 5 host samples |
| 4 | 47.25 | 39.75 | 7.5 host samples |
| 8 | 55.125 | 46.375 | 8.75 host samples |

The physical feedback loop does not contain the FIR. External patched feedback
does contain the changed output delay, and needs separate listening/behavior
assessment. Composed passive tails bypass the streaming FIR after preparation;
shortening the filter is not expected to speed up their steady processing.

Existing candidate tests cover impulse delay, multiple host rates, quality
fallback/switching, active-contact retuning, width changes, fold/wake, passive
tail composition and immediate-contact handoff. The passive-tail suite includes
504 configurations and four independent 60-second binaural tails. The
Rack-linked module suite passes its 14 groups, including allocation checks and
patch persistence. These compare candidate optimized paths with candidate
reference paths, validating integration independently of the changed response.

## Performance and build validation

Six alternating reference/candidate pairs per mode ran serially, with affinity
to CPU 0, after builds and numerical tests completed. This is Linux x86-64 on
Core Ultra 7 165H, GCC 12.3.0, `-O3 -march=nehalem`, strict floating point. No
debug callback timer is enabled. The baseline already includes the previous
FIR, isolated-strike and observer-configuration optimizations.

| Benchmark | Median saving | Range / consistency |
| --- | ---: | --- |
| Streaming FIR, 2× | 7.86% | Faster in 5/6 pairs |
| Streaming FIR, 4× | 10.96% | Faster in 6/6 pairs |
| Streaming FIR, 8× | 10.85% | Faster in 5/6 pairs |
| Whole Vessel callback, standalone | 0.51% | Fixture medians -1.18% to +2.87% |
| Whole Vessel callback, static V.Tune message | 0.66% | Fixture medians -0.20% to +2.50% |

FIR timings compare each process's median of seven batches, then take the
median paired percentage saving. Whole-module results take each fixture's
median paired percentage, then the median across 16 fixtures. The existing
active-module harness covers both bowls, two quality settings, zero/33 Hz
separation, and rubbing with/without repeated strikes at a 48 kHz host. The
linked harness excludes V.Tune's own callback and Rack message-flip scheduling.

Five standalone fixture medians and one linked fixture median regress. Only
one fixture per module mode improves in every pair. Some isolated FIR pairs
have large scheduling/frequency outliers (approximately 59% and 56% apparent
regressions at 2×/8×); none were discarded. Factor-1 bypass is an unchanged-work
control and measures a noisy -1.88% median saving. These results support lower
streaming-filter cost, but do **not** establish a robust general module speedup,
callback deadline bounds, Windows performance, or live Rack CPU-meter savings.

The normal `test-vessel` suite and full default Linux `plugin.so` link passed.
Candidate host-rate, strict serial oracle, dual-bowl, passive-tail, observer,
and Rack-linked module suites passed. Candidate host-rate and passive-tail
suites also passed ASan/UBSan with `-O1 -g -fno-omit-frame-pointer` and strict
floating point. Leak detection was disabled (`ASAN_OPTIONS=detect_leaks=0`)
because of the known ptrace environment limitation; this is not leak coverage.
No full Windows build or listening result is claimed.

## Reproduction

```sh
make -j4 test-vessel test-vessel-fir109 all \
  build/tools/vessel_fir_capture build/tools/vessel_fir_capture_109 \
  build/tools/vessel_benchmark_active_module \
  build/tools/vessel_benchmark_active_module_109
mkdir -p build/fir109/reference build/fir109/candidate
build/tools/vessel_fir_capture build/fir109/reference > build/fir109/reference/manifest.csv
build/tools/vessel_fir_capture_109 build/fir109/candidate > build/fir109/candidate/manifest.csv
OPENBLAS_NUM_THREADS=1 python3 tools/vessel/analyze_fir109.py \
  build/fir109/reference build/fir109/candidate build/fir109/analysis
# Run after builds, captures, analysis and tests finish:
python3 tools/vessel/benchmark_fir109.py build/fir109/timing --pairs 6 --cpu 0
```

Omit `--cpu` where Linux affinity is unavailable. The capture tool writes
native-endian interleaved double stereo `.raw` (internal rate) and `.out` (host
rate) files, plus coefficients and a CSV manifest. The analyzer requires NumPy
and SciPy and must run without Python's assertion-disabling `-O` option.

Local evidence is in `build/vessel-fir109-2026-10-09/` (ignored by Git). Captures
use the current physical engine, based on commit
`b74372e4f63c7ede7f9d1edf6b0ff334204d6e02`. The experiment changes neither the
solver nor the production default. Windows, live Rack timing, external feedback
and listening remain unverified.
