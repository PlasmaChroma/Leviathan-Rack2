#include "ChimeraPlaybackReader.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}
int main() {
    chimera::PlaybackReader reader;
    chimera::Reel reel(256, 256);
    const chimera::Region region{128, 48128};
    bool invalid = false;
    // Random access, fractional phase, arbitrary Splice boundaries and changing
    // speed compare the acceleration directly against the full FIR reference.
    unsigned random = 17;
    auto noise = [&] { random = random*1664525u + 1013904223u; return float(random >> 8)/8388608.f-1.f; };
    for (unsigned i = 0; i < 48256; ++i) reel.write(i, {noise(), noise()}, i);
    for (unsigned i = 0; i < 1000000; ++i) {
        const unsigned at = 200 + i%65;
        reel.write(at, {noise(), noise()}, i);
    }
    // All alignments and short region lengths exercise both partial edges and
    // whole blocks. Compare with a direct mean, including live overwrites.
    for (unsigned begin = 0; begin < 256; ++begin) {
        for (unsigned length : {1u, 15u, 16u, 17u, 63u, 64u, 255u, 256u, 480u, 512u}) {
            const chimera::Region shortRegion{begin, begin+length};
            reel.write(begin+length/2, {noise(), noise()}, 0);
            double left = 0, right = 0;
            for (unsigned i = begin; i < begin+length; ++i) {
                left += reel.readActive(i).l; right += reel.readActive(i).r;
            }
            const auto mean = reader.read(reel, shortRegion, -17.5, -512, invalid);
            need(std::fabs(mean.l-left/length) < 1e-5 &&
                 std::fabs(mean.r-right/length) < 1e-5 && !invalid,
                 "block mean matches direct summation at every alignment");
        }
    }
    {
        chimera::Reel imported(256, 256);
        for (unsigned i = 0; i < 48256; ++i)
            need(imported.appendImported(reel.readActive(i)), "append imported frame");
        imported.finishImport();
        for (unsigned size : {16u, 64u, 256u}) {
            for (unsigned at = 0; at < 48256; at += size) {
                const auto& a = imported.playbackMoments(at, size);
                const auto& b = reel.playbackMoments(at, size);
                for (unsigned j = 0; j < 4; ++j)
                    need(std::fabs(a.left[j]-b.left[j]) < 0.001f &&
                         std::fabs(a.right[j]-b.right[j]) < 0.001f,
                         "bulk moments agree with live incrementally maintained moments");
            }
        }
        // Resume real-time writes in a partially populated final block.
        for (unsigned i = 48200; i < 48500; ++i) {
            const chimera::StereoFrame value{noise(), noise()};
            imported.write(i, value, i); reel.write(i, value, i);
        }
        for (unsigned i = 0; i < 64; ++i) {
            const double speed = i*8+1;
            const auto a = reader.read(imported, {199, 48500}, 48000.5, speed, invalid);
            const auto b = reader.read(reel, {199, 48500}, 48000.5, speed, invalid);
            need(std::fabs(a.l-b.l) < 1e-5 && std::fabs(a.r-b.r) < 1e-5,
                 "bulk imported moments remain correct after live append and overwrite");
        }
    }
    double maxError = 0;
    for (unsigned i = 0; i < 500; ++i) {
        const chimera::Region splice = i%2 ? chimera::Region{199, 266} : chimera::Region{117, 47003};
        const double speed = 1.125 + 510.875*std::fabs(noise());
        const double position = 24000 + 24000*noise();
        const auto fast = reader.read(reel, splice, position, speed, invalid);
        const auto exact = reader.read(reel, splice, position, speed, invalid, false);
        maxError = std::max(maxError, std::fabs(double(fast.l)-exact.l));
        maxError = std::max(maxError, std::fabs(double(fast.r)-exact.r));
    }
    std::printf("accelerated FIR maximum error after repeated live overwrites=%g\n", maxError);
    need(maxError < 0.001 && !invalid, "block approximation and overwrite drift stay below -60 dBFS");
    for (double speed : {2.0, -2.0, 4.0, 16.0, 512.0}) {
        for (bool reject : {false, true}) {
            // Integer-period source frequency, well inside pass/stop bands.
            const double frequency = reject ? (speed == 2 || speed == -2 ? 15000 : 18000) :
                (std::fabs(speed) < 32 ? 500 : 10);
            for (unsigned i = 0; i < 48256; ++i) {
                const float value = i < region.begin || i >= region.end ? 32.f :
                    float(std::sin(2*chimera::profile1::kPi*frequency*(i-region.begin)/48000));
                reel.write(i, {value, -value}, i);
            }
            double energy = 0, error = 0;
            for (unsigned i = 0; i < 4096; ++i) {
                const double p = region.begin + i*speed + 0.375;
                const auto value = reader.read(reel, region, p, speed, invalid);
                energy += value.l * value.l;
                const double reference = std::sin(2*chimera::profile1::kPi*frequency*(p-region.begin)/48000);
                error += (value.l-reference)*(value.l-reference);
                need(std::fabs(value.l + value.r) < 1e-6, "stereo alignment");
            }
            const double rms = std::sqrt(energy/4096), rmsError = std::sqrt(error/4096);
            std::printf("speed=%g frequency=%g rms=%g error=%g\n", speed, frequency, rms, rmsError);
            need(!invalid && (reject ? rms < 0.003 : rmsError < 0.003),
                 "splice-local rejection/passband including reverse and maximum width");
        }
    }
    for (unsigned i = 0; i < 48256; ++i) reel.write(i, {0.5f, -0.5f}, i);
    for (double speed : {1.01, 1.125, 2.0, 32.0, 512.0}) {
        const auto value = reader.read(reel, {200, 203}, -123.25, speed, invalid);
        need(std::fabs(value.l - 0.5f) < 1e-6, "tiny Splice wraps and preserves DC");
    }
    reel.beginSnapshot(48256);
    while (!reel.readyForWorker()) reel.maintenanceTick();
    for (unsigned i = 0; i < 48256; ++i) reel.write(i, {0.25f, -0.25f}, i);
    need(std::fabs(reader.read(reel, region, 1234.5, 4, invalid).l - 0.25f) < 1e-6 &&
         reel.readSnapshot(1234).l == 0.5f, "quality playback reads live COW audio, not stale derived levels");
    volatile float sink = 0;
    for (double speed : {1.0, 2.0, 8.0, 32.0, 512.0}) {
        const auto start = std::chrono::steady_clock::now();
        for (unsigned i = 0; i < 6000; ++i)
            sink += reader.read(reel, region, i*speed+0.375, speed, invalid).l;
        const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
        std::printf("reader speed=%g core_percent_one_stereo_voice=%.3f\n", speed, seconds*800);
    }
    for (unsigned begin : {0u, 1u, 15u}) {
        const auto start = std::chrono::steady_clock::now();
        for (unsigned i = 0; i < 48000; ++i)
            for (unsigned voice = 0; voice < 8; ++voice)
                sink += reader.read(reel, {begin, begin+480}, i+voice*0.1, 512, invalid).l;
        const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
        std::printf("short-region begin=%u eight_voices_core_percent=%.3f\n", begin, seconds*100);
    }
    std::printf("PASS: bandlimited playback spectral, Splice, reverse, live COW and DC; sink=%g\n", float(sink));
}
