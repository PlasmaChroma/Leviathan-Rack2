#pragma once

#include "ChimeraWav.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace chimera { namespace bundle {

struct CommitResult {
    std::string manifest; // Relative to the module's patch storage directory.
    std::string error;
    std::uint64_t documentRevision = 0;
    std::uint64_t audioRevision = 0;
    explicit operator bool() const { return error.empty() && !manifest.empty(); }
};

struct LoadResult {
    std::unique_ptr<Reel> reel;
    std::string error;
    std::uint64_t documentRevision = 0;
    std::uint64_t audioRevision = 0;
    explicit operator bool() const { return bool(reel) && error.empty(); }
};

bool validManifestReference(const std::string& relativeManifest);

// The caller has created moduleRoot/chimera and holds a ready snapshot lease.
// IDs are unique within the module directory; a failed commit never changes a
// prior manifest. Fault injection is for the native transaction test only.
CommitResult commit(const std::string& moduleRoot, const std::string& bundleId,
                    const Reel& frozen, bool injectManifestFailure = false);
LoadResult load(const std::string& moduleRoot, const std::string& relativeManifest,
                std::uint32_t capacityPages = kMaxPages);

} } // namespace chimera::bundle
