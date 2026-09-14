# Eclipse2 hybrid-cap investigation

An offline comparison of Eclipse2's SVG renderer with the retained-cap hybrid.
The candidate now calls the same `Eclipse2RetainedCap.hpp` layer used by the plugin.
Integral Flux's debug menu exposes `Eclipse2 retained cap A/B (all modules)`.
It is session-only and off by default; checked selects the raster cap globally.

## Ring experiments and live cache

`make -j10 build/tools/eclipse2/ring` builds the native ring comparison. Run
`PATH="/c/Program Files/VCV/Rack2Pro:$PATH" build/tools/eclipse2/ring.exe` from
the repository. It compares baseline, precomputed LED positions, positions plus
shared bloom masks, the production cached-ring candidate, a procedural GLSL ring,
and batched preparation of four GLSL rings. All shader/FBO preparation is inside
the measured interval; results include outer framebuffer rebuild and presentation.

The procedural shader preserves the 25 round LEDs, bipolar activation, warm track
and layered glow. It uses signed-distance coverage and nearest-LED evaluation,
premultiplied composition, AdaptiveGlSurface and GL resource retirement. The
shader sources remain **offline only** in this directory. Initial nested shader
medians were about 352-396 us for four rings versus 148-159 us baseline; batching
helped to 299-308 us but did not win. Adding another framebuffer pass dominates
this version. These results do not establish an advantage for a whole-knob or
shared-atlas shader architecture; those would be separate experiments.

Geometry caching was pixel-identical but had no consistent timing advantage by
itself. Glow masks alone also showed no consistent whole-path win. Caching the
four static track strokes along with those changes was the useful combination:
the initial probe measured 114-118 us for four changing rings. Tail latency was
noisy and sometimes worse; this is a median CPU improvement, not an FPS claim.
The shadow was already only about 6.7 us per four knobs in component screening,
so it remains unchanged rather than adding another retained texture for it.

The production candidate is `src/visual/Eclipse2RingCache.hpp`, selectable through
Integral Flux Debug > `Eclipse2 cached ring A/B (all modules)`. It starts off,
requires debug mode, and is independent of the cap switch. Its static track is
136x136 top-down premultiplied RGBA8 baked once at runtime and retained in memory.
The image cache is bounded to eight independent NanoVG contexts and uses the
shared ownership helpers; glow masks reuse the existing aperture mask helper.
Unsupported sizes, LED counts and arc ranges fall back to the original renderer.
Image failures also retain the original draw. Toggle changes dirty idle knob
framebuffers. All rendering remains in Draw, preserving module/component metrics.

The runtime track and baseline share `src/visual/Eclipse2Track.hpp`, so edits to
track drawing update both automatically. The probe still emits a reference bake
under ignored `build/` and checks the runtime pixels against it; no RGBA assets
are packaged. The live cached ring is called directly for the combined candidate.

The 200 ring image cases cover five zooms through 3x, five values, four bloom
levels including off, and both unipolar/bipolar modes. Initial maximum channel
differences were 0 for geometry, 33/255 for masks, 31/255 for the static combination,
and 190/255 for the shader. These are not exact-pixel replacements. Additional
live-cache checks cover default/debug gating, fallback pixel parity under opacity,
asset parity, independent context images, invalid-handle recreation and context
events. Visual acceptance in Rack/DAW remains a live A/B evaluation.

## Finding

Eclipse2 places its shadow, LED ring and rotating SVG layer inside one outer
framebuffer. The SVG layer has its own framebuffer, but nested framebuffer
rendering bypasses it when the outer knob image is rebuilt. As a result, changing
the knob repeatedly submits the SVG geometry. Keeping the cap as an image avoids
that work while retaining the existing NanoVG shadow and ring.

This reuses the raster-cap part of HaloKnob2's hybrid architecture. It does not
port HaloKnob2's procedural GL ring shader or AdaptiveGlSurface to Eclipse2.
The measured bottleneck is large enough to tackle the cap first.

Screening run, four changing 34-pixel knobs, native Windows Rack 2.6.6 runtime:

| Component | CPU submission median range |
| --- | --- |
| Shadow | About 6.7 us |
| 25-LED ring | About 102-112 us |
| SVG cap | About 1022-1026 us |
| Raster cap | About 1 us |
| Complete dirty framebuffer rebuild + presentation, original | 1188.5-1198.2 us |
| Complete dirty framebuffer rebuild + presentation, raster cap | 137.5-166.9 us |

Component timing uses 100 warmup frames and 600 measured frames, three rotated
condition orders. Complete-framebuffer timing uses 100 warmup frames and 300
measured frames, three alternating orders, four independent framebuffer images,
and includes CPU NanoVG flush work. It uses real Rack framebuffers and explicitly
updates each dirty knob before presenting it. The minimal fixture disables
subpixel invalidation and uses integer screen positions for this timing test.
It does not reproduce the live Rack frame-budget scheduler. Parameter state and
scene traversal are not timed. Shader/GPU completion timings are not measured.
Both candidates retain cached-image presentation when idle; no idle gain is claimed.
The reproduction run confirmed similar medians (1192-1203 us original versus
140-157 us hybrid), but one hybrid p95 reached 1852 us versus 1451 us in its
baseline pair. The median improvement does not establish better worst-case tails.

## Appearance

Directly reusing HaloKnob2's NanoSVG CPU rasterization noticeably changes the cap's
gradient shading. The retained prototype instead bakes the exact production SVG
widget with Rack's NanoVG renderer at the knob's logical size (34 pixels), using
four raster samples per logical pixel. The premultiplied result receives mipmaps
before being rotated and presented as an image. Baking/readback is done before
the timing loop and is not a production preparation/lifecycle implementation.

72 image comparisons cover six zoom scales (0.75, 1, 1.19, 1.5, 2, 3), six knob
values, full/half opacity and fractional placement. The largest channel error was
95/255, with maximum mean error approximately 2.11/255 averaged over the knob's
projected rectangular area. At 3x/full opacity, per-angle maxima were 35-46/255
and average errors about 0.62-0.68/255. These differences are not acceptable as an
exact-pixel replacement claim. Resampling changes edge coverage; visual assessment
is needed before enabling this in production. The image probe currently compares
direct components, not the final outer framebuffer route under every transform.

The experiment emits raw 256x128 RGBA snapshots (bottom-up) under `build/` for a
representative 3x comparison. Timing/image logs also remain under ignored `build/`.
No shader, first-use latency, arbitrary SVG replacement, resized controls, other
LED counts, bipolar mode, or context-recreation integration is validated here.

## Reproduce

From native MINGW64 in the repository:

```sh
make -j10 build/tools/eclipse2/components
PATH="/c/Program Files/VCV/Rack2Pro:$PATH" build/tools/eclipse2/components.exe
```

The generator extracts the exact production cap, ring and shadow methods into
ignored `build/tools/eclipse2/reference.inc`; the reference is not a manually
maintained rendering copy. The target is separate from the plugin and test-fast.

## Live integration and baked asset

The plugin no longer ships the cap or track RGBA files. Each is baked once on
first enabled use with Rack's NanoVG SVG/track drawing, at 136x136 (4x logical
size). The cap bake uses the loaded default SVG and its 0.70 scale. CPU pixels
remain shared for the plugin lifetime, including across window/context recreation.
GPU images are still separate, validated and rebuilt per context with mipmaps.

`Eclipse2RuntimeBake.cpp` uses a temporary private NanoVG recorder inside the shared
AdaptiveGlSurface state guard. This allows first use during an active host draw,
including a nested framebuffer, without disturbing its command recording. Temporary
GL resources are released immediately; no destructor performs GL work. First-use
baking and readback stay inside Draw, including existing module/component telemetry.
A failed bake falls back to SVG and retries after context creation. Unavailable
host graphics do not consume the attempt. Idle drawing does not repeat the bake.

The native probes compare runtime pixels exactly with their independently rendered
reference pixels. They also check one-time preparation across context events and
pixel-identical uninterrupted main/nested host recordings when a cold bake is
inserted between host draws, including inherited transform, opacity, scissor and
non-default pixel-pack state. Both probes run without any `res` RGBA files.
Initial measured preparation was about 5.5 ms for the cap and 3.4 ms for the track;
these are first-use CPU/driver observations, not steady-state costs.

## Further work

Use an opt-in cap-image path with the original SVG as fallback. Prepare the
context-owned cap outside active nested framebuffer rendering, preserve arbitrary
SVG/size behavior, and use shared graphics lifecycle/retirement helpers for both
main and framebuffer NanoVG contexts. Include any step-time preparation in the
knob/module rendering metrics rather than reporting only the cheap image draw.
Keep macro Process/Step/Draw meanings unchanged. Measure cold creation, changing
and idle knobs, zoom/DPI, bipolar behavior and DAW reopen before changing defaults.
Only then decide whether the remaining ring cost warrants a dedicated procedural
shader; Eclipse2's round LEDs differ from HaloKnob2's segmented ring.
