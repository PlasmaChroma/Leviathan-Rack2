# Codex handoff — a staged, evidence-led Spectraphon-inspired implementation

## Objective

Use the SP67 analysis to build a modular spectral instrument with clearly identified compatibility boundaries. This is not a request to execute or modify the uploaded firmware. The first deliverable should be a tested independent DSP core and data model, not an untestable monolithic Rack module.

Read `REPORT.md`, `DSP_SPEC.md`, `ARRAY_FORMAT.md`, `CONTROL_IO_MAP.md`, and the reference helper docstrings before implementation. Analyst names and equations are reconstructed, not original source. Where a behavior is unresolved, retain an explicit design decision rather than quietly pretending the gap is recovered.

## Non-negotiable facts to preserve in the baseline

The storage unit is 64 coefficients per spectrum. The normal active synthesis bank reaches at most 60 terms in four-term groups. The DSP math assumes 48 kHz and uses 64-frame updates. SAM is a harmonic quadrature detector bank with cascaded one-poles. Linear and planar SAO are different readers. Partials changes a polynomial recurrence, with the odd seed `C0=r²`, and has a fitted compensation curve. The linear Focus function is a bit-domain power approximation with exact bypass behavior. Save/read use different 32767/32768 scaling.

These are more important to a faithful core than copying panel cosmetics or using a fashionable generic FFT implementation.

## Proposed architecture

```text
SpectralFrame          std::array<float, 64>
SpectralArray          bounded vector<SpectralFrame>, dimensions, provenance
ArrayBank              16 slots per side; independent persistent metadata
QuadratureAnalyzer     reference generator, DC blocker, 60 detector states
ArrayReader            linear and planar modes, explicit edge policy
SpectralInterpolator   64-value current and delta vectors, internal cadence
PolynomialOscillator   odd/even basis, active-group guard, gain compensation
PhaseEngine            independent phases; unresolved links explicit
ModeEngine             standard / experimental Noise / experimental Chaos
ControlCalibration     normalized controls and CV interpretation
ClockUiState           documented transitions, independently testable
OutputStage            separate per-lane gains/converters, not a single stereo mix
ArrayFileCodec         bounded RIFF parsing and explicit normalization policy
RackAdapter            parameters, ports, serialization, UI; no filesystem in audio thread
```

Do not allocate, parse files, acquire blocking mutexes or regenerate tables inside the real-time sample path. Build Array snapshots outside that path and publish them through a bounded handoff. The exact concurrency machinery should fit the existing Leviathan engine conventions; this report did not inspect the current plugin source and does not invent filenames or claim integration work already exists.

## Phase A — data model and deterministic numerical probes

Implement the frame/bank model, table loader or independent table generator, safe WAV parser, linear reader and planar reader. Port the Python tests to C++ with explicit float32/FMA policy. Add a switch between recovered one-subtraction addressing and a redesigned safe wrap policy, and record that selection in patch state.

Acceptance: complete 64-value frames; at most 1024 frames per slot; exact imported float preservation where possible; malformed RIFF rejection; explicit warnings for the observed header inconsistency; no silent clipping of factory coefficients; reproducible interpolation at boundaries; no out-of-range reads under malformed input.

Treat the extracted factory spectra and lookup files as separately reviewable assets. Analysis possession does not settle redistribution rights. A production distribution may need independently generated waveforms and independently authored spectral presets. Do not ship the raw firmware merely because it is present in this research bundle.

## Phase B — standard oscillator

Port the recurrence and compensation curve before adding complex modulation. Expose independent odd/even phases for tests. Match the unity-radius trigonometric identities, zero-Partials muting, 60-term ceiling, four-term admission and soft-clip equation. Use the assembly addresses in `DSP_SPEC.md` to audit sign and seed choices.

Acceptance: deterministic reference vectors; no coefficient 60–63 contribution in the normal branch; correct polarity of odd/even sums; an explicit distinction between mathematical reference arithmetic and an instruction-close float32 implementation; stable output for zero input and all normalized parameters.

Do not replace the recurrence with `amplitude[h]*r^h*sin(h*phase)` and call it equivalent. Do not use a conventional Chebyshev seed of one below full Partials.

## Phase C — SAM analyzer

Implement the separate analysis reference frequency, input DC blocker and cascaded quadrature filters. Keep analyzer pitch independent of synthesis pitch. Preserve Slide/Focus coupling. Implement the literal magnitude approximation as a compatibility option and an ordinary square root as a clearly labeled alternative.

Acceptance: known-tone selectivity sweeps, amplitude-step response, phase-insensitive steady magnitudes, absence of unsafe denormal/nonfinite propagation, correct table bounds, and repeatable behavior at the pole cap. Compare both magnitude options before assuming their differences are inaudible.

For arbitrary Rack sample rates, choose and document either an internally resampled 48 kHz core or a native-rate adaptation. A native-rate adaptation needs pole/timebase conversion; merely replacing one sample-rate constant while retaining every smoothing coefficient changes behavior.

## Phase D — phase/FM, clocks, capture and output mapping

This phase needs more evidence than the current numerical core. Trace complete external/internal FM source combination, the `60*u^4` index path, even-output offsets, signed phase behavior, Follow/Sync and Sine/Sub independence. Keep a separate confidence label on inferred routing.

Capture should store spectral frames, not input waveform slices. Determine capture tick cadence, frame count, start/end semantics, wrap behavior and saturation from code or controlled hardware files. Until then, implement a declared independent capture policy instead of claiming a recovered one.

Acceptance: explicit clock state tests, simultaneous side independence, deterministic reset/sync behavior, no audio-thread disk work, stable output-lane mapping, and documented unverified analog gains.

## Phase E — Noise, Chaos and auxiliary CV

Only begin a faithful port after translating the corresponding branch completely. Current findings identify the LCG, coefficient table, filters and coupled-phase ingredients, but do not provide a verified full algorithm.

A creative mode can be implemented earlier, but must be described as an independent Noise/Chaos design. It should not be used as evidence that the firmware branch has been reconstructed. Preserve all user patch-state distinctions between faithful and experimental modes.

## Hardware/fixture experiment matrix

| Experiment | Input and controls | What it discriminates |
|---|---|---|
| Untouched saved Array | One module-saved file, before any DAW rewrite | Header anomaly, bit depth, payload shape, file naming |
| Load/save round trip | Known coefficient bank, unchanged controls | 32767/32768 scaling and save-time normalization |
| Single-tone SAM sweep | Fixed analyzer controls, slowly swept sine | Harmonic detector centers and bandwidth |
| Tone amplitude step | Fixed tone, two Focus settings | Cascaded filter response and magnitude approximation |
| Analysis/playback separation | Fixed input tone, sweep oscillator pitch separately | Independent analysis and synthesis reference paths |
| Sparse spectral bank | One nonzero coefficient at a time | Index order, active 60-term ceiling, odd/even signs |
| Partials sweep | Sparse coefficients 1, 3, 5, then mixed | Polynomial-basis behavior versus simple harmonic attenuation |
| Boundary Array | Distinct values in every frame | Linear/planar edge indexing and clock offsets |
| FM comparison | Internal bus then equivalent external signal | Source normalization, phase wiring, Sine/Sub independence |
| Capture at known clocks | Short sequence of distinct input spectra | Frame timing and capture termination |
| LF mode clocks | Known repeated clocks and missing clocks | Divider, timeout and CV-state behavior |
| Output level test | Low-level sine and near-clip spectra | Digital/analog gain, clip order and lane correspondence |

No fixture in this table is claimed to have been run. The 31 included tests are host-side mathematical/file checks, not these hardware experiments.

## Definition of done for a first useful port

The first useful port can legitimately include standard SAM and both SAO readers while leaving Noise/Chaos and physical calibration incomplete. It should expose that status directly in developer documentation, retain a reproducible reference-test suite, and avoid describing itself as bit-exact. A polished UI is not a substitute for the unresolved routing and timing work.
