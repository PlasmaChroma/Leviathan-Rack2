# Bifurx render lifecycle: phases 3–4 implementation

Date: 2026-09-22

## Changes

- Moved spectrum state, synchronous FFT preparation, worker snapshot adoption, animation, and marker layout from `Bifurx.cpp` into `BifurxDisplay.cpp`. Kept SVF and character-stage DSP helpers with `Bifurx.cpp` so the audio compiler can still optimize them together with `process()`.
- Added `BifurxRenderClient` as the UI-thread owner of service registration, request numbering, accepted submission state, applied snapshot state, three reusable payload slots, and submit timing. Release is shared by worker-off, hidden/inactive backend, module rebind, generation/rate invalidation, and destruction. Snapshot cache updates only after display adoption.
- The module widget now sets presentation activity on both backends before child stepping. Inactive displays release their worker registration and skip target preparation and animation. The visible synchronous renderer retains its independent analysis subscription.
- Added explicit module-rebind and preview-rate invalidation. A rate change releases the worker session and prepares a synchronous curve on that tick. Rejected worker submission still uses the synchronous fallback from phases 0–2.
- Split value types, DSP declarations, audio module, preview code, non-hot module state, display declarations, NanoVG presentation, and UI diagnostics into named headers and sources. `BifurxRenderData.hpp` and `BifurxWorker.hpp` no longer include the compatibility umbrella or widget declarations. The module declaration does not own a render client.
- Kept module-level Debug Terminal `Process`, `Step`, and `Draw` scopes in their existing enclosing operations. The `WorkerSubmit` measurement still starts before UI request preparation and includes submission. Recorder storage and submit cadence moved to `BifurxDebug`.
- Updated plugin, runtime, GL, Premium, and Pro-export source lists. Premium validation exposed a missing shared SVG cache closure, so all three `SharedSvgCache` files were added to the export list.

## Validation

| Check | Result |
|---|---|
| Native MINGW64 `plugin.dll` | Passed; SHA-256 `0CCDED0A36F2F48A9C3E1B372B7F1B6472B20C17D64BCA150A768D16977DB787` |
| Focused Bifurx runtime | 67 tests passed, including local client release, inactive presentation, rebind, and rate-change cases |
| Native GL renderer | 69 checks passed on the NVIDIA RTX 3090 |
| Native `test-fast` | 109,950 checks passed with zero failures on the final candidate |
| Isolated Premium build and DRM licensing test | Passed on the final candidate; all five licensing checks passed |
| Pro export `--dry-run` | Read-only audit succeeds; 25 files differ in the sibling checkout, with no removal of the required SVG cache |
| `git diff --check` | Passed; Git reports only a line-ending conversion warning for `BifurxWorker.cpp` |

## Remaining acceptance

- Run the manual Rack matrix: add/remove Bifurx, switch worker/backend and visibility, reopen the host window, and close the host with a worker job in flight. The native GL test exercises a real graphics context but does not cover those host workflows.
- Capture comparable before/after Rack `Step`/`Draw` and worker-latency distributions from a pinned patch and environment. Phase 0 had no live baseline, so a performance non-regression claim cannot be made from tests alone.
- The sibling Pro-owned `src/plugin.cpp` still persists visual settings before worker shutdown. The isolated Premium build links that bootstrap but does not establish corrected shutdown ordering. Its small reorder must be made through the Pro checkout's ownership workflow; this workspace and the read-only export check did not edit it.
- The runtime and GL harnesses still include the runtime implementation directly. Each newly extracted source is linked once through explicit target lists, but conventional fixture/linkage cleanup from the implementation spec remains a separate structural follow-up.
- Phase 5's shader/geometry split is optional and was not undertaken. Phase 6 final acceptance remains open until the manual and performance checks above are complete.

No files were staged or committed.
