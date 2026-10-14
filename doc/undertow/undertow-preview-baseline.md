# Undertow preview baseline — 2026-09-05

Two user captures preserved with hashes and summaries in
[benchmarks/undertow-baseline](benchmarks/undertow-baseline/summary.json).

- 21:26:56: 699 rows, 10.295 seconds, shape=0 throughout, no captures.
- 21:27:16: 1,482 rows, 20.470 seconds. First capture attempt at row 204,
  2.157 seconds after the first row; treat the first 204 rows as pre-modulation.
  The remaining 1,278 rows include the modulated workload and its transitions.

Both use stock curve tracing, edge hardness 0.5, asymmetry disabled, 261.63 Hz,
transform scale 1.414 and pixel ratio 1.0. Shape ranges from 0 to 0.773 in the
second log. No rendering optimization was applied before or during this analysis.

| CPU time, µs | Unmodulated median / p95 | After modulation starts median / p95 |
| --- | ---: | ---: |
| Whole preview | 16.4 / 19.1 | 70.05 / 78.5 |
| History drawing | 0.2 / 0.4 | 56.0 / 63.0 |
| Simplification + path submission | 1.7 / 2.5 | 1.5 / 1.9 |
| Current contour stroke submission | 9.4 / 10.3 | 8.9 / 9.9 |
| Frequency label | 4.1 / 5.7 | 3.1 / 3.8 |

Components are nested inside preview draw. Medians need not sum to the enclosing
median. These are instrumented CPU submission durations, not GPU timings or FPS.
Simplification timing includes NanoVG move/line calls, not just geometry reduction.

The stable log has two sample initializations, one point rebuild, no capture
attempts and 58 simplified points submitted every draw. The second log has 1,034
sample rebuilds, 1,033 point rebuilds, 1,032 capture attempts and 322 accepted
captures. Sample generation/point rebuilding typically cost about 2.3/0.7 µs in
the full second capture. Accepted captures remain bounded by attempts.

The second capture's initial stable interval independently gives 15.6 µs median
preview time, close to the separate stable recording's 16.4 µs. Once modulation
starts, history represents about 79.5% of preview time (median within-row share).
Later rows 800 onward give 73.8 µs preview / 59.0 µs history medians.

## Implications

Retained simplification is a small potential improvement: it avoids scanning
unchanged points but does not eliminate the NanoVG path submission included in
the measured 1.7 µs. Settled-contour caching is the larger idle opportunity,
targeting roughly 11 µs of contour preparation/submission before cache overhead;
the label and centerline still draw. This is an opportunity, not a promised saving.

During modulation, repeated history drawing dominates. A settled contour cache
should bypass while the contour changes, so it should not be expected to remove
the approximately 56 µs history cost. A future snapshot-history experiment could
target it, but should preserve the original tracer appearance and be benchmarked
separately. Start with the small retained-simplification / settled-contour changes
and collect another matched log before selecting a new history backend.

Whole-module draw timing has additional variability (modulated p95 ~439 µs while
preview p95 is ~79 µs). Avoid attributing that entire tail to the preview.

## Snapshot comparison — 2026-09-05 22:07:58

[Raw capture and grouped statistics](benchmarks/undertow-snapshots-20260905/summary.json).
1,765 complete rows over 26.154 seconds. Mode switches from vector (0) to
snapshot (2) at row 875. Transform scale stays 1.682 on both axes and pixel
ratio stays 1.0. Compare six-trail rows to exclude the final idle section.

| Mode, six trails | Rows | Preview median / p95 | History median / p95 | Source points median |
| --- | ---: | ---: | ---: | ---: |
| Vector | 822 | 76.2 / 83.8 µs | 60.0 / 66.1 µs | 351 |
| Snapshot | 520 | 25.3 / 40.3 µs | 9.0 / 23.2 µs | 348 |

Median preview CPU time drops 66.8%; history drops 85%. Source complexity is
similar. Snapshot creation is included: 164 full-history build rows measure
37.55 / 43.9 µs preview median / p95, while 356 reuse rows measure
24.6 / 30.4 µs. The remaining direct contour is still rendered as before.

Snapshot mode records 163 accepted captures and 169 rasterizations. The six
extra renders occur at row 875, caching the existing six trails when switching
modes. That first-use row costs 2,710.7 µs history and 2,729.5 µs preview, a
real initialization hitch retained in the statistics. Subsequent passive fade
from rows 1395–1409 has zero captures/rasterizations as five trails become zero.
The 356 expired-history rows measure median 11.8 µs preview and 0.3 µs history.

Whole-module draw median falls from 130.15 to 80.2 µs among six-trail rows,
but p95 rises from 165.9 to 3,064.4 µs. Several spikes occur outside the preview
(e.g. row 937: module 3,154.2 µs, preview 27.0 µs); similar external spikes
also occur earlier in the vector phase. These fields cannot identify their
cause. Do not infer improved whole-module tail latency or completed GPU time
from the preview improvement.

This independently confirms the shared snapshot backend's steady-state CPU
benefit in Undertow. Remaining work is visual/context validation and, if the
mode-switch hitch matters in normal use, a separate initialization experiment.
No renderer changes were made while analyzing this capture.
