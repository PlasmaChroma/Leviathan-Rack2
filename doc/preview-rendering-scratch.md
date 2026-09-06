# Preview rendering experiments

Working tracker for Proc first, then Integral Flux and Undertow where results
justify it. Preserve the current appearance, released state/IDs, and module-level
Process / Step / Draw telemetry. Phosphor remains a Dragon King experiment.

## Evidence and scope

- Live observation: the current phosphor tracer is slower than Proc's stock tracer.
- Source audit: preview publication versions can change without changing the
  effective rise fraction or shape; stock history currently captures on versions.
- Source audit: an allocated inactive buffered tracer repeatedly zeros its pixels.
- Hidden native Windows GL contexts work here. Offline CPU and GPU comparisons
  are possible, but Rack frame pacing and visual quality still need live checks.

## Experiments

1. Geometry-key invalidation and idempotent buffered clearing. Benchmark scheduler
   decisions and retained-buffer clearing offline; verify slow accumulated changes,
   timing clamps, resize, and pending upload state. No new renderer in this step.
2. Preview-specific measurements: rebuild/simplification, captures, contour
   submission, history updates, allocations, marker/text. Preserve macro timings.
3. Compare a separately retained contour against direct NanoVG rendering using
   stationary and continuously changing shapes. Keep marker and label independent.
4. Change phosphor to update history only on deposits and decay during composition.
   Treat persistence changes and premultiplied alpha explicitly; test faint tails.
5. Consider snapshot history or retained stroke meshes only if measurements support
   them. Keep scheduling, stroke rendering, and resource ownership experiments separate.
6. Follow-ups: Integral Flux layer invalidation; Undertow retained simplification.

## Measurement rules

Use warm-up, repeated batches, median and p95 durations, identical workloads and
build flags. Clearly distinguish scheduler microbenchmarks, actual geometry work,
CPU rendering submission, GPU completion, and whole-module/frame measurements.
Do not infer FPS gains from avoided rebuild counts. GPU queries must be retrieved
asynchronously; no glFinish in the normal profiling path.

Live matrix: idle, marker-only, common-rate modulation, genuine shape modulation,
fade-only; normal/high zoom; one/many instances; context close/reopen.

## Results

### First baseline pass — 2026-09-05

Implemented in Proc: normalized geometry-key acceptance independent of publication
version; marker/frequency remain live; history captures follow accepted geometry;
inactive tracers clear on backend transitions. Resize resets history instead of
capturing coordinates from the old dimensions. Ratio tolerance is only 1e-6 to
absorb floating-point noise, measured against the last accepted key; shape keeps
its exact existing LUT comparison. This is not a screen-space simplification policy.

Shared buffered clearing now skips already-transparent storage and preserves an
outstanding clear upload. The tool exercises actual capture followed by clearing,
initial/finished/pending uploads, proportional timing, slow movement, asymmetry,
shape changes, and resize.

Native MINGW64, repository O3/fast-math flags, 5 warm-up batches then 50 measured
batches of 2,000 operations. Numbers below are median / p95 **batch-average** ns
per operation, not individual-call tail latency:

| Operation | Median ns | p95 ns |
| --- | ---: | ---: |
| Old repeated clear, 106×48 | 274.35 | 281.95 |
| Old repeated clear, 212×96 | 1391.35 | 1419.25 |
| Old repeated clear, 424×192 | 6050.80 | 6320.85 |
| Idempotent clear, all three sizes | 1.30 | 1.30 |
| Geometry key, common-rate workload | 1.95 | 1.95 |
| Geometry key, changing ratio | 1.50 | 1.55 |

The scheduler fixture accepts zero additional keys for 12,000 proportional timing
updates after initialization. A version-per-update baseline would request 12,000
rebuilds/capture attempts. This counts decisions, not actual geometry generation,
successful captures, or GPU work. Ordinary instances that never allocated the
frame-cache buffer did not have the measured pixel-clearing cost.

Validation: native plugin.dll linked; all 10 Proc runtime tests and the benchmark's
contract checks passed. Live visuals and total draw-time improvement remain unmeasured.

Reproduce inside the documented native MINGW64 environment:

```sh
make -j10 build/tools/preview_invalidation_benchmark
export PATH="/c/Program Files/VCV/Rack2Pro":/mingw64/bin:/usr/bin
./build/tools/preview_invalidation_benchmark.exe
```

Next: measure actual Proc preview work separately, then compare direct and retained
contour rendering. No new contour cache or phosphor scheduling change in this pass.

### Checkpoint feedback

The user tested this pass in Rack, noticed no behavior changes, and reported a
possible performance improvement. This is qualitative evidence, not a measured
speedup. The user committed the baseline before the next experiment.

### Actual contour benchmark — 2026-09-05

Moved the existing eight geometry/stroke methods into `ProcPreviewGeometry.hpp`.
The live widget and benchmark use the same implementation and Proc's own shape
math. Verified extracted arithmetic and NanoVG drawing calls against the checkpoint.
The production rendering backend is still direct NanoVG.

`tools/proc_preview_render_benchmark.cpp` creates a hidden native Windows GL
context and uses Rack's NanoVG GL2 implementation with antialiasing and stencil
strokes. It compares direct strokes with independently allocated retained contour
surfaces, one per simulated instance. Both render into the same output target.
Scenarios: 1 and 16 contours, 1× and 4× raster scale, stationary and changing rise
ratio. The render fixture uses shape=0.6; it is not an exhaustive shape sweep.

Each scenario discards 100 warm-up frames and measures 500 frames. CPU timings
include NanoVG frame submission and framebuffer switching; GPU elapsed queries
cover that rendering work and are collected asynchronously. Geometry generation,
allocations, query polling, glFlush, and pixel readback are outside CPU render
timings. No glFinish is used. All 500 GPU samples were obtained in every scenario
in both final runs. GPU timing shows quantization at these small durations.

RTX 3090, NVIDIA 560.94, native MINGW64 O3/fast-math. Representative first-run
medians in microseconds (CPU and GPU are distinct measurements, not additive):

| Workload | Direct CPU | Cached CPU | Direct GPU | Cached GPU |
| --- | ---: | ---: | ---: | ---: |
| Stationary, 1 contour, 1× | 11.3 | 0.7 | 6.144 | 5.120 |
| Changing, 1 contour, 1× | 11.5 | 12.2 | 6.144 | 16.384 |
| Stationary, 16 contours, 1× | 169.6 | 4.5 | 32.768 | 8.192 |
| Changing, 16 contours, 1× | 172.0 | 188.4 | 32.768 | 153.600 |
| Stationary, 1 contour, 4× | 11.2 | 0.6 | 7.168 | 6.144 |
| Changing, 1 contour, 4× | 11.4 | 12.1 | 6.144 | 17.408 |

Separate geometry benchmark, median batch-average microseconds:
0.490 for simplification alone, 1.407 for ratio rebuild with warm LUTs, and 12.241
for shape rebuild including LUTs. Fifty measured batches of 1,000 operations after
five warm-up batches. These operations overlap: do not add them together.

The repeat run preserved the tradeoff: stationary single-contour CPU 12.2 vs 0.7 µs;
changing single-contour GPU 6.144 vs 16.384 µs. Full distributions (median/p95),
sample counts, hardware identity, and pixel errors are in
[run 1](benchmarks/proc-preview-contour-run1.txt) and
[run 2](benchmarks/proc-preview-contour-run2.txt).

Outside timing, compare each scenario's final cached output against direct output.
All single-contour cases matched byte-for-byte; the 16-contour cases differed by
at most 1 in an 8-bit channel. GL error checks passed. This covers the sampled
contours and aligned scales, not highlights, fractional zoom, themes, or DAW reopen.

Interpretation: a retained contour is promising after geometry/appearance settles.
Continuously rebuilding it is counterproductive, particularly on the GPU. Next
candidate: optional settle-then-cache behavior with direct drawing while geometry,
highlight color, or resolution changes. Keep marker, text, and history independent.
Do not infer Rack frame-time gains from this isolated, unpaced offscreen workload:
widget traversal, shared NanoVG batching, live marker/text, history, grid, surface
lifecycle, and application frame pacing are not included. Preview-specific live
telemetry remains pending; existing Process / Step / Draw metrics are unchanged.

Validation: native plugin.dll build and all 10 Proc runtime tests passed. No cache
backend has been enabled in the live widget by this measurement pass.

Reproduce inside native MINGW64:

```sh
make -j10 build/tools/proc_preview_render_benchmark
export PATH="/c/Program Files/VCV/Rack2Pro":/mingw64/bin:/usr/bin
./build/tools/proc_preview_render_benchmark.exe
```

### Adaptive contour cache in Proc — 2026-09-05

Implemented automatic direct-while-changing / cached-when-settled rendering in
the live widget. After 100 ms with unchanged geometry, highlight selection/color,
dimensions, transform scale, and display density, the contour renders once into
a Rack FramebufferWidget. Changes immediately bypass that cache and restart the
settlement interval. No menu setting or saved-state change is required.

The contour surface is drawn explicitly after history and before marker/text.
Its normal tree draw does nothing; normal stepping/context events still propagate.
The marker, frequency, grid and history do not invalidate it. Nested framebuffer
draws and rotated/skewed transforms use the direct path. Raster resolution follows
Rack's current scale after settlement, rather than allocating repeatedly during zoom.

Rack owns the framebuffer resources and context events; cached images are checked
through the shared NanoVG lifecycle helper before reuse. Context events restart
settlement. A missing image falls back to direct drawing. A settled dirty image is
rendered before compositing to avoid displaying stale geometry if Rack would defer
the update. Rack's bypass/deferred-render behavior was checked against its
[FramebufferWidget implementation](https://github.com/VCVRack/Rack/blob/v2/src/widget/FramebufferWidget.cpp).

Validation: native Windows plugin.dll linked, all 10 Proc runtime tests passed,
and settlement checks passed for initial draw, unchanged keys, all key components,
100 ms eligibility, return to direct drawing, and context reset. The previous
synthetic rendering timings establish the hypothesis, not this integration's speed.

Next live check: slow/fast knob and CV changes, release then wait, hover highlights,
moving marker on a stable shape, normal/fractional/high zoom, and DAW close/reopen.
Look for any flash or contour shift when the cache becomes active; compare Draw
timings for stationary and continuously changing contours. Not installed by Codex.

### Phosphor history updates only on deposits — 2026-09-05

Adaptive contour caching was checkpointed after the user reported no visible
problems in Rack; a ~10 µs effect was hard to discern in whole-module telemetry.
The following experiment is independent of that cache.

Phosphor now retains history textures between captures and applies exponential
gain when drawing the existing presentation framebuffer. On a deposit, it folds
elapsed decay into the destination history texture and stamps the outgoing curve.
Both premultiplied RGB and alpha receive the same gain. Resource arrangement,
stroke ribbons, capture cadence and context protections remain unchanged.

Presentation still renders during visible fading. This removes the history
update pass, not all offscreen work. Idle expiry still stops dirtying. Accumulated
decay age integrates with the previously observed persistence; changing settings
affects future decay without jumping current brightness. Faint tails can differ
from the previous implementation because history no longer rounds to RGBA8 every
frame. Expiry follows accumulated decay age rather than retroactively applying a
new persistence to the entire time since the last deposit.

`tools/proc_phosphor_benchmark.cpp` compares the actual old/new renderHistory
paths using Proc's sampled contour, in the same hidden Windows GL context. The old
header is generated into build/tools from immutable Git blob
`2d1930c16b7db916ccc53bd05cec91430b7d5457`, avoiding a maintained duplicate renderer.
The new common benchmark utility retains asynchronous GPU-query collection.

RTX 3090 / NVIDIA 560.94 / native O3-fast-math, 100 warm-up and 500 measured
frames per case. Two runs, 1/16 instances, 1×/4× resolution, fade-only or 20 Hz
deposits on a simulated 60 Hz presentation timeline. Fade-only intervals are
seeded outside timing once per simulated second; persistence is 1.2 s for both
renderers. All GPU samples were retrieved in both runs.

Representative first-run medians (microseconds):

| Workload | Old CPU | New CPU | Old GPU | New GPU |
| --- | ---: | ---: | ---: | ---: |
| Fade-only, 1 instance, 1× | 7.8 | 7.3 | 8.192 | 3.072 |
| 20 Hz deposits, 1 instance, 1× | 9.1 | 8.8 | 8.192 | 4.096 |
| Fade-only, 16 instances, 1× | 420.5 | 229.3 | 122.880 | 28.672 |

Repeat single-instance fade-only medians: CPU 8.3 → 7.5 µs, GPU 8.192 → 3.072 µs.
Fade-only history swaps: 500 → 0 per measured instance. With 20 Hz deposits:
500 → 166. Deposit-heavy p95 timings are mixed; do not claim every workload is
faster. Full median/p95/sample counts are in
[run 1](benchmarks/proc-phosphor-lazy-run1.txt) and
[run 2](benchmarks/proc-phosphor-lazy-run2.txt).

CPU scope includes renderHistory and binding its target, but excludes capture,
query collection, flush, and readback. GPU scope covers the history/presentation
passes, not Rack's final NanoVG composite or complete module draw. Instances use
independent history resources and sequentially render to a shared test target;
this is not a live Rack frame-time measurement or a comparison against stock tracer.

GPU checks passed: byte-identical retained history during passive fade; decreasing
presentation brightness; persistence continuity; throttling; deposit accumulation;
stale capture rejection; expiry; toggles; resize; resource reset; source-over blend;
target binding restoration and no GL errors. The second run also verifies equal
elapsed decay across 1, 9, 18 and 43 presentation steps within one byte/channel.
Native plugin.dll and all 10 Proc runtime tests passed.

Reproduce in native MINGW64 (requires the baseline blob in local Git history):

```sh
make -j10 build/tools/proc_phosphor_benchmark
export PATH="/c/Program Files/VCV/Rack2Pro":/mingw64/bin:/usr/bin
./build/tools/proc_phosphor_benchmark.exe
```

Ready for live phosphor checks: release a changing curve and inspect the fade,
change persistence mid-tail, switch blend modes, zoom, and reopen a DAW editor.
Dragon King menu gating and saved settings are unchanged. Built, not installed.

### Integral Flux live baseline

The user's two-instance draw capture is preserved and analyzed in
[integral-flux-preview-baseline.md](integral-flux-preview-baseline.md).
Median combined preview CPU time: 28.0 µs / 110.8 µs, using NanoVG with stock
tracing. The step-derived counters have cross-instance attribution problems and
capture counts represent attempts; correct these before relying on them to verify
invalidation. Draw timings remain useful for the before/after comparison.

### Shared stock history investigation: Flux component logging

Undertow's idle/morph captures identify history submission as the dominant
modulated-preview cost; see [Undertow baseline](undertow-preview-baseline.md).
Flux now appends per-channel and combined history/contour CPU timings plus
submitted trail/point counts, preserving the earlier totals and rendering.
[Capture instructions and metric scope](integral-flux-draw-logging.md).
Next: capture idle, shape modulation, and fade-out in Flux, then decide on a
shared history experiment using Flux as a proxy for Proc. No new history backend
or capture policy is introduced by this instrumentation.

Flux's 21:39 component capture confirms the stock history bottleneck: with both
channels at six trails, median history 88.3 µs out of 114.7 µs preview, versus
17.3 µs current contour. CH4 history drops to 0.2 µs after expiration while
CH1 remains active. See the updated Integral Flux baseline for captured rows
and limitations. Next candidate is a retained snapshot history experiment,
measured against stock vectors before adoption.

### Retained stock snapshots — Flux experiment

Implemented `visual/SnapshotHistory.hpp` as a reusable widget with six retained
slots, sharing the stock vector capture ring. Integrated only into Flux as the
Dragon King **Snapshot cache (experimental)** option (appended mode 2). The
vector and buffered choices remain available. This preserves discrete linear
trail fading rather than phosphor accumulation.

`tools/snapshot_history_benchmark.cpp` uses Proc geometry, the shared snapshot
path drawing code, and native offscreen NanoVG/GL. It includes capture render
passes and NanoVG flushes but excludes Rack widget traversal, image validation,
allocation warmup and geometry generation. It tests 60 Hz presentation with a
20 Hz capture cadence, six slots, then fade and expiry at 1x/4x resolution.
Therefore its timings are backend evidence, not predictions of live Flux gains.
GPU timings cover all phases, including expired history. Image comparisons
sample modulation, fade and expiry; rasterization can differ slightly at edges.

Native results are saved in `benchmarks/snapshot-history-offline.txt`.
Next live test: same Flux workload, switch from Curve cache to Snapshot cache,
release CH4 modulation, and compare the CSV's history time and rasterization
counts. Also inspect zoom, mode switches and context recreation. No rollout to
Proc or Undertow until the Flux experiment is assessed.

Flux snapshot live validation: the 21:52:18 within-capture vector→snapshot
switch lowers full-history combined preview median from 111.8 to 42.3 µs.
The subsequent 21:52:40 run measures 31.6 µs; its 862 rasterizations match
862 captures, with no CH4 rasterization during release/fade. Detailed grouped
results are in the Integral Flux baseline. CPU benefit is established for
this workload; appearance, zoom and context recreation remain live checks
before rollout to the other previews.

### Proc and Undertow snapshot ports

Both now offer the shared **Snapshot cache (experimental)** option under
Dragon King Tracer Quality. Existing defaults and mode IDs 0/1 remain intact;
mode 2 round-trips in each module. Proc uses a separate internal phosphor
backend identifier and disables phosphor when snapshots are selected, avoiding
an ambiguous selection. Proc retains its geometry-change capture policy;
Undertow retains its own shape-change capture policy and line style.

Undertow adds visible trail/source-point/rasterization counts to its existing
CSV. Its history timing includes both slot creation and image reuse. The
current contour renderer is unchanged, so the new capture isolates the history
experiment. Next: a same-patch Undertow vector/snapshot comparison with morph
modulation and release, plus a Proc visual check of snapshots/phosphor switching,
zoom and context recreation.

Port validation: authoritative Windows plugin.dll link passed. Proc runtime
10/10 and Undertow shape 4/4 passed. Undertow module checks pass 12/14,
including the new default/mode round-trip check; the same two previously
identified monophonic summary/fingerprint baseline failures remain. No DSP
changes were made in this port. Built, not installed; live results pending.

Undertow live snapshot capture 22:07:58 independently confirms the port:
six-trail preview median 76.2→25.3 µs; history 60.0→9.0 µs. Fade has no
rasterizations. Mode selection initially caches six existing trails and costs
2.7 ms, so first-use behavior is a remaining caveat. Module-level outliers
outside preview prevent a whole-module tail-latency claim. Full analysis and
raw CSV are linked in the Undertow baseline.

### Snapshot history promoted to default

Proc, Integral Flux and Undertow now initialize to snapshot mode 2. Loading a
patch without `previewTracerCacheMode` selects snapshots even on an existing
instance. Explicit saved modes 0/1/2 remain intact; disabling Dragon King
preview options no longer forces vector history during load. The quality menu
remains a developer option, with snapshots labeled **Snapshot cache (default)**.
Proc phosphor stays separately opt-in; Flux's optional GL renderer still uses
its existing vector history. Default NanoVG rendering uses snapshots.

Tests cover new defaults, explicit saved modes, and missing-mode loads with
preview options disabled. This promotes the validated backend without migrating
explicit tracer choices in existing patches.

### PreviewWidgetOptions policy update

When `PreviewWidgetOptions` is disabled, all three modules now force snapshot
cache while loading, overriding saved vector/buffered choices. Proc also clears
its saved phosphor enable flag so it cannot override snapshot rendering; Flux
retains its existing forced NanoVG renderer behavior. When options are enabled,
explicit saved choices still round-trip. New instances already use snapshots.
This supersedes the preceding preservation policy for disabled preview options.

### Legacy saved default migration

The enabled-options loader previously restored legacy mode 0, overriding the
snapshot constructor default in old patches/templates. All three loaders now
migrate unversioned Curve cache settings to snapshots. New saves include
`previewTracerCacheVersion=1`, allowing explicit vector choices made after this
migration to survive reload when PreviewWidgetOptions is enabled. Legacy frame
and snapshot modes remain intact; disabled options still force snapshots.
Tests cover legacy migration and versioned mode round-trips.

### HaloKnob2 adaptive surface

HaloKnob2 now uses the shared `AdaptiveGlSurface` for its shader result. Each
46 x 46 knob reserves a quantized backing surface up to 3x density, retains
that capacity for the graphics context, and changes its active viewport as Rack
zoom changes. Normal value, bloom, and hover-state invalidation still redraws
the shader. The NanoVG browser/shader-failure fallback and context reset path
remain intact. This targets the many-per-module framebuffer reallocations seen
during zoom changes; live zoom profiling is still required to quantify the
reduction and check transient image quality.
