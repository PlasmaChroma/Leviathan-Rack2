#include "ChimeraWav.hpp"
#include "ChimeraWavReadBuffer.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>

namespace chimera { namespace wav {
namespace {

void put16(std::ostream& out, std::uint16_t v) {
    const char b[2] = {char(v), char(v >> 8)};
    out.write(b, 2);
}
void put32(std::ostream& out, std::uint32_t v) {
    const char b[4] = {char(v), char(v >> 8), char(v >> 16), char(v >> 24)};
    out.write(b, 4);
}
bool get16(std::istream& in, std::uint16_t& v) {
    unsigned char b[2];
    if (!in.read(reinterpret_cast<char*>(b), 2)) return false;
    v = std::uint16_t(b[0]) | (std::uint16_t(b[1]) << 8);
    return true;
}
template<class Reader>
bool get32(Reader& in, std::uint32_t& v) {
    unsigned char b[4];
    if (!in.read(reinterpret_cast<char*>(b), 4)) return false;
    v = std::uint32_t(b[0]) | (std::uint32_t(b[1]) << 8) |
        (std::uint32_t(b[2]) << 16) | (std::uint32_t(b[3]) << 24);
    return true;
}
bool tag(std::istream& in, const char* wanted) {
    char got[4];
    return bool(in.read(got, 4)) && std::memcmp(got, wanted, 4) == 0;
}
std::uint32_t floatBits(float value) {
    std::uint32_t bits;
    std::memcpy(&bits, &value, 4);
    return bits;
}
float fromBits(std::uint32_t bits) {
    float value;
    std::memcpy(&value, &bits, 4);
    return value;
}
bool seek(std::istream& in, std::uint64_t offset) {
    if (offset > std::uint64_t(std::numeric_limits<std::streamoff>::max())) return false;
    in.clear();
    in.seekg(std::streamoff(offset), std::ios::beg);
    return bool(in);
}
ImportResult failure(const char* message) {
    ImportResult result;
    result.error = message;
    return result;
}

} // namespace

bool writeCanonical(std::ostream& out, const Reel& reel, std::string& error) {
    if (!reel.readyForWorker()) { error = "snapshot_not_ready"; return false; }
    const SnapshotMetadata& meta = reel.snapshotMetadata();
    if (meta.validFrames > kMaxReelFrames || meta.markerCount > kMaxSplices) {
        error = "snapshot_bounds"; return false;
    }
    const std::uint64_t dataBytes = std::uint64_t(meta.validFrames) * 8;
    const std::uint64_t cueBytes = 4 + std::uint64_t(meta.markerCount) * 24;
    const std::uint64_t riffBytes = 4 + (8 + 18) + (8 + 4) +
                                    (8 + cueBytes) + (8 + dataBytes);
    if (riffBytes > UINT32_MAX) { error = "riff_too_large"; return false; }
    for (std::uint16_t i = 0; i < meta.markerCount; ++i) {
        const Marker& marker = meta.markers[i];
        if (!marker.id || marker.frame >= meta.validFrames ||
            (i && marker.frame <= meta.markers[i - 1].frame)) {
            error = "snapshot_markers"; return false;
        }
    }
    out.write("RIFF", 4); put32(out, std::uint32_t(riffBytes)); out.write("WAVE", 4);
    out.write("fmt ", 4); put32(out, 18);
    put16(out, 3); put16(out, 2); put32(out, 48000);
    put32(out, 384000); put16(out, 8); put16(out, 32); put16(out, 0);
    out.write("fact", 4); put32(out, 4); put32(out, meta.validFrames);
    out.write("cue ", 4); put32(out, std::uint32_t(cueBytes));
    put32(out, meta.markerCount);
    for (std::uint16_t i = 0; i < meta.markerCount; ++i) {
        const Marker& marker = meta.markers[i];
        put32(out, marker.id);
        put32(out, marker.frame);
        out.write("data", 4);
        put32(out, 0); put32(out, 0); put32(out, marker.frame);
    }
    out.write("data", 4); put32(out, std::uint32_t(dataBytes));
    // Encode a bounded chunk at a time, preserving canonical little-endian
    // bytes without two iostream calls for every stereo frame.
    const unsigned chunkFrames = 131072;
    std::unique_ptr<char[]> buffer(new char[chunkFrames * 8]);
    for (std::uint32_t begin = 0; begin < meta.validFrames;) {
        const unsigned count = std::min(chunkFrames, meta.validFrames-begin);
        for (unsigned i = 0; i < count; ++i) {
            const StereoFrame sample = reel.readSnapshot(begin+i);
            const std::uint32_t bits[2] = {floatBits(sample.l), floatBits(sample.r)};
            for (unsigned channel = 0; channel < 2; ++channel)
                for (unsigned byte = 0; byte < 4; ++byte)
                    buffer[i*8+channel*4+byte] = char(bits[channel] >> (byte*8));
        }
        out.write(buffer.get(), count*8);
        if (!out) { error = "write_failed"; return false; }
        begin += count;
    }
    if (!out) { error = "write_failed"; return false; }
    error.clear();
    return true;
}

ImportResult readStrict(std::istream& in, std::uint32_t capacityPages) {
    if (!capacityPages || capacityPages > kMaxPages) return failure("invalid_capacity");
    in.clear(); in.seekg(0, std::ios::end);
    const std::streamoff fileLength = in.tellg();
    if (fileLength < 12 || !seek(in, 0)) return failure("truncated_riff");
    if (!tag(in, "RIFF")) return failure("not_riff");
    std::uint32_t riffBytes = 0;
    if (!get32(in, riffBytes) || !tag(in, "WAVE")) return failure("not_wave");
    const std::uint64_t end = std::uint64_t(riffBytes) + 8;
    if (end < 12 || end > std::uint64_t(fileLength)) return failure("truncated_riff");
    bool haveFmt = false, haveData = false, haveFact = false;
    std::uint32_t factFrames = 0, dataBytes = 0;
    std::uint64_t labelBytes = 0;
    std::uint64_t dataOffset = 0;
    std::vector<Marker> cues;
    std::uint64_t cursor = 12;
    while (cursor + 8 <= end) {
        if (!seek(in, cursor)) return failure("seek_failed");
        char chunk[4];
        std::uint32_t size = 0;
        if (!in.read(chunk, 4) || !get32(in, size)) return failure("truncated_chunk");
        const std::uint64_t payload = cursor + 8;
        const std::uint64_t next = payload + std::uint64_t(size) + (size & 1u);
        if (next > end || next < payload) return failure("chunk_out_of_bounds");
        if (std::memcmp(chunk, "fmt ", 4) == 0) {
            if (haveFmt || size < 16 || size > 1048576) return failure("invalid_fmt");
            std::uint16_t format = 0, channels = 0, align = 0, bits = 0;
            std::uint32_t rate = 0, byteRate = 0;
            if (!get16(in, format) || !get16(in, channels) || !get32(in, rate) ||
                !get32(in, byteRate) || !get16(in, align) || !get16(in, bits))
                return failure("truncated_fmt");
            if (format != 3 || channels != 2 || rate != 48000 ||
                byteRate != 384000 || align != 8 || bits != 32)
                return failure("not_canonical_float32_stereo_48k");
            haveFmt = true;
        }
        else if (std::memcmp(chunk, "fact", 4) == 0) {
            if (haveFact || size < 4) return failure("invalid_fact");
            if (!get32(in, factFrames)) return failure("truncated_fact");
            haveFact = true;
        }
        else if (std::memcmp(chunk, "cue ", 4) == 0) {
            if (size < 4) return failure("invalid_cue");
            std::uint32_t count = 0;
            if (!get32(in, count) || std::uint64_t(count) * 24 + 4 != size ||
                count > 4096 || cues.size() + count > 4096)
                return failure("invalid_cue_count");
            for (std::uint32_t i = 0; i < count; ++i) {
                Marker marker{};
                std::uint32_t position = 0, chunkStart = 0, blockStart = 0;
                char fcc[4];
                if (!get32(in, marker.id) || !get32(in, position) ||
                    !in.read(fcc, 4) || !get32(in, chunkStart) ||
                    !get32(in, blockStart) || !get32(in, marker.frame))
                    return failure("truncated_cue");
                if (!marker.id || std::memcmp(fcc, "data", 4) ||
                    chunkStart || blockStart || position != marker.frame)
                    return failure("unsupported_cue_layout");
                cues.push_back(marker);
            }
        }
        else if (std::memcmp(chunk, "LIST", 4) == 0) {
            labelBytes += size;
            if (labelBytes > 1048576) return failure("label_metadata_too_large");
        }
        else if (std::memcmp(chunk, "data", 4) == 0) {
            if (haveData) return failure("multiple_data_chunks");
            dataOffset = payload; dataBytes = size; haveData = true;
        }
        cursor = next;
    }
    if (cursor != end) return failure("trailing_chunk_bytes");
    if (!haveFmt || !haveData) return failure("missing_fmt_or_data");
    if (dataBytes % 8) return failure("misaligned_data");
    const std::uint32_t frames = dataBytes / 8;
    if (frames > kMaxReelFrames || frames > capacityPages * kPageFrames)
        return failure("reel_too_long");
    if (haveFact && factFrames != frames) return failure("fact_length_mismatch");
    if (!frames && !cues.empty()) return failure("cues_on_empty_reel");
    ImportResult result;
    result.sourceRate = 48000;
    result.sourceFrames = frames;
    std::sort(cues.begin(), cues.end(), [](const Marker& a, const Marker& b) {
        return a.frame < b.frame || (a.frame == b.frame && a.id < b.id);
    });
    std::vector<Marker> markers;
    if (frames) {
        if (cues.empty() || cues.front().frame != 0) {
            std::uint32_t id = 1;
            for (;;) {
                bool used = false;
                for (const Marker& cue : cues) if (cue.id == id) { used = true; break; }
                if (!used) break;
                ++id;
                if (!id) return failure("cue_ids_exhausted");
            }
            markers.push_back({0, id});
        }
        for (const Marker& marker : cues) {
            if (marker.frame == frames) {
                result.warnings.push_back("end_sentinel_ignored");
                continue;
            }
            if (marker.frame > frames) return failure("cue_out_of_range");
            if (!markers.empty() && markers.back().frame == marker.frame) {
                if (marker.frame == 0) {
                    result.warnings.push_back("duplicate_zero_cue_ignored");
                    continue;
                }
                return failure("duplicate_cue_frame");
            }
            markers.push_back(marker);
        }
        if (markers.size() > kMaxSplices) return failure("too_many_splices");
        for (std::size_t i = 0; i < markers.size(); ++i)
            for (std::size_t j = 0; j < i; ++j)
                if (markers[i].id == markers[j].id) return failure("duplicate_cue_id");
    }
    try { result.reel.reset(new Reel(capacityPages, capacityPages)); }
    catch (...) { return failure("reel_allocation_failed"); }
    if (!seek(in, dataOffset)) return failure("seek_data_failed");
    SampleBuffer samples(in, dataBytes);
    for (std::uint32_t frame = 0; frame < frames; ++frame) {
        std::uint32_t lBits = 0, rBits = 0;
        if (!get32(samples, lBits) || !get32(samples, rBits)) return failure("truncated_data");
        float l = fromBits(lBits), r = fromBits(rBits);
        if (!std::isfinite(l)) { l = 0.f; ++result.nonfiniteSamples; }
        if (!std::isfinite(r)) { r = 0.f; ++result.nonfiniteSamples; }
        if (!result.reel->appendImported({l, r})) return failure("reel_write_failed");
    }
    result.reel->finishImport();
    if (result.nonfiniteSamples) result.warnings.push_back("nonfinite_samples_zeroed");
    if (frames && !result.reel->replaceMarkers(markers.data(),
            std::uint16_t(markers.size()))) return failure("invalid_cue_table");
    return result;
}

} } // namespace chimera::wav
