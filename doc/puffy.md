# Puffy v1 Implementation Specification

> Status: implementation handoff for an unreleased module.
>
> Module name and Rack slug: `Puffy`.
>
> Authority: this document replaces the earlier research brief and incorporates
> the review findings and architectural confirmations from `puffy-s5-notes.md`
> and `puffy-s5-confirmation.md`. Where recommendations conflict, this document
> defines the definitive Puffy v1 contract.

## 1. Product contract

Puffy is a 12 HP stereo saturation and peak-control module with a responsive
puffer-fish interface. It is a character effect that can safely finish a stereo
Rack signal, not an automatic mastering suite and not a clone of sonible's
`puffer:fish`.

The essential relationship is:

```text
PUFF + input dynamics -> audible character -> readable fish reaction
```

The front panel stays deliberately small:

- three original Leviathan character modes;
- one primary `PUFF` control;
- one `DEFLATE` output attenuation control;
- one `PUFF CV` input and attenuverter;
- stereo input and output.

Oversampling, automatic gain compensation, and limiter quality are context-menu
settings. The fish is a useful meter as well as a mascot: inflation communicates
effective drive, motion communicates input dynamics, and blush communicates
limiter gain reduction.

### 1.1 V1 success case

At the end of v1, a user can:

- patch a mono or stereo signal and obtain a stable stereo output;
- move continuously from an effectively clean signal to obvious saturation;
- select a warm, aggressive, or input-reactive character;
- modulate `PUFF` over its complete range with 0-10 V CV;
- reduce the processed output by up to 12 dB with post-limiter `DEFLATE` output trim;
- choose low-latency sample-peak safety or a 2 ms true-peak mastering limiter;
- compare characters without large accidental loudness jumps when Auto Deflate
  is enabled;
- understand drive, dynamics, and limiting by looking at the fish;
- save and restore all user choices without persisting transient DSP state.

### 1.2 Explicit non-goals

V1 does not include compression, multiband processing, loudness targeting,
side-chain input, wet/dry mix, attack/release controls, user transfer curves,
polyphonic voices, OpenGL, a 3D model, or a claim of bit-identical true-peak
compliance with a particular commercial meter.

Puffy does not use sonible's mode names, artwork, layout, transfer functions, or
trade dress. Its art and mode identities must be original.

## 2. Decisions resolved from the research

| Question | Puffy v1 decision |
| --- | --- |
| Product role | Stereo character saturator with final peak control |
| Width | 12 HP / 60.96 mm / 180 Rack px |
| Characters | `BLOOM`, `SPINE`, and `FRENZY` |
| Default character | `BLOOM` |
| Default amount | `PUFF = 0.25` |
| Stereo I/O | Separate L/R jacks; mono normalization while active |
| Polyphony | Channel 0 only; no polyphonic voice bank in v1 |
| Saturation oversampling | 4x default; 2x and 8x menu choices |
| Limiter | Fully stereo-linked |
| Default limiter | `LIVE`: zero-lookahead sample-peak safety |
| Master limiter | 2 ms lookahead, 4x reconstructed-peak detector, -1 dBFS reference ceiling |
| Level compensation | Static, mode-aware Auto Deflate; enabled by default |
| Visual renderer | Original 2D/pseudo-3D NanoVG artwork; no OpenGL |
| Audio/UI handoff | Atomic scalar snapshot at control rate |
| Persisted state | Rack params plus menu settings only |

The research supports oversampled nonlinear processing, DC protection after
asymmetric processing, a limiter after all nonlinear/filtering stages, and a
graphics fallback that does not depend on OpenGL. The exact algorithms below are
Leviathan design decisions, not reverse-engineered commercial behavior.

## 3. Panel and interaction contract

### 3.1 Panel regions

`res/Puffy.svg` is a 60.96 x 128.5 mm structural panel. A hidden `components`
layer is the source of truth for anchors, loaded through `PanelSvgUtils`, with
matching C++ fallback coordinates.

| Region | x mm | y mm | w mm | h mm |
| --- | ---: | ---: | ---: | ---: |
| Character selector | 4.0 | 7.0 | 52.96 | 11.0 |
| Fish viewport | 4.0 | 20.0 | 52.96 | 48.0 |
| Main controls | 4.0 | 70.0 | 52.96 | 29.0 |
| Jack field | 3.0 | 101.0 | 54.96 | 24.0 |

The art pass may move individual centers without changing the DSP contract.
Keep jacks below the fish so normal cabling does not hide the primary meter.

### 3.2 Required SVG anchor IDs

```text
fish_rect

character_param
puff_param
deflate_param
puff_cv_amount_param

input_l
input_r
puff_cv_input
output_l
output_r

limit_light

screw_tl
screw_tr
screw_bl
screw_br
```

### 3.3 Controls

| Label | Type | Range/default | Meaning |
| --- | --- | --- | --- |
| `CHARACTER` | 3-position snapped switch | 0..2, default 0 | `BLOOM`, `SPINE`, `FRENZY` |
| `PUFF` | large knob | 0..1, default 0.25 | Base saturation amount |
| `DEFLATE` | knob | 0..1, default 0 | 0 to 12 dB of post-effect attenuation |
| `PUFF CV` | attenuverter | -1..1, default 0 | Depth and polarity of amount CV |

The effective amount is:

```text
amountTarget = clamp(PUFF + PUFF_CV_AMOUNT * PUFF_CV / 10 V, 0, 1)
```

Thus +10 V with the attenuverter fully clockwise spans the full normalized
range. Bipolar +/-5 V modulation spans half the range in either direction.
Smooth `amountTarget` with a 1 ms one-pole filter. Character changes use a 10 ms
equal-power crossfade between old and new character outputs.

`DEFLATE` maps linearly in dB:

```text
manualDeflateDb = -12 dB * DEFLATE
```

It is a post-limiter output volume trim of 0 to -12 dB, not a limiter threshold and not a makeup-gain control. Placing it after the limiter guarantees a literal output level trim without altering limiter threshold, gain reduction, or visual metering response. Puffy never adds post-saturation gain through this control.

### 3.4 Ports and normalization

| Port | Contract |
| --- | --- |
| `IN L` | Left audio input and primary mono input |
| `IN R` | Right audio input; normalized from `IN L` |
| `PUFF CV` | Monophonic amount modulation |
| `OUT L` | Left processed output |
| `OUT R` | Right processed output |

If only `IN L` is connected, copy it to both channels. If only `IN R` is
connected, copy it to both channels. If neither is connected, process zero.
Read channel 0 only and always emit one output channel per jack.

Register direct Rack bypass routes:

```cpp
configBypass(INPUT_L, OUTPUT_L);
configBypass(INPUT_R, OUTPUT_R);
```

Rack bypass is literal jack-to-jack routing; active-mode mono normalization is
not promised while the module is bypassed.

## 4. Stable Rack API

Puffy is unreleased, so v1 IDs can be established cleanly. After release, IDs
must only be appended.

```cpp
enum ParamId {
    CHARACTER_PARAM,
    PUFF_PARAM,
    DEFLATE_PARAM,
    PUFF_CV_AMOUNT_PARAM,
    PARAMS_LEN
};

enum InputId {
    INPUT_L,
    INPUT_R,
    PUFF_CV_INPUT,
    INPUTS_LEN
};

enum OutputId {
    OUTPUT_L,
    OUTPUT_R,
    OUTPUTS_LEN
};

enum LightId {
    LIMIT_LIGHT,
    LIGHTS_LEN
};
```

Use `configSwitch()` for `CHARACTER_PARAM`, including the three display labels.
Use dB display scaling or a custom `ParamQuantity` so `DEFLATE` displays 0 to
-12 dB rather than an unexplained normalized value.

## 5. DSP reference and signal flow

Puffy treats nominal Rack audio full scale as:

```cpp
constexpr float kReferenceVolts = 5.f; // +/-5 V == 0 dBFS reference
```

This is an internal calibration convention, not a claim that Rack cables have a
hard digital full scale.

The authoritative signal order is:

```text
input safety
-> dynamics detector update (shared stereo instance)
-> amount smoothing
-> oversample
-> selected character
-> decimate
-> 5 Hz DC blocker
-> Auto Deflate (pre-limiter character compensation)
-> selected stereo limiter
-> manual DEFLATE (post-limiter output volume trim)
-> finite/output guard
```

The limiter must see the post-filter, post-Auto Deflate stream. Manual `DEFLATE` follows the limiter as a literal final volume trim. Do not put tone correction, Auto Deflate, or a DC blocker after the limiter.

### 5.1 Input safety

- Replace a non-finite input sample with zero and reset the affected channel's
  character/filter history before processing the next valid sample.
- Clamp the value entering the oversampler to +/-20 V. This is an internal
  numerical guard, not the audible peak-control mechanism.
- Never allocate, lock, log, or perform file I/O in `process()`.

Normalize each oversampled input sample with:

```text
x = inputVolts / 5 V
```

Character functions operate in normalized space and return normalized values.

### 5.2 Oversampling

Use Rack's fixed-size FIR helpers for both stereo channels. `FACTOR` below
denotes three compile-time-specialized banks, not a runtime template argument:

```cpp
dsp::Upsampler<FACTOR, 8>
dsp::Decimator<FACTOR, 8>
```

The context-menu factors are 2x, 4x, and 8x, with 4x as the default. Maintain
preallocated filter banks for all supported factors or otherwise guarantee that
changing the factor cannot allocate on the audio thread.

The selected character runs at the oversampled rate. The dynamic detectors that
control `FRENZY` run once per base-rate sample and hold their control values
across the generated subsamples.

Changing oversampling factor is a rare configuration event. Fade the processed
output to zero over 5 ms, reset the old and new resampler/DC/limiter state at the
zero crossing, then fade back over 5 ms. Do not hot-swap FIR histories.

### 5.3 Character A: BLOOM

`BLOOM` is smooth, symmetrical, and predominantly odd-harmonic. It should add
density without changing stereo balance or creating a DC offset.

For amount `a`:

```text
drive = 1 + 4*a^2
s = tanhAudio(drive*x) / tanhAudio(drive)
y = lerp(x, s, a)
```

Use `levi_math::tanhAudio()`, not `tanhLegacy()`, so the reference curve is
accurate and shared with the repository's tested math helper.

### 5.4 Character B: SPINE

`SPINE` has a firmer knee, stronger upper harmonics, and more obvious transient
edge. It remains deterministic and symmetrical.

```text
drive = 1 + 9*a^2
z = drive*x

if abs(z) < 1:
    s = z * (1.5 - 0.5*z^2)
else:
    s = sign(z)

y = lerp(x, s, a)
```

The piecewise curve is continuous with zero slope at +/-1. Do not evaluate the
cubic outside that interval. Oversampling is mandatory for this mode.

### 5.5 Character C: FRENZY

`FRENZY` is asymmetrical and input-reactive, but it is not random. Repeated
renders from identical input and state must be identical.

Puffy owns **exactly one** continuously updated, stereo-linked dynamics detector instance:

```cpp
struct PuffyDynamicsDetector {
    float fast;       // 1 ms attack, 45 ms release
    float slowSq;     // 180 ms one-pole average of p^2
    float transient;  // clamp((fast / max(sqrt(slowSq), 1e-4) - 1) / 2, 0, 1)
};
```

This detector updates at base rate for every sample regardless of the active character mode. `FRENZY` reads it when active, and the visual telemetry snapshot reads it in every character mode.

At base rate:

```text
p = max(abs(inL), abs(inR)) / 5 V
fast: 1 ms attack, 45 ms release
slowSq: 180 ms one-pole average of p^2
transient = clamp((fast / max(sqrt(slowSq), 1e-4) - 1) / 2, 0, 1)
fastControl = clamp(fast, 0, 1)
```

For each oversampled channel, calculate dual-branch normalized asymmetry:

```text
drive = 1 + 6*a^2 * (0.65 + 0.55*fastControl + 0.35*transient)
bias = 0.12*a * (0.25 + 0.75*fastControl)
zero = tanhAudio(drive * bias)

positiveNorm = max(tanhAudio(drive * (1 + bias)) - zero, 1e-4)
negativeNorm = max(zero - tanhAudio(drive * (-1 + bias)), 1e-4)

raw = tanhAudio(drive * (x + bias)) - zero

s = (raw >= 0) ? (raw / positiveNorm) : (raw / negativeNorm)

negativeScale = 1 + 0.10 * a * fastControl
if (s < 0):
    s = s * negativeScale

s = clamp(s, -1.25, 1.25)
y = lerp(x, s, a)
```

The dual-branch normalization anchors both positive (+1) and negative (-1) excursions smoothly while preserving input-reactive asymmetry without unconstrained negative hard-clipping. The shared detector prevents channel-independent drive motion from pulling the stereo image around. The asymmetry intentionally permits a small DC component; the common post-character DC blocker removes it.

### 5.6 DC blocker

Run an independent first-order 5 Hz high-pass/DC blocker on L and R after
decimation for every character. Recalculate its coefficient in
`onSampleRateChange()`. Applying it uniformly keeps mode changes structurally
consistent and catches filter/startup residue as well as `FRENZY` bias.

### 5.7 Auto Deflate

Auto Deflate is static, mode-aware pre-limiter gain compensation. It does not follow the
audio envelope and therefore must not pump.

```text
BLOOM:  autoDeflateDb = -2.5*a
SPINE:  autoDeflateDb = -4.0*a
FRENZY: autoDeflateDb = -3.0*a
```

When Auto Deflate is disabled, `autoDeflateDb` is zero. During a character
crossfade, apply the old compensation to the old character output and the new
compensation to the new character output before the equal-power crossfade. Do
not interpolate compensation dB separately. Outside a crossfade, total
pre-limiter gain compensation is:

```text
preLimiterGain = dbToLinear(autoDeflateDb)
```

These constants are tuning constants, not user state. They may be adjusted
before v1 release only if the level-matching acceptance test in section 11
demonstrates a systematic mismatch. Manual `DEFLATE` output trim (`manualDeflateDb`) is applied separately after the limiter.

## 6. Limiter contract

Both limiter modes are fully stereo-linked: derive one gain from the larger L/R
peak and apply that exact gain to both channels. Puffy prioritizes image
stability over partial transient unlinking in v1.

### 6.1 LIVE mode

`LIVE` is the default and adds no lookahead buffer.

```text
ceiling = 5.0 V
peak = max(abs(L), abs(R))
desiredGain = min(1, ceiling / max(peak, epsilon))
```

Gain attack is immediate. Gain release is a one-pole 50 ms return to unity.
Apply `min(currentGain, desiredGain)` on the current sample so no sample exceeds
the ceiling. A final linked guard may correct floating-point residue above the
ceiling.

`LIVE` is a sample-peak safety limiter. The UI and manual must not call it a
true-peak limiter.

### 6.2 MASTER mode

`MASTER` is an optional finishing mode:

| Property | Value |
| --- | ---: |
| Ceiling | `5 V * 10^(-1/20)` = approximately 4.456 V |
| Program delay | 2.0 ms, rounded to nearest sample |
| Detector oversampling | 4x |
| Detector FIR quality | 8 |
| Gain attack | Immediate from lookahead demand |
| Gain release | 80 ms one-pole |
| Stereo link | 100% |

### 6.2 MASTER mode

`MASTER` is an optional finishing mode:

| Property | Value |
| --- | ---: |
| Ceiling | `5 V * 10^(-1/20)` = approximately 4.456 V |
| Program delay | 2.0 ms, rounded to nearest sample |
| Detector oversampling | 4x |
| Detector FIR quality | 8 |
| Gain attack | Immediate from lookahead demand |
| Gain release | 80 ms one-pole |
| Stereo link | 100% |

Use `dsp::Upsampler<4, 8>` on the post-Auto Deflate detector signal. The detector peak
is the maximum absolute reconstructed sample across L and R. Maintain the
maximum over the program-delay horizon with a fixed-capacity monotonic queue,
not `std::deque`.

All delay and queue storage is fixed capacity and supports at least 4096 base
rate samples (~21 ms of headroom at 192 kHz). This capacity is intentionally over-provisioned
to ensure stability across high sample rates and must not be reduced. Clamp the configured
delay to storage capacity if an unusual sample rate exceeds it.

Delay the program signal by the same rounded sample count. Compute gain demand
from the lookahead maximum, apply the linked envelope to the delayed program,
then apply a final linked sample guard at the same ceiling. The detector and
program rings reset on sample-rate change, reset, limiter-mode change, and
non-finite input recovery.

The acceptance target is true-peak-safe behavior under the test suite, not
certification against every possible external reconstruction filter.

### 6.3 Transition coordinator

Puffy uses a single **Transition Coordinator** to resolve concurrent configuration changes cleanly.

Precedence rule:
```text
Structural Transition (oversampling / limiter mode) > Character Crossfade (10 ms)
```

1. **Normal character change**: Performs a 10 ms equal-power crossfade between old and new character outputs.
2. **Structural change (oversampling or limiter mode)**: Initiates a 5 ms output fade-down to zero.
3. **Concurrent requests**:
   - If a character change is requested during an ongoing structural fade-down, record only the target character without starting a parallel character crossfade.
   - If a structural change is requested during an active 10 ms character crossfade, the structural fade immediately overrides and takes control.
4. **Zero crossing**: At zero output, commit the requested oversampling factor, limiter mode, and target character simultaneously. Perform a single atomic reset of DSP histories (resamplers, DC blockers, limiter buffers).
5. **Fade up**: Fade output back up over 5 ms.

This single transactional coordinator guarantees click-free context changes, prevents overlapping mini-state-machines, and eliminates stale lookahead samples.

Puffy does not report or compensate latency to the Rack graph. The manual must
state that `MASTER` delays the output by approximately 2 ms plus fixed
resampling-filter latency.

## 7. Visual contract

### 7.1 Rendering approach

Use an original rigged 2D or pseudo-3D puffer fish drawn with NanoVG and cached
raster/vector parts. Do not require `OpenGlWidget`. Static panel and viewport
decoration remains in the panel SVG or a cached `FramebufferWidget` (using a neutral shared cache helper where appropriate); only the
fish and its small local effects redraw continuously.

The widget must work when `module == nullptr` for the module browser and Deep
Cache preview. Preview state is calm, deterministic, and requires no engine
thread or DSP object.

### 7.2 Audio-to-visual snapshot

At approximately 240 Hz, the audio thread publishes atomic scalar targets:

```cpp
struct PuffyVisualState {
    float effectiveAmount;   // 0..1
    float inputActivity;     // smoothed 0..1
    float transientActivity; // smoothed 0..1
    float gainReduction;     // 0..1, 1 at >= 6 dB GR
    int character;           // 0..2
};
```

Atomics may be individual relaxed scalars plus a sequence counter. The UI must
never read mutable DSP structs directly.

Compute `inputActivity` from a stereo-linked absolute peak follower with 5 ms
attack and 120 ms release, normalized so 5 V maps to 1. Compute
`transientActivity` directly from the single continuous `PuffyDynamicsDetector` instance in every character mode. Compute `gainReduction` as:

```text
gainReduction = clamp((-20*log10(max(limiterGain, 1e-6))) / 6 dB, 0, 1)
```

The UI smooths visual values independently of audio control:

```text
inflation = clamp(0.65*amount + 0.25*inputActivity + 0.10*gainReduction, 0, 1)
```

Character-specific motion:

- `BLOOM`: round body inflation, slow breathing, relaxed fin movement.
- `SPINE`: spine extension follows amount; jaw/outline tension follows
  transients.
- `FRENZY`: asymmetric squash and eye direction follow transient activity; no
  random audio behavior is implied.
- all modes: blush or warning tint follows limiter gain reduction.

The separate `LIMIT_LIGHT` follows the same gain-reduction target and reaches
full brightness at 6 dB reduction.

### 7.3 Idle animation

Blinking, glances, and micro-breathing are cosmetic widget-local animation.
They:

- do not depend on or alter audio RNG;
- do not need to reproduce across patch loads;
- stop advancing when the widget is not visible;
- use bounded update rates and never dirty the whole panel framebuffer.

Target 30 visual updates per second when visible. Drawing at the Rack frame rate
is acceptable, but expensive geometry/state preparation should run only when a
target or idle-animation phase materially changes.

## 8. Architecture and files

Keep Rack integration, DSP, and rendering separable:

```text
src/Puffy.hpp                      module declaration, enums, persisted settings
src/PuffyEngine.hpp                allocation-free character, detector, and limiter kernels
src/PuffyEngine.cpp
src/Puffy.cpp                      Rack configuration, process(), JSON
src/PuffyWidget.hpp                panel construction, controls, menu
src/PuffyWidget.cpp
src/PuffyFishWidget.hpp            NanoVG fish viewport, visibility & frame-rate handling
src/PuffyFishWidget.cpp
src/PuffyCharacterController.hpp   pose calculation, spring physics, idle scheduler
src/PuffyCharacterController.cpp
res/Puffy.svg
tests/puffy_engine_spec.cpp
```

Reuse `MathHelpers.hpp` and `PanelSvgUtils`. Use a neutral shared framebuffer caching helper if available without cross-module coupling. If Puffy and Sil can genuinely
share a tested true-peak detector/limiter kernel without changing Sil's sound,
extract that kernel into a neutral helper in a separate, reviewable change.
Puffy v1 must not silently alter Sil while being implemented.

Register:

- `extern Model* modelPuffy;` in `src/plugin.hpp`;
- `p->addModel(modelPuffy);` in `src/plugin.cpp`;
- a `plugin.json` entry with slug/name `Puffy`, description
  `Character stereo saturator with animated peak control.`, and tags
  `Distortion`, `Dynamics`, and `Visual`.

## 9. Lifecycle and realtime requirements

- Constructors configure fixed storage and safe defaults.
- `onSampleRateChange()` recalculates smoothing, detector, DC-blocker, delay,
  and release coefficients and resets dependent histories.
- `onReset()` resets DSP histories and restores menu settings to their defaults;
  Rack parameters follow normal Rack reset behavior.
- `process()` is bounded, allocation-free, wait-free, and exception-free.
- Context-menu callbacks publish requested enum/boolean changes atomically.
  The audio thread owns the actual state transition via the Transition Coordinator.
- Output must remain finite for finite or non-finite input. A non-finite sample
  also resets the shared `PuffyDynamicsDetector` and linked limiter, not only the
  affected channel history.
- Silence must settle to numerical silence; denormal protection may use state
  zeroing below a small threshold.

Only the currently selected character runs normally. During a character
crossfade, the old and new character run in parallel for at most 10 ms.

## 10. Persistence

Rack automatically persists the four parameters. Custom JSON schema:

```json
{
  "schemaVersion": 1,
  "oversampling": 4,
  "limiterMode": "live",
  "autoDeflate": true
}
```

Validation rules:

- missing or invalid `oversampling` -> 4;
- accepted oversampling values -> 2, 4, 8 only;
- missing or invalid `limiterMode` -> `live`;
- missing or non-boolean `autoDeflate` -> true;
- ignore unknown fields for forward compatibility.

Do not persist envelopes, resampler histories, limiter buffers, crossfade
position, meters, blink timing, or fish pose. Loading a patch begins with reset
DSP state and fades the processed stream in over 5 ms.

Context menu:

```text
Auto Deflate                     [check]

Oversampling
  2x
  4x                             [default]
  8x

Limiter
  Live — zero lookahead          [default]
  Master — 2 ms / true peak
```

Menu labels must expose the latency distinction. Do not hide `MASTER` latency
behind a generic `High quality` label.

## 11. Verification and acceptance tests

### 11.1 Transfer functions

- `amount = 0`, Auto Deflate off, and `DEFLATE = 0` produces unity gain within
  0.05 dB at 1 kHz below limiter threshold.
- Every character is finite and continuous across its piecewise boundaries.
- `BLOOM` and `SPINE` are odd within floating-point tolerance.
- `FRENZY` produces zero output for zero input after state settles.
- `FRENZY` negative excursion bounds and dual-branch normalization pass a test grid across amount (0.25, 0.50, 0.75, 1.00), fast (0.0, 0.5, 1.0), and transient (0.0, 1.0) without unconstrained negative hard-clipping.
- `DEFLATE` operates as a literal post-limiter output volume trim, reducing output level by the exact linear dB amount both above and below limiter engagement.
- All characters become audibly and measurably more nonlinear as amount rises.
- With Auto Deflate enabled, the gated RMS of each character on the shared pink
  noise fixture stays within 1.5 dB of its amount-zero RMS at amounts 0.25,
  0.50, 0.75, and 1.00. Run below limiter engagement and exclude the first
  500 ms. If one static compensation curve cannot pass without making normal
  program material sound obviously quieter, record the exception and tune
  against the common fixture rather than adding an envelope follower.

### 11.2 Aliasing and spectrum

- Test 997 Hz and near-Nyquist sine sweeps at 44.1, 48, 96, and 192 kHz.
- At the default 4x setting, aliased energy for a representative high-drive
  `SPINE` test is at least 12 dB below the same transfer curve run without
  oversampling.
- 8x must not perform worse than 4x by more than 1 dB in the same measurement.
- For a symmetrical steady sine, the mean output over a one-second window
  beginning 500 ms after onset stays below -70 dB relative to a 5 V reference
  in every character.

### 11.3 Stereo behavior

- Identical L/R input produces identical L/R output within floating-point
  tolerance.
- A peak on either side applies exactly the same limiter gain to both sides.
- `FRENZY` dynamics detector motion is shared; equal input does not create image drift.
- Mono normalization works from either input jack while active.

### 11.4 Limiter behavior

- `LIVE` never emits a sample above 5.0 V magnitude after tolerance.
- `MASTER` never emits a sample above its 4.456 V sample guard.
- Offline 4x reconstruction of the standard test set remains at or below the
  `MASTER` ceiling plus 0.1 dB.
- A single-sample impulse, alternating Nyquist-adjacent waveform, sine burst,
  and highly asymmetric `FRENZY` output are all included.
- Gain returns monotonically toward unity after the peak and does not overshoot.
- Limiter gain is bit-identical between L and R.

### 11.5 State and transitions

- JSON round-trips all three custom fields.
- Malformed, missing, and future-valued fields fall back safely.
- Sample-rate changes at 44.1/48/96/192 kHz produce correct delay lengths and no
  stale-buffer output.
- Character changes complete in 10 ms without a discontinuity.
- Limiter/oversampling changes perform the specified fade-reset-fade transition.
- Stress test: rapid concurrent changes to character, oversampling, and limiter mode are safely managed by the Transition Coordinator without clicks, dropouts, or stale lookahead samples.
- Reset and patch load emit no stale audio or gain-reduction state.

### 11.6 Realtime and UI

- A test allocator records zero allocations from repeated `process()` calls,
  including limiter activity and character crossfades.
- Thread sanitization finds no UI/audio data race in snapshot or setting changes.
- Browser preview construction with `module == nullptr` is deterministic and
  does not access audio state.
- On the project reference machine, a release build must process 10 seconds of
  48 kHz stereo audio in less than 1 second at 4x/`MASTER`; 8x (measured at worst-case: `8x saturation oversampling + MASTER limiter + active stereo audio + visual snapshot publication`) must remain at least 5x realtime.
- Hidden/offscreen fish animation does no continuing expensive preparation.

## 12. Implementation order

1. Build and test the standalone engine: oversampling, three characters, DC
   blocker, Auto Deflate, and both limiter modes.
2. Add the Rack module shell, stable IDs, normalization, bypass, lifecycle, and
   JSON.
3. Add the structural SVG and anchored controls.
4. Add the atomic visual snapshot and a simple procedural fish meter.
5. Replace placeholder fish geometry with final original art without changing
   the DSP or persistence contracts.
6. Run audio, transition, preview, and performance acceptance tests before
   making the module visible in `plugin.json`.

## 13. V1 release gate

Puffy is ready to unhide only when:

- all three modes are sonically distinct at matched level;
- the default 4x path meets the aliasing target;
- `LIVE` and `MASTER` satisfy their separately named peak guarantees;
- context changes and patch loading are click-safe;
- stereo linking and mono normalization are verified;
- the fish remains useful at 100% Rack zoom and inexpensive when idle;
- no borrowed commercial names, artwork, or layout remain.

The v1 priority order is sound, stability, stereo integrity, readable animation,
and visual polish. True 3D remains outside the release path.

# Appendix A. Puffy character animation and future 3D integration

> Status: visual architecture guidance and post-v1 development path.
>
> This appendix does not alter the normative v1 contract above. In particular,
> v1 continues to require an original NanoVG/pseudo-3D renderer and does not
> require OpenGL or a 3D asset. The purpose of this appendix is to ensure that
> the v1 character controller can later drive a true 3D Puffy without coupling
> audio processing to a particular renderer.

## A.1 Character principle

Puffy should not behave like a meter wearing a fish costume. He should behave
like a small creature whose physical and emotional state is influenced by the
module.

The visual system therefore combines three independent sources of motion:

1. **Control state** communicates the user's selected `PUFF` amount and
   character mode.
2. **Audio state** communicates actual signal activity, transients, and limiter
   gain reduction.
3. **Autonomous life** provides blinking, breathing, gaze shifts, and small fin
   movements even when the signal is silent.

No one source should completely control the pose. In particular, waveform
polarity and sample-by-sample audio values must never drive visible geometry
directly. Puffy communicates the recent behavior of the processor, not the
individual waveform.

A conventional limiter light remains the precise engineering indicator. Puffy
is a readable character meter and an emotional summary, not a calibrated
replacement for numeric metering.

## A.2 Renderer-independent architecture

Keep the character simulation independent from both the audio engine and the
visual backend:

```text
Puffy module DSP
  -> atomic PuffyVisualState snapshot
      -> PuffyCharacterController
          -> PuffyPose
              -> NanoVG/Pseudo-3D renderer       [v1]
              -> OpenGL 3D renderer              [future]
              -> static/browser preview renderer
```

Suggested responsibilities:

```text
PuffyCharacterController
  - smooths visual targets;
  - runs spring motion;
  - schedules blinks, glances, and fin twitches;
  - interprets DSP state as mood;
  - emits a renderer-neutral PuffyPose.

PuffyPose
  - inflation;
  - body squash/stretch;
  - spine extension;
  - vertical drift and body rotation;
  - left/right pupil direction;
  - independent eyelid closure;
  - mouth expression;
  - fin angles;
  - blush/warning intensity.

Renderer
  - converts PuffyPose into NanoVG parts, sprites, or mesh transforms;
  - owns graphics resources;
  - performs no audio analysis;
  - does not mutate DSP state.
```

A possible renderer-neutral pose structure is:

```cpp
struct PuffyPose {
    float inflation = 0.f;        // 0..1
    float squashX = 0.f;          // small signed normalized offset
    float squashY = 0.f;
    float bodyYaw = 0.f;          // radians, intentionally subtle
    float bodyPitch = 0.f;
    float verticalOffset = 0.f;   // normalized local viewport units

    float gazeX = 0.f;            // -1..1
    float gazeY = 0.f;            // -1..1
    float leftBlink = 0.f;        // 0 open, 1 closed
    float rightBlink = 0.f;

    float mouthSmile = 0.f;       // 0..1
    float mouthTension = 0.f;     // 0..1
    float leftFinAngle = 0.f;
    float rightFinAngle = 0.f;
    float spineExtension = 0.f;   // 0..1
    float blush = 0.f;            // 0..1
};
```

The v1 implementation may contain fewer pose channels, but its controller and
renderer boundary should allow fields to be added without touching the DSP
kernels or persistence schema.

## A.3 Audio-to-character interpretation

The existing `PuffyVisualState` remains the v1 audio/UI contract. The character
controller reads its values and derives slower visual states.

The baseline inflation should primarily reflect the control setting, while
actual signal activity adds only a smaller dynamic contribution:

```text
inflationTarget = clamp(
    0.65 * effectiveAmount
  + 0.25 * inputActivity
  + 0.10 * gainReduction,
  0,
  1)
```

Do not assign the target directly to the rendered pose. Use a damped spring so
Puffy swells, overshoots slightly, and settles like a pressurized soft body:

```cpp
velocity += (target - value) * stiffness * dt;
velocity *= std::exp(-damping * dt);
value += velocity * dt;
```

The exact constants are visual tuning values, but the system should satisfy the
following behavior:

- a slow knob movement appears smooth and deliberate;
- a fast increase produces a small, friendly overshoot;
- audio motion is visible but does not make Puffy vibrate;
- silence returns Puffy toward the amount-defined baseline, not necessarily to
  the fully deflated state;
- limiter activity produces a readable secondary reaction without obscuring the
  amount indication.

Suggested semantic mappings:

| Signal or control | Character interpretation |
| --- | --- |
| `effectiveAmount` | baseline inflation and mode-specific body treatment |
| `inputActivity` | subtle breathing amplitude and buoyant body motion |
| `transientActivity` | brief fin flick, jaw tension, or squash impulse |
| `gainReduction` | blush/warning tint and a small bracing expression |
| `character` | motion vocabulary and resting personality |

A future snapshot may add a smoothed stereo-bias scalar if testing shows value:

```cpp
float stereoBias; // -1 left-heavy, +1 right-heavy
```

This would allow a subtle gaze bias toward the more active channel. It is not
required for v1 and must not destabilize or visually exaggerate the stereo
image.

## A.4 Mode-specific personality

The three DSP characters should share one recognizable Puffy but differ in
motion language.

### A.4.1 BLOOM

- Roundest silhouette.
- Slow, relaxed breathing.
- Soft fin motion.
- Gentle inflation overshoot.
- Open, content expression.
- Limiting appears as warm blush before concern.

### A.4.2 SPINE

- Spines extend more strongly with amount.
- Transients produce a small, quick body tension.
- Fins move less, making the body feel firmer.
- Eyes become slightly more focused at high drive.
- Heavy limiting produces a braced expression rather than panic.

### A.4.3 FRENZY

- Slight asymmetric squash and recovery on strong transients.
- Faster gaze changes, but never continuous jitter.
- More energetic fin flicks.
- Occasional excited expression at high activity.
- The renderer may introduce a very small lateral wobble, but visual randomness
  must never imply randomness in the DSP algorithm.

Mode changes should crossfade or interpolate personality settings over roughly
150-300 ms even though the audio characters crossfade in 10 ms. A slower visual
transition reads as a change of temperament rather than a graphics discontinuity.

## A.5 Autonomous life system

Puffy should continue to appear alert when no signal is present. Autonomous
motion is cosmetic, widget-local, and independent from audio RNG.

Recommended idle behaviors:

| Behavior | Typical interval | Notes |
| --- | ---: | --- |
| Blink | 2.5-7.0 s | Fast close, short hold, softer reopen |
| Double blink | 10-15% of blink events | Second blink follows quickly |
| Gaze change | 1.5-5.0 s | Hold targets long enough to feel intentional |
| Fin twitch | 4-12 s | Low amplitude and occasionally asymmetric |
| Breathing | 3-5 s cycle | Very small change around baseline inflation |
| Vertical drift | continuous | Less than about one rendered pixel at normal zoom |
| Expression event | rare | Avoid constant smiling/frowning changes |

Use a small widget-local PRNG seeded from the module instance or widget address.
This prevents several Puffy modules from blinking in perfect synchronization.
The seed and event schedule do not need to persist across patch loads.

Idle events should be scheduled as state transitions rather than generated by
stacking unrelated sine waves. A useful gaze sequence is:

```text
choose target
-> quick eye saccade
-> damped settle
-> hold
-> optional tiny corrective movement
-> choose next target
```

The eyes are the highest-value animation feature. Small, well-timed pupil and
eyelid movement will contribute more life than large body motion.

Puffy may occasionally glance toward the `PUFF` control after it changes, or
toward the limiter indicator during sustained gain reduction. Avoid continuous
mouse tracking; it is distracting and can become uncanny.

## A.6 Update rate and visibility

Target 30 character simulation and redraw updates per second while Puffy is
visible. The character controller may accumulate real GUI delta time and step at
a bounded fixed interval.

Requirements:

- Do not run expensive geometry preparation at the audio sample rate.
- Stop autonomous event advancement while the widget is not visible.
- Do not dirty the entire panel framebuffer for local fish motion.
- Keep static viewport decoration in the SVG or a separate cached framebuffer.
- Clamp unusually large GUI `dt` values after a stall so springs and event timers
  cannot explode.
- Browser and Deep Cache previews remain deterministic and do not start an idle
  scheduler.

A context-menu animation preference is recommended when the character system is
mature:

```text
Puffy animation
  Full
  Reduced
  Audio reactive only
  Static
```

Suggested behavior:

- `Full`: all audio and autonomous animation.
- `Reduced`: inflation, blinking, and gentle breathing only.
- `Audio reactive only`: no autonomous gaze or fin events.
- `Static`: pose reflects the current amount/character but does not animate.

This setting is optional for v1. If added, it is a visual preference and must not
alter audio or require different DSP state.

## A.7 V1 pseudo-3D implementation path

The v1 NanoVG renderer can approximate the future 3D character using layered,
independently transformed art:

```text
back spines
body shadow
body base
body highlight and texture
side fins
mouth
eye whites
irises/pupils
upper eyelids
front spines and highlights
blush/limiter overlay
```

The body should be authored in multiple compatible shapes or deformation
anchors so inflation is sculpted rather than implemented as uniform scaling.
Uniform scaling would incorrectly enlarge Puffy's eyes, mouth, fins, and spikes
and could reintroduce the oversized cheek masses deliberately removed from the
character design.

Recommended v1 approaches, in increasing complexity:

1. Interpolate between deflated, normal, and fully puffed body contours.
2. Use a small set of pre-rendered body stages and crossfade adjacent stages.
3. Use a mesh-like 2D cage to deform a textured raster body.
4. Combine body-stage sprites with independent vector eyes, pupils, lids, fins,
   mouth, and local highlights.

Option 4 is likely the best quality/performance compromise. Blender can render
16-32 inflation stages from a fixed camera and lighting setup. The runtime then
interpolates between adjacent stages while keeping the expressive face and fins
independent.

The sprite sequence must be treated as an original source asset, not as the
module's only editable master. Preserve the Blender model, material setup,
camera, lights, and export script in the project art source.

## A.8 Future true-3D renderer

After v1 is stable, Puffy may gain a true 3D renderer implemented as an optional
replacement for the pseudo-3D viewport. This is a post-v1 enhancement and must
not block the initial release.

### A.8.1 Mesh budget

A small purpose-built asset is sufficient:

- body and spikes: approximately 1,500-3,000 triangles;
- separate eyes and pupils;
- separate fins;
- simple mouth or a few mouth morph targets;
- one compact texture atlas or simple procedural materials;
- one soft key light, ambient contribution, and a subtle floor shadow.

The exact budget is less important than keeping draw calls and state changes
small. Puffy occupies a limited viewport and does not need film-production
geometry.

### A.8.2 Sculpted inflation morph

Author at least two body forms with identical topology:

1. resting/normal Puffy;
2. fully inflated Puffy.

Store the inflated form as per-vertex position and normal deltas. The vertex
shader or CPU deformation interpolates using the pose inflation value:

```glsl
vec3 position = basePosition + inflation * inflatedPositionDelta;
vec3 normal = normalize(baseNormal + inflation * inflatedNormalDelta);
```

Inflation must be a sculpted morph, not uniform object scale. The inflated form
should:

- round and enlarge the body;
- spread and slightly rotate the spines;
- push the fins outward;
- preserve the designed eye scale;
- keep cheek volume integrated into the spherical body;
- retain a readable mouth at every interpolation point.

Additional small morphs may represent body squash, mouth tension, smile, and
blink shapes. Pupils and fins are better handled as independent transforms.

### A.8.3 OpenGL integration boundary

A future implementation may use Rack's `OpenGlWidget`, but it must preserve the
same controller and pose contracts used by the v1 renderer.

Suggested future file boundaries:

```text
src/PuffyCharacterController.hpp
src/PuffyCharacterController.cpp
src/PuffyPose.hpp
src/PuffyFishWidget.hpp             common widget interface
src/PuffyFishNanoVG.cpp             v1 renderer
src/PuffyFishOpenGL.cpp             optional future renderer
res/puffy/puffy.meshbin
res/puffy/puffy_albedo.png
```

The OpenGL widget owns all GPU resources. The module and DSP engine must not
include OpenGL headers or know which renderer is active.

Use a conservative rendering path compatible with Rack's supported graphics
environment:

- one compact vertex/index buffer;
- one small texture atlas;
- simple vertex and fragment shaders;
- minimal draw calls and state changes;
- no compute shaders;
- no deferred renderer;
- no postprocessing chain;
- explicit restoration of any graphics state changed by the widget.

The renderer should update at approximately 30 FPS and may use framebuffer
caching with explicit invalidation rather than redrawing at the monitor refresh
rate.

### A.8.4 Fallbacks

The 3D source project should also export a deterministic static render matching
the module camera. This supports:

- module browser previews;
- Deep Cache previews;
- systems where the 3D renderer cannot initialize;
- reduced/static animation modes;
- documentation and promotional art.

Failure to initialize optional 3D resources must silently fall back to the
NanoVG or static renderer without changing the audio path.

## A.9 Threading and realtime boundary

The audio thread remains limited to publishing the existing control-rate scalar
snapshot. It must never perform:

- character simulation;
- random event scheduling;
- mesh deformation;
- graphics calls;
- resource loading;
- locking or allocation.

The UI reads the atomic snapshot using relaxed atomics and, if needed, a sequence
counter to avoid torn multi-field observations. The UI then performs all
smoothing and character interpretation independently.

A future expanded snapshot should remain a tiny plain-data structure. Do not
publish mutable DSP objects, detector classes, limiter buffers, or pointers to
engine state.

## A.10 Character-controller pseudocode

```cpp
void PuffyCharacterController::update(
    float dt,
    const PuffyVisualState& visual,
    PuffyPose& pose) {

    dt = clamp(dt, 0.f, 1.f / 15.f);

    updateIdleScheduler(dt);

    const float baseInflation = clamp(
        0.65f * visual.effectiveAmount
      + 0.25f * visual.inputActivity
      + 0.10f * visual.gainReduction,
        0.f,
        1.f);

    const float breathing = idleBreath() *
        lerp(0.004f, 0.012f, visual.inputActivity);

    inflationSpring.setTarget(clamp(baseInflation + breathing, 0.f, 1.f));
    pose.inflation = inflationSpring.update(dt);

    const float transientImpulse = transientEdgeDetector.update(
        visual.transientActivity);

    updateModePersonality(dt, visual.character, transientImpulse, pose);
    updateGaze(dt, visual, pose);
    updateBlink(dt, pose);
    updateFins(dt, visual, transientImpulse, pose);

    pose.blush = gainReductionSmoother.process(
        dt,
        visual.gainReduction);
}
```

This pseudocode is descriptive rather than normative. The important boundary is
that `update()` consumes a snapshot and emits a pose without touching audio or
renderer state.

## A.11 Visual acceptance criteria

The character implementation is successful when:

- Puffy remains recognizable and appealing at 100% Rack zoom;
- his body reads as spherical rather than cheek-heavy at all inflation stages;
- turning `PUFF` produces a smooth, sculpted increase in volume;
- actual audio adds life without causing visible chatter;
- limiting is apparent from expression or blush before the user reads the light;
- blinks and glances feel irregular but intentional;
- several Puffy modules do not animate in lockstep;
- silent patches still show a calm, alert character;
- autonomous behavior never suggests that DSP output is random;
- the user can understand drive and limiting without relying solely on Puffy;
- hidden widgets consume no continuing expensive animation work;
- the browser preview is calm, deterministic, and independent of engine state;
- animation can be reduced or disabled if it becomes distracting;
- a future renderer can replace the v1 renderer without modifying the saturation
  or limiter engine.

## A.12 Recommended development progression

1. Implement the renderer-neutral `PuffyPose` and character controller.
2. Test blinking, gaze, breathing, and inflation using simple circles and pupils.
3. Connect the controller to the v1 `PuffyVisualState` snapshot.
4. Build the layered NanoVG or hybrid sprite renderer required for v1.
5. Tune animation with multiple module instances and real end-of-chain material.
6. Ship v1 only after the visual and realtime acceptance tests pass.
7. Preserve the final character in a compact Blender source project.
8. Prototype the inflated morph and simple OpenGL renderer after v1.
9. Compare the 3D renderer against the v1 renderer for GPU cost, legibility, and
   charm before making it the default.

The desired result is not merely an animated saturator. Puffy should feel like a
small synthetic organism living at the end of the signal chain: breathing in the
program material, swelling with drive, bracing gently against peaks, and
remaining quietly attentive when the music stops.
