# Bifurx Threading Implementation Plan

## Purpose

This document turns `bifurx-perf-threads.md` into an implementation spec for moving Bifurx visual preparation work off the Rack UI thread.

The goal is not to make the audio engine multithreaded and not to draw from a worker thread. The goal is to let a background worker prepare immutable display snapshots so the Rack UI thread spends less time doing FFT, response-curve, marker, and vertex preparation.

The short recommendation: this is feasible and likely worth doing if Bifurx UI cost remains material after the current redraw gating and OpenGL shader path. It should be implemented as an optional visual-worker pipeline with conservative fallback, not as a rewrite of the module or its DSP.

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

- Move pure CPU preparation off-thread.
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
- Do not read `module->params`, `widget->box`, Rack scene state, or expander state from the worker.
- Do not mutate `BifurxSpectrumBase::state` from the worker.
- Do not move filter DSP or audio analysis capture out of `Bifurx::process()` in this pass.
- Do not create a FIFO queue of old visual jobs.

## Threading Model

Use a latest-request / latest-snapshot model.

```text
Audio thread
  publishes preview state and analysis frames through existing double buffers

UI thread
  copies the newest module state, display size, flags, and animation state into a request

Bifurx visual worker
  computes target arrays, layout, and renderer-neutral geometry from that copied request

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

`BifurxUiRenderWorker`:

- Owns its worker thread.
- Owns request synchronization.
- Owns the latest completed immutable snapshot.
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
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;

	float width = 0.f;
	float height = 0.f;
	float uiFrameSec = 1.f / 60.f;

	bool hasPreview = false;
	bool hasOverlay = false;
	bool showModuleResponseOverlay = false;
	bool fftScaleDynamic = true;

	BifurxPreviewState previewState;
	BifurxSpectrumState displayState;
	BifurxAnalysisFrame analysisFrame;
	bool includeAnalysisFrame = false;
};
```

Important details:

- `displayState` should be a copied state object, not a pointer.
- `analysisFrame` is large, so only include it when `analysisSeq` changes.
- The worker should not use `module` to check `fftScaleDynamic`; that flag is copied into the request.
- The request should include dimensions because marker layout and refined curve points depend on `width` and `height`.

## Snapshot Contract

The snapshot should be immutable after publication. Use `std::shared_ptr<const BifurxUiRenderSnapshot>` or a fixed ring of owned buffers with atomic indices.

Suggested first-pass shape:

```cpp
struct BifurxUiRenderSnapshot {
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;

	BifurxSpectrumState state;
	BifurxMarkerLayout markerLayout;
	std::vector<BifurxCurvePoint> refinedCurvePoints;

	std::vector<BifurxGlVertex> fillVertices;
	std::vector<BifurxGlVertex> fillSoftCapVertices;
	std::vector<BifurxGlVertex> cyanVertices;
	std::vector<BifurxGlVertex> curveVertices;

	bool hasValidGeometry = false;
};
```

Renderer-specific notes:

- It is acceptable for the first implementation to prepare only renderer-neutral state and `refinedCurvePoints`.
- Preparing OpenGL vertex arrays in the worker is safe because they are plain CPU-side structs.
- `BifurxSpectrumGLWidget::GlVertex` is currently local to `src/BifurxGL.cpp`; if the worker prepares GL vertices, move a renamed POD type such as `BifurxGlVertex` into `Bifurx.hpp` or a small shared render header.
- VBO creation, buffer upload, shader setup, and draw calls stay in `BifurxSpectrumGLWidget::drawFramebuffer()`.
- NanoVG path may still draw paths from arrays on the UI thread.

## Worker API

Suggested API:

```cpp
class BifurxUiRenderWorker {
public:
	void start();
	void stop();
	void submitLatest(const BifurxUiRenderRequest& request);
	std::shared_ptr<const BifurxUiRenderSnapshot> getLatestSnapshot() const;

private:
	void run();
};
```

Implementation details:

- `start()` launches the thread lazily when the display widget is visible and worker mode is enabled.
- `stop()` sets a stop flag, wakes the worker, and joins the thread.
- `submitLatest()` overwrites the pending request.
- The worker uses a condition variable or short timed wait, not a busy spin.
- The worker processes only the newest pending request.
- The worker publishes a completed snapshot with release/acquire synchronization.

## Coalescing Rule

Do not queue visual jobs.

If the UI submits requests `101`, `102`, and `103` before the worker wakes, the worker should process `103` only. A display snapshot that is one or two frames stale is better than a backlog.

This is the right behavior for Bifurx because response curves and spectrum overlays are explanatory visuals. They should feel current, but they do not need audio-rate precision.

## First Implementation Scope

Phase 1 should move target preparation, not drawing.

Move these into pure worker-callable helpers:

- Axis cache generation from sample rate.
- Response curve target calculation from `BifurxPreviewState`.
- FFT overlay target calculation from `BifurxAnalysisFrame`.
- Display top target calculation.
- Marker layout calculation.
- Refined curve point calculation.

Keep these on the UI thread:

- `updateAnimation()`, unless profiling shows it is costly.
- NanoVG drawing.
- OpenGL draw calls.
- Shader compilation and VBO uploads.
- Debug terminal submission.
- Context menu and widget visibility behavior.

Leaving animation on the UI thread is a good first split because it keeps frame-to-frame smoothing deterministic and avoids the worker needing to run at display frame rate.

## Proposed Integration Steps

1. Add request and snapshot structs near `BifurxSpectrumState` in `Bifurx.hpp`.
2. Add a small `BifurxUiRenderWorker` implementation in new files, preferably `src/BifurxWorker.hpp` and `src/BifurxWorker.cpp`.
3. Extract pure functions from `BifurxSpectrumBase::updateCurveCache()` and `BifurxSpectrumBase::updateOverlayCache()`.
4. Have `BifurxSpectrumBase` build a request after `syncBase()` copies new module data.
5. Submit requests only when preview, analysis, display size, or relevant display flags changed.
6. Adopt completed snapshots in `BifurxSpectrumBase::runRenderTick()`.
7. Mark the framebuffer dirty when a new snapshot is adopted or animation remains active.
8. Add a context menu setting for visual worker mode.
9. Add performance logging fields so worker time and stale-frame count are visible in debug traces.

## Worker Mode Setting

Add a persisted setting on `Bifurx`:

```cpp
enum VisualWorkerMode {
	VISUAL_WORKER_OFF = 0,
	VISUAL_WORKER_AUTO,
	VISUAL_WORKER_ON,
	VISUAL_WORKER_COUNT
};
```

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

## Shared Worker vs Per-Widget Worker

First implementation:

- Use one worker per visible Bifurx display widget.
- Start it when the widget is constructed or first becomes active.
- Stop it in the widget destructor.

This is simpler and good enough to validate the design.

Polished implementation:

- Use one plugin-level shared `BifurxUiRenderService`.
- Track latest request per visible Bifurx display.
- Process newest-first or round-robin with stale dropping.
- Cap total worker CPU use across many visible Bifurx instances.

The shared service is architecturally better, but it is not the right first patch unless multiple visible Bifurx instances already show severe UI cost.

## Synchronization Requirements

Use one of these approaches:

- `std::mutex` plus `std::condition_variable` for pending requests.
- `std::mutex` around a `std::shared_ptr<const BifurxUiRenderSnapshot>` for completed snapshots.
- Or a fixed triple-buffer if profiling shows shared pointer and mutex overhead matter.

The mutex approach is acceptable for a first pass because the worker runs at 15-60 Hz and no lock should be taken from the audio thread.

Do not lock from `Bifurx::process()`.

## Lifecycle Requirements

The worker must stop cleanly when:

- the widget is destroyed
- the render mode changes and worker mode no longer needs it
- the module is removed
- Rack exits

Shutdown sequence:

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

Proceed in two patches.

Patch 1 should be a refactor-only extraction:

- extract pure curve and overlay target preparation helpers
- keep execution on the UI thread
- verify NanoVG and OpenGL output remain unchanged

Patch 2 should add the worker:

- introduce request/snapshot structs
- submit latest request from `BifurxSpectrumBase`
- adopt completed snapshots on the UI thread
- add worker menu settings and perf counters

That order keeps the highest-risk behavioral split separate from the mechanical extraction work.
