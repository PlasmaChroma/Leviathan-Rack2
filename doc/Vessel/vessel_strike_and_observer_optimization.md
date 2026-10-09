# Vessel strike and observer optimizations — 2026-10-09

## Retained changes

Two further items from [the audit](vessel_audit/README.md) are implemented:

1. `solveStrike()` caches the starting compression's square root and potential,
   reuses each trial's force/root, and evaluates the cancellation-free analytic
   derivative only when a Newton step is needed. The original brackets,
   tolerances, iteration limits, finite checks and failure behavior remain.
2. `VesselEngine` reuses the modal bank, friction certificate, control smoothing
   coefficient and active strike port when only the observer changes. Physical
   descriptor fields and settings are compared explicitly. The strict modal
   band limit is checked even when adjacent rates share a rounded timestep.
   Orbit resynchronization, observer calculation and the adapter's filtered-tail
   handoff remain in their existing order.

The observer path also handles identical repeated configurations. Changes to
pitch, decay, imperfection, contact settings, physical descriptors or timestep
take full preparation. Metadata pointers and unused descriptor slots are not
physical cache keys. A test/benchmark switch forces the full path.

The coupled strike/rub derivative was tested but **not retained**: its numerical
checks passed, but it did not establish a reliable callback improvement.
`ContactSolver.cpp` retains its original implementation. The FIR remains the
previously adopted four-accumulator, 129-tap implementation.

## Measured results

Baseline: revision `61451e7048b943def64215b0e0e4daf6d019461e`.
Environment: Linux x86-64, kernel 6.8.0-138-generic, Core Ultra 7 165H,
GCC 12.3.0, `-O3 -march=nehalem -fno-fast-math
-fno-unsafe-math-optimizations`. Benchmark children were pinned to logical CPU 0.
Timing runs were serial, without concurrent builds or tests; debug timing was
disabled. These are headless Linux measurements, excluding Rack scheduling,
GUI and audio-driver work.

### Isolated strikes

The direct solver benchmark uses the frozen previous solver and current solver
in one executable, called through volatile function pointers. It uses 2,048
deterministic contact inputs, both materials, all mallets and 96/192 kHz, with
32 passes per measurement and nine alternating paired repetitions.

| Solver | Previous median ns/solve | Retained median ns/solve | Saving |
| --- | ---: | ---: | ---: |
| Isolated strike | 66.692 | 55.472 | 16.82% |
| Unchanged coupled solver, control measurement | 448.973 | 448.083 | 0.20%, inconclusive |

The isolated solver was faster in all nine pairs. Whole-module impact timing
uses 64 fixtures: both materials, all four mallets, 96/192 kHz actual internal
rates, zero/33 Hz binaural separation, with and without rubbing. Each fixture
warms for 48,000 host frames and then processes 32,768 frames, with a strike
every 1,024 frames. Six baseline/candidate pairs reverse execution order.

| Callback phase | Median fixture saving | Fixture range |
| --- | ---: | ---: |
| Isolated strike onset | 4.64% | 0.76% to 12.67% |
| Continuing isolated contact | 5.92% | 4.22% to 7.35% |
| Coupled strike onset | 0.14% | -2.90% to 5.74% |
| Continuing coupled contact | 1.30% | -1.13% to 2.44% |

Each row covers 32 fixtures. Savings compare medians of six run means per
fixture. Coupled changes are not attributed to a solver optimization: that
solver is unchanged, and code layout and measurement variation remain factors.
The harness records sample counts, mean, median, p95, p99, p99.9 and maximum
for onset, ongoing contact and other callbacks. Onset has only 32 observations
per fixture/run, so its high percentiles are especially sparse. Timer overhead
is included, and none of these measurements establishes a deadline guarantee.

The separate block-timed active-use benchmark, with static width and only
occasional strikes, had a median fixture change of +0.24%, ranging from -1.61%
to +1.91% across four paired runs. This does not establish a steady-rubbing
speedup; the retained strike change targets short isolated contact intervals.

### Observer-only updates

`benchmark_observer_module` compares full and cached preparation in the same
executable, using two independent module instances. It alternates the order
over six repetitions for each of 16 fixtures: both materials, 96/192 kHz,
zero/33 Hz separation, ringing and rubbing. Width alternates at each existing
approximately 1 kHz control boundary. Each repetition processes 32,768 frames
after a one-second warmup.

Width-update callbacks measured **32.46–49.89% faster**, with a median fixture
saving of **42.39%**. Checksums matched exactly for every pair. The instrumented
stream mean, including ordinary frames and timing/checksum overhead, improved
1.49–10.20%, with a median of 4.52%, under this continuous width modulation.
Those percentages apply to this modulation workload, not static-width patches
or simultaneous pitch/material modulation that requires full preparation.

## Correctness and acceptance

The complete `test-vessel` suite passed before and after the work. The retained
code also passed the full Linux `plugin.so` build and focused AddressSanitizer/
UBSan runs. Leak detection was disabled because LeakSanitizer cannot run under
this terminal's ptrace environment.

New tests are included in both `test-vessel` and `test-fast`:

- `vessel_strike_gradient_spec`: 28,016 endpoint/equal/nextafter/near-equal and
  random gradient checks across all mallets. Cached force values match the
  existing force function exactly. An independent extended-precision quotient
  and integral-series oracle found maximum relative derivative error
  `6.515e-16`. Four thousand isolated roots match the previous solver exactly
  for the exercised inputs, with unchanged total iterations (9,879). A separate
  zero-cross-admittance comparison with the unchanged nested solver differs by
  at most `2.343e-16` after scaling by `max(1, |force|)`.
- `vessel_observer_configuration_spec`: 144 exact audio/state trajectories
  across both bowls, all mallets, 44.1/48/96 kHz hosts, all quality policies and
  zero/33 Hz separation. The comparisons include 124 observer changes while
  composed tails are active and 198 during contact. Another 35 checks cover
  physical cache keys, rate changes and transactional rejection, followed by
  adjacent-rate checks straddling the strict modal band boundary.

Independent before/after module traces cover 64 fixtures and 2,097,152 total
host frames. They include soft/hard strikes, repeated contact, rub release and
restart, and pitch/width changes. Out of 4,194,304 float audio samples, four
differed; the maximum difference was `1.490116119e-8 V` and the RMS difference
was `1.028975794e-11 V`. Maximum total-energy difference was `9.104e-15 J`.
There were no solver faults or nonfinite resets, and the maximum reported
one-step ledger residual was `1.543e-15 J`. These finite comparisons support
the exercised cases; they do not establish universal trajectory equivalence.
Windows builds, native Windows timing and live Rack listening remain unverified.

## Coupled candidate decision

Both eager and deferred derivative evaluation were screened in `solveContacts()`.
The deferred candidate's direct coupled solve measured 446.690 → 450.198 ns
(about 0.79% slower), and continuing coupled callbacks had a median fixture
regression of 2.52%. Other callback phases also moved, so the experiment cannot
separate all code-layout effects from arithmetic costs. The earlier eager form
showed a local kernel gain but inconsistent whole-module results as well.
The coupled production solver was restored rather than claiming an optimization
from its numerical improvement alone. Its prototype and evidence remain local.

## Reproduction

```sh
make -j4 test-vessel all
make -j4 build/tools/vessel_benchmark_strike_module \
  build/tools/vessel_benchmark_observer_module \
  build/tools/vessel_benchmark_active_module

# Preserve a baseline executable before editing DSP; run pairs serially.
build/tools/vessel_benchmark_strike_module > strike-timing.csv
build/tools/vessel_benchmark_strike_module --trace strike-trace.bin > strike-trace.csv

# Contains its own forced-full and cached configurations in one executable.
build/tools/vessel_benchmark_observer_module > observer-timing.csv
```

The trace file is native-endian binary doubles: 64 fixtures in the CSV's order,
32,768 frames each, with left output voltage, right output voltage, left total
engine energy and right total engine energy per frame. Trace mode enables
auditing and uses different controls from timing mode; it is an untimed
correctness check.

Local evidence is in `build/vessel-strike-2026-10-09/` (ignored by Git):

- `retained/`: final executables, tests/build/sanitizer logs, paired timing CSVs,
  traces, `summary.json`, `paired-summary.json`, `active-summary.json`,
  source/binary hashes in `environment.json`, and runner/summary scripts.
- `coupled-experiment/`: rejected candidate sources and `results/` measurements.
- The directory root holds the original baseline, first screening results and
  the direct solver benchmark source. The root screening candidate is not the
  retained implementation.

For a fresh native Windows measurement, use the documented MINGW64 build
environment and installed Rack runtime. No Windows bridge is present here.
