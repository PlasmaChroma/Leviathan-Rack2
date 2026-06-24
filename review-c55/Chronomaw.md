# Chronomaw Review

## 1. Executive Summary

Chronomaw is intended as an eight-output clock/modulation engine with banks, waveform shaping and direct timeline editing. The visual editor and serialization scaffold are substantial, but the engine ignores external clock and all four CV inputs, forces density mode to Monitor, and is explicitly hidden/WIP. It is not releasable.

Release readiness: 3/10

## 2. Module Inventory

- Source/UI: `Chronomaw.cpp/.hpp`, engine, state, waveforms, widget, and small registry/quantizer/clock/serialization/timeline headers. Asset: `res/chronomaw.svg`.
- Params (8): Run, BPM, bank, load/save, selected output, timeline zoom, density. Inputs: Clock, Run, Reset, CV1-4. Outputs: eight CV. Lights: run, sync, eight output.
- Menu/state: sampled future timeline toggle and waveform popup. JSON stores live state, 64 banks, output waveform/modifier/rates/shape/probability/seed, and UI state. No expander. `plugin.json` marks it hidden.

## 3. DSP and Audio/CV Correctness

Internal BPM uses `sampleTime`, outputs are clamped 0-5 V, and waveform state is deterministic. `FrameInputs::clkConnected/clkVoltage` are never consumed in `Engine::process()` (`ChronomawEngine.cpp:24-133`), while CV1-4 are never copied at all (`Chronomaw.cpp:199-218`). Reset is level-sensitive. `process()` overwrites density to Monitor and resets its param every sample (`Chronomaw.cpp:175-181`). This contradicts the advertised clock/CV/editing intent.

## 4. UI, Panel, and Interaction Review

The timeline, output selectors and direct controls form a coherent high-tech surface, but a 1,674-line custom widget carries significant redraw cost. Controls for nonfunctional inputs/modes are misleading. Keep the module hidden until behavior and labels match.

## 5. Performance Review

- Severity: Medium — every module maintains thousands of atomic timeline samples and updates histories at 60 Hz (`Chronomaw.hpp:51-86`; `Chronomaw.cpp:220-268`).
- Severity: Medium — timeline rendering builds/reserves vectors and many NanoVG paths (`ChronomawWidget.cpp:938-1260`).
- Severity: Low — core waveform DSP is fixed-size and allocation-free.

## 6. Stability and Rack Integration

JSON clamps most state and supports an early UI layout. The WIP serialization test is skipped by default. `outputStateFromJson()` does not consistently preserve defaults when boolean fields are absent and schemaVersion is written but not used for migration. Hidden registration is appropriate.

## 7. Code Quality and Maintainability

State/engine/UI separation is good. Several registry/serialization headers are declarations or scaffolding rather than active architecture. Complete a written input/mode contract and tests before expanding abstractions.

## 8. Musical Usefulness

The proposed eight-output banked modulation source could be valuable, but external synchronization and modulation are core requirements. In current form it is an internal-BPM generator with a sophisticated editor and does not deliver its browser description.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| CHRONOMAW-001 | Critical | DSP | External clock voltage/connect state is ignored. | `ChronomawEngine.cpp:24-133`; `Chronomaw.cpp:199-218` | Implement edge/PPQN clocking and clock-loss policy. |
| CHRONOMAW-002 | High | DSP/UI | CV1-4 are unused. | `Chronomaw.cpp:199-218` | Define routing/modulation matrix or remove jacks. |
| CHRONOMAW-003 | High | UI/DSP | Density is forced to Monitor each sample. | `Chronomaw.cpp:175-181` | Implement modes or remove/disable control. |
| CHRONOMAW-004 | High | Verification | WIP serialization test is skipped and no engine clock tests exist. | `Makefile` | Enable/fix serialization tests and add clock/waveform tests. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Implement external clock, define CV inputs, implement/remove density, and enable core tests.

### Should Fix Soon

Validate all waveform/probability/bank transitions and timeline performance.

### Nice to Have

Prune unused scaffold headers after the behavior contract stabilizes.

## 11. Suggested Tests

Internal/external clock at multiple PPQN and jitter, run/reset gates, clock disconnect, all waveforms/rates/probability seeds, CV routing, bank save/load, preset/duplicate, 44.1-192 kHz, timeline zoom, 50 instances and malformed schema migration.

## 12. Final Verdict

Status: Needs major work  
Primary blocker: advertised clock/CV inputs are nonfunctional  
Best next action: freeze UI work and complete an engine/input contract with tests
