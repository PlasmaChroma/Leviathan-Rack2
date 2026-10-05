# Native Rack implementation handoff

**Target:** a Leviathan instrument informed by the supplied arbhar firmware analysis.  
**Status:** implementable architecture and selected tested kernels, **not a complete compatibility specification or existing Rack plugin**.  
**Read first:** [main report](ARBHAR_REVERSE_ENGINEERING.md), [parameter bible](PARAMETER_BIBLE.md), [open questions](tables/open_questions.csv).

## 1. Engineering decision

Build the audio engine in native C++ rather than wrapping the complete Pd/ARM runtime. Use the original patches as the signal-flow specification and the ARM routines as behavioral evidence. The actual update depends on Pd APIs, process scheduling, shared memory, hardware interfaces, and file scripts; a CPU emulator alone does not supply those services.

Keep three categories visibly separate in code and documentation:

1. **Recovered behavior:** exact tables, confirmed data geometry, static kernels, enum values, and traced graph connections.
2. **Compatibility approximations:** areas whose musical behavior is known but whose exact implementation is not yet recovered, such as full scheduler jitter or analog input coloration.
3. **Rack adaptations:** voltage scaling, host sample-rate policy, file dialogs, visual presentation, and explicit microphone substitutes.

Do not label this a clean-room implementation, bit-exact emulation, licensed source port, or completed arbhar clone. Those claims are not established by this analysis. Do not ship manufacturer binaries, source snapshots, or extracted assets as plugin resources merely because they are present in this research bundle.

## 2. Proposed code organization

The names and interfaces below are **recommended architecture**, not recovered original C++ types.

```text
ArbharLikeEngine
  ControlSnapshot
  PerformanceState / GestureState
  EventQueue
  LayerStore[6]                  // stereo data, metadata, generation
  CaptureEngine                 // current and retiring writer states
  GrainEngine continuous
  GrainEngine struck
  FollowTransport
  WavetableEngine
  InputConditioning
  OutputAndEffectsGraph
    Reverb
    FeedbackDelay
    PhaseAndMonoPolicy
  ImportExportWorker            // never called synchronously from process()
```

`LayerStore` owns prepared audio storage. Each grain identifies a layer generation and snapshots launch parameters. It must not dereference a buffer replaced or freed by a file worker. A simple initial approach retains the old generation until all referencing grains finish and routes disposal to the worker. A more memory-efficient strategy can follow after correctness is established.

Each `GrainEngine` owns a preallocated pool. Reserve **82 storage slots per engine** for an initial compatibility-oriented implementation, but **do not set the audible polyphony contract to 82**. Model active, retiring, available, and scheduled states separately. The unresolved original allocation/retirement policy should live behind a small replaceable strategy rather than be spread across the renderer.

The two engines need separate scheduler state and random state. The firmware uses separate Pd processes and libc randomness; a single host-global random stream is not known to reproduce it. A seeded native generator is acceptable as a labelled approximation, with seed persistence and deterministic replay.

## 3. Time and sample-rate policy

Start with a **48 kHz internal compatibility domain** for the first deterministic reference engine. This keeps recovered positions, grain lengths, capture fades, and file capacities in the same nominal sample domain while the behavior is being validated. Resample at a clearly defined boundary for other host rates, or initially restrict the research harness to 48 kHz. This is a recommendation, not a claim that the hardware's measured clock is exactly 48 kHz.

An eventual host-rate-native engine is also reasonable, but must classify every constant before converting it:

| Constant kind | Port policy |
|---|---|
| Seconds or milliseconds | Convert through the selected processing rate. |
| Buffer/frame coordinates | Preserve duration when resampling; update all associated metadata. |
| Per-sample slew coefficient | Convert by a time-constant model only after its intended domain is established. |
| Per-block gain/retirement update | Preserve a defined control/DSP tick; do not apply per host sample. |
| File rate 49,148 Hz | Treat as an interchange convention, not the global live sample rate. |
| WT correction 0.976642 | Keep isolated and selectable until its calibration role is settled. |

The observed retirement decrement is 1/64 **per perform-block visit**, while a recorder Dub slew is about 0.0078 **per sample**. Confusing these domains will produce radically different envelopes. Do not hardcode a retirement time in milliseconds until the actual block cadence on that path is established.

At nominal 48 kHz, allocate six stereo channels pairs of 624,000 frames, with the usual Scan region spanning 480,000. Keep the additional 144,000 frames available. Do not expose the entire storage capacity as the ordinary Scan range by default.

## 4. Control, event, and snapshot semantics

Use normalized or musical units at the Rack boundary, then convert once into the compatibility domain. Never scatter ADC constants such as 4095 across the module widget and DSP code. A `ControlSnapshot` should carry at least: selected playback/record layer, Scan/Follow state, duration, spray range, pitch ratio, texture coordinate, direction probability, pan policy, Dub target, onset settings, and the explicit configuration enums.

Control lifetimes must be explicit:

- **Launch-sampled:** grain pitch, position, duration, direction, and other identified per-grain settings.
- **Continuously evolving:** transport position, capture envelope, global output/effect controls, and internal clocks.
- **Gesture-staged:** Track & Hold's selected controls, committed at the documented release event.

The exact original classification of every field is incomplete. Mark uncertain fields and test them; do not automatically update every active grain whenever a knob changes.

Capture, Strike, onset, parameter commits, preset changes, and file adoption should be timestamped events. For initially ambiguous simultaneous events, choose a deterministic order, document it as an approximation, and keep the ordering centralized. The first high-priority compatibility task is replacing that approximation with a tested original-event truth table.

`StrikeCVDelay` is a genuine configurable delay: Classic is zero in its factory text, the other five named factories and the initialization fallback are ten milliseconds. Delay the intended event and latch the intended controls at an explicit time. Do not treat the setting as a cosmetic trigger-pulse offset.

## 5. Grain renderer baseline

Use `reference/recovered_tables.hpp` and `reference/recovered_kernels.hpp` to construct a research baseline. The supplied implementation is standard C++17, independent of the Rack SDK. It includes:

- static rational clipping and a four-point interpolation branch;
- squared Length/Spray mapping and duration conversion;
- the ideal equal-power Dry/Wet law;
- a Follow helper checked against the restricted instruction-text fixture;
- a static reconstruction of the 101-by-515 window bank.

For Length, normalized `u` becomes integer-like units `100000*u*u`; the player converts using 1.44 and clamps 128–144,000 nominal samples. For Spray, the units convert using 4.8. A complete grain start-position calculation is **not** supplied. The original has distinct Scan/Follow and boundary-aware placement paths; do not quietly substitute bipolar randomness and call it recovered.

The `cubic4` helper does not supply boundary policy or anti-aliasing. Design the reader so that boundary handling is an explicit function: current layer edge, end of valid captured material, tail region, reverse read, and record-head protection. Use synthetic ramps to make incorrect branches obvious.

Similarly, `sampleWindowLinear` is a safe proposed lookup, not the original window-phase implementation. Keep the texture-to-row and phase-direction logic outside the immutable bank so it can be replaced without changing the extracted templates.

Population gain must not become an arbitrary limiter. Preserve the recovered table and the identified control-rate smoothing as a candidate implementation while tracing exactly which grains count toward it. A safety limiter added for development should be bypassable and marked as an adaptation so it cannot hide incompatible gain behavior.

## 6. Capture and layer operations

Do not begin with a perpetual circular buffer. Implement finite capture into a selected layer, a stop/fade transition, and an explicit retrigger/handover path. Maintain both playback-layer selection and recording-layer selection because locking/coupling features can distinguish them.

The recorder has two writer records and fading transitions. Preserve that capability in the data model even before the exact overlap equation is reconstructed. The Dub control is not sufficiently reverse engineered to justify a claimed exact feedback formula; use a clearly named provisional blend behind a strategy interface and prioritize its replacement.

Accumulative mode should be separate from ordinary capture. Implement explicit parked write position and resume semantics only after the corresponding event tests are available. Clear, copy, undo, and scene import should carry layer-generation changes and invalidate or retire active readers deliberately rather than causing dangling references.

All file decoding/resampling, bulk copying, directory inspection, and scene serialization must occur outside the per-sample callback. The audio thread can publish completion requests to a bounded queue. It should never call `system`, run SoX, launch Pd, mount USB, or take a filesystem mutex.

## 7. Input, onset, and effects

Expose IN and ONSET/RIGHT as explicit roles. In a mono compatibility mode, ONSET is the analysis source; in stereo mode it is the second audio channel. The hardware's microphone normalization should become a documented host adaptation, not automatic microphone access. Keep input gain/analog coloration separate from digital granular processing so later measurements can replace the model.

For onset, reproduce the **connected** analysis graph: four high-pass stages at 300 Hz and the identified `bonk~` configuration. Locate the appropriate dependency implementation and its licensing before claiming fidelity. A provisional spectral-flux detector can be useful during development but must not be presented as the extracted algorithm. Threshold settings alone are not the entire onset behavior: hold time, capture state, and the six onset profiles determine the resulting events.

Translate the primary reverb and delay graph as dedicated native classes. Preserve ordering and control ramps. The patch-level rational shaper is not interchangeable with `std::tanh`. Delay feedback and reverb may be active simultaneously; configuration does not require choosing exactly one effect.

Keep the auxiliary `rev3~` branch disabled in the conservative baseline until a nonzero normal control path is identified. The reverb duck/restore event must not clear internal delay lines unless further evidence proves that behavior. The full delay macro law is an open task; a local branch equation is not yet a complete front-panel mapping.

## 8. Host interface and persistence

Use Rack voltage values, not normalized DSP samples, at ports. A proposed initial scale is one internal audio unit = 5 V. Use Schmitt thresholds 0.1/1 V and a 10 V, 1 ms pulse for the trigger output. These follow [Rack conventions](https://vcvrack.com/manual/VoltageStandards), not measured physical arbhar thresholds.

Internal grain polyphony is independent of Rack's polyphonic cables. Begin with one instrument instance and explicit handling of polyphonic inputs: summed audio or a deliberately selected channel, first-channel CV, and no automatic multiplication of six-layer engines for every cable voice. State the policy in the module documentation.

Serialize configuration, performance mode, layer metadata, and random seeds with a versioned schema. Persisting audio in a patch versus external files is a product choice; either way, support missing-file recovery without freezing the DSP thread. On preset load, distinguish preset-only, layer-only, and full-scene operations according to `LoadConfiguration` rather than assuming every load replaces everything.

The panel can integrate the hardware expansion's CV ports. A Leviathan-specific visual design and a transparent naming policy are separate from this technical compatibility work. No branded panel asset or module design has been generated in this bundle.

## 9. Implementation phases and gates

| Phase | Deliverable | Gate before proceeding |
|---|---|---|
| A — deterministic core harness | Layer geometry, selected kernels, fixed random seed, one synthetic grain | Native reference tests pass; exact frame-boundary fixtures added. |
| B — dual engines and scheduler | Separate continuous/Strike streams, pool/retirement state | Event logs explain every allocated/retired slot and all simultaneous events. |
| C — Capture/Dub/Follow | Finite capture, handover, accumulative mode, separate record layer | Ramp/impulse traces pass; no ring-wrap assumptions; all boundary cases specified. |
| D — controls and presets | Exact factory/fallback distinctions, quantisation, staged gestures | Preset JSON round-trips; control timing and enum truth tables tested. |
| E — native effects and onset | Graph translation, verified macro ranges, detector profile logic | Impulse/step responses and route transitions match chosen oracle or carry explicit approximation labels. |
| F — Rack integration | Ports, persistence, UI, asynchronous files, sample-rate support | No callback allocation/blocking; stress tests and multi-rate timing pass. |
| G — fidelity refinement | Hardware/original-process differential corpus | Audible and numerical differences documented by category, not hidden in a “quality” knob. |

A useful first sounding module can be built before every fidelity gap closes, but it should be described as an experimental recreation. Do not turn an incomplete reverse-engineering assumption into an undocumented permanent architecture decision.

## 10. Priority instructions for Codex

Start from the native reference tests and the JSON schema, not by porting the unverified C source wholesale. Keep original firmware evidence read-only. Implement a standalone event-logged engine before the Rack widget. Place every unverified transfer or lifecycle policy behind a named function with a reference to `OPEN-xx` in `tables/open_questions.csv`.

The next reverse-engineering tranche should address **OPEN-01 through OPEN-04**: allocation/retirement, scheduler timing, capture/Dub transitions, and delay macro scaling. These determine the instrument's feel more than adding extra visual features. All other implementation claims should remain bounded by the evidence grades in the main report.
