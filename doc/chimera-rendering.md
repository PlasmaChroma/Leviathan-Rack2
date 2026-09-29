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

## Performance telemetry

Dragon King debug mode retains the first three metrics: `Process`, `Step`, and `Draw`.

- `Process` measures module audio processing.
- `Step` measures UI stepping, including any child GL surface rendering.
- `Draw` sums the module's normal, shadow, and light passes plus GL surface rendering performed during stepping. The sample is finalized at the next step, after all draw passes have contributed.

Additional fields report display-cache CPU time, live custom UI CPU time, light-pass CPU time, GL surface work in step, and cache-render count per reporting interval. Component timings are subsets, not extra work to add to the macro metrics. GL-in-step work belongs to both Step and Draw; do not sum those two columns to derive total thread utilization. These CPU timings do not measure asynchronous GPU completion.

## Regression checks

Run `make test-chimera-render` with the installed Rack runtime configured. The tests cover alternating live image contexts, independent windows, context-address reuse, image creation failures, marker/footer invalidation, cropped footer dimensions, retained marker publication, nonblocking UI identity updates, visibility gating, required recovery, reel bounds, and full-frame timing aggregation.

`test-fast` includes the shared raster regression. `test-chimera-repairs` includes the rendering regression alongside the module, dispatcher, persistence, and marker tests.

A real-host visual smoke test should include rapid Organize changes, recording growth, zoom/pan, scrolling the display out of view, and DAW editor close/reopen. No GPU speedup percentage is implied by these structural and native regression checks.
