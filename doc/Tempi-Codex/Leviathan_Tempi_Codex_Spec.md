# TEMPI 71 behavioral recreation for Leviathan

Revision 1 — 2026-09-28. Status: implementation specification; no module implementation implied.

## 1. Product contract

Build a six-channel rational polyclock and performance-memory instrument whose musical behavior follows TEMPI firmware 71. Preserve the way clocks relate, acquire tempo, change phase, respond to MOD, and remember edits. Rework interaction for Rack: readable values, direct editing, visible routing, and a persistent visualization are first-class requirements.

Mandatory first-release scope: shared Leading Tempo; six clock/trigger outputs; Human and Machine Programming; asymmetric phase programming; mute; all Shift and Run/Stop modes; four banks of sixteen States; explicit Store/Recall/Revert; State CV and Gate selection; Copy/Paste/Mutate; Mesh; receive-only Select Bus semantics through a software interface; complete patch persistence; and visualization of actual engine state.

Hardware panel gestures are not the fidelity target. Their musical operations and state-selection inhibition are. Do not reproduce PIC instruction execution, EEPROM delays, power-bus electronics, LED multiplexing, or a firmware updater in the plugin. No additional sequencer, swing, probability, per-lane ratio CV, host transport synchronization, expander, or polyphonic output bus is required for v1. Those are separate future extensions.

Use `Tempi` as a working code/asset name. Decide the public name and immutable model slug before shipping presets. Proposed panel width: 24 HP; adjust after an actual rendered layout review. These are design proposals, not recovered facts.

## 2. Source authority and fidelity accounting

Read these local sources before implementing their corresponding subsystem:

| Source | Role |
| --- | --- |
| [Behavioral engineering spec](../../firmware/Tempi/TEMPI71_BEHAVIORAL_ENGINEERING_SPEC.md) | Primary narrative contract, especially §§4–19 and 22–27 |
| [Evidence matrix](../../firmware/Tempi/TEMPI71_EVIDENCE_MATRIX.csv) | Confidence per subsystem |
| [Timing reconstruction](../../firmware/Tempi/docs/TIMING_RECONSTRUCTION.md) and [timing models](../../firmware/Tempi/tools/timing_models.py) | More precise capture, interpolation, reload, and commit-fragment contracts |
| [Readable arithmetic models](../../firmware/Tempi/tools/models.py) | Explicit integer widths, wrapping, truncation, ADC behavior, PRNG |
| [Select Bus detail](../../firmware/Tempi/docs/SELECT_BUS.md) | Exact parser/dispatch semantics; qualifies narrative “Save” shorthand |
| [Validation limits](../../firmware/Tempi/docs/VALIDATION_AND_LIMITS.md) | What the harness does and does not establish |
| [Firmware implementation checklist](../../firmware/Tempi/TEMPI71_IMPLEMENTATION_CHECKLIST.md) | Coverage cross-check, not proof of implementation |
| [Leviathan standard](../Leviathan-Standard.md) and [repository instructions](../../AGENTS.md) | Integration, performance, graphics, compatibility, validation |

The behavioral spec stands out for its explicit A/B/C/D confidence model and executable evidence. Preserve those distinctions: A = recovered/verified digital behavior; B = corroborated/documented semantics; C = inference; D = open. This document adds **R = deliberate Rack adaptation**. R never upgrades a source claim to A or B.

Where summaries conflict, consult the addressed routine, executable model, and narrow test domain. Record the resolution. For example, Select Bus `F4 ss` dispatches a copy-current-to-target path for `ss < 64`; it is not evidence for blindly persisting an arbitrary target slot. Likewise the behavioral spec's final pseudocode is illustrative: generating trigger edges after mute masking can manufacture a pulse on unmute. The output contract below makes the chosen Rack behavior explicit.

Maintain an implementation ledger mapping requirement IDs below to code, tests, evidence level, profile choice, and status. Existing reports record 13,409 follow-up timing cases; that is historical source evidence, not a test run of the new module. Shared decoder/harness definitions limit independence.

Two delivery claims are distinct:

- **Complete Rack prototype:** every required feature works under the declared Rack profile, including explicitly provisional Human/transition behavior.
- **Faithful behavioral release:** major Human, commit/history, source arbitration, mutation, and Mesh/Follow gaps have been resolved or bounded by reviewed evidence and regression traces. Any remaining deviations are disclosed. Do not claim bit-perfect or hardware-cycle equivalence.

## 3. State and ownership model

**STATE-01.** Keep three domains separate:

| Domain | Required contents |
| --- | --- |
| Musical memory | `stored[64]`, `editable[64]`; each State has signed ratio[6], phase byte[6], enable mask, MOD mask |
| Configuration | Current bank, Human resolution, Shift mode, Run/Stop mode, momentary/toggled MOD, six output modes, Leading Tap setting, Follow setting, stored Leading period; stored and edited configuration where relevant |
| Runtime | Master capture/history, six timer records, source permutation, Run/Stop phase displacement, physical lane run state, selector base/offset, dirty bits, Mesh bitmap, tap histories, clipboard, PRNG, pending transitions |

The logical State is fourteen bytes, not necessarily a packed C++ object. Use fixed-size containers; mask membership to six bits. Preserve phase values 0..255 in memory, including factory values above 3. Machine ratio editing covers -124..124. Corrupt imported values outside the supported musical range must be handled deterministically and reported, not used as array indices.

**STATE-02.** Shift mapping and induced Run/Stop offsets do not overwrite stored ratio/phase. State departure/re-entry resets those transformations. Mute belongs to the physical lane, not to the shifted timing source. Separate derived half-period, programmed phase offset, pending timing, and transient Run/Stop displacement.

**STATE-03.** Load factory content from the recovered [factory data](../../firmware/Tempi/analysis/factory_states.json), converted into a compile-time fixture or generated header with provenance. Do not require the firmware dossier at plugin runtime. Bank A contains the sixteen recovered patterns; States 17–64 are unity/zero-phase. All factory enable masks are `0x3f`, MOD masks zero. State 15 is six `r=-2` values with phases `[0,1,2,3,4,5]`.

## 4. Timing and arithmetic contract

**TIME-01 — common reference.** One shared master timeline and rational alignment metadata drive all six lanes. Independent free-running floating-point oscillators are insufficient. Recompute ratios, phase and alignment only when their dependencies change. Do not construct an array covering the entire LCM cycle or iterate every beat in that cycle.

**TIME-02 — exact recovered helpers.** For master half-period H and ratio code r:

```text
r > 0: h = truncTowardZero(wrapSigned32(4*H) / (r+4))
r = 0: h = H
r < 0: h = truncTowardZero(wrapSigned32(H*(4-r)) / 4)
h = clamp(h, 200, 0xFFFFFF)

base = r < 0 ? H : h
phaseOffset = truncTowardZero(wrapSigned32(base * phaseByte * 2) / 4)

alignmentFactor = r < 0 ? (4-r)/gcd(4-r,4) : 4/gcd(r+4,4)
```

Use explicit unsigned operations/sign conversion to reproduce wrapping without C++ signed-overflow undefined behavior. Preserve the executable models' ordering. A wider mathematical result is not an equivalent replacement for the wrapped helper. Keep the firmware's 16-bit LCM behavior testable separately from safe, wider runtime alignment bookkeeping; examine overflow before deciding how it affects scheduling. Never allow a zero/wrapped alignment length to cause division by zero or runaway work.

Fine ratio edits change r by ±1. Integer multiplication/division n uses ±4(n−1), with n in 1..32. Display `×1`, `×1.25`, `÷1.25`, etc.; a linear BPM multiplier knob cannot represent the recovered encoding correctly.

**TIME-03 — phase units.** A phase-code unit is a quarter master cycle for division, and a quarter channel cycle for unity/multiplication. Show that reference explicitly. Do not normalize every lane's phase parameter to 0..360 degrees of its own clock. Raw-byte editing can cover 0..255 while the display also shows the effective wrapped phase; wrapping the display must not erase the stored byte.

**TIME-04 — reload and commits.** Retain current reload, next reload, remaining countdown, and wave level. Recovered expiry order is decrement → toggle when signed remaining ≤0 → load old current into remaining → promote next into current. The output pipeline delays prepared values by one virtual timer event. Preserve it in the reference path and account for it in edge fixtures.

Commit guards defer when master or lane remaining <76 ticks, or lane remaining falls within inclusive `[nextH−75,nextH+75]`. These are necessary fragments, not a complete synchronization policy. Port/reconstruct `0x234A` history decisions before claiming faithful live edits. A prototype policy is specified in section 11; isolate it so replacement does not rewrite the engine.

**TIME-05 — sample bridge.** Keep the firmware arithmetic domain distinct from Rack seconds/samples. Use a sample-rate-independent virtual tick scale, fractional remainder, and monotonic integer timestamps. Host output edges occur at the first sample at or after their virtual event; error is less than one sample relative to that virtual schedule. Preserve absolute timing through sample-rate changes; do not round each half-period independently to samples and accumulate drift.

Actual hardware seconds-per-tick remains unknown. The prototype's declared scale in section 11 is a software choice, not a measurement inferred from the 200-tick clamp or nominal trigger duration. Do not silently change it after patches exist.

## 5. Leading Tempo and Rack I/O

**IO-01.** Provide four monophonic inputs: Leading Tempo, MOD, State Gate, State CV; six separate monophonic clock outputs. Read channel 0 of a polyphonic cable. Proposed R convention: low=0 V, high=10 V; Schmitt rising threshold 1 V and rearm ≤0.1 V. Sanitize nonfinite input voltages. These are Rack interface choices, not recovered physical electrical specifications.

**LEAD-01.** Implement at least two-edge external acquisition. Zero capture is no pending interval; reject signed-negative or nonzero interval <399 ticks. Accepted H is `clamp(trunc(interval/2),200,0xFFFFFF)`. Replace changed requested H directly; no added moving-average filter. Shorten a live countdown only when it exceeds the new H. Equal H does not force phase correction.

**LEAD-02.** Implement the separate captured-countdown/level correction from the timing model, including signed wrapping. Clear measurement on nonnegative elapsed ≥0xFFFFFF, or after an accepted interval when elapsed >15H. The latter is 7.5 full periods. In the recovered Tap-enabled/no-other-candidate case, clock loss leaves the master running at its last requested tempo. Display acquiring, tracking, and continuing-after-loss separately.

**LEAD-03.** Support stored internal tempo, external clock, and Channel 1 Leading Tap. Tap disabled reassigns the State combo control/CV to tempo. Keep this coupling visible: relabel the control and jack status as `TEMPO` and disable direct State-CV expectations. A separate Rack bank/slot picker remains available as an R convenience. A bus-derived physical clock is outside the receive-only Select Bus parser; do not treat `F8` as Leading Tempo.

**LEAD-04.** Use recovered ADC tempo deadband, endpoint doubling, integer interpolation and caller clamp with declared synthetic calibration until physical calibration is known. Do not replace that path with an unlabelled linear BPM law. A BPM entry for setting internal tempo is an additional R convenience, subject to the same master-period limits and active-source priority.

**OUT-01.** Each lane supports a 50% clock or nominal 10 ms trigger. Pulse width is in seconds, independent of virtual tick assumptions. Generate pulse envelopes from internal rising events before final mute/run output suppression. Muting cannot generate a new trigger or restart timing; stopped/muted outputs are zero immediately. On unmute, expose the remaining internal envelope, if any, without restarting it. At rates above 100 Hz, retrigger the envelope, potentially producing continuous high. This envelope/retrigger/unmute policy is R pending output-path evidence; test it and disclose it.

**OUT-02.** Internal timing and tap/edit capability continue while muted. Visualization distinguishes an internal event from a voltage transition. Run/Stop is a timing transition, not merely this output mask.

## 6. State selection and performance transformations

**SELECT-01.** Four banks ×16 slots; show A–D and 1–16, retain zero-based indices internally. Within a bank, effective slot is `(baseSlot+gateOffset)&15`. A changed absolute selection resets gateOffset. State Gate rising edges increment it. Bank changes and direct recalls clear the relative offset under the R policy.

**SELECT-02.** Map State knob plus CV through a declared clamped 0–5 V control domain and 10-bit ADC conversion. Preserve the recovered state quantizer and its previous-selection hysteresis using the chosen calibration array. Do not substitute a generic Schmitt band because the prose sounds equivalent; use `models.adc_state` fixtures. State CV wins over relative stepping on simultaneous changes under section 11's ordering.

**SELECT-03.** Explicit Human, Machine, Phase, Mute, and MOD editing sessions inhibit external State CV/Gate selection. In a direct interface, a drag/entry gesture opens a short transaction; Human and dedicated pages are explicit longer sessions. The display must show selection held. Do not freeze selection merely because a lane has keyboard focus. Exact backlog, exit, and MOD interaction remain qualified; prototype handling is in section 11.

**MOD-01.** Per-State MOD mask, global Shift `{Off,CW,CCW,Random}`, Run/Stop `{Off,Normal,All,Alternate}`, gate handling `{Momentary,Toggled}`. Toggled changes logical gate on rising edges only. Clearly indicate logical gate and physical input level when they differ.

**MOD-02.** Shift rotates timing-source assignments among enabled, MOD-member lanes; muted lanes are excluded. With fewer than two eligible lanes it is a no-op. Physical mute stays in place. CW/CCW must have a visible direction independent of the redesigned linear panel; define source-to-destination mapping and test it. Random uses the recovered PRNG; permutation draw order needs a separate evidence check, not an assumption based on the known generator alone.

**MOD-03.** Normal Run/Stop affects MOD members: logical high runs, low stops. All affects every lane; on Run, members start immediately, nonmembers align to Leading Tempo. Alternate runs members on high/nonmembers on low and stops the other group. Immediate restart can create a persistent runtime phase displacement. Track it independently from programmed phase. Subsequent Shift preserves the induced offset according to v71 semantics; state/timing changes clear or replace it as appropriate. Resolve source-owned versus destination-owned offset behavior using transition traces before the fidelity gate.

**MOD-04.** When Shift and Run/Stop are both enabled, MOD controls Run/Stop and State Gate controls Shift instead of stepping States. Show this reassignment beside the jack and in its tooltip. With Shift alone, MOD rising edges cause Shift. With neither enabled, State Gate steps and MOD has no timing action.

## 7. Human and Machine interaction

**EDIT-01.** Six large tap targets support Human Programming with at least two taps, simultaneous independent lane capture, and resolution 100/50/25% (50% default). Channel 1 also supports Leading Tap when enabled. Provide an explicit intent selector (`Channel learn` / `Leading tap`) so the Rack interface can disambiguate these actions without inventing hidden mouse chords. Both routes feed timestamped engine commands.

Human Programming is mandatory. Reconstruct `0x340A`: interval rejection, history length, candidate ratios, phase metric, resolution grids, multi-lane interaction, termination, and timing commit. The spec does not establish that 50% means any particular mathematical quantization. A prototype approximation must be identified in user documentation and the fidelity ledger.

**EDIT-02.** Each lane has direct ratio editing, coarse/fine steps, phase editing, mute, MOD membership, and clock/trigger mode. Provide numeric entry and keyboard operation alongside dragging. Coarse ratio sets integer multiples/divisions through 32; fine edits traverse the signed quarter-step code continuously across unity. Machine programming can affect a muted lane.

**EDIT-03.** Recover coarse/fine phase gesture-to-byte transforms (`0x11C0`) before claiming equivalent phase operations. A prototype raw phase-code editor and ±1 controls are valid R operations, but are not evidence that every hardware coarse gesture is implemented. Coarse phase must ultimately override prior fine adjustment as documented.

**EDIT-04.** Copy/Paste supports State and Bank scopes using a stable value copy, not a live pointer to the source. Paste modifies editable memory, never silently Stores it. Mutate uses `s=s*0x5851F42D4C957F2D+1 (mod 2^64)`, byte `(s>>49)&255`, ratio delta `(byte%7)-3`, phase delta `(byte%5)-2`. Preserve modulo bias. Draw order, normalization, repeated-mutation source, and hardware seed provenance are separate unresolved questions; use an explicit deterministic prototype policy and test it.

## 8. Memory, Rack persistence, and Select Bus

**MEM-01.** Switching States retains edits in the 64-State editable cache. Dirty means editable differs from stored. Store Current copies selected editable content to its stored counterpart; Recall Current restores it; Recall Bank restores sixteen editable States; Store All copies all editable States and persistent settings plus current Leading period. Revert restores dirty material/settings from stored memory. These operations affect module memory immediately, not a shared file outside the patch.

**MEM-02.** Rack patch save serializes both stored and editable memory. It must preserve unsaved edits *and their dirty distinction*. Saving a `.vcv` must not execute TEMPI Store. The patch contains versioned JSON with schema version, behavior profile version, both memory images, configuration, selection, Mesh, deterministic PRNG state, and persistent UI preferences. Encode uint64 values losslessly (for example hexadecimal strings). Derive/validate dirty flags from actual comparisons.

R load policy: restore memory and selected State; resume the saved current master rate when no external source has acquired; initialize canonical routing and clear transient Run/Stop displacement, gate latches, pending edits, pulses, and tap histories. This is musical-state recall, not sample-exact transport resume. Do not synthesize a trigger solely from deserialization. Document this policy in the user guide. Fresh instance defaults and Rack Initialize are distinct from patch load.

**MEM-03.** Prove thread-safe save/load ownership against the actual Rack SDK and host lifecycle before choosing an implementation. Save must see one coherent memory/configuration revision and work while audio is suspended or bypassed. Never wait for a future audio callback to satisfy a save. Never use the UI's SPSC display reader as a second save reader. Implement a separate immutable/coherent save publication or a verified host-synchronized mechanism; include load adoption and UI commands while stopped in the proof.

Validate exact array dimensions, enums, finite numeric fields, profile identifiers, and ranges before adopting JSON. Older schemas receive explicit migrations/defaults. Unknown future schemas must not be silently interpreted and rewritten as v1: retain their opaque payload or refuse adoption with a recoverable diagnostic. State/Bank edit, Paste, Mutate, Store, Recall and Revert should participate in Rack undo through value-based before/after commands, without moving real-time clock history backward.

**BUS-01.** Implement a byte parser and typed semantic receiver, with no global implicit broadcast. User-visible State/Bank/Mesh operations invoke the same validated semantic actions. Provide an advanced local `Receive Select Bus message…` action accepting a bounded hexadecimal message so the receive path is usable without a new expander or physical connection. It submits bytes through the module's command handoff; it performs no UART/MIDI/network I/O. Normal performance uses labelled musical controls.

Required parser behavior: exact `C0`, no generic `C1..CF` substitution or running status; ignored interleaved `F8`; header `F0 00 02 2D`; Mesh clear/set pairs `00 ss`/`01 ss`; Default `02`; Revert `03`; `F7` termination. Invalid input cannot index outside 64 States. Preserve parser acceptance separately from Follow-dependent application and selection gating.

**BUS-02.** For `F4 dd`, preserve recovered dispatch: dd<64 selects copy-current-to-target; 64≤dd<128 selects persistent-store dispatch. `0x40` is the conventional Save All value, but the recovered accepted range is wider. Trace `0x88D4` before deciding downstream persistence scope; do not substitute “Store target slot” based on the narrative label. Include Mesh clear and Follow bookkeeping in the trace.

**BUS-03.** Mesh is a 64-bit target set with visible membership and multi-paste workflow. Mesh edit, propagation, Store, and Revert must retain their distinct meanings. Trace Follow gating and propagation triggers before faithful release. The initial Rack policy is explicit Multi-Paste from the current editable State to selected Mesh targets, leaving stored copies unchanged; do not assume every arbitrary edit automatically propagates. Follow defaults off in the Rack profile; remote State requests apply when Follow is enabled, while other command effects must follow the traced dispatcher.

## 9. Visualization and panel contract — tier 1

**VIS-01.** The primary display is a six-lane time view sharing a Leading Tempo ruler. It must answer, without opening menus: what is each lane's ratio and phase, where is it in time, which output will fire, why is a lane silent, and what has been edited? A decorative animation or six blinking LEDs is insufficient.

Proposed 24 HP layout (placement concept, not pixel coordinates):

```text
┌─────────────────────────────────────────────────────────┐
│ Leviathan / working name     tempo · source · sync status │
│ A B C D   [16 State cells]    Store / Recall   dirty count │
│ Leading ruler ─────┬─────────┬─────────┬────── now ─────── │
│ 1  TAP  ratio phase  ─── six aligned timing lanes ── M MOD │
│ 2  TAP  ratio phase  ─────────────────────────────── M MOD │
│ 3  TAP  ratio phase  ─────────────────────────────── M MOD │
│ 4  TAP  ratio phase  ─────────────────────────────── M MOD │
│ 5  TAP  ratio phase  ─────────────────────────────── M MOD │
│ 6  TAP  ratio phase  ─────────────────────────────── M MOD │
│ Human resolution · edit intent · Shift · Run/Stop · latch │
│ LEAD in  MOD in  STATE GATE in  STATE CV in  STATE/TEMPO   │
│                   OUT 1  2  3  4  5  6                   │
└─────────────────────────────────────────────────────────┘
```

State controls may occupy a second row after real layout review; readability at Rack's default zoom takes priority over preserving this sketch. All six lanes and their outputs remain visible during programming.

**VIS-02.** Required layers and readouts:

- Master beat ruler, BPM/period, source/acquisition/loss status, and current master position.
- Six ratio labels and programmed phase units; pending edits distinguished from committed timing.
- Internal rising-edge history and actual gate/trigger history, including suppressed events while muted. Stopped lanes show stopped status without fabricating running output.
- Lane source badges such as `5 ← 2` during Shift, plus induced Run/Stop offset distinct from programmed phase.
- Active State, base slot and relative offset, bank, dirty marks, Mesh membership, and held State-selection indicator.
- Per-lane mute, MOD, run state, and clock/trigger mode; selected Human resolution and capture progress.

Use the documented blue/red/purple/pink state distinctions where helpful, but accompany color with text/icons. Input/output theme roles must not make status interpretation depend on a specific theme. Long divisions need adjustable 1/4/16/64-master-cycle windows and a next-event readout; never expand memory to a complete enormous alignment cycle.

**VIS-03.** History comes from the engine's actual event timeline; predictions are visually differentiated and invalidated on tempo/State/MOD/edit changes. For uncertain history transitions, omit prediction beyond the known commitment horizon. Do not extrapolate six UI oscillators. Muted internal edges remain visible, but low physical voltage remains truthful. Decimated history uses edge counts/first-last timestamps or equivalent bounded bins so fast pulses are not lost simply because no UI frame coincided with a high sample. A history gap/overflow must be marked, not drawn as silence.

**VIS-04.** Publish value-only snapshots at a bounded visual cadence (target ≤60 Hz) with bounded retained history. One widget owner reads the SPSC snapshot and distributes a local immutable copy to subwidgets. UI can sleep, disappear, or run slowly without affecting DSP. Turning visualization off must leave output traces bit-identical. Browser previews with null modules show a labelled representative static view.

**VIS-05.** Use `res/Tempi.svg` as the editable master if the working basename survives. Add stable anchors for display, each lane's controls, every port, State controls, and branding. Assemble with `PanelSvgUtils.hpp` and shared `VisualAssets.hpp` facilities (`visual_assets::SplitPanelRenderer` is the standard's starting reference); inspect APIs rather than inventing signatures. Use shared controls, semantic glass and theme-text groups, pure black base, and generated split assets. Regenerate split SVGs and the panel anchor atlas after master changes.

Cache the grid/static geometry separately from live markers, histories, and labels. Prefer Rack-owned framebuffers/NanoVG initially; custom GL is not required. Apply `NvgGraphicsLifecycle.hpp` and, if applicable, `GlLifecycleUtils.hpp`. Validate resize, zoom, high DPI, theme changes, preview construction, and DAW window close/reopen. Do not delete context-owned images in another context.

## 10. Implementation architecture and host lifecycle

**ARCH-01.** Suggested responsibility split, adjusted to the real build system:

| Component | Responsibility |
| --- | --- |
| `TempiCore` | Rack-independent tick/event scheduler and authoritative musical state |
| `TempiArithmetic` | Exact wrapped arithmetic, ratio/phase/alignment, PRNG |
| `TempiLeadingClock` / `TempiCommitPolicy` | Capture/source state machine; history and deferred commits |
| `TempiProgrammer` / `TempiStateStore` | Human/Machine operations, memory, dirty tracking, clipboard |
| `TempiModRouter` / `TempiSelectBus` | Performance transformations, protocol decoding and dispatch |
| `Tempi` Rack module | Params/ports, edge detection, command admission, lifecycle, serialization |
| Widget/display | User intents, theme-aware rendering, immutable visual consumption |

No Rack dependency is necessary in the mathematical/state core. Register the model in `plugin.hpp`, `plugin.cpp`, and `plugin.json`. The Makefile does not automatically compile arbitrary new subdirectories; add sources deliberately. Do not change released modules' IDs, state, or behavior to integrate this one.

**ARCH-02.** Audio owns running engine mutations. UI sends typed fixed-size commands; physical CV/gates are processed every sample, not UI-decimated. Define producer counts for UI, protocol, load, and automation before choosing queues. A latest-value snapshot is unsuitable for tap/Store/Shift commands. Use bounded ordered delivery with acknowledgements and explicit full-queue behavior: reject an unaccepted UI operation visibly, never silently drop an accepted discrete action. Continuous preview gestures may coalesce; their final committed value must be acknowledged. Bound command work per sample and document added command latency.

State-dependent controls are views into the selected editable State, not a second authoritative six-lane memory. Rebinding a knob after recall must not write the previous State's values into the newly selected State. Commands carry their target State and selection/edit revision; stale commands are rejected visibly or completed against their explicit target, never redirected silently. Provide Rack parameter metadata/tooltips for automatable controls and document whether automation targets the current State or a fixed slot. Freeze parameter/port/light IDs before distributing patches; later additions append. Cover recall during dragging and automation during State CV changes in tests.

**ARCH-03.** Allocate everything needed for clocking, history and normal commands before audio processing. No blocking locks, heap allocation/free, file I/O, JSON/string formatting, or sockets in process. Six lanes have bounded work. Avoid per-sample transcendental functions and repeated GCD/LCM. UI history loss cannot stall audio. Profile max-rate input, all six lanes, rapid State/MOD, and multiple instances.

**ARCH-04.** Define lifecycle transitions: sample-rate change preserves remaining musical time and pulse duration; bypass writes zero outputs and suspends timing under the R policy; resumption clears input measurement and transient pulses, reacquires external tempo, and restarts from a documented canonical boundary. Rack Initialize resets factory memory/settings. A separate `Restart timing` action resets transient timing without erasing memory. No new reset jack is required. A restored high cable is sampled as a fresh edge after rearming under the R policy; test and document its effect on toggled MOD. Save still works while bypassed/stopped.

**ARCH-05.** Gate developer telemetry with `isDragonKingDebugEnabled()`. The first three metrics remain aggregate module `Process`, `Step`, `Draw`. Add scheduler, history, and display components after those. Formatting/transport occur off audio. Report conditions and compare display enabled/disabled; establish a measured baseline rather than inventing a portable microsecond budget.

## 11. Explicit prototype profile and unresolved fidelity gates

Use behavior profile `rack-tempi71-v1` for the following provisional choices. Keep these values centralized and serialized; they are not discovered firmware constants. Recovered replacements must be versioned when existing patches would change.

Fresh Rack defaults: recovered factory memory in both images, bank A/slot 1, offset zero, all outputs 50% clock, Leading Tap enabled, Human resolution 50%, Shift off, Run/Stop off, momentary MOD, Follow off, empty Mesh/clipboard, identity routing, PRNG seed 1, internal 120 BPM, and a four-master-cycle display window. These configuration defaults are R choices unless separately verified against recovered initialization. Rack Initialize reinstates them; ordinary patch load does not.

| Area | Executable prototype policy | Faithful-release work |
| --- | --- | --- |
| Virtual time | 32,000 virtual ticks/s; fresh internal tempo 120 BPM (H=8000); fractional sample bridge and recovered clamps | Determine physical timebase or explicitly retain a qualified virtual adaptation |
| Calibration | `ADC=round(clamp(knobVolts+CV,0,5)*1023/5)`; `T[j]=32+64*j`; use recovered state/tempo helpers | Bound/measure calibration differences; never call synthetic thresholds hardware calibration |
| Source priority | Accepted external capture wins over same-tick internal candidate; suppress Leading taps while external measurement is active; otherwise Tap mode uses accepted Leading taps/last tempo, Tap-off admits combo tempo | Recover complete source/Follow/Human arbitration |
| Human estimate | Keep last two taps per lane; interval must be ≥399 ticks; end session after two requested master periods of inactivity; 100% considers ratio codes divisible by 4, 50% by 2, 25% all codes in -124..124 | Recover exact quantizer, debounce/history, session termination and resolution grids |
| Human phase | Choose candidate ratio minimizing absolute measured-period error, ties toward unity then lower code; choose nonnegative phase offset nearest tap position modulo candidate full period, from byte 0..255 in steps 4/2/1 for resolutions 100/50/25; ties choose smallest byte | Recover phase metric and actual coarse/fine relationships; these grids are provisional |
| Prototype commit | Queue newest requested values per lane, wait for numerical guards, then adopt at the next master rising boundary; place next lane rising event on the shared master-origin schedule plus programmed offset; clear that lane's induced offset on its timing edit | Reconstruct history scheduler; guards alone cannot establish faithful transitions |
| Edit freeze | Drop incoming State Gate edges during a session, retain latest absolute CV for exit; suspend Shift/Run changes during Human/Machine sessions, then reconcile current logical gate without replaying edges | Recover per-page behavior, deferred versus ignored operations, MOD interleavings |
| Shift direction | Eligible ascending indices E: CW moves old E[i] to E[(i+1)%N]; CCW reverses; random uses descending Fisher–Yates with PRNG byte modulo remaining count | Trace hardware direction/permutation draw order and phase-offset ownership |
| Run/Stop offset | Prototype offset stays with the physical destination through Shift; stop clears it, run reestablishes it; nonmember All restart waits for next master rising edge | Verify exact v71 timing/ownership and nonmember restart behavior |
| Mutation | Source is clipboard; channel order 0..5, one ratio draw then one phase draw; clamp ratio to -124..124, phase to 0..255; Bank order 0..15; repeated Mutate reuses clipboard with advanced PRNG; fresh seed 1 | Trace boundary normalization, source accumulation, seed and draw order |
| Remote/Mesh | Follow gates remote recall; explicit Multi-Paste copies current editable State to Mesh targets; retain parser acceptance/dispatch traces separately | Execute downstream copy/store/Follow/Mesh routines to establish complete effects |

For Leading taps in this prototype, use the interval between two taps through the accepted-period path; Channel learn uses a separate per-lane history. Timestamp UI events at audio command admission, state this limitation, and test deterministic tap sequences in the core. Host/UI latency is not a firmware debounce measurement.

Same-sample R ordering: (1) sample inputs and capture Leading edge using pre-transition master state; (2) apply ordered admitted commands and Leading candidates; (3) resolve absolute selection; (4) apply State Gate only if no absolute selection changed and routing permits, otherwise Shift; (5) apply MOD logical level/edge under the resulting State/configuration; (6) recompute/queue edits; (7) advance due virtual events and output pipeline; (8) render envelopes then masks; (9) publish visual state if due. Break ties by stable command sequence. Preserve source fixtures for recovered fragments independently of this scheduling choice.

The commit policy uses two stages: guards admit a pending change; the boundary commits an already admitted change. Do not demand a <76 guard pass on the exact boundary itself. Schedule phase-derived next events arithmetically, never by walking every historical cycle. Preserve the reference reload ordering around unchanged timers and test the prototype's explicit transition rule separately.

## 12. Implementation sequence and gates

Each gate requires code, focused verification, and an updated evidence ledger. Visualization starts early and cannot be postponed into an optional final polish task.

1. **P0 — evidence and integration proof.** Inventory sources and current APIs; freeze profile constants/defaults; record unresolved questions; prove save/load/stop/bypass ownership; port reference fixtures; register a null-safe skeleton and anchored layout. Confirm naming remains provisional. Resolve `F4` dispatcher scope and initialize global defaults explicitly rather than guessing from sparse EEPROM bytes.
2. **P1 — deterministic core plus live view.** Exact arithmetic, factory memory, internal/external Leading clock, six timers, pulse/mute policy, sample bridge, and actual six-lane history. Gate: steady-state and recovered timing-fragment tests, sample-rate comparison, no-UI parity, no allocations in process.
3. **P2 — state and Machine workflow.** Selector, hysteresis, edit sessions, phase, dirty/store/recall, State/Bank clipboard, safe serialization. Gate: persistence and simultaneous-event tests; direct controls show pending and committed timing correctly.
4. **P3 — performance transformations.** All MOD modes, routing, transient phase, combined jack reassignment, deterministic mutation. Gate: off-grid Run/Stop/Shift/State traces, destination mute invariants, display source/offset correctness.
5. **P4 — Human and fidelity closure.** Reconstruct Human/commit/source arbitration; implement all resolutions and phase operations; close or explicitly report profile deviations. Gate: instruction-backed or measured transition vectors beyond steady-state arithmetic. A usable approximation finishes prototype scope, not this fidelity gate.
6. **P5 — receiver and Mesh.** Parser, downstream semantic dispatch, Follow, advanced message entry, Mesh view, Multi-Paste, Store/Revert interactions. Gate: parser fixtures plus real semantic effects; dispatch hooks alone are insufficient.
7. **P6 — integration and delivery.** Final branding/slug/layout, full lifecycle/theme QA, portable example patches, documentation, performance measurements, routine suite and authoritative build. Gate: all mandatory features and acceptance coverage accounted for; deviations explicit.

The implementation agent may investigate later gaps earlier when that avoids rework. Do not mark a gate complete because its UI exists or because an isolated model passes.

## 13. Acceptance criteria

All cases below are future requirements. Import local fixture data rather than retyping large tables; tests must compare observable state/edge traces against evidence or the declared profile, not duplicate the implementation algorithm.

| Test ID | Required assertion |
| --- | --- |
| A01 Arithmetic | All supplied ratio/phase vectors and 249 ratio alignment cases; explicit wrapped extreme intermediates; H=8000: r=-124→256000, -8→24000, 0→8000, 1→6400, 8→2666, 124→250 |
| A02 Phase/factory | H=8000,r=-8,p=1→4000, not 12000; all 64 factory States including State 15 phases and State 16 fine divisions |
| A03 Capture | 398 rejected, 399 accepted→H200; H8000 timeout 120000 retained/120001 cleared; equal interval16000 skips correction; interval32000/R4000 yields H16000 and reload12000 low/22000 high |
| A04 Tempo/commit fragments | Synthetic ADC34→400000,35→199032,64→189984; nextH800 boundaries725/875 defer and724/876 pass other guards; remaining1/current333/next777 expires into remaining333/current777 |
| A05 Scheduler | Repeated cycles maintain declared rational relationships with tick truncation/pipeline accounted for; long noninteger runs; near-edge ratio/phase edits; shared-origin and no accumulated sample-rounding drift |
| A06 Output | 50%/10ms within sample quantization; high-rate retrigger policy; mute/edit/unmute retains timing and creates no fresh envelope; Run Stop actually changes timing; every stopped output low |
| A07 Selection | Base4, Gates→5→6, absolute10→10, Gate→11; wrap15→0; bank change; simultaneous CV/Gate priority; noisy boundary CV; page entry/exit; State CV reassignment in Tap-off mode |
| A08 MOD | Every Run/Stop × gate-mode combination; zero/one/multiple Shift-eligible lanes; muted exclusion; off-grid restart; Shift after induced offset; State change/re-entry clears transients; combined gate routing |
| A09 Memory | Edited State survives switching; Store/Recall/Bank/Revert scopes; patch save/reload preserves stored≠editable; no implicit Store; corrupt/truncated/old/future JSON handling; deterministic PRNG round-trip |
| A10 Programming | Ratio unity crossing and boundaries; coarse phase overriding fine once recovered; two taps minimum; 100/50/25, multi-lane captures, slow/fast taps, invalid taps, termination; tap-source arbitration |
| A11 Receiver | `C0 25` requests37; `C1` not a substitute; `C0 F8 16` requests22 without a clock event; no running status; wrong header/partial frame; repeated Mesh pairs; `F4` ranges and actual downstream memory effects |
| A12 Mesh/mutation | Example `F0 00 02 2D 01 07 01 3F 00 07 F7` leaves only63 of those targets; Follow on/off; Multi-Paste without Store; Store/Revert; PRNG state/bytes versus fixtures; delta boundaries and repeat source |
| A13 Ownership | Concurrent UI edits/save/audio; ordered command acceptance and overflow; save with audio stopped/bypassed; load during suspension; snapshot readers/teardown; no data races in exercised stress paths |
| A14 Visualization | Fast pulses visible in history; internal versus actual output; source permutation, induced offset, pending edits, dirty/Mesh cells, held selection; discontinuity markers; no-UI output trace identity |
| A15 Host/graphics | 44.1/48/96/192k sample rates and live changes; initialize/load/bypass/resume; null-module preview; theme, resize/zoom/high DPI; repeated DAW editor close/reopen without stale handles |
| A16 Performance | Multiple instances at worst supported tempo/control activity; bounded queues/history/event service; measured Process/Step/Draw with conditions; display on/off comparison; no audio-thread heap/lock/I/O |

Verify timer helper models in their documented domains and separately test negative/wrapped inputs not covered by historical differential runs. Full clock traces should log timestamp, lane, edge kind, State, routing and transition cause. Include at least the equal-rate phased factory State, fine-divisor drift State, changing external clock, and off-grid Run/Stop followed by Shift.

Run focused native/portable tests during each gate. At integration run `test-fast` and the authoritative native Windows `plugin.dll` build using [windows_build_from_wsl.md](../windows_build_from_wsl.md). Native Rack-linked tests need `RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"` with the documented runtime path ordering. Linux tests under WSL are useful but do not validate the DLL. Run panel splitter/background/anchor checks and inspect rendered/live layouts. Use targeted concurrency sanitizers with the repository's documented environment workaround where applicable. Report pass/fail/not-run honestly.

## 14. Required delivery

Deliver source, registered model/metadata, master and regenerated packaged assets, focused regression tests and evidence fixtures, a user guide explaining State versus Store and all input reassignments, and four self-contained example patches: factory phase relationships; State CV/Gate memory performance; Shift with Run/Stop; Human Programming and external acquisition. Include annotated screenshots showing mute/internal timing, dirty State memory, and transient routing.

The final report identifies implemented requirements, current behavior profile, remaining fidelity gaps, exact verification performed, measured performance, and whether the plugin was built, packaged, or installed. Do not install as an unrequested side effect of specification work. A future implementation task must finish the authorized module work while keeping prototype completeness distinct from evidence of faithful TEMPI behavior.
