#include "Tiamat/TiamatBuffer.hpp"
#include "Tiamat/TiamatBufferEngine.hpp"
#include "Tiamat/TiamatMath.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <sstream>
#include <string>
#include <vector>

static bool watchAllocations = false;
static unsigned audioAllocations = 0, audioDeletes = 0;
void* operator new(std::size_t n) {
    if (watchAllocations) ++audioAllocations;
    void* p = std::malloc(n ? n : 1);
    if (!p) throw std::bad_alloc();
    return p;
}
void* operator new[](std::size_t n) { return ::operator new(n); }
__attribute__((noinline)) void operator delete(void* p) noexcept { if (watchAllocations && p) ++audioDeletes; std::free(p); }
void operator delete[](void* p) noexcept { ::operator delete(p); }

namespace tiamat {
struct BufferTestAccess {
    static BufferChannel& ch(Buffer& b, unsigned c) { return b.channels_[c]; }
    static TransitionState& transition(Buffer& b) { return b.transition_; }
    static float* plane(Buffer& b, unsigned c) { return b.memory_.get() + c * b.capacity_; }
    static void process(Buffer& b, const float* x, float* y, unsigned frames, const BufferControls& c, Random& rng) {
        b.processFrames(x, y, frames, c, rng);
    }
    static void observe(Buffer& b, void (*fn)(const Buffer&, unsigned, void*), void* data) {
        b.observer_ = fn; b.observerData_ = data;
    }
};
}
using namespace tiamat;

static std::uint32_t bits(float x) { std::uint32_t b; std::memcpy(&b, &x, 4); return b; }
static const char* fields[] = {"N", "segment", "start", "end", "audible", "candidate", "bankW", "bankR",
    "repeats", "rpos", "wpos", "rate", "frozen", "requested", "resize"};
using State = std::array<double, 15>;
static State snapshot(const Buffer& b, unsigned c) {
    const auto& ch = b.channel(c);
    return {{double(ch.writer.captureFrames), double(ch.reader.sliceFrames), double(ch.reader.sliceStart),
        double(ch.reader.sliceStart + ch.reader.sliceFrames), double(ch.audibleFrames), double(ch.candidate),
        double(ch.writer.writeBank), double(ch.reader.anchor), double(ch.subdivisions), double(ch.reader.position),
        double(ch.writer.writePosition), double(ch.reader.rate), double(b.transition().freezeActive),
        double(b.transition().freezeRequested), double(ch.resizePending)}};
}
static State loadState(std::istream& in) {
    State s;
    for (auto& x : s) { in >> x; assert(in); }
    return s;
}
static void compare(const State& actual, const State& expected, const std::string& label) {
    for (unsigned i = 0; i < actual.size(); ++i) {
        if (actual[i] != expected[i]) {
            std::cerr << label << ": " << fields[i] << " got " << actual[i] << ", expected " << expected[i] << '\n';
            std::abort();
        }
    }
}
static void tag(std::istream& in, const char* expected) {
    std::string value; in >> value; assert(value == expected);
}
static void freeze(Buffer& b, unsigned anchor = 0) {
    BufferTestAccess::transition(b).freezeActive = true;
    for (unsigned c = 0; c < 2; ++c) BufferTestAccess::ch(b, c).reader.anchor = anchor;
}
static void fill(Buffer& b, float value) {
    for (unsigned c = 0; c < 2; ++c)
        std::fill(BufferTestAccess::plane(b, c), BufferTestAccess::plane(b, c) + b.capacity(), value);
}

struct Event { unsigned channel, frame; State state; };
struct EventCheck { std::vector<Event> expected; unsigned compared = 0; };
static void observeEvent(const Buffer& b, unsigned ch, void* ptr) {
    auto& check = *static_cast<EventCheck*>(ptr);
    for (const auto& event : check.expected) {
        if (event.channel == ch && event.frame == b.channel(ch).sampleCounter) {
            compare(snapshot(b, ch), event.state, "event ch=" + std::to_string(ch) + " frame=" + std::to_string(event.frame));
            ++check.compared;
        }
    }
}

static void readers(std::istream& in) {
    unsigned length, count; float rate;
    in >> length >> rate >> count;
    std::vector<unsigned> wraps(count);
    for (auto& w : wraps) in >> w;
    std::array<std::uint32_t, 16> samples;
    for (auto& s : samples) in >> s;
    std::vector<float> plane(length);
    for (unsigned i = 0; i < length; ++i) plane[i] = float(i);
    ReaderState r; r.sliceFrames = length; r.rate = rate; r.position = rate > 0 ? 0 : float(length - 1);
    unsigned checked = 0;
    for (unsigned frame = 0; frame < 160; ++frame) {
        const float old = r.position;
        const auto result = readLinear(plane.data(), length, r, rate, 1, 1, true, 0, length);
        assert(result.sample == old);
        if (frame < samples.size()) assert(bits(result.sample) == samples[frame]);
        const bool wrap = old + rate < 0 || old + rate > length - 1;
        assert(wrap == result.wrapped);
        assert(r.position == (wrap ? (rate < 0 ? float(length - 1) : 0.f) : old + rate));
        if (result.wrapped && checked < count) assert(frame == wraps[checked++]);
    }
    assert(checked == count);
}
static void windows(std::istream& in) {
    unsigned n, count; float w; in >> n >> w >> count;
    Buffer b(32768, 48000, n); Random rng;
    BufferControls c; c.frequency = 48000.f / n; c.window = w; c.freezeRequested = true; c.unique = false;
    freeze(b); fill(b, 1);
    std::vector<float> x(n * 2, 0), y(n * 2);
    BufferTestAccess::process(b, x.data(), y.data(), n, c, rng);
    for (unsigned i = 0; i < count; ++i) {
        tag(in, "W"); unsigned frame; std::uint32_t expected; in >> frame >> expected;
        if (bits(y[2 * frame]) != expected) {
            std::cerr << "window N=" << n << " w=" << w << " frame=" << frame << " bits=" << bits(y[2 * frame]) << " expected=" << expected << '\n';
            std::abort();
        }
        assert(bits(y[2 * frame + 1]) == expected);
    }
}
static void engines(std::istream& in) {
    unsigned id, exponent, draws, count; float silence, traverse;
    in >> id >> exponent >> silence >> traverse >> draws >> count;
    EventCheck check;
    for (unsigned i = 0; i < count; ++i) {
        tag(in, "E"); Event event; in >> event.channel >> event.frame; event.state = loadState(in); check.expected.push_back(event);
    }
    tag(in, "FINAL"); State final = loadState(in);
    Buffer b(32768, coefficientRate, 1000); Random rng, expectedRng;
    BufferControls c; c.frequency = coefficientRate / 1000.f; c.repeatsExponent = exponent;
    c.silence = silence; c.traverse = traverse; c.unique = false;
    for (unsigned ch = 0; ch < 2; ++ch) BufferTestAccess::ch(b, ch).reader.rate = 0;
    BufferTestAccess::observe(b, observeEvent, &check);
    std::array<float, 512> x, y;
    for (unsigned block = 0; block < 8; ++block) {
        for (unsigned i = 0; i < 256; ++i) x[2*i] = x[2*i+1] = float(std::sin(double(block * 256 + i) * .2));
        BufferTestAccess::process(b, x.data(), y.data(), 256, c, rng);
    }
    compare(snapshot(b, 0), final, "engine " + std::to_string(id));
    assert(check.compared == count);
    for (unsigned i = 0; i < draws; ++i) expectedRng.next();
    assert(rng.state() == expectedRng.state());
}
static void histories(std::istream& in) {
    unsigned anchor, changed, cursor; in >> anchor >> changed >> cursor;
    tag(in, "H"); std::array<std::uint32_t, 24> expected;
    for (auto& x : expected) in >> x;
    tag(in, "FINAL"); State final = loadState(in);
    Buffer b(32768, coefficientRate, 1200); Random rng;
    BufferControls c; c.frequency = coefficientRate / 1200.f; c.baseRate = .125f; c.window = 0;
    c.freezeRequested = true; c.unique = false;
    freeze(b, anchor); fill(b, .2f);
    for (unsigned ch = 0; ch < 2; ++ch) {
        BufferTestAccess::ch(b, ch).reader.rate = .125f;
        BufferTestAccess::ch(b, ch).writer.historyPosition = 32766;
    }
    std::array<float, 192> x, y; x.fill(.8f);
    b.processBlock(x.data(), y.data(), c, rng);
    for (unsigned i = 0; i < expected.size(); ++i) assert(bits(y[i]) == expected[i]);
    unsigned actualChanged = 0;
    for (unsigned i = 0; i < 4800; ++i) if (BufferTestAccess::plane(b, 0)[i] != .2f) ++actualChanged;
    assert(actualChanged == changed && b.channel(0).writer.historyPosition == cursor);
    compare(snapshot(b, 0), final, "history anchor=" + std::to_string(anchor));
}
static void timeTrajectories(std::istream& in) {
    float speed; bool frozen, clockRequests; unsigned bank;
    in >> speed >> frozen >> bank >> clockRequests;
    Buffer b(32768, coefficientRate, 2400); Random rng;
    std::ofstream trace;
    if (std::getenv("TIAMAT_TRACE") && speed == 1.f && frozen && bank == 0) {
        trace.open("build/tiamat-native-trace.txt");
        BufferTestAccess::observe(b, [](const Buffer& engine, unsigned ch, void* ptr) {
            auto& stream = *static_cast<std::ofstream*>(ptr);
            stream << ch << ' ' << engine.channel(ch).sampleCounter;
            for (double v : snapshot(engine, ch)) stream << ' ' << v;
            stream << ' ' << bits(engine.frequency()) << ' ' << bits(engine.channel(ch).reader.previousSample)
                << ' ' << bits(engine.channel(ch).priorSample) << '\n';
        }, &trace);
    }
    BufferControls c; c.window = 0; c.baseRate = speed; c.freezeRequested = frozen; c.unique = false;
    BufferTestAccess::transition(b).freezeActive = frozen;
    for (unsigned ch = 0; ch < 2; ++ch) {
        BufferTestAccess::ch(b, ch).reader.rate = speed;
        BufferTestAccess::ch(b, ch).reader.anchor = bank * 2400;
        for (unsigned i = 0; i < b.capacity(); ++i)
            BufferTestAccess::plane(b, ch)[i] = float(((i / 32) % 2 ? -1 : 1) * (.1 + .5 * i / b.capacity()));
    }
    float smoothMs = float(1000. * 2400. / 96028.);
    double phase = 0;
    const unsigned lengths[] = {2400, 4800, 1200, 2400}, blocks[] = {32, 256, 256, 256};
    const float markers[] = {.71f, .75f, .8f, .85f};
    unsigned totalFrames = 0;
    for (unsigned stage = 0; stage < 4; ++stage) {
        tag(in, "STAGE"); std::string name; unsigned expectedChanged, expectedGuards, count;
        in >> name >> expectedChanged >> expectedGuards >> count;
        struct Check { unsigned frame; bool guard; std::array<State, 2> states; };
        std::vector<Check> checks(count);
        for (auto& check : checks) {
            tag(in, "CHECK"); in >> check.frame >> check.guard;
            for (auto& s : check.states) { tag(in, "FINAL"); s = loadState(in); }
        }
        std::vector<float> before(BufferTestAccess::plane(b, 0), BufferTestAccess::plane(b, 0) + 4800);
        unsigned guards = 0;
        for (unsigned block = 0; block < blocks[stage]; ++block) {
            const float target = coefficientRate / float(lengths[stage]);
            const unsigned periodUs = unsigned(1000000. / target);
            const float periodMs = float(periodUs) * .001f;
            bool pending = false;
            for (unsigned i = 0; i < 96; ++i) {
                smoothMs = multiplyAdd(periodMs, .00013083219528198242f, smoothMs * .999869167804718f);
                c.timeChanging = std::abs(periodMs - smoothMs) > 5.f;
                phase += double(target) / 96028.;
                if (phase >= 1) { phase -= 1; if (!c.timeChanging) pending = true; }
            }
            guards += c.timeChanging;
            c.frequency = 1.f / (float(periodUs) * .000001f);
            c.clockRequest = pending && clockRequests;
            std::array<float, 192> x, y; x.fill((block / 4) % 2 ? -markers[stage] : markers[stage]);
            b.processBlock(x.data(), y.data(), c, rng);
            if (trace.is_open()) trace.flush();
            totalFrames += 96;
            for (const auto& check : checks) if (check.frame == totalFrames) {
                assert(check.guard == c.timeChanging);
                for (unsigned ch = 0; ch < 2; ++ch)
                    compare(snapshot(b, ch), check.states[ch], "time " + name + " speed=" + std::to_string(speed)
                        + " frozen=" + std::to_string(frozen) + " bank=" + std::to_string(bank)
                        + " frame=" + std::to_string(totalFrames) + " ch=" + std::to_string(ch));
            }
            for (float value : y) assert(std::isfinite(value));
        }
        unsigned changed = 0;
        for (unsigned i = 0; i < 4800; ++i) changed += before[i] != BufferTestAccess::plane(b, 0)[i];
        if (changed != expectedChanged || guards != expectedGuards) {
            std::cerr << "time " << name << " speed=" << speed << " frozen=" << frozen << " bank=" << bank
                << " changed=" << changed << "/" << expectedChanged << " guards=" << guards << "/" << expectedGuards << '\n';
            std::abort();
        }
    }
}

static void fixtures() {
    std::ifstream file("tests/fixtures/tiamat/buffer_v1.txt"); assert(file);
    std::string line; std::ostringstream stripped;
    while (std::getline(file, line)) if (!line.empty() && line[0] != '#') stripped << line << '\n';
    std::istringstream in(stripped.str()); std::string type;
    unsigned readerCount = 0, windowCount = 0, engineCount = 0, historyCount = 0, timeCount = 0;
    while (in >> type) {
        if (type == "READ") { readers(in); ++readerCount; }
        else if (type == "WINDOW") { windows(in); ++windowCount; }
        else if (type == "ENGINE") { engines(in); ++engineCount; }
        else if (type == "HISTORY") { histories(in); ++historyCount; }
        else if (type == "TIME") { timeTrajectories(in); ++timeCount; }
        else assert(false);
    }
    assert(readerCount == 48 && windowCount == 8 && engineCount == 7 && historyCount == 2 && timeCount == 10);
    std::cout << "Exact fixtures: 7680 reader calls, 8 windows, 7 engine trajectories, 2 history overlaps, 10 Time trajectories\n";
}

static void endpoints() {
    assert(bits(multiplyAdd(.001f, -19.92576026916504f, 39.9317626953125f)) == 1109370297u);
    Buffer b; Random rng;
    for (unsigned ch = 0; ch < 2; ++ch)
        for (unsigned i = 0; i < b.capacity(); ++i) assert(BufferTestAccess::plane(b, ch)[i] == 0);
    std::array<float, 4> ramp {{0, 1, 2, 3}};
    ReaderState r; r.anchor = 3; r.position = 1; r.sliceFrames = 4;
    assert(readLinear(ramp.data(), 4, r, 0, 1, 1, true, 0, 2).sample == 0); // absolute == capacity
    r.position = 1.5f;
    assert(readLinear(ramp.data(), 4, r, 0, 1, 1, true, 0, 2).sample == .5f);
    r.anchor = 0; r.position = 0; r.rate = 1;
    auto y = readLinear(ramp.data(), 4, r, -32, 1, .5f, true, 0, 2);
    assert(y.sample == 0 && r.rate == -3.5f && r.position == 3 && y.wrapped);
    for (unsigned a = 0; a < 24; ++a) {
        assert(sliceWindow(a, a, 1) == 0);
        for (unsigned p = 0; p < a; ++p) {
            const float gain = sliceWindow(a, p, 1);
            assert(std::isfinite(gain) && gain >= 0 && gain <= 1);
        }
    }
    assert(sliceWindow(1, 0, 1) == 1);
    assert(sliceWindow(2, 0, 1) == 0 && sliceWindow(2, 1, 1) == 1);
    assert(traverseIndex(4, .5f, 0) == 1 && traverseIndex(4, .5f, 2) == 2);
    assert(traverseIndex(256, 1, 0) == 255);
    for (unsigned size : {1u, 2u, 8u, 25u, maxCaptureFrames}) {
        Buffer small(size == maxCaptureFrames ? planeFrames : 128, coefficientRate, size);
        freeze(small); fill(small, 1);
        BufferControls c; c.frequency = coefficientRate / size; c.freezeRequested = true; c.window = .02f; c.silence = 1;
        std::array<float, 192> x, out; x.fill(.5f);
        for (unsigned i = 0; i < 16; ++i) {
            small.processBlock(x.data(), out.data(), c, rng);
            for (float sample : out) assert(sample == 0);
        }
        if (size == maxCaptureFrames) {
            assert(small.channel(0).writer.historyPosition == 0);
            assert(BufferTestAccess::plane(small, 0)[planeFrames - 1] == 1);
        }
    }
    // No processing allocation/deallocation, including rapid Time and direction
    // changes. Construction, complete zeroing and destruction are outside guard.
    BufferControls c; std::array<float, 192> x {}, out;
    double peakUs = 0, totalUs = 0;
    watchAllocations = true;
    for (unsigned i = 0; i < 2000; ++i) {
        c.frequency = coefficientRate / float(1 + (i * 197) % 100000);
        c.baseRate = (i % 2 ? -8.f : .125f); c.timeChanging = i % 3 == 0;
        c.clockRequest = i % 7 == 0; c.freezeRequested = i % 11 == 0;
        c.repeatsExponent = i % 9; c.silence = float(i % 5) * .25f;
        const auto start = std::chrono::steady_clock::now();
        b.processBlock(x.data(), out.data(), c, rng);
        const double us = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count();
        totalUs += us; peakUs = std::max(peakUs, us);
        for (float sample : out) assert(std::isfinite(sample));
        for (unsigned ch = 0; ch < 2; ++ch) {
            const auto& state = b.channel(ch);
            assert(state.writer.captureFrames >= 1 && state.writer.captureFrames <= maxCaptureFrames);
            assert(state.reader.sliceFrames >= 1 && state.reader.sliceStart + state.reader.sliceFrames <= planeFrames);
        }
    }
    watchAllocations = false;
    assert(!audioAllocations && !audioDeletes);
    std::cout << "Buffer 96-frame blocks: mean " << totalUs / 2000 << " us, max " << peakUs << " us; no audio allocations/deletions\n";
    BufferEngine engine;
    BufferBlockInput block;
    engine.eventState().controls.macroBend = engine.eventState().controls.macroBreak = true;
    block.primary.bend = .9f; block.primary.brk = .8f; block.primary.repeats = .4f;
    watchAllocations = true;
    for (unsigned i = 0; i < 2000; ++i) {
        block.primary.time = float(i % 101) / 100.f;
        block.restartRandom = i % 113 == 0;
        block.restoreSecondary = i == 999;
        engine.processBlock(x.data(), out.data(), block);
        for (float sample : out) assert(std::isfinite(sample));
    }
    watchAllocations = false;
    assert(!audioAllocations && !audioDeletes);
    std::cout << "Headless event/clock/buffer composition: no processing allocations/deletions\n";
}

int main() { fixtures(); endpoints(); std::cout << "Tiamat buffer tests passed\n"; }
