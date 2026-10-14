#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <istream>

namespace chimera { namespace wav {
// Worker-only buffer bounded by the validated WAV data chunk. Header parsing
// keeps its ordinary seeking stream; sample decoding never reads a later chunk.
class SampleBuffer {
public:
    SampleBuffer(std::istream& input, std::uint64_t bytes) : input_(input), remaining_(bytes) {}
    bool read(char* destination, unsigned bytes) {
        while (bytes) {
            if (cursor_ == available_) {
                available_ = unsigned(std::min<std::uint64_t>(remaining_, sizeof(buffer_)));
                cursor_ = 0;
                if (!available_ || !input_.read(buffer_, available_)) return false;
                remaining_ -= available_;
            }
            const unsigned count = std::min(bytes, available_-cursor_);
            std::memcpy(destination, buffer_+cursor_, count);
            cursor_ += count; destination += count; bytes -= count;
        }
        return true;
    }
private:
    std::istream& input_;
    std::uint64_t remaining_;
    unsigned cursor_ = 0, available_ = 0;
    char buffer_[65536];
};
} }
