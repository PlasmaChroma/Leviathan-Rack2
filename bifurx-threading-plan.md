# Bifurx Threading Implementation Plan

## Purpose

This document turns `bifurx-perf-threads.md` into an implementation spec for moving Bifurx visual preparation work off the Rack UI thread.

The goal is not to make the audio engine multithreaded and not to draw from a worker thread. The goal is to let a background worker prepare the practical maximum amount of safe CPU-side visual work so the Rack UI thread spends less time doing FFT, response-curve, marker, animation-target, layout, and vertex preparation.

The worker should produce render-ready data, not rendered pixels. In this plan, "render-ready" means immutable arrays, layouts, labels, confidence/state flags, and CPU-side vertex buffers that the UI thread can consume with minimal branching and minimal recomputation. It does not mean calling NanoVG, OpenGL, Rack widget APIs, or uploading textures/buffers from the worker.

The short recommendation: this is feasible and likely worth doing if Bifurx UI cost remains material after the current redraw gating and OpenGL shader path. It should be implemented as an optional visual-worker pipeline with conservative fallback, not as a rewrite of the module or its DSP.

Because real patches can contain several visible Bifurx modules, the preferred implementation is a single plugin-level visual worker service, not one worker thread per module. The service should keep at most one pending/in-flight calculation per Bifurx display identity and should replace stale pending work with the newest request.

## Current Architecture

Bifurx currently has a good starting boundary:

- `Bifurx::process()` runs audio DSP and publishes small UI-facing state.
- `Bifurx::publishPreviewState()` double-buffers `BifurxPreviewState`.
- `Bifurx::publishAnalysisFrame()` double-buffers full `BifurxAnalysisFrame` windows.
- `BifurxSpectrumBase::syncBase()` copies the newest preview state and analysis frame into UI display state.
- `BifurxSpectrumBase::updateCurveCache()` computes the response curve target.
- `BifurxSpectrumBase::updateOverlayCache()` runs FFT and prepares overlay target arrays.
- `BifurxSpectrumBase::updateAnimation()` slews current display arrays toward the newest targets.
- `BifurxSpectrumWidget` renders the NanoVG path.
- `BifurxSpectrumGLWidget` renders the OpenGL path.

The existing split means the worker should attach at the visual preparation layer, not the audio module layer.

## Design Verdict

The design is technically sound if kept narrow:

- Move all worthwhile pure CPU preparation off-thread.
- Keep all Rack widget access on the UI thread.
- Keep all NanoVG and OpenGL calls on the UI thread.
- Pass copied request data into the worker.
- Return immutable snapshots from the worker.
- Drop stale requests instead of queueing them.

The design is not a substitute for audio-thread optimization. It reduces UI stalls and makes patches with several visible Bifurx instances less likely to spend frame time recomputing the same visual data.

The main risk is complexity. The current code is still manageable because `BifurxSpectrumBase` owns all visual state. A worker split will add synchronization, lifecycle, and stale-data behavior. That trade is worth it only if profiling shows Bifurx visual prep is still a meaningful UI cost after shader rendering and dirty redraw gating.

## Non-Goals

- Do not call `gl*()` from the worker.
- Do not call NanoVG from the worker.
- Do not create or migrate OpenGL contexts for Bifurx visual preparation.
- Do not use worker-side CPU software rasterization as the default path.
- Do not read `module->params`, `widget->box`, Rack scene state, or expander state from the worker.
- Do not mutate `BifurxSpectrumBase::state` from the worker.
- Do not move filter DSP or audio analysis capture out of `Bifurx::process()` in this pass.
- Do not create a FIFO queue of old visual jobs.

Software rasterization is only a fallback research path. It would make the worker return an RGBA image, but it would also add scaling, text rendering, antialiasing, memory bandwidth, and UI-thread upload costs. Do not pursue it unless metrics prove that normal UI-thread drawing is the bottleneck and data-prep offload is insufficient.

## Threading Model

Use a latest-request / latest-snapshot model.

```text
Audio thread
  publishes preview state and analysis frames through existing double buffers

UI thread
  copies the newest module state, display size, flags, and animation state into a request

Bifurx visual worker
  computes target arrays, layout, labels, animation targets, and renderer-ready CPU geometry from that copied request

UI thread
  atomically adopts the newest completed immutable snapshot and renders it
```

The worker may be one frame or several frames behind. That is acceptable for visual overlays. It must never block audio and should not block UI drawing.

## Data Ownership

Every boundary should have clear ownership.

`Bifurx` module:

- Owns DSP state.
- Owns existing preview and analysis publish buffers.
- Does not own worker lifecycle in the first implementation.

`BifurxSpectrumBase`:

- Owns current display animation state.
- Owns worker submission and completed snapshot adoption helpers.
- Remains the shared base for NanoVG and OpenGL display paths.

`BifurxUiRenderService`:

- Owns one shared worker thread for all visible Bifurx displays.
- Owns request synchronization and per-display pending slots.
- Owns the latest completed immutable snapshot per display.
- Ensures each display has at most one pending/in-flight calculation.
- Replaces stale pending requests with newer requests instead of queueing old work.
- Does not know about Rack widgets, NanoVG contexts, OpenGL objects, or live module pointers.

Renderer widgets:

- Own renderer-specific draw resources.
- Consume `BifurxUiRenderSnapshot`.
- Upload VBOs and issue draw calls only in the UI/OpenGL render path.

## Request Contract

The request must contain every value the worker needs, copied by value on the UI thread.

Suggested first-pass shape:

```cpp
struct BifurxUiRenderRequest {
	uint64_t displayId = 0;
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;

	float width = 0.f;
	float height = 0.f;
	float uiFrameSec = 1.f / 60.f;
	float workerFrameSec = 1.f / 30.f;

	bool hasPreview = false;
	bool hasOverlay = false;
	bool showModuleResponseOverlay = false;
	bool fftScaleDynamic = true;
	bool prepareNanovgGeometry = true;
	bool prepareGlGeometry = true;
	bool prepareAnimation = false;

	BifurxPreviewState previewState;
	BifurxSpectrumState displayState;
	BifurxAnalysisFrame analysisFrame;
	bool includeAnalysisFrame = false;
};
```

Important details:

- `displayId` should identify a live Bifurx display/widget registration in the shared visual service. It must not be a raw widget pointer used by the worker.
- `displayState` should be a copied state object, not a pointer.
- `analysisFrame` is large, so only include it when `analysisSeq` changes.
- The worker should not use `module` to check `fftScaleDynamic`; that flag is copied into the request.
- The request should include dimensions because marker layout and refined curve points depend on `width` and `height`.
- `prepareAnimation` should be enabled only after target preparation is stable and profiling shows `updateAnimation()` remains a material UI cost.
- `workerFrameSec` lets the worker produce frame-rate-independent animation interpolation if animation is moved into snapshots.

## Snapshot Contract

The snapshot should be immutable after publication. Use `std::shared_ptr<const BifurxUiRenderSnapshot>` or a fixed ring of owned buffers with atomic indices.

Suggested first-pass shape:

```cpp
struct BifurxUiRenderSnapshot {
	uint64_t displayId = 0;
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;

	BifurxSpectrumState state;
	BifurxMarkerLayout markerLayout;
	std::vector<BifurxCurvePoint> refinedCurvePoints;
	std::vector<BifurxTextLabel> textLabels;
	std::vector<BifurxTickMark> tickMarks;
	std::vector<BifurxCurvePoint> nanovgResponsePolyline;
	std::vector<BifurxCurvePoint> nanovgOverlayPolyline;

	std::vector<BifurxGlVertex> fillVertices;
	std::vector<BifurxGlVertex> fillSoftCapVertices;
	std::vector<BifurxGlVertex> cyanVertices;
	std::vector<BifurxGlVertex> curveVertices;

	bool hasValidGeometry = false;
};
```

Renderer-specific notes:

- It is acceptable for the extraction patch to prepare only renderer-neutral state and `refinedCurvePoints`, but the worker target should include every CPU-side structure that is reasonably reusable by the active renderer.
- Preparing OpenGL vertex arrays in the worker is safe because they are plain CPU-side structs.
- `BifurxSpectrumGLWidget::GlVertex` is currently local to `src/BifurxGL.cpp`; if the worker prepares GL vertices, move a renamed POD type such as `BifurxGlVertex` into `Bifurx.hpp` or a small shared render header.
- VBO creation, buffer upload, shader setup, and draw calls stay in `BifurxSpectrumGLWidget::drawFramebuffer()`.
- NanoVG path should consume prebuilt polylines and label/tick layouts from the snapshot. It still issues NanoVG draw calls on the UI thread, but it should not rebuild response/overlay points, marker positions, or text placement during draw.
- If text measurement is needed for exact placement, keep the actual font measurement on the UI thread unless the worker can use a deterministic precomputed font metric table. Do not call NanoVG text APIs from the worker.

## Worker API

Suggested API:

```cpp
class BifurxUiRenderService {
public:
	void start();
	void stop();
	uint64_t registerDisplay();
	void unregisterDisplay(uint64_t displayId);
	void submitLatest(const BifurxUiRenderRequest& request);
	std::shared_ptr<const BifurxUiRenderSnapshot> getLatestSnapshot(uint64_t displayId) const;

private:
	void run();
};
```

Implementation details:

- `start()` launches one shared thread lazily when at least one display is registered and worker mode is enabled.
- `stop()` sets a stop flag, wakes the worker, and joins the thread.
- `registerDisplay()` returns an opaque display ID owned by the UI/widget side.
- `unregisterDisplay()` drops pending/completed work for that display ID.
- `submitLatest()` overwrites that display's pending request.
- The worker uses a condition variable or short timed wait, not a busy spin.
- The worker processes only the newest pending request for a display and never drains a FIFO backlog.
- The worker publishes completed snapshots per display with release/acquire synchronization.
- If a display already has an in-flight calculation, newer requests should replace its pending slot. When the in-flight work completes, the worker should process only the newest pending request, if any.

## Coalescing Rule

Do not queue visual jobs.

If the UI submits requests `101`, `102`, and `103` before the worker wakes, the worker should process `103` only. A display snapshot that is one or two frames stale is better than a backlog.

For multiple modules, apply the same rule per display:

- one shared worker thread
- one in-flight calculation per display at most
- one replaceable pending request per display at most
- no global FIFO of stale visual work
- no per-module worker threads

If five Bifurx modules all submit updates, the worker should choose from the latest request for each display. If one display submits ten updates while another submits one, the ten updates collapse to one newest request. Fairness can be round-robin across display IDs or newest-first with starvation protection.

This is the right behavior for Bifurx because response curves and spectrum overlays are explanatory visuals. They should feel current, but they do not need audio-rate precision.

## Per-Display State Machine

Each registered display should have one service-owned slot:

```cpp
struct DisplaySlot {
	bool active = true;
	bool inFlight = false;
	bool hasPending = false;
	BifurxUiRenderRequest pending;
	std::shared_ptr<const BifurxUiRenderSnapshot> latestSnapshot;
	uint64_t latestCompletedRequestSeq = 0;
	uint64_t replacedPendingCount = 0;
	uint64_t droppedLateResultCount = 0;
};
```

Required behavior:

1. `submitLatest(request)` locks the service state, finds the slot by `request.displayId`, and stores `request` into `slot.pending`.
2. If `slot.hasPending` was already true, the old pending request is discarded and `replacedPendingCount` increments.
3. `submitLatest()` never blocks on the current in-flight calculation.
4. The worker chooses a display slot with `hasPending == true` and `inFlight == false`.
5. The worker moves that pending request into a local variable, clears `hasPending`, sets `inFlight = true`, unlocks, and computes the snapshot.
6. When computation finishes, the worker locks again and checks that the display slot is still active.
7. If the slot is inactive or unregistered, the worker discards the result and increments `droppedLateResultCount`.
8. If the slot is still active, the worker publishes `latestSnapshot`, sets `latestCompletedRequestSeq`, and sets `inFlight = false`.
9. If a newer request arrived while the worker was computing, `hasPending` will already be true, and that latest pending request becomes eligible for the next pass.

This means a display can have one request being computed and one newer replacement request waiting. It can never have a queue of two or more waiting requests.

Example for one display:

```text
worker starts request 101       -> inFlight=101, pending=empty
UI submits 102                  -> inFlight=101, pending=102
UI submits 103                  -> inFlight=101, pending=103, 102 discarded
worker publishes 101            -> latestSnapshot=101, pending=103
worker later starts request 103 -> inFlight=103, pending=empty
```

Implementation invariants:

- `Bifurx::process()` must never call the service and must never take a service mutex.
- the worker must never dereference `Bifurx*`, `Widget*`, `NVGcontext*`, or GL handles.
- `submitLatest()` must copy all required data before publishing the request to the service.
- completed snapshots are immutable after publication.
- unregistering a display must make late worker results harmless.
- stale requests are replaced, not processed.

## Practical Maximum Worker Scope

The worker should own the practical maximum amount of work that is:

- pure CPU work
- independent of Rack widget state after the request is copied
- independent of NanoVG/OpenGL contexts
- reusable by at least one active renderer
- not more expensive to copy/upload than to compute on the UI thread

Target worker-owned work:

- Axis cache generation from sample rate and display dimensions.
- Response curve target calculation from `BifurxPreviewState`.
- FFT overlay target calculation from `BifurxAnalysisFrame`.
- Display top target calculation.
- Marker layout calculation.
- Tick mark and label placement.
- Refined curve point calculation.
- NanoVG-consumable response and overlay polylines.
- CPU-side OpenGL vertex generation for fill, soft caps, cyan overlay, and curve lines.
- Dirty-stage reuse and incremental caches keyed by size, render mode, preview sequence, and analysis sequence.
- Optional display-array animation/interpolation after target preparation is stable and profiling justifies moving it.

Keep these on the UI thread:

- NanoVG drawing.
- OpenGL draw calls.
- Shader compilation and VBO uploads.
- Debug terminal submission.
- Context menu and widget visibility behavior.
- Exact text measurement that requires the active NanoVG font context.
- Final framebuffer dirty decisions that depend on Rack widget visibility or renderer lifecycle.

Animation policy:

- First extraction keeps `updateAnimation()` on the UI thread.
- Worker mode may later publish already-interpolated display arrays when `prepareAnimation` is enabled.
- If animation moves to the worker, snapshots must include the source/target sequence and interpolation timestamp so the UI can reject stale animation frames cleanly.
- Do not make the worker run at unbounded Rack frame rate just to own animation. Worker animation should be capped by `Visual worker rate`; the UI may still do cheap final lerp toward the newest snapshot if needed.

Copy-cost rule:

- Do not move a stage into the worker if the snapshot copy/upload cost exceeds the UI-thread compute cost in normal patches.
- Prefer compact POD arrays and preallocated/reused vectors in snapshots.
- Large analysis frames should be copied only when `analysisSeq` changes.
- Large vertex arrays should be generated only for the active renderer path unless profiling shows dual-path generation is cheap enough.

## File Structure

Bifurx is already large enough that the worker implementation should not be added directly to `Bifurx.cpp`, `BifurxUI.cpp`, or `BifurxGL.cpp` except for narrow integration calls.

Recommended file layout:

```text
src/Bifurx.hpp
  Core module declarations, existing DSP-facing structs, small shared render data structs.

src/Bifurx.cpp
  Audio DSP, preview-state publication, analysis-frame publication, JSON for module settings.
  Keep audio-thread code here. Do not add worker service implementation here.

src/BifurxUI.cpp
  NanoVG widget integration, context menu, debug/perf UI logging, adoption of worker snapshots.
  Keep Rack widget and NanoVG calls here.

src/BifurxGL.cpp
  OpenGL widget integration, shader setup, VBO upload, GL draw calls.
  Keep all GL calls here.

src/BifurxRenderData.hpp
  Plain data contracts for visual preparation:
  BifurxUiRenderRequest, BifurxUiRenderSnapshot, BifurxGlVertex if needed,
  and any POD geometry/state structs shared by UI, GL, and worker code.

src/BifurxRenderPrep.hpp
src/BifurxRenderPrep.cpp
  Pure visual preparation helpers:
  axis cache generation, response curve targets, FFT overlay targets,
  marker layout, tick/label placement, refined curve point calculation,
  NanoVG-consumable polylines, and CPU-side GL vertex generation.
  These functions must not touch Rack widgets, GL, NanoVG, or live module pointers.

src/BifurxWorker.hpp
src/BifurxWorker.cpp
  Shared BifurxUiRenderService implementation:
  thread lifecycle, display registration, latest-request coalescing,
  one-in-flight-per-display state machine, snapshot publication, worker metrics.
```

Dependency direction:

```text
Bifurx.cpp        -> Bifurx.hpp
BifurxUI.cpp      -> Bifurx.hpp, BifurxRenderData.hpp, BifurxWorker.hpp
BifurxGL.cpp      -> Bifurx.hpp, BifurxRenderData.hpp, BifurxWorker.hpp
BifurxWorker.cpp  -> BifurxRenderData.hpp, BifurxRenderPrep.hpp
BifurxRenderPrep.cpp -> Bifurx.hpp, BifurxRenderData.hpp
```

Architectural rules:

- Do not include `BifurxUI.cpp` or `BifurxGL.cpp` concepts in worker/prep files.
- Do not put `std::thread`, `std::mutex`, or `std::condition_variable` in `Bifurx.cpp`.
- Do not put FFT/render-prep extraction back into widget draw functions after it is split out.
- Keep renderer-specific GPU objects and NanoVG drawing out of `BifurxRenderPrep`.
- If a helper needs `BifurxSpectrumBase::state`, pass a copied `BifurxSpectrumState` or a narrower explicit argument list.
- If `Bifurx.hpp` grows too much, move visual-only contracts into `BifurxRenderData.hpp` rather than adding more nested structs to `Bifurx`.

Suggested extraction order:

1. Add `BifurxRenderData.hpp` with request/snapshot contracts and any shared POD geometry.
2. Add `BifurxRenderPrep.hpp/.cpp` and move existing pure helper logic there.
3. Update `BifurxSpectrumBase` to call the prep helpers on the UI thread with identical behavior.
4. Expand prep helpers until they cover all reasonable CPU prep for the active render path.
5. Add `BifurxWorker.hpp/.cpp` only after the prep helpers are isolated and tested.

## Proposed Integration Steps

1. Add request and snapshot structs in `src/BifurxRenderData.hpp`.
2. Add pure preparation helpers in `src/BifurxRenderPrep.hpp/.cpp`.
3. Extract pure functions from `BifurxSpectrumBase::updateCurveCache()` and `BifurxSpectrumBase::updateOverlayCache()` into those prep helpers.
4. Verify `BifurxSpectrumBase` can call the prep helpers on the UI thread with unchanged behavior.
5. Extract marker layout, tick/label placement, and refined curve point generation into prep helpers.
6. Extract CPU-side GL vertex generation where the active OpenGL path can reuse the resulting POD arrays.
7. Feed both NanoVG and OpenGL UI paths from the same prep snapshot on the UI thread.
8. Add a shared `BifurxUiRenderService` implementation in `src/BifurxWorker.hpp/.cpp`.
9. Have `BifurxSpectrumBase` build a request after `syncBase()` copies new module data.
10. Submit requests only when preview, analysis, display size, render path, or relevant display flags changed.
11. Adopt completed snapshots in `BifurxSpectrumBase::runRenderTick()`.
12. Mark the framebuffer dirty when a new snapshot is adopted or animation remains active.
13. Add optional worker-side animation preparation if metrics show UI animation work remains material.
14. Add global-default and per-module override settings for visual worker mode in the context menu.
15. Add performance logging fields so worker time and stale-frame count are visible in debug traces.

## Worker Mode Setting

Worker mode should be controlled by global default plus per-module override.

Global plugin-level setting:

- `Bifurx visual worker default`: `Off / Auto / On`
- Stored in plugin settings, not in patch musical state.

Per-module persisted override on `Bifurx`:

```cpp
enum VisualWorkerMode {
	VISUAL_WORKER_INHERIT = -1,
	VISUAL_WORKER_OFF = 0,
	VISUAL_WORKER_AUTO,
	VISUAL_WORKER_ON,
	VISUAL_WORKER_COUNT
};
```

Effective mode resolution:

- if module override is `Inherit`, use global plugin default.
- if module override is `Off`, force worker off for that module.
- if module override is `Auto`, use per-module auto policy regardless of global default.
- if module override is `On`, force worker on for that module.

UI location:

- Do not add a front-panel control for worker mode.
- Expose both global default and per-module override through the module context menu.

Recommended default for development: `Off`.

Recommended default after validation: `Auto`.

`Auto` should enable the worker only when the display is visible and the current render path is expensive enough to benefit. A simple first auto rule is:

- enable for OpenGL shader mode when analysis overlay is active
- enable for NanoVG when Bifurx is visible
- disable if the worker fails to start or repeatedly falls behind

## Worker Rate

The worker should be rate-limited independently from Rack frame rate.

Suggested menu:

```text
Visual worker: Off / Auto / On
Visual worker rate: 15 Hz / 30 Hz / 60 Hz
```

Recommended default after validation: `30 Hz`.

The UI can still animate toward the newest snapshot every frame. The heavy target recomputation does not need to run every frame.

## Shared Worker Recommendation

Use one plugin-level shared `BifurxUiRenderService`.

This is now the recommended first worker implementation because the motivating failure mode is several Bifurx modules visible in the same Rack patch. One worker per visible widget is simpler locally, but it can multiply scheduler pressure and does not provide a global cap on visual-prep CPU.

The shared service should:

- own exactly one worker thread
- track latest request per visible Bifurx display
- track latest completed snapshot per visible Bifurx display
- allow at most one in-flight calculation per display
- keep at most one pending replacement request per display
- process display work round-robin or newest-first with starvation protection
- drop stale pending work rather than processing a backlog

The service can still be optional and conservative: when worker mode is off, `BifurxSpectrumBase` should keep the existing UI-thread path.

## Synchronization Requirements

Use one of these approaches:

- `std::mutex` plus `std::condition_variable` for pending requests.
- `std::mutex` around a `std::shared_ptr<const BifurxUiRenderSnapshot>` for completed snapshots.
- Or a fixed triple-buffer if profiling shows shared pointer and mutex overhead matter.

The mutex approach is acceptable for a first pass because the worker runs at 15-60 Hz and no lock should be taken from the audio thread.

Do not lock from `Bifurx::process()`.

The worker service may use mutexes on the UI thread and worker thread only. No service lock should be acquired from the audio thread.

## Lifecycle Requirements

The worker service must stop cleanly when:

- each widget/display unregisters
- the render mode changes and worker mode no longer needs it
- the module is removed
- Rack exits

Display unregister sequence:

1. Mark the display ID inactive.
2. Drop pending work for that display.
3. Drop completed snapshot for that display.
4. If the worker completes late for that display ID, discard the result.

Global shutdown sequence:

1. Store `stopRequested = true`.
2. Notify the condition variable.
3. Join the thread.
4. Drop pending request and completed snapshot.

The worker request must not contain a raw `Bifurx*`, `Widget*`, `NVGcontext*`, or GL object handle, so a late worker result cannot dereference a destroyed Rack object.

## Feasibility Notes

This path is feasible because Bifurx already publishes copied preview and analysis data. The risky part is not thread access to audio data; that part is already mostly solved. The risky part is separating `BifurxSpectrumBase` into pure preparation and UI-owned animation/rendering without accidentally retaining hidden dependencies on `module`.

Likely benefits:

- lower UI-thread spikes when FFT overlay updates
- less per-frame curve and geometry preparation cost
- smoother Rack UI when several Bifurx modules are visible
- cleaner boundary between data preparation and renderer-specific drawing

Likely costs:

- more code and more lifecycle complexity
- possible one-to-four-frame visual latency
- harder debugging when snapshots are stale
- duplicated state during the transition

This is a good path if profiling shows `updateOverlayCache()`, `updateCurveCache()`, or renderer geometry preparation are still among the larger UI costs. It is not the first thing to do if the remaining cost is mostly OpenGL driver time, VBO upload time, or actual drawing.

## Validation Plan

Measure before and after.

Use existing perf/debug logging and add:

- worker request count
- completed snapshot count
- dropped request count
- worker compute time EMA/max
- snapshot age in UI frames
- fallback count when worker disabled or late
- per-display pending replacement count
- per-display skipped stale request count

Later, add a compact debug terminal packet for these visual-worker metrics so the external debug terminal can show whether UI stalls are from worker backlog, stale snapshot age, drawing, or audio processing. That instrumentation can be a follow-up after the worker lifecycle is stable.

## Proof-First Milestone

Before implementing the worker service, prove that the visible lag is actually dominated by Bifurx visual preparation.

Use the existing DragonKing debug terminal path first. Bifurx already submits:

- `ui_ms`: UI draw cost EMA for the Bifurx display
- `audio_us`: sampled audio processing cost
- `curve_prep_us`: response-curve preparation time
- `overlay_prep_us`: FFT overlay preparation time

Initial proof workflow:

1. Enable DragonKing debug mode.
2. Run `tools/debug_terminal/server.py`.
3. Load a baseline patch with one visible Bifurx and normal modulation.
4. Record `ui_ms`, `audio_us`, `curve_prep_us`, and `overlay_prep_us`.
5. Load the problem patch or construct a patch with five visible Bifurx instances.
6. Compare per-instance values and total visible Bifurx UI cost.
7. Switch Bifurx render modes between NanoVG, OpenGL, and OpenGL SHDR where practical.
8. Toggle `Show Module Response` and `Dynamic FFT Scale` to identify overlay/curve sensitivity.

Interpretation:

- If `curve_prep_us` or `overlay_prep_us` scale strongly with visible Bifurx count and line up with UI stalls, the worker service is likely justified.
- If `ui_ms` is high while prep times are low, the bottleneck is probably drawing, framebuffer invalidation, OpenGL driver work, or NanoVG path cost. The worker will help less.
- If `audio_us` is high across five Bifurx modules, the problem is audio DSP load, not UI visual prep. The worker is the wrong first fix.
- If only one render mode is bad, prefer a targeted renderer fix before adding threading.

Decision gate:

- Proceed to `BifurxRenderPrep` extraction if prep cost is meaningfully visible in the debug terminal or CSV perf logs.
- Proceed to the shared worker service only after the extraction patch preserves behavior and metrics still show prep work as a material UI cost.
- Defer threading if the evidence points primarily at draw/driver/audio cost.

Manual test cases:

- one Bifurx, idle input
- one Bifurx with fast parameter modulation
- one Bifurx with active FFT overlay
- several visible Bifurx modules
- switch NanoVG / OpenGL fixed / OpenGL shader modes
- remove module while worker is active
- close patch while worker is active

Correctness checks:

- no audio-thread locks
- no Rack widget access in worker
- no GL or NanoVG calls in worker
- no use-after-free when module/widget is removed
- visual fallback still works when worker mode is off

## Recommended Path

Proceed in three patches.

Patch 1 should be a refactor-only extraction:

- extract pure curve and overlay target preparation helpers
- extract marker layout, tick/label placement, and refined polyline construction
- keep execution on the UI thread
- verify NanoVG and OpenGL output remain unchanged

Patch 2 should maximize CPU-side render preparation while still running on the UI thread:

- move shared POD geometry into `BifurxRenderData.hpp`
- prepare NanoVG-consumable polylines and layout snapshots
- prepare CPU-side OpenGL vertex arrays for the active GL render path
- add dirty-stage reuse keyed by display size, render mode, preview sequence, and analysis sequence
- measure snapshot build cost and copy/upload cost before adding the worker

Patch 3 should add the worker:

- introduce request/snapshot structs
- introduce the shared `BifurxUiRenderService`
- register/unregister each Bifurx display with the service
- submit latest request from `BifurxSpectrumBase`
- adopt completed snapshots on the UI thread
- optionally enable worker-side animation preparation only after target snapshots are stable
- add worker menu settings and perf counters

That order keeps the highest-risk behavioral split separate from the mechanical extraction work and ensures the worker receives the maximum useful CPU workload instead of only the earliest extracted helper functions.
