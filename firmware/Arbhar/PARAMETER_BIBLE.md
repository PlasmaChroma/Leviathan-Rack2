# Parameter and I/O bible

**Status:** a mixed-evidence implementation map, not a claim that every transfer function is recovered. `F`, `S`, `D`, and `R` have the meanings defined in the main report. JSON companion: `tables/parameter_bible.json`.

## 1. Inputs, outputs, and host adaptation

Hardware has two audio input roles: **IN** and **ONSET**. In stereo mode these become left/right; in mono mode ONSET is the analysis input. The documented hardware normalization is microphone → ONSET → IN. A native plugin should not silently open a computer microphone. Provide an explicit optional source or leave unpatched input silent, and label that adaptation. [D3 pp10/37; R]

| Port/group | Contract | Evidence |
|---|---|---|
| IN; ONSET / RIGHT | Two audio roles with mono/stereo reinterpretation | D3; `arbharRecorder.pd`, input externals |
| OUT 1; OUT 2 | Stereo audio; phase and cable-dependent summing need explicit policy | D3; `main_out_stereo~` |
| CAPTURE; STRIKE | Distinct gate/trigger event inputs | GPIO handlers and exact configuration enums |
| Pulse OUT | Mode-derived trigger output, not audio | `_setTriggerOut`; selected trigger-source config |
| Main CV | Intensity, Length, Scan, 1 V/oct | D3 panel; control path |
| Expansion CV | Spray, Layer, Direction, Texture, Deviation, Dub, Mod, Dry/Wet | D3 expansion panel; included control notes |

Integrating all expansion controls into the Rack panel is a recommendation, not a claim about the original physical layout. Use ±5 V as a proposed nominal audio scaling, Schmitt triggering at 0.1/1 V, and 10 V/1 ms pulse output for a Rack-oriented interface. These choices follow the host conventions and are **not measured arbhar circuit thresholds**. [R; D5]

## 2. Controls and recovered laws

“Notes ADC channel” below is an archive analysis aid. It is **not** a complete hardware schematic or proof of jack-to-voltage scaling. Null/— means not established in this handoff.

| Control | Notes ADC channel | Shared slot | Evidence | Recovered behavior and boundary |
|---|---:|---:|---|---|
| Scan / Follow traversal | 0 | 100 | S | Scan approximately 480000/4095 samples per calibrated 12-bit unit; Follow uses a separate speed helper. **Open:** Omega endpoint and capture-head boundary branches unresolved. |
| Grain duration / wavetable entry | 1 | 101 | S | u² * 100000 units, then *1.44 samples clamped 128..144000 in nominal-48k domain. **Open:** Mode-dependent entry thresholds and exact physical minimum require validation. |
| Pitch CV | 2 | — | F/S/D | 1 V/oct is the documented musical contract; raw hardware and WT paths require calibration. **Open:** Do not use raw ADC scaling as a Rack volts formula. |
| Pitch knob | 3 | — | F/S | Named pitch functions and exact PITCH_VALUES table recovered. **Open:** Full normal-mode center detent, quantisation, and MIDI merge not completely reconstructed. |
| Layer / Omega | 5 | 30 | F/S | Six layers plus Omega; play and record layer may be separate. **Open:** Notes distinguish layer CV channel 4; switching/hysteresis and coupling need complete truth table. |
| Dry / Wet | 6 | 6 | S | Steady-state equal-power cosine/sine curve; raw control is inverted in the main patch. **Open:** Pd table approximation and smoothing not reproduced by ideal C++ sin/cos. |
| Hold | 8 | 38 | F/S | Onset hold state; recorder converts shared milliseconds with factor 48. **Open:** Also affects Follow loop-length policy when configured; full panel law unresolved. |
| Spray | 9 | 103 | S | u² * 100000 units, then *4.8 samples; Scan and Follow use different placement branches. **Open:** Not a proven uniform bipolar offset; MIDI slot 212 participates upstream. |
| Grain direction probability | 10 | — | F/S | Per-grain direction and _dirPercent are present; reverse branch distinct. **Open:** Complete probability transfer and draw order unresolved. |
| Dub / recording blend | 11 | — | F/S | Recorder has handover state and coefficient slew near 0.0078/sample. **Open:** Complete record equation and all capture fades unresolved. |
| Continuous grain production | 12 | — | F/S/D | Separate continuous and struck engines; timing and amplitude randomization are independent options. **Open:** Exact density law and duration coupling are high-priority open work. |
| Configurable modulation | 13 | — | F/S | Enum selects none/pan/hold/reverb/delay; dedicated defaults also exist. **Open:** Do not make effects mutually exclusive in the 2.1-style feature set. |
| Grain window / texture | 14 | — | F/S | 101 x 515 reconstructed bank from exact templates. **Open:** Complete control-to-row and reverse phase stepping not proven. |
| Pitch deviation | 15 | — | F/S | Ordered quantisation sequences and setDeviation routine recovered. **Open:** Probability selection, continuous vs quantised behavior, and timing still incomplete. |
| Capture button/CV | — | — | F/S | CV modes latch/momentary/retrigger; separate button policy. **Open:** Simultaneous events, accumulative behavior, and record destination locking need tests. |
| Strike button/CV | — | — | F/S | Independent grain event source with configurable CV delay. **Open:** Pitch/control latch timing and scheduler order remain explicit fidelity work. |
| Shift / gesture layer | — | — | F/S/D | Multiple mode/gesture handlers and staged parameter behavior. **Open:** Full hold/double-press timing and precedence not recovered. |
| Onset sensitivity / alternate input level | — | — | F/D | Normal onset path is bonk~ after four hip~300 stages. **Open:** Analog control and stereo-mode level behavior not fully reconstructed. |
| Input level | — | — | D/S | Separate input conditioning and analog preamp/limiter target. **Open:** No authentic analog transfer measurement supplied. |
| Output level | — | — | D/S | Main output/phase/mute path separate from rational clipping. **Open:** Absolute hardware dB/V calibration not established. |

## 3. Exact configuration keys and defaults

Values below come from parsed files, not from assumed GUI defaults. `—` means the key was absent in that source; do not convert it to zero. Columns are the initialization fallback, then alpha/beta/gamma/delta/epsilon/zeta. Array-valued fields are preserved in full in JSON and in the original files.

| Key | Init | α | β | γ | δ | ε | ζ |
|---|---|---|---|---|---|---|
| `AccumulateRecordingsInLayer` | — | — | — | — | — | — | — |
| `AccumulativeCaptureMode` | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
| `ActivateCaptureOnButtonUp` | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| `AnalogEmulation` | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
| `AnalogEmulationOfOnsetInput` | — | — | — | — | — | — | — |
| `CaptureButtonAsGate` | — | — | — | — | — | — | — |
| `CaptureButtonMode` | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| `CaptureCVMode` | 1 | 1 | 1 | 1 | 2 | 1 | 1 |
| `CaptureCvMode` | — | — | — | — | — | — | — |
| `ClockBPM` | 120.0 | — | — | — | — | — | — |
| `ClockDivisions` | array (5) | — | — | — | — | — | — |
| `ClockMultiplications` | array (11) | — | — | — | — | — | — |
| `ClockedMode` | 0 | — | — | — | — | — | — |
| `DelayDefaultValue` | — | — | — | — | — | — | — |
| `DelayDefaultValueInPerCent` | — | 0 | 0 | 0 | 0 | 0 | 0 |
| `EffectOrder` | — | 0 | 0 | 0 | 0 | 0 | 0 |
| `EnableClockedModeSwitch` | 0 | — | — | — | — | — | — |
| `FollowLoop` | — | 1 | 1 | 1 | 1 | 1 | 1 |
| `FollowMode` | 0 | 0 | 0 | 0 | 1 | 0 | 0 |
| `FollowPositionOffset` | — | — | — | — | — | — | — |
| `FollowPositionOffsetWithScanCV` | — | 1 | 1 | 1 | 1 | 1 | 1 |
| `FollowScanOffset` | — | 0 | 0 | 0 | 0 | 0 | 0 |
| `FollowSetLoopLengthWithHold` | — | 1 | 1 | 1 | 1 | 1 | 1 |
| `FollowSpeedBiDirectional` | — | — | — | — | — | — | — |
| `FollowSpeedDirection` | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
| `HoldDefaultValue` | — | — | — | — | — | — | — |
| `InputClockDivision` | 1 | — | — | — | — | — | — |
| `InputMode` | 0 | 0 | 0 | 1 | 0 | 0 | 0 |
| `LinkAccumulativeRecordingCaptureAsGate` | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| `LoadConfiguration` | 3 | 1 | 1 | 1 | 1 | 1 | 1 |
| `ModCV` | 3 | 3 | 4 | 3 | 2 | 1 | 3 |
| `OnsetMode` | 1 | 1 | 1 | 4 | 6 | 3 | 4 |
| `OutputPhaseSwitch` | — | — | — | — | — | — | — |
| `PanningDefaultValue` | — | — | — | — | — | — | — |
| `PanningDefaultValueInPerCent` | — | 0 | 0 | 0 | 0 | 0 | 0 |
| `PhaseSwitch` | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
| `QuantiseTable` | array (24) | array (24) | array (10) | array (24) | array (10) | array (10) | array (24) |
| `RandomAmpWithRandomIntensity` | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| `RandomTimingWithRandomIntensity` | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
| `ReverbDefaultValue` | — | — | — | — | — | — | — |
| `ReverbDefaultValueInPerCent` | — | 0 | 0 | 0 | 0 | 0 | 0 |
| `ReverbReset` | — | 0 | 0 | 0 | 0 | 0 | 0 |
| `SetQuantiseTable` | — | — | — | — | — | — | — |
| `StrikeButtonToTrigger` | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| `StrikeCVDelay` | 10 | 0 | 10 | 10 | 10 | 10 | 10 |
| `StrikeCvDelay` | — | — | — | — | — | — | — |
| `WavetableCentreFrequency` | 130.813 | 130.813 | 130.813 | 130.813 | 130.813 | 130.813 | 130.813 |
| `feature` | — | — | — | — | — | — | — |

## 4. Enum semantics and specific cautions

**`AnalogEmulation`** — 0 = 0 - Disable; 1 = 1 - Enable.

**`CaptureButtonMode`** — 0 = 0 - Latching; 1 = 1 - Momentary.

**`CaptureCVMode`** — 0 = 0 - Latching; 1 = 1 - Momentary; 2 = 2 - Retrigger.

**`EffectOrder`** — 0 = 0 - Delay and Reverb in parallel; 1 = 1 - Delay and Reverb in series.

**`FollowLoop`** — 0 = 0 - Disable; 1 = 1 - Enable.

**`FollowMode`** — 0 = 0 - Scan Mode; 1 = 1 - Follow Mode.

**`FollowPositionOffset`** — 0 = 0 - Both Control; 1 = 1 - Scan Knob Only.

**`FollowPositionOffsetWithScanCV`** — 0 = knob and CV control speed/offset policy; 1 = Scan knob only offset policy; see original configuration comments.

**`FollowSpeedDirection`** — 0 = unidirectional; 1 = bidirectional; 2 = inverted-control unidirectional.

**`InputMode`** — 0 = 0 - Mono; 1 = 1 - Stereo.

**`LoadConfiguration`** — 0 = 0 - Load Nothing; 1 = 1 - Load Preset; 2 = 2 - Load Layers; 3 = 3 - Load Scene.

**`ModCV`** — 0 = 0 - None; 1 = 1 - Panning; 2 = 2 - Hold; 3 = 3 - Reverb; 4 = 4 - Delay.

**`OnsetMode`** — 1 = 1 - Alpha; 2 = 2 - Beta; 3 = 3 - Gamma; 4 = 4 - Delta; 5 = 5 - Epsilon; 6 = 6 - Zeta.

**`PhaseSwitch`** — 0 = 0 - Phase-Inverted; 1 = 1 - Phase-Corrected.

**`ReverbReset`** — 0 = disabled; 1 = Strike; 2 = onset; 3 = Strike and onset.

**`StrikeCVDelay`** — Milliseconds; Classic=0, other named factories=10, initialization fallback=10. Not a universal default.

**`QuantiseTable`** — Ordered signed sequence; preserve order, duplicates, and pairing rather than sorting to pitch classes.

**`FollowSpeedDirection`** — The compiled helper is numerically probed. Mode 2 reverses control orientation, not all playback signs.

**`EffectOrder`** — Parallel/series are exact config labels. Full gain-normalized transition behavior remains unresolved.

**`ReverbReset`** — The patch supports duck/restore. Tank-memory clearing is not established.

**`EnableClockedModeSwitch`** — Zero in initialization fallback. Present experimental/optional feature, not an assumed stock default.

**`ClockedMode`** — Zero in initialization fallback; disabled baseline.

**`LoadConfiguration`** — Startup fallback is 3; named factories are 1; HTML selected option is not actual startup precedence.

**`WavetableCentreFrequency`** — Hz; separate 0.976642 factor is applied downstream in the WT patch.

**`OnsetMode`** — Six named profiles. Full per-profile transition/trigger behavior still needs a truth table.

## 5. Shared-memory tracing

The 250-value block at key 61019 mixes raw/processed controls, UI state, MIDI state, and engine communication. `tables/shmem_patch_references.json` contains 150 literal patch references, and the disassembly shows additional direct accesses. This is not a complete typed layout. Of particular importance, compiled Length writes slot 101 and Spray writes slot 103; an older note about 103 is not authoritative.

Capture-related flags 105/106, onset marker 109, and record-position communication require ordering analysis before a native translation. Quantisation and MIDI ranges must not be treated as independent fixed-size application structs merely because an index is visible. The native engine should replace this mixed array with typed objects rather than recreating ad hoc magic indices. [S/R]

## 6. Sources

File evidence: `tables/presets.json`, `tables/preset_matrix.csv`, `tables/preset_editor_fields.json`, `extracted/Pin_Adc_Control_Info/`, compiled GPIO/player/recorder functions, and original Pd graphs. The main report defines D3 (official 2.0 manual, selected panel/I/O pages inspected) and D5 (official Rack voltage conventions).
