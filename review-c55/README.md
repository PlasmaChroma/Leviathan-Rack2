# Leviathan Plugin Suite Review

## 1. Overall Executive Summary

Leviathan is an ambitious 11-module suite with a coherent alien-artifact visual language, extensive custom rendering, and several genuinely distinctive instruments. Bifurx, Undertow, Proc, and Integral Flux are closest to public release. Temporal Deck has unusually strong helper/engine tests but retains audio-thread buffer lifecycle risk. Crownstep and Wyrm have concrete real-time blockers. Chronomaw and Bulkhead are correctly hidden and incomplete.

Architecture is strongest where pure DSP/behavior has been extracted into helpers and tested: Temporal Deck, Crownstep game rules, Bifurx filter behavior, Undertow shaping, Bulkhead geometry, and shared panel SVG utilities. It is weakest at UI/audio ownership boundaries: audio-thread mutexes, allocations, large buffer copies, unsynchronized visualization data, and many renderer variants.

The largest suite-wide release blocker is configuration, not a compiler error: `res/dragonking.txt` is distributable and enables debug mode. That causes per-sample high-resolution timing in Integral Flux, Proc and Undertow, enables repair/developer behavior in Sil, and exposes debug-oriented UI. TD.Scope performs per-sample timing even without that flag. Existing `make test` passes all enabled targets, but Chronomaw's WIP test is skipped and several modules have no DSP tests.

Visual coherence is high, but rendering complexity is also high: NanoVG, framebuffer, raster, OpenGL and shader paths recur across the suite. Public documentation is not release-ready: repository-root `README.md` is absent and `plugin.json` has empty manual/plugin URLs.

Overall release readiness: 6/10

## 2. Module Scorecard

| Module | Release Readiness | Main Strength | Main Risk | Recommended Status |
| ------ | ----------------- | ------------- | --------- | ------------------ |
| Integral Flux | 7/10 | Complete four-channel modulation/mixer design | Production debug overhead; no DSP tests | Needs polish |
| Proc | 7/10 | Focused, coherent function/slew engine | Production audio/UI profiling | Needs polish |
| Temporal Deck | 6/10 | Distinctive workflow and broad test suite | Live-to-sample conversion still allocates on audio | Hold from release |
| TD.Scope | 6/10 | Versioned interactive expander protocol | Per-sample timing; renderer complexity | Experimental |
| Undertow | 8/10 | Focused, bounded oscillator DSP | Debug overhead; mono only | Release candidate |
| Crownstep | 7/10 | Original concept; lock-free playback snapshots | AI cancellation and Rack stress | Needs polish |
| Bifurx | 8/10 | Strong filter DSP and verification | Renderer/worker lifecycle and copy spikes | Release candidate |
| Wyrm | 6/10 | Unique polyphonic drawable oscillator | Wavetable rebuild in audio callback | Hold from release |
| Sil | 6/10 | Sophisticated sample-rate-aware mastering chain | RT queue, UI races, weak end-to-end proof | Experimental |
| Chronomaw | 3/10 | Promising engine/UI separation | Clock/CV inputs and density nonfunctional | Hold from release |
| Bulkhead | 3/10 | Compelling spatial UI/geometry concept | Cumulative wall CV and SR-dependent delays | Hold from release |

## 3. Cross-Cutting Issues

- Production debug is enabled by packaged `res/dragonking.txt`; performance and feature behavior therefore differ from a normal release build.
- Mono/polyphony policy is inconsistent. Wyrm is polyphonic; most other signal processors silently use channel 1. This is not documented.
- External voltage and JSON finite-value sanitation is inconsistent, allowing NaN/Inf to poison persistent DSP state.
- Large UI/audio handoffs use a mix of atomics, double buffers, direct shared arrays and mutexes. There is no single ownership convention.
- Custom rendering is fragmented across NanoVG, framebuffer, raster, OpenGL and shader paths, increasing GPU compatibility and teardown risk.
- Large combined files (`TemporalDeckUI.cpp`, `CrownstepUI.cpp`, `VisualAssets.cpp`, `TemporalDeckEngine.hpp`, `Sil.cpp`) slow review and make state ownership harder to prove.
- Serialization generally exists, but schema migration conventions are inconsistent. Absolute sample/art paths reduce patch portability.
- Test coverage is strong in isolated pockets but absent for Integral Flux, Proc, Wyrm oscillator behavior, TD.Scope lifecycle/rendering, Sil's complete chain, Chronomaw engine behavior and Bulkhead reverb DSP.
- Packaging metadata validates against Rack tags and GPL-3.0-or-later matches `LICENSE.txt`, but root README/manual links are missing. Temporal Deck contains a stale missing Dragon King art fallback.

## 4. Shared Architecture Recommendations

- Ship a release-safe feature configuration: debug off, development menus/loggers compiled or gated out, and sparse opt-in profiling.
- Define a real-time boundary contract: no locks, heap operations, filesystem/network work, vector destruction or high-resolution clocks in `process()`.
- Add a shared fixed-capacity snapshot/ring toolkit for UI telemetry, waveform data and sequence publication, including generation-checked double buffers.
- Add `finiteVoltage()`/validated JSON helpers and apply them at all module boundaries.
- Publish a suite-wide polyphony convention: explicit mono labels, mono broadcast rules, and a maximum of 16 channels where supported.
- Consolidate visual backends around one guaranteed NanoVG fallback and at most one accelerated path, with shared GL-context lifecycle tests.
- Build a performance harness that reports average/p95/max callback time, allocations, module-instance scaling, UI frame cost and memory at 44.1-192 kHz.
- Standardize serialization with a schema version, typed readers, range/finite validation and missing-file status.

## 5. Release Blockers

| Severity | Module | Blocker | Why It Matters | Suggested Fix |
| -------- | ------ | ------- | -------------- | ------------- |
| Critical | Suite | Packaged debug mode is enabled | Adds audio-thread clocks and development behavior to release | Ship `debug:false` or omit the flag; compile out dev paths |
| High | Temporal Deck | Live-to-sample conversion allocates/copies in audio callback | User-triggered conversion can glitch under load | Preallocate or hand capture to worker |
| Critical | Wyrm | Wavetable rebuild in audio callback | Editing creates deterministic callback spikes | Double-buffer prebuilt tables and swap |
| Critical | Bulkhead | Wall CV accumulates into persistent geometry | Sustained CV rapidly corrupts behavior and saved state | Derive effective geometry without mutating base state |
| Critical | Chronomaw | Clock and CV inputs do not implement advertised behavior | Core module contract is broken | Implement/test inputs or keep module hidden |
| High | Sil | RT safety and mastering invariants unproven | Master-bus glitches/wrong ceiling are high impact | Fixed peak queue plus end-to-end fixtures |
| High | TD.Scope | Unconditional per-sample timing | Pure visual expander consumes avoidable audio CPU | Guard or sparsely sample metrics |

## 6. Performance Hotspots

### Audio Thread Risks

- Temporal Deck live-to-sample allocation/copy is resolved with a circular sample view; synchronous conversion preview scanning and large expander publication remain.
- Wyrm 8x2048 wavetable rebuild.
- Sil deque mutation and deep mastering chain.
- Bifurx periodic 3x2048 analysis copies.
- Integral Flux/Proc/Undertow debug timing; TD.Scope unconditional timing.
- Bulkhead per-sample geometry/square-root work and unscaled delay calculations.

### UI / NanoVG / Render Risks

- TD.Scope, Wyrm and Bifurx maintain multiple render backends and large caches.
- Chronomaw timeline and Temporal Deck platter/scope surfaces build many dynamic paths.
- Integral Flux/Proc custom preview controls instrument draw/step paths even in production.
- SVG anchor fallbacks and fixed display densities need 25%-400% zoom validation.

### Memory / Allocation Risks

- Temporal Deck long buffers plus simultaneous decoded/prepared/live copies.
- TD.Scope raster/history vectors and GL resources.
- Wyrm sand pixel/geometry buffers.
- Sil's deque allocation portability and sample-rate rolling-buffer resize.
- Worker teardown/join/detach ownership in Temporal Deck, Crownstep and Bifurx.

## 7. Recommended Release Plan

### Phase 1: Stabilize

Disable debug packaging; remove every lock/allocation/rebuild from audio callbacks; fix Bulkhead geometry/SR timing; keep Chronomaw/Bulkhead hidden; add finite-input and serialization validation; define reset and mono/poly behavior.

### Phase 2: Optimize

Profile instance scaling and callback maxima; reduce Bifurx/Temporal Deck copies; replace Sil's deque; bound memory; consolidate render paths and test GL loss/headless fallback.

### Phase 3: Polish

Create root README/manuals, simplify development menus, fix stale assets, document latency/ranges/polyphony, and verify labels/tooltips/lights/zoom.

### Phase 4: Package

Run a clean multi-platform Rack SDK build, enabled test suite, package inspection and Rack load/save stress; validate `plugin.json`, distributable assets, licenses, release notes, URLs and checksums.

## 8. Suggested Global Tests

- Clean Linux/Windows/macOS builds with warnings reviewed; `make test`, including a repaired/enabled Chronomaw target.
- Load Rack with only the packaged plugin; instantiate every visible/hidden module and exercise browser previews/headless screenshots.
- 1/10/50-instance CPU, callback-max, UI-frame, allocation and memory tests at 44.1/48/88.2/96/192 kHz.
- Save/load and duplicate every module during active processing; compare params, JSON state, latency and outputs.
- Random cable connect/disconnect, mono/poly 1/2/4/8/16 channels, NaN/Inf injection, missing assets and missing/corrupt sample files.
- Rapid sample-rate changes, Rack reset, module delete/reorder, worker-active shutdown and expander hot-plug.
- Visual checks at 25%, 50%, 100%, 200% and 400% zoom with NanoVG fallback, GL disabled and GL context recreation.
- Package audit confirming no debug flag/logging defaults, no unlisted/missing referenced assets, valid metadata and a root manual/README.

Review evidence: direct static inspection of `src/`, `res/`, `plugin.json`, `Makefile`, `LICENSE.txt`, docs/tests; `make -j4` completed with the current build already up to date; `make test` passed all enabled fast, Rack-linked and ODR targets; `tools/validate_plugin_json_tags.py plugin.json` passed. This was not a clean rebuild or an in-Rack manual audition, so visual/audible judgments and platform behavior still require the proposed tests.

## 9. Final Recommendation

Overall status: Selective release candidate; suite-wide release should wait  
Release blockers: packaged debug mode, Temporal Deck callback-time preview scan, Wyrm rebuild, Sil RT proof, hidden-module correctness  
Highest-leverage fix: enforce and test a no-lock/no-allocation/no-timer audio-thread contract  
Best candidate module for first release: Undertow (with Bifurx close behind)  
Module most needing redesign: Chronomaw's engine/input contract; Bulkhead's geometry modulation is the most urgent localized redesign
