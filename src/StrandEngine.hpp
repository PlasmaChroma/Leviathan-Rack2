#pragma once
#include <array>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <vector>

namespace strand {
// Three slots per source: capture, latest publication, and pinned playback.
constexpr int capacity = 65536;

struct FrequencyTracker {
    double sumFreq = 0.;
    int count = 0;
    double sampleTimer = 0.;
    double sampleRate = 48000.;
    std::atomic<float> displayFreq{0.f};

    FrequencyTracker() {
        reset(48000.);
    }

    FrequencyTracker(const FrequencyTracker& other) {
        *this = other;
    }

    FrequencyTracker& operator=(const FrequencyTracker& other) {
        if (this != &other) {
            sumFreq = other.sumFreq;
            count = other.count;
            sampleTimer = other.sampleTimer;
            sampleRate = other.sampleRate;
            displayFreq.store(other.displayFreq.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }

    void reset(double rate) {
        sumFreq = 0.;
        count = 0;
        sampleTimer = 0.;
        sampleRate = std::max(1.0, rate);
        displayFreq.store(0.f, std::memory_order_relaxed);
    }

    void onCycle(double duration, double rate) {
        if (duration > 0.) {
            double f = rate / duration;
            sumFreq += f;
            count++;
        }
    }

    void step() {
        sampleTimer += 1.;
        if (sampleTimer >= sampleRate) {
            sampleTimer -= sampleRate;
            float freq = (count > 0) ? float(sumFreq / double(count)) : 0.f;
            displayFreq.store(freq, std::memory_order_relaxed);
            sumFreq = 0.;
            count = 0;
        }
    }

    float getFrequency() const {
        return displayFreq.load(std::memory_order_relaxed);
    }
};

struct Cycle {
    std::vector<float> samples = std::vector<float>(capacity);
    int count = 0;
    double start = 0., duration = 0., first = 0.;
    bool forcedStart = false, forcedEnd = false;
    double fade = 0.;
    float rawAt(double p) const {
        if (p <= 0. || p >= duration || count == 0) return 0.f;
        if (p < first) return samples[0] * float(p / first);
        double index = p - first;
        int i = int(index);
        if (i >= count - 1) {
            double last = first + count - 1;
            return samples[count - 1] * float((duration - p) / (duration - last));
        }
        return samples[i] + float(index - i) * (samples[i + 1] - samples[i]);
    }
    float at(double p) const {
        float value = rawAt(p);
        double width = std::min(fade, duration * .25);
        if (width > 0.) {
            if (forcedStart && p < width) value *= float(std::max(0., p) / width);
            if (forcedEnd && duration - p < width) value *= float(std::max(0., duration - p) / width);
        }
        return value;
    }
};
struct Source {
    std::array<Cycle, 3> cycles;
    int writing = -1, latest = -1, pinned = -1;
    float previous = 0.f;
    bool negative = false, fell = false;
    void reset() {
        writing = latest = -1;
        previous = 0.f;
        negative = fell = false;
    }
    double crossing(float x) {
        double alpha = -1.;
        if (negative && previous <= 0.f && x > 0.f) {
            alpha = -double(previous) / (double(x) - previous);
            negative = false;
        }
        if (previous > 0.f && x <= 0.f) fell = true;
        if (x < 0.f) negative = true;
        return alpha;
    }
    double rise(double time, double limit, bool forced = false) {
        double acceptedDuration = 0.;
        if (writing >= 0) {
            Cycle& c = cycles[writing];
            c.duration = time - c.start;
            c.forcedEnd = forced;
            if ((fell || c.forcedStart || forced) && c.count > 0 && c.duration >= 2. && c.duration <= limit) {
                latest = writing;
                if (!forced) {
                    acceptedDuration = c.duration;
                }
            }
        }
        writing = -1;
        for (int i = 0; i < 3; ++i)
            if (i != latest && i != pinned) { writing = i; break; }
        Cycle& c = cycles[writing];
        c.count = 0;
        c.start = time;
        c.forcedStart = forced;
        c.forcedEnd = false;
        c.fade = limit * .005; // 0.5 ms at the ordinary 100 ms capture cap.
        fell = false;
        return acceptedDuration;
    }
    void append(float x, double time, double limit) {
        if (writing >= 0) {
            Cycle& c = cycles[writing];
            if (time - c.start > limit || c.count == capacity) writing = -1;
            else {
                if (c.count == 0) c.first = time - c.start;
                c.samples[c.count++] = x;
            }
        }
        previous = x;
    }
};
struct Lane {
    std::array<Source, 2> sources;
    std::array<FrequencyTracker, 2> freqTrackers;
    int playing = -1;
    double position = 0., tick = 0., limit = 4800.;
    double sampleRate = 48000.;
    void reset(double rate) {
        sampleRate = rate;
        playing = -1; position = tick = 0.;
        limit = std::min(rate * .1, double(capacity - 2));
        for (auto& s : sources) { s.pinned = -1; s.reset(); }
        for (auto& ft : freqTrackers) ft.reset(rate);
    }
    void select() {
        int next = playing < 0 ? 0 : 1 - playing;
        if (sources[next].latest < 0) next = 1 - next;
        if (playing >= 0) sources[playing].pinned = -1;
        playing = sources[next].latest >= 0 ? next : -1;
        if (playing >= 0) sources[playing].pinned = sources[playing].latest;
        position = 0.;
    }
    void advance(double delta) {
        if (playing < 0) select();
        // Accepted cycles are >= 2 samples, so at most one boundary per sample.
        if (playing >= 0) {
            double remaining = sources[playing].cycles[sources[playing].pinned].duration - position;
            if (delta > remaining) {
                delta -= remaining;
                select();
            }
            if (playing >= 0) position += delta;
        }
    }
    float process(float a, float b) {
        float values[2] = {std::isfinite(a) ? a : 0.f, std::isfinite(b) ? b : 0.f};
        struct Event { double alpha; int source; bool forced; };
        std::array<Event, 4> events;
        int eventCount = 0;
        for (int i = 0; i < 2; ++i) {
            double alpha = sources[i].crossing(values[i]);
            if (alpha >= 0.) events[eventCount++] = {alpha, i, false};
            if (sources[i].writing >= 0) {
                double deadline = sources[i].cycles[sources[i].writing].start + limit;
                if (deadline <= tick + 1.)
                    events[eventCount++] = {std::max(0., deadline - tick), i, true};
            }
        }
        // A real crossing wins over a timeout at the same timestamp.
        for (int i = 1; i < eventCount; ++i) {
            Event value = events[i];
            int j = i;
            while (j > 0) {
                const Event& previous = events[j - 1];
                bool before = value.alpha < previous.alpha ||
                    (value.alpha == previous.alpha &&
                     (value.forced < previous.forced ||
                      (value.forced == previous.forced && value.source < previous.source)));
                if (!before) break;
                events[j] = previous;
                --j;
            }
            events[j] = value;
        }
        double elapsed = 0.;
        for (int j = 0; j < eventCount; ++j) {
            const Event& event = events[j];
            Source& source = sources[event.source];
            // An earlier real crossing may have already started a new capture.
            if (event.forced && (source.writing < 0 ||
                source.cycles[source.writing].start + limit > tick + event.alpha)) continue;
            advance(event.alpha - elapsed);
            elapsed = event.alpha;
            double accepted = source.rise(tick + elapsed, limit, event.forced);
            if (accepted > 0.) {
                freqTrackers[event.source].onCycle(accepted, sampleRate);
            }
        }
        advance(1. - elapsed);
        for (int i = 0; i < 2; ++i) sources[i].append(values[i], tick + 1., limit);
        tick += 1.;
        freqTrackers[0].step();
        freqTrackers[1].step();
        if (playing >= 0 && position >= sources[playing].cycles[sources[playing].pinned].duration) select();
        if (playing < 0) select();
        return playing < 0 ? 0.f : sources[playing].cycles[sources[playing].pinned].at(position);
    }
};
} // namespace strand
