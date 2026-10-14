#pragma once

#include "ChimeraProfile.hpp"
#include <cstdint>

namespace chimera {

// Selection request arbitration only. The audio slice decides when a request
// commits, so a secondary musical voice can never change the active Splice.
class Selection {
public:
    void reset() {
        count_ = organizeBin_ = requested_ = 0;
        lastOrganize_ = 0.f;
        initialized_ = false;
    }
    void setRequested(std::uint16_t index, std::uint16_t count) {
        if (!count || index >= count) return;
        if (count != count_) organizeBin_ = bin(lastOrganize_, count);
        count_ = count;
        initialized_ = true;
        requested_ = index;
    }
    std::uint16_t organizeBin() const { return organizeBin_; }
    std::uint16_t requested() const { return requested_; }

    void observe(float organize, std::uint16_t count, bool shift) {
        if (!count) { reset(); return; }
        lastOrganize_ = organize;
        const bool countChanged = count != count_;
        if (countChanged) {
            count_ = count;
            if (organizeBin_ >= count) organizeBin_ = count - 1;
            if (requested_ >= count) requested_ = count - 1;
        }
        const double u = profile1::clamp01(organize);
        const std::uint16_t raw = bin(organize, count);
        if (!initialized_) {
            organizeBin_ = requested_ = raw;
            initialized_ = true;
        }
        else if (countChanged) organizeBin_ = raw;
        else if (raw > organizeBin_) {
            if (raw > organizeBin_ + 1 ||
                u >= (organizeBin_ + 1.1) / count) {
                organizeBin_ = raw;
                requested_ = raw;
            }
        }
        else if (raw < organizeBin_) {
            if (raw + 1 < organizeBin_ ||
                u <= (organizeBin_ - 0.1) / count) {
                organizeBin_ = raw;
                requested_ = raw;
            }
        }
        if (shift) requested_ = static_cast<std::uint16_t>((requested_ + 1) % count);
    }

private:
    static std::uint16_t bin(float organize, std::uint16_t count) {
        const double u = profile1::clamp01(organize);
        return u >= 1.0 ? count - 1 : static_cast<std::uint16_t>(u * count);
    }
    std::uint16_t count_ = 0, organizeBin_ = 0, requested_ = 0;
    float lastOrganize_ = 0.f;
    bool initialized_ = false;
};

} // namespace chimera
