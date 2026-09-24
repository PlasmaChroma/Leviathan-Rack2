#include "ChimeraRateBridge.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

static void need(bool condition, const char* message) {
    if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main() {
    const unsigned rates[] = {8000, 44100, 88200, 96000, 176400, 192000, 768000};
    for (unsigned rate : rates) {
        chimera::RateBridge bridge(rate);
        need(bridge.valid(), "quality-5 stereo bridge prepares at supported rate");
        chimera::HostState state{};
        state.connected[0] = true;
        unsigned coreCount = 0, recRises = 0;
        double outputEnergy = 0;
        unsigned positiveCrossings = 0;
        float previous = 0.f;
        const unsigned frames = rate * 2;
        for (unsigned h = 0; h < frames; ++h) {
            state.volts[0] = 3.f * std::sin(2.0 * 3.141592653589793 * 440.0 * h / rate);
            state.connected[10] = true;
            state.volts[10] = h == rate / 2 ? 10.f : 0.f;
            const chimera::HostOutput out = bridge.step(state,
                [&](const chimera::HostState& in) {
                    ++coreCount;
                    recRises += in.rises[2];
                    return chimera::HostOutput(in.volts[0], in.volts[0], 2.f, false);
                });
            outputEnergy += double(out.left) * out.left;
            if (h > rate / 4 && h < rate * 7 / 4 &&
                previous <= 0.f && out.left > 0.f) ++positiveCrossings;
            previous = out.left;
            need(std::isfinite(out.left) && std::isfinite(out.right),
                 "converted host output stays finite");
            need(!bridge.failed(), "bridge does not overflow or underrun in steady state");
        }
        need(coreCount >= 95900 && coreCount <= 96100,
             "core ticks remain at 48 kHz for two host seconds");
        need(recRises == 1, "one-host-frame REC pulse survives SRC mapping");
        need(std::sqrt(outputEnergy / frames) > 1.5 &&
             std::sqrt(outputEnergy / frames) < 2.5,
             "sine output retains approximate amplitude");
        need(positiveCrossings >= 658 && positiveCrossings <= 662,
             "440 Hz pitch survives host-rate conversion");
        std::printf("rate=%u core=%u inLatency=%d/%d outLatency=%d/%d eventHigh=%u underrun=%llu\n",
                    rate, coreCount, bridge.inputLatencyHost(), bridge.inputLatencyCore(),
                    bridge.outputLatencyCore(), bridge.outputLatencyHost(),
                    bridge.eventHighWater(),
                    static_cast<unsigned long long>(bridge.underruns()));
    }
    {
        chimera::RateBridge bridge(768000);
        need(bridge.valid(), "maximum-rate bridge prepares for event stress");
        chimera::HostState state{};
        unsigned maxRises = 0;
        for (unsigned h = 0; h < 2200; ++h) {
            for (unsigned jack = 0; jack < 13; ++jack) {
                state.connected[jack] = h < 1200 ? (h & 1u) != 0 : false;
                state.volts[jack] = state.connected[jack] ? 10.f : 0.f;
            }
            bridge.step(state, [&](const chimera::HostState& in) {
                unsigned rises = 0;
                for (unsigned k = 0; k < 5; ++k) rises += in.rises[k];
                if (rises > maxRises) maxRises = rises;
                return chimera::HostOutput();
            });
            need(!bridge.failed(), "worst-rate jack history and per-core budget remain bounded");
        }
        need(bridge.eventHighWater() >= 8000 &&
             bridge.eventHighWater() < chimera::RateBridge::kEventCapacity &&
             maxRises == 40,
             "all thirteen changing jacks fit input-delay history and five gate rises survive");
    }
    for (unsigned rate : rates) {
        chimera::RateBridge bridge(rate);
        chimera::HostState state{};
        state.connected[0] = state.connected[10] = true;
        unsigned core = 0, eventCore = 0, loudCore = 0;
        unsigned eosgHost = 0, loudHost = 0;
        float loudCoreValue = 0.f, loudHostValue = 0.f;
        const unsigned frames = 4000 + unsigned(bridge.inputLatencyHost() +
                                                bridge.outputLatencyHost());
        for (unsigned h = 0; h < frames; ++h) {
            state.volts[0] = h == 1000 ? 5.f : 0.f;
            state.volts[10] = h == 1000 ? 10.f : 0.f;
            const chimera::HostOutput out = bridge.step(state,
                [&](const chimera::HostState& in) {
                    if (in.rises[2]) eventCore = core;
                    if (std::fabs(in.volts[0]) > loudCoreValue) {
                        loudCoreValue = std::fabs(in.volts[0]);
                        loudCore = core;
                    }
                    ++core;
                    return chimera::HostOutput(in.volts[0], in.volts[0],
                                               in.rises[2] ? 8.f : 0.f,
                                               in.rises[2] != 0);
                });
            if (out.eosg && !eosgHost) eosgHost = h;
            if (std::fabs(out.left) > loudHostValue) {
                loudHostValue = std::fabs(out.left);
                loudHost = h;
            }
        }
        std::printf("aligned impulse at %u: core event=%u audio=%u, host EOSG=%u audio=%u\n",
                    rate, eventCore, loudCore, eosgHost, loudHost);
        const int declaredHostLatency =
            bridge.inputLatencyHost() + bridge.outputLatencyHost();
        need(std::abs(int(loudHost) - 1000 - declaredHostLatency) <= 1,
             "measured impulse latency matches prepared Speex group delays");
        need(eventCore && loudCore && eosgHost && loudHost &&
             std::abs(int(eventCore) - int(loudCore)) <= 6 &&
             std::abs(int(eosgHost) - int(loudHost)) <=
                 int(rate / 48000u + 3u),
             "host impulse and REC event share the delayed core/output timeline");
    }
    need(!chimera::RateBridge::supported(0.f) &&
         !chimera::RateBridge::supported(47999.5f) &&
         !chimera::RateBridge::supported(800000.f),
         "invalid and fractional host rates are rejected");
    std::puts("PASS: Chimera prepared host-rate bridge");
}
