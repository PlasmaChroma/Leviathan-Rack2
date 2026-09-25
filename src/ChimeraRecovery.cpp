#include "ChimeraRecovery.hpp"
#include <jansson.h>
#include <system.hpp>
#include <cstdio>
#include <dirent.h>
#include <fstream>

namespace chimera { namespace recovery {
namespace {
std::string join(const std::string& a, const std::string& b) { return a + "/" + b; }
bool safeId(const std::string& id) {
    if (id.empty() || id.size() > 64) return false;
    for (char c : id)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') || c == '-')) return false;
    return true;
}
bool parseNumber(json_t* value, std::uint64_t& out) {
    const char* text = json_string_value(value);
    if (!text || !*text) return false;
    out = 0;
    for (const char* p = text; *p; ++p) {
        if (*p < '0' || *p > '9' || out > (UINT64_MAX - (*p - '0')) / 10) return false;
        out = out * 10 + (*p - '0');
    }
    return true;
}
Entry parseEntry(json_t* value) {
    Entry entry;
    if (!json_is_object(value)) return entry;
    const char* manifest = json_string_value(json_object_get(value, "manifest"));
    if (!manifest || !bundle::validManifestReference(manifest) ||
        !parseNumber(json_object_get(value, "capturedAtMs"), entry.capturedAtMs) ||
        !parseNumber(json_object_get(value, "documentRevision"), entry.documentRevision) ||
        !parseNumber(json_object_get(value, "audioRevision"), entry.audioRevision))
        return Entry{};
    entry.manifest = manifest;
    return entry;
}
json_t* encodeEntry(const Entry& entry) {
    if (!entry) return json_null();
    json_t* value = json_object();
    json_object_set_new(value, "manifest", json_string(entry.manifest.c_str()));
    json_object_set_new(value, "capturedAtMs", json_string(std::to_string(entry.capturedAtMs).c_str()));
    json_object_set_new(value, "documentRevision", json_string(std::to_string(entry.documentRevision).c_str()));
    json_object_set_new(value, "audioRevision", json_string(std::to_string(entry.audioRevision).c_str()));
    return value;
}
bool publish(const std::string& root, const Journal& journal) {
    const std::string temp = join(root, "journal.json.tmp");
    const std::string target = join(root, "journal.json");
    json_t* data = json_object();
    json_object_set_new(data, "schemaVersion", json_integer(1));
    json_object_set_new(data, "latest", encodeEntry(journal.latest));
    json_object_set_new(data, "preRecord", encodeEntry(journal.preRecord));
    FILE* file = std::fopen(temp.c_str(), "wb");
    if (!file) { json_decref(data); return false; }
    const bool written = json_dumpf(data, file, JSON_SORT_KEYS | JSON_INDENT(2)) == 0;
    const bool flushed = std::fflush(file) == 0;
    const bool closed = std::fclose(file) == 0;
    json_decref(data);
    if (!written || !flushed || !closed || !rack::system::rename(temp, target)) {
        std::remove(temp.c_str());
        return false;
    }
    return true;
}
void prune(const std::string& root, const Journal& journal) {
    const std::string directory = join(root, "chimera");
    DIR* entries = opendir(directory.c_str());
    if (!entries) return;
    while (dirent* item = readdir(entries)) {
        const std::string name(item->d_name);
        if (name.compare(0, 5, "reel-")) continue;
        const std::size_t dot = name.rfind('.');
        if (dot == std::string::npos ||
            (name.substr(dot) != ".json" && name.substr(dot) != ".wav")) continue;
        const std::string id = name.substr(5, dot - 5);
        if (!safeId(id)) continue;
        const std::string manifest = "chimera/reel-" + id + ".json";
        if (manifest == journal.latest.manifest || manifest == journal.preRecord.manifest)
            continue;
        const std::string path = join(directory, name);
        // Only remove regular files in this private cache. A symlink can be
        // left for explicit inspection rather than followed or traversed.
        if (rack::system::isFile(path) && !rack::system::isDirectory(path))
            std::remove(path.c_str());
    }
    closedir(entries);
}
} // namespace

Journal inspect(const std::string& root) {
    Journal journal;
    json_error_t error;
    json_t* data = json_load_file(join(root, "journal.json").c_str(),
                                  JSON_REJECT_DUPLICATES, &error);
    if (!data) return journal;
    if (json_is_object(data) && json_integer_value(json_object_get(data, "schemaVersion")) == 1) {
        journal.latest = parseEntry(json_object_get(data, "latest"));
        journal.preRecord = parseEntry(json_object_get(data, "preRecord"));
    }
    json_decref(data);
    return journal;
}

CommitResult commit(const std::string& root, const std::string& id,
                    const Reel& frozen, Role role, std::uint64_t wallTimeMs) {
    CommitResult result;
    if (!safeId(id) || !wallTimeMs) { result.error = "invalid_recovery_id"; return result; }
    if (!rack::system::isDirectory(root) && !rack::system::createDirectories(root)) {
        result.error = "recovery_directory_unavailable"; return result;
    }
    const std::string directory = join(root, "chimera");
    if (!rack::system::isDirectory(directory) && !rack::system::createDirectories(directory)) {
        result.error = "recovery_directory_unavailable"; return result;
    }
    const bundle::CommitResult stored = bundle::commit(root, id, frozen);
    if (!stored) { result.error = stored.error; return result; }
    Entry entry;
    entry.manifest = stored.manifest;
    entry.capturedAtMs = wallTimeMs;
    entry.documentRevision = stored.documentRevision;
    entry.audioRevision = stored.audioRevision;
    Journal journal = inspect(root);
    if (role == PreRecord) journal.preRecord = entry;
    else journal.latest = entry;
    if (!publish(root, journal)) { result.error = "recovery_journal_commit_failed"; return result; }
    prune(root, journal);
    result.entry = entry;
    return result;
}

bundle::LoadResult load(const std::string& root, const Entry& entry,
                        std::uint32_t capacityPages) {
    if (!entry || !bundle::validManifestReference(entry.manifest)) {
        bundle::LoadResult failed; failed.error = "recovery_entry_missing"; return failed;
    }
    bundle::LoadResult result = bundle::load(root, entry.manifest, capacityPages);
    if (result && (result.documentRevision != entry.documentRevision ||
                   result.audioRevision != entry.audioRevision)) {
        result.reel.reset(); result.error = "recovery_revision_mismatch";
    }
    return result;
}

} } // namespace chimera::recovery
