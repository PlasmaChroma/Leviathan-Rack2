# Bifurx render worker lifecycle plan

## Goal

Make the visual render worker's ownership and shutdown rules explicit, then simplify its implementation while preserving audible output, rendered curves, and the audio thread's work.

## Current path

- `BifurxSpectrumBase::syncBase()` in `src/Bifurx.cpp` chooses worker or synchronous curve preparation, registers a display, and submits the latest preview and analysis sequence.
- `BifurxUiRenderService` in `src/BifurxWorker.cpp` owns one thread, a mutex-protected display map, a ready queue, a pending request per display, and the latest completed snapshot.
- Requests hold an immutable lease on a bounded, per-display FFT payload pool. A replacement request can inherit the pending payload when it has no newer analysis frame.
- The UI adopts completed snapshots by request sequence. When offload is disabled, it computes the pending curve and overlay synchronously. Visibility and renderer changes release registration in `src/BifurxUI.cpp`.
- `shutdownBifurxRenderService()` is called at plugin shutdown from `src/plugin.cpp`.

## Required behavior to preserve

1. Audio processing never waits for the render worker and never allocates because of a worker request.
2. A display has at most one in-flight job and one replaceable pending job. New requests must not create an unbounded backlog.
3. A completed job for an unregistered display cannot become visible to a new display, even if the latter reuses a UI object. Display IDs must remain unique during service lifetime.
4. A pending curve-only request must retain the newest eligible analysis payload; a completed curve-only request must retain the last completed overlay when sample rates match.
5. The UI may apply a completed snapshot older than its latest submission, then catch up. Switching worker mode off must calculate the latest preview and FFT without waiting for the worker or new audio.
6. Shutdown joins the worker before destroying its state. No new registrations or submissions are accepted after global plugin shutdown begins.
7. Existing `Process`, `Step`, and `Draw` Debug Terminal metrics keep their module-level meaning. Worker timings remain separate fields.

## Work sequence

### 1. Characterize the lifecycle before changing it

Add focused tests in `tests/bifurx_runtime_spec.cpp` using a local `BifurxUiRenderService` instance. Cover:

- Unregister a display while a request is queued and while one is in flight; no snapshot is exposed afterward. Register another display and verify the old result cannot appear under its ID.
- Stop with pending and in-flight work, then start and register again. Check that the new display receives only new work and that repeated start/stop calls finish cleanly.
- Submit from two displays, with one producer flooding updates. Verify both eventually receive a snapshot and each snapshot has the correct display ID and monotonically increasing request sequence.
- Replace a pending analysis request with a curve-only request, including a sample-rate change. Confirm payload inheritance only happens at a matching rate.
- Rapidly switch worker on/off and NanoVG/OpenGL visibility while preview and FFT sequences advance. Confirm synchronous catch-up and no stale adoption after re-registration.

Use bounded timeouts only to prevent a hung test; make correctness assertions on IDs, sequences, ownership, and rendered targets rather than elapsed milliseconds. Preserve the existing coalescing, pool, and fallback tests.

### 2. Document and tighten ownership in the service

Keep all `slots`, queue, pending request, in-flight flag, and latest snapshot changes under the service mutex. Make the slot transitions explicit in code: registered -> queued -> in flight -> completed, with unregister and stop valid from every state. A worker may finish an already claimed request after unregister, but it must discard that result.

Remove state only if the characterization tests show it is redundant. In particular, review whether `active`, `queued`, `hasPending`, and `inFlight` are all needed once the transition rules are written down. Keep request and payload movement outside expensive lock-held work. Do not hold the mutex while running `prepareCurveSnapshot()` or joining the thread.

### 3. Isolate UI handoff policy

Consolidate the registration/release rules shared by `syncBase()` and visibility or renderer changes. Keep the existing behavior that a worker snapshot can be applied even when a newer request has been submitted. Ensure releasing registration resets the per-display sequence and payload-pool state exactly once, and that re-registration cannot adopt an earlier snapshot.

Avoid broad rendering changes in this step. Preserve NanoVG and OpenGL output, curve animation, overlay continuity, and sample-rate invalidation.

### 4. Verify and measure

- Run the native MINGW64 `test-fast` suite and an authoritative `plugin.dll` build.
- In Rack, exercise rapid module add/remove, window close/reopen, renderer switching, worker toggle, and several visible Bifurx modules. Check for frozen curves, stale overlays, and shutdown hangs.
- Compare Debug Terminal `Step`/`Draw`, worker submit time, queue latency, and snapshot age before and after. Reject the refactor if normal rendering becomes measurably slower or the UI spends longer waiting on the service mutex.

## Scope control

Start with tests and a small state-machine cleanup. A new thread pool, lock-free queue, or change to audio-side publication is outside this plan unless measurements reveal a specific bottleneck. Keep the synchronous path as the fallback and as a reference for visual parity.
