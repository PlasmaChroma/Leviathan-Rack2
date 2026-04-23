# TDScope Shader Redesign Opportunities

## Purpose

`shaders.md` focuses on a shader-native implementation path.

This note is about a different question:

What becomes possible if `TDScope` stops treating the renderer as explicit row
geometry and instead treats it as reconstruction of a visual field from compact
row data?

The point is not only speed. The point is that a shader path opens up redesigns
that are awkward or expensive in the current CPU/geometry model.

## Current renderer mindset

The current advanced renderer still thinks in terms of:

- rows
- row segments
- connector lines
- halo passes

That is a geometry mindset. It works, but it means brightness, continuity, and
density are partly accidents of overlap and line width.

A shader-native redesign would instead think in terms of:

- body energy
- continuity energy
- halo falloff
- temporal persistence

Those are visual fields, not line primitives.

## Biggest redesign opportunity

### Replace “connectors” with continuity energy

The current connector model is explicit geometry between adjacent row edges.

That creates two problems:

1. it is expensive on CPU
2. it reads visually like outlines rather than body support

A shader redesign should instead derive continuity from neighboring row
intervals.

Input:

- row `i` interval `[x0_i, x1_i]`
- row `i+1` interval `[x0_j, x1_j]`
- intensities / transient drive

Output:

- a continuity field between the rows

Desired visual behavior:

- strong overlap becomes body-like support
- moderate shift becomes a bright but narrower bridge
- weak overlap contributes little

This would likely move the look closer to what `Tail raster` perceptually does,
without explicitly duplicating its fuzzier pixel behavior.

## Redesign ideas

### 1. Reconstruct a luminous surface instead of drawing strokes

Current mental model:

- each row is a stroke
- connectors link strokes
- halo sits on top

Shader redesign:

- each row contributes energy to a 2D field
- connectors are simply coherence between neighboring row contributions
- halo is wider falloff from the same field

This makes the renderer feel like one visual system instead of several loosely
matched layers.

### 2. Make brightness more energy-based and less width-based

Today, perceived brightness is strongly affected by:

- line width
- overlap
- connector duplication
- zoom-dependent rasterization behavior

A shader redesign could define:

- how much energy a row contributes
- how much continuity contributes
- how much halo broadens without stealing too much core definition

That would make zoom matching easier and reduce reliance on trial-and-error
width tuning.

### 3. Use sub-row interpolation instead of brute-force density

The current renderer is visibly tied to row count.

A shader can interpolate smoothly between rows. That allows:

- more continuous motion
- fewer visible discrete steps
- less need to crank row count just to improve continuity

This could let the renderer look denser without paying full dense-row CPU cost.

### 4. Separate “sharpness” from “brightness”

Right now those two are entangled.

If you make the trace brighter by adding width or connector overlap, it often
also gets fuzzier.

A shader field can separate:

- core sharpness
- shoulder softness
- continuity fill
- halo radius

That means “brighter but still sharp” becomes a first-class design target
instead of a difficult compromise.

### 5. Define the scope as a field sampled from row intervals

Instead of asking:

- what line segments should I emit?

Ask:

- for this pixel, how much support does the nearby row data give me?

That leads to a better renderer abstraction:

- row data is only control data
- final appearance is reconstructed at sampling time

This is probably the most important conceptual shift.

### 6. Better anti-aliasing than driver GL lines

With current GL lines, appearance depends on driver line rasterization and
line-smooth behavior.

A shader can provide a more stable edge model:

- sharp core
- controlled falloff
- less dependency on vendor line behavior
- more stable response across zoom levels

### 7. Temporal behavior can become deliberate

Today any temporal persistence is mostly inherited from row history and redraw.

A field-based renderer could add:

- controlled temporal smoothing
- transient bloom persistence
- motion-aware energy retention

That is more advanced, but shader-native rendering makes it much more natural.

### 8. Style can become parameterized

Instead of hardcoding lots of empirical width/alpha constants, a redesign can
define higher-level artistic parameters such as:

- core sharpness
- continuity strength
- halo softness
- transient bloom gain
- overlap gain

That should make the renderer easier to tune and compare.

## Quality goals unlocked by redesign

If done well, a shader-native redesign could:

- preserve the sharpness advantage already seen in current `OpenGL` mode
- recover the missing perceived brightness seen versus `Tail raster`
- reduce connector CPU cost substantially
- reduce dependence on very high row counts for visual smoothness

## Best practical redesign target

The strongest target is:

### A continuity field from row intervals

Not:

- explicit connector lines
- explicit row-edge outlines

But:

- row body energy
- continuity energy between rows
- halo from the same underlying reconstructed field

That is the redesign most likely to improve both quality and performance.

## What this does not solve automatically

A better renderer model does not automatically solve the `OpenGlWidget` zoom
freeze problem. That is a separate presentation-layer issue.

So this redesign note should be read as:

- renderer quality/performance opportunity
- not proof that the current Rack GL wrapper issues disappear

## Short version

The biggest shader-native opportunity is not “draw the same lines faster.”

It is:

### stop rendering rows and connectors as explicit geometry

and instead:

### reconstruct a coherent luminous continuity field from compact row data

That is the redesign most likely to buy real quality without paying dearly for
performance.
