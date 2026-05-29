# Chronomaw Timeline Rendering Spec

## Purpose

Chronomaw's timeline is a musical diagnostic surface, not a decorative scope. It must make fast timing, narrow gates, extreme width settings, multipliers, dividers, phase offsets, and modulation behavior legible without adding audio-thread risk.

The current timeline problem appears when the rendered display samples a fast or narrow event at too low a temporal resolution. At high multipliers or extreme width values, point sampling can miss a narrow pulse, catch only one edge, or catch adjacent pulses inconsistently. This produces apparent flicker, doubled pulses, missing pulses, or misleading history artifacts even when the DSP is producing valid continuous output.

The timeline architecture should therefore use interval summaries rather than point samples.

## Design Goals

- Preserve audio-thread determinism and low CPU cost.
- Render actual history as what happened, not what the engine predicts should have happened.
- Render future preview from deterministic engine state without recomputing expensive full projections every frame.
- Make extreme pulse widths visually truthful at high BPM and high multipliers.
- Keep history and future visually coherent across the `now` cursor.
- Avoid user-visible flicker caused by UI-frame aliasing.
- Support future additions such as min/max envelopes, zoom, per-output focus, modulation overlays, and debug inspection.

## Non-Goals

- Do not turn the audio thread into a scope renderer.
- Do not depend on Rack UI frame rate for historical signal truth.
- Do not claim future preview accuracy for non-deterministic or externally modulated behavior unless the preview explicitly models those inputs.
- Do not preserve misleading point-sampled display behavior for compatibility. Chronomaw is currently unreleased, so the timeline rendering contract can be corrected.

## Core Principle

Each visible timeline column or slot represents a time interval, not a single instant.

For every interval, store or compute:

- `avg`: average voltage across the interval.
- `min`: minimum voltage across the interval.
- `max`: maximum voltage across the interval.
- `valid`: whether the interval contains trustworthy data.

The renderer can draw `avg` as the primary trace and use `min/max` as an envelope when the signal changes faster than the visible resolution.

This resolves the main ambiguity:

- A narrow pulse that occupies only part of a display interval should not randomly appear or disappear.
- It should appear as a stable interval summary, either as reduced average energy or as a min/max envelope that shows the pulse occurred.

## Timeline Regions

The timeline has two different data sources.

### History Region

History is observed audio output. It must be captured from the audio thread because only the audio thread knows the exact per-sample output after clocking, reset, CV, modulation, random decisions, cross processing, quantization, mute, level, offset, and output clamping.

History must not be reconstructed from current parameters. Parameters may have changed since the event occurred.

### Future Region

Future preview is deterministic projection. It is useful for editing and understanding upcoming timing, but it is not an audio record.

Future preview should be generated on the UI thread from a snapshot of engine state. It should use periodic caching where possible and fall back to bounded interval sampling when the future is not safely periodic.

## History Capture Spec

### Audio-Thread Accumulator

The audio thread should accumulate per-output summaries between timeline history write ticks.

Recommended internal accumulator:

```cpp
struct TimelineIntervalAccumulator {
	float sumInternal[8];
	float sumOutput[8];
	float minInternal[8];
	float maxInternal[8];
	float minOutput[8];
	float maxOutput[8];
	int samples;
};
```

Current implementation has already moved toward this with `sum`/average accumulation. The next quality step is adding `min` and `max`.

### Per-Sample Update

After the engine computes final per-sample voltages, update the accumulator:

```text
for each output:
  sumInternal += internalVolts
  minInternal = min(minInternal, internalVolts)
  maxInternal = max(maxInternal, internalVolts)
  sumOutput += outVolts
  minOutput = min(minOutput, outVolts)
  maxOutput = max(maxOutput, outVolts)
samples += 1
```

This update must happen after the DSP result being visualized is final.

If the timeline needs both pre-output internal waveform and final output voltage, both should be accumulated separately as shown above.

### History Write Tick

At a fixed history cadence, convert the accumulator into a ring-buffer interval:

```text
avg = sum / samples
min = minAccumulator
max = maxAccumulator
valid = samples > 0
```

Then reset the accumulator.

The history cadence may remain tied to existing timeline storage density, but it should not be interpreted as an instantaneous sample rate. It is an interval-binning rate.

### Atomic Ring Buffer

The UI thread reads history while the audio thread writes it. The ring buffer should avoid locks on the audio thread.

Recommended stored shape:

```cpp
struct TimelineIntervalSample {
	std::atomic<float> avg;
	std::atomic<float> min;
	std::atomic<float> max;
	std::atomic<uint32_t> sequence;
};
```

A simple first implementation may use separate `avg/min/max` atomics and an atomic write position. A stronger implementation uses a sequence counter:

- Audio thread increments `sequence` to an odd value before writing.
- Audio thread writes `avg/min/max`.
- Audio thread increments `sequence` to the next even value after writing.
- UI thread retries if it observes an odd sequence or a changed sequence.

This avoids torn visual reads without adding a mutex to the audio path.

### History Reset Behavior

When the module resets or initializes:

- Clear history buffers.
- Reset accumulators.
- Mark intervals invalid until real samples are captured.
- Preserve no stale waveform data across reset.

### History Zoom Behavior

Zooming out means each screen column may cover multiple stored history intervals. The renderer should combine summaries:

```text
combinedAvg = weighted average of child interval averages
combinedMin = minimum of child interval minima
combinedMax = maximum of child interval maxima
```

Zooming in means multiple screen columns may map to one stored interval. The renderer may interpolate `avg`, but it should not invent min/max detail that was not captured.

For high-BPM viewing, min/max display is more truthful than interpolation alone.

## Future Projection Spec

### UI-Thread Ownership

Future projection must remain on the UI thread. The audio thread should publish only compact state needed for projection:

- current cycle count or phase anchor
- timing phase offsets
- selected or visible output settings
- current BPM or external clock estimate
- zoom/time scale
- timeline write position
- deterministic seed or event state where needed

The UI thread may compute a projection buffer during widget `step()` or an equivalent throttled update path.

### Snapshot Contract

Future projection must start from a coherent snapshot.

Snapshot fields should include:

```cpp
struct TimelineProjectionSnapshot {
	float bpm;
	float secondsPerBeat;
	float zoomSeconds;
	int selectedOutput;
	int64_t cycleCount[8];
	float timingPhaseOffset[8];
	OutputSettings outputs[8];
	uint32_t stateRevision;
};
```

The exact type names can follow existing Chronomaw code, but the contract matters:

- A projection is valid only for the state revision it was built from.
- Any timing, shape, width, phase, pattern, cross, quant, or zoom change invalidates the projection cache.
- The renderer should never mix intervals from different revisions in the same future buffer.

### Periodic Cache

For deterministic periodic outputs, generate one repeated period once and tile it.

The cache should represent normalized time over a repeat period:

```cpp
struct TimelinePeriodCache {
	uint32_t revision;
	int outputIndex;
	float periodSeconds;
	int sampleCount;
	std::vector<float> internal;
	std::vector<float> output;
	bool valid;
};
```

The cache resolution should be high enough to represent narrow widths:

- Start with `1024` samples per period for simple waveforms.
- Increase to `2048` if visual testing shows narrow pulses still vanish.
- Keep a hard cap to prevent UI cost spikes.

If memory allocation during UI updates is a concern, use fixed-size arrays or persistent vectors reused across updates.

### Repeat Period Selection

The safest initial repeat period is one base cycle for a given output.

For simple multiplier cases, the visible waveform often repeats multiple times inside that base cycle. A later optimization may reduce the cache period to the exact subcycle, but the first implementation should prefer correctness:

```text
period = one base clock cycle in seconds
```

A base-cycle cache handles:

- integer multipliers
- integer dividers
- phase
- width
- swing
- skew
- rotate
- shape changes

For divider cases where the output repeats across multiple base cycles, the period may need to be:

```text
period = divider cycles * base cycle duration
```

The effective timing ratio helper should be the source of truth. Avoid approximate float multiplier logic for repeat-period selection.

### Cache Eligibility

Use the periodic cache when all relevant inputs are deterministic across the preview window:

- internal clock or stable external clock estimate
- no active random waveform requiring unknown future random decisions, unless seeded decisions are projected explicitly
- no unmodeled CV modulation
- no unmodeled cross-source dependency that changes inside the preview window
- no pending reset/run transition

If the output is not cache-eligible, use bounded interval sampling directly from the projection evaluator.

### Future Interval Rendering

Each future display slot maps to a time interval:

```text
t0 = slot start time relative to now
t1 = slot end time relative to now
```

For cache-backed projection:

```text
map t0 and t1 into period cache coordinates
collect cache samples covered by interval
avg = average covered samples
min = minimum covered samples
max = maximum covered samples
```

If the interval wraps around the period boundary, split it into two cache ranges and combine summaries.

For fallback direct projection:

- Use fixed bounded supersampling per interval.
- Start with `4` samples per interval.
- Use `8` only if visual testing shows `4` is insufficient.
- Never allow unbounded sample counts based on zoom or multiplier.

### Narrow Pulse Policy

Extreme width settings are the main reason min/max exists.

If `max - min` exceeds a small threshold inside an interval, the renderer should show an envelope indication. This makes a narrow `5 V` pulse visible even if its average is low.

Recommended threshold:

```text
envelopeThreshold = 0.25 V
```

Below the threshold, draw only the average trace to avoid visual noise.

## Renderer Spec

### Basic Trace

The renderer should draw `avg` as the main line for each output lane.

For a stable slow waveform, this behaves like the current smooth trace.

### Envelope Overlay

When `max - min > envelopeThreshold`, draw a vertical or filled envelope from `min` to `max` for that interval.

Recommended visual treatment:

- Thin vertical stroke for narrow transient intervals.
- Subtle translucent fill when consecutive intervals have large min/max spread.
- Keep `avg` visible on top.

This avoids the false choice between:

- average-only display, which can hide narrow pulses
- point-sampled display, which flickers

### History vs Future Styling

History and future should be visibly related but distinguishable:

- History: observed, slightly stronger opacity.
- Future: projected, slightly softer opacity.
- `now` cursor remains the hard visual boundary.

Do not use radically different rendering semantics between history and future. Both should use interval summaries.

### Zoom Knob Interaction

The zoom knob controls seconds or beats visible on the timeline.

Current intended behavior:

- Knob center is default zoom.
- Turning right zooms in to preserve the current high-BPM inspection range.
- Turning left zooms out farther than the old default.

The renderer should treat zoom as changing interval width, not changing data truth.

When zoom changes:

- History combines or interpolates existing interval summaries.
- Future projection cache may remain valid if only view scale changes.
- Future display intervals must be recomputed for the new scale.

### Selected Output and All-Output Views

The current Chronomaw timeline shows all outputs. Future optimizations may compute higher-detail future projection only for selected output and lower-detail projection for non-selected outputs.

If this is done, the selected output must never visually disagree with the audio history at the `now` boundary.

## Performance Budget

### Audio Thread

Allowed:

- A few additions and min/max comparisons per output per sample.
- Atomic stores at the history interval cadence.

Not allowed:

- Dynamic allocation.
- Locks.
- Cache generation.
- UI geometry work.
- Expensive waveform projection beyond the actual DSP already being performed.

The history accumulator adds predictable cost:

```text
8 outputs * simple scalar operations per sample
```

This is acceptable if implemented plainly and kept branch-light.

### UI Thread

Allowed:

- Cache generation on state changes.
- Fixed-size interval summaries per visible slot.
- Bounded supersampling fallback.

Not allowed:

- Rebuilding all future caches every frame without revision changes.
- Unbounded loops proportional to multiplier, BPM, or zoom.
- Per-frame heap churn in hot paint paths.

Projection should be throttled to widget `step()` or revision changes, not `draw()`.

## Invalidation Rules

Invalidate future cache when any of these change:

- selected output if only selected output is cached
- output enable/mute
- multiplier/divider
- waveform/shape
- width
- phase
- swing
- skew
- rotate
- pattern/probability/euclidean settings
- random seed or random mode state
- cross source or cross operation
- quantizer settings if final output voltage is cached
- level/offset/invert
- BPM or external clock estimate
- reset/run state
- timing phase offset

Do not invalidate the period waveform cache for pure view-only changes if the cache is normalized and can be remapped:

- horizontal zoom
- scroll range
- panel size
- color scheme

However, the visible interval summary buffer must still be recomputed after zoom or layout changes.

## Random and Pattern Behavior

Random and probability-driven behavior require special handling.

Options:

1. Deterministic projection using seeded future event simulation.
2. Mark future as non-deterministic and use a neutral preview.
3. Show only timing windows, not exact random voltages.

For v1, prefer deterministic projection only when the current engine already has stable seed/event boundary semantics. Otherwise, avoid pretending random future events are exact.

History remains authoritative because it records what actually happened.

## External Clock Behavior

With external clock, future projection is only as accurate as the clock estimate.

The snapshot should include:

- last known BPM estimate
- lock state
- PPQN mode if relevant
- time since last clock edge if used by projection

If external clock is unlocked or unstable, future preview should visually indicate lower confidence or shorten the projection window.

Do not let external-clock uncertainty affect history rendering.

## Implementation Phases

### Phase 1: History Stability

Already mostly implemented:

- Accumulate actual audio samples between history writes.
- Store average voltage instead of instantaneous point samples.
- Reset accumulators on module reset.

Remaining:

- Add min/max accumulation.
- Store `avg/min/max` history intervals.
- Draw envelope when min/max spread is meaningful.

### Phase 2: Future Interval Supersampling

Before building the full periodic cache, add bounded interval supersampling in `updateTimelineFuturePreview()`.

Recommended first version:

- `4x` samples per visible interval.
- Use interval average.
- Optionally compute min/max in the same pass.
- Enable for all future intervals, or only for risky settings if CPU cost is measurable.

This is the lowest-risk fix for future-side flicker.

### Phase 3: Periodic Future Cache

Add a per-output future period cache:

- Build one deterministic repeat period.
- Reuse it while the projection revision remains valid.
- Summarize visible intervals from the cache.
- Fall back to bounded direct supersampling when not cache-eligible.

This improves both performance and visual stability.

### Phase 4: Unified Renderer

Move history and future rendering to a common interval-summary path:

```cpp
struct TimelineRenderSample {
	float avg;
	float min;
	float max;
	bool valid;
};
```

Both history and future feed this shape.

The renderer no longer cares whether a sample came from audio history, a periodic cache, or direct future projection.

## Testing Plan

### Unit-Level Tests

Add focused tests where practical:

- Extreme narrow width at high multiplier produces stable nonzero interval summaries.
- Extreme wide width produces stable high intervals without random dropouts.
- History accumulator average matches known synthetic sample sequence.
- History accumulator min/max matches known synthetic sample sequence.
- Future cache invalidates on multiplier, divider, width, phase, shape, and BPM changes.
- Cache-backed future projection matches direct evaluator for simple deterministic waveforms.

### Visual Regression Fixtures

Manual visual fixtures should include:

- `x5`, width `5%`, triangle or pulse-derived waveform.
- `x8`, narrow pulse.
- `x192`, default width and narrow width.
- Very low divider values.
- Zoom centered, zoomed in, and zoomed out.
- Internal clock and external clock estimate.

For each fixture, inspect:

- no random flicker in history
- no missing narrow pulses
- no false reset at high multiplier
- continuity across `now`
- consistent future tiling

### Performance Checks

Measure:

- audio CPU before and after min/max history accumulation
- UI CPU with `4x` future supersampling
- UI CPU with cache-backed future projection
- allocation count during steady-state drawing

The target is no allocation in `draw()` and no measurable audio glitch risk.

## Open Questions

- Should the timeline render average-only by default and show min/max only when zoomed out, or always show envelope when `max-min` exceeds threshold?
- Should history store both internal and final output min/max, or only final output min/max?
- Should selected output get higher future cache resolution than non-selected outputs?
- How should future preview communicate uncertainty for random and external-clock modes?
- Should the period cache use base-cycle periods first, then later optimize to exact subcycle periods?

## Recommended Next Change

The next Chronomaw timeline change should be:

1. Extend history storage from average-only to `avg/min/max`.
2. Update rendering to show a subtle min/max envelope for intervals with significant spread.
3. Add `4x` interval supersampling to future projection as an interim fix.
4. Only then add the periodic future cache if UI cost or visual quality still justifies it.

This sequence fixes the current artifact first and keeps the more complex periodic-cache design isolated from the audio-thread history path.
