# Data Bender inspired module implementation specification

Status: initial implementation contract, 2026-10-02. Audience: an implementing coding agent, including 6.1-Sol, with access to this repository. This document is sufficient to start work without the preceding conversation; the linked local evidence supplies detailed reference equations and fixtures.

Build a native stereo Rack buffer-manipulation effect that preserves Data Bender's musical identity: clocked capture, repeated fragments, signed playback speeds, probability-shaped Bend and Break, explorable frozen memory, and the five Corrupt textures. Preserve deliberate-sounding relationships and useful roughness. Exact CPU, peripheral, analog, and boot emulation are not product requirements.

This specification governs product choices. Original-instruction probes govern recovered algorithm behavior except for the explicit departures below. When a new discrepancy appears, record a focused decision and regression case here; do not silently substitute a familiar delay, granular, noise, or filter algorithm.

## 1 Scope and interpretation

V1 contains a stereo DSP core, host-rate adapter, six primary controls, original functional mode/button/gate behavior, secondary settings, a simple usable panel, persistence, and regression tests. There is one stereo voice. Audio and CV ports read channel zero; outputs are monophonic L/R.

Chimera-style waveform drawing, sample editing, enhanced interpolation, independent effect chains, additional modulation outputs, polyphony, file loading, and audio export are later phases. Do not build display infrastructure or a waveform cache in v1. The audio core must nevertheless have clean ownership so a later display can consume snapshots without redesigning the engine.

Use `Bender` as a provisional implementation prefix/namespace/model slug if a name is needed. It is not a final product naming decision. Final branding and visual artwork remain separate from the DSP contract. Do not reuse another module's IDs or modify its patch behavior.

### Fidelity decisions

| Behavior | V1 contract | Reason |
| --- | --- | --- |
| Bend rate table, probability distribution, sticky slew | Preserve | Primary musical voicing |
| Break repeat/silence/position coupling and thresholds | Preserve | Coordinated glitch behavior |
| Linear reading and discarded wrap overshoot | Preserve | Short-loop and aliasing character |
| Slice and capture windows, including early capture fade | Preserve for nondegenerate lengths | Audible rhythmic shaping |
| Shared memory, anchored frozen playback, continuing history | Preserve | Time edits explore material around the frozen region |
| 96-frame channel-major buffer processing | Preserve internally | Proven effect on stereo decisions and Time transitions |
| Corrupt curves, pulse holds, state and routing | Preserve | Character of the five textures |
| Sequential original width crossfeed | Preserve initially | Avoid changing an audible component before comparison |
| Driver rate versus initialization-rate mismatch | Explicit separation; normalize musical timing as below | Predictable seconds and external clock behavior |
| Invalid indexing, zero divisors, unsigned underflow | Replace with defined safe behavior | Never recreate unsafe native operations |
| Full Silence accidentally passing audio | Fix to mute when audible length is zero | Obvious endpoint defect |
| Board ADC calibration, physical inversion and debounce | Replace with ideal Rack input handling | Hardware implementation details |
| Settings width drift over save/load | Fix; persist canonical width | No reason for a patch to change on reopening |
| Natural boot randomness | Fixed owned seed with explicit restart semantics | Reproducible patches and tests |

There is one shipping v1 voicing, not a matrix of compatibility switches. A test-only reference clock driver is allowed. Enhanced alternatives belong to later work.

## 2 Required evidence and precedence

Read these before implementing the affected component. Paths are relative to this document.

| Evidence | Use |
| --- | --- |
| [Main report](../firmware/Data_Bender/Data_Bender_Reverse_Engineering.txt) | Overall reconstruction and signal path |
| [Control and timing report](../firmware/Data_Bender/analysis/controls/CONTROL_AND_TIMING_REPORT.txt) | Mappings, buttons, gates, mode separation, clock |
| [Buffer findings](../firmware/Data_Bender/analysis/buffer_engine/BUFFER_ENGINE_FINDINGS.txt) | Reader, memory layout, window equations, Macro decisions |
| [Corrupt DSP report](../firmware/Data_Bender/analysis/corrupt_dsp/recovered_corrupt_dsp.txt) | All effect equations and routing |
| [Independent DSP models](../firmware/Data_Bender/analysis/corrupt_dsp/reference_models.py) | Executable Corrupt reference |
| [Scheduling deep dive](../firmware/Data_Bender/Deeper_DSP_and_Rack_Design.txt) | Block order, clock request delivery, exact RNG, wrap behavior |
| [Time and Vinyl character](../firmware/Data_Bender/Time_Freeze_and_Vinyl_Character.txt) | Evolving frozen memory, slow retriggers, long-run Vinyl |
| [Windows build instructions](windows_build_from_wsl.md) | Authoritative validation toolchain |

Firmware reference: v1.4.7, SHA-256 `591eb538e2c6ce3024a1dfc1d85d8c7ef466ca5e13290191f2994aae2706f41d`. Decompiled files are pseudocode, not compilable recovered source; hard-float prototypes and conditional selects can be wrong. Prefer executed observations and decoded instructions. Do not copy the firmware binary into the shipping runtime or require Unicorn/Ghidra to run the module.

Older handoff statements that Vinyl rollover is untested are superseded by the character report. No physical board capture establishes the actual sample clock or analog gain. This specification does not promise hardware-null equivalence.

## 3 Rates and block adapter

Three constants have separate roles:

- `rendererRate = 48000` stereo frames/second, fixed across host sample rates.
- `coefficientRate = 96028`, retained for recovered Corrupt/output initialization and equations that use that constant.
- `blockFrames = 96`, exactly 192 interleaved floats.

The core receives complete blocks. Run the complete left Buffer pass before the complete right Buffer pass, followed by interleaved Corrupt and output processing. Retain the one-block Repeats control pipeline and per-block Dropout decision. Do not replace this with a one-frame port.

Convert host input/output using the repository's existing resampler dependency. At 48 kHz bypass sample-rate conversion but retain block buffering. Audio, control snapshots, and edge events must share one timebase; account for input resampler latency before applying an event to buffered input. Do not read current mutable Rack inputs while rendering an older block.

At 48 kHz, use a documented fixed 96-frame input-to-output adapter delay, including dry audio, with an initially silent output FIFO. At other host rates add the measured resampler delays; keep stereo and dry/wet aligned. Do not mix an undelayed dry host signal with delayed core wet audio. Tests must measure impulse latency and verify that latency stays bounded without FIFO drift.

### Musical timing departure

The production clock uses real time at `rendererRate`. Internal Time frequency is `exp(7.15461540222168 * x) / 16`, spanning 0.0625 to 80 events/second. External periods use actual edge timestamps. Do not divide internal cadence by two to emulate the initialization discrepancy.

For an effective requested period `T` seconds, send the recovered buffer frequency smoother this target:

```text
bufferFrequencyTarget = coefficientRate / (rendererRate * T)
Nrequested = min(floor(coefficientRate / smoothedBufferFrequency), capacity / 2)
```

Thus settled capture length follows `48000*T` subject to recovered rounding, smoothing and capacity. Keep buffer coefficient `.001` per core frame. The original callback period smoother and its 5 ms instability test operate on the effective period in real milliseconds. This normalization deliberately changes the firmware's literal timing mismatch while retaining buffer geometry, smoothing, read rates and coefficient voicing.

Maintain the recovered median-of-three external interval estimator, initialized with three 500 ms intervals. Ratio table is `[1/16,1/8,1/4,1/2,1,2,3,4,8]`, selected with `min(floor(8.25*x),8)`. Division counts incoming edges; multiplication schedules intervening boundaries from the filtered period. A new qualifying external edge reanchors the schedule; avoid emitting a duplicate boundary at that same timestamp. Clock loss after four last input periods sets a status indication and continues the last estimated schedule; it does not clear audio or Freeze. This continuation is a product choice for a deterministic loss policy.

Detect Rack clock/gate rises at host rate so narrow pulses are not missed, transport them to the core, then coalesce qualifying clock requests within each 96-frame block to the recovered pending flag with transition guard zero. This preserves block-level delivery rather than promising sample-accurate audible resets. Preserve integer edge counts for dividers before coalescing. Ignore external edges with intervals shorter than one core frame; retain the previous valid estimator on nonfinite/invalid timestamps. Clamp the effective buffer period to `[1/48000, Nmax/48000]`, while the musical clock retains its requested period. Compute elapsed boundary counts arithmetically and coalesce them, without iterating over an unbounded backlog. When a request exceeds capture capacity, continue the clock and show a capacity-limited status rather than silently relabeling the buffer as the requested duration.

## 4 Ports and controls

### Port contract

| Port | Behavior |
| --- | --- |
| Audio L, Audio R | Bipolar audio; unpatched R normals to L; both unpatched are zero |
| Time CV, Repeats CV, Mix CV | Add normalized `volts/5` before effective-control clamping |
| Bend CV | Macro amount CV with depth; Micro calibrated-independent ideal 1 V/oct |
| Break CV, Corrupt CV | Add `depth * volts/5` before clamping |
| Clock | External rising edge; source selection remains explicit |
| Freeze gate, Bend gate, Break gate | Latching edge actions or held-level contribution according to setting |
| Corrupt gate | Advance effect; optional reassignment to clock reset |
| Audio L out, Audio R out | One channel each, same delay and scale |

Use 1 V rising and 0.1 V falling Schmitt thresholds for Rack gates. Do not copy electrical inversion. On load/reset/adapter replacement seed the detector from current jack levels so a held gate does not invent a rising edge. Level-mode gates still contribute their current level.

Audio convention: internal `1.0` equals `5 V`, so input is `V/5`, output is `5*y`. Do not hard-clip valid audio at ±1 internally: Destroy deliberately shapes/clips, Vinyl can add peaks, and the original chain has its own gains. Replace nonfinite input with zero; contain nonfinite internal results and reset the affected small DSP state without clearing megabytes in `process()`.

### Primary parameter contract

Store six Rack primary controls normalized to `[0,1]`. Defaults below are product defaults, not claims about potentiometer boot positions.

| Parameter | Default | Meaning |
| --- | --- | --- |
| Time | .5 | Internal approximately 2.236068 Hz / 447.214 ms; external ratio ×1 |
| Repeats | 0 | One division |
| Mix | 1 | Wet |
| Bend | .5 | Unity speed in Micro; Macro amount while enabled |
| Break | 0 | No added Macro break/traversal/silence |
| Corrupt | 0 | Selected effect at minimum amount |

For the initial voicing retain the recovered effective overscan for Repeats, Mix, Break and Corrupt: `clamp(1.05*k + cvContribution,0,1)`. Time and Bend are unboosted. This preserves table-zone widths and onset locations. Tooltips show the effective value/operation; do not claim that every raw knob midpoint is a linear musical midpoint. Ideal Bend calibration is offset zero, scale one.

Provide actions for Clock source, Macro/Micro, Freeze, Bend, Break and Corrupt advance. Use direct Rack action handling with no artificial seven-millisecond debounce. Macro Bend/Break default off, Micro reverse/silence default off, Freeze off, internal clock, selected Corrupt Decimate. Store the four independent Macro/Micro flags; do not overload two booleans across both modes.

Macro Bend button toggles Bend enable. Micro Bend toggles reverse. Macro Break toggles Break enable. Micro Break switches Traverse (flag off) versus Silence (flag on). Corrupt advance cycles Decimate, Dropout, Destroy, DJ Filter, Vinyl; the original-three setting restricts cycling to the first three and returns an out-of-range selection to Decimate.

### Secondary settings

Expose these in the context menu initially; Shift gestures are optional UI shortcuts to the same canonical state and are not required to ship v1:

- Window control `[0,1]`, default `sqrt(.02)`; stored DSP coefficient is its square.
- Stereo separation `[0,1]`, default 1; crossfeed `a=.5*(1-separation)` with no repeated save/load boost.
- Bend, Break, Corrupt CV depths `[0,1]`, default 1. Micro Bend bypasses Bend depth for 1 V/oct.
- Unique/Shared Macro decisions, default Unique; independent of audio separation.
- Gate behavior Latching/Level, default Latching.
- Freeze button Latching/Momentary, default Latching; independent of gate behavior.
- Corrupt gate role Effect advance/Clock reset, default Effect advance.
- Corrupt effect set All five/Original three, default All five.
- Seed as a decimal/hex value and Restart random sequence action; default seed 1.
- Restore secondary defaults action, distinct from Rack module reset and audio clearing.

Do not add probability editors, a freeze-snapshot alternative, free-traversal switch, or a clean quality mode in v1. Those are enhanced features, despite being architecturally possible.

## 5 State and event rules

Use explicit typed state, not a byte-for-byte firmware object overlay. At minimum separate control state, clock state, buffer/global transition state, per-channel readers/writers, Corrupt state, output state and RNG state.

For each buffered block, resolve events in this product-defined order: queued reset/default commands; parameter/settings updates; button actions; transported gate edges/levels; clock scheduling; control mapping; pending buffer requests; left/right Buffer passes; Corrupt; output mix/filter/width. Within an event class preserve timestamp and arrival order. A reset command takes effect at the next core block and supersedes older queued commands. This rule replaces accidental UI-main-loop/interrupt ordering, not the recovered audio-loop order.

In Level gate mode, effective Bend/Break/Freeze is stored button state OR gate level. Button actions cannot cancel an asserted gate. In Latching mode each edge toggles the appropriate stored flag. Corrupt gate is always an edge action. Freeze momentary button handles both press/release; request combines it with the gate rule.

Freeze has requested state, active state and per-channel pending acceptance flags. In ordinary latch configuration a change in request does not itself force immediate sample acceptance: it is committed through qualifying pending clock/reset paths and reader crossings/wraps. In momentary-button configuration mirror the combined request directly into active Freeze before Buffer processing, as recovered. Preserve this distinction even when level gates are selected.

Keep the shared transition guard and channel-major acceptance behavior in v1. A crossing includes zero-to-zero, zero-to-positive and positive-to-zero; do not substitute only sign-bit inequality. Preserve countdown and reader-wrap conditions from the buffer evidence. Unfreeze follows the same request/acceptance structure. Two requests before acceptance replace the requested state; do not queue two future Freeze toggles independently of the resulting logical state.

Clock reset clears internal phase for the internal source. For external source it primes the divider so the next edge qualifies. It does not erase audio or reset the PRNG. Entering Macro sets the Micro base playback rate back to +1; retain independent button flags and the original reroll/settling behavior. Do not reset all effect/filter state on every mode change.

## 6 Buffer and reader contract

Allocate two completely zeroed float planes of `3,601,050` frames each: `28,808,400` bytes total, approximately 27.47 MiB. `Nmax=1,800,525`. At 48 kHz maximum capture length is approximately 37.511 seconds. This is a Rack contract, not a claim about physical module duration.

Each plane contains alternating capture banks and the auxiliary history writer in the same storage. Do not implement a separate immutable frozen loop or an unrelated second history allocation. History begins at the current clamped `2*N` boundary, retaining its cursor until that cursor must wrap/reposition. The frozen read-bank anchor can survive changes to N, exposing different material and potentially overlapping future history writes. This behavior must remain observable.

Preserve positive-rate write-before-read and nonpositive-rate read-before-write ordering. Unfrozen positive wraps select the current write bank; reverse wraps select the opposite bank. Preserve the N/4 bank-change guard and Time-instability suppression. Frozen playback continues advancing and evolving decisions while capture protection is active.

The reader returns interpolation at the old position, then updates its rate and phase:

```text
target = clamp(baseRate * macroRate[channel], -8, +8)
a = min(globalRateSlew, macroRateSlew[channel])
rate += a * (target-rate)
position += rate
```

Interpolate linearly between adjacent plane samples. Wrap below slice start to `sliceEnd-1`; wrap above `sliceEnd-1` to slice start. Discard overshoot. Do not change to modulo phase preservation, cubic interpolation, anti-alias filtering or oversampling in v1. A tiny slice may repeat one value at high rate; this is intentional reference behavior.

Both interpolation indexes must be bounded, including equality with capacity. Clamp every derived length/offset before indexing. For N too small to form a valid slice, use at least one addressable frame; no unsigned subtraction may produce a huge length. At maximum N, a history range with no free frames performs no history write. Test that full-capacity endpoint explicitly.

### Repeats and Micro

Base exponent is `floor(8*previousBlockEffectiveRepeats)`. Macro Break adds its exponent, clamped to `[0,8]`; `R=2^exponent`. Preserve the recovered reciprocal/+1 subdivision calculation, not simply integer `N/R`:

```text
S = min(N, uint32((coefficientRate * inverseSmoothedFrequency) / R + 1))
```

Micro knob speed is `2^(6*kBend-3)`, with the original two octave-snapping passes and strict comparisons. Apply `2^bendVolts` between passes, then reverse as selected. Preserve the octave targets `.125,.25,.5,1,2,4,8`, lowest-target ±15% snapping, other first-pass ±2.5% and second-pass ±1.5%. The reader clamps final target to ±8; very slow negative-CV magnitudes remain possible.

Traverse uses `q=(R-1)*t` and hysteretic rounding with h=.1 for R<=4, otherwise .3. A newly calculated candidate becomes active through the recovered wrap/crossing logic, not an immediate arbitrary read-head jump. Silence clears Traverse; Traverse clears manual Silence. Silence below .015 snaps to zero; otherwise use the effective amount, including 1.

`audibleFrames=floor((1-clamp(manualSilence+macroSilence,0,1))*S)`. The reader still traverses the full subdivision. If audibleFrames is zero, output zero for that channel; continue readers, clocks, history and random events. This is the explicit fix for the original underflow artifact.

### Windows

Use the exact piecewise linear slice window and capture/write-phase envelope in the buffer report. Window activates only when stored squared amount `w>.015`; minimum slice fade is 24 frames and nominal fade length is `floor(A*w*.5)`. Preserve the capture envelope's 240/480-frame constants and early zero region.

For degenerate `0<A<24`, use a safe triangle/trapezoid with `H=min(max(floor(A*w*.5),24),max(1,A/2))`; clamp gains to `[0,1]`. Treat this short-slice rule as a deliberate departure and test it separately from the original window vectors. A==0 always mutes. Preserve the use of the advanced reader position for envelope phase.

## 7 Macro decisions and random state

Use module-owned unsigned 64-bit PRNG state:

```cpp
state = state * UINT64_C(6364136223846793005) + UINT64_C(1);
return uint32_t(state >> 32) & UINT32_C(0x7fffffff);
```

`U=float(next()%255)/255`, so 1 is unreachable. Do not use host `rand()` or unrelated uniform floating distributions. Preserve shared-stream draw order across outer clock choices, left/right Macro helpers, subdivision jumps and Dropout. Vinyl's separate noise generators must also be per-instance; do not reproduce firmware globals shared between module instances.

Macro Bend at b<=.03 restores multiplier/slew to 1 at its qualifying reroll. Otherwise `j=uint32(U*b*1.5*9)` selects `[1,-1,2,.5,-2,-.5,.25,1.5,-.25,-1.5]`; j>=10 selects +1.5. At full amount +1.5 therefore has substantial extra weight. Above .667 retain/select slew using the original sticky branch and modulus formula in the evidence, including float-versus-double threshold arithmetic. Coefficients are 1, .005 and .0001; do not invent a separate zero-rate stop effect.

Macro Break preserves the recovered extra exponent, silence-table reachability, and random-position thresholds. Capture rerolls and per-reader-wrap jump rerolls are separate events. Shared mode copies left decisions without consuming the right's corresponding draws. Preserve existing decisions until their scheduled update, rather than treating knobs as per-sample probability controls.

Every qualifying outer clock request consumes the two retained-Corrupt draws even when currently inaudible. Preserve one-based retained choice `next()%3+1`. For `modulus=uint32(256*amount)==0`, consume the second draw but set retained amount to zero; do not execute native `%0` or reproduce a huge retained amount. Preserve the primary/retained Corrupt routing state from the dispatcher. The normal panel selection is 1..5; zero remains an internal routing state, not a sixth panel effect.

Seed changes and Restart random sequence apply at a core block boundary. They reset all stochastic generators to the documented seed-derived initialization, including Vinyl's initializer draws; they do not erase audio, change clock phase, or reset deterministic filter/playhead state. For Vinyl, initialize the shared dust generator with `s=uint32(seed)&0x7fffffff`, replacing zero with one; consume its five initializer draws. Reset both signed multiplicative noise seeds to 1, dust and modulation counters/held values to zero, and modulation period to 48014. Preserve the deterministic filter history and coefficients. This is the explicit restart policy; seed 1 reproduces the reference stochastic initialization. The Macro 64-bit stream uses the full supplied seed. Do not claim a hardware natural startup seed.

## 8 Corrupt and output

Signal order is input capture -> buffered read/silence/windows -> selected Corrupt -> equal-power dry/wet mix -> output Tone lowpass -> sequential width crossfeed. Dry comes from the same delayed input frame as wet processing. Board-specific right-channel polarity compensation is omitted.

Implement Decimate, Destroy, DJ Filter and Vinyl from the independent models and recovered equations, preserving persistent state, branch thresholds, table order and initialization. Do not replace them with generic Rack effects. Dropout is a persistent stereo gate: at amount<=.03 open it; otherwise toggle once per block when `next()%1024 < 376*u*u`. It is not an independently selected mute probability each sample.

For Vinyl preserve five/fifteen-frame pulse holds, common/private stereo layers, highpass placement, .3 crossfeed, distinct random streams and natural 48014-enabled-frame slow modulation period. Amount<=.05 bypasses and pauses sample-processing state. Do not add wow/flutter in v1. Golden vectors include long rollovers and forced rare events.

Mix retains endpoint mapping, per-frame `.001` smoothing and sine equal-power gains. A target below .03 becomes zero, or one when Freeze is requested. This affects the smoothed target, not an instantaneous wet jump. Retain the original output Tone lowpass: initialize at `coefficientRate=96028`, then set cutoff parameter `38000.0` (main literal `0x08003b4c`). Calculate its original coefficients using that ratio; do not clamp this parameter to the 48 kHz renderer Nyquist or reinterpret it as a physical 38 kHz cutoff. With `b=2-cos(2*pi*38000/96028)`, the feedback coefficient is `c=b-sqrt(b*b-1)` and feedforward is `1-c`. Use the original float32 calculation as the reference; calculate once at initialization. It processes the dry path too.

Width uses `Lnew=(1-a)*L+a*R`, then `Rnew=(1-a)*R+a*Lnew`. At a=.5, input (0,1) produces (.5,.75). Store canonical separation so reopening a patch does not change it. Symmetric width is a future explicit enhancement.

## 9 Architecture and realtime ownership

Suggested files are `src/BenderCore.hpp/.cpp`, `BenderBuffer.hpp`, `BenderCorrupt.hpp`, `BenderClock.hpp`, `BenderRateBridge.hpp`, `BenderState.hpp`, plus thin module/widget integration. Names can change consistently; ownership boundaries cannot disappear into the widget.

The audio thread exclusively mutates DSP state and sample planes. Parameter/settings commands enter through a bounded mechanism using repository precedents; background/UI code never edits a live buffer. Small status snapshots contain mode, requested/active Freeze, clock state, slice/count/rate and effect identity. Do not copy full sample memory into UI snapshots.

Prepare allocation, clearing, resampler construction and tables outside `process()`. On sample-rate change prepare a replacement bridge and swap at a safe boundary while retaining the fixed-rate core and captured audio. Prime new FIFOs, rebase timestamps and gate baselines, and fade output over 240 core frames to avoid a transport discontinuity. Do not allocate or delete a large object on the audio thread; retire it off-thread. Use a temporary silent output if preparation is incomplete.

Reuse patterns from `ChimeraRateBridge.hpp` for delayed controls/events and resampler latency, not its module-specific port IDs or state. `ChimeraPlaybackReader.hpp` is an enhanced-quality precedent, not the v1 reader. `ChimeraClock.hpp` is not a substitute for the required median filter.

Use cached mappings, integer exponent shifts, precomputed tables and fast math where perceptually stable. The reference oracle may use precise math offline. No per-sample allocation, file I/O, unbounded loops, mutex waits or logging. Amortize coefficient calculations without changing control cadence. Keep deterministic float32-sensitive state/threshold behavior intact. Benchmark concentrated 96-frame processing as well as average cost.

Follow repository debug gating and Process/Step/Draw/DrawLayer telemetry rules when instrumentation is added. Follow `NvgGraphicsLifecycle.hpp` and `GlLifecycleUtils.hpp` for any graphics resources. Split panel SVGs must be edited through the master and regenerated with the anchor atlas. No commits or staging.

## 10 Persistence and reset

Schema version 1 stores canonical secondary settings, independent mode flags, clock-source selection, effect-set/selection, seed and algorithm-version identifier. Primary knobs use Rack parameter serialization. Validate types and clamp loaded values; supply documented defaults for missing fields.

V1 does not persist captured audio, transient clock phase, pending requests or filter state. Patch load starts from cleared memory with Freeze inactive, even if saved while frozen, and restarts random streams from the saved seed. Thus repeated load plus the same subsequent input/event stream is reproducible, but patch load is not seamless transport continuation. Do not serialize transient RNG position while discarding the corresponding audio/filter state.

Rack reset restores product defaults, stops pending actions and replaces the core with cleared prepared state. Restore secondary defaults selects Macro, clears the four independent Bend/Break flags and button Freeze request, sets Micro base speed to +1 and traversal/manual Silence targets to zero, and restores Window, separation, depths, Unique mode, gate/button behavior, Corrupt gate role, all-five selection set and Decimate selection to their documented defaults. It retains primary knobs, selected clock source, seed, stochastic/filter history and audio. Gate levels still contribute; a Freeze release uses the normal transition rule rather than forcing every pending state to zero. Explicitly list these fields in its test. Seed restart is the separate action defined above. Do not make any of these actions clear a large live buffer synchronously.

Rack bypass routes L/R with the same L-to-R input normaling; it may use Rack's standard bypass latency behavior. Document that bypassing differs from Mix=0, which retains core delay, Tone and width.

## 11 Implementation phases

Complete phases in order. Each phase must update a short implementation status record with changed files, tests run and remaining decisions. Do not implement later enhanced features to compensate for failing base behavior.

| Phase | Deliverable | Exit condition |
| --- | --- | --- |
| 1 Reference fixtures | Fixture loader, owned RNG, control maps, extracted output Tone initialization, state types | Exact tables and seeded draws verified; no UI dependency |
| 2 Deterministic buffer | Allocation, writer/history, reader, Micro, windows, safe endpoints | Reader/bank/window fixtures pass; freeze expansion and history reuse demonstrated |
| 3 Events and Macro | Normalized production clock, test reference driver, Freeze transitions, Macro helpers and draw ordering | Timing and transition tests pass; deliberate scheduling differences documented |
| 4 Corrupt and output | Five effects, retained routing, mix, Tone and width | Short and long vectors pass; no cross-instance random coupling |
| 5 Rack integration | Rate bridge, controls/gates, simple panel/menu, persistence/reset | Host-rate/latency tests, native plugin build and usable installed module |
| 6 Enhanced presentation later | Chimera-style waveform widget, richer buffer/playhead view | Separate spec; no dependency of phases 1–5 |

Do not treat unexplained test differences as automatic permission to widen tolerances. First determine whether the difference is an explicit departure, a float rounding issue, an event-order error or a wrong reconstruction. Use a small new original-code probe when needed. Existing complete callback emulation is not available; preserve that limitation in reports.

## 12 Acceptance matrix

Headless tests must run without Rack UI or the firmware at runtime. Generate compact checked-in fixtures from the existing analysis JSON or offline oracle; record firmware hash, fixture setup, frame count and substitutions. Do not check only final audio RMS: test state transitions and event/draw order.

| Area | Required checks |
| --- | --- |
| Reader | Positive/negative/fractional rates, limits, old-position output, discarded overshoot, tiny-loop constant values, safe capacity equality |
| Memory | Zero initialization; alternating banks; history cursor wrap; frozen expand/shrink/restore at both anchors; max-N no-free-history safety |
| Micro | Both snap passes and edges; ideal 1 V/oct; exponent 0..8 and one-block lag; traversal hysteresis; Silence .9 and 1; zero-length mute |
| Windows | Existing normal-length vectors including w=.015 threshold; early capture fade; separate safe-degenerate expected values |
| Macro | Table reachability and weights from all 255 residues; slew hold branches; capture versus wrap rerolls; Shared draw count; silent/bypassed Corrupt clock draws |
| Freeze | Request without pending event; crossing including silence; countdown; momentary path; two requests before acceptance; simultaneous Time/clock; channel-major behavior |
| Clock | Internal 2 Hz at `x=ln(32)/7.15461540222168`; all external ratios; median outlier rejection; loss/reacquisition; reset before next edge; edge counting then block coalescing |
| Timing departure | At 48 kHz, 500 ms corresponds to approximately 24000 capture frames after settling, not 48014; clock request count matches elapsed real time |
| Corrupt | Original 64 short comparisons, Dropout toggle persistence, Vinyl 288096-frame rollovers/pause and five forced rare layers |
| Output | Mix endpoints and smoothing; Freeze-at-dry; dry path Tone response; asymmetric width vector; finite output/headroom |
| Host adapter | 44.1/48/96/192 kHz; long-run no drift; impulse latency; dry/wet alignment; short gate preservation; rate change with frozen audio retained |
| State | Load defaults/malformed JSON; no width drift; empty-memory load with Freeze off; deterministic restart; independent module instances |
| Realtime | No audio-thread allocations/large destruction or locks; bounded queues and resampler work; measured block spikes and sustained CPU/memory |

For direct component reproduction, require integer/state equality and packed float equality where existing models match bit for bit. DJ Filter retains its demonstrated float discrepancy: use a justified tolerance no looser than `2e-5` internal amplitude for the supplied vectors. A fast approximation elsewhere requires reported peak error, stable event decisions, and an A/B render; do not globally apply that tolerance to all components.

The safe endpoints, normalized timing, ideal gate/voltage handling, canonical width storage and defined load/reset policies are tested against this specification, not against failing original artifacts. The isolated buffer/corruption fixtures still use their original prepared rates/states and remain useful unchanged.

At phase 5 run `test-fast` and an authoritative Windows `plugin.dll` build through the documented MINGW64 bridge when practical; use the required Rack runtime directory. WSL-only linking does not establish Windows completion. Inspect panel/component placement and exercise modes, gates, Freeze/Time edits and save/reload in Rack when a running instance is available. Report manual checks that remain unperformed.

## 13 Decisions deferred without blocking the core

Final name, panel width/art direction and exact control positions can be settled at panel integration. The implementer may use a clearly labeled functional panel before final artwork. Do not introduce a waveform display merely to fill unused space.

Physical sample-clock/analog measurements would improve hardware comparison but do not override the explicit 48 kHz core and musical-time contract without a spec revision. Complete integrated rare gesture behavior can require further probes; label unresolved cases and preserve the demonstrated mechanisms rather than inventing an immutable loop.

Future waveform presentation should show capture region, read heads, slice selection, protected region and continuing history accurately. The later design must distinguish requested Freeze from active Freeze and avoid implying that every displayed frozen address is immutable. Those display semantics are informed by this core contract; rendering and editing remain outside v1.
