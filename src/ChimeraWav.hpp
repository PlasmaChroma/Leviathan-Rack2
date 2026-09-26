#pragma once

#include "ChimeraReel.hpp"
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace chimera { namespace wav {

struct ImportResult {
    std::unique_ptr<Reel> reel;
    std::string error;
    std::vector<std::string> warnings;
    std::uint32_t sourceRate = 0;
    std::uint32_t sourceFrames = 0;
    std::uint32_t nonfiniteSamples = 0;
    explicit operator bool() const { return bool(reel); }
};

// The caller owns file selection, overwrite policy and atomic rename. A ready
// snapshot lease must outlive this call; only the frozen pages are read.
bool writeCanonical(std::ostream& out, const Reel& reel, std::string& error, unsigned snapshot = 0);

// Strict import accepts only canonical 48 kHz stereo float32. The caller
// chooses prepared capacity; production import uses the full kMaxPages.
ImportResult readStrict(std::istream& in, std::uint32_t capacityPages = kMaxPages);

// Offline import of ordinary PCM/IEEE-float WAVs. A long source is rejected
// unless the caller explicitly requests truncation to the prepared Reel.
ImportResult readConvenience(std::istream& in, bool truncate = false,
                             std::uint32_t capacityPages = kMaxPages);

} } // namespace chimera::wav
