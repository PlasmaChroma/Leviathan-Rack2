# Turntable Cartridge Research: Scratch & Battle Carts

This document details the most popular cartridges used by scratch DJs and turntablists. It highlights why these specific models are chosen and how their mechanical design influences their "character" and performance.

---

## 1. Shure M44-7 (The Industry Legend)
*Status: Discontinued in 2018, but still the benchmark.*

*   **Character:** Heavy bass, extremely loud (9.5 mV), and punchy.
*   **Visual Character:** High-contrast "Panda" look (Matte Black body with Bright White stylus).
*   **Color Palette:**
    *   **Body (Black):** `rgb(26, 26, 26)` / `#1A1A1A`
    *   **Stylus (White):** `rgb(255, 255, 255)` / `#FFFFFF`
*   **Why Scratch DJs Pick It:** 
    *   **Skip Resistance:** Known as the "needle that never skips." It was engineered to stick in the groove even during violent scratch techniques.
    *   **Low Record Wear:** Despite its tracking ability, it uses a very light tracking force (1.5g – 3.0g), preserving the life of the vinyl during long practice sessions.
    *   **Sonic Identity:** It has a "vintage" warmth and a massive low-end that makes scratch samples sound "larger than life."
*   **DSP Implementation Notes:**
    *   **EQ:** A gentle "smile" curve with a significant low-shelf boost (approx +2-3dB) centered around 80Hz. A slight dip in the high-mids (3kHz) to reduce "harshness" during fast scratches.
    *   **Saturation:** Subtle even-order harmonics (warmth) to emulate its "Moving Magnet" character.
    *   **Dynamics:** A fast, soft-knee limiter or clipper to handle the high 9.5mV output without digital clipping, giving it that "fat" compressed feel.

### Implementation Notes for TemporalDeck

The M44-7 maps closely to the existing `CARTRIDGE_VINTAGE` character. If breaking it out as its own mode, the key differences from the current Vintage params are:

**EQ shape in `paramsForCartridge()`:**
```cpp
// M44-7: low shelf boost + high-mid dip
// hpHz: keep low (around 20Hz) — the M44-7 passes deep bass cleanly
// bodyHz: set around 80Hz to capture the low-shelf warmth
// bodyGain: positive and relatively strong (~0.18–0.22) for the bass hump
// lpHz: keep high (17kHz+), this cart has extended top end
// presenceGain: slightly negative around 3kHz (the "hardness dip")
return { 20.f, 80.f, 17500.f, 16000.f, 0.20f, -0.06f, 0.004f, 1.02f, 0.008f };
```

**The "fat limiter" character:**
The existing `fastTanh` saturation in `applyCartridgeCharacter()` handles even-order warmth well, but the M44-7's dynamic compression character is more like a soft clipper than a tape saturator. Consider a two-stage approach: run `fastTanh` at a lower drive, then follow with a `clamp(x, -0.85f, 0.85f)` normalized back to unity. This gives the "squashed but not fuzzy" feel of a hot MM signal hitting a mixer preamp.

**motionAmount interaction:**
The M44-7's suspension is relatively compliant — it tracks well but doesn't "stiffen" as dramatically under motion as a battle cart. In `applyCartridgeCharacter()`, consider keeping the `lpHz` blend from `motionAmount` subtle for this mode (reduce the `lpMotionHz` delta vs. `lpHz`).

---

## 2. Ortofon Concorde MKII Scratch
*Status: Current professional standard.*

*   **Character:** Bright, aggressive, and exceptionally loud (10.0 mV).
*   **Visual Character:** Modern and clean (Cool White body with Vibrant Red accents).
*   **Color Palette:**
    *   **Body (White):** `rgb(242, 242, 242)` / `#F2F2F2`
    *   **Accent (Red):** `rgb(230, 0, 0)` / `#E60000`
*   **Why Scratch DJs Pick It:** 
    *   **High Output:** With 10 mV of output, it provides a very hot signal that cuts through a mix, perfect for battle DJs.
    *   **Concorde Design:** The integrated "one-piece" headshell/cartridge design eliminates the need for wire mounting and alignment, making it robust for travel and quick setup.
    *   **Tracking:** Designed specifically for high tracking stability at higher forces (4.0g), ensuring it stays locked during fast back-and-forth movement.
*   **DSP Implementation Notes:**
    *   **EQ:** A high-shelf boost (+3dB) starting around 6kHz for "air" and bite. Very flat low-end (neutral).
    *   **Saturation:** Heavier "transistor-style" saturation to emulate the extremely high output driving a mixer's preamp. Focus on odd-order harmonics for "edge."
    *   **Physical Modeling:** Model a stiffer "suspension" (less phase-shift in the low-end during rapid direction changes).

### Implementation Notes for TemporalDeck

This maps to and should supersede the existing `CARTRIDGE_BATTLE` character, which is currently the closest approximation. Key adjustments:

**EQ shape:**
```cpp
// Concorde MKII: flat low, air boost, moderate presence
// hpHz: slightly higher than M44-7 (~30Hz) — stiff suspension rolls bass faster
// bodyHz: higher (~1800Hz) with low bodyGain — presence-focused, not body-focused
// lpHz: keep high but with a stronger motionHz drop for the "stiffen under scratch" effect
// presenceGain: positive and meaningful (~0.15) for the high-shelf air character
return { 30.f, 1800.f, 18000.f, 12000.f, 0.06f, 0.15f, 0.005f, 1.06f, 0.012f };
```

**Odd-order saturation:**
The current `fastTanh` is a symmetrical function (odd-order dominant), which is correct for this cart. Increase the `drive` parameter (try 1.08–1.12) and compensate with a lower `makeupGain` to keep output level consistent. The "edge" character comes from running slightly hotter into the tanh.

**Stiffer suspension / motionAmount:**
This is the most important physical modeling note for the Concorde. The stiff suspension means direction reversals produce less low-end smear but more transient "snap." In `applyCartridgeCharacter()`:
- Widen the `lpMotionHz` gap aggressively (e.g., `lpHz = 18000`, `lpMotionHz = 10000`) so the cart darkens more sharply under motion
- Consider a brief high-pass transient on direction flip (see Directional Transients section below) — this cart benefits from it more than any other

**The directional transient is currently absent from the engine** and is where the Concorde would most differ perceptually from the current Battle preset. See the General Artifact Modeling section for the recommended implementation.

---

## 3. Stanton 680 HP
*Status: A classic "New York" club and scratch favorite.*

*   **Character:** Warm mid-range, smooth highs, and a "fat" bottom end.
*   **Visual Character:** Industrial/Metallic (Polished Silver body with Cream stylus).
*   **Color Palette:**
    *   **Body (Silver):** `rgb(192, 192, 192)` / `#C0C0C0`
    *   **Stylus (Cream):** `rgb(255, 253, 208)` / `#FFFDD0`
*   **Why Scratch DJs Pick It:**
    *   **The "Sound":** Favored by DJs who prioritize the audio quality of the record as much as the scratch performance. It uses "Moving Iron" technology which many find more musical than standard "Moving Magnet" designs.
    *   **Durability:** Built like a tank. It was the standard in many high-end NYC clubs because it could handle both back-cueing and high-fidelity playback.
*   **DSP Implementation Notes:**
    *   **EQ:** A broad, gentle mid-range boost (400Hz - 1.2kHz) to add "body." A smooth high-frequency roll-off starting at 12kHz to emulate the "silky" highs.
    *   **Saturation:** Very clean, but with a unique "intermodulation" character (Moving Iron). Perhaps use a subtle parallel saturation stage.
    *   **Character:** Slightly "wider" stereo image (crossfeed modeling) to emulate the club/hi-fi heritage.

### Implementation Notes for TemporalDeck

The 680 HP requires a new tonal approach — it's the "audiophile" end of the spectrum and doesn't map cleanly to any existing preset.

**EQ shape:**
```cpp
// Stanton 680 HP: warm body, silky top, slightly wider
// hpHz: very low (18Hz) — extended bass response
// bodyHz: mid-centric (~700Hz) with moderate bodyGain
// lpHz: 12000Hz (the "silky" roll-off is the defining character)
// lpMotionHz: 9500Hz — motionAmount darkens it only gently
// presenceGain: slightly negative (~-0.04) — smooth, not airy
// crossfeed: higher than other modes (~0.012) for the wider image
// stereoTilt: noticeable (~0.02) — Moving Iron has slight channel mismatch
return { 18.f, 700.f, 12000.f, 9500.f, 0.14f, -0.04f, 0.012f, 1.01f, 0.02f };
```

**Moving Iron intermodulation:**
The "parallel saturation stage" mentioned in the research notes is the right call. The existing architecture applies saturation inline in `processChannel()`. For the 680 HP, consider adding a wet/dry blend where the saturated signal is mixed back at low level (~15–20%) rather than replacing the dry signal. Concretely:
```cpp
// Instead of: voiced = fastTanh(voiced * p.drive) / cachedDriveNorm
// For Moving Iron character:
float dry = voiced;
float sat = fastTanh(voiced * p.drive) / cachedDriveNorm;
voiced = dry * 0.82f + sat * 0.18f;
```
This preserves transient clarity while adding subtle harmonic color — the defining quality of Moving Iron vs. Moving Magnet.

**Crossfeed note:**
The existing `crossfeed` parameter in `CartridgeParams` already handles this. The 680 HP value should be meaningfully higher than other carts (0.012 vs. 0.005–0.006 for others) to give that sense of a slightly "collapsed" but spacious stereo image typical of high-compliance club cartridges.

---

## 4. Ortofon Q.Bert (The Specialist)
*Status: Signature model developed with DJ Q.Bert.*

*   **Character:** Highly accentuated mid-range frequencies.
*   **Visual Character:** "The Ghost" (All White body with Jet Black graphics).
*   **Color Palette:**
    *   **Body (White):** `rgb(255, 255, 255)` / `#FFFFFF`
    *   **Graphics (Black):** `rgb(0, 0, 0)` / `#000000`
*   **Why Scratch DJs Pick It:**
    *   **Scratch Articulation:** By boosting the frequencies where scratch "vowels" (ahh, fresh) sit, it makes the scratching sound more defined and "talkative."
    *   **Ultra-High Tracking:** Specifically tuned for the most extreme turntablist techniques.
*   **DSP Implementation Notes:**
    *   **EQ:** A resonant peak (peaking filter) around 2.5kHz - 3kHz with a medium Q. This emphasizes the "vocal" quality of common scratch samples.
    *   **Saturation:** Sharp, aggressive clipping on transients to emphasize the "click" of the crossfader and the "bite" of the scratch.

### Implementation Notes for TemporalDeck

The Q.Bert's "vocal peak" requires something the current `CartridgeParams` struct can't express — a resonant peaking EQ rather than the current shelf/body topology. This is the one cart that may need a structural change.

**The gap in the current EQ model:**
The current `processChannel()` uses a high-pass (rumble removal) + body lowpass + air lowpass architecture. This naturally produces shelf-like curves, but cannot create a mid-frequency peak without adding a dedicated bandpass stage. For a faithful Q.Bert emulation, add a resonant mid stage:

```cpp
struct CartridgeChannelState {
    OnePoleState rumble;
    OnePoleState body;
    OnePoleState air;
    OnePoleState midBp;   // ADD: bandpass for vocal peak
};
```

A one-pole bandpass is not ideal for a resonant peak — consider a simple two-multiplier biquad for the mid stage only. Since this only activates for one cartridge mode, the CPU cost is confined.

**The bandpass peak, approximated with existing tools:**
If a biquad isn't desirable right now, a reasonable approximation: set `bodyHz` to ~2800Hz with high `bodyGain` (+0.35 or more), and set `presenceGain` also positive (+0.20). This creates a pseudo-peak by summing the body and presence boosts near the same frequency. It won't have the Q of a true peaking filter but gets into the right territory for a first pass.

```cpp
// Q.Bert approximate: mid resonance via body+presence stack
// hpHz: moderate (25Hz) — not a bass-heavy cart
// bodyHz: 2800Hz — center of the "vocal" range
// bodyGain: high (0.32) — main source of the peak
// lpHz: 16000Hz — not rolled off, this cart is bright
// presenceGain: high positive (0.22) — stacks with body for pseudo-peak
// drive: higher (1.08) — aggressive transient clipping character
return { 25.f, 2800.f, 16000.f, 14000.f, 0.32f, 0.22f, 0.003f, 1.08f, 0.005f };
```

**Transient aggression:**
The Q.Bert's "click of the crossfader" character is partly about transient sharpening, not just EQ. The `scratchFlipTransientEnv` system in the existing engine already models direction-change transients — the Q.Bert preset should use a higher transient multiplier. This isn't currently parameterized per-cartridge; consider adding a `transientGain` field to `CartridgeParams` that scales the `jerk * 0.18f` coefficient in the scratch flip transient calculation.

---

## Technical Features Table

| Feature | Shure M44-7 | Ortofon MKII Scratch | Stanton 680 HP | Ortofon Q.Bert |
| :--- | :--- | :--- | :--- | :--- |
| **Primary Color** | Matte Black | Cool White | Polished Silver | White |
| **Stylus Tip** | Spherical (0.7 mil) | Spherical | Spherical | Spherical |
| **Output Voltage** | 9.5 mV | 10.0 mV | 8.0 mV | 11.0 mV |
| **Tracking Force** | 1.5g – 3.0g | 3.0g – 5.0g | 2.0g – 5.0g | 3.0g – 4.0g |
| **Best For** | Battle / Practice | Professional Gigs | Club / Sound Quality | Articulation |

---

## General Turntable Artifact Modeling

Beyond individual cartridge EQ/Saturation, a high-quality turntable model should include:

### 1. Stylus Friction (The "Grind")
- **Mechanism:** As the record moves under the needle, friction creates a broadband noise (surface noise).
- **DSP:** A low-level "pink noise" source modulated by the absolute playback velocity. When the speed is 0, the noise stops. As speed increases, the noise amplitude and "brightness" (LP filter cutoff) increase.

#### TemporalDeck Implementation Note
The existing `CARTRIDGE_LOFI` mode already implements hiss via `lofiRandSigned()` and `hissBase`. For non-lofi carts, a much subtler version of this is appropriate during scratch — the `motionAmount` variable in `applyCartridgeCharacter()` is exactly the right modulation source. A starting point:
```cpp
// Subtle stylus noise — add after voiced computation, before makeup gain
// Only active during scratch (motionAmount > 0)
if (motionAmount > 0.01f) {
    float noiseFloor = 0.00018f * motionAmount; // ~-75dBFS at full motion
    x += noiseFloor * lofiRandSigned();           // reuse existing RNG
}
```
Keep this extremely quiet for non-lofi modes. It's felt more than heard, but its absence is noticeable on headphones.

---

### 2. Tracking Distortion
- **Mechanism:** Inner-groove distortion occurs as the stylus approaches the center of the record.
- **DSP:** A variable saturation/LP-filter that increases as the `readHead` (position) moves closer to the virtual "center" of the buffer.

#### TemporalDeck Implementation Note
Inner-groove distortion on a real record relates to physical groove geometry — not directly analogous to a delay buffer. However, a plausible creative mapping: as `scratchLagSamples` approaches `accessibleLag` (the "oldest" part of the buffer), the signal has been in the buffer longest and could plausibly be treated as "more worn." Applying a gentle extra drive and LP rolloff at high lag ratios would be a musically useful approximation:
```cpp
float lagRatio = scratchLagSamples / std::max(accessibleLag, 1.f);
float innerGrooveExtra = lagRatio * lagRatio * 0.04f; // quadratic, subtle
// Add to existing drive before saturation
```

---

### 3. Harmonic Wobble (Wow/Flutter)
- **Mechanism:** Uneven platter rotation or off-center spindle holes.
- **DSP:** A low-frequency (0.5Hz - 4Hz) LFO modulating the `speed` variable. The "Battle" carts (M44-7, Concorde) should have *less* of this due to high-torque direct-drive motors, while "Vintage" modes could have more.

#### TemporalDeck Implementation Note
The existing lofi path already implements wow/flutter via `lofiWowPhaseA`, `lofiWowPhaseB`, and `lofiFlutterPhase`. These apply a pitch modulation (`wearTilt`) only in `CARTRIDGE_LOFI` mode. For other cartridge modes, wow/flutter should modulate `speed` (in `computeBaseSpeed()` or at the readhead advance step), not the audio signal directly. Per-cartridge wow depth values to consider:

| Cartridge | Wow Depth | Flutter Depth | Notes |
|---|---|---|---|
| M44-7 (Vintage) | 0.0035 | 0.0010 | Light — direct drive but older |
| Concorde (Battle) | 0.0015 | 0.0008 | Minimal — modern high-torque |
| Stanton 680 HP | 0.0045 | 0.0018 | More — club deck, older units |
| Q.Bert | 0.0020 | 0.0012 | Low — performance-focused |
| Lo-Fi | 0.0064 | 0.0022 | Current implementation |

Apply as a multiplier on `speed` before the readhead advance: `speed *= (1.f + wowValue)` using the existing phase accumulators factored out to be per-cartridge rather than lofi-only.

---

### 4. Directional Transients
- **Mechanism:** The physical cantilever "snaps" slightly when reversing direction.
- **DSP:** An envelope follower on the velocity delta. When direction flips, trigger a very short (5ms) impulse or high-pass "pop" to emulate the physical movement of the stylus in the groove.

#### TemporalDeck Implementation Note
This is **already partially implemented** via `scratchFlipTransientEnv` and the `prevScratchDeltaSign` / `deltaSign` direction-change detection in the legacy scratch path. The hybrid path explicitly zeros `scratchFlipTransientEnv` and bypasses the transient system entirely. This is one of the clearest perceptual differences between the two modes right now.

The recommended approach is to make the directional transient a cartridge-parameterized feature rather than a model-specific one:

1. Add `float transientGain` to `CartridgeParams` (range ~0.0–1.0, default 0.0 for Clean)
2. In the hybrid path, instead of zeroing `scratchFlipTransientEnv`, run a scaled version of the same direction-detection logic:
```cpp
// In hybrid scratch output processing, replace the current zero-out block:
if (hybridManualScratch && p.transientGain > 0.f) {
    int deltaSign = (readDeltaForTone > 0.2f) ? 1 : ((readDeltaForTone < -0.2f) ? -1 : 0);
    if (deltaSign != 0 && prevScratchDeltaSign != 0 && deltaSign != prevScratchDeltaSign) {
        float jerk = std::fabs(readDeltaForTone - prevScratchReadDelta);
        scratchFlipTransientEnv = std::max(scratchFlipTransientEnv,
            clamp(jerk * 0.18f * p.transientGain, 0.f, 1.f));
    }
}
```

Suggested `transientGain` values per cartridge:

| Cartridge | transientGain | Character |
|---|---|---|
| Clean | 0.00 | None |
| M44-7 (Vintage) | 0.35 | Soft, compliant cantilever |
| Concorde (Battle) | 0.75 | Pronounced — stiff suspension snaps |
| Stanton 680 HP | 0.25 | Smooth — Moving Iron damps it |
| Q.Bert | 0.90 | Maximum — this IS the cart's signature sound |

---

## Suggested `CartridgeParams` Struct Extension

To support the above without breaking existing modes, add two fields to `CartridgeParams`:

```cpp
struct CartridgeParams {
    float hpHz        = 0.f;
    float bodyHz      = 0.f;
    float lpHz        = 0.f;
    float lpMotionHz  = 0.f;
    float bodyGain    = 0.f;
    float presenceGain= 0.f;
    float crossfeed   = 0.f;
    float drive       = 1.f;
    float stereoTilt  = 0.f;
    float transientGain = 0.f;   // ADD: directional snap character
    float noiseFloor  = 0.f;     // ADD: stylus friction noise level
};
```

Both new fields default to zero, so all existing `CARTRIDGE_CLEAN` behavior is unchanged and the extended constructor remains backward compatible.

---

## Expanded `CartridgeCharacter` Enum

```cpp
enum CartridgeCharacter {
    CARTRIDGE_CLEAN,
    CARTRIDGE_VINTAGE,   // existing — maps to general M44-7 character
    CARTRIDGE_BATTLE,    // existing — maps to general Concorde character
    CARTRIDGE_LOFI,      // existing
    CARTRIDGE_STANTON,   // NEW: 680 HP
    CARTRIDGE_QBERT,     // NEW: Q.Bert vocal peak
    CARTRIDGE_COUNT
};
```

The `cartridgeLabel()` function, `paramsForCartridge()`, `makeupGainForCartridge()`, and `appendContextMenu()` submenu all need updating — all are straightforward additions following the existing pattern.
