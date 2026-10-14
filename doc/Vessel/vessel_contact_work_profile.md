# Vessel contact-work profiling — 2026-10-09

Historical baseline: this profile predates the [retained fast-friction change](vessel_fast_friction_validation.md). Current runs distinguish table lookups (`tanh_lookups`) from analytic `tanh` calls. The reproduction commands exercise the current source; the preserved baseline binaries/CSVs below retain the measurements reported here.

The first operation-count profile points toward the friction law's Gaussian
evaluation as a useful next benchmark. It does not show pathological solver
convergence or justify loosening tolerances. No solver optimization, pitch
behavior change, or filter change was adopted in this pass.

## Scope

96 fixtures cover both bowls, four mallets, Balanced/Reference quality,
zero/33 Hz binaural separation, and 0.002/0.2/1.5 revolutions per second.
Host rate is 48 kHz, actual internal rates are recorded per row, pressure is
2.5, and pitch remains at the default 261.625565 Hz. Each fixture runs these
consecutive phases:

- Startup rubbing: 0.5 seconds from rest.
- Sustained rubbing: 1 second.
- Overlapping strikes: 0.25 seconds, four strikes 62.5 ms apart, two soft and
  two hard launches (normalized velocities 0.1 and 1).
- Release: 0.5 seconds with rotation disengaged.
- Restart: 0.5 seconds with rotation engaged again.

The harness uses `DualBowlAdapter::process`, not Rack's module callback. Counts
per host callback mean one adapter invocation and include both bowls when
active. Auditing is enabled for ledger checks; this disables composed passive
tails. The results describe active contact work, not normal passive-tail CPU
cost. These 2.75-second fixtures do not establish long-term steady oscillation
for every material, nor cover all speeds, loads, pitches, morphs or host rates.

## Findings

The runs cover **12,672,000 host frames and 85,624,129 law evaluations**.

| Requested speed | Sustained laws / friction solve | Sustained calls executing exp | Sustained calls executing tanh | Inner solves / coupled solve during overlap | Laws / coupled solve during overlap |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0.002 rev/s | 2.000 | 100% | 100% | 4.73 | 10.16 |
| 0.2 rev/s | 2.015 | 100% | 2.82% | 4.66 | 9.36 |
| 1.5 rev/s | 1.181 | 100% | 0% | 4.71 | 6.99 |

These are pooled ratios of total counts across the 32 fixtures at each speed,
not averages of per-fixture ratios. Slow rubbing exercises the regularized
near-zero-slip law; ordinary and fast rubbing usually use the existing saturated
tanh branch. Optimizing tanh alone would therefore have speed-dependent reach.

No friction or outer-coupled bisection fallbacks, outer bracket expansions,
solver faults, or nonfinite resets occurred in this matrix. This is evidence
of efficient convergence for these stimuli, not permission to remove safeguards.
The separate accounting test deliberately exercises **195,358 friction fallback
steps**, verifying that the profiler can detect them.

The largest observed total was **164 law evaluations in one host frame**:
Crystal/Silicone, Reference quality, 33 Hz separation, slow rubbing with strikes.
Middle-speed and fast-speed overlap maxima were 120 and 80, respectively.
These combine internal ticks and both bowls; they are not single-solver iteration
counts. The existing `innerIterations` maximum cannot represent this total work.

Gaussian tail skipping alone has limited support here: no sustained-phase
evaluations had `8 <= abs(slip/weakeningVelocity) < 27`. At fast-speed overlap,
that interval covered approximately 3.06% of ordinary-rub law calls and 0.83%
of coupled-inner calls. Most evaluations therefore need a useful approximation
or cheaper arithmetic in the active curve, rather than only an earlier tail
cutoff. Existing offline Hermite prototypes offer a starting point; their value
and derivative must stay consistent and their slope certificate must remain valid.

## What this does and does not establish

Operation counts establish how often each path executes. They do **not** measure
the percentage of CPU time spent in exp, prove a lookup table is faster, or
establish that friction dominates modal work or the complete Rack callback.
The next controlled experiments should benchmark Gaussian evaluation and cached
invariant arithmetic independently, using the observed slip regimes, followed
by an uninstrumented full-module comparison. Coupled inner-solve warm starts
remain a possible separate trial, but existing inner solves already average
roughly two evaluations, so savings are not assured.

Pitch modulation and the matched-pole transform remain deferred by the user.
Normal builds continue to use the 129-tap FIR.

## Instrumentation and validation

`VESSEL_PROFILE_CONTACT_WORK` enables thread-local offline counters. With the
flag absent, the instrumentation expressions are discarded by the preprocessor;
normal DSP has no counters, timers, allocations or TLS access from this feature.
No debug-terminal telemetry behavior was changed. Do not use profiling binaries
for performance comparisons.

The plain and instrumented harnesses produced identical fingerprints for all
480 phase trajectories. Fingerprints include both output channels, mechanical
energy, compression, striker velocity and every modal state at every frame.
Energy residual summaries also match; the largest residual is 6.87e-16 J.
Fingerprint equality is finite regression evidence, not a collision-free or
universal proof of equivalence.

Additionally, GCC 12.3.0 `-O3 -march=nehalem` generated byte-identical `.text`
sections for the original and instrumentation-disabled `FrictionContact.cpp`
and `ContactSolver.cpp`. The linked normal plugin has no `contactWork` or
`ContactWorkScope` symbols. This check is compiler/build-specific, and does not
claim identical object metadata or builds on other platforms.

The dedicated counter test passed 8,000 friction solves covering Newton,
fallback, zero load and rejection, plus nested coupled accounting and scope
cleanup. The full normal Vessel suite and Linux `plugin.so` build passed.
Windows performance and live Rack behavior were not measured in this pass.

## Reproduction and data

```sh
make -j4 test-vessel-contact-work test-vessel all \
  build/tools/vessel_contact_work build/tools/vessel_contact_work_profile
mkdir -p build/vessel-contact-work
build/tools/vessel_contact_work > build/vessel-contact-work/plain.csv
build/tools/vessel_contact_work_profile > build/vessel-contact-work/profile.csv
python3 tools/vessel/summarize_contact_work.py \
  build/vessel-contact-work/plain.csv build/vessel-contact-work/profile.csv \
  build/vessel-contact-work/summary.json
```

Run the summarizer without Python's assertion-disabling `-O` option. It checks
paired identities/fingerprints and counter conservation before aggregating.
The CSV distinguishes `coupled_solves` (inner friction solves) from
`outer_solves` (coupled contact solver entries). Newton/fallback counts measure
chosen next-iterate steps; law evaluations count the residual evaluations.

[Machine-readable results](vessel_contact_work_results.json) include aggregate
and speed-specific phase totals. Local CSVs, binaries, code-generation evidence,
source/binary hashes and logs are under `build/vessel-contact-work/` (ignored
by Git). The original 0.2-only screening run was superseded by this speed sweep.
