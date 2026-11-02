# Shared math helpers

`src/common/MathHelpers.hpp` provides opt-in scalar DSP utilities. The shared
sine and tanh lookup tables are defined in `MathHelpers.cpp` and initialized at
library load, not on first audio use. Link that source when using either LUT.

Existing helper bodies and lookup tables are preserved. No module callers were
migrated in this update. `fastTanh`, `softClip` and `softLimit` retain the legacy
rational saturation curve; `tanhAudio` approximates mathematical tanh instead.

## Choosing an entry point

Use the existing bounded functions only when the caller guarantees their
header-documented domains. In particular, sine/triangle phases must be finite
and in [-2, 2], `wrap01Fast` only corrects one cycle, logarithm inputs must be
positive normal doubles, and fast exponents must be in [-1022, 1022].

New `*Checked` variants preserve fast-path arithmetic for valid inputs.
`exp2AudioChecked`, `exp2PitchChecked` and `log2AudioChecked` use standard-library
fallbacks outside those domains, including subnormals, infinity and NaN.
`powUnitAudioChecked` preserves the legacy nonpositive-base result of zero,
including 0^0; it is not a general replacement for `std::pow`. Other cases
outside its fast domain fall back to `std::pow`.

`sinCyclesAudioChecked` and `triangleCyclesAudioChecked` wrap arbitrary finite
float phases and return NaN for nonfinite phases without unsafe integer casts.
They cannot recover phase precision already lost in large float values.

Checked functions require strict floating-point compilation for their nonfinite
contracts. They add branches and may call libm; bounded functions remain the
choice for proven hot-path domains. No checked-path speedup is claimed.

`clampf` keeps its historical operand ordering, including signed-zero behavior.
With finite ordered bounds, NaN input returns the upper bound. `clamp01` returns
zero for NaN. Do not assume either has general `fmin`/`fmax` semantics.

## Validation

Run `make test-math-helpers`. The original suite locks saturation anchors and
LUT accuracy; the additional contract suite sweeps the exponent domain, checks
integer octaves, checked/legacy equivalence and exceptional inputs.

A GCC Linux strict-math run measured maximum relative exp2 error 2.985e-13,
absolute log2 error 6.822e-13 and pitch error 0.000163 cents. These are sampled
results, not exhaustive proofs. Undefined-behavior and float-cast-overflow
sanitizers passed the contract suite. Chimera's existing scalar-math suite also
passed, including its integer-frame timing bound.
