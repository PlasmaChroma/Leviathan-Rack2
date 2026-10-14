#pragma once
// Native scalar re-expressions of selected recovered arithmetic, NOT a complete arbhar engine.
// Evidence status: static reconstruction unless a function explicitly names a numerical probe.
// Address references are per shared object. No vendor binary is executed by this header.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <utility>
#include "recovered_tables.hpp"
namespace arbhar_re {
inline float finiteOrZero(float x) noexcept { return std::isfinite(x) ? x : 0.f; }
inline float clamp01(float x) noexcept { return std::clamp(finiteOrZero(x),0.f,1.f); }
// tanh_approx~ perform 0x2268, and hv_tanh~.pd. It is NOT std::tanh.
// NaN/Inf -> 0 is a Rack safety recommendation, not a recovered firmware branch.
inline float rationalClip(float x, float gain=1.f) noexcept {
 x=std::clamp(finiteOrZero(x),-6.f,6.f);
 const float xx=x*x;
 return finiteOrZero(((x*(27.f+xx))/(27.f+9.f*xx))*gain);
}
// One inspected 4-point branch at play perform 0x49f0..0x4a94.
// a,b,c,d are samples i-1,i,i+1,i+2. Caller must implement boundary policy.
// This alone does NOT reproduce complete resampling/anti-alias behavior.
inline float cubic4(float a,float b,float c,float d,float fraction) noexcept {
 const float f=clamp01(fraction),delta=c-b;
 const float a1=a-d+3.f*delta;
 const float a2=3.f*b-d-2.f*a+a1*f;
 return b+f*(delta+a2*((1.f-f)*0.16666670143604279f));
}
// GPIO _setPlayParameters 0x7e08..0x7e34 / 0x7e90..0x7ebc.
// u is a normalized already-calibrated control, increasing with desired length/spray.
inline std::uint32_t squaredControl(float u) noexcept {
 u=clamp01(u);return static_cast<std::uint32_t>((u*u)*100000.f);
}
// _getPositionRange 0x69d8..0x6a14: nominal-48k sample domain.
inline int durationSamplesFromControl(std::uint32_t units) noexcept {
 const float d=static_cast<float>(static_cast<double>(std::min(units,100000u))*1.4400000000000002);
 return static_cast<int>(std::clamp(d,128.f,144000.f));
}
inline float spraySamplesFromControl(std::uint32_t units) noexcept {
 return static_cast<float>(static_cast<double>(std::min(units,100000u))*4.8000000000000007);
}
// Pd main patch cos oscillators at frequency zero; output pair is (dry, wet).
inline std::pair<float,float> equalPowerMix(float wet) noexcept {
 const float a=clamp01(wet)*1.57079632679489661923f;
 return {std::cos(a),std::sin(a)};
}
// calculateFollowSpeed 0x3f10. Verified against restricted instruction-text probes.
// Mode 0 unidirectional; 1 bidirectional; 2 inverted-control unidirectional.
// "Inverted" here does NOT mean all-negative traversal speed.
// Input is the helper's 12-bit argument (not necessarily the panel ADC orientation).
inline float followSpeed(float input12bit,int mode) noexcept {
 float u=std::clamp(finiteOrZero(input12bit),0.f,4095.f);
 if(mode!=2){u=4095.f-u;if(mode==1)u+=u;}
 if(u<1100.f){const float t=1.f-u/1100.f;return 1.f+19.f*t*t;}
 if(u<1300.f)return 1.f;
 if(u<4095.f)return 1.f-(u-1300.f)/2795.f;
 if(u<=6890.f)return -1.f+(6890.f-u)/2795.f;
 if(u<=7090.f)return -1.f;
 const float t=1.f-(8190.f-u)/1100.f;return -(1.f+19.f*t*t);
}
// Safe translation of makingWndArray at play 0x5b34. Static reconstruction only.
// The binary reads a zero immediately beyond SQUARE and GAUSS[515]=SQUARE[0]=0.
// Explicit guards below reproduce those values without an out-of-bounds C++ read.
using WindowBank=std::array<std::array<float,515>,101>;
inline float edgeTaper(int i) noexcept {
 if(i<10)return static_cast<float>(i*0.1);
 if(i>504)return static_cast<float>((514-i)*0.1);
 return 1.f;
}
inline WindowBank makeWindowBank() noexcept {
 WindowBank out{};
 for(int row=0;row<101;++row){
  const float k=static_cast<float>((row-50)*0.02);
  for(int i=0;i<515;++i){
   float y=0.f;
   if(k<=0.f){
    const float g=(1.f+k)*kGaussian[i];
    const float sq=(i+1<515)?kSquare[i+1]:0.f;
    y=static_cast<float>(static_cast<double>(g)+static_cast<double>(sq)*(-static_cast<double>(k)*0.9));
    y=std::max(0.f,y);
   }else if(i!=0){
    const int gi=std::min(515,static_cast<int>(static_cast<float>(i)+k*200.f));
    const float g=gi<515?kGaussian[gi]:0.f;
    const float blend=(1.f-k)*(1.f+k);
    y=std::clamp(k*kSaw[i]+blend*g,0.f,1.f);
   }
   out[row][i]=y*edgeTaper(i);
  }
 }
 return out;
}
// Phase lookup policy below is a RECOMMENDATION, not recovered full perform behavior.
inline float sampleWindowLinear(const WindowBank& bank,int row,float phase) noexcept {
 row=std::clamp(row,0,100);const float pos=clamp01(phase)*514.f;
 const int i=std::min(513,static_cast<int>(pos));const float f=pos-i;
 return bank[row][i]+f*(bank[row][i+1]-bank[row][i]);
}
} // namespace arbhar_re
