#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>

enum class OctaviaPresenceState : int {
    Idle = 0, Inspecting = 1, Thinking = 2,
    Working = 3, Error = 4, Sleeping = 5
};

inline const char* octaviaPresenceName(OctaviaPresenceState state) {
    static const char* names[] = {"idle", "inspecting", "thinking", "working", "error", "sleeping"};
    return names[static_cast<int>(state)];
}

// HTTP threads publish pulses. UI/HTTP status reads resolve the same timeline;
// none of this runs on the audio thread. The short lock keeps leases coherent.
class OctaviaPresence {
    using State = OctaviaPresenceState;
    std::array<std::atomic<int64_t>, 6> until{};
    std::mutex mutex;
    State automatic = State::Idle;
    int64_t changedAt = 0;
    bool initialized = false;
    State overrideState = State::Idle;
    int64_t overrideUntil = 0;

public:
    enum { HoldMs = 2000, ActivityMs = 2500, ErrorMs = 4000,
           FadeMs = 350, DefaultLeaseMs = 30000, MaxLeaseMs = 300000 };
    struct Status {
        State target = State::Idle;
        State automatic = State::Idle;
        State overrideState = State::Idle;
        int64_t remainingMs = 0;
    };

    static int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    static State activity(const std::string& method, const std::string& path) {
        // Polling, lease changes and console housekeeping must not drive art.
        if (path == "/status" || path == "/presence" || path.compare(0, 8, "/console") == 0)
            return State::Idle;
        if (path == "/audio/analyze" || path == "/audio/compare")
            return State::Thinking;
        return method == "GET" || method == "HEAD" ? State::Inspecting : State::Working;
    }
    void pulse(State state, int64_t now = nowMs()) {
        if (state == State::Idle || state == State::Sleeping) return;
        auto& deadline = until[static_cast<int>(state)];
        const int64_t next = now + (state == State::Error ? ErrorMs : ActivityMs);
        auto previous = deadline.load(std::memory_order_relaxed);
        while (previous < next && !deadline.compare_exchange_weak(previous, next,
                std::memory_order_relaxed)) {}
    }
    bool setOverride(State state, int64_t leaseMs = DefaultLeaseMs, int64_t now = nowMs()) {
        if (static_cast<int>(state) < 0 || static_cast<int>(state) > 5
                || leaseMs < 1000 || leaseMs > MaxLeaseMs) return false;
        std::lock_guard<std::mutex> lock(mutex);
        overrideState = state;
        overrideUntil = now + leaseMs;
        return true;
    }
    void releaseOverride() {
        std::lock_guard<std::mutex> lock(mutex);
        overrideUntil = 0;
    }
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        overrideUntil = 0;
        initialized = false;
        for (auto& deadline : until) deadline.store(0, std::memory_order_relaxed);
    }
    Status status(bool running, bool startupFailed, int64_t now = nowMs()) {
        std::lock_guard<std::mutex> lock(mutex);
        State candidate = State::Idle;
        for (auto state : {State::Error, State::Thinking, State::Working, State::Inspecting}) {
            if (until[static_cast<int>(state)].load(std::memory_order_relaxed) > now) {
                candidate = state;
                break;
            }
        }
        if (startupFailed) candidate = State::Error;
        else if (!running) candidate = State::Sleeping;
        // Startup/stop bypass the hold; ordinary activity (including HTTP
        // errors) waits for the current state's minimum dwell time.
        if (!initialized || !running || startupFailed || automatic == State::Sleeping
                || now - changedAt >= HoldMs) {
            if (!initialized || candidate != automatic) changedAt = now;
            automatic = candidate;
            initialized = true;
        }
        if (overrideUntil <= now || !running || startupFailed) overrideUntil = 0;
        Status result;
        result.automatic = automatic;
        result.overrideState = overrideState;
        result.remainingMs = std::max<int64_t>(0, overrideUntil - now);
        result.target = result.remainingMs > 0 ? overrideState : automatic;
        return result;
    }
    State state(bool running, bool startupFailed, int64_t now = nowMs()) {
        return status(running, startupFailed, now).target;
    }
};

// Preserve the current mixture when retargeting mid-fade, rather than snapping
// to either endpoint. Only six small weights change; no texture is recreated.
class OctaviaPresenceFade {
    using State = OctaviaPresenceState;
    std::array<float, 6> weights_{{1.f, 0.f, 0.f, 0.f, 0.f, 0.f}};
    std::array<float, 6> from = weights_;
    State target = State::Idle;
    int64_t startedAt = 0;
    bool fading = false;
public:
    const std::array<float, 6>& weights() const { return weights_; }
    bool update(State next, int64_t now) {
        const auto previous = weights_;
        if (fading) {
            float t = std::max(0.f, std::min(1.f,
                float(now - startedAt) / OctaviaPresence::FadeMs));
            const float smooth = t * t * (3.f - 2.f * t);
            for (int i = 0; i < 6; ++i)
                weights_[i] = from[i] * (1.f - smooth)
                    + (i == static_cast<int>(target) ? smooth : 0.f);
            fading = t < 1.f;
        }
        if (next != target) {
            from = weights_;
            target = next;
            startedAt = now;
            fading = true;
        }
        return previous != weights_ || fading;
    }
};
