# Temporal Deck / TDScope live render refinement: fixed time buckets with progressive fill

## Goal

Refine `TDScope` live scope rendering so the live view uses **persistent fixed time buckets** instead of recomputing envelope slices directly from the current per-frame row sampling.

The purpose is to reduce:

* fuzzy peak flicker,
* unstable peak placement between adjacent rows,
* “breathing” caused by row-based re-aggregation as the live stream advances.

The core behavioral requirement is:

> In live mode, the scope display should use a fixed set of time buckets that span the visible live-buffer range. Incoming samples (or incoming scope min/max aggregates) are accumulated into those same buckets every frame. A bucket remains partially filled until its time span is complete. Once full, accumulation rolls to the next bucket. Bucket boundaries must remain stable across the entire live-buffer range so that scratching to different live positions still maps to the same temporal bucket grid.

This is a **live-mode-only change**. Sample mode can keep the current rendering behavior unless it is later desirable to unify both paths.

---

## Current problem

`TDScopeDisplayWidget` currently rebuilds the visual rows each draw by mapping each screen row to a lag interval and then scanning overlapping `ScopeBin`s to compute min/max for that row.

This is currently centered around:

* `sampleEnvelopeOverInterval(...)`
* `rebuildLane(...)`
* per-row arrays like `rowX0`, `rowX1`, `rowVisualIntensity`, `rowValid`, etc.

This means the visual aggregation grid is effectively tied to the current row sampling and current viewport mapping, not to a stable time grid. As a result:

* peaks can move between neighboring rows as timing shifts slightly,
* incomplete live intervals do not have a stable “home,”
* the display can shimmer when new live data lands near row boundaries.

---

## Desired behavior

### 1. Introduce a stable bucket grid in live mode

For live mode, define a fixed set of **display buckets** that cover the full visible live-buffer range.

Each bucket represents a stable time interval in the live timeline.

These buckets must be anchored to the live-buffer timeline, not regenerated from current pixel rows. That means:

* the same absolute live-buffer time always maps to the same bucket index,
* scratching around the live buffer does not change bucket boundaries,
* only the viewport selection changes which subset / mapping is visible.

### 2. Buckets accumulate progressively

Each bucket stores at minimum:

* `min`
* `max`
* `hasData`
* `fillFraction` or equivalent accumulated sample count / span coverage

When live data starts entering a new bucket:

* that bucket is initially partial,
* its min/max envelope grows as more samples land in it,
* it does **not** snap immediately to a fully authoritative value until its time span is complete.

This gives stable behavior for the “currently filling” frontier bucket.

### 3. Bucket rollover is time-based

When the accumulated live time crosses the current bucket’s end time:

* mark the current bucket complete,
* advance to the next bucket,
* clear/reset that next bucket before accumulation starts.

This should operate like a ring buffer over the visible live time range.

### 4. Scratching preserves bucket consistency

When the user scratches to another live position:

* do **not** regenerate a different bucket lattice for that view,
* instead sample from the same persistent bucket ring using the requested lag / viewport mapping.

Bucket placement must remain stable over the full live-buffer range.

### 5. Rendering still uses rows, but rows read from buckets

The screen rows can still exist as the final rasterization stage, but in live mode they should be derived from the persistent bucket data rather than from ad hoc `sampleEnvelopeOverInterval()` scans.

In other words:

* **persistent bucket grid = data model**
* **screen rows = presentation layer**

This is the key architecture change.

---

## Implementation approach

## A. Add persistent live bucket storage to `TDScopeDisplayWidget`

Add a live-only bucket cache for left/right lanes.

Suggested structure:

```cpp
struct LiveScopeBucket {
    float minNorm = 0.f;
    float maxNorm = 0.f;
    float coveredSamples = 0.f;   // how much of this bucket is filled
    float totalSamples = 1.f;     // nominal span of the bucket
    bool hasData = false;
    uint64_t epoch = 0;           // optional, helps detect stale/reused ring entries
};
```

For stereo, keep separate arrays for left/right.

Suggested members:

```cpp
std::vector<LiveScopeBucket> liveBucketsLeft;
std::vector<LiveScopeBucket> liveBucketsRight;

bool liveBucketsInitialized = false;
uint64_t liveBucketEpoch = 1;
int liveBucketCount = 0;
float liveBucketSpanSamples = 0.f;

// ring/frontier bookkeeping
int liveWriteBucketIndex = 0;
float liveWriteBucketConsumedSamples = 0.f;

// track host timeline anchor used to keep bucket grid stable
float liveBucketGridStartLagSamples = 0.f;   // or equivalent anchor
float liveBucketGridEndLagSamples = 0.f;
uint64_t cachedLivePublishSeq = 0;
```

---

## B. Define bucket count from display height, but keep it stable

Use approximately one bucket per rendered row, or a slightly higher resolution if desired.

Recommended first pass:

* `liveBucketCount = rowCount`

That keeps the visual mapping simple.

Important: bucket count should only change when the widget geometry changes, not every frame.

---

## C. Define bucket span from the visible live range

In live mode, compute the total visible live time span exactly as now from:

* `scopeHalfWindowMs`
* `sampleRate`
* live viewport bias / lag logic

Then define:

```cpp
liveBucketSpanSamples = totalWindowSamples / float(liveBucketCount);
```

But the buckets must be **anchored to the live buffer timeline**, not just to the current frame’s row intervals.

The anchor should be based on a stable origin in the live-buffer time axis. For example:

* bucket 0 starts at a fixed lag origin,
* or use a modulo of absolute live sample position if that is available from the host,
* or derive a stable anchor from `scopeStartLagSamples` / current live window definition.

What matters is:

> small viewport shifts must not move bucket boundaries.

If host-side absolute live sample time is available, use it. If not, derive the most stable possible lag-space anchor and keep it continuous across publishes.

---

## D. Update live buckets incrementally instead of rebuilding from rows

Add a live-mode path that updates bucket contents incrementally whenever a new snapshot arrives.

Pseudo-flow:

```cpp
if (sampleMode) {
    // keep existing behavior for now
} else {
    if (!liveBucketsInitialized || geometry changed || stereo layout changed || bucket count changed) {
        initializeLiveBuckets(...);
    }

    if (msg.publishSeq != cachedLivePublishSeq) {
        ingestLiveSnapshotIntoBuckets(msg);
        cachedLivePublishSeq = msg.publishSeq;
    }

    rebuildRowsFromLiveBuckets(...);
}
```

### `ingestLiveSnapshotIntoBuckets(msg)`

This function should:

1. Determine the time interval represented by the newest incoming live scope data.
2. Map that interval into one or more persistent live buckets.
3. Update each touched bucket’s min/max envelope.
4. Increase that bucket’s covered amount.
5. Advance / roll buckets when their span is fully consumed.

If the host message already provides pre-aggregated `ScopeBin`s for live data, you do **not** need per-sample ingestion. You can ingest bin-by-bin as long as mapping is done against the stable bucket grid.

For each source bin:

* determine its source lag interval,
* intersect with one or more persistent display buckets,
* merge min/max into those buckets,
* accumulate proportional coverage.

Merging rule:

```cpp
bucket.minNorm = std::min(bucket.minNorm, binMinNorm);
bucket.maxNorm = std::max(bucket.maxNorm, binMaxNorm);
bucket.hasData = true;
bucket.coveredSamples += overlapSamples;
```

Clamp `coveredSamples` to `totalSamples`.

---

## E. Partial-fill semantics

For the currently filling bucket:

* allow it to render even if incomplete,
* but expose its partial state via `fillFraction = coveredSamples / totalSamples`.

This partial-fill value can be used to reduce flicker. Two options:

### Option 1: keep geometry authoritative, reduce intensity until full

* envelope width uses actual accumulated min/max,
* intensity is scaled by `fillFraction`.

### Option 2: blend geometry toward center until fuller

* for partial buckets, blend min/max inward based on fill fraction,
* this makes the frontier bucket visually less jumpy.

Recommended first pass: **Option 1**. It is simpler and less likely to smear the waveform unnaturally.

---

## F. Completed buckets should be visually stable

Once a bucket is full:

* its envelope should remain fixed until that ring slot is reused,
* no re-aggregation from different row intervals should change it.

This is the main anti-flicker property.

---

## G. Render rows from buckets, not from source scope bins

Replace the live-mode use of `sampleEnvelopeOverInterval()` inside `rebuildLane()`.

For live mode:

* each row maps directly to one persistent bucket, or
* each row interpolates between adjacent persistent buckets if you want smoother appearance.

Simplest first pass:

```cpp
row i <-> bucket i
```

Then:

* `rowMinNorm` / `rowMaxNorm` come from the bucket,
* row validity comes from `bucket.hasData`,
* intensity comes from bucket amplitude plus optional `fillFraction` modulation.

This means the existing drawing pipeline can stay largely intact:

* `rowX0`
* `rowX1`
* `rowVisualIntensity`
* `rowBucket`
* connector drawing
* color mapping

Those are fine. The change is the source of truth feeding them.

---

## H. Keep the current row-tail hold only as a secondary polish layer

The current row tail hold logic in `rebuildLane()` (`kRowTailHoldFrames`, intensity decay, carry-forward of previous row state) is presently doing some anti-flicker work.

Once persistent buckets exist, this should become much less necessary.

Recommended behavior:

* keep it initially,
* but reduce or disable it for rows backed by valid persistent bucket data,
* reserve hold/decay only for genuinely missing data gaps.

Otherwise you risk “double smoothing.”

---

## I. Stereo should mirror the same design

If stereo view is active:

* left and right channels use separate persistent bucket arrays,
* both share the same time bucket boundaries.

Time alignment between left/right must remain identical.

---

## J. Reset conditions

Buckets should be reset when any of these materially change:

* widget row count / display height,
* mono/stereo layout,
* scope window size in a way that changes bucket span,
* a hard discontinuity in host timeline / transport mode / source validity,
* reconnect to Temporal Deck after link loss,
* switching between sample mode and live mode.

On reset:

* clear buckets,
* reinitialize the stable grid,
* restart the current write bucket/frontier.

---

## Suggested concrete refactor plan

### Step 1

Add persistent live bucket storage and initialization helpers to `TDScopeDisplayWidget`.

### Step 2

Split current lane rebuild into two paths:

* `rebuildLaneFromScopeBins(...)` for sample mode / legacy path
* `rebuildLaneFromLiveBuckets(...)` for live mode

### Step 3

Add `ingestLiveSnapshotIntoBuckets(...)` and only call it when `msg.publishSeq` changes.

### Step 4

Make row rendering for live mode consume persistent buckets.

### Step 5

Tune partial-fill intensity and reduce redundant hold smoothing.

---

## Important behavioral notes for Codex

### Do not anchor bucket boundaries to pixels

Pixels/rows are only for final draw.
The bucket grid must exist in time-space first.

### Do not fully rebuild live envelopes from scratch every draw

Only rebuild row presentation from the bucket cache.
Live data aggregation should be incremental.

### Do not let scratching redefine the bucket lattice

Scratching changes which portion of the stable timeline is being viewed, not where bucket edges are.

### Prefer stable placement over perfect instantaneous precision

This scope is a perceptual live display. Preventing shimmer matters more than perfectly re-solving every transient per frame.

---

## One-line design summary

> Convert live scope rendering from a row-sampled transient envelope into a persistent time-bucketed envelope cache, where each bucket accumulates progressively until full and remains fixed afterward, so peaks stay in stable temporal positions and scratching does not reshuffle the visual grid.
