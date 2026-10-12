# Deepcache MVP implementation notes

## Persistent raster database

Deepcache stores lossless QOI payloads in
`<Rack user>/Leviathan/Deepcache/previews-v1.pack` with a versioned binary index.
The binary index is intentionally small and fast to validate; it records each
cache key, artifact fingerprint, pack offset and length, image dimensions, and
payload checksum.

Startup validates the bounded index before accessing the pack. Index files are
limited to 64 MB; each referenced offset, encoded length, decoded size, and image
dimension is validated against the physical pack before use. The archive worker
reads and decodes one indexed QOI span at a time, so multi-gigabyte databases do
not require equivalent startup RAM. NanoVG image creation remains on Rack's draw
thread. Graphics-context recreation requests bounded per-entry re-decodes from
disk rather than retaining the complete compressed pack in memory.

Normal updates append instead of rewriting the pack. Compaction is requested
automatically only after at least 64 MB is reclaimable and dead space reaches
25%, or manually from the module menu. A transaction marker and old pack/index
backups make an interrupted compaction recoverable on the next launch.

Plugin render fingerprints include the plugin identity, version, path, Rack's
high-resolution plugin modification timestamp, relevant artifact sizes, panel
theme, and the Deepcache raster schema. Development builds therefore invalidate
without requiring a semantic version change.

The archive worker checks cancellation between entries and before commits. The
module destructor signals cancellation and joins it before browser teardown;
temporary data is discarded or recovered on the next launch.

Deepcache targets the Rack source available beside this repository at commit
`061ccf63`. It intentionally uses only public SDK declarations from plugin code,
although several required behaviors depend on conventions observed in Rack's
private browser implementation.

## Rack behavior dependencies

### Browser replacement

`app::Scene::browser` is a public `widget::Widget*`. Normal Rack browser actions
show and hide whichever widget this pointer names. Deepcache therefore detaches
the current browser, retains its exact pointer without casting it, installs a
`ui::MenuOverlay`, and restores the retained widget only while Deepcache still
owns `APP->scene->browser`.

If a later plugin replaces Deepcache and Deepcache is removed first, that plugin
may retain a raw pointer to the Deepcache overlay. Deleting it would corrupt the
replacement chain. Deepcache instead retires the overlay to a small pointer-safe
shell. If a successor later restores that shell, its next UI step heals the chain
to the browser that preceded Deepcache. The detached shell is deliberately
retained because Rack exposes no ownership-chain notification or safe way to
invalidate another plugin's backup pointer.

### Preview construction

Rack's stock browser constructs a preview with
`model->createModuleWidget(nullptr)`, attaches it below a `ZoomWidget` and
`FramebufferWidget`, steps the `ModuleWidget` once, and retains the hierarchy in
the model card. Deepcache follows this path on Rack's UI thread. A null return or
C++ exception fails only that entry and produces a retryable placeholder.

`DeepcacheWidget(nullptr)` creates panel children only. It does not register the
singleton, install a browser, allocate a cache manager, or start a worker.

### Module insertion

Deepcache follows the stock browser insertion sequence:

1. Update `settings::ModuleInfo` usage data.
2. Create the engine module and register it with the engine.
3. Create its attached `ModuleWidget`.
4. Deselect the rack, snapshot old positions, and call `addModuleAtMouse()`.
5. Add Rack's module-drag action to a `history::ComplexAction`.
6. Load the module template.
7. Serialize a `history::ModuleAdd` action and push the complex action.
8. Hide the active browser.

Rack's stock browser also writes `ModuleWidget::dragOffset()` and
`dragEnabled()`. Those accessors are marked private for external plugins, so
Deepcache does not call them.

### Filter and sort parity

Deepcache retains one model card per installed module and filters/reorders those
cards in place. The header provides stock-style search, brand, multi-tag,
favorites, reset, sort, zoom, count, and Library controls. Search covers plugin
and module names/slugs, descriptions, and tag aliases; brand, every selected tag,
favorites, enablement, visibility, and the Rack module whitelist are combined as
intersections. The brand popup retains Rack's normal single-column layout when
it fits and automatically flows into screen-bounded columns when it would exceed
the available window height.

Immutable normalized search metadata is built once when the browser is created.
Refreshes update only mutable Rack settings and usage fields, so typing in the
search box does not reconstruct model widgets or repeatedly lowercase all model
metadata. Sort selection is shared with Rack through `settings::browserSort` and
supports last updated, last used, most used, brand, module name, and random.

### Framebuffer lifetime

Before a browser or preview hierarchy is detached and destroyed, Deepcache walks
its descendants on the UI thread, dirties each `FramebufferWidget`, and calls
`deleteFramebuffer()` while `APP->window` is live. This is performed for the
previous browser at installation, Deepcache previews during clear/removal, and
the Deepcache browser during conflict retirement. It mirrors the relevant Rack
`FramebufferWidget::onContextDestroy()` contract and avoids destructor-time GL
deletion after the Window has gone away.

The framebuffer warm pass always runs from a transparent scene-level host
during Rack's normal draw phase. After the resident-widget phase completes, it
calls each outer preview `FramebufferWidget::render()` without compositing the
result, validates the resulting NanoVG image, and promotes the card to
`FRAMEBUFFER_READY`. Work is limited to four previews per frame under the same
cooperative UI budget. Transient failures are retried twice; a permanent failure
is isolated to that preview and remains available for Rack's normal lazy draw.

Framebuffer warming is a required part of Deepcache and has no disable option,
because it produces the raster assets used by the persistent database.
Graphics-context destruction
downgrades framebuffer-ready cards to resident, clears stale completion, and
schedules a raster-only rewarm for the next valid draw context.

The panel exposes the two phases separately. A bordered violet bar reports
completed plugin builds as a percentage, with the completed-plugin count beneath
it. A second bordered cyan bar reports plugins whose complete model set has
finished framebuffer warming. Hidden models remain part of their owning plugin's
work without appearing as surprising extra units in the displayed count.

## Thread boundary

The planner worker receives copied `ModelDescriptor` values. It performs only
validation, deduplication, cache-key construction, ordering, cancellation, and
queue publication. It never accesses Rack's plugin registry, widgets, scene,
NanoVG, OpenGL, or the audio engine.

The active `DeepcacheWidget::step()` consumes at most four requests per frame
under the selected cooperative time budget. Preview construction, failure
bookkeeping, cache clearing, and all framebuffer work therefore remain on the UI
thread. `DeepcacheModule::process()` only detects the panel-button edge, publishes
an atomic serial, and updates lights from atomic status.

## Validation boundary

Automated coverage lives in `tests/deepcache_planner_spec.cpp` and checks state
transitions, deterministic priority, scopes, invalid/duplicate removal, stable
keys, generation-scoped promotion, pause/resume, stale-job replacement,
cancellation, worker join, memory-backend lifecycle, combined browser filters,
searchable tag aliases, and all stock browser sort modes.

The following remain manual Rack integration checks in the authoritative
Windows/MSYS2 environment:

- stock browser replacement and restoration;
- MB-before-Deepcache and MB-after-Deepcache removal order;
- browser open/search/brand/tag/favorite/reset/sort/zoom/add-module behavior;
- removal, clear, and Rack shutdown while warming;
- duplicate Deepcache modules and a browser preview of Deepcache;
- third-party null, exception, and slow preview behavior;
- audio continuity at each UI budget;
- resident-preview reuse and measured cold/warm browser latency.
