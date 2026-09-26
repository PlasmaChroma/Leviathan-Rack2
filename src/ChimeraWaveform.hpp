#pragma once

#include "ChimeraReel.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>

namespace chimera {

// Immutable, worker-built display data. UI drawing never visits Reel pages.
struct WaveformSummary {
    static constexpr std::size_t kBins = 128;
    std::array<float, kBins> leftLow{};
    std::array<float, kBins> leftHigh{};
    std::array<float, kBins> rightLow{};
    std::array<float, kBins> rightHigh{};
    std::array<std::uint32_t, kMaxSplices> markers{};
    std::uint32_t frames = 0;
    std::uint16_t markerCount = 0;
    std::uint64_t documentRevision = 0;
    std::uint64_t audioRevision = 0;
    float peak = 0.f;
    bool stereo = false;

    static std::shared_ptr<const WaveformSummary> fromActive(const Reel& reel) {
        std::shared_ptr<WaveformSummary> result(new WaveformSummary);
        result->frames = reel.validFrames();
        result->markerCount = reel.markerCount();
        result->documentRevision = reel.documentRevision();
        result->audioRevision = reel.audioRevision();
        for (std::uint16_t i = 0; i < result->markerCount; ++i)
            result->markers[i] = reel.region(i).begin;
        result->fill([&](std::uint32_t frame) { return reel.readActive(frame); });
        return result;
    }

    static std::shared_ptr<const WaveformSummary> fromSnapshot(const Reel& reel) {
        const SnapshotMetadata metadata = reel.snapshotMetadata();
        std::shared_ptr<WaveformSummary> result(new WaveformSummary);
        result->frames = metadata.validFrames;
        result->markerCount = metadata.markerCount;
        result->documentRevision = metadata.documentRevision;
        result->audioRevision = metadata.audioRevision;
        for (std::uint16_t i = 0; i < result->markerCount; ++i)
            result->markers[i] = metadata.markers[i].frame;
        result->fill([&](std::uint32_t frame) { return reel.readSnapshot(frame); });
        return result;
    }

private:
    template <typename Reader>
    void fill(Reader read) {
        if (!frames) return;
        for (std::uint32_t i = 0; i < frames; ++i) {
            const StereoFrame sample = read(i);
            const std::size_t bin = std::size_t((std::uint64_t(i) * kBins) / frames);
            if (!std::isfinite(sample.l) || !std::isfinite(sample.r)) continue;
            leftLow[bin] = std::min(leftLow[bin], sample.l);
            leftHigh[bin] = std::max(leftHigh[bin], sample.l);
            rightLow[bin] = std::min(rightLow[bin], sample.r);
            rightHigh[bin] = std::max(rightHigh[bin], sample.r);
            stereo |= sample.l != sample.r;
            peak = std::max(peak, std::max(std::fabs(sample.l), std::fabs(sample.r)));
        }
    }
};

} // namespace chimera
