#include "ChimeraBundle.hpp"
#include <jansson.h>
#include <system.hpp>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace chimera { namespace bundle {
namespace {
std::string join(const std::string& parent, const std::string& leaf) {
    return parent + "/" + leaf;
}
bool safeId(const std::string& id) {
    if (id.empty() || id.size() > 64) return false;
    for (char c : id)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') || c == '-')) return false;
    return true;
}
bool exists(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    return file.good();
}
bool inside(const std::string& parent, const std::string& child) {
    try {
        const std::string base = rack::system::getCanonical(parent);
        const std::string target = rack::system::getCanonical(child);
        return target.size() > base.size() &&
            target.compare(0, base.size(), base) == 0 &&
            (target[base.size()] == '/' || target[base.size()] == '\\');
    }
    catch (...) { return false; }
}
std::string hashFile(const std::string& path) {
    std::ifstream in(path.c_str(), std::ios::binary);
    if (!in) return {};
    std::uint64_t hash = UINT64_C(14695981039346656037);
    char bytes[65536];
    while (in) {
        in.read(bytes, sizeof(bytes));
        const std::streamsize count = in.gcount();
        for (std::streamsize i = 0; i < count; ++i) {
            hash ^= std::uint8_t(bytes[i]);
            hash *= UINT64_C(1099511628211);
        }
    }
    if (!in.eof()) return {};
    std::ostringstream text;
    text << "fnv1a64-wav-v1:" << std::hex << std::setfill('0') << std::setw(16) << hash;
    return text.str();
}
bool parseRevision(json_t* root, const char* key, std::uint64_t& revision) {
    const char* text = json_string_value(json_object_get(root, key));
    if (!text || !*text) return false;
    revision = 0;
    for (const char* p = text; *p; ++p) {
        if (*p < '0' || *p > '9' ||
            revision > (UINT64_MAX - std::uint64_t(*p - '0')) / 10) return false;
        revision = revision * 10 + std::uint64_t(*p - '0');
    }
    return true;
}
CommitResult failed(const char* error) {
    CommitResult result; result.error = error; return result;
}
LoadResult loadFailed(const char* error) {
    LoadResult result; result.error = error; return result;
}
} // namespace

bool validManifestReference(const std::string& relativeManifest) {
    const std::string prefix = "chimera/reel-";
    const std::string suffix = ".json";
    if (relativeManifest.compare(0, prefix.size(), prefix) ||
        relativeManifest.size() <= prefix.size() + suffix.size() ||
        relativeManifest.compare(relativeManifest.size() - suffix.size(),
                                 suffix.size(), suffix)) return false;
    return safeId(relativeManifest.substr(prefix.size(),
        relativeManifest.size() - prefix.size() - suffix.size()));
}

bool pruneObsolete(const std::string& moduleRoot, const std::string& keepManifest) {
    if (!validManifestReference(keepManifest)) return false;
    const std::string directory = join(moduleRoot, "chimera");
    if (!inside(moduleRoot, directory)) return false;
    DIR* entries = opendir(directory.c_str());
    if (!entries) return false;
    const std::string keepId = keepManifest.substr(13, keepManifest.size() - 18);
    bool ok = true;
    while (dirent* entry = readdir(entries)) {
        const std::string name(entry->d_name);
        if (name.compare(0, 5, "reel-")) continue;
        const std::size_t dot = name.rfind('.');
        if (dot == std::string::npos ||
            (name.substr(dot) != ".json" && name.substr(dot) != ".wav")) continue;
        const std::string id = name.substr(5, dot - 5);
        if (!safeId(id) || id == keepId) continue;
        const std::string path = join(directory, name);
        if (!inside(directory, path) || std::remove(path.c_str())) ok = false;
    }
    if (closedir(entries)) ok = false;
    return ok;
}

static CommitResult commitImpl(const std::string& moduleRoot, const std::string& bundleId,
                    const Reel& frozen, bool injectManifestFailure, bool flat) {
    if (!safeId(bundleId)) return failed("invalid_bundle_id");
    if (!frozen.readyForWorker()) return failed("snapshot_not_ready");
    const SnapshotMetadata& metadata = frozen.snapshotMetadata();
    const std::string directory = flat ? moduleRoot : join(moduleRoot, "chimera");
    if (!flat && !inside(moduleRoot, directory)) return failed("storage_directory_escape");
    const std::string audioName = "reel-" + bundleId + ".wav";
    const std::string manifestName = "reel-" + bundleId + ".json";
    const std::string audioPath = join(directory, audioName);
    const std::string manifestPath = join(directory, manifestName);
    const std::string audioTemp = audioPath + ".tmp";
    const std::string manifestTemp = manifestPath + ".tmp";
    if (exists(audioPath) || exists(manifestPath)) return failed("bundle_id_exists");
    // Until the manifest commits, all new files belong to this attempt.
    // Clean them on normal failures and exception unwinding alike.
    struct PendingFiles {
        const std::string& audio;
        const std::string& audioTemp;
        const std::string& manifestTemp;
        bool audioCreated = false;
        bool committed = false;
        PendingFiles(const std::string& a, const std::string& at, const std::string& mt)
            : audio(a), audioTemp(at), manifestTemp(mt) {}
        ~PendingFiles() {
            if (committed) return;
            std::remove(audioTemp.c_str());
            std::remove(manifestTemp.c_str());
            if (audioCreated) std::remove(audio.c_str());
        }
    } pending(audioPath, audioTemp, manifestTemp);
    std::string digest;
    if (metadata.validFrames) {
        std::ofstream file(audioTemp.c_str(), std::ios::binary | std::ios::trunc);
        if (!file) return failed("audio_open_failed");
        std::string wavError;
        const bool written = wav::writeCanonical(file, frozen, wavError);
        file.flush();
        const bool flushed = bool(file);
        file.close();
        if (!written || !flushed || !file) {
            std::remove(audioTemp.c_str());
            return failed(wavError.empty() ? "audio_write_failed" : wavError.c_str());
        }
        digest = hashFile(audioTemp);
        if (digest.empty() || std::rename(audioTemp.c_str(), audioPath.c_str())) {
            std::remove(audioTemp.c_str());
            return failed("audio_commit_failed");
        }
        pending.audioCreated = true;
    }
    if (injectManifestFailure) return failed("injected_manifest_failure");
    json_t* root = json_object();
    if (!root) return failed("manifest_allocation_failed");
    json_object_set_new(root, "schemaVersion", json_integer(1));
    json_object_set_new(root, "cacheFormatVersion", json_integer(1));
    json_object_set_new(root, "audio", metadata.validFrames ?
        json_string(audioName.c_str()) : json_null());
    json_object_set_new(root, "hash", metadata.validFrames ?
        json_string(digest.c_str()) : json_null());
    json_object_set_new(root, "validFrames", json_integer(metadata.validFrames));
    json_object_set_new(root, "documentRevision",
        json_string(std::to_string(metadata.documentRevision).c_str()));
    json_object_set_new(root, "audioRevision",
        json_string(std::to_string(metadata.audioRevision).c_str()));
    json_t* markers = json_array();
    for (std::uint16_t i = 0; i < metadata.markerCount; ++i) {
        json_t* marker = json_object();
        json_object_set_new(marker, "id", json_integer(metadata.markers[i].id));
        json_object_set_new(marker, "frame", json_integer(metadata.markers[i].frame));
        json_array_append_new(markers, marker);
    }
    json_object_set_new(root, "markers", markers);
    FILE* file = std::fopen(manifestTemp.c_str(), "wb");
    if (!file) { json_decref(root); return failed("manifest_open_failed"); }
    const bool written = json_dumpf(root, file, JSON_SORT_KEYS | JSON_INDENT(2)) == 0;
    const bool flushed = std::fflush(file) == 0;
    const bool closed = std::fclose(file) == 0;
    json_decref(root);
    if (!written || !flushed || !closed ||
        std::rename(manifestTemp.c_str(), manifestPath.c_str())) {
        std::remove(manifestTemp.c_str());
        return failed("manifest_commit_failed");
    }
    pending.committed = true;
    CommitResult result;
    result.manifest = "chimera/" + manifestName;
    result.validFrames = metadata.validFrames;
    result.documentRevision = metadata.documentRevision;
    result.audioRevision = metadata.audioRevision;
    return result;
}

CommitResult commit(const std::string& moduleRoot, const std::string& bundleId,
                    const Reel& frozen, bool injectManifestFailure) {
    return commitImpl(moduleRoot, bundleId, frozen, injectManifestFailure, false);
}
CommitResult stage(const std::string& directory, const std::string& bundleId, const Reel& frozen) {
    return commitImpl(directory, bundleId, frozen, false, true);
}
CommitResult publishStaged(const std::string& directory, const std::string& moduleRoot,
                          const CommitResult& staged) {
    if (!staged || !validManifestReference(staged.manifest)) return failed("invalid_staged_bundle");
    const std::string destination = join(moduleRoot, "chimera");
    if ((!rack::system::createDirectories(destination) && !rack::system::isDirectory(destination)) ||
        !inside(moduleRoot, destination)) return failed("storage_directory_unavailable");
    const std::string name = staged.manifest.substr(8);
    const std::string audioName = name.substr(0, name.size()-5) + ".wav";
    const std::string manifest = join(destination, name), audio = join(destination, audioName);
    if (rack::system::exists(manifest) || rack::system::exists(audio)) return failed("bundle_id_exists");
    bool audioPublished = false;
    // Same-volume publication is a metadata-only rename. A cache on another
    // volume falls back to copy + temporary-file rename; neither path changes
    // the old selected manifest on failure.
    const auto publish = [&](const std::string& leaf, const std::string& target) {
        const std::string source = join(directory, leaf);
        if (std::rename(source.c_str(), target.c_str()) == 0) return true;
        const std::string temp = target + ".tmp";
        if (!rack::system::copy(source, temp) ||
            std::rename(temp.c_str(), target.c_str())) {
            std::remove(temp.c_str()); return false;
        }
        return true;
    };
    try {
        if (staged.validFrames) {
            if (!publish(audioName, audio)) return failed("staged_audio_publish_failed");
            audioPublished = true;
        }
        if (publish(name, manifest)) return staged;
    }
    catch (...) {}
    std::remove((manifest + ".tmp").c_str());
    std::remove((audio + ".tmp").c_str());
    if (audioPublished) std::remove(audio.c_str());
    return failed("staged_manifest_publish_failed");
}

LoadResult load(const std::string& moduleRoot, const std::string& relativeManifest,
                std::uint32_t capacityPages) {
    const std::string prefix = "chimera/reel-";
    const std::string suffix = ".json";
    if (!validManifestReference(relativeManifest))
        return loadFailed("invalid_manifest_path");
    const std::string id = relativeManifest.substr(prefix.size(),
        relativeManifest.size() - prefix.size() - suffix.size());
    const std::string directory = join(moduleRoot, "chimera");
    if (!inside(moduleRoot, directory) ||
        !inside(directory, join(directory, "reel-" + id + ".json")))
        return loadFailed("manifest_path_escape_or_missing");
    json_error_t parseError;
    json_t* root = json_load_file(join(directory, "reel-" + id + ".json").c_str(),
                                  JSON_REJECT_DUPLICATES, &parseError);
    if (!root) return loadFailed("manifest_missing_or_invalid");
    const json_int_t schema = json_integer_value(json_object_get(root, "schemaVersion"));
    const json_int_t cache = json_integer_value(json_object_get(root, "cacheFormatVersion"));
    std::uint64_t documentRevision = 0, audioRevision = 0;
    if (!json_is_object(root) || schema != 1 || cache != 1 ||
        !parseRevision(root, "documentRevision", documentRevision) ||
        !parseRevision(root, "audioRevision", audioRevision)) {
        json_decref(root); return loadFailed("unsupported_manifest_schema");
    }
    json_t* framesValue = json_object_get(root, "validFrames");
    if (!json_is_integer(framesValue) || json_integer_value(framesValue) < 0 ||
        json_integer_value(framesValue) > kMaxReelFrames) {
        json_decref(root); return loadFailed("invalid_manifest_length");
    }
    const std::uint32_t frames = std::uint32_t(json_integer_value(framesValue));
    json_t* audioValue = json_object_get(root, "audio");
    json_t* hashValue = json_object_get(root, "hash");
    if ((frames == 0 && (!json_is_null(audioValue) || !json_is_null(hashValue))) ||
        (frames != 0 && (!json_is_string(audioValue) || !json_is_string(hashValue)))) {
        json_decref(root); return loadFailed("invalid_manifest_audio");
    }
    LoadResult result;
    if (frames) {
        const std::string expected = "reel-" + id + ".wav";
        if (std::strcmp(json_string_value(audioValue), expected.c_str())) {
            json_decref(root); return loadFailed("invalid_audio_path");
        }
        const std::string path = join(directory, expected);
        if (!inside(directory, path)) {
            json_decref(root); return loadFailed("audio_path_escape_or_missing");
        }
        if (hashFile(path) != json_string_value(hashValue)) {
            json_decref(root); return loadFailed("audio_hash_mismatch_or_missing");
        }
        std::ifstream file(path.c_str(), std::ios::binary);
        wav::ImportResult imported = wav::readStrict(file, capacityPages);
        if (!imported || imported.sourceFrames != frames) {
            json_decref(root); return loadFailed("invalid_embedded_audio");
        }
        result.reel = std::move(imported.reel);
    }
    else {
        try { result.reel.reset(new Reel(capacityPages, capacityPages)); }
        catch (...) { json_decref(root); return loadFailed("reel_allocation_failed"); }
    }
    json_t* markers = json_object_get(root, "markers");
    if (!json_is_array(markers) || json_array_size(markers) != result.reel->markerCount()) {
        json_decref(root); return loadFailed("manifest_marker_mismatch");
    }
    for (std::size_t i = 0; i < json_array_size(markers); ++i) {
        json_t* marker = json_array_get(markers, i);
        json_t* idValue = json_object_get(marker, "id");
        json_t* frameValue = json_object_get(marker, "frame");
        if (!json_is_integer(idValue) || !json_is_integer(frameValue) ||
            json_integer_value(idValue) != result.reel->markerId(i) ||
            json_integer_value(frameValue) != result.reel->region(i).begin) {
            json_decref(root); return loadFailed("manifest_marker_mismatch");
        }
    }
    json_decref(root);
    result.reel->restoreRevisions(documentRevision, audioRevision);
    result.documentRevision = documentRevision;
    result.audioRevision = audioRevision;
    return result;
}

} } // namespace chimera::bundle
