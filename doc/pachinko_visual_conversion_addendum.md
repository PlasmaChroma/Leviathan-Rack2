# Leviathan Pachinko Module
## Visual Asset Conversion and VCV Rack Integration Addendum

> **Status:** Production guidance for converting the generated underwater pachinko artwork into a functional VCV Rack panel.
>
> **Naming:** This document deliberately uses `MODULE_NAME` as a placeholder. It does not assume **Mawfall** remains the final name.
>
> **Source artwork:** `a_highly_detailed_colorful_illustrated_panel_face_1.png`
>
> **Source dimensions:** 971 × 1619 pixels

---

## 1. Purpose

The generated artwork is visually strong enough to define the module’s new identity, but it should **not** be dropped directly into Rack as a single flattened panel and treated as finished.

The conversion should preserve the ornate underwater arcade character while turning the image into a practical instrument surface with:

- A large readable physics playfield.
- Clearly positioned controls and ports.
- Runtime ball, peg, sink, pulse, and glow animation.
- Proper Rack screws and hardware.
- No false or baked-in control holes that disagree with the implemented widget layout.
- A maintainable asset structure that can be revised without repainting the entire module.

This addendum supplements the core pachinko specification. The underlying musical concept remains a visible physics-based probability sequencer: balls fall through geometry, are captured by sinks, and emit associated gates.

---

## 2. Visual Direction Pivot

The original draft describes a dark abyssal artifact with obsidian teeth and restrained color. The generated image suggests a stronger and more distinctive direction:

### **Luminous Undersea Arcade Relic**

The module should feel like an impossible pachinko shrine recovered from a playful synthetic ocean civilization:

- Saturated cyan and deep marine blue.
- Pearlescent shells and luminous beads.
- Gold and chrome ornamental rails.
- Tropical fish, coral, bubbles, wave curls, and jewel-like nodes.
- A central liquid-blue playfield.
- An original aquatic sovereign or sea-spirit motif.
- Joyful arcade spectacle translated into premium synthesizer design.

The new visual direction should remain **original**. Do not reproduce official *Umi Monogatari / Great Sea Story* logos, character likenesses, machine layouts, iconography, or branded typography. The inspiration should be recognizable as a genre and atmosphere rather than a copied product.

---

## 3. Recommended Module Width

The source artwork has an aspect ratio of approximately **0.600**.

At Rack’s standard panel scale:

- Panel height: **380 px at 1×**.
- Width: **15 px per HP at 1×**.

The source composition naturally fits roughly **15 HP**. However, the functional specification includes eight sink outputs, utility outputs, inputs, controls, and a substantial live display. A direct 15 HP implementation would likely compromise usability.

### Recommended production width: **18 HP**

Use the generated composition as a central ornamental shrine and extend it into an 18 HP functional panel.

At 18 HP:

```text
1× panel size: 270 × 380 px
2× panel size: 540 × 760 px
4× panel size: 1080 × 1520 px
```

The source can be adapted to a 4× 18 HP master by:

1. Scaling the artwork to approximately 1520 px tall.
2. Preserving the central composition without stretching.
3. Extending the left and right borders with newly painted oceanic side rails.
4. Using those side rails for secondary CV inputs, utility outputs, status indicators, or labels.

### Alternative: **16 HP**

A 16 HP version requires less horizontal extension and may be viable if the utility I/O set is reduced. It should not be chosen solely to avoid asset editing; the physics display and output mapping are more important than minimizing panel width.

### Not recommended: direct stretched fit

Do not non-uniformly stretch the image from 971 × 1619 into the target Rack aspect ratio. The shell crest, circular ornaments, pearls, holes, and figure will visibly deform.

---

## 4. Do Not Treat the Generated Black Holes as Final Hardware Positions

The image contains many convincing black circular openings and four rounded mounting slots. These are concept-art elements, not reliable production coordinates.

### Required cleanup

The base panel asset should have the following painted out or separated:

- All circular black control/jack holes.
- The four rounded rectangular mounting slots.
- Any ring whose size or position conflicts with actual Rack widgets.
- Any decorative bead line that appears to be a physical collider but will not match the runtime board.

Actual Rack ports, knobs, buttons, lights, and screws must be placed as real widgets.

### Exception

Some circular ornaments may remain as **decorative sockets** when they are intentionally aligned to real controls. In that case:

- The decorative ring belongs to the panel artwork.
- The jack, knob, button, or light is placed precisely over it.
- The ornament should be larger than the widget and remain visible around its edge.

No empty black hole should remain unless a real widget occupies it.

---

## 5. Recommended Asset Decomposition

Do not ship the entire visual identity as one monolithic bitmap. Produce a layered asset package.

```text
res/MODULE_NAME/
  panel_base.png
  panel_ornament_overlay.png
  playfield_frame.png
  playfield_mask.png
  crest.png
  lower_shell_cluster.png
  sea_spirit.png                 optional separate layer
  side_rail_left.png             if 18 HP expansion is used
  side_rail_right.png
  pearl_ball.png                 optional sprite
  sink_glow.png                  optional sprite
  bubble_soft.png                optional sprite
  MODULE_NAME-structure.svg      screws, labels, simple vector marks
```

### 5.1 `panel_base.png`

Contains:

- Outer marine-blue substrate.
- Coral and fish background.
- Broad color gradients.
- Side ambience.
- No baked controls.
- No dynamic balls, flashes, pegs, or sinks.

### 5.2 `panel_ornament_overlay.png`

Contains front-facing decorative elements that may overlap the live display:

- Chrome and gold curls.
- Shell rims.
- Pearls.
- Foreground coral tips.
- Decorative wave lips.

This layer is drawn after the playfield so balls can visually pass behind the ornamental frame.

### 5.3 `playfield_frame.png`

Contains the frame immediately surrounding the active simulation area. It should visually define the board boundary without embedding actual physics geometry.

### 5.4 `playfield_mask.png`

A grayscale or alpha mask defining the visible simulation aperture.

Recommended convention:

- White or alpha 1.0: simulation visible.
- Black or alpha 0.0: simulation clipped.
- Soft edge allowed only for subtle glow; collision boundaries remain mathematically explicit.

### 5.5 Optional separated character layer

The aquatic sovereign figure can remain embedded in the base panel, but a separate layer offers more flexibility:

- Slight parallax.
- Independent brightness tuning.
- Theme variants.
- Easier removal if the composition becomes too crowded.

---

## 6. Panel Rendering Architecture

The generated artwork is raster-heavy and should not be forced through NanoSVG.

### Recommended Rack structure

1. Use a simple SVG panel or blank structural panel as the actual `ModuleWidget` panel.
2. Add a custom `FramebufferWidget` or equivalent cached NanoVG widget immediately above the panel.
3. Load the panel raster assets through NanoVG image handles.
4. Draw controls and Rack ports above the cached art.
5. Draw the animated playfield in its own widget.
6. Draw the foreground ornament overlay above the playfield but below physical controls where appropriate.

Suggested hierarchy:

```text
ModuleWidget
  Structural panel / fallback fill
  Cached background artwork
  Live playfield widget
    clipped simulation background
    rails and pegs
    sink state
    balls and trails
    capture flashes
  Foreground ornamental overlay
  Labels and indicators
  Knobs, buttons, switches
  Inputs and outputs
  Rack screws
```

### Why separate widgets matter

- The expensive background is cached.
- The playfield can redraw continuously without invalidating the entire module.
- Dynamic objects can pass behind foreground ornamentation.
- The art can be revised independently from physics code.
- Control positions remain precise and testable.

---

## 7. Preliminary Playfield Aperture

The final aperture should be measured from the cleaned production master, but the generated image suggests a tall central region beginning below the shell crest and ending above the lower pearl/output ornament.

Use normalized panel coordinates rather than hard-coding source pixels.

Recommended initial normalized bounds:

```cpp
struct NormalizedRect {
    float x;
    float y;
    float w;
    float h;
};

NormalizedRect playfieldRect {
    0.235f,  // x
    0.205f,  // y
    0.530f,  // width
    0.505f   // height
};
```

These values are only a starting point. The final playfield should be as large as possible while preserving:

- The upper shell crest.
- Side ornamental rails.
- Lower sink/output region.
- Adequate room for pegs and readable ball motion.

### Strong recommendation

The live playfield should not be restricted to the small apparent blue center alone. The cleaned frame can be widened subtly so that the physics board occupies more of the panel than the concept image initially implies.

The artwork is a visual starting point; musical readability takes priority over pixel-perfect preservation.

---

## 8. Mapping the Logical Physics Board into the Aperture

Keep the existing logical board space:

```cpp
constexpr float BOARD_W = 1000.f;
constexpr float BOARD_H = 1600.f;
```

Map the logical board into the aperture while preserving aspect ratio:

```cpp
struct BoardTransform {
    rack::math::Vec offset;
    float scale = 1.f;
};

BoardTransform computeBoardTransform(
    const rack::math::Rect& aperture,
    float boardW,
    float boardH
) {
    const float sx = aperture.size.x / boardW;
    const float sy = aperture.size.y / boardH;
    const float scale = std::min(sx, sy);

    const rack::math::Vec used {
        boardW * scale,
        boardH * scale
    };

    return {
        aperture.pos.plus(aperture.size.minus(used).mult(0.5f)),
        scale
    };
}
```

Use the same transform for:

- Ball positions and radii.
- Peg positions and radii.
- Rails and wall thickness.
- Sink capture regions.
- Debug geometry.
- Mouse-to-board interaction.

Do not infer collision geometry from painted ornament shapes. The visual frame and the physics geometry should be deliberately aligned but remain separate systems.

---

## 9. Static Art Versus Runtime Geometry

The generated image already contains bead lines, sockets, rails, and circles that resemble pachinko geometry. The production version should clarify which elements are decorative and which elements affect physics.

### Static decoration

- Gold and pearl beading around the outer arch.
- Shell crest.
- Fish and coral.
- Side wave scrolls.
- Character art.
- Lower ornamental pearl cluster.

### Runtime physics geometry

- Active pegs.
- Deflectors.
- Capsule rails.
- Funnel dividers.
- Sink capture zones.
- Ball paths.

### Visual rule

Every runtime collider should have a visible representation. Every strongly collider-like object inside the aperture should either:

1. Affect physics, or
2. Be visually subdued enough that users do not expect collision.

This is crucial. The user must be able to predict trajectories from the visible board.

---

## 10. Revised Peg and Ball Visual Language

The new panel theme suggests a more specific rendering vocabulary than the original dark-metal draft.

### Balls

Render balls as animated pearls or chrome pachinko beads:

- Bright central highlight.
- Cyan reflected rim.
- Small warm pearl reflection.
- Soft shadow beneath.
- Optional short trail at high velocity.
- Slight size increase or bloom during capture.

The physics circle remains simple even if the sprite appears dimensional.

### Pegs

Recommended peg styles:

- Small gold pins with pearl caps.
- Blue jewel studs.
- Coral-polished nodes.
- Chrome bubbles with a luminous core.

Avoid making every peg equally ornate. The board will become unreadable. A restrained peg hierarchy is preferable:

```text
Ordinary peg: small gold/pearl pin
Bumper peg: larger purple jewel
Guide peg: elongated chrome node
Special trigger: animated shell aperture
```

### Rails

Rails should echo the white-blue wave filigree and gold trim from the panel:

- Dark navy inner body.
- Cyan luminous edge.
- Fine gold outer highlight.
- Moderate thickness for clear collision expectations.

---

## 11. Sink and Output Integration

The eight primary sinks remain the musical center of the design.

### Recommended physical arrangement

Keep eight bottom capture lanes inside the playfield, left to right.

Below or immediately adjacent to the playfield, place eight corresponding output jacks in a clearly ordered row or shallow arc.

```text
Sinks:   1   2   3   4   5   6   7   8
Outputs: 1   2   3   4   5   6   7   8
```

Do not rely only on decorative symmetry. The mapping must be unambiguous.

### Cause-and-effect animation

When a sink captures a ball:

1. Ball compresses or blooms into the aperture.
2. Sink flashes.
3. A luminous pulse travels down a short conduit.
4. Corresponding output ring flashes.
5. Gate output fires.

The conduit animation may be implemented procedurally rather than baked into the panel.

### Conduit pulse model

```cpp
struct ConduitFlash {
    float phase = 0.f;
    float intensity = 0.f;
};
```

On capture:

```cpp
conduitFlashes[sinkIndex].phase = 0.f;
conduitFlashes[sinkIndex].intensity = 1.f;
```

During rendering:

- Advance `phase` from sink to jack.
- Decay `intensity` exponentially.
- Draw a short moving pearl or cyan-gold light streak.

---

## 12. Control Layout Strategy

The art is dense. Controls should occupy deliberate islands rather than being scattered over detailed coral and figure work.

### Preferred division for 18 HP

#### Upper utility band

- `DROP` button.
- `DROP` input.
- `RATE`.
- `DENSITY`.
- Small activity or active-ball indicator.

Place these around or just below the shell crest without covering its central pearl.

#### Side control rails

Left rail:

- `GRAVITY`.
- `BOUNCE`.
- `DRAG`.
- Associated CV inputs where retained.

Right rail:

- `TILT`.
- `CHAOS`.
- `LAYOUT` or board selector.
- `CLEAR` / reseed controls.

The precise split can change, but physics controls should frame the display instead of occupying the playfield.

#### Lower output shrine

- Eight sink outputs.
- `ANY`.
- `LEFT`.
- `RIGHT`.
- `VEL`.
- `POS`.
- `ACT`.

The lower shell and pearl ornament should frame these outputs without hiding labels or cable access.

### Cable clearance

Ensure plugs do not cover the central playfield more than necessary. Primary input and output jacks should be biased toward the lower and outer panel edges.

---

## 13. Typography and Naming

The image currently contains no text, which is ideal for adaptation.

### Typography goals

- High legibility at Rack scale.
- Pearlescent white or pale cyan lettering.
- Thin dark-blue shadow or outline.
- Restrained gold for section headers.
- No imitation of official Japanese pachinko branding.

### Title treatment

The eventual title should be rendered as a separate asset or vector path, not baked into the panel base.

```text
res/MODULE_NAME/title.svg
```

This permits final naming to change without repainting the panel.

### Labels

Keep functional labels simple and uppercase:

```text
DROP  RATE  DENSITY
GRAV  TILT  BOUNCE  DRAG  CHAOS
CLEAR  SEED  LAYOUT
ANY  LEFT  RIGHT  VEL  POS  ACT
1 2 3 4 5 6 7 8
```

The fantasy art supplies spectacle; labels should supply discipline.

---

## 14. Mounting Screws

Remove the four generated rectangular slots.

Use the Leviathan suite’s actual Rack screw widget or project-standard custom screw asset. The final screw positions must be consistent with other modules and should not be dictated by the generated image.

Recommended:

- One screw near top-left.
- One screw near top-right.
- One screw near bottom-left.
- One screw near bottom-right.

Keep screw centers at safe Rack-standard margins.

---

## 15. Runtime Lighting

The panel already contains extensive painted luminosity. Runtime lighting should therefore be selective.

### Animate

- Active ball highlights.
- Sink flashes.
- Output rings.
- Drop pearl/button.
- Special bumpers.
- Conduit pulses.
- Optional soft water shimmer inside the playfield.

### Keep static

- Most coral sparkle.
- Fish highlights.
- Shell crest glow.
- Broad caustic lighting.
- Character illumination.

### Guiding principle

Anything that changes should communicate state. Avoid animating the entire panel merely because it can glow.

---

## 16. Optional Water-Shimmer Effect

A very subtle shimmer can make the central playfield feel alive without requiring shaders.

Possible NanoVG approach:

- Draw two or three large translucent caustic bands.
- Move them slowly at different speeds.
- Clip them to the playfield mask.
- Use low alpha.
- Update at display frame rate, not audio rate.

Do not distort the ball positions or collider rendering. The shimmer is decorative only.

A shader-based treatment can be considered later, but it is not necessary for the MVP.

---

## 17. Raster Resolution and Export

### Recommended master

Create a cleaned **4× master** at:

```text
1080 × 1520 px for 18 HP
```

Then export:

```text
panel_base@4x.png
panel_base_2x.png
panel_base@1x.png      optional
```

Rack can render from the high-resolution master, but keeping a 2× version may reduce memory and loading cost while remaining visually sharp.

### Color format

Use:

- PNG.
- 8-bit RGBA.
- sRGB.
- Premultiplied-alpha behavior verified in Rack.

There is no practical need for 16-bit-per-channel panel assets at runtime.

### Compression

Run PNG optimization only after visual approval. Do not use palette reduction if it visibly damages gradients, caustics, or pearl highlights.

---

## 18. Alpha Cleanup Requirements

Generated imagery often contains dark or bright fringe pixels around cutout elements. Every separated overlay must be checked against both light and dark backgrounds.

### Required checks

- No black halo around chrome edges.
- No white fringe around coral or figure hair.
- No fake checkerboard transparency.
- Transparent pixels must have valid alpha, not merely painted background.
- Soft glow should taper smoothly rather than terminate as a hard box.

Where practical, bleed neighboring colors into fully transparent pixels before export to reduce interpolation seams.

---

## 19. Suggested Implementation Classes

```cpp
struct UnderseaPanelArt : rack::widget::FramebufferWidget {
    int baseImage = -1;
    int overlayImage = -1;

    void draw(const DrawArgs& args) override;
};

struct PachinkoPlayfield : rack::widget::Widget {
    MODULE_NAME* module = nullptr;
    int frameImage = -1;
    int maskImage = -1;

    void draw(const DrawArgs& args) override;
    void drawBoardBackground(const DrawArgs& args);
    void drawRails(const DrawArgs& args);
    void drawPegs(const DrawArgs& args);
    void drawSinks(const DrawArgs& args);
    void drawBalls(const DrawArgs& args);
    void drawEffects(const DrawArgs& args);
};

struct ForegroundOrnament : rack::widget::FramebufferWidget {
    int image = -1;
    void draw(const DrawArgs& args) override;
};
```

### Threading rule

All NanoVG image loading, creation, deletion, and drawing must occur on the UI/render thread. The audio thread owns physics and DSP state only.

The display should read a stable render snapshot rather than iterating mutable physics vectors while the audio thread modifies them.

---

## 20. Audio/UI State Handoff

Because the physics simulation runs in `process()` while the playfield renders on the UI thread, establish an explicit snapshot mechanism.

### Simple bounded snapshot

```cpp
struct BallRenderState {
    rack::math::Vec pos;
    rack::math::Vec vel;
    float radius = 0.f;
    float age = 0.f;
    uint32_t id = 0;
};

struct PachinkoRenderSnapshot {
    std::array<BallRenderState, MAX_BALLS> balls;
    int ballCount = 0;
    std::array<float, 8> sinkFlash {};
    std::array<float, 8> conduitFlash {};
};
```

Use either:

- Double buffering with an atomic active index, or
- A lightweight lock-free copy strategy.

Do not block the audio thread on a UI mutex.

---

## 21. Conversion Workflow

### Phase A — Art cleanup

1. Preserve the original generated image unchanged.
2. Create a layered working file.
3. Remove black holes and mounting slots.
4. Reconstruct art beneath removed holes.
5. Isolate foreground ornamentation.
6. Establish the expanded 18 HP canvas.
7. Extend side rails and background.
8. Define the clean playfield aperture.
9. Export a provisional panel composite.

### Phase B — Control-layout proof

1. Import the provisional panel into Rack.
2. Place temporary controls and ports.
3. Verify cable clearance.
4. Verify labels at 100% UI scale.
5. Confirm the playfield is large enough to read trajectories.
6. Revise artwork around actual widget coordinates.

### Phase C — Runtime board integration

1. Implement logical board-to-aperture transform.
2. Render simple pegs, rails, balls, and sinks.
3. Verify visual colliders match physics.
4. Add clipping/masking.
5. Add foreground overlay.
6. Add capture and conduit effects.

### Phase D — Final polish

1. Replace temporary widgets with Leviathan production widgets.
2. Finalize title and labels.
3. Tune glow intensity.
4. Optimize PNG assets.
5. Test light and dark Rack themes.
6. Test 75%, 100%, 125%, and 150% UI scaling where practical.
7. Verify Windows, Linux, and macOS rendering.

---

## 22. Codex Implementation Order

A coding agent should not attempt to solve art cleanup and module logic simultaneously.

Recommended order:

1. Build the module at the final HP width with a placeholder dark-blue panel.
2. Establish all parameter, input, output, and display coordinates.
3. Implement and validate physics.
4. Add the live playfield clipping rectangle.
5. Integrate provisional background art.
6. Adjust widget positions against the real composition.
7. Integrate cleaned layered assets.
8. Add animated lighting and conduit effects.
9. Replace the placeholder module name only after naming is settled.

The background should adapt to the instrument layout—not force the instrument into accidental AI-generated holes.

---

## 23. Visual Acceptance Criteria

The conversion is successful when:

1. The panel preserves the luminous undersea pachinko identity of the generated artwork.
2. No official franchise logo, character, or copied machine layout is present.
3. No false empty control holes remain.
4. All controls are legible and reachable.
5. Patch cables do not substantially obscure the active board.
6. The live playfield is the dominant performance visual.
7. Every visible runtime collider behaves as expected.
8. Sink-to-output mapping is immediately understandable.
9. Animated light communicates state rather than creating visual noise.
10. The panel remains sharp at normal Rack zoom.
11. Static raster art is cached and does not redraw unnecessarily.
12. The audio thread never performs image or NanoVG work.
13. The module still reads as part of the Leviathan suite despite its brighter, more ecstatic visual personality.

---

## 24. Recommended Immediate Decision

Before final art cleanup, freeze these three structural choices:

```text
Module width:       18 HP recommended
Playfield position: central, tall, dominant
Output arrangement: eight direct sink outputs along lower region
```

Everything else—including final name, precise control islands, character prominence, and secondary output placement—can evolve around those anchors.

The generated image should be treated as the module’s **visual constitution**, not its literal manufacturing diagram.
