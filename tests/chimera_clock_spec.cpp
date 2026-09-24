#include "../src/ChimeraClock.hpp"
#include <cstdio>
#include <cstdlib>

static void need(bool ok, const char* name) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", name); std::exit(1); }
}

int main() {
    chimera::ClockEstimator clock;
    need(!clock.step(true, false, 100).acceptedEdge && !clock.waiting(), "connection has no fabricated edge");
    need(!clock.step(true, false, 48099).acceptedEdge && !clock.waiting(), "initial timeout not early");
    need(clock.step(true, false, 48100).acceptedEdge == false && clock.waiting(), "initial timeout");
    auto edge = clock.step(true, true, 50000);
    need(edge.acceptedEdge && edge.newPhase && !clock.havePeriod() && !clock.waiting(), "first edge sets phase");
    edge = clock.step(true, true, 50001);
    need(!edge.acceptedEdge && edge.tooFast && clock.lastEdgeFrame() == 50000,
         "too-fast edge leaves accepted timestamp unchanged");
    edge = clock.step(true, true, 50002);
    need(edge.acceptedEdge && !edge.newPhase && clock.havePeriod() && clock.periodFrames() == 2,
         "two-frame period is valid after rejected edge");
    edge = clock.step(true, true, 50482);
    need(edge.acceptedEdge && clock.periodFrames() == 480, "third edge directly replaces period");
    clock.step(true, false, 98481);
    need(!clock.waiting(), "period timeout not early");
    clock.step(true, false, 98482);
    need(clock.waiting(), "period timeout reports waiting");
    edge = clock.step(true, true, 98482);
    need(edge.acceptedEdge && !clock.waiting() && clock.periodFrames() == 48000,
         "late edge re-locks without tempo guessing");
    clock.step(true, false, 194481);
    need(!clock.waiting(), "twice-period timeout not early");
    clock.step(true, false, 194482);
    need(clock.waiting(), "twice-period timeout");
    edge = clock.step(true, true, 98482 + chimera::ClockEstimator::kMaxPeriod + 1);
    need(edge.acceptedEdge && edge.newPhase && !clock.havePeriod() && !clock.waiting(),
         "overlong edge becomes new first edge");
    need(!clock.step(false, false, 3000000).acceptedEdge && !clock.connected() &&
         !clock.haveEdge() && !clock.havePeriod() && !clock.waiting(),
         "disconnect clears playback timing");
    edge = clock.step(true, true, 3000001);
    need(edge.acceptedEdge && edge.newPhase && !clock.havePeriod(),
         "reconnection starts a fresh phase without fabricated period");
    need(clock.tooFastCount() == 1, "too-fast diagnostic counted");
    std::puts("PASS: Chimera Clock estimator bounds, timeout, reconnection");
}
