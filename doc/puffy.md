# Puffy v1 Implementation Specification

> Status: implementation handoff for an unreleased module.
>
> Module name and Rack slug: `Puffy`.
>
> Authority: this document replaces the earlier research brief. Where a
> recommendation from that brief conflicts with this document, this document
> defines the Puffy v1 contract.

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
- reduce the processed output by up to 12 dB with `DEFLATE`;
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

It is an output attenuation, not a limiter threshold and not a makeup-gain
control. Puffy never adds post-saturation gain through this control.

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
-> amount smoothing
-> oversample
-> selected character
-> decimate
-> 5 Hz DC blocker
-> Auto Deflate
-> manual DEFLATE
-> selected stereo limiter
-> finite/output guard
```

The limiter must see the final post-filter, post-attenuation stream. Do not put
tone correction, a DC blocker, or output gain after it.

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

At base rate, measure a stereo-linked normalized peak envelope `fast` and RMS
envelope `slow`:

```text
p = max(abs(inL), abs(inR)) / 5 V
fast: 1 ms attack, 45 ms release
slowSq: 180 ms one-pole average of p^2
transient = clamp((fast / max(sqrt(slowSq), 1e-4) - 1) / 2, 0, 1)
```

For each oversampled channel:

```text
drive = 1 + 6*a^2 * (0.65 + 0.55*fast + 0.35*transient)
bias = 0.12*a * (0.25 + 0.75*fast)
zero = tanhAudio(drive*bias)
positiveNorm = max(tanhAudio(drive*(1 + bias)) - zero, 1e-4)
s = (tanhAudio(drive*(x + bias)) - zero) / positiveNorm
s = clamp(s, -1.25, 1.25)
y = lerp(x, s, a)
```

The shared detector prevents channel-independent drive motion from pulling the
stereo image around. The asymmetry intentionally permits a small DC component;
the common post-character DC blocker removes it.

### 5.6 DC blocker

Run an independent first-order 5 Hz high-pass/DC blocker on L and R after
decimation for every character. Recalculate its coefficient in
`onSampleRateChange()`. Applying it uniformly keeps mode changes structurally
consistent and catches filter/startup residue as well as `FRENZY` bias.

### 5.7 Auto Deflate

Auto Deflate is static, mode-aware gain compensation. It does not follow the
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
pre-limiter gain is:

```text
outputGain = dbToLinear(autoDeflateDb + manualDeflateDb)
```

These constants are tuning constants, not user state. They may be adjusted
before v1 release only if the level-matching acceptance test in section 11
demonstrates a systematic mismatch.

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

Use `dsp::Upsampler<4, 8>` on the post-Deflate detector signal. The detector peak
is the maximum absolute reconstructed sample across L and R. Maintain the
maximum over the program-delay horizon with a fixed-capacity monotonic queue,
not `std::deque`.

All delay and queue storage is fixed capacity and supports at least 4096 base
rate samples. At ordinary Rack sample rates this is comfortably larger than the
2 ms requirement. Clamp the configured delay to storage capacity if an unusual
sample rate exceeds it.

Delay the program signal by the same rounded sample count. Compute gain demand
from the lookahead maximum, apply the linked envelope to the delayed program,
then apply a final linked sample guard at the same ceiling. The detector and
program rings reset on sample-rate change, reset, limiter-mode change, and
non-finite input recovery.

The acceptance target is true-peak-safe behavior under the test suite, not
certification against every possible external reconstruction filter.

### 6.3 Limiter-mode changes

Changing limiter mode changes latency. Use the same 5 ms fade-down/reset/5 ms
fade-up transition used for oversampling changes. Do not crossfade delayed and
undelayed streams because that creates comb filtering.

Puffy does not report or compensate latency to the Rack graph. The manual must
state that `MASTER` delays the output by approximately 2 ms plus fixed
resampling-filter latency.

## 7. Visual contract

### 7.1 Rendering approach

Use an original rigged 2D or pseudo-3D puffer fish drawn with NanoVG and cached
raster/vector parts. Do not require `OpenGlWidget`. Static panel and viewport
decoration remains in the panel SVG or a cached `FramebufferWidget`; only the
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
`transientActivity` from the `FRENZY` detector formula even when another
character is selected. Compute `gainReduction` as:

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
src/Puffy.hpp              module declaration, enums, persisted settings
src/PuffyEngine.hpp        allocation-free character and limiter kernels
src/PuffyEngine.cpp
src/Puffy.cpp              Rack configuration, process(), JSON
src/PuffyWidget.cpp        panel, controls, menu, fish widget
res/Puffy.svg
tests/puffy_engine_spec.cpp
```

Reuse `MathHelpers.hpp` and `PanelSvgUtils`. If Puffy and Sil can genuinely
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
  The audio thread owns the actual state transition.
- Output must remain finite for finite or non-finite input. A non-finite sample
  also resets the shared `FRENZY` detectors and linked limiter, not only the
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
- `FRENZY` detector motion is shared; equal input does not create image drift.
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
- Reset and patch load emit no stale audio or gain-reduction state.

### 11.6 Realtime and UI

- A test allocator records zero allocations from repeated `process()` calls,
  including limiter activity and character crossfades.
- Thread sanitization finds no UI/audio data race in snapshot or setting changes.
- Browser preview construction with `module == nullptr` is deterministic and
  does not access audio state.
- On the project reference machine, a release build must process 10 seconds of
  48 kHz stereo audio in less than 1 second at 4x/`MASTER`; 8x must remain at
  least 5x realtime.
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
