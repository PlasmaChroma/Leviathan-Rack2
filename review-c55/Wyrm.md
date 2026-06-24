# Wyrm Review

## 1. Executive Summary

Wyrm is a 16-voice drawable wavetable oscillator with mip tables, FM, sync, folding, slither motion, editable rock constraints, and optional sand rendering. It is musically distinctive and one of the few polyphonic modules, but wavetable rebuilding occurs in the audio callback and the UI/rendering system is large and insufficiently tested.

Release readiness: 6/10

## 2. Module Inventory

- Source/UI: `Wyrm.cpp/.hpp`, `WyrmWidget.cpp`, `WyrmWaveEditor.cpp`, `WyrmSand.cpp/.hpp`, `WyrmSandGL.cpp`; shared visual/GL helpers. Assets: `res/wyrm.svg`, lock/reset and `Vahdrim'Keth` icons.
- Params (10): Frequency, Fine, FM attenuator, Fold, Slither, Slither speed, wave left/right, LFO, sync mode. Inputs (6): V/Oct, FM, Sync, Fold/Slither/Speed CV. Outputs: Fold, Raw. Lights: LFO, sync mode.
- Menu/state: editor lock, renderer, sand view/detail/persistence, rocks/mouse mode/count, point count, factory shape. JSON saves all editor/render/rock/wave data. No expander.

## 3. DSP and Audio/CV Correctness

Channel count follows the maximum connected modulation input up to fixed 16-channel arrays; mono CV broadcasts and poly CV uses `getPolyVoltage()` (`Wyrm.cpp:976-1136`). Frequency is sample-rate bounded and wave lookup interpolates mip levels. Sync is hard or directional soft. JSON wave values are clamped but not type/finite checked. There is no explicit MinBLEP for hard sync; mip filtering only addresses table bandwidth.

## 4. UI, Panel, and Interaction Review

Direct waveform/rock editing strongly communicates purpose and fits the artifact aesthetic. Renderer/sand controls are deep, and three render modes plus complex custom input increase fragility. Verify pointer capture, editor lock, framebuffer invalidation, and zoom alignment.

## 5. Performance Review

- Severity: Critical — any `waveVersion` change rebuilds 8x2048 wavetable samples inside `process()` (`Wyrm.cpp:193-222,964-972`), causing audio spikes while drawing.
- Severity: High — sand/OpenGL code builds vectors/pixel buffers and complex geometry (`WyrmSandGL.cpp`); GPU/memory budgets are not covered by tests.
- Severity: Medium — rock processing scales by voices and rocks; worst-case 16-voice profiling is needed.

## 6. Stability and Rack Integration

State is extensively serialized and arrays are bounded. `dataFromJson()` does not guard null/non-object roots and accepts numeric non-finite values (`Wyrm.cpp:871-960`). GL teardown and rapid renderer changes need real Rack stress. Duplicate modules correctly own independent arrays.

## 7. Code Quality and Maintainability

DSP and renderer files are separated, but wave editor and sand GL exceed 1,000 lines each. The lock-free version signal is appropriate; rebuilding should produce a complete table off-thread/UI-thread and atomically swap it.

## 8. Musical Usefulness

Drawable polyphonic waves, fold, slither, rocks, sync and LFO mode create a unique oscillator. Defaults are sensible. Rock/sand features risk obscuring basic synthesis and need a short manual with example patches.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| WYRM-001 | Critical | Audio RT | Wavetable rebuild runs in audio callback after edits/load. | `Wyrm.cpp:193-222,964-972` | Build into a back table off audio thread and atomically swap. |
| WYRM-002 | High | UI/GPU | Sand render paths allocate/construct substantial geometry. | `WyrmSandGL.cpp:287,457-704` | Cache/reuse buffers; profile all detail modes. |
| WYRM-003 | Medium | Robustness | JSON lacks root/type/finite validation. | `Wyrm.cpp:871-960` | Validate object/numbers and replace non-finite values. |
| WYRM-004 | Medium | Verification | No oscillator/polyphony/renderer tests. | `Makefile` | Add DSP and lifecycle tests. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Move wavetable construction off the audio thread.

### Should Fix Soon

Harden JSON, profile 16 voices/rocks/sand, and test renderer lifecycle.

### Nice to Have

Add sync bandlimiting and a concise musical manual.

## 11. Suggested Tests

Automate pitch/amplitude/finite output at 44.1-192 kHz and 1/16 voices; edit continuously while checking callback max; hard/soft sync spectra; extreme FM/fold/slither/rocks; preset/duplicate/malformed JSON; renderer switching/GL loss; 20 instances; zoom and pointer-capture tests.

## 12. Final Verdict

Status: Experimental  
Primary blocker: wavetable rebuild in `process()`  
Best next action: implement immutable double-buffered wavetable swaps
