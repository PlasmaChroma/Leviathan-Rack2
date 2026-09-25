#pragma once

#include <speex/speex_resampler.h>
#include <cmath>
#include <cstdint>
#include <memory>

namespace chimera {

// Rack-facing values, sampled once per host frame. The bridge owns delayed
// copies; neither the core nor the resampler reads mutable Rack ports later.
struct HostState {
    float params[12]{};
    float volts[13]{};
    bool connected[13]{};
    unsigned rises[5]{}; // PLAY, CLOCK, REC, SPLICE, SHIFT
    int command = 0;
    unsigned selectionCommands = 0;
};

struct HostOutput {
    float left = 0.f, right = 0.f, cv = 0.f;
    bool eosg = false;
    HostOutput() = default;
    HostOutput(float l, float r, float c, bool e)
        : left(l), right(r), cv(c), eosg(e) {}
};

class RateBridge {
public:
    static constexpr unsigned kFifoCapacity = 32768;
    static constexpr unsigned kEventCapacity = 16384;
    static constexpr unsigned kMaxCorePerHost = 32;
    static constexpr unsigned kMaxHostPerCore = 64;
    static constexpr unsigned kMaxEventsPerCore = 256;

    static bool supported(float rate) {
        return std::isfinite(rate) && rate >= 8000.f && rate <= 768000.f &&
               std::floor(rate) == rate;
    }

    explicit RateBridge(unsigned rate) : rate_(rate),
        history_(new HostState[kFifoCapacity]),
        coreHistory_(new HostOutput[kFifoCapacity]),
        output_(new HostOutput[kFifoCapacity]),
        events_(new Event[kEventCapacity]) {
        if (rate < 8000 || rate > 768000 || rate == 48000) return;
        int err = 0;
        input_ = speex_resampler_init(2, rate, 48000, 5, &err);
        if (!input_ || err != RESAMPLER_ERR_SUCCESS) return;
        outputResampler_ = speex_resampler_init(2, 48000, rate, 5, &err);
        if (!outputResampler_ || err != RESAMPLER_ERR_SUCCESS) return;
        inputLatencyHost_ = speex_resampler_get_input_latency(input_);
        inputLatencyCore_ = speex_resampler_get_output_latency(input_);
        outputLatencyCore_ = speex_resampler_get_input_latency(outputResampler_);
        outputLatencyHost_ = speex_resampler_get_output_latency(outputResampler_);
        valid_ = inputLatencyHost_ >= 0 && inputLatencyCore_ >= 0 &&
                 outputLatencyCore_ >= 0 && outputLatencyHost_ >= 0 &&
                 13u * (unsigned(inputLatencyHost_) + 16u) < kEventCapacity;
    }

    RateBridge(const RateBridge&) = delete;
    RateBridge& operator=(const RateBridge&) = delete;
    ~RateBridge() {
        if (input_) speex_resampler_destroy(input_);
        if (outputResampler_) speex_resampler_destroy(outputResampler_);
    }

    bool valid() const { return valid_; }
    unsigned rate() const { return rate_; }
    int inputLatencyHost() const { return inputLatencyHost_; }
    int inputLatencyCore() const { return inputLatencyCore_; }
    int outputLatencyCore() const { return outputLatencyCore_; }
    int outputLatencyHost() const { return outputLatencyHost_; }
    std::uint64_t underruns() const { return underruns_; }
    std::uint64_t overflows() const { return overflows_; }
    unsigned eventHighWater() const { return eventHighWater_; }

    // Seed a newly prepared bridge before its first frame. Existing high
    // Schmitt gates (including voltages in the hysteresis band) are baseline
    // state, not new edges after bypass. This never resets a used resampler.
    void seedGatesForResume(const HostState& host, const bool (&gateHigh)[5]) {
        if (hostFrames_ || coreFrames_) return;
        for (unsigned j = 0; j < 13; ++j) lastConnected_[j] = host.connected[j];
        for (unsigned k = 0; k < 5; ++k)
            lastGateHigh_[k] = host.connected[gateId(k)] && gateHigh[k];
    }

    template <class CoreFn>
    HostOutput step(const HostState& host, CoreFn core) {
        if (!valid_ || fault_) return {};
        history_[hostFrames_ % kFifoCapacity] = host;
        if (!queueEvents(host)) { fault_ = true; ++overflows_; return {}; }
        const float in[2] = {host.connected[0] ? host.volts[0] : 0.f,
                             host.connected[1] ? host.volts[1] :
                             (host.connected[0] ? host.volts[0] : 0.f)};
        float converted[kMaxCorePerHost * 2]{};
        spx_uint32_t consumed = 1, produced = kMaxCorePerHost;
        const int error = speex_resampler_process_interleaved_float(input_, in,
                              &consumed, converted, &produced);
        if (error != RESAMPLER_ERR_SUCCESS || consumed != 1 || produced > kMaxCorePerHost) {
            fault_ = true; ++overflows_; return {};
        }
        for (unsigned i = 0; i < produced; ++i) {
            if (!processCore(converted + 2 * i, core)) {
                fault_ = true; ++overflows_; return {};
            }
        }
        ++hostFrames_;
        if (outputRead_ != outputWrite_) {
            HostOutput result = output_[outputRead_ % kFifoCapacity];
            ++outputRead_;
            return result;
        }
        ++underruns_;
        // Speex starts with a bounded filter delay. Silence before its first
        // complete output is priming, never a fabricated input to the core.
        if (hostFrames_ > unsigned(inputLatencyHost_ + outputLatencyHost_ + rate_ / 100))
            fault_ = true;
        return {};
    }

    bool failed() const { return fault_; }
#ifdef CHIMERA_RATE_BRIDGE_TEST_HOOKS
    void fillEventQueueForTest() { eventWrite_ = eventRead_ + kEventCapacity; }
#endif

private:
    struct Event {
        std::uint64_t coreFrame;
        unsigned char jack;
        bool rise;
        unsigned value;
    };
    static unsigned char gateId(int index) {
        static const unsigned char ids[5] = {9, 8, 10, 11, 12};
        return ids[index];
    }
    unsigned rate_ = 0;
    std::unique_ptr<HostState[]> history_;
    std::unique_ptr<HostOutput[]> coreHistory_;
    std::unique_ptr<HostOutput[]> output_;
    std::unique_ptr<Event[]> events_;
    SpeexResamplerState* input_ = nullptr;
    SpeexResamplerState* outputResampler_ = nullptr;
    int inputLatencyHost_ = 0, inputLatencyCore_ = 0;
    int outputLatencyCore_ = 0, outputLatencyHost_ = 0;
    bool valid_ = false, fault_ = false;
    bool lastConnected_[13]{}, lastGateHigh_[5]{};
    std::uint64_t hostFrames_ = 0, coreFrames_ = 0, generatedHostFrames_ = 0;
    std::uint64_t eventRead_ = 0, eventWrite_ = 0;
    std::uint64_t outputRead_ = 0, outputWrite_ = 0;
    std::uint64_t underruns_ = 0, overflows_ = 0;
    unsigned eventHighWater_ = 0;

    static std::uint64_t ceilDivide(std::uint64_t n, unsigned d) {
        return (n + d - 1) / d;
    }
    bool queueEvents(const HostState& host) {
        const std::uint64_t target =
            ceilDivide(hostFrames_ * 48000u, rate_) + unsigned(inputLatencyCore_);
        for (unsigned j = 0; j < 13; ++j) {
            int gate = -1;
            for (int k = 0; k < 5; ++k)
                if (gateId(k) == j) gate = k;
            bool high = false;
            if (gate >= 0) {
                high = host.connected[j] && (lastGateHigh_[gate] ?
                    host.volts[j] > 1.f : host.volts[j] >= 2.5f);
            }
            if (host.connected[j] == lastConnected_[j] &&
                (gate < 0 || high == lastGateHigh_[gate])) continue;
            if (eventWrite_ - eventRead_ >= kEventCapacity) return false;
            events_[eventWrite_ % kEventCapacity] =
                {target, static_cast<unsigned char>(j),
                 gate >= 0 && high && !lastGateHigh_[gate], 0};
            ++eventWrite_;
            lastConnected_[j] = host.connected[j];
            if (gate >= 0) lastGateHigh_[gate] = high;
        }
        if (host.command) {
            if (eventWrite_ - eventRead_ >= kEventCapacity) return false;
            events_[eventWrite_ % kEventCapacity] =
                {target, 13, false, unsigned(host.command)};
            ++eventWrite_;
        }
        if (host.selectionCommands) {
            if (eventWrite_ - eventRead_ >= kEventCapacity) return false;
            events_[eventWrite_ % kEventCapacity] =
                {target, 14, false, host.selectionCommands};
            ++eventWrite_;
        }
        const unsigned occupancy = unsigned(eventWrite_ - eventRead_);
        if (occupancy > eventHighWater_) eventHighWater_ = occupancy;
        return true;
    }
    template <class CoreFn>
    bool processCore(const float* audio, CoreFn core) {
        const std::int64_t sourceCore = std::int64_t(coreFrames_) - inputLatencyCore_;
        const std::uint64_t numerator = sourceCore <= 0 ? 0 :
            std::uint64_t(sourceCore) * rate_;
        std::uint64_t sourceHost = numerator / 48000u;
        if (sourceHost > hostFrames_) sourceHost = hostFrames_;
        if (hostFrames_ - sourceHost >= kFifoCapacity) return false;
        HostState state = history_[sourceHost % kFifoCapacity];
        const float alpha = float(numerator % 48000u) / 48000.f;
        if (alpha > 0.f && sourceHost < hostFrames_) {
            const HostState& next = history_[(sourceHost + 1) % kFifoCapacity];
            for (int p = 0; p < 9; ++p)
                state.params[p] += alpha * (next.params[p] - state.params[p]);
            for (int jack = 2; jack <= 7; ++jack)
                if (state.connected[jack] == next.connected[jack])
                    state.volts[jack] += alpha *
                        (next.volts[jack] - state.volts[jack]);
        }
        state.volts[0] = audio[0];
        state.volts[1] = audio[1];
        state.command = 0;
        state.selectionCommands = 0;
        for (int k = 0; k < 5; ++k) state.rises[k] = 0;
        unsigned count = 0;
        while (eventRead_ != eventWrite_ &&
               events_[eventRead_ % kEventCapacity].coreFrame <= coreFrames_) {
            const Event e = events_[eventRead_ % kEventCapacity];
            ++eventRead_;
            if (++count > kMaxEventsPerCore) return false;
            if (e.jack == 13) state.command = int(e.value);
            if (e.jack == 14) state.selectionCommands |= e.value;
            if (e.rise) {
                for (int k = 0; k < 5; ++k)
                    if (gateId(k) == e.jack) ++state.rises[k];
            }
        }
        const HostOutput result = core(state);
        coreHistory_[coreFrames_ % kFifoCapacity] = result;
        ++coreFrames_;
        const float in[2] = {result.left, result.right};
        float out[kMaxHostPerCore * 2]{};
        spx_uint32_t consumed = 1, produced = kMaxHostPerCore;
        const int error = speex_resampler_process_interleaved_float(outputResampler_, in,
                              &consumed, out, &produced);
        if (error != RESAMPLER_ERR_SUCCESS || consumed != 1 || produced > kMaxHostPerCore ||
            outputWrite_ - outputRead_ + produced >= kFifoCapacity) return false;
        for (unsigned i = 0; i < produced; ++i) {
            const std::int64_t source = std::int64_t(generatedHostFrames_ * 48000u / rate_) -
                                        outputLatencyCore_;
            HostOutput frame{};
            frame.left = out[2*i]; frame.right = out[2*i+1];
            if (source >= 0 && std::uint64_t(source) < coreFrames_ &&
                coreFrames_ - std::uint64_t(source) < kFifoCapacity) {
                const HostOutput& control = coreHistory_[std::uint64_t(source) % kFifoCapacity];
                frame.cv = control.cv;
                frame.eosg = control.eosg;
            }
            output_[outputWrite_ % kFifoCapacity] = frame;
            ++outputWrite_;
            ++generatedHostFrames_;
        }
        return true;
    }
};

} // namespace chimera
