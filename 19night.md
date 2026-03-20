# TemporalDeck — Nightly Findings
*Session: 19night | Source: TemporalDeck.cpp (current)*

Two problem areas investigated: **rate knob accuracy** and **manual scratch visual alignment**.

---

## Issue 1: Rate Knob Accuracy

### Root Cause: `prevReadHead` is `float`, `readHead` is `double`

This is the primary rate accuracy bug. `readHead` was promoted to `double` (line 305) to handle long buffer sizes without precision loss, but `prevReadHead` was never updated to match:

```cpp
// Line 745 — still float
float prevReadHead = readHead;   // silent narrowing from double to float
```

Every downstream calculation that depends on `prevReadHead` is therefore operating on a precision-truncated snapshot:

- `readDeltaForTone = readHead - prevReadHead` — the per-sample delta used for tone and motion calculation is quietly degraded. At 44.1kHz with a large buffer (8-min mode), a `double`→`float` cast loses ~7 significant decimal digits of position. This produces a jittery `readDeltaForTone` that oscillates around the true value, which feeds directly into `motionAmount` and the cartridge model.
- `readDelta = readHead - prevReadHead` in the visual section has the same problem — the platter phase accumulates small but consistent errors that compound over time, causing the visual to slowly drift from where it should be.

**Fix:** Promote `prevReadHead` to `double`:
```cpp
double prevReadHead = readHead;
```
And update the subsequent wrap-unwrap arithmetic to use `double` consistently:
```cpp
double readDeltaForTone = readHead - prevReadHead;
// ... wrap logic using double(buffer.size) ...
float motionAmount = clamp(float((std::fabs(readDeltaForTone) - 1.0) / 3.0), 0.f, 1.f);
```

---

### Secondary: `integrateHybridScratch` Takes `float limit` and `float newestPos`

The function signature (line 677) accepts these as `float`:

```cpp
void integrateHybridScratch(float dt, float limit, float newestPos, ...);
```

But at the call sites (lines 864, 883), `limit` and `newestPos` are both declared as `float` in `process()` (lines 755, 770), even though `accessibleLag()` returns `double` and `newestReadablePos()` returns `double`. They are silently narrowed on assignment:

```cpp
float limit    = accessibleLag(bufferKnob);   // double -> float truncation
float newestPos = newestReadablePos();         // double -> float truncation
```

Inside `integrateHybridScratch`, `readHead` (double) is then clamped and integrated using these truncated `float` values. For the 8-min buffer mode (~481s × 44100 = ~21 million samples), a `float` has only ~7 significant digits, meaning position precision degrades to roughly ±2 samples near the buffer midpoint and worse further out. This manifests as subtle rate wobble during long takes — the read head doesn't move at a perfectly constant rate because the clamp boundary is imprecise.

**Fix:** Promote `limit` and `newestPos` to `double` throughout `process()` and update `integrateHybridScratch`'s signature to match:
```cpp
void integrateHybridScratch(float dt, double limit, double newestPos, ...);
```
This also means the `candidate` computation inside `integrateHybridScratch` (currently `float`) needs to become `double`.

---

### Tertiary: Rate CV Adds Linearly to a Non-Linear Knob Scale

`computeBaseSpeed()` adds the CV contribution directly to the knob-derived speed:

```cpp
float speed = baseSpeedFromKnob(rateKnob);
speed += clamp(rateCv / 5.f, -1.f, 1.f);
```

`baseSpeedFromKnob` is piecewise-linear with a kink at center: `[0, 0.5)` maps to `[0.5x, 1.0x]` and `[0.5, 1.0]` maps to `[1.0x, 2.0x]`. The CV adds a flat ±1x on top of that. This means:

- At knob center (1.0x), CV range is 0.0x–2.0x — full symmetric sweep. Fine.
- At knob minimum (0.5x), CV pushes the range to -0.5x–1.5x — the lower end goes negative (reverse!) which is probably not intended from the rate CV input. Negative speed from rate CV means the delay reads backward, producing an unexpected reverse effect when the rate knob is turned down and a positive CV is patched.
- At knob maximum (2.0x), CV can push to 1.0x–3.0x, but the `clamp(-3, 3)` cap truncates the upper end.

The more fundamental issue: the CV is adding in "speed units" but the knob operates in a compressed scale. A 1V CV shift at 1.0x speed feels like the same amount as a 1V shift at 0.5x, but it isn't — at 0.5x it's a 200% change, at 1.0x it's a 100% change. Users will notice the CV has very different effective range depending on where the rate knob sits.

**Recommendation:** Either document this as intentional (it does have practical uses for pitch-bending), or apply CV in the knob's parameterization domain (before `baseSpeedFromKnob`) so the same CV voltage always represents the same knob-equivalent displacement. A clean approach is to treat the knob position as a normalized value and add CV as an offset to that, then call `baseSpeedFromKnob` on the composite:

```cpp
float composite = clamp(rateKnob + clamp(rateCv / 10.f, -0.5f, 0.5f), 0.f, 1.f);
float speed = baseSpeedFromKnob(composite);
if (reverse) speed *= -1.f;
```
This maps ±5V to ±half the knob range, which feels natural and prevents accidental negative speeds from CV alone.

---

### Minor: `DeckRateQuantity` Display Doesn't Reflect CV

The tooltip/display string for the rate knob (line 1348) only reads the knob's own value:

```cpp
return string::f("%.2fx", TemporalDeckEngine::baseSpeedFromKnob(getValue()));
```

It doesn't account for the rate CV input. A user patching rate CV sees the knob display say "1.00x" while the actual running speed is different. This is a cosmetic/UX issue rather than a functional bug, but it contributes to the perception that the rate isn't working right.

---

## Issue 2: Manual Scratch Visual Alignment

### Root Cause: Grooves and Label Rotate at Different Rates

The draw code has a split rotation factor that has probably been the source of visual confusion for a while:

```cpp
// Line ~1871 — grooves
nvgRotate(args.vg, rotation * 0.92f);   // 8% slower

// Line ~1913 — label and spoke marks
nvgRotate(args.vg, rotation);            // full speed
```

The grooves and the label are rotating at different speeds. This means they visually drift apart as the platter turns — the groove pattern slowly rotates relative to the center label. On a real vinyl record, grooves and label are the same physical object. The `0.92f` damping was presumably added to make the groove visual feel less "spinny," but the side effect is that the two layers decouple and the record looks physically impossible. During a fast scratch this is most obvious: the center label snaps back and forth at full speed while the groove pattern lags behind, giving the impression that the record is made of two independently rotating pieces.

**Fix:** Either rotate both layers at the same rate, or ditch the layer split and apply a single scale factor to the whole platter group. The simplest version:

```cpp
// Apply the same rotation to both, with a consistent optional scale
constexpr float kVisualDampening = 1.0f; // set to 0.92 if desired, but apply to BOTH
nvgRotate(args.vg, rotation * kVisualDampening); // grooves
// ...
nvgRotate(args.vg, rotation * kVisualDampening); // label
```

If the `0.92f` was intentional for a "slipping platter" aesthetic, make it a named constant and apply it uniformly. As written, it's an accidental divergence.

---

### Secondary: Visual Delta Scale Mismatch During Manual Scratch

In `process()`, the visual delta for manual scratch is:

```cpp
float gestureDelta = platterGestureVelocity * dt;
```

`platterGestureVelocity` is set in `updateScratchFromLocal()` as:

```cpp
float velocity = tangentialPx * module->uiSampleRate.load() * 0.0007f * sensitivity * radiusRatio;
```

This velocity is in "samples per second" (loosely — it's a pixel-space heuristic, not a true physical velocity as noted in the previous review). When multiplied by `dt` (seconds), `gestureDelta` becomes a value in "samples." It is then fed into:

```cpp
platterPhase += visualDelta * platterRadiansPerSample();
```

`platterRadiansPerSample()` returns radians per buffer-sample at 33.3 RPM. So `gestureDelta` must be in buffer-samples for this to produce the correct angular displacement. The problem is that `gestureDelta` is in pixel-derived heuristic units, not actual buffer-sample displacement — the `0.0007f` constant makes it approximately right at 44.1kHz center sensitivity, but it doesn't scale correctly with sample rate and does not match the actual movement of the read head.

The result: **the platter graphic's rotation speed does not match the lag-change rate.** Moving your mouse 1cm across the platter rotates the graphic by some visually arbitrary amount that doesn't correspond to how much the vinyl label would actually have turned to produce that lag change.

**The correct approach** is to drive the platter visual from the same unit as the lag motion, not from a re-derived pixel velocity. The `lagDelta` computed in `updateScratchFromLocal()` is already in samples and is physically calibrated via `samplesPerRadian`. So we can derive the correct visual angle change directly:

```cpp
// In updateScratchFromLocal(), after computing lagDelta:
// lagDelta = deltaAngle * samplesPerRadian
// Therefore: deltaAngle = lagDelta / samplesPerRadian
// And: lagDelta / samplesPerRadian == deltaAngle (which we already have)

// Pass deltaAngle to the engine, not a heuristic velocity
module->setPlatterScratch(true, localLagSamples, deltaAngle / dt);
// Then in process(), gestureDelta = (deltaAngle/dt) * dt = deltaAngle — correct angular units
```

Alternatively, keep the current approach but fix the visual section to use `readDelta` (the actual buffer-sample displacement) rather than the gesture velocity for the platter phase, since `readDelta` is already in the right unit and represents the ground truth of what the audio read head did:

```cpp
// During manual touch scratch, just use readDelta for visual too.
// The smoothing in the audio path is subtle enough that readDelta
// is already a reasonable visual proxy.
visualDelta = readDelta; // drop the gesture blend entirely
```

This is the lower-risk fix and avoids the unit translation problem.

---

### Tertiary: Visual Branch Doesn't Cover Wheel Scratch

The special `manualTouchScratch` visual branch (lines 1268–1282) only fires for touch scratch, not wheel scratch. During wheel scratch the visual falls through to use raw `readDelta`, which comes from the smoothed hybrid motion model. The wheel scratch glide is deliberately slow, so `readDelta` during wheel input underrepresents how aggressively the user scrolled. The platter barely moves on a big wheel flick.

There's no `manualWheelScratch` branch in the visual section, so wheel scratch hits the default `visualDelta = readDelta` path. The same blend logic that compensates for touch scratch latency would help here:

```cpp
} else if (wheelScratch) {
    // For wheel scratch, the lag target is set discretely;
    // use scratchMotionVelocity (which is the integrated response) for the visual.
    float platterModelDelta = scratchMotionVelocity * dt;
    visualDelta = platterModelDelta;
}
```

---

## Summary Table

| # | Severity | Issue | Area |
|---|---|---|---|
| 1 | 🔴 BUG | `prevReadHead` is `float`, `readHead` is `double` — silent precision loss | Rate / Audio |
| 2 | 🟠 WARNING | `limit` and `newestPos` narrowed to `float` in `process()` — precision degrades on large buffers | Rate / Engine |
| 3 | 🟠 WARNING | Grooves and label rotate at different rates (`* 0.92f` vs `* 1.0f`) | Visual |
| 4 | 🟠 WARNING | Rate CV adds linearly to non-linear knob; can unexpectedly produce reverse at low rate settings | Rate |
| 5 | 🟠 WARNING | `gestureDelta` for visual is in pixel-heuristic units, not platter-radians — graphic motion doesn't track lag delta | Visual |
| 6 | 🔵 NOTE | Wheel scratch has no visual compensation branch — platter barely moves on scroll flick | Visual |
| 7 | 🔵 NOTE | `DeckRateQuantity` display ignores CV — tooltip shows wrong speed when CV patched | UX |

---

## Recommended Fix Order

1. **`prevReadHead` → `double`** — one-line change, fixes the foundational precision issue that affects both audio quality and visual accuracy simultaneously.
2. **Groove/label rotation unification** — one-line change, immediately fixes the most visible split-layer artifact.
3. **`limit` and `newestPos` → `double` in `process()` + `integrateHybridScratch` signature** — small but important for 8-min buffer correctness.
4. **Visual delta: use `readDelta` for manual touch scratch** — drops the heuristic gesture blend and lets the audio ground truth drive the graphic.
5. **Rate CV domain fix** — medium complexity, reconsider whether CV should add in speed-units or knob-units.
6. **Wheel scratch visual branch** — straightforward addition following the touch scratch pattern.
