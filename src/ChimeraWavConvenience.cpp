#include "ChimeraWav.hpp"
#include <speex/speex_resampler.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <istream>
#include <limits>
#include <vector>

namespace chimera { namespace wav {
namespace {
bool read16(std::istream& in, std::uint16_t& value) {
    unsigned char b[2];
    if (!in.read(reinterpret_cast<char*>(b), 2)) return false;
    value = std::uint16_t(b[0]) | (std::uint16_t(b[1]) << 8);
    return true;
}
bool read32(std::istream& in, std::uint32_t& value) {
    unsigned char b[4];
    if (!in.read(reinterpret_cast<char*>(b), 4)) return false;
    value = std::uint32_t(b[0]) | (std::uint32_t(b[1]) << 8) |
            (std::uint32_t(b[2]) << 16) | (std::uint32_t(b[3]) << 24);
    return true;
}
bool at(std::istream& in, std::uint64_t offset) {
    if (offset > std::uint64_t(std::numeric_limits<std::streamoff>::max())) return false;
    in.clear(); in.seekg(std::streamoff(offset), std::ios::beg);
    return bool(in);
}
ImportResult fail(const char* error) {
    ImportResult result; result.error = error; return result;
}
struct Format {
    std::uint16_t code = 0, channels = 0, bits = 0, align = 0;
    std::uint32_t rate = 0;
};
bool sample(std::istream& in, const Format& format, float& out,
            std::uint32_t& nonfinite) {
    unsigned char b[8] = {};
    const unsigned bytes = format.bits / 8;
    if (!in.read(reinterpret_cast<char*>(b), bytes)) return false;
    std::uint64_t raw = 0;
    for (unsigned i = 0; i < bytes; ++i) raw |= std::uint64_t(b[i]) << (8 * i);
    double value = 0;
    if (format.code == 3) {
        if (format.bits == 32) {
            const std::uint32_t word = std::uint32_t(raw);
            float decoded; std::memcpy(&decoded, &word, 4); value = decoded;
        }
        else std::memcpy(&value, &raw, 8);
    }
    else if (format.bits == 16) value = double(std::int16_t(raw)) / 32768.0;
    else if (format.bits == 24) {
        const std::int32_t signed24 = std::int32_t(std::uint32_t(raw) << 8) >> 8;
        value = double(signed24) / 8388608.0;
    }
    else value = double(std::int32_t(raw)) / 2147483648.0;
    if (!std::isfinite(value) || std::fabs(value) > std::numeric_limits<float>::max()) {
        value = 0; ++nonfinite;
    }
    out = float(value);
    return true;
}
bool frame(std::istream& in, const Format& format, StereoFrame& out,
           std::uint32_t& nonfinite) {
    if (!sample(in, format, out.l, nonfinite)) return false;
    if (format.channels == 1) out.r = out.l;
    else if (!sample(in, format, out.r, nonfinite)) return false;
    return true;
}

// Worker-only, quality-10 bandlimited conversion. Extend endpoints so even
// very short constant clips retain their level. The prefix is an exact
// rational-rate period, so discarding it does not shift samples or cue times.
bool resample(std::istream& in, const Format& format, std::uint32_t sourceFrames,
              std::uint32_t frames, ImportResult& result) {
    if (!frames) return true;
    if (format.rate == kCoreRate) {
        for (std::uint32_t i = 0; i < frames; ++i) {
            StereoFrame value{};
            if (!frame(in, format, value, result.nonfiniteSamples)) return false;
            if (!result.reel->appendImported(value)) return false;
        }
        return true;
    }
    int error = 0;
    std::unique_ptr<SpeexResamplerState, decltype(&speex_resampler_destroy)> converter(
        speex_resampler_init(2, format.rate, kCoreRate, 10, &error), speex_resampler_destroy);
    if (!converter || error != RESAMPLER_ERR_SUCCESS) return false;
    if (speex_resampler_skip_zeros(converter.get()) != RESAMPLER_ERR_SUCCESS) return false;
    unsigned a = format.rate, b = kCoreRate;
    while (b) { const unsigned rest = a % b; a = b; b = rest; }
    const unsigned period = format.rate / a;
    const unsigned latency = unsigned(speex_resampler_get_input_latency(converter.get()));
    const unsigned prefix = ((latency + period - 1) / period) * period;
    const std::uint64_t discard = std::uint64_t(prefix) * kCoreRate / format.rate;
    StereoFrame current{};
    if (!frame(in, format, current, result.nonfiniteSamples)) return false;
    const StereoFrame first = current;
    std::uint64_t inputIndex = 0, outputIndex = 0;
    unsigned readFrames = 1, written = 0;
    float input[2048], output[2048];
    while (written < frames) {
        for (unsigned i = 0; i < 1024; ++i, ++inputIndex) {
            if (inputIndex > prefix && readFrames < sourceFrames) {
                if (!frame(in, format, current, result.nonfiniteSamples)) return false;
                ++readFrames;
            }
            const StereoFrame value = inputIndex < prefix ? first : current;
            // Filtering finite but extreme floats must not overflow its state.
            input[2*i] = std::max(-64.f, std::min(value.l, 64.f));
            input[2*i+1] = std::max(-64.f, std::min(value.r, 64.f));
        }
        unsigned offset = 0;
        while (offset < 1024 && written < frames) {
            spx_uint32_t consumed = 1024 - offset, produced = 1024;
            if (speex_resampler_process_interleaved_float(converter.get(), input + 2*offset,
                    &consumed, output, &produced) != RESAMPLER_ERR_SUCCESS ||
                (!consumed && !produced)) return false;
            offset += consumed;
            for (unsigned i = 0; i < produced && written < frames; ++i, ++outputIndex) {
                if (outputIndex < discard) continue;
                if (!result.reel->appendImported({output[2*i], output[2*i+1]})) return false;
                ++written;
            }
        }
    }
    return true;
}
} // namespace

ImportResult readConvenience(std::istream& in, bool truncate,
                             std::uint32_t capacityPages) {
    if (!capacityPages || capacityPages > kMaxPages) return fail("invalid_capacity");
    in.clear(); in.seekg(0, std::ios::end);
    const std::streamoff length = in.tellg();
    if (length < 12 || !at(in, 0)) return fail("truncated_riff");
    char riff[4], wave[4]; std::uint32_t riffSize = 0;
    if (!in.read(riff, 4) || !read32(in, riffSize) || !in.read(wave, 4) ||
        std::memcmp(riff, "RIFF", 4) || std::memcmp(wave, "WAVE", 4))
        return fail("not_riff_wave");
    const std::uint64_t end = std::uint64_t(riffSize) + 8;
    if (end > std::uint64_t(length) || end < 12) return fail("truncated_riff");
    Format format;
    bool haveFormat = false, haveData = false;
    std::uint64_t dataOffset = 0;
    std::uint32_t dataBytes = 0;
    std::uint64_t labelBytes = 0;
    std::vector<Marker> cues;
    std::uint64_t cursor = 12;
    while (cursor + 8 <= end) {
        if (!at(in, cursor)) return fail("seek_failed");
        char tag[4]; std::uint32_t bytes = 0;
        if (!in.read(tag, 4) || !read32(in, bytes)) return fail("truncated_chunk");
        const std::uint64_t payload = cursor + 8;
        const std::uint64_t next = payload + std::uint64_t(bytes) + (bytes & 1u);
        if (next > end || next < payload) return fail("chunk_out_of_bounds");
        if (!std::memcmp(tag, "fmt ", 4)) {
            if (haveFormat || bytes < 16 || bytes > 1048576) return fail("invalid_fmt");
            std::uint32_t byteRate = 0;
            if (!read16(in, format.code) || !read16(in, format.channels) ||
                !read32(in, format.rate) || !read32(in, byteRate) ||
                !read16(in, format.align) || !read16(in, format.bits))
                return fail("truncated_fmt");
            const bool pcm = format.code == 1 &&
                (format.bits == 16 || format.bits == 24 || format.bits == 32);
            const bool floating = format.code == 3 &&
                (format.bits == 32 || format.bits == 64);
            if ((!pcm && !floating) || (format.channels != 1 && format.channels != 2) ||
                format.rate < 8000 || format.rate > 384000 ||
                format.align != format.channels * (format.bits / 8) ||
                std::uint64_t(byteRate) != std::uint64_t(format.rate) * format.align)
                return fail("unsupported_wav_format");
            haveFormat = true;
        }
        else if (!std::memcmp(tag, "data", 4)) {
            if (haveData) return fail("multiple_data_chunks");
            haveData = true; dataOffset = payload; dataBytes = bytes;
        }
        else if (!std::memcmp(tag, "cue ", 4)) {
            std::uint32_t count = 0;
            if (bytes < 4 || !read32(in, count) ||
                std::uint64_t(count) * 24 + 4 != bytes || count > 4096 ||
                cues.size() + count > 4096) return fail("invalid_cue_count");
            for (std::uint32_t i = 0; i < count; ++i) {
                Marker cue{}; std::uint32_t position = 0, chunkStart = 0, blockStart = 0;
                char chunk[4];
                if (!read32(in, cue.id) || !read32(in, position) ||
                    !in.read(chunk, 4) || !read32(in, chunkStart) ||
                    !read32(in, blockStart) || !read32(in, cue.frame))
                    return fail("truncated_cue");
                if (!cue.id || std::memcmp(chunk, "data", 4) ||
                    chunkStart || blockStart || position != cue.frame)
                    return fail("unsupported_cue_layout");
                cues.push_back(cue);
            }
        }
        else if (!std::memcmp(tag, "LIST", 4)) {
            labelBytes += bytes;
            if (labelBytes > 1048576) return fail("label_metadata_too_large");
        }
        cursor = next;
    }
    if (cursor != end || !haveFormat || !haveData || dataBytes % format.align)
        return fail("invalid_wave_layout");
    const std::uint32_t sourceFrames = dataBytes / format.align;
    const std::uint64_t converted =
        (std::uint64_t(sourceFrames) * 48000 + format.rate - 1) / format.rate;
    const std::uint32_t maxFrames = std::min<std::uint32_t>(
        kMaxReelFrames, capacityPages * kPageFrames);
    if (converted > maxFrames && !truncate) {
        ImportResult tooLong = fail("reel_too_long");
        tooLong.sourceRate = format.rate;
        tooLong.sourceFrames = sourceFrames;
        return tooLong;
    }
    const std::uint32_t frames = std::uint32_t(std::min<std::uint64_t>(converted, maxFrames));
    ImportResult result;
    result.sourceRate = format.rate; result.sourceFrames = sourceFrames;
    if (converted > maxFrames) result.warnings.push_back("source_truncated");
    if (!frames && !cues.empty()) result.warnings.push_back("empty_reel_cues_ignored");
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
                if (!id) return fail("cue_ids_exhausted");
            }
            markers.push_back({0, id});
        }
        for (const Marker& cue : cues) {
            if (cue.frame > sourceFrames) {
                result.warnings.push_back("out_of_range_cue_ignored"); continue;
            }
            if (cue.frame == sourceFrames) { result.warnings.push_back("end_sentinel_ignored"); continue; }
            const std::uint64_t mapped =
                (std::uint64_t(cue.frame) * 48000 + format.rate / 2) / format.rate;
            if (mapped >= frames) continue; // Cue lies in truncated tail.
            if (!markers.empty() && markers.back().frame == mapped) {
                result.warnings.push_back("colliding_cue_ignored"); continue;
            }
            markers.push_back({std::uint32_t(mapped), cue.id});
        }
        if (markers.size() > kMaxSplices) return fail("too_many_splices");
        for (std::size_t i = 0; i < markers.size(); ++i)
            for (std::size_t j = 0; j < i; ++j)
                if (markers[i].id == markers[j].id) return fail("duplicate_cue_id");
    }
    try { result.reel.reset(new Reel(capacityPages, capacityPages)); }
    catch (...) { return fail("reel_allocation_failed"); }
    if (!at(in, dataOffset)) return fail("seek_data_failed");
    if (!resample(in, format, sourceFrames, frames, result)) return fail("resample_or_decode_failed");
    result.reel->finishImport();
    if (frames && !result.reel->replaceMarkers(markers.data(),
            std::uint16_t(markers.size()))) return fail("invalid_cue_table");
    if (result.nonfiniteSamples) result.warnings.push_back("nonfinite_samples_zeroed");
    if (format.rate != 48000) result.warnings.push_back("resampled_to_48k");
    return result;
}

} } // namespace chimera::wav
