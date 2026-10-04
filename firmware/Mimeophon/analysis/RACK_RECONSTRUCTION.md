# MP86 continuation: information for a Rack recreation

Date: 2026-10-04. Canonical image: `31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9`.

This pass maps the main delay read machinery, selected control behavior, and input/output details. It does **not** implement a complete Mimeophon engine. Addresses below refer to the supplied binary and `disassembly/dsp_core.asm` unless stated otherwise. Public control names on unresolved state flags remain hypotheses.

The subsequent [Color/Halo continuation](COLOR_HALO_ROUTING.md) completes the Color core, its envelope scale, all eight Halo branch destinations, and the final wet shaper, with bounded instruction-comparison tests. Read it for the updated status of the routing gaps below.

## 1. Four delay heads: fields and initialization

Base `0x20001a14`, stride 44 bytes. Head addresses are `0x20001a14`, `0x20001a40`, `0x20001a6c`, `0x20001a98`. Heads **0 and 2 share main A**, heads **1 and 3 share main B**. The two pairs crossfade independently. These are two stereo head sets, not adjacent pairs sharing buffers.

| Offset | Type / descriptive name | Evidence and limits |
|---|---|---|
| +0 | u32 `selected` | Pair selection: first head equal to 1 chooses its normal read; changes the sign of its gain ramp. Pair toggles at `0x0802599e..0x080259e6`, including `0x08025eda` and `0x08025eec`. Not a generic head-enable flag. |
| +4 | f32 `delay` | Tracked target delay in samples: `0x0802430c..0x080243b8`; read at `0x08025528/0x08025534`. |
| +8 | f32 `moving_delay` | Alternate delay used when +16 is nonzero. Increments of 2 in `0x08025c80..0x08025d40`; inactive heads also advance this field at `0x080242a2` / `0x08025980`. |
| +12 | f32 `absolute_read_position` | Cursor plus clamped delay/modulation, **before** integer masking: `0x0802440c..0x08024414`. |
| +16 | u32 `use_moving` | Zero selects +4; nonzero selects +8: `0x080243c6..0x080243d8`, `0x08025534`. Cleared in normal target tracking; set in moving-read branch. |
| +20 | u32 `read_index` | Truncated position AND ring mask, `0x08024418..0x0802442a`. |
| +24 | u32, unresolved | All four initialized to 1. No meaning assigned from this pass. |
| +28 | f32 `fraction` | Absolute float position minus its truncated integer converted back to float: `0x0802441c..0x08024430`. |
| +32 | f32 `gain` | Multiplies the interpolated read. Paired gains sum to 1 during audited crossfade. |
| +36 | 32-bit buffer address | A/B/A/B; dereferenced at `0x080243fc`, corresponding sites. Use host pointers separately in Rack. |
| +40 | unresolved word | No meaning assigned from this pass; startup BSS zero. |

Initialization (`initialization.asm`, `0x0802350e..0x08023574`) sets selected flags to `[1,1,0,0]`, gains to `[1,1,0,0]`, and +24 to `[1,1,1,1]`. Other fields in this BSS region start at zero; do not initialize every head with gain 1. `DelayHeadLayout` in the new header preserves the 44-byte firmware layout using a **u32 address**, not a 64-bit host pointer.

## 2. Main read interpolation: four taps, quadratic polynomial

For neighbors `a=ring[i-1]`, `b=ring[i]`, `c=ring[i+1]`, `d=ring[i+2]`, and fraction `t`, the six audited read sites implement:

```text
outer = a + d
p = ((outer - b) - c) * (t * 0.5)
p = p - (b + outer) * 0.5       // fused subtract in firmware
p = p + c * 1.5                 // fused add
y = b + t * p                   // fused add
```

In real arithmetic, the weights are:

```text
a: t*(t-1)/2
b: 1-t*(t+1)/2
c: t*(3-t)/2
d: t*(t-1)/2
```

This is quadratic in `t`, despite using four samples. At `t=0.5`, the weights are `[-1,5,5,-1]/8`. Constant signals and linear ramps are reproduced; negative outer weights permit overshoot. Do not substitute linear interpolation or a conventional cubic kernel when assessing fidelity.

`tools/audit_delay_reads.py` verifies the listing bytes against the canonical binary and symbolically checks all six kernels, including both extra-head paths in the crossfades. The memory-to-variable mappings are manually annotated; this is not CPU emulation or an independent decoding check. Results are in `delay_read_audit.json`.

### Coordinates and numerical behavior

The ordinary reader computes:

```text
position = float(cursor) + max(selected_delay + modulation, 2.0f)
integer = truncate_to_signed_integer(position)
i = integer & 0x1fffff
t = position - float(integer)
```

All four neighbor indices wrap independently. The mask applies to the integer index, not the floating position. Float32 addition before fraction extraction matters: beyond position 2^21, spacing is 0.25 sample. Replacing this with a double-precision phase accumulator changes fractional reads at long coordinates, even with an identical interpolation kernel. The reference helper reproduces this order for finite valid coordinates.

Immediately after interpolation, a raw absolute-bit comparison greater than `0x7f800000` replaces **NaN** with `+100`, before multiplying by gain (`0x0802447c..0x0802448e`). This is not a saturation clamp and not an infinity test. Later channel-level guards handle additional cases and retain previous values on some paths; the helper does not model those later guards.

## 3. Delay-time tracking and head crossfades

The ordinary target-tracking branch clears +16, forms `error=target-head.delay`, and applies:

```text
error >  64: increment =  7.15274715423583984375  // bits 0x40e4e34e
error < -64: increment = -7.15274715423583984375
abs(error) <= 0.001: increment = error
otherwise: increment = error*(error*error+27)/(9*error*error+27)
head.delay += increment
```

Evidence: `0x0802430c..0x080243b8`, `0x08025baa/0x08025bb2`, `0x08025e0e/0x08025e16`, `0x080268a8..0x080268e4`. This is nonlinear tracking, not ordinary exponential smoothing. At the original rate its large-error step is about 7.15 delay samples per audio frame. Other modes bypass it, including direct assignment at `0x0802592a..0x08025946`; do not apply it globally to all time changes.

On a pair transition the current sample is the **sum of the two reads weighted by the old gains**. Only then does the first gain advance up/down according to its selected flag, clamp to [0,1], and set the second gain to `1-first`. The increment is loaded from `0x200037f0`. Evidence: `0x0802572a..0x08025766`, `0x0802687a..0x080268a2`, `0x08025f08`; second pair at `0x080258ea..0x08025926`, `0x08026920`, `0x08025ef8`.

That increment initializes to .001 and receives the zone-slew table at `0x080262c6..0x08026304`. Nominal full ramps therefore take approximately 64, 128, 256, or 1000 frames (1.333, 2.667, 5.333, or 20.833 ms at 48 kHz), subject to float accumulation and event timing. The separate `[68,132,260,1004,...]` transition scale must **not** replace these increments as a hard-coded fade duration.

Status words `0x20002ae4` and `0x20002aa4` have visible request/transition behavior: 1 toggles the chosen head and becomes 2; 2 enters dual-head rendering; the rendering path resets status and reasserts 2 while a ramp is interior. The helper advances gains only. Full event arbitration and interrupted transitions remain to be reconstructed.

## 4. Hold-like and reverse-like behavior: preserve actual memory operations

`0x20004f94` is a Hold-like flag by behavior; its physical input mapping is not proven here. When the sampled flag is zero, `0x080241ac..0x080241c2` decrements the shared cursor at `0x20004c60` and masks it. When nonzero, `0x080253fa` updates separate offsets and bypasses that decrement. The flag is also reread during each sample at `0x08024b30..0x08024b58`.

Crucially, nonzero bypasses updates of the stored recording values at `0x20002ab0` and `0x20001ad8` (`0x08024b58..0x08024b8e`), but the final stores to both main rings still execute at `0x08025194` and `0x080251a2`. This implements a frozen cursor/retained recording-value behavior along this path; it is not evidence of a global write-disable instruction. Preserve it until event traces establish equivalence of a simpler model.

The reverse-like mode at `0x200037f4 == 1` selects moving delays advancing by 2 per frame. With a cursor decrementing by 1, increasing the relative read delay by 2 moves the absolute read position forward by 1: reversal relative to the normal read direction. The moving delay is clamped to at least 2; restart/request checks compare it with `2*head.delay+12` and `2*head.delay+16` (`0x08025c80..0x08025d40`, `0x08025e1e..0x08025e3a`). Additional event conditions affect the restart. Implementing this as only a negative playback increment would omit the windows and head transitions.

## 5. Stereo-linked input attenuation and Color envelope

Before the delay and dry/wet processing, each stereo frame updates state at `0x20003884`:

```text
envelope += 0.01 * (abs(L)+abs(R)-envelope)
if envelope < 0.0005:
    gain = envelope*envelope*4000000
    L *= gain
    R *= gain
```

Evidence: `0x08023f1e..0x08023f60`. This is smooth low-level attenuation with a shared stereo detector, not an independent hard gate on each channel. The downstream dry operands also use these modified inputs. Thresholds are in firmware float units; they do not establish Rack voltage scaling or analog noise levels. Presence in MP86 does **not** establish that this was introduced by MP86.

The later Color gain-polynomial envelope is distinct. For the first channel at `0x08024860..0x080248b4`, an increasing magnitude updates with attack coefficient .9; otherwise the stored envelope multiplies by `.9999` (`0x08025384`). The polynomial input is the scaled envelope clamped to **[0.5,5]**, not [0,1]. The other channel mirrors this at `0x08024b0c..0x08024b42` / `0x08025378`. The upstream scale and complete tap routing still need finishing; no complete Color reference is claimed.

## 6. Color table addressing and final Mix law

For the in-range corrected Color integer `q` at `0x08026044`, the coefficient lookup uses:

```text
k = clamp(trunc(float(3584-q) * float32(1/14)), 0, 254)
byte_offset = (255-k) & ~3
coefficient = float_at(0x0802b1c4 + byte_offset)
```

Thus this access reaches float indices 0..63. The same `k` is stored as the Color cascade target at `0x08023e68/0x08023e6c` and is also used in a conventional four-byte-indexed shape-table access at `0x08023e7c`. These are deliberately separate interpretations of an integer. Endpoint branches use immediate constants; do not extend this formula unconditionally to out-of-range corrected input. The higher in-range branch repeats the byte-offset lookup at `0x080268e8..0x0802691a`.

After wet shaping, the final mix operations `0x080251b8..0x0802523c` reduce, for each channel, to:

```text
out_before_final_clamp = dry*(1-m*m) + shaped_wet*(2*m-m*m)
```

Here `m` is the smoothed Mix state at `0x20003874`. This algebra is a curved crossfade, not a linear or trigonometric equal-power crossfade: both gains are .75 at its midpoint. `dry` refers to the already attenuated input above; `shaped_wet` refers to the wet operand after the immediately preceding shaping instructions, not the raw delay read. Do not omit those stages or infer analog unity gain from this formula.

## 7. Timing and host adaptation

The sample loop starts at `0x08023efe` and loops back from `0x08025242`. Out-of-line blocks after the return at `0x08025272` are still part of the function and can execute per sample. Address ordering alone is not a scheduling classification.

| Work | Audited cadence | Rack implication |
|---|---|---|
| Mix / Repeats .001 smoothing (`0x08023d48..0x08023d9a`) | Once per four-frame call | Calling this every host sample speeds its response fourfold at 48 kHz. |
| Halo matrix gain .001 smoothing (`0x08023e02..0x08023e3c`) | Once per call | Preserve block phase as well as coefficient. |
| Input attenuation envelope .01 | Every stereo frame | Nominal time constant about 2.073 ms. |
| Color tap-position .001 (`0x08024706..0x0802471e`) | Every stereo frame | Same numeric coefficient as block controls, different time constant. |
| Head tracking, read coordinates, fades, ring operations | Every stereo frame on their selected branches | Preserve branch and read-before-write order. |

**Recommended implementation strategy, not a recovered firmware fact:** start with an internal 48 kHz engine processing four-frame quanta and an explicit host-rate adapter. Allocate the two 2^21-float rings once (16 MiB total per instance), outside the audio callback. This preserves sample-count tables and control cadence while DSP equivalence is developed. Include buffering/resampling delay in the host latency assessment.

If later running natively at the Rack rate, rescale buffer capacities, delay lengths, windows, moving-head velocities, clocks, and smoothing together. A one-pole coefficient can be converted during setup with `a_new=1-(1-a_old)^(old_update_rate/new_update_rate)`; that does not reproduce event quantization or solve the nonlinear tracking law. Keep expensive setup math outside `process()`. The reference header's `std::fma`/division is for auditing; benchmark a production implementation before putting it in a hot path.

## 8. Initialization boundary and remaining work

An additional missing-device-data boundary is visible in initialization: it reads `0x08018000..0x0801800c`, below the update image. A marker comparison against 1242 selects a path; the fallback sets values including 1984, 945, 1039, and 3023 and derives control scaling (`initialization.asm`, `0x08023374..0x08023420`). These are calibration-like by use, but the exact physical mapping remains unproven. This is distinct from the high-flash saved-settings scan already recorded in the report. A harness must explicitly choose erased/missing-data fallback or a captured device calibration; never silently zero every flash read.

Next work should join the now-mapped reads into a deterministic full frame trace:

1. Continue from `EVENT_TRANSITIONS.md` and `HOLD_AND_INPUTS.md`, which now cover expiry arbitration, request consumption, reverse windows, Hold transport/retargeting and bounded raw-input acquisition. Finish accepted-clock processing, UI callback timing and downstream status writes; exercise complete interrupted transitions and Hold+Flip combinations over multiple frames.
2. Integrate the now-traced Color core, envelope multiplier, recording values and Halo injection from `COLOR_HALO_ROUTING.md` into one frame harness; retain the surrounding exceptional-value guards.
3. Integrate Halo's mapped writes and final wet shaping with the now-recovered modulation helpers in `MODULATION_SCHEDULER.md`. Preserve shared random draw order, delay-expiry triggers, exact unity-ratio synchronization and boundary resets; the external event arbiter is still outstanding.
4. Add CPU-backed or hardware-backed comparison traces for startup, steady processing, and mode changes. Current tests establish selected algebra and reference behavior only.

## Reproduction and validation

```sh
python tools/audit_delay_reads.py
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffp-contract=off tests/test_delay_read.cpp -o test_delay_read
./test_delay_read
```

Alternatively, `python tools/test_rack_components.py --compiler /path/to/g++` runs the symbolic audit, original sanity test, and new native test, and writes `analysis/continuation_validation.json` and `MANIFEST.continuation.sha256`. The compiler's runtime DLL directory must be on PATH on Windows.

Executed on 2026-10-04 with native Windows MINGW64 Python and `x86_64-w64-mingw32` G++. Six byte-checked symbolic kernels passed. Native tests cover 4100 basis cases, constant/ramp reproduction, wraparound, minimum read distance, float32 coordinate quantization, NaN fallback, nonlinear tracking, complementary fades, and stereo attenuation. Original component sanity tests were also rerun. No full ARM execution, physical audio, full engine equivalence, or Rack module integration was performed. These analysis/reference-only changes do not alter the plugin sources.

The locally present `mp86_firmware.zip` contains a WAV whose SHA-256 matches `input/mp86.wav`: `d116d3b0d5d54443437934216377d178321e401e706321e1ad72d5ccc05a9652`. This checks local archive consistency, not independently the archive's download provenance. The original `MANIFEST.sha256` remains a record of the original extraction bundle; new/updated continuation files are recorded separately.
