# Procedural Panel Rendering Implementation Notes

## Purpose

These notes capture the proposed workflow for enhancing Leviathan panel artwork with NanoVG while keeping the SVG artwork as the spatial and visual foundation.

Integral Flux is the first candidate because its artwork can already be split into:

- `res/flux.panel.svg` — background and structural panel artwork
- `res/flux.labels.svg` — labels and decal artwork

The intended result is not a fully procedural replacement for the SVG panel. NanoVG should provide a transparent enhancement pass between the background and labels.

## Target Rendering Stack

The target layer order for Integral Flux is:

```text
1. flux.panel.svg
2. Cached transparent NanoVG enhancement layer
3. flux.labels.svg
4. Displays and other dynamic visual surfaces
5. Knobs, buttons, ports and lights
```

This protects label clarity while allowing the background to gain depth, illumination and stronger Leviathan artifact character.

The NanoVG layer should not repaint the opaque panel base unless a future art mode explicitly calls for that. Its initial role is comparable to a material and lighting pass over the existing panel composition.

## SVG Remains the Spatial Source of Truth

Panel geometry should not be independently duplicated as hardcoded C++ coordinates. Changes to bays, labels, displays or controls would otherwise require manually coordinating the SVG and NanoVG implementations.

The authoring SVG should contain stable semantic layers and enhancement guides:

```text
flux.svg
├── PANEL_BASE
├── LABELS
├── COMPONENT_ANCHORS
└── ENHANCEMENT_GUIDES
    ├── ART_TITLE_BAY
    ├── ART_LEFT_BAY
    ├── ART_RIGHT_BAY
    ├── ART_CENTER_BAY
    ├── ART_IO_BAY
    ├── ART_SPINE_TOP
    ├── ART_SPINE_BOTTOM
    ├── ART_HALO_CENTER
    ├── ART_TRACE_*
    └── ART_QUIET_*
```

The exact names can change, but they must be stable once consumed by code.

Enhancement guides should use simple SVG geometry where practical:

- Rectangles for control bays and quiet zones
- Circles for halos and ornamental nodes
- Points or short lines for spine endpoints and connection anchors
- Simple polylines for authored traces if extraction support is added

These guide elements are design metadata. They should not appear visibly in either production SVG layer.

## Collaborative Ownership Model

The SVG and NanoVG work should have separate, explicit responsibilities.

### SVG Responsibilities

- Overall panel composition
- Control and display placement
- Background structure and large permanent shapes
- Label content, typography and alignment
- Enhancement regions and anchor geometry
- Quiet zones where procedural detail must remain subdued

### NanoVG Responsibilities

- Glass-like shading and inset depth
- Cyan/violet edge illumination
- Low-alpha halos and localized atmosphere
- Central flux spine styling
- Circuit traces and IO bus styling
- Subtle field curves and vignette effects
- Theme-dependent color and intensity choices

This lets panel geometry be adjusted visually in the SVG editor while material and lighting behavior is tuned in code.

## Authoring and Generation Workflow

The desired workflow is:

1. Edit the master `flux.svg` artwork.
2. Maintain the semantic layer names and enhancement-guide IDs.
3. Generate `flux.panel.svg` and `flux.labels.svg` from the master.
4. Generate or update compiled anchor data for controls and enhancement guides.
5. Build the plugin and compare the composed result in Rack.
6. Tune NanoVG styles without duplicating or moving the underlying geometry.

Generated panel and label files should not be edited independently. The master SVG is authoritative.

The existing `PanelSvgUtils` and generated `PanelAnchorAtlas` are a useful foundation. The preferred runtime path is compiled atlas lookup, with SVG parsing retained as a development or compatibility fallback. If the current atlas cannot represent a required guide type, extend the generator deliberately rather than adding a second collection of hand-maintained coordinates.

## Rendering and Caching Structure

Rack's `FramebufferWidget` caches the drawing of its children. A procedural widget should not derive from `FramebufferWidget` and expect an overridden `draw()` method to be cached automatically.

A practical first implementation is:

```text
IntegralFluxWidget
├── SvgPanel: flux.panel.svg
├── FramebufferWidget: static overlay cache
│   ├── IntegralFluxPanelEnhancement: TransparentWidget
│   └── SvgWidget: flux.labels.svg
├── Dynamic display widgets
└── Controls, ports and lights
```

The enhancement widget draws first inside the overlay cache, followed by the label SVG. This guarantees label ordering and avoids re-rendering outlined label paths every frame.

Recommended framebuffer behavior:

- Set `dirtyOnSubpixelChange` to false unless testing reveals alignment problems.
- Invalidate only for size, graphics-context, theme or art-style changes.
- Do not invalidate for module parameters, audio state, lights or display updates.
- Keep animated effects outside the static overlay cache if animation is added later.

The initial implementation uses two panel-related caches: Rack's SVG panel cache and the enhancement/label overlay cache. This is acceptable for the prototype, but GPU memory should be measured with many instances. A later composite panel widget could cache the base SVG, NanoVG enhancement and labels together if a single-cache design proves worthwhile.

## Initial Integral Flux Enhancement Vocabulary

The first version should remain restrained. Useful elements are:

1. Glass shading inside the major control regions.
2. Thin cyan/violet edge accents around those regions.
3. A central vertical flux spine with a few authored nodes.
4. A subtle halo behind the central emblem or display region.
5. A bottom IO bus with branches aligned to jack groups.
6. A small number of deterministic circuit traces.
7. Very low-alpha field curves or atmospheric gradients.

Avoid in the first pass:

- Dense decorative fields
- Random layouts generated per instance
- Per-frame background animation
- Large blur stacks
- Runtime text replacing the label SVG
- Procedural geometry that competes with labels or controls

## Quiet Zones and Readability

Separating labels into an overlay protects them from being physically covered, but it does not guarantee visual clarity. Bright or high-frequency procedural detail can still make text difficult to read.

Quiet zones should therefore be authored in the SVG and exported as metadata. Suggested rules are:

- Bright traces must not cross quiet zones.
- Glass fills and broad gradients may cross quiet zones.
- Faint field curves may cross at substantially reduced opacity.
- Halos should be centered away from dense labels unless they improve contrast.

Quiet-zone behavior can initially be implemented through hand-authored routing. More generalized clipping or alpha attenuation should only be added if multiple modules need it.

## Asset Size and Performance Context

Current uncompressed Integral Flux asset sizes are approximately:

```text
flux.svg          212 KB
flux.panel.svg     78 KB
flux.labels.svg   139 KB
```

The split files are not inherently smaller than the monolithic SVG. Their immediate value is compositing flexibility. NanoVG becomes an asset-size optimization only if it eventually replaces meaningful background SVG complexity or if shared procedural helpers replace repeated artwork across multiple modules.

For the first prototype, visual quality and workflow validation matter more than a small per-module size reduction. Relevant measurements are:

- Packaged asset size
- Plugin binary-size change
- First render/cache-build time
- Steady-state UI draw time
- GPU framebuffer memory per module
- Behavior at 25% through 400% zoom
- Multiple-instance scaling
- Graphics-context recreation

## Proposed Implementation Sequence

### Phase 1: Prepare the SVG Contract

- Confirm the master, panel and label layer definitions.
- Remove accidental visible background content from the labels export.
- Add named enhancement regions, anchors and quiet zones.
- Ensure the split script excludes metadata guides from visible output.
- Regenerate and validate the anchor atlas.

### Phase 2: Prove the Three-Layer Composition

- Load `flux.panel.svg` as the base panel.
- Add a cached transparent enhancement/label overlay.
- Render a minimal NanoVG test treatment between the two SVG layers.
- Confirm label and control alignment at several zoom levels.

### Phase 3: Build the First Art Pass

- Add glass bays and edge accents.
- Add the central spine and halo.
- Add the bottom IO bus and a limited set of traces.
- Tune quiet-zone behavior and opacity.

### Phase 4: Generalize Carefully

- Extract shared primitives only after the Integral Flux vocabulary stabilizes.
- Define a reusable Leviathan panel theme and style structures.
- Test the same system on one differently shaped module before committing to a suite-wide API.

## Acceptance Criteria for the Prototype

- `flux.panel.svg`, NanoVG enhancements and `flux.labels.svg` render in the intended order.
- Labels remain sharp, aligned and readable.
- Controls, ports, lights and displays remain unchanged and correctly positioned.
- Static enhancement drawing is cached.
- No audio-thread work is introduced.
- The visual result remains recognizably based on the SVG panel.
- Moving an authored enhancement guide and regenerating the atlas moves the corresponding NanoVG element without editing C++ coordinates.
- The system remains usable in module-browser previews and after graphics-context recreation.

## Open Decisions

- Whether enhancement guides remain in the master SVG only or also survive invisibly in `flux.panel.svg`.
- Whether the production build should retain `flux.svg` as a fallback after the split path is proven.
- Whether simple trace routes should be encoded as SVG polylines or assembled from exported anchor points.
- Whether the final renderer should keep two caches or use one composite cached panel.
- Whether panel art themes are global, per-module or user-selectable.

The first implementation should resolve these questions through measurement and a working Integral Flux prototype rather than designing the complete suite-wide system in advance.
