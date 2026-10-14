# MP86: Color and Halo routing continuation

2026-10-04. Continues `RACK_RECONSTRUCTION.md`; all addresses below refer to MP86 image SHA-256 `31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9`.

This pass completes the finite-input Color cascade/tap/envelope section, maps the final values of all eight Halo feedback writes, and recovers the final wet shaper. `reconstruction/color_feedback_components.hpp` contains bounded reference components. It is still not a complete instrument: hardware controls, event arbitration, delay-head scheduling, and several surrounding exceptional-value guards are outside these components.

## 1. Frame-level signal order and state boundaries

The following names describe dataflow, not recovered source identifiers:

- `dryL/R`: inputs after the stereo low-level attenuation described in the first continuation.
- `readL/R`: main delay reads after the channel guards at `0x080245a2..0x0802461e`.
- `G`: smoothed Repeats gain at `0x20001b58`.
- `B`: smoothed mode blend at `0x20004c64`, live register s2. This tends toward the Hold-like state, but is not simply its instantaneous Boolean value.
- `A`: smoothed Halo amount at `0x20003888`; distinct from Halo matrix gain at `0x20004d24`.
- `Hprevious`: the previous frame's unnormalized Hadamard outputs at `0x20004e60..0x20004e7c`.

At `0x08024618..0x08024642` and alternate blocks `0x08025390..0x080253b0`, `0x08025d6c..0x08025d80`, the inputs before Color conditioning are:

| State at `0x20002b50` | State at `0x20001b54` | Left | Right |
|---|---|---|---|
| zero | any | dryL + G*readL | dryR + G*readR |
| nonzero | 1 | dryL + G*readR | dryR + G*readL |
| nonzero | other | (dryL+dryR)/2 + G*readR | G*readL |

This establishes a cross-feedback/mono-injection distinction without yet assigning the physical switch/jack meanings of those flags.

When `A > .22`, each input is replaced with `input*(1.22-A) + Hprevious[channel]*(A-.22)` (`0x08024642..0x08024676`). Importantly, this uses Hprevious[0/1], not this frame's matrix. Each channel then blends toward its own main read using `input += B*(read-input)` (`0x0802467a..0x08024688`, `0x0802488c/0x080248b0`). A previous-good-input guard rejects values outside +/-10 before the DC blocker.

The finite DC-blocking recurrence preceding each Color cascade is:

```text
ap = previous_input + (-.9999)*(input-previous_ap)
previous_input = input
previous_ap = ap
conditioned = (input-ap)*.5
```

Evidence: `0x080246aa..0x0802470e`, `0x080248f6..0x0802495a`. The actual instructions also reset some NaN states; those guards need to be retained when assembling the complete path. The Color reference starts with `conditioned`, not raw dry input or raw main read. Its input boundary is explicit in the tests.

## 2. Complete audited Color core

Each 44-byte structure at `0x20004c74` / `0x20004ca0` contains:

| Offset | State |
|---|---|
| +0 | signed integer target index, normally 0..254 |
| +4 | smoothed index u |
| +8 | smoothed allpass coefficient a |
| +12,16,20,24 | previous input of stages 0..3 |
| +28,32,36,40 | previous allpass output of stages 0..3 |

The envelope is separate: left `0x2000385c`, right `0x200037fc`.

### Control, feedback, and stages

For each frame:

```text
u += .001*(target-u)
feedback = u <= 176 ? u*(-.0028409091755747795) : (u-426)*.002
x = conditioned + feedback*.3*(OLD stage3_input + OLD stage3_allpass)
a += .01*(coefficient_table[truncate(u)]-a)
```

Preserve the old stage-3 states in that feedback term. This feedback is an additional part of Color absent from the original four-independent-stages helper. Both control smoothers run per sample.

Four allpass-average stages follow: `ap=previous_input+a*(x-previous_ap)`, `y=(x+ap)*.5`. The five output candidates are:

| Candidate | Value |
|---|---|
| T0 | feedback-conditioned input * 1.144 |
| T1 | first averaged stage * 1.012 |
| T2 | second averaged stage * 1.144 |
| T3 | third averaged stage * 1.277 |
| T4 | (fourth-stage input + fourth allpass output) * .665 |

T4 is mathematically fourth averaged stage times 1.33, but that reordering changes float rounding. Exact constants and fused operations are preserved in the reference.

Let `p=u*float32(.02083330042660236358642578125)`. If `p<4`, truncate p to select adjacent candidates and use its fractional part for a linear interpolation. If `p>=4`, interpolate **T3 toward T4 with fraction .999989986419677734375**. This upper branch deliberately avoids reading a nonexistent T5. It does not return T4 exactly. At u=192, the approximate reciprocal still gives p just below 4; preserve the literal rather than replacing it with exact division by 48.

### Envelope compensation

On the morphed signal v:

```text
if abs(v) > envelope: envelope += .9*(abs(v)-envelope)
else: envelope *= .9999
m = clamp(envelope*scale, .5, 5)
color = v * (c0-c1*m+c2*m*m-c3*m*m*m)
```

The previously unresolved scale is established by `0x08024048..0x08024176`, with alternate entries `0x0802547e`, `0x080254e6`, `0x08026872`:

```text
factor = 1 + .7*(G-.7)
scale = (zone>1 && factor>=1) ? 1.5*factor : 1.5
```

Thus the envelope compensation depends on Repeats and zone, in addition to Color. The reference accepts scale explicitly and supplies a separate calculator so its scheduling remains visible. Polynomial coefficients still come from the exact zone tables. The left and right instruction sequences share the same scale and polynomial family.

Evidence for the full core: left `0x080246fe..0x080248dc`, right `0x0802494a..0x08024b54`, including their out-of-line branch targets. The new bounded instruction tests execute these sequences, including addressing, table reads, branches, and state writes.

## 3. Recording and Halo injection are different signals

Let colorL/R be the compensated Color outputs. If the live Hold-like flag at `0x20004f94` is zero, the retained recording samples update as:

```text
recordL = colorL + B*(old_recordL-colorL)
recordR = colorR + B*(old_recordR-colorR)
```

When the flag is nonzero, these retained values are not updated (`0x08024b58..0x08024b8a`). Both are nevertheless stored into the main rings later in the sample, as established in the first continuation.

Halo's injected pair instead uses:

```text
injectionL = .65*(readL + B*(colorL-readL))
injectionR = .65*(readR + B*(colorR-readR))
```

Evidence: `0x08024b90..0x08024bfa`. The ordinary settled B=0 path injects the main reads; at B=1 it injects Color outputs. Do not wire the recording samples directly into Halo as an assumed universal topology.

## 4. All eight Halo writes mapped

The auxiliary cursor decrements once and wraps with 32767. Eight reads at offsets `[1838,2241,2662,3128,3688,4251,4861,5539]` feed unnormalized H8. Denote its outputs H0..H7 in the existing Sylvester order. They are stored at `0x20004e60 + 4*i` before feedback processing. Let g be the smoothed matrix gain.

| Ring write offset | Final value | Evidence |
|---|---|---|
| 0 | injection/filter branch 0 on g*H0 + .25*injectionL | `0x08024d16..0x08024dbc` |
| 1839 | injection/filter branch 1 on g*H1 + .25*injectionR | `0x08024dc0..0x08024e28` |
| 2242 | 969-sample modulated allpass on g*H2, coefficient .7 | `0x08024f10..0x08024fce` |
| 2663 | 803-sample modulated allpass on g*H3, coefficient .7 | `0x08024fd2..0x080250a6` |
| 3129 | g*H4 directly | `0x080250aa..0x080250c8` |
| 3689 | 1236-sample integer allpass on g*H5, coefficient .4 | `0x080250ce..0x0802511e` |
| 4252 | g*H6 directly | `0x08025122..0x08025130` |
| 4862 | 1511-sample integer allpass on g*H7, coefficient .4 | `0x08025134..0x08025154` |

The writes at `0x08024f3a` (raw g*H2 to 2242) and `0x08024fe0` (raw g*H3 to 2663) are intermediate stores. They are overwritten by the corresponding allpass output in the same frame. No intervening read of those ring locations appears in this section. Taking the first store as the branch output loses both modulated allpasses.

### Injection branches 0 and 1

Each branch independently performs:

```text
x = clamp(g*H + .25*injection, -float32(2/3), float32(2/3))
x = x - float32(1/3)*x*x*x
x = x + damping*(old_damping_memory-x)
damping_memory = x
ap = previous_input - previous_ap*a + x*a
previous_input = x; previous_ap = ap
ring_write = (x-ap)*.5
```

Here a has bits `0xbf7e5477` (-.993476331233978271484375). The damping scalar is the live first word of the control scratch at `0x20004c4c` selected through the unusual **Color-controlled byte-offset lookup**. That table participates directly in Halo feedback damping; it is not another interchangeable Color cascade coefficient. Its current lookup and endpoint branches are described in the first continuation.

Branch 0 state: damping `0x20004d74`, previous input `0x20003850`, previous allpass `0x20002b2c`. Branch 1 state: `0x20002b34`, `0x20004c68`, `0x20002b4c`. In finite algebra the last stage is an allpass-subtraction highpass, distinct from the preceding smoothing filter.

### Short allpass addressing

The 969/803 buffers use `position=cursor+fraction*length`, linearly interpolate the two wrapped neighbor samples, write at the original integer cursor, then increment that cursor with length wrap. Their fractions come from `0x20001b2c` / `0x20002b58`, which are slowly modulated. These reads are **not** the main delay's quadratic interpolation. The 1236/1511 buffers read/write at their integer cursors and increment afterward. Every allpass uses `write=input+coefficient*delayed`, `out=delayed-coefficient*write`.

The modulation-boundary code (`0x08024e2c..0x08024f0c`) advances fractions between approximately .05 and .35, reverses increment direction on an overshoot, and resets relative to the crossed bound. The stochastic increment updates occur elsewhere. Full modulation scheduling is not included in the new `halo_write_values` helper: the caller supplies the four delayed samples. Tests independently execute the actual addressing and final writes for representative valid fractions and wrap positions.

## 5. Wet output and mix

The output taps use the **current raw H outputs**, not the just-filtered feedback writes:

```text
sumL = ((H0+H2)+H4)+H6
sumR = ((H1+H3)+H5)+H7
wetL = injectionL + A*(sumL-injectionL)
wetR = injectionR + A*(sumR-injectionR)
```

Evidence: `0x08025158..0x080251b4`. Preserve the original sum order when evaluating float agreement. Gain g shapes the feedback writes; it is not applied again to these raw output sums.

Then each wet signal is clamped to [-1,1] and transformed by:

```text
shaped_wet = 1.5*x - .5*x*x*x
out = clamp(dry*(1-mix*mix) + shaped_wet*(2*mix-mix*mix), -1, 1)
```

The 1.5 constant is established at `0x08024fec`, carried in s6 to `0x080251f2`; +/-1 and .5 are established separately. This final wet curve differs from the +/-2/3, x-x^3/3 injection-branch curve. Reusing one saturator for both changes the sound. The header implements both with their audited operation order.

## 6. Validation and remaining boundary

Run the existing runner:

```text
python tools/test_rack_components.py --compiler /path/to/g++
```

`tools/generate_color_trace.py` creates a temporary test-only C++ switch interpreter from byte-checked instruction records. It executes the two Color slices and the Halo tail (`0x08024f10..0x08025158` plus its branch target). Unsupported instructions, unknown memory reads, or escaping branches fail the test. Floating arithmetic uses native float and explicit std::fma; the build disables implicit contraction. The translator trusts the original disassembly and does not emulate ARM exceptions, FPSCR modes, peripherals, or NaN propagation rules.

Executed with native Windows MINGW64:

- 8,192 Color comparisons across both channel slices and both polynomial families, exercising every target index, feedback history, coefficient state, attack/decay, and tap selection. All 8,192 output values were bit-identical; all ten float state words and the envelope were checked numerically (2e-6 relative/absolute tolerance).
- 128 Halo tail comparisons check six ring outputs, four short-buffer writes, and all four cursor updates, including ring and short-buffer wrap. Branches 0/1 currently have analytic/native component tests, not instruction-slice differential tests.
- Existing six-kernel delay audit, 4,100 basis tests, and original component sanity tests pass.

This is stronger than testing only DSP identities, but it is **not firmware-execution equivalence**. The fixtures explicitly supply boundary registers and memory; they do not prove those states arise during full startup or mode changes. The subsequent [modulation pass](MODULATION_SCHEDULER.md) maps the shared random generator, two velocity/position walkers, initialization and delay-counter expiry coupling, with five additional bounded instruction slices. Next highest-value work is event/control mapping and a complete frame harness joining these sections in order. Remaining audio details include the exceptional-value guards and end-to-end state initialization; hardware gain and calibration still require separate evidence.
