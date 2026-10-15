# First live analytic contour results — 10 September 2026

The 18:38:32 and 18:39:01 Rack captures confirm that the new pilot was active.
The user reports no detectable preview quality drop on first inspection. This is
initial live visual feedback, not a complete shape/zoom/animation acceptance test.

The latest capture has 2993 rows: 507 pilot-off, then 2486 pilot-on. Both previews
presented the shader on every requested row, with zero fallbacks. CH1 updated 523
times and reused its cached image 1963 times; CH4 updated 521 times and reused
1965 times. The earlier capture likewise has zero fallbacks.

## Updating previews

For the latest capture's 520 rows where **both** shader surfaces updated:

| CPU scope | Median |
|---|---:|
| Full module widget draw | 788.65 us |
| Both previews, including existing history/overlays | 550.6 us |
| Current contour path | 482.65 us |
| Sum of both state capture/restore scopes, computed per row | 427.5 us |
| Sum of both shader callback scopes, computed per row | 20.5 us |

The per-row state-capture/restore share of contour CPU time has a median of
90.85%. These are nested CPU scopes; medians are not additive and the callback
is not GPU time. The state scopes may include driver synchronization/waiting;
this does not prove that copying state data alone consumes that time.

The earlier file corroborates the concentration: 175 dual-update rows have
453.5 us median state time, 20.5 us median shader callback time, and 90.89%
median state share. This is a live confirmation that optimizing curve math
alone cannot recover most of the current contour cost.

## Comparison limits

Neither file is a clean equal-work active A/B. The first baseline is effectively
static after initialization. The latest baseline has 83 single-preview rebuild
rows after initialization; its shader-active section has 520 dual-preview rebuild
rows. Its active off medians (including the initial row) are contour 24.5 us and
preview 72.95 us, but a multiplicative speed ratio against the dual-update shader
section would conflate different work. No such ratio is claimed.

Stable shader cache hits are much cheaper than updates. Existing point generation,
history and ball still run; geometry-elimination savings are not part of this live
pilot. ui_step_ema_us is smoothed and is not a raw per-frame Step measurement.

## Next target

Keep the analytic shader's appearance result as encouraging initial evidence,
but do not promote this execution path as a performance win. The next change
should replace or consolidate the expensive broad GL state boundary while
preserving host state, lifecycle and cached-image correctness. Profile individual
capture/restore operations (including driver waits), and consider one boundary
for both module previews rather than two independent surfaces. Further atan
optimization is a secondary concern.

Source CSVs were copied read-only into local
`doc/benchmarks/lumin-function-live-20260910/`. The
[durable summaries](lumin-evidence/function-first-live-results.json) include source
hashes, all settings/counters, explicit filtered cohorts and limitations. No
runtime code or Rack settings changed during this analysis.
