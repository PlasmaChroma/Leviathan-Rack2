# GLSL 1.20 Polyline and Waveform Rendering: Performance Research and Experiment Ranking

## Executive conclusion

Dragon King Leviathan, the strongest result of this research is that **the analytical screen-space distance formulation is the right geometric primitive to keep**, but **fullscreen execution is not necessarily the right domain on which to evaluate it**.

Your present SHDR path already has several unusually good properties: it derives body coverage from minimum screen-space point-to-segment distance; clamps the projection to each segment, thereby naturally retaining endpoint distance semantics; minimizes squared distance over seven segments and performs a single square root; evaluates all three translucent body layers from the same scalar distance; and now combines waveform and body evaluation in one fullscreen fragment shader. fileciteturn0file0 The measured squared-distance change reduced the 489×445 median body time from 112.34 μs to 94.79 μs, while the attempted per-fragment vertical rejection usually regressed after normalization, an excellent concrete example of why apparently obvious shader culling must be measured rather than assumed. fileciteturn0file1 NVIDIA's contemporary guidance for the GLSL-1.20-era GPU generation makes exactly that warning: fragment branches execute under SIMD/predication constraints, branch coherence matters, and every optimization should be independently benchmarked because source transformations can inhibit driver optimizations. citeturn18view0turn14search0

The three experiments I would prioritize are:

| Rank | Experiment | Why it ranks here | Geometry |
|---|---|---|---|
| **First** | **Pack neighboring curve Y values into the unused RGBA channels of the existing curve texture** | Directly attacks the seven-segment shader's texture-fetch cost with almost no architectural or compatibility risk. A conservative first version reduces eight endpoint fetches to five while preserving the current window exactly. | **Exact** |
| **Second** | **Split the cheap waveform pass from a body-only pass rasterized over disjoint CPU-generated conservative X tiles** | Potentially removes the expensive distance shader from most editor pixels *before the fragment shader runs*, unlike the rejected fragment-level bound test. | **Exact if bounds are conservative** |
| **Third** | **Choose fixed shader variants whose segment neighborhood is derived from stroke support radius and segment spacing rather than always seven** | Gives a mathematical criterion for how many segments can possibly affect a visible body pixel, replacing an empirical neighborhood with an exact visible-support bound. | **Exact for rendered body support** |

A fourth, very low-risk experiment deserves to travel with those: rewrite straight-alpha layer composition as a **premultiplied internal accumulator**, eliminating repeated divide/unpremultiply/re-premultiply cycles, and specialize oscillator/envelope shaders at the program level rather than making a uniform branch part of the monolithic shader. That is mathematically geometry-neutral and fully GLSL 1.20 compatible.

Conversely, I would **not** make a conventional sampled SDF the primary body representation. Valve's production distance-field work demonstrates why: one sampled distance channel is excellent for smooth antialiasing, outlines, and glows, but sharp corners progressively round as field resolution falls; Valve had to use multiple edge-distance channels to recover pointed corners. citeturn16view1turn16view2turn16view3 Your requirement that acute peaks and rock-constrained bends survive exactly makes that a materially different problem.

Throughout this report I use **Documented** for conclusions directly supported by specifications, vendor guidance, or papers, and **Renderer inference** for deductions specifically from Wyrm's shader and geometry.

## Constraints and present baseline

GLSL 1.20 is less restrictive than is sometimes remembered. It formally supports `if`/`else`, `for`, `while`, `do`, `break`, `continue`, `discard`, and `texture2D`; therefore fixed-count distance loops, branchless arithmetic, lookup textures, and multiple shader variants all fit the language level. citeturn17view5turn17view4turn17view6 What GLSL 1.20 does **not** provide is a performance contract for any of those constructs: the implementation decides how flow control is realized on the hardware. NVIDIA documented that older fragment processors could predicate both sides of branches and that later SIMD fragment processors still paid when neighboring fragments diverged. citeturn18view0turn21view0

There is also an important nuance around derivatives. `dFdx` and `dFdy` are in the GLSL 1.20 specification, but that same specification explicitly permits approximate derivative calculations and says derivatives evaluated inside a non-uniform conditional are undefined. Your decision not to depend on derivative behavior for portable AA is therefore defensible even though the functions exist syntactically. citeturn17view3 A screen-pixel AA width derived explicitly from framebuffer size, as the current shader does, is far easier to reason about across the legacy-driver population. fileciteturn0file0

The current texture requirement also needs to be separated from the language requirement. `RGBA32F` is supplied by `GL_ARB_texture_float`, which added 16- and 32-bit floating-point texture components and was specified to work with OpenGL 1.5-era implementations; **GLSL 1.20 by itself does not guarantee the format**. citeturn17view0 Because Wyrm already allocates a one-row `GL_RGBA32F` curve texture, proposals that merely repurpose its unused channels add no new format requirement, whereas proposals requiring another exotic renderable float format would increase compatibility exposure. fileciteturn0file0

The body shader's expensive kernel is straightforward:

1. determine a center segment from fragment X;
2. fetch eight curve points;
3. evaluate seven clamped point-to-segment squared distances;
4. take the minimum;
5. take one square root;
6. convert that single distance into outer, middle, and core coverage;
7. composite those translucent layers and then composite body over waveform. fileciteturn0file0

That architecture is geometrically attractive. A clamped projection onto a real line segment includes the segment endpoints in its distance function, so thresholding minimum segment distance produces the intended endpoint neighborhoods without a separate cap primitive. The body material's three layers all derive from one distance scalar, avoiding the cracks and mismatched joins that independently rasterized strips can produce. The project's own strip experiments exposed precisely those peak/cap differences and led back to analytical screen-space distance. fileciteturn0file1

The main unresolved issue is not therefore “how to stroke a polyline.” It is:

> **How little fragment work can be performed while continuing to evaluate the same exact point-to-polyline distance wherever that distance can affect the final pixel?**

That framing is strongly supported by older analytic-vector work. Loop and Blinn rasterized only triangles covering a curve's Bézier control hull, then ran the implicit curve test in the pixel shader; the expensive analytical representation was thus evaluated only over a geometric region known to contain the curve. citeturn22view0 Qin, McCool, and Kaplan went in another direction: a Voronoi-derived uniform-grid accelerator selected a small constant number of candidate geometric features, after which the shader still computed exact distance to those features; they reported exact reconstruction of contours and sharp features rather than replacing geometry with a sampled approximation. citeturn23view0turn23view1 Both papers point toward the same principle that fits Wyrm unusually well: **accelerate candidate selection or raster domain; do not compromise the final geometric distance test.**

## Analytical distance and exact candidate reduction

The current fullscreen analytical approach should remain the reference renderer against which alternatives are judged. It has very little CPU setup, does not require tessellating joins or caps, naturally shares the curve representation with the waveform, and keeps all layer boundaries mutually coherent because they are functions of the same distance. fileciteturn0file0 Its downside is simply arithmetic multiplicity: at the largest stated editor size, roughly 977×509, a fullscreen body computation can be invoked for almost half a million fragment locations, regardless of whether most are tens or hundreds of pixels from the body.

### Comparative assessment

| Technique | GLSL 1.20 feasibility | CPU/GPU balance | Visual risk | Compatibility risk | Exact required geometry? |
|---|---|---|---|---|---|
| **Current fullscreen seven-segment distance** | Excellent: fixed loops and `texture2D` are standard GLSL 1.20. citeturn17view5turn17view6 | Minimal CPU, maximum fragment work. | Only risk is a true nearest segment lying outside the fixed neighborhood. | Low beyond existing float-texture requirement. | **Conditional:** exact over the tested seven segments; globally exact only when they contain every segment that can matter. |
| **Smaller fixed neighborhood** | Excellent. | Directly reduces texture and ALU cost. | Dangerous if chosen empirically; peaks/folds can select a farther-by-X segment. | Very low. | **No**, if merely “try five.” |
| **Radius-derived fixed neighborhood variants** | Excellent; compile several constant-loop shaders and choose one on CPU. | Tiny CPU decision, potentially meaningful fragment savings. | None if candidate bound is proved conservatively. | Very low; avoids fragile variable-loop behavior. | **Yes, for every pixel where body coverage can be nonzero.** |
| **Dynamic fragment early-exit/break** | Legal GLSL 1.20. citeturn17view4turn17view5 | May save ALU but introduces divergent control flow. | None if lower-bound logic is correct. | Performance varies greatly on old hardware. | Potentially yes. |
| **Packed curve endpoint values** | Excellent; RGBA swizzles and `texture2D` are baseline shader features. | Slightly more CPU packing, fewer texture reads per body fragment. | None if indices are packed with identical clamping semantics. | No new format if existing RGBA32F is retained. | **Yes.** |

The radius-derived neighborhood is particularly interesting because Wyrm's geometry gives a proof that is stronger than “seven seemed sufficient.”

**Renderer inference.** Let

\[
R = \frac{\text{outerWidthPx}}{2}.
\]

The current shader uses

\[
d = \frac{\text{distancePx}}{R}
\]

and the outer mask reaches exactly zero when \(d \ge 1\). Thus, no segment farther than \(R\) can change body output. fileciteturn0file0

For any segment with horizontal interval \([x_0,x_1]\), Euclidean distance obeys

\[
\operatorname{dist}(p,\text{segment})
\ge
\operatorname{dist}(p_x,[x_0,x_1]).
\]

Therefore, if the segment's X interval lies more than \(R\) from `p.x`, that segment **provably cannot contribute nonzero outer-body coverage**, regardless of its Y coordinates or slope.

That matters because Wyrm's `curvePoint()` constructs monotonically and uniformly spaced screen-space X coordinates from the texture index. fileciteturn0file0 If the segment spacing is \(\Delta x\), the maximum relevant index radius is derived from \(R/\Delta x\), with one conservative guard segment to absorb interval/end-point conventions. Instead of saying “always inspect three segments left and three right,” the CPU can compute the required `K` for the current framebuffer and body width, select a `K=3`, `5`, `7`, `9`, etc. shader variant, and know why that variant is safe.

This has two useful consequences.

First, **a smaller K can be exact**, not approximate, when the expanded editor gives sufficiently wide X spacing. Second, the same calculation can reveal the opposite: if a particular zoom/sample-count combination theoretically requires nine candidates, then the existing seven-segment window is already an approximation even though it has looked visually sound so far. That turns an empirical validation concern into a measurable invariant.

I would favor fixed shader variants over a uniform loop limit. The language permits general loops, but legacy GPU implementations had substantially different flow-control behavior, and NVIDIA specifically advised resolving branches/static cases earlier in the pipeline where possible. citeturn18view0turn14search0

### Packing neighboring curve values

The highest-confidence microarchitectural experiment comes from the fact that Wyrm uploads **four floats per curve point but presently uses only the red channel for Y and initializes the other channels largely as padding/alpha**. fileciteturn0file0 NVIDIA's GPU Gems guidance explicitly describes packing adjacent scalar elements into RGBA and arranging values used together so that fewer accesses retrieve more useful data. citeturn14search0 This is unusually applicable here because Wyrm already pays for an RGBA32F texel.

A conservative exact packing is:

```text
texel i:
    R = y[i]
    G = y[clamp(i - 1)]
    B = y[clamp(i - 2)]
    A = y[clamp(i - 3)]
```

The red channel remains `y[i]`, so the waveform's existing linearly filtered `.r` lookup is unchanged. For a body window centered at segment/point index `c`, one texel-center read gives:

```text
lo.rgba = { y[c], y[c-1], y[c-2], y[c-3] }
```

That supplies four of the eight body endpoints. The four forward samples can initially remain ordinary exact-center reads. **The conservative first experiment therefore changes eight body endpoint texture reads into five, with identical point data and identical clamping at both ends.**

There is no new texture object, no additional texture format, no interpolation approximation, no geometry approximation, and effectively no increase in upload size: the existing 16-byte RGBA32F texel is simply made useful. citeturn17view0 CPU packing rises by a few cached vector reads/stores per curve point only when the geometry texture is rebuilt.

If that test wins, a second version can provide the forward quartet in another packed row/texture or use a spatially coherent endpoint fallback so interior pixels recover all eight Y values in **two** texture reads. I would not begin there; the five-read form isolates the value of packing with nearly zero added complexity.

A related safe simplification is to stop reconstructing every endpoint X coordinate through `curvePoint()`. Since X spacing is uniform, pass `xFirstPx`/`segmentDxPx` or equivalent uniforms and derive successive endpoint X values arithmetically. That follows NVIDIA's documented advice to hoist uniform work out of the fragment stage. citeturn14search0 Mathematically it is the same geometry, although floating-point evaluation order may differ by sub-ULP amounts from today's repeated expression.

## Conservative raster domains and transparent work

The most promising architectural change is **not fragment culling; it is fragment non-generation**.

That distinction explains why the previous vertical-rejection experiment can regress without invalidating the larger idea. Wyrm's rejected experiment first launched the fragment shader over the fullscreen quad, fetched/bounded local points, took a branch/discard path, and only then avoided some segment projections. fileciteturn0file1 GLSL's `discard` simply abandons the fragment's buffer updates once shader execution reaches it. citeturn17view4 NVIDIA explicitly distinguished that from raster/depth mechanisms that prevent the expensive fragment program from executing at all, noting that pre-shader rejection can save substantially more work when the rejected region has spatial locality. citeturn18view0

For Wyrm, no depth trick is necessary. The CPU already owns the canonical sampled curve, and its X coordinate is monotonic. A small list of **disjoint conservative rectangles** can bound the body.

For example, divide the editor into fixed X bins, perhaps 16–64 physical pixels wide. For each bin:

1. determine which line segments, expanded horizontally by outer radius \(R\), can reach that bin;
2. take the minimum and maximum Y of their endpoints;
3. expand those Y values by \(R\);
4. add a one-pixel conservative raster guard;
5. emit one rectangle for that X bin.

Because a line segment's Y coordinate is linear, its extrema over its interval occur at its endpoints. Expanding the endpoint range by the stroke radius therefore contains the entire round tubular neighborhood of those segments. The first and last bins must also extend horizontally by \(R\) to contain the endpoint caps. This is a renderer-specific geometric deduction from Wyrm's line-segment distance representation. fileciteturn0file0

Crucially, the rectangles should be **disjoint in X**. Do not draw one translucent quad per segment: those would overlap around joins and repeatedly composite the same body material. A disjoint tile partition invokes the exact body shader at most once for each body-domain pixel, while conservative Y expansion merely shades a few surplus transparent pixels.

This is conceptually aligned with two documented graphics techniques. Loop and Blinn used rasterized control-hull triangles to limit where their analytical curve pixel program ran. citeturn22view0 NVIDIA's conservative-rasterization work explicitly describes the trade between tight bounding geometry and fill rate and notes that moving conservative setup to the CPU can reduce GPU load at the expense of CPU work. citeturn15view0

Hardware conservative rasterization itself should **not** be part of the baseline. `GL_NV_conservative_raster` is a much later optional extension written against OpenGL 4.3-era specifications, even though its purpose includes conservative tile/bin population. citeturn17view2 CPU-created rectangles use only old fixed-function rasterization and the shader facilities you already rely on.

The expected architecture becomes:

```text
Pass A:
    cheap analytical waveform
    one fullscreen quad

Pass B:
    exact analytical body
    one batched set of disjoint conservative rectangles
```

No VBO is required. Your compatibility code can emit the rectangles in one immediate-mode `GL_QUADS` batch, supplying normalized `vUv` coordinates exactly as the current fullscreen quad does. fileciteturn0file0

This is the one circumstance in which I expect **separate passes to have a strong structural advantage over the new combined shader**. With two fullscreen passes, combining removes redundant waveform rasterization. With conservative body tiles, however, the cost models become:

\[
C_\text{combined}
\approx
N_\text{screen}(C_\text{wave}+C_\text{body})
\]

versus

\[
C_\text{split+tiled}
\approx
N_\text{screen}C_\text{wave}
+
N_\text{body-domain}C_\text{body}.
\]

When the polyline tube occupies a small fraction of the panel, \(N_\text{body-domain}\ll N_\text{screen}\). That conclusion is a **renderer-specific inference**, not a vendor benchmark; the CPU tile construction and old-driver draw overhead still have to be measured.

The method preserves acute peaks, bends, round caps, AA, and nested layers because **the rectangles do not define the body at all**. They merely determine where the unchanged exact analytical body shader is allowed to run. The only visual failure mode is a non-conservative bound, which is straightforward to detect with a debug mode that draws the tile union around the reference fullscreen body.

A useful diagnostic metric is therefore not only body GPU time but:

\[
\text{body-domain fraction}
=
\frac{\sum \text{tile raster area}}
     {W H}.
\]

Capture that alongside the existing asynchronous timer data. If it is consistently small, the case for separation becomes very strong.

## Lookup textures and distance-field caching

There are three substantially different ideas hiding under “use a lookup texture,” and their geometric properties differ enough that they should not be conflated.

| Lookup representation | What the shader still computes | Build/update cost | Geometry status | Recommendation |
|---|---|---|---|---|
| **Candidate feature/index grid** | Exact distance to a small selected set of segments | CPU/GPU preprocessing when curve changes | Can be exact | Interesting, but Wyrm's monotonic X gives a cheaper specialization |
| **Nearest-segment ID per pixel/tile** | Exact distance to one segment | Potentially large 2D table generation/upload | Exact only if ID selection itself is exact at required sampling resolution | Low priority |
| **Stored distance/SDF** | Usually only threshold/material evaluation | Full field generation and caching | Sampled approximation unless field is generated/evaluated under stronger conditions | Poor fit for exact peaks/bends |

Qin, McCool, and Kaplan provide the strongest literature precedent for the first category. Their vector-texture system used Voronoi analysis and a uniform grid to identify a small constant number of geometric features, then evaluated **exact** feature distance in the shader, allowing sharp features to survive. citeturn23view0turn23view1 The principle is excellent, but Wyrm has a simpler topology than arbitrary glyphs: the path's X coordinate is monotonic and regularly sampled. A 2D Voronoi accelerator is therefore probably more machinery than necessary. The radius-derived X candidate range is essentially a specialized accelerator with no lookup texture and O(1) CPU setup.

A nearest-segment-ID texture becomes attractive only if the seven projection operations remain dominant after sample packing. A coarse 2D tile could store, for example, two or four candidate indices rather than one ID. The shader would fetch those candidates and perform exact segment distance. That is much safer than storing only “the nearest segment at the tile center,” which can fail when a Voronoi boundary crosses the tile. The exact vector-texture literature supports feature **sets**, not arbitrary single-feature downsampling, for precisely this reason. citeturn23view1

The drawbacks for Wyrm are update cost and encoding. A table that changes whenever Slither moves the curve or a rock changes the path must be rebuilt and uploaded with that geometry revision. The current renderer deliberately treats Slither as a live GL invalidation source, so continuously animated states are exactly where expensive preprocessing has the least opportunity to amortize. fileciteturn0file1 An `RGBA8` candidate texture would be maximally legacy-friendly but constrains index encoding/precision; float candidate IDs preserve larger index ranges but inherit the existing floating-texture compatibility dependency. citeturn17view0

### Conventional signed distance fields

Valve's SIGGRAPH 2007 production technique is highly relevant because it demonstrates both the appeal and the limit of SDFs. A single sampled distance scalar supports antialiased thresholding and allows effects such as outlines and glows to be generated cheaply from the same field. citeturn16view1turn16view2 That maps beautifully onto Wyrm's outer/middle/core material concept.

But Valve also explicitly documented the failure that matters here: **a single low-resolution signed distance field rounds sharp corners as resolution decreases**. Their proposed remedy was to retain multiple edge distances in different channels so intersecting edges could reconstruct a pointed corner. citeturn16view2turn16view3 Thus:

**A simple sampled SDF does not preserve Wyrm's required geometry exactly.**

It can be made visually very good, but “visually very good” and “acute/rock geometry identical to the analytical segment representation” are different acceptance criteria.

GPU distance-field generation under old OpenGL is technically possible through render-to-texture multipasses. `EXT_framebuffer_object`, standardized in the OpenGL 1.x/2.x era, explicitly enables offscreen rendering directly into texture images without the copy implied by `CopyTexSubImage`. citeturn17view1 It is nevertheless an API extension rather than a GLSL-1.20 language guarantee, and float-texture renderability introduces another capability matrix beyond merely sampling an RGBA32F texture.

Jump Flooding is often suggested in this design space, but its original paper describes an **approximation** to a discrete Voronoi diagram/distance transform, not an exact continuous line-segment field. citeturn22view2 There are exact GPU Euclidean distance-transform algorithms—Cao et al.'s Parallel Banding Algorithm explicitly computes exact EDT for a **binary image**—but that exactness concerns distance to a discrete binary image, not exact distance to the original continuous polyline segments. citeturn22view3 Rasterizing a line first and taking its exact pixel-grid EDT therefore still changes the underlying geometric object.

A cached exact full-resolution continuous polyline distance texture could, in principle, be generated by evaluating your current segment distance at every target texel once and then reused. But for Wyrm that mainly moves the same work into a preprocessing stage. When geometry is static, the enclosing `FramebufferWidget` already caches the final rendered result; when Slither is active, the field changes continuously. fileciteturn0file1 The caching opportunity is therefore much smaller than it would be for static glyphs or decals.

My ranking for distance fields is consequently:

**production-quality fallback/effect technology: strong; primary exact dynamic Wyrm body: weak.**

## Material evaluation, branches, and pass composition

The current segment-distance function is already admirably branchless: projection uses `clamp`, and the fixed loop reduces with `min`. fileciteturn0file0 I would not contaminate that kernel with fine-grained per-segment early-outs unless measurement proves a win. NVIDIA's GLSL-era hardware documentation shows why: fragmented SIMD branches can result in both paths executing, while spatially coherent or nearly universally taken/not-taken conditions are much better candidates. citeturn18view0turn21view0 The vertical-bound result you already measured is stronger evidence for this particular GPU than any generic optimization rule. fileciteturn0file1

There are, however, several branch/material changes with unusually favorable risk profiles.

### Accumulate material in premultiplied form internally

The shader's `over()` function currently accepts straight-alpha colors, computes a premultiplied sum, and then divides RGB by the resulting alpha to return straight alpha. The next `over()` call then multiplies that RGB by alpha again. fileciteturn0file0

Algebraically, those repeated normalization cycles are unnecessary.

Represent the intermediate as:

\[
P=(C_rA,\ C_gA,\ C_bA,\ A)
\]

and compose a straight-alpha source \((C_s,a_s)\) with:

\[
P'_{rgb}=C_s a_s+P_{rgb}(1-a_s)
\]

\[
P'_a=a_s+P_a(1-a_s).
\]

Only when the final shader output genuinely needs straight RGB do you perform:

\[
C_\text{out}=
\frac{P_{rgb}}{\max(P_a,\epsilon)}.
\]

That reduces potentially several dependent divisions to one final reciprocal/divide. It preserves the same Porter-Duff “over” result mathematically; differences are limited to ordinary floating-point reassociation. This is **renderer-specific algebra**, not a speculative visual approximation.

It is fully GLSL 1.20 compatible and requires no additional texture or API feature. CPU work is zero, or slightly less GPU arithmetic still if the constant layer RGB×base-alpha products are passed pre-premultiplied as uniforms.

### Do not make every branch branchless

Some existing branches are much less suspicious than others.

`uEnvelope` is uniform over the complete draw. The best legacy-friendly treatment is not necessarily `mix()`; it is to compile **oscillator and envelope shader variants** and choose one on the CPU. NVIDIA's documented “static branch resolution” recommendation is to move invariant decisions out of the fragment program and rasterize with a specialized program when possible. citeturn18view0turn14search0 This also gives the legacy compiler less dead state to carry.

The positive/negative oscillator branch changes only across a horizontal line at `y=0.5`, so it has excellent spatial coherence. Vendor guidance suggests coherent branches are precisely the cases least likely to suffer severely. citeturn18view0 I would leave it alone until profiling says otherwise.

The alternating-column branch is more questionable because its condition can change frequently in X. A branchless alpha multiplier such as a `step`/`mod`-derived shade mask is worth A/B testing after the larger changes, but it is not automatically cheaper: predication may already be what the compiler emits, and unconditional shade arithmetic can replace branch divergence with guaranteed work. NVIDIA explicitly warns that such source transformations must be benchmarked. citeturn14search0

### Combined versus separate passes

For the **current fullscreen/fullscreen architecture**, combining is rational.

The former arrangement effectively costs:

```text
all screen pixels × waveform shader
+
all screen pixels × seven-segment body shader
```

whereas the combined path costs:

```text
all screen pixels × combined shader
```

and can share phase, Y, column, size-related terms, and some common state. Your code now follows that approach, composing waveform followed by body in the same shader. fileciteturn0file0

But there is a documented countervailing effect on legacy-class hardware: larger shaders can require more temporary registers, and NVIDIA's GeForce 6 description explains that increased register use can reduce the number of fragments kept in flight and therefore reduce the ability to hide texture-fetch latency. It also specifically cautions that shader length matters when a long fragment shader is evaluated over a full screen. citeturn21view0turn21view1turn21view2 This does **not** mean the combined shader is slower; it means the gain must be measured rather than assumed.

The same-size A/B test currently pending in your plan is therefore exactly the right experiment. fileciteturn0file1 Measure combined and split at identical framebuffer dimensions, curve revision, envelope state, and Slither state, because your earlier captures already demonstrated session/size effects large enough to obscure modest optimizations. fileciteturn0file1

More importantly, the answer can change after body-domain rasterization:

| Architecture | Expensive body evaluated where? | Likely role |
|---|---:|---|
| Separate fullscreen wave + fullscreen body | Entire panel | Baseline/reference |
| Combined fullscreen wave/body | Entire panel | Likely best of the **fullscreen** choices |
| Fullscreen wave + conservatively tiled body | Only near body | Most promising structural optimization |
| Fullscreen combined + fragment body rejection | Entire shader starts everywhere | Least attractive given existing rejection result |

All three first architectures can preserve exactly the same visual result. The choice is about where fragment work executes, not stroke geometry.

## Ranked experiments and decision matrix

The final ranking deliberately favors experiments that either preserve the analytical geometry outright or have a simple mathematical proof of conservative equivalence. That matches the renderer's history: strip-based shortcuts saved conceptual work but reintroduced precisely the peak and cap differences the analytical distance implementation was created to eliminate. fileciteturn0file1

| Priority | Experiment | Expected GPU effect | CPU effect | Visual/geometry risk | GLSL 1.20 / compatibility |
|---|---|---|---|---|---|
| **First** | **Pack backward neighboring Y samples into G/B/A; use one packed read + four forward reads** | Reduces body endpoint texture fetches from 8 to 5; also permits vector/swizzle use. Potential gain depends on whether texture latency/bandwidth is limiting. | Negligible O(N) packing when curve texture changes; same texture allocation size. | **None. Exact current Y samples and clamp semantics.** | Excellent. Same sampler, same RGBA32F texture, same `.r` waveform representation. |
| **Second** | **Separate waveform/body and rasterize body over disjoint conservative X tiles** | Can reduce expensive body fragment invocations from full editor area toward the actual curve tube's conservative area. | O(N+tiles) bounds whenever geometry changes; one small immediate-mode rectangle batch. | **None if every tile is conservative.** Debug/reference fullscreen path can prove it pixel-for-pixel. | Excellent; no VBO, geometry shader, derivative, or hardware conservative-raster extension required. |
| **Third** | **Select fixed K-segment shader from \(R/\Delta x\)** | Reduces projections and corresponding endpoint reads wherever fewer than seven candidates are mathematically sufficient. Also detects configurations where seven is insufficient. | Tiny per-size/material calculation and shader-program selection. | **None when K follows the horizontal-distance support proof.** | Excellent; fixed loops are safer than relying on efficient dynamic flow control. |
| Next | Premultiplied internal material accumulator | Removes repeated RGB normalization/divisions during nested compositing. | None. | No geometric risk; only FP reassociation. | Excellent. |
| Next | Envelope/oscillator shader specialization | Removes a uniform mode branch and dead mode-specific work/state. | One additional program and CPU selection. | Exact. | Excellent; follows static branch-resolution guidance. |
| Later | Exact feature-list lookup texture | Could reduce candidate segment tests substantially. | Preprocessing/upload when curve changes. | Exact only when candidate list construction is conservative. | Feasible, but added texture/data complexity. |
| Later | Nearest-segment ID texture | Potentially one distance test per fragment. | Potentially large 2D update. | Single coarse ID can be wrong around feature boundaries. | Feasible but encoding/filtering require care. |
| Low | Conventional sampled SDF | Very cheap final material shader. | Field generation/cache overhead. | **Not exact:** sharp features round under finite sampling unless augmented. | Rendering easy; generation may require FBO/float capability. |
| Low | Jump-flooded SDF | Fast parallel approximate field generation in suitable GPU architectures. | Several render-to-texture passes/setup. | JFA itself is approximate, and raster seeds do not represent exact continuous segments. citeturn22view2 | Poor fit for minimal GLSL-1.20 baseline. |
| Avoid | More fragment-level Y-bound/discard logic | Sometimes avoids seven projections. | None. | Geometry can be conservative, but performance already regressed locally. | Legal GLSL 1.20 but hardware-sensitive. |

For the first experiment, I would keep the test deliberately surgical:

```glsl
// Conceptual packing:
// curve texel c = { y[c], y[c-1], y[c-2], y[c-3] }

vec4 back = sampleCurveTexel(centerIndex);

float y0 = back.a; // c - 3
float y1 = back.b; // c - 2
float y2 = back.g; // c - 1
float y3 = back.r; // c

// Preserve existing exact clamped fetches for c+1..c+4 in experiment A.
float y4 = sampleY(centerIndex + 1.0);
float y5 = sampleY(centerIndex + 2.0);
float y6 = sampleY(centerIndex + 3.0);
float y7 = sampleY(centerIndex + 4.0);
```

That experiment changes almost nothing except fetch count. NVIDIA's period-appropriate optimization literature specifically recommends application-specific RGBA packing of adjacent/repeated scalar data when values will be consumed together, while also warning that an extra lookup can lose when it merely trades cheap arithmetic for bandwidth. Here the unusual advantage is that **there is no extra lookup and no larger current texel**: three already-paid channels become useful. citeturn14search0

For the second experiment, retain the existing fullscreen analytical body as a debug oracle. Render a binary overlay of the proposed conservative tile union and assert visually or in a diagnostic readback that every nonzero reference-body pixel lies inside a tile. Once that invariant is established, the body fragment program itself need not change. NVIDIA's conservative-raster literature emphasizes exactly this separation between an overestimated domain and an exact later test: conservative coverage is allowed to include excess work, but must never exclude a potentially relevant sample. citeturn15view0

For the third experiment, record the selected K in the timing CSV. The interesting result is not simply “K=5 was faster.” It is the scaling relation between framebuffer width, curve texture count, outer body radius, and resulting mathematically required candidate count. If the panel expands while the curve sample spacing expands too, K may fall and the analytical body becomes cheaper per fragment exactly when the total fragment count rises. If the sample count instead scales proportionally with editor width, K may remain approximately constant. That distinction is renderer-specific and should emerge directly from your existing geometry policy rather than from guesswork. fileciteturn0file0turn0file1

The decisive architectural fork after those experiments is therefore:

> **If packed samples and adaptive K make the fullscreen body cheap enough, keep the combined fullscreen pass. If the body remains dominant, separate it again—not to return to strips, but so the rasterizer can restrict the exact analytical shader to conservative body tiles.**

That preserves the essential achievement of the current SHDR design: the body remains an exact screen-space distance problem with one continuous distance scalar driving round endpoints, acute centerline geometry, antialiasing, and all nested translucent material layers. What changes is merely how many fragments are invited to solve that problem.