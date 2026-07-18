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

There are no known P0 or P1 issues after the fixes made during this review and
the subsequent Stoermelder MB coexistence fix. The
archive now has exclusive cross-process ownership with a memory-only fallback,
browser cache misses use the budgeted executor, successful captures retire live
module trees immediately, and archive shutdown cancels queued work plus observes
cancellation during QOI encode and chunked I/O. Filesystem flush and rename
latency remains OS-controlled, as it is for any safely joined file worker.

## Severity scale

- **P0 — Critical:** likely crash, unsafe memory access, or destructive data
  loss in ordinary use.
- **P1 — High:** credible hangs, archive corruption, or severe UI/resource
  degradation in supported use.
- **P2 — Medium:** incorrect status/invalidation, recoverable lifecycle gaps,
  or performance problems with narrower triggers.
- **P3 — Low:** hardening, diagnostics, portability, or documentation debt.

## P1 findings resolved

### P1-1: Inter-process archive ownership

**Resolved.** `archive-v1.lock` is held exclusively for the archive worker's
entire disk lifetime. Windows uses an exclusive Unicode `CreateFileW` handle;
POSIX uses nonblocking `flock`. A contender enters `BUSY`, performs no database
reads or writes, and leaves browser warming operational in memory. Other lease
failures are reported as archive error code 6 rather than being mistaken for
contention. The archive spec exercises simultaneous workers and denied writes.

### P1-2: Budgeted construction for visible cache misses

**Resolved.** `draw()` now submits a deduplicated on-demand index and paints the
placeholder. Both on-demand and planner work pass through the same executor,
four-item ceiling, and configured UI-time budget. On-demand work continues even
without an active full-cache pass and always enters the framebuffer/persistence
pipeline.

### P1-3: Same-session retirement of live preview trees

**Resolved.** After framebuffer readback, the card immediately replaces its
live third-party widget hierarchy with `DeepcacheRasterWidget` and releases the
framebuffer through the existing lifecycle path. The raster and pending archive
write share one immutable RGBA allocation, avoiding a second full pixel copy.
Raster upload remains context-aware and retryable.

### P1-4: Bounded cooperative shutdown

**Resolved to the strongest safe joined-thread guarantee available here.**
Shutdown rejects new work, immediately drops queued writes/decoded results, and
the QOI encoder checks cancellation every 4,096 pixels. Pack/compaction writes
check between 1 MB chunks and pack reads between 4 MB chunks. The join can now
wait only for a small compute/I/O chunk or an already-entered filesystem
flush/rename call. The latter is controlled by the OS and cannot safely be
aborted without detaching a thread that owns archive state. Physical-disk and
network-mounted user folders remain part of the authoritative Windows test
matrix.

### P1-5: Stoermelder MB-first browser collision

**Resolved after reproduction by the user.** Adding Deepcache to a rack that
already contained Stoermelder MB could freeze Rack while Deepcache detached and
traversed MB's live replacement browser as though it were Rack's stock browser.
Deepcache now detects both `Stoermelder-P1/Mb` and the legacy
`Stoermelder-PackTau/Mb` before reading or modifying the browser slot. It enters
an inert `STANDBY / MB ACTIVE` state and creates no cache manager, overlay, warm
host, archive thread, or browser tree. Removing and re-adding Deepcache after MB
is removed activates it normally.

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
13. **Exclusive archive lease.** A process holds `archive-v1.lock` across load,
    append, index publication, and compaction. Contention is a visible
    `MEMORY ONLY / DATABASE IN USE` state rather than a corruption risk.
14. **Budgeted on-demand construction.** Browser drawing no longer constructs
    third-party module widgets. Visible misses are deduplicated and consumed by
    the same bounded UI executor as planned work.
15. **Same-session raster retirement.** Successful captures immediately delete
    live preview widget/framebuffer trees and install context-safe raster cards;
    archive encoding shares their immutable pixel ownership.
16. **Finer shutdown cancellation.** Queued memory is released immediately;
    QOI encode and payload/compaction writes now observe cancellation inside a
    single preview rather than only between previews.
17. **MB-first compatibility gate.** Known Stoermelder MB modules are detected
    before any browser mutation. Deepcache remains visibly inert instead of
    stacking two incompatible owners of Rack's raw browser pointer.

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
  restart, stale fingerprint rejection, simultaneous-worker lease contention,
  raced-write release in memory-only mode, cancellation, compaction, corrupt
  pack, corrupt index, repair, and a second restart. Test pixels include varying
  alpha to exercise the cancelable QOI encoder's RGBA path.
- `build/tests/deepcache_planner_spec`: 11/11 passed.
- `build/src/Deepcache.cpp.o`: compiled cleanly with the Rack SDK headers.
- Archive test under AddressSanitizer and UndefinedBehaviorSanitizer: passed
  with leak detection disabled because LeakSanitizer is unavailable under this
  environment's ptrace wrapper.
- ThreadSanitizer binary compiled, but the runtime could not start in this WSL
  environment (`unexpected memory mapping`), so no TSan result is claimed.
- Final full plugin linking is intentionally not treated as authoritative in
  WSL; it remains a Windows/MSYS2 validation item.

## Recommended remaining implementation order

1. Replace per-card promotion with bulk/viewport prioritization.
2. Strengthen artifact/resource fingerprints and runtime theme invalidation.
3. Clarify global-coverage versus scoped-run semantics for the cyan bar.
4. Add persistent-upload retries, memory telemetry/limits, and corruption
   diagnostics.
