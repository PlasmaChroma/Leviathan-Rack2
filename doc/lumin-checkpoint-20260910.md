# Lumin checkpoint and research review — 10 September 2026

## Checkpoint scope

Last commit at review: `d9a524c` — `Integral flux preview adapter`.
The index was empty at initial review. The user subsequently explicitly authorized
the agent to stage and commit this checkpoint as an exception to the repository's
usual user-owned commit workflow.

Suggested checkpoint commit title:

> Checkpoint Lumin renderer experiments and host-queue bridge evidence

This is an experimental checkpoint, not promotion of a replacement renderer.
Keep the following together so the code and measured status remain intelligible:

- Shared/restricted `AdaptiveGlSurface` batching and diagnostics.
- Session-only Flux analytic contour pilot, shared program and module batch.
- CSV analyzer changes and tests preserving total Process/Step/Draw semantics.
- Shared render headers and focused GL tests.
- Archived experiment source under `tools/experiments/lumin`, including the
  newly tested CPU geometry callback bridge and its explicit Makefile target.
- Experiment reports, research brief, durable JSON evidence and source hashes.
- The user-supplied `lumin-dr-custom-rend.md`, preserved without rewriting it.
- Existing benchmark-tool changes and `.gitignore` exclusion for raw captures.

Do not accidentally omit untracked files: much of the implementation and evidence
is new. The tracked-only diff statistic substantially understates this checkpoint.
Do not add `build/`, `dist/`, `work/`, or raw `doc/benchmarks/` captures to Git.

Reproduction caveat: older round-stroke and callback-bridge targets consume pinned
pristine NanoVG inputs from ignored `work/nanovg`. The modified archived vendor
directory is a different artifact and must not be substituted for those inputs.
This checkpoint records source/evidence but does not make those targets buildable
from a clean checkout without restoring the pinned input files. The source revision
is `VCVRack/nanovg@0bebdb314aff9cfa28fde4744bcb037a2b3fd756`; the preparation script
checks its core hash. Preserve local inputs and captures separately before any
workspace cleanup. Normal plugin building does not depend on this callback probe.

## Current outcome

The analytic shader is visually encouraging but its current custom surface path
is still slower live. Latest matched dual-rebuild cohorts: module Draw 376.1 us
for NanoVG versus 675.0 us for the shader; contour 37.55 versus 327.1 us. Separate
runs are not deterministic replays. Shared state scope helped compared with the
older candidate, but target/submission cost remains material.

The callback bridge is offline only. It forwards optimized CPU-generated stroke
geometry into Rack's existing NanoVG queue. It passed 3,000 exact image comparisons
and beat installed NanoVG's CPU median in all 12 final matched comparisons
(8.8–61.6%, variable across runs). It does not execute our analytic shader, create
an offscreen surface or replace host callbacks. Live inherited-state handling and
cross-version compatibility remain unresolved.

Native plugin link/package previously passed; callback harness native build/run
passed; `make plugin.dll` remained up to date after that isolated probe. Focused
batch/pilot tests passed at their documented revisions. No full test-fast result
is claimed. `git diff --check` passed during checkpoint review (line-ending notices
only). No additional runtime source change was made for this research review.

## Review of the supplied Deep Research report

The report's main recommendation is useful: test the analytic shader inside
Rack-owned `OpenGlWidget`/`FramebufferWidget` execution before optimizing our
manual guard further. Its key observation is supported by the public source:
the default GL widget callback does not surround itself with our broad state
snapshot. That is evidence for a different experiment, not proof that arbitrary
shader state may be left behind.

The report also correctly distinguishes ordinary widget traversal from final GL
submission: direct main-framebuffer GL during traversal is not a substitute for
ordered NanoVG rendering. This does not rule out our CPU geometry bridge, which
inserts an ordinary stroke into the existing host queue and is submitted later.

Important qualifications and corrections:

1. Its table groups a "NanoVG custom render command / host callback" under
   requiring a host change. That is reasonable for a new arbitrary shader command,
   but too broad if applied to invoking the already exposed stroke callback.
   Our new bridge provides a concrete counterexample for supported stroke geometry.
2. It predates the shared-boundary live capture and the callback results. Its
   statement that equal-work shader-vs-NanoVG evidence is absent needs updating
   with our latest cohorts and their remaining comparison limits.
3. We now know installed Rack reports 2.6.6, with pinned NanoVG inputs whose public
   headers match the SDK. That narrows the version uncertainty; it does not prove
   future ABI compatibility or equivalence to every public `v2` source revision.
4. The report's confidence percentages and labor/cost estimates are author
   judgments, not measured engineering evidence. They should not govern promotion.
5. A module-wide image cannot simply be inserted "at the contour z-position"
   without considering the current per-preview history/contour/ball ordering.
   Native framebuffer rendering can also defer dirty updates under frame-budget
   pressure; lower timings must not be credited to rendering fewer updates.
6. Stock `OpenGlWidget::step()` dirties every frame. A native-path experiment must
   preserve cache-hit behavior and account for nested-framebuffer bypass, clipping,
   resize policy and context lifecycle rather than assuming automatic equivalence.
7. The exported Markdown contains opaque ChatGPT citation tokens without a usable
   source bibliography. These are not independently resolvable outside the research
   conversation. The links below re-establish the key host-source references; they
   do not constitute verification of every citation in the report.

## Next decision

Keep two distinct routes:

- **Near-term contour optimization:** finish the callback bridge's live inherited
  state/compatibility contract before adding its separate experimental backend.
- **Custom shader architecture:** a bounded Rack-native framebuffer no-op/clear/
  shader comparison against the current surface method, with equal dirty cadence
  and complete Draw/frame timing. Do not start an atlas or cross-module framework
  merely because it appears in the report.

These routes answer different questions. A successful stroke bridge would preserve
the earlier CPU geometry gain; it would not establish an execution method for
arbitrary custom shaders. Neither route is a shipping win until measured live.

## References checked during this review

- [Rack OpenGlWidget implementation](https://raw.githubusercontent.com/VCVRack/Rack/v2/src/widget/OpenGlWidget.cpp)
- [Rack FramebufferWidget implementation](https://raw.githubusercontent.com/VCVRack/Rack/v2/src/widget/FramebufferWidget.cpp)
- [Rack Window implementation](https://raw.githubusercontent.com/VCVRack/Rack/v2/src/window/Window.cpp)
- [Shared-boundary live results](lumin-function-shared-live-results.md)
- [Callback bridge experiment](lumin-callback-bridge-experiment.md)
- [Original research report](../lumin-dr-custom-rend.md)

Public `v2` links are moving implementation references, not binary-identity proofs.
