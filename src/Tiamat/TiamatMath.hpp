#pragma once

namespace tiamat {

// The offline float32 oracle evaluates multiply-add in binary64 and rounds
// once to float. Use the same expression for the bounded engine recurrences.
// MinGW's current fmaf returned the wrong adjacent float for the regression
// vector in tiamat_buffer_spec; do not delegate event-sensitive state to it.
// Local -ffp-contract=off keeps evaluation consistent across build targets.
inline float multiplyAdd(float a, float b, float c) noexcept {
    return float(double(a) * double(b) + double(c));
}

} // namespace tiamat
