// Standalone streaming FIR microbenchmark, not Vessel::process().
// Reimplements the same symmetric two-channel 2x decimation arithmetic/storage.
#include "FirCoefficients.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <emmintrin.h>

struct Stereo { double left=0.0, right=0.0; };
static_assert(sizeof(Stereo)==2*sizeof(double) && offsetof(Stereo,right)==sizeof(double), "packed stereo samples required");
volatile double checksumSink = 0.0;

template <std::size_t N, bool FourAccumulators>
class Fir2x {
    std::array<Stereo,2*N> ring_{};
    std::array<double,(N+1)/2> coef_{};
    std::size_t cursor_=0;
    bool phase_=false;
public:
    explicit Fir2x(const std::array<double,N>& full) {
        std::copy_n(full.begin(),coef_.size(),coef_.begin());
    }
    bool push(const Stereo& input,Stereo& output) noexcept {
        const std::size_t newest=cursor_;
        ring_[newest]=ring_[newest+N]=input;
        if (++cursor_==N) cursor_=0;
        phase_=!phase_;
        if (phase_) return false;
        const Stereo* lo=ring_.data()+newest+1;
        const Stereo* hi=ring_.data()+newest+N;
        constexpr std::size_t half=(N-1)/2;
        std::size_t i=0;
        __m128d a0=_mm_setzero_pd(),a1=a0,a2=a0,a3=a0;
        auto term=[&](std::size_t k) {
            return _mm_mul_pd(_mm_set1_pd(coef_[k]),
                _mm_add_pd(_mm_loadu_pd(&lo[k].left),_mm_loadu_pd(&hi[-std::ptrdiff_t(k)].left)));
        };
        if (FourAccumulators) {
            for (;i+3<half;i+=4) {
                a0=_mm_add_pd(a0,term(i)); a1=_mm_add_pd(a1,term(i+1));
                a2=_mm_add_pd(a2,term(i+2)); a3=_mm_add_pd(a3,term(i+3));
            }
            a0=_mm_add_pd(_mm_add_pd(a0,a1),_mm_add_pd(a2,a3));
        }
        for (;i<half;++i) a0=_mm_add_pd(a0,term(i));
        a0=_mm_add_pd(a0,_mm_mul_pd(_mm_set1_pd(coef_[half]),_mm_loadu_pd(&lo[half].left)));
        _mm_storeu_pd(&output.left,a0);
        return true;
    }
};
std::vector<Stereo> makeInput(std::size_t size) {
    std::vector<Stereo> input(size);
    std::uint64_t state=UINT64_C(88172645463325252);
    auto next=[&]() {
        state^=state<<13;state^=state>>7;state^=state<<17;
        return 2.0*double(state>>11)/9007199254740992.0-1.0;
    };
    for (auto& s:input) {s.left=next();s.right=next();}
    return input;
}
template<std::size_t N,bool Four>
double measure(const std::array<double,N>& h,const std::vector<Stereo>& input) {
    Fir2x<N,Four> filter(h);Stereo out;double checksum=0.0;
    for (std::size_t i=0;i<4096;++i) filter.push(input[i],out);
    const auto start=std::chrono::steady_clock::now();
    for (const auto& s:input) if(filter.push(s,out)) checksum+=out.left+out.right;
    const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    checksumSink=checksum;
    return elapsed*1e9/(input.size()/2); // ns per emitted stereo host frame.
}
int main() {
    using namespace vessel_audit;
    const auto input=makeInput(1000000);
    Fir2x<129,false> ref(original129);
    Fir2x<129,true> cand(original129);
    double maxError=0.0,squaredError=0.0,squaredReference=0.0;
    Stereo a,b;
    for (const auto& s:input) {
        const bool ra=ref.push(s,a),rb=cand.push(s,b);
        if(ra!=rb) throw std::runtime_error("output cadence mismatch");
        if(ra) {
            maxError=std::max(maxError,std::max(std::abs(a.left-b.left),std::abs(a.right-b.right)));
            squaredError+=(a.left-b.left)*(a.left-b.left)+(a.right-b.right)*(a.right-b.right);
            squaredReference+=a.left*a.left+a.right*a.right;
        }
    }
    std::array<std::vector<double>,5> times;
    for(int repeat=0;repeat<9;++repeat) {
        for(int slot=0;slot<5;++slot) {
            const int which=(repeat%2)?4-slot:slot;
            double ns=0.0;
            switch(which) {
                case 0:ns=measure<129,false>(original129,input);break;
                case 1:ns=measure<129,true>(original129,input);break;
                case 2:ns=measure<105,true>(equiripple105,input);break;
                case 3:ns=measure<93,true>(equiripple93,input);break;
                case 4:ns=measure<109,true>(equiripple109,input);break;
            }
            times[which].push_back(ns);
        }
    }
    const char* names[]={"original129_one_accumulator","original129_four_accumulators",
                         "equiripple105_four_accumulators","equiripple93_four_accumulators","equiripple109_four_accumulators"};
    std::cout<<std::setprecision(12)<<"{\n  \"scope\":\"Standalone streaming FIR; not Vessel callback\",\n";
    std::cout<<"  \"same_filter_max_absolute_output_error\":"<<maxError<<",\n";
    std::cout<<"  \"same_filter_relative_rms_error_db\":"<<10*std::log10(squaredError/squaredReference)<<",\n";
    std::cout<<"  \"measurements_ns_per_stereo_output_frame\":{\n";
    for(int k=0;k<5;++k) {
        std::sort(times[k].begin(),times[k].end());
        std::cout<<"    \""<<names[k]<<"\":{\"min\":"<<times[k].front()<<",\"median\":"<<times[k][4]
                 <<",\"max\":"<<times[k].back()<<"}"<<(k==4?"\n":",\n");
    }
    std::cout<<"  }\n}\n";
}
