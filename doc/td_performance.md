# Temporal Deck & TD.Scope Performance Analysis

## Overview
Performance degradation is most significant in **LIVE mode**. While Sample mode performs relatively well due to static buffer indexing, LIVE mode suffers from high-frequency DSP calculations in the audio thread and heavy geometry rebuilding in the UI thread.

## 1. DSP Thread Bottlenecks (`TemporalDeck.cpp`)

### **A. Double-Phase Bin Calculation**
In `evaluateScopeBinAtIndex`, LIVE mode performs a second "phase pass" when decimating (`scopeStride > 1`) to reduce "dancing peaks."
*   **Impact:** This effectively doubles the CPU cost per bin in the audio thread.
*   **Recommendation:** Implement **SIMD-accelerated min/max searches** (SSE/AVX) for the bin accumulation loop. Since the buffer is a contiguous array of floats, we can process 4 or 8 samples per iteration.

### **B. Fixed Evaluation Budget**
`kScopeEvaluationBudgetPerPublish` is fixed at 16,384 samples.
*   **Impact:** On high-sample-rate systems (96kHz+), this budget is exhausted quickly, leading to low-resolution previews.
*   **Recommendation:** Scale this budget dynamically based on the current engine sample rate and whether Stereo mode is active.

### **C. High-Frequency Expander Publishing**
The expander publishes at 60Hz.
*   **Impact:** 1024 bins (Mono/Stereo) are `memcpy`'d across the expander and processed by the UI at a very high rate.
*   **Recommendation:** Reduce the default `SCOPE_BIN_COUNT` from 1024 to 512. For a 6HP module, 1024 bins is far beyond the pixel density of the display widget.

---

## 2. UI Thread Bottlenecks (`TDScope.cpp`)

### **A. Expensive Autoscale Logic**
The `TDScopeDisplayWidget::draw` function performs statistical analysis on the incoming signal to handle "Auto" range mode.
*   **Impact:** It uses `std::nth_element` on a `std::vector<float>` allocated **every frame** on the UI thread to find the 99th percentile (p99). This is a massive hit to the UI thread.
*   **Recommendation:** Move p99 and Peak-Hold calculations to the **DSP thread** within `TemporalDeck`. Send these values as pre-computed fields in the `HostToDisplay` message.

### **B. Geometry Overdraw (O(Pixels) Complexity)**
The current drawing logic iterates over every vertical pixel row (`rowCount`) to rebuild the scope lane.
*   **Impact:** On high-DPI displays or large window sizes, this results in 800+ iterations per lane, per frame.
*   **Recommendation:** Decimate the drawing rows. Iterate over the **bins** ($O(N)$ where $N=512$) and map them to pixel coordinates, rather than iterating over every pixel and searching for bins.

### **C. NanoVG Path Batching Overload**
The drawing logic uses **three passes** (main, boost, connect) across **16 intensity buckets**.
*   **Impact:** This results in tens of thousands of `nvgMoveTo`/`nvgLineTo` calls per frame.
*   **Recommendation:** 
    *   Consolidate the "main" and "boost" passes into a single path.
    *   Implement an **FBO (Framebuffer Object)** cache. Only redraw the scope texture when `publishSeq` changes. If the UI is running at 144Hz but the DSP only publishes at 60Hz, we are wasting 84 frames of redundant geometry rebuilding.

---

## 3. Recommended Optimization Roadmap

| Priority | Task | Target Thread |
| :--- | :--- | :--- |
| **Critical** | Move Autoscale (p99/Peak) to DSP | UI (Remove `std::nth_element`) |
| **High** | Decimate UI Rows (Bin-centric drawing) | UI (Reduce $O(N)$ iterations) |
| **High** | SIMD Min/Max Search for Bins | DSP (Reduce Audio Thread load) |
| **Medium** | Reduce `SCOPE_BIN_COUNT` to 512 | DSP/UI (Reduce Memcpy/Draw calls) |
| **Medium** | FBO Caching for Scope Display | UI (Bypass NanoVG on idle frames) |
