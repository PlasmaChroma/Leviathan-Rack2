# Wyrm Performance Review - May 19

Scope: normal Wyrm DSP, waveform editor, and Wyrm OpenGL body rendering. Sand simulation/raster behavior is intentionally left aside except where the normal widget/render path still instantiates or touches it.

This is a static code review, not a runtime profiler pass.

## Status Update (Implemented Since Initial Draft)

### Completed

1. Audio loop cheap wins in `Wyrm::process()`:
   - Hoisted `SYNC_INPUT` connection check out of the per-channel loop.
   - Moved display-only no-FM frequency calculation into channel 0 only.

2. Conservative DSP fast path:
   - Added a common-case fast path for:
     - no rocks
     - no slither activity
     - no fold activity
     - mono pitch modulation (`!voctPoly && !fmPoly`)
     - mono slither-speed CV (`!slitherSpeedCvPoly`)
   - General path remains intact for all other states.

3. Rock snapshot derived-value precompute:
   - Extended `WyrmRockStateSnapshot` with per-rock derived values used by the static resolver path:
     - wrapped phase
     - default clearance phase
     - default `rx`
     - default inverse `rx`
     - default radius value
   - Updated snapshot-based `cachedRockBoundsAtPhase()` and `resolveAgainstRocks()` to use those values in default-clearance flow.

4. NanoVG editor caching:
   - Added cache for per-point display waveform values.
   - Added cache for body path samples + near-rock flags.
   - Conservative invalidation keys include wave version, rock state index, size, point/sample counts, slither phase, and slither amount.

5. OpenGL body sampling cache:
   - Added persistent sample cache in `WyrmSandGlWidget`.
   - `drawBodyGl()` and `drawBodyMaskGl()` now reuse sampled body data when keys are unchanged.

6. OpenGL strip geometry reuse across layered passes:
   - Refactored strip rendering to separate:
     - join/offset computation
     - strip draw from precomputed offsets
     - feather draw from precomputed inner/outer offsets
   - Reused width-specific offset buffers across repeated layer passes in `drawBodyGl()` and `drawBodyMaskGl()`.

7. Temporary cache effectiveness telemetry:
   - Added `perfBodySampleCacheHits` and `perfBodySampleCacheMisses` counters.
   - Exported as Wyrm debug metrics:
     - `body_cache_hit`
     - `body_cache_miss`
   - Updated debug terminal Python renderer to display the new fields.

8. Slither speed LUT:
   - Replaced realtime `std::pow()` in `slitherSpeedFactor()` with a 512-entry LUT + linear interpolation.
   - Keeps the same control curve while removing pow calls from the hot path.

### Remaining

1. Broader fast-path coverage:
   - Current fast path is intentionally conservative.
   - Additional branch-specialized paths can still be added (for example: no rocks + slither active, no rocks + fold active, and no rocks + slither + fold).

2. Runtime profiling pass:
   - Static wins were implemented, but a focused runtime measurement pass is still needed to confirm new bottlenecks.

3. Shader-first body evaluation:
   - GL SHDR still depends on CPU-side sampled geometry.
   - A more shader-native path remains future work.

## Current Position

Most obvious low-risk wins from this review have been implemented.

At this point, the practical remaining work is:

1. Add a few more no-rock branch-specialized fast paths in `Wyrm::process()`.
2. Validate final impact with runtime profiling/telemetry across representative patches.

Anything beyond that is likely architectural rather than quick-win tuning.

## Summary

Wyrm's audio loop is compact, but the expensive case is clear: polyphony plus rocks plus slither. Rock collision resolution is the main DSP multiplier because it can scan up to 6 rocks for up to 3 passes, and the slither path may run a second rock clamp per channel per sample.

The UI/render side can be heavier than the audio side. NanoVG redraws are mostly contained by the framebuffer dirty logic, but slither forces continuous redraw. The OpenGL paths are visually useful, but they still build dense CPU-side geometry every frame, so "GL" does not currently mean "mostly GPU evaluated."

## Findings

### Medium: avoidable frequency display work in the audio loop

File: `src/Wyrm.cpp`

`displayHzNoFm` is computed inside the channel loop even though it is only used for channel 0.

Current behavior:

- Every channel computes `baseFreq * rack::dsp::exp2_taylor5(voct + fine)`.
- Only `c == 0` stores the value to `displayFrequencyHz`.
- At 16 channels, that is 15 avoidable exponent approximations per audio sample.

Recommended fix:

- Move `displayHzNoFm` into the `if (c == 0)` block.
- Keep the actual FM-inclusive `hz` calculation per channel.

Expected impact:

- Small in mono.
- Clear scalar win in poly patches.
- Very low implementation risk.

### Medium: rock collision resolution dominates the expensive DSP case

File: `src/Wyrm.cpp`

When rocks are enabled, every channel runs `applyRockPush()`. When slither is active, it may also run `applyRockClamp()`.

Each call can:

- Iterate through every active rock.
- Repeat for up to 3 passes.
- Do phase wrapping, radius checks, cache interpolation, projection, and clamps.

The cached default path avoids repeated `sqrt()` for normal audio resolution, which is good. The remaining loop and branch work still scales with:

- Channel count.
- Active rocks.
- Slither amount.
- Audio sample rate.

Recommended fixes:

- Add explicit fast paths for common cases:
  - no rocks
  - no slither
  - no fold
  - mono CV
- Precompute more per-rock derived data into the published rock snapshot:
  - wrapped phase
  - expanded radius
  - inverse radius
  - cached clearance values

Expected impact:

- Best DSP win for real poly patches using rocks.
- Moderate implementation risk if rock behavior is touched too broadly.

### Medium: NanoVG editor redraw cost spikes when slither is active

File: `src/WyrmWaveEditor.cpp`

The editor body path samples up to 768 points per draw. Each body sample can do:

- Catmull-Rom interpolation.
- Visual rock push resolution.
- Slither offset.
- Visual rock clamp resolution.
- Near-rock checks.

Normally the framebuffer limits redraws to real changes. With slither active, the widget marks itself dirty continuously.

Recommended fixes:

- Cache static body points when wave version, rock state, point count, size, and zoom are unchanged.
- Split static base shape from animated slither overlay.
- Avoid resolving visual rock bounds twice per sample when slither amount is zero.

Expected impact:

- Meaningful UI improvement when slither is on.
- Low risk if implemented as a cache with conservative invalidation.

### Medium: OpenGL body rendering is still CPU-heavy

File: `src/WyrmSandGL.cpp`

The normal GL and GL SHDR paths rebuild sampled body points every frame. Sample count can climb up to 8192 based on zoom.

Despite using OpenGL for drawing, the current path still does substantial CPU work:

- Copy wave points.
- Generate sampled body points.
- Run Catmull interpolation.
- Run visual rock resolution.
- Allocate/fill a `std::vector<Vec>`.
- Build strip geometry through immediate mode draw calls.

Recommended fixes:

- Cache sampled body points until inputs change.
- Reuse a member vector instead of allocating a local vector each draw.
- Cache expanded strip offsets/normals where possible.
- Eventually move more evaluation into the shader path.

Expected impact:

- Strongest render-side opportunity.
- Needed before GL SHDR can be justified as the efficient default path.

### Low/Medium: GL strip geometry work is repeated per layer

File: `src/WyrmSandGL.cpp`

`drawBodyStrip()` and `drawBodyStripFeather()` recompute normals, joins, miters, and offsets for each color/feather pass.

In normal GL mode, multiple passes are drawn over the same path:

- outer body
- feathers
- middle body
- more feathers
- inner highlight

Recommended fixes:

- Precompute per-sample normals/joins once per frame.
- Reuse that data for each strip width.
- Consider a small geometry cache keyed by width and sample revision.

Expected impact:

- Lower CPU overhead in GL modes.
- Useful after the larger sample-cache work.

### Low: sync input connection check is inside the channel loop

File: `src/Wyrm.cpp`

`inputs[SYNC_INPUT].isConnected()` is checked per channel. This should be hoisted next to the other per-sample connection booleans.

Expected impact:

- Small.
- Very low risk.

### Low: slither speed uses `std::pow()`

File: `src/Wyrm.hpp`

`slitherSpeedFactor()` uses `std::pow(2.f, ...)`.

This is usually not a major cost because mono slither-speed CV computes it once per sample. If slither-speed CV is polyphonic, it is computed per channel.

Recommended fix:

- Replace with `rack::dsp::exp2_taylor5()` if the approximation is acceptable for this control.

Expected impact:

- Small.
- Low risk if the feel of the speed knob remains acceptable.

## SIMD Assessment

SIMD is possible but not the first optimization to make.

The no-rock oscillator path has clean per-channel independence:

- phase advance
- wavetable lookup
- optional slither
- optional fold
- output writes

That path could eventually be vectorized. The current general path is less SIMD-friendly because it includes:

- Schmitt trigger state per channel.
- soft sync direction state per channel.
- optional poly/mono CV reads.
- rock collision loops with branches.
- slither rock clamping.

Better order of operations:

1. Remove avoidable scalar work.
2. Add fast paths for common simple states.
3. Improve rock snapshot precomputation.
4. Cache UI/GL sampled geometry.
5. Revisit SIMD for the clean no-rock/no-slither or no-rock paths.

## Recommended Optimization Order

1. Move `displayHzNoFm` inside `if (c == 0)` and hoist sync connection outside the channel loop.
2. Add explicit audio fast paths for common states:
   - no rocks
   - no slither
   - no fold
   - mono modulation
3. Expand `WyrmRockStateSnapshot` with derived values to reduce per-sample rock math.
4. Cache NanoVG editor body samples with conservative invalidation.
5. Cache GL body samples and reuse vectors/geometry buffers across frames.
6. Reduce repeated GL strip offset work.
7. Reassess SIMD after the scalar paths are cleaner.

## Practical Takeaway

The obvious first patch is small: remove the extra display-frequency exponent work and hoist sync connection state. That gives a safe polyphony win.

The bigger architectural win is to split Wyrm into fast paths. A simple patch with no rocks and no slither should run with very little branching. The current all-in-one loop keeps the behavior easy to read, but it makes every channel pay for checks related to features that may be inactive.

For rendering, GL SHDR will not be a truly efficient default until the CPU-side body sampling and strip generation are reduced or moved toward shader evaluation.
