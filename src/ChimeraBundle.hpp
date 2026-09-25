#pragma once

#include "ChimeraWav.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace chimera { namespace bundle {

struct CommitResult {
    std::string manifest; // Relative to the module's patch storage directory.
    std::string error;
    std::uint32_t validFrames = 0;
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
// Stage outside Rack's archive, then publish only a completed attempt on the
// saving thread. A timed-out worker never writes into patch storage.
CommitResult stage(const std::string& directory, const std::string& bundleId, const Reel& frozen);
CommitResult publishStaged(const std::string& directory, const std::string& moduleRoot,
                          const CommitResult& staged);
LoadResult load(const std::string& moduleRoot, const std::string& relativeManifest,
                std::uint32_t capacityPages = kMaxPages);

// Call only after a new bundle is committed and selected by the module.
// Removes obsolete module-owned revisions (including orphaned failed commits)
// so Rack does not archive every prior WAV on each save.
bool pruneObsolete(const std::string& moduleRoot, const std::string& keepManifest);

} } // namespace chimera::bundle
