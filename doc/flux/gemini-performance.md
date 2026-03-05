# Performance Audit: IntegralFlux (Maths Emulation)

## 1. Executive Summary
Current CPU usage for the `IntegralFlux` module is approximately **5%** on the target system, compared to **1.1%** for similar modules like Befaco Rampage. The primary bottlenecks are frequent calls to expensive transcendental functions (`std::pow`, `std::tanh`) and redundant calculations within the sample-rate `process()` loop.

## 2. Identified Bottlenecks

### 2.1 Expensive Transcendental Functions (High Impact)
The code relies heavily on `std::pow` and `std::tanh` inside the `process()` loop.
- **`computeStageTime`**: Calls `std::pow` twice per call. Since it's called twice per channel (Rise and Fall), this results in **8 `std::pow` calls per sample**.
- **`computeShapeTimeScale`**: Calls `std::pow` to interpolate timing scales.
- **`softSatSym` / `softSatPos`**: Calls `std::tanh` for every bus output (SUM, OR, INV) every sample.
- **`slopeWarp`**: Calls `std::pow` inside the integration step.

### 2.2 Redundant Parameter Processing (Medium Impact)
Knob positions and many CV inputs do not change at audio rates, yet the following are recomputed every sample:
- **`computeStageTime`**: Re-calculates the entire timing law even if the knob hasn't moved.
- **`attenuverterGain`**: Simple linear math, but called every sample for 4 channels.
- **`shapeSignedFromKnob`**: Re-calculated every sample.

### 2.3 Branching and Clamping (Low/Medium Impact)
- Extensive use of `clamp` and `std::fmax`/`std::fmin` across the code prevents effective SIMD auto-vectorization and adds branch pressure.
- `processUnifiedShapedSlew` adds a separate path with its own math overhead when external signals are processed.

---

## 3. Comparison with Reference (Befaco Rampage)
The Befaco Rampage (1.1% CPU) likely employs the following techniques which are currently missing in `IntegralFlux`:
1.  **Approximations**: Uses `dsp::ApproxExp` or Look-Up Tables (LUTs) instead of `std::pow`.
2.  **Decimated Processing**: Updates timing constants every 8 or 16 samples instead of every sample.
3.  **SIMD**: Processes multiple channels or segments in parallel using SIMD instructions.

---

## 4. Optimization Recommendations

### 4.1 Short-Term (Immediate Gain)
1.  **Cache Timing Constants**: Only call `computeStageTime` when the corresponding knobs or CVs change significantly (or use a decimated timer to update every 16 samples).
2.  **Use `dsp::ApproxExp`**: Replace `std::pow(2.f, x)` with Rack's optimized approximation.
3.  **Fast Tanh**: Replace `std::tanh` in the mixing bus with a fast polynomial approximation or `dsp::ApproxTanh`.

### 4.2 Medium-Term (Structural Improvements)
1.  **Look-Up Tables (LUTs)**: Implement a LUT for the `slopeWarp` function. The curve is controlled by a single `shape` parameter, making it an ideal candidate for a 2D LUT (phase vs. shape).
2.  **Parameter Smoothing**: Use `dsp::ClockDivider` to process UI-related logic (knobs, light brightness) at a lower rate (e.g., 1/32 of the sample rate).

### 4.3 Long-Term (High Efficiency)
1.  **SIMD Vectorization**: Rewrite the core integration loop to use `simd::float_4` to process both channels 1 and 4 simultaneously.

## 5. Action Plan
1.  [ ] Implement `dsp::ClockDivider` for timing law updates.
2.  [ ] Replace `std::pow` with faster alternatives.
3.  [ ] Replace `std::tanh` with a polynomial approximation.
4.  [ ] Benchmark against the 5% baseline.
