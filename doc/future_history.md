# Chronomaw Sampled Future Timeline

## Purpose

The Sampled Future Timeline is an optional Chronomaw timeline mode that projects likely future output behavior from real emitted history.

This feature is not future history storage. Chronomaw history remains a record of output points that were actually emitted. The future side of the timeline starts empty unless the feature is enabled and the UI requests a projection.

## User-Facing Control

Add a module context menu item:

```text
Sampled Future Timeline
```

Behavior:

- Off by default for the MVP.
- When enabled, the future region may show projected output points.
- When disabled, the future region remains deterministic preview-only or empty according to the existing timeline mode.
- The setting is patch-local unless a later product decision makes it a global visual preference.

The menu label should describe the feature without implying that future samples are real output history.

## Core Invariants

- History contains only real output points emitted by the DSP engine.
- Projected future points are derived, disposable UI data.
- Projected points are never appended to history.
- Projected points are never used as training input for later projections.
- Real emitted points always supersede any projection for the same timeline region.
- Prediction confidence may drop during edits or unstable behavior, but history capture does not change.

## MVP Data Model

History points should carry the current phase when they are emitted:

```cpp
struct HistoryPoint {
	double time;
	float value;
	float phase;      // normalized [0, 1)
	uint32_t epochId; // behavior regime assigned by timeline analysis
};
```

Future samples are query results:

```cpp
struct ProjectedPoint {
	double time;
	float value;
	float phase;      // normalized [0, 1)
	float confidence; // [0, 1]
};
```

Epoch state tracks whether a behavior regime has enough real history to predict from:

```cpp
struct EpochStats {
	uint32_t epochId;
	uint32_t pointCount;
	float phaseCoverage; // [0, 1]
	bool stable;
};
```

The first MVP can keep this in UI/timeline-owned state. It does not need to be part of the audio thread's critical path.

## Phase Representation

Store phase as a normalized float:

```text
0.0 <= phase < 1.0
```

Degrees or radians are display concerns:

```cpp
float degrees = phase * 360.0f;
float radians = phase * 2.0f * PI;
```

Circular distance should wrap across zero:

```cpp
float phaseDistance(float a, float b) {
	float d = std::fabs(a - b);
	return std::min(d, 1.0f - d);
}
```

## MVP Implementation Clarifications

These decisions are part of the first implementation and should not be left to local interpretation.

Phase source:

- Store the per-output phase that corresponds to the emitted output value.
- This should be the phase after divider/multiplier and phase-offset behavior have been applied.
- Capture it at the same DSP point where the final emitted output value is known.
- If later DSP ordering changes make this ambiguous, prefer the phase that best explains the rendered output transition the user sees.

Interval history:

- If history is stored as interval summaries, store the interval midpoint phase for the MVP.
- Do not increase history resolution solely for this feature.
- Do not change the history write cadence solely for this feature.
- A later implementation may add `phaseStart` and `phaseEnd` if wide intervals need more precise projection.

Epoch changes:

- Increment the timeline epoch when the output-settings revision changes for any timeline-relevant parameter.
- The MVP may use the existing state/parameter revision plumbing if present.
- If no revision counter exists, add the smallest scoped timeline revision needed to detect relevant output-setting changes.
- Epoch changes are analysis metadata; they do not alter emitted output behavior.

Initial projection scope:

- The first implementation may project only the selected output or the currently visible output set.
- It does not need to precompute projections for all eight outputs when those projections are not being displayed.
- Projection should be generated for the requested visible future window only.

Fallback behavior:

- Until at least one stable epoch exists, return no sampled future projection.
- Once a previous stable epoch exists, using it with reduced confidence is allowed while the current epoch is still unstable.
- Low-confidence sparse projection before any stable epoch is deferred.

## History Capture

At each timeline history write tick, append only the observed output interval or point:

```cpp
history.push_back({
	currentTime,
	emittedValue,
	currentPhase,
	currentEpochId
});
```

This write happens only after the output value has actually been produced. It must not be called for projected future samples.

If the existing timeline stores interval summaries rather than point samples, the same rule applies: the interval summary is history only after the engine has emitted the samples covered by that interval.

## Epochs

An epoch represents a behavior regime. It lets the timeline distinguish old stable behavior from newly edited behavior that has not yet accumulated enough phase coverage.

For the MVP, start a new epoch when the user changes a timeline-relevant parameter:

- Timing multiplier or divider.
- Phase offset.
- Width, waveform, slew, level, offset, invert, mute, or probability.
- Euclidean, loop, cross, quantizer, random, or CV assignment settings.
- Any reset or clear action that invalidates the old behavior basis.

Automatic change detection can be deferred.

## Epoch Stability

An epoch becomes stable when it has enough real points across the phase cycle.

Initial MVP thresholds:

```text
minimum points: 64
phase bins: 32
minimum covered bins: 26
minimum observed span: 1 full phase cycle
```

This gives about 81% phase coverage before the current epoch becomes the preferred source of truth.

The thresholds are intentionally conservative. A temporarily strange forecast during editing is acceptable; adopting a new source of truth before enough real data exists is not.

## Projection Source Selection

When `Sampled Future Timeline` is enabled, choose a prediction source for the requested future window:

```text
If current epoch is stable:
    project from current epoch
Else if a previous stable epoch exists:
    project from the previous stable epoch with reduced confidence
Else:
    return no projection or low-confidence sparse projection
```

Blending current partial history with the previous stable epoch is a later enhancement. The MVP should prefer a simpler and more debuggable source selection rule.

## Projection Query

Projection is lazy. The timeline asks for a visible future window and receives derived samples:

```cpp
std::vector<ProjectedPoint> projectSampledFutureTimeline(
	const std::vector<HistoryPoint>& history,
	double fromTime,
	double toTime,
	double step,
	uint32_t currentEpochId
);
```

The function must not mutate history.

For each future step:

- Estimate the future phase.
- Find recent real history points in the selected epoch near that phase.
- Interpolate or average nearby values.
- Return a projected value and confidence.

The first implementation can use binned phase lookup rather than a full nearest-neighbor search:

```text
target phase -> phase bin -> average observed value for that bin
```

If a target bin has no data, use nearby bins within a small wrapped radius or return no point.

## Confidence

Confidence is a UI and debugging signal, not a correctness guarantee.

Recommended MVP factors:

- Epoch stability: stable current epoch is highest.
- Fallback source: previous stable epoch is lower.
- Phase locality: exact or nearby phase-bin matches are higher.
- Coverage: sparse epochs are lower.

Projected points with low confidence should render subdued or may be omitted entirely.

## Rendering

The timeline should visually separate observed history from sampled future projection:

- Observed history: solid, primary rendering.
- Sampled future timeline: dashed, translucent, or otherwise secondary.
- Low confidence: subdued.
- No projection: empty future region.

Real history always wins visually. As time advances and real output points are emitted, any stale projected points for that region should disappear or be replaced by observed history.

## Serialization

MVP serialization should include only the user-facing enable flag:

```json
{
  "ui": {
    "sampledFutureTimeline": false
  }
}
```

Do not serialize projected points.

Epoch metadata may be rebuilt from history. If rebuilding proves expensive later, cached epoch summaries may be serialized as an optimization, but they must remain invalidatable derived data.

## Audio-Thread Constraints

The audio thread may publish the phase and output value needed for real history capture.

The audio thread must not:

- Search history for projection.
- Allocate projection buffers.
- Compute future sampled timeline windows.
- Block on UI state.

Projection belongs to the UI thread or another non-audio execution path.

## MVP Non-Goals

- No machine learning.
- No persistent future history.
- No training on projections.
- No automatic regime detection.
- No blending across epochs.
- No promise of accurate projection while the user is actively changing behavior.
- No long-horizon guarantee.

## Implementation Order

1. Add the patch-local `Sampled Future Timeline` enable flag.
2. Add the Chronomaw context menu item with that exact label.
3. Store normalized phase on real emitted history points.
4. Assign `epochId` to history points.
5. Start a new epoch on timeline-relevant parameter changes.
6. Track phase-bin coverage for each epoch.
7. Mark epochs stable once coverage thresholds are met.
8. Implement a lazy projection query that returns disposable `ProjectedPoint` values.
9. Render projected future samples separately from observed history.
10. Add a focused test that proves projected points are not appended to history.

## Acceptance Criteria

- With `Sampled Future Timeline` disabled, no sampled future projection is rendered.
- With it enabled and no stable history, the future region remains empty or low-confidence.
- Once a stable epoch exists, the future region can show projected samples.
- Changing a relevant parameter starts a new epoch and reduces confidence until enough new phase coverage exists.
- History size increases only when real output points are emitted.
- Projected points disappear or are replaced as real history reaches the same time region.
- Reloading a patch restores the enable flag but does not restore projected points.
