Dragon King Leviathan — yes. I did a static pass over Bifurx on the `expander` branch. I **haven’t run a local Rack profiler/build here**, so this is code-review guidance rather than measured CPU attribution.

## Verdict

Bifurx is already in pretty good shape. The big architecture choices are sensible: the DSP path caches coefficients, control updates are tiered, preview publishing is divided down, and the heavy FFT/curve work is mostly pushed into the UI/update side rather than living directly in the audio loop. The module is split across `Bifurx.cpp`, `Bifurx.hpp`, `BifurxGL.cpp`, and `BifurxUI.cpp`, with the module described as a dual-peak multimode filter with live response/spectrum preview. ([GitHub][1])

The remaining wins are mostly **small audio-thread cleanup** plus **UI/GL render-path cleanup**. I would not start by rewriting the core filter math.

---

## Highest-value safe optimizations

### 1. Gate unconnected slow-CV voltage reads

In `process()`, Bifurx correctly checks whether resonance, balance, and span CV inputs are connected, but then still reads and clamps those voltages unconditionally. That is tiny per sample, but it is in the hot path and costs nothing to clean up. ([GitHub][2])

Current shape is effectively:

```cpp
const bool resoCvConnected = inputs[RESO_CV_INPUT].isConnected();
const bool balanceCvConnected = inputs[BALANCE_CV_INPUT].isConnected();
const bool spanCvConnected = inputs[SPAN_CV_INPUT].isConnected();

const float resoCvNorm = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f;
const float balanceCvNorm = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f;
const float spanCvNorm = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f;
```

Recommended:

```cpp
const float resoCvNorm = resoCvConnected
    ? clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f
    : 0.f;

const float balanceCvNorm = balanceCvConnected
    ? clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f
    : 0.f;

const float spanCvNorm = spanCvConnected
    ? clamp(inputs[SPAN_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f
    : 0.f;
```

This preserves behavior exactly when CV is connected.

---

### 2. Reduce `sanitizeCoreState()` frequency

`process()` appears to sanitize both filter cores every sample before doing the rest of the DSP. That is robust, but it means repeated finite checks/clamps in the audio thread even when the filter is behaving normally. ([GitHub][2])

A safer optimization would be to run this on a small divider:

```cpp
if (++sanitizeCounter >= 64) {
    sanitizeCounter = 0;
    sanitizeCoreState(coreA);
    sanitizeCoreState(coreB);
}
```

Or better: use a cheap sentinel check first, then do the full sanitize only if a state exceeds a wide bound or becomes non-finite.

I would treat this as **medium-safe**, not completely free. Test it hard with high resonance, TITO modulation, extreme cutoff CV, and mode switching.

---

### 3. Collapse duplicated parallel-mode cases

The switch body has several mode cases that all do the same structural work: process A, process B, then combine via `combineModeResponse`. ([GitHub][2])

This is not going to magically halve CPU, but it can reduce branch/code footprint and make future compiler optimization easier:

```cpp
case 1:
case 2:
case 4:
case 5:
case 7:
case 8: {
    const SvfOutputs a = pA(excitation);
    const SvfOutputs b = pB(excitation);
    modeOut = combineModeResponse<float>(
        a, b, mode, lowWeight, bpWeight, highWeight, notchWeight, peakWeight
    );
} break;
```

This is low-risk cleanup.

---

## UI / visualization optimizations

### 4. Cache or refactor `calculateRefinedCurvePoints()`

This one looks more meaningful than most of the audio-thread micro-optimizations.

`calculateRefinedCurvePoints()` builds a vector of all 513 curve points, adds refinement points around the cutoff markers, sorts, uniques, and then evaluates Y values. That happens in the render path. ([GitHub][2])

Because the base X grid is stable, I would avoid rebuilding/sorting it every draw. Better options:

```cpp
// Conceptual direction
// 1. Precompute base x positions.
// 2. Insert the few marker-refinement points using a fixed small merge.
// 3. Recompute only when marker X positions, widget size, sample rate,
//    or preview sequence changes.
```

This should preserve visual quality while shaving GUI work, especially with multiple Bifurx instances open.

---

### 5. Batch OpenGL shader draws

The OpenGL path currently draws multiple primitive groups separately: fill, soft cap, cyan module response, and main curve. The shader helper binds/unbinds program state and buffer state per call, so there may be avoidable driver overhead. ([GitHub][3])

I would move toward:

```cpp
glUseProgram(program);

// upload or bind once
// draw fill
// draw soft cap
// draw cyan response
// draw curve

glUseProgram(0);
```

Even if you keep separate `glDrawArrays()` calls, avoiding repeated shader/buffer setup is likely cleaner.

---

### 6. Pre-reserve GL vectors

The GL renderer clears and repopulates vertex vectors for fill, soft cap, cyan response, and curve drawing. `clear()` keeps capacity once grown, but explicit `reserve()` avoids first-draw churn and makes intent clear. ([GitHub][3])

Rough reserves:

```cpp
fillVerts.reserve(3200);
softCapVerts.reserve(6500);
cyanVerts.reserve(600);
curveVerts.reserve(600);
```

Tiny win, very safe.

---

## Conditional / quality-risk optimizations

### 7. Add a tiny coefficient-change threshold for TITO mode

The fast path already uses cached coefficients when TITO, V/oct, FM, and slow CV conditions allow it. When TITO modulation is active, the code can fall back into dynamic coefficient work because cutoff values move per sample. ([GitHub][2])

A possible optimization:

```cpp
if (std::abs(std::log2(newCutoff / cachedCutoff)) > threshold) {
    cachedCoeffs = makeSvfCoeffs(newCutoff, damping);
    cachedCutoff = newCutoff;
}
```

Suggested threshold range:

```cpp
1e-4f to 3e-4f octaves
```

That is roughly sub-cent territory. It should be inaudible in most cases, but because Bifurx is a resonant filter with animated peaks, I would gate this behind a setting or test branch first.

---

### 8. Optional “Eco Visuals” mode

Bifurx currently uses a 513-point response curve and a 4096-point FFT with a 2048-sample hop. Those are reasonable quality choices, but if many Bifurx modules are open, the UI could dominate. ([GitHub][4])

An optional lower-cost visual mode could use:

```cpp
curve points: 513 -> 257
FFT size: 4096 -> 2048
FFT hop: 2048 -> 1024 or 2048
```

I would make this a context-menu option only. The current defaults are musically/visually nicer.

---

## What I would not optimize first

I would **not** start by replacing the SVF core, softclip, or the existing fast approximations. The code already uses fast math helpers such as `fastExp2`, `fastLog2`, `fastTan`, and `fastTanh`-based clipping, and it already has divider-based preview/control/perf scheduling. ([GitHub][4])

I would also leave the FFT architecture mostly alone unless profiling shows UI cost is the main pain. The audio thread publishes analysis frames, while the UI side handles curve cache and overlay cache updates when sequences change. ([GitHub][2])

---

## Recommended patch order

1. **Safe audio cleanup:** gate unconnected CV reads, collapse duplicated mode cases, reserve GL vectors.
2. **UI render cleanup:** cache/refactor `calculateRefinedCurvePoints()`.
3. **GL cleanup:** batch shader binds/draw setup.
4. **Experimental DSP optimization:** divider/sentinel for `sanitizeCoreState()`.
5. **Only if profiling demands it:** TITO coefficient thresholding or optional Eco Visuals mode.

After changes, I’d run the existing Bifurx tests, especially the filter/runtime specs listed in the Makefile, and do an A/B patch with high resonance, TITO active, fast FM, and aggressive mode switching. ([GitHub][5])

[1]: https://github.com/PlasmaChroma/Leviathan-Rack2/tree/expander/src "Leviathan-Rack2/src at expander · PlasmaChroma/Leviathan-Rack2 · GitHub"
[2]: https://github.com/PlasmaChroma/Leviathan-Rack2/blob/expander/src/Bifurx.cpp "Leviathan-Rack2/src/Bifurx.cpp at expander · PlasmaChroma/Leviathan-Rack2 · GitHub"
[3]: https://github.com/PlasmaChroma/Leviathan-Rack2/blob/expander/src/BifurxGL.cpp "Leviathan-Rack2/src/BifurxGL.cpp at expander · PlasmaChroma/Leviathan-Rack2 · GitHub"
[4]: https://github.com/PlasmaChroma/Leviathan-Rack2/blob/expander/src/Bifurx.hpp "Leviathan-Rack2/src/Bifurx.hpp at expander · PlasmaChroma/Leviathan-Rack2 · GitHub"
[5]: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/Makefile "raw.githubusercontent.com"
