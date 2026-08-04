# Reliquary — SPC/RSN Performance Extraction Module

**Project:** Leviathan-Rack2  
**Working title:** Reliquary  
**Document status:** Implementation specification, Draft 0.1  
**Date:** 2026-08-04

---

## 1. Purpose

Reliquary is a VCV Rack module that loads Super Nintendo SPC snapshots and RSN soundtrack collections, resumes the captured SPC700/S-DSP program, and exposes the resulting performance as modular control signals.

The module is not merely an SPC audio player. Its defining feature is a translation layer that converts the S-DSP's temporary eight-voice hardware allocation into stable, user-visible logical instrument lanes. Those lanes provide pitch, gate, trigger, envelope, and level signals that remain coherent even when the original sound driver moves an instrument between physical DSP voices.

The working metaphor is an illuminated archive of preserved machine performances: Reliquary opens the snapshot, restores the original sound program, and reveals the score beneath its shifting hardware state.

---

## 2. Primary goals

Reliquary shall:

1. Load standalone `.spc` files.
2. Load `.rsn` collections containing multiple SPC tracks.
3. Play the authentic emulated stereo output.
4. Observe note, pitch, envelope, sample-source, noise, and voice-allocation activity directly from the emulated system.
5. Map temporary S-DSP hardware voices onto stable logical Rack lanes.
6. Expose logical lanes through fixed-size polyphonic CV outputs.
7. Display the current logical-to-physical mapping clearly on the module.
8. Keep all file I/O, archive extraction, indexing, and expensive analysis outside the audio thread.
9. Install as part of the ordinary Leviathan plugin package with no required external executable or runtime library.
10. Remain architecturally independent of any one SPC emulator or archive decoder.

---

## 3. Non-goals for the first release

The first release shall not attempt to:

- Convert an SPC performance into MIDI files.
- Recover the composer's original sequence data or tracker project.
- Guarantee correct semantic instrument names such as “bass,” “snare,” or “strings.”
- Guarantee absolute concert-pitch output for every BRR sample automatically.
- Edit SPC RAM, driver code, BRR samples, or DSP programs.
- Create or modify RAR/RSN archives.
- Provide arbitrary seeking with sample-perfect reconstruction.
- Expose every album instrument simultaneously when a collection contains more logical identities than the available Rack lanes.
- Reimplement the full SPC700/S-DSP emulator before the musical routing concept is proven.

These capabilities may be explored later without changing the core interfaces defined here.

---

## 4. Terminology

### 4.1 SPC snapshot

A captured SPC700/S-DSP machine state containing SPC RAM, CPU state, DSP registers, and optional metadata. Resuming the state allows the original sound driver to continue running.

### 4.2 RSN collection

A RAR archive conventionally containing a game's or soundtrack's set of SPC snapshots.

### 4.3 Physical voice

One of the eight S-DSP voice slots. Physical voices are temporary playback resources and are not assumed to correspond to permanent instruments or musical tracks.

### 4.4 Timbre identity

An identity derived primarily from the underlying BRR sample content and loop information.

### 4.5 Articulation identity

A timbre identity qualified by envelope or performance configuration when the same sample is used in musically distinct ways.

### 4.6 Logical instrument

A user-facing identity representing a stable sound or articulation independent of the physical S-DSP voice currently producing it.

### 4.7 Logical lane

A stable Rack polyphonic channel assigned to one logical instrument instance. Multiple sibling lanes may be reserved for simultaneous notes produced by the same logical instrument.

### 4.8 Mapping

The current relationship among logical lane, logical instrument, BRR identity, source number, and active physical S-DSP voice.

---

## 5. User experience

### 5.1 Basic workflow

1. The user loads an `.spc` or `.rsn` file.
2. Reliquary validates and prepares the source on a worker thread.
3. For an RSN, the module presents a track list and selects the first valid track or restores the serialized selection.
4. Playback begins when requested.
5. The display shows logical lanes and the current hardware voice feeding each active lane.
6. Polyphonic outputs provide pitch, gate, trigger, envelope, and level data.
7. The user patches these outputs into oscillators, envelopes, samplers, sequencers, or other Rack modules.
8. The original stereo SPC output remains available as an audible reference or production source.

### 5.2 Mapping behavior

The user shall see stable lane identities during a track. For example:

```text
CV 01  Instrument 01 · 1    DSP 6    active
CV 02  Instrument 01 · 2    —        idle
CV 03  Instrument 02 · 1    DSP 2    active
CV 04  Percussion 01        DSP 7    trigger
```

If the SPC sound driver later moves Instrument 01 from DSP voice 6 to DSP voice 3, CV lane 1 remains Instrument 01. Only the temporary `DSP` field changes.

### 5.3 Track changes

Mappings may differ between tracks unless the user pins an identity or enables collection-aware mapping. A `TRACK CHANGE` trigger shall be emitted whenever the selected track changes so downstream patches can reset or react.

Reliquary shall favor the same lane for the same fingerprint across tracks when practical, but the first release does not guarantee a globally unique lane for every instrument in a large collection.

---

## 6. Panel and I/O specification

Panel width is provisional. The design should target approximately 16–20 HP, subject to a visual mockup.

### 6.1 Audio outputs

- **L** — emulated left audio output.
- **R** — emulated right audio output.

Audio outputs shall be Rack-standard bipolar audio voltage and shall preserve the emulator's stereo mix.

### 6.2 Polyphonic CV outputs

Each output shall expose a fixed 16-channel polyphonic cable while a source is loaded:

- **PITCH** — logarithmic pitch CV.
- **GATE** — note-active state according to the selected gate policy.
- **TRIG** — short pulse on logical note-on events.
- **ENV** — emulated voice envelope normalized to 0–10 V.
- **LEVEL** — estimated post-envelope voice level normalized to 0–10 V.

Unused lanes shall output 0 V.

### 6.3 Event outputs

- **TRACK** — pulse on successful track transition.
- **END** — pulse when the current track reaches its configured end condition.
- **ERROR** — optional pulse when a load or runtime failure occurs after a valid source was previously active.

### 6.4 Inputs

- **RUN** — gate-controlled transport. High runs; low pauses. If unpatched, the panel run state applies.
- **RESET** — trigger to restart the current SPC snapshot from its initial state.
- **NEXT** — trigger to load the next valid track.
- **PREV** — trigger to load the previous valid track.
- **TRACK CV** — selects a track according to the configured CV mode.
- **SPEED CV** — optional playback-speed modulation, applied around the panel speed setting.

### 6.5 Controls

- Load source button.
- Run/pause button.
- Restart button.
- Previous and next track buttons.
- Track selector encoder or stepped knob.
- Speed control.
- Global transpose control.
- Mapping view button.
- Settings button or context-menu entries for advanced policies.

### 6.6 Track CV modes

At minimum:

- **Indexed 0–10 V:** the voltage range maps across all valid tracks.
- **Semitone:** each 1/12 V step advances one track.
- **Trigger-only:** selection is controlled by NEXT/PREV and panel controls; TRACK CV is ignored.

Indexed selection shall use hysteresis to prevent noisy CV from repeatedly crossing adjacent track boundaries.

---

## 7. Display specification

The display is a central functional element, not decorative metadata.

### 7.1 Playback view

Shows:

- Collection or file name.
- Current track index and total track count.
- SPC title, game, artist, and duration when metadata is available.
- Playback position.
- Play, pause, indexing, loading, and error state.
- Compact live lane activity.

### 7.2 Mapping view

A scrollable table with columns similar to:

```text
CV   NAME                 ID        DSP   STATE
01   Instrument 01 · 1    A38C91    6     G3
02   Instrument 01 · 2    A38C91    —     —
03   Instrument 02        72F0E4    2     +0.42V
04   Percussion 01        19DA02    7     TRIG
```

Required information:

- Stable CV lane number.
- User label or generated label.
- Short sample/articulation fingerprint.
- Current physical DSP voice or idle marker.
- Current state, relative pitch, or note label when calibrated.

### 7.3 Hardware diagnostic view

Shows the eight physical S-DSP voices and their current destination lanes:

```text
DSP 0 → CV 05  Instrument 03 · 1
DSP 1 → CV 08  Instrument 04
DSP 2 → CV 03  Instrument 02
```

This view is diagnostic and shall not be the default user abstraction.

### 7.4 Mapping editor

The first release should support at least:

- Rename logical instrument.
- Move or swap lane assignment.
- Pin assignment.
- Mute or solo logical instrument.
- Mark identity as tonal, percussion, noise, or effect.
- Set pitch-root calibration.
- Reset mapping for current track.

Later versions may add merge, split, collection-wide propagation, and learn-next-event operations.

---

## 8. Source loading model

All sources shall normalize to one collection representation:

```cpp
struct SpcTrack {
    std::string archiveName;
    std::vector<uint8_t> spcBytes;
    SpcMetadata metadata;
    double declaredDurationSeconds = 0.0;
    double declaredFadeSeconds = 0.0;
    uint64_t contentHash = 0;
};

struct SpcCollection {
    std::string sourcePath;
    std::string displayName;
    uint64_t sourceHash = 0;
    std::vector<SpcTrack> tracks;
};
```

A standalone SPC becomes a one-track collection.

### 8.1 Loading states

- Empty.
- Loading source.
- Extracting collection.
- Validating tracks.
- Preparing runtime.
- Ready.
- Error.

The module shall continue outputting the prior valid source while a replacement source is prepared, unless the user explicitly clears it.

### 8.2 RSN extraction

RSN extraction shall be delegated to the internal archive-reader contract described in the separate UnRAR specification.

The loader shall:

- Process solid archives sequentially.
- Extract candidate entries to memory.
- Validate SPC content by signature and structure, not filename alone.
- Ignore unrelated entries.
- Preserve archive ordering unless track metadata or a user setting specifies another order.
- Avoid temporary files.

### 8.3 Memory policy

The default RSN path shall retain validated SPC snapshots in memory after extraction. SPC snapshots are small enough that this provides instant subsequent track initialization and avoids repeatedly decoding a solid archive.

Configurable hard limits shall prevent hostile or malformed archives from causing excessive allocation.

---

## 9. SPC runtime abstraction

Reliquary shall not couple its mapping engine directly to one emulator implementation.

```cpp
class ISpcRuntime {
public:
    virtual ~ISpcRuntime() = default;

    virtual bool load(std::span<const uint8_t> spcBytes,
                      SpcRuntimeError& error) = 0;
    virtual void reset() = 0;
    virtual void setTempo(double multiplier) = 0;

    virtual void renderNative(int16_t* interleavedStereo,
                              size_t nativeFrames) = 0;

    virtual std::array<PhysicalVoiceSnapshot, 8>
        voiceSnapshots() const = 0;

    virtual std::span<const uint8_t> audioRam() const = 0;
    virtual uint8_t dspRegister(uint8_t address) const = 0;

    virtual void setEventSink(ISpcEventSink* sink) = 0;
};
```

### 9.1 Initial implementation strategy

The initial runtime should adapt a mature existing SPC core rather than begin with a new emulator. The core must provide or be modified to provide:

- SPC snapshot loading.
- SPC700 execution.
- S-DSP audio rendering.
- Read-only audio RAM access.
- Physical voice-state snapshots.
- Timestamped observation of relevant DSP register writes or equivalent effective events.

The exact runtime dependency remains a pre-implementation gate because technical fit, modification effort, license obligations, and single-package distribution must all be resolved. The interface above shall remain stable regardless of the selected core.

### 9.2 Runtime event stream

The event observer shall report at least:

```cpp
enum class SpcEventType {
    DspRegisterWrite,
    KeyOn,
    KeyOff,
    SampleEnd,
    VoiceStateBoundary
};

struct SpcEvent {
    uint64_t nativeClock;
    SpcEventType type;
    uint8_t physicalVoice;
    uint8_t address;
    uint8_t value;
};
```

Reliquary must observe:

- KON and KOFF.
- Voice pitch low/high registers.
- SRCN.
- ADSR1, ADSR2, and GAIN.
- PMON and NON.
- DIR.
- ENDX.
- Effective ENVX and OUTX state.

The emulator has eight voices and directly exposes key-on, key-off, pitch, sample-source, envelope, noise, and modulation state through the S-DSP register model. This makes direct event extraction preferable to mixed-audio pitch detection.

---

## 10. Instrument identity

### 10.1 Sample resolution

At a logical note-on event, Reliquary shall resolve the physical voice's `SRCN` through the current DSP directory and audio RAM to identify the BRR sample start and loop address.

### 10.2 Timbre fingerprint

The base identity shall hash:

- Reachable BRR blocks for the sample.
- BRR end and loop flags.
- Loop offset relative to the sample start.
- A version tag for the fingerprint algorithm.

The fingerprint must not depend on:

- Physical voice number.
- SRCN value alone.
- Absolute RAM address.
- Pan or channel volume.

### 10.3 Articulation fingerprint

Balanced identity mode shall optionally distinguish materially different uses of the same BRR sample by including a normalized representation of:

- ADSR versus GAIN mode.
- Attack, decay, sustain, and release configuration.
- Direct or shaped gain mode.
- Noise-enabled state.

Small or rapidly changing envelope differences should not create uncontrolled identity proliferation.

### 10.4 Identity modes

- **Loose:** BRR timbre only.
- **Balanced:** BRR timbre plus meaningful articulation differences. Default.
- **Strict:** BRR timbre plus detailed articulation and tuning cluster.

### 10.5 Generated labels

Safe generated labels shall be deterministic:

- `Instrument 01`
- `Instrument 02`
- `Percussion 01`
- `Noise 01`
- `Effect 01`

Heuristic semantic labels may be added later, but uncertain classifications must not be presented as fact.

---

## 11. Logical lane routing

### 11.1 Physical voice state

```cpp
struct PhysicalVoiceState {
    bool keyed = false;
    bool releasing = false;
    uint8_t dspVoice = 0;
    uint8_t srcn = 0;
    uint16_t pitch = 0;
    uint8_t envelope = 0;
    uint8_t output = 0;
    uint8_t adsr1 = 0;
    uint8_t adsr2 = 0;
    uint8_t gain = 0;
    bool noise = false;
    bool pitchModulated = false;

    InstrumentId instrument;
    int logicalLane = -1;
};
```

### 11.2 Logical lane state

```cpp
struct LogicalLaneState {
    InstrumentId instrument;
    uint8_t siblingIndex = 0;
    bool pinned = false;
    bool gate = false;
    bool releasing = false;
    float pitchCv = 0.f;
    float envelopeCv = 0.f;
    float levelCv = 0.f;
    int physicalVoice = -1;
    uint64_t lastActivityFrame = 0;
};
```

### 11.3 Note-on routing

On each logical note-on:

1. Resolve the current physical voice's sample and articulation identity.
2. Find all lanes reserved for that identity.
3. Select the lowest-numbered free sibling lane.
4. If no sibling is free and an unassigned lane remains, create a new sibling reservation.
5. Bind the physical voice to the selected lane until release completion, sample end, or a new note replaces the voice.
6. Emit a trigger on that lane.
7. Raise the gate according to the configured gate policy.

### 11.4 Lane stability

A lane assigned to an identity shall not be silently reused for another identity during the same track.

When the user pins a lane, the assignment shall persist across track reloads and, when the fingerprint is present, across tracks in the same collection.

### 11.5 Overflow

If a track requires more than 16 stable logical sibling lanes:

- Pinned assignments take priority.
- Active identities with the highest configured priority take remaining lanes.
- Unexposed events remain visible in the mapping display as overflow but do not mutate existing lane meanings.
- An overflow indicator shall appear.

A future expander may expose additional lane banks. Dynamic lane reassignment during active playback is explicitly forbidden in the default mode.

---

## 12. Pitch output

### 12.1 Relative pitch mode

Relative pitch is the first-release default and shall represent the logarithmic ratio of the DSP pitch register to the native sample rate reference:

```text
relativePitchCv = log2(dspPitch / 4096)
```

This preserves intervals, vibrato, portamento, arpeggiation, and pitch bends but does not guarantee that 0 V corresponds to a standard musical note.

### 12.2 Calibrated pitch mode

The user may assign a root pitch to an instrument fingerprint. Reliquary then offsets the relative CV to produce a conventional 1 V/oct signal.

Calibration shall be stored by fingerprint and collection identity rather than by physical voice or SRCN alone.

### 12.3 Quantized mode

Optional semitone quantization may be applied after calibration or relative-pitch conversion.

### 12.4 Noise voices

Noise-enabled voices shall be marked unpitched by default. Their PITCH output may either:

- Hold 0 V, or
- Emit a normalized noise-rate CV when that advanced option is enabled.

### 12.5 Pitch modulation

The initial release may expose base register pitch only. An effective-pitch mode that includes S-DSP pitch modulation is desirable but shall not delay the MVP unless the selected runtime already makes it inexpensive and reliable.

---

## 13. Gate, trigger, envelope, and level semantics

### 13.1 Trigger

TRIG shall pulse on every logical note-on, even if the lane was already active due to a retrigger. Default width: 1 ms, configurable within a conservative range.

### 13.2 Gate modes

- **Key gate:** high from KON until KOFF or voice replacement.
- **Audible gate:** high from KON until envelope reaches silence, sample ends, or the voice is replaced. Default.
- **Trigger gate:** fixed-duration pulse only.

### 13.3 Envelope

ENV shall scale the effective DSP envelope to 0–10 V and should update at audio rate or at a sufficiently high control rate to preserve the original contour.

### 13.4 Level

LEVEL shall estimate audible contribution from envelope, current sample output, and voice volume. It is diagnostic/modulation CV and is not required to reconstruct exact isolated audio amplitude.

---

## 14. Audio and timing

### 14.1 Native rendering

The selected runtime shall render at its native SPC output rate. Reliquary shall resample to the Rack engine rate using a deterministic, allocation-free resampler.

### 14.2 Event alignment

DSP events shall be timestamped in native runtime time. Reliquary shall translate them to Rack sample positions within the current processing block.

For the MVP, alignment within one Rack sample is the target. More exact internal DSP-cycle presentation is optional unless required to avoid musically observable errors.

### 14.3 Track transition

Track changes shall use two runtime instances:

1. Prepare the staged runtime outside the audio thread.
2. Begin a short audio crossfade.
3. Force old logical gates low at the transition boundary.
4. Swap active and staged runtimes.
5. Emit `TRACK CHANGE`.

Default crossfade: approximately 10 ms, configurable or internally tuned.

### 14.4 End detection

End of track shall be determined by this priority:

1. Valid configured/metadata duration and fade.
2. User-defined duration override.
3. Sustained-silence detector after a minimum play time.
4. Manual or infinite playback when no reliable end condition exists.

---

## 15. Threading and real-time safety

### 15.1 Worker-thread responsibilities

- File dialogs and file reading.
- RAR extraction.
- SPC validation and metadata parsing.
- Runtime construction and snapshot loading.
- BRR fingerprint analysis.
- Optional pre-indexing.
- Serialization compression or expansion if later introduced.

### 15.2 Audio-thread responsibilities

- Advance the active SPC runtime.
- Resample audio.
- Consume preallocated event data.
- Update physical and logical lane state.
- Emit audio and CV.
- Apply already-prepared runtime swaps.

### 15.3 Prohibited audio-thread operations

- File access.
- Archive decoding.
- Heap allocation.
- Mutex waits.
- Logging on normal event paths.
- Metadata string manipulation.
- Collection-wide analysis.

### 15.4 Communication

Prepared sources and mapping updates shall cross into the audio thread through atomically exchanged immutable objects, lock-free queues, or bounded handoff structures proven safe under Rack's process model.

---

## 16. Pre-indexing

### 16.1 MVP behavior

The MVP may discover identities incrementally during playback. Once assigned, a lane remains stable for the current track.

### 16.2 Optional fast analysis

A worker-thread analysis pass may run the SPC faster than real time without final audio rendering to discover:

- Instrument fingerprints.
- Maximum simultaneous sibling count per identity.
- Pitch ranges.
- Activity density.
- Likely percussion/noise classification.

The pass shall have a bounded analysis duration and cancellation support.

### 16.3 Collection-aware mapping

When enabled, Reliquary may pool fingerprints across tracks and attempt to keep common instruments on consistent lanes. Manual pins always override automatic collection mapping.

---

## 17. Serialization

Reliquary shall serialize:

- Source kind and source path.
- Source content hash.
- Selected track index and track hash.
- Transport state where appropriate.
- Track CV mode.
- Mapping identity mode.
- Logical lane assignments and pins.
- User labels.
- Pitch calibrations.
- Gate policy.
- Tempo and transpose.
- Track duration overrides.

### 17.1 External file behavior

By default, the patch stores a reference to the SPC or RSN file and validates it using size/hash information on reload.

If the source is missing or changed:

- The module shall display a clear missing-source state.
- Serialized mapping information shall remain intact.
- The user may locate a replacement file.

Embedding complete RSN data in Rack patches is deferred because it can substantially increase patch size. Embedding a standalone SPC may be considered as a later option.

---

## 18. Error handling

User-visible error categories shall include:

- File not found.
- Unsupported source type.
- Invalid SPC snapshot.
- Invalid or damaged RAR archive.
- Encrypted archive unsupported.
- Multi-volume archive unsupported.
- Archive dictionary exceeds configured limit.
- Expanded size exceeds configured limit.
- No valid SPC entries found.
- Runtime failed to initialize.
- Unsupported SPC behavior or emulator failure.

Errors shall be shown in plain language with optional technical detail in a diagnostic view or log.

A failed replacement load shall not destroy the currently active valid source.

---

## 19. Security limits

The source loader shall enforce configurable constants with conservative defaults:

- Maximum archive file size.
- Maximum number of entries.
- Maximum uncompressed size per entry.
- Maximum cumulative uncompressed size.
- Maximum RAR dictionary size.
- Maximum metadata string length.
- Maximum analysis time per track and per collection.

Archive paths shall never be written to disk, so path traversal entries are ignored as names rather than resolved as filesystem destinations.

Malformed input must fail without crashing Rack, allocating unbounded memory, blocking indefinitely, or corrupting the previously active source.

---

## 20. Performance budgets

These are initial engineering budgets, subject to measurement on representative hardware.

### 20.1 Steady-state audio

- No heap allocations in `process()`.
- No blocking synchronization in `process()`.
- One active SPC runtime plus one staged runtime only during transitions.
- Mapping/event overhead should be small relative to emulation and resampling.
- The module should remain practical in normal multi-module patches on supported Rack systems.

### 20.2 Loading

- Archive extraction occurs on one background loader thread.
- Typical RSN collections should become browsable without writing temporary files.
- Track changes after initial extraction should be near-instant aside from runtime preparation and crossfade.

### 20.3 Rendering

- Display animation should use framebuffer caching for static layers.
- Mapping rows should update only when their visible state changes.
- High-frequency envelope animation may be decimated for display without affecting CV output.

---

## 21. Testing strategy

### 21.1 Unit tests

- SPC signature and structural validation.
- Metadata parsing.
- BRR traversal and loop detection.
- Timbre fingerprint stability across RAM relocation.
- Articulation grouping policies.
- Relative pitch conversion.
- Logical sibling allocation.
- Lane pinning and overflow behavior.
- Gate policies and retriggers.
- Serialization round trips.

### 21.2 Runtime integration tests

- Known SPC files produce stable audio hashes or bounded reference comparisons.
- KON/KOFF sequences produce expected triggers and gates.
- Physical voice reassignment preserves logical lane identity.
- Same BRR sample under different SRCN/RAM addresses resolves to the same fingerprint.
- Noise and pitch-modulation flags are observed correctly.

### 21.3 RSN tests

Defined in the separate UnRAR specification, including solid archives, RAR generations, malformed archives, size limits, and supported architectures.

### 21.4 Manual musical tests

- Melody output follows audible SPC line.
- Chords allocate stable sibling lanes.
- Percussion retriggers reliably.
- Track changes reset gates cleanly.
- Mapping display agrees with audible events.
- Calibrated pitch drives a Rack oscillator in tune with the original voice.

---

## 22. Implementation phases

### Phase 0 — dependency and feasibility spikes

- Complete embedded UnRAR spike.
- Select and wrap an SPC runtime.
- Demonstrate authentic stereo playback from one SPC.
- Observe KON, KOFF, SRCN, pitch, envelope, and RAM.
- Produce a stripped binary-size report.

### Phase 1 — raw SPC module

- Load standalone SPC.
- Stereo audio playback.
- Run, pause, restart.
- Eight raw physical voice diagnostics.
- Raw pitch, gate, trigger, and envelope extraction.

### Phase 2 — logical mapping MVP

- BRR fingerprinting.
- Balanced instrument identity.
- Sixteen logical lanes.
- Stable sibling allocation.
- Mapping display.
- Relative pitch mode.
- Audible and key gate modes.

### Phase 3 — RSN collection support

- Embedded RAR extraction.
- Track browser.
- Next, previous, and Track CV.
- Metadata and duration handling.
- Crossfaded track transitions.
- Per-track stored mappings.

### Phase 4 — user mapping tools

- Rename, pin, move, mute, solo.
- Pitch-root calibration.
- Collection-aware lane preference.
- Mapping reset and import/export sidecar if desired.

### Phase 5 — advanced analysis

- Fast pre-indexing.
- Role heuristics.
- Effective pitch modulation.
- Per-voice audio stems.
- Additional lane banks or expander.

---

## 23. MVP acceptance criteria

Reliquary MVP is complete when:

1. The module loads valid standalone SPC files on all supported platforms.
2. The module loads representative solid and non-solid RSN collections using the embedded decoder.
3. The user can browse and switch tracks without external tools or dependencies.
4. Stereo audio is recognizably and reliably reproduced.
5. KON/KOFF and pitch activity are extracted without mixed-audio analysis.
6. A sound moved between physical S-DSP voices remains on the same logical Rack lane during a track.
7. Polyphonic PITCH, GATE, TRIG, and ENV outputs remain deterministic.
8. The display clearly shows the current logical lane mapping and active DSP source.
9. File loading and extraction cause no audio-thread allocation or blocking.
10. Malformed or unsupported sources fail safely with useful messages.
11. Patch serialization restores source, track, and user mapping state when the source remains available.
12. Binary size and portability remain within the gates defined by the UnRAR and SPC dependency spikes.

---

## 24. Deferred decisions

The following shall be decided during Phase 0 or visual design:

- Exact SPC runtime implementation and distribution strategy.
- Final panel width and control layout.
- Whether LEVEL is included in the initial physical panel.
- Whether source data can optionally be embedded in patches.
- Default track-mapping scope: strict per-track or collection-aware preference.
- Exact overflow presentation and future expander protocol.
- Whether pre-indexing is enabled automatically or on demand.
- Whether internal RAR-packed assets are worthwhile after final package-size measurement.

---

## 25. Architectural summary

```text
Source file
├── .spc ───────────────────────────────┐
└── .rsn → Embedded RAR reader          │
                    ↓                   │
               SpcCollection ←─────────┘
                    ↓
               ISpcRuntime
          ┌─────────┴─────────┐
          │ audio renderer    │ event/state observer
          ↓                   ↓
       resampler       physical voice tracker
                              ↓
                       BRR/articulation identity
                              ↓
                       logical lane router
                              ↓
              PITCH / GATE / TRIG / ENV / LEVEL
                              ↓
                         mapping display
```

Reliquary's central engineering principle is that emulation restores the machine, while logical routing reveals the music.
