#pragma once

#include "ChimeraReel.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace chimera { namespace edit {

enum Kind { EraseSplice, DeleteSplice, ClearReel, MoveMarker, RemoveMarker };
struct Request {
    Kind kind = EraseSplice;
    std::uint16_t splice = 0;
    std::uint32_t frame = 0; // New frame for MoveMarker.
};
struct Result {
    std::unique_ptr<Reel> reel;
    std::string error;
    explicit operator bool() const { return bool(reel) && error.empty(); }
};

// Worker-only: source is an immutable, leased Reel snapshot. No operation
// mutates it or the active core. A replacement is adopted only after a core
// revision fence confirms the source has not changed meanwhile.
Result build(const Reel& source, const Request& request,
             std::uint32_t capacityPages = kMaxPages,
             std::uint32_t reservePages = kMaxPages);

} } // namespace chimera::edit
