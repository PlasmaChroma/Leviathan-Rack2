Dragon King Leviathan, I think Codex is thrashing because the current design is trying to make “rocks as solid obstacles” emerge from several partial systems:

1. stored waveform point projection,
2. runtime `applyRockPush()`,
3. slither-only `applyRockClamp()`,
4. editor drawing with its own visual slither phase,
5. rounded path rendering that can cut through geometry.

That is too many tiny wyrms arguing over one stone.

## Main diagnosis

The most important bug: **`applyRockClamp()` only checks whether the final slither target lands inside the rock interval. It does not check whether the slither motion crosses through the forbidden interval.**

So this case can clip straight through a rock:

```cpp
base   < lower
target > upper
```

The final `target` is not inside `[lower, upper]`, so the current code does nothing. The slither effectively teleports from below the rock to above it. The uploaded code does runtime audio like this:

```cpp
const float base = applyRockPush(lookupWave(phase[c]), phase[c]);
const float slither = applyRockClamp(base, phase[c], slitherOffset(...));
const float raw = clamp(base + slither, -1.f, 1.f);
```

So if `applyRockClamp()` misses a crossing, the final output can violate the obstacle idea even though each individual helper function looks locally reasonable.

The second structural issue: **`pushWavePointsOutsideRock()` only projects control points and straight segments, but the actual waveform is Catmull-interpolated.** A Catmull curve can overshoot between legal control points. Then the display path samples the curve and additionally rounds it with `nvgQuadTo()`, which can visually cut corners around the rock even when sampled values are legal. The editor builds display values through `applyRockPush()` plus `applyRockClamp()`, but then renders a rounded body path on top of that sampled result.

## Immediate fixes I would make first

### 1. Replace `applyRockClamp()` with crossing-aware projection

Current logic should be changed from “is target inside rock?” to “does the vertical travel from base to target intersect the forbidden interval?”

Conceptually:

```cpp
float Wyrm::applyRockClamp(float base, float ph, float offset) const {
	float y0 = base;
	float y1 = clamp(base + offset, -1.f, 1.f);

	for (int i = 0; i < rockCount; ++i) {
		if (i == liftedRock) continue;

		float lower = 0.f;
		float upper = 0.f;
		if (!cachedRockBoundsAtPhase(i, ph, &lower, &upper)) {
			continue;
		}

		const bool y0Inside = (y0 > lower && y0 < upper);
		const bool y1Inside = (y1 > lower && y1 < upper);
		const bool crossesUp = (y0 <= lower && y1 >= lower);
		const bool crossesDown = (y0 >= upper && y1 <= upper);

		if (y0Inside || y1Inside || crossesUp || crossesDown) {
			if (y0 <= lower) {
				y1 = lower;
			}
			else if (y0 >= upper) {
				y1 = upper;
			}
			else {
				y1 = (std::fabs(y1 - lower) < std::fabs(upper - y1)) ? lower : upper;
			}
		}
	}

	return clamp(y1, -1.f, 1.f) - base;
}
```

This alone should stop the most obvious “slither clips through the rock” failure.

### 2. Stop constraining dragged rock center against the waveform

`constrainedRockValueForDrag()` currently constrains the **center** of the rock based on `baseWaveAtPhase()`, but the rock has vertical radius. So it can still overlap the waveform by half its height. Worse, if the actual goal is “rock pushes into waveform and waveform bends,” then constraining the rock center fights the intended behavior.

I would either remove this constraint entirely for `ROCK_MOUSE_DRAGS`, or make it a “lift mode only” behavior. Drag mode should move the rock freely and let the waveform resolver respond.

### 3. Fix `pushWavePointsOutsideRock()` side voting

Right now `pushWavePointsOutsideRock()` detects segment intersections, but it stores one `spanPreferUpper` for the whole affected span. If multiple segments intersect with different preferred sides, the last one wins and the entire neighborhood can flip to the wrong side.

Replace:

```cpp
bool forceSpan = false;
bool spanPreferUpper = false;
```

with a per-point side vote:

```cpp
std::array<int, kWyrmPointCountMax> sideVote {};
```

When a segment intersects:

```cpp
const int vote = preferUpper ? 1 : -1;
sideVote[i] += vote;
sideVote[(i + 1) % pointCount] += vote;
```

Then for each point near the rock:

```cpp
bool preferUpper = sideVote[i] > 0;
if (sideVote[i] == 0) {
	preferUpper = moduleOrHelperBaseWaveAtPhase(ph) >= rock.value;
}
pushPointOutsideRock(i, rock, preferUpper, true);
```

Also add the missing wrap segment from the final point back to point `0`. The current loop only handles `0 -> 1 ... pointCount - 2 -> pointCount - 1`, so a rock near phase wrap can miss the seam.

### 4. Fix the fallback guard scale

In `pushPointOutsideRock()`, the fallback branch uses:

```cpp
const float guard = rock.radiusValue + kWyrmRockClearance;
lower = rock.value - guard;
upper = rock.value + guard;
```

But the actual rock vertical radius elsewhere is scaled by:

```cpp
kWyrmRockValueScale * (rock.radiusValue + clearance)
```

So the fallback guard is inconsistent and probably too large. It should be:

```cpp
const float guard = kWyrmRockValueScale * (rock.radiusValue + kWyrmRockClearance);
```

That will reduce weird snap/discontinuity behavior near rock edges.

## Better redesign: one final rock resolver

The cleaner approach is to stop thinking of rocks as destructive edits to `wavePoints`. Treat rocks as **obstacle modifiers** evaluated consistently for audio and UI.

Create one core function:

```cpp
float Wyrm::resolveAgainstRocks(float anchorY, float desiredY, float ph) const;
```

Where:

* `anchorY` is the unslithered or previously resolved waveform position.
* `desiredY` is where the waveform wants to go after slither or editing.
* The function collects all active rock intervals at that phase.
* It projects `desiredY` to the nearest legal value without crossing a forbidden interval from `anchorY`.

Then audio becomes:

```cpp
float base = lookupWave(phase[c]);
float baseResolved = resolveAgainstRocks(base, base, phase[c]);

float desired = baseResolved + slitherOffset(phase[c], slitherPhase[c], slitherAmount);
float raw = resolveAgainstRocks(baseResolved, desired, phase[c]);
```

The editor display should call the **same function**, not an approximate visual version.

This gives you one sacred law:

> Any final waveform value shown or heard must pass through `resolveAgainstRocks()`.

No more “the UI thinks it is legal but audio thinks otherwise” spiral.

## How the resolver should work

Because each rock produces a vertical forbidden interval at a phase:

```cpp
lower = rock.value - edgeY;
upper = rock.value + edgeY;
```

you can solve this as a 1D projection problem.

Pseudo-code:

```cpp
float Wyrm::resolveAgainstRocks(float anchorY, float desiredY, float ph) const {
	float y = clamp(desiredY, -1.f, 1.f);

	for (int pass = 0; pass < 3; ++pass) {
		bool changed = false;

		for (int i = 0; i < rockCount; ++i) {
			if (i == liftedRock) continue;

			float lower = 0.f;
			float upper = 0.f;
			if (!cachedRockBoundsAtPhase(i, ph, &lower, &upper)) continue;

			const bool yInside = (y > lower && y < upper);
			const bool anchorInside = (anchorY > lower && anchorY < upper);
			const bool crossesFromBelow = (anchorY <= lower && y >= lower);
			const bool crossesFromAbove = (anchorY >= upper && y <= upper);

			if (yInside || anchorInside || crossesFromBelow || crossesFromAbove) {
				if (anchorY <= lower) {
					y = lower;
				}
				else if (anchorY >= upper) {
					y = upper;
				}
				else {
					y = (std::fabs(y - lower) < std::fabs(upper - y)) ? lower : upper;
				}
				changed = true;
			}
		}

		if (!changed) break;
	}

	return clamp(y, -1.f, 1.f);
}
```

The extra passes help when one rock pushes the wave into another rock.

## Display fix: stop rounded drawing near rocks

The current editor body path samples the waveform, then conditionally uses quadratic smoothing. Around rocks, that smoothing can visually cut into the rock. The sample points may be legal while the drawn curve between them is not.

Inside `emitRoundedBodyPath()`, add a helper:

```cpp
bool phaseNearAnyRock(float phase, float margin) const;
```

Then:

```cpp
if (phaseNearAnyRock(phase, 1.5f / bodySampleCount)) {
	nvgLineTo(args.vg, p1.x, p1.y);
}
else if (cornerCos >= roundCosThreshold) {
	nvgQuadTo(args.vg, p1.x, p1.y, midOut.x, midOut.y);
}
else {
	nvgLineTo(args.vg, p1.x, p1.y);
}
```

Around obstacle contact, use dense polyline, not rounded path smoothing. The wyrm may be graceful, but not through stone.

## Important visual clearance issue

The waveform stroke is up to about `4.0f` pixels wide in the editor body path, while the rock collision clearance is only `kWyrmRockClearance = 0.012f` in value units. If the centerline is mathematically outside the rock, the visible stroke can still overlap the rock edge.

For drawing only, inflate the rock clearance by half the waveform stroke width converted to value units:

```cpp
float strokeValueClearance = 2.f * (0.5f * maxStrokePx) / box.size.y;
float visualClearance = std::max(kWyrmRockClearance, strokeValueClearance);
```

Use that for display collision/rock edge testing, while audio can keep the smaller clearance.

## Recommended implementation order

1. Patch `applyRockClamp()` to detect crossing, not only final target inclusion.
2. Remove or disable `constrainedRockValueForDrag()` for drag mode.
3. Fix `pushWavePointsOutsideRock()` side voting and add wrap-segment handling.
4. Disable `nvgQuadTo()` smoothing near rocks.
5. Consolidate `applyRockPush()` and `applyRockClamp()` into one `resolveAgainstRocks(anchorY, desiredY, ph)` function.
6. Make both audio and editor display use that one resolver.

The deeper redesign is the one I would trust: **rocks should be constraints at evaluation time, not scattered mutations of the stored waveform.** Stored points remain the wyrm’s body; rocks become non-destructive terrain; slither becomes motion through that terrain; the resolver becomes the single physical law.
