# Deepcache correctness, performance, and lifecycle review

Date: 2026-07-18

## Executive assessment

Deepcache has a sound basic thread boundary: Rack widgets, NanoVG, OpenGL, and
module construction stay on Rack's UI/draw thread; the planner worker handles
only copied descriptors; and the archive worker owns file I/O plus QOI work.
The active-widget destructor removes the draw host, joins both workers, and only
then tears down the browser tree. NanoVG image handles are context-owned and are
invalidated through the shared graphics-lifecycle helper.

The persisted archive now fails safely for the corruption cases exercised in
tests. A corrupt index becomes an empty cache. An invalid offset, checksum, QOI
payload, fingerprint, or decoded dimension becomes a cache miss. No failed
payload reaches NanoVG, and a later render can append a replacement that is
readable on the following launch. Interrupted compaction retains a transaction
marker and backups for restart recovery. Hard I/O or transaction errors stop
that archive worker session instead of allowing later writes to publish offsets
against uncertain pack contents.

There are no known P0 issues after the fixes made during this review. The most
important remaining work is inter-process archive locking, removing unbudgeted
module construction from `draw()`, releasing live preview hierarchies after
raster capture, and deciding how strictly Rack shutdown must be time-bounded.

## Severity scale

- **P0 — Critical:** likely crash, unsafe memory access, or destructive data
  loss in ordinary use.
- **P1 — High:** credible hangs, archive corruption, or severe UI/resource
  degradation in supported use.
- **P2 — Medium:** incorrect status/invalidation, recoverable lifecycle gaps,
  or performance problems with narrower triggers.
- **P3 — Low:** hardening, diagnostics, portability, or documentation debt.

## Open findings

### P1-1: The archive has no inter-process ownership or locking

All Rack processes use the same
`<Rack user>/Leviathan/Deepcache/previews-v1.pack` and `index-v1.bin`. The
archive serializes access inside one `DeepcacheArchiveWorker`, but it has no OS
file lock, lease file, per-process database, or single-writer protocol.
Append offsets are derived from process-local `packBytes_`, while index replace
and compaction rename shared paths ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L437),
[DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L497)). Two standalone/DAW Rack
processes can therefore append using stale offsets, overwrite one another's
indexes, or rename files while the other process is using them.

Checksums should prevent corrupt pixels from being displayed on the next
restart, but they do not prevent loss of cache coverage or live archive errors.
Before release, choose one of:

1. a cross-platform exclusive writer lock with read-only fallback;
2. one pack per process followed by a controlled merge; or
3. a process-specific cache directory and no sharing.

### P1-2: A cache miss visible in the browser constructs a module inside `draw()`

`DeepcacheModelBox::draw()` promotes the request and then immediately calls
`ensurePreviewConstructed()` ([Deepcache.cpp](src/Deepcache.cpp#L1250)). This
bypasses the configured UI budget and the four-items-per-frame executor. A fast
scroll across uncached cards can synchronously call arbitrary third-party
`createModuleWidget(nullptr)` implementations in one draw frame. This is the
path most likely to recreate the perceived fill-in/stutter that Deepcache is
intended to eliminate.

The draw path also does not independently schedule persistence. If no cache
generation is active, an on-demand preview can remain process-local. If normal
browser drawing makes the framebuffer ready before its planner request is
consumed, the existing ready branch does not necessarily run the capture/write
path used by `warmFramebuffers()`.

Recommended fix: `draw()` should render a cheap placeholder and submit a
deduplicated on-demand request. Only the budgeted executor should construct it.
Framebuffer readiness should feed a separate idempotent persistence queue keyed
by model index, regardless of whether readiness came from hidden warming or
normal browser drawing.

### P1-3: Newly built previews retain every live module widget and framebuffer

After a successful warm/capture, RGBA is sent to the archive
([Deepcache.cpp](src/Deepcache.cpp#L617)), but the card continues to own its
`ModuleWidget`, `ZoomWidget`, `FramebufferWidget`, and module-specific child
resources. It is only converted to `DeepcacheRasterWidget` when a persisted QOI
is loaded on a later launch. With roughly 1,700 models, the initial-build
session can retain a very large number of third-party widget trees and GPU
framebuffers. When the browser is visible, their step behavior may also add
ongoing UI cost.

Recommended fix: after successful capture, replace the live hierarchy with a
raster widget in the same session and release the framebuffer through the
existing lifecycle path. Avoid doubling every RGBA buffer by sharing immutable
pixel ownership between the raster card and pending archive write, or by
installing the raster after QOI encoding acknowledges the write.

### P1-4: Shutdown is cooperative but not strictly time-bounded

`shutdown()` signals cancellation and immediately joins the archive thread
([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L194)). Startup and final pack
reads are now chunked and cancellation-aware. Compaction checks between entries.
However, cancellation cannot interrupt a QOI encode, one payload write, a
filesystem flush, index serialization, or rename transaction
([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L412),
[DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L437)). A slow or failing disk can
therefore block removal or Rack shutdown until the current operation returns.

This cannot be solved safely by detaching the thread because it owns object
state and shared files. Options are to reduce the maximum raster size, chunk all
possible writes, avoid forced flush on cancellation, or accept/document a
bounded-to-one-operation shutdown policy. Physical disk or network-mounted user
folders should be included in the authoritative Windows test matrix.

### P2-1: Cache fingerprints do not fully identify rendered plugin assets

The fingerprint includes plugin slug/version/path, the plugin directory's
`modifiedTimestamp`, dark-panel preference, and the sizes of the plugin binary
and `plugin.json` ([Deepcache.cpp](src/Deepcache.cpp#L89)). It does not include
artifact modification times or contents, nor any `res/` files. An in-place
development build that preserves version and binary size can reuse a stale
raster if the directory timestamp is unchanged. Panel/resource-only changes can
also be missed.

The implementation note currently overstates this as reliable development-build
invalidation. A better design is a cheap persisted artifact manifest using
high-resolution mtimes and sizes for the binary, manifest, and relevant resource
tree, with a content hash fallback when metadata is unchanged or unavailable.
Hashing every plugin binary in full on every Rack startup would be correct but
works against Deepcache's startup-performance goal.

### P2-2: The cyan progress denominator ignores the selected cache scope

Framebuffer plugin totals are built once from every installed model
([Deepcache.cpp](src/Deepcache.cpp#L738), [Deepcache.cpp](src/Deepcache.cpp#L822)),
while the planner can run only favorites or visible search results. Such a run
can enter `READY` with the cyan bar permanently partial because untouched models
still count against each plugin's completion.

Define whether cyan means "database coverage of all installed plugin builds" or
"framebuffer completion for this run." If it means global coverage, label it as
coverage and decouple it from the active-run state. If it means stage progress,
build its target map from the scoped descriptor set.

### P2-3: Browser refresh can promote requests in quadratic time

Every search/filter refresh loops over all cards and calls `promote()` for every
filter-visible model ([Deepcache.cpp](src/Deepcache.cpp#L1518)). Each promotion
linearly searches and erases from the planner deque while holding its mutex
([DeepcachePlanner.cpp](src/DeepcachePlanner.cpp#L219)). With most of ~1,700
models visible, typing a character can perform O(N²) queue work on the UI
thread. "Visible" here means filter-visible, not viewport-visible.

Use a bulk-priority update with an indexable queue/set, or promote only cards in
the scroll viewport. Search refresh should never issue thousands of individual
mutex acquisitions and deque scans.

### P2-4: Same-process panel-theme changes do not invalidate raster previews

The startup fingerprint includes `preferDarkPanels`, but a runtime preference
change only sends a dirty event to the model container
([Deepcache.cpp](src/Deepcache.cpp#L1480)). Existing `DeepcacheRasterWidget`
pixels remain from the previous theme. Invalidate/reload the raster set when the
preference changes, or explicitly state that a Deepcache rebuild/restart is
required.

### P2-5: A transient NanoVG upload failure is not retried by the startup queue

`uploadPersistentImages()` pops an index before `ensurePersistentImage()` and
does not requeue it when image creation fails
([Deepcache.cpp](src/Deepcache.cpp#L788)). A transient allocation/context failure
can leave that card resident but not manager-counted as framebuffer ready,
making cyan coverage stick below 100%. Add a bounded retry count and retain the
RGBA fallback even after retries are exhausted.

### P2-6: Removing the active Deepcache does not promote a passive instance

Only the first widget becomes `gActiveDeepcacheWidget`; later instances are
permanently marked passive ([Deepcache.cpp](src/Deepcache.cpp#L2197)). When the
active widget is destroyed, the global is cleared, but an existing passive
widget is not elected ([Deepcache.cpp](src/Deepcache.cpp#L2218)). The custom
browser therefore disappears until a new Deepcache is added or the patch is
reloaded. Maintain a UI-thread registry and promote the oldest surviving
instance after browser restoration.

### P2-7: Full-pack residency has no memory ceiling or admission policy

Loading the entire compressed pack is an explicit latency choice, but
`readPackFile()` sizes a vector from the complete file length
([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L268)). The worker now catches
allocation exceptions and reports an archive error, yet a very large valid pack
can still cause severe process memory pressure or an OS-level kill before C++
recovery. The decoded queue is bounded, but installed raster cards intentionally
retain RGBA for graphics-context recovery.

Expose expected/actual RAM use, define a supported ceiling, and consider a
hybrid policy: keep the QOI pack hot, retain decoded RGBA for recently used or
currently visible cards, and decode other in-RAM QOI payloads on demand. This
avoids disk hits without requiring every preview in three forms (compressed
RAM, decoded RAM, and GPU image).

### P2-8: Framebuffer capture uses synchronous GPU readback on the UI thread

`captureFramebuffer()` calls `glGetTexImage()` directly
([Deepcache.cpp](src/Deepcache.cpp#L1065)). The cooperative budget is checked
only between cards, so one large/stalled readback can exceed the selected frame
budget substantially. Measure this on representative GPUs. If it is material,
use a staged PBO/fence pipeline or reduce capture dimensions; either requires
careful context-recreation handling.

### P3-1: Recoverable corruption has no user-visible repair diagnostic

Corruption is safely converted into cache misses, but the panel reports
`EMPTY`/`UPDATING`, not how many index or payload records were rejected. Add
recoverable-corruption counters and a debug-gated log packet. Reserve `ERROR`
for I/O or recovery failures that prevent archive operation.

### P3-2: Index publication is atomic by rename but not power-loss durable

Streams are flushed, but files and containing directories are not explicitly
`fsync`/`FlushFileBuffers`-ed before backups are removed. A machine power loss
can lose the most recent cache writes even when the process observed a successful
rename. This does not threaten patches or user content—the preview database is
disposable—but the durability claim should be "crash recoverable under normal
filesystem semantics," not fully power-loss transactional.

### P3-3: Browser successor compatibility intentionally retains a shell

When another plugin replaces Deepcache and Deepcache is removed first, it keeps
a retired overlay shell alive to protect a successor's raw backup pointer
([Deepcache.cpp](src/Deepcache.cpp#L969)). This is defensible given Rack's raw
`Scene::browser` API, but it is an intentional lifetime extension that should
remain in manual removal-order and shutdown tests.

### P3-4: Database-directory creation failure is reported as `EMPTY`

If `<Rack user>/Leviathan/Deepcache` cannot be created, initialization emits a
warning and returns before starting the archive worker
([Deepcache.cpp](src/Deepcache.cpp#L738)). Memory warming still works, which is a
good fallback, but the panel remains `EMPTY` rather than explaining that
persistence is unavailable. Publish a dedicated database error code while
continuing the in-memory cache path.

## Fixes made during this review

1. **Bounded persisted startup flow.** The decoded handoff is limited to 16
   entries/64 MB with worker backpressure, and UI installation is limited by the
   configured frame budget. Auto-start waits until both decoded and upload
   queues are empty ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L150),
   [Deepcache.cpp](src/Deepcache.cpp#L732)).
2. **Bounded archive write production.** Framebuffer warming pauses when the QOI
   writer queue reaches 16 entries or 64 MB, preventing an initial cache pass
   from accumulating an unbounded RGBA backlog
   ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L132),
   [Deepcache.cpp](src/Deepcache.cpp#L625)). A single accepted preview can exceed
   the byte threshold, but input validation caps one preview at 128 MB.
3. **Cancelable sequential reads.** Startup and post-compaction pack reads now
   use 4 MB chunks and observe shutdown between chunks
   ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L268)).
4. **Fatal archive-session handling.** Startup I/O failure, append failure,
   compaction failure, and unexpected worker exceptions now put the archive in
   `ERROR` and stop subsequent writes for that worker session. This prevents a
   partial append from making later process-local offsets unsafe
   ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L205)).
5. **Commit-before-ready ordering.** A newly encoded preview is counted ready
   only after its index has been atomically published
   ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L487)).
6. **Stronger input and corruption validation.** Writes must match a wanted key,
   fingerprint, dimensions, byte count, and 128 MB limit. Reload removes stale,
   out-of-range, checksum-failed, QOI-failed, and dimension-mismatched records
   from the live index ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L362),
   [DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L437)).
7. **Compaction rollback hardening.** Failure to create/flush the transaction
   marker restores the previous in-memory index and removes staged files. A
   failed commit tracks each successful rename, never deletes an original whose
   backup step failed, and keeps the marker so the next launch can continue recovery
   ([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L547)).
8. **Explicit corruption restart tests.** The archive spec now damages both the
   QOI pack and binary index, verifies that no corrupt preview is returned, then
   rebuilds and confirms a clean subsequent reload
   ([deepcache_archive_spec.cpp](tests/deepcache_archive_spec.cpp#L186)).
9. **Planner exception containment.** Planner allocations/sort exceptions no
   longer escape a `std::thread` and terminate Rack; the failed generation is
   published to the UI as cache `ERROR`
   ([DeepcachePlanner.cpp](src/DeepcachePlanner.cpp#L283)).
10. **Browser-install rollback.** If custom browser allocation/installation
    throws after the stock browser is detached, Deepcache restores the exact
    previous browser and cleans partial state
    ([Deepcache.cpp](src/Deepcache.cpp#L910)).
11. **Pending-autostart cancellation.** Clearing memory now cancels a start that
    was waiting for archive load, while `rebuild()` can deliberately request it
    again ([Deepcache.cpp](src/Deepcache.cpp#L448)).
12. **Low-risk startup reductions.** Plugin fingerprints are computed once per
    plugin rather than once per model. Planner inputs move to the worker, and
    normalized sort keys are computed once per descriptor rather than inside
    every comparator call.

## Corruption and restart behavior after this review

| Condition | Result |
|---|---|
| Pack absent | Empty cache; normal rendering/appends proceed. |
| Index absent, invalid magic/version, truncated, or structurally invalid | Existing pack is ignored as dead data; cache rebuild proceeds. |
| Entry fingerprint stale | Entry rejected and removed from the live index; model is rebuilt. |
| Offset/length outside pack | Entry rejected before pointer arithmetic or decode. |
| Payload checksum mismatch | Entry rejected; QOI decoder is not called. |
| Malformed QOI or dimension mismatch | Entry rejected; no pixels reach NanoVG. |
| Interrupted index replace with usable `.bak` | Backup index is accepted; missing newest writes rebuild. |
| Interrupted compaction with marker/backups | Old authoritative pair is restored before load. |
| Read, append, index, or compaction I/O failure | Archive worker enters `ERROR`, stops accepting persistence work, and joins safely on teardown. In-memory browser operation can continue. |
| Unexpected archive-worker exception | Converted to database error code 5 rather than terminating the process. |

The database remains a disposable performance cache. Recovery intentionally
prefers dropping coverage and rerendering over attempting to salvage uncertain
pixels.

## Validation performed

- Complete `make test-fast`: passed.
- `build/tests/deepcache_archive_spec`: passed, including append/replacement,
  restart, stale fingerprint rejection, cancellation, compaction, corrupt pack,
  corrupt index, repair, and a second restart.
- `build/tests/deepcache_planner_spec`: 11/11 passed.
- `build/src/Deepcache.cpp.o`: compiled cleanly with the Rack SDK headers.
- Archive test under AddressSanitizer and UndefinedBehaviorSanitizer: passed
  with leak detection disabled because LeakSanitizer is unavailable under this
  environment's ptrace wrapper.
- ThreadSanitizer binary compiled, but the runtime could not start in this WSL
  environment (`unexpected memory mapping`), so no TSan result is claimed.
- Final full plugin linking is intentionally not treated as authoritative in
  WSL; it remains a Windows/MSYS2 validation item.

## Recommended implementation order

1. Add and test a cross-process archive ownership strategy.
2. Replace draw-time construction with a deduplicated budgeted on-demand queue.
3. Convert newly captured live module trees into same-session raster cards.
4. Decide and document the maximum acceptable shutdown wait; then bound the
   remaining noninterruptible operation accordingly.
5. Replace per-card promotion with bulk/viewport prioritization.
6. Strengthen artifact/resource fingerprints and runtime theme invalidation.
7. Clarify global-coverage versus scoped-run semantics for the cyan bar.
8. Add persistent-upload retries, memory telemetry/limits, and corruption
   diagnostics.
