#include "../reconstruction/color_feedback_components.hpp"
#include "instruction_machine.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>

#include "color_trace.generated.hpp"

static void close(float actual,float expected) {
    assert(std::isfinite(actual) && std::isfinite(expected));
    if (to_bits(actual)==to_bits(expected)) return;
    if (std::abs(actual-expected)>2e-6f*std::max(1.0f,std::abs(expected))) {
        std::cerr << "Mismatch: " << actual << " versus " << expected << '\n';
        std::abort();
    }
}

int main() {
    using namespace mp86_audit;
    std::mt19937 random(86);
    auto value = [&]() { return float(int(random()%8193)-4096)/4096.0f; };
    std::size_t comparisons=0, exact=0;
    // Both zone coefficient families and every target index; nonzero states
    // exercise feedback, attack/decay, table truncation, and all tap branches.
    for (bool right : {false,true}) for (int trial=0;trial<4096;++trial) {
        ColorChannel channel;
        channel.state.target=trial%255;
        channel.state.position=trial<255 ? float(trial) : float(random()%25400)/100;
        channel.state.coefficient=value()*0.8f;
        for (auto& x:channel.state.previous_input) x=value()*0.2f;
        for (auto& x:channel.state.previous_allpass) x=value()*0.2f;
        channel.envelope=std::abs(value());
        const float input=value(), scale=0.5f+std::abs(value())*3;
        const int family=trial%2;
        std::array<float,4> c{{mp86_tables::zone_polynomial_c0[family],
                              mp86_tables::zone_polynomial_c1[family],
                              mp86_tables::zone_polynomial_c2[family],
                              mp86_tables::zone_polynomial_c3[family]}};
        Machine m;
        const std::uint32_t base=0x20004c74, state=base+(right?44:0);
        const std::uint32_t envelope=right?0x200037fc:0x2000385c;
        m.r[8]=base; m.r[13]=0x20010000;
        m.memory[m.r[13]+84]=0x20002ad4;
        m.memory[m.r[13]+20]=0x20004f94;
        m.memory[0x20004f94]=0;
        for (std::size_t i=0;i<4;++i) m.storef(0x20002ad4+4*i,c[i]);
        for (std::size_t i=0;i<256;++i)
            m.storef(0x0802b5c4+4*i,mp86_tables::color_allpass_coefficient[i]);
        std::array<std::uint32_t,11> words{};
        std::memcpy(words.data(),&channel.state,44);
        for (std::size_t i=0;i<11;++i) m.memory[state+4*i]=words[i];
        m.storef(envelope,channel.envelope);
        m.s[12]=0.5f; m.s[21]=0.001f; m.s[23]=1.144f;
        m.s[25]=0.01f; m.s[29]=176; m.s[31]=from_bits(0x3caaaa99);
        m.s[10]=scale;
        if (right) {
            m.s[15]=input*2; m.s[11]=0;
            m.s[16]=-c[3]; m.s[0]=c[2]; m.s[5]=-c[1]; m.s[9]=c[0];
            trace_right(m);
        } else {
            m.s[11]=input*2; m.s[9]=0;
            trace_left(m);
        }
        const float actual=channel.process(input,scale,c);
        const float expected=m.s[right?9:6];
        close(actual,expected);
        exact += to_bits(actual)==to_bits(expected);
        ++comparisons;
        std::memcpy(words.data(),&channel.state,44);
        for (std::size_t i=1;i<11;++i) close(from_bits(words[i]),m.loadf(state+4*i));
        close(channel.envelope,m.loadf(envelope));
    }

    // Execute actual tail address calculations and writes, including the two
    // intermediate ring stores which are overwritten by allpass outputs.
    for (int trial=0;trial<128;++trial) {
        Machine m;
        m.r[3]=trial%2 ? 32760 : 0; m.r[10]=32767; m.r[14]=0xd0000000;
        m.r[7]=0x20004d24; m.r[9]=0x20004e00; m.r[11]=0x20001a14;
        m.r[13]=0x20010000;
        m.memory[m.r[13]+40]=0; m.memory[m.r[13]+24]=0xd1000000;
        m.memory[m.r[13]+36]=4; m.memory[m.r[13]+76]=0x200038a4;
        m.memory[m.r[13]+4]=0; m.memory[m.r[11]+36]=0xd0800000;
        m.s[3]=0.7f; m.s[22]=0.4f;
        const float gain=0.18f+std::abs(value())*0.16f;
        m.storef(m.r[7],gain);
        std::array<float,8> matrix{};
        for (int i=0;i<8;++i) { matrix[i]=value(); m.storef(m.r[9]+96+4*i,matrix[i]); }
        const std::uint32_t bases[4]={0x20001b6c,0x20002b64,0x200038e8,0x20000278};
        const std::uint32_t cursors[4]={0x20002b38,0x20001b50,0x200038e4,0x20004c58};
        const std::uint32_t lengths[4]={969,803,1236,1511};
        std::array<float,4> delayed{};
        std::array<std::uint32_t,4> indices{};
        const float fractions[2]={0.05f+std::abs(value())*0.3f,0.05f+std::abs(value())*0.3f};
        m.storef(0x20001b2c,fractions[0]); m.storef(0x20002b58,fractions[1]);
        for (int j=0;j<4;++j) {
            indices[j]=trial%2 ? lengths[j]-1 : trial;
            m.memory[cursors[j]]=indices[j];
            for(std::uint32_t k=0;k<lengths[j];++k)
                m.storef(bases[j]+k*4,float(int(k%31)-15)/32);
            if(j<2) {
                const float position=std::fma(fractions[j],float(lengths[j]),float(indices[j]));
                const auto integer=std::uint32_t(position);
                const float t=position-float(integer);
                const float a=m.loadf(bases[j]+4*(integer%lengths[j]));
                const float b=m.loadf(bases[j]+4*((integer+1)%lengths[j]));
                delayed[j]=std::fma(t,b-a,a);
            } else delayed[j]=m.loadf(bases[j]+indices[j]*4);
        }
        std::array<HaloInjectionBranch,2> state{};
        const auto expected=halo_write_values(matrix,{{0,0}},gain,0,delayed,state);
        const auto cursor=m.r[3];
        trace_halo_tail(m);
        const int offsets[8]={0,1839,2242,2663,3129,3689,4252,4862};
        for(int i=2;i<8;++i) close(m.loadf(0xd0000000+4*((cursor+offsets[i])&32767)),expected.ring[i]);
        for(int j=0;j<4;++j) {
            close(m.loadf(bases[j]+4*indices[j]),expected.allpass[j]);
            assert(m.load(cursors[j])==(indices[j]+1)%lengths[j]);
        }
    }

    // Independent checks of the mapped Halo allpass branch destinations.
    std::array<float,8> h{{1,2,3,4,5,6,7,8}};
    std::array<float,4> delayed{{0.2f,-0.1f,0.4f,-0.3f}};
    std::array<HaloInjectionBranch,2> branches{};
    const auto writes=halo_write_values(h,{{0,0}},0.25f,0,delayed,branches);
    close(writes.ring[4],1.25f); close(writes.ring[6],1.75f);
    const int destinations[4]={2,3,5,7};
    for(int i=0;i<4;++i) {
        const float g=i<2?0.7f:0.4f;
        const float input=h[destinations[i]]*0.25f;
        close(writes.allpass[i],input+g*delayed[i]);
        close(writes.ring[destinations[i]],delayed[i]*(1-g*g)-g*input);
    }
    // Settled injected branches reject DC; independent state per channel.
    HaloInjectionBranch branch;
    float dc=0;
    for(int i=0;i<10000;++i) dc=branch.process(0,0.25f,0.2f,0);
    assert(std::abs(dc)<1e-5f);
    assert(final_wet_shape(2)==1 && final_wet_shape(-2)==-1);
    assert(final_mix(0.25f,0,0)==0.25f);
    close(final_mix(0,0.5f,1),0.6875f);
    close(final_mix(0.25f,0,0.5f),0.1875f);
    assert(final_mix(1,1,0.5f)==1);
    assert(color_envelope_scale(0,7)==1.5f);
    assert(color_envelope_scale(1.27f,1)==1.5f);
    close(color_envelope_scale(1.27f,2),2.0985f);
    std::cout << "PASS: " << comparisons << " Color instruction-slice comparisons ("
              << exact << " bit-identical outputs), all Color state words/envelopes, "
                 "128 Halo ring/allpass instruction-slice comparisons, "
                 "Halo branch mapping/DC rejection, final shaping/mix.\n";
}
