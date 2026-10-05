# Native Rack implementation handoff — Lúbadh-derived instrument

**Purpose:** implement a new Leviathan instrument informed by the supplied Lúbadh 2.1.0 firmware. This is a staged engineering handoff, not a claim that the remaining algorithms or hardware interface are already fully recovered.

Read `LUBADH_REVERSE_ENGINEERING.md` first. Exact factory values are in `../tables/factory_presets.json`, parameter semantics in `PARAMETER_BIBLE.md`, and selected portable kernels in `../native/recovered_kernels.hpp`.

## 1. Non-negotiable design boundaries

Use native C++ processing. The research ARM interpreter is an offline test tool and must never run inside Rack. Do not link, execute, embed as a runtime dependency, or distribute the original executables with the module. Do not transplant the hardware Linux process/thread architecture.

Keep physical IO adaptation, normalized control state, tape transport, effects, display, and file management separate. Preserve the difference between record speed and playback speed, and between stored tape audio and the currently audible post-effect signal. Input, playback, and write-clipping stages have separate state and roles.

Real-time processing must perform no filesystem calls, directory enumeration, memory allocation, vector resizing, mutex waits, worker-thread creation, or destruction of large audio buffers. Prepare new assets off-thread, hand them over at an explicit boundary, and retire replaced storage off-thread. No unbounded loop may be driven by an unchecked speed, malformed asset, or dense event queue.

Do not silently fill unresolved details with a generic library effect and label the result faithful. Every approximation must have a compatibility status and a testable replacement boundary.

## 2. Suggested source boundaries

```text
LubadhDerivedModule.cpp       Rack parameter/event/voltage adapter; choose a new module name
DeckEngine.hpp/.cpp           state + playback/record transport, no Rack dependency
TapeStorage.hpp/.cpp          valid extent, sample reads/writes, safe asset swapping
TapPool.hpp/.cpp              four logical engines and bounded transition slots
DeckControls.hpp              normalized values and immutable preset snapshot
ControlMapper.hpp/.cpp        speed table, Time roles, quantization, control cadence
TapeColor.hpp/.cpp            separate input/playback stages; independent state
WriteProcessor.hpp/.cpp       input interpolation + write envelope + stored-sample blend
FlutterModel.hpp/.cpp         periodic/stochastic speed disturbance, reproducible seed
PlateModel.hpp/.cpp           replaceable exact/research implementation boundary
LinkController.hpp/.cpp       explicit shared transport/events, not audio summation
ClockController.hpp/.cpp      jack mode, interval averaging, timeout, phase/output events
AssetService.hpp/.cpp         background import/export, persistence, version checks
DisplaySnapshot.hpp           bounded audio-to-UI snapshot; no mutable engine access
```

The names are recommendations. Do not change unrelated Leviathan infrastructure to fit them. Existing plugin coding conventions should win where behavior remains identical.

## 3. State and coordinate model

Each deck should explicitly own:

```text
DeckMode: Empty, FirstRecord, Overdub, Playback
RecordPolicy: SignedVariable, FixedForward
LoopPlaybackSpeedPolicy: Follow, HoldAtActivation
OneShotPlaybackSpeedPolicy: Follow, HoldAtActivation
PlaybackRunMode: Loop, OneShot
RecordRunMode: Latched, Gated, ClockArmed, OneShot
MonitorMode: Off, Armed, On
PresetConfig: complete 31-field behavioral configuration
```

These are **native semantic enums**, not recovered integer values for every firmware field. Serialize them using versioned names or deliberately frozen IDs; never assume a compiler enum layout matches the firmware.

Maintain separate values for allocated capacity, recorded valid length, selected region start/length, active tap coordinates, record interpolation history, base speed, ramped speed, held speed, flutter factor, and touch factor. Do not overload a single `length` or `speed` variable with all of these roles.

Use double precision or a suitable split integer/fraction representation for long-running tape positions. Float32 alone cannot represent every adjacent integer at a 29.5-million-sample coordinate. The firmware's split-coordinate approach is a warning here. The reference cubic helper supplies a polynomial, not a safe long-buffer address abstraction.

An implementation of `writeAtSpeed` must account for every storage cell crossed by the recording trajectory. At absolute speed above one, interpolate appropriate input positions for multiple tape writes; below one, retain fractional history across host frames. At zero, do not divide by speed or burn CPU trying to chase an unchanging coordinate. Reverse must use its own validated direction/boundary rules.

Initially use small deterministic test buffers. Enable ten-minute capacity only after indexing, wrapping, region selection, and transitions are proven.

## 4. Proposed Rack-facing contract

The following is a **proposed adaptation**, not measured hardware calibration. Keep it in one testable adapter so later analog measurements can replace it.

### 4.1 Stable per-deck parameter names

Freeze semantic IDs before shipping patches:

```text
A_INPUT_GAIN, A_OUTPUT_GAIN, A_SPEED, A_START, A_LENGTH, A_TIME
A_RECORD, A_RETRIGGER, A_ERASE, A_TOUCH
B_INPUT_GAIN, B_OUTPUT_GAIN, B_SPEED, B_START, B_LENGTH, B_TIME
B_RECORD, B_RETRIGGER, B_ERASE, B_TOUCH
LINK_MODE, AUX_INPUT_BLEND, AUX_OUTPUT_BLEND
```

The public panel can be more compact; parameter IDs should not depend on layout. Advanced configuration lives in a preset editor/context menu or an optional expanded panel. Preset loading changes configuration atomically; it must not partly update one effect while another uses a stale field.

### 4.2 IO names and provisional scaling

| IO | Proposed native contract | Firmware/hardware uncertainty |
|---|---|---|
| A/B audio input | Start with 5 V ↔ internal 1.0, separately adjustable calibration | Not recovered analog gain or clipping; do not call this hardware-exact. |
| A/B audio output | Internal 1.0 ↔ 5 V at unity output gain | Actual module level, headroom, gain controls and analog response need measurement. |
| A/B speed CV | Compatibility control domain follows the documented 0–5 V map; optionally offer 0 V = original-pitch musical mode | Physical jack offset/summing and negative CV are not fully established. |
| A/B start, length, Time CV | Normalize through a configurable bipolar adapter and add to knob before clamp | Quickstart says 5 Vpp; use ±2.5 V only as an explicit provisional interpretation, not a ±5 V fact. |
| A/B record input | Schmitt-style event detector; selected role is latch, gate, or external clock | Default proposed thresholds: rise at 1 V, reset at 0.1 V; not measured original thresholds. |
| A/B retrigger and erase input | Separate edge detectors with configurable event scheduling | Exact pulse capture/debounce and chord priority need testing. |
| A/B clock output | Proposed 0/10 V pulse, initially 1 ms | Output voltage and pulse width are design values pending measurement. |
| Auxiliary input/output | One mono input distributed to decks, one output mixed from decks | Gain law, internal normalled routing, and pre/post levels remain open. |

Do not automatically advertise full Rack polyphony. The initial target is two monophonic deck inputs with internal multi-head playback. Processing all cable channels as independent complete decks would radically multiply memory and CPU, and is a new feature, not inherent Lúbadh behavior.

### 4.3 Link and normalled routing

Linked stereo operation must share intended transport/events without summing the two recorded buffers. Preserve channel separation through recording, playback, and save/load. The exact original switch-mode/master-control matrix is unresolved; isolate it behind `LinkController` and do not guess that every control always copies left to right.

Model normalled bounce routing explicitly, with cable connection state and level stages. A software feedback route requires a defined causal delay. Start with an explicitly documented delay and measure its effect; do not accidentally create a zero-delay algebraic loop or an asymmetry caused by processing deck A before B. Distinguish external/normalled bouncing from internal overdub of a deck's own stored samples.

Auxiliary curved fading is not proven equal-power. A provisional equal-power or hybrid option can exist behind a clearly named routing-law setting, but do not present an arbitrary curve as an extracted analog law.

## 5. Timing policy

Make the timing policy a first-class engine option, not scattered magic constants:

```text
LegacyResearch:
    128-frame control/block conventions where recovered
    literal DSP rate/constants retained
    file import compatibility can use 49,148 Hz
    no claim that host wall-clock behavior matches actual hardware

HostNative:
    seconds and physical Hz preserved across host sample rates
    delays, control ramps and scheduling explicitly converted
    native file rate policy, with a separate original-file compatibility option
```

The default shipping policy should be chosen only after the first useful comparisons. Switching policy with recorded material needs a documented conversion or rejection, not silent pitch/duration drift.

Sample-rate changes must not discard recordings, change musical region boundaries unexpectedly, or leave effect state at inconsistent rates. Preserve audio with explicit source-rate metadata; resample or remap coordinates under a planned state transition. Avoid large conversion work in the audio callback.

An oversampled coloration option may improve aliasing but changes legacy behavior. Keep the tape transport at the intended base time coordinate and identify whether effect latency is compensated. A newly designed high-quality path should coexist with, not silently replace, the compatibility path.

## 6. DSP integration order

### Phase A — deterministic base transport

Implement Empty/FirstRecord/Playback/Overdub, recorded extent, signed read position, safe four-sample interpolation, and simple bounded region transitions. Begin with effects bypassed except a deliberate output safety stage. Preserve unprocessed test access to buffers for comparison.

Acceptance: silence remains exactly silent; unity-speed playback of a recorded fixture aligns to the documented latency; record extent is correct; no out-of-bounds access; no non-finite values; behavior is independent of arbitrary host block subdivision.

### Phase B — authentic write model and fades

Implement signed-variable versus fixed-forward record policy. Add the per-write input envelope, retained-old-sample feedback coefficient, write clipping, and the reconstructed hybrid fade table. Ensure existing tape is read before the chosen writes overwrite it, including multi-write cases.

Acceptance: a ramp/impulse fixture produces expected storage coordinates at +0.25, +0.5, +1, +2, +4 and corresponding negative speeds; stall is bounded; inverse-speed logic is stable; short regions respect fade limits; old-buffer feedback is not accidentally sourced from the wet output.

### Phase C — tap pools and pitch hold

Implement four logical engines with bounded transition storage and explicit ages. Follow versus Hold is stored per tap on activation; loop and one-shot policies remain separate. Reconstruct voice stealing/transition exhaustion before calling the pool compatible.

Acceptance: held taps retain pitch while new taps acquire new pitch; single-tap retrigger crossfades rather than hard-switches; fifth-engine requests do not leak voices or allocate; transition exhaustion has deterministic behavior; clearing/replacing a recording retires all stale pointers.

### Phase D — recovered tape coloration

Integrate separate input and playback TapeAge banks, Wear and diffuser states. Add independent output/write clippers and their parameter law, including unscaled Knee and TapeAmount-scaled Compensation. Keep input biquad coefficients and full anti-alias adaptation replaceable until verified.

Acceptance: compare kernel output with included probe vectors; preserve diffuser delay signs and 853-sample finite support; match clipper threshold branches; Clean preset disables its configured coloration without pretending remaining filtering/clipping is absent.

### Phase E — flutter, touch and plate

Port the actual periodic/random modulation pipeline with a reproducible test seed. Separate UI touch pressure from the simulated charge/return model. Fully trace the plate state, delays, coefficient transforms, damping and output sum before declaring it recovered. The supplied `reverbControls` helper is only the control mapping.

Acceptance: stationary input yields repeatable seeded modulation; rate changes preserve intended modulation frequency; touch release converges correctly; reverb impulse/tail comparisons cover low/mid/high amount and silence/reset. A placeholder plate must be clearly marked in the engine and tests.

### Phase F — clock, presets, link, assets and UI

Add clock averaging/resolution/timeout, record arming, delayed retrigger, link semantics, and all ten recovered presets. Preserve comments' enum spellings only at import/export boundaries; use clear internal names. Add async import/export and complete patch persistence. Then build display animations from snapshots rather than exposing mutable engine state to the UI.

Acceptance: parameter/factory matrix matches supplied JSON; changing presets cannot partially update an active block; stereo assets stay aligned; corrupted/oversized files fail safely; undo/load/patch close never cause use-after-free; UI rate does not affect DSP.

## 7. Required deterministic tests

| Test | Required observation |
|---|---|
| Unit-rate impulse | Known record/read/write alignment and no unexplained extra head shift. |
| Ramped input at signed speeds | Exact storage coordinate ordering; no uninitialized gaps. |
| Speed crossing zero | Bounded work, finite output, appropriate stall behavior. |
| Fixed record / shifted playback | New input remains recorded at forward unit rate. |
| Reverse while recording | Direction-specific interpolation history and write order remain coherent. |
| Region close to buffer ends | Safe four-point neighborhood access and chosen wrap/clamp policy. |
| Smallest selected loop | Valid region, bounded fade, no division by zero or full-pool spin. |
| Immediate repeated retrigger | Bounded slots, correct steal order, smooth transitions. |
| Hold-mode chord | Existing pitches remain held while new head uses updated speed. |
| Silent overdub | Old buffer decays through the intended feedback/write clip path, not arbitrary wet-output feedback. |
| Correlated multiple taps | No accidental clipping/normalization error hidden by uncorrelated test material. |
| Clock averaging | Known pulse train yields expected rate; first interval and timeout are specified. |
| Pitch CV and trigger same frame | Defined ordering; optional retrigger delay acquires intended pitch. |
| Linked stereo impulses | No deck skew; distinct channel samples survive record/save/load. |
| Normalled feedback | Defined causal delay and consistent A/B processing order. |
| Sample-rate change | Duration/pitch policy and effect timing remain explicit. |
| Save/reload | Audio hash/metadata round-trip; no silent dependence on original external file. |
| Preset switch mid-loop | Bounded smooth changes without malformed memory or stale configuration. |
| Resource stress | No audio-thread allocation, long-loop memory within budget, bounded tap/event queues. |

Run tests at 44.1, 48, 88.2, and 96 kHz for the native mode. These are proposed test rates, not assertions about original hardware support. Legacy research tests should retain their declared internal clock assumptions.

## 8. Persistence and memory

Original full-capacity audio alone needs about 225 MiB for two decks. A Rack patch with several instances can therefore become large quickly. Provide a documented capacity setting; allocate capacity outside real-time processing; never change the underlying storage pointer without safe handoff. Do not equate ten minutes of advertised operation with a universally fixed host sample count.

Persist schema version, selected presets and modified values, valid recorded extents, region, transport modes, link mode, monitor/one-shot state, calibration/timing policy, and actual audio assets. Decide explicitly whether tap positions/effect tails are resumed or reset on load; document the choice. A preset is not a complete snapshot.

Use immutable or copy-on-write snapshots only where their worst-case real-time behavior is controlled. Unbounded page copying during a high-speed write is not automatically safe merely because it is called copy-on-write. A background serializer with preallocated transfer buffers and a defined snapshot boundary may be simpler.

For portable patches, store audio with the patch's asset system or a stable managed sidecar, not just a user's absolute filesystem path. Validate length/rate/channel counts and fail with a visible message rather than substituting random memory or allocating an attacker-controlled size.

## 9. Completion and fidelity gates

**Playable prototype:** two-deck record/play/overdub, usable signed varispeed, safe region manipulation, single-tap fades, async persistence. Call it a prototype, not a completed recreation.

**Behavioral recreation:** ten preset configurations, multi-tap/Hold modes, clocks, one-shots, monitoring, link, routing and touch with documented deviations. All state/event/asset tests pass.

**Higher-fidelity DSP recreation:** complete verified write/read transitions, anti-alias stages, filter coefficients, flutter, plate, normalization and analog calibration. Compare actual recordings across parameter sweeps and hardware revisions. Publish remaining differences rather than a meaningless overall fidelity percentage.

Do not block useful native implementation on every last UI detail. Do block claims of precise equivalence on the unresolved features that audibly determine behavior.

## 10. Immediate next reverse-engineering work

The most valuable next analysis is a **whole record/playback transport harness** around `recordInput`, `AudioData::interpolate/add`, and Tap transitions, followed by a full `MonoPlate` reconstruction. In parallel, hardware clock/analog measurements would resolve uncertainties that no amount of reading this update alone can settle. These tasks fit the evidence and directly improve the new module rather than adding unrelated features.
