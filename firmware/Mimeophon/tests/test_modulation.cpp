#include "../reconstruction/modulation_components.hpp"
#include "instruction_machine.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include "color_trace.generated.hpp"

constexpr std::uint32_t rng_address=0x20002acc;
constexpr std::uint32_t position_a=0x20001b2c, position_b=0x20002b58;
constexpr std::uint32_t delta_a=0x20004c6c, delta_b=0x20003854;

static void check(const mp86_audit::HaloModulation& state,const Machine& m) {
    assert(state.random_state==m.load(rng_address));
    assert(to_bits(state.a.increment)==m.load(delta_a));
    assert(to_bits(state.b.increment)==m.load(delta_b));
    assert(to_bits(state.a.position)==m.load(position_a));
    assert(to_bits(state.b.position)==m.load(position_b));
}
static void position_trace(Machine& m) {
    // Live-ins established at 0x08024d90/96/98 before the two damping paths.
    m.r[0]=position_a; m.r[4]=delta_a; m.s[11]=from_bits(0x3eb33333);
    trace_mod_positions(m);
}

int main() {
    using namespace mp86_audit;
    HaloModulation state;
    Machine m;
    trace_mod_init(m);
    check(state,m);
    assert(m.load(0x2000387c)==0x2f800000u);
    std::mt19937 fixtures(86);
    // All three clamp branches for each channel, then a deterministic stream
    // of random seeds/states. Tests uint32 conversion, overflow and A/B order.
    std::size_t random_checks=0, position_checks=0;
    // Inverse-LCG fixtures force unsigned conversion endpoints and an exact
    // zero perturbation, including equality with either velocity limit.
    for (std::uint32_t next : {0u,0x7fffffffu,0x80000000u,0xffffffffu}) {
        for (float delta : {-3e-6f,0.0f,3e-6f}) for (bool second : {false,true}) {
            state.random_state=(next-907633515u)*0x2356ae1du;
            m.memory[rng_address]=state.random_state;
            state.a.increment=state.b.increment=delta;
            m.storef(delta_a,delta); m.storef(delta_b,delta);
            state.perturb(second);
            if (second) trace_mod_random_b(m); else trace_mod_random_a(m);
            check(state,m); ++random_checks;
            assert(state.random_state==next);
            if (next==0x80000000u)
                assert(to_bits(second ? state.b.increment : state.a.increment)==to_bits(delta));
        }
    }
    for (int trial=0;trial<8192;++trial) {
        state.random_state=fixtures(); m.memory[rng_address]=state.random_state;
        const float delta=trial%3==0 ? 4.1e-6f : trial%3==1 ? -4.1e-6f
                          : float(int(fixtures()%6001)-3000)*1e-9f;
        state.a.increment=state.b.increment=delta;
        m.storef(delta_a,delta); m.storef(delta_b,delta);
        for (bool second : {false,true}) {
            state.perturb(second);
            if (second) trace_mod_random_b(m); else trace_mod_random_a(m);
            check(state,m); ++random_checks;
        }
    }
    // Exact equality must retain the increment. Overshoot must reset to the
    // crossed boundary plus twice the reversed increment, discarding overshoot.
    for (float p : {0.05f,0.35f,0.1f,std::nextafter(0.05f,0.0f),
                    std::nextafter(0.35f,1.0f)}) {
        for (float delta : {-3e-6f,-1e-6f,0.0f,1e-6f,3e-6f}) {
            state.a={p,delta}; state.b={p,-delta};
            m.storef(position_a,p); m.storef(position_b,p);
            m.storef(delta_a,delta); m.storef(delta_b,-delta);
            state.advance(); position_trace(m); check(state,m); ++position_checks;
        }
    }
    HaloWalk bounce{0.35f,1e-6f}; bounce.advance();
    assert(to_bits(bounce.position)==to_bits(std::fma(-1e-6f,2.0f,0.35f)));

    // Multi-frame drift with different synthetic expiry intervals. Event
    // arbitration is not simulated: these are explicitly supplied triggers.
    state=HaloModulation{}; trace_mod_init(m);
    int turns_a=0,turns_b=0;
    for (int frame=0;frame<400000;++frame) {
        if (frame%97==0) { state.perturb(false); trace_mod_random_a(m); ++random_checks; }
        if (frame%151==0) { state.perturb(true); trace_mod_random_b(m); ++random_checks; }
        const float da=state.a.increment,db=state.b.increment;
        state.advance(); position_trace(m); check(state,m); ++position_checks;
        turns_a+=da!=state.a.increment; turns_b+=db!=state.b.increment;
        assert(state.a.position>=0.05f && state.a.position<=0.35f);
        assert(state.b.position>=0.05f && state.b.position<=0.35f);
    }
    // Directed long trajectory guarantees boundary turns without relying on
    // chance cancellation in the randomly perturbed trajectory above.
    state=HaloModulation{}; trace_mod_init(m);
    for (int frame=0;frame<500000;++frame) {
        const float da=state.a.increment,db=state.b.increment;
        state.advance(); position_trace(m); check(state,m); ++position_checks;
        turns_a+=da!=state.a.increment; turns_b+=db!=state.b.increment;
    }
    assert(turns_a>=2 && turns_b>=2);

    std::size_t counter_checks=0;
    for (float ratio : {1.0f,std::nextafter(1.0f,0.0f),std::nextafter(1.0f,2.0f),0.5f,2.0f}) {
        for (std::uint32_t ca : {0u,1u,2u,0xffffffffu,0x80000000u,0x7fffffffu}) {
            DelayExpiryCounters counters{ca,42};
            m.r[13]=0x20010000;
            m.memory[m.r[13]+28]=0x20003870;
            m.memory[m.r[13]+60]=0x20003868;
            m.memory[m.r[13]+52]=0x20002aac;
            m.storef(0x20003870,ratio);
            m.memory[0x20003868]=ca; m.memory[0x20002aac]=42;
            counters.decrement(ratio); trace_mod_counters(m);
            assert(counters.a==m.load(0x20003868));
            assert(counters.b==m.load(0x20002aac));
            assert(DelayExpiryCounters::expired(counters.a)==(m.comparison<0));
            ++counter_checks;
        }
    }
    // Reload N expires N+1 later decrements, including a zero-delay case.
    for (float delay : {0.0f,1.75f,97.875f}) {
        auto count=DelayExpiryCounters::reload(delay,0.25f);
        const auto n=count;
        for (std::uint32_t i=0;i<n;++i) { --count; assert(!DelayExpiryCounters::expired(count)); }
        --count; assert(DelayExpiryCounters::expired(count));
    }
    std::cout << "PASS: binary-checked modulation initialization; " << random_checks
              << " RNG updates, " << position_checks << " position steps, "
              << counter_checks << " counter cases, all bit-identical; "
              << turns_a << '/' << turns_b << " boundary turns.\n";
}
