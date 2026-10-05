# Lúbadh 2.1.0 — firmware reverse engineering for a native Rack instrument

**Analyzed input:** `lubadh-v2.1.0.tar.gz` supplied in this conversation.  
**Scope:** ARM executable/static data, factory presets, update/file-management scripts, selected original-instruction arithmetic probes, and official product/2.1 quickstart cross-checks.  
**Deliverable status:** substantial engineering analysis and tested reference kernels, **not** a completed Rack module or a hardware-equivalent emulator.

## 1. Executive findings

This is a particularly informative update package. It contains ordinary, dynamically linked **32-bit ARM/Linux executables with C++ symbols intact**, rather than an opaque encrypted microcontroller image. The main program exposes named application, transport, tape-effect, clock, state-machine, and file-management components. Ten richly commented factory presets and an HTML preset editor supply an unusually useful configuration specification alongside the machine code.

The central instrument is not merely a stereo delay with a tape effect inserted afterward. It has two deck engines, distinct initial-record/overdub/playback states, variable-speed **writing as well as reading**, multiple logical playback engines, transition taps, pitch-hold modes, loop-region selection, event scheduling, and preset-selectable clock/record semantics. Reproducing these relationships matters more than substituting a generic tape saturator.

The strongest newly recovered details are:

- The active tape-color chain has separately recoverable filtering, signed-square wear, a short signed-delay diffuser, flutter/crinkle modulation, and nonlinear clipping. Some attractive-looking class names elsewhere in the executable are **not the classes used by the main processing path**.
- The crossfade table is generated from a hybrid linear/sinusoidal amplitude curve. It is neither a plain linear fade nor an ordinary constant-power crossfade.
- The overdub buffer write is `writeClip(inputContribution + oldBuffer * feedbackCoefficient)`. Playback reverb is not directly substituted for `oldBuffer` in that routine.
- The tap organization is **four logical engines with five transition slots apiece**, not straightforward twenty-note polyphony.
- There are **three different timebase numbers**: requested JACK 48,000 Hz, a recurring DSP constant of 49,170.25390625 Hz, and SoX file conversion at 49,148 Hz. Treating them as interchangeable would bake unexplained pitch/timing errors into a recreation.
- Eight selected ARM routines were exercised through a deliberately restricted instruction interpreter. **5,925 numerical comparisons passed**. A separate C++ reference suite passed **8,154 invariant checks**. These are useful arithmetic checks, not end-to-end firmware or hardware validation.

For implementation, start with a native, deterministic two-deck transport and the recovered kernels. Keep the analog interface, clock calibration, unresolved tap transitions, anti-alias behavior, and complete plate network explicitly open rather than filling them with plausible-sounding guesses.

## 2. Evidence rules and reproducibility

This report uses five evidence levels:

| Label | Meaning |
|---|---|
| **F — file fact** | Directly present in the supplied bytes, symbols, preset values, or script text. |
| **S — static reconstruction** | Derived from inspected instructions and dataflow; the complete path has not necessarily been executed. |
| **P — numerical probe** | Actual selected instruction text from the supplied binary executed in the included restricted ARM/VFP interpreter, compared with an independently expressed formula. |
| **D — documentation** | Official public product/update/quickstart information; not proof of exact machine-code behavior. |
| **R — recommendation** | A proposed implementation choice for Rack, not a claim about the original. |

All addresses below are **virtual addresses in the uploaded `lubadh_main` executable**, unless another binary is named. These are not raw file offsets. The primary file hash is:

```text
lubadh_main SHA-256
2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4
```

`evidence/manifest.json` hashes every extracted package file. `tables/recovered_constants.json` also records the original archive hash. `evidence/main_disassembly.txt` preserves LLVM output; the individual `evidence/functions/*.asm` files provide manageable function-level views with literal/import annotations. Annotation comments are aids, not a substitute for checking instructions: PC-loaded doubles can actually contain two unrelated 32-bit values, and linear string-reference annotations are not a complete dataflow analysis.

No updater, launcher, hardware driver, or complete firmware process was run. The update scripts include system-changing operations and are included as **inert evidence only**. No claims are made about a booted device, its analog electronics, actual codec clock, scheduler latency, or audio listening tests.

The official full 2.0 manual was located but could not be retrieved successfully in this session. It was **not fully read**. The 2.1 quickstart was inspected, including relevant page images; the product, firmware, and preset-editor information was cross-checked. A long manual's existence is not being counted as reviewed evidence.

## 3. Package, platform, and software architecture

### 3.1 Executables and configuration

| Executable | Bytes | Apparent responsibility |
|---|---:|---|
| `lubadh` | 27,768 | Supervisory/startup application |
| `lubadh_main` | 642,160 | Main UI/control and audio engine |
| `preset_load` | 236,148 | Hjson preset parsing and handoff |
| `audio_load` | 121,840 | Audio loading/import coordination |
| `audio_save` | 78,320 | Save/export coordination |
| `autosave_audio` | 85,216 | Automatic audio persistence |
| `autosave_data` | 299,024 | Automatic configuration/state persistence |
| `erase` | 79,800 | Erasure worker |

The package also contains ten factory `.txt` presets, the browser preset editor, and startup/update/USB/SoX scripts. The preset parser successfully recovered **31 flattened fields** from every factory preset. The complete values and cross-preset comparison are supplied, rather than only a few hand-picked defaults.

The main ELF contains 4,823 symbol-table entries and 1,635 defined function symbols. These counts include library machinery and aliases; they do **not** mean 1,635 distinct DSP algorithms. The curated application/DSP function set contains 272 unique addresses. **F**

### 3.2 Runtime environment

The executables are little-endian ARM ELF/EABI5 hard-float binaries with dynamic uClibc linkage; the interpreter path is `/lib/ld-uClibc.so.0`. Attribute/build evidence includes ARMv8 application-profile code, VFP/NEON support, and a Buildroot GCC 9.4.0 toolchain string. Older compiler strings in startup components do not establish that the entire application used that compiler. The exact hardware model should not be guessed from `wiringPi` alone. **F**

The software uses JACK, Linux threading, shared memory/semaphores, GPIO/ADC handling, Hjson parsing, and separate helper processes. This is an update payload, not a full system image: dependent libraries, the actual codec/driver configuration, and every calibration source are not supplied here.

`AudioEngine::process` at `0x49244` coordinates presets, IO, deck processing, and display updates. Its normal two-deck path creates a worker thread for one channel, processes the other, and joins. The inspected path includes allocation for thread state. That may have been acceptable in the original appliance, but it is **not a design to copy into Rack's audio callback**. Calls named file loader/saver in the callback show coordination; their presence alone does not prove every disk operation occurs synchronously there. **S**

### 3.3 Hardware-revision gain

`Application::setup` at `0x2b03c` checks a hardware input and selects an input compensation of **1.0** or approximately **3.1**. Associated strings distinguish new hardware requiring no compensation from old hardware requiring a boost. That value is passed into the audio engine and applied to input samples. **S**

This is a useful warning against deducing Rack voltage scaling from one unexplained DSP gain. It is revision compensation, not a recovered universal volts-to-float calibration. The analog interface remains a separate model.

## 4. Timebases, capacity, and latency

| Domain | Recovered value | Evidence and interpretation |
|---|---:|---|
| Requested JACK sample rate | 48,000 Hz | Literal startup command; does not itself measure the physical converter clock. |
| JACK period | 128 frames | Literal `-p128`. |
| ALSA periods | 3 | Literal `-n3`; not a complete input-to-output latency measurement. |
| DSP arithmetic rate | 49,170.25390625 Hz | Float bits `0x47401241`, repeatedly embedded in DSP/control calculations. |
| Audio-file conversion rate | 49,148 Hz | `soximport`, `soxexport`, `soxpreview`. |
| Deck capacity | 29,502,000 float samples | Shared-memory array type and indexing bounds. |
| Audio storage per deck | 118,008,000 bytes | Capacity × four bytes. |

The exact command is:

```sh
jackd -P70 -dalsa -r48000 -p128 -n3 &
```

The two principal buffers alone total about **225.08 MiB**, before effect delay lines, temporary vectors, saved states, and other overhead. Their capacity is exactly `600 * 49170` samples. At a genuine 48 kHz clock, the same sample count would last **614.625 seconds**. At the recurring DSP rate it is very slightly less than 600 seconds. Those are arithmetic consequences, not evidence that the hardware actually records for either exact duration. **F/S**

The difference between the SoX and DSP numbers is small but real. The difference between 48 kHz and approximately 49.17 kHz is large enough to affect pitch and timing. A suitable hardware experiment is a known-frequency recording exported to WAV, combined with an independently timed long recording. Until that is done, preserve these domains separately in the reconstruction.

**Rack recommendation:** provide an explicit compatibility timing policy. A research mode can use fixed 128-frame processing and literal legacy constants. A host-native mode can preserve seconds and Hz at the host rate, with deliberate conversion of delay lengths, smoothing, loop lengths, and control periods. Do not silently mix host-rate storage with legacy-rate pitch formulas. Any eventual oversampling wrapper must keep transport time separate from effect processing rate. **R**

## 5. Controls and IO

### 5.1 Functional inventory

Each deck needs an audio input/output path; speed, start, length, and Time controls; Record, Retrig/Shift, and Erase actions; configurable recording and retrigger inputs; and clock output behavior. The auxiliary input/output crossfaders and deck link are part of the instrument, not decorative controls. Capacitive interaction is a separate speed disturbance, not just a second manual speed knob.

The official auxiliary diagram shows input distribution and output mixing with curved fades and audio-rate modulation capability. It does **not** numerically define the analog crossfader transfer law. The expander supplies Start, Length, Time, and Erase inputs for each deck. Its note says **“bipolar (5Vpp)”**; that is not equivalent to saying ±5 V. The preset speed comments, meanwhile, describe a 0–5 V mapped control domain. Hardware offsets, attenuation, summing, and clipping must therefore be measured or traced before asserting exact physical jack ranges. **D/F**

For a Rack adaptation, normalized parameter domains can be defined now; volts-to-domain conversion should remain an explicit adapter. The proposed contract in `RACK_IMPLEMENTATION_HANDOFF.md` is labeled as a design, not recovered circuitry.

### 5.2 Speed modes

`SpeedControl` selects Notched, Stepped, Smooth, or V/oct behavior. Default positive speed markers are `[0, 0.5, 1, 2, 4]`; some sequencing/delay presets add `0.25`. The executable contains named `fillSpeedTable` routines and a 4,096-entry control table. Exact notch widths, sign handling, and step boundaries deserve a separate table reconstruction rather than an arbitrary quantizer. **F/S**

The supplied preset comments specify this V/oct control mapping:

```text
0 <= V < 1: speed = 0.25 * V
1 <= V <= 5: speed = 2^(V - 3)
```

Thus 1 V gives 0.25×, 3 V gives 1×, and 5 V gives 4×. Reverse in this mode is a separate deck-flip operation. This is not automatically the most intuitive Rack pitch input convention; a Rack-centered 0 V = original pitch option would be a deliberate adaptation. **F/R**

`setPotSpeed` at `0x37510` and `processSpeed` at `0x37408` implement a finite ramp with block-based steps. The inspected slew setup uses an integer-converted time divided by a nominal **2.7** and then adds one step. It is not simply a one-pole exponential smoother whose time constant equals the preset milliseconds. Flutter and touch multiply/disturb speed separately. **S**

### 5.3 ADC behavior and control change detection

`ADC::interpretVal` at `0x6c104` smooths into an **integer cached value**:

```text
cached = trunc(float32(alpha*raw + (1-alpha)*cached))
```

That integer history was numerically probed. The input is constrained to a 12-bit domain in the examined control path. Speed change detection requires a difference greater than nine counts; Time requires greater than 29. Replacing this with unconstrained high-resolution float smoothing may remove recognizable control behavior. For a modern Rack option that is fine, but keep it distinct from a compatibility path. **P/S**

### 5.4 Time, clock, and quantization

Time has ordinary clock-division and dub-level functions plus a preset-selected third function: crossfade duration, speed-slew amount, or tape amount. `TimePot=2` means the third function is tape amount; it does **not** mean the entire Time UI has only three raw preset enum states. **F/S**

Clock division sets are:

```text
All:       1,2,3,4,5,6,7,8,9,10,11,12,16,24,32,64
Even:      2,4,6,8,10,12,16,24,32,64
Odd:       1,3,5,7,9,11
Powers 2:  1,2,4,8,16,32,64
```

Start/length quantization uses corresponding lists without 1. A source-comment contradiction says both that `Quantisation=0` disables quantization and that it chooses the All list. Runtime enable/selection is separate from the preset list selector. Do **not** implement `Quantisation=0` as unconditional disabling based solely on that sentence. **F/S**

`RecordJack=2` changes the **record input** into an external clock input. The averaging setting counts inter-pulse durations; resolution is pulses per **full loop**, not inherently pulses per quarter note. Resolution zero follows the selected clock division. Timeout zero disables expiration. The default average is four and resolution is 64. The Clockable preset uses a three-second timeout. **F**

The quickstart overview wording about retrigger synchronization should not override the explicit configurable RecordJack definition. Retrigger remains a loop-start/playhead event; external-speed clocking is a separate mode. `RetrigDelay`, used by several sequencing presets, is handled through a block countdown. Exact first-edge, lost-clock, simultaneous-event, and output pulse-width behavior remains unprobed. **S**

### 5.5 Selected version-2.1 UI cross-checks

The 2.1 quickstart shows independent per-deck preset selection: hold both Shift controls and the target Erase, choose with Time, and load with Record. The runtime menu accommodates up to twelve files, while this update ships ten factory presets. Monitoring and one-shot recording/playback are independent modes, not additional factory presets. Avoid carrying an older two-slot A/B preset assumption into this version. **D/F**

The complete chord-to-event priority matrix has not been proven. A native event layer should represent semantic actions first and map button chords afterward. This prevents modifiers used for saving, preset selection, or mode changes from accidentally generating musical retriggers.

## 6. Deck state, transport, and multi-tap behavior

### 6.1 Explicit deck states

The executable names and dispatches four principal states:

| State | Constructor VA | Button handler VA |
|---|---|---|
| Empty deck | `0x37008` | `0x3a470` |
| First recording | `0x37024` | `0x3a6ac` |
| Overdub recording | `0x3704c` | `0x3a4ec` |
| Playback | `0x37068` | `0x3a5a0` |

`Channel::setState` is at `0x3a018`. Their distinction is substantive: first recording establishes valid recorded extent and playback setup; overdubbing modifies existing material; playback and empty states interpret recording/arming differently. Inspected record event paths move Empty → FirstRec, FirstRec → Playback, Playback → Overdub, and Overdub → Playback, with additional gate/arming conditions. This is not a complete event transition table for every mode. **S**

The first-record handler checks minimum completion conditions, updates recorded extent, allocates/activates playback taps, and handles hold-speed configuration. Do not assume the `MinLength` preset alone defines every first-record stopping guard: it is clearly used for selected loop-region length, while the first-record handler has additional block-related checks. **S**

### 6.2 Variable-speed writing versus fixed-speed writing

The most consequential transport preset is `RecordSpeed`:

- **0 / variable:** writing follows signed transport speed. Incoming audio must be resampled into tape coordinates; recording backward is distinct from merely reversing playback afterward.
- **1 / fixed:** writing remains forward at original speed while playback can run at another speed. This supports the older-style pitch-shifting/delay behavior.

`recordInput` at `0x47818` calculates the magnitude of effective record speed, uses an inverse-speed increment when sufficiently nonzero, preserves a fractional interpolation position, and writes across the tape indices traversed by the record tap. When the tap has crossed multiple storage samples, the input is interpolated separately for those writes. A near-zero guard uses approximately `1e-6` to avoid inverse-speed blowup. **F/S**

This is why a simplistic implementation that writes exactly one sample per audio frame and changes only a playback cursor is insufficient. It can approximate the fixed-record preset but not the variable-speed tape model. Zero-speed, direction changes, multiple writes per frame, and transitions need explicit regression cases.

### 6.3 Input interpolation and read addressing

`Channel::InputBuffer::interpolate` at `0x38ab4` reads four samples `a,b,c,d` and evaluates:

```text
delta = c - b
y = b + t * [ delta
      - (1-t) * inv6 * (d + 2a - 3b + t*(d-a-3*delta)) ]
inv6 float bits = 0x3e2aaaad
```

This is a four-point cubic/Lagrange-type expression, not ordinary linear interpolation and not the usual Catmull–Rom formula. The slightly unusual stored `inv6` value is **0.16666670143604279**. The arithmetic and sampled positions were probed. **P**

The InputBuffer implementation clamps an integer sample index but computes the fractional coordinate afterward; out-of-range positions can therefore extrapolate rather than behave like a separately clamped fraction. That is not an invitation to feed arbitrary positions in the native engine.

`AudioData::interpolate` at `0x38748` uses the same form but different addressing: the inspected positive/negative speed paths use different integer anchors, including `position+2` versus `position-3`, with boundary clamps and neighboring reads. The negative-speed path is not just a positive interpolator given a negative phase increment. That address selection is statically recovered, not numerically verified as a complete moving tape reader. **S**

### 6.4 Logical voices and transition storage

`TapManager::activate_new` at `0x4eea8` and `TapEngine::activate` at `0x4e358` reveal **four engines**, each occupying 680 bytes. Each engine contains **five 132-byte Tap slots** plus bookkeeping. A free engine is preferred. If none is free, an oldest-engine selection participates in fade-out/replacement. Within an engine, activation searches for a free transition slot and can return null if all slots are occupied. **S**

This storage arrangement supports overlap during retriggers, direction/boundary changes, and fades. It should not be presented as twenty independently controllable musical voices. A faithful model needs two levels: musical engine allocation and short-lived tap transitions. Stress behavior when many transitions overlap still needs targeted execution.

`PlaybackSpeed.Looping` and `.Oneshot` independently choose Follow or Hold. Hold snapshots speed at tap creation, allowing simultaneous pitches when MultiTap is enabled. It is not a global sample-and-hold applied to the deck's speed knob. **F/S**

### 6.5 Start, length, and fades

`setLoopingParameters` at `0x376ac` maps normalized start/length into the recorded span. The inspected unquantized start path uses the 12-bit control fraction and clamps inside the recorded range; the length path uses the recorded length minus one with a minimum based on `MinLength`. Quantized paths use floor-grid operations; length includes a one-unit offset rather than simply rounding to the nearest duration. Modifier-held unquantized offsets need further event-level testing. **S**

The selected crossfade span is bounded by the region, including a half-region limit, and an inspected path enforces a **128-sample floor** at `0x37888`. Consequently a preset duration of zero does not establish universally instantaneous splices. Small loops and overlapping transitions deserve special care.

## 7. Recording and playback signal paths

### 7.1 Observed active processing order

The main channel processor starts at `0x47c08`. On its active tape-processing path, the calls occur in this order:

```text
Input preparation / hardware gain compensation
  → TapeInFilter                         call 0x47d4c
  → TapeFilter (TapeAge amount)           call 0x47d5c
  → TapeCompander (Wear amount)           call 0x47d6c
  → TapeAllpass (Hysterisis amount)       call 0x47d7c
  → input anti-alias stage               call 0x47ddc
  → InputBuffer                          call 0x47df0

Tape/tap playback and mixing
  → playback anti-alias stage            call 0x483ac
  → voice-level / monitoring-related mix
  → TapeFilter                           call 0x48464
  → TapeCompander                        call 0x48474
  → TapeAllpass                          call 0x48484
  → MonoPlate                            call 0x484a4, once per output sample
  → preview contribution                 call 0x484c4
  → output SoftClipper                   call 0x484d4

Recording work for active record taps
  → recordInput                          call 0x48518
  → AudioData::add                        call 0x47a44
  → write SoftClipper                     call 0x3887c
  → overwrite addressed tape samples
```

This is a **call/dataflow reconstruction with conditional branches**, not a claim that every state always passes through every box. Monitoring, preview, record envelopes, tap mixing, and bypass cases have additional branches. The exact anti-alias cutoff adaptation and every branch's signal ownership are not fully reconstructed.

### 7.2 The decisive overdub equation

`AudioData::add` at `0x387fc` first combines the already prepared input-contribution vector with existing stored samples and a per-write feedback vector. It then calls a SoftClipper and commits the resulting vector back to the addressed buffer positions:

```text
for each addressed sample j:
    candidate[j] = newInputContribution[j]
                 + existingTape[index[j]] * feedbackCoefficient[j]

writeClipper.process(candidate)

for each addressed sample j:
    tape[index[j]] = candidate[j]
```

The multiply-add is a VFP fused operation on the inspected path. Indices are clamped before reading and writing. The normal dub coefficient comes from channel configuration; recording-start/stop fades can modify that coefficient and the incoming contribution separately. It is not safe to collapse every transient to a single global `MaxDubLevel` multiplier. **S**

Crucially, `oldBuffer` is read from the tape storage. It is **not the current post-reverb output vector**. CPU scheduling runs the output effects before the record write, but execution order is not a feedback connection. Reverb can still be printed through an external or normalized cross-deck routing path; that is a different path requiring its own routing model.

Likewise, the active TapeAge/Wear/Hysterisis effects occur on input and playback. Their presence does not prove that the stored old sample is passed through all three again on every internal overdub. The write soft clip certainly acts on the summed candidate. Claims about repeated degradation must distinguish internal overdub from re-recording/bouncing the audible output.

## 8. Recovered tape and output algorithms

### 8.1 Parameter-to-object mapping

`preset_load`'s key-to-store sequence around `0x1763c–0x1776c`, cross-checked with `setTime` at `0x37a44`, resolves an important struct-order trap. Relative to the inspected preset sub-base at +16,384, the fields are:

```text
+624 WowFlutterDepth     +628 CrinkleDepth
+632 Wear                +636 TapeAge
+640 Hysterisis          +644 Knee
+648 Compensation       +652 LowCutFreq
+656 LowCutQ             +660 HighCutFreq
+664 Reverb
```

Text-file ordering is not memory layout. In particular, Wear and TapeAge should not be swapped by guessing adjacent offsets.

Let `t = TimeTapeAmount` in `[0,1]`. The active amounts are clamped products `t * presetValue` for tape age, wear, diffuser, flutter/crinkle, compensation, and reverb. Knee is clamped from its preset **without multiplying by t**. Input and playback have separate stateful filter/diffuser instances. **S**

### 8.2 Active TapeAge: a static five-one-pole bank

`TapeFilter::process` is at `0x4f524`; its constructor at `0x4f494` installs five coefficients. With independent one-pole states:

```text
h   = 0.6 * (HP100(x) + HP150(x))
wet = 0.6 * (LP500(h) + LP1000(h)) + LP3000(0.2*x)
y   = x + a*(wet-x)
a   = clamp(t * preset.TapeAge, 0, 1)
```

The stored coefficient values are:

| Stage | Coefficient c |
|---|---:|
| HP100 | 156.5137939453125 |
| HP150 | 104.34252166748047 |
| LP500 | 31.30275535583496 |
| LP1000 | 15.65137767791748 |
| LP3000 | 5.21712589263916 |

Their nominal frequency labels follow `c = Fs / (pi*frequency)` with the recurring DSP rate. The one-pole recurrence is:

```text
low[n] = (x[n] + x[n-1] - (1-c)*low[n-1]) / (1+c)
high[n] = x[n] - low[n]
```

LP and HP kernels at `0x4b0b0` and `0x4b104` were probed. The full filter-bank wiring is static reconstruction. Some intermediate 0.6 scaling is performed in double precision before returning to float; the reference preserves that broad ordering. **P/S**

The binary also contains a standalone class named `TapeAge`, whose setter suggests moving high/low cutoffs and resonance. **That is not the active `TapeFilter` used at the above call sites.** Porting that more intuitively named class instead would create a different sound. The same caution applies to a standalone `TapeSaturation` symbol: current active clipper calls must guide selection.

### 8.3 Input filtering is a different stage

`TapeInFilter::process` at `0x4f474` calls a biquad followed by a one-pole lowpass. The preset independently supplies `LowCutFreq`, `LowCutQ`, and `HighCutFreq`; the common defaults are 20 Hz, 0.2, and 15 kHz. The comments describe a resonant low-cut/head-bump role followed by high-cut filtering. **F/S**

Do not fold these controls into the TapeAge filter bank: they are distinct effects with different state. The exact biquad coefficient/Q transformation is not fully validated in this handoff. A familiar RBJ filter would be an approximation until its coefficient equations are matched.

### 8.4 Wear is signed-square blending

`TapeCompander::process` at `0x4fc60` implements:

```text
y = (1-w)*x + w*x*abs(x)
w = clamp(t * preset.Wear, 0, 1)
```

This was probed with 560 comparisons. There is no attack/release envelope in this kernel. For normalized magnitudes below one it attenuates quiet samples more strongly than loud ones. For magnitudes above one, the signed-square contribution can grow; downstream clipping matters. Calling it a conventional compressor would misdescribe the algorithm. **P**

### 8.5 “Hysterisis” is a signed feedforward diffuser

The active `TapeAllpass::process` at `0x4fce8` has delay lengths **68, 159, 251, and 375 samples**. Read each delay before writing; call the delayed values `d0..d3`:

```text
wet = 0.175 * (x + d0 + d1 + d2 + d3)
y   = x + h*(wet-x)

write delay0 = x
write delay1 = x - d0
write delay2 = x + d0 - d1
write delay3 = x + d0 + d1 - d2
h = clamp(t * preset.Hysterisis, 0, 1)
```

A 1,200-sample impulse probe at amount 0.7 matched the original instruction stream to within about `1.43e-8`. The nonzero impulse positions extend through sample 853. In this fixed-coefficient topology, the full-wet DC gain is 0.7, so it is **not a conventional unity-magnitude all-pass network**, notwithstanding the class name. It is also not a physical magnetic-hysteresis model. **P/S**

This small diffuser is a plausible contributor to the instrument's smearing texture. Preserve its signs, delay lengths, and read-before-write order before deciding to substitute a more elaborate physical model.

### 8.6 Soft clipping: two different transfer curves

`SoftClipper::setParams` is at `0x4ffe0`; vector processing is at `0x4fea8`. Both were included in the probe suite. Let clamped knee and compensation be `k` and `c`.

When float `k`, promoted to double, is at most the stored double threshold 0.01:

```text
u = (1+c)*x
A = 28.274333953857422
B = 9.42477798461914
R(u) = clamp(u*(A+u*u)/(A+B*u*u), -1, 1)
y = R(u)
```

The constants are approximately `9*pi` and `3*pi`. This is not exact `tanh`, and not the commonly substituted `27/9` rational approximation. Compensation is **pre-gain** in this mode.

For `k > 0.01`, first clamp input to `[-1,1]`, then use odd symmetry. For magnitude `m`:

```text
if m <= k:
    z = m
else:
    a = m-k
    z = k + a / (1 + (a/(1-k))^2)

y = sign(x) * z * [1 + c*(2/(1+k)-1)]
```

Compensation is **post-gain** in this mode. With `k=1`, the outside-knee branch is never reached for the clamped input, avoiding division by zero. The binary has distinct output and write clipper instances; matching the per-instance parameter setup remains part of whole-engine validation. **P/S**

The suite covers the threshold neighborhood, positive/negative signals, several compensation values, and finite amplitudes up to several times nominal. It does not establish NaN/Infinity handling, every FPSCR mode, or hardware analog saturation.

### 8.7 Wow, flutter, and crinkle

`Flutter` construction at `0x4f1c0` and processing at `0x502c4` reveal two sinusoidal components with nominal constructor frequencies **0.6 Hz and 5 Hz**, combined with different weighting. Phase increment is block-related, and random/filtered components contribute irregularity. A Mersenne-Twister implementation and seeding machinery are present. **S**

The sine table construction uses 256 phase samples, while runtime wrap/interpolation has a 255-based convention worth preserving during a later exact port. The final speed-factor transform uses approximately:

```text
factor = 0.958333015442
       + 0.0833339691162 * (combined + 2.5)/5
```

At zero combined disturbance this centers near unity. Do not interpret its constants as a proven global modulation bound: the preceding random/filter state and parameter interactions matter.

The periodic component, filtered irregularity, contact/touch disturbance, and block timing should remain distinct. A single arbitrary detune LFO or white noise added to audio would not reproduce the recovered structure. The full stochastic process and its exact distribution were not probed here.

### 8.8 Reverb: recovered control law, incomplete network port

The update's official 2.1 change notes introduce reverb; the active implementation is `MonoPlate::process` at `0x496c0`, a roughly 3 KB instruction body with inlined delay/filter operations. Its actual output call is confirmed at `0x484a4`. **D/S**

The Time/preset control law is recoverable. Let `u=clamp(t*preset.Reverb,0,1)`:

```text
norm  = sqrt(u*u + (1-u)*(1-u))
wet   = u / norm
dry   = (1-u) / norm
decay = min(2*u, 0.9)
```

Both wet/dry balance and a feedback/decay coefficient change; it is not merely a wet-gain knob. The complete plate's delays, damping, modulation, and output-tap algebra have **not** been converted into a verified native model. Do not assign a specific named published topology or an RT60 formula from `MonoPlate` alone. The reference header supplies the control mapping, not a substitute plate falsely labeled original. **S/R**

## 9. Crossfade table: a distinctive, reconstructable detail

The 256-entry table lives in BSS at `0x93f98`; it is **generated at startup**, not stored as a ready-to-copy 1,024-byte table in the ELF. The relevant initialization loop is `0x19c6c–0x19cac`.

With `t` starting at float `1/255`, π stored as float, and `t` incremented in float:

```text
q = 0.5 - 0.5*cosf(pi*t)
F = 0.5*(sqrt(q) + t)
```

Entry zero is explicitly zero. The ideal analytic shape for `0 <= t <= 1` is:

```text
F(t) = 0.5 * [t + sin(pi*t/2)]
```

The fade envelopes interpolate table entries and use the complementary/reversed coordinate for opposing fades. Since this is a hybrid amplitude curve, neither unity amplitude sum nor constant power should be presumed during overlap. A generic host crossfade helper can produce audibly different center gain and transient behavior. **S**

`tables/reconstructed_xfade_table.csv` and `.f32le` reproduce the initialization arithmetic using host `cosf` and fused operations. They are clearly named **reconstructed**: the original uClibc `cosf` implementation is not included, and no live device table was dumped. The analytic helper in the C++ header is useful for understanding the curve; use the table when investigating finer compatibility.

## 10. Factory presets as behavioral specifications

The ten presets are not just tone variations. Their structure demonstrates distinct instruments built from the same engine.

| Preset | Most important changes from Tape Looper |
|---|---|
| Tape Looper | Variable-speed recording; tape amount on third Time function; follow-speed taps. |
| Clean | Tape effects and reverb amounts zero; knee 0.3; compensation zero; third Time controls fade; 5 ms slew. |
| Classic | Fixed forward recording, preserving variable-speed playback/pitch shifting. |
| Multi-Tape | MultiTap; held one-shot speeds; record jack clocking; tap-tempo chord. |
| Clockable | External clock record jack; power-of-two clock divisions; three-second timeout; 25 ms retrigger delay; 5 ms slew. |
| Octave-Delay | Fixed recording; looping speed hold; MultiTap; stepped speed including 0.25×; zero slew; tap tempo. |
| SequencingMono | Fixed recording; V/oct; extra 0.25× marker; Time controls slew; 300 ms configured slew; delayed retrigger. |
| SequencingPoly | SequencingMono plus MultiTap and hold for both looping and one-shot taps. |
| Melontron | Variable recording; V/oct; MultiTap; both hold modes; maximum configured wow/flutter depth; delayed retrigger. |
| Broken-Tape | Maximum configured wow/flutter, crinkle, age and diffuser; Wear 0.5; compensation 1; 2,000 ms slew; touch Stall. |

All exact values are in `tables/factory_presets.json`; `factory_preset_matrix.csv` is convenient for implementation checks. The original comments contain typos and some stale explanations, so code evidence wins where there is a demonstrated conflict.

In particular, **Clean is not proven bit-transparent**: its input filters remain configured, and soft clipping still has knee 0.3. Do not make a “Clean preset must null against input” acceptance test. Test the specific disabled coloration parameters and preserved processing instead.

## 11. Sample import/export and persistent state

The supplied shell scripts define more than a generic WAV loader:

- Import converts to 32-bit floating-point, single-channel raw data at 49,148 Hz. Mono sources are copied to both destinations. Linked stereo loading separates channels; the unlinked stereo branch combines channels before duplicating the result. Exact mixing normalization should follow the invoked SoX behavior, not an invented sum convention.
- Export creates dual-mono WAVs for a single deck and a stereo WAV for linked decks. The temporary-file and output-name scheme is a hardware file-management implementation, not a recommended Rack architecture.
- Dedicated autosave audio/data and erase helpers show persistence is separate from ordinary playback state, with shared-memory coordination.

These scripts were read, not executed. Their paths and USB mounting assumptions are not appropriate to reuse directly in a plugin. **F**

For Rack, save the actual audio content and valid region, not only file paths. A preset describes behavior; it is not a full performance-state snapshot. Use versioned metadata and checksummed audio chunks, background serialization, and an atomic state swap on load. Stereo-linked assets must stay sample-aligned. Format conversion should be an explicit compatibility option because the discovered file rate differs from both other timing domains. **R**

## 12. Numerical validation: what passed and what did not run

### 12.1 Original-instruction arithmetic probes

`probes/arm_leaf_probe.py` reads the supplied ELF and LLVM disassembly. It models a limited set of ARM/VFP operations, conditions, integer/float registers, memory, and fused operations. Unsupported instructions fail rather than being silently skipped. It is independently written and **not a certified ARM emulator**. No operating system, GPIO, JACK server, launcher, or full Channel constructor is executed.

| Test group | Comparisons | Maximum absolute discrepancy |
|---|---:|---:|
| SoftClipper, including setter | 3,360 | 1.474e-7 |
| TapeCompander / Wear | 560 | 6.110e-7 |
| OnePole lowpass | 384 | 1.325e-8 |
| OnePole highpass | 384 | 1.987e-8 |
| InputBuffer cubic interpolation | 9 | 2.829e-8 |
| Integer ADC smoothing | 28 | 0 |
| TapeAllpass / signed diffuser impulse | 1,200 | 1.431e-8 |
| **Total across eight routines** | **5,925** | **All within test tolerances** |

The first suite exercises 206 distinct instruction addresses; the diffuser probe exercises 108 additional addresses in its own routine. Coverage of those addresses is not coverage of all possible inputs/branches. Explicit object fields are initialized for the tests; complete constructor/setup behavior is not thereby established.

The discrepancies reflect float arithmetic versus independently expressed reference equations within stated tolerances. The tests do not validate denormal flushing, NaNs/Infinities, exception flags, physical ADC noise, or original-libm bit identity.

### 12.2 Native C++ reference

`native/recovered_kernels.hpp` contains standalone C++17 expressions for clipping, wear, cubic interpolation, one-poles, the active TapeAge bank, the signed diffuser, ideal hybrid fade, reverb control mapping, documented V/oct mapping, and the scalar write blend. It contains no Rack dependency, heap allocation in processing, firmware execution, or disk access.

`native/test_kernels.cpp` compiled with `g++ -std=c++17 -O2 -Wall -Wextra -Werror` and passed **8,154 invariant checks**, including symmetry, interpolation of a linear sequence, diffuser finite-tail/DC behavior, fade endpoints, dry/wet normalization, and bypass identities. These are **separate native-model tests**, not 8,154 additional hardware comparisons.

No full-transport, complete reverb, alias-spectrum, double-deck, analog, or listening-test equivalence claim is made. The report supplies the raw evidence and tests needed to continue, not a fabricated confidence percentage.

## 13. Remaining questions ranked by implementation impact

| Priority | Question | Best next evidence/action |
|---|---|---|
| P0 | What is the actual converter clock and intended relation among 48,000 / 49,170.25390625 / 49,148? | Timed record/export and known-frequency measurement; complete system/driver image. |
| P0 | What are exact volts-to-float, CV sum/offset, analog gain/saturation, normalled bounce and auxiliary gain laws? | Schematics or calibrated hardware sweeps; firmware cannot reveal absent analog transfer functions. |
| P0 | What are complete variable-speed record/read boundary rules at zero, reverse, short loops and simultaneous transitions? | Extend instruction probes or native instrumentation around Tap and recordInput; differential buffers. |
| P0 | What is the exact plate topology and parameter initialization? | Trace constructor/state layout and process arithmetic; impulse responses after controlled initialization. |
| P1 | How does speed-dependent anti-alias filtering change during record/playback? | Trace filter setup and transitions; swept-sine/impulse spectral comparison. |
| P1 | What are notched/stepped speed table thresholds and signed endpoint rules? | Reconstruct or execute fillSpeedTable; export all 4,096 values per factory mode. |
| P1 | What is the complete link-mode event/control master matrix? | Trace checkLink plus ADC/button/jack forwarding; asymmetric stereo tests. |
| P1 | What happens under fifth-engine requests or all transition slots busy? | Rapid retrigger stress tests, age ordering, fades and null-return handling. |
| P1 | How are tap-count normalization and monitor gain combined? | Complete callback branch dataflow; correlated/uncorrelated multi-head level tests. |
| P1 | What are clock first-pulse, timeout, arming and jack/button priority semantics? | Event-sequence harness and digital capture; exact clock output pulse width. |
| P2 | What is exact touch response and stochastic modulation behavior? | Trace cap calibration and flutter filters/RNG; seeded statistical tests. |
| P2 | What exact state survives save/load/autosave? | Full metadata layout review and round-trip fixture tests. |

The lack of a full source tree is not a reason to stop at symbol names: substantial kernels are already recovered and checked. Conversely, symbols and reasonable approximations should not be described as complete algorithm recovery.

## 14. Recommended implementation order

Build the deterministic headless transport, with small test buffers, before designing a polished module panel. Separate physical voltages, normalized controls, tape coordinates, host time, and effect processing time. Freeze semantic events and persisted IDs, then implement first-record/playback/overdub, signed varispeed writing, interpolation, and crossfaded tap transitions. Add preset-driven fixed/variable record and Hold/Follow behavior before MultiTap stress handling.

Integrate the verified coloration kernels with separate input, playback, and write state. Keep the unresolved plate replaceable and explicitly labeled while investigating it; do not let a guessed reverb obscure transport comparisons. Add clock/link/IO and sample persistence only after deterministic headless tests make failures localizable. Finally add a Rack adapter, with no allocations, filesystem operations, or per-block worker creation in its processing path.

The companion **`RACK_IMPLEMENTATION_HANDOFF.md`** turns this order into concrete module boundaries, proposed interface contracts, test cases, and acceptance gates. The highest-value next fidelity work is the whole transport/write harness, the actual plate, and the three-rate calibration—not another generic tape-effect redesign.

## 15. Sources and evidence navigation

### Uploaded primary evidence

`extracted/bin/*`, `extracted/presets/new_factory/*`, and `extracted/scripts/*` are the supplied update's contents. See `evidence/manifest.json` for hashes. Every code-level claim above names a routine/address or a literal source file; the selected assembly and full binary listings are included.

### Official public cross-checks

```text
Product / analog-input description and feature inventory:
https://www.instruomodular.com/product/lubadh/

Version 2.1 change notes:
https://www.instruomodular.com/firmware/

Version 2.1 quickstart; page images inspected:
https://www.instruomodular.com/wp-content/uploads/2025/03/Lubadh-V2.1-Quickstart.pdf

Preset editor (a local copy is also in the supplied update):
https://www.instruomodular.com/wp-content/uploads/preset_editors/Lubadh_Preset_Editor.html

Located but not successfully retrieved/read in full:
https://www.instruomodular.com/wp-content/uploads/2023/11/Lubadh-Manual-Firmare-V2-A5-.pdf
```

The public sources establish product/version context, not a substitute DSP specification. The detailed equations, numeric tables, factory values, and numerical probes come from the uploaded material.

### Reuse boundary

Keep the supplied firmware and vendor assets as research evidence. The reference equations and proposed module design do not establish permission to redistribute vendor binaries, branding, artwork, or sample libraries. A new Leviathan module should have its own implementation and assets; release rights are a separate question from technical recoverability.
