# Vessel four-accumulator FIR — 2026-10-09

## Result

Retained four independent SSE2 accumulators in `StereoDecimator::pushStage()`.
The actual decimator measured 13.9–18.1% faster for factors 2/4/8 on this Linux
machine. Complete headless callback measurements showed smaller, noisy gains:
median per-case savings of 2.01% standalone and 1.27% with V.Tune linked.
One linked Metal/Suede case measured a 1.21% regression. This is a modest local
improvement, not a demonstrated speedup for every patch or machine.

The 129 coefficients, output cadence, delay, history storage and scalar fallback
are unchanged. The SSE2 output sum is reassociated; mechanical/contact reductions
are unchanged. This implements the first FIR experiment from
[the optimization audit](vessel_audit/README.md). Shorter filters and solver
changes were not included in this experiment.

## Correctness

The original branch-wrapped scalar oracle remains in `vessel_host_rate_spec`.
The normal build compares the four-accumulator result with an absolute bound
scaled by input peak, cascade depth and coefficient L1 norm. A separate
`vessel_host_rate_serial_spec` build defines `VESSEL_SERIAL_FIR`, retaining the
original SSE2 summation order and requiring exact equality with the oracle.
Both are included in `test-vessel` and `test-fast`.

The comparison covers factors 1/2/4/8, amplitude scales from `1e-100` to `1e100`,
random stereo input, DC, alternating signs, isolated impulses, tones, near
cancellation and a final zero-input tail. The maximum observed difference was
`7.10543e-16` times the input peak. The serial SSE2 and forced scalar builds
matched the oracle exactly. These are finite test results, not a universal
floating-point error proof.

The complete Vessel suite passed before and after the change. It includes
filter response, folded tones, stereo isolation, impulse latency, reset,
mechanical state/energy checks, seven host rates, three quality policies,
binaural transitions, passive-tail composition and contact re-entry. All
32 whole-module fixture fingerprints matched across all baseline/candidate
runs; those fingerprints cover float audio outputs and energy in separate
untimed release, strike re-entry and pitch-modulation traces.

## Measurement

- Baseline revision: `1f854f4e079d29cf685af461244c9aebdaf1fec8`.
- Linux x86-64, kernel 6.8.0-138-generic, Intel Core Ultra 7 165H.
- GCC 12.3.0, `-O3 -march=nehalem -fno-fast-math -fno-unsafe-math-optimizations`.
- Each benchmark process pinned to logical CPU 0; parent affinity unchanged.
- Benchmarks ran serially, without concurrent builds or tests.

The isolated benchmark links two separately compiled copies of the actual
`HostRateAdapter.cpp` into one executable, with a distinct namespace for the
serial copy and unused host-adapter sections discarded. Each measurement feeds
2,097,152 input frames after warmup. Nine paired repetitions reverse the
implementation order on alternate repetitions. Values below are median
nanoseconds per emitted stereo output frame, including input history writes.

| Factor | Serial SSE2 | Four accumulators | Saving |
| --- | ---: | ---: | ---: |
| 1 (bypass) | 1.964 | 1.965 | approximately zero |
| 2 | 40.524 | 34.903 | 13.87% |
| 4 | 119.809 | 98.144 | 18.08% |
| 8 | 279.539 | 229.216 | 18.00% |

The candidate was faster in all nine pairs at each of factors 2/4/8. This is
decimator cost, not the percentage improvement of Vessel as a whole. Composed
steady passive tails bypass the streaming FIR.

Whole-module measurements use `benchmark_active_module.cpp`, with debug timing
disabled, at a 48 kHz host and C4. The 16 fixtures cover Crystal/Wood and
Metal/Suede, 96/192 kHz internal settings, zero/33 Hz binaural separation,
continuous rubbing and rubbing with a strike every 250 ms. Each executable
run uses three fresh instances per fixture, one simulated second of warmup and
one timed second per instance. Each standalone/linked variant has six paired
baseline/candidate runs, alternating execution order.

For each fixture, saving is `1 - median(candidate means)/median(baseline means)`.
The table summarizes those fixture savings; it is not a workload-weighted total.

| Callback mode | Median fixture saving | Fixture range |
| --- | ---: | ---: |
| Standalone | 2.01% | 0.16% to 4.36% |
| Static V.Tune message | 1.27% | -1.21% to 3.31% |

The linked Metal/Suede, 96 kHz, zero-separation, rub-only fixture was slower in
four of six pairs, with a 1.21% median regression. Small changes remain subject
to CPU frequency, scheduling and code-layout effects. This experiment does not
establish callback deadline bounds, per-callback timing percentiles, live Rack
CPU-meter savings or Windows performance. The linked harness excludes V.Tune's
own processing and Rack message-flip scheduling.

## Reproduction and artifacts

Both callback implementations can now be built together without replacing an
executable between measurements:

```sh
make -j4 test-vessel
make -j4 build/tools/vessel_benchmark_active_module \
  build/tools/vessel_benchmark_active_module_serial
# Run serially; reverse order between pairs. Also repeat with --expander.
build/tools/vessel_benchmark_active_module_serial > before.csv
build/tools/vessel_benchmark_active_module > after.csv
make -j4 all
```

On Windows, use the documented MINGW64 environment and put the installed Rack
runtime first on PATH. The Windows bridge is absent from this Linux workspace,
so no native Windows build or timing result is claimed.

Local evidence is under `build/vessel-fir-2026-10-09/` (ignored by Git):
baseline/candidate binaries, test/build logs, 24 callback CSVs,
`paired-summary.json`, `fir-timing.csv`, `fir-summary.json`, source/binary hashes
in `environment.json`, the paired-run script, and source/build scripts for the
isolated benchmark. The paired-run script resumes existing CSVs; use a fresh
artifact directory or remove only those generated CSVs for a fresh measurement.

The full Linux `plugin.so` build and forced-scalar host-rate suite passed.
The host-rate suite also passed AddressSanitizer and UBSan (`-O1`,
`-fsanitize=address,undefined`, `-fno-omit-frame-pointer`). LeakSanitizer initially
failed because it cannot operate under this terminal's ptrace environment;
rerunning with `ASAN_OPTIONS=detect_leaks=0` passed. Leak detection is therefore
not covered. Windows and live Rack listening remain unverified.
