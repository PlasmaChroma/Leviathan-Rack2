# Bifurx render worker: phases 0–2 implementation record

**Starting revision:** `cb86a6da95087fa56bc3d1fbb6b3f1aab45b8e25` on September 22, 2026. The working tree already contained the Bifurx per-callback debug-flag edit, the two untracked Bifurx planning documents, and unrelated untracked files. None were staged or committed.

## Scope completed

- Inventoried the worker service, UI registration/submission/adoption path, plugin shutdown, runtime/GL tests, Makefile recipes, and Pro synchronizer. No source files were moved.
- Replaced independent service lifecycle flags with explicit `Stopped`/`Running`/`Stopping` and `Idle`/`Queued`/`InFlight` states. Admission and terminal closure are checked under the service mutex. A second stop waits for the first join; restart keeps display IDs unique; terminal shutdown cannot restart.
- Removed queued display IDs on unregister, discarded results from unregistered or stopping displays, and moved superseded request/snapshot/slot ownership out of the mutex before retirement. The worker still performs CPU preparation outside the mutex and keeps one replaceable pending request per display.
- Required a matching sample rate before reusing a completed curve or overlay. Preserved pending FFT payload inheritance and in-flight predecessor overlay carry-forward.
- Made `start()` and `submitLatest()` report admission. Bifurx falls back to synchronous target preparation if registration or submission fails, and advances accepted submission counters only after successful admission.
- Moved main-plugin worker shutdown to the start of `destroy()` and updated the Pro bootstrap template to match. The existing Pro-owned bootstrap is a separate checkout and was not edited.
- Added test-only claim, publication, and pre-admission gates to the real service. Production builds exclude these hooks.

## Verification

| Check | Result |
|---|---|
| Native toolchain | MINGW64, `x86_64-w64-mingw32` |
| Focused Bifurx runtime suite | Passed, 63 tests; repeated ten times before the last invariant guard, then passed again in final `test-fast` |
| Native `make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"` | Passed on final tree; 109,950 checks, zero failures |
| Native `make -j10 plugin.dll` | Passed; `plugin.dll` SHA-256 `D28C060819724930E34C4DC89882A8E8A074CEFCA4BB01695DB5D0F1B2D9B99B` |
| Native `make -j10 test-bifurx-gl RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"` | Passed; 69 GL checks on NVIDIA RTX 3090 |
| `git diff --check` | Passed |
| Pro export `--dry-run` | Passed as a read-only audit; reports 15 files of drift in the sibling checkout, including current worker changes and pre-existing differences |

New deterministic runtime coverage checks admission before start and after shutdown, failed start, restart and ID exhaustion, queue boundedness during display churn, flooded-display fairness, queued and in-flight unregister, retirement outside the scheduling mutex, matching and mismatched payload inheritance, in-flight completed-overlay carry-forward, concurrent stop/join, terminal shutdown during stop, and late admission after closure. Existing audio, pool, fallback, coalescing, and GL tests remain in the runners.

## Validation still needed

- **Not run:** before/after interactive Rack `Step`/`Draw` and worker-latency measurements. There was no pinned live-Rack measurement from before the edit, so the performance acceptance comparison in the specification remains open. The `Process`, `Step`, and `Draw` timing boundaries were not changed.
- **Not run:** manual Rack add/remove, visibility, renderer switching, window reopen, and host shutdown matrix. The native GL test exercises a real context but does not replace those host workflows.
- **Not run:** premium build/DRM validation. No source closure changed. `tools/sync_bifurx_to_pro.py --dry-run` was non-mutating; the existing sibling Pro checkout still calls worker shutdown after settings persistence in its Pro-owned `src/plugin.cpp` and requires a separate authorized bootstrap update before claiming matched Pro shutdown behavior.

These remaining checks belong in the later integration and acceptance work from `bifurx-render-lifecycle-implementation-spec.md`. This record covers the in-place service milestone only; display-client extraction and source-layout phases 3–4 have not begun.
