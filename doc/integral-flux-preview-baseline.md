# Integral Flux preview baseline — 2026-09-05

Captured logs begin at filename timestamp 20:59:54. Two instances, 1,536 complete
rows each. Original CSV snapshots and SHA-256 digests are preserved under
[benchmarks/integral-flux-baseline](benchmarks/integral-flux-baseline/summary.json).
These are draw-call CPU timings, not GPU completion or application frame times.
The CSV has no per-row timestamps, zoom, curve parameters or marker-visibility
fields, so duration and exact workload cannot be reconstructed from it alone.

Both instances: preview_render_mode=0 (NanoVG), tracer enabled, tracer_mode=0
(stock curve cache), halo_nanovg_forced=0. No recorded knob dragging. This is a
baseline of the existing NanoVG path, not the optional combined GL framebuffer.

| CPU timing, µs | Instance 1 median | Instance 1 p95 | Instance 2 median | Instance 2 p95 |
| --- | ---: | ---: | ---: | ---: |
| ModuleWidget subtree | 135.3 | 172.7 | 217.2 | 272.1 |
| Total instrumented draw | 142.65 | 181.7 | 222.7 | 279.5 |
| Both previews | 28.0 | 34.0 | 110.8 | 129.7 |
| CH1 preview | 14.3 | 17.9 | 48.15 | 65.1 |
| CH4 preview | 13.4 | 16.4 | 60.6 | 71.0 |
| Preview framebuffer | 0 | 0 | 0 | 0 |

Median within-row preview share of ModuleWidget subtree time: 20.8% for instance
1 and 50.3% for instance 2. Preview time includes waveform, tracer, marker and
frequency label. It is not the potential saving from contour caching alone.
Per-channel timings are nested within preview timings, which are nested within
module timing; do not add them as separate costs.

Large total-draw outliers (up to 22.5 ms) cluster around rows 10–16. Preview
durations stay comparatively small in those rows; other visual components show
large spikes. This suggests initial visual/cache work, but the log does not prove
the underlying cause. Means are inflated by these outliers. Excluding the first
60 rows leaves median preview times at 28.0 / 111.1 µs, so the difference between
the two instances persists beyond startup.

## Counter attribution limitation

Instance 1 reports 2,222 rebuilds and 2,218 tracer captures; instance 2 reports
zero. These cannot be interpreted as per-instance activity. The step counters in
IntegralFluxWidget.cpp are thread-local globals shared by all Flux instances,
incremented in preview step(), then cleared after each module draw. The first
drawn module can consume counts accumulated across multiple instances. Also,
the capture counter increments after every capture call, even when throttling
rejects it: it counts attempts, not accepted captures.

The timing counters are reset immediately before each ModuleWidget::draw(), so
the two previews' draw timings are still useful per-module baseline measurements.
Before using rebuild/capture counts to verify invalidation, fix their ownership
and distinguish attempts from successful captures.

## Next comparison

Keep both instances, patch settings, zoom, tracer mode, debug settings and capture
procedure consistent. Compare the same median/p95 preview and subtree timings
after the geometry-key and settled-contour changes. Preserve live marker/history
updates. More detailed contour/history/label timing would clarify the expected
savings, especially for the more expensive second instance.

No rendering or telemetry implementation was changed while inspecting this baseline.

## Logging-only correction for the next capture

Step diagnostics now belong to each IntegralFlux instance and are consumed only
by that module's draw. This fixes attribution for preview dirty requests,
point rebuilds, capture attempts, linear-point dirty requests and glyph dirty
requests. Collection is gated by Dragon King debug mode.

The existing `preview_tracer_captures` column retains its historical meaning of
**attempts**. A new column, `preview_tracer_accepted_captures`, is appended and
counts only successful captures. Existing column order and draw timing scopes
are preserved. No renderer, geometry invalidation, DSP or saved-state changes.

Native Windows plugin build and all nine Integral Flux runtime checks passed,
including a two-instance consumption test with a real throttled tracer capture.
The next capture should establish the corrected-counter baseline before any
rendering optimization is applied.

## Corrected capture — 21:06:48

Two complete 1,399-row logs contain the new accepted-capture column. Snapshots
and hashes are in [corrected baseline summary](benchmarks/integral-flux-corrected-baseline/summary.json).
Both still use NanoVG and stock tracing, with no logged knob dragging.

| Measurement | Instance 1 | Instance 2 |
| --- | ---: | ---: |
| Preview median / p95, µs | 29.5 / 35.6 | 113.5 / 129.9 |
| Module subtree median / p95, µs | 161.1 / 219.9 | 243.8 / 301.3 |
| Point rebuilds | 2 | 2,014 |
| Capture attempts | 0 | 2,012 |
| Accepted captures | 0 | 778 |
| Preview framebuffer time | 0 | 0 |

The attribution now matches distinct workloads: instance 1 only initializes its
two contours; instance 2 repeatedly rebuilds and attempts captures. Accepted counts
never exceed attempts, and throttling rejects 1,234 of instance 2's attempts.
The previous log had attributed the shared activity to instance 1, illustrating
why its old per-instance step counters were misleading.

The preview cost separation persists: 29.5 vs 113.6 µs median after excluding the
first 60 rows. This supports testing settled-contour reuse on instance 1 and
geometry-key invalidation on instance 2. The log still cannot distinguish
rate-only updates from actual contour changes in instance 2, nor separate live
curve cost from tracer/marker/text cost. Do not assume all 2,014 rebuilds are
redundant or all 113.5 µs is recoverable.

Use this corrected capture as the pre-rendering-change baseline. Small timing
differences from the previous session are not an optimization result; no renderer
change occurred between these captures.

## Settled-contour port ready for comparison

Integral Flux's two NanoVG contours now use Proc's shared 100 ms settlement
framebuffer. The key includes effective rise fraction, signed curve, Maths/Shark
Fin mode, dimensions, highlight selection/color and transform/display scale.
Marker, frequency label and tracer history remain outside the contour cache.
The optional combined OpenGL preview path is unchanged.

This pass deliberately preserves version-based geometry rebuilds and capture
scheduling, isolating contour rendering from the later invalidation experiment.
The corrected per-instance log counters remain intact. Existing
preview_framebuffer_us and per-channel framebuffer fields now also measure
NanoVG contour-cache generation; this work is included in preview_draw_us and
module draw timing, not additional to them. Cached geometry reduction statistics
retain their prior meaning.

Windows plugin.dll, all nine Integral Flux runtime checks and shared settlement
checks passed. Built, not installed. Capture the same two-instance patch with the
same renderer/tracer and zoom settings to compare against the 21:06:48 baseline.

## Settled-cache capture — 21:13:45

User briefly stopped some modulation during this recording. Two complete
1,697-row logs, still NanoVG with stock tracing. Snapshots and summary are in
[settled capture](benchmarks/integral-flux-settled-capture/summary.json).

| Combined preview CPU time | Corrected baseline median / p95 | With cache median / p95 |
| --- | ---: | ---: |
| Instance 1, stable | 29.5 / 35.6 µs | 9.6 / 14.9 µs |
| Instance 2, mixed workload | 113.5 / 129.9 µs | 112.3 / 132.1 µs |

Instance 1 still reports only its two initialization rebuilds and no captures.
Its contour framebuffer work appears once, at row 1 (27.9 µs combined). Median
preview CPU cost falls 19.9 µs, about 67%; p95 falls 20.7 µs. Module subtree median
falls from 161.1 to 143.9 µs. The similar stable workload and one-time framebuffer
work provide live evidence that contour reuse is helping. This is CPU submission
time, not a measured FPS or GPU-completion gain.

Instance 2's full-recording median is not a matched workload comparison because
this capture includes a modulation pause. CH4 alone records a contour framebuffer
render at row 1039 (13.3 µs). Its drawing drops below 10 µs for 300 consecutive
rows, 1054–1353, then returns to normal modulated cost. CH1 continues updating.
CH4 medians: 60.6 µs in rows 950–1038, approximately 4.2 µs during the low-cost
interval, and 61.6 µs in rows 1401–1696. The delay between the cache render and
the lowest draw cost is consistent with existing tracer history fading out.

Do not assign the entire paused-CH4 reduction to the cache: stopping modulation
also removes history drawing and rebuild work. An uncached paused interval was
not captured. Nevertheless, the stable first instance supplies an independent
before/after comparison, while CH4 shows the expected transition into and out of
the settled state without repeatedly rebuilding the framebuffer.

Instance 2 reports 2,229 rebuilds, 2,227 capture attempts and 860 accepted captures.
Version-based invalidation remains a separate possible optimization; these counts
alone do not establish whether actual geometry changed. No further code changes
were made while analyzing this capture.
