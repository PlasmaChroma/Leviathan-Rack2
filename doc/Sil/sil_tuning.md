Dragon King Leviathan, yes — your suspicion is basically right, but I would **not** solve this by only making the saturator hotter. The deeper issue is that Sil now has several *subtractive / peak-constraining* stages, while the loudness restoration is still mostly **peak-percentile based** and capped fairly conservatively.

The current chain can easily become:

```text
Low Recovery
→ Impact Air
→ Remove Mud       // possible -2.5 dB around 315 Hz
→ Glue Compressor  // net negative gain, because makeup is only 50% of GR
→ Stereo Enhance   // possible -2 dB Mid cut around 350 Hz
→ Saturator        // tries to restore peak, max +2 dB makeup
→ Final Limiter    // -1.0 dBFS ceiling
```

So even before the limiter, Sil may already be losing perceived body. Then the limiter prevents the chain from ever matching bypass peak level if bypass is at or above 0 dBFS. That is technically correct for streaming-safe mastering, but bad for A/B optics unless RMS/perceived loudness comes up.

## Main findings

### 1. The glue compressor is a real loudness sink

Current behavior:

```cpp
glue.makeupDb = clamp(glue.gainReductionDb * kGlueMakeupFraction, 0.f, kGlueMaxMakeupDb);
const float totalGainDb = -glue.gainReductionDb + glue.makeupDb;
```

With:

```cpp
kGlueMakeupFraction = 0.50f;
kGlueMaxGainReductionDb = 3.f;
```

So if glue is doing 2 dB of gain reduction, it only returns 1 dB. Net result: **-1 dB**.

That may be musically appropriate for “glue,” but now that Sil also has mud removal and stereo mid-cut, the cumulative feel can become smaller.

I would change glue makeup from **50%** to something closer to **70–80%**, or make it adaptive.

Suggested first pass:

```cpp
static constexpr float kGlueMakeupFraction = 0.75f;
static constexpr float kGlueMaxMakeupDb = 2.0f;
```

That keeps glue audible but reduces the “mastering-on got quieter” impression.

---

### 2. Glue threshold is adapting from post-master output, creating a feedback loop

This is probably the most important structural issue.

The glue adaptive threshold uses:

```cpp
const float programDbFs = estimateRollingProgramDbFs();
```

But the rolling buffer is filled here:

```cpp
pushRollingSample(outL, outR);
```

That means the glue threshold is learning from the **already mastered / limited / possibly reduced** output.

So if the chain gets quieter, the glue threshold can move lower, which can cause more compression, which can make the chain quieter again.

That is a little mastering ouroboros eating its own tail.

Better: use a pre-glue or input-derived signal for the adaptive threshold.

Minimum fix:

```cpp
// Instead of this near the end:
pushRollingSample(outL, outR);

// Push pre-glue program material instead:
pushRollingSample(cleaned.l, cleaned.r);
```

But since `cleaned` is scoped earlier, the cleanest solution is to move the rolling update to right after:

```cpp
const sil_micropeak::StereoSample cleaned(preMasterL, preMasterR);
pushRollingSample(cleaned.l, cleaned.r);
```

Then remove the later `pushRollingSample(outL, outR);`.

This makes the compressor adapt to the source program, not its own processed result.

---

### 3. Saturator is peak-targeted, not loudness-targeted

The saturator currently estimates a recent peak percentile:

```cpp
const float recentLoudPeak = saturatorEstimateRecentPeakPercentile();
const float recentPeakDb = 20.f * std::log10(recentPeakNorm);
const float desiredMakeupDb = clamp(
    kSaturatorTargetPreLimiterDb - recentPeakDb,
    0.f,
    kSatMaxMakeupDb
);
```

This means it tries to lift **peak level** toward:

```cpp
kSaturatorTargetPreLimiterDb = -0.75f;
```

But perceived A/B loudness is much more tied to **RMS / LUFS-ish energy**, not peak. If mud removal and mid-side shaping remove body, the peak may still look fine while the sound feels smaller.

So yes, the saturator needs more authority, but it should probably receive a **stage-compensation term** or a **short-term loudness makeup term**, not just a higher peak target.

---

### 4. Saturator max makeup is probably too low now

Current values:

```cpp
static constexpr float kSatMaxMakeupDb = 2.0f;
static constexpr float kSatMaxDrive = 1.45f;
static constexpr float kSaturatorTargetPreLimiterDb = -0.75f;
```

Given the current chain, `+2 dB` may not be enough if these are active together:

```text
Remove Mud:       up to -2.5 dB in low-mid body
Glue:             up to net -1.5 dB
Stereo mid cut:   up to -2.0 dB around 350 Hz Mid
Final limiter:    peak ceiling -1.0 dBFS
```

I would not jump to something wild, but I would raise the available makeup.

Suggested first pass:

```cpp
static constexpr float kSaturatorTargetPreLimiterDb = -0.50f;
static constexpr float kSatMaxMakeupDb = 3.25f;
static constexpr float kSatMaxDrive = 1.60f;
```

But I would also reduce the drive-per-makeup coupling:

```cpp
const float desiredDrive = clamp(
    1.f + desiredMakeupDb * 0.18f,
    kSatMinDrive,
    kSatMaxDrive
);
```

instead of:

```cpp
1.f + desiredMakeupDb * 0.28f
```

Reason: more makeup should not automatically mean much more color. Let the saturator become a **level-restoring soft clipper**, not a distortion stage that suddenly changes the module’s identity.

---

### 5. The limiter metrics are passed to the saturator but not actually used

This block exists:

```cpp
saturator.limiterEngagement = limiterTriggerEma;
saturator.limiterRecentGrDb = limiterRecentGrDb;
```

But those values do not currently affect `desiredMakeupDb` or `desiredDrive`.

That is a missed opportunity. The saturator should push harder when the limiter is not engaging, and back off when the limiter is working too much.

Suggested control law:

```cpp
float desiredMakeupDb = clamp(
    kSaturatorTargetPreLimiterDb - recentPeakDb,
    0.f,
    kSatMaxMakeupDb
);

// Encourage activity if limiter is not being touched.
const float limiterUnderEngaged = 1.f - clamp(saturator.limiterEngagement / 0.25f, 0.f, 1.f);
desiredMakeupDb += kSatLimiterSeekBoostDb * limiterUnderEngaged;

// Back off if limiter is digging too hard.
const float limiterOverworked = softKnee01(
    saturator.limiterRecentGrDb,
    kSatLimiterGrBackoffStartDb,
    kSatLimiterGrBackoffKneeDb
);
desiredMakeupDb -= kSatLimiterGrBackoffDb * limiterOverworked;

desiredMakeupDb = clamp(desiredMakeupDb, 0.f, kSatMaxMakeupDb);
```

Suggested constants:

```cpp
static constexpr float kSatLimiterSeekBoostDb = 0.75f;
static constexpr float kSatLimiterGrBackoffStartDb = 1.25f;
static constexpr float kSatLimiterGrBackoffKneeDb = 1.5f;
static constexpr float kSatLimiterGrBackoffDb = 1.5f;
```

That gives Sil a mastering-brain behavior:

```text
Not hitting limiter? Nudge up.
Lightly hitting limiter? Good.
Limiter working hard? Back off.
```

Much better than a fixed peak target alone.

---

## The big patch I’d recommend

Add a dedicated **Master Loudness Compensation** stage before the saturator or folded into the saturator makeup calculation.

Goal:

```text
Mastering ON should be approximately equal or slightly louder than bypass in perceived loudness,
while final output remains -1.0 dBFS true-peak constrained.
```

### New state

```cpp
float inputRmsEnv = 1e-8f;
float preSatRmsEnv = 1e-8f;
float loudnessMakeupDb = 0.f;
```

### New constants

```cpp
static constexpr float kLoudnessCompAttackSec = 0.75f;
static constexpr float kLoudnessCompReleaseSec = 3.50f;
static constexpr float kLoudnessCompTargetLiftDb = 0.75f;
static constexpr float kLoudnessCompMaxDb = 2.25f;
static constexpr float kLoudnessCompLimiterBackoffStartDb = 1.0f;
static constexpr float kLoudnessCompLimiterBackoffAmountDb = 1.5f;
```

### Concept

Measure input RMS and pre-saturator RMS:

```cpp
const float inputMono = 0.5f * (inL + inR);
const float preSatMono = 0.5f * (enhancedL + enhancedR);

inputRmsEnv = rmsCoeff * inputRmsEnv + (1.f - rmsCoeff) * inputMono * inputMono;
preSatRmsEnv = rmsCoeff * preSatRmsEnv + (1.f - rmsCoeff) * preSatMono * preSatMono;

const float inputRmsDb = 10.f * std::log10(inputRmsEnv / (kAudioFullScaleV * kAudioFullScaleV) + 1e-12f);
const float preSatRmsDb = 10.f * std::log10(preSatRmsEnv / (kAudioFullScaleV * kAudioFullScaleV) + 1e-12f);
```

Then derive compensation:

```cpp
float desiredLoudnessMakeupDb = inputRmsDb - preSatRmsDb + kLoudnessCompTargetLiftDb;
desiredLoudnessMakeupDb = clamp(desiredLoudnessMakeupDb, 0.f, kLoudnessCompMaxDb);
```

Back it off if the limiter is already working:

```cpp
const float limiterBackoff = softKnee01(
    limiterRecentGrDb,
    kLoudnessCompLimiterBackoffStartDb,
    1.0f
) * kLoudnessCompLimiterBackoffAmountDb;

desiredLoudnessMakeupDb = std::max(0.f, desiredLoudnessMakeupDb - limiterBackoff);
```

Then add it to saturator makeup:

```cpp
desiredMakeupDb = clamp(
    desiredMakeupDb + loudnessMakeupDb,
    0.f,
    kSatMaxMakeupDb
);
```

This is the move I’d trust most. It lets Sil compensate for the tonal/dynamic changes it just created, instead of hoping peak-normalized saturation solves everything.

---

## Also: final limiter is not truly hard ceiling yet

This is adjacent but important if you make the saturator hotter.

Current limiter uses attack smoothing:

```cpp
const float coeff = (desiredGain < limiterGain) ? limiterAttackCoeff : limiterReleaseCoeff;
limiterGain = desiredGain + coeff * (limiterGain - desiredGain);
outL = saturatedL * limiterGain;
outR = saturatedR * limiterGain;
```

With:

```cpp
limiterAttackCoeff = exp(-1 / (0.0005 * sr));
```

At 44.1 kHz, this is not instantaneous. A sudden peak can pass above the ceiling before the gain catches up.

If Sil is going to drive harder into the limiter, I’d change attack behavior to never exceed the instantaneous desired gain:

```cpp
if (desiredGain < limiterGain) {
    limiterGain = desiredGain; // hard attack / safety
}
else {
    limiterGain = desiredGain + limiterReleaseCoeff * (limiterGain - desiredGain);
}
```

That will be safer for a mastering chain. If you want the limiter to sound smoother, add actual lookahead delay later. Right now the comment says lookahead-ish, but the signal is not delayed enough to make that fully true.

---

## My recommended tuning order

Do this in this order, not all at once blindly:

1. **Fix glue threshold feedback**

   ```cpp
   pushRollingSample(cleaned.l, cleaned.r);
   ```

   instead of post-limiter output.

2. **Raise glue makeup**

   ```cpp
   kGlueMakeupFraction = 0.75f;
   kGlueMaxMakeupDb = 2.0f;
   ```

3. **Give saturator more makeup, but gentler drive scaling**

   ```cpp
   kSaturatorTargetPreLimiterDb = -0.50f;
   kSatMaxMakeupDb = 3.25f;
   kSatMaxDrive = 1.60f;

   desiredDrive = clamp(
       1.f + desiredMakeupDb * 0.18f,
       kSatMinDrive,
       kSatMaxDrive
   );
   ```

4. **Use limiter feedback inside the saturator**
   Push up when limiter is idle, back off when limiter GR exceeds roughly 1–2 dB.

5. **Add RMS/loudness compensation**
   This is the real “A/B optics” fix.

6. **Make limiter attack hard or add true lookahead**
   Especially if saturation is tuned hotter.

---

## Minimal patch recommendation

If you want the smallest sane patch first:

```cpp
// Constants
static constexpr float kGlueMakeupFraction = 0.75f;
static constexpr float kGlueMaxMakeupDb = 2.0f;

static constexpr float kSaturatorTargetPreLimiterDb = -0.50f;
static constexpr float kSatMaxMakeupDb = 3.25f;
static constexpr float kSatMaxDrive = 1.60f;
```

Change drive mapping:

```cpp
const float desiredDrive = clamp(
    1.f + desiredMakeupDb * 0.18f,
    kSatMinDrive,
    kSatMaxDrive
);
```

Move rolling program update to pre-glue:

```cpp
const sil_micropeak::StereoSample cleaned(preMasterL, preMasterR);
pushRollingSample(cleaned.l, cleaned.r);
```

Remove the later:

```cpp
pushRollingSample(outL, outR);
```

And make limiter attack hard:

```cpp
if (desiredGain < limiterGain) {
    limiterGain = desiredGain;
}
else {
    limiterGain = desiredGain + limiterReleaseCoeff * (limiterGain - desiredGain);
}
```

That should immediately make Sil feel less like it loses level when enabled.

The deeper, more elegant version is to add short-term RMS compensation. That turns Sil from “a chain of good processors” into a coherent mastering intelligence: it can subtract mud, glue dynamics, widen carefully, saturate tastefully, and still emerge with the ceremonial truth of mastering:

```text
same soul,
cleaner body,
slightly larger shadow.
```
