# Integral Flux round-stroke live pilot

Status: historical failed live experiment. Its runtime hook, setting reader and
normal-build linkage were removed during cleanup on 10 September. Offline source
and tests remain in `tools/experiments/lumin`. The local preference was set false;
an already installed experimental binary reads that setting on its next launch.
The following setup describes the historical run, not the current source build.

Local test setup: `Leviathan-2.9.1-win-x64.vcvplugin` was copied to Rack's plugin
directory and `IntegralFluxRoundStroke=true` was written into the existing user
preferences. Rack was closed during installation. The package takes effect on
the next launch. [Installation hashes and validation](benchmarks/flux-live-round-stroke-20260910/installation.json)
are preserved locally; the prior preferences were backed up in `work/`.

## Historical enable and compare procedure

Use the same installed build and patch for both configurations. In Rack's
`Leviathan/dragonking.txt`, keep `debug`, `IntegralFluxDrawLogging`, and
`IntegralFluxLumenPreview` true. Set `IntegralFluxRoundStroke` false for A and
true for B. Restart Rack between configurations: the setting is read at plugin
initialization and fixed when the widget is constructed. It is not serialized
into patches and does not change parameter IDs or saved renderer modes.

The prior Lumen flag enables the ownership adapter; it does not enable this
optimization. The new CSV suffix makes the distinction explicit:

- `round_stroke_enabled`: requested experimental configuration.
- `round_stroke_private_draws`: actual private current-contour presentations.
- `round_stroke_fallbacks`: requested contour draws handled by legacy drawing.
- `round_stroke_surface_us`: CPU preparation, private surface rendering and
  presentation submission, including attempted fallbacks. Nested inside contour,
  preview and module timing; never add these timings together.

Keep existing `Process`, `Step`, and `Draw` meanings. Other CSV columns retain
their ordering; the new fields are appended. Old captures lack these fields and
remain readable as unavailable, not zero. During steady modulation expect
nonzero private draws in B. An enabled flag alone does not establish that the
private path ran. Once settled, existing contour caching is reused and those
counters normally go to zero.

## Integration

The current contour alone uses a private, prefixed NanoVG implementation on an
existing managed `AdaptiveGlSurface`. Its endpoint fast path was tested against
Rack's full stroke output offline. History, marker, label, controls, legacy
pruning and the 100 ms settlement policy are unchanged. Highlighted rise/fall
segments retain their original separate colors and endpoint behavior.

The surface renders at current effective pixel density with a one-pixel margin
and fractional screen-position alignment. Presentation uses exact density rather
than stretching a rounded-up texture extent; host composition applies inherited
opacity and clipping. Front/back surfaces retain peak capacity until context
reset. This adds rasterization, texture composition and memory overhead, so the
earlier offline CPU gain is not promised as the live gain.

Only the ordinary positive, uniform, unrotated editor path at effective densities
0.25–8 is admitted. Nested framebuffer captures, other NanoVG contexts, rotated
or unsupported transforms, browser/null-module previews, allocation failure and
repeat draws in the same host frame retain the legacy path. Per-frame admission
prevents overwriting an image already queued for host presentation.

Private GL changes occur within the shared surface state guard. Shader, buffer,
texture and framebuffer cleanup uses the existing context-lease retirement
service, including destruction without a current GL context. Context events reset
the private surface; reused NVG pointers cannot revive abandoned resources.

## Validation

Native `plugin.dll` linking passed. The actual experimental surface passed 30
composed-image checks at five densities, three fractional offsets and two opacity
levels with clipping active: maximum per-channel difference 1/255, maximum
alpha-normalized aggregate error 0.1854%. The 1% / 32-byte integration screening
limits were set in the test before its first run. These results include the
additional image-composition pass and differ from the prior pixel-identical
direct-render fixture.

The same test verifies framebuffer restoration, repeat-frame/nested/rotation
fallbacks, GPU-object retirement without a current context, retirement draining,
and resource recreation after context destruction with the same NVG pointer.
The existing GL surface/lifecycle test passed 549 checks. The legacy adapter test,
all nine Flux runtime checks, and all eight CSV analyzer tests passed. The known
unrelated full-fast-suite theme compilation issue was not rerun.

## Live capture

### First enabled run — 10 September 2026

Capture `integral_flux_draw_1_20260910_074152_0.csv` contains 2,776 complete
rows. `round_stroke_enabled=1`, with 2,385 actual private presentations and four
fallbacks across the run. The [saved summary](benchmarks/flux-live-round-stroke-20260910/summary.json)
preserves the source hash, exact selections, settings and limitations.

Post-hoc selections are steady [250,750), modulated [1000,2000), and settled
[2250,2776). During modulation there are two private presentations per row and
no fallbacks: 1,399 point rebuilds and 682 accepted captures/history rasterizations,
close to the prior baseline's 1,408 and 702 over its 1,000-row active selection.
Neither active interval contains knob surface work. Shape changes start at row
824 and end at 2008; the final private presentation is at 2012.

| Active CPU timing | Prior 07:07:47 median / p95 | Enabled 07:41:52 median / p95 |
|---|---|---|
| Module widget draw | 300.80 / 452.89 us | 758.70 / 1133.05 us |
| Preview draw | 63.75 / 120.93 us | 539.05 / 776.91 us |
| Current contour | 18.90 / 26.00 us | 484.05 / 693.32 us |
| History draw | 16.90 / 69.00 us | 17.70 / 73.23 us |
| Private surface (nested) | unavailable | 482.40 / 691.21 us |

The enabled preview median is 8.46x the previous capture; module widget draw is
2.52x. Quiet preview medians are similar (35.8 versus 35.4 us), with zero private
work. Settled preview is 35.9 us versus 20.5 us previously, also with zero private
work; this difference is not evidence of active private-path cost.

The private surface scope dominates the current-contour timing. It combines
preparation, state handling, private rendering and composition submission; this
capture does not isolate which part is responsible or measure GPU/frame time.
This is a similar-workload comparison, not a matched same-build A/B; exact shape,
zoom/DPI, power/thermal and audio conditions were not recorded. Nevertheless,
the observed integration cost fails the live gate. Do not promote this path.
The next [custom shader prototype](flux-shader-candidate.md) follows the user's
chosen direction and introduces an empty-surface control. It confirms substantial
common surface cost even without drawing lines; its visual/performance gates
also remain unmet. Preserve the successful offline math candidate separately.

Run the familiar quiet / independent rise-or-fall modulation / stopped-and-settled
sequence with history enabled. Keep patch, zoom, DPI, power conditions and audio
settings fixed. Check normal zoom, a zoom transition, highlight interaction and
window reopen separately. Preserve cold startup separately from warm intervals.
Compare macro Draw/Step alongside current contour and new surface timings. A
regression or visual issue is grounds to return the flag to false; no production
default should change based on a single capture.
