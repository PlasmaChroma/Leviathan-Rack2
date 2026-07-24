# Deepcache correctness, performance, and lifecycle review

Date: 2026-07-19 (post-churn follow-up)

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

There are no known P0 correctness issues after this follow-up. Two P1 risks
remain architectural rather than newly introduced bugs: optional 200% previews
can consume several gigabytes across a large library because compressed bytes
and GPU images are retained; and arbitrary third-party widget construction or
rendering cannot be isolated from a hard in-process hang or fault.

The archive now has exclusive cross-process write ownership with a read-only snapshot fallback,
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

## Open P1 findings

### P1-A: Optional 200% all-GPU residency still has no admission policy — mitigated

The raster format can render at twice the module's logical width and height,
independent of browser zoom and Rack UI scale. That optional 200% setting
quadruples RGBA pixels relative to the default 100% cache. A measured library
around 1,700 modules added at least 1.6 GB with 200% previews while Deepcache
retained the complete QOI pack, every decoded RGBA buffer, and a NanoVG image
for every warmed preview.

Permanent decoded-RGBA residency is now removed for recoverable entries. The UI
releases pixels after GPU upload and a confirmed archive commit; archive-loaded
entries release them immediately after upload. Startup handoff plus pending UI
uploads are bounded, and the Cache statistics menu reports hot QOI, retained
RGBA, pending upload RGBA, and estimated GPU RGBA separately. On graphics-context
recreation, the worker re-decodes from the hot pack without disk I/O, prioritizes
visible cards, tags results by context generation, uploads them, and releases the
temporary pixels again. Read-only DAW workers service the same re-decode requests.
Missing previews rendered by a read-only worker are encoded into volatile QOI
entries, allowing the second DAW context to release RGBA and restore later
without acquiring the database write lease.

The remaining risk is the combination of a fully resident compressed pack and
one uncompressed GPU texture for every card, especially at 200%. Measure the new
steady state on Windows before choosing a GPU admission/LRU policy; retaining
every GPU image is the feature that eliminates first-scroll upload latency.

### P1-B: Third-party preview code cannot be hard-failure isolated in-process

Deepcache proactively calls `createModuleWidget(nullptr)`, `step()`, and drawing
code for every installed model. C++ exceptions are contained, but an infinite
loop, deadlock, access violation, or plugin-level global conflict cannot be
timed out or recovered safely on Rack's UI thread. The proactive all-model pass
increases exposure compared with waiting for the user to scroll to a model.

A persistent user/developer denylist can handle known offenders. True generic
containment would require an out-of-process renderer or host support; detaching
or aborting a stuck UI call is not safe.

## P1 findings resolved

### P1-1: Inter-process archive ownership

**Resolved.** `archive-v1.lock` is held exclusively for the archive worker's
entire disk lifetime. Windows uses an exclusive Unicode `CreateFileW` handle;
POSIX uses nonblocking `flock`. A contender loads the committed pack/index as a
validated read-only snapshot, enters `READ_ONLY`, and leaves browser warming
operational in memory for missing entries. It never repairs, appends, or compacts.
Other lease failures are reported as archive error code 6 rather than being
mistaken for contention. The archive spec exercises simultaneous workers,
snapshot decoding, denial of shared-pack writes, and volatile-QOI fallback.

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
host, archive thread, or browser tree. After MB is removed, the surviving
Deepcache detects the free scene slot and activates automatically.

### P1-6: Detached browser menus retained raw pointers after teardown — resolved

Brand/tag menu items, the sort action, and the failed-preview retry action could
outlive the custom browser because Rack menus are separate scene overlays. If
Deepcache was removed while one remained open, a later menu step or action could
dereference a deleted browser/card. Detached menu objects now carry weak browser
or manager lifetime tokens and become inert after teardown; retry routes through
the manager using a stable model index rather than capturing a card pointer.

### P2-1: Cache fingerprints do not fully identify rendered plugin assets — partially resolved

The stable storage key remains `deepcache-raster-v3-canonical-2x` for in-place
replacement compatibility, while the artifact fingerprint schema now includes
the selected 100% or 200% resolution. Binary/manifest artifact fingerprints also
include file modification time (with subsecond precision where the platform
exposes it) as well as size. Same-version development rebuilds therefore
invalidate normally even when the linked binary size is unchanged.

Resource-only changes can still be missed because recursively statting or hashing
every installed plugin resource would work against the startup-performance goal.
A future persisted artifact manifest can close that gap without repeating the
full resource-tree scan on every launch.

### P2-2: The cyan progress denominator ignores the selected cache scope — resolved

User-selectable cache scopes were removed. Every cache pass now covers all
installed modules. Purple reports the construction stage and cyan reports global
framebuffer/database coverage across installed plugin builds.

### P2-3: Browser refresh can promote requests in quadratic time — resolved

Refresh now collects matching indices and submits one bulk promotion. The worker
partitions its queue once under one mutex acquisition, preserving promoted and
unpromoted ordering. Pending-count queries are constant-time because the output
deque contains only the active generation.

### P2-4: Same-process panel-theme changes do not invalidate raster previews — resolved

A theme change is detected by the always-running manager, clears stale rasters,
recomputes artifact fingerprints, and restarts an active pass. The archive
worker accepts the new fingerprint for each known cache key, so rebuilt images
are persisted immediately and do not require another rebuild after restart.

### P2-5: A transient NanoVG upload failure is not retried by the startup queue — resolved

Persistent uploads now carry a bounded retry count and a failed item is deferred
to a later UI frame, rather than being retried repeatedly in one frame or being
dropped immediately.

### P2-6: Removing the active Deepcache does not promote a passive instance — resolved

Inactive widgets perform a throttled ownership check once per second. A surviving
duplicate automatically claims the scene after the active owner is removed. The
same path promotes an MB-standby Deepcache after Stoermelder MB is removed.

### P2-7: Full-pack residency has no memory ceiling or admission policy — partially resolved

Loading the entire compressed pack is an explicit latency choice, but the 2x
raster experiment makes the total-residency problem severe enough to track as
open P1-A above. In particular,
`readPackFile()` sizes a vector from the complete file length
([DeepcacheArchive.cpp](src/DeepcacheArchive.cpp#L268)). The worker now catches
allocation exceptions and reports an archive error, yet a very large valid pack
can still cause severe process memory pressure or an OS-level kill before C++
recovery. Both the archive decoded queue and UI upload handoff are now bounded;
installed recoverable cards release RGBA rather than retaining it for context
recovery. Cache statistics expose the remaining QOI, RGBA, pending-upload, and
estimated GPU components. A very large valid compressed pack itself still has
no admission ceiling.

### P2-8: Framebuffer capture uses synchronous GPU readback on the UI thread

`captureFramebuffer()` calls `glGetTexImage()` directly
([Deepcache.cpp](src/Deepcache.cpp#L1065)). The cooperative budget is checked
only between cards, so one large/stalled readback can exceed the selected frame
budget substantially. Measure this on representative GPUs. If it is material,
use a staged PBO/fence pipeline or reduce capture dimensions; either requires
careful context-recreation handling.

### P2-9: Rebuild could not recover a fatal archive worker — resolved

`Rebuild cache` now requests a destructive reset from the archive worker that
owns the inter-process write lease. The reset runs off the UI thread, removes
the pack, index, backups, compaction marker, and staging files, clears archive
progress, and leaves the worker ready to persist the newly rendered previews.
A read-only lease contender cannot reset the shared database. Fatal load,
append, and compaction errors keep the owner thread alive waiting for either
shutdown or this explicit recovery request.

The QOI encoder also rejects non-positive dimensions, overflowing pixel counts,
and undersized RGBA input before indexing the source buffer.

### P2-10: Browser drawing could bypass canonical warming — resolved

`DeepcacheModelBox::draw()` treated any lazily created Rack framebuffer as
`FRAMEBUFFER_READY`. Depending on whether the browser drew a card before the
hidden warm host reached it, the warm pass could skip its canonical render and
persist dimensions derived from the current browser/UI transform. This explains
the observed sequence-dependent top-left cropping at 200% and 300% UI scale.

Only successfully uploaded raster widgets now become framebuffer-ready. Live
module trees remain resident until the warm host deletes any opportunistic Rack
framebuffer, applies the pixel-ratio-compensated canonical transform, captures
the result, and replaces the tree with its raster.

### P2-11: Rebuild could admit stale startup decodes and lose reset state — resolved

A Rebuild requested during archive startup cleared the current browser, then
immediately allowed archive results again. The archive thread could finish and
publish old decoded entries after that clear; the next planner pass saw those
cards as complete and skipped them. Separately, publishing `LOADING` after
releasing the reset mutex allowed a fast worker completion to be overwritten by
a late `LOADING`, potentially stranding the displayed state.

Rebuild now rejects decoded results until the archive handoff is empty and the
new planner generation actually begins. Pack reads and decoded-queue waits
observe reset requests, reset publication is ordered under the queue mutex, and
an owner can accept a reset while initial lease acquisition is still underway.
A 20-entry regression test fills the bounded startup handoff, resets mid-decode,
and verifies that no pre-reset pixels escape after `EMPTY` is published.

### P2-12: A surviving read-only archive instance never promotes to owner

When another Rack process/context holds the write lease, the contender loads one
validated snapshot and then waits only for shutdown. If the original owner exits,
the survivor remains `READ_ONLY`/memory-only for the rest of its lifetime and
does not persist previews it rendered while secondary. A throttled lease retry is
straightforward, but reconciling already-rendered memory-only rasters into a newly
owned archive needs an explicit manager/worker handoff and should not be improvised.

### P2-13: Generic browser-successor conflict handling is intentionally terminal

If an unknown plugin replaces `Scene::browser` after Deepcache is active,
Deepcache safely detects pointer loss and stops its workers. If that successor is
later removed, the retired overlay can heal the raw backup chain, but the stopped
manager does not reactivate. This avoids touching an unknown owner's browser or
restarting non-reusable workers, at the cost of requiring Deepcache to be removed
and re-added for recovery. Known MB-first cases still use automatic standby
promotion.

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

### P3-4: Database-directory creation failure is reported as `EMPTY` — resolved

Directory creation failure now publishes database error code 7 and `ERROR`, while
the browser continues using the existing memory-only rendering path.

### P3-5: Database errors and scale-churn oversampling were inconsistent — resolved

The ERR aperture now includes an explicit archive `ERROR` state without treating
normal `LOADING` as failure. Before each canonical render, framebuffer oversample
is also refreshed from the current Rack pixel ratio, so a card constructed before
a UI-scale change cannot retain an unnecessarily expensive low-DPI temporary
oversample policy.

### P3-6: Planner thread creation could escape module activation — resolved

The archive worker already converted `std::thread` construction failure into a
published error, while the planner constructed its thread directly in the member
initializer and could throw through `PreviewCacheManager` activation. The planner
now contains startup failure and publishes the submitted generation as failed, so
the existing cache state machine reaches `ERROR` without an exception escaping.

## Fixes made during this review

1. **Bounded persisted startup flow.** The decoded handoff is limited to 16
   entries/64 MB with worker backpressure, and UI installation is limited by the
   configured frame budget. Automatic startup waits until both decoded and upload
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
13. **Exclusive archive write lease.** One worker holds `archive-v1.lock` across
    recovery, append, index publication, and compaction. Contenders validate and
    decode a read-only committed snapshot without risking archive corruption.
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
18. **Generation-zero startup sentinel fix.** While database loading delays the
    first planner submission, generation zero is now explicitly neither ready
    nor failed. This prevents a false cache `ERROR` state—and ERR LED—during a
    normal database load.
19. **Linear-time filter promotion.** Browser refresh submits one set of visible
    indices; the planner partitions the queue once and reports pending work in
    constant time.
20. **Development-build and theme invalidation.** Binary/manifest mtimes join
    sizes in raster schema 2. Runtime panel-theme changes are detected while the
    browser is closed, refresh fingerprints, and persist the replacement rasters.
21. **Bounded persistent-upload retry.** Transient NanoVG image creation failures
    retry across later UI frames without creating an unbounded queue.
22. **Safe UI and ownership lifetimes.** Context-menu callbacks use an expiring
    manager token, stopped managers clear their browser pointer, and inactive
    Deepcache widgets can claim a newly free scene slot once per second.
23. **Archive hardening.** Index entry/string allocations are realistically
    bounded, compaction retains its already-copied byte vector instead of
    rereading the committed pack, and database-directory failure publishes a
    dedicated error while leaving memory-only warming available. The normal
    compaction threshold is evaluated at startup as well as after writes, including
    safe truncation when the pack contains no live entries.
24. **Worker-owned persistent reset.** Rebuild can recover a fatal owner session
    without UI-thread file deletion, while read-only contenders remain unable to
    purge shared data.
25. **Selectable canonical raster identity.** Persisted previews compensate for
    Rack's framebuffer pixel ratio at 100%, 150%, 200%, and 300% UI scale. The
    default 100% or optional 200% resolution is part of the artifact fingerprint
    and persists as a shared plugin setting rather than patch state. The stable
    storage key is retained so changed-resolution payloads replace old records and
    compaction can reclaim them.
26. **Canonical warm-state enforcement.** A normal browser draw no longer marks
    a live module framebuffer as cache-ready or bypasses canonical rendering.
27. **Reset/load race closure.** Reset interrupts pack/decode startup, orders
    visible state publication with the reset flag, and keeps the UI-side stale
    result gate closed until the replacement planner generation begins.
28. **Detached-menu lifetime guards.** Brand, tag, sort, and retry actions use
    weak owner tokens instead of dereferencing deleted browser objects.
29. **Post-churn status/performance cleanup.** Archive failures light ERR, and
    framebuffer oversampling is refreshed after Rack UI-scale changes.
30. **Planner startup containment.** Failure to allocate the planner thread is
    reported through the normal failed-generation state rather than escaping
    Deepcache activation.
31. **Compressed/GPU steady-state lifecycle.** Recoverable cards release decoded
    RGBA after upload, committed writes acknowledge when release is safe, startup
    and UI upload handoffs are bounded, and graphics-context recreation re-decodes
    from hot QOI by generation. Read-only workers keep newly rendered misses as
    volatile QOI instead of retaining their full bitmap indefinitely.

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
| Read, append, index, or compaction I/O failure | Archive worker enters `ERROR`, stops accepting persistence work, retains the write lease, and waits for explicit Rebuild/reset or joined teardown. In-memory browser operation can continue. |
| Unexpected archive-worker exception | Converted to database error code 5; the owner waits for reset or shutdown rather than terminating the process. |

The database remains a disposable performance cache. Recovery intentionally
prefers dropping coverage and rerendering over attempting to salvage uncertain
pixels.

## Validation performed

- Complete `make test-fast`: passed.
- `build/tests/deepcache_archive_spec`: passed, including append/replacement,
  restart, stale fingerprint rejection, simultaneous-worker read-only snapshot
  loading, commit acknowledgement, disk-free QOI re-decode by graphics generation,
  read-only volatile-QOI encode/restore without pack mutation, raced-write release,
  cancellation, compaction, corrupt
  pack, corrupt index, repair, a pack-only interrupted-compaction backup, an
  oversized index-key declaration, live theme-fingerprint replacement across a
  restart, fatal-I/O reset recovery, worker-owned database reset, and reset while
  the bounded startup decode handoff is full. The race-focused archive suite also
  passed ten consecutive runs. Test pixels include varying alpha to exercise the
  cancelable QOI encoder's RGBA path.
- `build/tests/deepcache_planner_spec`: 12/12 passed, including stable bulk
  promotion and both 100% and 200% canonical render transforms at 100%, 150%,
  200%, and 300% Rack UI scale.
- `build/src/Deepcache.cpp.o`: compiled cleanly with the Rack SDK headers.
- Archive test under AddressSanitizer and UndefinedBehaviorSanitizer: passed
  with leak detection disabled because LeakSanitizer is unavailable under this
  environment's ptrace wrapper.
- ThreadSanitizer binary compiled, but the runtime could not start in this WSL
  environment (`unexpected memory mapping`), so no TSan result is claimed.
- Final full plugin linking is intentionally not treated as authoritative in
  WSL; it remains a Windows/MSYS2 validation item.

## Recommended remaining implementation order

1. Measure the new Windows steady-state telemetry after decoded-RGBA release. If
   GPU residency remains excessive, add a visible/recent GPU LRU while retaining
   the hot QOI pack for disk-free restoration.
2. Add a persistent denylist for known unsafe third-party models; document that
   generic hard-failure isolation requires an out-of-process renderer or host support.
3. Extend fingerprinting to resource-only changes without imposing a large
   physical-disk metadata scan at startup.
4. Add user-visible recoverable-corruption diagnostics.
5. Measure synchronous 2x framebuffer readback stalls on representative Windows
   GPUs before deciding whether a PBO/fence pipeline is justified.
6. Consider lease promotion for a surviving read-only DAW/standalone instance
   after the original archive owner exits.
