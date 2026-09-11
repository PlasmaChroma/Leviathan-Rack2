# Flux analytic current-contour live pilot — 10 September 2026

**Built and packaged, disabled by default. Not installed or measured live yet.**
The shader now lives in `src/render/FunctionCurve.hpp`, shared by offline probes
and the optional live recipe `FunctionContourPilot.hpp`. The archived include
forwards to this shared implementation so live/offline math cannot drift.

## Running the A/B

1. Reinstall the newly built Leviathan package using the existing workflow, then
   restart Rack so the updated plugin binary is loaded.
2. Use the same patch, zoom, modulation and tracer settings for both samples.
   Record a baseline with the pilot unchecked.
3. With Dragon King debug enabled, right-click each Flux under test and select
   **Preview Visual > Analytic contour pilot (session only)**. Both previews in
   that module use the candidate. This is a new UI toggle, not the old
   IntegralFluxRoundStroke configuration flag.
4. Allow the initial shader compilation/cache warmup to settle, then collect
   the matching capture. Keep IntegralFluxDrawLogging enabled using the existing
   logging setup. Turning the toggle off restores the existing contour recipe.

The toggle is not serialized. It starts off for new widget instances/restarts,
and enabling it selects the existing NanoVG preview mode (rather than the older
raw OpenGL ribbon renderer). Disabling the toggle leaves that preview mode at
NanoVG. No released parameter IDs, patch/state schema, or audio behavior changed.

## Scope and eligibility

Only the current contour changes. Both Maths and Shark Fin, hover/drag highlight
colors and independent colored sides are supported. The pilot uses the validated
TwoStep kernel and an image cache keyed by shape, mode, dimensions, colors,
highlight, density and fractional position. A settled hit does not update GL.

Existing point generation/pruning, cached history, tracer ball, labels and timing
semantics remain. Consequently this experiment cannot realize the earlier
geometry-elimination savings. It answers where the shader's real Rack draw path
lands first. There is still a separate protected surface per preview; a shared
module target/atlas is not implemented here.

The recipe falls back to the current-source legacy contour for nested framebuffers,
non-main NanoVG contexts, missing current GL context, rotation/shear/nonuniform
scale, out-of-range extent/density/shape, graphics failure, and a changed-source
second write in the same host frame. Density is 0.5–8 and preview bounds at most
512x256 logical pixels. Stable same-frame image reuse is allowed. Context leases
invalidate the cache on context epoch changes; cached image size is validated.

## Telemetry

Existing module Process/Step/Draw and prior CSV columns retain their meanings.
New suffix fields are added independently for CH1 and CH4:

- `chN_function_requested`, `used`, `updated`, `cache_hit`, `fallback`.
- `chN_function_capture_us`, `setup_us`, `target_us`, `shader_us`, `restore_us`.

The CSV analyzer recognizes the new counters/settings and phase timings. Actual
`used`/`updated` evidence is required before interpreting a capture as a shader
run. Compare total module and preview timings; component scopes are nested and
percentiles must not be added. Phase timestamps run only with Flux CSV logging.

`AdaptiveGlSurface::BatchStats` now optionally measures CPU time spent saving
state, establishing shader state, ensuring/binding/clearing the target, running
the callback and restoring state. No phase clock reads occur when no stats pointer
is passed. This does not claim GPU time or attribute a blocking driver call solely
to CPU computation.

## Offline phase finding

One-preview pilot probe initially measured median capture 43.5 us, restore
31.9 us and shader callback 2.8 us; total including presentation/flush 85.5 us.
A later validation run measured 57.1/42.8/3.6 us respectively, total 112.7 us.
This variation prevents a stable cost percentage claim, but saving/restoring
state clearly dominates the measured CPU path in these runs. A trial saving
only declared generic attributes did not improve timing; it was reverted.
The original four-attribute batch isolation contract remains intact.

## Verification / outstanding work

- Native Windows plugin.dll full link and dist packaging pass.
- Pilot tests cover settled cache hits, same-frame overwrite protection,
  rotation/nested-context fallback, context epoch rebuild, and fractional image
  composition matching the direct shader within one byte.
- Existing batch output/state tests pass, plus 549 GL lifecycle checks.
- The shared shader retains its 300-case optimization-equivalence result (one
  byte max difference); nine analyzer tests pass.
- Full test-fast was not run. Live Rack performance, DAW reopen behavior and user
  visual acceptance are still outstanding. Known analytic-vs-NanoVG peak/narrow
  side differences were not declared fixed.

Package: `dist/Leviathan-2.9.1-win-x64.vcvplugin`. No installation or Rack settings
changes were made. No files were staged or committed.

[Package/source hashes and validation](lumin-evidence/function-live-pilot-summary.json).
Raw phase logs: local `doc/benchmarks/lumin-function-live-pilot-20260910/`.

## First live capture received

The [first live results](lumin-function-first-live-results.md) confirm real shader
use with zero fallbacks and initially acceptable appearance to the user. Active
updates remain expensive: roughly 91% of measured contour CPU time falls in
state capture/restoration. The off/on sections have unequal update workloads;
no clean A/B percentage is claimed.


## Shared module pass candidate (2026-09-10)

The next candidate prepares visible CH1/CH4 contours before ModuleWidget::draw,
then submits both dirty targets under one protected GL boundary. They share a
shader program/VBO. Each widget presents its cached image at the existing contour
slot, preserving trail/ball ordering. This is two targets and two shader draws
under one state scope, not an atlas or a single draw call. Unchanged targets skip
rendering; the same-frame overwrite restriction and context checks remain.

A restricted shader-only batch contract avoids matrix stack work and saves only
enable/color/viewport/polygon/scissor attribute groups plus pixel-store client
state, explicit bindings and generic attributes 0..3. Other clients keep the full
compatibility guard. A mixed batch uses the full guard. Restricted callbacks must
not mutate matrices, legacy client arrays, other texture units or other state.

Module Process/Step/Draw retain their meanings. Shared preparation is included in
module Draw and preview/contour totals exactly once. Existing per-channel phase
columns charge the shared batch to the first dirty channel (CH1 when both change).
CH4 zero phase time therefore does not imply zero rendering; use the sum of phase
columns and the per-channel update counters.

The matched real-GL offline probe rendered two 106x48 previews at 1x. Three rotated
order runs (360 measured frames each) gave total CPU medians of 104.3-105.2 us for
separate full scopes, 73.6-91.0 us for grouped full scopes, and 61.8-75.5 us for
restricted grouped scopes. These include image submission and NanoVG flush;
they are neither GPU execution timings nor a measured Rack speedup.

Native Windows build/package and focused real-GL tests pass, including grouped
initial updates, all-cache-hit frames, one dirty preview, absence of a second
boundary at presentation, fractional pixel equivalence, full/restricted/mixed
state restoration and context lifecycle behavior. This turn did not rerun the
full test-fast suite. Source/package hashes and measurements are in
[lumin-evidence/function-shared-pass-results.json](lumin-evidence/function-shared-pass-results.json).

The updated dist package is ready for the next live test. Reinstall it and enable
"Analytic contour pilot (session only)" again, then repeat the standard capture.
No installation or settings changes were made by the agent. Shader math and legacy
pruning are unchanged. Trails/ball and point generation still use the existing
paths; a full preview replacement remains the architectural goal.


## Shared-boundary live follow-up

The [shared-boundary live analysis](lumin-function-shared-live-results.md) confirms
the candidate was installed and used without fallback. The new capture includes
a comparable active baseline in the preceding run. Both-preview module Draw
medians were 376.1 us baseline versus 675.0 us shader. State cost decreased versus
the earlier candidate, but target and submission scopes remain expensive. This
execution method is still not a performance win. See the report for cohort limits.
