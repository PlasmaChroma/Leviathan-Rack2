# Codex handoff — continue MP86 analysis without re-guessing verified facts

## Objective

Latest pass: [analysis/HOLD_AND_INPUTS.md](analysis/HOLD_AND_INPUTS.md) closes local Hold transport and retargeting, validates the raw Hold poll and clock-qualification prefix, and statically traces Flip's secondary gesture/save path. Four more bounded slices pass 15,331 differential cases. See `reconstruction/hold_control_components.hpp` and `tests/test_hold_control.cpp`. Continue with the accepted-clock handler, TIM2 clock-tree cadence, secondary UI/persistence behavior and frame integration. Hold offsets continue drifting during fades; retargeting skips wrapping when status is 2; either fading pair suppresses both token updates. Do not re-derive these stages from older remaining-work lists below.

Continuation on 2026-10-04: read [analysis/RACK_RECONSTRUCTION.md](analysis/RACK_RECONSTRUCTION.md) before repeating the delay-head/read-kernel work below. It adds field-offset evidence, six symbolically audited read kernels, finite-input C++ reference helpers and native tests. Full event arbitration, complete routing, and CPU/hardware equivalence remain open. `analysis/delay_read_audit.json` is the new symbolic check record; the original validation record describes the earlier extraction pass.

The subsequent [Color/Halo pass](analysis/COLOR_HALO_ROUTING.md) closes the finite Color core, envelope multiplier, eight Halo write destinations and final wet-shaper equations. Its reference components pass 8,192 Color and 128 Halo-tail comparisons against byte-checked, host-executed instruction slices. These are bounded tests, not CPU or hardware equivalence.

The [modulation pass](analysis/MODULATION_SCHEDULER.md) recovers startup, shared LCG ordering, expiry-triggered velocity changes, asymmetric limit resets, per-frame position turns and exact unity-ratio counter synchronization. Five additional instruction slices check `reconstruction/modulation_components.hpp`; exact counts are in `analysis/continuation_validation.json`. Continue with clock/Hold/Flip event arbitration, exceptional-value guards and frame integration. Do not substitute independent random generators, a sine LFO, overshoot reflection or epsilon-based unity detection.

The [event pass](analysis/EVENT_TRANSITIONS.md) now covers the entire sample expiry prefix through both random updates, request consumption for both head pairs, and the selected-head reverse stage. Its 31,312 native differential cases compare all fixture memory writes and the transient restart flags. Expiry suppresses requests for non-idle pairs but still advances RNG; reverse windows can overwrite a fading status; request consumption retains gains. Remaining event work is physical-input/clock acquisition, Hold's retargeting window and integration with downstream status writes. Preserve the cached Hold value separately from live Hold reads. See `reconstruction/event_components.hpp` and `tests/test_events.cpp`.

Continue from a completely decoded application image toward a testable understanding of the DSP. Do not spend the first phase reimplementing a generic FSK modem or estimating tables from the front-panel manual: the bytes and several key structures are already recovered.

Read `REPORT.md`, `analysis/artifact_status.json`, `analysis/validation_results.json`, and `tables/table_manifest.json` before writing DSP replacement code.

## Fixed identity and import parameters

```text
BIN SHA-256  31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9
Size         49152 / 0xC000
Flash base   0x08020000
Reset word   0x08020299
Reset code   0x08020298
Startup      0x080201c8
Main         0x08027648
DSP entry    0x080239b4
Callbacks    0x0802697c, 0x08026ab4
Architecture little-endian ARM Thumb, Cortex-M7/F7-class, hardware float
```

The image is the direct concatenation of 192 CRC-valid 256-byte payloads. There is no successful descrambling/decompression stage. Do not prepend guessed bootloader data or trim trailing zeros. The ELF provided is generated for analysis; the BIN is the canonical evidence.

## State and storage already traced

Startup copies 372 bytes from flash `0x0802be08` to RAM `0x20000000`, then zeros RAM up to `0x20005a80` exclusive. Stack top is `0x2004c000`. Other state outside that area is not a captured RAM image.

Main delay storage is two arrays of 2^21 floats at **0xd0800000 and 0xd1000000**. Do not move the second address down to 0xd0000000: that address belongs to the distinct 32,768-float auxiliary ring. Four 44-byte head structures at `0x20001a14` share the two main buffers. Length and mask live at `0x200037f8` and `0x20001b20`.

The two callbacks each process **four stereo frames**, not four scalar samples. RX and TX each contain 16 signed 32-bit words. The first callback reads RX words 0…7 and writes TX words 8…15; the other reverses those halves. Conversion multipliers are exported by address in the report and assembly. Do not silently use standard Q31 constants instead.

Nominal SAI audio rate is 48 kHz. The update WAV independently happens to be 48 kHz. Hardware clock accuracy and complete I/O latency have not been measured.

## Verified DSP building blocks

### Hadamard feedback network

The auxiliary ring's eight taps feed an **unnormalized** H8 transform. `tables/halo_hadamard8.csv` gives its signs; the C++ butterfly has a sanity test. The live coefficient comes from `halo_matrix_gain`, range about .176776707… .348771602.

Using the original coefficient with a normalized H8 is wrong. To use a normalized orthogonal matrix, multiply the stored gain by sqrt(8); the effective scalar then spans approximately .5… .986475.

Read and write offsets are both exported in `halo_aux_ring_taps.csv`. Read offsets alone are not the individual segment lengths. Several branches include nonlinear/damping stages and additional allpass state.

### Short allpasses

Four RAM arrays have lengths 969, 803, 1236 and 1511 floats. The first pair uses linearly interpolated reads and coefficient .7 in the audited path; the second pair uses coefficient .4. The recurrence is `w = x + g*d; y = d - g*w`. Slow tiny pseudorandom modulation is present. It must not automatically be labeled dither or removed as presumed noise.

### Color

The two audited Color paths have four cascaded allpass-average stages, intermediate taps, coefficient smoothing, and interpolated tap selection. The supplied C++ recurrence models only that subset. Do not call it the complete Color effect or substitute an arbitrary ladder/biquad while claiming fidelity.

A further stage applies the zone-dependent polynomial `c0 - c1*m + c2*m*m - c3*m*m*m` as a signal gain. Trace the envelope/control variable `m`, its clipping, tap scaling, and coupling before combining the pieces into a full path.

### Exact constants

The six large float arrays are 2048, 128, 128, 256, 256 and 256 entries. Nine additional small flash tables are exported. `exact_tables.hpp` retains float32 values exactly through hexadecimal literals.

The exponential table resembles 2^(i/2048), but regeneration differs at 38 entries in the measured check. Keep the exact table for bit-conscious work. The Repeats table reaches 1.27, but that is not a universal whole-loop gain.

The 13 tempo values are delay ratios; manual rate/frequency ratios are reciprocals. The tempo table is reconstructed from immediate stores, not presented as an original contiguous ROM block.

## Highest-priority remaining trace

1. Establish a sound function boundary and complete control flow for `0x080239b4`. Recover indirect jump-table edges and verify pointer-derived roots. The current recursive traversal is useful evidence, not a complete decompiler.
2. Give the four delay-head structs typed fields, with byte-offset evidence. Trace their changes in ordinary forward delay, reverse/Flip, Hold, and clocked mode. Identify read interpolation, head transitions, recording/write inhibition, and sample-counter wrap behavior.
3. Separate control update, per-sample DSP and event handling without altering execution order. Identify which smoothers update per block versus per sample.
4. Complete Color's signal equation and Halo's eight feedback writes. Track all injection/output taps, filtering, saturation, and cross-channel coupling. Preserve H8 scaling and the partitioned-ring organization.
5. Build an offline reference harness only after the state layout is sufficiently known. Initialize all state from recovered startup, not arbitrary defaults. First run at the original four-frame quantum and nominal rate; only then consider adapting to Rack's callback and sample-rate model.

A pragmatic starting test matrix is impulse, silence, steady sine, low-level noise and a step, with Repeats/Halo at minimum, middle and maximum. Test across all eight zones, then add transitions. Record exact state and output traces rather than relying only on listening.

No instruction-accurate CPU emulation or hardware golden recordings were produced in this pass. Do not invent golden output files. If an emulator is introduced, account for ARM VFP fused operations, rounding/NaN behavior, interrupt timing, memory initialization and peripheral stubs.

## Hardware-dependent boundary

The recovered codec writes use I²C address argument 0x34 and a WM8731/TLV320AIC23-compatible register map. The corresponding interface value indicates 32-bit I²S slave mode. The chip part number and analog converter precision are not proven. Distinguish serial word width from effective resolution.

Some input gain is controlled through stereo codec-volume writes; the saved setting region lies outside the update image. Reconstructing only float code may miss analog gain behavior.

## MP86-specific change analysis

Do not attribute an isolated visible operation to the MP86 noise-floor update without comparing another revision. The primary product page provides the advertised purpose, not the binary diff.

An earlier firmware WAV would permit the same CRC-verified extraction. Compare literal floats, audio conversion constants, codec writes, high-pass/damping paths, modulation state, and function-level control flow. Raw byte differences alone need alignment because compilation can relocate literals and branch offsets.

## Acceptance criteria for subsequent claims

An “extracted” value must cite exact image offsets or a reproducible immediate-store trace. A “reconstructed” block must specify omitted routing/state and show its supporting instruction range. A “matching emulator” needs a stated test oracle and measured agreement. A “hardware-authentic” claim needs hardware evidence.

Preserve uncertainty instead of normalizing suspicious code into what a conventional synthesizer would do. The recovered byte-offset table access and Hadamard gain convention are examples where an apparently helpful cleanup could change the sound.
