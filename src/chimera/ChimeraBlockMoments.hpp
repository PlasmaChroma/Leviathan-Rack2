#pragma once
#include "ChimeraProfile.hpp"
#include <memory>
#include <cstdint>

namespace chimera {
// Core-owned live polynomial sums. No snapshot copies: snapshots contain raw
// audio only. A bounded refresh prevents drift under repeated overdubbing.
class BlockMoments {
public:
    struct Block {
        float left[4]{}, right[4]{};
        std::uint16_t invalid = 0;
        std::uint8_t updates = 0;
    };
    explicit BlockMoments(unsigned frames) : frames_(frames),
        count_(frames/16 + frames/64 + frames/256), blocks_(new Block[count_]) {}
    std::uint64_t bytes() const { return std::uint64_t(count_) * sizeof(Block); }
    const Block& get(unsigned frame, unsigned size) const { return blocks_[index(frame, size)]; }
    // Off-audio initialization after sequential import into a fresh Reel.
    // Build each block once, instead of updating three moment levels for every
    // decoded frame and repeatedly refreshing them along the way.
    template<class Read>
    void buildImported(unsigned validFrames, Read read) {
        for (unsigned size : {16u, 64u, 256u}) {
            double basis[256][4];
            for (unsigned i = 0; i < size; ++i) {
                const double x = (2.0*i - (size-1)) / (size-1);
                basis[i][0] = 1;
                for (unsigned j = 1; j < 4; ++j) basis[i][j] = basis[i][j-1]*x;
            }
            for (unsigned begin = 0; begin < validFrames; begin += size) {
                double left[4]{}, right[4]{};
                unsigned invalid = 0;
                for (unsigned i = 0; i < size && begin+i < validFrames; ++i) {
                    const auto value = read(begin+i);
                    invalid += !profile1::finite(value.l) || !profile1::finite(value.r);
                    const double l = bounded(value.l), r = bounded(value.r);
                    for (unsigned j = 0; j < 4; ++j) {
                        left[j] += l*basis[i][j]; right[j] += r*basis[i][j];
                    }
                }
                Block& block = blocks_[index(begin, size)];
                for (unsigned j = 0; j < 4; ++j) {
                    block.left[j] = float(left[j]); block.right[j] = float(right[j]);
                }
                block.invalid = invalid;
                block.updates = 0;
            }
        }
    }
    template<class Read>
    void update(unsigned frame, StereoFrame old, StereoFrame value, Read read) {
        const bool oldInvalid = !profile1::finite(old.l) || !profile1::finite(old.r);
        const bool newInvalid = !profile1::finite(value.l) || !profile1::finite(value.r);
        const double dl = bounded(value.l)-bounded(old.l), dr = bounded(value.r)-bounded(old.r);
        if (!dl && !dr && oldInvalid == newInvalid) return;
        for (unsigned size : {16u, 64u, 256u}) {
            Block& block = blocks_[index(frame, size)];
            block.invalid = std::uint16_t(int(block.invalid) + int(newInvalid) - int(oldInvalid));
            if (++block.updates == 0) {
                double left[4]{}, right[4]{};
                const unsigned begin = frame / size * size;
                for (unsigned i = 0; i < size; ++i) {
                    const auto source = read(begin+i);
                    const double x = (2.0*i - (size-1)) / (size-1);
                    double power = 1;
                    for (unsigned j = 0; j < 4; ++j, power *= x) {
                        left[j] += bounded(source.l)*power;
                        right[j] += bounded(source.r)*power;
                    }
                }
                for (unsigned j = 0; j < 4; ++j) {
                    block.left[j] = float(left[j]); block.right[j] = float(right[j]);
                }
            }
            else {
                const double x = (2.0*(frame % size) - (size-1)) / (size-1);
                double power = 1;
                for (unsigned j = 0; j < 4; ++j, power *= x) {
                    block.left[j] += float(dl*power); block.right[j] += float(dr*power);
                }
            }
        }
    }
private:
    static double bounded(float value) { return profile1::clamp(profile1::audio(value), -64.0, 64.0); }
    unsigned index(unsigned frame, unsigned size) const {
        return (size == 16 ? 0 : size == 64 ? frames_/16 : frames_/16 + frames_/64) + frame/size;
    }
    unsigned frames_, count_;
    std::unique_ptr<Block[]> blocks_;
};
} // namespace chimera
