#include "../src/OctaviaPresence.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>

int main() {
    using S = OctaviaPresenceState;
    OctaviaPresence p;
    assert(p.state(false, false, 0) == S::Sleeping);
    assert(p.state(false, true, 1) == S::Error);
    p.reset();
    assert(p.state(true, false, 0) == S::Idle);
    assert(p.activity("GET", "/modules") == S::Inspecting);
    assert(p.activity("POST", "/parameters") == S::Working);
    assert(p.activity("POST", "/audio/analyze") == S::Thinking);
    assert(p.activity("POST", "/audio/compare") == S::Thinking);
    for (const auto& path : {"/status", "/presence", "/console/1/workers/w/heartbeat"})
        assert(p.activity("POST", path) == S::Idle);
    p.pulse(S::Inspecting, 100);
    assert(p.state(true, false, 100) == S::Idle); // settle first
    assert(p.state(true, false, 2000) == S::Inspecting);
    p.pulse(S::Working, 2100);
    p.pulse(S::Thinking, 2200);
    assert(p.state(true, false, 2200) == S::Inspecting);
    assert(p.state(true, false, 4000) == S::Thinking);
    assert(p.state(true, false, 5999) == S::Thinking); // expired, still settling
    assert(p.state(true, false, 6000) == S::Idle);
    p.pulse(S::Error, 6100);
    assert(p.state(true, false, 8000) == S::Error);
    assert(p.state(false, false, 8001) == S::Sleeping); // stop bypasses dwell

    p.reset();
    p.setOverride(S::Thinking, 30000, 10000);
    auto status = p.status(true, false, 10000);
    assert(status.target == S::Thinking && status.remainingMs == 30000);
    p.pulse(S::Working, 10001);
    assert(p.state(true, false, 12000) == S::Thinking); // incidental HTTP cannot steal override
    assert(p.status(true, false, 12000).automatic == S::Working);
    assert(p.setOverride(S::Working, 1000, 12001)); // intentional edit bypasses dwell
    assert(p.state(true, false, 12001) == S::Working);
    assert(p.setOverride(S::Working, 1000, 12500)); // renew same state
    assert(p.status(true, false, 13002).remainingMs == 498);
    assert(p.status(true, false, 13500).remainingMs == 0);
    assert(p.state(true, false, 14000) == S::Idle);
    p.setOverride(S::Thinking, 1000, 15000);
    assert(!p.setOverride(S::Error, 999, 15000));
    assert(!p.setOverride(S::Error, 300001, 15000));
    assert(p.state(true, false, 15000) == S::Thinking);
    p.releaseOverride();
    assert(p.state(true, false, 15001) == S::Idle);
    p.setOverride(S::Working, 1000, 16000);
    assert(p.state(false, false, 16001) == S::Sleeping);
    assert(p.status(true, false, 16002).remainingMs == 0); // stop clears lease
    p.setOverride(S::Thinking, 1000, 17000);
    assert(p.state(false, true, 17001) == S::Error);
    p.reset();
    assert(p.state(true, false, 18000) == S::Idle);
    // A delayed concurrent pulse cannot shorten a newer pulse.
    std::thread late([&] { p.pulse(S::Inspecting, 20000); });
    std::thread early([&] { p.pulse(S::Inspecting, 19000); });
    late.join(); early.join();
    assert(p.state(true, false, 22499) == S::Inspecting);

    OctaviaPresenceFade fade;
    assert(!fade.update(S::Idle, 0));
    assert(fade.update(S::Thinking, 0));
    fade.update(S::Thinking, 175);
    assert(std::abs(fade.weights()[0] - .5f) < .0001f);
    assert(std::abs(fade.weights()[2] - .5f) < .0001f);
    const auto midpoint = fade.weights();
    fade.update(S::Working, 175); // interrupted transition is continuous
    assert(fade.weights() == midpoint);
    fade.update(S::Working, 350);
    float sum = 0.f;
    for (float weight : fade.weights()) { assert(weight >= 0.f); sum += weight; }
    assert(std::abs(sum - 1.f) < .0001f);
    fade.update(S::Working, 525);
    assert(fade.weights()[3] == 1.f);
    assert(!fade.update(S::Working, 526)); // stop framebuffer redraws at rest
    std::cout << "PASS: presence dwell, priority, leases, expiry, reset, concurrent pulses and interrupted fades\n";
}
