#include "../reconstruction/event_components.hpp"
#include "instruction_machine.hpp"
#include <iostream>
#include "color_trace.generated.hpp"

using namespace mp86_audit;
constexpr std::uint32_t stack=0x20010000, head_base=0x20001a14;
static void inputs(Machine& m,const ExpiryInputs& in) {
    m.r[13]=stack;
    for (const auto& binding : std::array<std::array<std::uint32_t,2>,8>{{
            {{0,0x20002b44}},{{8,0x20002ae4}},{{12,0x20002aa4}},
            {{28,0x20003870}},{{52,0x20002aac}},{{60,0x20003868}},
            {{88,0x20004d78}},{{20,0x20004f94}}}})
        m.memory[stack+binding[0]]=binding[1];
    m.memory[stack+16]=in.hold_snapshot;
    m.memory[0x20004d78]=in.clock_mode; m.memory[0x200037f4]=in.flip;
    m.memory[0x20003890]=in.token;
    m.storef(0x200038a0,in.hold_anchor);
    m.storef(0x20003870,in.ratio); m.storef(0x2000388c,in.timing_offset);
    m.storef(0x20002b44,in.delay_a); m.storef(0x20001ad0,in.delay_b);
    m.storef(0x2000387c,0x1p-32f);
}
static void state_memory(Machine& m,const ExpiryEvents& e) {
    m.memory[0x20003868]=e.counters.a; m.memory[0x20002aac]=e.counters.b;
    m.memory[0x20002aa4]=e.a.status; m.memory[0x20002ae4]=e.b.status;
    m.memory[0x20001b28]=e.a.seen_token; m.memory[0x20001b44]=e.b.seen_token;
    m.storef(0x20002aa8,e.a.hold_offset); m.storef(0x20004c70,e.b.hold_offset);
    m.memory[0x20004e80]=e.a.timer; m.memory[0x200055d4]=e.b.timer;
    m.memory[0x20002acc]=e.modulation.random_state;
    m.storef(0x20004c6c,e.modulation.a.increment);
    m.storef(0x20003854,e.modulation.b.increment);
}
static void heads_memory(Machine& m,const std::array<DelayHeadLayout,4>& heads) {
    std::array<std::uint32_t,44> words{};
    static_assert(sizeof(words)==sizeof(heads));
    std::memcpy(words.data(),heads.data(),sizeof(words));
    for (std::size_t i=0;i<words.size();++i) m.memory[head_base+4*i]=words[i];
}
static std::array<DelayHeadLayout,4> fixture_heads(int selection) {
    std::array<DelayHeadLayout,4> heads{};
    for (int i=0;i<4;++i) {
        heads[i].delay=100.0f+i;
        heads[i].moving_delay=11.0f+i;
        heads[i].gain=0.2f+float(i%2)*0.125f;
        heads[i].read_index=1234+i;
        heads[i].buffer_address=0xd0800000u+std::uint32_t(i%2)*0x800000u;
        heads[i].use_moving=7; heads[i].unknown24=1; heads[i].unknown40=55;
    }
    heads[0].selected=selection&1; heads[2].selected=1-heads[0].selected;
    heads[1].selected=(selection>>1)&1; heads[3].selected=1-heads[1].selected;
    heads[2].gain=1-heads[0].gain; heads[3].gain=1-heads[1].gain;
    return heads;
}

int main() {
    std::size_t expiry_cases=0,consume_cases=0,reverse_cases=0;
    // Cartesian truth table includes nonbinary mode/status values to catch
    // accidental boolean normalization of firmware's exact comparisons.
    for (auto sa : {0u,1u,2u,7u}) for (auto sb : {0u,1u,2u,7u})
    for (auto clock : {0u,1u,2u}) for (auto hold : {0u,1u,2u})
    for (auto flip : {0u,1u,2u}) for (int seen=0;seen<4;++seen)
    for (int expires=0;expires<4;++expires) for (float ratio : {1.0f,0.75f}) {
        ExpiryInputs in;
        in.clock_mode=clock; in.hold_snapshot=hold; in.flip=flip;
        in.token=5; in.hold_anchor=260; in.delay_a=128.75f; in.delay_b=64.125f;
        in.timing_offset=0.375f; in.ratio=ratio;
        ExpiryEvents e;
        e.a={sa,(seen&1)?5u:4u,91.25f,19};
        e.b={sb,(seen&2)?5u:4u,81.5f,23};
        e.counters.a=(expires&1)?0u:2u; e.counters.b=(expires&2)?0u:2u;
        e.modulation.random_state=12345u+std::uint32_t(expiry_cases)*100003u;
        e.restart_a=e.restart_b=true; // Must be cleared on non-expiring frames.
        Machine actual; inputs(actual,in); state_memory(actual,e);
        Machine expected=actual;
        e.step(in); state_memory(expected,e);
        trace_expiry_events(actual);
        assert(actual.memory==expected.memory);
        assert(actual.r[6]==std::uint32_t(e.restart_a));
        assert(actual.r[5]==std::uint32_t(e.restart_b));
        ++expiry_cases;
    }

    for (int select=0;select<4;++select)
    for (auto sa : {0u,1u,2u,7u}) for (auto sb : {0u,1u,2u,7u}) {
        auto heads=fixture_heads(select);
        ExpiryInputs in; in.delay_a=345.75f; in.delay_b=456.25f;
        ExpiryEvents e; e.a.status=sa; e.b.status=sb;
        Machine actual; inputs(actual,in); state_memory(actual,e); heads_memory(actual,heads);
        actual.r[11]=head_base; actual.r[3]=sb; actual.r[2]=sa;
        Machine expected=actual;
        consume_requests(heads,e.a.status,e.b.status,in.delay_a,in.delay_b);
        heads_memory(expected,heads); state_memory(expected,e);
        trace_consume_requests(actual);
        assert(actual.memory==expected.memory);
        ++consume_cases;
    }

    // Probe either side and exactly on both reverse thresholds, after +2.
    // Width-four request window is deliberately tested with in-flight fades.
    const std::array<float,11> positions{{-10,0,2,
        std::nextafter(212.0f,0.0f),212.0f,std::nextafter(212.0f,1000.0f),
        214.0f,std::nextafter(216.0f,0.0f),216.0f,
        std::nextafter(216.0f,1000.0f),1000.0f}};
    for (int select=0;select<4;++select) for (int restarts=0;restarts<4;++restarts)
    for (auto sa : {0u,1u,2u}) for (auto sb : {0u,1u,2u})
    for (float pa : positions) for (float pb : positions) {
        auto heads=fixture_heads(select);
        const int ia=heads[1].selected==1?1:3, ib=heads[0].selected==1?0:2;
        heads[ia].delay=heads[ib].delay=100;
        heads[ia].moving_delay=pa-2; heads[ib].moving_delay=pb-2;
        ExpiryEvents e; e.a.status=sa; e.b.status=sb;
        ExpiryInputs in;
        Machine actual; inputs(actual,in); state_memory(actual,e); heads_memory(actual,heads);
        actual.r[11]=head_base; actual.r[1]=ib; actual.r[2]=ia; actual.r[3]=1;
        actual.r[6]=(restarts&1)!=0; actual.r[5]=(restarts&2)!=0;
        Machine expected=actual;
        reverse_window(heads[ib],e.b.status,(restarts&2)!=0);
        reverse_window(heads[ia],e.a.status,(restarts&1)!=0);
        heads_memory(expected,heads); state_memory(expected,e);
        trace_reverse_windows(actual);
        assert(actual.memory==expected.memory);
        ++reverse_cases;
    }

    // Concrete interrupted-fade regression: late reverse request overwrites 2;
    // next consumption toggles heads but retains current complementary gains.
    auto heads=fixture_heads(3);
    std::uint32_t status_a=0,status_b=2;
    heads[0].delay=100; heads[0].moving_delay=212;
    const float gain0=heads[0].gain,gain2=heads[2].gain;
    reverse_window(heads[0],status_b,false);
    assert(status_b==1);
    consume_requests(heads,status_a,status_b,100,123);
    assert(status_b==2 && heads[0].selected==0 && heads[2].selected==1);
    assert(heads[2].delay==123 && heads[0].gain==gain0 && heads[2].gain==gain2);
    std::cout << "PASS: " << expiry_cases << " expiry arbitration cases, "
              << consume_cases << " request-consumption cases, " << reverse_cases
              << " reverse-window cases: complete memory writes and restart flags match "
              << "binary-checked slices; interrupted-fade regression.\n";
}
