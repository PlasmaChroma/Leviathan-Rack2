# Leviathan Chimera — Codex Implementation Specification

**Status:** reviewed design; Phase 0 integration proofs required, not recovered firmware<br>
**Specification revision:** 4 (Phase 0 event-history bound; unreleased DSP profile remains 1)<br>
**Prepared:** 2026-09-23  
**Target:** a new stereo module inside the existing Leviathan VCV Rack 2 plugin  
**Working model slug:** `Chimera`<br>
**Primary behavioral source:** the supplied `Morphagene_Tech_Brief.md`  
**Companion documents:** `ACCEPTANCE_TESTS.md`, `IMPLEMENTATION_PLAN.md`, `CODEX_START.md`, `reference_vectors.json`

## 0. Read this before implementing

Implement **Chimera**, an independent, Morphagene-inspired **shared-Reel, independent-record-head, multi-playback-head instrument**. Do not implement a generic granular effect and rename its controls. The sound-memory feedback topology, full-Splice endpoint, clocked origin movement, queued selection, and separation of playback rate from finite Gene duration are essential.

This specification makes deliberate numerical choices where the source brief does not establish the hardware implementation. Those choices are normative for **this software implementation**, not assertions about Make Noise firmware. Matching this specification is not evidence of bit-exact or perceptually indistinguishable hardware emulation.

The repository integration notes come from selected publicly readable files on the `expander` branch. They are not a checkout, commit-pinned audit, or a claim that the user's working branch is identical. The implementing agent MUST inspect the actual working tree, its `AGENTS.md`, build settings, and current APIs before editing. Do not change branches, reset local changes, or replace existing shared infrastructure to conform to this document.

### 0.1 Evidence labels

| Label | Meaning in this specification |
|---|---|
| **B-A** | The supplied brief reports a confirmed behavior. This spec has not independently re-established that hardware fact. |
| **B-B** | The brief presents a strong inference, such as four musical playback voices. |
| **B-C/D** | The brief identifies an estimate or unresolved primitive. |
| **D** | A concrete design decision for the Leviathan implementation. |
| **R** | Integration guidance supported by the inspected repository/SDK sources. |

The brief's A–D categories and its uncertainty must remain visible in developer documentation. Do not silently upgrade B-C/D items into compatibility claims. Hardware descriptions are grounded in the supplied brief; external reading for this specification was limited to repository and Rack integration, not a replacement hardware investigation.

### 0.2 Core decisions

- One stereo instrument, not sixteen independent polyphonic Reel engines.
- Fixed **48,000 Hz** core and Reel sample domain. Rack's host rate is adapted at the boundary.
- **8,352,000 stereo frames / 174 seconds** maximum Reel length; float32 samples; double read positions.
- **300 Splices** and **32 Reel slots**. The brief flags 99-versus-300 as a documentation conflict; 300 is our explicit choice.
- Four musical Gene slots; bounded short-lived transition cursors are allowed for de-clicking and are not additional musical voices.
- Cubic interpolation, short cosine-edge windows, logarithmic finite Gene duration, deterministic high-Morph randomness.
- A single audio-thread-owned mutable Reel. Workers never read mutable sample memory without the immutable snapshot protocol.
- Embedded patch audio by default, not absolute-path-only persistence and not base64 sample arrays in JSON.
- A split engine/module/widget/worker design, an inexpensive cached waveform, and a small Octavia semantic adapter.
- All options enumerated in the brief are implemented. Unknown hardware button gestures, byte-identical RIFF layout, and converter artifacts are not invented as confirmed behavior.

### 0.3 Non-goals

No proprietary firmware execution; no firmware download; no Make Noise logo or copied panel artwork; no bundled third-party Reel recordings; no new standalone plugin; no FFT/phase-vocoder replacement for granular transport; no sixteen-channel duplication of the sample store; no SD/FAT32 device emulation; no networking or inference engine inside the module; no wholesale refactor of Temporal Deck, Iris, Octavia, or the renderer.

Use **Chimera** as the Leviathan working title, panel/manifest name, and model slug. Morphagene identifies the source hardware and behavioral inspiration only. Before public distribution, finalize attribution text and original panel artwork. Keep public identity configuration separate from DSP types so that naming does not require an engine rewrite. This is a product boundary, not a claim of permission or legal clearance.

## 1. Behavioral contract and traceability

The following source references use headings in the supplied brief. Original citation identifiers embedded in that brief are provenance from its earlier research, not newly verified citations in this specification.

| Requirement | Brief basis | Implementation contract |
|---|---|---|
| MG-001 | Executive summary; reconstructed signal flow | Shared stereo Reel, fixed-rate forward writer, independently positioned fractional readers. |
| MG-002 | Gene-Size; engineering conclusions | At the CCW endpoint, play the complete selected Splice. Away from it, Gene lifetime is measured in output/core frames, independent of playback speed. |
| MG-003 | Vari-Speed | Signed transport, center Stop, approximately 1× at 9:30/2:30, maximum classic magnitude 2×. |
| MG-004 | Morph; `mcr1–3` | Gap → unity density → overlap → configured signed ratios and stereo motion. Four musical slots is a B-B inference adopted as D. |
| MG-005 | S.O.S.; TLA equations | `out = (1-s)*live + s*playback`; default writer captures that same pre-output-conditioning bus. |
| MG-006 | `inop` | Alternate writer captures live input only. Do not add an independent feedback control to the canonical topology. |
| MG-007 | State machines | Organize/Shift request a future selection; default commitment occurs on the primary Gene/Splice boundary. |
| MG-008 | Clock modes | Clock can advance Gene origins or control source-time traversal independently from within-Gene pitch. Exact stride is D. |
| MG-009 | Control/jack tables | Preserve every named control, three attenuverters, six continuous CVs, five gate/clock inputs, stereo audio I/O, CV OUT, EOSG. |
| MG-010 | Firmware-controlled behavior | Implement `ckop`, `vsop`, `inop`, `pmin`, `omod`, `gnsm`, `rsop`, `pmod`, `cvop`, `mcr1`, `mcr2`, `mcr3`. |
| MG-011 | Hardware, memory, files | Canonical export is stereo 48 kHz float32 WAV; sample-frame markers; 32 named Reel slots. Exact hardware RIFF conventions remain unverified. |
| MG-012 | Unspecified primitives | Numerical approximations live in a versioned compatibility profile and are covered by deterministic tests. |

Acceptance test IDs in the companion matrix map to these requirements. A release checklist MUST distinguish specification conformance from hardware comparison, which requires a physical reference unit or independently supplied measurements.

## 2. Repository integration and file ownership

### 2.1 Observed integration points

The inspected `Makefile` includes `src/*.cpp`, selected subdirectories, and separate fast/Rack-linked test groups. Its source glob is not generally recursive. `plugin.hpp` declares model pointers and `leviathanPluginUserRootPath()`; `plugin.cpp` registers models and has explicit plugin shutdown. [R1–R3]

`TemporalDeck.hpp` exposes sample load/save and several interpolation choices. Treat it as a place to inspect reusable implementation utilities, not permission to adopt its long-play transport or copy a large module wholesale. Iris demonstrates patch-asset hooks. Moirai separates compiler/engine concerns and implements `OctaviaSemanticControl`. [R4–R6]

`SpscLatestSnapshot` is explicitly one-producer/one-consumer and latest-value, not a lossless event queue. `PhonexWidget.cpp` demonstrates split panel/labels, anchor lookup, and existing Leviathan controls. Reuse these patterns only with their actual thread and lifecycle contracts. [R7–R8]

### 2.2 Proposed source layout

Keep translation units top-level to fit the current build glob. These are proposed new files, not claims that they already exist.

```text
src/Chimera.hpp                 Rack class, frozen IDs, thin API, PImpl declaration
src/Chimera.cpp                 configuration, lifecycle, process adapter
src/ChimeraTypes.hpp            fixed PODs, enums, handles, capacities
src/ChimeraProfile.hpp          profile-v1 numerical constants and transfer laws
src/ChimeraEngine.hpp/.cpp      rack-independent core coordinator
src/ChimeraControls.hpp/.cpp    normalization, smoothers, event resolution
src/ChimeraTransport.hpp/.cpp   primary cycle, selection, clock, Play state
src/ChimeraGrains.hpp/.cpp      scheduler, readers, windows, stereo accumulation
src/ChimeraRecorder.hpp/.cpp    recording and marker insertion state machines
src/ChimeraReel.hpp/.cpp        paged audio storage, bounds, immutable snapshots
src/ChimeraRateBridge.hpp/.cpp  host/core SRC and event timestamp mapping
src/ChimeraJobs.hpp/.cpp        non-RT service, file jobs, cancellation, retirement
src/ChimeraWav.hpp/.cpp         canonical WAV/markers/options import and export
src/ChimeraState.hpp/.cpp       patch schema, save snapshots, migration, history
src/ChimeraSemantic.hpp/.cpp    Octavia document/commands/status adapter
src/ChimeraWidget.cpp           controls, context menus, model definition
src/ChimeraDisplay.hpp/.cpp     waveform cache and bounded dynamic overlays
res/Chimera.svg                 editable master, labels and hidden component anchors
res/Chimera.panel.svg           generated runtime panel; never edit directly
res/Chimera.labels.svg          generated runtime labels; never edit directly
res/Chimera/...                 original optional raster assets
presets/Chimera/...             parameter presets; no unlicensed audio
manual/Chimera.md               user documentation; adapt directory to repo
```

A small responsibility may share a file with its closest neighbor when that improves clarity, but DSP, filesystem code, serialization, and widget drawing MUST remain separate. Target approximately 200–700 lines per substantive `.cpp`; a file approaching 900 lines triggers a responsibility review, not arbitrary splitting.

Register exactly one `modelChimera`, one manifest entry, and one `createModel<Chimera, ChimeraWidget>("Chimera")`. Preserve all existing module IDs, ordering-sensitive data, JSON keys, and model registrations. New enums become append-only at first release. Do not alter sibling edition identity or premium packaging unless the current checkout explicitly requires it.

Edit artwork and anchors only in `res/Chimera.svg`; regenerate with `python3 tools/split_svg_labels.py res/Chimera.svg --overwrite`, then `make generate-panel-anchor-atlas`. Follow the checkout's `AGENTS.md` if its pipeline changes.

### 2.3 Dependency rule

The engine, mapping tests, scheduler tests, and Reel bounds tests MUST compile without Rack headers or a window. Pass fixed POD input/output frames. Use current project test/compiler conventions; do not globally raise the C++ standard or add a new package manager.

Reuse an existing WAV decoder and SRC dependency only after checking its actual availability and license. The Rack SRC wrapper inspected uses Speex and constructs/destroys state when rates/settings change; those operations do not belong in `process()`. [R11] A dedicated small adapter may use the existing Speex dependency; do not add a second resampling library merely for this module.

## 3. Stable parameter and port schema

### 3.1 Parameters

All normalized primary controls use `[0,1]`. Attenuverters use `[-1,1]`. Buttons are momentary Rack parameters but generate semantic actions, not persistent transport states.

| ID | Enum | Label | Default | Meaning |
|---:|---|---|---:|---|
| 0 | `SOS_PARAM` | S.O.S. | 0 | Live/playback mix; CV attenuator when patched. |
| 1 | `GENE_SIZE_PARAM` | Gene Size | 0 | Full-Splice endpoint to shortest finite Gene. |
| 2 | `VARISPEED_PARAM` | Vari-Speed | 5/6 | Classic forward 1×. |
| 3 | `MORPH_PARAM` | Morph | 1/6 | Seamless nominal 1/1 density. |
| 4 | `SLIDE_PARAM` | Slide | 0 | Source origin offset. |
| 5 | `ORGANIZE_PARAM` | Organize | 0 | Requested Splice, or candidate Reel in Reel Mode. |
| 6 | `GENE_ATT_PARAM` | Gene CV amount | 0 | Gene attenuverter. |
| 7 | `VARISPEED_ATT_PARAM` | Vari-Speed CV amount | 0 | Rate/pitch attenuverter. |
| 8 | `SLIDE_ATT_PARAM` | Slide CV amount | 0 | Slide attenuverter. |
| 9 | `REC_PARAM` | Record | 0 | Release-resolved record command. |
| 10 | `SPLICE_PARAM` | Splice | 0 | Release-resolved marker command. |
| 11 | `SHIFT_PARAM` | Shift | 0 | Release-resolved next-Splice command. |
| 12 | `NUM_PARAMS` | — | — | Sentinel. |

Configure Vari-Speed travel at ±112.5° from noon (225° total) so its `1/6`, `1/2`, and `5/6` positions correspond to 9:30 reverse 1×, noon Stop, and 2:30 forward 1×. Other controls may retain the normal theme travel. Do not give Vari-Speed a snapping detent that compromises fine modulation. Add context actions “Forward 1×”, “Reverse 1×”, and “Stop” that set the knob appropriately for its current mode.

The initial mix of zero intentionally makes a fresh empty module useful for monitoring and recording. Loading a Reel MUST NOT silently change S.O.S.; show “Turn S.O.S. toward Reel” when playback is active but fully inaudible through the mix.

### 3.2 Inputs

| ID | Enum | Voltage convention / behavior |
|---:|---|---|
| 0 | `AUDIO_L_INPUT` | Mono source when R is unpatched. |
| 1 | `AUDIO_R_INPUT` | Stereo right; PM source in enabled/active PM mode. |
| 2 | `SOS_CV_INPUT` | Nominal 0–8 V; unpatched normalization +8 V. |
| 3 | `GENE_SIZE_CV_INPUT` | Nominal 0–8 V through attenuverter. |
| 4 | `VARISPEED_CV_INPUT` | Nominal ±4 V; mode-specific pitch scaling. |
| 5 | `MORPH_CV_INPUT` | Nominal 0–5 V, additive, no attenuverter. |
| 6 | `SLIDE_CV_INPUT` | Nominal 0–8 V through attenuverter. |
| 7 | `ORGANIZE_CV_INPUT` | Nominal 0–5 V, additive, no attenuverter. |
| 8 | `CLOCK_INPUT` | Rising edges; ≥2.5 V high. |
| 9 | `PLAY_INPUT` | Gate plus rising-edge behavior; unpatched = high. |
| 10 | `REC_INPUT` | Rising-edge record toggle request. |
| 11 | `SPLICE_INPUT` | Rising-edge marker request. |
| 12 | `SHIFT_INPUT` | Rising-edge next-Splice request. |
| 13 | `NUM_INPUTS` | Sentinel. |

All inputs consume **channel 0 only**. Do not sum polyphonic channels, silently treat a poly cable as stereo, or duplicate the engine. Declare `polyphony: 1` in capabilities. An optional future poly input convention requires a new explicit mode and tests; it is not implicit v1 behavior.

### 3.3 Outputs and lights

| ID | Output | Contract |
|---:|---|---|
| 0 | `AUDIO_L_OUTPUT` | Left, one channel, 5 V per internal unit. |
| 1 | `AUDIO_R_OUTPUT` | Right, one channel, 5 V per internal unit. |
| 2 | `CV_OUTPUT` | Envelope or primary Gene ramp, clamped 0–8 V. |
| 3 | `EOSG_OUTPUT` | Boundary pulse, 0 or 10 V. |
| 4 | `NUM_OUTPUTS` | Sentinel. |

Use fixed light IDs: `REC_LIGHT=0`, `REC_ARMED_LIGHT=1`, `PLAY_LIGHT=2`, `PENDING_LIGHT=3`, `CLOCK_LIGHT=4`, `PM_LIGHT=5`, `IO_BUSY_LIGHT=6`, `CLIP_LIGHT=7`, `ERROR_LIGHT=8`, `NUM_LIGHTS=9`. A Splice/Reel color belongs in display metadata rather than hundreds of Rack lights. State MUST also be readable as text; do not communicate recording, clipping, or errors only by color.

Add compile-time ID assertions and a manifest/schema golden test.

## 4. Units, numerical safety, and invariants

```cpp
namespace chimera {
constexpr uint32_t kCoreRate = 48000;
constexpr uint32_t kMaxReelFrames = 8352000;
constexpr uint16_t kMaxSplices = 300;
constexpr uint8_t kReelSlots = 32;
constexpr uint8_t kMusicalVoices = 4;
constexpr uint16_t kPageFrames = 256;
constexpr uint32_t kMaxPages = 32625;
struct StereoFrame { float l; float r; };
struct Region { uint32_t begin; uint32_t end; }; // [begin,end)
}
```

Positions inside a Reel use sample frames, never interleaved scalar indices or bytes. `length = end - begin`; valid audio is `[0, validFrames)`. Empty Reels have `validFrames=0`, zero Splices, no active readers. Nonempty Reels have an implicit marker at frame 0. Up to 299 further markers create up to 300 regions. Frame `validFrames` is an end sentinel, not an additional Splice marker.

Markers MUST be strictly ascending, unique, and `<validFrames`. One-frame Splices are valid. Every interpolation tap wraps within the voice's own retained region; never read an adjacent Splice to obtain an interpolation guard sample.

Use double for fractional read coordinates, clock phase, and fractional onset accumulation. Float32 positions near the end of an 8-million-frame Reel are not an acceptable slow-speed accumulator. Use 64-bit monotonic frame/event counters. Serialization exposes large counters as decimal strings when a JSON client could lose integer precision.

Convert Rack audio volts to internal audio by dividing by 5. Normalize L-only input to both channels; R-only remains `(0,R)` in ordinary mode. Neither patched is zero. This jack normal is D because the brief does not establish the exact hardware normalization. On the bypass path retain the same audio-only mono normal, with no gain, S.O.S., or PM effects.

Reject/sanitize non-finite values before math or addressing. Invalid sample values become zero; invalid controls use the last finite value, or the default before first valid input. Accumulate an error counter, not log messages from the audio thread. Production builds must preserve finite checks even if other plugin DSP uses fast-math; apply TU-local flags or bit-level finite tests where necessary.

There is no ±1 hard clipping in the Reel. Use a final ±12 V Rack output guard and a separate internal ±64-unit emergency recording guard with a sticky overload flag. These are D safety limits, not codec emulation. Reapply the finite ±12 V guard after host output SRC so reconstruction overshoot cannot violate the Rack-facing limit. No hidden limiter, saturator, or DC-removal pass is applied to loaded WAV samples.

## 5. Signal path and frame ordering

```text
Rack host L/R -> input rate bridge -> mono normal -> selected gain -> 5 Hz input DC blocker
                                                                          |
                                            Reel readers -> grain sum ----+-> S.O.S. bus
                                                                          |      |
                                          inop=1: live only ---------------|      +-> inop=0 writer
                                                                          |      |
                                                                          +------+
                                                                                 |
                                                           5 Hz output DC blocker + voltage guard
                                                                                 |
                                                                 output rate bridge -> Rack L/R
```

The writer's default tap is the **post-S.O.S., pre-output-DC-blocker, pre-output-guard** bus. The CV energy follower observes the final core audio bus after output conditioning, before host SRC. PM is sourced from the right input audio stream before input gain and DC filtering; it is a coordinate signal, not a recording source.

At each core frame, execute this order:

1. Adopt ready fixed-size control commands and safe transaction handles; reject stale generations.
2. Consume timestamped input events for this frame; resolve record requests before same-frame Clock edges.
3. Apply immediate transport actions; perform scheduled primary-boundary transitions; create due voices.
4. Read **all** musical and transition cursors from the pre-write Reel state; sum playback.
5. Compute the S.O.S. bus and observable audio/CV; enqueue timestamped EOSG events.
6. Before a write, execute the page snapshot/write barrier, then write exactly one stereo frame if recording.
7. Advance reader positions, lifetimes, source trajectory, and recording cursor; schedule resulting boundary actions for the next frame.
8. Publish bounded telemetry and perform bounded snapshot/retirement maintenance when due.

There is no algebraic feedback loop: at a colliding address, playback reads the old frame before recording overwrites it. Do not change that ordering for SIMD convenience. The TLA reference test depends on it.

DC blocker: `y[n] = x[n] - x[n-1] + R*y[n-1]`, `R=exp(-2*pi*5/48000)`. Provide a developer test switch that bypasses input/output conditioning to permit exact buffer recurrence tests. The default 5 Hz pole is D, not a measured hardware high-pass cutoff.

Input gain choices are `-3 dB`, `0 dB (modular)`, `+6 dB`, `+12 dB`. The software interprets “modular level” as 0 dB; the brief does not establish its exact analog gain. Smooth gain changes over 5 ms. Do not claim to reproduce codec PGA noise behavior.

Unless a cosine window or one-pole is explicitly specified, a K-frame fade/crossfade is linear: frame j=0..K-1 uses blend `(j+1)/K`, reaching the target on the Kth frame. Use K=48 for 1 ms and K=240 for 5 ms. An interrupted fade restarts from its current gain/value; no stack of fade jobs is created. This applies to gain, record-source routing, PM routing, stop tails, and store-swap fades.

## 6. Host sample-rate bridge

### 6.1 Required behavior

Store and process Reel data at 48 kHz regardless of Rack rate. A 1-second 48 kHz Reel remains one second long and at the same pitch in a 44.1/96/192 kHz patch. Recording 10 seconds captures 480,000 frames, apart from the explicitly defined gate-boundary quantization. Host-rate changes never reinterpret existing Reel bytes at a different rate.

At exactly 48 kHz, use a direct single-frame path: no SRC state, no batching latency, one core frame per host frame. At other rates, use preallocated input/core/output FIFOs and prepared stereo SRC states. Initial target: Speex quality 5; measure and expose the resulting latency, not an invented fixed number.

Support positive finite integer host sample rates from 8,000 through 768,000 Hz, with required release tests at 44,100, 48,000, 88,200, 96,000, 176,400, and 192,000 Hz. Reject unsupported/fractional rates explicitly rather than silently round and drift. This range is a software support policy, not a statement of every rate Rack exposes in its UI.

### 6.2 Preparation and bounded work

Prepare both converters, latency information, and FIFO capacity off `process()`. Do not call SRC `setRates`, `setQuality`, constructors, or destructors inside the sample callback. The inspected Rack wrapper refreshes allocated state for those calls. [R11]

On `onSampleRateChange`, publish the requested rate/generation to the module service. Until a matching prepared bridge is adopted, pause core recording/transport, cancel clock-armed requests, and fade output to zero. Do not write wrongly resampled samples. Display “Preparing sample rate”; a rate change is allowed a finite audible interruption. State migration preserves Reel bytes, markers, options, current Splice, and fractional positions in core units. The old bridge is retired off-thread.

Use capacity 32,768 frames per audio FIFO and a correspondingly bounded control/event history. At a non-48 kHz host callback, feed the available frame, service up to 32 new core frames, and up to 64 output SRC frames; the supported ratio needs at most six core frames per host frame in steady state. Converter-consumption counts, not guessed ratios, govern queue removal. Never pad a steady-state underrun with unmarked invented input samples. Zero output, increment a diagnostic, and enter controlled recovery if a prepared bridge fails the steady-state contract.

Priming zeros are allowed before the first real input. Do not discard real samples to conceal startup latency. The test harness verifies both channel alignment and exact cumulative frame counts.

### 6.3 Controls and edges must share the audio time axis

Detect gate transitions at the host rate so short host pulses are not missed. Maintain host-frame timestamps, including plug/unplug events. Obtain the input SRC group delay from its API and verify it with an impulse. If the delay expressed in core frames is `dIn`, an edge at host frame `h` maps to:

For bounded event admission, represent a changed jack by one record per jack per host frame containing its connection state and gate level/edge; a connection and edge changing together do not create two queue entries. Continuous control samples use a separate timestamped history. At the supported maximum 768 kHz, 16 host frames map to one 48 kHz frame, so even all 13 inputs changing on each host frame produce at most 208 jack records per core interval, below the 256-event processing cap. Reserve at least 16,384 jack-event entries for input SRC delay history. The installed quality-5 Speex probe measured a maximum 640-host-frame input latency at 768 kHz; `13*640=8,320` delayed jack records, leaving capacity for scheduling margin. Re-measure if converter quality or implementation changes; reject an unsupported configuration whose measured bound exceeds the prepared queue instead of dropping events.

```text
coreEventFrame = ceil(h * 48000 / hostRate + dIn)
```

Use rational/integer accumulation to avoid long-run drift; the equation states the intended timing. Core continuous controls are interpolated from host values at the corresponding delayed source time. Do not linearly interpolate gates, run Schmitt triggers on FIR-filtered gate voltages, or sample only the newest knob value for a whole queued block. Connection state is timestamped and held, not interpolated.

EOSG pulses and CV ramp phase are returned on the **same effective output timeline as audio**, including output SRC latency and FIFO startup delay. Use timestamped pulses for EOSG and a bounded held/linear CV bridge; do not FIR-filter 10 V gates into ringing voltages. Test `CV OUT` and EOSG alignment with audible Gene boundaries at every required host rate.

The clocked recorder's pulse-to-audio relationship must remain invariant after subtracting the documented common adapter latency. This is a release gate, not a later refinement.

## 7. Controls and profile-v1 transfer laws

All formulas below are **D** unless their behavioral intent is tagged B-A. For nonnegative frame counts, `round(x)` means `floor(x+0.5)`, matching C++ `std::round`, not Python ties-to-even rounding. Put their constants in `ChimeraProfile`, serialize `dspProfile=1`, and do not change the sound of saved profile-1 patches when tuning a later profile.

### 7.1 Normalization and smoothing

```text
sosTarget   = clamp01(kSOS * (sosPatched ? V_SOS / 8 : 1))
geneTarget  = clamp01(kGene + aGene * V_Gene / 8)
slideTarget = clamp01(kSlide + aSlide * V_Slide / 8)
morphTarget = clamp01(kMorph + V_Morph / 5)
orgTarget   = clamp01(kOrganize + V_Organize / 5)
```

Raw negative voltages on nominally unipolar inputs are accepted by the software's additive laws and then parameter-clamped. This is a deliberate Rack convenience, not a claim about safe electrical operation or hardware over-range behavior. Clamp incoming control voltages to ±24 V before further calculations; non-finite values follow section 4.

Use 48 kHz one-pole smoothing `x += (1-exp(-1/(tau*48000)))*(target-x)`: S.O.S. 1 ms; Gene control 2 ms; classic rate-control coordinate 1 ms; exponential pitch CV 0.25 ms; Morph 2 ms. Slide has the explicit slew in section 10. Organize uses hysteresis rather than a time smoother. No ordinary gate or PM signal passes through these control smoothers.

Settle a smoother exactly to an unchanged target when its normalized error falls below `1e-7`. Full-Splice uses the hysteresis in section 7.5; Stop uses the exact deadband in sections 7.3/7.4, without an additional unspecified hysteresis. Initialize smoothers to the first sanitized control observation. In pitch modes smooth the knob coordinate with 1 ms and attenuated/clamped pitch volts with 0.25 ms before rate mapping. In tests, evaluate both raw transfer functions and runtime smoothed behavior.

### 7.2 S.O.S.

`bus = (1-s)*live + s*playback`, independently for L/R. No equal-power curve and no second independent feedback coefficient. CV insertion changes the knob from a base mix to a CV amount. Test unpatched, patched 0 V, 4 V, 8 V, negative, and over-range cases.

### 7.3 Classic Vari-Speed (`vsop=0`)

```text
x = clamp(2*kRate - 1 + aRate * V_Rate / 4, -1, +1)
epsilon = 0.0001
q = max(0, (abs(x)-epsilon)/(1-epsilon))
qUnity = (2/3-epsilon)/(1-epsilon)
gamma = log(0.5)/log(qUnity)
rate = sign(x) * 2 * pow(q, gamma)
```

Set rate exactly zero for `abs(x)<=epsilon`. This curve gives ±2 at the extremes and ±1 at normalized knob 1/6 and 5/6, with greater resolution near zero. It intentionally does not turn the brief's approximately −26-semitone useful range into a minimum nonzero speed; rates approaching Stop remain available.

An optimized implementation may use a precomputed monotonic lookup table if max absolute rate error is `<1e-5` and the named anchor values remain exact. Do not evaluate `pow` for unchanged controls every sample.

### 7.4 One-volt-per-octave modes (`vsop=1,2`)

Bidirectional mode uses the classic **knob-only** rate `r0=classic(kRate,0)`. Forward-only uses:

```text
if kRate <= epsilon: r0 = 0
else:
    q = (kRate-epsilon)/(1-epsilon)
    qUnity = (0.75-epsilon)/(1-epsilon)
    r0 = 2 * pow(q, log(0.5)/log(qUnity))
```

Thus forward-only unity is knob 0.75, Stop is full CCW, and maximum knob-only speed is 2. Both modes then use:

```text
pitchVolts = clamp(aRate * V_Rate, -8, +8)
rate = clamp(r0 * exp2(pitchVolts), -32, +32)
```

CV changes magnitude but does not flip the knob-selected direction in bidirectional pitch mode. At knob Stop, finite pitch CV cannot restart playback. These summing/Stop choices are D because the brief does not establish them. An attenuverter value of +1 gives exact software 1 V/oct without physical trim. No undocumented hardware tracking precision is claimed.

The 32× pitch-mode cap is a software safety/performance choice distinct from the classic ±2× cap. With high-Morph ratios, clamp final per-voice magnitude to 512×. Report rate limiting in telemetry.

### 7.5 Gene Size

Enter full-Splice mode at `uGene<=0.0001`; leave at `uGene>=0.0002`. Empty Reel is a separate state. Full-Splice mode's primary cycle tracks **source traversal**, so its duration follows Vari-Speed. Do not apply the finite-duration rule to that endpoint.

For a finite Gene in a Splice of `L` frames:

```text
Nmin = min(16, L)
Nmax = L
Ng = clamp(round(exp(log(Nmax) + uGene*(log(Nmin)-log(Nmax)))), 1, L)
```

The minimum is 16 core frames (about 0.333 ms) unless the Splice is shorter. This is D; the source supplies no exact minimum. Finite Gene duration is latched at voice onset. Speed changes during that Gene alter source excursion, not its scheduled expiry. The primary timing cycle uses the duration captured at its own start.

Changing Gene Size changes newly scheduled voices and the next primary cycle. For a drastic size reduction, apply bounded voice replacement rules, not a dynamically growing pool. At full-Splice→finite or finite→full mode transition, reset primary scheduling at the current source coordinate with the normal transition envelope.

## 8. Sample reader and grain envelopes

### 8.1 Cubic interpolation

Read four **region-wrapped** samples `a,b,c,d` at integer offsets −1,0,+1,+2 around `floor(position)`. For fraction `t` use:

```text
out = b + 0.5*t*(c-a + t*(2*a-5*b+4*c-d + t*(3*(b-c)+d-a)))
```

Apply the identical fractional coordinate to L/R. Handle one-, two-, and three-frame regions through wrapped taps, not special unsafe memory loads. Cubic overshoot is permitted. The interpolator is not a band-limited variable-rate converter; do not advertise it as alias-free.

For v1 there is one interpolation mode, explicitly called **Classic cubic**. A future higher-quality reader can be added as a versioned option, but changing host SRC quality must never silently change the per-Gene kernel. Do not attempt to remove existing source aliasing with a post-read low-pass and call that anti-aliasing.

For a coordinate `p`, wrap with `a + ((p-a) - floor((p-a)/L)*L)`. Use a fast in-range branch and one general fallback, not an unbounded while loop. Individual cursor increments, PM excursions, and imported markers must remain safe even for a one-frame region and a 512× rate.

### 8.2 Finite Gene window

For a voice of `N` samples and age `j=0..N-1`:

```text
short edge: E = min(floor((N-1)/2), max(1, round(0.002*48000)))
smooth edge: E = min(floor((N-1)/2), max(shortEdge, round(0.20*N)))
if N <= 2 or E == 0: w = 1
else:
    edgePhase = clamp(min(j/E, (N-1-j)/E), 0, 1)
    w = 0.5 - 0.5*cos(pi*edgePhase)
```

`gnsm=0` selects the short edge; `gnsm=1` selects the smooth edge. This is a cosine-edged flat-top window, **not** a full-length Hann by default. At tiny Genes it naturally becomes almost all envelope. Allow N≤2 as a safe discrete special case; there are insufficient samples to produce conventional fade edges.

Precompute a 2,049-entry half-cosine table off-thread, or use a measured approximation with `<1e-5` amplitude error. The normative formula is the reference.

### 8.3 Full-Splice behavior

The primary full-Splice cursor retains its region and fractional coordinate. Its cycle completion is based on integrated absolute base-rate travel reaching `L`, not `L` output frames. Maintain fractional excess so wrapping does not create tempo drift. Slide/PM address offsets do not increment this cycle-travel counter.

For envelope evaluation, estimate the current full-Splice wall duration from `L/max(abs(baseRate),1e-6)` and map source-travel phase to that window. At unity density and `gnsm=0`, use the explicit plateau and residual boundary correction in section 8.5. At `gnsm=1`, keep the audible smooth window. A forced retrigger may use the fixed transition cursor in section 8.4; a natural unity wrap does not require an extra musical onset before its actual boundary. At `r=0`, retain the last valid finite edge estimate and suspend full-Splice natural completion.

At other densities, use the estimated full-Splice wall duration `L/max(abs(baseRate),1e-6)` as the scheduler's duration estimate; musical slots complete by source-travel distance. Clip only the scheduler estimate, not actual read position. Stop mode prevents spontaneous long-lived voice accumulation.

Clamp that scheduling duration estimate to `[1, L/1e-6]` core frames. Each full-Splice musical voice expires after its own absolute effective-rate travel reaches its retained region length; one expiring voice emits one completion, even if its last increment crosses multiple wraps. The independent primary cycle uses base-rate travel, retains modulo-L remainder, and resolves at most one boundary transition per core frame. Multiple source wraps within one frame cannot create extra sample-time onsets. At base-rate Stop, suspend full-Splice launches as well as travel; active PM may continue reading retained stationary cursors.

### 8.4 Stop, reversal, and transition cursors

When the effective base rate is zero and PM is inactive, fade wet playback to zero over 48 core frames while preserving cursor coordinates. Finite Gene timers and external clock interpretation may continue; full-Splice natural boundary generation does not. Recording and live monitoring are independent and continue. With PM active, stationary-cursor address modulation may remain audible; this is explicitly a D behavior.

Crossing the sign of rate keeps current cursor positions and reverses subsequent increments. It does not seek to a Splice endpoint. Existing finite Genes keep their expiry times. Starting a new reverse voice uses `wrap(origin-1)` so a default origin at the Splice start begins at its final source frame.

Use four fixed musical slots. Each slot can retain one outgoing 48-frame transition cursor when replaced. There are at most **eight sample-reading cursors**. If another replacement arrives during a tail, replace the previous tail with a 48-frame scalar residual fade from its last output, rather than allocating another cursor. This slight emergency transition approximation is D and has a stress test. `omod=1` immediate Organize deliberately bypasses this extra crossfade.

### 8.5 Dynamic Enveloping at unity density

Back-to-back windows that both reach zero would create an audible dip at the nominal seamless setting. Define this case explicitly rather than asking the implementer to guess a crossfade. Let the base window be `w` from section 8.2 and use:

```text
unityBlend = gnsm == 0 ? clamp01(1 - abs(density-1)/0.025) : 0
effectiveWindow = (1-unityBlend)*w + unityBlend
```

This weight exists only during a voice's actual lifetime. Therefore D<1 still has its scheduled gap, while exactly D=1 and `gnsm=0` has a unity plateau. Use `effectiveWindow` in both the sample weight and the common normalization denominator. `gnsm=1` intentionally retains its audible edge fades, including at D=1. Each voice latches `gnsm`, including its unity-blend eligibility, at onset; existing voices retain their window policy when the option changes. Ordinary Morph smoothing covers density motion. The boundary residual uses the incoming primary cycle's window policy.

At a natural boundary/onset in this near-unity region, compensate the remaining address discontinuity **after** stereo accumulation and normalization, before S.O.S. Let `newRaw` be the first newly rendered stereo playback frame without this correction and `previous` the last emitted wet frame, including any prior correction. Set `residual = previous-newRaw`; for ages `j=0..E-1` add:

```text
E = min(96, floor((estimatedCycleFrames-1)/2))
correction = unityBlend * residual * (1 - (0.5 - 0.5*cos(pi*j/E)))
```

At j=E the correction is zero. Disable this correction for E<1. Starting from silence uses `previous=0`. A new qualifying boundary replaces the residual from the currently emitted frame; it never stacks an unbounded number of residuals. Natural unity boundaries use this correction instead of a simultaneous transition-cursor crossfade, so two separate de-click methods do not compound. Forced retriggers use section 8.4; `omod=1` bypasses both. No source samples are read before their scheduled onset.

This is a bounded, causal **software Dynamic Enveloping approximation**, not an asserted hardware algorithm. A constant source at D=1 must have no periodic amplitude hole after startup; a discontinuous loop is continuous at its boundary sample and approaches the new source over E frames. A changing source can still have a derivative or timbral transition. Do not claim mathematical reconstruction of a smooth loop from arbitrary source material.

Clear the residual whenever no musical voice contributes, including scheduled D<1 gaps and a stopped transport. Do not use this correction to fill a no-voice gap. A new onset following a gap uses `previous=0`.

## 9. Morph scheduler and stereo behavior

### 9.1 Density curve

Let `m` be smoothed Morph. Piecewise-linear density `D(m)` has these anchors:

| m | D | Region |
|---:|---:|---|
| 0 | 0.90 | Small gap between Gene envelopes. |
| 1/6 | 1.00 | Nominal 1/1 continuous stream. |
| 1/2 | 2.00 | Nominal twofold overlap; hybrid clock threshold. |
| 5/6 | 3.00 | Upper untransposed overlap region. |
| 1 | 4.00 | Maximum four-slot density and chord/pan behavior. |

Starting/retriggering a nonempty transport emits one onset immediately, starts the primary cycle, and resets onset phase to zero; do not wait an entire hop before first sound. Reset the next musical slot to 0 on an explicit Play retrigger or committed Splice change, but do not reset the PRNG seed/state until module transport initialization or an explicit seed reset. Schedule hop `H = max(1, durationEstimate / D)`, in core frames. Maintain a double onset phase accumulator. Advance by `1/H` per core sample; emit an onset when it crosses 1 and retain the remainder. There is at most one normal onset per core frame. At steady finite parameters, the primary cycle has duration `Ng`; staggered onset rate is approximately `D*48000/Ng`.

At density 1 and `gnsm=0`, apply section 8.5 rather than placing two zero-ended windows back-to-back. For D<1, preserve actual no-voice gaps. For D>1, overlap musical slots; do not create an extra always-on dry Reel voice.

Primary cycle and musical slot identity are different concepts. The primary cycle supplies Organize/Play commitment and CV ramp timing. **Do not commit a pending Splice at whichever high-ratio secondary voice happens to finish first.**

Maintain a non-reading primary cursor for marker capture and the main playhead. At each primary-cycle start, initialize it to that cycle's Slide/trajectory origin (reverse uses `wrap(origin-1)`); advance by base rate after rendering and apply Slide delta once. It retains the primary region, excludes PM and chord ratios, and holds its last address while transport is stopped. It adds no audio voice. At an ordinary finite primary boundary it resets to the current origin even when the Morph onset schedule is between onsets.

### 9.2 Chord ratios and deterministic randomness

`high = clamp01((m-5/6)/(1/6))`. At each musical onset, consume exactly two PRNG draws regardless of Morph setting. First draw selects chord activation with probability `high`. Second draw selects pan in `[-high,+high]`.

Assign musical slots round-robin 0..3. Slot 0 has ratio 1. For slots 1..3, use their configured `mcr` ratio when chord activation succeeds, otherwise 1. At Morph=1 every eligible slot uses its configured ratio. Thus the high region introduces increasingly frequent configured transpositions rather than inventing undocumented continuous random pitch intervals. Ratio sign is retained; negative × negative yields forward playback. Latch ratio and pan at onset. Changing `mcr` settings affects new onsets only.

Use `xorshift32` with seed `0x6D2B79F5` by default:

```text
x ^= x << 13; x ^= x >> 17; x ^= x << 5;  // uint32 wraparound
uniform = (x >> 8) / 16777216.0
```

Zero seed is remapped to the default. Record the user seed in patch state, not live PRNG state. Reload restarts the reproducible sequence; it does not restore sample-exact live transport. Do not use wall-clock time or Rack's global random generator in this DSP path.

### 9.3 Stereo panning and overlap normalization

Preserve stereo channels. Apply a stereo **balance**, not a mono fold-down:

```text
gL = sqrt(2)*cos(pi*(pan+1)/4)
gR = sqrt(2)*sin(pi*(pan+1)/4)
```

At center, both gains are 1; hard side suppresses the opposite channel and boosts the retained channel by sqrt(2). This balance law is D. Pan does not swap the channels or reduce the original stereo stream to mono.

Accumulate windowed, balanced voices. Use a common stereo denominator `max(1, sumOfMusicalEnvelopeWeights)` so overlap does not multiply the gain of identical coherent copies. Do not normalize each channel independently or include a side's sample amplitude in the denominator. Transition replacement gains are folded into their slot's total contribution so tails do not cause an unintended gain rise. This normalization preserves the explicit gap when aggregate window weight is below 1.

A mono constant fixture with centered voices should remain unity during an overlapped plateau, not grow fourfold. Highly decorrelated grains may sound quieter; this is a deliberate stable-feedback baseline rather than an unknown hardware gain law.

## 10. Slide and source-time trajectory

Use a wrapping region model, not endpoint-fitting by subtracting the rate-dependent source window length. The brief leaves that choice unresolved.

```text
slideTargetFrames = uSlide * (L-1)
slideActual += clamp(slideTargetFrames-slideActual, -64, +64) // per core tick
origin = region.begin + slideActual + trajectoryOffset
```

The 64-source-frames-per-core-frame limit corresponds to scanning 64 seconds of source per second; a full 174-second move can take roughly 2.72 seconds. Small-region changes are much faster. This is D, not a measured hardware scan rate. Do not multiply that speed by Vari-Speed.

Each frame's change in `slideActual` offsets existing musical cursors as well as new origins, within their retained regions. Otherwise Slide would be inaudible until the next long Gene begins. Apply the delta once; do not repeatedly overwrite every cursor with the absolute origin and thereby destroy playback progression.

In free unclocked finite-Gene operation, `trajectoryOffset=0`: new Genes revisit the Slide-defined origin. Their within-Gene reads advance at their own rates. In full-Splice operation, playback naturally traverses the region.

Clocked modes add a trajectory relative to this anchor. When a pending Splice commits, reset the trajectory into the new region and initialize its Slide value from the current normalized control. Rebase cursors during mode changes with the normal transition policy; never carry an absolute old-Reel address into a newly loaded Reel.

## 11. Selection, primary cycles, and Play

### 11.1 Organize and Shift arbitration

For N>0, raw Organize selection is `min(N-1, floor(N*uOrganize))`. Equal bins are independent of Splice duration. Use hysteresis of **10% of one bin** on either side of the current bin boundary. Large jumps directly choose the target bin; do not walk through every intermediate Splice.

Maintain separate `organizeBin`, `requestedSplice`, and `currentSplice`. An Organize **bin change** sets `requestedSplice=organizeBin`. A Shift edge sets `requestedSplice=(requestedSplice+1)%N`. A stationary knob must not overwrite Shift's result on the next sample. If both occur at one timestamp, resolve Organize first and Shift second.

Under `omod=0`, commit at the next primary Gene/Splice boundary. Under `omod=1`, commit immediately and do not add the normal splice-change fade; a click is expected and documented. A Play retrigger commits a pending selection immediately before restarting at Slide, including under default `omod=0`.

On commitment, initialize scheduling/primary phase in the new region. Retire prior-region musical voices using their bounded transition slots rather than leaving long old Genes audibly active; `omod=1` hard-retires them without this fade. When stopped, selection may commit immediately to make the next playback destination visible, without producing sound. Marker edits preserve selected Splice identity by stable marker ID, not merely a shifting array index.

### 11.2 Play modes

A Schmitt gate uses high threshold 2.5 V and low threshold 1.0 V. The low threshold is D. On first initialization, a patched low gate is low, a patched high gate is high, and an unpatched gate is normal-high. Normal-high enables autoplay when a nonempty Reel becomes ready; it is not a synthesized cable edge every frame.

| `pmod` | Rising/high | Falling/low | Unpatched |
|---:|---|---|---|
| 0 | Rising restarts at requested Splice + Slide; high permits looping. | Set `stopAtPrimaryBoundary`; low at startup keeps playback stopped. | Continuous playback. |
| 1 | Rising starts/restarts immediately. | Stop launch activity immediately, with at most a 48-frame de-click tail. | Continuous playback. |
| 2 | Rising commits requested Splice and retriggers. | No stop action. | Continuous playback. |

“Immediately” means on the mapped core event frame; a de-click tail does not advance musical transport or emit EOSG. Existing outgoing tails retain their old region until finished. End-of-cycle stop retires all voices coherently, not just one slot.

Unplugging PLAY changes its logical normal to high. If the previous logical state was low, treat that as a single rise. Inserting an already-high cable while normal-high remains high does not add a spurious retrigger. Plugging low triggers the mode's falling/low behavior.

## 12. Clock behavior and pitch/time separation

The brief establishes mode roles but not exact source stride, estimator, phase correction, or timeout. This section is the explicit D reconstruction.

### 12.1 Estimator

Keep `lastEdgeFrame`, `periodFrames`, `havePeriod`, and connection state. First edge establishes phase. Second edge establishes period. Each subsequent valid period replaces the old period immediately; no automatic half/double-tempo correction. Ignore intervals shorter than two core frames with a diagnostic. Use a supported period range of 2..2,880,000 core frames (up to 60 seconds). No audio-rate smoothing of Clock edges. A Clock event does not itself start a Play-stopped transport.

An interval above 2,880,000 resets `havePeriod`; that edge becomes a new first edge and still performs recording/Gene Shift actions once. An interval below two frames is rejected by the playback estimator/Gene Shift action without moving its last accepted edge. Recording quantization still responds to each detected rising edge. Timeout before any period exists is 48,000 frames after connection/first edge. These are separate estimator and record-event policies.

Connected is not the same as running. Recording remains clock-armed while the cable is connected, even if the clock has stopped. Playback stretch reports `clockWaiting` after `max(2*period, 48000)` frames without an edge; freeze its source trajectory rather than silently switch to free mode. On a new edge, re-lock. Disconnecting Clock cancels armed record changes without starting or stopping recording; playback returns to free trajectory mode with a short transition.

### 12.2 Effective playback clock mode

`ckop=1`: Gene Shift. `ckop=2`: Stretch. `ckop=0`: Gene Shift at density ≤2 and Stretch above 2. Add a density hysteresis band ±0.02 around 2 for hybrid mode only. Serialize `ckop`, not the currently inferred side of the threshold.

### 12.3 Gene Shift

A Gene Shift edge is an explicit primary-cycle boundary for queued selection and boundary-sensitive Play stopping: resolve those first, then begin a fresh primary cycle if playback remains enabled. This is D. Merge a coincident natural primary boundary and Clock-forced boundary into one transition; only actually natural voice completions emit EOSG. On each valid rising edge that leaves playback enabled, advance `trajectoryOffset` by `direction * stepFrames`, wrapping in the resulting current Splice. `stepFrames` is the current finite primary Gene length; for full-Splice mode, it is L. Direction is the sign of base Vari-Speed; at Stop use the last nonzero direction, initially forward.

Request one immediate onset at the new origin. Reset the onset phase to avoid a duplicate normal onset that sample. Between edges, normal Morph scheduling continues from that origin. This produces repeatable clock-stepped source slices. In full-Splice mode, a shift wraps by L and therefore retriggers the same region rather than skipping to a different Splice; SHIFT, not CLK, changes Splices.

### 12.4 Stretch

At a Clock edge, capture `stepFrames` as above. Once a period P exists, source trajectory velocity is:

```text
sourceFramesPerCoreFrame = direction * stepFrames / P
```

Per-Gene read velocity remains `baseRate * chordRatio`. Origins advance according to the clock trajectory; pitches do not enter its velocity. The next Clock edge establishes a new anchor one captured step beyond the previous anchor. Correct accumulated phase error at that edge by rebasing new Gene origins; old Genes keep their own cursors and finish normally. Morph still determines onset overlap. A Clock edge in Stretch does not add an extra forced musical onset.

Before two edges establish a period, hold the trajectory. Changes to finite Gene Size affect the next edge's step and newly started Genes, not a retrospectively reinterpreted past interval. Vari-Speed sign reverses source traversal; its magnitude does not. At full-Splice size, one clock interval represents one nominal traversal, although long-grain pitch/time separation will be coarse; no hidden finite grain size is substituted.

This step law gives a concrete baseline for measurement. It is not a claim that hardware uses “one Gene of source per clock.” Include that unresolved point prominently in the calibration checklist.

## 13. Recording state machine and exact write semantics

### 13.0 Recording readiness

Separate storage readiness (`unprepared`, `preparing`, `ready`, `failed`) from the recorder state. An imported/prepared store is record-ready only after its full active/COW capacity is allocated and prefaulted. An empty module remains small until first audio-input connection, an explicit “Prepare recording memory” action, `record.prepare`, or a REC request asks the worker to prepare capacity. Normal first input connection may therefore prepare storage before the performer presses REC; the callback only posts a fixed readiness request.

If a start arrives before readiness, request preparation but reject that start with `record_not_ready`; display “Preparing memory — press REC again when ready.” Do not silently start later or invent the missed initial audio/Clock edge. No record arm is latched before storage is ready. A subsequent start follows the exact timing rules below. This first-use preparation boundary is a software limitation required by lazy allocation and the no-allocation callback contract. `record.prepare` is idempotent and allows an agent to wait for readiness before a timed capture.

### 13.1 States and requests

```text
Idle
ArmedStartCurrent
ArmedStartAppend
RecordingCurrent
RecordingAppend
ArmedStopCurrent   (still writing)
ArmedStopAppend    (still writing)
```

Keep actual state, requested state, and destination separate. Button/semantic commands choose `Current` or `Append`; `rsop` changes only the default/alternate command assignments. An empty Reel turns a Current start into the initial Append session.

- Idle + start: begin immediately without a Clock cable; otherwise arm for the next rising Clock edge.
- ArmedStart + another toggle: cancel the armed start. It must not become an immediate recording.
- Recording + toggle: stop immediately without Clock; otherwise arm a stop and continue writing.
- ArmedStop + another toggle: cancel the armed stop and continue recording.
- Clock rise: apply at most the pending state transition for that frame.
- Capacity reached during Append: finalize immediately even when a clocked stop is pending.
- Switching destination while already recording is rejected as `recording_busy`; an ordinary toggle means stop, not a silent destination switch.

A clock-armed start latches the selected record region **when it actually starts**, not when the button was first pressed. An active Current session retains that region even if Organize subsequently changes playback to another Splice. This behavior is D and is tested explicitly.

### 13.2 Same-frame event priority

This section is the authoritative event-order rule when a higher-level processing diagram is less specific. Preserve host-time ordering when events map to one core frame. For events with an identical logical timestamp use this order:

1. Adopt metadata/options transactions already eligible before this frame.
2. Organize bin changes, then SHIFT events.
3. PLAY edge/level transition; a rising Play may commit the pending selection.
4. Record command requests, including resolved button chords.
5. Clock rising edge: resolve recording arm/stop, then clock-playback action.
6. SPLICE marker requests.
7. Render pre-write playback, compute bus, and write the frame if recording.

A finite voice born at core frame b renders ages 0 through N-1 on frames b through b+N-1; its natural completion is due at the beginning of b+N. At frame entry, determine due natural voice/primary boundaries from the preceding state without committing selection yet. After steps 1–6 and before step 7, resolve the due primary boundary against the now-current requested selection and Play state, emit due natural completion events, and schedule any normal onset not superseded by a forced onset that frame. An actual prior natural completion still counts once when it coincides with a retrigger; the retrigger itself adds no completion event. Full-Splice travel is accumulated after rendering and uses the same next-frame completion convention.

A start and Clock at the same timestamp include that frame in the recording. A stop and Clock at the same timestamp exclude it. A marker on the same timestamp as a start uses the new recording cursor before the first write. Separate REC and SPLICE **gate jacks** do not automatically form a button chord; they are independent events.

### 13.3 Current-Splice/TLA recording

Latch `[recordBegin,recordEnd)` from the committed current Splice, initialize `recordCursor=recordBegin`, and increment it by one per core frame. Wrap at `recordEnd`. Do not use Vari-Speed, Morph, Slide, the currently audible voice address, or host sample rate to advance the writer.

Default `inop=0` writes the post-S.O.S. bus. `inop=1` writes gain-conditioned live audio only. This is replacement, not `buffer += source`; the old sample is already represented through playback in the default source. Do not add the old destination sample a second time.

If the user changes `inop` during a session, latch the new source choice on the next core frame with a 48-frame source crossfade. The routing setting is persistent; the in-progress recording session is not.

The writer may overlap a playback cursor. Read-before-write ordering is mandatory. Natural record wrap does not emit EOSG; EOSG belongs to playback. A full Reel does not prevent Current recording.

### 13.4 Append / initial recording

Append begins at the current `validFrames`. Reserve the prospective new Splice slot before arming so capacity errors are reported before recording starts. While appending, newly written frames extend `validFrames` one at a time; unwritten capacity is never playable audio. On an empty Reel, there is no Reel playback until at least one valid frame exists; initial monitoring uses the S.O.S. live path.

Freeze pre-Append playback region ends at the previous valid length, including newly launched readers of those existing Splices. Appended audio is not eligible for playback until finalization, including initial recording into an empty Reel. This avoids a growing one-frame loop feeding back into its first recording. A snapshot during Append includes the captured new frames and a provisional start marker at the Append start (zero for initial recording), plus written staged markers, so it reloads as a valid stopped Reel without resuming recording.

While appending to an existing Reel, existing playback continues independently. Playback is not forced onto the growing destination. When finalizing a nonempty appended segment, commit its boundary and request it as the next Splice; on initial recording it becomes current immediately. Existing playback uses normal `omod` commitment. This auto-request is D.

A zero-frame canceled recording creates no marker or empty region. At capacity, include the last available frame, finalize, clear pending stop, and signal `reelFull` once. At 300 existing Splices, starting another Append is rejected; Current/TLA remains available. A finalization must not append a duplicate marker at frame zero.

Count the reserved Append-start marker against the 300-region limit for every subsequent marker insertion, including while armed. Additional markers inside the appended segment consume the remaining slots and stay staged until their addressed frame is written. Reject excess markers without stopping recording; finalization must always have its reserved slot. Canceling a zero-frame start releases the reservation. Finalization selects the appended segment's first stable marker ID and commits all written staged markers without duplicates.

### 13.5 Splice markers during playback and recording

A SPLICE event records an integer frame address:

- During a recording session: the fixed-rate record cursor before this frame's write.
- Otherwise while playing: `floor(primaryPlaybackAddress)` after Slide/transport actions but **before PM displacement**.
- Otherwise: the stopped primary cursor's region-wrapped address.

These address-selection choices are D because the brief leaves the exact priority unclear. Quantize to a frame, not a scalar channel sample. Ignore a duplicate address and return a non-error “already exists” outcome. Reject addresses outside valid audio; an Append marker exactly at the soon-to-be-written endpoint is staged and committed only after that frame is actually written.

Insertion does not cut or move audio. Existing voices keep their captured region until their natural end; new selection tables become effective at the next primary boundary. A Current writer keeps its latched destination, even if a marker splits that region. No marker edit may invalidate a retained reader/writer region.

Keep marker insertion bounded by 300 entries in a fixed array. No vector growth or sorting allocation in the audio callback. The metadata commit carries a monotonically increasing revision and stable marker IDs.

## 14. Buttons, Reel Mode, and destructive actions

The brief establishes release-resolved REC, chords, and long-hold erase behavior in general, but not enough detail to recover every hardware gesture. Do not label the following complete interaction map as exact hardware reproduction.

### 14.1 Software interaction map

| Gesture | Action |
|---|---|
| REC press/release alone | Default recording command; if already recording, request stop. |
| SPLICE press/release alone | Add a marker. |
| SHIFT press/release alone | Request next Splice. |
| REC+SPLICE chord | Alternate recording command (`Current`/`Append` assignments follow `rsop`). |
| SHIFT+REC chord | Cycle input gain −3 → 0 → +6 → +12 → −3 dB. |
| SHIFT+SPLICE chord | Enter/leave Reel Mode. |

Resolve a chord on the first release after both participating buttons overlap. Suppress both single-button release actions until both buttons are up. Third-button involvement cancels the gesture without mutating audio. Mouse/touch input cannot always hold two controls conveniently, so every chord action also has a named context-menu command. Gate inputs are sample-timed; manual UI commands occur at the next mapped core event boundary.

Do not add undocumented three-second destructive button gestures in v1. Erase, clear, and delete operations are explicit menu/semantic transactions with confirmation or an explicit destructive flag. This is an intentional UI divergence to avoid accidental data loss, not an omitted hidden behavior.

### 14.2 Reel Mode

Reel Mode is a UI selection state, not a second DSP engine. Organize's **knob only** selects one of 32 slot bins. Organize CV does not initiate or continually retarget disk operations in this mode. The current Reel continues sounding while previewing a slot.

SHIFT confirms the candidate slot; REC or SPLICE cancels Reel Mode without performing their ordinary actions. A second SHIFT+SPLICE exits without changing Reels. Resetting the UI cancels Reel Mode. No preview alone reads a full WAV.

A confirmed switch while recording or clock-armed is rejected with “Stop or cancel recording before changing Reels.” For a dirty stopped Reel, first obtain and commit an immutable checkpoint to its module-owned cache; then prepare the target Reel. If persistence fails, retain the old Reel and show the error. An empty slot prepares an empty Reel rather than pretending a missing external file is a valid blank recording.

Only after the complete target data and metadata validate may the audio thread adopt it. Fade old playback over 5 ms, adopt the new store, initialize transport, and fade in over 5 ms. Do not require both full engines to render during a swap. Retain the old store until all audio references and worker snapshot leases have been released.

### 14.3 Editing commands

Implement Add Marker, Move Marker, Remove Marker, Erase Splice Audio (replace region with zeros), Delete Splice (remove frames and compact subsequent audio/markers), Clear Reel, Rename Reel, and Rename Splice. Frame zero cannot be removed while a Reel is nonempty.

Heavy edits run off-thread from an immutable snapshot and yield a replacement Reel. Disable destructive audio edits during recording or an armed recording transition. A later explicit command may stop-and-edit, but a delete request itself must not silently stop recording. Moving/removing markers uses a validated fixed metadata transaction and normal boundary adoption.

Deleting a Splice remaps subsequent marker positions by the removed length, preserves stable IDs for surviving markers, and selects the nearest surviving region. Deleting the last region leaves an empty Reel. Importing a new file into a nonempty slot is destructive and requires the same explicit acknowledgement as Clear.

## 15. CV OUT, EOSG, and phase modulation

### 15.1 Envelope follower (`cvop=0`)

Use `energy = 0.5*(L*L + R*R)` on the conditioned core output in internal units (Rack volts divided by 5). Starting from zero, update `energyState += alpha*(energy-energyState)`, with `alpha=1-exp(-1/(tau*48000))`, attack tau 5 ms when energy exceeds state and release tau 80 ms otherwise. Output `clamp(8*sqrt(max(energyState,0)),0,8)` volts. An internal full-scale constant stereo value tends to 8 V in the filter-bypassed test harness. This asymmetric energy follower is not a true RMS meter: a unit-peak sine does not generally settle to `8/sqrt(2)`. Use the recurrence-based sine/step anchors in `reference_vectors.json`, with conditioning bypassed, to test its actual calibration.

These detector, time constants, and calibration are D. A squaring detector is chosen to avoid cancellation of anti-phase L/R content; do not sum the waveforms and rectify the sum. Do not use the pre-S.O.S. Reel alone as the envelope source. Precompute filter coefficients; use a measured fast square-root approximation if needed, keeping CV error within 1 mV over 0–8 V. No per-sample exponential is required.

### 15.2 Ramp (`cvop=1`)

Output a rising 0–8 V saw derived from primary-cycle normalized phase. Finite mode uses `age/Ng`; full-Splice mode uses accumulated traversal distance/L. Reverse playback does **not** reverse the voltage slope. At a primary boundary or Play retrigger, reset to zero. While playback is stopped, hold zero. At Vari-Speed Stop in full-Splice mode, hold the current ramp phase; finite timing follows the Stop policy in section 8.

No secondary chord ratio speeds up the ramp. The high-Morph EOSG stream may have more events than the ramp. This difference is intentional.

### 15.3 EOSG

Emit an event for each naturally completed musical Gene or full-Splice voice traversal. Do not emit events for forced aborts, rate-bridge reset, recording wraps, or transition-tail expiry. Simultaneous completions share one physical pulse, but telemetry counts all completions.

Pulse width is not known from the brief. Profile 1 uses a maximum of 240 core frames (5 ms), reduced to `max(1, floor(0.45*expectedCompletionInterval))` for rapid steady granular activity. Take the interval estimate from the current hop and recent distinct completion spacing; clamp the width to 1..240 frames. A new event arriving before an existing pulse ends retriggers the pulse and may merge with it. Do not promise every event is electrically separable at arbitrarily high event rates.

Return pulses on the audio-aligned host timeline. Zero the output when no pulse is active, including stopped, empty, or bypassed states.

### 15.4 Audio-rate PM (`pmin=1`)

Use an input-presence state machine, not simply `isConnected(L)`:

- Compute `leftEnergy += alpha10ms*(rawLeftNormalized^2-leftEnergy)`, initialized to zero, with one symmetric 10 ms coefficient; RMS is its square root. Compare energy to squared thresholds to avoid a per-frame square root.
- Enter PM when right input is connected and left RMS is below 0.001 for 3 consecutive seconds.
- Exit when left RMS exceeds 0.002 for 16 core frames, R is unplugged, or `pmin` is disabled.
- Crossfade PM depth and right-audio routing over 5 ms on state transitions.

The thresholds and times are D; the source only establishes a several-second absence-detection behavior. A quiet connected left cable must eventually permit PM. Switching modes must not route a full-strength DC CV into the recorder by mistake.

When PM is active, remove R from the live recording/monitoring stream. Left remains the live source; do not substitute the PM signal as mono audio. Compute:

```text
pmFrames = clamp(rawRightVolts, -10, +10) * 96
readAddress = wrap(cursorPosition + pmBlend*pmFrames, voiceRegion)
```

This equals a 10 ms source displacement at +5 V, a deliberately chosen calibration. Apply the same displacement to all active read cursors before interpolation. PM never changes the persistent cursor, the primary-cycle counter, the marker-capture address, or the writer. Do not control-rate decimate PM.

## 16. Firmware-style options and compatibility profile

Expose a clearly labeled “Chimera behavior” context submenu. Descriptive labels are primary; show option keys in tooltips or an advanced view. All option defaults are zero except ratios 2,3,4 and input gain 0 dB.

| Key | Type / accepted values | Adoption rule |
|---|---|---|
| `ckop` | integer 0,1,2 | Next core frame; rebase trajectory with 48-frame transition. |
| `vsop` | integer 0,1,2 | Next core frame; recompute rate, retain cursor, smooth rate coordinate. |
| `inop` | integer 0,1 | 48-frame recording-source crossfade. |
| `pmin` | integer 0,1 | Presence state machine / 5 ms routing transition. |
| `omod` | integer 0,1 | Next core frame; enabling immediate mode commits an existing request. |
| `gnsm` | integer 0,1 | New voices; existing voice windows remain latched. |
| `rsop` | integer 0,1 | Future idle start requests only. |
| `pmod` | integer 0,1,2 | Re-evaluate current PLAY level; do not invent a rising edge. |
| `cvop` | integer 0,1 | Switch at next core frame; output ramp/envelope does not alter audio. |
| `mcr1..3` | finite signed float; `0.0625 <= abs(value) <= 16` | New musical onsets only. |

Reject zero, NaN, infinity, out-of-range ratios, unknown enum values, and partial malformed edits. Do not clamp an invalid semantic edit and return success. Patch migration may substitute a default for a corrupt optional field, but it must expose a warning; a user edit and a damaged saved document are distinct cases.

Provide an options-text importer/exporter using the listed keys and whitespace-separated values, blank lines, and `#` comments. Validate the whole file before adoption. Return unknown keys as warnings and preserve them in a non-executable extras map for re-export; duplicate recognized keys are an error. This is a software interchange grammar, not verified byte compatibility with a specific hardware options filename or parser.

Keep `dspProfile`, `schemaVersion`, and user-facing software version separate. Do not write an invented “MG204-compatible” version into the patch.

## 17. Reel memory, snapshots, and realtime ownership

### 17.1 Why a shared pointer is not enough

Workers and the waveform widget may not scan a live float array while the audio thread overwrites it. A `shared_ptr` keeps allocation alive but does not make sample mutation safe. A seqlock around non-atomic float payload does not erase the C++ data race. Copying a full 66.8 MB buffer in `process()` is also prohibited.

Use a **paged, audio-owned store with one immutable snapshot lease and copy-on-write pages**. This is D infrastructure chosen to make TLA recording, WAV export, patch saving, and waveform generation coexist safely.

### 17.2 Data and pool

Use 256 stereo frames per page (2,048 bytes). A full Reel occupies 32,625 pages, exactly 66,816,000 audio bytes. Prepare up to one equally sized reserve page set for copy-on-write. Allocate and prefault pages off-thread before the store becomes record-ready. The empty module/browser preview does not allocate 128 MB or start a private thread.

The audio owner holds:

```text
activePageId[32625]
snapshotPageId[32625]
capturedEpoch[32625]
freePageIds[]                 // fixed-capacity stack
activeSnapshotEpoch
snapshotScanCursor
snapshotMetadata             // fixed-size copy at the cut
```

A page belongs to active audio, a frozen snapshot, or the free pool; it may be shared active+snapshot only while immutable. While a snapshot is leased, a write to a shared page first clones its 2,048 bytes into a free page and updates the active page ID. Later writes to that new active page do not clone again for the same snapshot.

Use at most one snapshot lease per store. A second requested export/save may share the same cut only if its requested revision matches; otherwise queue it. Do not starve a patch save behind continuously requested waveform snapshots.

### 17.3 Constant-cost snapshot cut and incremental capture

At a core-frame boundary, increment snapshot epoch, capture valid length, markers, selection/options revision, and `capturedThroughFrame`. Set scan cursor to zero. No entire page table is copied in that callback.

Each following core frame captures up to **eight** previously uncaptured page references. Before any recording write, if that logical page has not yet been captured in this epoch, capture its original page ID first; then perform copy-on-write if needed. The combination produces the exact Reel contents as of the cut even while a circular writer changes pages ahead of the scan.

Publish the frozen snapshot to the service only after every valid page reference is captured. The worker can then read only those immutable pages. At full length, scanning eight references per core frame takes about 85 ms of core time; this is not audio latency. The worst extra audio work is one 2 KiB page copy per first write to a shared page, plus eight pointer/tag operations. No data-dependent full-Reel loop runs in `process()`.

The cut occurs before that frame's events/render/write and covers writes strictly before the cut frame; serialize the last included frame/write count explicitly. Capture only `ceil(snapshotValidFrames/256)` pages, not a moving valid-length target. Post-cut Append pages outside that bound need no COW; a partially filled last snapshot page does. Publish/release handles with acquire/release synchronization. A logical page is protected only while its snapshot reference is captured and still retained; epoch equality alone is insufficient during reclamation.

On worker release, reclaim snapshot-only pages incrementally, eight entries per core frame. Do not start a new snapshot until reclamation is complete. Active pages are never returned to the free pool. Epoch wraparound is handled by a stopped/off-thread rebuild, not an audio-thread table clear.

All consumers sharing a cut are service-side readers of one lease; release to the core only after the last consumer finishes. Reclamation clears each reference's retained flag before allowing further writes to treat the active page as unshared. No page can be freed twice or cloned against a recycled snapshot page ID.

If the reserve is exhausted because of an invariant violation, stop recording safely and report the invariant error; never reuse a leased page or allocate on the audio thread. With one lease and a full reserve, one clone per logical page is sufficient regardless of how many TLA passes occur.

### 17.4 Stopped host / no callback progress

A patch save must not wait forever for audio callbacks that are not running. Introduce a lightweight exclusive **core ownership token** with states Idle, Audio, Maintenance. The audio callback attempts one acquire, never spins, and releases at its end. Normal jobs do not take this token and do not contend with audio.

After at least 250 ms without module heartbeat advancement, a save-side maintenance path may attempt an Idle→Maintenance acquire. The atomic ownership claim, not the heartbeat guess, provides safety. While owned, it may establish a snapshot cut and finish capturing page references/metadata without advancing musical time, then release. It never performs file I/O while owning the core. This bounded metadata operation is permitted outside `process()`.

Maintenance also drains completed lease releases and finishes prior reclamation before starting a newer cut. It can resume an in-progress capture. It never revokes a lease from a worker still reading. Bypassed modules must permit this path: heartbeat means progress of core snapshot maintenance, not merely entry into Rack's bypass callback. Ownership transfer serializes the sole producer/consumer roles of affected queues; maintenance and audio must never publish concurrently into an SPSC channel.

If the host resumes during this rare maintenance interval, its callback does not wait or access the owned core: it holds/ramps output toward zero using bridge-local state, records a timing discontinuity, and re-primes timestamp alignment after ownership returns. Do not claim sample-perfect continuity across a stopped-host restart. During continuously running live saves, the normal incremental path must have zero ownership misses and zero missing recorded frames.

If ownership or job completion cannot be obtained within a bounded save timeout (default 10 seconds), mark the module save failed visibly and preserve the last committed assets, following section 20.3's host failure policy. Do not silently save an older recording while claiming the current revision was saved. Never call an engine API that upgrades Rack's save-hook lock from inside `onSave`.

### 17.5 Resource budgets

Default audio payload budget per populated module: one active+reserve store, about 127.44 MiB. A prepared replacement may temporarily double that; maximum raw audio payload is **256 MiB** per module, plus explicitly measured metadata, SRC state, and waveform caches. Decode directly into the prepared store rather than create an additional full temporary float vector.

Retired stores with outstanding leases count toward this same budget. Before admitting another prepared store, wait for retirement or return `busy`; never permit active + prepared + leased retired full stores. Cancellation does not release payload credit until destruction actually occurs off-thread.

A 32-slot bank does not keep 32 full mutable Reels in memory. Inactive Reels are immutable cached files plus small metadata. Only active and one prepared replacement are resident. Expose a memory estimate in diagnostics. Browser previews and truly empty modules remain lightweight.

## 18. Worker service, queues, and lifecycle

Prefer a plugin-scoped service following the current explicit plugin lifecycle pattern, with at most two workers globally. Do not reuse a graphics-only worker for blocking disk work. A new `ChimeraIoService` may be initialized lazily and shut down from the existing plugin `destroy()` path after current ownership rules are verified. [R3]

Operations include Prepare Store, Decode Reel, Snapshot Encode, Patch Asset Commit, Build Waveform, Apply Heavy Edit, Prepare Rate Bridge, and Retire Store. Prioritize patch-save and record-readiness work above optional waveform updates. A long decode may be cancelled at bounded chunk boundaries.

### 18.1 Queue contracts

| Direction | Payload | Contract |
|---|---|---|
| UI/semantic -> service | JSON/path/strings/jobs | Non-RT mutex/queue allowed; bounded accepted jobs, default 16/module. |
| Service -> audio | fixed command/handle | One-producer SPSC, 64 entries; reject new work if full. |
| Audio -> service | completions, snapshot-ready, retire handles | One-producer SPSC, 128 entries; reserved entries for retirement/completion. |
| Audio -> UI consumer | fixed telemetry snapshot | Latest-value SPSC or equivalent proven exchange; replaceable. |
| Worker -> UI | immutable display/metadata result | Owned handle transfer; UI frees/retire off audio. |
| Host gates -> core | timestamped edges | Fixed queue; never substitute a latest-value snapshot. |

UI and Octavia are multiple logical producers. Route both through one service/control dispatcher before the SPSC audio command queue, or use separately proven queues; do not label an accidental MPSC use “SPSC.” The existing `SpscLatestSnapshot` must have exactly one consumer. [R7] UI and semantic status should consume copies through a single dispatcher or use distinct snapshots, not concurrently call `readLatest()` on the same object.

Commands carry `moduleGeneration`, monotonic request ID, expected document revision where relevant, and fixed payload or an ownership handle. Every accepted mutation gets an eventual applied/rejected result. Record start/stop commands are not coalesced like slider previews. At queue saturation reject with `busy`; never drop a REC edge invisibly. A final emergency stop flag may override queued work and is cleared only after audio acknowledgement.

Service-to-audio adoption transfers a prepared store by integer handle or trivially copied pointer under explicit ownership. Old stores enter a retirement queue and are destroyed only off audio. Do not let the last `shared_ptr` reference disappear in `process()` or a realtime sample-rate callback.

### 18.2 Module removal and plugin unload

Jobs own a cancellation/generation token and their data, not a raw `Module*` to dereference later. Removing a module invalidates its generation; completions for that generation are discarded and their payload retired. The widget can disappear before a job completes. No worker may call NanoVG/GL or touch a destroyed widget.

Do not synchronously join a private per-module thread from the audio callback. The service outlives individual jobs and drains/cancels at plugin shutdown. A deleted module's Rack patch storage is not eagerly removed; Rack's own cleanup/undo semantics must be respected. [R9]

If no safe reusable executor exists in the actual checkout, implement the small dedicated service rather than weakening these lifetime guarantees. Add a repeated add/remove/load/save stress test under ASan and TSAN.

## 19. WAV, markers, and bank interchange

### 19.1 Canonical representation

Canonical internal/export audio is little-endian float32, stereo, 48 kHz, interleaved L/R. Every marker position is a **sample frame offset**. Do not encode channel-scalar offsets. Preserve sample amplitude; no automatic normalization, dithering, fades, or denoising during canonical export.

Use a small RIFF writer around the existing decoder infrastructure or a validated existing encoder. Emit `RIFF/WAVE`, a float-format `fmt ` chunk (format tag 3, two channels, 48,000 Hz, 384,000 bytes/sec, block align 8, 32 bits, zero extension length), a `fact` sample-length chunk, `cue ` markers, optional `LIST/adtl` labels, and `data`. Chunk byte counts exclude their eight-byte chunk headers; odd payloads receive a pad byte. Validate arithmetic with 64-bit intermediates.

For each cue entry write a unique identifier, the corresponding frame position, `fccChunk="data"`, zero chunk/block offsets, and `dwSampleOffset=frame`. Emit start cues for all regions, including zero. The software's own reader normalizes repeated zero cues to one. Do not emit an end sentinel as a new region.

This is the chosen **software marker interchange profile**. The brief explicitly does not establish which precise RIFF chunks hardware emits or accepts. Mark export “hardware-oriented WAV; physical-unit marker validation pending” until a real round trip is tested. Do not claim exact hardware marker interoperability from an internal writer/reader test alone.

### 19.2 Import modes

**Strict Reel import:** require stereo, float32, 48 kHz, <=174 seconds. Useful for comparing supplied hardware Reels without hidden conversion.

**Convenience import:** support PCM16/24/32 and float32/64, one or two channels, integer rates 8,000..384,000 Hz; resample off-thread to 48 kHz and duplicate mono. More than two channels, compressed codecs, RF64/RIFX, unsupported container layouts, or malformed files receive an explicit unsupported/invalid result, not a silent downmix. Additional formats may be added later through the actual decoder, but are not required by v1.

Default overlength policy is **reject** with reported duration; expose an explicit `truncateTo174Seconds` import option. A user's original file is never modified. Imported NaN/Inf samples become zero with a count/warning. Rate-converted markers use `round(sourceFrame*48000/sourceRate)`, then sorting, duplicate removal, and bounds checks. End-of-file cues are ignored as end sentinels with a warning; negative/out-of-range/overflowed cues fail strict mode and warn/drop in convenience mode. More than 300 resulting regions rejects the import; do not silently discard later Splices.

Ignore safe unknown RIFF chunks, check file bounds before seeking/reading them, and cap label metadata to 1 MiB total. Validate frame count, format agreement, integer multiplications, alignment, and chunk overlap before allocating a store. No parser may trust the extension alone.

### 19.3 Reel bank

Map zero-based slot 0..31 to `mg1.wav`..`mg9.wav`, then `mga.wav`..`mgw.wav`. Keep the active slot separate from whether a slot contains audio. Directory import examines only these expected filenames; it does not recursively search the user's filesystem or execute arbitrary options files.

Directory export writes canonical Reels and an optional software-side manifest to a user-selected destination. It does not format a device or require FAT32. An overwrite request must be explicit; all in-plugin recordings first live in module-owned storage, never in the imported original directory.

Names, colors, and software-specific marker IDs live in the manifest/patch metadata and optional label chunks. Audio compatibility must not depend on an unknown private WAV chunk. Content hashes are calculated on canonical frames and marker positions with a versioned hash description; filesystem mtime alone is not identity.

## 20. Patch persistence, consistency, and undo

### 20.1 Source separation

Use three storage roles:

1. The active mutable store, owned by the core.
2. Immutable checkpoint files in a per-instance cache under the plugin's writable root.
3. Embedded, module-relative patch assets that are complete when `onSave()` returns.

Use `leviathanPluginUserRootPath()` for plugin-specific caches rather than hardcode `Leviathan` or `Leviathan-Pro`. [R2] Use Rack's module patch storage APIs after module addition, not in a constructor or `process()`. [R9] Imported source paths are optional provenance/relink hints; patch playback defaults to embedded content.

### 20.2 Patch JSON

Rack parameters remain in Rack's parameter serialization. Additional state uses this shape:

```json
{
  "schemaVersion": 1,
  "dspProfile": 1,
  "bankId": "random-non-secret-identifier",
  "activeSlot": 0,
  "selectedSpliceId": "s1",
  "seed": 1831565813,
  "inputGainDb": 0,
  "options": {
    "ckop": 0, "vsop": 0, "inop": 0, "pmin": 0,
    "omod": 0, "gnsm": 0, "rsop": 0, "pmod": 0, "cvop": 0,
    "mcr1": 2.0, "mcr2": 3.0, "mcr3": 4.0
  },
  "storage": {
    "mode": "embedded",
    "manifest": "chimera/bank-bundle42.json",
    "savedDocumentRevision": "42",
    "savedAudioRevision": "1203"
  },
  "ui": {"displayMode": "reel", "zoom": 1.0, "scroll": 0.0}
}
```

The example revisions are illustrative. The bank manifest enumerates slot, relative audio filename, content hash, valid frames, marker IDs/frames/names/colors, and cache format version. Empty slots have no dummy 174-second WAV. Reject absolute paths and traversal components inside embedded asset references; resolve and verify containment within this module's storage directory.

Separate **document revision** (authored options, markers, bank selection) from **audio revision** (recorded/edited sample state). A record session increments an audio mutation counter once per write block (256 frames), plus a final partial-block revision on stop/snapshot cut. Snapshot identity also includes exact captured core frame/write count so revisions cannot conceal a partial page.

Do not serialize armed recording, active record state, pressed buttons, live job handles, FIFO contents, PRNG cursor, PM presence countdown, gate detector state, or fractional running transport. Reload starts with recorder Idle, no armed transitions, no queued jobs, and deterministic fresh transport at the selected region. Unpatched PLAY may autoplay; a patched low gate suppresses it according to `pmod` after the first connected-input observation.

### 20.3 Save transaction

A save must first settle or explicitly exclude pending metadata/reel edits. Fence the control dispatcher, capture a coherent `SaveBundle` containing options, bank selection, markers, exact audio snapshot, and both revisions, then serialize **that bundle**. New live changes after the cut are permitted but belong to a newer unsaved revision. Do not serialize newest marker metadata against older audio.

`onSave` requests/coalesces the needed immutable snapshot and waits on the non-RT side for its files to be committed. The worker writes temporary files, flushes/closes them, and renames to revision-specific final names; the manifest is committed last. `dataToJson()` returns the corresponding saved bundle metadata for that save transaction. Serialization for clipboard/history outside a save returns a safe authored snapshot, never a direct scan of audio-owned state.

Use revision-specific manifests as well as audio filenames: write `chimera/bank-<bundleId>.json` and publish that path only after all referenced assets exist. Ordinary `dataToJson()` calls also occur without `onSave` (autosave/history); they must identify their actual durable audio cut and must not serialize newer marker bounds over older audio. Keep unsaved authored state separately where necessary, with an explicit recovery warning.

**Host failure policy / Phase 0 proof:** the installed SDK declares `onSave()` as `void`. The local Rack source also calls it during patch-manager destruction, so blindly throwing from every failed hook is not safe. Do not promise that a module error automatically cancels Rack's patch archive or preserves an existing `.vcv` file. On failure retain the last coherent bundle, keep dirty/error state visible, and serialize a `saveFailure` diagnostic identifying the unsaved revision; with no prior bundle, serialize a missing-audio state rather than reference partial assets. Successful saves still require the exact requested cut. Phase 0 must test Save, Save As, periodic autosave, duplication, and shutdown on the supported runtime and record whether safe host-level cancellation exists. Do not add an unverified exception/engine-lock workaround. Whole-patch atomicity is a host integration limitation, separate from atomic module-owned assets.

Rack's documented save contract requires assets ready in `onSave`; the inspected patch manager calls save preparation before archiving. Simply queueing a future WAV write and returning is incorrect. [R9–R10] Do not assume save preparation excludes engine processing; inspect the working SDK locking contract and retain the ownership protocol even if a particular host happens to pause. [R13] Report any timeout, disk-full, permission, or rename failure; preserve the last valid checkpoint and old assets. Never return success with a partially written referenced asset.

Save to a new location and load on another machine must work with the original source directory removed. An embedded bank can reach roughly 2.14 GB of raw audio if all 32 slots are full; display that potential size before a bulk embed/export. Do not unexpectedly keep those 32 Reels resident in RAM.

### 20.4 Autosave and crash recovery

Create an immutable checkpoint after recording stops and after every destructive edit. While actively recording, request a checkpoint at most once every 10 seconds when no higher-priority snapshot is in flight. This bounds typical recovery loss; it is not a guarantee against power failure or a claim of synchronous durability for every sample.

Keep a journal of committed immutable cache revisions, prune only unreferenced generations on the worker, and never delete the last good one before the next manifest commit. On recovery, offer the latest coherent checkpoint and report its captured timestamp; do not pretend to restore an unfinished recording perfectly.

### 20.5 Missing assets, duplicates, reset, bypass

Missing/corrupt embedded audio keeps metadata available for relinking, shows an error, and produces no Reel audio; live monitoring remains available. Do not silently replace it with a factory Reel.

Duplicate/module-preset/clipboard workflows may not automatically carry the same assets as a full Rack patch. Verify the actual SDK's behavior. Implement a new instance cache identity and content-addressed immutable checkpoint references with copy-on-write; the two instances must never share a writable store. A cross-machine preset without its assets reports a missing asset and offers relink/export-bank instructions. A same-process duplicate must copy/link the checkpoint off-thread and become independently editable. Do not promise portable audio in a bare JSON clipboard payload.

Reset stops/cancels recording, clears transient transport/options to defaults and ordinary parameters through the base hook, but **preserves recorded audio and markers**. Clear Reel is explicit and destructive. Randomize affects continuous musical controls only, not buttons, files, bank identity, or destructive commands. Bypass stops/cancels recording, freezes Reel playback, zeros CV/EOSG, and directly passes normalized L/R audio without S.O.S. On unbypass, reinitialize edge detectors from current levels and fade playback in; a held REC gate is not a fresh rising edge.

### 20.6 Undo boundary

Ordinary parameter drags use Rack history. Metadata edits and destructive audio edits create coherent history entries referencing immutable before/after checkpoints. Keep at least one destructive edit recoverable within a 256 MiB audio-payload history budget on disk/cache, not extra live stores. If history cannot be retained, explicitly warn before the destructive action.

Do not make every live-recorded frame a history action. Provide “Restore pre-recording checkpoint” for the most recent session when available. Undo does not erase or overwrite an exported external file; it restores the module's state only. File side effects and module history are distinct operations.

## 21. Octavia semantic integration

Implement the current in-process `OctaviaSemanticControl` interface, after checking the working tree. The inspected interface defines `CAPABILITIES`, `GET_DOCUMENT`, `VALIDATE`, `EDIT`, `GET_STATUS`, and `COMMAND`; implementations own their schema and semantics. [R12] Do not create a new HTTP server, require an external AI provider, or hardcode an Octavia module ID.

Capability identifier: **`leviathan.chimera.reel-engine`**. Schema version: 1. The semantic adapter routes through the same transaction service used by UI actions. It never directly writes the Reel, advances a playback cursor, or makes filesystem calls from `process()`.

### 21.1 Authored document versus telemetry

`GET_DOCUMENT` returns options, input gain, seed/profile, active slot, slot summaries, and marker metadata for the requested slot. It contains no sample arrays, base64 WAV, per-sample logs, or thousands of waveform points. Default response is the active slot; all-slot summaries are an explicit request.

`GET_STATUS` returns at least:

```text
schemaVersion, moduleGeneration
acceptedDocumentRevision, activeDocumentRevision, pendingDocumentRevision
activeAudioRevision, durableAudioRevision, capturedThroughFrame
activeSlot, requestedSlot, currentSpliceId, requestedSpliceId
validFrames, capacityFrames, storageReadiness, playbackActive, recordingState
recordDestination, recordCursor, recordArmed
baseRate, finiteGeneFrames or fullSplice, density, activeMusicalVoices
clockConnected, clockLocked, clockWaiting, estimatedPeriodFrames
pmEnabled, pmActive, inputGainDb
loading, saving, dirty, activeJobId, lastCompletedRequestId
coreRate, hostRate, latencyFramesAtHostRate
residentAudioBytes, peakResidentAudioBytes
queueOverruns, ownershipMisses, rateBridgeUnderruns, clippedFrames
errorCode, errorMessage
```

Use compact defaults and an optional diagnostics expansion. A typical status response target is <4 KiB; all 300 marker names belong in an explicitly requested document, not every status poll. Status reports accepted versus actually applied state, so an agent does not treat a queued Reel load as already audible.

### 21.2 Edit contract

An EDIT contains `expectedRevision` and a list of operations applied atomically:

```json
{
  "expectedRevision": "42",
  "operations": [
    {"op": "setOption", "key": "ckop", "value": 2},
    {"op": "setOption", "key": "mcr2", "value": -1.5},
    {"op": "renameSplice", "spliceId": "s3", "name": "Glass attack"}
  ]
}
```

Return validation errors with operation index and field path. A stale document revision rejects the entire edit without partial changes. VALIDATE runs the same checks without changing revisions, scheduling jobs, reading arbitrary files, or mutating audio.

Supported metadata edits: set option, set seed, set input gain, rename Reel/Splice, set marker color, add/move/remove marker. Audio-changing edits use COMMAND jobs with explicit destructive acknowledgement. Continuous knob movement should generally use Octavia's existing parameter operations; do not create a competing shadow parameter system.

### 21.3 Command contract

Required commands:

| Command | Behavior |
|---|---|
| `record.prepare` | Prepare/prefault recording capacity asynchronously; no recording or clock arm. |
| `record.start` | Explicit destination `current` or `append`; quantize `clock` or `none`. No toggle ambiguity. |
| `record.stop` | Idempotent stop; quantize `clock` or `none`. |
| `record.cancelArmed` | Cancel pending transition; active recording continues if only an armed stop was canceled. |
| `play.retrigger` | Commit requested Splice and restart at Slide. |
| `play.stop` | Stop immediately with de-click; does not change PLAY parameter/cable voltage. |
| `splice.select` | Select by stable ID; `boundary` or explicit `immediate` commitment. |
| `splice.next` | Relative increment, not a knob mutation. |
| `reel.select` | Select a slot asynchronously with dirty-Reel checkpoint handling. |
| `reel.import` | Explicit local path/asset handle, slot, import policy, and replacement acknowledgement. |
| `reel.export` | Explicit destination and overwrite flag; snapshot coherent audio. |
| `bank.import` / `bank.export` | Asynchronous 32-slot directory interchange with progress and per-slot results. |
| `splice.eraseAudio` / `splice.delete` | Snapshot-based destructive transaction; idle-only. |
| `reel.clear` | Explicit destructive acknowledgement; idle-only. |
| `job.cancel` | Cancel preparation before adoption; already applied mutations require undo. |
| `record.restoreCheckpoint` | Restore the available pre-session checkpoint as an explicit edit. |

Semantic playback stop sets an internal stop latch; Play rising or `play.retrigger` clears it. A continuously high PLAY cable does not immediately undo an explicit stop on the next sample. `pmod=1` still responds to the next real level transition. Report the latch in status.

For commands that replace/delete existing audio, move/remove existing markers, clear/restore a Reel, or import over a populated slot, require `expectedRevision` plus `expectedAudioRevision` or a matching snapshot token, and `destructive:true`. Starting Current/TLA over existing audio requires this explicit acknowledgement at acceptance; an ordinary successful recording does not require repeated revision checks on every frame. Nondestructive marker additions/options edits use document-revision checks. `record.stop`, `record.cancelArmed`, `record.prepare`, `play.stop`, and `job.cancel` never require a destructive flag or a current audio revision; a stale client must still be able to stop recording safely. File overwrite is a separate `overwrite:true`. Restrict paths to user-selected or existing authorized project/plugin storage roots through the current Octavia permission model; a capability request does not authorize arbitrary filesystem mutation.

Return `requestId`, `status` (`accepted`, `armed`, `preparing`, `applied`, `rejected`, `cancelled`), relevant revisions, and an application frame when known. Assign idempotency keys so a retried network request does not start and then stop a recording or duplicate a marker. The bounded deduplication cache belongs off-thread; use the last 256 command IDs per module generation.

Async jobs may additionally report `running` and terminal `failed`; use `cancelled` consistently. Deduplication is guaranteed only while a key remains cached in the same generation; expose that scope and reject a cached key reused with a different payload. Explicit `quantize:none` bypasses cable quantization; `quantize:clock` requires a connected Clock and otherwise rejects with `clock_not_connected`. Panel/gate toggles retain section 13's automatic cable policy. Repeated explicit start for the same active/armed destination is a no-op; a different destination is `recording_busy`. Explicit stop while ArmedStart cancels the start; stop while Idle is a no-op. `record.stop` with `quantize:none` overrides an existing armed stop immediately. EDIT operations that move/remove markers carry the same destructive acknowledgement and revision preconditions specified above; validation and actual adoption both recheck the preconditions.

### 21.4 Completion examples

A complete agent operation is: import request → preparation status → applied audio revision → select/retrigger → confirm audible transport/mix status. Merely returning an accepted job is not completion.

Do not flood prompts with audio blobs. For listening, use Octavia's existing monitor/recording facilities and patch the L/R outputs conventionally. No private unbounded sample-download endpoint is required for v1.

## 22. Panel and interaction design

### 22.1 Layout

Target **28 HP** with a 142.24 mm-wide, standard-height Rack panel. This is a software layout decision, not a reproduction of physical module dimensions. Preserve the familiar six primary controls, with Gene/Vari-Speed/Slide attenuverters visibly adjacent. Use original Leviathan visual language and existing knob/jack components.

Initial anchor plan, in millimeters, is a fit-check starting point:

| Area | Placement |
|---|---|
| Title | Centered near y=7; may be baked into raster. |
| Display | x=6..136.24, y=14..31. |
| Main row 1 | SOS (22,46), GENE SIZE (71,46), VARI-SPEED (120,46). |
| Main row 2 | MORPH (22,77), SLIDE (71,77), ORGANIZE (120,77). |
| Attenuverters | Gene (85,60), Vari-Speed (132,60), Slide (86,77). |
| Buttons | REC (13,62), SPLICE (31,62), SHIFT (49,62). |
| CV row y=95 | SOS, GENE, RATE, MORPH, SLIDE, ORGANIZE at x=12,36,60,84,108,132. |
| Gate row y=107 | CLK, PLAY, REC, SPLICE, SHIFT at x=12,36,60,84,108. |
| Audio/output row y=119 | IN L, IN R, OUT L, OUT R, CV OUT, EOSG at x=12,36,60,84,108,132. |

Control dimensions, labels, ports, screws, and tooltip hitboxes must be measured in the actual theme. Adjust this proposed geometry before freezing assets; do not change numerical IDs. A 28 HP original design is preferred over a cramped imitation that makes the recording state hard to read.

Use the existing split panel/labels and SVG anchor pipeline demonstrated in Phonex, or its current successor in the working tree. [R8] Keep normal labels as text/vector overlays and the title as the optional raster exception. Support the current dark/light theme behavior without maintaining two unrelated coordinate layouts.

### 22.2 Waveform/status display

Show the active Reel's stereo envelope overview, current Splice shading, pending Splice outline, primary play position, record position when recording, and up to four small Gene indicators. Main status text shows Reel/Splice number, record/armed state, and an error/busy indication. A simple frame/time readout is more valuable than a costly animated tape simulation.

Display click can request a seek/Slide change; dragging a marker performs a preview and submits a metadata edit only on release. A stationary playhead display must not enqueue repeated seek commands. Distinguish edits to a selected region from arbitrary pixel positions outside audio.

The waveform does not draw every sample. Use worker-built stereo min/max bins, default 1,024 bins, with min/max pyramids for zoom. Build from immutable snapshots. Rebuild a stopped/imported Reel once per audio revision. During recording, publish bounded peak summaries for written pages/blocks and overlay them over the latest completed overview; do not snapshot the entire Reel at 30 Hz merely to animate its waveform.

### 22.3 Rendering contract

Static panel and waveform geometry are cached. Playheads, record indicators, and text telemetry are small dynamic overlays. Cap visible telemetry consumption at 30 Hz, waveform-recording summaries at 10 Hz, and UI text formatting at 10 Hz. Invisible/off-screen widgets stop expensive display work while DSP continues unchanged.

Do not rescan WAV data, rebuild a framebuffer, upload an entire waveform texture, allocate a full path, or run an FFT every UI frame. Default v1 drawing may use cached NanoVG geometry; no custom GL renderer is required. Any adopted existing GL path must follow the repository's context-create/context-destroy and deferred resource-retirement rules.

Use `NvgGraphicsLifecycle.hpp` for persistent NanoVG image ownership/validation and `GlLifecycleUtils.hpp` for applicable GL resource checks. Invalidate on context change, validate before reuse, and rebuild lazily. Never delete an image from a foreign context or perform destructor-time GL cleanup. Test DAW window close/reopen as well as widget removal.

A browser widget with `module==nullptr` shows a small deterministic static placeholder, instantiates no audio service, opens no files, and allocates no Reel. UI callbacks and workers never access audio-owned cursor structs or mutable pages directly.

## 23. Performance, thread safety, and instrumentation

### 23.1 Hard realtime requirements

After preparation, `process()` performs no heap allocation/free, filesystem operation, mutex wait, condition-variable wait, thread creation/join, JSON parse/encode, string formatting, logging, directory traversal, or full-Reel scan. A realtime shared-pointer decrement that can destroy a payload counts as a forbidden free.

Work is bounded by four musical slots, four transition readers, a fixed number of commands/events, eight snapshot scan references (plus one write-barrier capture), and at most one recording-page copy for a core frame. Cap non-event audio commands consumed at four per core frame. Host edge/connection processing is capped at 256 events per core frame. At 768 kHz, 16 host frames map to one core interval and five gate inputs alone can produce 80 transitions, so the former 64-event budget was insufficient. Include connection changes in the bound; size the event queue for the entire prepared input-delay history at the worst supported rate, not merely one interval. Queue overflow latches a visible transport error, immediately stops active recording and cancels arms, and requires an explicit fresh start after recovery; never silently drop REC toggles and continue writing.

Only begin optional snapshot work when the core store is ready. Correctness does not depend on widget visibility or service scheduling latency. Backpressure may delay loading/saving; it must not delay the next audio sample.

### 23.2 Targets to measure, not claims of achieved performance

On a declared release benchmark machine in an optimized build, target ≤5% of one CPU core for one module at 48 kHz, maximum Morph, PM, and recording, excluding file decoding. Target ≤10% with worst-case snapshot copy-on-write activity. These are engineering budgets and must be accompanied by measured percentiles and hardware/compiler details; they are not promised cross-machine results.

UI target: average <0.2 ms and p95 <0.5 ms per visible module at 100% zoom with the overview cached. Empty/browser creation must remain visibly lightweight; report its actual allocations and construction time. Do not meet CPU targets by silently reducing musical voice count, ignoring modulation, downsampling the core, or skipping writes.

Report host-rate SRC overhead separately from core cost. Test 44.1/96/192 kHz and the 8/768 kHz support extremes for boundedness. Do not extrapolate 48 kHz measurements to every supported rate.

### 23.3 Instrumentation

Provide inexpensive counters for onset/completion count, active readers, COW page copies, snapshot capture progress, pool availability, callback allocations, FIFO high-water marks, stale-job discard count, accepted/applied revisions, queue errors, and peak resident memory. Timing instrumentation is behind the existing debug/profile conventions, not an always-on clock call around every individual sample.

Gate developer/debug-terminal output with `isDragonKingDebugEnabled`. Preserve the first three Debug Terminal metrics as module-total `Process`, `Step`, and `Draw`, including work split across overlays/caches/backends; component timings follow those metrics.

Mandatory tools: allocation trap around test callbacks; ASan/UBSan for bounds/lifetime; TSAN for service/core/UI exchanges; long offline renders to detect NaN and integer wrap hazards. Run both relaxed optimization and release-equivalent math flags. A TSAN success does not prove latency; allocation/lifecycle/performance tests are separate.

## 24. Verification strategy

The companion `ACCEPTANCE_TESTS.md` is part of the normative specification. Implement a deterministic engine harness that feeds sample frames, continuous controls, connection changes, and timestamped events, and records audio, CV, EOSG events, state transitions, and sample-store changes.

Use synthetic, original fixtures: impulses, distinct stereo ramps, sine tones, a discontinuous loop, tagged source regions, single-frame Splices, and small buffers for exhaustive testing. Never require a copyrighted factory Reel just to run CI. Distinguish the 48 kHz core oracle from the host-rate bridge tests.

Golden tests have three categories:

- **Exact:** IDs, state transitions, marker addresses, record frame counts, PRNG integers, byte-preserving canonical PCM float payloads when conditioning is bypassed.
- **Tolerance-based:** cubic values, exp/log control mappings, window coefficients, filtered audio, pitch estimates, SRC timing after calibrated latency.
- **Behavioral/manual:** panel fit, musical usability, audible gap/overlap/chord transition, clip/error readability, and any physical-hardware comparison.

Do not use one platform's entire floating output buffer as a bit-exact golden across compilers/CPUs unless the implementation deliberately controls those operations. Publish tolerances and failure criteria. The supplied `reference_vectors.json` contains mathematical mapping/PRNG expectations for this design, not an implementation test result.

## 25. Calibration ledger: what remains a hardware hypothesis

Keep these profile-v1 decisions in one developer table and a hardware comparison worksheet:

| Unknown in brief | Profile-1 decision | Measurement that can revise it |
|---|---|---|
| Interpolation/anti-aliasing | Four-point cubic; no extra per-Gene band-limiting. | Sine sweeps at fractional and >1× rates. |
| Finite Gene taper/minimum | Logarithmic from L to min(16,L) frames. | EOSG/impulse captures through a slow Gene CV sweep. |
| Morph onset/voice law | Density .9→1→2→3→4; four round-robin slots. | Isolated impulse Reel, voice-rate and overlap measurements. |
| Window shape/time | 2 ms cosine edge; smooth edge up to 20% lifetime. | Grain-edge impulse/step captures. |
| Unity-density enveloping | Special seamless boundary de-click treatment. | Full-loop and finite-Gene endpoint measurements. |
| High-Morph randomness | Seeded choice of configured slot ratio plus balance. | Repeated identical-source statistical recordings. |
| Slide wrapping/scan | Region wrapping, 64 source frames/core tick slew. | Step CV at several Reel lengths and directions. |
| Clock stride/timeout | One nominal Gene of source per edge; last-period estimator. | Known marker grid and measured Clock/Vari-Speed matrix. |
| Pitch-mode summing | Knob chooses base/direction, exponential CV changes magnitude. | Center/negative/over-range pitch-CV tests. |
| PM | 96 source frames/V; RMS presence detector and 3 s activation. | DC/audio PM sensitivity and cable/signal-presence tests. |
| CV envelope | Asymmetric squared-energy follower, 5/80 ms attack/release; not true-RMS calibration. | Amplitude steps and L/R anti-phase fixtures. |
| EOSG width | Adaptive up to 5 ms. | Oscilloscope capture across Gene/Morph settings. |
| Mono normal / gain reference | Copy L to R; modular gain = 0 dB. | Single-channel patch and amplitude sweep. |
| Marker RIFF layout | Software `cue `/labels profile. | Export a real hardware Reel; inspect and round-trip exact chunks. |
| Button chord details | Explicit software map; destructive actions in menus. | Full current manual/physical interaction verification. |

Changing one of these decisions later requires new profile-version tests and a documented migration/default policy. Do not overwrite profile 1 and rebrand every changed result as a bug fix. Actual bounds, data-race, and missing-write bugs may be fixed without preserving broken behavior.

## 26. Definition of done

A v1 implementation is complete only when it:

1. Builds within the existing plugin and introduces no regressions in required existing tests.
2. Implements the full front-panel schema, options, and state machines in this document, including initial recording, TLA, append, clock arming, Play modes, pending Splices, PM, and CV/EOSG.
3. Preserves the 48 kHz memory/time model across required host rates, with measured event/audio latency alignment.
4. Passes bounds, stereo, recurrence, deterministic scheduling, queue, race, lifetime, and save/load tests.
5. Persists audio portably in a Rack patch, including a save cut taken during recording, without racing sample memory or returning before referenced assets exist.
6. Provides a usable original panel, cached waveform, readable states/errors, menu alternatives to chords, and null-widget safety.
7. Exposes a compact, revisioned Octavia semantic adapter with no realtime filesystem/network behavior.
8. Includes a user manual, profile/uncertainty ledger, import/export limitations, and measured performance results.

A playback-only sampler, a free-running granular demo without recording, or a module that saves only source paths is not the requested finished instrument. Each can be a milestone, but must be labeled as such.

Do not claim hardware equivalence, exact marker compatibility, measured latency, passing sanitizers, or a successful Rack build until the corresponding test was actually performed.

## 27. Sources and integration evidence

**B — user-supplied primary task material:** `Morphagene_Tech_Brief.md`, especially “Reconstructed signal flow and DSP algorithms,” “Control and jack implementation specification,” “State machines, timing, and firmware-controlled behavior,” and “Unspecified primitives and best engineering estimates.” The original file is included unchanged in the specification package. Its hardware claims and uncertainties are the basis of this specification.

**Selected external integration references, consulted 2026-09-23:** These branch URLs are mutable. Capture the real checkout commit in Phase 0; do not treat an access date as a commit hash. File observations are selective and API signatures may differ in the working branch.

```text
R1  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/Makefile
R2  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/plugin.hpp
R3  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/plugin.cpp
R4  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/TemporalDeck.hpp
R5  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/Iris.cpp
R6  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/Moirai.hpp
R7  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/SpscLatestSnapshot.hpp
R8  https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/PhonexWidget.cpp
R9  https://vcvrack.com/docs-v2/structrack_1_1engine_1_1Module
R10 https://raw.githubusercontent.com/VCVRack/Rack/v2/src/patch.cpp
R11 https://raw.githubusercontent.com/VCVRack/Rack/v2/include/dsp/resampler.hpp
R12 https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/OctaviaSemanticControl.hpp
R13 https://raw.githubusercontent.com/VCVRack/Rack/v2/src/engine/Engine.cpp
```

The GitHub tree page was not retrievable in this environment; selected raw source files were readable. No local repository build, running Rack session, or physical Morphagene measurement was performed while writing this specification.
