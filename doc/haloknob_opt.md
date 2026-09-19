Dragon King Leviathan, I did a source-level audit of the current `master` implementation, including the rendering primitives, invalidation paths, Integral Flux integration, Rack’s framebuffer behavior, and the existing OpenGL infrastructure in Leviathan. I was not able to run the repository under a profiler here, so the timing conclusions below are architectural rather than measured—but the dominant work is unusually clear from the code.

## Verdict

**The strongest path is a per-knob, framebuffer-cached OpenGL renderer driven by a mostly analytic fragment shader.**

That gives you:

* The same excellent idle behavior: one cached framebuffer image.
* Active updates reduced from roughly **200+ NanoVG path submissions** to one or a few GPU draws.
* More visual headroom: normal-mapped or genuinely 3D cap, emissive bloom, Fresnel edges, specular movement, better antialiasing.
* Independent refresh: only the knob whose value or interaction state changed becomes dirty.
* A clean route to a true 3D cap without committing the entire control to conventional mesh rendering.

I would build the **analytic “2.5D” shader first**, then add a real 3D cap mesh only if it produces a visible quality gain. At 46 logical pixels, a good shader can look more convincingly three-dimensional than a small mesh while remaining easier to antialias.

---

# What HaloKnob2 is currently doing

`LeviathanHaloKnob2` inherits `app::SvgKnob`, but hides the normal `SvgKnob` SVG and installs everything beneath the same outer `FramebufferWidget`:

1. Backplate SVG
2. Background glow
3. Segmented value ring
4. Foreground glow
5. Rotating center SVG
6. Cap reflections

Every value change propagates the new value into every child and dirties the entire outer framebuffer. Hover and drag changes swap the center SVG and also dirty the entire framebuffer. Changing global halo brightness does the same. Even `backLayer->valueNorm` is updated despite `rotateWithValue` being false. 

Integral Flux currently creates six of these controls: two Surge, two Sink, and two Curve knobs. It already counts dirty, active, and dragging Halo draws, but its timer wraps the whole knob draw rather than the individual rendering stages. 

## Important correction about the nested SVG caches

My earlier update assumed that the `SvgLayer` framebuffer cached the back and center SVGs during an outer Halo redraw. After checking Rack’s `FramebufferWidget` internals, that is **not what happens**.

Rack explicitly bypasses a `FramebufferWidget` when it is already drawing inside another framebuffer:

```cpp
if (bypassed || args.fb) {
    Widget::draw(args);
    return;
}
```

Because each `SvgLayer::cachedSvgFb` lives inside the Halo’s outer framebuffer, its cache is bypassed whenever the outer framebuffer redraws. The SVG paths are therefore redrawn directly during active Halo rendering. 

That makes the monolithic outer cache even more expensive than it initially appears.

---

# Why active rendering is expensive

With bloom enabled, the segmented work alone is approximately:

* Background and foreground glow:

  * 5 glow widths × 16 segments
  * **80 NanoVG strokes**
* Main LED ring:

  * 16 segments
  * Each segment performs a dark backing fill, gradient fill, outer stroke, and inner highlight stroke
  * **64 fills/strokes**
* Inner ring reflections:

  * 2 reflection rings × 16 segments
  * **32 strokes**
* Cap reflections:

  * 2 reflection rings × 16 segments
  * **32 strokes**

That is already **208 segmented fill/stroke operations**, before counting:

* Backplate SVG rendering
* Center SVG rendering
* Dark track bands
* Four guide arcs
* Terminators
* Cap shadow
* FBO clear/begin/end work
* Arc tessellation and gradient setup



The expensive part is therefore not filling 46×46 pixels. That is tiny. The likely dominant CPU work is:

* Rebuilding NanoVG paths
* Arc tessellation
* Repeated `nvgBeginPath()` / fill / stroke calls
* SVG path traversal
* Reconstructing geometry whose topology never changes

The framebuffer only saves this work when nothing changes. Once dirty, the whole ceremonial summoning circle is invoked again.

---

# Immediate low-risk improvements

These are worthwhile even if the final destination is OpenGL.

## 1. Stop using `SvgKnob` as the visual foundation

HaloKnob2 does not use the normal `SvgKnob` rendering path. It calls `SvgKnob::setSvg()`, hides `sw`, and then calls `SvgKnob::onChange()`, which rotates the hidden transform and dirties the framebuffer before HaloKnob2 performs its own update.

I would derive from `app::Knob` and create the required rendering widget explicitly:

```cpp
struct LeviathanHaloKnob2 : app::Knob {
    widget::FramebufferWidget* visualFb = nullptr;
    // ...
};
```

At minimum, call `app::Knob::onChange(e)` rather than `app::SvgKnob::onChange(e)` and remove the hidden SVG machinery. This is a modest optimization, but it also makes the ownership model much clearer. Rack’s `SvgKnob` normally rotates its `TransformWidget` and dirties its framebuffer on every change. ([GitHub][1])

## 2. Disable outer subpixel invalidation

The nested SVG framebuffers already set:

```cpp
dirtyOnSubpixelChange = false;
```

but the outer Halo framebuffer does not appear to do so. Rack normally dirties a framebuffer if its fractional world position changes by roughly 0.1 pixels. That can cause unnecessary rerenders while panning or moving modules. 

Add:

```cpp
visualFb->dirtyOnSubpixelChange = false;
```

This will not improve knob dragging, but it should prevent unrelated rack movement from awakening all the Halo machinery.

## 3. Track dirty reasons separately

Introduce explicit invalidation:

```cpp
enum HaloDirty : uint32_t {
    HALO_DIRTY_NONE       = 0,
    HALO_DIRTY_VALUE      = 1 << 0,
    HALO_DIRTY_CENTER     = 1 << 1,
    HALO_DIRTY_BLOOM      = 1 << 2,
    HALO_DIRTY_STYLE      = 1 << 3,
    HALO_DIRTY_GEOMETRY   = 1 << 4,
};
```

The desired mapping is:

| Event            | What actually changed                                 |
| ---------------- | ----------------------------------------------------- |
| Parameter value  | Ring state, center angle, value-dependent reflections |
| Hover/drag       | Center appearance only                                |
| Halo brightness  | Bloom and reflection intensity                        |
| Palette/config   | Ring and bloom colors                                 |
| Zoom/DPR/size    | Everything                                            |
| Static backplate | Only asset or geometry changes                        |

Even before layers are separated, this instrumentation will reveal how much active work comes from value changes versus hover behavior.

## 4. Cache both center appearances permanently

Currently hover/drag calls `centerLayer->setSvg()`, dirties the nested SVG framebuffer, and dirties the outer framebuffer. 

Load or rasterize both center states once:

```cpp
normalCenterImage
litCenterImage
```

Then select an image handle rather than reparsing/redrawing an SVG. Rotation should transform a cached texture, not redraw vector paths.

---

# The best optimized 2D path

A layered NanoVG refactor can help, but there is an important trap:

> **Do not place cached Rack framebuffers inside another Rack framebuffer.**

Nested caches are bypassed during the parent render. The layers must be sibling widgets, or their framebuffer textures must be sampled manually. 

A valid hierarchy would be:

```text
LeviathanHaloKnob2 : app::Knob
├── staticFramebuffer
├── dynamicRingFramebuffer
├── rotatedCenterImageWidget
└── capReflectionFramebuffer
```

### Static framebuffer

Cache indefinitely:

* Backplate
* Dark ring tracks
* Guides
* Terminators
* Cap shadow
* Fully inactive ring appearance

### Dynamic ring framebuffer

Redraw only:

* Active portion of LED ring
* Active/inactive boundary segment
* Active glow
* Value-dependent reflections

### Center image

Draw a cached raster texture with a NanoVG transform. This is only one image operation and requires no framebuffer refresh when its angle changes.

### Cap reflections

Potentially keep these with the dynamic ring, or isolate them if hover effects are added later.

## But layering has an idle tradeoff

Today, each knob costs one cached image composite per frame. Splitting it into three or four sibling framebuffer layers makes active invalidation narrower, but idle rendering now composites three or four textures.

That may still be cheap, but it is not automatically free. This is why I think a shader path is architecturally better: **one cached surface at idle and one extremely cheap rerender when active.**

---

# A stronger 2D optimization without multiple live layers

An intermediate option is to keep one outer framebuffer but replace the procedural work with cached image states.

The segmented display has a highly constrained state space:

* 16 segments
* Some number of fully active segments
* At most one partially blended segment

You could pre-render:

* 17 complete boundary states: 0–16 active segments
* A small partial-segment atlas, perhaps 16 or 32 interpolation levels
* Normal and lit cap images
* Static backplate

On a value change, the framebuffer redraw becomes approximately:

1. Draw boundary-state texture
2. Draw rotated partial-segment texture
3. Draw rotated cap texture
4. Draw cap reflection overlay

That preserves the current one-texture idle path while replacing hundreds of NanoVG paths with a few image composites.

This would likely provide a major active-speed improvement without introducing custom OpenGL shaders. The drawbacks are:

* Atlas management by palette, bloom setting, scale, and pixel ratio
* More texture memory
* Regeneration when global bloom changes
* Less flexibility for future visual evolution

It is a very good fallback or proof-of-concept, but the shader is ultimately cleaner.

---

# Recommended OpenGL architecture

## One cached OpenGL surface per knob

I would not begin with one module-wide OpenGL widget. Integral Flux’s six knobs occupy widely separated areas, so a module-sized transparent FBO would clear and update far more pixels than necessary whenever one knob changes.

Instead:

```text
LeviathanHaloKnob3D : app::Knob
└── HaloGlSurface : widget::OpenGlWidget
```

Each surface remains 46×46 logical pixels and refreshes independently.

The shader program and immutable material textures can eventually be shared, but each knob retains its own small framebuffer and dirty state.

## Critical OpenGlWidget detail

Rack’s default `OpenGlWidget::step()` intentionally marks the widget dirty every frame. ([GitHub][2])

Therefore Halo must override it:

```cpp
void HaloGlSurface::step() {
    // Preserve FramebufferWidget caching.
    widget::FramebufferWidget::step();
}
```

Do **not** call:

```cpp
widget::OpenGlWidget::step();
```

Set dirty only through state setters:

```cpp
void setValue(float value) {
    if (std::fabs(value - state.value) < 1e-6f)
        return;
    state.value = value;
    setDirty();
}
```

This preserves the exact idle behavior you already value.

The existing Bifurx OpenGL implementation provides useful shader compilation, transparent clearing, buffer reuse, and fallback patterns. Leviathan also already links and builds OpenGL code. 

---

# The analytic shader path

A single quad can reproduce almost all of HaloKnob2 procedurally.

For each fragment:

```glsl
vec2 p = localPosition - center;
float radius = length(p);
float angle = atan(p.y, p.x);

float sweepPosition = normalizeAngleToHaloSweep(angle);
float segmentPosition = sweepPosition * 16.0;
float segmentIndex = floor(segmentPosition);
float segmentFraction = fract(segmentPosition);
```

From those values, the shader can compute:

* Ring inclusion using signed radial distances
* Segment gaps
* Active versus inactive color
* Partial segment interpolation
* Guide rings
* Inner and outer reflection rings
* Gaussian bloom bands
* Terminators
* Antialiased edges using `fwidth()` and `smoothstep()`

The repository already uses GLSL 1.20 shaders with `fwidth()`, so this fits the compatibility level currently exercised by Bifurx. 

A representative state block would be:

```cpp
struct HaloVisualState {
    float valueNorm;
    float knobAngle;
    float bloomAmount;
    float hoverAmount;
    float dragAmount;

    NVGcolor activeColor;
    NVGcolor activeHighlight;
    NVGcolor inactiveColor;
    NVGcolor inactiveHighlight;

    uint32_t paletteId;
};
```

Each active redraw then becomes:

* Bind program
* Upload a few uniforms
* Draw one quad
* Optionally draw one cap mesh
* Finish framebuffer

No arc generation. No SVG traversal. No per-segment CPU loop.

---

# 2.5D versus full 3D

## 2.5D shader

The cap is represented by:

* Albedo texture or procedural material
* Normal map
* Height/bevel profile
* Fixed panel-space lighting
* Specular and Fresnel response
* Rotated UVs for the pointer and surface markings

Advantages:

* One quad
* Excellent antialiasing
* Easy hover illumination
* Easy bloom integration
* Visually detailed at very small sizes
* No mesh silhouette problems

At 46 pixels, this is probably the highest quality-per-cost route.

## True 3D cap

The next stage would use a small lathed mesh:

* 32–48 radial slices
* 4–8 profile rings
* Approximately 128–384 vertices
* Orthographic projection
* One directional key light
* Soft environment reflection
* Normal map or procedural brushed-metal material

The ring should still remain analytic rather than becoming geometry. Its segmentation, glow, and changing active boundary are better suited to a fragment shader.

The rendering stack would be:

1. Analytic backplate and emissive ring quad
2. Actual 3D cap mesh with depth
3. Analytic foreground reflection and bloom pass

This is a genuine 3D path while keeping the rapidly changing portion efficient.

### My judgment

A conventional all-mesh 3D knob is not the optimal first move. It adds:

* Mesh/resource lifecycle complexity
* Depth and transparency ordering
* Small-scale silhouette aliasing
* More work to match the current exact ring appearance

The best endpoint is likely:

> **Analytic GPU ring + true 3D cap mesh + cached per-knob OpenGL framebuffer.**

That should improve both active speed and visual quality.

---

# GL lifecycle requirements

Leviathan has already encountered driver stalls from steady-state `glIsProgram()`, `glIsBuffer()`, and `glIsTexture()` validation. The repository’s own validation notes report multi-millisecond spikes and recommend event-driven reset, lazy recreation, and diagnostic-only per-draw validation. 

Halo should follow the same rules:

* Reset local handles in `onContextDestroy()`
* Lazily recreate resources during the next draw
* Avoid GL cleanup from destructors unless the context is known current
* Avoid `glIs*` calls during normal rendering
* Keep extra validation behind `extraGlValidation`
* Clear the FBO with transparent alpha
* Explicitly restore or normalize modified GL state

For an initial implementation, per-surface shader resources are simplest and safest. Once stable, programs and immutable textures can be shared through a context-aware resource cache.

---

# Recommended implementation order

## Phase 1 — Instrument the current renderer

Add timers around:

* Outer framebuffer render
* Back SVG
* Background glow
* Light arc
* Foreground glow
* Center SVG
* Cap reflection

The correct place to measure total redraw cost is an override of `drawFramebuffer()`, not merely the knob’s normal `draw()`, because Rack can defer dirty framebuffer rendering according to frame budget. Rack’s framebuffer code performs the actual rerender only when dirty and sufficient frame time remains. 

Also count dirty causes:

```text
value
hover
drag
bloom
scale
subpixel
context
```

## Phase 2 — Surgical current-path cleanup

Land:

* `app::Knob` rather than hidden `SvgKnob`
* Outer `dirtyOnSubpixelChange = false`
* Remove value updates from static layers
* Cache normal/lit center rasters
* Prevent repeated same-value invalidation
* Add precise dirty-reason metrics

This gives a cleaner baseline and is low risk.

## Phase 3 — OpenGL analytic prototype

Implement one `HaloGlSurface` per knob:

* One quad
* Ring, glow, reflections, guides, and cap shadow in shader
* Existing cap raster sampled and rotated
* Default and orange palettes as uniforms
* Current NanoVG Halo as fallback

This phase should already accomplish the rendering-speed goal.

## Phase 4 — Improve the cap

Add either:

* A normal/height-mapped cap, or
* The procedural lathed 3D mesh

Compare at multiple zoom levels before committing the mesh. The mesh should survive a blind visual comparison, not merely satisfy the philosophical definition of “3D.”

## Phase 5 — Shared resources

After correctness:

* Share shader programs
* Share normal/albedo textures
* Share the static quad VBO
* Preserve per-knob FBO and dirty state

Do not prematurely centralize all knob rendering into one large module FBO.

---

# Benchmark matrix

I would test these cases:

| Scenario                                      | What it reveals                  |
| --------------------------------------------- | -------------------------------- |
| One Integral Flux, idle                       | Any steady-state regression      |
| Drag one Halo for 10 seconds                  | Normal interactive active cost   |
| Rapid mouse movement across all six           | Hover invalidation cost          |
| Host automation on all six params             | Worst simultaneous dynamic cost  |
| Ten Integral Flux instances, one active       | Scaling and FBO pressure         |
| Pan rack without changing values              | Subpixel invalidation            |
| 75%, 100%, 150%, 200% zoom                    | Tessellation and texture scaling |
| DPR 1 and DPR 2                               | FBO and antialiasing behavior    |
| Standalone and plugin-host context recreation | GL lifecycle correctness         |

Acceptance targets I would set:

* **No measurable idle-frame regression**
* **At least 50% lower CPU redraw cost** for the optimized NanoVG/atlas phase
* **At least 80% lower CPU submission cost** as a target for the shader path
* No framebuffer allocations during ordinary value movement
* No `glIs*` calls during production draws
* Screenshot differences confined to intentional visual improvements
* No stale frame after drag release, hover exit, or automation stop

The percentage figures are targets, not measured claims.

---

# Final recommendation

I would take this route:

1. Clean up the current inheritance and instrumentation.
2. Build `HaloGlSurface`, one per knob.
3. Reproduce the current ring analytically in one fragment shader.
4. Initially retain the existing center artwork as a rotated texture.
5. Add normal-map lighting.
6. Only then prototype the actual 3D cap mesh.
7. Keep the current renderer as a compatibility fallback until the GL path has survived standalone and DAW context cycling.

The deepest structural insight is that **more Rack framebuffer nesting is not the answer**. Nested caches are bypassed during an outer framebuffer render. The winning move is either:

* replace the expensive content inside the existing single framebuffer with cheap texture/shader composition, or
* make the layers siblings—but accept extra idle composites.

The per-knob cached OpenGL surface gives the most coherent balance: **one texture at rest, one tiny GPU render when alive, and substantially more visual dimensionality available than the current NanoVG path.**

[1]: https://raw.githubusercontent.com/VCVRack/Rack/v2/src/app/SvgKnob.cpp "raw.githubusercontent.com"
[2]: https://raw.githubusercontent.com/VCVRack/Rack/v2/include/widget/OpenGlWidget.hpp "raw.githubusercontent.com"
