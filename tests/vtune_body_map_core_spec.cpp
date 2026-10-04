#include "vtune/BodyMapCore.hpp"
#include "vessel/PitchColorMap.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>

using namespace vtune_body;
static bool near(float a, float b, float eps = 2e-5f) { return std::abs(a-b) <= eps; }
static float reportSum(const Weights& w) { return std::accumulate(w.begin(), w.begin()+kBandCount, 0.f); }
static float sum(const Weights& w) { return std::accumulate(w.begin(), w.end(), 0.f); }

int main() {
    for (int i = 0; i < vessel_pitch_color::count; ++i) {
        for (float octave : {.25f, .5f, 1.f, 2.f, 4.f, 8.f}) {
            const auto w = vessel_pitch_color::weights(vessel_pitch_color::centers[i] * octave);
            assert(near(w[i], 1.f));
            assert(near(std::accumulate(w.begin(), w.end(), 0.f), 1.f));
        }
        if (i < 6) {
            const auto w = vessel_pitch_color::weights(.5f * (vessel_pitch_color::centers[i]
                + vessel_pitch_color::centers[i + 1]));
            assert(near(w[i], .5f) && near(w[i + 1], .5f));
        }
    }
    for (float invalid : {0.f, -1.f, std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::quiet_NaN()}) {
        const auto w = vessel_pitch_color::weights(invalid);
        assert(std::accumulate(w.begin(), w.end(), 0.f) == 0.f);
    }
    std::cout << "PASS: shared bowl/chakra centers, octave equivalence, crossfades and invalid inputs.\n";
    std::size_t samples = 0;
    // Dense logarithmic sweep: normalized weights, boundedness, support and
    // at most two report layers at steady state (not while crossfading in time).
    for (int n=0; n<=100000; ++n) {
        float f = kMinHz * std::exp2((n / 100000.f) * std::log2(kMaxHz / kMinHz));
        if (n==100000) f=kMaxHz;
        const auto w=evaluate(f,Mode::Report);
        assert(near(reportSum(w),1.f));
        int active=0;
        for (std::size_t i=0;i<kBandCount;++i) {
            assert(std::isfinite(w[i]) && w[i]>=0.f && w[i]<=1.f);
            if (w[i]>1e-5f) ++active;
        }
        assert(active>=1 && active<=2);
        ++samples;
    }
    for (std::size_t i=0; i+1<kBandCount; ++i) {
        const float b=kBands[i].highHz;
        const auto mid=evaluate(b,Mode::Report);
        assert(near(mid[i],.5f) && near(mid[i+1],.5f));
        const auto lo=evaluate(b*(1.f-1e-6f),Mode::Report);
        const auto hi=evaluate(b*(1.f+1e-6f),Mode::Report);
        for (std::size_t k=0; k<kBandCount; ++k) assert(near(lo[k],hi[k],1e-4f));
    }
    assert(evaluate(20.f,Mode::Report)[0]==1.f);
    assert(evaluate(2000.f,Mode::Report)[6]==1.f);
    for (float bad : {0.f,-1.f,19.99f,2000.01f,
            std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
        assert(sum(evaluate(bad,Mode::Report))==0.f);
    }
    assert(sum(evaluate(528.f,Mode::Off))==0.f);
    assert(sanitizeMode(100)==Mode::Report && sanitizeMode(-1)==Mode::Report);
    assert(sanitizeMode(1)==Mode::Report && sanitizeMode(2)==Mode::Report);
    assert(sanitizeMode(3)==Mode::Off);
    // Equal wall-clock times give the same interpolation at different UI FPS.
    Animation a,b;
    const auto target=evaluate(45.f,Mode::Report);
    for(int i=0;i<30;++i) a.advance(target,1.0/30.0);
    for(int i=0;i<144;++i) b.advance(target,1.0/144.0);
    for(std::size_t i=0;i<kBandCount;++i) assert(near(a.weights()[i],b.weights()[i]));
    // Leap: no invented intervening-band activity.
    Animation leap; leap.snap(evaluate(25.f,Mode::Report));
    leap.advance(evaluate(1500.f,Mode::Report),1.0/60.0);
    assert(leap.weights()[0]>0.f && leap.weights()[6]>0.f);
    for (std::size_t i=1;i<6;++i) assert(leap.weights()[i]==0.f);
    for(int i=0;i<120;++i) leap.advance({},1.0/60.0);
    assert(sum(leap.weights())<1e-5f);
    const auto square=aspectFit(100.f,100.f);
    assert(near(square.height,100.f));
    assert(near(square.width/square.height,kSourceWidth/kSourceHeight));
    assert(near(2*square.x+square.width,100.f));
    const auto exact=aspectFit(788.f,1002.f);
    assert(exact.x==0.f && exact.y==0.f && exact.width==788.f && exact.height==1002.f);
    assert(aspectFit(0.f,100.f).width==0.f);
    assert(aspectFit(std::numeric_limits<float>::infinity(),100.f).width==0.f);
    std::cout << "PASS: " << samples << " frequency sweep samples; boundaries, invalid values, "
                 "saved mode migration, frame-rate independence, jump/disconnect fades, and aspect fit.\n";
}
