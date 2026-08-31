# Integral Flux Performance Optimization Implementation Record

Status: implemented and validated through the current equivalence-first pass

Modules: `Integral Flux` and `Proc`

Last updated: 2026-08-31

## 1. Purpose

This document records the performance work, compatibility constraints, regression coverage, validation results, and remaining optimization opportunities for Integral Flux and Proc.

The governing requirement is:

> Improve audio-thread and UI performance without changing released audio behavior, patch compatibility, or visible frame quality.

Integral Flux and Proc are released modules. Their parameter, input, output, and light IDs are therefore append-only. Existing serialization keys and meanings must remain compatible.

Proc is treated as the single-channel relative of Integral Flux. Shared function-generator, slew, trigger, preview, and telemetry patterns should remain aligned where their documented module behavior is the same.

## 2. Non-negotiable invariants

### Audio behavior

- Do not reorder or reinterpret released parameter, input, output, or light IDs.
- Do not change existing patch/state serialization semantics.
- Preserve the exact phase integrator, curve warp, timing, signal injection, clamp, BLEP, gate, and output arithmetic unless a deliberate behavior change is separately approved.
- Preserve trigger acceptance and rearm behavior.
  - A trigger begins a rise from idle.
  - A trigger during rise is ignored.
  - A trigger during fall restarts from the beginning and resets the function output to its minimum before the new rise advances.
- Preserve idle slew behavior when Signal IN is patched and the function generator is inactive.
- Preserve Integral Flux mixer normalization: patching a channel's variable output removes that channel from the `SUM`, `INV`, and `OR` buses.
- Preserve Proc `HALT`: active phase and output state remain held exactly while HALT is high, then continue when it falls.
- Preserve Proc's main/negative output relationship and amplitude behavior.

### Screen behavior

- A preview frame must contain one coherent curve publication and one coherent dot publication. The UI must never combine fields from different audio-thread publications.
- Cached preview work must reproduce the same points and the same NanoVG path simplification tolerance as the uncached implementation.
- Rebuilding may be deferred until source curve state changes, but visible responsiveness must not regress.
- The `Process`, `Step`, and `Draw` Debug Terminal metrics retain their module-level meanings. Component timings, if added later, belong after those three macro metrics.

### Performance behavior

- No new allocation, lock, file access, or UI work may enter the per-sample audio path.
- Debug instrumentation must remain gated by `isDragonKingDebugEnabled()`.
- Expensive work should be cached or moved to a lower-frequency path when the cache key fully describes the result.
- Preserve arithmetic order in released DSP unless a deliberate behavior change is approved; focused behavioral assertions and long stress traces guard that contract without requiring compiler-identical float bits.

## 3. Hot-path map

The audit identified four useful performance domains:

| Domain | Original cost pattern | Current treatment |
| --- | --- | --- |
| Audio telemetry | High-resolution timer work on every sample while debug was enabled | Sample one of every 64 process calls |
| Injection coefficient | Integral Flux recomputed a sample-rate-dependent exponential in the process path | Cache by exact `sampleTime` value |
| Preview curve generation | Segment lookup tables rebuilt whenever curve geometry was rebuilt | Cache LUTs by curve and shape-mode key |
| Preview drawing | Polyline simplification repeated for every visible draw segment | Cache full, rise, and fall simplified paths when points rebuild |

Preview publication also had a correctness problem adjacent to performance work: independently loaded atomics could form a mixed frame. Curve and dot publications now use separate sequence counters so readers retry only the publication that was concurrently changing.

## 4. Implemented changes

### 4.1 Integral Flux audio and shared state

Implementation: `src/IntegralFlux.cpp`

#### Cached signal-in injection coefficient

`injectAlphaBaseForSampleTime()` caches the result of:

```cpp
OUTER_INJECT_GAIN * clamp(
    1.f - std::exp(-sampleTime / OUTER_INJECT_TAU),
    0.f,
    1.f);
```

The coefficient is recomputed only when `sampleTime` changes by more than `1e-12f`. In normal Rack operation this means once at initialization and again after a sample-rate change, rather than once per audio sample.

The formula and float result are unchanged. The cache key is the input that fully determines the result.

#### Sampled audio performance telemetry

When Dragon King debug is enabled, Integral Flux records process timing on one of every 64 calls. Debug-disabled behavior remains a cheap gate and does not call the clock.

This reduces the observer cost of profiling the audio thread while retaining representative `Process` timing telemetry. The metric continues to represent total module-level processing.

#### Coherent preview publications

Curve state and dot state have independent odd/even sequence counters:

1. Writer increments the relevant counter to odd with acquire/release ordering.
2. Writer stores the publication fields with relaxed atomics.
3. Writer increments the counter to even with release ordering.
4. Reader loads an even starting sequence, copies the fields, and accepts them only if the ending sequence matches.

Separating curve and dot counters prevents high-frequency dot updates from invalidating otherwise stable curve snapshots.

#### Headless test boundary

`INTEGRAL_FLUX_HEADLESS_TEST` excludes UI inclusion and model registration while retaining the production module implementation. The runtime test includes the real implementation rather than maintaining a second DSP model.

### 4.2 Integral Flux preview UI

Implementation: `src/IntegralFluxUI.inc`

#### Segment LUT cache

Rise and fall preview LUTs are cached using:

- signed curve value;
- function shape mode.

They rebuild only when either key changes. Point generation continues to sample the same LUT values at the same positions.

#### Simplified path cache

The UI stores three simplified paths:

- complete function path;
- rise segment;
- fall segment.

These paths rebuild immediately after the source point array rebuilds. NanoVG drawing then submits the cached points directly rather than calling `wave_preview::simplifyPath()` for every segment on every draw.

The simplification input, stride, and tolerance remain unchanged (`stride = 1`, `tolerance = 0.02f`). Highlighting selects among cached segment paths and does not change the curve geometry.

### 4.3 Proc parity work

Implementation: `src/Proc.cpp`

Proc now uses the corresponding safe patterns where its architecture matches Integral Flux:

- independent curve and dot preview sequence counters;
- coherent preview readers;
- one-in-64 audio process telemetry sampling while debug is enabled;
- cached preview rise/fall LUTs keyed by curve value;
- cached full/rise/fall simplified NanoVG paths;
- `PROC_HEADLESS_TEST` boundary around UI and model registration.

Proc's module registration remains present in normal production builds. The headless guard exists only for the runtime test translation unit.

Proc's per-sample injection coefficient exponential has not yet been changed in this pass. Porting the sample-time cache used by Integral Flux is the clearest next equivalence-first DSP optimization.

## 5. Rejected DSP optimization

An attempted stage-timing refactor changed the modulated runtime behavior. It was rejected and the original DSP arithmetic was restored.

This is an important precedent: a change that is mathematically plausible or perceptually small is not considered behavior-preserving when it alters the released reference trace. Such a change requires either:

- a different implementation that preserves the exact float operation sequence; or
- explicit approval to treat the difference as an intentional audio behavior revision.

Neither applies to the current equivalence-first optimization phase.

## 6. Regression test architecture

### 6.1 Integral Flux runtime specification

File: `tests/integral_flux_runtime_spec.cpp`

Coverage:

- released parameter, port, output, and light schema;
- persisted module settings round-trip;
- concurrent preview curve and dot coherence;
- `SUM`/`INV`/`OR` mixer normalization as variable outputs are patched;
- bidirectional idle slew behavior;
- trigger acceptance, rise rejection, and fall restart behavior;
- 48 kHz modulated audio/state stress traces at timing update divisors `/1` and `/8`;
- BLEP-enabled finite, bounded, active stress traces at 44.1, 96, and 192 kHz.

The stress traces observe every output voltage plus relevant internal phase/timing state. They reject non-finite values, runaway state, loss of meaningful activity, and collapsed timing/sample-rate scenarios rather than comparing compiler-specific float fingerprints.

### 6.2 Proc runtime specification

File: `tests/proc_runtime_spec.cpp`

Coverage:

- released schema;
- persisted settings round-trip;
- legacy `ch1CycleLatched` state loading;
- concurrent preview curve and dot coherence;
- bidirectional idle slew and exact negative-output inversion;
- trigger acceptance, rise rejection, and fall restart behavior;
- exact HALT freeze and subsequent cycle resume;
- 48 kHz modulated audio/state stress traces at timing update divisors `/1` and `/8`;
- BLEP-enabled finite, bounded, active stress traces at 44.1, 96, and 192 kHz.

### 6.3 Cross-platform trace contracts

The long traces are stress scenarios rather than compiler fingerprints. They require all sampled outputs and internal state to remain finite and bounded, require meaningful signal activity, and verify that timing modes and sample-rate scenarios do not collapse into indistinguishable execution. Small checksums are retained only as diagnostic activity witnesses; no exact float-bit value is a pass/fail baseline.

Portable behavior is specified directly by the focused schema, persistence, publication, mixer, slew, trigger, inversion, and HALT assertions. This avoids coupling correctness to compiler, fast-math, architecture, or standard-library details.

## 7. Build and test integration

`Makefile` builds both Rack-linked runtime specifications and runs them from `test-fast`:

```text
build/tests/integral_flux_runtime_spec
build/tests/proc_runtime_spec
```

Inside the native MSYS2 MINGW64 environment, the routine authoritative command is:

```sh
make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
```

For a focused iteration:

```sh
make -j10 \
  build/tests/integral_flux_runtime_spec \
  build/tests/proc_runtime_spec \
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"

PATH="/c/Program Files/VCV/Rack2Pro:/mingw64/bin:/usr/bin:$PATH" \
  ./build/tests/integral_flux_runtime_spec

PATH="/c/Program Files/VCV/Rack2Pro:/mingw64/bin:/usr/bin:$PATH" \
  ./build/tests/proc_runtime_spec
```

The Rack application directory must remain ahead of compiler runtime directories so the tests load the installed `libRack.dll` and its matching runtime dependencies.

Production compilation can be verified with:

```sh
make -j10 plugin.dll
```

Use `make -B -j10 plugin.dll` when a forced from-source production compile and link is warranted. Do not use forced rebuilds as the default iteration loop because they rebuild the entire plugin.

## 8. Validation record

The current implementation completed the following native Windows validation:

- focused Integral Flux runtime specification: passed;
- focused Proc runtime specification: passed;
- full `test-fast`: passed;
- forced production compile and `plugin.dll` link: passed;
- `git diff --check`: no whitespace errors; only the repository's existing Windows line-ending warnings were reported.

The production build compiled both `src/IntegralFlux.cpp` and `src/Proc.cpp` without their headless guards and successfully linked them with the complete plugin.

## 9. Next implementation phase

### Priority 1: cache Proc's injection coefficient — implemented

Proc previously evaluated:

```cpp
SIGNAL_INJECT_GAIN * clamp(
    1.f - std::exp(-args.sampleTime / SIGNAL_INJECT_TAU),
    0.f,
    1.f);
```

inside `process()`. Proc now caches the result by exact `sampleTime`, following Integral Flux's `injectAlphaBaseForSampleTime()` implementation. This removes one exponential per sample per Proc instance during steady sample-rate operation.

The focused Linux/Rack-SDK run passes the cross-platform behavioral and stress-trace contracts. Native Windows build and smoke-test validation remains required for the production release path.

Acceptance requirements:

- both 48 kHz timing-mode stress traces pass;
- all three multi-rate stress traces pass;
- focused tests, full `test-fast`, and `plugin.dll` build pass.

### Priority 2: measure the preview changes in Rack

Use realistic instance counts and compare:

- module hidden versus visible;
- tracer enabled versus disabled;
- curve cache versus frame cache;
- NanoVG and GL preview paths where applicable;
- Dragon King debug disabled for production cost, then enabled only for telemetry collection.

Record total `Process`, `Step`, and `Draw` values without renaming their meanings. If component-level timings are needed, append them as separate fields.

### Priority 3: mechanical shared-core cleanup only after profiling

Integral Flux and Proc intentionally duplicate related function-generator logic. A shared helper could reduce maintenance drift, but it is not automatically a performance improvement and could change inlining or float operation order.

Any shared-core extraction must:

- preserve module-specific gate semantics, amplitude behavior, shape modes, mixer behavior, and HALT behavior;
- preserve released arithmetic and the focused behavioral contracts;
- compile in the production C++11 plugin build and the C++17 test harnesses;
- pass every focused contract and stress trace before it is retained.

Do not combine the modules merely for aesthetic deduplication.

## 10. Optimization acceptance checklist

For every subsequent Integral Flux or Proc optimization:

1. State the exact cost being removed and its execution frequency.
2. Identify the complete cache key or invariant that makes the optimization safe.
3. Run the focused module specifications before and after the change.
4. Reject or restore the change if a focused behavior contract or stress-trace invariant fails unexpectedly.
5. Run full native `test-fast`.
6. Compile and link the production Windows `plugin.dll`.
7. Smoke-test both modules in Rack, including patch reload, cable normalization, cycle/trigger behavior, slew, HALT, and visible preview motion.
8. Compare real `Process`, `Step`, and `Draw` telemetry or an external profiler result. Do not retain complexity without a measurable win.

## 11. Relevant files

| File | Responsibility |
| --- | --- |
| `src/IntegralFlux.cpp` | Integral Flux DSP, state, preview publication, audio telemetry |
| `src/IntegralFluxUI.inc` | Integral Flux preview generation and drawing |
| `src/Proc.cpp` | Proc DSP, state, preview, UI, and telemetry |
| `tests/integral_flux_runtime_spec.cpp` | Integral Flux compatibility and cross-platform stress tests |
| `tests/proc_runtime_spec.cpp` | Proc compatibility and cross-platform stress tests |
| `tests/wave_preview_simplification_spec.cpp` | Shared path simplification behavior |
| `Makefile` | Test build and `test-fast` integration |
| `doc/windows_build_from_wsl.md` | Authoritative Windows toolchain invocation from WSL-like terminals |

This record describes an equivalence-first performance phase. It does not authorize audible approximation, released-schema changes, or visual-quality reductions.
