# State machines and event semantics

**Purpose:** identify the state a native implementation must represent, and separate recovered facts from incomplete transition logic. The state names below are proposed software names, not a recovered source enum. Addresses are virtual addresses in the named shared object.

## 1. Capture state is not one boolean

The GPIO layer exposes `_setRecordingState`, `_setRecordingStateWithReset`, `_toggleRecordingState`, `_toggleAccumRecording`, and distinct button-down/up handling. The recorder owns the audio-write phase. `arbhar_rec~` perform at 0x4288 shows two writer records, fades, finite bounds, and a smoothed Dub-related coefficient. [F/S]

A native model should keep these dimensions separate:

| Dimension | Candidate values | Evidence boundary |
|---|---|---|
| User capture request | idle / active / retrigger requested | Exact CV enum values recovered; all simultaneous-event precedence not recovered. |
| Writer lifecycle | stopped / starting / writing / retiring | Two writer records and fading observed; full envelope transition equation remains open. |
| Capture policy | ordinary / accumulative | Exact configuration switch exists; do not infer ring-buffer behavior. |
| Write destination | selected record layer | Distinct play/record layer and coupling controls exist. |
| Write position | reset position / current position / parked resume position | Mode dependent; capture reset versus accumulative resume must be explicit. |
| Input mode | mono analysis/record split / stereo | Changes both data routing and onset-related behavior. |

### Capture CV contract

Configuration labels are exact: **0 latch, 1 momentary, 2 retrigger**. A likely baseline interpretation is edge-toggle, gate-held recording, and new-capture restart respectively, but exact interaction with active fades, onset requests, button state, and accumulative mode requires a truth table. Treat the following as required tests, not a claimed completed reconstruction:

| Existing state | Event | Required question |
|---|---|---|
| Stopped | CV rising edge | Start now or after scheduling boundary? Which controls/layer are latched? |
| Writing | CV rising edge in latch mode | Which writer enters retirement, and when does its last sample occur? |
| Writing | CV falling edge in momentary mode | How does the stop fade interact with remaining input and Dub? |
| Writing | CV rising edge in retrigger mode | Does old writing overlap new writing, and how are their gains combined? |
| Accumulative paused | Start request | Resume the parked write position or reset under a particular linkage option? |
| Any | Manual button + onset in same interval | Which event wins; can a second event retrigger the first? |
| Any | Record-layer change or scene load | Which grains/writers retain the previous layer generation? |
| Writing near end | End reached | Fade, stop, park, or schedule another action? No assumed perpetual wrap. |

Starting functions: GPIO `_toggleAccumRecording` 0xbb70, `_setRecordingStateWithReset` 0xb398, `tick_processButtonDown` 0xbcfc, `tick_processButtonUp` 0xb3d8; recorder `arbhar_rec_tilde_checkCaptureButton` 0x4a14 and perform 0x4288.

## 2. Grain slot lifecycle

The compiled player distinguishes an active flag from a retirement flag and gain. Pool capacity is 82 records; activity-limit semantics are a separate question. Proposed model:

```text
AVAILABLE -> PREPARED -> ACTIVE -> FINISHED -> AVAILABLE
                          |
                          +-> RETIRING -> AVAILABLE
```

`PREPARED` is a native design state for explicit delayed launches; it is not a proven original enum. Parameters should be snapped at a deliberately defined point. A scheduled Strike event, for example, may intentionally wait for pitch CV stabilization, so “trigger edge” and “grain launch” cannot automatically be collapsed.

The retirement gain step is observed at the perform-block level. Count block visits in traces and report block size/rate alongside sample counts. Do not use `gain -= 1/64` in Rack's per-sample callback.

Required trace fields: engine, event ID, record index, previous/new lifecycle, active count, retirement count, layer/generation, source event timestamp, actual start timestamp, duration, pitch ratio, texture row, direction, and random draws. The trace must be optional and nonblocking; the audio thread can write to a bounded preallocated ring.

Starting functions: player constructor 0x619c, `play_next` 0x6c34, `play_next_override` 0x393c, `_killAllGrains` 0x294c, `arbhar_play_tilde_kill_selected` 0x29a8, perform 0x4248.

## 3. Continuous and Strike engines

Separate sources are visible in the main/core graph. A continuous-engine tick and a Strike input are not interchangeable. There are distinct trigger clocks, MIDI/chord paths, and output gain controls.

Native scheduling should represent periodic/asynchronous candidate events, cancellation or coalescing policy, delayed Strike events, and launch-sampled controls. The exact Intensity-to-rate curve, duration-dependent limits, and randomized scheduling remain open. Keep timing randomization independent of amplitude randomization because configuration exposes both switches and the named factory presets use different defaults for them.

Starting functions: player `arbhar_play_tilde_tickInternTrig` 0x7928, `arbhar_play_tilde_tickPolyphonicTrig` 0x7ac8, `_setInternTrig` 0x7a58; GPIO Strike timing, clock, and event handlers; `arbharGrainCore.pd`.

## 4. Follow, Scan, and Omega

The pure Follow-speed helper is numerically probed, but it is only one part of a transport. A complete state model needs current head position, direction, speed, loop bounds, offset configuration, selected layer, capture position, and any recording-head exclusion zone.

Scan and Follow use different start-position/spray branches. Omega introduces a combined layer/position coordinate and a top-end guard. Do not reuse a single `wrap(scan + uniform(-spray,+spray))` expression for every mode.

Required boundary events include: Follow stop/zero crossing; unity-speed plateau entry/exit; loop change while grains remain alive; playback-layer switch; capture reset while Follow is reading; negative pitch/read increment; all Omega layer boundaries; exact maximum control value.

Starting anchors: GPIO `calculateFollowSpeed` 0x3f10, `_setPlayParameters` 0x7c50, `_setFollowMode` 0x76b8; player `_getPositionRange` 0x6868. The comparison near 0x6a20 in that player function remains a specific unresolved dataflow branch.

## 5. Gesture state and Track & Hold

Do not model Shift as a global freeze switch. Separate ordinary performance, Shift-modified gestures, staged parameter edits, configuration menus, and file operations. An explicit staged snapshot allows Track & Hold changes to be applied together without suspending unrelated capture or effects state.

The complete button-order detector and hold/double-tap timing are not reconstructed. A native prototype can choose a clear gesture policy, but it must remain marked as an adaptation until compared with the GPIO handlers or hardware. File-management gestures are especially dangerous to guess because they can mean erase, undo, load, or save depending on state.

Starting anchors: GPIO `_getButton`, `_setOrder_button`, `getModeFromOrder`, `tick_processButtonDown` 0xbcfc, `tick_processButtonUp` 0xb3d8, and controls patch. Do not run the original shell actions while testing gesture interpretation.

## 6. Onset, Hold, and reverb ducking

The detector generates analysis events; the current onset profile decides how those events affect capture, grains, or other behavior. Keep detector state, onset holdoff, capture hold state, and reverb duck state separate. A single “onset enabled” boolean is insufficient.

A practical native decomposition is `DetectorEvent -> OnsetPolicy -> TimedInstrumentEvents`. The exact six policy tables must be recovered rather than guessed from their Greek names. The connected default analysis is `bonk~` with 256-point/32-hop/11-filter arguments after four high-pass stages; the custom FFT tester is not the normal path.

The reverb interruption patch ramps level down and restores it over 250 ms. Treat tank clearing as unproven. Test onset/Strike events while reverb is already ducking, while routing changes, and while the reverb macro is zero.

Starting anchors: recorder `_processOnsetData` 0x4044; GPIO `_setOnsetMode` 0x6424 and `_setHoldValue` 0xe4f4; `arbharRecorder.pd`, `arbharReverb.pd`.

## 7. Persistence and preset application

`LoadConfiguration` distinguishes nothing/preset/layers/scene. Applying a preset is not necessarily replacing audio. The startup script chooses USB preset, then autosave, then the initialization fallback; the initialization file is not identical to factory alpha.

Use atomic staged file adoption in Rack. A recommended transition is `idle -> preparing off-thread -> ready -> audio-boundary adoption -> old-generation retirement -> idle`. Failure before adoption should retain the current sound and report an error without corrupting current layers. This is a host reliability design, not a recovered original file-state enum.

## 8. Compatibility gate

These proposed state names become implementation scaffolding now. They become **recovered compatibility behavior only when each transition is linked to compiled dataflow or an observed oracle trace**. Keep open transitions in the issue ledger rather than filling them with plausible but undocumented behavior.
