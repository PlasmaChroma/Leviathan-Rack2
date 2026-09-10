# Integral Flux Lumen pilot

Status: exploratory and controlled laptop captures analyzed below; minimal adapter
implemented with native focused tests and an initial live B capture. Matched,
repeated A/B overhead validation remains pending. No measured
Lumen speedup yet. This is the first test from
[RFC section 14.5](Leviathan_Render_RFC.md#145-first-targeted-test--integral-flux).

## Capture

### Selecting A or B in the pilot build

The new `IntegralFluxLumenPreview` Boolean in the user `dragonking.txt` selects
B when true and `debug` is also true. False or absent selects A. Restart Rack
between configurations and use the same installed build and saved patch for both.
Selection is fixed when the module widget is constructed and is not serialized
into the patch. The CSV appends `lumen_preview_adapter` (0=A, 1=B); existing
column ordering, `Process`/`Step`/`Draw` meanings, and component timers are preserved.
Older captures lack this column and remain readable, with that setting marked
unavailable. The renderer-mode columns still distinguish the optional legacy paths.

```json
{
  "debug": true,
  "IntegralFluxDrawLogging": true,
  "IntegralFluxLumenPreview": true,
  "extraGlValidation": false
}
```

Merge these keys into existing preferences. The adapter is
[`LegacyCurvePreview`](../src/render/LegacyCurvePreview.hpp): a Rack-owned subtree
containing the same contour and snapshot widgets, with shared extent propagation
and explicit presentation. It adds one ordinary widget per preview. The legacy
14-value contour key, 100 ms settlement, geometry preparation, outgoing-contour
captures, history timing, marker/label rendering, and NanoVG/GL fallback behavior
are retained. Knob rendering is unchanged.

This is an ownership/layout adapter with existing invalidation delegated to the
existing caches. It does not implement the proposed RenderDevice, verified context
epochs, presentation leases, or a new host bridge, and does not certify the RFC's
interop invariants. Test zoom, browser/null-module appearance, and reopen in Rack
before drawing conclusions from its CPU captures.

Native Windows `plugin.dll` linking passed. The new Rack-linked CPU test verifies
single ownership, step/context-event delivery, explicit composition, contour/history
independence, and teardown using probe layers without a GL context. All nine Flux
runtime checks and the seven analyzer tests passed. Full native `test-fast` was
attempted but stopped compiling the unrelated `theme_persistence_spec` with
SIMDe/intrinsics redefinition errors on this laptop's GCC 15.2; this is not a full
suite pass. No live adapter capture has been collected yet.

### Workload

Use one visible Integral Flux with its six HaloKnob2 controls and two previews.
Keep the existing NanoVG preview and snapshot history (CSV mode 0, tracer enabled,
tracer mode 2). Keep power mode, zoom, DPI, audio settings, and viewport fixed.
Save the patch and the modulation source/settings used so it can be replayed for B.
Modulate rise or fall independently to change contour shape; changing both equally
can change period without exercising semantic geometry rebuilds.

The existing logger requires `debug: true` and `IntegralFluxDrawLogging: true`
in `Leviathan/dragonking.txt` under Rack's user directory. Merge these keys into
the existing JSON rather than replacing other preferences. Keep
`extraGlValidation` false for production-path measurements. Configuration is read
at plugin initialization; restart Rack after changes. Logs are written under
`Leviathan/IntegralFlux` in Rack's user directory. Stop the capture/close Rack before
analysis so its final CSV row is complete. This tooling does not alter the installed
plugin or debug configuration.

Record cold creation separately, then approximately 30 seconds each of:

1. Fixed shapes, cycling channels, moving markers, and settled contours.
2. Repeatable independent rise/fall modulation, with snapshot histories active.
3. Modulation stopped, histories allowed to expire, and settled contours restored.

Record transition boundaries externally and select unambiguous CSV row intervals;
the current CSV has no timestamps, so row IDs cannot establish elapsed seconds.
Do not convert row counts to time using an assumed frame rate. If boundaries cannot
be established, use separate homogeneous captures and record that cold behavior
was excluded manually. Keep knob interaction, zoom, and reopen checks separate
from these intervals. Repeat matched runs before setting numerical overhead gates.

## Analyze

From the repository root, using Python's standard library only:

```sh
python3 tools/analyze_flux_draw_log.py /path/to/integral_flux_draw.csv --label A-full-run1 > /tmp/flux-A-full.json
python3 tools/analyze_flux_draw_log.py /path/to/integral_flux_draw.csv --label A-static-run1 --start-row 100 --end-row 1800 > /tmp/flux-A-static.json
```

The example row bounds are placeholders. `--start-row` is inclusive and
`--end-row` exclusive, using the CSV's `row` field. Nothing is silently trimmed as
warm-up. Each output records a SHA-256 of the source file, module/instance identity,
selected rows, settings, timing distributions, work counters, and missing fields.
Analyze each module/file independently; do not concatenate captures.

Compare the same fields in A (existing renderer) and B (minimal framework adapter):

| Fields | Interpretation |
|---|---|
| `module_widget_draw_us`, `preview_draw_us` | Nested CPU submission scopes, not host frame times |
| `history_draw_us`, `contour_draw_us` | Components within preview time |
| `halo_step_surface_us` | Knob surface work performed during step; keep visible beside draw-time work |
| `halo_gl_surface_framebuffer_us` and corresponding draw count | Knob GL updates performed during draw |
| `preview_point_rebuilds` | Geometry build count |
| `preview_tracer_captures`, `preview_tracer_accepted_captures` | Capture attempts and accepted captures, respectively |
| `history_rasterizations` | Snapshot raster work; not the number of composites |

The analyzer keeps EMA values separate from raw timing distributions and treats
trail counts as gauges, not event totals. Missing fields are unavailable, not zero.
It warns about mixed renderer settings and rejects incomplete or malformed CSVs.
Percentiles use linear interpolation between sorted observations.

Do not add nested timings or their percentiles. Existing draw logs do not fully
include their own file-writing cost and do not measure GPU execution, source age,
host frame pacing, shader compile counts, or memory. Collect those separately
before claiming the broader RFC performance gate has passed. An instrumented
baseline also needs a diagnostics-off host check before shipping.

## Experiment record

Keep this information beside each capture summary; unknown values remain unknown:

- Configuration A/B/C, exact plugin source/build, Rack version, and saved patch.
- GPU/driver, power source/mode, display size/scaling, zoom, thermal/run conditions.
- Audio sample rate, buffer size, engine settings, and any observed dropouts.
- Workload, modulation settings, selected row boundaries, run duration/repetitions.
- Visual/interaction/zoom/reopen observations and any fallback activation.
- Before B: numerical overhead tolerance based on repeated A runs and their variation.
- Before C: named optimization, minimum useful benefit, frame/source-age tolerances,
  memory ceiling, and measurement sources. Do not infer these from the CSV alone.

The first adapter test passes on visual/lifecycle/invalidation correctness and
acceptable overhead. Choose a preview optimization from its measurements afterward.
Halo shader sharing remains a separate potential patch-load/reopen experiment.

Tool validation:

```sh
python3 -m unittest discover -s tests -p test_analyze_flux_draw_log.py
```

## First laptop capture — 9 September 2026, 17:26:52 filename timestamp

One instance, 5,305 complete rows (0–5304). The user followed the sequence roughly
with additional interaction. Renderer settings stay constant: NanoVG preview,
tracer enabled, snapshot mode, Halo NanoVG fallback not forced. The local
[snapshot and summaries](benchmarks/flux-laptop-20260909/summary.json) preserve
the source hash and exact selections. These intervals were selected after
inspection to characterize workloads, not to demonstrate an improvement.

| Interval, row IDs inclusive–exclusive | Module subtree CPU median / p95 | Both previews CPU median / p95 |
|---|---|---|
| Quiet early, 750–1750 | 202.2 / 281.44 µs | 20.1 / 28.1 µs |
| Quiet later, 2750–3250 | 337.45 / 410.89 µs | 34.1 / 50.2 µs |
| Active previews, 4250–5250 | 412.25 / 647.65 µs | 115.6 / 225.92 µs |

Both quiet intervals have zero point rebuilds, accepted captures, history
rasterizations, and recorded knob surface work. The early interval still contains
36.5 µs total preview framebuffer work, so it is not proof of zero contour-cache
refreshes. Neither interval reports history trails. Their differing CPU costs
show why quiet state alone does not establish matched conditions: zoom, power,
temperature, marker visibility, and other host work are not recorded here.

The active interval contains 1,480 point rebuilds, 734 accepted captures, and
exactly 734 history rasterizations, with 10–12 visible trails. It records no knob
dragging, knob GL framebuffer draws, or knob step-surface time. Current contour
submission is 60.0 / 76.81 µs median/p95; history submission is 18.1 / 114.81 µs.
This supports investigating current-contour cost and capture-frame history tails
while preserving the already independent knob caches. It does not identify a GPU
bottleneck or establish redundant geometry work.

The largest module subtree observations are rows 1 and 2, approximately 36.6 and
31.8 ms, while their preview times remain small. Row 0 also records 17.0 ms of
knob step-surface work. Preserve these as cold observations; the log cannot assign
them specifically to shader compilation. They remain separate from warm results.

The final rows still show changing previews and history activity; there is no
confirmed final post-modulation settling interval. This capture is usable for
exploration, but lacks exact workload/environment metadata, host/GPU timing,
source age, and repeated matched runs. No numerical migration gate is established
from it yet, and the current checkout is not asserted to identify the installed
binary that produced it.

## Controlled laptop capture — 9 September 2026, 17:32:26 filename timestamp

The next capture contains 2,921 complete rows (0–2920), one module, unchanged
renderer settings, and no recorded knob dragging. The user reports a prepared
patch and a final settling period. The [preserved snapshot and summaries](benchmarks/flux-laptop-controlled-20260909/summary.json)
record exact row selections and the file hash. Transition boundaries below are
observations of work counters, not inferred elapsed seconds.

- After initialization, preview changes begin at row 846.
- The final point rebuild and accepted capture occur at row 2089.
- The contour cache refreshes at row 2095; the last visible history is row 2108.
- Rows 2109–2920 contain no further point rebuilds, captures, history
  rasterizations, or visible trails. The selected final interval also has zero
  preview framebuffer time and zero knob surface work.

| Interval, row IDs inclusive–exclusive | Module subtree CPU median / p95 | Both previews CPU median / p95 |
|---|---|---|
| Steady, 200–800 | 316.8 / 394.92 µs | 32.1 / 39.02 µs |
| Modulated, 1000–2000 | 384.3 / 519.49 µs | 107.5 / 206.15 µs |
| Settled, 2200–2921 | 323.9 / 393.4 µs | 32.9 / 47.0 µs |

Steady and settled intervals have zero point rebuilds, accepted captures,
snapshot rasterizations, and knob surface work. Their similar median preview
costs support a return to the existing cached path after modulation. These are
different workload phases of the same implementation, not an A/B speedup.

The selected modulated interval records 1,445 point rebuilds and 710 accepted
captures, matched by 710 rasterizations. Current-contour CPU submission is
58.1 / 70.41 µs median/p95, while history is 17.2 / 112.61 µs. Knob step-surface
time and draw-time surface updates remain zero throughout this interval. Across
the entire capture, all 884 accepted captures match 884 rasterizations.

This is the preferred initial workload reference for the minimal adapter:
preserve quiet/settled work counts and independent knob caching, then compare
active current-contour and history costs. Both remain candidate costs to examine;
these counters do not prove geometry builds are redundant or diagnose GPU cost.

Cold work remains separate: row 0 records approximately 38.5 ms of knob
step-surface work, and row 1 has a 23.5 ms module subtree draw. Neither establishes
shader compilation time. Keep shared-program loading experiments separate from
the warm preview comparison. Exact host/GPU timing, source age, environment/build
metadata, and repeated matched runs remain necessary before migration thresholds
or performance claims; no new runtime has been benchmarked here.

## First adapter capture — 9 September 2026, 20:29:48 filename timestamp

The capture contains 7,902 complete rows, with `lumen_preview_adapter=1` throughout.
Other recorded settings match the earlier capture: NanoVG previews, enabled
snapshot tracing, Halo fallback not forced. The user reports the same general
sequence with additional tweaks. The [snapshot and summaries](benchmarks/flux-laptop-adapter-20260909/summary.json)
preserve the input and selections. Use the earlier controlled capture as a
provisional A reference; it predates the adapter build and is not a same-build,
alternating A/B experiment.

Selected B intervals are steady 400–4000, modulated 4400–6000, and settled
7600–7902 (exclusive ends). The modulated selection precedes the first recorded
knob surface update after startup at row 6208 and dragging at row 6258.
Later interaction is excluded from the table but preserved in the full summary.

| Phase | A preview median / p95 | B preview median / p95 | A module subtree median / p95 | B module subtree median / p95 |
|---|---|---|---|---|
| Steady | 32.1 / 39.02 µs | 32.4 / 45.4 µs | 316.8 / 394.92 µs | 327.5 / 402.91 µs |
| Modulated | 107.5 / 206.15 µs | 109.3 / 209.5 µs | 384.3 / 519.49 µs | 406.7 / 527.3 µs |
| Settled | 32.9 / 47.0 µs | 32.6 / 46.2 µs | 323.9 / 393.4 µs | 327.85 / 404.3 µs |

The B steady and settled selections contain zero point rebuilds, captures,
history rasterizations, preview framebuffer time, and knob surface work. During
the selected modulation interval, 2,370 point rebuilds accompany 1,136 accepted
captures and exactly 1,136 rasterizations, with no knob surface work or dragging.
Across the entire capture, all 1,803 accepted captures match 1,803 rasterizations.
Counters support preservation of independent caches and capture-driven history.

After the final tweaks, the last point rebuild is row 7429, last visible history
row 7445, last knob surface work row 7454, and last contour framebuffer refresh
row 7461. The final selection therefore follows both fading and cache settlement.

Preview medians differ from A by approximately +0.9%, +1.7%, and -0.9% across
these phases. Tail observations are not uniformly equivalent: steady preview p95
increases by 6.38 µs, and settled module subtree p99 is 510.95 µs versus 438.12 µs
in A. Unequal interval sizes, user tweaks, unrecorded environment/source details,
different builds, and lack of repeated runs prevent assigning those differences
to adapter overhead or declaring a numerical gate passed. Current-contour and
history submission remain the active-preview candidates (B medians 59.8 and
18.1 µs respectively).

This is encouraging initial CPU/counter evidence for the adapter. It does not
certify visual parity, browser/capture/reopen behavior, GPU cost, host frame
pacing, or source freshness. Preserve this result without expanding the runtime
on the strength of a claimed speedup: no optimization was introduced, and none
is established by this capture.
