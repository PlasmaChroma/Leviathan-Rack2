# Fast friction evaluation — 2026-10-09

Production now prepares reciprocal velocity scales once per friction solve and
uses a monotone cubic Hermite tanh table. Gaussian weakening remains analytic.
The output FIR stays at 129 taps, pitch behavior is unchanged, and no solver
tolerances, iteration limits, bracketing or recovery checks were relaxed.

## Implementation

`PreparedFrictionLaw` is constructed after descriptor/dynamic validation and
the zero-load return. It caches the two reciprocal scales and mu difference
for the current solve; it has no persistent invalidation or parameter cache.
Both ordinary rubbing and coupled strike/rub inner solves use it.

The tanh curve has 2,048 monotone cubic intervals over normalized magnitude
0–20. Its derivative comes from the same cubic as the force, including the
generator's monotonicity limiting. Magnitudes at or above 20 still saturate
to ±1. Rounded lookup boundaries are clamped to the last valid interval.
The generated runtime table is 64 KiB, shared by the plugin; the experimental
Gaussian table is not compiled into production. `generate_friction_tables.py`
now checks both the existing offline tables and the runtime tanh-only header.

`frictionValue()` remains the analytic reference. Prepared evaluation falls
back to it for subnormal descriptor velocity scales to avoid overflowing
reciprocals. The descriptor must outlive the per-solve preparation object.
The production uniqueness check uses the conservative slope bound used by the
validated combined experiment, including the tanh rounding margin. Subnormal
fallback uses the original analytic bound. A slightly larger certificate can
reject a descriptor extremely close to the old certificate limit; normal
fixtures retained their exact internal-rate selection.

Offline contact profiling now distinguishes `tanh_lookups` from actual
`std::tanh` calls. Profiling is still compiled out of normal builds.

## Isolated screening

Five candidates were tested against independently compiled analytic controls:
Gaussian-only lookup, tanh-only lookup, per-solve reciprocals with analytic
functions, both lookups, and reciprocals plus tanh lookup. All edits were made
in isolated copies before production integration. Earlier unsuccessful table
experiments motivated measuring each component separately.

The existing eight-fixture harness covers Metal/Suede, overlapping Metal/Wood,
Crystal/Suede, high-pressure silicone reversal, very slow Felt, a pitch sweep,
binaural Metal, and high Crystal. Each runs at Reference and Economy quality,
with actual internal rates recorded. Each eight-second trajectory includes
strikes and contact release. A separate audited run records stereo physical
pickup audio and energy diagnostics; timed runs disable auditing and disk I/O.
Two timed repeats per invocation were averaged, with process order
analytic/candidate/candidate/analytic, pinned to CPU 0. Screening percentages
compare the averaged before/after invocation times.

| Candidate | Median engine saving across 16 fixtures | Fixture range | Worst waveform RMS error, percent of reference RMS |
| --- | ---: | ---: | ---: |
| Gaussian lookup | 1.31% | -1.88% to +3.65% | 2.09e-6% |
| Tanh lookup | -0.23% | -2.92% to +15.14% | 3.11e-8% |
| Reciprocals, analytic curves | 4.55% | +1.83% to +8.36% | 5.21e-10% |
| Both lookups | -0.07% | -3.49% to +14.54% | 2.11e-6% |
| Reciprocals + tanh | 2.89% | -0.08% to +18.60% | 2.92e-8% |

Every repeated audited capture was byte-identical to its same-variant repeat.
There were no reported solver faults, nonfinite samples or changes in recorded
rate selection. These are short deterministic trajectories, not universal
equivalence or a long-term acoustic calibration. RMS comparisons use the whole
stereo signal energy, without gain fitting or delay alignment (filter unchanged).

## Whole-module candidate comparison

The headless Rack-linked Vessel callback benchmark then compared analytic,
tanh-only, reciprocal-only and combined binaries in four alternating-order
runs. Ordinary fixtures retain Crystal/Wood and Metal/Suede at 0.2 rev/s and
2.5 N. A second set uses Felt for both bowls at 0.0001 rev/s and 15 N. Each set
has 16 fixtures: two bowls, Balanced/Reference quality, zero/33 Hz separation,
and rubbing with/without repeated strikes. All hosts run at 48 kHz. The harness
includes controls and normal telemetry, with debug timing disabled; it excludes
Rack scheduling, GUI, driver and V.Tune's own callback. No expander is connected
in this comparison.

| Candidate | Ordinary median fixture saving | Ordinary fixture range | Slow Felt median fixture saving | Slow Felt fixture range |
| --- | ---: | ---: | ---: | ---: |
| Tanh only | 0.94% | -0.60% to +2.70% | 16.21% | +9.68% to +19.68% |
| Reciprocals only | 3.40% | +2.45% to +6.81% | 0.94% | -2.49% to +3.17% |
| Combined | 1.97% | -0.31% to +4.30% | 14.37% | +8.06% to +20.43% |

Each fixture statistic is the median of four paired percentage savings; the
headline is the median across fixtures. The combined candidate trades some
ordinary-rubbing gain for a large slow-contact gain. One ordinary fixture had
a small negative median. All slow-Felt combined fixture medians improved,
with 14/16 improving in every pair. Gaussian lookup did not show sufficient
broad benefit in screening to retain it.

### Final production confirmation

After adding the subnormal-scale fallback and moving the tanh table into its
generated runtime header, the final implementation was compared afresh with
the preserved analytic callback binary in four alternating pairs per mode:

| Final production | Median fixture saving | Fixture range | Fixtures faster in all four pairs |
| --- | ---: | ---: | ---: |
| Ordinary rubbing | **3.05%** | -0.30% to +9.56% | 8/16 |
| Slow Felt | **16.85%** | +10.67% to +22.02% | 15/16 |

The ordinary Metal/Suede, Balanced, 33 Hz, rub-only fixture has the -0.30%
median regression. Results do not establish a gain for every patch. Differences
between the candidate screen and this confirmation illustrate timing/code-layout
variation; they are not evidence that adding the fallback itself accelerated
processing. All 32 final callback fixture fingerprints match the validated
combined candidate, including the untimed release/re-entry/retuning trace.

## Numerical and safety validation

- Dense screens cover all four mallets, three loads, broad/narrow slip ranges,
  saturation boundaries and adjacent representable values. They compare force
  and derivative with the analytic law, check force bounds/dissipation,
  derivative consistency and the conservative negative-slope certificate.
  Across all screened variants the largest errors were 1.31e-9 N in force and
  0.00412 N/(m/s) in derivative; these aggregate maxima are not unique to the
  retained implementation.
- Both reciprocal candidates passed the existing engine, host-rate, dual-bowl,
  passive-tail and Rack-module suites before integration. This includes long
  tails, active contacts, coefficient/damping changes and failure checks.
- The final normal implementation passed `test-vessel`, counter accounting
  tests, generated-table checks and the full Linux plugin link.
- ASan/UBSan passed the final dense/edge/subnormal curve tests and host-rate
  suite. Leak detection was disabled for the known ptrace limitation, so this
  does not establish leak coverage.
- All 16 final production eight-second captures match the validated combined
  candidate **byte for byte**. The worst retained-candidate RMS error against
  the analytic control is 2.92e-8%; worst peak error is 9.72e-8% of reference
  peak. Maximum final absolute energy-ledger residual was 7.88e-12 J.

All performance measurements here are Linux/Core Ultra 7 165H/GCC 12.3.0,
`-O3 -march=nehalem` with strict floating-point flags. Builds, sanitizer runs
and numerical analysis did not overlap the reported timing runs. CPU affinity
reduces migration but does not eliminate scheduling, frequency, cache or code
layout variation. These are not deadline bounds or Windows/live Rack results.
Listening and native Windows validation remain outstanding.

## Reproduction and evidence

```sh
make -j4 test-vessel test-vessel-contact-work all
# The probe reconstructs the analytic control in its isolated source copies.
# Run these serially, without other builds or benchmarks:
python3 tools/vessel/probe_fast_friction.py --gaussian-only --cpu 0 --output build/friction/gaussian
python3 tools/vessel/probe_fast_friction.py --hybrid --cpu 0 --output build/friction/tanh
python3 tools/vessel/probe_fast_friction.py --reciprocals --cpu 0 --output build/friction/reciprocals
python3 tools/vessel/probe_fast_friction.py --cpu 0 --output build/friction/both
python3 tools/vessel/probe_fast_friction.py --reciprocal-tanh --cpu 0 --output build/friction/reciprocal-tanh
python3 tools/vessel/validate_friction_candidates.py build/friction
python3 tools/vessel/benchmark_friction_candidates.py build/friction --cpu 0
OPENBLAS_NUM_THREADS=1 python3 tools/vessel/summarize_friction_candidates.py build/friction
```

Omit CPU affinity on platforms without `sched_setaffinity`. Use the appropriate
Rack SDK/runtime on Windows. Run Python checks without `-O`. The active-module
benchmark now accepts `--speed`, `--pressure` and `--mallet`, records them in CSV,
and retains its original defaults and optional static-expander mode.

Local evidence is under `build/vessel-friction-candidates/`: isolated source
trees/binaries, eight-second captures, timing CSVs, candidate and production test
logs, and production capture/timing comparisons. Machine-readable aggregate
results are in [vessel_fast_friction_results.json](vessel_fast_friction_results.json).
