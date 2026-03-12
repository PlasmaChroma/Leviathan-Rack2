# Integral Flux

## Unified Performance Optimization Master Plan

---

# 1. Executive Synthesis

Current CPU: ~5%
Reference (Rampage): ~1.1%

The gap is real but not mysterious.

Both audits converge on three dominant cost centers:

1. Transcendental math at audio rate (`std::pow`, `std::tanh`)
2. Recomputing timing laws every sample
3. Over-processing (BLEP, mixing saturation, CV scaling) when not strictly necessary

The good news:

* Your architecture is already clean.
* Warp caching was implemented correctly (huge win).
* There are no catastrophic design flaws.
* This is an optimization pass problem, not a redesign problem.

---

# 2. The True Performance Hierarchy

From highest impact to lowest:

### Tier 1 — Critical Path (Audio-Rate Transcendentals)

From both reports:

* `std::pow` inside `computeStageTime`
* `std::pow` inside warp shaping
* `std::tanh` inside mix non-ideal mode

Gemini quantifies it:

> ~8 `pow()` calls per sample 

Codex confirms the same structural hotspot 

This is almost certainly your primary 5% driver.

---

### Tier 2 — Redundant Recalculation

`computeStageTime()` recomputed every sample for both channels.

Codex flags this strongly 
Gemini independently flags the same issue 

Knobs and many CVs are not audio-rate signals.

This is wasted work.

---

### Tier 3 — BLEP on EOR/EOC

Codex correctly points out:

* BLEP running continuously even though gates are CV-level 

This is elegant DSP, but arguably unnecessary in a CV module.

---

### Tier 4 — Mixing Saturation

Gemini:

* `std::tanh` per bus per sample 

Codex:

* Suggests fast-path small-signal bypass 

Moderate impact, easy win.

---

# 3. Consolidated Optimization Strategy

We merge both documents into a three-phase roadmap.

---

# Phase A — Zero-Risk Immediate Gains

These do not change sound character meaningfully.

### 1. Replace std::pow with faster alternatives

Case breakdown:

* `pow(x, 2)` → `x * x`
* `pow(2.f, x)` → `dsp::approxExp2(x)`
* fixed exponent warps → inline multiply

This alone may cut 1–2% CPU.

---

### 2. Cache Timing Constants

Instead of:

```
computeStageTime() every sample
```

Do:

* Recompute only when:

  * knob changes
  * relevant CV changes beyond epsilon
  * shape affects timing law

Or:

* Use `dsp::ClockDivider` to update timing every 8–16 samples.

Both reports recommend this  

Expected gain: high.

---

### 3. Fast Tanh

Replace:

```
std::tanh(x)
```

With:

Option A (Rack built-in):

```
dsp::approxTanh(x)
```

Option B (polynomial approx):

```
x * (27 + x*x) / (27 + 9*x*x)
```

Or implement small-signal fast path as Codex suggests 

Expected gain: medium-high.

---

### 4. Add BLEP Toggle

Add context menu:

```
☐ Bandlimited EOR/EOC
```

Default: OFF.

When off → hard gate.

Codex flags this cleanly 

Expected gain: medium.

---

# Phase B — Structural Improvements

If Phase A still leaves you above ~2–3%:

### 5. Decimated Control-Rate Updates

Use `ClockDivider` for:

* stage timing laws
* CV scaling
* UI light brightness
* non-audio logic

This is how Rampage likely achieves low CPU 

Expected gain: high.

---

### 6. LUT for Warp

Warp is 1D shape vs phase.

Perfect LUT candidate.

2D table (shape × phase) with linear interpolation.

Expected gain: medium-high.

Risk: minor curve deviation if table too coarse.

---

# Phase C — Advanced (Only If Necessary)

### 7. SIMD for CH1 + CH4

Process both outer channels simultaneously via `simd::float_4`.

Gemini suggests this 

But only worth it if CPU still >2%.

---

# 4. What This Likely Achieves

Conservative expectation:

5% → 2–3% after Phase A.

With decimation:

Possibly ~1.5–2%.

Matching Rampage exactly may not be possible if you keep:

* non-ideal mix
* warp modeling
* extra realism layers

But you can likely cut CPU by half or more.

---

# 5. The True Strategic Question

Are you optimizing for:

A) Exact behavioral fidelity
or
B) Performance-per-instance density

Rampage is optimized for density.

Integral Flux currently leans toward modeling precision.

You can support both by:

* Performance mode toggle
* High-accuracy mode toggle

That’s a very Culture-level move.

---

# 6. Recommended Master Implementation Order

1. Replace pow/tanh
2. Cache stage timing
3. Add BLEP toggle
4. Benchmark
5. Add control-rate decimation
6. Benchmark again
7. Consider LUT only if still above target

Measure after every change.

Do not batch blindly.

---

# Final Assessment

Both audits are aligned.
They reinforce each other.
There is no contradiction.

The bottleneck is not mysterious.

It is transcendental math + unnecessary recomputation at audio rate.

You do not need to redesign the module.

You need to:

* Reduce math cost
* Stop recomputing constants
* Offer performance modes

If you'd like, I can now:

* Draft the formal “Integral Flux Performance Spec v1.0”
* Or write the exact patch diff plan for Phase A implementation

Your slope engine is already elegant.
Now we refine its efficiency until it flows.
