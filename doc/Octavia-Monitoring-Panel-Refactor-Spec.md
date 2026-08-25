# Octavia Monitoring + Panel Refactor Specification

**Project:** Leviathan Rack2  
**Target branch:** `expander`  
**Primary module:** `src/Octavia.cpp`  
**Related module:** `src/Sibyl.cpp` / Sibyl control and timing infrastructure  
**Panel asset:** `res/Octavia.svg`  
**Status:** Implementation specification  

---

## 1. Purpose

Refactor Octavia from a two-input always-on audio analyzer into a lightweight, explicit **machine-listening and measurement layer** for VCV Rack.

Octavia must be able to:

1. Keep a persistent stereo **Master L/R** listening point with live panel meters.
2. Add four freely assignable monitor inputs, **A, B, C, and D**.
3. Continuously retain cheap, sample-aligned rolling history from all connected monitor inputs.
4. Perform detailed analysis only when requested.
5. Compare arbitrary monitored points in a patch without forcing A/B or stereo semantics on the physical jacks.
6. Support future sample-accurate observation triggers from Sibyl so a sequence can intentionally create and capture an experiment.
7. Make machine attention visible to the human through per-monitor status LEDs.
8. Avoid materially increasing Octavia's idle audio-thread cost.

The design goal is not merely “more analyzer ports.” Octavia is the sensory interface through which an AI can observe the effect of edits to a Rack patch, compare before/after states, and eventually participate in controlled tune → observe → compare → refine loops.

---

## 2. Current Branch Baseline

This specification is based on the current `expander` implementation.

### 2.1 Existing audio inputs

`Octavia` currently exposes two inputs:

```cpp
enum InputId { AUDIO_IN_L, AUDIO_IN_R, INPUTS_LEN };
```

These are configured as `Audio Analyze L` and `Audio Analyze R` and currently occupy numeric input IDs 0 and 1.

### 2.2 Existing rolling capture

Octavia currently maintains two independent `AudioRingBuf` objects:

- 4096 samples each;
- one ring for left and one for right;
- atomic sample storage;
- independent atomic heads;
- written from `process()` and snapshotted by HTTP handlers.

The current 4096-sample window is approximately 93 ms at 44.1 kHz.

### 2.3 Existing continuous loudness path

The existing `LoudnessMeter` continuously performs more work than the visible panel meters strictly require. It currently maintains:

- BS.1770 K-weighting filters;
- raw and K-weighted sums;
- peak accumulation;
- clipping counts;
- L/R cross-products for correlation;
- 100 ms block history;
- published long-term sums;
- up to 36,000 100 ms loudness blocks (approximately one hour).

The live panel display itself only reads the latest four 100 ms blocks for momentary LUFS and the recent meter peak values.

### 2.4 Existing detailed analysis

The current HTTP analyzer provides:

- RMS and peak;
- coarse frequency information;
- detailed problem-frequency analysis;
- DC offset;
- clipping information;
- broad spectral bands;
- hum detection;
- resonance detection and temporal stability;
- feedback suspicion;
- optional raw spectrum output.

The detailed analyzer currently performs repeated Goertzel scans over 4096-sample snapshots and obtains temporal stability by taking two snapshots separated by approximately 120 ms.

### 2.5 Existing panel

The current panel is narrow and uses SVG anchor IDs for key controls and regions. Relevant existing elements include:

- `OCTOPUS_STATUS` framebuffer region;
- `READ_ACTIVITY_LIGHT`;
- `WRITE_ACTIVITY_LIGHT`;
- `LOUDNESS_METERS`;
- `START_PARAM`;
- `AUDIO_IN_L` and `AUDIO_IN_R`.

The refactor should continue using SVG anchors rather than making C++ layout coordinates authoritative.

---

## 3. Core Design Decisions

The following are requirements for this refactor.

### 3.1 Preserve Master L/R

The existing input IDs 0 and 1 become semantically:

```text
MASTER L
MASTER R
```

They remain Octavia's persistent primary listening point and continue driving the live LUFS and dBFS panel meters.

Do **not** repurpose these inputs as Monitor A/B.

### 3.2 Add four independent monitor probes

Append four new physical inputs:

```text
A
B
C
D
```

These are independent mono observation points.

They may be used in any combination:

- one monitor by itself;
- A/B as a before/after comparison;
- A+B as a stereo pair;
- C+D as a stereo pair;
- A+B versus C+D as stereo before/after;
- four unrelated patch locations;
- arbitrary pair or group comparisons requested by the agent.

Stereo meaning is an analysis/API concept, not a hardware constraint.

### 3.3 Separate listening from analysis

Octavia must distinguish three concepts:

1. **Capture** — cheap continuous rolling sample history.
2. **Live metering** — minimal always-on Master L/R visual metering.
3. **Analysis** — expensive DSP performed only after an explicit request.

No detailed spectral analysis should execute continuously in `process()`.

### 3.4 Use one shared Rack-frame timeline

Master L/R and A-D must share one frame-indexed observation history.

The six observed channels are:

```text
MASTER_L
MASTER_R
A
B
C
D
```

Each captured frame must correspond to a single Rack engine frame so snapshots across multiple probes are temporally matched.

Use Rack's process-frame timeline (`ProcessArgs::frame`) as the canonical sample-time anchor where practical.

### 3.5 Physical cables remain the sensory boundary

Octavia listens to monitored audio/CV through physical Rack patch cables.

Do not implement hidden arbitrary reads from other modules' outputs as a substitute for patch cables.

Sibyl or an AI may trigger **when** Octavia captures/analyzes, but physical monitor cables determine **what** Octavia can hear.

---

## 4. Input and Light Compatibility

Rack patch compatibility depends on numeric IDs. Existing IDs must not move.

### 4.1 Input IDs

Refactor the enum to equivalent semantics while preserving numeric values:

```cpp
enum InputId {
    MASTER_L_INPUT = 0,
    MASTER_R_INPUT = 1,
    MONITOR_A_INPUT,
    MONITOR_B_INPUT,
    MONITOR_C_INPUT,
    MONITOR_D_INPUT,
    INPUTS_LEN
};
```

Add compile-time assertions for compatibility:

```cpp
static_assert(MASTER_L_INPUT == 0);
static_assert(MASTER_R_INPUT == 1);
```

Renaming the C++ identifiers is permitted; changing the numeric IDs is not.

### 4.2 Light IDs

Preserve all existing light IDs and append new monitor lights after them:

```cpp
MONITOR_A_LIGHT,
MONITOR_B_LIGHT,
MONITOR_C_LIGHT,
MONITOR_D_LIGHT
```

Do not insert the new lights in the middle of the existing light enum.

---

## 5. Panel Refactor

### 5.1 Overall direction

Widen Octavia's panel to support the new sensory role.

The widened panel should:

- make the octopus substantially larger and more visually dominant;
- preserve the existing title, server status/control area, read/write indicators, and dual Master meter concept;
- retain Master L/R near the lower portion of the module;
- add a dedicated vertical monitor rail along the right side for A-D;
- place one small status LED adjacent to each A-D jack;
- remain legible and uncluttered rather than turning A-D into four additional meter strips.

### 5.2 Width

The exact HP width is an asset/layout decision rather than a DSP requirement.

**Recommended starting target: 10 HP (50.8 mm).**

If the larger octopus, meter geometry, and A-D monitor rail cannot breathe comfortably at 10 HP, moving to 12 HP is acceptable before the layout is finalized. Do not silently expand beyond that without revisiting the design.

### 5.3 Suggested visual hierarchy

The intended hierarchy is:

```text
+------------------------------------+
|              OCTAVIA               |
|                                    |
|       [ enlarged octopus ]     A o |
|                              led * |
|                               B o  |
|                              led * |
|       server / status          C o |
|                              led * |
|                               D o  |
|                              led * |
|                                    |
|      [ LUFS ]      [ dBFS ]        |
|        MASTER L     MASTER R       |
+------------------------------------+
```

This is conceptual only. Final positions must come from SVG anchors.

### 5.4 SVG anchors

Add or migrate toward these semantic anchor IDs:

```text
TITLE_LABEL
OCTOPUS_STATUS
READ_ACTIVITY_LIGHT
WRITE_ACTIVITY_LIGHT
START_PARAM
LOUDNESS_METERS
MASTER_L_INPUT
MASTER_R_INPUT
MASTER_L_LABEL
MASTER_R_LABEL
MONITOR_A_INPUT
MONITOR_B_INPUT
MONITOR_C_INPUT
MONITOR_D_INPUT
MONITOR_A_LIGHT
MONITOR_B_LIGHT
MONITOR_C_LIGHT
MONITOR_D_LIGHT
MONITOR_A_LABEL
MONITOR_B_LABEL
MONITOR_C_LABEL
MONITOR_D_LABEL
```

During migration, C++ may fall back to the old `AUDIO_IN_L`, `AUDIO_IN_R`, `AUDIO_LABEL_L`, and `AUDIO_LABEL_R` anchors so intermediate panel assets still load.

### 5.5 Monitor LEDs

The A-D LEDs indicate **machine attention/status**, not signal amplitude.

Minimum required behavior:

| State | LED behavior |
|---|---|
| Jack disconnected | Off |
| Connected and rolling history available | Dim steady |
| Snapshot requested/captured | Short bright flash |
| Detailed analysis using this monitor | Bright or gently pulsing |

A single theme-appropriate LED color with brightness/animation states is sufficient for v1.

Do not add continuous mini level meters beside A-D.

If an analysis request selects multiple monitors, all selected monitors should show the active state.

### 5.6 Master analysis indication

Master L/R already have live meters and do not require dedicated new LEDs in v1.

If inexpensive, the enlarged octopus/status presentation may react globally while any analysis is active, including Master-only analysis. This is optional and must not introduce a continuously expensive render path.

The existing READ/WRITE activity lights retain their HTTP read/write meaning and should not be overloaded as generic analysis-state indicators.

---

## 6. Observation History Architecture

### 6.1 Channel model

Introduce an internal observation-channel enum independent of the Rack input enum:

```cpp
enum class ObserveChannel : uint8_t {
    MasterL = 0,
    MasterR,
    A,
    B,
    C,
    D,
    Count
};
```

Provide stable string conversion:

```text
masterL
masterR
A
B
C
D
```

API parsing should be case-insensitive for A-D and should accept clear aliases such as `master_l` / `master_r` if convenient.

### 6.2 Shared frame publication

Replace the two independent logical ring heads with one shared frame timeline.

A frame represents all six monitor positions at the same Rack sample time.

A suitable conceptual slot is:

```cpp
struct ObservationSlot {
    std::atomic<float> volts[6];
    std::atomic<uint8_t> connectedMask;
    std::atomic<uint64_t> frameTag;
};
```

The exact representation may be optimized, but the implementation must guarantee that a consumer can detect whether a slot belongs to the requested Rack frame and must not knowingly return a torn/overwritten snapshot.

The existing code already uses atomic samples to avoid C++ data races between the audio and HTTP threads. Preserve equivalent memory-model safety in the new design.

### 6.3 History length

Increase history beyond the current 4096 frames so trigger processing has useful pre/post-roll headroom.

Recommended initial constant:

```cpp
static constexpr uint32_t OBSERVATION_HISTORY_FRAMES = 262144; // power of two
```

This yields approximately:

- 5.94 s at 44.1 kHz;
- 5.46 s at 48 kHz;
- 2.73 s at 96 kHz;
- 1.37 s at 192 kHz.

This is intentionally a moderate memory-for-latency tradeoff. If profiling shows the atomic representation makes this unnecessarily expensive in memory, the capacity may be adjusted while preserving the same architecture.

### 6.4 Disconnected monitors

Disconnected A-D inputs should not cause optional analysis work.

The history must still preserve a connection mask so consumers can distinguish:

- disconnected;
- connected but silent;
- insufficient history;
- expired snapshot.

It is acceptable to skip voltage stores for disconnected channels if the connection mask makes stale slot contents impossible to misinterpret.

### 6.5 Polyphony

For v1, each physical monitor jack observes Rack channel 0 only.

Report the physical port's channel count in monitor status metadata where useful, but do not silently fold or average polyphonic channels.

Future per-poly-channel observation may be added separately.

---

## 7. Minimal Always-On Master Meter

The two current live panel meters must remain responsive and visually similar.

However, the always-on Master meter should be reduced to only the work needed for live display and a small amount of immediate status.

### 7.1 Keep continuously

For Master L/R only:

- input capture into the shared observation history;
- BS.1770 K-weighting required for the live LUFS presentation;
- short rolling K-weighted block history;
- recent peak for dBFS presentation.

### 7.2 Remove from idle continuous accumulation

Do not continuously accumulate these solely for future analysis:

- integrated loudness since reset;
- long-term raw RMS sums;
- long-term stereo correlation sums;
- long-term clipping totals;
- hour-long R128 block history;
- long-term maximum peaks.

These move to triggered measurement sessions.

### 7.3 Live meter block history

The current UI only needs four 100 ms blocks for momentary LUFS.

Use a small power-of-two history, recommended:

```cpp
static constexpr int MASTER_METER_BLOCKS = 32;
```

Thirty-two 100 ms blocks also permit an immediate 3 s short-term value if useful without retaining an hour of state.

No mutex should be required for ordinary live meter display if atomic block publication can provide the necessary values.

---

## 8. Snapshot Model

A **snapshot** is an immutable, explicitly triggered observation window copied from rolling history for later analysis.

### 8.1 Snapshot definition

Each snapshot should record at minimum:

```cpp
struct SnapshotDescriptor {
    uint64_t id;
    uint64_t triggerFrame;
    uint32_t preFrames;
    uint32_t postFrames;
    uint8_t requestedMask;
    float sampleRate;
    std::string label;
};
```

A snapshot covers:

```text
[triggerFrame - preFrames, triggerFrame + postFrames]
```

### 8.2 Default interactive snapshot

For a request meaning “analyze what is happening now”:

- anchor to the latest fully published frame;
- default to a recent window ending at or near that frame;
- use 4096 analysis samples initially unless a specific analyzer requires more context.

### 8.3 Post-roll snapshots

If `postFrames > 0`, the snapshot request becomes pending until the requested end frame exists in rolling history.

The audio thread must **not** block waiting for post-roll.

The analysis/server side waits or polls the published frame counter and freezes the snapshot when complete.

### 8.4 Frozen snapshot storage

Use a bounded snapshot pool rather than retaining unbounded captures.

Recommended initial limit:

```text
8-16 snapshots
```

Eviction policy may be oldest-unpinned/oldest-completed.

The pool exists to support:

- delayed analysis;
- multiple comparisons from the same exact data;
- future Sibyl-triggered captures;
- reproducible machine evaluation.

### 8.5 Temporal identity

All analysis output must include enough timing metadata for the caller to know what was measured:

- snapshot ID;
- trigger frame;
- start frame;
- end frame;
- sample rate;
- duration;
- age when analysis began or completed where useful;
- connection mask/state.

---

## 9. Triggered Measurement Sessions

Some measurements describe an interval rather than a short frozen waveform.

Examples:

- integrated loudness;
- long-duration RMS;
- clipping count;
- stereo correlation over a defined program segment;
- A/B loudness comparison across multiple seconds.

Introduce a triggered **measurement session** distinct from a snapshot.

### 9.1 Session behavior

A measurement session has:

```text
selected monitor(s)
start frame
optional end frame or duration
measurement ID / label
state: pending | active | complete | cancelled
```

The extra per-sample work exists only while the session is active.

### 9.2 Master optimization

When a triggered measurement includes Master L/R, reuse the live Master K-weighted samples where practical rather than running duplicate K-weighting filters.

A-D only need K-weighting while a measurement requests it.

### 9.3 Concurrency

Initial implementation should permit only one heavyweight loudness/interval measurement session at a time unless profiling demonstrates that multiple sessions are harmless.

Reject, queue, or coalesce conflicting requests explicitly; do not silently run multiple expensive measurement paths.

---

## 10. Analysis Engine

### 10.1 Threading rule

Detailed analysis must never run on Rack's audio thread.

Heavy work should execute on a dedicated Octavia analysis worker or otherwise on a non-audio thread with serialization.

### 10.2 Basic per-channel analysis

A frozen channel snapshot should support:

- RMS;
- peak;
- crest factor;
- clipping count / threshold crossings;
- DC offset;
- connection state;
- sample window metadata.

### 10.3 Detailed spectral analysis

Preserve or improve the useful information already available from `/audio/{port}/analyze`:

- broad spectral-band levels;
- noise floor estimate;
- hum detection;
- stable resonance candidates;
- feedback suspicion;
- artifact/problem flags;
- optional raw spectrum.

### 10.4 FFT migration

Do **not** scale the existing repeated full-band Goertzel scan directly across six channels as the long-term architecture.

Create an analyzer abstraction and migrate broad spectral analysis toward a shared FFT approach, preferably using Rack's available DSP FFT facilities when compatible with the target SDK.

Preferred hybrid model:

```text
FFT
  -> broad spectrum
  -> spectral bands
  -> resonance candidates
  -> spectral deltas

Targeted Goertzel
  -> precise 50/60 Hz hum probes
  -> harmonics
  -> selected frequency refinement
```

If FFT integration creates build risk during the first implementation stage, the existing Goertzel analyzer may remain temporarily behind the new interface, but this must not become an excuse to run detailed analysis continuously.

### 10.5 Temporal stability without sleep-based capture

The existing detailed analyzer takes two snapshots approximately 120 ms apart.

The new snapshot history should allow both analysis windows to be selected from the same frame-indexed captured history rather than relying on a server-thread sleep between independent snapshots.

This improves temporal reproducibility and makes sequence-triggered analysis deterministic.

---

## 11. Grouping and Comparison Semantics

A-D are physically independent. The API must allow the caller to assign interpretation per request.

### 11.1 Valid examples

```text
analyze A
analyze C
compare A B
stereo(A, B)
stereo(C, D)
compare stereo(A, B) stereo(C, D)
analyze MASTER
compare A MASTER_L
```

### 11.2 Stereo groups

A stereo group is simply two selected mono monitors assigned roles:

```text
left = A
right = B
```

Stereo analysis may report:

- balance;
- correlation;
- mid/side energy;
- side-to-mid ratio;
- pair RMS/peak/loudness where meaningful.

### 11.3 Comparison response

Do not force the agent to subtract two unrelated result blobs.

A comparison should provide:

- absolute metrics for source/group 1;
- absolute metrics for source/group 2;
- explicit delta values (`target - reference`) where meaningful.

Useful deltas include:

- RMS dB;
- peak dB;
- crest factor;
- loudness LU/dB;
- spectral-band dB;
- DC offset;
- stereo width/balance/correlation changes.

### 11.4 Level-normalized spectral delta

Detailed comparison should support a level-normalized spectral difference so the agent can distinguish:

```text
B is simply louder than A
```

from:

```text
B has materially different tonal/harmonic balance after level compensation
```

This may be implemented after the initial snapshot path but belongs in the target analyzer contract.

### 11.5 Latency estimation

Cross-correlation-based A→B latency estimation is a desirable advanced feature for before/after chains containing oversampling, lookahead, or other delay.

Treat this as a later analysis enhancement, not a blocker for the first monitoring release.

---

## 12. External HTTP/API Contract

Preserve current routes where practical while introducing named monitor semantics.

### 12.1 Monitor discovery

Add:

```text
GET /audio/monitors
```

Example response shape:

```json
{
  "sampleRate": 48000,
  "publishedFrame": 123456789,
  "monitors": [
    {"id":"masterL","connected":true,"channels":1,"liveMeter":true},
    {"id":"masterR","connected":true,"channels":1,"liveMeter":true},
    {"id":"A","connected":true,"channels":1,"liveMeter":false},
    {"id":"B","connected":false,"channels":0,"liveMeter":false},
    {"id":"C","connected":false,"channels":0,"liveMeter":false},
    {"id":"D","connected":false,"channels":0,"liveMeter":false}
  ]
}
```

### 12.2 Snapshot endpoint

Add a snapshot operation using a POST body rather than encoding the entire request in a path.

Suggested route:

```text
POST /audio/snapshot
```

Suggested request:

```json
{
  "monitors": ["A", "B"],
  "preMs": 100,
  "postMs": 250,
  "label": "filter-test-1"
}
```

Return a stable snapshot ID and timing metadata.

If the route is implemented synchronously, it may wait on the server/worker thread for post-roll, but never on the audio thread. A later asynchronous `202 + jobId` contract is acceptable if needed.

### 12.3 Analyze snapshot

Suggested route:

```text
POST /audio/analyze
```

Request may either reference an existing snapshot or request a convenience “latest” capture.

Example:

```json
{
  "snapshotId": 42,
  "channels": ["A"],
  "detail": "detailed",
  "includeSpectrum": false
}
```

### 12.4 Compare

Suggested route:

```text
POST /audio/compare
```

Examples:

```json
{
  "snapshotId": 42,
  "reference": {"channels":["A"]},
  "target": {"channels":["B"]}
}
```

or:

```json
{
  "snapshotId": 43,
  "reference": {"stereo":{"left":"A","right":"B"}},
  "target": {"stereo":{"left":"C","right":"D"}}
}
```

### 12.5 Triggered measurement

Provide an interval-measurement operation suitable for loudness and other long-window metrics.

A simple first contract may remain synchronous for duration-based measurements:

```text
POST /audio/measure
```

with a body such as:

```json
{
  "channels": ["masterL", "masterR"],
  "seconds": 10,
  "metrics": ["loudness", "rms", "peak", "correlation"]
}
```

Internally, start and end the measurement on engine-frame boundaries rather than relying solely on `sleep_for()` timing.

---

## 13. Legacy Route Compatibility

Existing clients should not break unnecessarily.

### 13.1 `/audio/0` and `/audio/1`

Continue to interpret:

```text
0 = Master L
1 = Master R
```

Route these requests through the new snapshot/analyzer substrate.

### 13.2 `/audio/{port}/analyze`

Continue supporting port 0/1 as Master L/R.

The response may gain timing metadata but should preserve currently useful fields where practical.

### 13.3 `/audio/loudness/reset`

Change its internal meaning from “clear an always-running hour-long accumulator” to:

> arm/reset a triggered Master L/R loudness measurement session.

This preserves the old reset → play → read workflow without paying the full measurement cost at all times.

### 13.4 `/audio/loudness`

Return the state/results of the current or most recently completed legacy Master measurement session.

If no session has been armed, return an explicit state/error rather than pretending an idle continuous integrated measurement exists.

### 13.5 `/audio/measure?seconds=N`

Keep the convenience endpoint if current MCP/tooling depends on it, but implement it using the new triggered measurement session rather than reset + server wall-clock sleep as the authoritative timing mechanism.

---

## 14. Analysis Worker and Request Serialization

Introduce a bounded heavyweight analysis path.

Recommended initial design:

```text
HTTP / internal trigger
        |
        v
 snapshot freeze / measurement control
        |
        v
 bounded analysis queue
        |
        v
 one Octavia analysis worker
        |
        v
 result / snapshot cache
```

Requirements:

- at most one detailed FFT/Goertzel analysis executing at a time initially;
- bounded queue size, recommended 8;
- reject or coalesce excess duplicate “latest” requests rather than allowing unbounded work;
- no locks that can block the audio thread;
- snapshot memory is bounded;
- analysis cancellation is desirable but not required for the first implementation.

Basic metadata/status requests must remain cheap even while detailed analysis is busy.

---

## 15. Monitor Activity State

Each A-D monitor needs machine-readable activity state to drive LEDs and API status.

A conceptual state object:

```cpp
struct MonitorActivity {
    std::atomic<bool> connected{false};
    std::atomic<uint32_t> activeAnalysisUsers{0};
    std::atomic<uint64_t> snapshotGeneration{0};
};
```

Exact implementation may differ.

Rules:

- `connected` follows physical cable state;
- selected monitors increment/mark analysis activity while a detailed job is active;
- snapshot creation increments a generation counter used to create a short LED flash;
- LED envelopes may follow the same generation/envelope pattern already used by Octavia's READ/WRITE activity lights;
- do not make server threads directly mutate Rack light objects.

---

## 16. Sibyl Integration Contract

Sibyl integration is part of the target design, but should be layered so the initial monitoring refactor does not depend on completing the entire sequencer feature.

### 16.1 Required architectural hook

Provide a non-HTTP internal mechanism capable of expressing:

```cpp
struct ObservationTrigger {
    int64_t octaviaModuleId;
    uint64_t triggerFrame;
    uint32_t preFrames;
    uint32_t postFrames;
    uint8_t monitorMask;
    char label[...] or stable label/reference;
};
```

Do not require Sibyl to send a physical trigger cable merely to request a machine observation.

### 16.2 Exact frame capture

A future Sibyl sequence event should be able to say, conceptually:

```text
snapshot monitors A,B,C,D at Rack frame N
with X frames of pre-roll and Y frames of post-roll
```

Because Octavia's observation history uses the same Rack-frame timeline, the resulting snapshot can capture the event's consequence exactly even if the analysis itself occurs later.

### 16.3 Thread safety

Do not have Sibyl hold a raw mutable pointer to Octavia and call analysis functions from its audio thread.

Use a bounded lock-free or atomic publication mechanism suitable for cross-module control.

### 16.4 Multiple Octavia modules

Any future Sibyl trigger must resolve a specific Octavia module ID.

Do not assume that there can only ever be one Octavia module in a patch, even if the current HTTP server configuration usually encourages a single active server.

### 16.5 Scope for this implementation

For the first monitoring/panel refactor it is sufficient to:

- define the trigger structure/interface;
- ensure the history is frame-addressable;
- provide a safe insertion point for future Sibyl publication.

Full composition-schema support for sequenced analysis markers may be implemented as a follow-up phase.

---

## 17. No Physical SNAP Jack in This Refactor

Do not add a dedicated physical snapshot/gate input as part of this panel refactor.

Sibyl and AI-triggered analysis can use software/internal control.

A future physical SNAP jack could be valuable because it would let arbitrary Rack clocks/sequencers mark observations, but that is a separate user-facing feature and should not consume panel space until its UX is deliberately designed.

---

## 18. Performance Requirements

Performance is a first-class acceptance criterion.

### 18.1 Idle audio-thread work

With no active detailed analysis or interval measurement:

- Master L/R: shared rolling capture + minimal live meter DSP only;
- A-D connected: rolling capture only;
- A-D disconnected: connection bookkeeping only, with unnecessary sample/analysis work skipped;
- no FFT;
- no Goertzel;
- no heap allocation;
- no blocking mutex acquisition;
- no server-thread waits in `process()`.

### 18.2 Analysis-time work

Expensive operations may run only on non-audio threads and should be serialized initially.

### 18.3 Benchmark matrix

Profile before and after at minimum:

```text
44.1 kHz
48 kHz
96 kHz
192 kHz if the local test environment supports it
```

Test cases:

1. Octavia loaded, no monitor cables.
2. Master L/R connected, live meters active.
3. Master + A-D all connected, no analysis.
4. Detailed analysis of one monitor.
5. Detailed analysis of four monitors.
6. Stereo comparison A+B versus C+D.
7. Active 10-second loudness measurement.

Record Rack engine CPU impact or an equivalent reproducible microbenchmark.

### 18.4 Acceptance target

The refactor must not create an obvious idle Rack-performance regression.

Prefer an explicit measurable target during implementation; as a practical guideline, all-six-channel idle monitoring should remain small relative to a typical DSP module and substantially cheaper than continuously running the detailed analyzer.

If the six-channel atomic ring becomes a measurable bottleneck, optimize the storage representation while preserving coherent frame-addressable snapshots and C++ memory safety.

---

## 19. Error and State Handling

Analysis APIs must distinguish these states explicitly:

```text
disconnected
connected_silent
insufficient_history
snapshot_expired
pending_postroll
analysis_queued
analysis_active
complete
cancelled
engine_not_running
```

Do not return stale disconnected-buffer data as if it were valid audio.

When a multi-monitor snapshot contains a disconnected requested monitor, preserve the snapshot for connected monitors but identify the unavailable monitor clearly unless the caller requested strict all-or-nothing behavior.

---

## 20. Suggested Internal File Refactor

`Octavia.cpp` is already large. This feature is a good point to keep the new audio observation subsystem isolated.

Recommended decomposition:

```text
src/Octavia.cpp
src/OctaviaObservation.hpp
src/OctaviaObservation.cpp
src/OctaviaAnalysis.hpp
src/OctaviaAnalysis.cpp
src/OctaviaMeasurement.hpp
src/OctaviaMeasurement.cpp
src/OctaviaObservationBus.hpp        // future Sibyl/internal trigger interface
```

Exact file names are flexible, but do not add another large monolithic block of analyzer implementation to `Octavia.cpp` if it can be cleanly separated.

Possible responsibilities:

### `OctaviaObservation`

- six-channel rolling history;
- frame publication;
- connection mask;
- snapshot freezing;
- snapshot metadata/pool.

### `OctaviaAnalysis`

- basic waveform metrics;
- FFT/Goertzel spectral backend;
- resonance/problem detection;
- group/stereo analysis;
- comparison/delta generation.

### `OctaviaMeasurement`

- triggered long-window accumulators;
- loudness sessions;
- interval timing and results.

### `OctaviaObservationBus`

- bounded internal trigger publication;
- module-ID targeting;
- no Rack UI/server dependency.

---

## 21. Testing Requirements

Add targeted tests where the project's test infrastructure permits.

### 21.1 Compatibility

- Existing patches keep cables connected to input IDs 0 and 1.
- Existing Master routes still address ports 0 and 1.
- Existing READ/WRITE/STATUS light IDs do not move.

### 21.2 Rolling history

- all six connected inputs record the correct frame-aligned values;
- disconnected monitor state is preserved;
- ring wrap does not return frames with the wrong tag;
- expired snapshots fail cleanly;
- sample-rate changes update timing metadata correctly.

### 21.3 Snapshot timing

- pre-roll starts at the expected frame;
- post-roll ends at the expected frame;
- multiple monitors in one snapshot have the same start/end frame;
- a pending post-roll request never blocks `process()`.

### 21.4 Analysis

Synthetic fixtures should cover:

- silence;
- DC offset;
- sine tones at known frequencies;
- 50 Hz hum;
- 60 Hz hum;
- clipping;
- mono duplicated to stereo;
- inverted stereo;
- known L/R imbalance;
- known delay between comparison channels;
- broad-band noise;
- before/after gain-only change;
- before/after tonal change.

### 21.5 LED states

- disconnected = off;
- connected = dim;
- snapshot generation produces a visible flash envelope;
- selected monitor(s) show active analysis state;
- activity state returns to connected-idle after the job finishes.

### 21.6 Master meter regression

- Master LUFS meter remains responsive;
- Master dBFS meter remains responsive;
- disconnect behavior remains sensible;
- meter smoothing remains visually stable;
- removing long-term accumulation does not change the intended live visual semantics.

---

## 22. Implementation Sequence

### Phase 1 — Panel and compatibility skeleton

1. Widen `res/Octavia.svg`.
2. Enlarge the octopus/status region.
3. Rename existing panel semantics to Master L/R.
4. Append A-D input IDs.
5. Append A-D light IDs.
6. Add SVG anchors and physical jacks/lights.
7. Preserve existing patch IDs.

**Exit condition:** panel loads, old patches retain Master cables, A-D jacks and status lights exist.

### Phase 2 — Shared observation substrate

1. Replace the two independent logical rings with one six-channel frame-addressed observation history.
2. Add connected mask and published frame metadata.
3. Add monitor status API.
4. Add bounded snapshot pool.
5. Add latest-frame and pre/post-roll snapshots.

**Exit condition:** A-D can be captured cheaply and multiple channels from one snapshot are sample-aligned.

### Phase 3 — Master meter slimming

1. Reduce continuous Master DSP to live K-weighted meter + peak only.
2. Replace one-hour block history with small live meter history.
3. Move long-window accumulation into triggered measurement state.
4. Preserve meter appearance/behavior.

**Exit condition:** Master meters remain live while idle continuous analysis cost is reduced.

### Phase 4 — New analysis and comparison API

1. Move detailed analysis behind the snapshot service.
2. Add named channel parsing.
3. Add stereo grouping.
4. Add A/B and arbitrary comparison deltas.
5. Serialize expensive analysis.
6. Migrate broad spectral work toward FFT.
7. Preserve targeted Goertzel where useful.

**Exit condition:** an agent can explicitly compare arbitrary patch observation points without continuous detailed DSP.

### Phase 5 — Legacy route migration

1. Route `/audio/0`, `/audio/1`, and detailed legacy Master analysis through the new backend.
2. Reinterpret loudness reset/read as triggered measurement state.
3. Replace wall-clock authoritative measurement timing with frame-based sessions.
4. Validate MCP/tool compatibility.

**Exit condition:** current clients continue to function while the internal architecture is trigger-driven.

### Phase 6 — Sibyl observation hook

1. Add internal `ObservationTrigger` publication.
2. Target Octavia by module ID.
3. Capture exact trigger-frame snapshots with pre/post-roll.
4. Later extend Sibyl's composition/event schema with explicit analysis markers.

**Exit condition:** Sibyl can intentionally schedule an event and Octavia can later analyze exactly the corresponding monitored result.

---

## 23. Acceptance Scenarios

The feature is successful when the following workflows are natural and reliable.

### Scenario A — Simple before/after

```text
A = signal before filter
B = signal after filter
```

Agent changes cutoff, triggers one synchronized A+B snapshot, and receives explicit B-minus-A measurement deltas.

### Scenario B — Stereo before/after

```text
A = pre L
B = pre R
C = post L
D = post R
```

Agent requests:

```text
compare stereo(A,B) against stereo(C,D)
```

and receives tonal, level, and stereo-field changes from one matched observation window.

### Scenario C — Local edit versus Master

```text
A = source stem
B = processed stem
MASTER L/R = final patch mix
```

Agent can determine both:

1. whether the local processing changed A→B as intended;
2. whether the resulting Master mix remains acceptable.

### Scenario D — Triggered loudness check

Master meters remain live during ordinary patching with only lightweight DSP.

Agent explicitly requests a 10-second Master loudness measurement. Long-window accumulation runs only for that interval and then returns integrated results.

### Scenario E — Future Sibyl experiment

Sibyl schedules an edit/event and observation marker at engine frame `N`.

Octavia freezes A-D and/or Master around exactly frame `N`, including requested pre/post-roll. Detailed analysis happens later without threatening the event timing.

---

## 24. Non-Goals for This Refactor

Do not expand scope into the following unless implementation uncovers a hard dependency:

- hidden cableless arbitrary module-output probing;
- per-monitor continuous spectrograms;
- four additional live level meters;
- polyphonic per-channel analysis;
- a physical SNAP trigger jack;
- subjective audio-LLM listening inside the plugin;
- autonomous parameter search logic itself;
- multiple simultaneous heavyweight FFT workers;
- unbounded recording or disk capture.

Octavia provides measurement and observation. Higher-level agents decide what to change and why.

---

## 25. Final Architectural Summary

The intended system is:

```text
                         OCTAVIA

        MASTER L/R                     A   B   C   D
            |                          |   |   |   |
            +------------+-------------+---+---+---+
                         |
                  shared frame-indexed
                  rolling observation
                       history
                         |
            +------------+-------------+
            |                          |
    minimal live Master          explicit trigger
     LUFS + peak meter          / snapshot / session
            |                          |
        panel meters              frozen capture
                                       |
                                analysis worker
                                       |
                         +-------------+-------------+
                         |                           |
                    single/group                 compare
                      analysis                 deltas A→B
                         |                           |
                         +-------------+-------------+
                                       |
                                  AI / MCP / Sibyl
```

The key invariant is:

> **Octavia may listen continuously, but it should think expensively only when something asks it to pay attention.**

That architecture preserves a responsive living module for the human while providing a precise, low-overhead sensory substrate for machine-guided patch tuning.
