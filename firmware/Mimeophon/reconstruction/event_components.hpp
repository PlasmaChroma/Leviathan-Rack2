#pragma once
// Selected MP86 event stages, not the full GPIO/clock/control state machine.
// See analysis/EVENT_TRANSITIONS.md for ordering and live-in assumptions.
#include "delay_read_components.hpp"
#include "modulation_components.hpp"
#include <array>

namespace mp86_audit {
struct ExpiryPair {
    std::uint32_t status=0;     // 0 idle, 1 request, 2 fading on mapped paths
    std::uint32_t seen_token=0;
    float hold_offset=0;
    std::uint32_t timer=0;
};
struct ExpiryInputs {
    float ratio=1, delay_a=0, delay_b=0, timing_offset=0;
    std::uint32_t clock_mode=0, hold_snapshot=0, flip=0, token=0;
    float hold_anchor=0;
};
struct ExpiryEvents {
    DelayExpiryCounters counters;
    ExpiryPair a,b; // A controls heads 1/3; B controls heads 0/2
    HaloModulation modulation;
    bool restart_a=false,restart_b=false; // transient r6/r5, not RAM latches

    void expire(ExpiryPair& pair,std::uint32_t& count,float delay,
                bool second,const ExpiryInputs& in,bool& restart) noexcept {
        count=DelayExpiryCounters::reload(delay,in.timing_offset);
        pair.timer=720;
        if (pair.status==0 && in.clock_mode==1) {
            if (pair.seen_token!=in.token) {
                pair.seen_token=in.token;
                pair.status=1;
            } else if (in.hold_snapshot==1) {
                pair.hold_offset=in.hold_anchor;
                pair.status=1;
            } else if (in.flip==1) {
                pair.status=1;
                restart=true;
            }
        }
        modulation.perturb(second);
    }
    void step(const ExpiryInputs& in) noexcept {
        counters.decrement(in.ratio);
        restart_a=restart_b=false;
        if (DelayExpiryCounters::expired(counters.a))
            expire(a,counters.a,in.delay_a,false,in,restart_a);
        if (DelayExpiryCounters::expired(counters.b))
            expire(b,counters.b,in.delay_b,true,in,restart_b);
    }
};

// 0x08024288..24298 and out-of-line blocks. B is consumed before A.
// Delays are the current values at this later stage, not necessarily those
// used at expiry; intervening Hold/control code can update them.
inline void consume_request(DelayHeadLayout& first,DelayHeadLayout& second,
                            std::uint32_t& status,float target) noexcept {
    if (status!=1) return;
    if (first.selected==1) {
        second.delay=target;
        first.selected=0; second.selected=1;
    } else {
        first.delay=target;
        first.selected=1; second.selected=0;
    }
    status=2;
    // Neither gains nor moving-delay coordinates reset here.
}
inline void consume_requests(std::array<DelayHeadLayout,4>& heads,
                             std::uint32_t& status_a,std::uint32_t& status_b,
                             float delay_a,float delay_b) noexcept {
    consume_request(heads[0],heads[2],status_b,delay_b);
    consume_request(heads[1],heads[3],status_a,delay_a);
}

// Selected-head reverse stage only. The nonselected heads' earlier +2 update
// at 0x08024298..242e6 is not performed by this helper.
inline void reverse_window(DelayHeadLayout& head,std::uint32_t& status,
                           bool restart) noexcept {
    head.use_moving=1;
    head.moving_delay=std::max(head.moving_delay+2.0f,2.0f);
    const float twice=head.delay+head.delay;
    if (head.moving_delay>twice+16.0f)
        head.moving_delay=2.0f;
    else if (head.moving_delay>twice+12.0f)
        status=1; // Can overwrite a status of 2; preserves gain continuity later.
    else if (restart)
        head.moving_delay=2.0f;
}
} // namespace mp86_audit
