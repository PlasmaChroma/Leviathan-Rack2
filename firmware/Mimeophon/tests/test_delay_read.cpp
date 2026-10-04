#include "../reconstruction/delay_read_components.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <limits>

int main() {
    using namespace mp86_audit;
    // Independent basis weights, including endpoints and the distinctive midpoint.
    for (int k = 0; k <= 1024; ++k) {
        const float t = float(k) / 1024;
        const std::array<float,4> expected{{t*(t-1)/2, 1-t*(t+1)/2,
                                           t*(3-t)/2, t*(t-1)/2}};
        for (int j = 0; j < 4; ++j) {
            std::array<float,4> x{}; x[j] = 1;
            assert(std::abs(delay_interpolate(x[0],x[1],x[2],x[3],t)-expected[j]) < 1e-7f);
        }
        assert(delay_interpolate(1,1,1,1,t) == 1);
        // A ramp is reproduced exactly for these dyadic test points.
        assert(delay_interpolate(-1,0,1,2,t) == t);
    }
    assert(delay_interpolate(1,0,0,0,0.5f) == -0.125f);

    std::array<float,8> ring{{2,3,4,5,6,7,8,9}};
    DelayHeadLayout head{}; head.delay=2.5f; head.gain=1;
    read_delay_head(head,ring.data(),7,5,0);
    assert(head.read_index==7 && head.fraction==0.5f);
    const float expected = (-ring[6]+5*ring[7]+5*ring[0]-ring[1])/8;
    assert(read_delay_head(head,ring.data(),7,5,0)==expected);
    head.use_moving=1; head.moving_delay=0;
    assert(read_delay_head(head,ring.data(),7,0,0)==ring[2]); // minimum delay 2
    head.use_moving=0; head.delay=2.125f;
    read_delay_head(head,ring.data(),7,2097151,0);
    assert(head.fraction==0); // float32 addition loses sub-ULP fraction here
    ring.fill(std::numeric_limits<float>::quiet_NaN()); head.gain=0.25f;
    assert(read_delay_head(head,ring.data(),7,0,0)==25);

    assert(tracked_delay_step(100,100)==100);
    assert(tracked_delay_step(0,0.0005f)==0.0005f);
    assert(tracked_delay_step(0,1)==28.0f/36.0f);
    assert(tracked_delay_step(0,100)==0x1.c9c69cp+2f);
    assert(tracked_delay_step(0,-100)==-0x1.c9c69cp+2f);
    assert(std::abs(tracked_delay_step(0,64)-tracked_delay_step(0,65))<1e-6f);

    DelayHeadLayout second{}; head.selected=1; head.gain=0; second.gain=1;
    for(int i=0;i<64;++i) advance_head_gains(head,second,1.0f/64);
    assert(head.gain==1 && second.gain==0);
    head.selected=0;
    for(int i=0;i<64;++i) advance_head_gains(head,second,1.0f/64);
    assert(head.gain==0 && second.gain==1);

    StereoInputAttenuation gate;
    float left=0,right=0; gate.process(left,right);
    assert(left==0 && right==0 && gate.envelope==0);
    left=0.025f; right=0.025f; gate.process(left,right);
    assert(std::abs(left-0.025f)<1e-7f);
    gate.envelope=0.00025f; left=0.000125f; right=-left;
    gate.process(left,right);
    assert(std::abs(left-0.00003125f)<1e-10f && right==-left);
    std::cout << "PASS: 4100 interpolation basis cases, DC/ramp, ring wrap, minimum delay, "
                 "float32 coordinates, NaN fallback, nonlinear tracking, crossfade, stereo attenuation.\n";
}
