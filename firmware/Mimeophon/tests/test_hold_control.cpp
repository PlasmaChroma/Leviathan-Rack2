#include "../reconstruction/hold_control_components.hpp"
#include "instruction_machine.hpp"
#include <iostream>
#include "color_trace.generated.hpp"

using namespace mp86_audit;
constexpr std::uint32_t sp=0x20010000;
static void bind(Machine& m) {
    m.r[13]=sp;
    m.memory[sp]=0x20002b44; m.memory[sp+8]=0x20002ae4;
    m.memory[sp+12]=0x20002aa4;
}
static void pairs(Machine& m,const ExpiryPair& a,const ExpiryPair& b) {
    m.memory[0x20002aa4]=a.status; m.memory[0x20002ae4]=b.status;
    m.memory[0x20001b28]=a.seen_token; m.memory[0x20001b44]=b.seen_token;
    m.storef(0x20002aa8,a.hold_offset); m.storef(0x20004c70,b.hold_offset);
}
static void transport(Machine& m,const HoldTransport& t) {
    m.memory[0x20004c60]=t.cursor; m.memory[0x20004dcc]=t.latched;
}
static std::array<float,7> offsets(float anchor,float delay,float margin) {
    const float lo=margin+anchor,hi=(anchor+delay)+margin;
    return {{std::nextafter(lo,-INFINITY),lo,std::nextafter(lo,INFINITY),
             (lo+hi)*0.5f,std::nextafter(hi,-INFINITY),hi,std::nextafter(hi,INFINITY)}};
}
int main() {
    std::size_t transport_cases=0,retarget_cases=0,input_cases=0,clock_cases=0;
    for (auto cached : {0u,1u,2u}) for (auto latched : {0u,1u,2u})
    for (float drift : {-1.0f,0.0f,1.0f}) for (auto cursor : {0u,1u,0x1fffffu})
    for (auto status : {0u,1u,2u}) {
        HoldTransport t{cursor,latched};
        ExpiryPair a{status,4,12.25f,0},b{status,5,-7.5f,0};
        Machine actual; bind(actual); pairs(actual,a,b); transport(actual,t);
        actual.memory[sp+16]=cached; actual.memory[0x20001b20]=0x1fffff;
        actual.s[24]=drift;
        Machine expected=actual;
        t.step(cached,drift,0x1fffff,a,b);
        pairs(expected,a,b); transport(expected,t);
        trace_hold_transport(actual);
        assert(actual.memory==expected.memory); ++transport_cases;
    }
    for (auto sa : {0u,1u,2u,7u}) for (auto sb : {0u,1u,2u,7u})
    for (float anchor : {0.0f,260.0f}) for (float wc : {0.0f,0.0737930089f,1.27f})
    for (float zs : {0.001f,512.0f,2048.0f}) {
        const float da=1024.25f,db=32.75f,margin=(wc*zs)*8;
        for (float oa : offsets(anchor,da,margin)) for (float ob : offsets(anchor,db,margin)) {
            ExpiryPair a{sa,3,oa,0},b{sb,4,ob,0};
            float out_a=da,out_b=db;
            Machine actual; bind(actual); pairs(actual,a,b);
            actual.storef(0x20002b44,da); actual.storef(0x20001ad0,db);
            actual.storef(0x200038a0,anchor); actual.memory[0x20003890]=9;
            actual.r[3]=1; actual.s[11]=wc; actual.s[5]=zs;
            Machine expected=actual;
            retarget_hold(a,b,out_a,out_b,anchor,wc,zs,9);
            pairs(expected,a,b); expected.storef(0x20002b44,out_a); expected.storef(0x20001ad0,out_b);
            trace_hold_retarget(actual);
            assert(actual.memory==expected.memory);
            assert(actual.r[2]==a.status && actual.r[3]==b.status);
            ++retarget_cases;
        }
    }
    // End-to-end local Hold stages: entering resets offsets and requests both
    // heads, retarget wraps those zeros to the high edge, consumer keeps gains.
    ExpiryPair a{},b{};
    HoldTransport t{321,0};
    t.step(1,-1,0x1fffff,a,b);
    float da=100,db=200;
    retarget_hold(a,b,da,db,260,0.5f,1,6);
    assert(da==364 && db==464 && a.status==1 && b.status==1 && t.cursor==321);
    std::array<DelayHeadLayout,4> heads{};
    heads[0].selected=heads[1].selected=1;
    heads[0].gain=heads[1].gain=0.625f;
    heads[2].gain=heads[3].gain=0.375f;
    consume_requests(heads,a.status,b.status,da,db);
    assert(a.status==2 && b.status==2 && heads[3].delay==364 && heads[2].delay==464);
    assert(heads[0].gain==0.625f && heads[2].gain==0.375f);
    t.step(1,-1,0x1fffff,a,b);
    da=100; db=200;
    retarget_hold(a,b,da,db,260,0.5f,1,7);
    assert(da==363 && db==463 && a.seen_token==6 && b.seen_token==6);
    t.step(0,0,0x1fffff,a,b);
    assert(t.cursor==320 && t.latched==0 && a.status==1 && b.status==1);

    for (auto p1 : {0u,512u,0xfffffdffu,0xffffffffu})
    for (auto p2 : {0u,2u,0xfffffffdu,0xffffffffu})
    for (auto count : {0u,8u,9u,10u,11u,100u,0xffffffffu})
    for (auto hold : {0u,1u,2u}) {
        HoldRawInput state{count,hold};
        Machine actual;
        actual.memory[0x40020410]=p1; actual.memory[0x40020810]=p2;
        actual.memory[0x20005158]=count; actual.memory[0x20004f94]=hold;
        Machine expected=actual; state.poll(p1,p2);
        expected.memory[0x20005158]=state.count; expected.memory[0x20004f94]=state.hold;
        trace_hold_input(actual);
        assert(actual.memory==expected.memory); ++input_cases;
    }
    // Reach count 10 by qualified polling, then exercise the literal chord
    // branch: count stays 10, so each further both-high poll toggles again.
    HoldRawInput raw;
    for (int i=0;i<10;++i) raw.poll(512,0);
    assert(raw.hold==1 && raw.count==10);
    raw.poll(512,2); assert(raw.hold==0 && raw.count==10);
    raw.poll(512,2); assert(raw.hold==1 && raw.count==10);
    raw.poll(0,2); assert(raw.count==0);

    for (auto gpio : {0u,1024u,0xfffffbffu,0xffffffffu})
    for (auto q : {0u,4u,5u,6u,0xffffffffu})
    for (auto high : {0u,1u,2u,0xffffffffu})
    for (auto elapsed : {0u,239u,240u,241u,48000u,0x7fffffffu,0x80000000u,0xffffffffu}) {
        ClockRawPrefix state{q,high};
        const std::uint32_t last=0xffff0000u,now=last+elapsed;
        Machine actual; actual.r[4]=0x20005878;
        actual.memory[0x40021810]=gpio; actual.memory[0x20005878]=now;
        actual.memory[0x20002b48]=last; actual.memory[0x20004c5c]=q;
        actual.memory[0x200056c4]=high;
        Machine expected=actual;
        const auto outcome=state.poll(gpio,now,last);
        expected.memory[0x20004c5c]=state.qualification;
        expected.memory[0x200056c4]=state.high_count;
        trace_clock_prefix(actual);
        assert(actual.memory==expected.memory);
        const auto pc=outcome==ClockPrefixExit::ordinary ? 0x08023b20u
                     : outcome==ClockPrefixExit::short_interval ? 0x08025fb6u : 0x080264e8u;
        assert(actual.exit_pc==pc); ++clock_cases;
    }
    std::cout << "PASS: " << transport_cases << " Hold transport cases, " << retarget_cases
              << " retarget cases, " << input_cases << " raw Hold polls, " << clock_cases
              << " clock-prefix cases; all fixture memory/exit results match binary-checked slices; "
              << "local Hold entry/fade/exit and raw chord regressions.\n";
}
