# Deepcache – Opus Review (2026-07-19)

Comprehensive review of the Deepcache module preview caching system.
Files reviewed: `Deepcache.hpp/cpp`, `DeepcacheArchive.hpp/cpp`, `DeepcachePlanner.hpp/cpp`,
`DeepcacheBrowserLogic.hpp/cpp`, `deepcache_archive_spec.cpp`, `deepcache_planner_spec.cpp`.

---

## Summary

Deepcache is a VCV Rack module browser replacement that pre-renders module previews into
QOI-encoded images, persists them in an append-only pack file with a binary index, and
serves them as NanoVG textures. The architecture spans three threads (audio, UI, archive
worker) plus a planner worker, connected by atomics, mutexes, and condition variables.

Overall the system is well-engineered — separation of concerns between Planner, Archive,
BrowserLogic, and CacheManager is clean; corruption recovery is tested; shutdown paths
are cancellation-aware. The issues below are genuine but most are moderate-severity
hardening items rather than showstoppers.

---

## Issues by Severity

### 🔴 P0 — Correctness / Data Loss Risk

#### DC-01: Pack file grows unboundedly on repeated fingerprint-unchanged re-imports

**File:** `DeepcacheArchive.cpp` lines 686–730

`appendPreview()` does not check whether an entry with the same `cacheKey` already exists
in `entries_` before appending. If the same model preview is submitted more than once in
the same session (e.g. via `warmFramebuffers` → `captureFramebuffer` flow on context
re-create), the old encoded payload remains in `packedBytes_` / the pack file as dead
bytes, and only the index is overwritten.

The auto-compaction heuristic (64 MB dead bytes AND ≥25% dead) protects against eventual
disk exhaustion, but for users with large module collections this can cause unnecessary
pack growth between compactions. More importantly, during the session the in-memory
`packedBytes_` vector keeps growing, since the old bytes are never reclaimed until the
next compaction + re-read cycle.

**Recommendation:** Before appending, check if `entries_` already contains the key with a
matching fingerprint and skip the write. This is a fast map lookup and prevents all
redundant I/O.

---

#### DC-02: `compactArchive()` re-reads the entire pack file after commit

**File:** `DeepcacheArchive.cpp` lines 839–843

```cpp
std::vector<std::uint8_t> compactedBytes;
if (!readPackFile(packPath_, compactedBytes))
    return false;
packedBytes_.swap(compactedBytes);
```

After a successful compaction the code has already written every live entry into the
temporary pack and already has a correct `compacted` entry map. Rather than rebuilding
`packedBytes_` from the just-written data, it re-reads the full file from disk. If the
file is 200+ MB, this is an unnecessary I/O+memory spike on the worker thread. More
critically, if the re-read fails (disk full, permission issue), the function returns
`false` which triggers a **fatal error** (error code 3), even though the compaction itself
succeeded and the on-disk state is perfectly valid.

**Recommendation:** Build `packedBytes_` from the data already written during compaction
instead of re-reading from disk. This eliminates a failure mode and halves peak memory.

---

#### DC-03: `loadArchive` recovery doesn't handle partial backup state

**File:** `DeepcacheArchive.cpp` lines 562–588

When a `compaction-v1.pending` marker exists, recovery tries to restore from `.bak` files.
If the pack backup exists but the index backup does not (a narrow window during the
compaction commit sequence), recovery restores the old pack but then falls through to load
the potentially-mismatched index. The checksum validation on each entry prevents corrupt
data from surfacing, but entries may reference wrong offsets, causing all previews to be
treated as cache misses and rebuilt unnecessarily.

This is not a data-corruption risk (checksums catch it) but it does mean a startup after
a rare crash window rebuilds the entire cache from scratch.

**Recommendation:** If the marker exists and one backup is missing, log a warning and
treat the archive as empty rather than loading a potentially inconsistent pair.

---

### 🟡 P1 — Threading / Lifecycle

#### DC-04: `PreviewPlannerWorker` thread starts unconditionally in constructor

**File:** `DeepcachePlanner.cpp` lines 169–171

```cpp
PreviewPlannerWorker::PreviewPlannerWorker()
    : thread_(&PreviewPlannerWorker::run, this) {
}
```

The thread is spawned immediately on construction, before any work is submitted. This
means every `PreviewCacheManager` (one per active Deepcache widget) has a background
thread spinning on `condition_.wait()` even when the cache is disabled, in standby mode,
or the user has never pressed "Start cache."

In Rack Pro with multiple Rack contexts, each context with a Deepcache module spawns its
own planner thread even if the module is a duplicate instance that immediately enters
DISABLED state.

**Recommendation:** Defer thread creation to the first `submit()` call, or add a
`start()` method. The `DeepcacheArchiveWorker` already follows this pattern correctly.

---

#### DC-05: `PreviewCacheManager::stop()` doesn't clear dangling `browser_` pointer

**File:** `Deepcache.cpp` lines 502–511

After `stop()`, the `browser_` pointer is left pointing at a `DeepcacheBrowser` that may
be deleted by the overlay's `retireForSuccessor()` or `restore()` paths. While `stopped_`
guards most code paths, some const query methods like `residentPreviewCount()` and
`framebufferReadyPreviewCount()` read through `browser_` without checking `stopped_`.

These are only called from the context menu submenu lambda (lines 2719–2723), which
captures a raw `PreviewCacheManager*` pointer. If the menu is open when the widget is
being destroyed, the lambda could fire on a stopped manager with a stale browser pointer.

**Recommendation:** Set `browser_ = nullptr` in `stop()` and add null guards to the query
methods that dereference it.

---

#### DC-06: Archive worker `canAcceptWrite()` returns `true` when not started

**File:** `DeepcacheArchive.cpp` lines 286–292

```cpp
bool DeepcacheArchiveWorker::canAcceptWrite() const {
    if (!started_ || canceled() || fatalError_... || leaseUnavailable_...)
        return true;  // ← returns true when not started
```

This is semantically inverted. When the archive is not started, `enqueue()` will correctly
reject the write, but the `warmFramebuffers()` loop in `PreviewCacheManager` uses
`canAcceptWrite()` as a gate — if it returns true, it proceeds to do expensive
framebuffer rendering and capture work, only to have the `enqueue()` silently fail.
The wasted work is the perf concern.

**Recommendation:** Return `false` when `!started_`. The intention appears to be "don't
backpressure the UI when the archive can't store data anyway" but the effect is wasted CPU
on framebuffer captures that go nowhere.

---

#### DC-07: Context menu captures raw `PreviewCacheManager*` with no lifetime guard

**File:** `Deepcache.cpp` lines 2679–2727

```cpp
PreviewCacheManager* manager = internal_->cacheManager;
menu->addChild(createMenuItem("Start cache", "", [manager]() { manager->start(); }));
```

All context menu lambdas capture `manager` by raw pointer. If the user deletes the
Deepcache module while a context menu is still open, the lambdas hold a dangling pointer.
VCV Rack does not guarantee menu destruction before `ModuleWidget` destruction.

This is the same pattern class as DC-05 but for external-facing UI. The probability is
low (requires deleting the module with its own context menu open) but the consequence is
a crash.

**Recommendation:** Capture a `std::weak_ptr` or use a reference-counted guard. Alternatively,
check `APP->scene->rack->hasModule()` inside each lambda.

---

#### DC-08: `DeepcacheBrowserOverlay` stores raw `previousBrowser` pointer indefinitely

**File:** `Deepcache.cpp` lines 1043–1055

When a Deepcache overlay is installed, it saves `scene->browser` as `previousBrowser`.
If a second Deepcache is added and then the first is removed, the first's overlay calls
`retireForSuccessor()` which keeps it alive as a shell. The second overlay's
`previousBrowser` now points at this retired shell. If the retired shell is later
garbage-collected (it's not currently, but future refactoring could), this becomes a
dangling pointer chain.

The code has a comment acknowledging this is intentional ("Rack provides no chain
notification mechanism"), and the `step()` guard on `retired` handles the re-healing case.
This is acceptable given Rack's ownership model but worth documenting as a known fragility.

**Recommendation:** Document the invariant that retired overlays must never be deleted
while a successor exists. Consider adding a debug assertion.

---

### 🟢 P2 — Performance

#### DC-09: `promote()` on `PreviewPlannerWorker::output_` is O(n) linear scan + erase/insert

**File:** `DeepcachePlanner.cpp` lines 219–232

Every time the user scrolls the browser and new model boxes become visible, `promote()`
is called for each visible model. Each call does a linear scan of the output deque (which
can contain thousands of entries for large module collections), followed by an erase from
the middle and an insert at the front — both O(n) on a deque.

During browser refresh (which calls `refresh()` → for each visible box calls
`cacheManager->promote()`), this is O(V × N) where V is visible boxes and N is queue
depth.

**Recommendation:** For the common case, consider a priority queue or at minimum a
hash-set index mapping `modelIndex → deque position` to turn the scan into O(1) lookup.

---

#### DC-10: `pendingRequestCount()` scans entire output deque under lock

**File:** `DeepcachePlanner.cpp` lines 259–266

```cpp
return static_cast<std::size_t>(std::count_if(output_.begin(), output_.end(), ...));
```

This is called every `step()` frame from `finishIfComplete()` while the cache is warming.
For a 2000-module collection this is 2000 iterations under mutex lock per UI frame,
blocking the worker thread from `tryPop()` operations.

**Recommendation:** Maintain a counter that is decremented on `tryPop()` and reset on
`submit()`/`cancel()`, replacing the linear scan.

---

#### DC-11: `DeepcacheBrowser` constructor calls `lowercase()` repeatedly for sort

**File:** `Deepcache.cpp` lines 1649–1652

```cpp
std::stable_sort(models.begin(), models.end(), [](plugin::Model* a, plugin::Model* b) {
    return std::make_tuple(lowercase(a->plugin->brand), ...) <
           std::make_tuple(lowercase(b->plugin->brand), ...);
});
```

`lowercase()` allocates and returns a new string on every comparison. For a sort of N
models this is O(N log N) string allocations. The `BrowserModelRecord` already has
normalized fields, but they aren't populated until after this sort.

**Recommendation:** Pre-compute lowercase keys once into a parallel vector, then sort
by index. This is only called once during browser construction so it's not critical-path,
but it does contribute to browser open latency which is user-visible.

---

#### DC-12: Triple `lowercase()` function definitions across three TUs

**Files:** `DeepcacheBrowserLogic.cpp:11`, `DeepcachePlanner.cpp:12`, `Deepcache.cpp:83`

Identical `lowercase()` helper defined in anonymous namespaces in three different files.
Not a bug but code duplication that should be factored out.

**Recommendation:** Move to a shared utility header.

---

### 🔵 P3 — Robustness / Hardening

#### DC-13: No upper bound on total `packedBytes_` size in memory

**File:** `DeepcacheArchive.cpp` lines 596–598

`readPackFile()` loads the entire pack file into `packedBytes_` with no size cap. The
pack file grows as previews are appended. For very large module collections (2000+
modules × ~50 KB QOI each = ~100 MB), this is a significant memory commitment. The
compaction heuristic requires 64 MB of dead bytes before triggering, so the steady-state
memory for the pack mirror can be 200+ MB.

**Recommendation:** Consider an mmap-based approach or lazy loading of individual entries
on demand rather than holding the entire pack in memory. Alternatively, add a configurable
size cap that triggers compaction earlier.

---

#### DC-14: `encodeQoiCancelable` accesses `rgba` without bounds check

**File:** `DeepcacheArchive.cpp` lines 81–85

```cpp
const std::size_t offset = i * 4u;
QoiPixel pixel(rgba[offset], rgba[offset + 1], rgba[offset + 2], rgba[offset + 3]);
```

The function trusts that `rgba.size() >= pixelCount * 4`. The caller in `appendPreview()`
does validate `byteCount == write.rgba->size()` before calling, so this is guarded
externally. However, the function itself has no precondition check.

**Recommendation:** Add `assert(rgba.size() >= pixelCount * 4)` as a debug invariant.

---

#### DC-15: `readString` allows up to 1 MB string length from untrusted index file

**File:** `DeepcacheArchive.cpp` lines 166–171

```cpp
if (!readValue(stream, length) || length > 1024u * 1024u)
    return false;
value.resize(length);
```

While 1 MB is a reasonable guard, a corrupted or maliciously crafted index file could
contain many entries each claiming a 1 MB string, causing the loading loop to allocate
gigabytes before failing. The entry count cap is 1,000,000 entries × 2 strings per entry
× 1 MB = 2 TB theoretical worst case.

**Recommendation:** Reduce the per-string limit to something more realistic (e.g. 4096
bytes for a cache key, 256 bytes for a fingerprint) and add a total-bytes-consumed guard
to the index loading loop.

---

#### DC-16: Index file uses native byte order — non-portable across architectures

**File:** `DeepcacheArchive.cpp` lines 156–163

`readValue` / `writeValue` use `reinterpret_cast` to read/write `uint64_t`, `uint32_t`
values directly, inheriting the host's byte order. If a user copied their Deepcache
directory between a big-endian and little-endian system, the index would be unreadable.

In practice, VCV Rack targets x86/ARM64 (all little-endian), so this is not a real
concern today. It would become one if Rack ever targets a big-endian platform.

**Recommendation:** Acceptable for now. Document the assumption.

---

#### DC-17: `DeepcacheModule::dataFromJson` double-reads `json_string_value` without null check

**File:** `Deepcache.cpp` line 2488

```cpp
const std::string scope = json_string_value(value) ? json_string_value(value) : "all";
```

If `value` is a JSON type other than string (e.g. integer due to patch corruption),
`json_string_value()` returns `NULL`. The ternary handles this correctly, but
`json_is_string(value)` would be a more defensive guard consistent with the pattern
used for `autoStart` (which uses `json_boolean_value` directly, which has its own
safe fallback).

**Recommendation:** Add `json_is_string(value)` check for consistency.

---

### ⚪ P4 — Testing / Documentation

#### DC-18: Archive test doesn't exercise interrupted compaction recovery

The `deepcache_archive_spec.cpp` tests cover: append, reload, fingerprint invalidation,
compaction, shutdown-during-compaction, pack corruption, and index corruption.

Missing scenarios:
- **Interrupted compaction with partial file writes**: The marker/backup recovery code in
  `loadArchive` is exercised indirectly by the shutdown-during-compaction test, but the
  specific edge case of "marker exists + pack backup exists + index backup missing" (the
  narrow window from DC-03) is not tested.
- **Concurrent read-only workers during owner compaction**: The test creates a contender
  that enters READ_ONLY, but doesn't test what happens if the owner compacts while the
  contender is loading (the 100ms retry loop in `run()`).
- **Queue backpressure**: No test verifies that `enqueue()` correctly rejects writes when
  the queue is at `kMaxWriteQueueEntries` or `kMaxWriteQueueBytes`.

**Recommendation:** Add targeted tests for these edge cases. The existing corruption tests
are excellent and provide good confidence in the core recovery paths.

---

#### DC-19: Planner test uses `sleep_for` for synchronization

**File:** `deepcache_planner_spec.cpp` line 165

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(10));
const bool remainedPaused = !worker.isPlanReady(200);
```

This relies on a timing assumption that 10ms is enough for the worker thread to *not*
complete its work (which it shouldn't because it's paused, but on a heavily loaded CI
machine the thread might not even have woken up yet). The assertion is correct in
practice but formally racy.

The archive spec avoids this by using `waitUntil()` with a predicate, which is the
better pattern.

**Recommendation:** Not a real issue given the semantics (paused worker can't progress),
but worth noting.

---

## ✅ Strengths Worth Highlighting

| Area | Detail |
|------|--------|
| **Corruption handling** | Checksum validation on every loaded entry; fingerprint-based staleness detection; graceful degradation to cache miss rather than crash or corrupt display |
| **Compaction safety** | Multi-stage commit with marker file, backup/restore, and cancellation awareness at every I/O boundary |
| **Lease-based exclusion** | Clean file-locking protocol prevents two Rack instances from corrupting the shared archive; contender gracefully degrades to read-only |
| **Cancellation** | `stopping_` atomic checked at encoding boundaries, write chunk boundaries, and condition waits — enables fast shutdown |
| **Scene ownership** | `gDeepcacheSceneOwners` mutex-guarded map correctly handles Rack Pro's multi-context architecture |
| **Stoermelder MB detection** | Proactive conflict avoidance prevents two browser replacements from corrupting each other |
| **Browser chaining** | The overlay's `previousBrowser` backup/restore mechanism is delicate but well-reasoned for Rack's lack of browser lifecycle hooks |
| **UI budget control** | Configurable per-frame time budget prevents preview construction from janking the UI |
| **Test coverage** | Archive spec covers the full lifecycle including corruption repair; planner spec covers priority ordering, scoping, generation management, and browser filter/sort parity |

---

## Architecture Diagram

```
                        ┌──────────────────┐
                        │ DeepcacheModule   │ (audio thread – lights only)
                        │    atomics ──────►│
                        └────────┬─────────┘
                                 │
                        ┌────────▼─────────┐
                        │ DeepcacheWidget   │ (UI thread)
                        │  ┌──────────────┐ │
                        │  │ Internal     │ │
                        │  │  ├ overlay   │ │
                        │  │  ├ manager ──┤─┼──────────────────────────┐
                        │  │  └ warmHost  │ │                          │
                        │  └──────────────┘ │                          │
                        └──────────────────┘                          │
                                                                       │
                   ┌───────────────────────────────────────────────────▼──┐
                   │                PreviewCacheManager (UI thread)        │
                   │   ┌─────────────┐  ┌──────────────────┐             │
                   │   │ PlannerWorker│  │ArchiveWorker     │             │
                   │   │(own thread)  │  │(own thread)      │             │
                   │   │submit/tryPop │  │enqueue/tryPop    │             │
                   │   └──────┬──────┘  │  ┌────────────┐  │             │
                   │          │         │  │Pack + Index │  │             │
                   │          │         │  │  (on disk)  │  │             │
                   │          │         │  └────────────┘  │             │
                   │          │         └──────────────────┘             │
                   │   ┌──────▼──────────────────────────────┐          │
                   │   │   MemoryPreviewCacheBackend          │          │
                   │   └─────────────────────────────────────┘          │
                   │   ┌─────────────────────────────────────┐          │
                   │   │   DeepcacheBrowser → ModelBox[]     │          │
                   │   └─────────────────────────────────────┘          │
                   └────────────────────────────────────────────────────┘
```

---

## Issue Index

| ID | Severity | Category | Summary |
|----|----------|----------|---------|
| DC-01 | 🔴 P0 | Correctness | Redundant appends grow pack file unnecessarily |
| DC-02 | 🔴 P0 | Correctness | Compaction re-reads pack file, can cause false fatal error |
| DC-03 | 🔴 P0 | Correctness | Partial backup recovery may cause full cache rebuild |
| DC-04 | 🟡 P1 | Threading | Planner thread spawns unconditionally on construction |
| DC-05 | 🟡 P1 | Lifecycle | `stop()` leaves dangling `browser_` pointer |
| DC-06 | 🟡 P1 | Correctness | `canAcceptWrite()` returns true when not started |
| DC-07 | 🟡 P1 | Lifecycle | Context menu lambdas capture raw pointer |
| DC-08 | 🟡 P1 | Lifecycle | Overlay chains raw `previousBrowser` pointer indefinitely |
| DC-09 | 🟢 P2 | Performance | `promote()` is O(n) linear scan on large queue |
| DC-10 | 🟢 P2 | Performance | `pendingRequestCount()` scans deque under lock every frame |
| DC-11 | 🟢 P2 | Performance | Repeated `lowercase()` allocations in sort comparator |
| DC-12 | 🟢 P2 | Code quality | Triple `lowercase()` definition across TUs |
| DC-13 | 🔵 P3 | Robustness | No upper bound on in-memory pack file mirror |
| DC-14 | 🔵 P3 | Robustness | QOI encoder trusts rgba size without precondition |
| DC-15 | 🔵 P3 | Robustness | 1 MB string limit in index reader is too permissive |
| DC-16 | 🔵 P3 | Portability | Index file uses native byte order |
| DC-17 | 🔵 P3 | Robustness | `dataFromJson` missing type check on scope field |
| DC-18 | ⚪ P4 | Testing | Missing edge-case tests for recovery and backpressure |
| DC-19 | ⚪ P4 | Testing | Sleep-based synchronization in planner test |
