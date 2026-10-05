# Lúbadh transport, recording and tail contracts

The continuation probes now execute selected **original ELF instruction bytes** in Unicorn, rather than relying only on the restricted disassembly interpreter. They establish useful tape-motion and recording contracts under explicit object fixtures. They do not run the appliance, its operating system, or the complete audio callback. Logical loop wrapping, event scheduling and integration of transition allocation remain open.

## Evidence and execution boundary

The source is `extracted/bin/lubadh_main`, SHA-256 `2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`. `probes/arm_byte_probe.py` loads its allocated sections into a Cortex-A15 emulator and enables VFP. Only five identified memory/lifetime imports are host hooks: `memmove`, `memcpy`, `memset`, scalar `operator new`, and scalar `operator delete`. DSP and transport routines execute their original bytes. Unmodelled imports, interrupts, missing returns, instruction-budget exhaustion and unmapped accesses fail the probe. There is no host firmware process or filesystem/device passthrough.

This harness permits initialized test objects without the full Channel constructor. Its broad fixture memory map is not a substitute for per-object memory safety validation. The allocator is a bounded bump allocator; deletion does not recycle memory. Neither helper reproduces appliance allocation failure behavior.

`probe_interpreter_crosscheck.py` independently checks the byte emulator against the existing restricted interpreter: **6,503 comparisons passed with zero discrepancy**, covering SoftClipper, Compander, both OnePole modes, AntiAlias, ADC smoothing, InputBuffer interpolation, and TapeAllpass. This cross-check strengthens the instruction interpretation; it does not establish ARM hardware identity, complete floating-point environment equivalence, or analog agreement.

## Head coordinates and motion

`Tap::move` at `0x4bde0` stores position as an integer and a separate float fraction. Tap offsets 4/8 hold current position/fraction; offsets 12/16 preserve the previous pair. The effective displacement is:

```text
chosen_speed = held flag at Tap+20 ? held speed at Tap+24 : supplied speed
delta = float32(float32(chosen_speed * factor) * frame_count)
fraction += delta
normalize fraction into [0, 1), adjusting integer position
```

The probe includes positions beyond `2^24`, where a single float coordinate would lose one-sample resolution. A reimplementation should preserve separate coordinates or equivalent precision. This routine itself does not wrap to the selected loop region. Wrapping belongs to the calling state machine and must be recovered separately.

## Playback interpolation depends on direction

`AudioData::interpolate` at `0x38748` selects held speed when enabled, otherwise LinkData offset 0. For physical capacity `C = 29,502,000`, integer position `p`, and fraction `t`, its four-sample neighborhood starts at:

```text
anchor = clamp(p + (chosen_speed >= 0 ? 2 : -3), 1, C-3) - 1
```

It interpolates samples at `anchor..anchor+3` using the same cubic equation as InputBuffer, including the float constant `0.16666670143604279`. Zero speed takes the nonnegative branch. The neighborhood changes with direction; reversing displacement alone is insufficient. Physical clamps are checked near both ends, including sparse fixtures at the high end. This does not show how logical loop endpoints reach this reader or how reverse splices are scheduled.

## Recording crosses tape cells

The complete `AudioEngine::recordInput` routine at `0x47818` executes, including original fade generation, InputBuffer interpolation, AudioData blending and SoftClipper processing. The test initializes work vectors and supplies the InputBuffer contents explicitly.

The routine combines the caller's factor with held/following speed. It writes `abs(current_integer - previous_integer)` tape cells. Addresses start at the previous integer and proceed in the sign of effective speed; the newly reached current integer is excluded. At ordinary positive/negative speeds the resampling phase advances by `float32(1 / abs(effective_speed))` for each written cell. The reciprocal is zero below the float32 `1e-6` threshold.

When the integer coordinate does not change, no cell is written. The routine nevertheless advances input phase once by that reciprocal. It retains only the fractional part of phase on return. This is a specific routine contract, not evidence that arbitrary caller speed/coordinate combinations are valid.

The blend is performed before buffer-write clipping:

```text
contribution = interpolated_input * incoming_fade_gain
blended = contribution + old_tape_sample * retention
stored = SoftClipper(blended)
```

With no write fade, retention is the supplied overdub feedback. Tests include held-speed overrides, nonunity factors, feedback 0/0.5/1, and three clipping configurations. Input and playback coloration are outside these fixtures.

## Fades have distinct roles

Each Tap owns four 24-byte fade states beginning at offsets 28, 52, 76 and 100. Active tap fade gains multiply. The probe checks one, two and four concurrent fades with valid in-table coordinates; expiry and slot-allocation decisions are not included.

Channel offset 6504 holds an additional write fade. Its stationary branch changes both incoming gain and old-buffer retention. With table interpolation `F`, current integer fade coordinate `i`, fraction `f`, and overdub feedback `b`, the checked branch uses:

```text
incoming_gain = F(i + f)
retention = b + (1-b) * F(254 - i + f)
```

The reversed lookup uses the observed `254` convention. Do not replace it with a presumed complementary curve. This formula is checked for stationary fades only; moving write fades and endpoint behavior remain open. Fade fixtures use the bundle's reconstructed host-cosine lookup table, not an original-device table dump.

## Block history and physical writes

`InputBuffer::input` at `0x38a78` copies the previous buffer's final four samples to its beginning, then copies the new block immediately after them. The block-sequence probe preserves recording phase across calls and exercises forward, reverse, stall and speed changes with fixed block sizes 7, 32 and 128. These are explicit call sequences, not an execution of the Channel callback.

`AudioData::add` at `0x387fc` clamps physical write indices to `[0, C-1]`. It gathers every old sample and forms every blend before clipping the contribution vector and scattering results. Multiple indices clamped to the same cell therefore read the same original cell; the last scatter wins. Low/high-capacity fixtures check this distinction from a sequential in-place overdub. This memory primitive does not establish the full-buffer recording transition or whether out-of-range indices occur during normal operation.

## First-record completion separates loop and storage lengths

`probe_tail_setup.py` executes the first-record completion slice `0x3a8a4..0x3a8e0`, stopping before `setLoopingParameters` at `0x3a8e4`. For the first-record counter `N` at Channel offset `0x2e8`, it checks:

| Field | Observed assignment |
|---|---|
| Logical loop length, Channel+0x4c | `N` |
| Separate end marker, Channel+0x50 | `N + 2458` |
| Maximum fade span, Channel+0x84 | `min(N-1, 12292)` |
| Stored length, through pointer at Channel+0x58 | `N + 12292` |

Eight explicit counters pass 32 assertions. The added 12,292 storage samples are approximately 249.989 ms at the recurring DSP constant 49,170.25390625, or 256.083 ms at nominal 48 kHz. This supports separating stored tail from logical period, as the version 2 manual describes. It does not resolve the actual converter rate.

The extra 2,458-sample marker is approximately 50 ms at the DSP constant. Its semantic purpose remains unproven. Completion eligibility, actual trailing-sample writes, capacity saturation, imported-file tail tags and the subsequent loop-parameter calculation are not exercised. A near-capacity counter fixture deliberately exposes the slice's arithmetic without asserting UI reachability or a device defect.

## Recorded checks and next work

`transport_probe_results.json` records **15,369 comparisons**, maximum absolute error `8.034546417068356e-8`, and 600 distinct instruction addresses. Its CSV retains every scalar comparison. The byte cross-check and tail slice have separate result files.

`record_sequence_probe_results.json` records **316,614 comparisons** across 18 block sequences and two physical-scatter cases, maximum absolute error `2.9802322387695312e-8`, and 430 distinct instruction addresses. Most comparisons check the entire small tape after each block, including unchanged cells; this is not 316,614 independent DSP stimuli. These counts must not be combined into a hardware-validation claim.

The follow-up [loop-region and allocation findings](LOOP_REGION_AND_ALLOCATION_FINDINGS.md) now execute `setLoopingParameters` and the TapManager allocation/update routines, including slot saturation and held/stalled reclamation. The callback's signed boundary transitions and recording-tail scheduling remain the next integration target. Use short logical regions inside larger physical storage to distinguish region wrapping, valid extent, tail access and capacity clamps. Audible transitions and the caller's response to allocation exhaustion are still needed to turn the recovered primitives into a dependable tape instrument.

## Reproduction

From the repository root, install the optional analysis dependencies into an isolated environment:

```sh
python3 -m venv /tmp/lubadh-research-env
/tmp/lubadh-research-env/bin/pip install -r firmware/Lubadh/requirements-analysis.txt
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_interpreter_crosscheck.py
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_transport.py
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_record_sequences.py
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_tail_setup.py
```

These optional dependencies are research tools and are not required by the Rack plugin or existing restricted leaf probes. No module source, plugin build settings, baseline hash list or baseline validation totals are changed.
