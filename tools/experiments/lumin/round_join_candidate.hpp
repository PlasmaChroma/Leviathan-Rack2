#pragma once
#include <algorithm>
#include <cmath>

// Offline experiment only. Inputs are finite unit outward normals, ordered
// counterclockwise over a turn in [0, pi], and ncap is in [2,64]. The caller
// supplies space for ncap samples. No context or production renderer access.
namespace round_join_candidate {
constexpr float pi=3.14159265358979323846f;
struct Normal { float x,y; };
struct Policy {
 int ncap;
 float threshold;
 explicit Policy(int count):ncap(count),threshold(std::cos(2*pi/count)+1e-5f) {}
};
inline int reference(Normal a,Normal b,const Policy& policy,Normal* out) {
 float first=std::atan2(a.y,a.x),last=std::atan2(b.y,b.x);
 if(last<first) last+=2*pi;
 const int count=std::max(2,std::min(policy.ncap,
  int(std::ceil((last-first)/pi*policy.ncap))));
 for(int i=0;i<count;++i) {
  float angle=first+float(i)/float(count-1)*(last-first);
  out[i]={std::cos(angle),std::sin(angle)};
 }
 return count;
}
inline int candidate(Normal a,Normal b,const Policy& policy,Normal* out) {
 // Positive turn and unit-vector guards deliberately send collinear, branch-cut
 // and uncertain inputs to reference. Margin protects the sample-count boundary.
 float dot=a.x*b.x+a.y*b.y,cross=a.x*b.y-a.y*b.x;
 if(cross>1e-6f && std::fabs(a.x*a.x+a.y*a.y-1)<1e-6f &&
    std::fabs(b.x*b.x+b.y*b.y-1)<1e-6f &&
    (policy.ncap==2 || dot>policy.threshold)) {
  out[0]=a;out[1]=b;return 2;
 }
 return reference(a,b,policy,out);
}
}
