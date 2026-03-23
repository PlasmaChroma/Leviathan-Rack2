# TemporalDeck Sample Mode Technical Design

## Goal

Add a new operating mode to `TemporalDeck` that lets the user load an audio file into the existing scratch buffer and treat it as the playback source instead of live input. The first implementation should be practical, conservative on RAM, and should not destabilize the current live-audio workflow.

This mode should support:

- loading a sample from the module context menu
- copying decoded sample data into the scratch buffer
- truncating oversized files to a hard maximum duration
- basic transport behavior (`play`, `pause`, `stop`/`return-to-start`)
- visible timeline feedback on the platter arc
- interactive seeking from the UI

This mode should explicitly avoid trying to solve everything at once. Time-stretching, non-destructive editing, file saving, multi-sample management, waveform rendering, and advanced metadata handling can wait.

## Current State

The existing module is already close to what we need:

- `TemporalDeckBuffer` in `src/TemporalDeck.cpp` already owns a float buffer with mono/stereo storage modes and interpolated reads.
- The engine already has separate notions of `readHead`, `timelineHead`, and `scratchLagSamples`.
- The UI already has:
  - a module context menu (`TemporalDeckWidget::appendContextMenu()`)
  - a platter interaction surface (`TemporalDeckPlatterWidget`)
  - a top half-ring light arc that currently visualizes lag depth
- Buffer duration modes already include long allocations:
  - `10 min stereo`
  - `10 min mono`

That means sample mode is mostly an engine-state and UI-behavior expansion, not a rewrite.

## Product Definition

### User model

The module has two source modes:

1. `Live`
   Audio input is written into the rolling scratch buffer, which behaves as it does now.

2. `Sample`
   The scratch buffer is populated from a decoded audio file. Playback reads from that loaded material instead of requiring live input.

### Initial UX

Context menu actions:

- `Load sample...`
- `Clear sample`
- `Sample mode`
  - `Off`
  - `On when sample loaded`

Likely behavior:

- Loading a file automatically enables sample mode.
- Clearing the file returns the module to live mode.
- If no sample is loaded, the module behaves exactly as it does today.

## Scope For First Pass

### In scope

- file picker from context menu
- decode common audio file formats supported by Rack helpers/libs we already have available
- copy decoded audio into the scratch buffer
- truncate at a hard limit @ 10 minutes (otherwise buffer is equal to length of sample)
- transport state
- seek/timeline UI
- disabling or redefining live-only behaviors that do not make sense in sample mode
- serialization of loaded-file path and sample-mode settings

### Out of scope

- background streaming from disk
- preserving loaded audio inside patch JSON
- waveform overviews
- loop region editing
- pitch-locked time stretch
- transient detection / slicing
- multi-file playlists
- drag-and-drop import

## Core Design Decisions

### 1. Reuse the existing scratch buffer

Do not add a second audio store for the first version. The loaded sample should become the content of `TemporalDeckBuffer`.

Why:

- avoids duplicating RAM use
- keeps scratch interpolation paths unchanged
- keeps platter/scratch logic operating on one source of truth

Implication:

- sample loading is a destructive buffer replacement operation
- live input capture should be disabled while sample mode is active

### 2. Hard duration cap

Use a hard maximum of 10 minutes for v1.

Policy:

- if the decoded file is longer than 10 minutes, truncate to the first 10 minutes
- stereo files may either:
  - force `10 min stereo` behavior for v1, rejecting/truncating past that limit, or
  - be allowed only when the selected storage mode can hold them

Recommended rule for implementation:

- define a sample-mode-specific hard cap in seconds
- define a required storage format based on source channel count
- if the current buffer mode cannot hold the file:
  - auto-switch to a compatible buffer allocation if possible
  - otherwise downmix to mono and notify via log/UI text

Pragmatic v1 recommendation:

- support:
  - up to `10 min` for stereo
  - up to `10 min` for mono
- if a stereo file exceeds the stereo capacity, truncate to stereo capacity rather than inventing background resampling/downmix policy immediately

This is slightly less clean than “always 10 min”, but it matches the current memory model and avoids silently forcing mono on users.

### 3. Sample mode is non-recording by default

When sample mode is active:

- `INPUT_L_INPUT` / `INPUT_R_INPUT` are ignored for buffer writes
- feedback should not write back into the source asset unless we intentionally want destructive overdub behavior

Recommended first-pass policy:

- sample mode uses loaded data as a fixed source
- wet playback can still use mix/feedback in the output path, but the underlying sample buffer itself is not rewritten

This avoids a large class of “why did my loaded sample get destroyed?” problems.

### 4. Transport is explicit

Live mode does not need transport. Sample mode does.

Add explicit transport state:

- `stopped`
- `playing`
- `paused`

Recommended behavior:

- `play`: advance `timelineHead`
- `pause`: freeze timeline motion but still allow scratching/seek interaction
- `stop`: pause and return transport position to sample start

## Engine Changes

### New persistent state

Add a sample-mode state block to `TemporalDeck::Impl` / engine state:

- `bool sampleModeEnabled`
- `bool sampleLoaded`
- `std::string samplePath`
- `std::string sampleDisplayName`
- `int64_t sampleFrameCount`
- `int sampleChannelCount`
- `float sampleSampleRate`
- `bool sampleTruncated`
- `bool sampleTransportPlaying`
- `double sampleStartFrame`
- `double sampleEndFrame`
- `double samplePlayheadFrame`

Notes:

- `samplePlayheadFrame` should represent logical position within the loaded sample, not just circular buffer lag.
- We should also track `loadedFramesInBuffer` separately from `buffer.filled` if that makes playback bounds clearer.

### Buffer semantics in sample mode

Today the buffer is treated as a rolling circular capture store. In sample mode we want fixed loaded content.

Recommended approach:

- still use `TemporalDeckBuffer`
- add a “fixed-content” interpretation:
  - `filled = loadedFrameCount`
  - `writeHead = loadedFrameCount % size`
  - no further writes while sample mode is active
- do not wrap playback across the entire circular allocation by default
- clamp transport/seeking to `[0, loadedFrameCount)`

This is the most important engine change: sample playback must stop or hold at end-of-file instead of accidentally wrapping into old circular-buffer semantics.

### Reading model

Define a sample-mode read model:

- transport advances forward through sample frames
- scratch input temporarily offsets or drives read position around the transport anchor
- seeking directly sets transport position

That suggests separating:

- `transportFrame`
- `scratchOffsetFrames`
- `effectiveReadFrame = clamp(transportFrame - scratchOffsetFrames, 0, loadedFrameCount - 1)`

The exact sign convention can follow the current lag-based implementation, but the important point is that sample mode should use bounded linear media coordinates, not infinite circular lag semantics.

### File load path

We need a synchronous, safe first implementation:

1. invoke Rack file chooser from the context menu
2. decode file into temporary float buffers
3. choose storage mode / truncate if needed
4. reset the deck engine into sample state
5. copy decoded data into `TemporalDeckBuffer`
6. initialize playhead and UI state

Implementation note:

- the disk decode should happen on the UI thread only long enough to complete the import action
- audio-thread mutation must be synchronized carefully

Recommended mechanism:

- decode to a temporary heap buffer outside the DSP hot path
- acquire a module lock or stage a “pending sample swap” object
- perform the actual engine-state swap atomically at a safe boundary

Do not decode directly from the audio thread.

### Playback end behavior

For v1, use:

- `play` stops automatically at end of sample
- playhead remains at end
- user can:
  - seek back
  - hit stop to return to start
  - scratch near end without wraparound

Looping can be added later as a separate option.

### Parameter behavior changes in sample mode

#### Keep as-is

- `RATE`
- `RATE CV`
- `MIX`
- cartridge color
- scratch sensitivity
- reverse

#### Needs redefinition or disablement

- `FREEZE`
  - in live mode it freezes capture/playback relationship
  - in sample mode it may become redundant with `pause`

- `SLIP`
  - current semantics are tied to live-audio slip return
  - this may still be useful as “return to transport anchor after scratching”, but it needs an explicit definition in sample mode

- `FEEDBACK`
  - if the buffer is read-only in sample mode, feedback cannot mean destructive recirculation into the sample buffer
  - options:
    - disable it in sample mode
    - keep it as output-only effect feedback if there is a downstream delay path

Recommended v1 policy:

- map `FREEZE` to transport `pause` when sample mode is active
- keep `SLIP` as “return to transport position after scratch release”
- leave `FEEDBACK` at 0 or visually de-emphasize it in sample mode until a clean semantic is designed

## UI Changes

### Context menu

Add menu items under a new section:

- `Sample`
  - `Load sample...`
  - `Clear sample`
  - separator
  - `Enable sample mode`
  - `Auto-play on load`
  - `Truncate oversized files`

Possibly also:

- `Imported file info`
  - file name
  - duration
  - stereo/mono
  - truncated yes/no

### Transport controls

We need at least two visible controls on the panel in sample mode:

- `Play/Pause`
- `Stop`

Implementation options:

1. Reuse existing buttons conditionally.
2. Add small on-panel icon buttons near the platter.
3. Put transport only in the context menu for the first code pass.

Recommended path:

- implement engine transport first
- add visible UI buttons in the same iteration if layout can be done cleanly

Avoid hiding transport entirely in the menu. Once a sample is loaded, play/pause needs to be immediate.

### Timeline display

The top half-ring is the right place for sample position.

Recommended display mapping in sample mode:

- dim background arc = full loaded sample extent
- bright segment or pointer = current playhead
- optional secondary marker = scratch/drag target when interacting

Interaction:

- click or drag on the top ring to seek
- left edge = start
- right edge = end
- clamp to loaded sample bounds

This should be separate from platter scratching:

- platter surface = scratch gesture
- top arc = timeline seek

That split is clean and teachable.

### Text readout

Replace the current lag-only text in sample mode.

Suggested display:

- current time / total time
- example: `01:42 / 07:58`

Lag can still be shown while actively scratching, but the default sample-mode readout should be transport-centric.

### Visual state cues

Sample mode should be obvious without opening the menu.

Possible cues:

- different center label color when sample loaded
- small `SAMPLE` badge
- play/pause indicator near the platter
- different arc styling from live mode

## Serialization

Persist:

- sample mode enabled flag
- sample path
- autoplay preference
- truncation preference

Do not persist full sample audio in patch JSON.

On patch reload:

- attempt to reload from saved path
- if the file is missing:
  - keep patch functional
  - mark sample as unavailable
  - fall back to live mode or inactive sample mode

This needs explicit user-visible failure handling, at least via log and a subtle UI state.

## Threading / Safety

This is the main engineering risk.

Potential hazards:

- swapping a large buffer while DSP is reading from it
- blocking the UI for too long during decode
- changing allocation size from a menu action while audio is running

Recommended safe pattern:

1. Decode file into a temporary import object.
2. Build the final storage decision there.
3. Queue a sample-install operation on the module.
4. At a controlled point, swap the engine state to the new loaded sample.

If we do not already have a module mutex or swap mechanism, we should add one rather than trying to mutate `TemporalDeckBuffer` piecemeal under audio load.

## Testing Plan

### Manual tests

- load short mono file and verify playback
- load short stereo file and verify playback
- load file longer than supported duration and verify truncation
- scratch while playing
- scratch while paused
- seek via top arc while playing
- seek via top arc while paused
- reverse playback in sample mode
- reload patch with valid sample path
- reload patch with missing sample path
- switch from sample mode back to live mode
- verify no writes occur from live input while sample mode is active

### Code-level tests

Add focused tests for:

- import truncation logic
- sample-position clamping
- end-of-sample stop behavior
- sample-mode slip return semantics
- JSON persistence of sample metadata/path

If possible, keep file-decoder tests separated from engine tests so the transport logic can be exercised with synthetic buffers.

## Implementation Plan

### Phase 1: Engine groundwork

- add source mode enum: `Live` / `Sample`
- add transport state
- add bounded sample-playback coordinates
- add serialization fields
- ensure sample mode disables buffer writes

### Phase 2: Import pipeline

- add context menu action for `Load sample...`
- decode file into temporary float storage
- choose mono/stereo allocation policy
- truncate if needed
- swap loaded data into the module

### Phase 3: UI state

- add sample loaded badge/readout
- change arc rendering in sample mode from lag meter to timeline
- expose current/total time text

### Phase 4: Interaction

- add play/pause and stop controls
- add arc seeking
- preserve platter scratching on loaded content
- also any compensation (boosting) on forward scratch to compensate for buffer writes is not needed in sample mode

### Phase 5: Parameter cleanup

- define exact sample-mode semantics for `FREEZE`, `SLIP`, `FEEDBACK`
- disable or relabel anything still ambiguous

## Open Questions

1. Should stereo sample imports above 10 minutes be truncated in stereo?
answer: yes
2. Should `pause` preserve scratch responsiveness with no transport motion? Recommended answer: yes.
3. Should `reverse` reverse transport, scratch response, or both? Recommended answer: both, matching current playback expectation.
4. Should `stop` return to absolute sample start or to a user-defined cue later? For v1: absolute start.
5. Do we want sample mode to auto-engage when a file is loaded, or require a separate enable toggle? Recommended answer: auto-engage.

## Recommendation

The cleanest first implementation is:

- one new source mode: `Sample`
- one import path from the context menu
- one bounded playhead model
- one visible transport pair: `play/pause` and `stop`
- one arc repurposing: timeline + seek
- one conservative storage policy:
  - stereo up to current stereo capacity
  - mono up to current mono capacity
  - truncate anything longer

That gives us a usable sample deck without committing prematurely to destructive overdub, loop editing, or disk streaming. It also keeps the code aligned with the current `TemporalDeck` architecture instead of fighting it.
