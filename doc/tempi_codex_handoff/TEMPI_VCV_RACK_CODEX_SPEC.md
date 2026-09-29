# TEMPI → VCV Rack: Complete Codex Implementation Specification

**Specification:** 1.1.0 (implementation-readiness review)<br>
**Date:** 2026-09-29<br>
**Reference target:** the supplied TEMPI 71 firmware-analysis archive and its behavioral dossier  
**Implementation target:** a native VCV Rack 2 module, integrated into the existing Leviathan plugin checkout when available  
**Working C++/model name:** `Tempi` / `modelTempi`; treat the eventual public-facing product name as a separate branding decision  
**Deliverable:** functioning source code, headless tests, Rack integration, panel controls, persistence, documentation, and reproducible validation—not another design document

---

## Navigation

[1. Instructions to the implementing Codex agent](#1-instructions-to-the-implementing-codex-agent) · [2. Evidence, authority, and compatibility claims](#2-evidence-authority-and-compatibility-claims) · [3. Architecture and ownership](#3-architecture-and-ownership)

[4. Rack interface, controls, and initial state](#4-rack-interface-controls-and-initial-state) · [5. Complete data model](#5-complete-data-model) · [6. Exact arithmetic kernel [F]](#6-exact-arithmetic-kernel-f)

[7. Leading Tempo, timebase, capture, and tempo CV](#7-leading-tempo-timebase-capture-and-tempo-cv) · [8. Shared timing scheduler [P: RationalBoundaryV1]](#8-shared-timing-scheduler-p-rationalboundaryv1) · [9. Output shaping, mute, and event integrity](#9-output-shaping-mute-and-event-integrity)

[10. State memory, factory programs, and transactions](#10-state-memory-factory-programs-and-transactions) · [11. State CV, Gate, Free/Follow, and programming freeze](#11-state-cv-gate-freefollow-and-programming-freeze) · [12. Machine Programming and phase edit policy](#12-machine-programming-and-phase-edit-policy)

[13. Human Programming [M/P: HumanQuantizerV1]](#13-human-programming-mp-humanquantizerv1) · [14. Complete gesture and page state machine](#14-complete-gesture-and-page-state-machine) · [15. MOD: Shift and Run/Stop](#15-mod-shift-and-runstop)

[16. Copy, Paste, Mutation, and PRNG](#16-copy-paste-mutation-and-prng) · [17. Select Bus reception, execution, and Mesh](#17-select-bus-reception-execution-and-mesh) · [18. Deterministic processing order and simultaneous events](#18-deterministic-processing-order-and-simultaneous-events)

[19. Rack lifecycle, threading, undo, and resource handling](#19-rack-lifecycle-threading-undo-and-resource-handling) · [20. UI completeness and diagnostics](#20-ui-completeness-and-diagnostics) · [21. Native patch schema and migration](#21-native-patch-schema-and-migration)

[22. Automated acceptance tests](#22-automated-acceptance-tests) · [23. Implementation phases and completion gates](#23-implementation-phases-and-completion-gates) · [24. Uncertainty register and replacement seams](#24-uncertainty-register-and-replacement-seams)

[25. Source register, provenance, and use of companion files](#25-source-register-provenance-and-use-of-companion-files) · [26. Final implementation guardrails](#26-final-implementation-guardrails)

---

## 1. Instructions to the implementing Codex agent

Implement this specification. Begin by inspecting the actual checkout, its `AGENTS.md` files, build rules, Rack SDK version, model registration, theme system, existing controls, and test conventions. Do not change branches, rename unrelated modules, replace the plugin infrastructure, or assume that a previously discussed repository layout still exists.

The module must be a playable six-channel timing instrument, not six unrelated clock dividers behind a similar panel. Preserve the distinction between canonical timing programs, physical output destinations, temporary performance transformations, editable State memory, and explicitly stored State memory.

The supplied evidence is unusually strong for certain numerical routines and incomplete for some full-device behavior. Implement the recovered routines exactly where specified. For unresolved behavior, implement the **named software policy** given here; do not leave a stub, invent a claim of firmware equivalence, or delay the entire implementation awaiting hardware measurements. Keep these policies isolated so a later measurement can replace one policy without rewriting the module.

Work incrementally, keeping the headless tests and plugin build passing. The final implementation report must identify completed requirements, tests actually executed, remaining defects, and any intentional deviations from this contract. Do not report an unexecuted test as passed.

**Read this as a contract, not a brainstorming brief.** This revision fixes
serialization ownership, resolves tempo/Run and event-order contradictions,
and adds concrete execution rules and smaller implementation steps. It does
not claim additional firmware recovery. The module is not yet implemented;
`TempiPoliciesV1` and schema 1 remain draft production identifiers. The supplied
recovered fixtures and reference models must remain byte-for-byte unchanged.

Begin with this file and [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md).
Implement one plan step at a time; do not ask another model to design missing
behavior or replace the named policies. Each step states its inputs, files,
tests and completion gate. Cosmetic layout choices can follow repository
conventions; musical behavior must follow this contract.

### 1.1 Required functionality

The release described here includes all of the following:

- Six related clocks, integer and fractional multiplication/division, asymmetric phase programming, external/internal Leading Tempo, and per-output clock/10 ms trigger modes.
- Human Programming, coarse/fine Machine Programming, all programming pages, mute, Shift, Normal/All/Alternate Run/Stop, and Momentary/Toggled MOD operation.
- Four Banks of sixteen States; editable versus saved memory; Store, Recall, Revert, Copy, Paste, Mutation, factory programs, State CV, State Gate, and Free/Follow behavior.
- A working receive-side Select Bus parser and command execution path, including Mesh bookkeeping and copy/store distinctions. It must be exercisable without physical hardware.
- A complete Rack adapter, usable panel and accessible alternative controls, deterministic JSON persistence, lifecycle behavior, undo support for discrete edits, and an automated regression suite.

### 1.2 Explicit non-goals

Do not embed or execute the PIC firmware in the shipped module. Do not ship the recovered firmware, update WAV, bootloader fragments, or a synthetic EEPROM image as runtime dependencies. Do not emulate analog buffering, MCU instruction timing, power-bus electrical signaling, or undocumented physical voltage thresholds. Do not add audio synthesis, swing, probability, Euclidean sequencing, a seventh output, polyphonic clock engines, or an invented reset input to the primary module.

A public Select Bus hardware bridge, another module's proprietary expander compatibility, and automatic cross-plugin discovery are not prerequisites for the core release. The parser, semantic executor, explicit injection interface, and any compatible existing local transport integration are prerequisites; a silently disconnected placeholder is not acceptable.

---

## 2. Evidence, authority, and compatibility claims

### 2.1 Evidence labels

Use these labels in code comments, tests, and the implementation report:

| Label | Meaning | What it permits |
|---|---|---|
| **F** | Recovered firmware arithmetic/data or a specifically exercised firmware fragment | An exact claim limited to that routine and tested domain |
| **M** | Documented musical behavior, reconciled with the supplied v71 analysis | A behavioral requirement, not a claim about unseen implementation details |
| **P** | Deliberate Rack/software policy defined in this specification | A complete, testable fallback; never call it recovered behavior |
| **U** | Remaining uncertainty about the original device | A reason to isolate a policy, not a reason to omit the feature |

For F claims, use this order: executable recovered fixtures and directly decoded
bytes; corrected behavioral dossier; dedicated v71 behavior/changelog and
manual sections; older summary prose. For P behavior, this version of the main
specification is authoritative, including its explicit tie-breaking rules;
an older reference model is not an alternate Rack policy. A conflict between
an F fixture and this file must be reported with the exact case, not resolved
by silently changing either. New policy fixtures are labeled P and never
replace the original evidence.

### 2.2 Resolved source conflicts

1. **Factory enable mask:** use `0x3F`, not `0xFF`. The synthetic factory image, extracted tables, and updated dossier agree on `0x3F`. `docs/MEMORY_AND_STATES.md` contains stale `FF` prose. Runtime uses only six mask bits.
2. **Select Bus `F4`:** data `0..63` copies the active program into a target **editable** State. Data `64..127` selects the persistent-store dispatcher path. Do not label every `F4` message a persistent save, and do not restrict the store form to only byte `0x40`.
3. **Clock Edit:** implement the dedicated page behavior: double-press both PGM buttons to enter; Channel 1 controls the combined Leading Tap/output-mode choice. Do not reproduce contradictory old quick-reference entries.
4. **Run/Stop terminology:** implement v71 Momentary/Toggled behavior. Do not add obsolete Jumbled/Unjumbled modes from earlier documentation.
5. **Human resolution:** the Program Edit choices are Channel 1 = 100%, Channel 2 = 50%, Channel 3 = 25%. Do not adopt the inconsistent tip that assigns 100% to Channel 3.
6. **Raw phase versus a phase knob:** the recovered phase byte does not mean a universal fraction of the destination's period. Divider phase is referenced to Leading Tempo.

### 2.3 What has actually been validated

The supplied package contains a PIC18-focused harness. During preparation of this specification, `tools/validate_and_extract.py` was rerun successfully: 1,743 ratio cases, 42 six-lane phase cases, 249 alignment cases, 64 factory States, 64 EEPROM addressing cases, 64 GPIO patterns, 64 PRNG cases, 4,096 ADC hysteresis cases, seven LCM cases, and seven Select Bus scenarios passed. These are offline, shared-harness comparisons—not independent physical-device measurements or validation of a complete clone.

The package also contains a successful **previously generated** `analysis/timing_validation.json` reporting 13,409 timing cases. A complete fresh run of its timing validator was not completed during preparation of this document. Retain that distinction in any provenance report.

The companion handoff includes source models and numerical fixtures, not an implemented Rack module. The implementing agent must port and exercise them against its C++ implementation.

---

## 3. Architecture and ownership

### 3.1 Layering

Use four layers with one-way dependencies:

```text
Rack widget / context menus / keyboard
              │ bounded commands; immutable snapshots
              ▼
Rack Module adapter ──────► headless TempiCore
                               ├── exact reference math
                               ├── leading-clock service
                               ├── shared timing scheduler
                               ├── program/memory transactions
                               ├── selector + MOD performance state
                               ├── gesture and Human policies
                               └── Select Bus parser/executor
              ▲
      six voltages + light/display snapshot
```

The headless core must compile without Rack, NanoVG, GLFW, JSON libraries, or a running GUI. Parsing/serializing Rack JSON belongs in the adapter or a serialization translation unit, not in the sample loop. The core uses fixed-size data and bounded work.

### 3.2 Proposed file boundaries

Adapt names to the existing repository, but preserve these responsibilities:

```text
src/Tempi.cpp                       Rack lifecycle, ports, adapter, model registration
src/TempiWidget.cpp                 Panel, context menus, keyboard, display
src/tempi/TempiCore.hpp/.cpp         Engine orchestration and typed commands
src/tempi/TempiTypes.hpp             Fixed-size model, enums, snapshots
src/tempi/TempiMath.hpp/.cpp         Exact wrap/truncate, ratios, phase, ADC, PRNG
src/tempi/TempiLeading.hpp/.cpp      Capture, loss, ADC tempo arbitration
src/tempi/TempiScheduler.hpp/.cpp    Shared master, lane edge scheduling, commits
src/tempi/TempiMemory.hpp/.cpp       Working/saved States, factory table, transactions
src/tempi/TempiPerformance.hpp/.cpp  Mute, permutation, Run/Stop, output shaping
src/tempi/TempiSelector.hpp/.cpp     State CV/Gate/Follow and programming freeze
src/tempi/TempiGestures.hpp/.cpp     Physical gesture finite-state machine
src/tempi/TempiHuman.hpp/.cpp        HumanQuantizerV1
src/tempi/TempiSelectBus.hpp/.cpp    Receive parser and typed semantic commands
src/tempi/TempiSerialization.cpp    Rack JSON translation and schema migration
res/Tempi.svg                       Panel, plus existing theme variant conventions
res/tempi/                          Only required original assets
manual/Tempi.md                     User-facing operating guide
tests/tempi/                        Headless tests and golden fixtures
```

Do not create a single multi-thousand-line module translation unit. Avoid generic frameworks that make this one module more difficult to audit. Reuse existing plugin command queues, theme controls, and test helpers if they satisfy the contracts below.

### 3.3 Ownership rules

The audio/engine thread owns all mutable musical state while the module runs. Widget code must not directly edit `workingStates`, permutations, clock fields, or globals. It sends typed, bounded commands. DSP publishes a coherent display snapshot; GUI rendering reads only that snapshot.

Do **not** assume serialization excludes audio processing. In the inspected
Rack source, `Engine::moduleToJson()` and engine processing both use shared
engine locks; `dataToJson()` can overlap `process()`. Engine-mediated
`moduleFromJson`, Reset and Randomize take the exclusive lock. Verify these
entry paths in the target SDK/source; arbitrary widget calls do not inherit
that exclusion. Serialization reads a published persistence snapshot, never
live `Memory` or `SelectorRuntime` fields. A double-buffer without reader
ownership and a seqlock over ordinary struct fields are both insufficient.

Use a proven SPSC mailbox for one UI producer and the engine consumer, plus a separately owned bus input path if necessary. For snapshots, use a safe triple-buffer/reader-ownership exchange, atomically published immutable storage, or an existing proven plugin helper. Do not use a C++ data-racing seqlock over ordinary struct fields.

Use **two separate** `snapshot_transport::SpscLatestSnapshot<T>` exchanges
from `src/SpscLatestSnapshot.hpp`: one small numeric display snapshot and one
complete value-only persistence snapshot. Widget `step()` is the sole display
consumer and shares a local copy with its child views. Serialize consumers are
serialized by an adapter-side mutex used **only outside DSP**; they copy the
persistence exchange before constructing JSON. No widget/inspector may also
consume that exchange. Engine publication never takes that mutex.

Publish persistence after every frame that changes a persisted field, including
selection, H, PRNG, clipboards, Mesh and retained ADC state, not only on Store.
Publish initialized data in the constructor and after exclusive load/reset.
Saving reflects the latest completed engine transaction; commands not yet
accepted are pending and must not appear as acknowledged edits. Saving does
not drain the engine's command queue from a second consumer. With the engine
stopped, save the last accepted state; show unaccepted UI edits as pending.

### 3.4 Bounded command interface

Expose commands as a tagged fixed-size value type. Required semantic commands include:

```text
SetRatio(destination, code)          SetPhase(destination, byte)
SetEnabled(destination, bool)        SetModMember(destination, bool)
HumanTap(destination, capturedTap)   TapLeading(capturedTap)
MachineCoarse(destination, sign, n)  MachineFine(destination, signedSteps)
PhaseCoarse(destination, signedCount) PhaseFine(destination, signedSteps)
SelectState(globalIndex, origin)     SetBank(bank)
StoreState / RecallState / RecallBank / StoreAll / Revert
CopyState / PasteState / MutateState / CopyBank / PasteBank / MutateBank
SetGlobal(field, value)              SetInternalTempoBpm(value)
DefaultCurrentState                 FactoryReset
MultiPaste(meshMask)                 BakeShift
ReceiveSelectBusBytes(payload)       SetFollow(bool)
ButtonEdge(button, pressed, timestamp) / ReleaseAllUiButtons
EnterPage(page) / ExitPage            TempoBusInterval(interval, capturedState)
BeginUiTransaction(id) / EndUiTransaction(id)
ApplyHistoryRecord(record)            (exclusive restore path, not UI queue)
```

Use enum fields, not arbitrary strings in the engine. Validate every command before use. Maximum UI commands admitted per sample: 64; queue capacity at least 256 commands. Reject overflow visibly through a diagnostic counter. Do not silently overwrite an unconsumed Store/Recall command. Coalescing is allowed only for an explicitly replaceable command, such as the latest numeric inspector value for the same field.

Timestamps are engine-time timestamps, not wall clock. A UI event cannot be applied in the past: use its capture timestamp for tap measurement but apply its musical transaction no earlier than the next engine processing opportunity. Future scheduled commands must be bounded and sorted; the first implementation may reject timestamps beyond the current frame rather than introducing an unbounded event scheduler.

---

### 3.5 Concrete producer topology and core boundary [P]

Implement the first release with one UI command producer (Rack's UI thread),
one engine consumer, and no worker. Panel parameters are sampled by the
adapter, not also echoed into the UI command queue. Keyboard held buttons use
a separate UI-held bitmask command. The effective button level is the OR of
panel and keyboard ownership; one source releasing must not release another
source's hold. `ReleaseAllUiButtons` clears keyboard/UI ownership and pending
classifications, not a physically/automation-held Rack parameter.

Hex injection is a separate UI-to-engine packet queue: four fixed packet slots,
each holding `length<=256` and 256 bytes. Publish a whole slot atomically only
after validation. Consume at most 64 bytes/frame, retaining a packet cursor.
That supplies 1024 bytes of bounded capacity and cannot partially enqueue a
packet. This UI injection queue is the initial Select Bus transport. Do not
let a second thread become a producer; a future transport gets its own queue
and an explicit merge order. A rejected whole packet has not lost any accepted
bytes, so it **does not** reset a partially received parser message. An actual
transport-loss marker resets the parser immediately before the next accepted
packet, never in the middle of already accepted bytes.

Core entry points must provide these responsibilities (names may match these):

```text
initializeDefaults()                    fixed value initialization, no Rack calls
processFrame(FrameInput, CommandSpan)    one audio sample; fixed-size FrameOutput
applyCommand(ValidatedCommand)           the single musical mutation path
capturePersistent()                     fixed value object, engine/exclusive owner only
restorePersistent(ValidatedPersistent)   engine/exclusive owner only; deterministic restart
displaySnapshot()                       numeric fixed value object
```

`FrameInput` includes sample rate, four sanitized channel-0 voltages and
connection bits, nine Rack parameter values, bypass flag and keyboard-held
mask. `CommandSpan` is an adapter-owned fixed array of at most 64 commands.
`FrameOutput` has six voltages, lights/status and diagnostics. Commands carry
`origin`, monotonic producer sequence, module-generation and optional history
transaction ID. Reject stale generations after Reset/load; no raw widget or
Module pointers occur in command payloads. Large validated imports/history
records use a fixed owned slot ID, not a borrowed pointer to UI stack memory.

Destination indices are `0..5`, bank `0..3`, slot `0..15`, global State `0..63`
everywhere in C++ and JSON. Add one only for user-facing labels. Invalid
command indices/enums reject the whole command and consume no PRNG draws.
UI event timestamps supplied to the core are snapshots of engine time. A
button first observed by `process()` captures that exact frame's master
snapshot; the GUI must not attach a decimated display-phase estimate as an
accurate tap time. Delayed classification retains the engine-captured snapshot.

---

## 4. Rack interface, controls, and initial state

### 4.1 Stable port and parameter IDs

Use stable contiguous IDs and never reorder them after release. Match these IDs unless a preexisting released `Tempi` model requires a migration:

| Kind | ID | Name / behavior |
|---|---:|---|
| Param | 0 | `STATE_PARAM`, normalized `0..1`, default `0` |
| Param | 1..6 | `CHANNEL_1_PARAM` through `CHANNEL_6_PARAM`, momentary buttons |
| Param | 7 | `PGM_A_PARAM`, momentary |
| Param | 8 | `PGM_B_PARAM`, momentary |
| Input | 0 | `LEADING_INPUT`, monophonic rising-edge clock |
| Input | 1 | `STATE_CV_INPUT`, monophonic State/tempo CV |
| Input | 2 | `STATE_GATE_INPUT`, monophonic rising-edge State advance or Shift |
| Input | 3 | `MOD_INPUT`, monophonic gate/trigger |
| Output | 0..5 | `CLOCK_1_OUTPUT` through `CLOCK_6_OUTPUT`, each monophonic |

Provide six RGB channel lights, PGM A/B indicators, State/Bank indication, Leading Tempo indication, and MOD indication. Their internal light enumeration may use the existing plugin's color helpers; document it and keep it stable. LEDs are not additional parameters and must not own musical state.

Use `configButton` for momentary buttons; name every port and parameter. Reading a polyphonic input uses channel 0 only. Outputs always report one channel. There is no hidden dependency on whether an output cable is connected: all engines continue to run.

### 4.2 Voltage rules [P, Rack]

Clock/trigger output LOW is `0 V`; HIGH is `10 V`. Input Schmitt thresholds are `0.1 V` low and `1.0 V` high. A channel-0 input containing NaN/Inf is treated as `0 V` and increments a rate-limited diagnostic; never let nonfinite values enter a comparator or JSON state.

The State CV meaningful range is `0..5 V`. Clamp outside that range before conversion. Leading and Gate inputs are edge controls, not continuously measured tempo CV inputs. Use a Schmitt implementation consistent with Rack's API and explicitly initialize its connection state as described below.

TEMPI's documented 10 ms trigger duration is intentional even though Rack's ordinary trigger convention is shorter. Do not change it to 1 ms merely because a Rack utility defaults to that duration.

### 4.3 State combo knob [M/P]

This knob is an absolute control when no State CV cable is connected and an **attenuator**, not an additive offset, when patched:

```text
x = clamp(STATE_PARAM, 0, 1)
effectiveVolts = STATE_CV connected ? x * clamp(Vin, 0, 5) : 5*x
adc = floor((effectiveVolts / 5) * 1023 + 0.5)
```

When Leading Tap is enabled, this feeds State selection. When Leading Tap is disabled, it feeds the recovered tempo-control curve. Do not implement `knob + CV`, bipolar modulation, or direct `floor(16*x)` State selection.

### 4.4 Default configuration [F/M/P]

Instantiate the recovered factory States into both saved and working memory. Select Bank 1, State 1. Reset transient permutation to identity, all run offsets to zero, all clipboards invalid, the Select Bus parser empty, Mesh clear, and the PRNG to unsigned seed `1`.

Software global defaults are:

```text
Human resolution       100%
Shift                   Off
Run/Stop                Off
MOD gate behavior       Momentary
Follow                  false
Leading Tap             enabled
Output modes 1..6       Clock50
Stored/current H        8000 virtual ticks
Virtual tick frequency  32000 Hz
Output voltage          10 V high
```

The virtual timebase and resulting 120 BPM default are explicit software choices, not a measured PIC oscillator frequency. The factory program bytes are firmware-derived; not all global power-up defaults have been recovered with equal confidence.

On initialization or patch load, output one engine frame of LOW, then start the master at a rising origin. Do not deserialize half-held buttons or leave a trigger high indefinitely.

### 4.5 Connection changes

A newly connected high Leading or State Gate cable initializes the Schmitt detector to its current level and does not synthesize an edge; wait for a subsequent low-to-high transition. A newly connected MOD cable establishes the momentary level immediately; in Toggled mode it initializes the physical detector without flipping the stored logical latch. Unplugging Leading begins holdover, not a phase reset. Unplugging MOD disables its run constraint and returns all destinations to running on the canonical grid without creating an artificial off-grid restart event.

These connection behaviors are `ConnectionPolicyV1`, not recovered cable-detection circuitry.

---

## 5. Complete data model

### 5.1 Persistent program types

```cpp
struct Program {
    std::array<std::int8_t, 6> ratio{};   // legal live edit range: [-124, 124]
    std::array<std::uint8_t, 6> phase{};  // retain imported raw bytes [0, 255]
    std::uint8_t enableMask = 0x3f;
    std::uint8_t modMask = 0;
};

enum class HumanResolution : std::uint8_t { Full100, Half50, Quarter25 };
enum class ShiftMode : std::uint8_t { Off, Clockwise, CounterClockwise, Random };
enum class RunMode : std::uint8_t { Off, Normal, All, Alternate };
enum class ModGateMode : std::uint8_t { Momentary, Toggled };
enum class OutputMode : std::uint8_t { Clock50, Trigger10ms };

struct GlobalSettings {
    HumanResolution human = HumanResolution::Full100;
    ShiftMode shift = ShiftMode::Off;
    RunMode run = RunMode::Off;
    ModGateMode modGate = ModGateMode::Momentary;
    bool follow = false;
    bool leadingTap = true;
    std::array<OutputMode, 6> outputMode{};
};

struct Memory {
    std::array<Program, 64> saved;
    std::array<Program, 64> working;
    GlobalSettings savedGlobals;
    GlobalSettings workingGlobals;
    std::uint32_t savedLeadingH = 8000;
    std::bitset<64> dirty;
};
```

Equality must compare fields, not struct padding. A program uses fourteen logical bytes, but native `sizeof(Program)` need not be fourteen. Never serialize raw C++ object memory.

### 5.2 Runtime types

Keep at least these distinct objects:

- `LeadingRuntime`: requested/current reload, countdown, master level, elapsed measurement, capture mailbox, acceptance/loss flags, master phase history, and active source indication.
- `CanonicalLane[6]`: committed ratio/phase, pending version, nominal half-period, reduced frequency, canonical phase/edge state, and saturation status.
- `DestinationRuntime[6]`: canonical source index, run permission, temporary Run displacement/anchor, trigger end timestamp, output-mode state, and light pulse state.
- `SelectorRuntime`: bank, absolute base, gate offset, active State, last quantized ADC value, pending selection, bus anchor, and programming-freeze bookkeeping.
- `GestureRuntime`: held-button mask, captured press order/timestamps, page, pending clicks, double-click/chord state, coarse counts, and consumed flags.
- `HumanRuntime[6]`: bounded tap history and master-phase snapshots.
- `BusRuntime`: parser state, bounded input queue, last valid requested State, Mesh mask, transport sequence, and error counters.
- `ClipboardRuntime`: separate value-owned State and sixteen-State Bank snapshots, each with a validity flag.
- PRNG state: one explicit unsigned 64-bit integer shared by Mutation and Random Shift; no GUI-owned randomness.

### 5.3 Canonical versus destination identity

A State stores six canonical programs. A Shift changes `sourceForDestination[6]`; it does not rewrite the State. Mute, MOD membership, output jack, output mode, and destination-specific Run behavior remain attached to physical destinations.

A ratio/phase edit addressed to a physical destination resolves its currently mapped canonical source and edits that source. The inspector must show both the physical destination and its current source. Because the mapping is a permutation, an ordinary edit changes exactly one canonical source.

An explicit **Bake Shift into working State** command snapshots the six currently mapped canonical ratio/phase pairs into the six physical slots, returns the permutation to identity, and marks the State dirty. It does not bake runtime Run offsets, alter masks, or Store anything. This explicit command is the software equivalent of capturing a useful arrangement; do not hide an unverified “auto-bake” action in MOD membership changes.

---

## 6. Exact arithmetic kernel [F]

### 6.1 Integer semantics

The reference routines depend on signed 32-bit wrap followed by division truncated toward zero. Ordinary C++ signed overflow is undefined behavior; do not reproduce it by multiplying `int32_t` values and hoping the compiler wraps.

A safe pattern, with intermediates constrained as below, is:

```cpp
inline std::int64_t signed32(std::uint64_t bits) noexcept {
    const std::uint64_t u = bits & 0xffffffffULL;
    return (u < 0x80000000ULL)
        ? static_cast<std::int64_t>(u)
        : static_cast<std::int64_t>(u) - 0x100000000LL;
}

inline std::int64_t wrapSigned32(std::int64_t value) noexcept {
    return signed32(static_cast<std::uint64_t>(value));
}

inline std::uint32_t ratioHalfPeriod(int r, std::uint32_t masterH) noexcept {
    std::int64_t h = wrapSigned32(masterH);
    if (r > 0)
        h = wrapSigned32(h * 4) / (r + 4);
    else if (r < 0)
        h = wrapSigned32(h * (4 - r)) / 4;
    if (h < 200) return 200;
    if (h > 0x00ffffff) return 0x00ffffff;
    return static_cast<std::uint32_t>(h);
}

inline std::int64_t phaseOffset(int r, std::uint8_t p,
                                std::uint32_t masterH) noexcept {
    const std::int64_t base = (r < 0)
        ? wrapSigned32(masterH)
        : static_cast<std::int64_t>(ratioHalfPeriod(r, masterH));
    return wrapSigned32(base * static_cast<std::int64_t>(p) * 2) / 4;
}
```

C++ integer division of these signed values truncates toward zero. The displayed multiplications fit `int64_t` over the required input domains. Assertions/validation must keep the musical `r` domain in `[-124,124]`; fixture-specific wider-domain experiments belong in separate tests, not unchecked runtime commands.

### 6.2 Ratio encoding

For stored signed code `r`:

```text
r > 0: frequency ratio = (r + 4) / 4
r = 0: frequency ratio = 1
r < 0: frequency ratio = 4 / (4 - r)
```

Reduce to coprime positive integers `N/D`, where channel frequency is `N/D` times Leading frequency. Examples:

| Code | Meaning | H=8000 recovered channel half-period |
|---:|---|---:|
| -124 | ÷32 | 256000 |
| -12 | ÷4 | 32000 |
| -8 | ÷3 | 24000 |
| -5 | ÷2.25 | 18000 |
| -4 | ÷2 | 16000 |
| -2 | ÷1.5 | 12000 |
| -1 | ÷1.25 | 10000 |
| 0 | unity | 8000 |
| 1 | ×1.25 | 6400 |
| 2 | ×1.5 | 5333 |
| 4 | ×2 | 4000 |
| 8 | ×3 | 2666 |
| 12 | ×4 | 2000 |
| 124 | ×32 | 250 |

Coarse multiplier `n=1..32` is `r=4*(n-1)`. Coarse divider is `r=-4*(n-1)`. Fine Faster increments the signed code by one; Fine Slower decrements it by one; clamp at the endpoints.

Do not interpret a fine step as an equal semitone, equal BPM increment, or fixed percentage of current frequency. Crossing zero follows the piecewise code mapping above.

### 6.3 Alignment denominator

The firmware-derived alignment factor is the denominator `D` of the reduced frequency ratio:

```text
r < 0: D = (4-r) / gcd(4-r, 4)
r >=0: D = 4 / gcd(r+4, 4)
```

The shared alignment length is `lcm(D0..D5)` in master cycles. Compute it safely with `uint64_t` using divide-before-multiply. The recovered helper's narrow representation does not establish every overflow behavior; do not deliberately overflow a 16-bit Rack scheduler.

Do not allocate a sequence with one element per global alignment tick and do not hold an edit until the entire six-lane LCM elapses. LCM is an invariant/diagnostic and a synchronization aid, not an excuse for unbounded memory or unusably delayed controls.

### 6.4 Phase encoding

```text
base = r < 0 ? masterH : ratioHalfPeriod(r, masterH)
offsetTicks = truncTowardZero(signed32(base * phaseByte * 2) / 4)
```

For a divider, a one-code phase step is one quarter of a **master** cycle. For unity/multipliers, it is one quarter of the recovered **channel** cycle. At `H=8000, r=-8, p=1`, the offset is `4000`, not `12000`.

Preserve raw phase `0..255` when loading a native patch or imported program. A large raw byte can generate a wrapped, negative signed offset; normalize it only when positioning it on the runtime timeline, using Euclidean modulo. Do not first truncate it to `0..3`, and do not replace the exact phase helper with a floating phase percentage.

### 6.5 ISR reload fragment

Keep this as an exact reference helper, independently tested:

```text
countdown = signed32(countdown - 1)
if countdown <= 0:
    level ^= 1
    countdown = old currentReload
    currentReload = nextReload
```

For `countdown=1, current=333, next=777`, the result is a toggled level, `countdown=333`, and `current=777`. Loading `777` into countdown is wrong.

The original output GPIO pipeline also adds one ISR step between preparing and driving an output. The Rack policy does **not** add that uncalibrated hardware pipeline to audio output. Keep the fact in the reference tests and provenance, rather than claiming the software edge timestamps are instruction-exact hardware timestamps.

### 6.6 Recovered commit guard

Define an exact predicate for the recovered necessary guard:

```text
defer if signed masterRemaining < 76
      or signed laneRemaining < 76
      or nextH-75 <= signed laneRemaining <= nextH+75
```

For `nextH=800`, remaining `725` and `875` defer; `724` and `876` pass that particular band check. Other countdown checks still apply. Passing this predicate is **not** proof that the complete original timing commit would occur; the historical decision logic remains unresolved.

---

## 7. Leading Tempo, timebase, capture, and tempo CV

### 7.1 Virtual tick scale [P: VirtualClock32kV1]

Use a fixed `32000 Hz` virtual tick domain. All recovered `H`, phase offsets, capture intervals, clamps, and guards use this domain. One full Leading cycle is nominally `2*H` ticks:

```text
secondsPerMasterCycle = 2*H / 32000
BPM = 60*32000 / (2*H)
```

This maps default `H=8000` to 120 BPM. It does not claim that the physical module runs its timer at 32 kHz. The range `H=200..0xFFFFFF` follows the recovered clamp, so its physical-tempo interpretation is necessarily a software policy.

Maintain a monotonic 64-bit virtual tick count and a fractional sample-to-tick remainder. Do not reset that remainder on normal tempo edits or lose it at sample-rate changes. A rational accumulator for integer sample rates is preferred: add 32000 per sample and consume `sampleRate` per virtual tick. For noninteger rates, use a bounded double accumulator with compensated conversion; never convert an entire long-running absolute sample count through a low-precision float.

Virtual deadlines are emitted at the first Rack sample not earlier than the deadline. At supported rates, temporal quantization is bounded by one virtual tick plus one engine sample. Multiple virtual ticks in one sample must be processed in chronological order. Supported validation rates: 22050, 44100, 48000, 96000, and 192000 Hz. At rates below 22050 Hz, remain stable and report reduced temporal resolution; do not allocate or run an unbounded catch-up loop.

### 7.2 Leading capture state [F]

Keep separate fields for requested half-period `H`, current reload, countdown, master level, measurement elapsed, measuring flag, accepted flag, and a capture mailbox containing the new interval plus the master countdown/level at that edge.

The first eligible rising edge starts an interval measurement. The next rising edge supplies a full-cycle interval. Capture timestamps are in virtual ticks; retain the associated master remainder and level from the same logical instant. Zero capture is an empty mailbox, not a malformed measured interval.

Process loss before consuming the mailbox:

```text
if elapsed >= 0xFFFFFF
   or (accepted and elapsed > 15*H):
    measuring = 0
    elapsed = 0
    accepted = 0
```

The second comparison is strict. With `H=8000`, accepted capture remains valid at `elapsed=120000` and expires at `120001`. `15*H` is 7.5 complete master cycles, not fifteen complete cycles.

For a nonzero signed capture interval:

```text
if interval < 399:
    measuring = 0
    elapsed = 0
    consume the capture
    do not automatically clear an already-set accepted flag
    do not apply a new H
else:
    accepted = 1
    Hnew = clamp(floor(interval / 2), 200, 0xFFFFFF)
```

A negative signed interval is rejected. A zero interval is left as the empty condition. After an accepted capture, clear its saved remainder. Match the supplied helper's edge cases instead of “cleaning up” them into a different filter.

### 7.3 Period replacement and transient correction [F]

If `Hnew == requestedH`, skip both period replacement **and** phase correction. Do not retrigger/reset the master merely because another equal-period edge arrived.

If changed:

```text
requestedH = Hnew
currentReload = Hnew
countdown = min(countdown, Hnew)
timingDirty = true

forward = signed32(Hnew - capturedRemaining)
while forward > floor(Hnew/2):
    forward = truncTowardZero(forward / 2)

backward = capturedRemaining
while backward > floor(Hnew/2):
    backward = truncTowardZero(backward / 2)

currentReload = wrap32(capturedLevel
    ? Hnew + forward
    : Hnew - backward)
```

Do not replace this with exponential smoothing, a moving-average BPM estimator, absolute-value folding, or a reset-on-every-input-edge clock. A negative `forward` stays negative unless the specified comparison enters the loop.

Example: captured interval `32000`, `R=4000`, new `H=16000` gives current reload `12000` for level 0 and `22000` for level 1. Equal interval `16000` when H is already 8000 makes neither correction.

The exact helper returns its wrapped field even for pathological states. The live scheduler uses a separately named safety sanitizer, `LiveReloadSafetyV1`, restricting an active segment to `1..0x01FFFFFF` ticks and incrementing a diagnostic if it changes that field. Never modify the exact helper to hide a failing fixture.

### 7.3.1 Capture producer and arbitration completion [P around F helper]

The live physical capture producer mirrors the recovered sequence: on each
eligible rising edge, latch `elapsed` (zero if measurement is not active),
pre-event remaining countdown and level; then zero elapsed and set measuring.
Only virtual ticks with measuring set increment elapsed. Service loss before
consuming the new mailbox; an invalid <399 capture clears measuring, so the
next edge starts a fresh pair rather than reusing that rejected interval.
Do not implement a permanently rolling last-edge delta that bypasses this.

Loss runs once per frame before candidate admission; capture state is updated
even when a lower-priority candidate loses arbitration. Build candidates
without mutating the master, choose the highest-priority **valid completed**
candidate, then apply its correction once. An acquiring/invalid physical edge
does not suppress a valid completed tap. A valid equal-H physical candidate
wins arbitration but applies no replacement or correction. This prevents a
losing tap from changing the fields against which the physical helper compares.

For a direct BPM change: reject nonfinite/nonpositive BPM; compute
`H=clamp(floor(960000/BPM + 0.5),200,0xFFFFFF)`. On a changed H, assign
requested/current H and shorten remaining countdown as in §7.3, but apply no
capture correction. Internal BPM candidates rank below tempo CV in §7.6;
otherwise the last valid internal edit in UI FIFO order wins. For a completed
Leading Tap pair, use its own interval and the second tap's engine-captured
countdown/level without resetting the physical input's measurement history.
An abstract Tempo Bus interval uses the same value contract in headless tests;
no external Tempo Bus transport or new panel input is required for this release.

### 7.4 Clock loss and holdover [M/P]

Accepted capture loss clears acquisition flags; it does not silence the module. Continue at the last accepted requested H with ongoing phase. Do not restore saved tempo just because a cable is unplugged or a timeout occurs.

Saved Leading tempo is restored by explicit Recall/Revert operations where specified, factory reset, or patch-load policy—not by ordinary clock dropout. The display distinguishes acquiring, externally accepted, holdover, internal, tap, and tempo-CV modes.

### 7.5 Tempo-control curve [F plus P calibration]

Use the exact curve with these **synthetic software calibration thresholds**:

```text
T[j] = 32 + 64*j, j=0..15
```

These are not measured factory calibration values. The recovered table of candidate full periods is:

```text
200000, 180000, 130000, 80000,
70000, 40000, 12000, 10000,
8500, 6000, 5000, 3500,
2500, 1750, 1000, 250
```

Let previous accepted ADC value be `P`, input be `x`, and current requested H be `H`:

1. `margin = min(10, floor(P/50)+2)`.
2. For `x=65535` or `max(0,P-margin) <= x <= P+margin`, return full period `2*H`, keep `P`, and report no movement.
3. Otherwise set retained ADC to `x` and movement true.
4. If `x <= T[0]+margin`, candidate period is **`2*200000`**, not 200000.
5. Else if `x > T[15]`, candidate is 250.
6. Else if `x` exactly equals a threshold, use its table entry.
7. Otherwise find `T[j] < x < T[j+1]`, compute integer `slope = trunc((V[j]-V[j+1])/(T[j+1]-T[j]))`, then `candidate = V[j+1] + slope*(T[j+1]-x)`.

Truncate the slope **before** multiplying. At admission, clear the candidate's low bit, divide by two, and clamp to `200..0xFFFFFF`. Therefore candidate 250 becomes H=200, not H=125.

With previous ADC 0 and H=8000: ADC 34 gives candidate 400000/H200000; 35 gives 199032/H99516; 64 gives 189984/H94992; 96 gives 180000/H90000; 992 and 993 give 250/H200.

### 7.6 Source arbitration [P: LeadingSourcesV1]

There is one Leading clock. Select Bus State messages are not tempo messages; ignored MIDI `F8` bytes do not become clock pulses.

At a processing instant, the priority below increases from first to last. Compute candidates first, then apply only the winning valid candidate (§7.3.1):

1. Admit a moved tempo-CV candidate only when Leading Tap is disabled and no external interval measurement is active.
2. Admit an explicit completed Leading Tap pair, if enabled.
3. Admit an eligible abstract Tempo Bus interval, if Follow is enabled and no physical Leading cable is connected.
4. Admit a physical Leading capture. It has priority over a simultaneous tap, bus interval, or tempo-CV candidate.

An ADC helper may retain its moved ADC value even when its candidate is not admitted. Continue updating capture/loss state while a programming page is open. An `externally accepted` display status certifies an accepted interval, not proven zero phase error against every external edge. In particular, this reconstruction's equal-H rule does not force a one-time reset just to make a familiar clock-lock animation look correct. The complete original acquisition/history behavior remains a refinement seam. A physical cable that is connected but has lost clock remains in holdover and blocks Tempo Bus takeover in this version; an explicit source setting can be added only as a versioned extension.

A completed Channel 1 Leading Tap pair may temporarily set the tempo while an external source is present, but the next accepted physical interval wins. No indefinite hidden tap-override latch exists.

The inspector's explicit internal BPM edit directly changes the requested H using nearest integer conversion, then the same replacement/commit infrastructure. It does not pretend to be an external capture and does not fabricate a captured remainder. It wins over no source indefinitely; later external captures can replace it.

---

## 8. Shared timing scheduler [P: RationalBoundaryV1]

### 8.1 Why this is a named policy

The ratio/phase arithmetic and some timer guards are recovered; the complete historical timing-commit routine is not. A constant truncated half-period and indefinitely exact rational alignment cannot both be true for every ratio. For example, repeating 2666 ticks forever for ×3 at H=8000 drifts against a 16000-tick master cycle.

`RationalBoundaryV1` therefore uses the recovered nominal period and phase arithmetic, a shared master timeline, and bounded fractional edge placement. Its fractional interval correction is **not claimed to reproduce the original firmware's exact correction sequence**. This is the default production engine. Do not offer an “exact firmware” switch that merely changes labels.

### 8.2 Master timeline

Represent master position as a 64-bit whole-cycle counter plus a bounded fractional cycle, rather than a growing `float`. The master counter's rising origins are integer cycles; falling origins are half cycles. The master ISR-reload ordering in §6.5 governs the duration of its half-cycle segments.

Between segment boundaries, master position progresses monotonically from its current position to the next half-cycle boundary. When the capture service shortens the current countdown, preserve the already-reached master position and redistribute the remaining fraction over the new remaining countdown; do not jump backward. When a half-cycle expires, commit the exact half-cycle boundary, load the old current reload, promote requested H, and begin the next segment.

An efficient implementation may use Q32 fractional cycles plus integer remainders. At the maximum allowed H, one Q32 unit is less than one virtual tick; bound intermediate products and rebase per segment. The integer master-cycle counter must never be converted wholesale to floating point. If using floating fractions, derive them from bounded integer segment state rather than accumulating unrestricted phase error.

### 8.3 Unsaturated canonical lane lattice

For frequency `N/D`, recovered signed phase offset `O`, and current master H, define the canonical phase displacement in master-cycle units as:

```text
beta = EuclideanModulo(O / (2*H), D/N)
channelPosition = (masterPosition - beta) * N/D
```

A canonical rising edge occurs at every integer channel position. Falling edges occur at half integers. Ratio/phase/H changes rebuild the relevant phase displacement through the commit policy, not an independent free-running oscillator reset.

At constant H and an integer master origin, equivalent half-edge deadlines are:

```text
edge(j) = origin + O + j * H * D / N
```

Schedule at the first virtual tick not earlier than a rational deadline. The rounding remainder is carried; do not round a period once and repeat it forever. At H=8000 and ×3 with phase zero, one master cycle's half-edge deadlines are:

```text
0, 2667, 5334, 8000, 10667, 13334, 16000
```

The recovered nominal half-period remains 2666 for the inspector/reference math. The above alternating 2667/2666 interval sequence is the software alignment policy.

Every unsaturated lane returns to its phase-relative canonical alignment after D master cycles; all unsaturated lanes realign over their LCM. Accumulated drift must not grow with test duration.

Use exact rational comparisons or fixed-point math with a bounded error of at most one virtual tick. If a ratio threshold lies exactly at a representable boundary, consistent integer comparison must determine the edge, not a floating epsilon that changes with runtime length.

### 8.4 Saturation

Evaluate the recovered period clamp. A lane is saturated when its unclamped rational half-period would lie outside `200..0xFFFFFF`; mark it visibly in the inspector. It cannot simultaneously honor the requested ratio and the recovered timing bound.

For a saturated lane use `ClampedLaneV1`: a shared-tick-domain lattice with full period `2*clampedH`, the recovered phase offset normalized by that period, and a common module origin. This exception does not use a drifting floating oscillator. It uses integer tick deadlines and does not claim nominal-ratio realignment.

A Run start records both a master-phase restart anchor and a virtual-tick restart anchor. Unsaturated mapped sources use the master displacement; saturated mapped sources use the tick anchor. This makes transitions into/out of saturation and Shift deterministic. A ratio/phase edit clears the affected destination's temporary displacement **when that timing edit commits**. An H-only tempo change preserves both restart representations, including transitions into/out of saturation; see §15.5.

### 8.5 Edge enumeration and masks

The scheduler reports internal rising/falling events separately from the currently high level. Merely selecting a different source, unmuting, or changing an output mode is not an internal oscillator edge.

Enumerate every crossing within a virtual step in chronological order. Normal nominal rates are bounded, but transient source corrections can compress a segment. Cap emergency crossings at 128 per virtual tick, advance to the mathematically correct final phase, and increment an overrun diagnostic if exceeded. No silent infinite loop or allocation is permitted.

Do not process LEDs at a rate that determines clock edges. LEDs may be decimated; clocks, Schmitt detectors, button edge capture, and trigger duration accounting may not be reduced to a 30/60 Hz display rate.

### 8.6 Pending edits and safe commits [P: NextSafeEdgeV1]

Each canonical lane has committed timing and at most one pending replacement. Repeated edits replace the pending target, not append an unbounded queue. Program memory changes immediately, so the UI/dirty state reflects the edit; the inspector can show `pending` until timing commits.

For a lane edit:

1. Compute the proposed nominal H, phase, reduced ratio, and saturation status.
2. Evaluate the recovered necessary guard using the master remaining time and old lane's next half-edge remaining time.
3. Wait while the guard rejects, but no longer than one **current complete master period** from the first pending edit. Replacing the pending target does not postpone this deadline indefinitely.
4. If the guard has not admitted by that deadline, admit under a documented software fallback and increment `commitGuardFallbacks`.
5. If the old lane is HIGH, allow its currently active high interval to end naturally. Do not insert a new rise before that old fall. A long divided pulse may therefore finish after guard admission.
6. Commit the new program while LOW. The next new rising deadline must be strictly after the commit instant and no earlier than one virtual tick after the retained fall.
7. A new phase that would already be high at commit is not allowed to manufacture a mid-high trigger; wait for its next rising boundary.

A phase/ratio edit clears temporary Run displacement for the destination mapped to that canonical source. Mute never blocks an edit or its internal commit. Other channels must not reset or lose phase because one channel was edited.

State activation is a six-lane transaction with a different rule: cancel old output pulses, clear transient mapping/displacement, switch all canonical programs together, keep the master continuous, and schedule each new lane's next canonical rising edge strictly after activation. This can intentionally truncate an outgoing State's high gate. It must not partially activate one State over six unrelated GUI frames.

Global H changes update the master immediately through §7 and queue any needed lane phase/saturation rebuilds coherently. They do not clear Run displacement merely because tempo changed; a stored channel timing edit or State activation does.

### 8.6.1 Implementation details that are not optional

- A pending lane target is a value `{ratio, phase, HForPhase, N, D, nominalH,
  saturated, version}`. `HForPhase` is the requested H captured when building
  that target. Until commit, use the old committed beta/saturation values;
  never recompute beta from new H on every sample behind the commit guard.
- An H change advances the common master with the new segment rules immediately
  and replaces all affected pending targets in one batch. Each lane still
  commits at its own safe LOW point; “coherently” means one H generation, not
  an undocumented reset or simultaneous truncation of all six gates.
- Freeze the guard fallback deadline at `firstRequestTick + 2*HAtFirstRequest`.
  New H/target values do not extend it. On admission while HIGH, remember the
  required old fall and stop reevaluating the guard. Do not repeatedly defer
  again when LOW is finally reached. The guard bound is not a bound on a long
  divided HIGH finishing naturally.
- Protect the old internal HIGH of both the canonical lane and its currently
  mapped destination if Run displacement makes their levels differ. Keep their
  old schedule until those falls; only then clear displacement and commit.
  Store the old schedule as bounded value state, not a second persistent bank.
- LOW-at-commit holds LOW until the **strictly next** new rising boundary.
  No falling-edge search may turn the lane HIGH early. At constant H, rising
  times are `origin + O + k*(2*H*D/N)`, integer k. Normalize O modulo the full
  period (not the half period), preserving rise/fall parity even for negative O.
  Compute the first k whose deadline is strictly greater than commit time,
  then ceil that rational deadline to a virtual tick.
- Example: H=8000, old unity HIGH, edit to x2/p0 at tick1000, guard admitted
  at tick3000. Retain the old fall at8000; commit there; next rise is16000,
  **not**8000. With new p2 (O4000), next rise is12000 instead.
- Equality is field equality. Reissuing the already pending/committed ratio
  and phase is a no-op: do not clear Run displacement, restart deadlines or
  create history. Explicit Recall/Default are activation operations even if
  their resulting data happen to match; ordinary identical self-copy is not.

Use integer quotient/remainder for repeated constant-H deadlines. For variable
H, keep whole master cycles separate from a bounded fractional segment. Before
multiplying by N, reduce whole cycles modulo D (and handle signed Run-relative
cycle differences explicitly). Never multiply a 64-bit lifetime counter by
N, by Q32, or by H. Use checked wider intermediates or factor cancellation
for phase/comparator products; the C++11 Windows build must not depend on a
compiler's signed-overflow behavior or an unavailable platform integer type.

### 8.7 Reference-versus-runtime separation

The exact recovered six-lane ISR helper, commit predicate, and arithmetic helpers remain regression targets even where the production rational scheduler has an explicitly different edge-placement policy. Test both contracts. A numerical helper passing does not certify the whole live scheduler as firmware-identical.

---

## 9. Output shaping, mute, and event integrity

### 9.1 Output pipeline

For each physical destination, perform:

```text
canonical source + temporary Run displacement
                  ↓
internal timing level and genuine rising events
                  ↓
Run permission and per-destination trigger shaping
                  ↓
output-enable/mute mask
                  ↓
0 V / 10 V physical output
```

Keep canonical clocks running while muted or stopped. A stopped destination does not stop its source program for another destination that might receive it through a later Shift.

### 9.2 Clock mode

Clock50 exposes the internal square level while enabled and running. Unmuting during an existing internal HIGH makes the jack HIGH immediately, because mute is an output mask. That masked output transition is not classified as a new internal rising event.

A Shift can similarly make a Clock50 output immediately reflect the newly selected source's current level. Do not generate a trigger from that routing discontinuity.

### 9.3 Trigger mode

A genuine eligible internal rising event sets `triggerEnd = max(triggerEnd, eventTime + 10 ms)`. Physical trigger output is HIGH until the end timestamp, subject to mute and Run permission. Overlapping triggers retrigger/extend the high interval; do not shorten 10 ms to half a fast channel period.

Entering mute or stopped Run state clears the destination's audible trigger pulse. While muted, internal edge/LED tracking continues, but no audible trigger is armed for resurrection on unmute. Unmuting or entering Trigger mode waits for a subsequent genuine rising event.

An explicit off-grid Run start is a genuine synthetic timing restart event by contract and may trigger immediately. Merely reloading a State under an already-HIGH MOD level is not such a restart.

### 9.4 Light behavior

Channel timing flashes continue while muted, corroborating the underlying timing engine. Use separate visual channels for timing, mute, MOD membership, selection/page feedback, and stopped status. A single overwritten brightness field must not obscure all of them.

The software color design may differ from the original hardware. Prefer a neutral/white timing flash, an obvious muted marker, a distinct MOD marker, and text/shape support for color-blind accessibility. No light envelope or animation may consume PRNG draws or change timing.


## 10. State memory, factory programs, and transactions

### 10.1 Working versus saved memory [F/M]

All sixty-four States exist in both `working` and `saved` memory. Editing changes the working copy. Selecting another State neither Stores nor discards the previous working copy. Selecting that State again returns to its working copy.

Dirty status is a comparison against the saved copy. Reversing an edit back to saved content must clear dirty; a sticky “was ever edited” bit is insufficient. Track global settings dirty separately from State dirty. Track current Leading tempo versus saved Leading tempo separately too, without flashing a State dirty indicator on every external clock correction.

An active program is read from `working[activeIndex]`; a committed lane may temporarily lag behind it only while a timing update is pending. The pending/committed distinction must not turn into a third silently persistent program bank.

### 10.2 Transaction table

| Operation | Working memory | Saved memory | Globals/tempo | Runtime |
|---|---|---|---|---|
| Select State | No edits | No edits | Unchanged | Activate target; clear Shift and Run offsets |
| Store State | Unchanged | Copy active working State | Unchanged | No phase reset |
| Recall State | Restore active State from saved | Unchanged | Unchanged | Reactivate restored State |
| Recall Bank | Restore all 16 working States in current Bank | Unchanged | Unchanged | Reactivate active State once |
| Store All | Unchanged | Copy all 64 working States | Save current globals and requested H | No phase reset |
| Revert | Restore all working States | Unchanged | Restore saved globals and saved H | Clear transients; reactivate current index |
| Default Current | Unity ratios, phase zero, mask `0x3F`, MOD mask 0 | Unchanged | Unchanged | Reactivate default working State |
| Factory Reset | Restore all factory States | Restore all factory States | Restore software defaults in both layers | Fresh initialization |
| Rack patch save | No edits | No edits | No edits | Serialization only; **not Store** |

Copying all saved States during Revert is equivalent to reloading only dirty States and simpler to verify. A bank transaction is atomic from the musical engine's perspective. It must not expose partially copied banks to a bus command, State Gate edge, or JSON snapshot.

Store operations in Rack are immediate memory transactions, not simulated EEPROM writes. Give brief UI confirmation, but do not block the engine or sleep to imitate physical write latency.

### 10.3 Factory table [F]

Bank 1 is the following literal table. All enable masks are decimal 63; all MOD masks are zero. Unless listed otherwise, every phase is zero.

| Slot (1-based) | Six signed ratio codes | Six phase codes |
|---:|---|---|
| 1 | `0, 0, 0, 0, 0, 0` | `0,0,0,0,0,0` |
| 2 | `0, -4, -12, -28, -60, -124` | `0,0,0,0,0,0` |
| 3 | `-4, -8, -16, -24, -40, -48` | `0,0,0,0,0,0` |
| 4 | `0, -4, -8, -12, -16, -20` | `0,0,0,0,0,0` |
| 5 | `-4, -12, -20, -28, -36, -44` | `0,0,0,0,0,0` |
| 6 | `-8, -16, -24, -32, -40, -48` | `0,0,0,0,0,0` |
| 7 | `-4, -8, -16, -28, -48, -80` | `0,0,0,0,0,0` |
| 8 | `0, 4, 12, 28, 60, 124` | `0,0,0,0,0,0` |
| 9 | `4, 8, 16, 24, 40, 48` | `0,0,0,0,0,0` |
| 10 | `0, 4, 8, 12, 16, 20` | `0,0,0,0,0,0` |
| 11 | `4, 12, 20, 28, 36, 44` | `0,0,0,0,0,0` |
| 12 | `8, 16, 24, 32, 40, 48` | `0,0,0,0,0,0` |
| 13 | `4, 8, 16, 28, 48, 80` | `0,0,0,0,0,0` |
| 14 | `4, 8, 12, -4, -8, -12` | `0,0,0,0,0,0` |
| 15 | `-2, -2, -2, -2, -2, -2` | `0,1,2,3,4,5` |
| 16 | `-4, -5, -6, -7, -8, -9` | `0,0,0,0,0,0` |

Banks 2–4 are forty-eight unity States, phase zero, enable `0x3F`, MOD 0. Validate all sixty-four against `factory_states.csv`, not just the first few musical descriptions.

### 10.4 Optional raw EEPROM import/export

A raw importer is useful but not required for the first release. If implemented, accept exactly 1024 bytes and decode State `s`, channel `c` using:

```text
ratio:  0x000 + 64*c + s, signed byte
phase:  0x180 + 64*c + s, unsigned byte
enable: 0x300 + s
MOD:    0x340 + s
```

Those 896 State bytes are understood. The remaining global/calibration bytes are only partially understood. Do not invent a complete EEPROM format or advertise an exported file as safe to flash to hardware.

Reject out-of-range raw ratio bytes rather than silently reinterpret a signed byte as an unsigned multiplier. Preserve an original imported image separately only if needed for a deliberate round-trip utility. Runtime masks use low six bits. Native patch loading may clamp malformed values as described in §21; a hardware-image import must instead report an explicit invalid-data error and remain transactional.

---

## 11. State CV, Gate, Free/Follow, and programming freeze

### 11.1 Quantizer [F plus P calibration]

Use thresholds `T[j]=32+64*j` and the recovered hysteresis, not a generic quantized knob:

```text
if adc == 65535: return previous
j = first index for which adc < T[j], or 16 if none
if j == 16: return 15
if j == 0: return 0
margin = floor((T[j] - T[j-1]) / 3)
if previous == j and adc <= T[j-1] + margin: return j-1
if previous == j-1 and adc <= T[j] - margin: return j-1
return j
```

Keep `previous` in `0..15`. For the normal Rack input path, ADC is `0..1023`; `65535` is reserved for the reference/helper sentinel. The quantizer returns a Bank-local slot.

### 11.2 Absolute base plus Gate offset

In Free mode:

```text
slot = (absoluteBase + gateOffset) & 15
state = 16*bank + slot
```

A real change in the quantized State CV/knob base replaces `absoluteBase` and sets `gateOffset=0`. A State Gate rising edge increments `gateOffset` modulo 16, provided that jack is not currently assigned to Shift.

The knob/CV does not rewrite selection every sample when its quantized value is unchanged. This is essential: an unchanging knob must not immediately undo every Gate step or bus selection.

At the same timestamp, a genuine absolute-base change wins over a Gate edge: apply the new base, zero the offset, and consume the Gate edge without stepping. State Gate does not cross Banks. An explicit Bank selection changes the Bank and clears the Gate offset, retaining the current effective base unless a simultaneous valid absolute selection supplies another slot.

### 11.3 Free/Follow [M/P: SelectorPriorityV1]

Receive and remember valid bus State requests in both Free and Follow. Free does not activate them. When Follow becomes true, apply the last valid remembered bus State, if any, as a new bank/base anchor and clear Gate offset.

In Follow mode a new valid bus State sets Bank and base slot, with Gate offset zero. A **subsequent changed** local absolute CV/knob value overrides its slot within the current Bank, and local Gate edges walk from the effective base. At an identical timestamp, local absolute change outranks bus selection; bus selection outranks Gate stepping.

The displayed Bank/State is always the actual active global index, not merely the last knob quantizer output. A bus-selected slot must remain selected while the physical knob is unchanged. Likewise, entering Follow must not discard a remembered State solely because the knob still has its startup value.

Mesh reception and last bus State storage continue while Free. Do not infer that Mesh filters State selection; the recovered selector evidence does not establish that rule.

### 11.4 Freeze matrix

State selection is frozen during explicit programming interaction. Use this matrix:

| Context | State activation | Human input | Ratio/phase gestures | MOD Shift pulses | Run/Stop input |
|---|---|---|---|---|---|
| Normal idle | Enabled | Enabled | Enabled | Enabled | Enabled |
| Active Human window | Frozen | Enabled | Cancels affected Human history | Dropped | Held/reconciled on exit |
| Active coarse/fine gesture | Frozen | Consumed as gesture | Enabled | Dropped | Held/reconciled on exit |
| Mute page | Frozen | Not a bare channel action | Available via modifiers/direct editor | Enabled | Enabled |
| MOD-membership page | Frozen | Not a bare channel action | Available via modifiers/direct editor | Enabled | Enabled |
| Phase page | Frozen | Disabled | Phase gestures enabled | Dropped | Held/reconciled on exit |
| Program/Clock/Bank Edit | Frozen | Disabled | Page operations only | Dropped | Held/reconciled on exit |
| State Edit modal | Frozen | Disabled | Page operations only | Dropped | Held/reconciled on exit |

Continue sampling every input Schmitt detector while frozen. Keep the most recent valid pending absolute selection request, with the normal tie-breaking rules; do not queue Gate edges. On exit, apply the latest pending absolute selection once and reconcile current momentary MOD level once. Toggled MOD edges that occurred while that input was held are consumed/dropped, not replayed as a burst.

An explicit program-edit command, such as Paste into the selected State or Recall State, is not forbidden by the State-selection freeze; it is the purpose of that page. A Bank Edit `SetBank` action is likewise an explicit page operation. These transaction commands bypass automatic selector freeze in a controlled way and clear stale pending selections if their destination would conflict.

### 11.4.1 Selection records and reconciliation [P]

Maintain `observedQuantizedSlot` separately from `absoluteBase`: a bus/direct
selection changes the latter, never fabricates a knob movement. Initialize
the observation from the actual first-frame combo voltage while retaining the
restored base. On a control scan, compare the newly quantized slot against that
observation, update it even while frozen, and propose an absolute request only
on change. Hold one pending request `{bank, slot, timestamp, source}`; newer
timestamp wins, and local absolute beats Follow for an equal timestamp.

While frozen, update observation/lastBusState normally, replace that one
pending request, and drop Gate increments. Leaving freeze applies it once;
if its target is already active it just clears the offset as requested and
does not reactivate timing. Freeze predicates are recomputed after gesture/page
actions and before automatic selection. Thus a Human press coincident with
State Gate freezes selection soon enough to consume that Gate without stepping.

Direct UI `SelectState` and `SetBank` are explicit transactions, bypass automatic
freeze, clear pending selection/Human history, and exit an active held timing
gesture without generating its deferred Human action. `SetBank(b)` retains
the **effective slot** `(absoluteBase+gateOffset)&15` as its new base and clears
offset; it must not unexpectedly jump back to the pre-Gate base. Bus State
requests are automatic and obey Follow/freeze; a bus copy/store is an explicit
memory transaction. An unchanged automatic target is not a fresh activation.

Define membership/global/mode reconciliation as level-only: stops clear pulses
and displacement; newly permitted destinations hold LOW until the next
canonical rise, without an immediate synthetic trigger. Shift routing itself
retains §9.2's immediate square-level behavior. This same rule applies on
freeze exit, MOD reconnect/unplug, Run mode/member changes and state activation.

### 11.5 Leading Tap disabled

When Leading Tap is disabled, the combo knob/CV controls tempo and must not produce absolute State changes. Hold the last State base. State Gate, explicit selection, Bank Edit, and Follow remain functional. On re-enabling Leading Tap, evaluate the current knob/CV as a new absolute State request once, using ordinary freeze/priority rules.

---

## 12. Machine Programming and phase edit policy

### 12.1 Coarse ratio gesture [M/F]

Holding PGM A before a channel press starts a divisor count for that channel. Holding PGM B first starts a multiplier count. The first press in a new gesture sets `n=1` (unity); subsequent presses increment to 32. Maintain a separate count for each channel touched while that PGM remains held.

```text
PGM A: r = -4*(n-1)
PGM B: r =  4*(n-1)
```

While a coarse gesture is active, pressing the opposite PGM doubles the current counts of the channels already participating, saturating at 32. A later channel press increments its current count by one. The opposite button is consumed as doubling and must not enter Phase or State Edit.

A new gesture begins when the primary PGM is released and later pressed again. Do not initialize its count from the existing ratio: coarse entry is an explicit replacement.

### 12.2 Fine ratio gesture [M/F]

Holding one or more channels **before** pressing PGM A/B selects those destinations for Fine edits. A means one slower code step; B means one faster code step. Holding a button alone must not auto-repeat unless a separate, documented accessibility option is explicitly enabled; default is one edit per press.

A consumed channel press is not also a Human tap. Clearing Human history for edited destinations prevents a stale tap interval from unexpectedly overwriting Machine Programming.

### 12.3 Phase edit policy [P: PhaseCodeEditV1]

The stored-byte-to-offset math is recovered; every physical coarse/fine byte transformation is not. Implement this explicit mapping:

```text
W(r) = r < 0 ? 4-r : 4
Fine step: p = EuclideanModulo(p + signedSteps, W(r))
```

Here `W` is the canonical phase-code cycle for the nominal ratio. For a divider it spans one divided period; for unity/multiplication it spans four channel-quarter units. An imported noncanonical byte remains unchanged until an actual phase edit occurs. Editing then canonicalizes it through this modulo.

For coarse Phase, the primary PGM begins a new coarse gesture and resets its entered phase count to zero, thereby overriding previous Fine phase. PGM A means earlier, PGM B later. With signed count `c`:

```text
C(r) = r < 0 ? 4 : r+4
p = EuclideanModulo(c * C(r), W(r))
```

Each channel press changes `c` by the selected direction. This moves in master-cycle terms; an integer multiplier can consequently show no audible coarse phase change. Keep the count bounded by reducing modulo the number of distinct reachable phase positions. Do not overflow an 8-bit phase counter before applying Euclidean modulo.

The reachable-count modulus is `W/gcd(C,W)`. The opposite PGM during an
established Phase coarse gesture doubles each participating signed count,
then reduces and recomputes p; it is consumed exactly like ratio doubling.
Resetting the gesture's count on modifier press does not edit a program until
its channel participates.

On the Phase page, a channel-first gesture followed by PGM A/B performs the Fine phase operation instead of changing ratio. Ratio never changes as a side effect of a phase edit. Exiting Phase clears its transient coarse counts.

### 12.4 Direct editor

Provide exact numeric entry for signed ratio code and raw phase byte, alongside a formatted musical ratio and explanatory phase units. Raw phase entry is intentionally different from a Fine step: it accepts `0..255` without canonicalizing. Numeric edits use the same typed commands, dirty tracking, and safe commits as panel gestures.

Show `÷2.25`, `×1.5`, `unity`, etc., not a misleading linear BPM knob. Also show recovered nominal half-period, effective period/saturation, current mapping, and any pending timing commit in an advanced inspector.

---

## 13. Human Programming [M/P: HumanQuantizerV1]

### 13.1 Fidelity boundary

The instrument must support rhythmic tapping rather than merely a tap-tempo convenience button. However, the exact quantizer at the identified firmware routine is not fully recovered. Implement the following deterministic policy and identify it as `HumanQuantizerV1` in diagnostics and the patch policy version.

The interface between tap collection and quantization must be replaceable:

```text
quantize(tapHistory, masterHistory, resolution, currentProgram)
    -> optional { ratioCode, phaseByte, confidence/status }
```

No inference service, machine learning model, network call, or non-deterministic process is needed.

### 13.2 Tap capture

Timestamp a channel's rising button press immediately. A gesture classifier may later discover that it was part of Machine Programming; in that case consume/discard it as Human input. Do not measure the interval between release events or between delayed UI frame callbacks.

For an unmodified short channel action in Normal mode, submit the captured press timestamp. Keep four taps, giving up to three complete intervals. Capture the bounded master-cycle/fraction snapshot with each accepted press; this does not require a sample-by-sample history buffer covering long slow tap intervals. Ignore duplicate/bounce intervals shorter than 5 ms. Do not manufacture an interval on the first tap. Histories are per destination and reset on State activation, ratio/phase Machine edit, or explicit cancellation.

Keep two separate lifetimes:

- At each accepted tap set history expiry to that tap time plus
  `max(2 seconds, 4*ratioHalfPeriod(-124,H_at_tap)/32000 seconds)`.
  A later tempo change does not silently extend that existing deadline.
  Expire when `now > expiry`, not at equality. A tap arriving after expiry
  becomes a new first tap. This allows slow divisions to be learned.
- The **active programming window** freezes the State selector while a relevant button is held and for 750 ms after the last eligible Human press. Long inter-tap gaps do not freeze automatic State changes indefinitely. A State change clears the history.

That 750 ms window is `HumanActivityWindowV1`, not a recovered hardware timeout. Directly exposing an explicit “arm Human programming” action is allowed, but it must display its armed/frozen status and provide Cancel.

History eligibility uses the captured press timestamp, not delayed release.
While a potential Human press awaits classification, retain that destination's
prior history and its original expiry. On release, compare the captured press
against that expiry; a long hold cannot retroactively invalidate an interval
that was eligible when pressed. A consumed Machine press discards that pending
Human capture instead.

### 13.3 Ratio quantization

For each complete interval, measure elapsed **master cycles** from timestamped master history, not simply seconds divided by the present BPM. This remains meaningful when tempo changed between taps. Choose the median of the last up to three valid interval lengths; for two intervals choose their arithmetic mean. Bounded rational/fixed-point arithmetic must make tie handling deterministic.

Let the selected interval in master cycles be `d`. Enumerate candidates:

```text
100%: all r in [-124,124] divisible by 4
 50%: all r in [-124,124] divisible by 2
 25%: all integer r in [-124,124]
```

For each candidate's reduced frequency N/D, compare absolute period error `abs(d - D/N)`. Choose the least error. Break exact ties by smaller `abs(r)`, then smaller signed `r`. Values outside the candidate range clamp to the nearest attainable candidate. Compare cross-multiplied bounded integers or an explicitly specified rational comparator; do not introduce platform-dependent random tie breaking.

This is a period-nearest quantizer, not a claim that the original used that exact distance metric. Keep saturation visible when the selected program's recovered half-period clamp changes its realizable rate.

### 13.4 Phase quantization

After choosing r, choose a phase code that best aligns a canonical rising edge with the most recent tap on the master timeline. Use the exact recovered phase-offset helper for every candidate, normalize to the candidate's period, and minimize circular timing distance to that tap.

Candidate phase codes are:

| Resolution | Divider (`r<0`) | Unity/multiplier |
|---|---|---|
| 100% | `0,4,8,... < W(r)` | `0` |
| 50% | `0,2,4,... < W(r)` | `0,2` |
| 25% | Every integer `0..W(r)-1` | `0,1,2,3` |

Break equal phase-distance ties with the smaller unsigned code. Use the same constant-H local mapping as the scheduler's new-program commit calculation. Commit the ratio/phase pair as one transaction after the second valid tap and refine it with later taps.

For saturation, use the chosen candidate's clamped tick lattice and the last
tap's captured relative virtual tick for circular distance; ratio selection
still uses elapsed master cycles. For unsaturated phase, use the most recent
tap's captured master position and H at classification/commit-request time.
This is the same H recorded in the pending target; do not consult a future H
when evaluating those candidates.

The policy's displayed confidence is informational: first tap = waiting; two taps = learned; later taps = refined. It must not randomly discard a valid pair or silently refuse fractional ratios in 25% mode.

### 13.5 Leading Tap on Channel 1

When Leading Tap is enabled, an unmodified Channel 1 Human tap belongs to Leading Tempo rather than channel ratio learning. Two valid taps supply a full-period interval in virtual ticks to the Leading service. They do **not** reset Channel 1's stored ratio to unity or overwrite its phase.

Coarse/Fine Machine gestures on Channel 1 remain available. When Leading Tap is disabled, Channel 1 participates in Human ratio/phase learning like the other five channels, and the combo knob/CV controls Leading Tempo.

---

## 14. Complete gesture and page state machine

### 14.1 Accessibility is part of implementation

Do not rely solely on simultaneous mouse presses on two small buttons. Every operation below must also be reachable through a context menu, inspector, or focus-scoped keyboard interface. Direct actions and physical gestures must call the same command layer.

Use PGM A/B and channels 1–6 as the panel's primary vocabulary. Preserve their relationships without copying Make Noise logos, product artwork, or a misleading manufacturer identity. A Leviathan-themed panel is appropriate.

### 14.2 Gesture thresholds [P: GesturePolicyV1]

Use engine-time thresholds:

```text
PGM chord window      40 ms
PGM double-press      250 ms
PGM long hold         350 ms
Human button debounce 5 ms
```

A PGM single-click action is deferred until its double-click window expires. A consumed modifier/chord gesture cancels its pending single action. A channel's Human timestamp remains its original press time even if classification completes later.

Classify press order before page shortcuts:

1. Channel held before PGM → Fine ratio/phase.
2. One PGM held before channel → Coarse ratio/phase.
3. Opposite PGM during an established Coarse gesture → double its coarse count.
4. Both PGM held with no established edit → State Edit modal after 350 ms, or immediately when a channel command is pressed while both are held.
5. Otherwise, completed PGM click/chord sequences select pages below.

Once a sequence is consumed, its releases do not emit additional page changes or Human taps. Releasing one component of a chord does not retroactively turn it into a single click. Focus loss emits `ReleaseAllUiButtons`, cancels pending clicks, and leaves outputs/program memory intact.

### 14.2.1 Gesture classifier completion [P]

Use press timestamps for the chord window (inclusive <=40 ms); measure hold
from the later press of the both-held pair. A click requires release strictly
before 350 ms with no consumed channel/modifier action. A single PGM held
350 ms without an edit produces no page click on release. Both-held at350 ms
enters State Edit once. A both-held channel action enters State Edit immediately
and executes that channel command once; channel release cannot repeat it.

Double-click interval is from the first sequence's release to the second
sequence's first press, inclusive <=250 ms. The second completed short release
emits the double action immediately; cancel the single action. A both-click
finishes when both PGM are released. If neither double nor hold occurs, emit
the deferred single action at release+250 ms. Input events at a deadline are
classified before its timeout, so a press exactly at250 ms counts as double.
An opposite PGM arriving outside40 ms is not a short both-click, but both-held
350 ms still enters State Edit unless a Fine/Coarse gesture already owns it.

Same-frame ties use channel edges before PGM edges; simultaneous PGM A+B with
a fresh channel edge selects State Edit, not Fine. A previously held channel
continues to give Fine priority. Page-local channel operations in State, Bank,
Program and Clock Edit outrank Fine/Coarse; modifiers cannot accidentally edit
ratios on those pages. Mute/MOD modifier gestures consume the bare toggle.

A potential Human press freezes selection immediately and stores an engine
timestamp/master snapshot, but commits its tap only on unconsumed release.
A later modifier/focus loss discards it before any Human edit occurs. There
is no maximum Human hold: a long bare channel hold becomes one tap on release,
dated at its original press; debounce compares captured press times. Once a
press is consumed by Machine programming, its Human active window is canceled
for that destination. Timeout/history arithmetic uses elapsed engine time,
not UI frame arrival or operating-system key repeat.

Context-menu page entry is explicit and does not simulate fake held buttons.
State Edit entered this way remains until Done/Escape; the “release both” exit
only applies to a State Edit entered through a physical/keyboard chord.

### 14.3 Page entry and exit

| Gesture from Normal | Destination |
|---|---|
| Single PGM A | Mute page |
| Single PGM B | MOD membership page |
| Short simultaneous PGM A+B | Phase page |
| Double PGM A | Bank Edit |
| Double PGM B | Program Edit |
| Double simultaneous PGM A+B | Clock Edit |
| Hold both PGM | State Edit modal |

From Mute, single A exits; from MOD, single B exits; from Phase, short both exits. From Bank/Program/Clock Edit, the corresponding single entry control exits; a dedicated Done command and Escape always exit to Normal. State Edit ends when both PGM are released, returning to Normal. These exit rules deliberately resolve inconsistent manual quick-reference wording and must be shown in the help overlay.

No page-entry gesture may also execute its page's Channel 1 command because of a stale held channel from another gesture. Page changes clear pending channel actions after releasing their UI ownership.

### 14.4 Channel actions by page

| Page | Ch 1 | Ch 2 | Ch 3 | Ch 4 | Ch 5 | Ch 6 |
|---|---|---|---|---|---|---|
| Normal | Leading Tap or Human | Human | Human | Human | Human | Human |
| Mute | Toggle enable 1 | Toggle enable 2 | Toggle enable 3 | Toggle enable 4 | Toggle enable 5 | Toggle enable 6 |
| MOD | Toggle member 1 | Toggle member 2 | Toggle member 3 | Toggle member 4 | Toggle member 5 | Toggle member 6 |
| Phase | Select/hold for phase gestures on corresponding destination; unmodified taps alone do not change timing | Same | Same | Same | Same | Same |
| State Edit | Recall State | Store State | Recall Bank | Copy State | Paste State | Mutate State |
| Bank Edit | Cycle Bank | Store All | Toggle Free/Follow | Copy Bank | Paste Bank | Mutate Bank |
| Program Edit | Human 100% | Human 50% | Human 25% | Cycle Shift mode | Cycle Run mode | Toggle Momentary/Toggled |
| Clock Edit | Cycle combined choice below | Toggle output 2 mode | Toggle output 3 mode | Toggle output 4 mode | Toggle output 5 mode | Toggle output 6 mode |

Program Shift cycle is `Off → Clockwise → CounterClockwise → Random → Off`. Run cycle is `Off → Normal → All → Alternate → Off`.

Clock Edit Channel 1 cycles:

```text
Leading Tap enabled + Clock50
Leading Tap enabled + Trigger10ms
Leading Tap disabled + Clock50
Leading Tap disabled + Trigger10ms
(back to first)
```

That cycle changes two global fields, not the channel's ratio or phase. Selecting other channels changes only their output mode.

### 14.5 Context menu / inspector inventory

Provide direct access to exact ratio/phase, mute/MOD membership, output mode, Human resolution, Leading Tap, internal tempo, Shift mode, Run mode, MOD gate mode, all State/Bank operations, Free/Follow, current State selection, factory reset, Bake Shift, Mesh/MultiPaste, and diagnostics.

The advanced inspector must expose State and Bank numbering, dirty status, active source mapping, runtime Run status/offset, nominal ratio, phase interpretation, saturation, timing policy version, bus reception status, and pending commit indicator. A simple scrollable overlay or existing plugin settings widget is preferable to dozens of tiny permanent knobs.

Use a confirmation UI for destructive Factory Reset. Recall/Revert should have undo history rather than a blocking modal on every performance action. Copy/Paste into empty clipboard is a visible no-op. No menu item is allowed to exist as a nonfunctional stub.

### 14.6 Keyboard and panel layout

Focus-scoped keyboard mapping: `1..6` are channels, `A/B` are PGM, Escape cancels/exits. Do not intercept these while a text field is being edited or when the module does not have explicit keyboard focus. Allow held keys to implement the same order-sensitive gestures; ignore operating-system repeat for edge-only controls.

Suggested Rack layout: **12 HP**, 60.96 mm wide by 128.5 mm high, with a three-column/two-row channel-button array and corresponding six outputs. The hardware's narrower panel width is not a required constraint for a usable virtual implementation.

A starting geometry, subject to actual component fit checks:

```text
Title band:                 y 5..12 mm
State knob / bank display:  center x 30.48, y 23 mm
PGM buttons:                x 13 and 47.96, y 23 mm
Channel buttons 1..3:       x 12,30.48,48.96; y 43 mm
Channel buttons 4..6:       x 12,30.48,48.96; y 61 mm
Outputs 1..3:               same x; y 82 mm
Outputs 4..6:               same x; y 99 mm
Four inputs:               x 8,23,38,53; y 117 mm
```

Labels must not overlap control hitboxes or cables. Use the plugin's existing light/dark-theme conventions. Static artwork may be cached; avoid a custom GL renderer, off-thread Rack widget access, or a high-cost animated background for a clock module.

---

## 15. MOD: Shift and Run/Stop

### 15.1 Eligibility and jack assignment

A State's MOD mask determines membership. For Shift, eligible destinations are `modMask & enableMask & 0x3F`: muted destinations are excluded. For Run/Stop, membership is the MOD mask itself; mute remains a separate output mask.

Routing of the two control inputs is:

| Shift | Run/Stop | MOD input | State Gate input |
|---|---|---|---|
| Off | Off | No performance action | Advance State |
| On | Off | Shift on rising edge | Advance State |
| Off | On | Run/Stop control | Advance State |
| On | On | Run/Stop control | Shift on rising edge; **no State advance** |

Run/Stop is inactive when the MOD jack is unpatched. Shift needs a real eligible rising edge. Switching modes while a cable is high must not manufacture a Shift edge or toggle the logical latch.

### 15.2 Shift permutation [M/P: ShiftPermutationV1]

Store a destination-to-source permutation. A Shift permutes its current source assignments among eligible destinations and leaves ineligible assignments untouched. Zero or one eligible destination is a no-op and consumes zero random draws.

Define the panel's physical clockwise ring in one-based destination numbers as:

```text
1 → 2 → 3 → 6 → 5 → 4 → 1
```

Filter that ring to the eligible destinations. For Clockwise, each eligible destination receives the preceding eligible destination's source. CounterClockwise uses the succeeding source. This geometric direction convention is an explicit software decision; the exact physical-to-rotation correspondence is not established solely by the arithmetic evidence.

With every destination eligible and identity mapping, one Clockwise Shift produces:

```text
destination: 1 2 3 4 5 6
source:      4 1 2 5 6 3
```

For Random, collect eligible destinations in that same ring order, copy their existing source values, and run reverse Fisher–Yates: for `i=k-1` down to 1, draw one recovered PRNG byte, set `j=byte%(i+1)`, swap, then assign the shuffled values back in ring order. Exactly `k-1` draws. This shuffle/draw order is a specified policy, not a recovered exact permutation routine.

A State activation resets the mapping to identity. Editing membership or mute does not automatically reset or bake a permutation. An edit to a mapped destination follows §5.3.

### 15.3 Logical MOD gate

Momentary uses the physical Schmitt level (no extra millisecond debounce). Toggled flips a persistent runtime logical level on each eligible MOD rising edge while Run is enabled, MOD is patched, and Run input is not frozen. Falling edges do not flip it. Switching Momentary/Toggled samples/reconciles the current run truth table without treating the mode change as a new off-grid start event.

The logical toggle latch is runtime session state, not part of each State's fourteen bytes. State changes retain its logical level but reset channel offsets/mapping. Patch load resets it to false under the deterministic restart policy.

On Momentary -> Toggled, seed the latch from the current physical level,
without treating it as an edge. On Toggled -> Momentary use the current
physical level. Unpatching MOD clears the latch and displacement; reconnect
while Toggled leaves the latch false even if high, waiting for a real rise.
These are mode/connection reconciliations, not synthetic Starts. Membership
and mute edits consume no PRNG values and do not themselves toggle the latch.

### 15.4 Run truth table

Let `m` mean destination MOD membership and `g` the logical MOD gate. If Run is enabled and MOD is patched:

```text
Off:       run = true
Normal:    run = !m || g
All:       run = g
Alternate: run = m ? g : !g
```

Mute does not change this truth table. All mode stops nonmembers too; Alternate's complementary group is not merely inverted mute.

### 15.5 Restart and temporary displacement [M/P]

Run/Stop is not implemented as only `output *= gate`.

On a genuine logical run transition from stopped to running:

- **Normal:** newly running members produce a rising event immediately and establish a temporary timing displacement anchored at that instant. Nonmembers continue uninterrupted.
- **All:** members restart immediately with temporary displacement. Nonmembers rejoin their canonical Leading grid, waiting for their next canonical rising boundary rather than acquiring an off-grid start.
- **Alternate:** whichever group starts now restarts immediately and establishes its own temporary displacement; the other group stops and clears its audible pulses.

For an unsaturated source, choose displacement so its phase is zero at the restart's master position, then continue at its source ratio on the shared timeline. Retain this destination-owned displacement through later Shift operations. Store a master displacement `deltaM = Mstart - betaAtStart`, so a mapped
source subsequently uses `(M - betaNew - deltaM) * Nnew/Dnew`. Retaining only
`Mstart` and ignoring beta is wrong after Shift to a phased source. For the
saturated fallback, also store `deltaT = Tstart - normalizedOffsetAtStart`;
use `(T - normalizedOffsetNew - deltaT)` modulo the new clamped full period.
Compute both representations at every genuine Start, even if only one is
currently used. Keep signed differences in bounded/rebased representation.
For an unsaturated source, the tick representation uses its recovered nominal
full period to normalize the offset at Start; for a saturated source, the
master representation uses its nominal requested N/D lattice and beta.

Stopping clears that destination's temporary displacement and trigger pulse. A later start creates a new displacement. A State activation and a committed edit to that destination's mapped canonical ratio/phase also clear its displacement. A Shift by itself does not. An H-only tempo update preserves the master-relative displacement; this specific tempo-change representation is a software policy.

On State activation under an already-HIGH logical gate, reconcile run permissions without pretending a fresh MOD transition occurred. On reconnect/mode change/programming-freeze exit, reconcile momentary permission but only a real eligible run-start event gets the documented immediate off-grid restart. A resume caused only by unpatching MOD rejoins the canonical grid.

### 15.6 Simultaneous and repeated transitions

A duplicate Run value is idempotent. A Stop and a scheduled rising edge on the same engine timestamp must result in LOW, with no surviving trigger. An explicit eligible Start may emit one rise on that timestamp, never two because the canonical lattice also happened to cross zero.

A Shift on the same timestamp as Start is processed first; Start applies to the newly mapped source. A State activation cancels a same-timestamp Shift and Run restart but evaluates the final MOD level for permission. These rules are part of the deterministic event ordering in §18.

---

## 16. Copy, Paste, Mutation, and PRNG

### 16.1 Clipboards

Copy State captures the active **working canonical program**, by value. Copy Bank captures the current Bank's sixteen working programs, by value. Later source edits must not alter either clipboard. Shift permutations and Run offsets are not included; use Bake Shift first when capturing a routing arrangement is intended.

Paste copies a valid clipboard into the active State or selected Bank, updates dirty comparisons, and reactivates the active State once if affected. Invalid clipboard is a no-op with visible feedback. No saved memory changes until Store.

Mutate State uses the State clipboard as its source; Mutate Bank uses the Bank clipboard. Both write into the current destination without changing the clipboard. Repeated Mutation therefore varies the copied source, not an unintentionally accumulating previous mutation. An empty relevant clipboard is a no-op and consumes zero random draws.

### 16.2 Recovered random generator [F]

```cpp
inline std::uint8_t nextRandom(std::uint64_t& state) noexcept {
    state = state * 0x5851F42D4C957F2DULL + 1ULL;
    return static_cast<std::uint8_t>((state >> 49) & 0xffULL);
}
```

Unsigned 64-bit wrap is intentional and defined. Starting from seed 1, the next state is `0x5851F42D4C957F2E` and the returned byte is `0x28`.

There is one stream per module instance shared by Mutation and Random Shift. Persist the exact 64-bit state as a fixed-width hexadecimal string, not a JSON floating number. No GUI animation, preview, rejected command, no-op Shift, or empty-clipboard operation consumes draws.

### 16.3 Mutation policy [F perturbations / P order and normalization]

For each destination program, visit canonical channels `0..5` in order. For each channel draw the ratio byte first, then the phase byte:

```text
dr = (ratioByte % 7) - 3
dp = (phaseByte % 5) - 2
newRatio = clamp(sourceRatio + dr, -124, 124)
newPhase = EuclideanModulo(sourcePhase + dp, W(newRatio))
```

Preserve source enable and MOD masks. State Mutation uses exactly twelve draws. Bank Mutation visits destination slots `0..15`, each with the same channel order, for exactly 192 draws. Ratio range handling, phase normalization, and this total ordering are `MutationPolicyV1`; the generator and small perturbation forms are recovered, but the full historical clipping/draw path is not exhaustively established.

Prepare the entire result before publishing a Bank change. Consume draws only after command validation; a failed transaction must not partially change memory or advance half the expected stream.

### 16.4 Rack Randomize

Override Rack Randomize with a deliberate musical operation: mutate a value snapshot of the active working State using the same twelve-draw algorithm, independently of clipboard validity, and leave the clipboard unchanged. Do not randomize momentary buttons, the State knob, current Bank, stored memory, or unrelated globals through a base-class default.

An explicit Seed command may be provided in the inspector. Seed zero is legal for the LCG. Default seed 1 favors reproducibility; two newly initialized instances are allowed to produce the same sequence until the user changes a seed or their command histories diverge.

---

## 17. Select Bus reception, execution, and Mesh

### 17.1 Scope and transport boundary

Implement a byte-stream receiver, not generic MIDI handling. The recovered TEMPI path is receive-only. `F8` clock bytes are ignored by this parser; a tempo bus is a separate typed timing source.

The electrical interface and MCU oscillator frequency are not sufficiently established to infer safe wiring. This Rack module must not send arbitrary voltages on the power bus, automatically open serial devices, or claim a conventional MIDI DIN connection is equivalent.

Provide three implementation layers:

1. `SelectBusParser::pushByte(uint8_t)` with exact stream state.
2. A fixed-size typed semantic event queue and executor in `TempiCore`.
3. A Rack-accessible receiver entry point, plus an advanced inspector action to inject a bounded hexadecimal byte sequence for testing/operation. Parse text on the UI side, enqueue bytes, and display resulting state/error counters.

If the checkout already has a compatible Select Bus transport, integrate it after verifying its contract. Otherwise ship the core receiver and injection path; do not fake support for an unrelated expander. A future bridge can use the same API without changing parser semantics.

### 17.2 Exact recognized statuses [F]

| Byte | Receiver behavior |
|---|---|
| `C0` | Begin a one-data-byte State request |
| `F4` | Begin a one-data-byte copy/store request |
| `F0` | Begin proprietary frame and reset header index |
| `F7` | Clear parser status and payload index |
| Other status bytes `80..FF` | Ignore without generally cancelling the accepted status |

Only `C0` is selected. `C1..CF` are not other channel variants of the same operation. A consumed C0 data byte ends the message; no generic running status. An ignored interleaved `F8` does not become a timing event.

### 17.3 State select

```text
C0 ss
```

The parser emits a State request for its data byte. The semantic layer accepts only `ss=0..63`, remembers it in Free or Follow, and activates according to §11. Invalid 64..127 values are counted/rejected semantically, not wrapped to a valid State.

Examples:

```text
C0 25       → request decimal State 37
C1 26       → no new State request
C0 F8 16    → request decimal State 22; no clock event
C0 01 02    → request State 1 only; trailing 02 has no running status
```

### 17.4 Copy/store dispatcher [F plus P complete store transaction]

```text
F4 dd
```

For data `0..63`, copy the currently active working canonical program to `working[dd]`, recompute dirty, and do not persist it. If dd is the active index, the canonical data is unchanged; do not invent a timing reset solely for an identical self-copy.

For **every** data value `64..127`, invoke the persistent-store transaction and clear Mesh. The recovered dispatcher selects its store path with target W=0 and flag=1; the complete Rack operation is defined here as `Store All` (all working States plus current globals and requested H). Persisting globals/tempo in this software transaction is a policy-level completion of the broader Store semantics, not a claim that the hooked dispatcher tests independently verified every EEPROM write.

Store does not change the selected State, reset the master, or clear the working program's current Shift permutation. Mesh clearing is a separate confirmed dispatcher effect.

### 17.5 Proprietary frame

```text
F0 00 02 2D [commands...] F7
```

The exact three-byte header is mandatory. A mismatch clears status/index; do not continue scanning the mismatched payload as though the header succeeded.

| Payload command | Extra byte | Semantic action |
|---:|---|---|
| `00` | State 0..63 | Clear that Mesh bit |
| `01` | State 0..63 | Set that Mesh bit |
| `02` | None | Default Current working State |
| `03` | None | Revert working States/settings from saved memory |

Accept repeated commands in one frame. An action is emitted when its required bytes have arrived; F7 terminates the frame but is not a transaction commit required for every action. An invalid Mesh State is ignored and counted, with parser alignment retained for the next command.

Test:

```text
F0 00 02 2D 01 07 01 3F 00 07 F7
```

The resulting Mesh bitmap has only bit 63 set. Default/Revert must execute real memory transactions; parser callbacks that merely log an opcode are insufficient.

For an unknown payload opcode, `ParserRecoveryV1` discards payload data until F7 or a new recognized starting status; do not guess an unknown opcode's parameter length. A fresh F0 restarts header acquisition. A fresh C0/F4 supersedes the current payload as a new recognized message. Preserve the exact ignored-status behavior for other high-bit bytes.

### 17.6 Mesh semantics [F bookkeeping / P MeshTransactionV1]

Maintain a 64-bit Mesh mask independently of the current State and Free/Follow selection. Remember mask changes received while Free. Enabling Follow does not erase that remembered mask.

Do not assume that mere Mesh membership continuously mirrors local edits, filters State selection, or causes automatic persistent writes; those broad behaviors are not established by the isolated bitmap/parser evidence.

Implement a reachable explicit **MultiPaste to Mesh** transaction. It snapshots the active working canonical program once and copies that value into every selected Mesh destination in ascending State order. It does not modify saved memory, consume PRNG values, or repeatedly read a changing source. If the current State is included, activate it at most once and only if content changed. Empty Mesh is a no-op. A later `F4` persistent-store form or explicit Store All makes those copies persistent.

A transport that implements a leader's MultiPaste operation through individual F4 copy messages is naturally supported. Do not emit Select Bus messages back out of this receive-only module.

### 17.7 Bounded parser/transport behavior

Use a capacity of at least 512 bytes with explicit occupancy; do not reproduce the original ring's ambiguous full condition. Admit at most 64 bytes per engine frame. On overflow, reject the entire incoming packet, increment `busOverflow`, and leave already queued complete data intact. Reset the parser at the next controlled boundary if transport loss makes message alignment unknown; expose this reset in diagnostics.

A test injection packet is at most 256 bytes. Validate the whole hex string before queueing any byte. Error messages must identify malformed input without blocking the audio thread.

If adding an adjacent Rack expander transport, validate the neighboring model, use Rack's double-buffered expander messages, and acknowledge its one-engine-frame latency. A versioned POD header must contain a magic, protocol version, byte count, and monotonic sequence; payload bytes must be length-bounded. Tempo events, if present, require a separate tagged field and must not be inferred from raw F8. Do not dereference an arbitrary neighbor's message pointer without a known model/protocol contract.

---

## 18. Deterministic processing order and simultaneous events

### 18.1 Engine frame contract

The adapter invokes the core with current sample duration, current port values/connection flags, and bounded queued commands. Sample n observes inputs and writes outputs at timestamp `t[n]`; it must not
emit an edge from `(t[n],t[n+1]]` early. `t[0]=0`. Advance to a new sample using
the duration of the preceding sample; a rate change supplies the duration for
the next interval. Keep whole virtual ticks and a bounded fractional remainder.
For an integer-rate accumulator, rescale its remainder when the denominator
changes; preserve its fraction of a virtual tick, not its raw numerator.

Within a frame:

1. Advance the old timing state through deadlines strictly before `t[n]`, in
   order, using the previously accepted controls. This includes virtual master
   ticks and pulse expirations. No new input is backdated into that interval.
2. Sanitize current inputs, prime connection changes, and capture Schmitt/
   button edges at `t[n]` against the pre-event master state. A capture interval
   uses the difference of whole virtual ticks (floor, never nearest rounding).
3. Decode up to64 bus bytes; process their semantic commands in byte order,
   then sampled panel/keyboard gesture actions, then up to64 UI commands in
   producer FIFO order. Resolve selectors using §18.2. Classify gestures before
   automatic selection so the freeze matrix is already current.
4. Service Leading loss and candidate arbitration once, then eligible Shift,
   then Run permission/Start/Stop. A scheduled virtual boundary exactly at
   `t[n]` has not fired yet, so these controls precede its output effects.
5. Process timing/commit deadlines equal to `t[n]`, coalescing duplicate rises,
   and apply output masks. Write the level at `t[n]` to all six Rack outputs.
6. Publish changed persistence, update lights, and optionally publish the
   decimated display snapshot. Do not advance to `t[n+1]` before returning.

The first frame after initialization/load primes controls and writes LOW.
The second frame establishes the shared rising origin and emits phase-zero
rises permitted by masks/Run. Express replay tick deadlines relative to this
origin. A State activation later in life uses strictly-next-rise behavior,
not the initialization exception. Use a half-open pulse interval `[rise,end)`;
exactly at its end the pulse is LOW unless a genuine new rise extends it.

At supported rates at most two virtual ticks elapse between frames. Bound
fallback advancement to64 virtual ticks/frame for unsupported low rates; on
excess, skip to the correct final master/lane phase, clear audible triggers,
output LOW for that frame and count an overrun (no burst of replayed edges).
Reject nonfinite/nonpositive sample rates with LOW output and a diagnostic,
retaining musical memory. No catch-up on the next valid frame. The 128-crossing
limit in §8.5 is **total across six canonical lanes and six displaced views**
per virtual tick, not 128 per lane. UI deadlines are serviced every frame.
ControlScanV1 scans at frame0 and the first sample at/after each successive
32-tick boundary; when a Gate edge arrives between scans, use the previously
observed base rather than an extra opportunistic ADC scan.

If an event lies exactly on a virtual boundary, apply the relevant control transaction before emitting that boundary's scheduled output event. An edge capture observes the pre-event master state at that instant; source replacement then acts on it.

### 18.2 Priority rules

Within the same timestamp:

```text
Factory Reset / explicit Revert
    > explicit Recall/Paste/Bank memory transaction
    > valid local absolute State selection
    > valid Follow State selection
    > State Gate step
    > Shift
    > Run start / scheduled rising edge
```

This is precedence over **automatic selection and output actions**, not a
sort that reverses FIFO memory commands. Explicit memory commands execute in
the bus -> gesture -> UI order above. Within each source keep FIFO; apply
edits/copies to the logical selected State at that point. C0 in Follow changes
that logical index immediately unless frozen, so `C0 05 F4 06` copies working
State5 into working State6. Coalesce resulting audible State activations to
the final index/content once at this timestamp. In Free the same bytes copy
the original active State because C0 only remembers a request.

Recall/Paste/Revert/Default affecting the active program mark a replacement
barrier: suppress this timestamp's automatic local/Follow/Gate selection and
Shift/Start, not later explicit FIFO operations. A Store neither replaces the
active program nor suppresses selection. Therefore `StoreState; RecallState`
stores the edited program then recalls it; `RecallState; StoreState` recalls
the saved one then stores that value. Factory Reset is the exception: execute
it once and invalidate all remaining commands/partial parser/queues from the
old generation; keep outputs LOW for the initialization frame.

Without such a barrier, the latest valid direct UI State selection wins over
automatic local CV; changed local CV wins over Follow; Follow wins over Gate.
A losing Follow request still updates `lastBusState`, but its final Bank
change is discarded. Local CV uses the latest explicit Bank selection in this
frame, or otherwise the frame-entry Bank. Earlier FIFO copy/store commands
still read the provisional logical State at their point in the stream; their
effects are not retroactively redirected by later selection arbitration. Multiple requests from one source use its last valid value. Use
explicit proposal records to resolve this; do not let incidental array-loop
order choose a Bank. A same-target automatic proposal consumes its priority
but does not truncate gates or clear runtime mapping.

A higher-level State activation cancels same-timestamp Shift and off-grid Run restart, then reconciles the current run truth table. Stop dominates a same-timestamp scheduled rise. If both a true Run start and a canonical rise are eligible, emit only one physical rising event/trigger.

State CV movement wins over a simultaneous State Gate; no additional step. A Shift routed through State Gate prevents State advancement regardless of whether fewer than two eligible destinations made that Shift a no-op.

Two independent same-time edits to different channels both apply. Two edits to the same field apply FIFO, with the final validated value becoming the pending target. UI thread scheduling order must not leak into unlogged, unrepeatable array mutations.

### 18.3 Replay

The headless test harness must accept a timestamped event stream containing input voltage/connection changes and typed commands, then output timestamped internal and physical rising/falling events, active State changes, and relevant diagnostics. Replaying the same initial patch, seed, and event stream at the same sample rate must produce identical event traces.

Cross-sample-rate comparison uses musical deadlines and the permitted
quantization bound, not identical sample indices. For traces whose input
ordering/classification is preserved at every tested rate, PRNG draws, State
contents, command results and final memory must be bit-identical. Use events
safely separated from debounce/scan/gesture boundaries for that test. Events
that quantize to the same sample at one rate but distinct samples at another
can legitimately take different simultaneous-event branches; test those
against each rate's explicit oracle, not an impossible blanket equality rule.
The adapter cannot detect voltage edges that occur wholly between samples.


## 19. Rack lifecycle, threading, undo, and resource handling

### 19.1 Adapter API

Use the actual SDK's signatures. For current Rack 2 documentation, the relevant overrides are:

```cpp
void process(const ProcessArgs& args) override;
void processBypass(const ProcessArgs& args) override;
json_t* dataToJson() override;
void dataFromJson(json_t* root) override;
void onReset(const ResetEvent& e) override;
void onRandomize(const RandomizeEvent& e) override;
void onSampleRateChange(const SampleRateChangeEvent& e) override;
void onBypass(const BypassEvent& e) override;
void onUnBypass(const UnBypassEvent& e) override;
```

Check the installed SDK, especially `RandomizeEvent` rather than copying an incorrect event type from a prose example. Add `onExpanderChange` only if implementing a supported expander transport.

Register the model in the plugin's real registration path, manifest, browser metadata, and build source lists. Use the plugin's existing include and namespace conventions. Do not duplicate `pluginInstance`, introduce a second entry point, or assume source files are discovered recursively without checking the Makefile.

### 19.2 Real-time constraints

Inside `process`/`processBypass`, prohibit heap allocation, file/network access, mutex waits, GUI API calls, JSON parsing, logging to a console, firmware disassembly, and lazy initialization with an uncontrollable lock. Fixed-size copies of 64 short States are acceptable for rare explicit transactions.

Configure/load buffers outside processing. Cache formatted display strings on the UI thread from numeric snapshots. Use fixed-size counters/flags for diagnostics and retrieve them through snapshots. Do not construct a string for every clock edge.

Sampling controls may be optimized only where behavior is preserved. Schmitt edges and momentary button edges must be checked every frame. The continuously varying State CV/knob quantizer may be serviced at a fixed **1000 Hz virtual control rate** under `ControlScanV1`; keep its accumulator independent of the GUI frame rate. At coincident scan and Gate input timestamps, apply the absolute-selection precedence in §11. Do not sample State CV only when a Gate arrives.

Every engine iteration has a bounded upper limit. Large catch-up requests from malformed input or sample-rate changes must not cause an unbounded `while` loop. Record a diagnostic, resynchronize according to the lifecycle policy, and remain responsive.

### 19.3 Bypass [P: BypassHoldoverV1]

Bypass forces all six physical outputs LOW and clears audible trigger pulses. The headless core continues to advance, sample clocks/controls, and retain memory. `processBypass` must explicitly invoke the required core work; do not rely on Rack automatically calling the ordinary process method while bypassed.

On unbypass, Clock50 reflects the current allowed internal level; Trigger10ms waits for the next genuine rising event. Pulses created during bypass are not resurrected. No master phase reset, State recall, or accidental MOD toggle occurs on bypass/unbypass.

### 19.4 Sample-rate changes

Preserve master phase, requested H, lane programs, active State, PRNG, and any in-flight pulse's physical end time. Recompute sample conversion and remaining fractional sample-to-tick remainder so elapsed physical time remains continuous. No 10 ms pulse may become a different-duration pulse merely because the host changed from 48 to 96 kHz.

Input Schmitt states remain primed; a sample-rate change does not reconnect every cable. Display smoothing constants may be recalculated independently. Exercise rate changes during an external-clock acquisition, a trigger, a slow divided clock, and a pending timing commit.

### 19.5 Reset and Randomize

Rack Reset is an explicit full initialization: reset Rack params, restore both factory memory layers and default globals, clear clipboard/bus/gesture/runtime state, set seed 1, and restart at the origin policy in §4. Rack's Reset command is itself the user's destructive action; the additional inspector Factory Reset should ask confirmation before issuing it.

Rack Randomize is the active-State twelve-draw mutation in §16.4. Do not call a base method that independently randomizes every parameter. Never leave PGM or channel params held as a consequence of Randomize.

### 19.6 Undo/redo

Provide Rack history for discrete user-originated musical edits: Machine gestures, direct numeric edits, mute/MOD toggles, global settings, Paste/Mutation, Recall/Revert, Store, and factory reset. Coalesce one physical held gesture into one history action while allowing its preview edits to take effect live.

A history transaction carries an ID. The engine captures bounded before/after
values for affected memory, globals, selector fields and PRNG where needed.
It publishes completion to the UI, which creates a Rack history action.
Undo/redo use the synchronous exclusive adapter route below, then call the
same typed core restore operation. They must not write live core state before
acquiring engine exclusion or enqueue a restore that Rack can silently lose.

History restore reactivates affected programs using normal activation/commit rules; it does not attempt to rewind wall-clock time. Undoing Mutation restores its pre-mutation PRNG state, so repeating the same Mutation from that state is deterministic. Undoing Store restores the previous saved memory as well as dirty status.

Do not create a history entry for each clock edge, CV State step, received bus byte, or LED update. If the fixed completion queue is full, defer/reject a new undoable UI transaction visibly rather than committing a change with silently missing history. Bus-origin operations are not added to Rack history by default, but their effects remain serialized.

### 19.6.1 Bounded history ownership

Reserve completion capacity **before** an undoable edit begins. Use at least
16 fixed history records and a 16-entry completion queue; a record contains
generation, transaction ID, affected-field/State bitsets and before/after
values. A gesture reserves one record on its first actual mutation, updates
its after value, and completes on gesture end/cancellation. Focus loss closes
history for already applied Machine edits; it does not silently roll them back.
An unconsumed Human gesture has no edit/history. Completed Human learning is
also undoable (one tap-release learning transaction).

Only capture/restore affected fields: undoing a ratio must not overwrite a
later unrelated stored Bank or external H. Full-memory before/after is allowed
as storage, but its write mask controls restore. Random operations include
PRNG; Store includes its saved fields. Completion records transfer to the UI,
which copies them into an allocated Rack history action and acknowledges slot
release. DSP never frees that action. History actions resolve the current
module by engine ID and verifies `modelTempi`, not by retaining its pointer.
A missing module makes restore a no-op. Rack Undo of module removal may recreate
the same engine ID; older history actions must work on that restored module.
Generation checks invalidate queued commands, not otherwise valid stored Rack
history actions. Stamp the current generation when executing a history action.
Rack's own Reset/Randomize history must not be duplicated by a second custom
history entry for the same lifecycle callback.

Rack `history::Action::undo/redo()` return void and cannot defer stack movement.
Therefore implement this concrete synchronous bridge on the UI thread:

1. Resolve the module ID and verify its model. A scoped **thread-local** guard
   holds the target pointer and a pointer to the action-owned validated masked
   restore record only for the duration of this call (never in the Action's
   persistent fields or an engine queue).
2. Call `APP->engine->moduleFromJson(module, emptyJsonObject)`; that public API
   acquires exclusive engine ownership, including while audio is stopped.
3. Override `Tempi::fromJson()`. When the thread-local guard targets this exact
   object, apply the masked restore, invalidate stale queued commands, publish
   persistence/display, and return **without** base JSON parsing/resetting
   parameters. For all ordinary calls invoke `Module::fromJson(root)` normally.
4. Clear the guard with RAII and release the temporary JSON outside DSP.

The guard is an in-process capability, never a JSON key or serialized command.
An imported patch cannot request a history operation. Do not send a stale full
snapshot through normal `dataFromJson` and overwrite unrelated recent fields.
Do not call any engine-locking API recursively inside the exclusive callback.
This narrowly justified `fromJson` override is the only extra adapter override
needed beyond §19.1; include a test proving the normal load path calls the base.

If Undo occurs during an unfinished live gesture, cancel that preview by
restoring its captured affected fields and releasing its reserved record
before applying the requested completed history action. Do not push a new
history entry from inside Undo. Keep detector levels primed so held buttons
cannot reappear as fresh presses. The headless harness provides a deterministic
completion sink; clocks and bus/automation-origin edits do not require a
widget or generate UI history.

### 19.7 Removal, duplication, and headless use

Removing a widget or module cancels its UI commands safely; no worker may retain a dangling Module pointer. The core needs no background worker thread. Module duplication clones saved/working memory and PRNG through normal patch data, then uses the deterministic runtime restart policy rather than copying raw pointers or timer object memory.

A missing GUI, browser preview widget with a null Module pointer, or unopened inspector must not prevent clocks from running or cause a crash. Preview uses static defaults and no engine state mutation.

---

## 20. UI completeness and diagnostics

### 20.1 Required observable status

The user must be able to distinguish the following without guessing from one ambiguous LED:

```text
Bank and State number; edited versus stored
Current page and what each channel button does there
Leading source, estimated BPM, acquisition/holdover
Each destination's ratio, phase, source mapping, mute and MOD membership
Run permission and presence of a temporary Run displacement
Clock versus 10 ms trigger mode
Follow and remembered bus State / Mesh membership
Pending timing commit and ratio saturation
Current policy/schema version and nonzero diagnostic counters
```

A compact permanent display may show only State/Bank/tempo and page; the rest can live in the inspector and tooltips. Do not draw sixteen indecipherable status abbreviations on the panel simply to satisfy a checklist.

### 20.2 Error handling

Expose counters for invalid input values, command overflow, bus overflow/malformed frames, rejected bus States, sanitized JSON fields, recovered-guard fallback, live reload sanitization, and scheduler overrun. Use a terse status message for an invalid clipboard or destructive command cancellation.

No dialog may be opened directly from the DSP thread. Do not flood the UI with one message per invalid sample. Diagnostics must not alter clock timing, PRNG draw count, or memory content.

### 20.3 User manual requirements

Ship a short musical guide plus a complete control reference. It must explain Human versus Machine entry, quarter-step ratios, the two phase references, programming freeze, mute versus stop, Shift eligibility, combined Shift/Run jack reassignment, working versus stored States, and what saving a Rack patch preserves.

Include examples for ÷3, ×1.5, six staggered ÷1.5 clocks, Alternate Run/Stop, and an edited-but-not-Stored State followed by Recall. Describe the named software approximations in a small technical note; do not advertise a cycle-perfect hardware clone.

The user-facing name and credits must not suggest manufacturer endorsement. Reuse the Leviathan visual system without importing the original manufacturer's logo or exact product artwork.

---

## 21. Native patch schema and migration

### 21.1 Format contract

Use a versioned JSON object in Rack's module `data` field. Rack serializes params separately. Use readable enum strings in JSON and fixed enum types in C++. The following keys are required in a fully emitted v1 patch:

| Key | Type / cardinality | Meaning |
|---|---|---|
| `schemaVersion` | integer, 1 | Structural version |
| `policyVersion` | string, `TempiPoliciesV1` | Collection of named software policies |
| `virtualTickHz` | integer, 32000 | Explicit timebase identification, not a free control |
| `savedStates` | exactly 64 Program objects | Explicitly Stored memory |
| `workingStates` | exactly 64 Program objects | Current editable memory |
| `savedGlobals` | GlobalSettings object | Stored settings |
| `workingGlobals` | GlobalSettings object | Live settings |
| `savedLeadingH` | integer `200..0xFFFFFF` | Stored Leading tempo |
| `session` | object | Stable session selection, current H, PRNG, bus metadata |
| `stateClipboard` | Program or null | Value-owned State clipboard |
| `bankClipboard` | exactly 16 Programs or null | Value-owned Bank clipboard |

A Program object has exactly these defined fields; unknown fields may be ignored:

```json
{
  "ratio": [-2, -2, -2, -2, -2, -2],
  "phase": [0, 1, 2, 3, 4, 5],
  "enableMask": 63,
  "modMask": 0
}
```

A GlobalSettings object is:

```json
{
  "humanResolution": "100",
  "shiftMode": "off",
  "runMode": "off",
  "modGateMode": "momentary",
  "follow": false,
  "leadingTap": true,
  "outputModes": ["clock", "clock", "clock", "clock", "clock", "clock"]
}
```

Allowed enum strings are `100/50/25`; `off/clockwise/counterClockwise/random`; `off/normal/all/alternate`; `momentary/toggled`; and `clock/trigger10ms` respectively.

A Session object is:

```json
{
  "activeState": 0,
  "bank": 0,
  "baseSlot": 0,
  "gateOffset": 0,
  "currentLeadingH": 8000,
  "rngStateHex": "0000000000000001",
  "lastBusState": null,
  "meshHex": "0000000000000000",
  "lastStateQuantizedSlot": 0,
  "lastTempoAdc": 0
}
```

Hex strings encode the unsigned integer with most-significant digit first. Mesh bit s corresponds to global State s; it is not a printed little-endian byte dump. The saved active State index must agree with `16*bank + ((baseSlot+gateOffset)&15)`; if malformed input disagrees, trust the sanitized `activeState`, derive Bank/base from it, and clear Gate offset.

The companion `fixtures/default_patch_v1.json` is a complete example with all sixty-four States in both layers. It is not a Rack patch wrapper with cables/model IDs, and must be fed as the module's data payload in serialization tests.

### 21.2 What is and is not restored

Restore both memory layers, both globals layers, current and saved H, selector selection, PRNG, clipboard values, last bus State, and Mesh. Recompute all dirty bits; never trust a serialized dirty mask.

Do not restore the following runtime details: absolute engine timestamps, fractional master origin, trigger pulses, held buttons, programming page, unfinished tap histories, pending parser partial frame, transient Shift permutation, or temporary Run displacement. Reset those deterministically and restart LOW then at the master origin as in §4.
Reset/load also invalidates UI/bus partial input and old pending command
generations; UI-owned completion records are retired off audio through their
acknowledgment path, never freed by process().

This is `PatchRestartV1`: patches preserve musical programs and edits but **do not resume at the exact hardware playback sample**. The inspector must not falsely imply that an old unsaved Shift permutation was Stored merely because the patch was saved.

On the first engine frame after loading, prime input detectors and the current State ADC observation without treating initial cable levels or the restored knob as fresh control movements. Preserve the serialized selected State. Subsequent actual knob/CV movement follows normal priority. No generated Gate or MOD toggle occurs from patch restoration.

### 21.3 Malformed and old patches

Parse into a temporary validated value object before replacing live memory. Check JSON types before reading numbers. Missing `data` initializes factory defaults. Unknown fields are ignored, with no attempt to execute them or treat them as commands.

For schema 1 with missing fields, fill from defaults and preserve every valid
independent field. Validate booleans strictly (do not treat arbitrary numbers
as booleans), enum strings by exact spelling, indices by integer range, and
hex values as exactly16 ASCII hexadecimal digits (upper/lowercase accepted;
write lowercase). Missing/invalid PRNG defaults to seed1; Mesh defaults to0.
Invalid clipboard cardinality/type invalidates that entire clipboard, not
sixteen independent partial paste sources. More than64 States are ignored
after the64th; short State arrays repair their missing elements from baseline. Missing working States are copied from the corresponding saved State; missing saved States use factory defaults. A malformed Program array is repaired element-by-element to its relevant baseline, with explicit diagnostics. Clamp integral ratio and phase values to their supported ranges; reject nonintegers rather than silently truncating JSON floating values. Mask enable/MOD fields to six bits after validating integer type.

For unknown/floating/missing-nonlegacy `schemaVersion`, incompatible
`policyVersion`, or `virtualTickHz` other than32000, do not reinterpret timing.
Initialize safe factory runtime and retain a deep copy of the original data
off audio. `dataToJson()` returns that original blob unchanged, including on
autosave, until the user chooses a confirmed **Replace unsupported data with
defaults** inspector action. Reject musical edits while that recovery state is
active; display a clear warning. Missing entire module `data` is normal factory
initialization. A schema1 object with absent policy/timebase uses the schema1
defaults; it is not a fictitious pre-v1 migration. Unknown additional keys in
an otherwise supported object remain ignorable.

Introduce a real migrator when a second schema exists. Do not add a pretend migration for an imaginary older released Tempi format.

### 21.4 Persistence invariants

The following must hold:

```text
load(save(module)) preserves saved and working content independently
saving a Rack patch does not reduce dirty-State count
Store State clears only that State's dirty flag
Recall State restores the saved version even after a Rack save/load
Store All captures current H without changing current phase
patch reload clears transient Shift/Run, but not unsaved program edits
64-bit PRNG state round-trips exactly, including 0 and 0xFFFFFFFFFFFFFFFF
```

File import/export and JSON construction may allocate outside the process loop. Any disk dialog or file read occurs on the UI side, then submits a validated bounded import transaction.

---

## 22. Automated acceptance tests

### 22.1 Required test organization

Provide independent test targets for exact math, parser, memory, Human/gesture policies, live scheduler, and the Rack adapter. The headless core tests must run without opening Rack or an audio device.

Port the supplied CSV/JSON fixtures into C++ tests. Do not only rerun the Python reference model against its own output and call that validation of the C++ module. Add small independent rational/integer oracles for the production scheduler and separate them from the implementation under test.

Use deterministic seeds, machine-readable reports, and named test IDs. Fail on a mismatch; do not regenerate expected outputs automatically during the test run. Fixture regeneration is a separate explicit maintainer command.

### 22.2 Numerical and Leading tests

| ID | Test | Required result |
|---|---|---|
| MATH-01 | All 1,743 supplied ratio rows | Exact integer match |
| MATH-02 | All 42 bundled scalar phase rows | Exact signed result; separately exercise six-lane wiring (the historical harness also ran six-lane cases) |
| MATH-03 | H8000, r-8, p1 | Offset 4000, not 12000 |
| MATH-04 | H8000, r8 | Nominal half-period 2666 |
| MATH-05 | Signed wrap/division boundaries | Match reference; no UBSan overflow |
| MATH-06 | All 249 alignment factors | Exact denominator match |
| MATH-07 | Mixed large denominator LCMs | No 16-bit overflow or unbounded allocation |
| MATH-08 | PRNG fixture stream, seed1 first result | First byte 0x28; all rows match |
| MATH-09 | Full valid ratio/phase ranges | Bounded outputs; negative wrapped phase handled |
| ADC-01 | Threshold and hysteresis boundary vectors | Exact helper match at every T[j] and margin endpoint; the original 4,096 harness cases are not bundled |
| ADC-02 | All previous slots × all 1024 ADC values | Independent reference agreement |
| ADC-03 | Unpatched knob 0/1 | ADC 0/1023 |
| ADC-04 | Patched 5 V with knob 0.5 | ADC 512; attenuator, not addition |
| ADC-05 | Sentinel 65535 | Previous State / previous tempo ADC retained |
| LEAD-01 | First external edge | Starts measurement, no invented interval |
| LEAD-02 | Interval 398 versus 399 | Reject 398; accept 399 with H clamp 200 |
| LEAD-03 | Accepted elapsed120000/120001 at H8000 | Retain / expire respectively |
| LEAD-04 | Zero mailbox | No replacement and no fake rejection |
| LEAD-05 | Invalid capture after prior acceptance | Preserve accepted flag unless separate loss clears it |
| LEAD-06 | Equal-H capture | No period replacement or correction |
| LEAD-07 | Interval32000, R4000, L0/L1 | Current reload12000/22000 |
| LEAD-08 | Signed negative forward intermediate | No abs-value substitution |
| LEAD-09 | Missing input after valid clock | Hold last requested H and continue |
| LEAD-10 | ADC34/35/64/96/992 with P0 | Exact candidates and clamped H in §7 |
| LEAD-11 | Simultaneous tap and physical capture | Physical accepted capture wins |
| LEAD-12 | Tempo ADC while measurement active | Retained ADC may update; candidate not admitted |
| LEAD-13 | Ignored F8 bus bytes | Zero tempo scheduling events |
| ISR-01 | remaining1/current333/next777 | Remaining333, promoted777, level toggled |
| ISR-02 | Guard next800, remaining725/875 | Defer both endpoints |
| ISR-03 | Guard next800, remaining724/876 | Pass band test if other guards pass |

### 22.3 Scheduler and output tests

| ID | Test | Required result |
|---|---|---|
| TIME-01 | Six unity clocks, factory State1 | Coincident rising edges after initialization |
| TIME-02 | H8000, ×3, phase0 | First half-deadlines0,2667,5334,8000,10667,13334,16000 |
| TIME-03 | ×1.25, ×1.5, ÷1.25, ÷2.25 | Rational realignment at each reduced D |
| TIME-04 | Mixed six-lane long run | No growing drift relative to shared master |
| TIME-05 | Factory State15 | Six ÷1.5 programs with 4000-tick phase increments at H8000 |
| TIME-06 | Imported phase255 with wrapped negative offset | Stable normalized schedule, no NaN/huge allocation |
| TIME-07 | Only channel3 edited | Other canonical lanes preserve timing |
| TIME-08 | Edit while high | Old high ends naturally; no premature new rise |
| TIME-09 | Repeated edits reject guard | Latest target commits; first deadline prevents starvation |
| TIME-10 | Ratio clamp at fast/slow extremes | Saturation exposed; clamped lattice used |
| TIME-11 | Mid-cycle Leading change | Master phase continuous and monotonic |
| TIME-12 | State activation | Atomic six-lane change, no master reset |
| OUT-01 | Mute while running | Output LOW, canonical phase and LED timing continue |
| OUT-02 | Unmute during Clock50 HIGH | Physical HIGH allowed; no invented internal rise |
| OUT-03 | Unmute in Trigger mode | No pulse until next true edge |
| OUT-04 | Trigger duration all sample rates | 10 ms within one virtual tick plus one sample |
| OUT-05 | Overlapping trigger events | High extends; no forced 50% truncation |
| OUT-06 | Shift to an already-HIGH source | Square reflects level; no invented trigger |
| OUT-07 | Output cable removal/reconnect | No clock reset or halted engine |
| OUT-08 | Stop exactly on a scheduled rise | LOW; no surviving trigger |
| OUT-09 | True Start plus canonical rise same instant | Exactly one trigger |
| OUT-10 | NaN/Inf input | Finite outputs and incremented diagnostic |

### 22.4 Memory, selector, performance, and mutation tests

| ID | Test | Required result |
|---|---|---|
| MEM-01 | All 64 factory programs | Exact CSV match; enable63 everywhere |
| MEM-02 | Edit, select away, select back | Working edit retained, saved copy unchanged |
| MEM-03 | Store one State | Only selected saved State changes |
| MEM-04 | Recall one State | Saved content restored; globals unchanged |
| MEM-05 | Recall Bank | Exactly sixteen working States restored |
| MEM-06 | Store All | Both global/tempo saved fields updated; no phase reset |
| MEM-07 | Revert | All saved content/settings restored, transients cleared |
| MEM-08 | Dirty edit reversed to original | Dirty flag clears |
| MEM-09 | Copy then edit source then Paste | Original copied value pasted |
| MEM-10 | Empty clipboard Paste/Mutation | No changes, zero PRNG draws |
| MEM-11 | Bank transaction with active State | Single coherent activation, no partial bank |
| SEL-01 | Gate stepping sixteen times | Wrap within Bank |
| SEL-02 | CV base change after Gate steps | Offset cleared |
| SEL-03 | Same-time CV and Gate | CV wins; Gate consumed |
| SEL-04 | Stable knob after bus/explicit selection | Does not overwrite selection every sample |
| SEL-05 | Programming freeze + many Gates | No queued burst on exit |
| SEL-06 | Pending absolute selection in freeze | Latest eligible request applied once on exit |
| SEL-07 | Leading Tap disabled | Combo controls tempo; State Gate still works |
| SEL-08 | Receive State while Free then enable Follow | Remembered State becomes anchor |
| SEL-09 | Local changed CV in Follow | Overrides slot under stated priority |
| SHIFT-01 | All eligible, CW identity | Sources4,1,2,5,6,3 at destinations1..6 |
| SHIFT-02 | CW then CCW | Original permutation restored |
| SHIFT-03 | Muted or nonmember destinations | Assignments excluded and unchanged |
| SHIFT-04 | Fewer than two eligible | No-op; zero draws |
| SHIFT-05 | Random k eligible | Exactly k-1 draws; deterministic permutation |
| SHIFT-06 | Edit through shifted destination | Correct canonical source edited |
| SHIFT-07 | State activation | Identity mapping; stored data not rewritten by Shift |
| SHIFT-08 | Bake Shift | Canonical data captures mapping; no saved writes or Run-offset bake |
| RUN-01 | Every m/g pair in every mode | Exact truth table |
| RUN-02 | Normal member Start off-grid | Immediate rise and temporary displacement |
| RUN-03 | All mode nonmember restart | Next canonical rise, not immediate off-grid rise |
| RUN-04 | Alternate gate transitions | Groups alternate with immediate starts |
| RUN-05 | Shift after Run start | Destination displacement retained |
| RUN-06 | Timing edit/State change | Relevant displacement cleared |
| RUN-07 | High MOD during State change | No false new off-grid Start |
| RUN-08 | Unpatched MOD in configured Run mode | All destinations run canonically |
| RUN-09 | Both Shift and Run enabled | State Gate shifts, never steps State |
| RNG-01 | State Mutation | Exactly12 draws, fixed order, clipboard unchanged |
| RNG-02 | Bank Mutation | Exactly192 draws, slot/channel order |
| RNG-03 | Endpoint ratio/phase mutation | Defined clamp and Euclidean modulo |
| RNG-04 | Undo Mutation then repeat | Same result after PRNG restoration |

### 22.5 Human, gestures, parser, and integration tests

| ID | Test | Required result |
|---|---|---|
| HUMAN-01 | One tap only | No ratio replacement |
| HUMAN-02 | Δmaster=1.5 at100/50/25% | Quantizer follows candidate grids and tie rules |
| HUMAN-03 | Exact ×1.5 tapping in50% mode | Select r2 |
| HUMAN-04 | Exact ÷2.25 tapping in25% mode | Select r-5 |
| HUMAN-05 | Tempo changes between taps | Use recorded master advance, not latest BPM alone |
| HUMAN-06 | First tap later consumed by Machine gesture | No accidental Human commit |
| HUMAN-07 | Channel1 Leading Tap enabled | H changes; stored channel1 ratio/phase unchanged |
| HUMAN-08 | Programming window ends before long tap-history timeout | State unfreezes; history remains until its separate limit |
| GEST-01 | PGM-first versus channel-first | Coarse versus Fine, never both |
| GEST-02 | Double A/B versus single A/B | Correct Edit page, no intermediate single-page action |
| GEST-03 | Short both, double both, hold both | Phase, Clock Edit, State Edit respectively |
| GEST-04 | Opposite PGM after Coarse established | Doubles count; does not open State Edit |
| GEST-05 | Focus loss while held | Releases/cancels without stuck button or extra edit |
| GEST-06 | Fine phase at p0 earlier for r-5 | Euclidean wrap within W9, not uint8→255 |
| GEST-07 | Coarse phase after Fine | Overrides Fine baseline under PhaseCodeEditV1 |
| BUS-01 | C0 25 and C0 F8 16 | Decimal37 and22; no F8 clock |
| BUS-02 | C1 26 and missing running status | No improper State request |
| BUS-03 | F4 target0..63 | Working copy only; target correct |
| BUS-04 | Every F4 data64..127 | Store path and Mesh clear, no unsupported range shortcut |
| BUS-05 | Valid repeated Mesh frame | Only bit63 remains in supplied example |
| BUS-06 | Invalid header/truncation/new status | Deterministic parser recovery, no stray action |
| BUS-07 | Payload02/03 | Real Default/Revert transactions |
| BUS-08 | Mesh received while Free | Preserved and available when Follow enabled |
| BUS-09 | Explicit MultiPaste | Snapshot source once; saved content unchanged |
| BUS-10 | Queue overflow/malformed hex | Atomic rejection and diagnostic |
| PATCH-01 | Save/load dirty working State | Dirty content survives; Recall still finds old saved version |
| PATCH-02 | Maximum 64-bit PRNG and Mesh | Exact hexadecimal round trip |
| PATCH-03 | Malformed arrays, nulls, floats, enums | Safe deterministic repairs, no crash |
| PATCH-04 | Future schema/policy | Safe unsupported-data handling, no blind reinterpretation |
| PATCH-05 | Load with held-high inputs | No invented Gate, toggle, or clock capture |
| PATCH-06 | Load selected bus State with unchanged knob | Restored State remains until real movement |
| RACK-01 | Bypass/unbypass mid-trigger | No resurrected pulse; clock core continues |
| RACK-02 | Sample-rate change mid-pulse and mid-divider | Physical duration/phase preserved |
| RACK-03 | Randomize | Musical mutation only; no held params |
| RACK-04 | Reset | Both factory layers/defaults and seed restored |
| RACK-05 | Browser preview/null module | Safe rendering; no engine access |
| RACK-06 | Module removal during UI completion | No dangling pointer or unjoined thread |
| RACK-07 | Dense GUI edits + TSAN-supported harness | No data races in commands/snapshots/history |
| RACK-08 | Existing plugin build and unrelated-module tests | No regressions |

### 22.5.1 Added implementation-boundary gates

| ID | Test | Required result |
|---|---|---|
| FRAME-01 | Control and virtual rise exactly on sample timestamp | Control first; no one-sample-early output |
| FRAME-02 | Initialization and later same-program State activation | One LOW startup frame; origin rise next frame; ordinary activation strictly next rise |
| FRAME-03 | Rate change with fractional tick remainder | Preserve physical fraction and pulse end; no duplicated/skipped origin |
| FRAME-04 | Invalid rate / excessive tick catch-up | Bounded LOW/diagnostic path; memory retained |
| COMMIT-01 | Replaced pending edits plus H changes | First-request fallback deadline never moves |
| COMMIT-02 | Guard admitted while canonical or displaced destination HIGH | Wait retained falls once; strict next new rise |
| COMMIT-03 | H-only transition across saturation | Preserve both Run displacement representations |
| ORDER-01 | Follow versus Free: C0 05 F4 06 | Copy correct logical source in byte order |
| ORDER-02 | Store;Recall versus Recall;Store | FIFO explicit transactions, no priority sorting reversal |
| ORDER-03 | Follow bank request and local movement same frame | Local target wins without adopting losing bus Bank |
| ORDER-04 | Human press and State Gate same frame | Freeze first; Gate dropped |
| GEST-08 | Exactly40/250/350 ms and one frame either side | Defined inclusive chord/double and hold precedence |
| GEST-09 | Context-menu State Edit | Stays open without fake held PGM; Done/Escape exits |
| GEST-10 | Panel and keyboard hold same button | OR ownership; release one does not release the other |
| SNAP-01 | Save repeatedly during audio edits/Store/bus activity | Coherent persistence, TSAN clean, no DSP mutex |
| SNAP-02 | Widget and serializer concurrently consume | Separate exchanges; no stolen reader slot |
| HIST-01 | Completion queue full, active gesture, focus loss | Reserved record prevents untracked mutation; one completion |
| HIST-02 | Undo ratio after unrelated change | Restore write mask only, no unrelated Bank/H rewind |
| HIST-03 | Removal/load/reset and synchronous history restore | Generation-safe queues; normal JSON remains normal; undo works after undoing module removal |
| PATCH-07 | Unsupported policy/timebase then autosave | Preserve original blob until explicit replacement |
| BUS-11 | Reject whole packet during partial accepted frame | Existing parser state/data retained, no false loss reset |

`fixtures/review_policy_vectors.json` supplies additional **P-only** rational
edge/commit/Run/Human examples. `tools/check_review_contract.py` checks those
examples and document structure; it is not a substitute for production C++
tests implementing the IDs above. `validation.json` and `timing_validation.json`
are historical aggregate reports, not thousands of executable input rows.

### 22.6 Duration, sample rate, robustness, and performance gates

Run the audio-frame engine for at least 60 simulated seconds for representative mixed ratios at each supported sample rate. Run a 24-hour-equivalent scheduler alignment test using an event-driven test driver that advances between meaningful deadlines; compare against an independent rational oracle. Do not imply that an event-driven scheduler test exercised every Rack audio frame for 24 hours.

Fuzz valid/invalid JSON and parser input with bounded reproducible corpora. Exercise every valid ratio code with representative raw phase bytes including 0,1,3,4,127,255. Run AddressSanitizer/UndefinedBehaviorSanitizer on supported platforms; run a thread sanitizer or targeted ownership tests where available.

Record measured CPU cost with the exact compiler, optimization flags, sample rate, hardware, module count, and whether the GUI was visible. No universal “under X% CPU” claim is possible without a defined machine. Acceptance is: no allocations/locks in normal DSP; bounded workloads under worst supported controls; no unexplained CPU scaling with tempo when saturated; no hidden work in draw; and no major regression against a plain six-lane event scheduler baseline on the same machine. Report the actual numbers, not invented benchmark targets as achieved results.

A performance improvement may not drop source clocks for unpatched outputs, skip input edges, shorten trigger pulses, or reduce the State machine to frame-rate updates.

---

## 23. Implementation phases and completion gates

### Phase 0 — Repository reconnaissance and test scaffolding

Inspect checkout/build conventions and source evidence. Establish the new headless target, fixed types, fixtures, and build registration without disturbing existing modules. Record the actual SDK/platform used. Gate: an empty but registered build target plus executing fixture-loader tests; no claim of a finished module yet.

### Phase 1 — Exact kernel and Leading service

Implement signed arithmetic, ratios, phase, alignment, PRNG, State/tempo ADC, capture/loss/correction, and exact ISR/guard reference helpers. Gate: all original numerical vectors and the new boundary tests pass against C++.

### Phase 2 — Shared timing and output engine

Implement VirtualClock32kV1, RationalBoundaryV1, saturation, commit policy, trigger shaping, mute, lifecycle origin, and headless event traces. Gate: exact reference helpers still pass; independent alignment, pulse, drift, and edit-transition tests pass. Keep policy differences explicit.

### Phase 3 — Memory, selector, Shift, and Run/Stop

Implement all factory States, working/saved transactions, selector hysteresis and freeze, routing permutation, gate truth tables, restart offsets, Copy/Paste/Mutation, and deterministic ordering. Gate: every MEM/SEL/SHIFT/RUN/RNG test passes; no UI is required to prove functionality.

### Phase 4 — Human Programming and gesture controller

Implement named Human/phase/gesture policies, all pages, source-specific Channel 1 behavior, and typed direct controls. Gate: complete keyboard/command replay scripts can exercise every musical operation without Rack rendering.

### Phase 5 — Rack integration and persistence

Build the actual module, stable params/ports, panel, themes, inspector, safe snapshots, native schema, history, bypass/reset/randomize, and sample-rate changes. Gate: Linux/Windows/macOS builds available in the project's supported CI matrix; do not claim a platform was built if its toolchain was unavailable. Exercise the module interactively where Rack is available and record what was checked.

### Phase 6 — Select Bus and Mesh

Implement parser, semantic executor, injection interface, memory commands, Mesh/MultiPaste, diagnostics, and any verified existing transport adapter. Gate: BUS tests pass and a user can execute a received State request and copy/store operation through a functioning Rack-accessible receiver path.

### Phase 7 — Hardening and handoff

Run regression, replay, sanitizer, malformed-data, sample-rate, long-run scheduler, and measured performance tests. Finish user documentation, architecture notes, policy limitations, and visual fit checks. No fake buttons, empty menu callbacks, or unimplemented required semantic commands remain.

Phases may overlap for efficient implementation, but a later visual phase must not replace missing musical behavior. Do not stop after Phase 2 and label a generic six-clock module “Tempi complete.”

### 23.1 Definition of done

The implementation is complete when all required behavior in §1.1 is reachable, every mandatory test has a recorded outcome, the supported plugin build succeeds, patch persistence preserves both memory layers, and the code/documentation identifies all P-policy behavior honestly.

There must be no TODO standing in for Human learning, Run displacement, Store/Recall, Phase, Select Bus execution, or an entire programming page. Open **fidelity** questions may remain, with the implemented fallback identified; open missing-feature stubs may not be relabeled as fidelity questions.

### 23.2 Final Codex handoff report

Provide changed-file list, build/test commands, test counts and failures, actual benchmark environment/results, policy versions, any remaining defects, and a small set of usage/demo patch instructions. Include the following artifacts in the repository:

```text
manual/Tempi.md
doc/tempi/architecture.md
doc/tempi/fidelity-and-policies.md
tests/tempi/README.md
tests/tempi/fixtures/...
tests/tempi/replays/...
```

Provide at least three reproducible demonstration event scripts or Rack patches: factory polymetric clocks; Shift plus Alternate Run/Stop; and unsaved State editing followed by Store/Recall. Rack patch wrappers must use the actual registered plugin/model identifiers from the checkout, not guessed identifiers from this document.

---

## 24. Uncertainty register and replacement seams

| Topic | What is known | Implemented policy | Evidence needed for refinement |
|---|---|---|---|
| Physical timer frequency | Tick-domain fields and clamps | VirtualClock32kV1 | Measured timer cadence or verified oscillator/configuration path |
| Full timing commit/history | Numerical guard and reload fragments | RationalBoundaryV1 + NextSafeEdgeV1 | Complete routine tracing and recorded edge traces through edits |
| Fractional correction sequence | Ratio codes and alignment denominators | Remainder-carry rational deadlines | Long hardware edge traces at fractional and prime ratios |
| Human rhythm quantization | Tap learning/resolution concepts | HumanQuantizerV1 | Completed quantizer control/data-flow plus controlled tapping traces |
| Human active-window duration | Selection freezes during programming | HumanActivityWindowV1 | Button/State CV behavior with measured gaps |
| Fine/coarse phase gestures | Exact phase arithmetic; gesture families | PhaseCodeEditV1 | UI-to-phase-byte traces for divided and noninteger multiplied clocks |
| ADC physical calibration | Threshold-driven functions and table | T[j]=32+64*j | Real calibration bytes and voltage/ADC transfer measurements |
| Leading source/Follow arbitration | Capture/ADC fragments and public semantics | LeadingSourcesV1 + SelectorPriorityV1 | Simultaneous physical/tap/bus/CV behavior traces |
| Exact Random Shift shuffle | Shift role and shared PRNG | ShiftPermutationV1 | Random permutation call path and draw consumption |
| Mutation clipping/draw order | LCG and perturbation forms | MutationPolicyV1 | Full mutation path across endpoints, copies, and Banks |
| Mesh editing propagation | Bitmap/parser/copy-store dispatch | MeshTransactionV1 | Complete leader/receiver transaction traces; no inferred auto-mirroring |
| F4 store global scope | Persistent dispatcher path | Store All transaction completion | Full persistent routine call graph and changed-byte trace |
| Physical electrical behavior | MCU pin roles; user-facing jack roles | Rack voltage/Schmitt conventions | Schematic or bench measurements |
| UI timing and palette | Page concepts; version changes | GesturePolicyV1 and Leviathan UI | Measured hold/double thresholds and specific target revision UI behavior |
| Runtime restoration in Rack | No original Rack format exists | PatchRestartV1 | Product decision, not a reverse-engineering question |

Keep these seams as small policy classes/functions or explicit enum/version fields. Avoid sprinkling `if (approximate)` throughout unrelated code. Refining a policy must include fixture migration, old-patch behavior strategy, and a new policy identifier when audible behavior changes.

---

## 25. Source register, provenance, and use of companion files

### 25.1 Supplied archive sources

All archive paths below are relative to the extracted `Tempi/` directory:

| Source ID | File(s) | Role |
|---|---|---|
| A1 | `TEMPI71_BEHAVIORAL_ENGINEERING_SPEC.md` | Main consolidated behavior/evidence dossier; especially §§4–19 and timing addenda |
| A2 | `TEMPI71_IMPLEMENTATION_CHECKLIST.md` | Existing implementation checklist |
| A3 | `TEMPI71_EVIDENCE_MATRIX.csv` | Claim-level evidence and confidence |
| A4 | `tools/models.py` | Exact ratio, phase, PRNG, ADC, EEPROM decoding models |
| A5 | `tools/timing_models.py` | Recovered Leading capture and tempo-control helpers |
| A6 | `analysis/ratio_test_vectors.csv`, `phase_test_vectors.csv`, `ratio_alignment_factors.csv` | Golden numerical fixtures |
| A7 | `analysis/factory_states.csv`, `factory_states.json` | All sixty-four decoded factory States |
| A8 | `docs/SELECT_BUS.md`, parser trace/dispatcher tests | Precise receive parser and F4 distinction |
| A9 | `docs/TIMING_RECONSTRUCTION.md`, `analysis/timing_validation.json` | Timing findings and recorded validation limits |
| A10 | `analysis/global_eeprom_partial_map.csv`, `docs/MEMORY_AND_STATES.md` | Partial globals/memory; apply the mask correction in §2 |
| A11 | `analysis/disassembly_reachable.asm`, `functions.csv`, `ram_symbols.csv` | Further investigation and address-level evidence |
| A12 | `tools/validate_and_extract.py`, `validate_timing.py`, `pic18_harness.py` | Reproducible source-analysis harness, not module runtime |

Sections 5–7, 10–12, and 16–17 principally derive their F claims from A1/A4–A12. Their M claims use the supplied consolidated dossier plus the dedicated official documentation below. Architectural, threading, JSON, and live scheduling choices are expressly P unless otherwise marked.

The companion handoff copies the relevant reference models, fixtures, primary Markdown evidence, and a checksum inventory. It does not replace the full original archive for instruction-level investigation.

### 25.2 Official external references

These references were consulted for the implementation contract on 2026-09-28. They are not a claim that the supplied firmware is the latest firmware available on that date.

- **R1 — Rack Plugin Guide:** <https://vcvrack.com/manual/PluginGuide>. Port/parameter setup, JSON methods, engine/UI boundaries, and expander guidance.
- **R2 — Rack Voltage Standards:** <https://vcvrack.com/manual/VoltageStandards>. Gate conventions and hysteretic edge handling. TEMPI's 10 ms mode is a deliberate module-specific exception to the ordinary short-trigger convention.
- **R3 — Rack Module API:** <https://vcvrack.com/docs-v2/structrack_1_1engine_1_1Module>. Actual lifecycle callback names and event types; verify against the SDK in the checkout.
- **R4 — Rack Plugin Development Tutorial:** <https://vcvrack.com/manual/PluginDevelopmentTutorial>. Standard registration/build integration patterns.
- **R5 — Official TEMPI product page:** <https://www.makenoisemusic.com/modules/tempi/>. Product/manual provenance.
- **R6 — Official TEMPI manual:** <https://www.makenoisemusic.com/wp-content/uploads/2024/03/tempimanual.pdf>. Dedicated control descriptions, combo attenuator behavior, programming pages, Clock Edit, Follow, and the v71 changelog. In the inspected PDF, page indices 6–7 show the panel/control roles, 27 covers Clock Edit, and 33 contains the relevant later changelog; these are zero-based PDF indices, not printed page numbers. Prefer dedicated sections and updated behavior over inconsistent older quick-reference entries.

The initial1.0 handoff did not inspect Leviathan. The1.1 review inspected this
checkout on2026-09-29. Reverify on another branch; these are integration facts,
not permission to replace infrastructure:

- `src/plugin.hpp` declares models; `src/plugin.cpp` registers them;
  `plugin.json` supplies module browser metadata. No `modelTempi` exists yet.
- `Makefile` includes `src/*.cpp` and selected subdirectories, **not** arbitrary
  recursive sources. Add `SOURCES += $(wildcard src/tempi/*.cpp)` explicitly.
  The installed SDK compiles production as C++11; use C++11-compatible source
  and compile headless production tests with the same standard.
- SDK flags include unsafe floating math. Add target-specific
  `-fno-fast-math -fno-unsafe-math-optimizations` for Tempi adapter/math/scheduler
  objects where NaN checks, rounding and deterministic timing require it,
  following the existing Chimera/Mandelwake pattern.
- Use `src/SpscLatestSnapshot.hpp` under its single-consumer contract (§3.3),
  `src/PanelSvgUtils.hpp`, the existing theme controls, and the root AGENTS.md.
  If the panel is split, edit only master `res/Tempi.svg`; regenerate through
  `tools/split_svg_labels.py` and `make generate-panel-anchor-atlas`.
- New basic performance stats must follow the repository's explicit Process,
  Step, Draw and DrawLayer contract (`DL (us)` label). Gate collection through
  `isDragonKingDebugEnabled`; keep worker/component work out of these totals.
- This workspace validates with native MSYS2 MINGW64 and `../Rack-SDK`;
  Rack-linked tests need `RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`.
  Full `plugin.dll` linking is required here. Do not claim macOS/Linux plugin
  builds without those actual toolchains. No staging or commits are authorized.
- The broad `test-fast` baseline currently fails Sibyl P5/P6 companion/source
  fixtures. Record fresh baseline output in Phase0, distinguish unchanged
  failures from regressions, and do not modify unrelated Sibyl to green Tempi.


The local Rack source check used `../Rack/src/engine/Engine.cpp`:
`Engine::moduleToJson`/`Engine::toJson` and processing use shared locking,
while `Engine::moduleFromJson`/Reset/Randomize use exclusive locking.
`../Rack-SDK/include/engine/Module.hpp` alone does not establish that saving
has exclusive ownership. This is why §3.3 requires a persistence exchange.

### 25.3 Input hashes

```text
Tempi(1).zip
  d946a2a08b191de277fcd2f68d0399daf30cf4162342e4505b84e36ca72d2269

TEMPI71_BEHAVIORAL_ENGINEERING_SPEC.md
  0580f85ae45c4d0754b2b66c6c8febeb7ccceb2f16e4dd29b840fb5f682b90f2

firmware/SYNTHETIC_factory_eeprom.bin
  53659308a0a00f724d370661951175276fd719647aa6f0f0d731a54f008168b9

firmware/tempi71_recovered.hex
  fee378dcb92fa2f85bf4ef8bdb35f12b61920612576f9aca302ab46bcf00a5c8
```

The archive manifest's `input/tempi71.wav` path is absent in the extracted top-level directory, but the exact WAV is present inside `tempi71update.zip` as `tempi71.wav`. Its hash matches the manifest:

```text
dfea58ec1694b63f645dc85af62d0971077942b281ae445c04ebaa2bebcbd09c
```

When rerunning the complete source recovery, extract that exact nested member to the expected `input/tempi71.wav` path and verify the hash. Do not substitute another downloaded firmware revision or silently change fixtures to make a run pass.

The nested WAV and recovered firmware are not required to build or run the Rack implementation and are intentionally not redistributed in the companion handoff.

### 25.4 Companion check command

From the extracted handoff directory, run:

```bash
python3 tools/check_handoff.py
```

This verifies packaged checksums, reference fixture consistency, the complete default data payload, and selected policy examples. It is an integrity/reference check only. It does **not** compile a Rack module, validate the future C++ engine, or independently prove physical hardware behavior.

`VALIDATION_NOTES.md` also records a successful UBSan-enabled compilation/test of the specification's extracted C++ ratio, phase, and PRNG snippets. The optional source is `tools/spec_kernel_smoke.cpp`; it is a narrow arithmetic check, not a Rack implementation.

The implementing project must add its own build/test commands. Document exact commands that actually exist; do not paste a fictitious `make test-tempi` target into a completion report without creating it.

---

## 26. Final implementation guardrails

Keep these invariants visible in review:

1. A divider's phase unit is a quarter master cycle; a multiplier's is a quarter channel cycle.
2. Stored timing, temporary Shift routing, and temporary Run displacement are different data.
3. Mute suppresses output, not internal timing; Run starts can create a new timing origin.
4. Six lanes share a timing reference and bounded rational alignment rather than drifting independently.
5. Rack patch save is not TEMPI Store, and both memory layers survive a patch round trip.
6. State CV/knob is an absolute base with hysteresis; State Gate is a relative offset and can be reassigned to Shift.
7. A masked/routed level change is not automatically a true timing edge or a new 10 ms trigger.
8. Select Bus parsing, its copy/store command, and a separate tempo source must not be conflated.
9. Exact numerical fixtures and explicit software policies are separate test targets and separate compatibility claims.
10. Every required musical function has a working command path, a reachable UI path, and an acceptance test.

**Implement the instrument described above, preserving both its musical workflow and the boundaries of what the evidence actually establishes.**
