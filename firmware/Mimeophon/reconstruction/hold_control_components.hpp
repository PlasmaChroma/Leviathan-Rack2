#pragma once
// Finite MP86 Hold stages and bounded raw-input prefixes, not the whole UI.
#include "event_components.hpp"

namespace mp86_audit {
struct HoldTransport {
    std::uint32_t cursor=0,latched=0;
    void step(std::uint32_t cached_hold,float block_drift,std::uint32_t mask,
              ExpiryPair& a,ExpiryPair& b) noexcept {
        if (cached_hold!=0) {
            a.hold_offset+=block_drift;
            b.hold_offset+=block_drift;
            if (latched==0) {
                a.hold_offset=b.hold_offset=0;
                a.status=b.status=1;
                latched=1;
            }
        } else {
            cursor=(cursor-1u)&mask;
            if (latched==1) {
                latched=0;
                a.status=b.status=1;
            }
        }
    }
};

// Called only on the live Hold == 1 branch. Input delays have already passed
// the ordinary target calculation/clamps and timing-offset adjustment.
inline void retarget_hold(ExpiryPair& a,ExpiryPair& b,float& delay_a,float& delay_b,
                          float anchor,float window_control,float zone_scale,
                          std::uint32_t current_token) noexcept {
    const float margin=(window_control*zone_scale)*8.0f;
    auto wrap=[&](ExpiryPair& pair,float delay) {
        if (pair.status==2) return;
        const float high=(anchor+delay)+margin;
        const float low=margin+anchor;
        if (pair.hold_offset>high) {
            pair.hold_offset=low; pair.status=1;
        } else if (pair.hold_offset<low) {
            pair.hold_offset=high; pair.status=1;
        }
    };
    wrap(a,delay_a);
    wrap(b,delay_b);
    // Both token stores are bypassed when either pair is still fading.
    if (a.status!=2 && b.status!=2) a.seen_token=b.seen_token=current_token;
    delay_a=a.hold_offset;
    delay_b=b.hold_offset;
};

struct HoldRawInput {
    std::uint32_t count=0,hold=0;
    // Stable register snapshots for this bounded poll. Reads in hardware
    // are repeated; pin changes during a poll are not modeled here.
    void poll(std::uint32_t port_40020410,std::uint32_t port_40020810) noexcept {
        const bool first_high=(port_40020410&512u)!=0;
        const bool second_high=(port_40020810&2u)!=0;
        bool toggle=false;
        if (first_high && !second_high) {
            ++count; toggle=count==10;
        } else if (!first_high) {
            count=0;
        } else {
            toggle=count==10;
        }
        if (toggle) {
            if (hold==0) hold=1;
            else if (hold==1) hold=0;
        }
    }
};

enum class ClockPrefixExit { ordinary, short_interval, accepted_handler };
struct ClockRawPrefix {
    std::uint32_t qualification=0,high_count=0;
    // Per four-frame DSP call; accepted_handler is an exit boundary, not a
    // completed clock event. The downstream handler resets qualification.
    ClockPrefixExit poll(std::uint32_t gpio_40021810,std::uint32_t now,
                         std::uint32_t last) noexcept {
        if ((gpio_40021810&1024u)!=0 && qualification==5) {
            ++high_count;
            if (high_count!=1) return ClockPrefixExit::ordinary;
            const auto elapsed=now-last;
            const bool positive_long=elapsed<=0x7fffffffu && elapsed>240;
            return positive_long ? ClockPrefixExit::accepted_handler : ClockPrefixExit::short_interval;
        }
        ++qualification;
        // Signed comparison, including raw wrap behavior.
        if (qualification<=0x7fffffffu && qualification>5) qualification=5;
        high_count=0;
        return ClockPrefixExit::ordinary;
    }
};
} // namespace mp86_audit
