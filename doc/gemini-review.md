# Code Review: Maths Module Emulation (Revision 2013)

## 1. Overview
This review covers the implementation of the `Maths` module in `src/Maths.cpp`, `src/plugin.hpp`, and `src/plugin.cpp`. The module aims to emulate the Make Noise Maths (2013 revision) for VCV Rack 2.

## 2. Architecture & Design

### 2.1 Struct-Based Channel Logic
The use of `OuterChannelState`, `OuterChannelConfig`, and `OuterChannelResult` is a good abstraction for handling the nearly identical logic of Channels 1 and 4. This promotes code reuse and reduces the risk of copy-paste errors between the two channels.

### 2.2 Mixing Bus Modeling
The `MixNonIdealCal` struct and the associated logic for "Analog Mix Non-Idealities" show an effort to go beyond basic math and model hardware characteristics like soft saturation and diode drops. This is a high-quality touch for an emulation.

---

## 3. DSP & Logic Analysis

### 3.1 Channels 1 & 4 (Function Generator)
*   **Calibration**: The timing constants (`logShapeTimeScale`, `expShapeTimeScale`, and `minTime` in `computeStageTime`) are accurately calibrated against the hardware measurements provided in `doc/Measurements.md`.
*   **Shaping**: The `slopeWarp` approach successfully models the LIN/LOG/EXP response.
*   **Cycle Logic**: 
    *   *Observation*: `cycleCvGate.process(...)` is used, which returns true only on the rising edge. 
    *   *Issue*: The specification suggests the Cycle CV should be level-sensitive (HIGH = ON, LOW = OFF). The current implementation will only trigger a cycle start on the rising edge but won't hold the cycle active if the CV is high and the button is off.
*   **EOR/EOC Outputs**:
    *   *Logic*: EOR is high during `OUTER_FALL`. EOC is high during `OUTER_RISE`.
    *   *Review*: This is a standard interpretation of "End of Rise" (starting the next phase) and "End of Cycle" (starting the rise). However, it should be verified if EOC should also be high during `IDLE` or if it's strictly a "during rise" gate.

### 3.2 Slew Limiting (Signal Input)
*   *Observation*: When in `OUTER_IDLE` and a signal is patched, `processRampageSlew` is used.
*   *Issue*: This uses a different math model (based on Befaco Rampage) than the function generator's `slopeWarp`. In the hardware Maths, the same core circuitry is used for both slew and function generation. Using two different models might lead to inconsistent shaping between the "Slew" and "Envelope" modes.

### 3.3 Channels 2 & 3
*   *Accuracy*: Correct implementation of normalization (+10V for Ch 2, +5V for Ch 3) and attenuverter logic.

### 3.4 Mixing Bus
*   *Correctness*: The break-normal behavior (removing a channel from the bus if its Variable Out is patched) is correctly implemented.
*   *Saturation*: Soft saturation is well-implemented but currently hardcoded to 10V.

---

## 4. Performance & Efficiency

### 4.1 Numerical Integration Bottleneck
*   *CRITICAL*: `slopeWarpScale(s)` is called **every sample** for both channels. This function contains a loop with 16 iterations of `slopeWarp` (which includes `std::pow`).
*   *Impact*: This is a significant performance drain. At 44.1kHz, this results in over 1.4 million iterations per second.
*   *Recommendation*: Cache the result of `slopeWarpScale` and only recompute it when the `Shape` parameter (or the relevant CV) changes.

### 4.2 Math Functions
*   `std::pow` and `std::tanh` are used in the sample loop. While necessary for accuracy, they are expensive. Consider using approximations or look-up tables if high instance counts are expected.

---

## 5. Code Quality & Best Practices

### 5.1 Global State (UI)
*   *Issue*: `imageHandle` in `src/Maths.cpp` is a global variable.
*   *Impact*: This can cause issues in VCV Rack where multiple instances or context changes might occur. 
*   *Recommendation*: Move `imageHandle` to the `MathsWidget` class as a member variable.

### 5.2 Magic Numbers
*   There are several hardcoded calibration constants in `process()`. While they match `Measurements.md`, they would be better documented as named constants in a `Calibration` namespace or struct.

---

## 6. Comparison with Specifications (Codex)

| Feature | Codex Requirement | Implementation Status |
| :--- | :--- | :--- |
| **Cycle CV** | Gate-qualified (+2.5V HIGH) | Partial (Edge-triggered instead of level) |
| **INV Output** | Included (Revision 2013) | Correct |
| **Normalization** | Ch 2 (+10V), Ch 3 (+5V) | Correct |
| **Slew/Function** | Consistent shaping | **Diverged** (Uses Rampage model for slew) |
| **EOR/EOC** | Binary 0V/10V | Correct |

---

## 7. Recommendations & Action Items

1.  **Optimize `slopeWarpScale`**: Move this calculation out of the sample loop. Update it only when the shape control moves.
2.  **Unify Slew and Function Logic**: Replace `processRampageSlew` with a version that uses the same `slopeWarp` logic as the function generator to ensure consistent behavior across all modes.
3.  **Fix Cycle CV Input**: Change the logic to be level-sensitive rather than edge-triggered.
4.  **Refactor UI Assets**: Move `imageHandle` into the widget class to avoid global state issues.
5.  **Expand Context Menu**: Consider adding options to switch between Revision 2013 and "Classic" modes as suggested in the Codex.
