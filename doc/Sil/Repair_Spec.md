# Sil Repair Spec

## Purpose

Repair mode is Sil's conservative realtime cleanup layer for generated, decoded, or otherwise peak-hostile stereo material. It should target defects that can be handled safely in a mastering module: isolated sample spikes, short decoder-shaped micropeaks, and peak overs near the final ceiling.

Repair must not become a broad restoration engine. It should avoid full STFT repair, denoising, declipping, phase reconstruction, or heavy transient rewriting. Those belong upstream or offline.

## Current Implementation State

As of the current `src/Sil.cpp`, Repair is implemented as a front-panel switch named `Repair`.

Currently wired behavior:

- Repair defaults to enabled.
- Repair mode currently expands the final limiter delay to `1 ms`.
- The current limiter detector sees ahead over the delayed audio using a monotonic peak queue.
- The processed signal and bypass signal are currently delayed by matching buffers when Repair is enabled.
- The current final limiter uses a gentler Repair-specific knee and timing.
- Repair state is serialized as `repairEnabled`.

Legacy micropeak attempt currently present but not wired into the live audio path:

- `SilMicropeak.hpp` contains chunk analysis, per-channel stereo micropeak detection, and a one-sample look-neighbor cleanup filter.
- `Sil.cpp` contains a `MicropeakWorkerState`, worker-thread lifecycle helpers, hold/confidence logic, and a `micropeakCleanupFilter`.
- `tests/sil_micropeak_spec.cpp` covers detection and cleanup behavior.
- `process()` does not currently call `startMicropeakWorker()`, `pushMicropeakSample()`, `consumeMicropeakHoldSample()`, or `micropeakCleanupFilter.process()`.
- `MICROPEAK_LIGHT` is currently forced to `0`.

Treat this micropeak code as a failed first attempt, not as a behavior contract. It was designed before Repair had a real 1 ms buffered path, so it over-relies on chunk-level detection, delayed hold state, and a one-sample replacement primitive. It can be deleted or heavily rewritten. Keep only scaffolding that still helps the new buffered repair design, such as enum slots, panel/light wiring, serialization, delay-buffer patterns, or tests that remain meaningful after being rewritten.

So, in the live module today, Repair means "use the look-ahead safety limiter mode." The old micropeak code should not be wired in as-is.

## Signal Path

The current live mastering path is:

```text
Input L/R
  -> Repair-controlled limiter and bypass delay setup
  -> Low-band mono recovery
  -> Impact Air high shelf
  -> Remove Mud dynamic peaking cut
  -> Midrange Enhance dynamic peaking lift
  -> Rolling program analysis tap
  -> Glue compressor
  -> Stereo Enhance M/S EQ
  -> Adaptive saturator
  -> Final stereo-linked limiter
  -> Mastering bypass select
  -> Output L/R
```

The target v1 path when Repair and Mastering are both enabled is:

```text
Input L/R
  -> 1 ms pre-master Repair buffer
  -> buffered local repair
  -> Low-band mono recovery
  -> Impact Air high shelf
  -> Remove Mud dynamic peaking cut
  -> Midrange Enhance dynamic peaking lift
  -> Rolling program analysis tap
  -> Glue compressor
  -> Stereo Enhance M/S EQ
  -> Adaptive saturator
  -> Final stereo-linked limiter, one-sample fast-path safety delay
  -> Output L/R
```

Local artifact repair should happen before the mastering-shaping stages. The reason is practical: compression, saturation, EQ movement, and limiting can smear or mask the exact sample-window evidence needed to distinguish a decoder spike from a musical transient. Downstream mastering stages should operate on the repaired signal, not hide the defect and then ask the repair stage to infer what happened.

The final limiter remains the last safety stage, but it should not use the Repair buffer as limiter look-ahead. Repair owns the 1 ms pre-master buffer. The limiter returns to the one-sample fast path after the mastering stages.

The one-sample limiter fast path is still required when Repair is enabled. If the 1 ms Repair budget is fully consumed before mastering, the limiter needs one sample of output delay to apply its computed gain safely to the sample that may overshoot after mastering. This is not a second look-ahead stage; it is the limiter's fast-path safety sample.

Important causality constraint: a full 1 ms pre-master repair delay and a full 1 ms post-master limiter look-ahead cannot both exist inside only 1 ms of total module latency if the mastering stages are processed causally. Since Repair should operate before mastering, v1 chooses the repair-priority model:

```text
Input -> pre-master repair using the full 1 ms budget
      -> mastering stages
      -> one-sample/fast-path final limiter
      -> output
total latency = 1 ms + 1 sample

Not allowed:
Input -> 1 ms pre-master repair
      -> mastering stages
      -> 1 ms post-master limiter
      -> output
total latency = 2 ms
```

This intentionally gives the full look-ahead budget to Repair, because local artifact decisions benefit from true future context and need to happen before mastering stages reshape the signal. The cost is that the final limiter is a fast safety limiter, not a look-ahead limiter, when Repair is enabled. The remaining one sample is reserved for limiter safety, not for another repair or look-ahead process.

## Repair Look-Ahead

Repair look-ahead is controlled by:

```cpp
static constexpr float kRepairLookaheadSeconds = 0.001f;
static constexpr int kMaxLimiterLookaheadSamples = 512;
```

`configureLimiterLookahead(sampleRate, repairEnabled)` maps this to samples:

```text
repair disabled -> 1 sample latency
repair enabled  -> round(sampleRate * 0.001), clamped to 1..512 samples
```

Typical latencies:

```text
44.1 kHz -> 44 samples
48 kHz   -> 48 samples
96 kHz   -> 96 samples
192 kHz  -> 192 samples
```

The limiter delay buffers are allocated to `latency + 1` samples. A matching bypass delay is allocated at the same length. This alignment matters because otherwise the Mastering bypass would compare delayed processed audio against non-delayed input and create misleading timing or combing during switching.

Implementation detail: the buffer is reconfigured from `process()` when the Repair switch changes. Reallocation only happens when the requested latency or buffer length changes.

This 1 ms look-ahead belongs to the pre-master Repair stage. Do not add a separate post-master limiter look-ahead buffer that increases total module latency to 2 ms. The only allowed post-master delay is the limiter's one-sample fast-path safety delay.

When Repair is disabled, the 1 ms Repair window is not active. The limiter remains on the fast-path one-sample delay, and local repair stages must be bypassed.

## Limiter Behavior

The limiter remains stereo-linked and uses the maximum absolute value of L/R.

Detector path:

```text
peak = max(abs(saturatedL), abs(saturatedR))
detectorPeak = peak
```

Audio path:

```text
use one-sample fast-path delay convention
apply smoothed limiterGain
```

The fast path must retain enough state to apply the limiter decision to the delayed sample rather than to a sample that has already been emitted. This is why the one-sample limiter delay remains even when Repair owns the full 1 ms pre-master buffer.

Ceiling:

```cpp
kLimiterCeilingDb = -1.0f;
```

The limiter may keep a small Repair-specific pre-ceiling soft knee:

```cpp
kLimiterRepairKneeLeadDb = 0.75f;
kLimiterRepairKneeDepthDb = 0.20f;
```

If `detectorPeak` exceeds the ceiling, desired gain is `limiterCeiling / detectorPeak`. If it is within the Repair knee below the ceiling, desired gain eases down by up to `0.20 dB`. This makes Repair mode start working just before the hard safety point instead of waiting for an actual over.

Repair mode may keep faster attack and slower release than normal limiter mode:

```text
normal attack  -> 0.50 ms
normal release -> 80 ms
repair attack  -> 0.15 ms
repair release -> 120 ms
```

The limiter is a final safety stage only. It should not be described or implemented as the main Repair look-ahead mechanism once pre-master Repair owns the 1 ms buffer.

## Legacy Micropeak Attempt

The existing `SilMicropeak.hpp` implementation is useful mostly as a record of what was tried. It defines the micropeak target as an isolated, narrow, high-amplitude outlier. The detector checks:

- absolute peak above a full-scale-relative floor,
- immediate neighbor drop,
- peak-to-neighbor ratio,
- local isolation against a surrounding window,
- local max outlier status,
- narrow half-peak width,
- roughness from a short second-difference style measure.

Default profile values:

```text
min peak              -> 0.30 full-scale
min neighbor drop     -> 0.045 full-scale
min peak/neighbor     -> 1.40
min isolation ratio   -> 3.25
min local max ratio   -> 1.28
single-event severity -> 0.55
multi-event average   -> 0.12
min events            -> 2
local radius          -> 24 samples
exclusion radius      -> 2 samples
max half-peak width   -> 4 samples
min roughness         -> 4.5
```

The cleanup primitive is intentionally narrower than the detector. `repairMicropeak()` only replaces the center sample when the previous, center, and next samples prove a strong isolated spike:

```text
peak >= 0.52 full-scale
peak - neighborMax >= 0.10 full-scale
peak >= 1.55 * neighborMax
```

When active, the replacement is:

```text
center = 0.5 * (previous + next)
```

This is a one-sample interpolation repair, not a gain smoother or multi-sample reconstruction.

Do not treat these thresholds, state machines, or tests as mandatory for the next implementation. The important lesson is narrower:

- the defect target is still short, isolated, decoder-shaped damage,
- broad transients must still be vetoed,
- repair should stay local and bounded,
- detection must be based on the actual delayed sample window now available from the 1 ms buffer.

## New Buffered Local Repair Direction

The next Repair implementation should replace the old chunk/hold micropeak path with a sample-window repair stage before the mastering-shaping stages. It uses the full 1 ms Repair buffer. The limiter remains on the one-sample fast path.

Required flow:

```text
input/pre-master L/R
  -> write into 1 ms Repair buffer
  -> analyze current delayed center sample with past and future neighbors
  -> apply local repair only if the center window proves a defect
  -> emit repaired delayed sample
  -> mastering stages
  -> final limiter on one-sample fast-path safety delay
```

Preferred repair strategy:

- First choice: local gain smoothing over a tiny raised-cosine window when the event is suspicious but not obviously corrupt.
- Stronger case: interpolate only the corrupted center sample or very short span using known left/right context from the buffer.
- The repair window should be sample-scale, not chunk-scale.
- The stage should act independently per channel for detection, but stereo-link the final decision when the event is clearly common to both channels.
- The output must remain aligned with the bypass delay model. Repair-on latency is the 1 ms Repair buffer plus the limiter's one-sample safety delay.

Promising detector inputs from the research doc:

- first difference and second difference around the delayed center sample,
- center peak versus immediate neighbors,
- center peak versus local RMS or local mean absolute value,
- half-peak width,
- local roughness,
- short-window transient veto,
- optional repeated-cadence score if periodic decoder spikes are observed.

The 1 ms buffer changes the design materially: future samples are real now, so the repair stage can inspect both sides of an event before deciding. This makes the old asynchronous chunk detector less attractive for v1 because it has coarse timing, delayed activation, and no direct control over the specific center sample being emitted.

Placement caveat: since local repair is pre-mastering and the limiter is post-mastering, implementation should not try to make the limiter consume the same 1 ms buffer as look-ahead. The limiter sees the repaired, mastered stream causally and uses the one-sample fast path.

Useful scaffolding that may be kept:

- `MICROPEAK_LIGHT` as a local-repair activity indicator.
- `REPAIR_ENABLED_PARAM` and `repairEnabled` serialization.
- Existing limiter/bypass delay infrastructure, refactored if necessary so local repair and limiting share timing cleanly.
- `sil_micropeak::StereoSample` style helpers if they are still convenient.
- Tests that assert false-positive avoidance, rewritten against the new buffered repair API.

Scaffolding that should probably be removed or replaced:

- `MicropeakWorkerState`.
- Chunk submission and pending-buffer handoff.
- Hold-sample activation as the gate for repair.
- Confidence based on chunk scoring.
- The current one-sample `CleanupFilter` API if the new stage owns its own ring buffer.

Runtime requirements:

- No worker thread is required for the first buffered repair pass.
- No blocking, allocation, or mutex use in `process()`.
- Buffer lengths and thresholds must update on sample-rate change.
- Repair disabled means no local repair is applied.
- The limiter uses only the one-sample fast path whether Repair is enabled or disabled.
- Repair-on latency is `repairLookaheadSamples + 1 limiter safety sample`.
- Do not stack a post-master limiter look-ahead after the Repair buffer.

Repair-buffer contract:

- There is one Repair look-ahead buffer.
- The Repair look-ahead buffer exists only when Repair is enabled.
- With Repair disabled, the limiter delay is the one-sample fast path.
- Local repair may inspect samples inside the full 1 ms pre-master Repair window.
- The limiter does not use this window for post-master look-ahead.
- The repaired pre-master sample is emitted from the Repair buffer into the mastering chain.
- The final output is emitted after the limiter's one-sample safety delay.
- Bypass alignment must match the active path: one sample when Repair is off, `repairLookaheadSamples + 1` when Repair is on.
- Adding future Repair stages must consume the existing pre-master Repair window or run causally; they must not add another look-ahead delay.

LED behavior:

```text
MICROPEAK_LIGHT = repairEnabled ? smoothed local repair amount : 0
```

Use a smoothed light update on the existing `lightDivider`.

## Repair LED Contract

Repair operations should have explicit LED feedback when they are active. Use intensity where the operation has a meaningful depth or gain-reduction amount. Use boolean-style pulses/holds where the operation is an event decision.

Current available repair-related UI:

- `REPAIR_ENABLED_LIGHT`: mode status.
- `LIMITER_ACTIVE_LIGHT`: final limiter activity.
- `MICROPEAK_LIGHT`: currently unused; should become local repair activity.

Recommended mapping:

```text
REPAIR_ENABLED_LIGHT
  on when Repair is enabled
  off when Repair is disabled

MICROPEAK_LIGHT
  intensity = smoothed local spike/few-sample repair amount
  pulse/hold is acceptable for strict center-interpolation events
  off when Repair is disabled

LIMITER_ACTIVE_LIGHT
  intensity = existing limiter ceiling proximity / limiting demand
  remains the one-sample fast-path limiter indicator
```

If future Repair stages are added, assign each meaningful operation a distinct activity readout if panel space already exists or can be cleanly added:

```text
HF_SMOOTH_LIGHT
  intensity = high-band attenuation amount

SIDE_STABILIZE_LIGHT
  intensity = high-band side attenuation / stabilization amount

CAUTION_LIGHT
  boolean or intensity = repair confidence is low, vetoes are frequent,
  or removed energy is high enough that musical material may be affected
```

Do not light Repair LEDs merely because analysis is running. LEDs should represent audible or potentially audible action:

- repair sample was changed,
- gain notch was applied,
- high-band attenuation was applied,
- stereo side stabilization was applied,
- limiter gain demand or pre-ceiling knee is active.

Suggested smoothing:

```text
event-like repair LED -> fast attack, 80-200 ms hold/release
gain-reduction LED    -> proportional intensity with existing light smoothing
mode LED              -> steady on/off
```

For v1, no new panel control is required. If no new LED anchor is added, use `MICROPEAK_LIGHT` for the local buffered repair stage and keep `LIMITER_ACTIVE_LIGHT` for the final one-sample safety limiter.

## Implementation Notes

The first implementation should be a small synchronous ring-buffer stage, not a worker-thread analyzer. Keep it close to the limiter delay code so latency ownership is obvious.

Suggested state:

```cpp
struct RepairLookaheadState {
	std::vector<float> l;
	std::vector<float> r;
	int write = 0;
	int latency = 1;
	float activity = 0.f;
};
```

The actual implementation may reuse or refactor current delay infrastructure, but the limiter should not use the Repair buffer as post-master look-ahead. The important rule is that Repair owns the 1 ms pre-master buffer and the limiter remains one-sample fast-path after mastering.

Suggested per-sample shape when Repair is enabled:

```cpp
// Write newest pre-repair sample.
repairBufL[write] = preMasterL;
repairBufR[write] = preMasterR;

// The emitted sample is latency samples behind the write head.
const int center = readIndexForLatency(write, latency);
const int prev1 = wrap(center - 1);
const int next1 = wrap(center + 1);
const int prev2 = wrap(center - 2);
const int next2 = wrap(center + 2);

RepairDecision dl = analyzeLocalWindow(repairBufL, center, prev1, next1, prev2, next2);
RepairDecision dr = analyzeLocalWindow(repairBufR, center, prev1, next1, prev2, next2);
linkStereoDecision(dl, dr);

float repairedL = applyLocalRepair(repairBufL, dl);
float repairedR = applyLocalRepair(repairBufR, dr);
```

The exact indexing should match the existing delay-buffer convention in `Sil.cpp` where practical. Avoid introducing a second delay convention unless refactoring the current limiter-owned buffer into a Repair-owned pre-master buffer makes that unavoidable.

Suggested detector scoring:

```text
d1Prev = x[center] - x[prev1]
d1Next = x[center] - x[next1]
d2     = x[center] - 0.5 * (x[prev1] + x[next1])
peak   = abs(x[center])
near   = max(abs(x[prev1]), abs(x[next1]))
local  = mean(abs(x[center - N .. center + N]), excluding center +/- 1)
```

Useful score terms:

```text
neighborDrop = peak - near
neighborRatio = peak / max(near, eps)
localRatio = peak / max(local, eps)
roughness = abs(d2) / max(local, eps)
symmetry = min(abs(d1Prev), abs(d1Next)) / max(max(abs(d1Prev), abs(d1Next)), eps)
```

A candidate should require several agreements, not one hot metric:

```text
candidate =
  peak is meaningfully above silence/full-scale floor
  neighborDrop is large enough
  neighborRatio is large enough
  localRatio is large enough
  half-peak width is tiny
  broad-transient veto is false
```

Start conservative. False negatives are preferable to repairing musical attacks.

Suggested broad-transient veto:

```text
veto if abs(prev2), abs(prev1), abs(center), abs(next1), abs(next2)
form a wider raised shape rather than an isolated needle.
```

Practical implementation: reject if either neighbor is still above roughly `50-65%` of center, or if the half-peak width exceeds a tiny sample span. Also reject if both channels show a coherent broadband rise that lasts more than a few samples.

## V1 Micropeak Tuning Pass

The first live buffered detector has proven useful as a correctness baseline, but it is likely too strict for real material. After the ring-buffer chronology fix, earlier visible detections disappeared, which strongly suggests those detections were buffer artifacts rather than real repairs. The next tuning pass should therefore improve sensitivity deliberately, without weakening the broad-transient and clean-HF vetoes.

Current live defaults in `RepairKernel.hpp`:

```text
min peak              -> 0.52 full-scale
min neighbor drop     -> 0.10 full-scale
min peak/neighbor     -> 1.55
max neighbor share    -> 0.65
min isolation ratio   -> 3.0
repair action         -> interpolate center only
```

Recommended v1 tuning target:

```text
min peak              -> 0.35 to 0.42 full-scale
min neighbor drop     -> 0.045 to 0.070 full-scale
min peak/neighbor     -> 1.30 to 1.40
max neighbor share    -> 0.68 to 0.72
min isolation ratio   -> 2.4 to 2.8
repair action         -> interpolate center only
```

The safest first profile to try:

```text
min peak              -> 0.40 full-scale
min neighbor drop     -> 0.060 full-scale
min peak/neighbor     -> 1.35
max neighbor share    -> 0.70
min isolation ratio   -> 2.6
```

Rationale:

- Lowering `min peak` matters most because real decoder or generated-audio ticks may be audible well below `0.52 full-scale`.
- Lowering `min neighbor drop` should make the detector see smaller single-sample discontinuities, especially on already-mastered sources.
- Lowering `min peak/neighbor` and `min isolation ratio` increases sensitivity, but those should move less aggressively because they protect transients.
- Raising `max neighbor share` slightly allows more real waveform slope around the defect, but it must stay low enough that wide attacks do not qualify.

Do not add panel controls for these thresholds in v1. Use constants or `CandidateConfig` defaults and tune against the synthetic harness plus real patches.

Test expectations for tuning:

- Invariant tests should remain stable: buffer chronology, per-channel independence, broad-transient veto, and clean HF no-hit behavior.
- Threshold-edge tests may move with the profile: below-min-peak veto, neighbor-ratio veto, and exact single-spike sensitivity.
- Add at least one lower-amplitude synthetic spike case before lowering thresholds so the new sensitivity has a visible target.
- Add at least one sloped-neighbor spike case before raising `max neighbor share`, so increased sensitivity is not only tested on zero-neighbor impulses.

Tuning procedure:

```text
1. Add or update synthetic tests for the intended new hit class.
2. Lower one threshold group at a time.
3. Run sil_repair_spec and test-fast.
4. Listen with Repair enabled on clean transients and bright HF sweeps.
5. Keep the profile only if the counter rises on plausible defects without rising on clean material.
```

For v1, the detector should still prefer missed repairs over false repairs. More sensitivity is useful only if the event shape still looks like a tiny local defect from the real delayed window.

Suggested repair decision:

```text
low confidence  -> no repair
medium          -> tiny local gain notch
high            -> interpolate center or declared tiny span
```

For the first pass, it is acceptable to implement only `no repair` and `interpolate center` if the detector is very strict. Add gain-notching after the detector behavior is measurable.

Center interpolation:

```text
y = 0.5 * (x[prev1] + x[next1])
```

Tiny-span interpolation, if needed later:

```text
replace samples inside the detected span with linear interpolation
between the nearest clean samples, then optionally crossfade endpoints
with a two- or four-sample raised-cosine edge.
```

Activity metering:

```text
activityTarget = max(repairDepthL, repairDepthR)
activity = smooth(activity, activityTarget, fast attack, slower release)
```

Do not expose detector thresholds as panel controls for v1. If debugging is needed, prefer compile-time constants or a temporary context-menu/debug profile.

## Guardrails

Repair must stay conservative:

- No heap allocation in the steady-state audio path.
- No blocking locks in `process()`.
- No FFT restoration in v1 Repair.
- No broadband denoising.
- No stereo narrowing as a generic repair.
- No interpolation wider than a tiny, proven-corrupt local event.
- No micropeak cleanup when Repair is disabled.
- The final limiter remains the last safety stage.

The module should prefer missed repairs over false repairs. A single musical transient must be allowed through unless it also matches the isolated micropeak shape.

## Verification

Existing legacy tests:

```text
make test
```

The current micropeak suite verifies the old implementation:

- clean sine is not detected,
- high-frequency sine sweep is not detected,
- repeated isolated micropeaks are detected,
- lower-level repeated micropeaks are detected,
- broad transient is not treated as a micropeak,
- single strong micropeak is detected by severity,
- debug profile detects weaker spikes,
- pre-limiter view preserves more evidence than post-limiter view,
- per-channel stereo analysis remains independent,
- cleanup interpolates isolated spikes,
- cleanup leaves broad transients alone,
- stateful cleanup is gated by active hold.

These tests should be treated as disposable or rewrite targets. Keep the intent where it still applies, especially the clean-signal and broad-transient false-positive checks.

Tests to add for the new buffered Repair:

- With Repair off, `process()` must not alter a synthetic isolated spike via local repair.
- With Repair on, a forced isolated center spike should repair only the center sample or declared tiny span.
- With Repair on, a broad transient should pass unchanged.
- With Repair on, a clean high-frequency sweep should pass unchanged.
- With Repair on, repeated decoder-like single-sample spikes should reduce without dulling surrounding signal.
- With Repair on, final output should be delayed by `repairLookaheadSamples + 1 limiter safety sample`.
- With Repair on and local repair enabled, total latency should equal `repairLookaheadSamples + 1 limiter safety sample`, not Repair look-ahead plus a second limiter look-ahead.
- With Repair off, output should use the one-sample limiter delay path.
- With Mastering bypassed and Repair enabled, bypass output should be delayed to match `repairLookaheadSamples + 1`.
- Toggling Repair should not leave stale limiter gain, stale queue state, or stale bypass delay content that creates a burst.

## Open Decisions

The research document recommends future high-band dynamic smoothing and high-band M/S stabilization for synthetic shimmer and side fizz. Those are not part of the current live Repair implementation.

Before adding those stages, decide whether they belong under the Repair switch or under the existing mastering stages:

- High-band shimmer damping overlaps with `Impact Air`, `Stereo Enhance`, and the saturator/limiter feedback loop.
- High-band side stabilization overlaps with `Stereo Enhance`.
- Adding new Repair stages should not make Repair a second hidden mastering chain.

For the next implementation pass, the cleanest scope is to replace the old micropeak worker/cleanup path with a synchronous 1 ms buffered local-repair stage, then evaluate whether high-band repair still has a distinct job.
