#include "ChimeraWav.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>

static void need(bool condition, const char* message) {
    if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static std::uint32_t u32(const std::string& bytes, std::size_t at) {
    return std::uint32_t(std::uint8_t(bytes[at])) |
           (std::uint32_t(std::uint8_t(bytes[at+1])) << 8) |
           (std::uint32_t(std::uint8_t(bytes[at+2])) << 16) |
           (std::uint32_t(std::uint8_t(bytes[at+3])) << 24);
}
static void set32(std::string& bytes, std::size_t at, std::uint32_t value) {
    for (int i = 0; i < 4; ++i) bytes[at+i] = char(value >> (8*i));
}
static void append16(std::string& bytes, std::uint16_t value) {
    bytes += char(value); bytes += char(value >> 8);
}
static void append32(std::string& bytes, std::uint32_t value) {
    append16(bytes, std::uint16_t(value)); append16(bytes, std::uint16_t(value >> 16));
}
static void append64(std::string& bytes, std::uint64_t value) {
    append32(bytes, std::uint32_t(value)); append32(bytes, std::uint32_t(value >> 32));
}
static std::string simpleWav(std::uint16_t code, std::uint16_t bits,
                             std::uint16_t channels, std::uint32_t rate,
                             std::uint32_t frames) {
    std::string wav = "RIFF"; append32(wav, 0); wav += "WAVEfmt ";
    append32(wav, 16); append16(wav, code); append16(wav, channels);
    append32(wav, rate);
    const std::uint16_t align = channels * (bits / 8);
    append32(wav, rate * align); append16(wav, align); append16(wav, bits);
    wav += "data"; append32(wav, frames * align);
    for (std::uint32_t i = 0; i < frames; ++i) {
        for (std::uint16_t channel = 0; channel < channels; ++channel) {
            const float value = channel ? -0.5f : 0.5f;
            if (code == 1 && bits == 16) append16(wav, std::uint16_t(channel ? -16384 : 16384));
            if (code == 1 && bits == 24) {
                const std::uint32_t raw = channel ? 0xc00000u : 0x400000u;
                wav += char(raw); wav += char(raw >> 8); wav += char(raw >> 16);
            }
            if (code == 1 && bits == 32) append32(wav, channel ? 0xc0000000u : 0x40000000u);
            if (code == 3 && bits == 32) {
                std::uint32_t raw; std::memcpy(&raw, &value, 4); append32(wav, raw);
            }
            if (code == 3 && bits == 64) {
                const double wide = value;
                std::uint64_t raw; std::memcpy(&raw, &wide, 8); append64(wav, raw);
            }
        }
    }
    set32(wav, 4, std::uint32_t(wav.size() - 8));
    return wav;
}
static bool sameBits(float a, float b) {
    return std::memcmp(&a, &b, sizeof(float)) == 0;
}

int main() {
    for (unsigned rate : {8000u, 44100u, 48000u, 96000u, 192000u, 383999u}) {
        for (unsigned frames : {0u, 1u, 7u}) {
            std::istringstream stream(simpleWav(3, 32, 2, rate, frames), std::ios::binary);
            auto converted = chimera::wav::readConvenience(stream, false, 1);
            need(bool(converted) && converted.reel->validFrames() ==
                 (std::uint64_t(frames) * 48000 + rate - 1) / rate,
                 "empty and short clips keep rounded duration at extreme rational rates");
            for (unsigned i = 0; i < converted.reel->validFrames(); ++i) {
                const auto value = converted.reel->readActive(i);
                need(std::fabs(value.l - 0.5f) < 0.001f && std::fabs(value.r + 0.5f) < 0.001f,
                     "short constant endpoints retain level and stereo polarity");
            }
        }
    }
    for (unsigned rate : {48000u, 96000u}) {
        std::string wav = simpleWav(3, 64, 1, rate, 1);
        const double huge = std::numeric_limits<double>::max();
        std::memcpy(&wav[44], &huge, sizeof(huge));
        std::istringstream stream(wav, std::ios::binary);
        auto converted = chimera::wav::readConvenience(stream, false, 1);
        need(bool(converted) && converted.nonfiniteSamples == 1 &&
             converted.reel->readActive(0).l == 0,
             "finite double outside float range is sanitized before conversion");
    }
    for (unsigned rate : {44100u, 96000u, 192000u}) {
        for (double frequency : {1000.0, 18000.0, 30000.0}) {
            if (frequency >= rate * 0.5) continue;
            const unsigned frames = rate / 5;
            std::string wav = simpleWav(3, 32, 2, rate, frames);
            for (unsigned i = 0; i < frames; ++i) {
                const float left = float(std::sin(2.0 * 3.141592653589793 * frequency * i / rate));
                const float right = -left;
                std::uint32_t bits;
                std::memcpy(&bits, &left, 4); set32(wav, 44 + i*8, bits);
                std::memcpy(&bits, &right, 4); set32(wav, 48 + i*8, bits);
            }
            std::istringstream stream(wav, std::ios::binary);
            auto converted = chimera::wav::readConvenience(stream, false, 40);
            need(bool(converted) && converted.reel->validFrames() == 9600,
                 "bandlimited conversion preserves exact duration");
            double energy = 0, error = 0;
            for (unsigned i = 1000; i < 8600; ++i) {
                const auto value = converted.reel->readActive(i);
                need(std::fabs(value.l + value.r) < 1e-6, "resampling preserves stereo phase");
                energy += double(value.l) * value.l;
                const double expected = std::sin(2.0 * 3.141592653589793 * frequency * i / 48000);
                error += (value.l - expected) * (value.l - expected);
            }
            const double rms = std::sqrt(energy / 7600);
            if (frequency < 24000)
                need(std::fabs(rms - std::sqrt(0.5)) < 0.002 && std::sqrt(error / 7600) < 0.002,
                     "conversion preserves passband gain and time alignment");
            else need(rms < 0.0001, "downsampling rejects ultrasonic alias by at least 77 dB");
        }
    }
    {
        // More than two chunks plus a partial tail; verify bytes after each
        // writer-buffer boundary and the final short write through roundtrip.
        const unsigned frames = 2*131072+17;
        const unsigned pages = (frames+255)/256;
        chimera::Reel large(pages, pages);
        for (unsigned i = 0; i < frames; ++i)
            large.write(i, {float(i%32768)/32768.f, -float(i%8192)/8192.f}, i);
        large.beginSnapshot(frames);
        while (!large.readyForWorker()) large.maintenanceTick();
        std::ostringstream encoded(std::ios::binary);
        std::string error;
        need(chimera::wav::writeCanonical(encoded, large, error), "write multiple WAV chunks");
        std::istringstream decoded(encoded.str(), std::ios::binary);
        auto restored = chimera::wav::readStrict(decoded, pages);
        need(bool(restored) && restored.reel->validFrames() == frames, "multi-chunk WAV length");
        for (unsigned i = 0; i < frames; ++i) {
            const auto a = large.readSnapshot(i), b = restored.reel->readActive(i);
            need(sameBits(a.l, b.l) && sameBits(a.r, b.r), "multi-chunk stereo payload exact");
        }
    }
    chimera::Reel reel(2, 2);
    for (std::uint32_t i = 0; i < 300; ++i)
        need(reel.write(i, {float(i) / 37.f, -float(i) / 53.f}, i),
             "prepare distinct stereo frames");
    need(reel.addMarker(37) && reel.addMarker(256), "prepare frame-based markers");
    const std::uint32_t ids[3] = {reel.markerId(0), reel.markerId(1), reel.markerId(2)};
    need(reel.beginSnapshot(300), "establish immutable export cut");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    need(reel.write(0, {99.f, 99.f}, 301), "overwrite active page after cut");
    std::ostringstream out(std::ios::binary);
    std::string error;
    need(chimera::wav::writeCanonical(out, reel, error), "write canonical snapshot WAV");
    const std::string bytes = out.str();
    need(bytes.compare(0, 4, "RIFF") == 0 &&
         bytes.compare(8, 4, "WAVE") == 0 &&
         u32(bytes, 4) + 8 == bytes.size(), "RIFF size covers all chunks");
    const std::size_t fmt = bytes.find("fmt ");
    const std::size_t fact = bytes.find("fact");
    const std::size_t cue = bytes.find("cue ");
    const std::size_t data = bytes.rfind("data");
    need(fmt != std::string::npos && fact != std::string::npos &&
         cue != std::string::npos && data != std::string::npos,
         "canonical format/fact/cue/data chunks exist");
    need(u32(bytes, fmt+4) == 18 && u32(bytes, fact+4) == 4 &&
         u32(bytes, fact+8) == 300 && u32(bytes, cue+4) == 4+3*24 &&
         u32(bytes, data+4) == 300*8,
         "chunk byte counts and valid-frame length are exact");
    for (std::uint32_t i = 0; i < 3; ++i) {
        need(u32(bytes, cue+12+i*24) == ids[i] &&
             u32(bytes, cue+32+i*24) == (i == 0 ? 0u : (i == 1 ? 37u : 256u)),
             "cue ID and dwSampleOffset use sample-frame units");
    }
    std::istringstream in(bytes, std::ios::binary);
    chimera::wav::ImportResult imported = chimera::wav::readStrict(in, 2);
    need(bool(imported) && imported.sourceFrames == 300 &&
         imported.reel->markerCount() == 3, "strict canonical WAV roundtrip");
    for (std::uint32_t i = 0; i < 300; ++i) {
        const chimera::StereoFrame original = reel.readSnapshot(i);
        const chimera::StereoFrame restored = imported.reel->readActive(i);
        need(sameBits(original.l, restored.l) && sameBits(original.r, restored.r),
             "canonical finite payload remains bit-identical");
    }
    for (std::uint16_t i = 0; i < 3; ++i)
        need(imported.reel->markerId(i) == ids[i], "cue identities roundtrip");
    need(imported.reel->readActive(0).l != reel.readActive(0).l,
         "export used frozen sample rather than post-cut active overwrite");
    {
        std::string bad = bytes;
        bad.resize(bad.size()-1);
        std::istringstream stream(bad, std::ios::binary);
        need(!chimera::wav::readStrict(stream, 2), "truncated RIFF is rejected");
    }
    {
        std::string bad = bytes;
        bad[fmt+10] = 1; // Mono channel count.
        std::istringstream stream(bad, std::ios::binary);
        need(!chimera::wav::readStrict(stream, 2), "strict import rejects mono");
    }
    {
        std::string bad = bytes;
        set32(bad, fmt+12, 44100);
        std::istringstream stream(bad, std::ios::binary);
        need(!chimera::wav::readStrict(stream, 2), "strict import rejects another rate");
    }
    {
        std::string bad = bytes;
        set32(bad, data+4, UINT32_MAX);
        std::istringstream stream(bad, std::ios::binary);
        need(!chimera::wav::readStrict(stream, 2), "oversized chunk cannot escape RIFF bounds");
    }
    {
        std::string bad = bytes;
        set32(bad, data+8, 0x7fc00000u); // Nonfinite left sample.
        std::istringstream stream(bad, std::ios::binary);
        chimera::wav::ImportResult sanitized = chimera::wav::readStrict(stream, 2);
        need(bool(sanitized) && sanitized.nonfiniteSamples == 1 &&
             sanitized.reel->readActive(0).l == 0.f,
             "nonfinite sample is zeroed with count");
    }
    {
        // Mono PCM16 at 24 kHz must duplicate channels and double the frame count.
        std::string pcm = "RIFF"; append32(pcm, 0); pcm += "WAVEfmt ";
        append32(pcm, 16); append16(pcm, 1); append16(pcm, 1);
        append32(pcm, 24000); append32(pcm, 48000);
        append16(pcm, 2); append16(pcm, 16);
        pcm += "cue "; append32(pcm, 28); append32(pcm, 1);
        append32(pcm, 7); append32(pcm, 2); pcm += "data";
        append32(pcm, 0); append32(pcm, 0); append32(pcm, 2);
        pcm += "data"; append32(pcm, 8);
        append16(pcm, 0); append16(pcm, 16384);
        append16(pcm, 32767); append16(pcm, 0);
        set32(pcm, 4, std::uint32_t(pcm.size() - 8));
        std::istringstream stream(pcm, std::ios::binary);
        chimera::wav::ImportResult converted =
            chimera::wav::readConvenience(stream, false, 1);
        need(bool(converted) && converted.sourceRate == 24000 &&
             converted.sourceFrames == 4 && converted.reel->validFrames() == 8,
             "convenience import resamples PCM16 mono");
        need(converted.reel->markerCount() == 2 &&
             converted.reel->region(1).begin == 4 &&
             converted.reel->markerId(1) == 7,
             "convenience cue maps to 48 kHz frame with stable ID");
        const chimera::StereoFrame sample = converted.reel->readActive(3);
        need(std::isfinite(sample.l) && sample.l > 0.5f && sameBits(sample.l, sample.r),
             "mono duplication and bandlimited interpolation");
        std::istringstream strict(pcm, std::ios::binary);
        need(!chimera::wav::readStrict(strict, 1),
             "strict import continues rejecting convenience WAV");
    }
    for (std::uint16_t code : {std::uint16_t(1), std::uint16_t(3)}) {
        const std::uint16_t first = code == 1 ? 16 : 32;
        const std::uint16_t last = code == 1 ? 32 : 64;
        for (std::uint16_t bits = first; bits <= last; bits += code == 1 ? 8 : 32) {
            for (std::uint16_t channels : {std::uint16_t(1), std::uint16_t(2)}) {
                const std::string wav = simpleWav(code, bits, channels, 96000, 8);
                std::istringstream stream(wav, std::ios::binary);
                chimera::wav::ImportResult converted =
                    chimera::wav::readConvenience(stream, false, 1);
                need(bool(converted) && converted.sourceFrames == 8 &&
                     converted.reel->validFrames() == 4,
                     "convenience formats and channel layouts decode at 96 kHz");
                const chimera::StereoFrame sample = converted.reel->readActive(2);
                need(std::fabs(sample.l - 0.5f) < 0.00001f &&
                     std::fabs(sample.r - (channels == 1 ? 0.5f : -0.5f)) < 0.00001f,
                     "PCM/float amplitudes and stereo identity survive conversion");
            }
        }
    }
    {
        const std::string wav = simpleWav(1, 16, 1, 8000, 200);
        std::istringstream reject(wav, std::ios::binary);
        need(!chimera::wav::readConvenience(reject, false, 1),
             "overlength convenience import rejects by default");
        std::istringstream truncate(wav, std::ios::binary);
        chimera::wav::ImportResult cut = chimera::wav::readConvenience(truncate, true, 1);
        need(bool(cut) && cut.reel->validFrames() == 256 &&
             !cut.warnings.empty() && cut.warnings[0] == "source_truncated",
             "explicit truncation respects Reel capacity and reports the cut");
    }
    std::puts("PASS: Chimera canonical WAV writer, strict reader, convenience format matrix");
}
