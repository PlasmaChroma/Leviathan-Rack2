# Lúbadh firmware coverage and next research steps

The existing Lúbadh 2.1.0 bundle supplies a useful DSP foundation and substantial static evidence. Its main limitation is integration coverage: the tested arithmetic kernels do not establish how the complete tape instrument behaves. The next major effort should verify transport, transitions, and loop-tail handling with small deterministic buffers. This audit also advances the anti-alias investigation with a new reproducible instruction probe.

## What the existing work establishes

The main executable retains C++ symbols, so many remaining questions can be investigated directly. The bundle includes 272 curated function listings, all ten factory presets, and 31 parsed preset fields. It distinguishes fixed from signed variable-speed recording, logical engines from transition slots, and input coloration from playback coloration and buffer-write clipping.

The existing restricted instruction tests were rerun successfully: 4,725 comparisons in the leaf suite and 1,200 in the diffuser probe. The native C++ reference again passed 8,154 invariant checks. These numbers retain their original scopes. They establish selected arithmetic behavior under explicit fixtures, not full firmware execution, measured hardware agreement, or listening equivalence.

The weakest areas remain transport integration and logical boundary transitions, the plate network, clock and link events, speed-table construction, stochastic modulation, and saved-state semantics. Static recovery is useful evidence for these areas, but a proposed Rack architecture is not evidence that they have been validated.

## Transport follow-up

The subsequent [transport findings](TRANSPORT_FINDINGS.md) narrow the read/write gap with original ELF-byte execution in Unicorn. A separate interpreter cross-check passed 6,503 comparisons. The complete recording primitive, split integer/fraction head motion, directional playback interpolation, overdub clipping and selected active fades passed 15,369 comparisons. First-record completion bookkeeping passed 32 assertions, establishing a 12,292-sample storage extension separate from logical loop length. Block-history and physical-scatter probes extend this to explicit call sequences.

These initial transport results do not validate the complete Channel callback, logical wrap transitions, actual recording-tail writes, moving write fades or transition-slot exhaustion. They replace the initial proposed transport harness below with an implemented research harness; the remaining integration fixtures are still required. Detailed source addresses, tolerances, dependencies and per-probe limitations are retained in the transport report and separate result JSONs.

The later [loop-region and allocation findings](LOOP_REGION_AND_ALLOCATION_FINDINGS.md) further narrow this scope: original region calculation passes 18,880 field comparisons, and allocation/reclamation passes 5,454 assertions. Per-call endpoint lag, direct versus manager-level saturation returns, and held/stalled fade completion are established. Complete callback transitions and audible output remain unvalidated.

The [ordinary-boundary findings](BOUNDARY_TRANSITION_FINDINGS.md) and subsequent [wrapped-splice/render findings](WRAPPED_SPLICE_AND_RENDER_FINDINGS.md) connect these primitives to selected callback slices. Ordinary and wrapped boundary fixtures pass, as do 128 raw head-sum render fixtures. Actual LinkData publication, complete active-list iteration, recording-head overlap suppression, final output coloration/gain and whole-instrument/hardware agreement remain open.

## Newly checked anti-alias behavior

`probes/probe_antialias.py` exercises `AntiAlias::setCutoff` at `0x4b1c0`, `AntiAlias::process` at `0x4b1f0`, and the original channel dispatch slice from `0x47cc4` up to `0x47d44`. It uses explicitly initialized objects and starts after `processSpeed`; it does not run the full constructor or callback.

The filter has two cascaded one-pole lowpass sections, each storing coefficient, previous input, and previous lowpass output. Its total filter state is 24 bytes. The setter updates both coefficients while retaining their histories:

```text
c = float32(1 / (pi_double * float32(cutoff_argument)))
low[n] = (x[n] + x[n-1] - (1-c)*low[n-1]) / (1+c)
```

There is no sample-rate numerator in the inspected setter. The process routine uses these values directly. This differs materially from interpreting the argument as a conventional lowpass frequency in Hz.

The inspected dispatch uses `abs(LinkData[0])` to choose the argument:

```text
speed >= 1              -> 20000
0.001 < speed < 1        -> float32(20000 * speed)
speed <= float32(0.001)  -> 20
```

Signs are discarded for this selection. `setRecordSpeedMode` at `0x39bfc` identifies LinkData offset `0xa0` as the record-speed mode. Mode zero sends the speed-dependent argument to both input and playback filters; fixed-record mode sends 20000 to the input filter while keeping playback speed-dependent. The probe deliberately makes the second speed field and modulation fields differ from the first, checking which field this slice actually uses. It does not establish how all upstream controls produce that field.

The two filters occupy Channel offsets 6544 and 6568. The constructor listing at `0x3cf0c` through `0x3cf58` statically supports four separate OnePole constructions. Processing call sites are `0x47ddc` and `0x483ac`.

### Probe coverage and interpretation

The new run passed **10,646 output comparisons**, **24 setter assertions**, and **26 dispatch cases**, exercising 69 instruction addresses. It covers impulse, DC, alternating polarity, seeded random samples, irregular block sizes, empty vectors, and live cutoff changes. Additional coefficients exercise the recurrence beyond its production argument range. Guard bytes check that a third section is not processed.

Maximum discrepancy against the explicitly rounded reference was zero, with an allowed absolute tolerance of `2e-7`. The interpreter and reference share host `fmaf`; zero discrepancy is not a claim of independent ARM hardware bit identity. A real-arithmetic reference initially diverged by over `5e-6` on alternating input because the small coefficients place poles close to -1. The final model preserves float32 rounding rather than hiding that difference by enlarging the tolerance.

An ideal transfer-function calculation at an explicitly assumed 48 kHz shows why the formula matters. With argument 20000, the calculated attenuation at 20 kHz is only about `3.06e-8` dB. Even argument 20 attenuates 20 kHz by only about 0.0306 dB, with much stronger attenuation close to Nyquist. These are ideal recurrence calculations, not measured device spectra; float32 transients and upstream processing remain separate questions.

**Implementation consequence:** preserve the literal stage as a research option. A conventional sample-rate-aware anti-alias filter would be a deliberate redesign. End-to-end alias rejection, held-speed taps, and upstream speed/modulation interactions remain open.

Results and raw comparisons are in `probes/antialias_probe_results.json` and `probes/antialias_probe_vectors.csv`.

## Documentation gaps now narrowed

The earlier bundle could not retrieve the complete version 2 manual. This audit downloaded its 36-page PDF, searched the extracted text, and visually inspected pages 4, 7, 12, 13, and 33. It does not claim a full visual review. Version 2 documentation remains a baseline to cross-check against 2.1 firmware, not proof that every behavior is unchanged.

The [official manual](https://www.instruomodular.com/wp-content/uploads/2023/11/Lubadh-Manual-Firmare-V2-A5-.pdf) adds these documented constraints:

- Page 33 describes 250 ms of additional recorded tail. Import tags select silent padding, consumption of the file's ending as tail, or a copy of its beginning. These affect loop timing and splices.
- Pages 7 and 13 distinguish pre/post output-level selection for the auxiliary output and describe auxiliary crossfader CV as bipolar +/-5 V summed with the fader. This is separate from the expander's 5 Vpp wording.
- Page 12 identifies left-to-right control mirroring in linked operation. The precise firmware forwarding matrix still needs reconstruction.
- Page 4 specifies 48 kHz. That strengthens the documented nominal-rate evidence without explaining the firmware DSP and file-conversion constants.
- Page 7 describes unity input gain at the knob midpoint and analog limiting before the codec. Firmware DSP alone cannot recover that analog response.

Downloaded PDF SHA-256: `976fa384ede3924ef6586cdc4d7b278fbd2c1fc2bc6ceb5743b6f69f10b5ee51`. The download was kept in `/tmp/lubadh-v2-manual.pdf`; it is not added to the firmware bundle. Original report statements about earlier retrieval failures remain historical.

## Additional static lead in tap mixing

The inspected path at `0x483b0` calls `TapManager::num_engines`. For more than one engine, `0x483c8` through `0x483dc` calculates a target proportional to `float32(0.82)^(N-1)`, using repeated float multiplication. The following code moves the current gain toward that target in eight block updates when the count changes. The zero/one-engine branch joins through `0x49054`; the unchanged-count ramp branch is at `0x48d28`.

This is a useful lead for `MIX-01`, with targets approximately 1, 0.82, 0.6724, and 0.551368 for one through four engines. It is **static reconstruction only**: neither the NEON engine-count routine nor the complete monitor/head mixing path was executed in this audit. Check how active-engine bookkeeping treats simultaneous fading transitions before using these values as a complete loudness rule. Arbitrary averaging by head count would overlook the observed branch.

## Recommended research order

| Order | Investigation | Concrete completion evidence |
|---|---|---|
| 1 | Signed transport and write path, including tails | Original-instruction/native comparisons of small tape buffers, read samples, positions, valid extents, and transitions. |
| 2 | Engine allocation, transition exhaustion, and mixing | Rapid retrigger sequences covering all four engines and five slots per engine, with allocation/fade histories and correlated-head levels. |
| 3 | Link and clock state machines | Asymmetric deck fixtures and event sequences exposing first edge, timeout, gate/latch, arm, delayed retrigger, and simultaneous-event priority. Begin with `interpretLink` at `0x27d08` and its downstream dispatch. |
| 4 | Exact speed tables and remaining DSP | All 4096 entries per factory speed configuration; input-filter coefficients; constructor/state map and impulse comparison for MonoPlate; seeded flutter statistics. |
| 5 | Physical interface and rate calibration | Known-frequency record/export, timed recordings, calibrated gain/CV sweeps, and measurements of normalled bounce routing. |

Orders 1 through 4 can progress using supplied binary evidence. Physical analog laws and the actual converter clock require hardware measurements or additional driver/system evidence. A hardware request should not block software transport research.

### First transport harness

Start with a ring/tape fixture containing distinguishable sample values, plus a ramp or impulse input. Record both the resulting tape and every read/write coordinate. Exercise speeds 0, +/-0.25, +/-0.5, +/-1, +/-2, and +/-4; zero-to-forward, forward-to-reverse, and reverse-to-stall transitions; fixed and variable record modes; first-record completion and overdubbing; short loops and wrap boundaries.

Represent physical capacity, valid audio extent, logical loop period, selected region, and tail extent separately until their exact relationships are traced. Add native recording and all three import tail policies to the fixture set. Test whether the documented tail duration remains fixed in host time or moves with tape speed; the manual alone does not settle that.

Compare uncolored buffer writes before combining the full DSP chain. Then add fades, engine overlap, held versus following pitch, monitoring, and cross-deck operation in stages. Every recovered rule should retain a source address and a reproducible failing case when it disagrees with the reference. This will turn the static findings into a practical instrument specification.

## Reproduction and scope

```sh
python3 firmware/Lubadh/probes/test_recovered_dsp.py
python3 firmware/Lubadh/probes/probe_diffuser.py
python3 firmware/Lubadh/probes/probe_antialias.py
g++ -std=c++17 -O2 -Wall -Wextra -Werror firmware/Lubadh/native/test_kernels.cpp -o /tmp/lubadh_kernel_tests
/tmp/lubadh_kernel_tests
```

No Rack module source or runtime behavior was changed by this audit. No original appliance binary or update script was launched. The archived bundle hash list and validation summary describe the earlier deliverable; the new continuation results are recorded separately rather than silently folded into its counts.
