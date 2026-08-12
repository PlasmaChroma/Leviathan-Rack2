# Deepcache staged warming plan

## Status

This document proposes a faster persisted-preview startup path for Deepcache.
It is a design and implementation plan, not a description of current behavior.

The intended result is that a Deepcache installation with a mostly complete
raster archive becomes useful after reading its small index and hydrating the
first useful cards. Full GPU residency may continue incrementally in the
background.

## Problem

The current warm-cache startup path performs the following work for every
matching archive entry:

1. read the QOI payload from the pack;
2. checksum the payload;
3. decode it to RGBA on the archive worker;
4. transfer the RGBA through the bounded UI handoff;
5. create an individual NanoVG image on the draw thread; and
6. release the temporary RGBA after upload.

Deepcache does not allow normal preview planning to begin until archive loading,
the decoded handoff, and the persistent upload queue have all drained. A cache
hit therefore avoids constructing and rendering a live module widget, but it is
not a cheap startup event. With a large library, whole-library decode and GPU
upload remain on the critical path.

The pipeline also decodes entries for models that are not currently display
eligible. The UI recognizes those models after decode and discards their RGBA
without creating an image. This preserves knowledge that compressed data exists,
but spends disk bandwidth, checksum time, decode time, allocation traffic, and
worker-queue capacity unnecessarily.

## Goals

- Make time to a useful browser proportional to the index and the first useful
  cards, rather than the complete installed library.
- Identify cached models before their raster payloads are decoded.
- Start planning genuine cache misses as soon as archive metadata is available.
- Hydrate currently useful models before background entries.
- Preserve the current eventual all-GPU-resident behavior unless a later memory
  policy explicitly changes it.
- Preserve bounded worker/UI queues and the configurable cooperative UI budget.
- Treat the persistent cache as disposable: isolate a corrupt entry and rebuild
  it without failing unrelated previews.
- Avoid accessing Rack widgets, NanoVG, OpenGL, or the plugin registry from the
  archive worker.
- Keep graphics-context destruction and recreation safe.

## Non-goals

- Changing the archive format solely for this work.
- Adding a GPU-memory eviction policy.
- Replacing one-image-per-card rendering with texture atlases.
- Parallelizing QOI decoding before measurements show that decoding is the
  limiting stage.
- Optimizing cold-cache framebuffer capture or synchronous `glGetTexImage()`;
  this plan is specifically about loading persisted raster hits.

## Proposed pipeline

Startup is divided into metadata discovery and raster hydration.

### Phase 1: index discovery

The archive worker opens the pack, loads the binary index, performs structural
validation, removes entries that are irrelevant to the current installation,
and compares each remaining fingerprint with the wanted fingerprint.

For each matching entry whose dimensions and pack range are valid, the worker
publishes an **indexed candidate**. This means that Deepcache has a plausible
compressed recovery source; it does not yet mean the QOI payload has passed its
checksum or decoded successfully.

The UI manager consumes indexed candidates and records their model indices in
the compressed set. Once all candidates have been published, it may submit the
planner input. The planner excludes indexed candidates and schedules only true
misses or stale entries.

Archive metadata readiness must not wait for the decoded queue or NanoVG upload
queue to drain.

### Phase 2: prioritized hydration

After metadata discovery, the archive worker hydrates indexed candidates in
priority tiers:

1. models whose cards intersect the current browser viewport, plus a small
   look-ahead margin;
2. models selected by the active filter, followed by favorites and recently
   used models;
3. all other display-eligible models; and
4. no automatic hydration for display-ineligible models.

Within a tier, entries should be ordered by physical pack offset. This retains
most of the current sequential-I/O benefit without forcing first-use cards to
wait behind the complete archive.

The existing `visibleModelIndices()` reports filter visibility, which is usually
the entire library under the default filter. It is not sufficient for tier one.
Deepcache needs a separate viewport-aware query based on the scroll viewport,
card positions, and a modest prefetch margin.

Hydration consists of pack read, checksum validation, QOI decode, bounded UI
handoff, and NanoVG upload. The archive worker should retain one pack handle for
the background sweep; issuing the existing single-entry decode request for every
model would reopen the pack repeatedly and lose physical-order batching.

### Phase 3: background completion

Once the first priority tier is hydrated, the browser is usable and normal miss
construction can proceed under the existing UI budget. Remaining cached cards
continue hydrating in the background until every display-eligible indexed entry
owns a valid image in the current graphics context.

The user-visible cache state should distinguish usability from background
completion. It is acceptable for the existing warming indication to remain
active while the browser is already usable. `READY` should continue to mean that
all requested construction and background hydration work has completed.

## Indexed-candidate contract

Index discovery must not claim that payload integrity has been proven. An
indexed candidate has passed only:

- cache-key lookup;
- artifact-fingerprint comparison;
- dimension and decoded-size bounds;
- encoded-length bounds; and
- pack offset/range bounds.

Checksum and QOI validation remain part of hydration. If either fails:

1. remove the entry from the process-local live index;
2. remove the model from the compressed/persistent sets;
3. clear any partial raster state;
4. enqueue that model as an ordinary preview miss; and
5. continue hydrating unrelated entries.

The owner may later publish a repaired append/index entry through the existing
archive write path. A read-only contender keeps the repaired result volatile as
it does today.

## Scheduling and reprioritization

Hydration priority can change after startup when the browser opens, scrolls, or
its filters change. Priority promotion should be cheap and generation-scoped.
Duplicate requests for the same cache key must coalesce.

Recommended scheduling rules:

- viewport requests may promote an entry ahead of the background sweep;
- filter/favorite changes may promote a set of entries;
- a graphics-context generation change invalidates pending uploads and requests
  fresh hydration for the new generation;
- stale-generation decoded results are discarded by the existing generation
  check;
- display-ineligible entries remain indexed but are not decoded or uploaded;
- if an entry becomes eligible later, request hydration from its indexed source;
- if an entry becomes ineligible while queued, cancel or harmlessly discard its
  hydration result.

The archive worker should sort entries by pack offset inside each priority tier.
It may read across small gaps as the current startup loader does. Exact global
priority is less important than avoiding a whole-library barrier.

## Queue and thread boundaries

The existing decoded handoff limits of 16 entries and 64 MiB remain appropriate.
Metadata publication needs a separate bounded or batch-oriented channel because
it carries only cache keys and entry metadata, not RGBA.

One practical interface is:

- archive worker publishes batches of indexed candidates;
- UI drains candidate batches during `step()`;
- archive worker publishes an explicit index-discovery-complete event;
- UI starts the planner after consuming that event;
- UI sends viewport and priority promotions to the archive worker; and
- decoded previews continue through the existing bounded queue.

No worker callback should directly mutate `DeepcacheModelBox` instances. All
model-state changes and NanoVG uploads remain on Rack's UI/draw thread.

## UI upload policy

`nvgCreateImageRGBA()` remains necessary under the current per-card image design
and may dominate total warm time. Uploads must remain cooperative to avoid long
UI stalls.

The current configurable budget should be retained initially. Measurements may
justify a separate adaptive background-upload budget later—for example, a
larger allowance while the browser is closed and a smaller allowance while the
user is interacting—but this should not be introduced until phase timings are
available.

The time-budget check should continue to permit progress when one upload exceeds
the budget. It must not begin another upload after the budget is already
exhausted.

## QOI allocation improvement

The current decoder allocates a complete QOI output buffer, copies it into
`DecodedPreview::rgba`, and frees the original buffer. After the staged loader is
working, add a decoder path that writes directly into pre-sized owned storage or
otherwise transfers ownership without a full RGBA copy.

This is a contained secondary optimization. It reduces allocation and memory
bandwidth but does not remove the whole-library startup barrier by itself.

## Instrumentation

Before and after implementation, collect debug-gated timings and counts for:

- model and plugin count;
- index bytes, parse time, and candidate count;
- stale, missing, structurally invalid, checksum-failed, and QOI-failed counts;
- encoded bytes read;
- pack-read time;
- checksum time;
- QOI decode time;
- time blocked by the decoded-queue limits;
- decoded RGBA bytes;
- UI handoff/install time;
- NanoVG image creation total, average, maximum, and count;
- time to index discovery;
- time to first viewport fully hydrated;
- time to browser usability;
- time to complete background hydration; and
- peak pending-upload bytes.

Instrumentation must be gated by `isDragonKingDebugEnabled()`. Aggregate phase
statistics are preferable to one log line per preview. Particularly slow
individual decodes or uploads may retain thresholded warnings.

## Implementation stages

### Stage 1: measurements

Add aggregate phase timers without changing scheduling. Record representative
1x and 2x warm loads on a large installed library.

### Stage 2: metadata readiness

- Add the indexed-candidate publication path.
- Populate compressed model knowledge before decode.
- Start the planner after index discovery rather than after GPU uploads.
- Preserve the current full hydration order temporarily.

This isolates the readiness-state change from priority scheduling.

### Stage 3: priority hydration

- Add a viewport-aware model query.
- Add hydration priority tiers and promotion.
- Sort by pack offset inside each tier.
- Stop automatically decoding display-ineligible entries.

### Stage 4: allocation cleanup

Decode QOI directly into owned RGBA storage and compare phase measurements.

### Stage 5: evidence-driven follow-ups

Only if measurements justify them, evaluate:

- more than one QOI decoder;
- adaptive upload budgets;
- persisted atlas pages or another batched GPU representation; or
- a GPU-memory residency policy.

## Test plan

Extend archive and planner coverage with the following cases:

- index candidates are published before any payload decode completes;
- a matching indexed candidate is excluded from miss construction;
- a stale fingerprint is not published as a candidate;
- a structurally invalid range is not published as a candidate;
- checksum failure during lazy hydration rebuilds only the affected model;
- malformed QOI during lazy hydration rebuilds only the affected model;
- viewport entries hydrate before background entries;
- entries within the same tier retain deterministic physical-order scheduling;
- duplicate promotions coalesce;
- stale graphics-generation results are ignored;
- display-ineligible entries are indexed without being decoded;
- newly eligible entries hydrate on demand;
- decoded and upload queues remain within their entry and byte bounds;
- shutdown cancels index discovery and hydration without deadlock;
- owner and read-only archive workers preserve their existing lease behavior;
- context destruction followed by restoration rehydrates viewport entries first;
  and
- a fully warm archive reaches the same final ready counts as the current path.

Run `make test-fast` for regression coverage. In WSL, focused tests and source
compilation are authoritative for this work; final plugin linking remains a
Windows/MSYS2 validation step.

## Acceptance criteria

The work is complete when:

1. planner submission no longer waits for all persisted rasters to be decoded
   and uploaded;
2. the first viewport can become fully raster-backed before the remainder of a
   large archive;
3. display-ineligible cache hits perform no startup QOI decode or GPU upload;
4. corrupt lazy-hydration entries fall back to isolated rebuilding;
5. queue memory remains bounded;
6. eventual background completion preserves the existing display-eligible
   all-GPU-resident behavior;
7. graphics-context recreation remains generation-safe;
8. debug measurements show time to browser usability separately from time to
   full warming; and
9. focused archive/planner tests and `make test-fast` pass.

## Open decisions

- Whether `READY` should continue to mean full background hydration, or whether
  a distinct user-visible `USABLE` state is worth adding.
- The viewport look-ahead distance used for tier-one prefetch.
- Whether favorites and recent modules need separate priority tiers.
- Whether background hydration should pause during active browser interaction.
- The measured threshold at which an adaptive upload budget or texture atlas
  becomes worthwhile.
