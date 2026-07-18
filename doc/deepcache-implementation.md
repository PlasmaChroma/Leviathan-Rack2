# Deepcache MVP implementation notes

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

The experimental framebuffer warm pass runs from a transparent scene-level host
during Rack's normal draw phase. After the resident-widget phase completes, it
calls each outer preview `FramebufferWidget::render()` without compositing the
result, validates the resulting NanoVG image, and promotes the card to
`FRAMEBUFFER_READY`. Work is limited to four previews per frame under the same
cooperative UI budget. Transient failures are retried twice; a permanent failure
is isolated to that preview and remains available for Rack's normal lazy draw.

The setting remains disabled by default because warming every installed preview
can use substantial GPU memory. Enabling it applies immediately to an existing
resident cache as well as future rebuilds. Graphics-context destruction
downgrades framebuffer-ready cards to resident, clears stale completion, and
schedules a raster-only rewarm for the next valid draw context.

The panel exposes the two phases separately. A bordered cyan bar reports module
widget construction as a percentage, with the exact constructed/total module
count beneath it. A second bordered violet bar reports framebuffer attempts and
shows `OFF` while the experimental pass is disabled. READY is published only
after both bars have completed when framebuffer warming is enabled.

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
