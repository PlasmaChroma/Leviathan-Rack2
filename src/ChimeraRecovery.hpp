#pragma once

#include "ChimeraBundle.hpp"
#include <cstdint>
#include <string>

namespace chimera { namespace recovery {

enum Role { Latest, PreRecord };
struct Entry {
    std::string manifest;
    std::uint64_t capturedAtMs = 0;
    std::uint64_t documentRevision = 0;
    std::uint64_t audioRevision = 0;
    explicit operator bool() const { return !manifest.empty(); }
};
struct Journal { Entry latest, preRecord; };
struct CommitResult {
    Entry entry;
    std::string error;
    explicit operator bool() const { return bool(entry) && error.empty(); }
};

// root is a private per-patch/per-module cache directory. A journal rename is
// the publication boundary; a crash leaves either the old or the new entry.
Journal inspect(const std::string& root);
CommitResult commit(const std::string& root, const std::string& id,
                    const Reel& frozen, Role role, std::uint64_t wallTimeMs);
bundle::LoadResult load(const std::string& root, const Entry& entry,
                        std::uint32_t capacityPages = kMaxPages);

} } // namespace chimera::recovery
