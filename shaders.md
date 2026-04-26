# TDScope Shader-Native Notes

## Why consider shaders

Current `OpenGL` mode spends a meaningful amount of time on CPU-side prep:

- rebuilding per-row geometry
- rebuilding connector batches
- submitting many line segments

The connector path is especially expensive because it is currently expressed as
explicit geometry between adjacent rows. That makes connectors cost CPU time,
allocation pressure, and draw submission overhead.

At the same time, connectors in `Tail raster` are perceptually closer to a
secondary body pass than to thin edge outlines. That suggests a shader-native
approach should stop thinking of connectors as literal line segments.

## Important caveat

We isolated a separate zoom-freeze issue tied to `widget::OpenGlWidget`
presentation. A shader renderer inside the same presentation wrapper may still
inherit that freeze behavior.

So:

- shader work can still be a good steady-state performance move
- shader work is not by itself a fix for the `OpenGlWidget` zoom freeze

## Core design shift

Do not model connectors as explicit geometry.

Instead:

1. Upload compact per-row data to the GPU.
2. Let the shader reconstruct:
   - main trace body
   - halo
   - continuity energy between adjacent rows

In other words, connectors become a continuity field, not a pair of CPU-built
diagonal edge lines.

## Minimum GPU data model

Per visible row:

- `x0`
- `x1`
- `visualIntensity`
- `colorDrive`
- `valid`

Optional later:

- precomputed `center`
- precomputed `span`
- lane id / stereo side

This can live in:

- 1D texture
- texture buffer
- SSBO / storage buffer

The first practical version should keep history/ring management on CPU and only
move rendering/compositing to GPU.

## Two realistic shader architectures

### Option A: Fullscreen field shader

Render one quad over the scope rectangle.

Fragment shader:

- determines which row(s) the fragment corresponds to
- samples current row plus neighbors
- computes body coverage from `x0/x1`
- computes halo energy
- computes continuity/connector energy from adjacent rows

Pros:

- most flexible visual model
- easiest way to make connectors feel like body continuity
- avoids explicit connector geometry entirely

Cons:

- highest shader complexity
- easiest to overpay in fragment cost if the logic gets too ambitious

### Option B: Instanced row quads

Render one quad or strip per row.

Vertex data per instance:

- row position
- `x0`
- `x1`
- `visualIntensity`
- `colorDrive`
- `valid`

Fragment shader:

- shades body and halo for the row
- optionally samples adjacent rows from a row-data texture to add continuity

Pros:

- simpler migration path
- still removes most CPU connector geometry work
- keeps row semantics explicit

Cons:

- less elegant than a full field approach
- continuity logic still needs adjacent-row sampling if we want connectors to
  read like body energy

## Recommended connector model

For adjacent visible rows `i` and `i+1`, derive:

- `center0 = (x0_0 + x1_0) * 0.5`
- `center1 = (x0_1 + x1_1) * 0.5`
- `span0 = x1_0 - x0_0`
- `span1 = x1_1 - x0_1`

Continuity strength should depend on:

- interval overlap
- center shift
- span change
- intensity / transient drive

Desired behavior:

- strong overlap + small shift => strong body-like continuity
- moderate shift + transient => bright but narrower bridge
- weak overlap => little or no connector energy

That gives a visual closer to `Tail raster`, which appears to gain brightness
from connector overlap, while still letting the shader keep sharper definition.

## Best first target

The best quality/performance compromise is probably:

1. keep CPU row-history prep
2. upload row data only
3. render main body + halo in shader
4. replace explicit connectors with adjacent-row continuity energy

That should:

- reduce CPU batch-building cost substantially
- eliminate most explicit connector geometry generation
- make it easier to match `Tail raster` brightness without making the result as
  fuzzy

## What should stay on CPU initially

Keep these on CPU for the first shader pass:

- history ring maintenance
- row extraction from scope bins
- transient/color-drive generation
- visibility / row validity decisions

This keeps the migration bounded. If performance is still insufficient later,
we can revisit moving more prep work to GPU.

## Migration steps

### Phase 1

- keep current `OpenGL` mode data prep
- replace line-segment connector generation with row-data upload
- prototype instanced or fullscreen shader rendering

### Phase 2

- tune continuity energy so connectors read like body support instead of edge
  outlines
- match brightness against `Tail raster`

### Phase 3

- profile whether CPU prep or GPU fill is now the bottleneck
- decide whether more prep should move to GPU

## What success looks like

A successful shader-native version should:

- outperform current GL mode at dense row counts
- remove connector CPU cost as a primary bottleneck
- look brighter/denser than current GL mode
- stay sharper than `Tail raster`

## Open question

Even if the renderer itself improves, we still need to decide whether the final
presentation should continue to use `widget::OpenGlWidget`, given the zoom-freeze
behavior already observed.
