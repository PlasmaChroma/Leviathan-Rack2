# Deepcache — VCV Rack Module Browser Preview Cache

## Codex Implementation Specification

### Project

Leviathan VCV Rack plugin

### Module

* **Name:** Deepcache
* **Slug:** `Deepcache`
* **Category:** Utility
* **Initial width:** 4 HP
* **Description:** Preloads module-browser previews for faster browsing.
* **Storage:** Persistent QOI raster previews plus hot process/GPU memory

---

# 1. Objective

Create a new Leviathan utility module named **Deepcache** that replaces VCV Rack’s standard module browser with a compatible custom browser capable of pre-constructing and retaining module preview widgets.

The MVP must prove that:

1. A Rack plugin can safely replace `APP->scene->browser`.
2. Deepcache can enumerate all installed Rack models.
3. Deepcache can incrementally construct browser preview widgets before the user scrolls to them.
4. Completed preview rasters remain recoverable from hot QOI data and are reused by the Deepcache browser.
5. Cache generation does not block the audio engine or cause large UI stalls.
6. Removing Deepcache restores the browser that was active before Deepcache.
7. Rack can close without OpenGL or framebuffer teardown crashes.

Deepcache persists completed framebuffer previews beneath
`<Rack user>/Leviathan/Deepcache`. The complete compressed pack is read
sequentially into RAM at startup, decoded off the UI thread, and uploaded to
NanoVG before cached cards are considered framebuffer-ready. Decoded RGBA is
released after a successful upload once a committed QOI recovery source exists.
Only a bounded handoff/upload window retains full bitmap data.

Persisted previews default to a canonical 1x render scale independent of browser
zoom. A plugin-level **Cache resolution** option can select 200% for sharper
scaling when cards are enlarged, at roughly four times the uncompressed pixels.
The choice is shared by every Deepcache instance and persists in
`<Rack user>/Leviathan/Deepcache/settings.json`; it is deliberately not patch
state because changing it invalidates and rebuilds the shared preview database.

Only one archive worker may write at a time. Other Deepcache instances load a
validated read-only snapshot of the committed pack and index, then render only
missing or stale previews into their own memory/GPU context.

The database is append-only during normal updates. Changed previews append new
QOI payloads and atomically publish a compact binary index; superseded payloads
become reclaimable dead space. Compaction copies live payloads into temporary
files and commits them as a recoverable pair. Shutdown cooperatively cancels
database work and joins the worker instead of waiting for all remaining work.

---

# 2. Reference Implementations

Codex should inspect the following source files before implementing Deepcache:

## Stoermelder PackOne

* `src/modules/trial-and-error/Mb.cpp`
* `src/modules/trial-and-error/Mb_v2.cpp`

Use MB as a reference for:

* replacing `APP->scene->browser`;
* retaining and restoring the previous browser;
* implementing a singleton browser-modifying module;
* constructing custom browser model cards;
* creating preview `ModuleWidget` instances with a null engine module;
* cleaning framebuffers before browser teardown;
* adding modules selected from the replacement browser.

Do not blindly copy MB source. Implement the minimum required functionality independently unless the Leviathan repository’s licensing explicitly permits direct reuse.

## VCV Rack

* `src/app/Browser.cpp`
* `src/widget/FramebufferWidget.cpp`
* `include/app/Scene.hpp`

Use Rack’s browser as the behavioral reference for:

* preview widget construction;
* browser search;
* browser visibility;
* module selection;
* adding modules to the rack;
* favorites and hidden-module behavior;
* preview sizing and oversampling;
* framebuffer lifecycle.

---

# 3. Critical Threading Constraint

Deepcache should use a dedicated background worker, but the worker must not directly construct Rack widgets or render previews.

The following operations must occur on Rack’s UI thread:

* reading or modifying widget trees;
* accessing `APP->scene`;
* replacing `APP->scene->browser`;
* calling `plugin::Model::createModuleWidget()`;
* calling `ModuleWidget::step()`;
* creating or deleting `FramebufferWidget` objects;
* accessing NanoVG;
* accessing OpenGL;
* drawing or dirtying GPU framebuffers;
* adding a selected module to the rack.

The background worker may perform:

* cache planning;
* task ordering;
* deduplication;
* cache-key generation;
* immutable metadata processing;
* cancellation and pause coordination;
* future database reads and writes;
* future image compression or decompression, provided no Rack graphics API is touched.

The audio engine thread must never perform cache construction, widget operations, file I/O, locking, or framebuffer work.

---

# 4. High-Level Architecture

```text
DeepcacheModule
    |
    | atomic status only
    v
DeepcacheWidget
    |
    +-- DeepcacheBrowserOverlay
    |       |
    |       +-- DeepcacheBrowser
    |       |       |
    |       |       +-- one DeepcacheModelBox per installed model
    |       |
    |       +-- previous browser pointer
    |
    +-- PreviewCacheManager
            |
            +-- PreviewPlannerWorker
            |       background std::thread
            |
            +-- UI build queue
            |       consumed incrementally from Widget::step()
            |
            +-- optional WarmRenderHost
                    UI-thread framebuffer warming
```

The browser’s model cards and the cache must share the same preview objects. Deepcache must not construct one set of previews for caching and a second set for browser display.

---

# 5. Required Components

## 5.1 `DeepcacheModule`

The engine module should contain no DSP functionality.

Responsibilities:

* store user configuration in patch JSON;
* expose lightweight atomic cache status to panel widgets;
* update status lights without locks or allocation;
* remain safe when instantiated without a `ModuleWidget`.

The module constructor must not:

* start a thread;
* replace the Rack browser;
* access `APP->scene`;
* construct previews;
* allocate a browser overlay.

This is essential because Rack may create a Deepcache preview using:

```cpp
modelDeepcache->createModuleWidget(nullptr);
```

A browser preview of Deepcache must not recursively activate Deepcache.

Suggested state:

```cpp
struct DeepcacheModule : rack::engine::Module {
    enum ParamIds {
        NUM_PARAMS
    };

    enum InputIds {
        NUM_INPUTS
    };

    enum OutputIds {
        NUM_OUTPUTS
    };

    enum LightIds {
        PLANNING_LIGHT,
        WARMING_LIGHT,
        READY_LIGHT,
        ERROR_LIGHT,
        NUM_LIGHTS
    };

    std::atomic<int> cacheState;
    std::atomic<int> completedCount;
    std::atomic<int> totalCount;
    std::atomic<int> failedCount;
};
```

Avoid reading the cache manager directly from `process()`.

---

## 5.2 `DeepcacheWidget`

`DeepcacheWidget` owns all UI-level services.

When constructed with `module == nullptr`, it must only create the static panel preview.

When constructed with a real module:

1. Attempt to become the active Deepcache owner for the current Rack scene.
2. If successful, create the cache manager.
3. Install the Deepcache browser overlay.
4. Start the planner worker.
5. Optionally begin automatic cache warming.
6. Publish status to the module through atomics.

Only one Deepcache instance may modify a particular scene's browser at a time.
Rack Pro DAW instances have independent scenes, so each stem may have its own
active Deepcache even though the plugin DLL and archive path are process-global.

Additional Deepcache modules in the same scene may exist, but they must remain
passive and clearly indicate that another instance is active.

When the active instance is removed:

1. request worker shutdown;
2. join the worker thread;
3. stop UI cache processing;
4. delete Deepcache framebuffers while the graphics context is valid;
5. restore the previous browser when Deepcache still owns its recorded scene's browser;
6. unregister that scene's owner.

Never detach cache objects while the worker can still publish tasks.

---

## 5.3 `DeepcacheBrowserOverlay`

Suggested fields:

```cpp
struct DeepcacheBrowserOverlay : rack::widget::OpaqueWidget {
    rack::widget::Widget* previousBrowser = nullptr;
    DeepcacheBrowser* browser = nullptr;
    PreviewCacheManager* cacheManager = nullptr;
    bool installed = false;
};
```

Installation procedure:

```cpp
previousBrowser = APP->scene->browser;

if (previousBrowser) {
    previousBrowser->hide();
    APP->scene->removeChild(previousBrowser);
    releaseDetachedBrowserFramebuffers(previousBrowser);
}

browser = new DeepcacheBrowser(cacheManager);
addChild(browser);

APP->scene->browser = this;
APP->scene->addChild(this);
installed = true;
```

Restoration procedure:

```cpp
if (installed && APP->scene->browser == this) {
    APP->scene->browser = previousBrowser;

    if (previousBrowser) {
        APP->scene->addChild(previousBrowser);
        previousBrowser->hide();
    }

    APP->scene->removeChild(this);
}
```

If another plugin replaces the browser after Deepcache, Deepcache must not overwrite that plugin’s browser during destruction.

Specifically:

```cpp
if (APP->scene->browser != this) {
    // Clean up Deepcache-owned resources only.
    // Do not change APP->scene->browser.
}
```

---

## 5.4 `DeepcacheBrowser`

Implement only the VCV Rack 2 browser style. Do not reproduce MB’s legacy browser modes.

Minimum browser behavior:

* open from normal Rack browser actions;
* close with Escape;
* search by plugin brand, plugin name, module name, and module slug;
* show all installed models;
* support Rack favorites;
* support Rack-hidden modules;
* add a selected module to the rack;
* load the module template preset;
* push an appropriate history action;
* preserve standard browser zoom behavior;
* maintain one persistent `DeepcacheModelBox` per model;
* hide and show existing model boxes when filtering rather than rebuilding them.

The browser must remain usable while caching is underway.

If the user encounters a model whose preview is not yet resident, that model must be immediately promoted ahead of background tasks.

---

## 5.5 `DeepcacheModelBox`

Each installed Rack model should have one long-lived model card.

Suggested fields:

```cpp
struct DeepcacheModelBox : rack::widget::OpaqueWidget {
    rack::plugin::Model* model = nullptr;

    rack::widget::Widget* previewRoot = nullptr;
    rack::widget::ZoomWidget* zoomWidget = nullptr;
    rack::widget::FramebufferWidget* framebuffer = nullptr;
    rack::app::ModuleWidget* moduleWidget = nullptr;
    ModuleWidgetContainer* moduleContainer = nullptr;

    PreviewEntryState state = PreviewEntryState::EMPTY;
    bool queued = false;
    bool failed = false;
    std::string failureReason;
};
```

Required method:

```cpp
bool ensurePreviewConstructed();
```

This method must run only on the UI thread.

Expected construction path:

```cpp
bool DeepcacheModelBox::ensurePreviewConstructed() {
    if (state == PreviewEntryState::RESIDENT)
        return true;

    if (state == PreviewEntryState::CONSTRUCTING)
        return false;

    state = PreviewEntryState::CONSTRUCTING;

    previewRoot = new widget::TransparentWidget;
    addChild(previewRoot);

    zoomWidget = new widget::ZoomWidget;
    previewRoot->addChild(zoomWidget);

    framebuffer = new widget::FramebufferWidget;

    if (APP->window->pixelRatio < 2.f)
        framebuffer->oversample = 2.f;

    zoomWidget->addChild(framebuffer);

    moduleContainer = new ModuleWidgetContainer;
    framebuffer->addChild(moduleContainer);

    moduleWidget = model->createModuleWidget(nullptr);

    if (!moduleWidget) {
        state = PreviewEntryState::FAILED;
        failed = true;
        return false;
    }

    moduleContainer->addChild(moduleWidget);
    moduleContainer->box.size = moduleWidget->box.size;

    moduleWidget->step();
    updateZoom();

    state = PreviewEntryState::RESIDENT;
    return true;
}
```

The exact ownership arrangement may be adjusted to match Rack widget ownership conventions.

Calling this method more than once must be harmless.

The browser’s normal draw path should call `ensurePreviewConstructed()` only as a fallback. A successfully warmed model must never be reconstructed during ordinary browsing.

---

# 6. Cache State Machine

Use explicit states:

```cpp
enum class CacheState {
    DISABLED,
    IDLE,
    PLANNING,
    WARMING,
    PAUSED,
    READY,
    CLEARING,
    ERROR,
    STOPPING
};

enum class PreviewEntryState {
    EMPTY,
    QUEUED,
    CONSTRUCTING,
    RESIDENT,
    FRAMEBUFFER_READY,
    FAILED
};
```

Expected state flow:

```text
IDLE
  -> PLANNING
  -> WARMING
  -> READY
```

Optional branches:

```text
WARMING -> PAUSED -> WARMING
WARMING -> ERROR
READY   -> CLEARING -> IDLE
ANY     -> STOPPING
```

One failed third-party module preview must not place the entire cache into `ERROR`. Record the failed entry and continue.

Use the global `ERROR` state only for failures that prevent Deepcache from operating, such as browser installation failure or worker startup failure.

---

# 7. Background Planner Worker

Implement a dedicated `std::thread` owned by `PreviewCacheManager`.

Use portable C++ facilities supported by the Rack SDK:

* `std::thread`
* `std::mutex`
* `std::condition_variable`
* `std::atomic`
* `std::deque`
* `std::vector`

Do not require `std::jthread`.

## Worker input

The UI thread must first create an immutable snapshot:

```cpp
struct ModelDescriptor {
    size_t modelIndex;
    std::string pluginSlug;
    std::string pluginVersion;
    std::string modelSlug;
    std::string brand;
    std::string displayName;
    bool favorite;
    bool hidden;
};
```

The background worker should not traverse Rack’s live plugin registry.

## Worker responsibilities

For the MVP, the worker should:

1. receive the immutable model snapshot;
2. remove invalid or duplicate entries;
3. generate stable cache keys;
4. prioritize entries;
5. publish ordered UI construction tasks;
6. respond to pause, resume, cancel, and shutdown requests;
7. update atomic planning statistics.

Suggested priority:

1. currently visible browser results;
2. favorite modules;
3. recently requested modules;
4. remaining modules sorted by brand and display name.

## Worker output

```cpp
struct PreviewBuildRequest {
    size_t modelIndex;
    uint64_t generation;
    int priority;
};
```

The worker may place requests in a mutex-protected queue.

The UI thread should drain the queue. The audio thread must never access it.

## Cache generation IDs

Every new cache run should receive a monotonically increasing generation ID.

This prevents stale worker requests from a cancelled cache run being applied after a new run begins.

```cpp
if (request.generation != activeGeneration)
    discard(request);
```

---

# 8. UI-Thread Cache Executor

The cache executor should run from a UI widget’s `step()` method.

It must process work incrementally rather than constructing every preview in one frame.

Default limits:

```text
UI time budget: 2.0 ms per frame
Minimum work:    no more than one preview if the previous preview exceeded budget
Maximum work:    4 previews per frame
```

Suggested loop:

```cpp
double start = system::getTime();

while (!queue.empty()) {
    if (processedThisFrame >= maxPreviewsPerFrame)
        break;

    if (processedThisFrame > 0) {
        double elapsedMs = (system::getTime() - start) * 1000.0;
        if (elapsedMs >= uiBudgetMs)
            break;
    }

    PreviewBuildRequest request = popRequest();

    DeepcacheModelBox* box = browser->getModelBox(request.modelIndex);

    if (!box)
        continue;

    if (box->state == PreviewEntryState::RESIDENT ||
        box->state == PreviewEntryState::FRAMEBUFFER_READY)
        continue;

    box->ensurePreviewConstructed();
    processedThisFrame++;
}
```

The budget is cooperative rather than preemptive. A single badly behaved module constructor may exceed the budget, but Deepcache must never start another preview in the same frame after an overrun.

Record:

* construction duration per model;
* maximum construction duration;
* average duration;
* total elapsed warm-up time;
* number of failures.

Log unusually slow previews in debug builds.

---

# 9. MVP Cache Definition

For the initial proof of concept, a preview is considered **resident** when:

1. its preview widget hierarchy has been created;
2. `model->createModuleWidget(nullptr)` has succeeded;
3. the resulting `ModuleWidget` has been stepped once;
4. the hierarchy remains owned by its persistent browser model box.

This removes widget construction and asset initialization from later browser interaction.

The MVP should attempt to retain generated `FramebufferWidget` contents after normal browser rendering. It must not discard a framebuffer simply because its model box scrolls out of view.

Do not implement memory-pressure eviction in the first version.

---

# 10. Required Hidden Framebuffer Warm Pass

Render framebuffers before the browser is opened so every completed preview can be reused as a persistent raster asset.

This work must still occur on the UI thread.

Create a scene-level `WarmRenderHost` capable of drawing one resident preview through a valid Rack `DrawArgs` while preventing the preview from becoming visibly composited into the scene.

The preferred experiment is:

```cpp
void WarmRenderHost::draw(const DrawArgs& args) {
    if (!activePreview)
        return;

    nvgSave(args.vg);
    nvgGlobalAlpha(args.vg, 0.f);

    drawActivePreviewWithValidClip(args);

    nvgRestore(args.vg);
}
```

Verify that:

* the framebuffer itself is generated;
* the final framebuffer image is composited with zero visible alpha;
* the preview is not culled because it is outside the viewport;
* no input events are intercepted;
* Rack’s normal scene remains visually unchanged.

If zero-alpha composition prevents framebuffer creation, test drawing behind an opaque Rack scene element.

Do not call OpenGL directly from the background worker.

This stage is preferred but must not block the initial RAM-cache proof.

---

# 11. User Controls

## Panel controls

The initial panel should contain:

* separate construction and framebuffer progress indicators;
* a two-row database status display;
* planning, warming, ready, and error LEDs.

Cache operations use explicitly named context-menu actions. Deepcache has no
state-dependent panel button because automatic startup handles normal operation and a
single multifunction control obscures whether it will pause, cancel, or rebuild.

## Status indication

Suggested colors:

* dim: idle;
* amber: planning;
* cyan: warming;
* violet: paused;
* green: ready;
* red: fatal error.

Display either:

```text
812 / 1047
```

or a percentage.

The panel must remain understandable without requiring the custom browser to be open.

---

# 12. Context Menu

Add the following context-menu options:

* Rebuild cache
* UI work budget

  * 0.5 ms
  * 1 ms
  * 2 ms
  * 4 ms
  * 8 ms
* Show cache statistics

Defaults:

```text
UI work budget:                2 ms
Framebuffer warm pass:         always enabled
Retain resident previews:      enabled
```

---

# 13. Patch Persistence

Persist only settings and user intent.

Suggested JSON:

```json
{
  "uiBudgetMs": 2.0
}
```

Do not serialize:

* raw pointers;
* queue contents;
* worker state;
* module widgets;
* framebuffer handles;
* cache completion state.

The cache should be reconstructed after Rack restarts.

---

# 14. Clearing the Cache

`clearCache()` must execute on the UI thread.

For each `DeepcacheModelBox`:

1. remove the preview root from the model box;
2. call `setDirty()` on its framebuffer;
3. call `deleteFramebuffer()` while the graphics context is valid;
4. delete the preview hierarchy;
5. reset all preview pointers to null;
6. reset entry state to `EMPTY`;
7. preserve the model card itself.

After clearing, the browser must remain usable and lazily reconstruct previews when needed.

Never clear a preview while it is being constructed.

Cancel the current generation and drain stale queue entries before clearing.

---

# 15. Browser and Graphics Lifecycle Safety

Deepcache must explicitly handle graphics teardown.

Before removing a browser tree that contains `FramebufferWidget` descendants:

```cpp
void releaseFramebuffers(widget::Widget* root) {
    if (!root)
        return;

    std::deque<widget::Widget*> queue;
    queue.push_back(root);

    while (!queue.empty()) {
        widget::Widget* widget = queue.front();
        queue.pop_front();

        if (auto* framebuffer =
                dynamic_cast<widget::FramebufferWidget*>(widget)) {
            framebuffer->setDirty();
            framebuffer->deleteFramebuffer();
        }

        for (widget::Widget* child : widget->children)
            queue.push_back(child);
    }
}
```

Perform equivalent cleanup for:

* the detached previous browser;
* Deepcache’s own browser;
* cache clearing;
* Deepcache removal;
* plugin or application shutdown paths where possible.

Do not delete a framebuffer from the worker thread.

---

# 16. Error Handling

A third-party model preview may:

* return a null widget;
* throw an exception;
* take an unusually long time;
* create a widget with unexpected dimensions;
* assume a non-null engine module;
* start its own background services.

Deepcache should:

* catch standard C++ exceptions around preview construction where supported;
* mark the individual entry as failed;
* record the model’s full name;
* continue warming remaining entries;
* render a simple failed-preview placeholder;
* allow retrying failed entries;
* never repeatedly retry the same failed model every frame.

Deepcache cannot recover from a segmentation fault inside a third-party plugin, but it should avoid increasing that risk beyond Rack’s existing browser preview behavior.

---

# 17. Browser Conflict Handling

Deepcache may coexist in a Rack installation with MB or another browser-replacement module.

Stoermelder MB is a known hard conflict because it owns the same raw browser
slot. If an `Stoermelder-P1/Mb` or legacy `Stoermelder-PackTau/Mb` module is
already present when Deepcache is added, Deepcache must enter standby before it
reads, detaches, traverses, or replaces the active browser. The panel displays
`STANDBY / MB ACTIVE`; removing and re-adding Deepcache after MB is removed
activates Deepcache normally.

Rules:

1. Store whatever browser is active when Deepcache activates.
2. Do not assume the stored browser is Rack’s stock browser.
3. Replace only the current `APP->scene->browser`.
4. Restore only when `APP->scene->browser == deepcacheOverlay`.
5. Do not cast the previous browser to a private browser implementation.
6. Do not attempt to merge cache contents with another plugin’s browser.
7. Show a warning in the context menu when browser ownership changes unexpectedly.

The replacement chain must not be corrupted when modules are removed in a different order than they were added.

---

# 18. Suggested Source Layout

Adjust names to match existing Leviathan conventions.

```text
src/
  Deepcache.cpp
  Deepcache.hpp

  deepcache/
    DeepcacheTypes.hpp
    DeepcacheBrowser.cpp
    DeepcacheBrowser.hpp
    DeepcacheModelBox.cpp
    DeepcacheModelBox.hpp
    PreviewCacheManager.cpp
    PreviewCacheManager.hpp
    PreviewPlannerWorker.cpp
    PreviewPlannerWorker.hpp
    WarmRenderHost.cpp
    WarmRenderHost.hpp

res/
  Deepcache.svg         # Combined authoring/reference asset
  Deepcache.panel.svg   # Runtime panel surface and component anchors
  Deepcache.labels.svg  # Runtime outlined label layer
```

Update:

* `plugin.hpp`
* `plugin.cpp`
* `plugin.json`
* any module registration tables;
* build files only where required by the repository layout.

---

# 19. Future Database Compatibility

Do not implement database persistence in the MVP, but create a clean abstraction boundary.

Suggested interface:

```cpp
struct PreviewCacheBackend {
    virtual ~PreviewCacheBackend() = default;

    virtual bool contains(const PreviewCacheKey& key) = 0;
    virtual void invalidate(const PreviewCacheKey& key) = 0;
    virtual void clear() = 0;
};
```

MVP implementation:

```cpp
struct MemoryPreviewCacheBackend : PreviewCacheBackend {
    // Tracks runtime cache records only.
};
```

A future database cache key should be able to incorporate:

```text
Rack major/minor version
operating system
architecture
plugin slug
plugin version
model slug
browser zoom
pixel ratio
preview width and height
Rack light/dark preference
module panel theme where discoverable
Deepcache cache format version
```

A future persistent backend may store:

* PNG preview images;
* QOI preview images;
* compressed raw RGBA;
* dimensions;
* cache key;
* creation timestamp;
* validation metadata.

Persistent raster previews should eventually allow the browser to display a stored image without constructing the underlying module widget.

Do not include SQLite, QOI, PNG encoding, or database migrations in the MVP.

---

# 20. Implementation Phases

## Phase 1 — Browser replacement spike

* create Deepcache module;
* enforce singleton behavior;
* replace `APP->scene->browser`;
* restore previous browser safely;
* display a minimal list of installed models;
* add a selected model to the rack;
* verify null-module Deepcache previews have no side effects.

## Phase 2 — Persistent model cards

* create one model card per installed model;
* implement search and filtering;
* create previews lazily;
* retain previews after scrolling;
* confirm cards are not recreated during refresh.

## Phase 3 — Background planner

* add worker thread;
* snapshot immutable model metadata;
* build prioritized task queue;
* add generation IDs;
* implement pause, resume, cancel, and shutdown.

## Phase 4 — Incremental UI warming

* consume worker tasks from UI `step()`;
* construct previews under a frame-time budget;
* expose progress;
* prioritize user-visible models;
* add failure handling and statistics.

## Phase 5 — Cache clearing and lifecycle hardening

* release preview trees;
* delete framebuffers safely;
* test browser replacement conflicts;
* test module removal;
* test Rack shutdown;
* test multiple Deepcache instances.

## Phase 6 — Hidden framebuffer warming

* attempt invisible UI-thread framebuffer rendering;
* measure GPU-memory impact;
* keep the pass always enabled as part of normal cache construction.

---

# 21. Testing Requirements

## Unit-level tests

Where practical, test:

* cache state transitions;
* generation cancellation;
* task deduplication;
* priority ordering;
* cache-key generation;
* stale task rejection;
* pause and resume;
* worker shutdown;
* model failure bookkeeping.

## Manual integration tests

### Browser ownership

1. Start Rack without Deepcache.
2. Add Deepcache.
3. Confirm Deepcache becomes the active browser.
4. Remove Deepcache.
5. Confirm the original browser is restored.
6. Repeat with MB installed before Deepcache.
7. Repeat with MB installed after Deepcache.

### Preview warming

1. Start with a cold Rack session.
2. Add Deepcache.
3. Allow automatic cache generation to begin.
4. Open and close the browser while warming.
5. Scroll rapidly through uncached and cached sections.
6. Confirm visible models receive priority.
7. Confirm progress reaches the expected model count.
8. Confirm no preview is reconstructed after reaching resident state.

### Audio safety

1. Run a CPU-intensive patch.
2. Begin Deepcache warming.
3. Monitor audio for glitches.
4. Test each UI work budget.
5. Confirm no cache operation runs inside `Module::process()`.

### Lifecycle

1. Pause and resume repeatedly.
2. Cancel during planning.
3. Cancel during warming.
4. Clear while ready.
5. Remove Deepcache while warming.
6. Close Rack while warming.
7. Reload a patch containing Deepcache.
8. Create multiple Deepcache instances.
9. Open a browser preview of Deepcache itself.

### Failure handling

1. Simulate a model returning null.
2. Simulate a thrown exception.
3. Simulate a very slow constructor.
4. Confirm remaining previews continue.
5. Confirm failed models show placeholders.

---

# 22. Performance Instrumentation

In development builds, expose:

```text
Installed models
Queued previews
Resident previews
Rendered framebuffers
Failed previews
Total warming time
Average preview construction time
Maximum preview construction time
Current UI budget
Approximate resident preview count
```

Record baseline browser behavior before Deepcache:

* first browser-open latency;
* first large scroll latency;
* frame-time spikes during initial browsing.

Compare against:

* Deepcache after RAM warm-up;
* Deepcache after framebuffer warm-up.

The MVP is successful even if framebuffer rasterization still occurs lazily, provided widget construction and resource initialization are measurably removed from browser interaction.

---

# 23. Acceptance Criteria

The implementation is complete when all of the following are true:

1. `Deepcache` appears in the Leviathan plugin and loads successfully.
2. Adding the first real Deepcache instance installs its browser.
3. A preview-only `DeepcacheWidget(nullptr)` has no global side effects.
4. Only one Deepcache instance owns the browser.
5. All installed Rack models are represented by persistent model boxes.
6. Cache planning runs on a dedicated worker thread.
7. Rack widget and graphics operations run exclusively on the UI thread.
8. Cache warming is incremental and obeys a configurable UI budget.
9. Cache progress is visible on the module panel.
10. The browser remains usable during warming.
11. User-visible uncached models are promoted ahead of background work.
12. Resident preview widgets are reused rather than reconstructed.
13. Individual preview failures do not abort the cache.
14. Clear cache releases resident preview trees and GPU framebuffers.
15. Removing Deepcache restores the previous browser when appropriate.
16. Removing Deepcache during warming does not crash or deadlock.
17. Rack shutdown does not produce framebuffer or OpenGL-context crashes.
18. Cache contents are not serialized to the patch.
19. No database file is created by the MVP.
20. Linux and Windows builds succeed without platform-specific thread code.

---

# 24. Codex Execution Directive

Implement this module in small, testable phases.

Do not begin with persistent database storage.

Do not attempt to render previews from the audio thread or worker thread.

Browser replacement, persistent recoverable previews, and the worker-planned,
UI-executed framebuffer warming queue are all required parts of Deepcache.

Document any Rack private implementation behavior that Deepcache depends upon, particularly browser replacement, framebuffer cleanup, preview construction, and module insertion.

---

# 25. Current Persistent Implementation Notes

The implemented status bars intentionally describe different domains:

* purple is progress for the all-module construction stage;
* cyan is global framebuffer/database coverage across installed plugin builds.

Deepcache no longer exposes a cache-scope setting. Visible and favorite modules
can still receive scheduling priority, but every complete pass targets all
installed modules.

The persistent archive is an append-only QOI pack plus atomic binary index under
`<Rack user>/Leviathan/Deepcache`. One process owns the write lease; additional
Rack contexts load a validated read-only snapshot and continue warming missing
previews in memory. Failure to create the database directory is reported as a
persistence error but does not disable memory-only browser acceleration.
The owning worker evaluates archive compaction after startup and after writes;
it compacts when dead data is at least 64 MB and at least 25% of the pack.

The archive's QOI pack remains hot in process memory, but successfully uploaded
cards do not permanently retain decoded RGBA. On a DAW graphics-context destroy,
NanoVG handles are invalidated and decoding pauses. When a new context appears,
the archive worker re-decodes from the in-memory QOI pack through the same
bounded handoff, visible cards are prioritized, and each temporary RGBA buffer
is released again after upload. Read-only archive workers support the same
disk-free restoration path. Memory-only previews without a recoverable QOI
entry are encoded into a volatile in-memory QOI entry by read-only workers; they
never mutate the shared database. RGBA is retained only while encoding/upload is
pending or when the archive worker cannot accept either persistent or volatile
work.

Persistent GPU residency is limited to models Rack can display at all. Models
that are hidden, disabled, or not whitelisted retain their QOI database entries
but do not retain NanoVG images. Deepcache periodically reconciles this
eligibility independently of search, brand, tag, and favorite filters. A model
that becomes eligible is decoded from the hot QOI pack and uploaded; one that
becomes unavailable releases its context-owned image on the UI thread. The cyan
GPU-warm progress target includes only display-eligible plugin builds.

Plugin fingerprints include the plugin identity, Rack panel-theme preference,
plugin directory timestamp, and binary/manifest size plus file modification time.
Changing the panel theme during a session is detected even while the browser is
closed. It invalidates resident rasters, refreshes their fingerprints, and
persists the rebuilt images so the next launch can reuse them.
