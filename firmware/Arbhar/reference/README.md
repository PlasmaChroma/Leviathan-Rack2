# Selected native reference kernels

These are **small scalar C++17 reconstructions, not a complete arbhar engine or Rack plugin**. No manufacturer executable is loaded or run. Build and test:

```sh
cmake -S reference -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/test_kernels tables/follow_probe.csv
```

`test_kernels` accepts an optional second argument naming a float32 window-bank output file. Without that argument, it only reads the CSV and prints test results. Example: `./build/test_kernels tables/follow_probe.csv tables/reconstructed_window_bank.f32`.

## What is implemented

| Kernel | Evidence and important limitation |
|---|---|
| `rationalClip` | Static reconstruction of `tanh_approx~` at VA `0x2268`, cross-checked against `hv_tanh~.pd`. This is not `std::tanh`; its gain-one maximum is 14/13. |
| `cubic4` | One four-point branch from the grain perform routine. Caller must provide the original boundary policy; complete resampling and alias behavior are not reproduced. |
| `squaredControl`, duration and spray conversion | Recovered length/spray arithmetic after ADC calibration. These functions do not reproduce the physical input circuit, MIDI merge timing, or every panel mode. |
| `equalPowerMix` | Steady-state sine/cosine law from the main Pd graph. Does not reproduce the Pd oscillator lookup approximation or control filtering. |
| `followSpeed` | Algebraic expression checked against 12,288 cases from a restricted instruction-text interpreter for `calculateFollowSpeed`. Not a full Follow transport. |
| `makeWindowBank` | Static reconstruction of `makingWndArray`, 101 rows of 515 samples, using exact extracted template floats and explicit safe guards. Not executed against a booted ARM library. |
| `sampleWindowLinear` | **Recommended** safe lookup policy only; not recovered full grain-window stepping. |

`finiteOrZero`, normalized-input clamping, and out-of-bounds avoidance are **Rack safety choices**, not assertions about original firmware behavior. Header comments distinguish these choices from reconstructed arithmetic. Exact vendor-derived table bytes remain research evidence, not newly licensed assets.

## Validation scope

The optimized build and a separate AddressSanitizer/UndefinedBehaviorSanitizer build each passed **107,421 checks**. That count includes **12,288 Follow fixture comparisons**, so the two counts must not be added as independent firmware validations. The instruction-text probe separately reported no tolerance failures and maximum absolute error `2.182713028986427e-6` against the algebraic formula; the native float implementation's maximum fixture error was `1.90734863281e-6`.

Tests cover algebraic invariants, finite outputs, odd soft clipping, constant/DC interpolation, linear interpolation examples, control endpoints, equal-power mixing, window bounds/taper endpoints, and the Follow fixture. They do **not** certify original-instrument sound, scheduling, actual hardware timing, analog behavior, or all instruction-set corner cases. The interpreter implements a narrow finite-input subset with explicit failure on unsupported instructions; it is not a CPU emulator.

To rerun the sanitizer build using GCC or Clang:

```sh
c++ -std=c++17 -O1 -g -Wall -Wextra -Wpedantic -ffp-contract=off \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    reference/test_kernels.cpp -o build/test_kernels_sanitized
./build/test_kernels_sanitized tables/follow_probe.csv
```
