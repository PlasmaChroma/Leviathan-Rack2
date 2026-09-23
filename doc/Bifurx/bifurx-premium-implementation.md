# Bifurx Premium implementation and audition record

Implementation date: 2026-09-16. Follow-up to `bifurx-ast-prem.md`.

This is a development build for auditioning, not a qualified Premium release.
DRM work (PREM-05), including vendor synchronization and activation tests, is
explicitly deferred at Leviathan's request until a test key is available. No
DRM synchronization/activation implementation, license files, Pro checkout, public manifest, or installed plugin was
changed. Bifurx is mono in normal processing, Display Only, and bypass.

## Audition controls and behavioral contract

The context menu now offers **A/B: Legacy dark boundary FIR (temporary)**.
Unchecked selects the new wide-passband candidate; checked selects the previous
16-tap, cutoff-0.9 boundary chain. The setting persists and supports undo/redo.
Both choices retain 2x processing at the input and output nonlinear boundaries;
the nonlinear filter core and TITO still run at host rate. This is not whole-core
oversampling. There is no level-dependent oversampling bypass.

The candidate uses 64-tap Blackman–Harris FIRs with cutoff 1.0. The two boundary
pairs have an impulse peak at **62 samples**, versus **14 samples** for legacy.
At 48 kHz these correspond to approximately **1.292 ms** and **0.292 ms**. The
candidate is approximately linear phase in its clean passband. It rolls off
near Nyquist: the measured release passband target is through 16 kHz at 44.1 kHz
and 18 kHz at 48 kHz, not a claim of flat response through Nyquist.

Mode/limiter changes retain the 3 ms fade-out/fade-in. The candidate adds a
32-sample silent interval for the delayed output-boundary transition; changing
boundary type or leaving Display Only primes the selected chain while silent
(64 candidate samples, 16 legacy samples). Thus audition switching is deliberately
not an instantaneous, phase-aligned comparison. Display Only and bypass are
immediate mono pass-through after any applicable mode transition; they do not
have the processing chain's fixed delay. Parallel dry/wet patches must account
for that difference. Soft Limiting still bounds the physical output to +/-5 V.

The gold curve is the nominal, small-signal, unity-LEVEL linear response including
the selected boundary chain. It does not predict driven nonlinear/TITO behavior.
The two frequency markers identify the actual core frequencies, not nearby
response extrema. Their bottom-lane placement never modifies curve geometry.

The spectrum is **tone-equivalent peak dBFS**, referenced to a **5 V peak sine**.
Hann-window main-lobe power is normalized by its coherent gain and equivalent
noise bandwidth. Plot cells retain peaks instead of losing tones between log-axis
points. It is not a calibrated noise power spectral density; nearby unresolved
tones contribute combined energy. The existing subsonic fade remains intentional,
and a 4096-sample window cannot resolve arbitrary sub-bin structure. Response
color/overlay values taper toward neutral when input power is too low to support
a gain estimate (confidence scale approximately -80 dBFS). Generated energy is
still visible in the output spectrum; its ratio to silence is not an LTI gain.

NaN and either infinity on external audio/CV inputs map to zero before normal
finite-voltage limiting and modulation. Finite CV mappings remain unchanged.

## Finding disposition

| Review ID | Implemented work | Remaining qualification |
|---|---|---|
| PREM-01 | Wide FIR candidate, persisted A/B selection, guarded transitions, production passband/alias/impulse tests, gold curve includes boundary response | Listening choice, broader alias grid, minimum-machine/host budget |
| PREM-02 | One output channel in normal, Display Only, and bypass; clear old higher channels | Activated Premium check deferred with DRM |
| PREM-03 | Menu/JSON publish requested quality only; audio detects changes and invalidates its own cache | TSan-capable host harness if available |
| PREM-04 | Explicit GL baseline, separate RGB/alpha blending, production resource validation; actual renderer hostile-state and fallback tests | Windows/other-GPU/DAW close/reopen and failure qualification |
| PREM-05 | Deferred; no licensing changes | Key, vendor contract, full activation/refresh matrix |
| PREM-06 | Saturate stillness counter before addition; avoid order comparisons on wrapping analysis/preview sequences | Long real Rack sessions |
| PREM-07 | Construct/evaluate display biquads in double using production warped frequency/damping; audio math unchanged | Extended all-mode/high-Q musical grid |
| PREM-08 | Clamp FFT coordinate and share preparation between synchronous and worker paths | Low-frequency listening/visual judgment below FFT resolution |
| PREM-09 | Defined/calibrated tone units, peak-preserving log cells, excitation-confidence rule | Visual preference and broadband material audition |
| PREM-10 | CPU/GLSL color law agrees; paused shape and scale changes invalidate visuals | Palette/zoom screenshots in target hosts |
| PREM-11 | Exact core-frequency anchors; cosmetic lanes separate from curve | Final public manual update |
| PREM-12 | Corrected independent Band+High reference, explicit oracle scope, production suite in test-fast, shipping flags, Bifurx header prerequisite, platform-specific Bifurx executables | Native Windows repetition |
| PREM-13 | Neutral non-finite CV sanitization, including recovery tests | None beyond host integration |
| PREM-14 | Module-owned interval feeds CSV and Debug Terminal; renderer-independent recording; Draw includes step-time surface generation | Live combined capture/GPU profiling |
| PREM-15 | LL telemetry requires debug + recorder; preview preparation sleeps without demand and publishes immediately on wake; settled previews avoid pow; removed ineffective adaptive calculation; Display Only skips input resampling/character/TITO preparation | More complex modulated workloads |
| PREM-16 | Viewport/window visibility controls subscriptions and renderer steps; audio-side 250 ms heartbeat lease handles stopped editor stepping; no queued offscreen backlog | Actual DAW/editor and zoom matrix |
| PREM-17 | Reuse worker frequency axes and expected-curve mesh/upload until its own revision/geometry changes; existing unused fallback work was already removed in this checkout | Allocator, browser-construction, batching changes remain measurement-driven options |
| PREM-18 | Separate displayed preview metadata; elapsed-time overlay smoothing; retain scale history and pending/completed FFT payloads independently of curve requests; clear analysis across reset/rate changes | Delayed-worker and host cadence stress beyond harness |
| PREM-19 | Mode menu/arrows record resulting MODE edits; quality, self-oscillation, limiting, and boundary selection record module edits on UI thread | Interactive undo/redo and patch persistence smoke test |

Step measures all module stepping. Draw measures visible rendering, including
surface generation performed during Step. These totals intentionally overlap;
do not sum them as disjoint costs. CSV's first fields are Process, Step, Draw,
in CPU microseconds, followed by ranges and component metrics. GPU completion
time is not represented as CPU submission time. Recorders share the same
module interval, rather than resetting competing audio counters.

## Validation environment and evidence

Native Ubuntu Linux, Intel Core Ultra 7 165H, x86-64 Linux Rack SDK.
Graphics tests ran on Mesa Intel Arc (MTL), OpenGL 4.6 compatibility profile,
Mesa 23.2.1-1ubuntu3.1~22.04.4. Production
runtime tests use `-O3 -funsafe-math-optimizations -march=nehalem`. This session
provides Linux build evidence, not Windows qualification.

- Full `plugin.so` build/link passed.
- Production runtime suite: **55 tests passed**. Independent model suite: **31 tests passed**.
  Changing only `Bifurx.hpp` rebuilds the production test; compiler-generated
  dependencies also cover transitive headers.
- All **55 production runtime tests** also pass with UBSan at `-O1`, with undefined
  behavior configured to stop the run.
- Actual Bifurx OpenGL tests compare pixels under inherited stencil rejection,
  alpha rejection, reverse-subtract blending, color masks, bound VBOs, and altered
  unpack state, including shader failure fallback. They check host-state restoration
  and premultiplied RGBA: **69 checks passed**. Shared GL lifecycle suite: **549 checks passed**.
- Full-module low-level High+High passband grid at 44.1/48/96/192 kHz: worst
  absolute gain error **0.045312 dB**, including core response. Candidate boundary
  impulse peak: **62 samples**.
- Independent stable TPT reference comparisons down to 4 Hz: maximum tested
  complex error **0.000021**. Production high-Q Band+Band comparison at 10/20 Hz
  across four rates: maximum display error **0.074005 dB**.
- Coherent/off-bin tones at 0/-6/-20/-60 dBFS across four rates: maximum tested
  peak error **0.002203 dB** (outside the intentional subsonic fade).
- Production candidate boundary alias tests at three normalized fundamentals,
  two drive levels, and input/limiter nonlinearities: minimum measured folded-power
  improvement over unoversampled processing **5.017237 dB**. The candidate gate is
  at least a twofold power reduction, not a claim of alias-free output. This is
  distinct from the legacy helper's original single-frequency alias tests.
- Broader `test-fast` was allowed to continue after failures. The remaining
  reproducible failure was the unrelated theme-service suite. An earlier run also
  failed the unrelated Octavia recording test; it passed on the continuation run.
  Neither unrelated subsystem was modified.

Temporary diagnostics and benchmark CSV are under `build/bifurx-premium/`.
The benchmark is an in-process loop with debug off, no visual subscribers, and
static controls; it excludes Rack scheduling, DRM, UI, and GPU work. It is not
an audio-deadline or supported-minimum-machine qualification. The final 48 kHz one-instance loop measured roughly
83 ns/sample legacy and 151 ns/sample candidate, with Display Only at 31–32 ns.
At 32 instances, the corresponding per-module values were about 87/164 ns and
34–37 ns for Display Only.
Use the retained benchmark source/CSV for repetition; do not compare these
workstation numbers directly with the Windows review's hardware timings.

## Assets and release closure

Linux Inkscape successfully regenerated all split assets in a temporary directory.
Panel, labels, and background are byte-identical to the checkout. Outlined theme
text paths differ in numeric precision between outlining environments; those
runtime files were preserved. The anchor atlas was regenerated against actual
checkout bytes; its preexisting differences were byte-size/hash records (including
line-ending-sensitive assets), not anchor geometry changes. Repeat generation on
the final packaging checkout and qualify the outline/font toolchain there.

The public free/Premium packaging boundary remains a product decision. No module
was removed from the ordinary development build. Successful activation, platform
promises, minimum hardware, level-matched listening approval, public manual
publication, and the full release matrix from the review remain explicit gates.
