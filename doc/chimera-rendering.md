# Chimera rendering

The UI reads published metadata and immutable worker-built waveform summaries. It never scans live audio pages. The display uses a waveform framebuffer, an ordinary-marker framebuffer, live playhead/record/selection/hover geometry, and a footer framebuffer sized only to the text strip.

Selection changes update the live highlighted stems and any changed footer text. They do not rebuild ordinary markers. Recording-length growth rebuilds frame-scaled marker geometry but retains the footer until its displayed content changes. Opaque emphasized stems cover the ordinary stems beneath them. Waveform and marker caches keep Rack's subpixel invalidation behavior to preserve alignment while panning.

The marker mailbox can skip copying unchanged tables for a continuing consumer. A new or reopened overlay still receives the retained table, even without a new publication.

## Image ownership

`visual_assets::loadRasterMipmapHandle()` retains separate image maps for each concurrent NanoVG draw context. Each map records the owning Window image's context so a main-window lifecycle event also invalidates framebuffer-context entries. Ordinary main/framebuffer switching retains both maps. A creation event invalidates reused context addresses, including when a host omitted the preceding destruction event.

Caches are local to the rendering thread. Context events invalidate bookkeeping; the destroyed NanoVG contexts own resource cleanup. Rejected stale numeric handles are not deleted because a recreated context may have reassigned them. Newly created images with invalid dimensions are reclaimed immediately in their known-current context.

Chimera reels and VUs borrow shared textures. Removing and recreating modules reuses a bounded set of image assets within a live context; widget destructors perform no graphics cleanup. Reel and VU asset paths are resolved once, and reels skip drawing when their disks, tape, and intake housings are outside the clip box.

## UI/control handoff

The dispatcher started by module `onAdd()` owns routine service pumping and checkpoint maintenance. Widget `step()` observes published state and does not call `serviceStep()`.

When the host patch path, autosave path, module ID, or widget owner changes, the widget attempts a nonblocking publication of the recovery identity. If control is busy, it retries on a later frame. Explicit save/edit operations retain their existing synchronous semantics; saving also refreshes the recovery identity.

Optional waveform snapshots require a recent actual display draw, with a 500 ms visibility grace period and the existing one-second attempt throttle. Widget stepping alone does not advertise visibility. Framebuffer previews do not advertise it either. Required post-record and periodic recovery remain independent of visibility.

## Asynchronous incremental waveform summaries

Display summaries use one plugin-wide `WaveformService` thread, separate from
the two I/O workers. Loads, edits and recovery commits no longer scan waveforms
on their I/O jobs. After adoption, a visible display requests its own summary;
hidden modules do not build display data. Rendering uses the last completed
immutable summary, or an empty waveform after a different Reel is adopted,
until a complete replacement is published.

The queue admits at most 64 jobs, with one job per module cache. Each turn
processes at most 32 pages (8,192 source frames), then rejoins the tail. New
arrivals join the tail too. This prevents an admitted busy module from
monopolizing the worker; it is not a wall-clock deadline guarantee. Newer
revisions coalesce behind the active job rather than cancelling it on every
recorded sample. The next eligible refresh captures the newest state. A full
queue drops optional attempts and releases their cut; retries use the existing
one-second throttle.

Each Reel maintains a core-owned version per logical page. Capture freezes the
version before subsequent writes, alongside the immutable page reference.
The worker caches each page's extrema, valid length and version. Unchanged
pages reuse their extrema. Pages crossing a display-bin boundary are reread
for exact placement (at most 511 pages), so all 512 bins, peak, stereo flag and
nonfinite handling match the original full scan even as recording grows the
Reel. Unique Reel identity invalidates the cache after load/edit or address
reuse. Audio writes add one non-atomic page-version increment; scanning,
allocation and reduction stay off audio and UI callbacks.

Snapshots still use the existing leased ordinary cut. Save, edit, pending
adoption, required post-record recovery, and loss of visibility cancel optional
work. The dispatcher also cancels a display attempt older than 250 ms; this is
a best-effort lease-age limit, not a realtime bound. Cancellation is checked
between turns. Core reclaim starts only after worker acknowledgment. Completed
cache pages survive cancellation so later refreshes can reuse that progress.
Teardown transfers the source Reel to the cancelled job until its last reader
is gone. Existing recording scratch limits still apply.

Version tables add 32 bytes per logical page to a production Reel with three
snapshot slots (1,044,000 bytes at full capacity), charged to the existing
store budget. A module's off-audio peak cache is separately bounded by the Reel
page count (about 1 MiB at full capacity on the native build).

`make test-chimera-waveform` checks exact equivalence, append/overwrite/COW,
marker changes, restored revisions, Reel replacement, invalid samples, fair
turns, duplicate admission, capacity, cancellation, and completion while both
I/O workers are blocked. It prints cold/incremental timings for 12 distinct
ten-second Reels. `test-chimera-render` also checks module coalescing,
save-driven cancellation, acknowledged release and teardown retention. Offline
worker timings do not establish live audio-device deadlines or sustained
host-level many-instance performance.

Validation on 2026-09-29: native `plugin.dll`, waveform, phase-2 snapshot/job/
service, module, render, dispatcher, patch, checkpoint, playback-reader and
full-save checks passed. The waveform-service fixture passed WSL GCC TSAN
with `setarch x86_64 -R`, and ASan/UBSan. A native 12-Reel run measured 51.97 ms
cold versus 27.21 ms after one second of overdubbing per Reel; source reads
fell from 5,760,000 to 1,987,584 (65.5% fewer). Incremental turn p99 was 81.50 us
and maximum 128.60 us in that run. These compare cold and warm cache workloads,
not an end-to-end host speedup against the previous implementation.

The broad native `test-fast` run still failed at the unrelated Sibyl P5
companion and P6 combined-source fixtures. Logs are in
`test-results/chimera-waveform-{validation,final,module,plugin,tsan,asan,test-fast}.log`.

## Performance telemetry

Dragon King debug mode retains `Process`, `Step`, and `Draw` as the first three metrics and adds `DrawLayer` fourth.

- `Process` measures module audio processing.
- `Step` measures the full module widget `step()` call, including any child GL surface rendering and telemetry bookkeeping.
- `Draw` sums only normal module widget `draw()` calls.
- `DrawLayer` separately sums every module `drawLayer()` call per UI cycle (including shadow/light passes). Completed cycles provide min, max, and arithmetic mean; the terminal's A key selects Range or Average. Samples are finalized at the next step, so reporting never splits a cycle.

Additional fields report display-cache CPU time, live custom UI CPU time, light-pass CPU time, GL surface work in step, and cache-render count per reporting interval. Component timings are subsets, not extra work to add to the macro metrics. GL-in-step work is included only in Step; it is never added to Draw or DrawLayer. These CPU timings do not measure asynchronous GPU completion.

## Regression checks

Run `make test-chimera-render` with the installed Rack runtime configured. The tests cover alternating live image contexts, independent windows, context-address reuse, image creation failures, marker/footer invalidation, cropped footer dimensions, retained marker publication, nonblocking UI identity updates, visibility gating, required recovery, reel bounds, and full-frame timing aggregation.

`test-fast` includes the shared raster regression. `test-chimera-repairs` includes the rendering regression alongside the module, dispatcher, persistence, and marker tests.

A real-host visual smoke test should include rapid Organize changes, recording growth, zoom/pan, scrolling the display out of view, and DAW editor close/reopen. No GPU speedup percentage is implied by these structural and native regression checks.
