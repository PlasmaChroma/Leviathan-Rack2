#include "TiamatCore.hpp"
namespace tiamat {
void Core::processBlock(const float* input, float* output, const BufferBlockInput& block) noexcept {
    for (unsigned i = 0; i < input_.size(); ++i) input_[i] = finiteOrZero(input[i]);
    if (block.restartRandom) corrupt_.restartRandom(block.seed);
    buffer_.processBlock(input_.data(), wet_.data(), block);
    buffer_.processCorrupt(corrupt_, wet_.data(), wet_.data());
    output_.processBlock(input_.data(), wet_.data(), output, buffer_.mapped().mixTarget, buffer_.mapped().crossfeed);
}
} // namespace tiamat
