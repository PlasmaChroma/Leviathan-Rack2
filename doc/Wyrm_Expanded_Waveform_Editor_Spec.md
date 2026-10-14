# Wyrm Expanded Waveform Editor Overlay

> Historical implementation specification. The animated `WyrmSand` system was
> removed in August 2026; references to its shared state and widget names below
> describe the architecture at the time this overlay was originally designed.

**Implementation specification for Codex / Terra 5.6**  
**Target:** Leviathan VCV Rack plugin, current Wyrm implementation  
**Feature type:** UI-only waveform editor expansion  
**DSP impact:** None  
**Patch-format impact:** None

---

## 1. Feature Summary

Add a small **expand editor** button to Wyrm that opens the existing waveform editor as a much wider temporary overlay.

The expanded editor must:

- Extend substantially to the left and right of Wyrm.
- Render above neighboring modules and cables without moving or resizing any module.
- Preserve all existing editor behavior:
  - Direct waveform point drawing.
  - Rock dragging and lifting.
  - Shift-modified rock interaction.
  - Editor lock state.
  - Sand view.
  - NanoVG, OpenGL, and OpenGL SHDR rendering.
  - Slither animation.
  - LFO waveform tracer.
  - Current point-count options.
- Follow Wyrm when the rack is panned or zoomed.
- Remain inside the visible Rack window.
- Collapse back into Wyrm from a dedicated collapse button.
- Close safely when Wyrm is deleted or the patch changes.

The expanded state is transient UI state. It must not be serialized into the patch.

---

## 2. Existing Implementation Context

The current Wyrm editor is already largely size-independent:

- `WyrmWaveEditor` derives its waveform geometry, rock geometry, hit testing, waveform-material raster, body samples, hover guides, and sand updates from `box.size`.
- `WyrmSandGlWidget` also derives OpenGL projection, textures, body samples, hover guides, and render-target sizing from `box.size`.
- `WyrmWidget` currently creates:
  1. A `WyrmSandGlWidget`.
  2. A `FramebufferWidget`.
  3. A `WyrmWaveEditor` inside the framebuffer.
- The OpenGL widget and NanoVG framebuffer occupy the same editor rectangle.
- Both editor layers share one `std::shared_ptr<WyrmSand>`.
- The actual waveform, rocks, lock state, renderer settings, and animation state live on `Wyrm`.

This means the editor does not need a second data model. The main work is widget ownership, scene placement, input-coordinate generalization, and lifecycle safety.

---

## 3. Architectural Decision

### 3.1 Use a scene-level overlay

The expanded editor shall be a temporary widget attached to `APP->scene`, not a child that merely draws outside the Wyrm module.

Do **not** change `WyrmWidget::box`, the module width, rack-grid placement, or module collision bounds.

Changing the module box would interfere with Rack placement, dragging, collision detection, and neighboring module positions. Drawing beyond the existing module box without leaving the module hierarchy would also create unreliable hit testing outside Wyrm’s bounds.

### 3.2 Reuse one live editor surface

Do not create a second simultaneously active `WyrmWaveEditor` and `WyrmSandGlWidget`.

Instead, refactor the current editor stack into a reusable composite widget named conceptually:

```cpp
struct WyrmEditorSurface : Widget;
```

The same surface shall be:

- Hosted inside Wyrm while collapsed.
- Temporarily reparented into the expanded overlay while expanded.
- Returned to its compact host when collapsed.

This avoids:

- Two editors advancing and publishing `uiSlitherPhase`.
- Two sand simulations competing or updating at different sizes.
- Duplicate OpenGL rendering.
- Duplicate performance instrumentation.
- Divergent hover, drag, and cached-render state.
- Unnecessary GL and NanoVG resource destruction/recreation on each toggle.

Reparenting may end any active Rack event selection or drag through Rack’s normal `removeChild()` behavior. This is acceptable because expansion is initiated from a separate button, not during an editor drag.

---

## 4. New UI Structure

Refactor the current editor setup into the following hierarchy.

### Collapsed

```text
WyrmWidget
└── WyrmEditorDock
    └── WyrmEditorSurface
        ├── WyrmSandGlWidget
        └── FramebufferWidget
            └── WyrmWaveEditor
```

### Expanded

```text
Scene
└── WyrmExpandedEditorOverlay
    ├── overlay chrome / title / collapse button
    └── WyrmEditorSurface
        ├── WyrmSandGlWidget
        └── FramebufferWidget
            └── WyrmWaveEditor

WyrmWidget
└── WyrmEditorDock
    └── expanded-state placeholder
```

`WyrmEditorDock` remains in the module at all times. It is the visual and positional anchor used by the scene overlay.

---

## 5. `WyrmEditorSurface`

Implement `WyrmEditorSurface` in `WyrmWidget.cpp` for the MVP, or extract it into dedicated Wyrm editor-overlay files if preferred.

Suggested members:

```cpp
struct WyrmEditorSurface : Widget {
    Wyrm* module = nullptr;
    std::shared_ptr<WyrmSand> sandState;
    Widget* sandGlWidget = nullptr;
    widget::FramebufferWidget* editorFramebuffer = nullptr;
    TransparentWidget* waveEditor = nullptr;

    explicit WyrmEditorSurface(Wyrm* module);
    void setEditorSize(Vec size);
    void resetVisualTransitionState();
};
```

### 5.1 Construction

Construction shall preserve the current stacking order:

```cpp
sandState = std::make_shared<WyrmSand>();

sandGlWidget = createWyrmSandGlWidget(module, sandState);
addChild(sandGlWidget);

waveEditor = createWyrmWaveEditor(module, sandState);

editorFramebuffer = new widget::FramebufferWidget();
editorFramebuffer->dirtyOnSubpixelChange = false;
editorFramebuffer->addChild(waveEditor);
addChild(editorFramebuffer);
```

The framebuffer must remain the direct parent of `WyrmWaveEditor`, because the editor currently locates its framebuffer with:

```cpp
dynamic_cast<widget::FramebufferWidget*>(parent);
```

### 5.2 Resizing

`setEditorSize()` must update all four relevant boxes:

```cpp
void WyrmEditorSurface::setEditorSize(Vec size) {
    setSize(size);

    sandGlWidget->setPosition(Vec());
    sandGlWidget->setSize(size);

    editorFramebuffer->setPosition(Vec());
    editorFramebuffer->setSize(size);

    waveEditor->setPosition(Vec());
    waveEditor->setSize(size);

    editorFramebuffer->setDirty();
}
```

Use `setPosition()` and `setSize()` rather than writing every box directly when practical, so Rack resize and reposition events are emitted.

### 5.3 Visual transition reset

When moving between compact and expanded sizes:

- Call `sandState->resetHistory()`.
- Mark the editor framebuffer dirty.
- Allow existing size-keyed caches to rebuild naturally.
- Do not modify waveform points, rocks, selected shape, slither settings, or DSP state.

It is acceptable for the transient sand disturbance history to reset when changing sizes. Preserving the waveform and rock configuration is mandatory.

---

## 6. Generic Mouse Coordinate Conversion

Both editor render paths currently calculate local mouse coordinates by assuming they are descendants of `APP->scene->rack`.

That assumption becomes invalid once the editor surface is reparented directly under the scene.

Replace the rack-relative calculation in both:

- `WyrmWaveEditor::currentLocalMousePos()`
- `WyrmSandGlWidget::currentLocalMousePos()`

with a generic scene-to-local conversion.

Suggested helper:

```cpp
static Vec localMousePosForWidget(Widget* widget) {
    if (!widget || !APP || !APP->scene) {
        return Vec();
    }

    const Vec sceneMouse = APP->scene->getMousePos();
    const Vec absoluteOrigin = widget->getAbsoluteOffset(Vec());
    const float absoluteZoom = std::max(widget->getAbsoluteZoom(), 1e-6f);

    return sceneMouse.minus(absoluteOrigin).div(absoluteZoom);
}
```

This must work in both conditions:

- The compact editor is inside Rack’s zoom hierarchy.
- The expanded editor is a screen-space scene child with an absolute zoom of `1`.

Do not retain any code path that requires the editor to be a descendant of `APP->scene->rack`.

---

## 7. Compact Editor Dock

Add a `WyrmEditorDock` at the existing `WYRM_WAVE_EDITOR` rectangle.

Suggested responsibilities:

- Own `WyrmEditorSurface` while collapsed.
- Expose the compact editor size.
- Act as the anchor for overlay positioning.
- Draw a subdued placeholder while the surface is detached.
- Track whether the editor is expanded.

Suggested members:

```cpp
struct WyrmEditorDock : Widget {
    bool expanded = false;

    void draw(const DrawArgs& args) override;
};
```

When expanded, draw only a subtle inactive treatment, for example:

- Existing dark editor recess remains visible.
- A low-opacity border or glow.
- A small centered `EXPANDED` label or inward/outward glyph.
- No animated duplicate waveform.

Do not leave another live waveform editor inside the dock.

The existing preview-frame enhancement around the compact editor may remain in place.

---

## 8. Expand Button

Add a small UI-only button near the compact waveform editor.

### 8.1 Placement

Preferred SVG anchor:

```text
WYRM_EDITOR_EXPAND_BUTTON
```

If the anchor is absent, use a deterministic fallback near the upper-right edge of the compact editor frame, positioned so it does not overlap the editable waveform region.

The button should ideally sit on the frame or immediately outside the editor surface, not inside the drawing area.

### 8.2 Behavior

- Left-click while collapsed calls `WyrmWidget::openExpandedEditor()`.
- The button is disabled when `module == nullptr`, such as a browser preview.
- It is not a Rack parameter and must not appear in `ParamId`.
- It must not modify engine state.
- Use either:
  - A small dedicated NanoVG button drawing horizontal outward arrows, or
  - A `LeviathanIconButton` with a new cached expand icon.

A NanoVG glyph is preferred for the MVP because it avoids adding an asset dependency.

Suggested accessible description or tooltip text:

```text
Expand waveform editor
```

---

## 9. Expanded Overlay

Implement a scene child named conceptually:

```cpp
struct WyrmExpandedEditorOverlay final : widget::OpaqueWidget;
```

The overlay’s own box shall cover only the expanded panel, not the entire scene.

This provides the desired behavior:

- The editor and its chrome block interaction with modules directly beneath them.
- Rack controls outside the overlay remain usable.
- The overlay is visually floating rather than globally modal.

### 9.1 Scene insertion order

Insert the overlay above the rack but below Rack’s menu bar and browser UI.

Preferred insertion:

```cpp
APP->scene->addChildBelow(overlay, APP->scene->menuBar);
```

Fallback to `APP->scene->addChild(overlay)` only if the preferred sibling insertion cannot be used safely.

### 9.2 Suggested members

```cpp
struct WyrmExpandedEditorOverlay final : widget::OpaqueWidget {
    WyrmEditorDock* anchorDock = nullptr;
    WyrmEditorSurface* editorSurface = nullptr;
    std::function<void()> collapseAction;

    Widget* collapseButton = nullptr;

    void step() override;
    void draw(const DrawArgs& args) override;
    void onHoverKey(const event::HoverKey& e) override;
};
```

The overlay does not own Wyrm DSP state. It only temporarily owns the editor surface in the widget tree.

### 9.3 Dimensions

Use screen-space pixel dimensions so the expanded editor remains legible regardless of rack zoom.

Recommended target:

- Preferred width: approximately `900 px`.
- Minimum useful width: approximately `480 px`.
- Maximum width: available scene width minus `16–24 px` margins.
- Preferred editor height: approximately `260–300 px`.
- Maximum height: available scene height minus menu bar and outer margins.
- Title/chrome height: approximately `28–34 px`.
- Interior padding: approximately `8–12 px`.

The exact formula may adapt to the current window:

```text
target width  = clamp(preferred width, usable minimum, available width)
target height = clamp(preferred height, usable minimum, available height)
```

If the Rack window is narrower than the normal minimum, use all available width rather than overflowing.

### 9.4 Positioning

On each `step()`:

1. Resolve the compact dock’s absolute scene origin with `getAbsoluteOffset(Vec())`.
2. Resolve its screen-space size using `getAbsoluteZoom()`.
3. Find the dock’s screen-space center.
4. Center the expanded panel on that point.
5. Clamp the panel into the available scene rectangle.
6. Account for the visible menu bar at the top.
7. Resize and reposition the editor surface inside the overlay only when the required size changes.

The overlay should therefore follow Wyrm when the user:

- Pans the rack.
- Changes rack zoom.
- Moves Wyrm.
- Resizes the Rack window.

Do not rebuild size-dependent caches merely because the overlay position changes. Only resize when the calculated editor dimensions actually change.

### 9.5 Chrome

Draw a premium floating-editor frame consistent with Wyrm:

- Dark opaque or nearly opaque background.
- Rounded corners.
- Soft shadow.
- Purple/cyan edge treatment.
- Optional thin gold center accent matching the waveform zero line.
- Small title such as `WYRM // WAVEFORM`.
- Dedicated collapse button in the upper-right corner.

The editor interior should remain visually identical to the compact editor rather than introducing a separate editing design.

### 9.6 Collapse controls

Mandatory:

- Collapse button with inward horizontal arrows.

Recommended:

- Pressing `Escape` while the pointer is over the overlay collapses it.
- Add a Wyrm context-menu item:
  - `Expand Waveform Editor` while collapsed.
  - `Collapse Waveform Editor` while expanded.

Do not require outside-click collapse for the MVP. Outside controls should remain usable.

---

## 10. `WyrmWidget` Ownership and Lifecycle

Add explicit editor-related members to `WyrmWidget`.

Suggested members:

```cpp
struct WyrmWidget : ModuleWidget {
    WyrmEditorDock* editorDock = nullptr;
    WyrmEditorSurface* editorSurface = nullptr;
    WyrmExpandedEditorOverlay* expandedEditorOverlay = nullptr;
    Widget* expandEditorButton = nullptr;

    void openExpandedEditor();
    void closeExpandedEditor();
    bool isEditorExpanded() const;

    ~WyrmWidget() override;
};
```

### 10.1 Opening

`openExpandedEditor()` shall:

1. Return immediately if already expanded.
2. Return if `module == nullptr`.
3. Return if `APP`, `APP->scene`, `editorDock`, or `editorSurface` is unavailable.
4. Create the overlay.
5. Record the compact dock as the overlay anchor.
6. Remove the surface from `editorDock`.
7. Add the surface to the overlay.
8. Set `editorDock->expanded = true`.
9. Insert the overlay into the scene.
10. Size and position it immediately or on its first `step()`.
11. Reset transient sand-path history and dirty the framebuffer.

### 10.2 Closing

`closeExpandedEditor()` shall:

1. Return if not expanded.
2. Remove `editorSurface` from the overlay before the overlay is deleted.
3. Add the surface back to `editorDock`.
4. Restore its compact size.
5. Set its position to `(0, 0)` inside the dock.
6. Set `editorDock->expanded = false`.
7. Clear back-pointers on the overlay.
8. Call `expandedEditorOverlay->requestDelete()`.
9. Set `expandedEditorOverlay = nullptr`.
10. Reset transient sand history and dirty the framebuffer.

Do not immediately `delete` the overlay from within a button event callback.

### 10.3 Destruction

`~WyrmWidget()` must close or detach the overlay before normal child destruction proceeds.

Required outcome:

- No scene-level widget may retain a pointer to a deleted Wyrm module or Wyrm widget.
- No editor surface may be orphaned outside the module after Wyrm is removed.
- Loading a new patch while expanded must not crash.
- Deleting Wyrm from the context menu while expanded must not crash.
- Rack shutdown or DAW editor teardown must not cause a GL call from an invalid lifecycle path.

A safe shutdown sequence is:

1. Detach the surface from the overlay.
2. Reattach it to the dock when the dock is still valid.
3. Null the overlay’s anchor, surface, and callback references.
4. Request deletion of the empty overlay.
5. Allow normal `ModuleWidget` child destruction to delete the reattached surface.

---

## 11. Rendering Requirements

### 11.1 NanoVG

Expanded mode must preserve:

- Sand image rendering.
- Waveform polarity fills.
- Alternating point-column shading.
- Zero line.
- Wyrm body strokes.
- Rocks.
- Hover guides.
- Drag/lift annotations.
- LFO tracer.

`WyrmWaveEditor` already invalidates size-keyed caches and wave-material raster dimensions. Verify that resizing the surface causes:

- `waveMaterialPixels` to rebuild.
- Cached display values to invalidate when required.
- Cached body points to rebuild for the new size.
- The parent framebuffer to become dirty.

### 11.2 OpenGL and OpenGL SHDR

Expanded mode must preserve:

- Sand texture rendering.
- Wave-column texture rendering.
- Hover guides.
- Body strip rendering.
- Shader-softened body rendering.
- Existing GL lifecycle protections.

Verify that resizing causes:

- The `OpenGlWidget` framebuffer to resize.
- `waveColumnTexture` to rebuild for the new dimensions.
- Body sample caches to invalidate based on `cachedBodySize`.
- Sand image upload dimensions to update.
- No GL resources to be recreated continuously while merely panning the rack.

### 11.3 Renderer switching while expanded

Changing among:

- NanoVG
- OpenGL
- OpenGL SHDR

must work while the editor is expanded without collapsing it.

The current visibility switching between the GL widget and NanoVG editor shall remain intact.

---

## 12. Interaction Requirements

All current editing semantics must remain unchanged in expanded mode.

### Point editing

- Click-drag writes waveform points.
- Crossing multiple columns fills intermediate points as it does now.
- Values clamp to `[-1, 1]`.
- Slither compensation remains based on the rendered slither phase.
- Lock state prevents point modification.

### Rock interaction

- Hit testing scales with expanded width and height.
- Drag mode continues sculpting the waveform.
- Lift mode temporarily removes the rock from collision resolution.
- Shift continues selecting the alternate rock behavior.
- Sand stamps follow the expanded coordinate system.
- Phase remains periodic and value remains clamped.

### Dragging outside the editor

Rack drag events may continue after the pointer leaves the editor box.

The generic local mouse conversion must continue returning valid local coordinates. Existing phase and value clamping may handle out-of-bounds positions.

### Underlying modules

The overlay’s opaque panel must consume pointer interaction over its own rectangle. Knobs, ports, cables, and modules beneath the expanded editor must not receive clicks through it.

Controls outside the overlay remain usable.

---

## 13. State and Serialization

Do not add the expanded state to:

- `Wyrm::dataToJson()`
- `Wyrm::dataFromJson()`
- `ParamId`
- Module DSP state

Opening a saved patch must always begin with Wyrm in its normal compact state.

Waveform edits made while expanded are naturally persisted through the existing waveform-point serialization.

---

## 14. Non-Goals

The MVP does not include:

- Physically resizing Wyrm’s HP width.
- Moving neighboring modules.
- Editing multiple waveform cycles.
- New waveform tools, brushes, selections, undo layers, or zoom/pan inside the waveform itself.
- A detachable operating-system window.
- Persistent overlay dimensions or position.
- Preserving the exact transient sand disturbance field through size changes.
- A second independent waveform view.
- DSP changes or wavetable-format changes.

---

## 15. Recommended File Changes

### `WyrmWidget.cpp`

Primary implementation location:

- Add `WyrmEditorSurface`.
- Add `WyrmEditorDock`.
- Add expand/collapse glyph buttons.
- Add `WyrmExpandedEditorOverlay`.
- Replace the current local editor construction with `WyrmEditorSurface`.
- Add member pointers to `WyrmWidget`.
- Add open, close, and destructor lifecycle methods.
- Add context-menu expand/collapse action.
- Add SVG-anchor lookup and fallback placement.

### `WyrmWaveEditor.cpp`

- Replace rack-relative mouse conversion with generic scene-to-local conversion.
- Preserve the framebuffer-parent assumption.
- Confirm size-change dirtying and cache invalidation.
- No waveform behavior changes.

### `WyrmSandGL.cpp`

- Replace rack-relative mouse conversion with generic scene-to-local conversion.
- Verify resize invalidation and framebuffer behavior.
- No shader or visual redesign required.

### `Wyrm.hpp`

No engine-state changes are required.

Only add declarations if the chosen file organization requires shared editor-overlay types or helper factories. Prefer keeping UI-only types out of the DSP header when possible.

### Panel SVG / visual assets

Optional but recommended:

- Add `WYRM_EDITOR_EXPAND_BUTTON` anchor.
- Add expand/collapse SVG icons only if the project prefers assets over NanoVG glyphs.

The feature must retain a computed placement fallback so missing panel anchors do not break construction.

---

## 16. Implementation Sequence

### Phase 1 — Coordinate generalization

1. Add the generic scene-to-local mouse helper.
2. Update NanoVG and OpenGL editor paths.
3. Verify existing compact behavior at multiple rack zoom levels.

### Phase 2 — Composite editor surface

1. Create `WyrmEditorSurface`.
2. Move current GL widget, framebuffer, and editor creation into it.
3. Keep the surface inside the existing editor rectangle.
4. Confirm zero visual or behavioral regression.

### Phase 3 — Dock and overlay

1. Add `WyrmEditorDock`.
2. Add scene overlay.
3. Implement reparenting.
4. Implement screen-space sizing and anchor tracking.
5. Implement safe collapse and destruction.

### Phase 4 — Controls and chrome

1. Add compact expand button.
2. Add overlay collapse button.
3. Add premium overlay frame.
4. Add context-menu action.
5. Add optional Escape behavior.

### Phase 5 — Validation

Run the complete matrix below and repair lifecycle, input, or rendering regressions before considering the feature complete.

---

## 17. Validation Matrix

Test each renderer:

- NanoVG
- OpenGL
- OpenGL SHDR

Test each visual mode:

- Sand off
- Sand on

Test representative point counts:

- 32
- 128
- 256

Test rock states:

- No rocks
- Multiple rocks
- Mouse drags rocks
- Mouse lifts rocks
- Shift alternate behavior

Test editing:

- Point click.
- Slow drag.
- Fast drag across many columns.
- Drag beyond editor edges.
- Lock/unlock while expanded.
- Factory-shape reset while expanded.
- Shape previous/next while expanded.

Test animation:

- Slither off.
- Slither active.
- LFO tracer visible.
- Audio-rate tracer hidden.

Test layout:

- Rack zoomed far out.
- Rack at normal zoom.
- Rack zoomed in.
- Rack panned while expanded.
- Wyrm moved while expanded.
- Wyrm beside modules on both sides.
- Wyrm near left window edge.
- Wyrm near right window edge.
- Small Rack window.
- Window resize while expanded.
- Fullscreen toggle while expanded.

Test lifecycle:

- Expand and collapse repeatedly.
- Delete Wyrm while expanded.
- Load another patch while expanded.
- Close Rack while expanded.
- Open and close a DAW plugin editor while expanded.
- Switch renderers repeatedly while expanded.
- Expand more than one Wyrm instance.

Test performance:

- Confirm only one editor surface steps and renders per Wyrm.
- Confirm no audio-thread work is added.
- Confirm panning does not recreate size-dependent textures.
- Confirm resize causes only the expected cache and texture rebuilds.
- Confirm no sustained GL errors or framebuffer churn.

---

## 18. Acceptance Criteria

The feature is complete only when all of the following are true:

1. Clicking the expand button opens a substantially wider editor above adjacent modules.
2. Wyrm and neighboring modules do not move.
3. The Wyrm module box and rack-grid width remain unchanged.
4. The expanded editor is a scene-level screen-space overlay.
5. The same live editor surface is reused rather than duplicated.
6. Point editing behaves identically in compact and expanded modes.
7. Rock drag/lift behavior works in expanded mode.
8. Lock, sand, slither, tracer, point count, and factory-shape controls remain coherent.
9. NanoVG, OpenGL, and OpenGL SHDR all render correctly.
10. The overlay follows rack pan, rack zoom, module movement, and window resizing.
11. The overlay remains clamped inside the visible scene.
12. The overlay blocks interaction with modules directly beneath it.
13. The collapse button restores the editor to its exact compact location and size.
14. Expansion is not written to patch JSON.
15. Deleting Wyrm or loading a patch while expanded does not crash or leak an orphaned scene widget.
16. Repeated expand/collapse cycles do not accumulate GL, NanoVG, framebuffer, or widget resources.
17. No DSP behavior or audio-thread performance regression is introduced.

---

## 19. Final Design Intent

The expanded editor should feel as though Wyrm temporarily unfolds a larger instrument surface across the rack, not as though the module itself has changed size.

The compact module remains the physical object in the patch. The expanded editor is a transient lens: wider, clearer, and more precise, but always anchored to the same waveform, the same rocks, and the same living wyrm beneath the sand.
