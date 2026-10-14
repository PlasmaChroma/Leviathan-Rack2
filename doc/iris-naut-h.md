# Iris and Nautiloid Rendering/Zoom Implementation Plan

## Purpose

This document turns the Iris/Nautiloid rendering review into an implementation
plan. The primary goals are:

1. keep zoom and panning responsive without wasting CPU work;
2. make graphics-context loss and recreation safe;
3. preserve the last known-good Iris sound across worker failures;
4. reduce large image copies, mutex hold times, and GPU image churn;
5. retain visual and audio behavior unless a change is explicitly called out.

This work is scoped to `Iris`, `Nautiloid`, their widgets, their private worker
pipelines, and shared graphics helpers where the helper is genuinely reusable.
It does not require changes to Integral Flux or Proc.

## Progress checkpoint (2026-08-31)

- Phase 0 instrumentation is in place; the complete manual baseline scenario
  capture remains part of final validation.
- Phase 1 is complete: display and Iris requests have independent identities,
  Iris work is demand-gated, interaction updates use worker-paced one-active/
  newest-pending coalescing, and final or invalidated Iris work is cancellable
  between rows. Patch restore queues its initial authoritative Iris render once
  rather than repeatedly cancelling slower modes while generation zero is
  pending; Spider and Barnsley restore coverage guards this startup path.
- Phase 2 is complete: CPU fallback workers start lazily, park/wake without UI
  thread joins, and join only during module destruction.
- Phase 3 is complete: Nautiloid follows the shared GL lifecycle pattern,
  forgets stale context-owned names safely, validates ready shader programs,
  and retries initialization after context recreation.
- Phase 4 is complete: failed Iris worker requests preserve the published
  table/source/preview, missing-file reloads can rebuild the retained source
  while keeping the read error visible, preview construction occurs outside
  the snapshot lock, and rebuild/reload requests share immutable source
  ownership instead of copying canonical pixels.
- Phase 5 implementation is complete: Iris and Nautiloid raster displays reuse
  validated NanoVG image handles with `nvgUpdateImage()`, acquire immutable
  presentation snapshots without full-frame UI copies, and no longer wrap the
  primary raster quad in a second framebuffer. Iris's waveform grid/content
  separation remains unchanged.
- Phase 6 is complete: the CPU fallback cache publishes immutable generations
  composed of shared tile frames, retains the previous generation by pointer
  across reset/recenter, and lets composite/reprojection readers work outside
  `cacheDataMutex`. Partial composites begin after useful center coverage, are
  limited to one per 60 ms while incomplete, and still publish the final crop.
  Interactive pan and zoom previews crop any requested viewport contained by
  the current generation's 3x world coverage before falling back to geometric
  reprojection, and defer speculative zoom-ahead work during drag instead of
  updating only the independently paced Iris source. Brief tile smearing can
  still appear during interaction, but the authoritative final render replaces
  it; this is accepted for the rarely used fallback path.
- Phase 7 is complete: the GPU preview now uses the shared adaptive GL surface
  with Nautiloid-specific stable 4x-capacity front/back buffers, 16-pixel
  active-extent quantization, active-prefix rendering, and context-safe
  recreation. Fractal slider, pan, and wheel interaction render at current
  Rack density, then refine to at least 2.7x density after settling. It
  publishes surface render time, density, and active/capacity dimensions to the
  fractal pipeline log.
- Phase 8 implementation is complete: an actual-framebuffer world-pixel test
  keeps shallow views on the fast float shader and lazily selects per-mode
  double-single variants for deep views. Center, span, mode constants, and
  orbit arithmetic retain high/low components; failed deep compilation falls
  back to the fast GPU shader. The experiment defaults off and is exposed as a
  per-module context-menu option for direct fast/deep A/B comparison. Live
  driver compilation and visual comparison against CPU output at maximum zoom
  remain to be validated in Rack.
- Focused Phase 4-8 worker, runtime, cache-policy, adaptive-surface,
  precision-policy, and lifecycle-contract tests and the local Linux plugin
  build pass. Manual Rack
  zoom/context validation
  remains part of final acceptance.
  The native MINGW64 `test-fast` suite and authoritative Windows `plugin.dll`
  build passed through the original Phase 3 checkpoint; native validation of
  the worker-paced Phase 1 follow-up and Phases 4-8 remains pending where the
  documented Windows bridge is available. Phase 8 live validation is next.

## Current architecture

Nautiloid currently produces two distinct render products:

```text
Fractal state
  |-- GPU display shader ----------------------> Nautiloid display
  |-- CPU display/reprojection/tile workers ---> CPU display fallback
  `-- 1024x256 Iris source worker -------------> Iris expander source
                                                   |
                                                   `-> Iris wavetable worker
```

The separation is correct: the GPU display is screen-resolution artwork, while
the Iris product is canonical wavetable source data. The problem is that the
Iris branch is currently requested as though it were another display preview,
even when no Iris consumes it. The CPU fallback branch is also started before
the UI has established whether the normal GPU renderer is available.

Iris uses a worker-owned triple-buffered wavetable pipeline for audio, but its
UI snapshots and NanoVG images still follow a copy/convert/delete/recreate
pattern. Its NanoVG context ownership is already handled correctly with
`NvgGraphicsLifecycle.hpp`.

## Required invariants

The implementation should preserve these rules throughout the work:

- The audio thread must not allocate, join a thread, wait on a mutex, or copy a
  source image/table.
- A completed user interaction must always result in one authoritative final
  state, even if intermediate work was coalesced or cancelled.
- Stale display or Iris work must never overwrite a newer result.
- A failed Iris load/rebuild must not replace the current audible table.
- GL and NanoVG names are context-owned and must not be reused after a context
  replacement without validation/recreation.
- `Process`, `Step`, and `Draw` retain their module-level telemetry meanings.
- GPU and CPU paths should represent the same location and palette within the
  precision limits documented below.

## Phase 0: establish measurements

### Add Nautiloid baseline telemetry

Iris already publishes the standard Debug Terminal `Process`, `Step`, and
`Draw` metrics. Nautiloid should gain the same module/widget aggregation before
performance changes are evaluated.

Keep the existing fractal-pipeline counters. They are component metrics and
should appear after the three stable macro metrics rather than replacing them.
Useful additional fields are:

- GPU surface render CPU time;
- CPU display renders and stale drops;
- Iris renders and stale drops;
- cache composite publishes;
- bytes copied into presentation/preview products, at least in debug builds;
- active GPU density and framebuffer dimensions;
- worker-start, worker-stop, and synchronous-join counts.

### Baseline scenarios

Capture measurements for:

1. Nautiloid idle at 100%, 200%, and 400% Rack zoom;
2. continuous slider zoom with no Iris attached;
3. the same interaction with an Iris attached and accepting Nautiloid input;
4. wheel zoom and drag pan;
5. GPU preview disabled to exercise CPU fallback;
6. Rack/DAW editor close and reopen;
7. cloning Nautiloid and cloning a Nautiloid+Iris pair.

No artificial UI framerate cap is part of this plan.

## Phase 1: separate display requests from Iris-source requests

### Problem

`requestInteractiveZoomPreview()` currently submits a 1024x256 Iris render on
every interactive preview request. The slider can issue these requests every
40 ms. This occurs even when no Iris is attached.

Interactive requests also reuse `nextRequestSerial` instead of advancing it.
The Iris worker can therefore finish different source images carrying the same
publication generation. Consumers watching `irisPreviewGeneration` cannot
reliably observe those intermediate results.

### Request types

Replace the shared serial coupling with distinct request identities:

```cpp
struct DisplayWorkerRequest {
  FractalState state;
  double cacheCenterX = 0.0;
  double cacheCenterY = 0.0;
  bool forceCacheRecenter = false;
  bool interactionActive = false;
  uint64_t serial = 0;
};

struct IrisWorkerRequest {
  FractalState state;
  int colorMode = COLOR_PRISM;
  uint64_t serial = 0;
  bool authoritative = false;
};
```

Maintain `nextDisplayRequestSerial` and `nextIrisRequestSerial` separately.
Every submitted Iris request receives a new Iris serial. Publication must check
that serial against the newest desired Iris serial immediately before changing
`irisCompatibleSource`, `irisExpanderOwnedSource`, or
`irisPreviewGeneration`.

### Consumer demand

Track whether the Iris product has a consumer. The demand state may be an
atomic written by `process()` from the expander relationship already computed
there. It should distinguish:

- no Iris attached;
- Iris attached but intentionally retaining an image source;
- Iris attached and accepting Nautiloid updates;
- an explicit `requestIrisSourceSync()` that overrides normal demand gating.

With no consumer, ordinary display zoom/pan/color changes must not render a
canonical Iris source. Attaching or explicitly syncing Iris must submit the
current state, so nothing needs to be pre-rendered merely in case an Iris is
attached later.

### Interaction policy

Recommended initial behavior:

- GPU/CPU display previews remain responsive at their existing cadence.
- With no consuming Iris, submit no Iris work during or after interaction.
- With a consuming Iris, offer intermediate states independently of display
  rendering. Keep at most one active render and one coalesced newest pending
  state, allowing the worker to begin the pending state immediately when it
  becomes idle.
- On drag/slider/wheel completion, always submit one authoritative Iris request
  when Iris demand is active.
- Color-only changes reuse the canonical uncolored Iris source and publish a
  newly colored source without rerunning the fractal solver.

Centralize this scheduling rather than maintaining unrelated timing behavior
in the slider, wheel, and pan widgets. Preview offers must not cancel the
single active preview merely because a newer state is pending; otherwise fast
input can starve a slower renderer. A final request still supersedes active
preview work immediately. A module method accepting an interaction phase
(`Preview` or `Final`) is preferable to each widget deciding worker policy.

### Stale cancellation

Latest-request-wins queuing prevents a backlog but does not stop a source that
is already rendering. Add a cheap cancellation check at least once per output
row in the offline fractal generator. The check should compare the request's
Iris serial with the current cancellation authority and also honor module
shutdown. A merely pending preview does not take that authority until the
worker activates it; a final request or lifecycle invalidation does so
immediately.

Avoid `std::function` in the inner pixel path. A small cancellation token,
function pointer plus context, or a dedicated cancellable worker overload is
adequate because this is worker/offline code. Do not add atomics to each pixel
iteration.

### Acceptance criteria

- Zooming Nautiloid with no Iris attached produces zero Iris renders.
- An attached Iris receives monotonic generations.
- A superseded Iris render cannot publish after a newer result.
- Continuous movement runs at worker capacity without a fixed producer-side
  delay or an unbounded request backlog.
- Interaction end produces the exact final Nautiloid state in Iris.
- Color changes do not rerun canonical fractal iteration when geometry is
  unchanged.

## Phase 2: make GPU/fallback worker lifecycle non-blocking

### Problem

The module constructor requests a render before the widget has proved that the
GL shader is usable. That starts the display, cache, and reprojection workers.
When the first GPU frame succeeds, `setGpuPreviewAvailable()` synchronously
calls `stopFallbackWorkers()`, which joins those workers from UI rendering.
The monolithic 768x512 display render cannot currently cancel between rows, so
first display, cloning, or context restoration can stall.

### Recommended lifecycle

Use these states conceptually:

```text
GPU probe pending -> GPU active
                  -> CPU fallback active
CPU fallback active -> GPU active, fallback workers parked
```

Implementation details:

1. Do not start fallback workers from the module constructor merely because
   GPU availability is not known yet.
2. Let the widget report one of:
   - GPU renderer ready;
   - GPU renderer failed/disabled and CPU fallback required;
   - no display consumer.
3. Lazily create fallback workers only after fallback is genuinely required.
4. Once fallback workers have existed, park them when GPU rendering resumes
   instead of joining them from `step()` or `drawFramebuffer()`.
5. Join workers only during module destruction, or from a demonstrably safe
   non-render lifecycle point.
6. Add per-row cancellation to the monolithic display render so shutdown and
   mode transitions are bounded.

Keeping three fallback threads alive for every normal GPU Nautiloid would waste
stack/address-space resources. The important combination is therefore *lazy
creation plus parking after first use*, not unconditional construction.

### Acceptance criteria

- No `join()` is reachable from widget `step()`, `draw()`, or
  `drawFramebuffer()`.
- Normal GPU startup creates only the always-needed worker(s), not the three
  fallback workers.
- Disabling GPU preview starts fallback lazily and still produces a display.
- Re-enabling GPU preview cannot block on a complete CPU fractal render.

## Phase 3: adopt the shared GL lifecycle pattern

### Nautiloid GL widget changes

Bring `NautiloidGlPreview` in line with Bifurx and the repository lifecycle
standard:

- implement `onContextCreate()`;
- forget all old program/shader names without issuing deletion calls when the
  owner context is uncertain;
- clear `initAttempted`/`ready` flags and mark the display dirty;
- track the owning render context and reset if it changes unexpectedly;
- validate ready programs before reuse, at least when extra GL validation is
  enabled, and rebuild lazily on failure;
- keep destructor cleanup non-GL, as it is today.

If the shared helper is extended, add a focused program/shader validation
operation to `GlLifecycleUtils` rather than embedding another module-specific
interpretation. Do not broaden the helper into a Nautiloid resource graph; the
mode-program array and its reset ordering remain local.

A failed shader compilation should remain diagnosable, but a transient failure
must not make `initAttempted=true` permanent across a new graphics context.

### Acceptance criteria

- Editor close/reopen reconstructs the active fractal without stale GL names.
- Context recreation does not require recreating the Rack module.
- A failed or invalid program falls back safely and can retry after context
  recreation.
- No GL deletion occurs from the widget destructor without a known current
  owner context.

## Phase 4: preserve Iris's last known-good state

### Failure behavior

The Iris worker currently constructs and publishes the default table after any
non-default worker request fails. Replace that behavior with:

1. keep `activeTableIndex`, pending table state, `snapshotTable`, source, and
   preview unchanged;
2. publish the error string and `loadFailed=true`;
3. set `loading=false`;
4. release any acquired expander source slot;
5. leave the worker build slot available for the next request.

The explicit `REQUEST_DEFAULT` path remains the only normal operation that
replaces the sound with the default table.

If a reload cannot read the file but successfully rebuilds the already retained
source, the rebuilt table may publish while the reload error remains visible.
Document and test this distinct partial-success case.

### Build previews outside the snapshot lock

`publishWorkerResult()` currently copies the table and builds the entire
1024x256 converted preview while holding `snapshotMutex`. Build temporary
publication products first:

```cpp
std::vector<uint8_t> nextPreview;
buildPreview(table, &nextPreview);

{
  std::lock_guard<std::mutex> lock(snapshotMutex);
  // Move/copy only the final snapshot state here.
  snapshotPreview = std::move(nextPreview);
}
```

The audio thread does not use this mutex, but shorter holds prevent display,
status, serialization, and waveform requests from stalling each other.

### Reduce source copies

Use owned immutable source references for rebuild and expander requests where
possible. The existing `ownedSource` path is the right direction. A practical
incremental design is:

```cpp
using SourcePtr = std::shared_ptr<const iris::SourceField>;
```

- publish a `SourcePtr` for the current UI/source snapshot;
- place the same pointer in rebuild requests;
- copy source pixels only when an operation genuinely mutates them;
- retain the audio wavetable triple buffers independently.

This is UI/worker ownership, not an audio-thread shared-pointer exchange.

## Phase 5: reuse NanoVG images and remove redundant frame copies

### Stable image handles

`IrisDisplay`, `NautiloidDisplay`, and `NautiloidIrisMiniDisplay` delete and
recreate their NanoVG images whenever a generation changes. When the context
and dimensions are unchanged:

1. validate the existing handle with the shared lifecycle helper;
2. update its pixels with `nvgUpdateImage()`;
3. recreate only when the context, dimensions, or validation state changed.

Preserve the current rule that an image handle is never deleted through a
different `NVGcontext*`.

If this sequence is repeated in three widgets, add a narrow helper to
`NvgGraphicsLifecycle.hpp`. The helper should own only lifecycle/update policy,
not module-specific RGB-to-RGBA conversion.

### Snapshot ownership

Avoid this per-generation chain:

```text
worker/source vector -> mutex-protected snapshot copy -> UI RGB copy
                     -> UI RGBA conversion -> new NanoVG image
```

Publish an immutable RGB frame or `SourceField` behind shared ownership. The UI
can acquire it with a brief locked pointer copy (or C++11 atomic shared-pointer
free functions), release the mutex, convert into its persistent RGBA scratch
buffer, and update the existing NanoVG image.

For Nautiloid CPU display, keep the canonical preview uncolored so a palette
change can recolor the persistent RGBA buffer without rerunning fractal
geometry. For Iris, source-channel filtering remains a UI-derived view of the
same immutable source.

### Outer framebuffer decision

After image-update reuse is in place, benchmark removing the outer
`FramebufferWidget` around `IrisDisplay` and the CPU `NautiloidDisplay`. Each
widget already draws one persistent texture; caching that quad into a second,
Rack-zoom-dependent texture may cost more memory and framebuffer churn than it
saves.

Do not remove the waveform grid/content separation in Iris. The cached waveform
and live scan/phase overlays are appropriately separated.

## Phase 6: fix the CPU fallback cache architecture

### Remove the per-tile full presentation copy

The display cache is 2304x1536 RGB:

```text
2304 * 1536 * 3 = 10,616,832 bytes (about 10.1 MiB)
```

There are 216 128x128 tiles. Copying `stitchedRgb8` into
`displayPresentationCache.rgb8` after every tile creates more than 2 GiB of
memory traffic during a complete cache fill.

The immediate low-risk correction is:

- retain the old presentation cache once, immediately before a cache reset or
  incompatible recenter;
- do not mirror the current stitched cache into the presentation cache after
  every new tile;
- sample current data from `displayTileCache` and retained data from the frozen
  presentation generation;
- if a current presentation representation is required, update only the
  completed tile rectangle with `PresentationLayer::writeTile()`.

### Publish composites by time and usefulness

The current fixed interval of four tiles can publish up to 54 768x512 preview
frames per complete fill. Replace it with a time-aware policy:

- publish once when enough center tiles exist to materially improve the view;
- publish no faster than approximately 50-70 ms while incomplete;
- always publish the final complete result;
- do not publish if the display serial became stale.

The exact interval should be selected from telemetry. The goal is limiting
full-frame crop/upload work, not limiting Rack's overall UI framerate.

### Immutable cache generations

`publishDisplayReprojection()` currently makes large local cache copies while
holding `cacheDataMutex`, then performs a full 768x512 reprojection within the
same lock scope. Replace mutable cross-worker observation with immutable cache
generation objects:

```cpp
struct DisplayCacheGeneration {
  // Geometry metadata, RGB storage, and tile-valid map.
};

using DisplayCacheGenerationPtr =
  std::shared_ptr<const DisplayCacheGeneration>;
```

Writers build or update private storage and publish a new immutable generation
at useful boundaries. Reprojection acquires pointers briefly and processes
without holding `cacheDataMutex`. Pool/reuse the large backing allocations so
shared ownership does not imply repeated 10 MiB allocation.

### Acceptance criteria

- No complete 2304x1536 image copy occurs after each tile.
- Reprojection does not hold `cacheDataMutex` for a full output frame.
- Partial publications are bounded by time and finality, not merely tile count.
- CPU fallback remains visually continuous during pan/zoom.

## Phase 7: adaptive GPU surface for Rack zoom

Nautiloid's `OpenGlWidget` correctly calls `FramebufferWidget::step()`, so it
already receives Rack's normal framebuffer caching behavior. The remaining
opportunity is to avoid framebuffer reallocations and unbounded fragment work
at extreme Rack zoom.

Adapt the `AdaptiveGlSurface` pattern used by Bifurx:

- allocate stable maximum-density front/back surfaces;
- quantize active extents to avoid small reallocations;
- render only the active prefix;
- cap density at a measured value (the shared starting policy is 0.25x-2x with
  a 16-pixel quantum);
- redraw at lower density during continuous fractal interaction if telemetry
  demonstrates a benefit;
- mark dirty and refine at the current Rack density when interaction ends.

Nautiloid's fractal state changes are independent of Rack view zoom. Do not
confuse the module's fractal zoom with the surface's pixel density.

For Iris's raster image display, an adaptive GL render surface does not create
new source detail. Its canonical source is 1024x256. Prefer stable image
sampling and avoiding a redundant outer framebuffer rather than up-rendering
the raster through a larger offscreen surface.

## Phase 8: deep-zoom GPU precision

### Problem

Nautiloid stores center coordinates as doubles but uploads them with
`glUniform2f`. At deep zoom and centers away from zero, float spacing becomes
larger than a display pixel in fractal space. This can cause visible multi-pixel
panning steps and disagreement with the double-precision CPU/Iris generators.

### Fast and precise shader variants

Keep the current float shader for ranges where it is accurate. Select a deep
precision variant when the float unit-in-last-place at the center becomes too
large relative to a display pixel:

```text
world pixel size = 2 * viewport half-span / framebuffer extent
```

Use a conservative threshold such as float center error below one quarter of a
world pixel. Compute the decision on the CPU from the actual framebuffer size,
center, mode, and fractal zoom.

The deep variant should represent coordinates as high/low float pairs and
carry double-single arithmetic through the orbit calculation. Merely uploading
`centerHigh + centerLow` and immediately adding them into one float does not
solve the problem.

Compile the deep variant lazily and only for modes that need it. If shader
compilation cost becomes visible, prewarm one program per idle UI step rather
than compiling all modes in one frame.

### Precision validation

Add deterministic reference locations near center magnitudes of 0, 0.5, 1,
and 2 at shallow and maximum zoom. Verify:

- one-screen-pixel pan steps remain observable;
- cursor-centered wheel zoom preserves its focus point;
- GPU classification/color structure agrees with CPU reference samples within
  an explicitly chosen tolerance;
- context fallback does not jump to a materially different location.

## UI cadence and small rendering work

After the structural work, apply these low-risk cleanups:

- Route wheel zoom through the same centralized preview/final request policy as
  slider zoom. It currently submits both an interactive and a full request for
  every wheel event.
- Route drag pan through the same policy rather than maintaining an independent
  timer.
- Cache Iris frequency and Nautiloid position/zoom strings until their rounded
  displayed values change.
- Provide a deterministic Nautiloid null-module/browser preview rather than an
  empty black display.
- Keep live scan lines and phase tracers out of the expensive cached image
  generation path.

## Tests

### Automated tests

Add focused tests for:

- display and Iris serials advancing independently;
- no Iris request when no consuming Iris is present;
- forced sync bypassing normal demand gating;
- stale Iris results being rejected;
- final interaction state always being published;
- color-only reuse of canonical geometry;
- Iris worker failures preserving the last known-good table/source/preview;
- reload partial-success behavior;
- cache recenter/shift correctness after removing per-tile presentation copies;
- time-bounded composite publication decisions using an injected/test clock;
- cancellation leaving the previously published result valid;
- location-code round trips retaining byte-for-byte Iris source equivalence.

The existing `iris_wavetable_spec` and `nautiloid_location_code_spec` should
remain part of `test-fast`. Worker/lifecycle logic may need a small testable
coordinator extracted from Rack-facing classes rather than attempting to drive
the full Rack UI in a standalone test.

### Manual Rack validation

Test at minimum:

1. add, clone, delete, and reload both modules;
2. attach/detach Iris while Nautiloid is idle and while it is moving;
3. keep Iris on an image source while attached, then explicitly request sync;
4. zoom with slider, wheel, CV, and cursor-centered focus;
5. pan manually and by CV at shallow/deep zoom;
6. switch every fractal and color mode;
7. disable/re-enable GPU preview;
8. close/reopen the Rack/DAW editor;
9. test 100%, 200%, and 400% Rack zoom;
10. force an Iris load failure and confirm that audio does not change.

## Build and validation sequence

During iteration, use focused WSL tests and compilation. Before considering a
phase complete, run the authoritative native Windows suite from the documented
MINGW64 bridge:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 test-fast \
     RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"'
```

Then build the authoritative Windows plugin incrementally:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 plugin.dll'
```

The final acceptance step is a Rack runtime smoke test, particularly for GL
context recreation and interaction latency.

## Recommended implementation order

1. Add Nautiloid telemetry and capture baselines.
2. Split display and Iris request identities; gate Iris work by demand.
3. Remove UI-thread fallback-worker joins and add cancellation.
4. Adopt shared GL lifecycle validation/reset behavior.
5. Preserve Iris's last known-good table on failures.
6. Build Iris publication products outside the snapshot lock.
7. Reuse NanoVG images and reduce snapshot copies.
8. Remove the fallback cache's full presentation copy per tile.
9. Publish immutable cache generations and shorten cache locks.
10. Add the adaptive GPU surface and tune density from measurements.
11. Add the deep-precision shader variant.
12. Complete manual Rack/context validation.

The first five steps are the safest high-value tranche. Adaptive rendering and
deep-precision shader work should follow measurement and correctness cleanup,
not precede them.
