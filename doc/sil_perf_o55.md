# Sil — Performance Review (o55)

_Scope: `src/Sil.cpp`, `src/SilLevelMeter.hpp`, `src/SilLimiterPeakWindow.hpp`, `src/SilSpectrumSnapshot.hpp` as of `911de8bd` (2026-10-11)._
_Goal: make Sil cheaper without a noticeable quality change. Every audio-thread claim below was measured on a standalone replica of `Sil::process()`. Each change was A/B'd for speed and null-tested against the shipping output._

---

## 0. TL;DR

The audio thread is **not** dominated by the limiter, the FFT, or the EQs. It is dominated by **per-sample decision math**, about 14 `log10f`, 1 `powf` and 3 `sqrtf` per sample. That math feeds envelopes and gain smoothers whose time constants are 4 ms to 3.5 s, so it does not need to run every sample. The second surprise is **`clamp()`**. Rack defines it as `std::fmax(std::fmin(x,b),a)`, and under Rack's flags that compiles to two **libm function calls**, not `minss/maxss`. Sil does this 32 times per sample.

| # | Change | Measured speed (ns/sample, lower is better) | Quality vs. shipping (null depth) | Effort | Risk |
|---|---|---|---|---|---|
| **A1** | Detector/decision math at control rate (÷16) | 379 → 185 (**2.05×**) | −75.8 dB (≈0.0014 dB gain error) | M | Low |
| **A2** | `clamp()` → `std::min/std::max` helper | 379 → 334 (**1.13×**) | rounding noise only (−94 dB) | S | None (NaN-only semantics) |
| **A3** | 4-lane SIMD detector banks (mud / mid / stereo) | +8% on top of A1 | −88 dB alone | M | Low |
| **A4** | Precomputed one-pole coefficients (no per-sample divide) | ~+5% | −89 dB | S | None (if cutoffs preserved, see S1) |
| **A5** | Limiter metric: `log10` only when above ceiling | +2–4% | **exact** | XS | None |
| **A6** | Rolling program RMS → fixed block ring | ~0% speed; **3.84 MB → 12 KB per instance** | inaudible (feeds a 1024-sample threshold update) | S | None |
| **A7** | Biquad design cache (cos/sin hoisted, L/R shared, skip if unchanged) | ~+1–2% | −89 dB | S | None |
| **A8** | Idle fast path after 15 s of digital silence (freeze state) | ~100% while unpatched or silent | matches continued silence (residual ≤1.4% of slowest smoother) | S | Low |
| **A9** | Debug timer: time blocks, not samples | removes ~20–30% **measurement** inflation | n/a | XS | None |
| — | **All of A1–A5 + A6 + A7** | 379 → **129 ns/sample (≈2.9×)** | −77 dB | — | — |

Tested and **rejected**: polyphase rewrite of the true-peak upsampler (slower), dropping the Hermite detector (audible limiter change, −33 dBFS diff), and neutral-EQ bypass (no measurable gain). See §4.

There is also a **correctness finding** that any refactor of the detector filters must handle deliberately. Every `dsp::RCFilter` in Sil is tuned 2π× too low. The "120 Hz" low-band split is really at 19 Hz, and the "180–520 Hz" mud detector is really at 29–83 Hz. See §6, S1.

---

## 1. Method

**Replica bench.** I built a standalone C++ file with a line-for-line port of `Sil::process()`, including the full chain, the level meter, the histogram and spectrum ring writes, and the limiter peak window. It also contains exact copies of Rack v2's `RCFilter`, `Upsampler<4,8>`, `exp2_taylor5` and `clamp`. Each proposal is a compile-time flag on the same template (`SilCore<Flags>`), so the baseline is literally the same code with every flag off.

**Program material.** The bench feeds 20 s of a deterministic synthetic mix through the chain. The mix has kick, two-note bass, decorrelated L/R ~470 Hz "mud" noise, hats, a pad, and a slow 0.2 Hz swell so every stage moves. The baseline peaks at 4.46 V, so the saturator and limiter are both working.

**Metrics**
- *ns/sample*: best of 15 runs, variants interleaved round-robin, pinned to one core. Compiled with Rack's plugin flags: `-O3 -funsafe-math-optimizations -march=nehalem`.
- *Ir/sample*: callgrind instruction count inside `process()`. This is deterministic and useful when wall-clock noise (±5–10% on the VM) hides small effects.
- *Null depth*: `10·log10(Σ(opt−base)² / Σbase²)` over both channels, skipping the first second. *Peak diff* is the largest single-sample difference in dBFS, with 5 V as full scale.

**Caveats.** Numbers are from a Linux/glibc Xeon at 2.1 GHz. Your Windows build links mingw-w64's libm, where `log10f`/`powf` are generally slower than glibc's. **The A1/A2/A5 wins should be equal or larger on your machine.** Absolute ns will differ, but the ratios are what matter. The bench excludes Rack's per-module engine overhead (port I/O, param reads).

---

## 2. Where the time goes (shipping code, per sample)

| Work | Count / sample | Notes |
|---|---|---|
| `log10f` (`toDbSafe`/`toDbFsSafe`/glue level) | **~14** | impact air 1, mud 2, mid-enhance 3, glue 1, stereo 5, limiter metric 1, plus a few inside knees |
| `powf` | 1 | glue `10^(dB/20)` every sample |
| `sqrtf` | 3 | low-band correlation, glue level, mid-enhance LED |
| `fminf`/`fmaxf` **libm calls** | ~64 | 32 `clamp()` sites in `process()` × 2 calls each |
| `dsp::RCFilter::process` (divide in recursion) | 24 | 4 low-band, 6 mud, 6 mid, 1 glue, 8 stereo, minus the shared sidechain |
| Biquad (TDF-II) | 8 | impact-air L/R, mud L/R, mid L/R, stereo M/S |
| `Upsampler<4,8>::process` | 2 | 32 MACs each, true-peak detector |
| Hermite 4× causal | 2 | 3 evaluations each |
| `StereoLevelMeter` (double) | 4 double biquads | K-weighting L/R |
| Coefficient redesign (`pow`+`cos`+`sin`[+`sqrt`]) | 7 per 32 samples | mid-enhance on ÷64. L and R are designed **separately with identical inputs** |

callgrind on the baseline puts **~25% of all instructions inside `log10f`/`logf`/`powf` alone**, before counting the call overhead and surrounding arithmetic. With A1+A2 applied, the remainder is spread across the RC detector banks (≈1/3), Hermite + FIR detector (≈1/6), the double-precision meter (≈6%) and the biquads.

---

## 3. Recommendations: audio thread

_Code-level detail for every item (where it goes in `Sil.cpp`, what to watch for, how to verify it) is in §8._

### A1. Move decision math to control rate (÷16) — the big one

**What runs per sample today.** Every sample, each stage converts its envelopes to dB, runs one or more `softKnee01`, and computes a *target* (lift/cut dB). Then a one-pole smoother slews toward that target. The fastest smoother among the adaptive stages is the impact-air gain attack at 10 ms (480 samples at 48 kHz). The fastest envelope is the impact-air detector attack at 4 ms (192 samples). Updating the targets every 16 samples (0.33 ms) is invisible to those time constants.

**Stays per sample** (cheap, and needed for correct envelope dynamics):
- every RC detector filter and every `|x|` attack/release envelope
- every one-pole gain smoother (`smoothedLiftDb`, `smoothedCutDb`, `gainReductionDb`, …) running toward the last target
- all audio-path biquads, the saturator shaper, the limiter

**Moves to the control tick** (`if (++crCount >= 16)`):
- Impact air: `transientDeltaDb`, `softKnee01` → `targetLiftDb`
- Remove mud: `toDbSafe(mudEnv) − toDbSafe(refEnv)` → `targetCutDb`
- Mid enhance: level gate, deficit, presence guard, limiter backoff → `targetLiftDb`, plus the `sqrt` LED
- Glue: `levelDb`, knee → `targetGrDb`. The GR one-pole stays per sample. **The `pow` moves to the tick**: compute `gainLinNext` there and ramp linearly over the block.
- Stereo enhance: all 5 dB conversions and 4 knees → `targetMidCutDb`, `targetSideLiftDb`, plus the LED
- (Optional) Low-band correlation → `targetLowSideGain`. It has a 100 ms τ and sits behind a 50 ms attack.

Glue pattern (what the bench measured):

```cpp
if (cr) gTargetGr = glueTargetGrDb();                         // log10 + knee, 1/16 rate
gGr = gTargetGr + (gTargetGr > gGr ? glueAttackCoeff : glueReleaseCoeff) * (gGr - gTargetGr);
if (cr) {                                                      // pow once per block
    const float mk = clamp(gGr * kGlueMakeupFraction, 0.f, kGlueMaxMakeupDb);
    gGainPrev = gGainNext;
    gGainNext = std::pow(10.f, (-gGr + mk) / 20.f);
    gGainStep = (gGainNext - gGainPrev) * (1.f / kCR);
}
const float gain = gGainPrev; gGainPrev += gGainStep;          // linear ramp, no zipper
```

**Divider sweep** (A1 alone, min/max clamp build):

| Divider | ns/sample | Peak diff | Null depth |
|---|---|---|---|
| 4 | 192.7 | −78.9 dBFS | −85.1 dB |
| 8 | 179.8 | −74.6 dBFS | −81.3 dB |
| **16** | **161.8** | −67.9 dBFS | −75.8 dB |
| 32 | 161.5 | −60.3 dBFS | −69.8 dB |
| 64 | 164.7 | −51.3 dBFS | −63.5 dB |

**16 is the knee of the curve.** Going past it gains no speed, because once the logs are gone the per-sample filters dominate. Use **8** if you want a more conservative null. Either way the residual is a smooth gain-trajectory difference of about 0.001–0.002 dB, not noise or distortion.

LED values computed on the tick are also fine. `lightDivider` already samples them every 32 samples.

### A2. Replace `clamp()` in the hot path with a min/max helper

```cpp
static inline float clampf(float x, float lo, float hi) { return std::min(std::max(x, lo), hi); }
```

Rack's `clamp` uses `std::fmax/fmin` for NaN-propagation semantics. Without `-ffinite-math-only`, GCC must honour that, so it emits `call fmaxf` / `call fminf`. In the bench binary that was **749 call sites**. `-funsafe-math-optimizations`, which Rack uses, does not include `-ffinite-math-only`.

- Measured: **379 → 334 ns/sample (1.13×)**, output differs only by rounding noise (−94 dB null; GCC reassociates one product once `fmax`/`fmin` calls disappear, measured on the real `Sil.cpp`).
- Semantic difference: only when an input is NaN. If a NaN ever reaches Sil's chain, the output is already garbage either way.
- This is **plugin-wide**: `grep` finds 1,486 `clamp(` calls across `src/`. A `leviathan::clampf` in a shared header, swapped in on hot paths, would pay off in every DSP module. Alternatively, add `-fno-trapping-math -ffinite-math-only` per object for DSP-heavy files, but that changes `isnan`/`isfinite` behaviour, so prefer the helper.

### A3. 4-lane SIMD detector banks

Each adaptive stage runs N independent bandpass detectors (HP→LP) from **the same input**, then N identical envelope followers:

| Stage | Lanes | Input per lane |
|---|---|---|
| Remove mud | mud, bass, presence, (spare) | mono |
| Mid enhance | lowRef, core, presence, (spare) | monoPostMud |
| Stereo enhance | mid350, midBroad, side6k, sideBroad | mid, mid, side, side |

Each stage becomes one `float_4` HP, one `float_4` LP, and one vector envelope (`abs`, compare, select coefficient, FMA). That turns 22 scalar one-poles plus 10 scalar envelopes into 6 vector one-poles plus 3 vector envelopes. Rack's `simd::float_4` is the natural type. The bench used GCC vector extensions with identical math.

- Measured: T2 at 154.8 ns becomes **T2+SIMD at 143.1 ns** (shipping clamp), and 141.1 → 129.4 with A2.
- Null −88 dB on its own. The only difference is rounding, from using the A4 coefficient form.
- Fold A4 in here: the vector one-pole stores `k = 1/(1+c)` and `m = (1−c)/(1+c)`.

### A4. Precompute one-pole coefficients (no per-sample divide)

`dsp::RCFilter::process` computes `(x + x₁ − y₁(1−c)) / (1+c)`, a dependent divide in a recursive loop, 24 times per sample. Rewriting it as `y = k(x+x₁) − m·y₁`, with `k`, `m` set in `setCutoff`, removes the divide from the loop-carried chain. Measured ~+5–7% alone, null −89 dB. **Keep the exact same `r` argument** so the effective cutoffs (see S1) don't move.

### A5. Limiter metric: only take the log when it can be non-zero (exact)

```cpp
const float limiterDemandDb = std::max(0.f, toDbFsSafe(detectorPeak) - kLimiterCeilingDb);
```

`limiterDemandDb` is 0 whenever `detectorPeak ≤ 5·10^(−1/20)`, which is most samples on most material. Precompute that linear threshold and skip the `log10f` below it. **Bit-identical output**, 2–4%.

The same trick generalises to any `softKnee01(toDb(ratio), th, knee)`: compare `ratio` against `10^((th±knee/2)/20)` and only take the log inside the knee. It is exact, but once A1 is in place it buys little (measured ~1%). Only use it for anything you decide must stay per-sample.

### A6. Rolling program RMS → fixed block ring (memory, not speed)

`rollingBufferL/R` hold **10 s of stereo float samples**: 3.84 MB per instance at 48 kHz, 7.7 MB at 96 kHz. They exist only to answer one question every 1024 samples: what is the mean-square of audible mono over the last 10 s? Replace them with a ring of `(sumSq, count)` blocks:

- Use **1024 blocks of `sr·10/1024` samples** (≈9.8 ms each). The block count is then fixed, so there is **no reallocation in `onSampleRateChange`**. Today `configureRollingBuffer` does `vector::assign` on the engine thread.
- Footprint drops from 3.84 MB to ~12 KB per instance. The bench shows no speed change for one instance, because the streaming access prefetches well. What you get is less L2/L3 eviction of *other* modules' working sets in a large patch, plus no MB-scale allocation on rate change.
- The window boundary is quantised to ~10 ms on a 10 s window that feeds a threshold updated every 21 ms. Inaudible.

(The saturator's percentile history already uses this pattern, with 1000 fixed bins.)

### A7. Biquad design cache

The four adaptive EQs have **fixed centre frequency, Q and sample rate**. Only the gain changes. Today each tick recomputes `pow`, `cos` and `sin` (plus `sqrt` for the shelf), and does it **twice** because the L/R designs are identical.

- Cache `cos(w0)` and `alpha` per stage at sample-rate change. On a tick, compute `A = 10^(g/40)` once, write L, and copy to R.
- Skip the redesign when `g == lastG` (exact float compare). Idle stages sit at exactly their resting value once their smoother converges.
- Measured ~1–2%. It is low on the list, but it is the right shape and it makes A1's ticks cheaper.

### A8. Idle fast path (silence or unpatched)

When both inputs are disconnected, Rack feeds 0 V, and Sil runs the whole chain on zeros forever. After 15 s of **exact digital silence** (`inL == 0 && inR == 0`), every adaptive history has drained: the rolling RMS window and the saturator percentile bins hold no audible entries, the delay lines are zero, and the slowest smoother (saturator drive release, τ = 3.5 s) is within e^(−15/3.5) ≈ 1.4% of its silence asymptote. So:

- Count consecutive silent samples. Past 15 s, **freeze** the state and enter idle. Do *not* reset to defaults: a reset differs from continued silence. For example, the low-band side gain decays to 0 in silence, while a reset puts it back at 1.
- In idle: output 0, keep `levelMeter`/histogram/spectrum pushes (cheap), keep stepping `masteringMix`, and skip the chain.
- Leave idle on the first non-zero input sample, and process that same sample through the full chain. The frozen state is what continued silence would have produced, to within the residue above.

This saves effectively 100% of DSP for Sils parked in a patch. It does not change behaviour when signal is present. Keep the warm A/B design untouched: this keys on *silence*, not on the mastering switch.

### A9. Fix the debug timer so profiling Sil doesn't distort it

With Dragon King debug on, `process()` calls `steady_clock::now()` twice **per sample**. On Windows that is QueryPerformanceCounter, roughly 20–30 ns each. Against an optimised ~130 ns `process()`, the measurement itself is a 30–45% inflation, which will hide the wins above. Time one sample in every 64 (or accumulate across a 256-sample window and divide) and report that.

---

## 4. Tested and rejected

| Idea | Result | Verdict |
|---|---|---|
| Contiguous polyphase rewrite of `Upsampler<4,8>` (phase-major kernel, doubled history) | 0.94× (**slower**); GCC already unrolls Rack's Q=8 loop well | Skip |
| Drop the Hermite 4× detector (keep the FIR) | ~1.03×, but **peak diff −33 dBFS / null −46 dB**. Hermite catches peaks the FIR doesn't, so the limiter behaves differently | Reject |
| Neutral-EQ bypass with hysteresis | 0.98–1.02× (noise). The biquads are cheap and you'd add state-reset subtlety | Skip |
| Integer `%` → compare-wrap in the two delay lines | within noise | Optional hygiene only |
| Control divider > 16 | no further speed | Use 8–16 |

---

## 5. Recommendations: UI thread

The UI side is already in good shape: framebuffer caches, a demand-driven FFT, the stagger coordinator, and lock-free snapshots. What's left is incremental.

**U1. Histogram raster: stop shifting the whole image.** The histogram texture is 1000 × ~45 px RGBA (≈180 KB at the panel's 15.1 mm height). Every 30 Hz refresh does a full-image `memmove` to scroll left by about 3 columns, then a full `nvgUpdateImage`. Instead:
- Treat the texture as a ring. Write only the `advance` new columns at `writePtr % width`, and draw with `nvgImagePattern(..., ox = −offset)` plus `NVG_IMAGE_REPEATX` so the scroll is a UV offset. That removes the 180 KB `memmove` per refresh.
- Hoist the colour out of `writePixel`. `drawSpan` computes one colour per span but `writePixel` re-multiplies and clamps it per pixel. Precompute the 16 bucket colours as packed `uint32` once per palette change.
- Optional: upload only the dirty column range with `glTexSubImage2D` via `nvglImageHandleGL2`. That cuts the upload from 180 KB to ~540 B per refresh (5.4 MB/s → ~16 KB/s per instance).

**U2. Consider dropping the `FramebufferWidget` around the histogram.** Its content is already a cached texture drawn as one quad. The FBO adds a render-to-texture pass every refresh, plus an FBO allocation at zoom × pixel-ratio size. Measure with the existing `histogram_fb_us` CSV column first. If FBO time exceeds the inner draw, remove the wrapper. The stage-history widget should **keep** its FBO because of its 16 text draws.

**U3. Uncached text widgets.** `SilHorizontalMeter` (×2) and `SilBoostReadout` re-run `nvgText` (plus `snprintf` and a gradient fill for the meters) every frame. Cache the formatted string and only `snprintf` when the displayed tenth changes, as `SilBoostReadout` already does. Optionally wrap them in a framebuffer that is dirtied when the string or the bar's integer pixel width changes. This is small, but it is per frame, per instance.

**U4. Spectrum.** The FFT runs only when the left spectrum's framebuffer re-renders (≈24 Hz, and only when visible). That is two 2048-point real FFTs plus 256 `log10f`, around tens of µs. Leave it. On the audio side, the 16 KB snapshot copy every ~2000 samples is about 8 B/sample, which is negligible.

**U5. Trivial.** In `SilWidget::draw`, move `dynamic_cast<Sil*>(module)` below the `measurePerf` early-out.

---

## 6. Side findings that intersect this work (not perf)

**S1. Every `dsp::RCFilter` cutoff is 2π (≈6.28×) lower than its constant says.** Rack's `RCFilter::setCutoff(r)` takes **angular frequency in radians/sample**. `setCutoffFreq(f)` is the one that takes `f/fs`. Sil passes `hz / sampleRate` to `setCutoff`. Verified numerically against the filter's actual −3 dB point at 48 kHz:

| Constant | Intended | Actual −3 dB |
|---|---|---|
| `kLowBandCutoffHz` | 120 Hz | **19.1 Hz** |
| `kMudLowHz` / `kMudHighHz` | 180–520 Hz | **28.6–82.8 Hz** |
| mud ref "bass" | 80–160 Hz | 12.7–25.5 Hz |
| mud ref "presence" | 700–3000 Hz | 111–477 Hz |
| `kStereoSide6kLowHz`–High | 4.2–9.5 kHz | 668 Hz–1.51 kHz |
| `kStereoSideBroadHighHz` | 12 kHz | ~1.9 kHz |

So "Low recovery" only collapses side content below ~19 Hz, and the mud detector is listening to the bass region. This also explains why some tuning constants (`kStereoMidBandNormDb = 7.5`, etc.) needed large normalisation offsets. **This is a voicing decision, not a perf item.** Fixing it (switch to `setCutoffFreq`) will change what every stage reacts to and needs a retune pass against `doc/Sil/sil_tuning.md`. The perf refactors above (A3/A4) must **preserve the current `r` values** so they can land independently. Recommend tracking the fix separately.

**S2. `stereoEnhance.coeffsNeutral` is computed but never read.** Either use it (it is the hook the old neutral-bypass idea needed) or delete it.

---

## 7. Suggested order and acceptance

1. **A2 + A5 + A9.** Mechanical changes with bit-identical or exact output. A9 first, so in-Rack measurements are trustworthy.
2. **A1 at ÷16.** Biggest win. Gate it on the null test below.
3. **A4 → A3.** Restructure the three detector banks into `float_4`, preserving cutoffs.
4. **A6, A7, A8.** Memory and idle hygiene.
5. **U1–U3** as UI time allows.
6. S1 as its own tuning project, after perf lands.

**Acceptance test.** Add a headless spec alongside `tests/sil_boost_spec.cpp` that renders 20–30 s of mixed program material and compares against a golden render from the pre-change build:
- null depth ≤ −70 dB, and peak diff ≤ −60 dBFS, after a 1 s settle
- all 8 stage LED values within 0.03 of golden (sampled at the light divider)
- `limiterRecentGrDb`, `saturator.makeupDb`, `glueAdaptiveThresholdDb` within 0.01 dB at end of render
- 44.1 / 48 / 96 kHz

For in-Rack timing, compare `process_us` from the existing timing CSV, after A9, with one Sil and then 8 Sils, before and after.

---

## 8. Implementation notes

Line numbers refer to `src/Sil.cpp` at `911de8bd`. The items are ordered to match §7, so each step leaves a shippable build. Run the §7 null test after every step, not just at the end. That makes a regression attributable to one change.

### 8.0 Prep: a golden render before touching anything

1. Copy `tests/sil_boost_spec.cpp`'s harness (it already does the `SIL_HEADLESS_TEST` include trick and drives `process()` directly) into `tests/sil_perf_golden_spec.cpp`.
2. Feed it a deterministic program: the generator in the bench (`makeProgram`, kick, bass, mud noise, hats, pad, slow swell) is ~20 lines and exercises every stage. Also add one quiet pass (scale by 0.05) so the glue threshold leaves its −8 dB clamp, and one pass with only L patched.
3. Pre-change: write the L/R output and the 8 LED values (every 32 samples) to `build/tests/sil_golden_<sr>.bin` for 44.1/48/96 kHz.
4. Post-change: compare. Fail on null > −70 dB, peak diff > −60 dBFS, any LED Δ > 0.03. A2 and A5 should land at rounding-noise level (about −94 dB), which catches accidental behaviour changes early.

### 8.1 A9: block-safe debug timing (NOT implemented, see §9)

`process()` line 1088 and line 1695. Replace per-sample timing with a sampled one:

```cpp
// members
uint32_t perfSampleCounter = 0;
static constexpr uint32_t kPerfStride = 61;   // prime: never phase-locks to the ÷16/32/64/512/1024 dividers

// top of process()
const bool measurePerf = isDragonKingDebugEnabled() && (++perfSampleCounter >= kPerfStride);
if (measurePerf) perfSampleCounter = 0;
```

- **The stride must not be a multiple of any divider.** With A1 in place, 1 sample in 16 is a "tick" sample that costs several times more than the rest. A stride of 64 would always land on the same phase and report either only tick samples or none. 61 cycles through all phases.
- `steady_clock::now()` itself costs 20–40 ns on Windows. Measure it once at construction (time 1000 back-to-back `now()` pairs, take the min) and subtract that from each reading, or the optimised numbers will look worse than they are.
- `debugMetrics.recordProcess()` now sees 1/61 of the samples. Mean and percentiles stay valid. A "max" over a window becomes a sampled max, so treat it as indicative.

### 8.2 A2: `clampf` helper

Add near the top of `Sil.cpp` (or in a shared `src/FastMath.hpp` if you want it plugin-wide):

```cpp
namespace sil {
// std::min/max lower to minss/maxss. Rack's clamp() uses std::fmin/fmax, which are
// libm calls under -funsafe-math-optimizations (no -ffinite-math-only).
static inline float clampf(float x, float lo, float hi) { return std::min(std::max(x, lo), hi); }
}
```

- **Replace `clamp(` with `sil::clampf(` for float arguments in `process()` (32 sites) and in the helpers it calls**: `softKnee01`, `saturatorPeakToHistIndex`, `estimateRollingProgramDbFs`, the saturator update block, and the limiter. Leave the int overloads alone. Rack's `clamp(int,int,int)` already uses `std::min/max`. Leave the UI code alone too; it gains nothing measurable.
- **NaN semantics differ.** Rack's `clamp(NaN, lo, hi)` returns `hi`, which silently sanitises. `clampf(NaN, …)` returns NaN. In practice this changes nothing: a NaN arriving at Sil's inputs poisons `lowpassL1`'s state at line 1115 before any clamp sees it, today and after the change. If you want Sil to survive a NaN-emitting upstream module, add one guard where the inputs are read (lines 1102–1105). It is cheap, and `std::isfinite` is reliable because Rack's flags don't include `-ffinite-math-only`:
  ```cpp
  const float rawL = inputs[INPUT_L_INPUT].getVoltage();
  const float rawR = inputs[INPUT_R_INPUT].getVoltage();
  const float safeL = std::isfinite(rawL) ? rawL : 0.f;
  const float safeR = std::isfinite(rawR) ? rawR : 0.f;
  ```
- Expect a render within rounding noise of the golden (about −94 dB null). It is not bit-identical: with the libm calls gone GCC reassociates the mid-enhance activation product. A null worse than −85 dB means a site was converted where an argument could be NaN, or an `int`/`float` overload was mixed up.

### 8.3 A5: limiter metric short-circuit

Line 1606. Add `float limiterDemandThreshold` set in `updateDynamicsCoefficients()` (line 752), next to `limiterCeiling`:

```cpp
// demandDb = max(0, 20log10(peak/5) - kLimiterCeilingDb) > 0  <=>  peak > 5 * 10^(kLimiterCeilingDb/20)
limiterDemandThreshold = kAudioFullScaleV * std::pow(10.f, kLimiterCeilingDb / 20.f);
```

```cpp
const float limiterDemandDb = (detectorPeak > limiterDemandThreshold)
    ? std::max(0.f, toDbFsSafe(detectorPeak) - kLimiterCeilingDb) : 0.f;
```

`detectorPeakDbTp` is only used to form `limiterDemandDb`, so it can go. The threshold is numerically the same as `limiterCeiling`; keep it as a named member anyway, because `limiterCeiling` is about the output guard and this is about the metric. Expect a bit-identical render.

### 8.4 A1: control-rate decision math

**Shape.** Add one tick counter and one method that recomputes every *target*. The per-sample code keeps every envelope, every smoother and every audio-path filter where it is now. It just reads targets instead of computing them.

```cpp
static constexpr int kDecisionDivision = 16;
int decisionCounter = kDecisionDivision - 1;   // first sample is a tick, so targets are valid from sample 0
```

```cpp
// in process(), right after the bypass delay (≈ line 1111)
const bool decisionTick = (++decisionCounter >= kDecisionDivision);
if (decisionTick) decisionCounter = 0;
```

The divider has to tick *after* each stage's envelopes update for that sample, but *before* its smoother runs. The cleanest way is to keep the target code inline at its current location, wrapped in `if (decisionTick) { … }`, rather than hoisting it into a function called at the top of `process()`. Hoisting would read envelopes one sample stale, which is harmless but breaks the "only the rate changed" story for the null test.

**Per stage (what goes inside `if (decisionTick)`).**

| Stage | Lines | Move inside the tick | Stays per sample |
|---|---|---|---|
| Low band | 1135–1137 | `denom` sqrt, `lowCorr`, `targetLowSideGain` (make it a member) | `lowBandCorrLL/RR/LR` EMAs; the `lowBandSideGain` smoother |
| Impact air | 1157–1163 | `transientDeltaDb`, `transientGate`, `impactAir.targetLiftDb` | `env`, `slowEnv`, `smoothedLiftDb` smoother, `ledAmount`, shelf process |
| Remove mud | 1213–1216 | `refEnv`, `mudDeltaDb`, `activation`, `targetCutDb` | the 6 RC filters, 3 envelopes, `smoothedCutDb` smoother, `ledAmount`, peaking process |
| Mid enhance | 1255–1275, 1287–1293 | everything from `refEnvMid` to `targetLiftDb`; also the `activeDb`/`ledNorm`/`sqrt` LED block | the 6 RC filters, 3 envelopes, `smoothedLiftDb` smoother, lift process |
| Glue | 1317–1337 | `levelDb`, `overDb`, knee → `targetGrDb` (member); then, after the GR smoother, `makeupDb`, `pow` → next linear gain | sidechain HP, `rmsEnv`, `gainReductionDb` smoother, the linear-gain ramp |
| Stereo | 1376–1397, 1449–1461 | all `toDbFsSafe`/`toDbSafe`/`softKnee01`/`inverseSoftKnee01` → `targetMidCutDb`, `targetSideLiftDb`; the LED deadband block | 8 RC filters, 4 envelopes, both smoothers, mid/side EQ process |

**Members that become persistent.** Today these are locals that are recomputed every sample. They must become members so they persist between ticks: `targetLowSideGain`, `glue.targetGrDb`, `glue.gainLinPrev`, `glue.gainLinNext`, `glue.gainLinStep`. The stage `target*` members already exist (`impactAir.targetLiftDb`, `removeMud.targetCutDb`, `midEnhance.targetLiftDb`, `stereoEnhance.targetMidCutDb`/`targetSideLiftDb`). Reset them in the matching `reset*State()` helpers and in `glue.reset()`, and initialise the glue gains to 1.

**Glue linear ramp.** This is the only place where the audio-path gain changes form: a per-sample `pow` becomes a per-block linear ramp. Use the §3 A1 code. Two details:
- On the tick, the sample's gain is `gainLinPrev` (the previous block's end value), and `gainLinNext` is the new endpoint. The ramp reaches `gainLinNext` exactly at the next tick, so there is no discontinuity and no zipper.
- The trajectory is the old curve delayed by at most 16 samples (0.33 ms) against a 30 ms attack. That is the dominant term in A1's −76 dB null.

**Cross-stage reads.** Two stages read other stages' state, and both are fine at tick rate:
- Mid enhance reads `removeMud.ledAmount` (line 1258). It is smoothed per sample, so reading it on the tick is just a sample of a smooth signal.
- Mid enhance and the saturator read `limiterRecentGrDb`, which is a 120 ms EMA.

**LEDs.** `lightDivider` (÷32) reads the LED locals only on its own tick. With `kDecisionDivision = 16`, every light tick is also a decision tick. **Keep the light divider a multiple of the decision divider**, or a light tick can read an LED local that wasn't computed this sample. Simplest: make the LED values members (`midEnhance.ledAmount` and `stereoEnhance.ledAmount` already are) and have the light block read the members, never the locals.

**Optional: spread the spike.** All stages tick on the same sample, so that sample costs roughly 16× the decision math. Rack processes in blocks, so the average is what matters for CPU. If you see that sample in the A9 timing and dislike it, give each stage its own phase: impact air at 0, mud at 4, mid at 8, stereo at 12, all at ÷16.

**Pick 16, or 8 to be cautious.** The sweep in §3 is the data. If the golden test at 16 shows any LED Δ > 0.03, the likely culprit is the mid-enhance `sqrt` LED. Compute it per sample again (it is one sqrt) rather than lowering the divider.

### 8.5 A4 + A3: one-pole coefficients, then SIMD detector banks

**Step 1 (A4), scalar.** Add a drop-in replacement for `dsp::RCFilter` and swap the type of all 24 detector filters. Keep **every `setCutoff(r)` call and every `r` value exactly as it is** (see S1):

```cpp
namespace sil {
struct OnePoleRC {
    float k = 1.f, m = 0.f, x = 0.f, y = 0.f;
    // Same contract as rack::dsp::RCFilter::setCutoff: r is in radians/sample.
    void setCutoff(float r) { const float c = 2.f / r; k = 1.f / (1.f + c); m = (1.f - c) / (1.f + c); }
    void process(float in) { const float o = k * (in + x) - m * y; x = in; y = o; }
    float lowpass() const { return y; }
    float highpass() const { return x - y; }
    void reset() { x = y = 0.f; }
};
}
```

It is algebraically identical to `(x + x₁ − y₁(1−c)) / (1+c)`. Expect a null around −89 dB from rounding, and land it on its own.

**Step 2 (A3), banks.** Replace each stage's filter set with one bank:

```cpp
namespace sil {
struct DetectorBank4 {           // 4 independent HP->LP bandpass detectors + |x| envelopes
    using f4 = rack::simd::float_4;
    f4 hpK = 1.f, hpM = 0.f, hpX = 0.f, hpY = 0.f;
    f4 lpK = 1.f, lpM = 0.f, lpX = 0.f, lpY = 0.f;
    f4 env = 1e-6f;
    static void coeffs(float r, float& k, float& m) { const float c = 2.f / r; k = 1.f / (1.f + c); m = (1.f - c) / (1.f + c); }
    void setLane(int lane, float hpR, float lpR) {
        coeffs(hpR, hpK[lane], hpM[lane]);
        coeffs(lpR, lpK[lane], lpM[lane]);
    }
    void reset(float envFloor = 1e-6f) { hpX = hpY = lpX = lpY = 0.f; env = envFloor; }
    // in: per-lane input. Returns nothing; read env[i].
    void process(f4 in, float attack, float release) {
        const f4 hpOut = hpK * (in + hpX) - hpM * hpY; hpX = in; hpY = hpOut;
        const f4 band = hpX - hpY;                                  // == RCFilter::highpass()
        const f4 lpOut = lpK * (band + lpX) - lpM * lpY; lpX = band; lpY = lpOut;
        const f4 ax = rack::simd::fabs(lpY);
        const f4 c = rack::simd::ifelse(ax > env, f4(attack), f4(release));
        env = ax + c * (env - ax);
    }
};
}
```

| Stage | Lanes 0–3 (HP Hz, LP Hz) | Input |
|---|---|---|
| `removeMud.bank` | mud (180, 520), bass (80, 160), presence (700, 3000), unused | `f4(mono, mono, mono, 0)` |
| `midEnhance.bank` | lowRef (140, 560), core (700, 2400), presence (2600, 6500), unused | `f4(monoPostMud, …, 0)` |
| `stereoEnhance.bank` | mid350 (270, 470), midBroad (120, 1400), side6k (4200, 9500), sideBroad (1000, 12000) | `f4(mid, mid, side, side)` |

Pass the Hz values through the existing `norm()` lambda (`hz / sampleRate`, clamped), exactly as `updateRemoveMudCutoffs`/`updateMidEnhanceCutoffs`/`updateStereoEnhanceCutoffs` (lines 895–935) do today.

Integration points:
- **Envelopes.** The three `updateEnv` lambdas (lines 1204, 1247, 1366) go away. Read `bank.env[0..3]` wherever `removeMud.mudEnv`, `midEnhance.coreEnv`, `stereoEnhance.side6kEnv` and so on were read. Keep the old members as plain copies if other code (debug, tests) reads them; it costs 11 scalar stores.
- **Resets.** `resetMidEnhanceState()`/`resetStereoEnhanceState()` reset the individual filters; replace those calls with `bank.reset()`. Note that `resetRemoveMudState()` (line 936) currently resets the mud **envelopes but not the mud filters**. To keep the render bit-for-bit comparable, reset only `bank.env` there, not the filter state. Whether that asymmetry was intended is a separate question.
- **The unused lane 3 in the mud and mid banks.** Feed it 0. Don't try to pack the glue sidechain HP in: its input (`glueMono`, post-mid-enhance) is a different signal at a different point in the chain.
- **Low band (optional).** Its 4 filters are two serial pairs (L1→L2, R1→R2). Packing L/R into 2 lanes of one `float_4` per stage halves them, but it's ~3% and adds a shuffle. Skip unless you're already in there.
- **Checked:** this struct compiles against Rack v2's `simd` headers, and lane 0 tracks a scalar `RCFilter` HP→LP pair plus `updateEnv` to within 6×10⁻⁵ relative (rounding) over 10 s of noise.
- **Alignment.** `float_4` members want 16-byte alignment. `Sil` is heap-allocated by Rack's `createModel`, and C++17 `new` respects `alignas` on over-aligned types, so this is fine. If the headless test allocates `Sil` on the stack, that is also fine.

### 8.6 A7: biquad design cache

Replace `Biquad::setPeaking`/`setHighShelf` calls on the coefficient ticks (lines 1169–1182, 1222–1225, 1281–1284, 1424–1440) with a cached design per stage:

```cpp
namespace sil {
struct PeakingDesign {
    float cosW0 = 1.f, alpha = 0.f, lastGainDb = NAN; bool neutral = true;
    void configure(float sr, float hz, float q) {
        neutral = (sr <= 1.f || hz <= 1.f || q <= 1e-4f);        // preserves Biquad's guard
        if (neutral) return;
        const float fc = clampf(hz, 10.f, 0.48f * sr);
        const float w0 = 2.f * float(M_PI) * fc / sr;
        cosW0 = std::cos(w0); alpha = std::sin(w0) / (2.f * q);
        lastGainDb = NAN;                                          // force the next apply()
    }
    // Writes coefficients into dst (z state untouched). Returns false if skipped.
    bool apply(float gainDb, Biquad& dst) {
        if (std::fabs(gainDb - lastGainDb) < 1e-4f) return false;  // NaN compares false -> applies
        lastGainDb = gainDb;
        if (neutral) { dst.b0 = 1.f; dst.b1 = dst.b2 = dst.a1 = dst.a2 = 0.f; return true; }
        const float A = std::pow(10.f, gainDb / 40.f);
        const float inv = 1.f / (1.f + alpha / A);
        dst.b0 = (1.f + alpha * A) * inv; dst.b1 = -2.f * cosW0 * inv; dst.b2 = (1.f - alpha * A) * inv;
        dst.a1 = dst.b1;                  dst.a2 = (1.f - alpha / A) * inv;
        return true;
    }
};
}
```

- Add `Biquad::copyCoeffs(const Biquad&)` and do `design.apply(g, L); R.copyCoeffs(L);`. That halves the work for impact air, mud and mid enhance. Stereo has two distinct designs (mid at 350 Hz/Q 7.3, side at 6 kHz/Q 0.71), each applied to one filter.
- Write a `ShelfDesign` twin for impact air with `sqrtA = std::sqrt(A)`, mirroring `setHighShelf`.
- Call `configure()` from the constructor and from `onSampleRateChange()`, where the code currently calls `setPeaking(sr, …, 0.f)` directly (lines 1035–1042, 1074–1079). Then call `apply(0.f, …)` once so the coefficients start out the same as today.
- **The skip epsilon is 1e-5 dB, not exact equality.** The smoothers decay geometrically and only reach exactly 0 after FTZ kicks in, seconds later. The epsilon lets idle stages stop redesigning almost immediately. 1e-4 dB was tried first and measurably worsened the 96 kHz null (−67 dB against −76 dB), so it is 1e-5.
- **`stereoEnhance.coeffsNeutral` (line 1437):** delete it.

### 8.7 A6: rolling program RMS → fixed block ring

Replace `rollingBufferL/R` and the fields at lines 224–230, plus `configureRollingBuffer`, `pushRollingSample` and `estimateRollingProgramDbFs` (lines 596–651):

```cpp
static constexpr int kRollingBlocks = 1024;                       // fixed: no allocation, ever
std::array<double, kRollingBlocks> rollingBlockSumSq {};
std::array<int,    kRollingBlocks> rollingBlockCount {};
int rollingBlockIndex = 0, rollingSamplesPerBlock = 1, rollingSamplesInBlock = 0;
double rollingAccumSumSq = 0.0;  int rollingAccumCount = 0;
double rollingMonoSqSum = 0.0;   int rollingAudibleCount = 0;

void configureRollingBuffer(float sampleRate) {                    // now allocation-free
    rollingSamplesPerBlock = std::max(1, int(std::round(sampleRate * kRollingBufferSeconds / kRollingBlocks)));
    rollingBlockSumSq.fill(0.0); rollingBlockCount.fill(0);
    rollingBlockIndex = rollingSamplesInBlock = rollingAccumCount = rollingAudibleCount = 0;
    rollingAccumSumSq = rollingMonoSqSum = 0.0;
}

void pushRollingSample(float l, float r) {
    const float mono = 0.5f * (l + r);
    if (std::fabs(mono) > kAdaptiveSilenceVolts) { rollingAccumSumSq += double(mono) * mono; ++rollingAccumCount; }
    if (++rollingSamplesInBlock < rollingSamplesPerBlock) return;
    rollingMonoSqSum   += rollingAccumSumSq - rollingBlockSumSq[rollingBlockIndex];
    rollingAudibleCount += rollingAccumCount - rollingBlockCount[rollingBlockIndex];
    rollingBlockSumSq[rollingBlockIndex] = rollingAccumSumSq;
    rollingBlockCount[rollingBlockIndex] = rollingAccumCount;
    rollingAccumSumSq = 0.0; rollingAccumCount = rollingSamplesInBlock = 0;
    if (++rollingBlockIndex >= kRollingBlocks) {
        rollingBlockIndex = 0;
        // Once per 10 s: rebuild the running sum so add/subtract drift can't accumulate.
        rollingMonoSqSum = 0.0; for (double v : rollingBlockSumSq) rollingMonoSqSum += v;
    }
}

float estimateRollingProgramDbFs() const {
    if (rollingAudibleCount <= 0) return -100.f;
    return toDbFsSafe(std::sqrt(float(std::max(0.0, rollingMonoSqSum) / double(rollingAudibleCount))));
}
```

- **Checked:** against the shipping `pushRollingSample`/`estimateRollingProgramDbFs` on 40 s of noise whose level swings 26 dB, with 2 s silent gaps, the program-level estimate agrees within **0.025 dB** at every 1024-sample update.
- `rollingFilled` and `rollingWriteIndex` go away. The old "not filled yet" guard is equivalent to `rollingAudibleCount <= 0`, because unfilled blocks hold zeros.
- **Behaviour delta:** the current sample's partial block is not in the estimate until it completes (≤9.8 ms late), and the window edge moves in 9.8 ms steps. The threshold is recomputed every 1024 samples (21 ms) and clamped to [−24, −8] dB. Verify with the quiet golden pass, since the loud pass sits on the clamp and hides any difference.
- `onSampleRateChange()` no longer allocates multi-MB vectors on the engine thread.

### 8.8 A8: idle fast path (freeze, don't reset)

```cpp
// members
uint32_t silentSamples = 0;
uint32_t idleEntrySamples = 0;              // set in onSampleRateChange: 15 s worth
bool idle = false;
// onSampleRateChange / ctor
idleEntrySamples = uint32_t(15.f * sampleRate);
silentSamples = 0; idle = false;
```

```cpp
// in process(), right after inL/inR are formed (≈ line 1105), before the bypass delay
const bool silentNow = (inL == 0.f && inR == 0.f);
silentSamples = silentNow ? std::min(silentSamples + 1, idleEntrySamples) : 0u;
if (!silentNow) idle = false;                         // wake on this very sample; full chain runs below
else if (silentSamples >= idleEntrySamples) idle = true;
if (idle) { processIdle(args, hasL || hasR); return; }
```

`processIdle()` should:
- Set both outputs to 0 V. After 15 s of silence the bypass and limiter delay lines hold only zeros, so 0 is exactly what the full chain would produce.
- Step `masteringMix` the same way `process()` does (line 1620 onward), so a toggle during idle is honoured on wake.
- Run `levelMeter.process(0, 0, connected)`, the histogram min/max/bin publish and the spectrum ring write plus snapshot publish with zeros. The displays keep scrolling.
- Skip lights, or re-run them from the frozen member LED values. They are already at their converged values, so skipping is visually identical.
- Keep the A9 timing hook.

Details:
- **Do not reset any state on entry or exit.** The whole point is that frozen ≈ continued silence (see §3 A8).
- **Exact-zero test, not a threshold.** A dither floor or a DC offset is "signal" and must keep the chain running, so `== 0.f` is the right test.
- `silentSamples` saturates at `idleEntrySamples`, so it can't overflow during long idle stretches.
- The saturator counters (`samplesInBin`, `updateDivider`) and the dividers freeze mid-count. That shifts tick phases by up to one period after wake, which is inaudible and invisible to the null test (the golden test has no 15 s silences). Add a separate test that checks a 20 s silence followed by program audio matches the non-idle build within −70 dB.

### 8.9 UI-thread notes

**U1. Histogram ring texture** (`HistogramWidget::draw`, from line 1883). Stop shifting pixels; instead, write into the column that matches the bin index.
- Texture column `i` holds bin `i`. On refresh, write only the bins in `[rasterWritePtr, lastWritePtr)` (mod 1000) to columns of the same index. This deletes the `memmove` loop at lines 1922–1930.
- Draw in two segments so the oldest bin sits on the left. Use two scissored rects rather than `NVG_IMAGE_REPEATX`. NanoVG's GL2 backend can drop REPEAT on non-power-of-two textures, and this one is 1000 px wide.
  ```cpp
  const float W = box.size.x, H = box.size.y;
  const float split = float(Sil::HISTOGRAM_BINS - lastWritePtr) / float(Sil::HISTOGRAM_BINS) * W;
  const float ox = -float(lastWritePtr) / float(Sil::HISTOGRAM_BINS) * W;
  const NVGpaint pA = nvgImagePattern(args.vg, ox,     0.f, W, H, 0.f, rasterImage, 1.f);  // cols [ptr,1000) -> [0,split)
  const NVGpaint pB = nvgImagePattern(args.vg, ox + W, 0.f, W, H, 0.f, rasterImage, 1.f);  // cols [0,ptr)    -> [split,W)
  nvgBeginPath(args.vg); nvgRect(args.vg, 0.f, 0.f, split, H);     nvgFillPaint(args.vg, pA); nvgFill(args.vg);
  nvgBeginPath(args.vg); nvgRect(args.vg, split, 0.f, W - split, H); nvgFillPaint(args.vg, pB); nvgFill(args.vg);
  ```
  At the seam, linear filtering may sample one texel from the other end of the texture. If a one-pixel line shows, create the image with `NVG_IMAGE_NEAREST`; at 1000 texels into ~296 px it won't look worse.
- **Colours.** Precompute `uint32_t bucketRGBA[16]`, `dividerRGBA` and `black` once per palette change (premultiplied, as `writePixel` does now). Then write whole `uint32_t` values with no per-pixel float maths, and drop the bounds check from the inner loop by clamping `y0`/`y1` once per span.
- **Partial upload (optional).** `nvgUpdateImage` always uploads the full texture. `Deepcache.cpp:414` already uses `nvglImageHandleGL2`; do the same here and `glTexSubImage2D` only the changed column range (two calls when the range wraps). Make sure to set `GL_UNPACK_ROW_LENGTH` to 1000 for the sub-rect and restore it to 0 afterwards. Leaving it set corrupts NanoVG's own uploads.
- **The palette-change path** keeps doing a full rebuild, as now.
- The stage-history widget (`SilStageHistoryWidget::draw`, `memmove` at line 2296) has the same `memmove` pattern at 160 px wide. Apply the same change if you're in there; it is smaller.

**U2. Histogram framebuffer.** Before removing it, add the `histogram_fb_us` vs `histogram_draw_us` columns from the timing CSV to a quick comparison at 100% and 200% zoom. If you remove it, the widget is drawn directly every frame (one textured quad). Keep `step()`'s 30 Hz refresh gating, so the CPU-side raster work is unchanged.

**U3. Meter text.** In `SilHorizontalMeter` (line 2339), store `char text[8]` plus the last displayed tenths as an `int`, mirroring `SilBoostReadout::step()`, and only `snprintf` on change. The `nvgText` call per frame remains. Removing it as well means wrapping the meter in a `FramebufferWidget` and calling `setDirty()` when the tenths or the bar's integer pixel width changes. Do this only if the per-frame profile shows it.

**U5.** In `SilWidget::draw()` (line 2788), move `dynamic_cast<Sil*>(module)` after the `if (!measurePerf) return;` check.

### 8.10 Do-not-touch list

These look like optimisation opportunities but are load-bearing or measured not worth it:
- **The Hermite intersample detector** (`intersamplePeak4xCausal`). Removing it changed the limiter output by −33 dBFS (§4).
- **`Upsampler<4,8>`.** A contiguous polyphase rewrite was slower. GCC already unrolls the Q=8 loop.
- **`LimiterPeakWindow`.** O(1) amortised, fixed storage, already tested (`tests/sil_limiter_peak_window_spec.cpp`).
- **`StereoLevelMeter` in double.** It is ~6% of the optimised cost. Float TDF-II would probably be fine, but the 38 Hz high-pass poles sit close to the unit circle, and the meter's job is to agree with Octavia's. Not worth the validation work.
- **Warm A/B.** The chain keeps running while mastering is off, by design (`doc/Sil/optimize.md` §0). A8 keys on silence, not on the switch.
- **The `r` values passed to every `setCutoff()`.** See S1. Preserve them through A3/A4 and change them only in a deliberate retune.

---

## Appendix: raw bench results

Wall-clock, best of 15 interleaved, 48 kHz, `-O3 -funsafe-math-optimizations -march=nehalem`. The rows come from two separate runs, each with its own baseline, and speedups are relative to that run's baseline.

```
Run 1: single changes, shipping clamp         ns/smp  speedup   peak diff    null
baseline (shipping)                            375.8   1.00x        —          —
ctrl ÷16                                       182.3   2.06x   −67.4 dBFS  −75.7 dB
onepole                                        350.2   1.07x   −83.3 dBFS  −89.2 dB
coef cache                                     372.3   1.01x   −81.7 dBFS  −89.0 dB
limlog                                         367.0   1.02x     exact       exact
roll (block RMS)                               373.8   1.01x     exact*      exact*
nomod                                          375.2   1.00x     exact       exact
poly upsampler                                 398.5   0.94x     exact       exact
noherm                                         371.5   1.01x   −33.2 dBFS  −45.8 dB
neutral bypass                                 374.5   1.00x   −90.2 dBFS −102.4 dB

Run 2: combinations
baseline (shipping clamp)                      379.4   1.00x
ctrl ÷16                                       185.0   2.05x
T2 = ctrl+onepole+coef+nomod+limlog+roll       154.8   2.45x
T2 + simd                                      143.1   2.65x   −73.5 dBFS  −77.1 dB
baseline, min/max clamp                        334.4   1.13x   (bit-identical)
T2 + simd, min/max clamp                       129.4   2.93x   −73.5 dBFS  −77.3 dB
```
\* Identical here only because the adaptive glue threshold sat on its clamp in both builds on this loud test mix (−23.6 dB in the real-code reference render, not −8 dB). On quieter material, expect threshold differences of a small fraction of a dB, updated every 1024 samples.

Instruction counts (callgrind, inside `process()`): shipping 3242 Ir/sample; with A2 3010; A1 1886; A1+A2 1742; T2 1614; T2+A2 1475.

---

## 9. Implementation record

Implemented on 2026-10-11 against the real `src/Sil.cpp`, verified by rendering seven scenarios (48/44.1/96 kHz, quiet, left-only, mastering toggle, 22 s silence gap) through both the original and the modified file and comparing.

| Item | Status | Notes |
|---|---|---|
| A1 control-rate decisions (÷16) | **Done** | Glue gain ramps linearly across each block, so `pow` runs once per 16 samples. |
| A2 `clampf` | **Done** | Rounding-noise differences only (not bit-identical, see §8.2). |
| A3 SIMD detector banks, A4 one-pole coefficients | **Done** | `src/SilDspKernels.hpp`: `OnePoleRC`, `DetectorBank4`. Every `r` passed to `setCutoff` is unchanged, so S1 is untouched. |
| A5 limiter demand `log10` gate | **Done** | |
| A6 block-ring rolling RMS | **Done** | 1024 blocks, allocation-free. The partial block is included in the estimate (without it, the first 250 ms showed glue LED drift up to 0.054). |
| A7 biquad design cache | **Done** | Skip threshold 1e-5 dB. `coeffsNeutral` removed. The mud design is now also re-solved on sample-rate change. |
| A8 idle path | **Done** | After 15 s of exact digital silence. Not a pure freeze: it keeps every periodic counter on the phase continued silence would give (a pure freeze cost about 17 dB of null after a wake), empties the 10 s and 8 s level histories, and holds the light targets. |
| A9 sparse debug timing | **Skipped** | `AGENTS.md` requires timing every `process()` call; timing stays per call, including on the idle path. |
| U3 meter text cache, U5 `dynamic_cast` order | **Done** | Compiled only against stubs (UI is excluded from the headless build). Please eyeball the meter readout once. |
| U1 histogram texture, U2 histogram framebuffer | **Deferred** | Needs visual verification. |

Results (cloud harness, `-march=native`; relative numbers are what matter):

| Metric | Original | Modified |
|---|---|---|
| Program, 48 kHz | ≈490 ns/sample | ≈165 ns/sample (**≈3×**) |
| Program, 96 kHz | ≈490–510 ns/sample | ≈160–166 ns/sample |
| Digital silence (after 15 s) | ≈275 ns/sample | ≈26 ns/sample (**≈10×**) |
| Null vs. original, worst of 7 scenarios | n/a | −74.8 dB |
| Peak error, worst | n/a | −66 dBFS |
| LED drift, worst | n/a | 0.0205 (criterion 0.03) |

Tests: `tests/sil_boost_spec.cpp` passes unchanged. New `tests/sil_perf_regression_spec.cpp` holds golden RMS/peak values measured from the *original* Sil on a deterministic stimulus (tolerance 0.05 dB RMS, 0.1 dB peak), checks exact zero output while idle, and checks that audio after a 20 s gap matches the non-idle result (−78 dB null). The Makefile rules for it are in `doc/sil_perf_makefile.patch` because the Makefile is write-protected for the assistant.
