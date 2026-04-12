# TD.Scope Live Redesign Tracker

## Objective
Make `TD.Scope` in `LIVE` mode efficient by design instead of rebuilding a costly moving preview window from raw buffer data on every publish.

The target model is:

- `Sample` mode: query a stable recorded buffer
- `Live` mode: query a stable recorded buffer that is continuously extending

That means live scope should move toward persistent precomputed envelope data, not repeated resampling of raw audio on the UI publish path.

## Current Problem

### Observed behavior
- `LIVE` scope preview costs materially more than `Sample` mode.
- The current live path pays that cost repeatedly at publish time.
- The current implementation also includes extra anti-shimmer work that is only active in live mode.

### Current hot path
- Scope preview work is measured in `src/TemporalDeck.cpp` by `uiScopePreviewCostUs`.
- `computeScopeWindowParams()` prepares a moving live window every publish.
- `buildScopeWindowBinsWithCache()` / `buildScopeWindowBins()` iterate all preview bins.
- `evaluateScopeBinAtIndex()` resamples the source data for each bin.
- In live mode, when `scopeStride > 1`, a second lattice-phase pass is added to reduce “dancing peaks”.

### Why live mode is more expensive right now
- Live mode currently re-derives min/max envelopes from raw buffer reads on every preview publish.
- Sample mode does not pay the same live-only second-phase sampling cost.
- The current cache reuses shifted bins opportunistically, but the fundamental source remains per-publish reconstruction.

## Design Goal

Shift live scope from:

- `query-time reconstruction`

to:

- `incremental envelope accumulation`

The goal is that new audio pays the cost once, and scope preview mostly reads already-prepared summaries.

## Proposed Architecture

### Core idea
Add a persistent live min/max envelope cache beside the raw live audio ring buffer.

The cache should:

- advance as new live audio arrives
- store min/max summaries in a ring structure
- support direct extraction of preview bins without rescanning raw audio

### Mental model
Treat live mode like sample mode on a rolling recording.

Instead of asking:

- “what are min/max values if I rescan this region now?”

ask:

- “which precomputed summaries already cover this region?”

### Likely shape

#### Level 0: base live envelope ring
- Fixed-width summary blocks over live audio, e.g. `N` samples per summary.
- Each block stores:
  - min left
  - max left
  - min right
  - max right
  - sequence/timeline identity

#### Optional higher levels
- Level 1 aggregates adjacent Level 0 blocks.
- Level 2 aggregates adjacent Level 1 blocks.
- Continue as needed.

This creates a multiresolution envelope pyramid for live data.

### Query strategy
- For a requested live scope window, choose the finest useful level.
- Map the visible lag window onto summary blocks at that level.
- Merge overlapping summary blocks into the outgoing `ScopeBin` array.

This should replace the current per-bin raw-audio resampling path for live mode.

## Threading / Safety Constraints

### Hard rule
- Do not move heavy preview reconstruction into the audio thread.

### Acceptable work in audio thread
- Lightweight accumulation into the current base envelope block.
- Lightweight rollover when a base envelope block completes.

### Preferred heavier work
- Higher-level aggregation should happen outside the audio thread when possible.
- If higher-level updates are cheap enough and strictly bounded, they may be folded into a very small incremental path, but that needs profiling.

## Phase Plan

### Phase 1: Baseline live envelope ring
- [x] Add a Level 0 live min/max ring alongside the current live audio ring.
- [x] Accumulate min/max envelopes incrementally as audio arrives.
- [x] Track stable timeline identity for envelope blocks.
- [x] Add a live preview path that reads Level 0 summaries instead of rescanning raw audio.
- [x] Keep sample-mode path unchanged initially.

### Phase 2: Replace live query-time reconstruction
- [ ] Route `LIVE` scope preview publish away from `evaluateScopeBinAtIndex()` raw-tap scanning.
- [ ] Remove or sharply reduce the live-only second lattice phase.
- [ ] Preserve visual stability near `NOW`.

### Phase 3: Optional multiresolution levels
- [ ] Add 1-2 coarser aggregation levels above Level 0.
- [ ] Choose query level based on visible window width and output bin density.
- [ ] Reduce merge cost for large live windows.

### Phase 4: Unification
- [ ] Decide whether sample mode should query through the same generalized envelope-window path.
- [ ] Share more code between live and sample preview generation if the design remains clean.

## Expected Benefits

- Lower and more predictable `uiScopePreviewCostUs` in live mode.
- Less dependence on publish-time resampling.
- Reduced need for expensive live-only anti-shimmer hacks.
- Better architectural symmetry between live and sample scope behavior.

## Risks

- Timeline identity for a rolling ring must be stable enough that preview queries do not misread wrapped summaries.
- If Level 0 resolution is too coarse, the live preview may lose transient detail.
- If Level 0 resolution is too fine, write-side overhead may be higher than desired.
- If higher-level aggregation is not carefully bounded, work can migrate back into the hot path.

## Open Design Decisions

- [ ] What should the Level 0 block size be?
- [ ] Should Level 0 update on every sample or on small fixed chunks?
- [ ] Should higher levels be updated in audio-thread micro-increments or by a background/consumer path?
- [ ] Should stereo always be stored explicitly, or should mono preview be derived from stereo summaries only when needed?
- [ ] What visual stability mechanisms remain necessary once live preview reads persistent summaries?

## Near-Term Implementation Notes

### Current likely optimization target
The best immediate leverage is in `src/TemporalDeck.cpp`:

- `evaluateScopeBinAtIndex()`
- `buildScopeWindowBins()`
- `buildScopeWindowBinsWithCache()`

Specifically:

- live-mode raw-tap accumulation
- live-only second-phase sampling when `scopeStride > 1`

### Likely first cut
Implement only a Level 0 live summary ring first.

That is enough to validate the model before introducing a full pyramid.

### Current first-cut details
- Level 0 block size is currently `32` live samples per summary block.
- Each block stores:
  - left min/max
  - right min/max
  - mid min/max
  - absolute block key
- Live scope query now prefers these persistent summaries first.
- Raw-tap reconstruction remains available as a fallback path if the summary query cannot satisfy a bin.
- Live shifted-bin cache reuse is temporarily disabled while the new absolute-timeline summary path settles.

## Progress Log

- 2026-04-11: Created live redesign tracker.
- 2026-04-11: Confirmed current live scope cost is fundamentally higher by design because `LIVE` preview rebuilds bins from raw buffer reads and adds a second live-only sampling phase in `evaluateScopeBinAtIndex()`.
- 2026-04-11: Chosen architecture direction: persistent rolling live envelope summaries, treating live preview as a continuously extending sample-like buffer query.
- 2026-04-11: Implemented Phase 1 first cut:
  - added a Level 0 live envelope ring in `TemporalDeckEngine`
  - update path runs incrementally on live writes
  - introduced absolute live sample position tracking for scope queries
  - live scope bins now query Level 0 summaries before falling back to legacy raw reconstruction
  - temporarily disabled live shifted-bin cache reuse to avoid mixing wrapped and absolute timeline assumptions during the transition
- 2026-04-11: Verified compile for `TemporalDeck.cpp` and `TDScope.cpp`.

## Change Log

- Added `tdscope-live-redesign.md` as the working design/progress doc for the `TD.Scope` live-mode efficiency redesign.
- Added Level 0 live envelope summaries to `src/TemporalDeckEngine.hpp`.
- Switched live scope query in `src/TemporalDeck.cpp` to prefer persistent Level 0 summaries.
