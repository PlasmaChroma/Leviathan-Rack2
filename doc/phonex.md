# Phonex Implementation Plan

Status: Draft implementation plan  
Module: `LEVIATHAN // PHONEX`  
Target: VCV Rack 2, Windows-first Leviathan build  
Form factor: 14 HP

## 1. Product definition

Phonex is a monophonic, voltage-controlled, order-10 LPC speech synthesizer and
circuit-bending instrument. It combines a low-rate speech-synthesis backend
with authored phrase data, typed-text pronunciation, external excitation,
formant manipulation, time/pitch separation, and deterministic corruption.

Phonex is inspired by Texas Instruments speech-synthesis hardware, but it is
not intended to be a bit-exact TMS5100/TMS5220 emulator or an implementation of
the federal LPC-10 communications codec. The name `LPC-10` refers to the ten
reflection coefficients used by the synthesis filter.

The implementation must be clean-room and must not depend on a dumped Speak &
Spell or other commercial speech ROM. A vintage ROM may be useful for private
comparison, but it is not an input, build dependency, or redistributable asset.

## 2. Locked product decisions

The following decisions supersede ambiguities in the initial module
specification.

### 2.1 Text entry

Phonex will accept text through a normal Rack `ui::TextField`; it will not
reproduce a physical alphabet keyboard on the panel.

- The panel contains a compact, horizontally scrolling `UTTERANCE` field.
- Enter submits the current text and retriggers the resulting utterance.
- Pressing the `BANK / WORD` encoder also retriggers the active source.
- Turning the encoder selects a bundled phrase and makes ROM/preset playback
  active again.
- Double-clicking the compact field may open a larger multiline editor if the
  14 HP layout proves too restrictive.
- The last submitted text and text-source selection are persisted in patch
  JSON.

Text entry is a UI/control-plane feature. Strings, dictionaries, parsing,
allocation, and pronunciation work must never occur in `Module::process()`.

### 2.2 Speech sources

The DSP engine consumes a generic immutable LPC sequence rather than reading a
specific ROM directly. Three producers may feed that interface:

1. Bundled authored phrase data.
2. Typed text compiled into phonemes and then LPC frames.
3. Direct phoneme-script input for precise or experimental pronunciation.

This separation lets all sources share transport, interpolation, excitation,
warping, bending, and output processing.

### 2.3 ROM and corpus

No original hardware ROM is required. In this document, `ROM` means immutable
speech data compiled into the plugin from Leviathan-owned or clearly
redistributable source material.

Development begins with procedural test frames and a small synthetic phoneme
bank. The final corpus can be expanded by:

- recording original source speech and analyzing it into LPC frames;
- authoring phonemes and transitions directly in coefficient space;
- generating synthetic vowels, fricatives, stops, and transitions; and
- importing only material with documented redistribution rights.

Every bundled phrase or phoneme source must carry provenance metadata in the
editable corpus source.

### 2.4 Excitation blend

`EXCITE BLEND` retains the specified 0 to 1 range and default of 0:

- `0`: use the voiced/unvoiced decision encoded in the current frames;
- `1`: force the excitation selected by the `Forced excitation` context-menu
  setting; and
- intermediate values crossfade the automatic and forced excitation signals.

The forced target defaults to `Voiced`; `Unvoiced` is also available. When
`EXT EXCITE` is connected, it bypasses both internal sources and enters the
lattice after normalization.

### 2.5 Trigger input

`TRIG / GATE` has two persisted modes:

- `Retrigger phrase` (default): a rising edge resets and starts the active
  utterance.
- `Advance one frame`: each rising edge advances by one frame; the sign of
  `SPEED / SCRUB` selects forward or reverse direction and its magnitude is
  ignored.

### 2.6 Internal sample rate

The context menu exposes:

- `8 kHz`;
- `10 kHz` (default); and
- `Host rate`.

At 8 kHz and 10 kHz, an internal phase accumulator schedules synthesis ticks
independently of the host sample rate. The default frame contains 200 internal
samples, giving 25 ms frames at 8 kHz and 20 ms frames at 10 kHz.

Low-rate reconstruction has two persisted modes:

- `Raw hold`: zero-order hold for strong period images and aliasing.
- `Filtered`: inexpensive interpolation and fixed low-pass reconstruction.

### 2.7 Interpolation

`TI-style linear` is the default interpolation mode. Energy, pitch, and
reflection coefficients interpolate linearly, but interpolation is suppressed
across voiced/unvoiced transitions. Voicing itself changes only at a frame
boundary.

An optional `Monotone cubic` mode may be provided for smoother extreme scrub
movements. Cubic output must be clamped to prevent coefficient overshoot.

## 3. Signal architecture

```text
 Bundled phrases -----------+
                            |
 Typed text -> G2P ---------+--> Immutable LPC sequence
                            |          |
 Phoneme script ------------+          v
                                  Frame transport
                                  and interpolation
                                         |
                      +------------------+------------------+
                      |                                     |
             Chirp / LFSR excitation              External excitation
                      |                                     |
                      +------------------+------------------+
                                         v
                                  10-stage lattice
                                         |
                                  Bend / starvation
                                         |
                              Reconstruction / degradation
                                         |
                                  scale and soft clip
                                         |
                                     AUDIO OUT
```

The speech frontend produces data only. The real-time backend has no knowledge
of spelling, words, dictionaries, or text widgets.

## 4. Proposed source layout

| File | Responsibility |
| --- | --- |
| `src/PhonexTypes.hpp` | Frames, sequences, phrase metadata, and shared enums |
| `src/PhonexBitstream.hpp/.cpp` | Packed frame reader and safe decoder |
| `src/PhonexRom.hpp/.cpp` | Bundled phrase/phoneme lookup |
| `src/PhonexRomData.inc` | Generated immutable corpus data |
| `src/PhonexPronunciation.hpp/.cpp` | Text normalization, G2P, and phoneme script parser |
| `src/PhonexSequenceCompiler.hpp/.cpp` | Phoneme timing, transitions, and LPC sequence assembly |
| `src/PhonexSequenceMailbox.hpp` | Lock-free publication of prepared sequences |
| `src/PhonexEngine.hpp/.cpp` | Transport, excitation, lattice, rate conversion, and bending |
| `src/Phonex.hpp/.cpp` | Rack module, controls, ports, triggers, and serialization |
| `src/PhonexWidget.cpp` | Split-panel UI, text field, display, encoder, and menus |
| `tools/generate_phonex_rom.py` | Corpus validation and C++ data generation |
| `tools/phonex_rom/` | Editable corpus, phrase definitions, and provenance |
| `res/Phonex.svg` | Editable 14 HP master panel and hidden anchors |
| `res/Phonex.panel.svg` | Generated runtime panel |
| `res/Phonex.labels.svg` | Generated label layer |
| `tests/phonex_engine_spec.cpp` | Standalone DSP, decoder, transport, and bend tests |
| `tests/phonex_pronunciation_spec.cpp` | Text normalization and pronunciation tests |
| `tests/phonex_module_spec.cpp` | Rack configuration and persistence tests |
| `tests/phonex_panel_contract_spec.py` | Panel dimensions, layers, and anchor contract |

## 5. Core data contracts

### 5.1 Decoded frame

```cpp
struct LpcFrame {
    float energy = 0.f;
    float pitchPeriod = 0.f;
    std::array<float, 10> reflection{};
    bool voiced = false;
    bool silence = false;
};
```

`pitchPeriod` is expressed in internal synthesis samples. A zero-energy frame
is silence. Stop and repeat are bitstream commands and are resolved before the
engine receives decoded frames.

### 5.2 Immutable sequence

An `LpcSequence` contains:

- a bounded array/span of decoded or packed frames;
- phrase/utterance identity;
- frame timing metadata;
- optional phoneme-boundary markers; and
- a generation number for UI/audio publication.

The engine must support sequences prepared from bundled data and sequences
prepared dynamically without branching on their origin.

### 5.3 Text publication

The UI or a non-audio compiler prepares a complete sequence in an inactive,
preallocated mailbox slot. Publication changes an atomic generation/index.
The audio thread adopts a ready slot only at a safe process boundary.

Use a double- or triple-buffered fixed-capacity mailbox. Do not use a mutex in
the audio callback, and do not allow the final reference to a heap allocation
to be destroyed on the audio thread.

## 6. Text and pronunciation frontend

The initial text target is deterministic English pronunciation with an
explicit phoneme escape path.

Processing stages:

1. Normalize whitespace, punctuation, case, common numbers, and abbreviations.
2. Split text into words and punctuation/prosody tokens.
3. Look up optional dictionary overrides.
4. Apply deterministic letter-to-sound rules for unknown words.
5. Convert the result to the internal phoneme inventory.
6. Assign default stress, pitch contour, energy, and duration.
7. Insert transition frames and silence.
8. Produce a bounded immutable `LpcSequence`.

The direct phoneme syntax should use a documented ASCII inventory, such as an
ARPABET-like form inside brackets:

```text
hello [L IY V AY AH TH AH N]
```

The exact syntax and supported English coverage must be frozen before public
release. Unknown or unpronounceable tokens should produce a visible status
message and a deterministic fallback, never an audio-thread exception.

## 7. DSP implementation

### 7.1 Pitch

For internally generated voiced excitation:

```text
baseHz  = internalRate / decodedPitchPeriod
pitchHz = baseHz * 2^(rootOctaves + voctAtten * voctVoltage)
```

Cache pitch conversion at control/internal-tick rate. Use a fast exponential
approximation such as `rack::dsp::exp2_taylor5`; do not call `pow()` per host
sample. Pitch CV does not rescale the vocal-tract coefficients.

### 7.2 Excitation

The voiced source is the specified dual-slope triangular chirp. The unvoiced
source is a deterministic 17-bit LFSR with a documented primitive polynomial
and nonzero reset seed.

Tests must establish the exact chirp cycle, LFSR sequence, and LFSR period.
Changing either after release would change existing patches and therefore
requires an explicit compatibility mode.

### 7.3 Lattice filter

The order-10 reflection filter is processed only on internal synthesis ticks
in 8/10 kHz modes and on every host sample in host-rate mode.

Stable operation clamps effective coefficients just inside `(-1, 1)`. Bend
mode may extend to a documented limit above unity to permit self-oscillation,
but every stage must remain finite. The engine must detect and recover from
NaN/infinity without poisoning later samples.

### 7.4 Formant warp

Combine the knob and `WARP CV`, clamp to the declared bipolar range, and smooth
the result. Compute warped coefficients on internal ticks using a fast tanh
approximation or lookup table:

```text
k'[m] = tanh(k[m] * (1 + warp))
```

Ten standard-library `tanh()` calls per host sample are not acceptable.

### 7.5 Starvation and circuit bending

`BEND / STARVE` and `BEND CV` control a deterministic combination of:

- internal clock slowdown;
- bounded clock jitter;
- skipped synthesis calculations;
- state leakage during skipped cycles; and
- controlled access to unstable coefficient values.

`GLITCH / MANGLE` is a snapped integer from 0 through 15. Each nonzero value
selects a fixed corruption recipe combining address XOR, signed frame offset,
packed-field masks, or occasional command corruption. Level 0 must be exactly
identical to the clean decoder path.

All pseudo-random corruption uses a module-owned serialized seed. Do not call
Rack global randomness per sample.

### 7.6 Output stage

Normalize the engine with a fixed calibrated gain and use a cheap bounded soft
clip. `AUDIO OUT` must remain finite and within approximately +/-5 V under
maximum warp and instability. Add a lightweight DC blocker only if reference
renders demonstrate a persistent offset problem.

## 8. Transport behavior

- `SPEED / SCRUB` covers -4x to +4x with a reliable zero deadband.
- Positive speed advances toward the last frame; negative speed moves toward
  the first frame.
- At zero speed, `SCRUB CV` maps -5 V to the beginning and +5 V to the end.
- Retrigger resets playhead, interpolation, excitation phase, decoder command
  state, and EOX arming.
- `FRAME CLK` emits one 1 ms pulse whenever the selected source frame changes.
- `EOX` emits one 1 ms pulse at the end in forward playback and at the start in
  reverse playback.
- EOX rearms only after retriggering or moving away from the terminal boundary.
- A large scrub jump emits at most one frame pulse for the observed change; it
  does not enqueue a burst for every skipped frame.

## 9. Rack module contract

Phonex is unreleased, so its initial enum layout can be chosen freely. Freeze
the following ordering before its first public release and append future IDs.

```text
Params:
PITCH, VOCT_ATTEN, SPEED, WARP, EXCITE_BLEND,
BEND, GLITCH, WORD, WORD_PUSH

Inputs:
VOCT, TRIG_GATE, SCRUB_CV, WARP_CV, BEND_CV, EXT_EXCITE

Outputs:
AUDIO, FRAME_CLK, EOX

Lights:
VOICED, FRAME, EOX, BEND
```

State schema version 1 persists:

- submitted text;
- active source (`Bundled` or `Text`);
- internal-rate mode;
- reconstruction mode;
- interpolation mode;
- trigger-input mode;
- forced-excitation target; and
- deterministic glitch/starvation seed.

Normal Rack parameters are already serialized and should not be duplicated in
module JSON. Clamp all loaded settings and tolerate missing schema-1 fields.

## 10. Panel implementation

The master panel is `71.12 mm x 128.5 mm` and follows the requested matte dark
slate, cyan/amber indicator, monochrome vector, and runic/schematic visual
language.

`res/Phonex.svg` is the editable source of truth. It contains hidden anchors
for all controls, ports, lights, the text field, the phrase/status display, and
the encoder push target. Runtime placement uses `PanelSvgUtils` and the
components layer rather than duplicated hard-coded positions.

The widget uses `visual_assets::SplitPanelRenderer` with generated panel and
label layers. The text field should follow existing `ui::TextField` patterns in
`OctaviaConsole.cpp`, `Deepcache.cpp`, and `CrownstepSettingsOverlay.cpp`.

After master changes:

```sh
python3 tools/split_svg_labels.py res/Phonex.svg --overwrite
make generate-panel-anchor-atlas
```

Never edit `res/Phonex.panel.svg` or `res/Phonex.labels.svg` directly.

## 11. Implementation phases

### Phase 1: Contracts and reference fixtures

- Freeze frame, sequence, packed-command, and engine-setting types.
- Create procedural voiced, unvoiced, silence, and transition fixtures.
- Implement deterministic corpus generation and `--check` validation.
- Establish scalar reference equations for golden tests.

Exit: fixtures decode exactly and malformed packed streams fail safely.

### Phase 2: Transport and clean DSP core

- Implement forward/reverse/frozen transport.
- Implement frame interpolation and boundary events.
- Implement chirp, LFSR, lattice, output scale, and finite recovery.
- Implement 8 kHz, 10 kHz, and host-rate scheduling.

Exit: standalone engine tests pass with procedural sequences.

### Phase 3: Warp, bend, and glitch

- Add cached fast formant warp.
- Add deterministic starvation and jitter.
- Add the 15 fixed corruption recipes.
- Add clean/bent reference renders for audition.

Exit: clean level 0 is invariant; all extreme settings remain bounded.

### Phase 4: Authored phoneme corpus

- Define the phoneme inventory and transition strategy.
- Create or record the initial clean-room corpus.
- Add provenance metadata and the generated compiled ROM.
- Tune durations, energy, pitch, and joins by audition.

Exit: the engine can speak a useful direct phoneme sequence without text G2P.

### Phase 5: Text frontend

- Implement normalization and direct phoneme syntax.
- Implement dictionary overrides and deterministic letter-to-sound fallback.
- Assemble phoneme output into timed LPC sequences.
- Implement the lock-free sequence mailbox.

Exit: arbitrary supported text produces deterministic sequences without any
audio-thread text work.

### Phase 6: Rack wrapper and persistence

- Add module enums, configuration, ports, pulses, lights, reset, and JSON.
- Connect CV scaling and context-menu settings.
- Register the model in `plugin.hpp`, `plugin.cpp`, and `plugin.json`.
- Add Rack-facing tests.

Exit: headless module tests cover controls, pulses, source switching, and state.

### Phase 7: Panel and text UI

- Author the 14 HP master SVG and anchors.
- Add split-panel rendering and themed components.
- Add text entry, status feedback, phrase display, and composite encoder push.
- Add preview-safe null-module behavior.

Exit: panel contract and module-browser preview tests pass.

### Phase 8: Performance, audition, and release hardening

- Profile clean, text, bend, and host-rate modes.
- Remove allocations, locks, and expensive per-sample operations.
- Add optional Dragon King Debug Terminal metrics if profiling requires them;
  preserve the `Process`, `Step`, and `Draw` macro metric contract.
- Audit corpus provenance and distributable contents.
- Run native Windows tests and build.

Exit: all automated validation and the authoritative `plugin.dll` build pass.

## 12. Test plan

### Standalone engine

- Ten-stage impulse response against a double-precision reference.
- Stable coefficient limits and intentional unstable-range recovery.
- Exact chirp fixture and LFSR period/determinism.
- Pitch-period accuracy at all internal modes and common host rates.
- Long-duration internal scheduler tick counts.
- Linear/cubic endpoints and voiced/unvoiced transitions.
- Forward, reverse, freeze, scrub, retrigger, and clocked advance.
- Exactly one frame/EOX event per defined transition.
- External-excitation bypass and normalization.
- Zero-glitch identity and seeded nonzero-glitch repeatability.
- Finite, bounded output under randomized extreme controls.

### Pronunciation and sequence compiler

- Normalization of case, whitespace, punctuation, and supported numbers.
- Dictionary override precedence.
- Deterministic fallback pronunciation.
- Direct phoneme parsing and invalid-token diagnostics.
- Duration, stress, silence, and transition insertion.
- Maximum text/sequence length handling without truncation ambiguity.
- Identical input/settings produce identical frame sequences.

### Rack module

- Parameter ranges, defaults, snap behavior, and port names.
- V/oct and attenuverter mapping.
- 1 ms pulses within one host sample of tolerance.
- Bundled/text source switching.
- Patch JSON round trip and malformed-state clamping.
- Reset and sample-rate-change behavior.

### Panel

- Exactly 14 HP.
- Required anchors exist in master and generated panel.
- Labels are split correctly.
- Text-field and display rectangles are valid.
- Preview construction succeeds with a null module.

## 13. Build and validation

Add the new standalone and Rack test binaries to `TEST_BINS_NON_RACK` or
`TEST_BINS_RACK` as appropriate. Validation order:

```sh
python3 tools/generate_phonex_rom.py --check
python3 tests/phonex_panel_contract_spec.py
make validate-plugin-json
make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10
```

Run the final test and plugin builds from native MSYS2 MINGW64. A successful
native link producing `plugin.dll` is the authoritative Windows result.

## 14. Definition of done

Phonex is complete when:

- no vintage or proprietary speech ROM is required or distributed;
- bundled and typed-text sources drive the same tested DSP backend;
- supported arbitrary text compiles deterministically into pronounceable LPC
  sequences;
- 8 kHz, 10 kHz, and host-rate modes survive host sample-rate changes;
- transport and trigger outputs behave correctly in both directions;
- maximum bend/glitch settings never produce NaN, infinity, or output beyond
  the documented voltage bound;
- the audio path performs no allocation, locking, file I/O, or text work;
- corpus provenance is complete;
- panel generation, manifest validation, `test-fast`, and the native Windows
  plugin build all pass; and
- parameter, input, output, light, JSON, chirp, LFSR, and corruption contracts
  are frozen for release compatibility.

## 15. Primary technical references

- NIST FIPS 137, 2,400 bit/s LPC:
  <https://nvlpubs.nist.gov/nistpubs/Legacy/FIPS/fipspub137.pdf>
- Texas Instruments TSP50C0x/1x Family Speech Synthesizer Design Manual:
  <https://www.ti.com/lit/ml/spss011d/spss011d.pdf>
- Texas Instruments MSP50C3x User's Guide:
  <https://www.ti.com/lit/ug/spsu006c/spsu006c.pdf>
- Speak & Spell parameter-interpolator patent and learning-aid architecture:
  <https://patents.searchlight.law/doc/US4189779>

