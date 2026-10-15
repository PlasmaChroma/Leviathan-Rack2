# Aperture rendering candidates

## Default shared bloom renderer

Shared bloom masks are enabled by default across all shared ApertureLight widgets, including outside debug mode.
With Dragon King debug enabled, right-click an Integral Flux module and find
`Integral Flux Debug` -> `Shared aperture bloom masks (all modules)`.
The toggle applies to shared ApertureLight widgets across modules for this process
only; it is not saved in patches or plugin preferences. Toggle off for the baseline while debugging. Outside debug mode the optimized default always applies.

The adjacent `Bloom:` label snapshots the previous observed UI frame's aggregate
aperture bloom CPU microseconds, mask draw count / total bloom calls (including
off lights), and allocated mask slots. Reopen the menu to refresh the snapshot.
This is separate from Flux's existing normal-pass aperture timing and does not
alter module-level Process/Step/Draw metrics. Timing is debug-gated and active
for both baseline and optimized rendering for comparable readings.

Only changing blooms use masks; the original 100 ms settlement still collapses
steady blooms into a per-light image. Tint/alpha, nested framebuffers, rotated or
nonuniform transforms, and effective pixel density outside 0.75-2 use the original
renderer. Core, highlight and socket rendering remain unchanged.

Unlike the offline candidate's FBO masks, the live renderer generates white radial
RGBA images once on the CPU and uploads them through the shared NanoVG lifecycle
helper. This avoids framebuffer switches during an active host frame. Cache limits
are eight independent NanoVG contexts and 32 radius pairs per context; exceeding
either limit falls back without evicting images queued for drawing. Images are
validated before reuse and released through context-destroy events. CPU square
roots occur only during one-time mask creation, not steady rendering.

Native `aperture_bloom_pilot_spec` exercises the full production widget path:
1,296 reference/pilot image comparisons at 1x/2x DPI, six scales, four sizes,
three brightnesses, tint/alpha, clipping and fractional placement; maximum
channel difference 2/255. It also checks startup/non-debug defaults and the debug baseline override, nested/rotated fallback,
settlement, bloom-off, invalid images, cache capacity, alternating NanoVG contexts,
and context destroy/recreate. A full-widget eight-light changing-source probe
measured 24.1-24.2 us baseline versus 17.4-17.5 us pilot CPU submission. These remain
offline results; real Rack/DAW appearance, window reopen, first-use cost and live
performance remain distinct from the offline measurements. The user reported no visible or lifecycle problems during the live pilot before promotion. No measured live Rack speedup is claimed.

```sh
make -j10 build/tests/aperture_bloom_pilot_spec
PATH="/c/Program Files/VCV/Rack2Pro:$PATH" build/tests/aperture_bloom_pilot_spec.exe
```

## Original offline prototypes

Explicit offline experiment. Nothing here is compiled into the plugin or enabled
in live Rack. Uses Rack 2.6.6's real NanoVG backend with the minimal window fixture
used by the aperture settling tests.

From native MINGW64 in the repository:

```sh
make -j10 build/tests/aperture_candidates
PATH="/c/Program Files/VCV/Rack2Pro:$PATH" build/tests/aperture_candidates.exe
```

The target is not part of `test-fast`. A nonzero exit indicates a fixture failure,
GL error, or a retained-geometry/bloom-only image difference exceeding 6/255.
Highlight-mask differences are reported without failing: that candidate is known
to need more work.

## Candidates

| Mode | Work measured |
| --- | --- |
| 0 | Production direct core, highlight and bloom |
| 1 | Shared masks for bloom and highlight; procedural core |
| 2 | Retained geometry for all five dynamic circles |
| 3 | Eight independent static socket framebuffer images |
| 4 | The same eight presentations referencing one socket image |
| 5 | Shared bloom masks only; procedural core and highlight |
| 6 | Shared highlight mask only; procedural core and bloom |

Masks are white radial gradients rendered once at four samples per logical pixel
into padded images. Image paint supplies the current color and alpha. Outer bloom
and inner bloom remain separate draws, preserving their overlap order. Each mask
is shared by radius pair within this one-context fixture. This is a shared image
cache, not yet an atlas, and brightness is continuous rather than quantized.

Retained geometry captures and copies the host's circle fill and AA vertices on
first use. Later draws submit a cheap rectangle solely to obtain the host's current
paint, composite, scissor and fringe callback state, replace its geometry with the
retained circle, and let the original backend enqueue it. It preserves the host
queue; it creates no private GL renderer. The probe updates vertex translation
when an identical circle is reused at another position. This callback ABI is
explicitly restricted to Rack 2.6.6.

The socket experiment holds eight real independent caches for its baseline, then
references only the first image for the shared condition. It measures presentation
and verifies identical pixels for eight identical Small teal sockets at 1x. It
does not implement a production cache manager or measure allocation savings.

## Image and timing scope

432 reference scenes cover four light sizes, four uniform scales (0.75, 1, 1.5,
2), three brightnesses, three fractional placements, clipping, full opacity,
half opacity and an RGB/alpha tint. Colors use a fixed three-channel mix. Each
candidate is compared with direct production component drawing over an opaque
panel-colored destination. These tests do not compare against the full widget's
grouped framebuffer path under inherited tint/alpha; retain that production
fallback until it is separately validated.

Timing uses eight copies of changing Small lights, four rotated condition orders,
100 warmup frames and 1000 measured frames. State refresh is outside timing.
Dynamic timing excludes static sockets, widget traversal, and settlement logic.
`flush8` includes CPU execution of `nvgEndFrame`, not GPU completion. Cache creation
is outside steady measurements. First-frame latency, GPU timings, and live Rack
frame rate remain unmeasured.

Final screening run, September 12, 2026:

| Candidate | Drawing median range, eight lights | Maximum channel error |
| --- | --- | --- |
| Production direct dynamic | 19.2 us | Reference |
| Bloom + highlight masks | 10.9-16.7 us | 64/255 |
| Retained dynamic geometry | 8.5-13.0 us | 1/255 |
| Bloom masks only | 12.2 us | 2/255 |
| Highlight mask only | 17.9 us | 64/255 |
| Independent sockets | 1.9-3.0 us | Reference |
| Shared socket | 1.8-2.5 us | 0/255, narrow socket test |

Earlier runs also favored dynamic masks and retained geometry, with varying
absolute timings. These figures describe this fixture, not an expected live
module-wide speedup. Raw logs are in ignored `build/aperture-candidates.log`.

## Integration work still needed

Bloom-only masks are the simplest promising candidate. Keep the tiny highlight
procedural: the supersampled mask changes its subpixel appearance substantially.
Retained circles offer greater measured CPU savings but need a production wrapper
with scoped callback restoration, reentrancy protection, bounded caches and
fallbacks. The current geometry key admits only positive, axis-aligned uniform
scale at fixed device density with AA enabled. Varying device density/AA, rotated
or nonuniform transforms, nested framebuffer rendering, context replacement and
extreme zoom are not covered by this prototype.

Mask images are created and destroyed explicitly inside one live context; this
fixture does not implement context-switching ownership or lifetime reuse. Before
shipping, use the shared NanoVG lifecycle helpers, include all appearance and
resolution inputs in bounded cache keys, validate image ownership, and rebuild
lazily after context changes. Socket sharing also needs color/geometry/zoom keys
and per-instance invalidation semantics. The resource-sharing rationale is
stronger than the small/noisy steady-state socket timing result.

Live instrumentation should add a separate aperture bloom metric: Flux's existing
`aperture_draw_us` covers the normal pass only. Preserve existing module-level
`Process`, `Step`, and `Draw` meanings.
