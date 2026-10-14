#include "round_join_candidate.hpp"
#include <cstdio>
#include <cstdlib>
using namespace round_join_candidate;
static void check(bool value,const char* message) {
 if(!value) {std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}
}
int main() {
 unsigned long long cases=0,fast=0;double maximum=0;
 for(int cap=2;cap<=64;++cap) {
  Policy policy(cap);
  for(int orientation=0;orientation<129;++orientation) {
   const double start=-3.141592653589793+orientation*(6.283185307179586/128);
   for(int step=0;step<=258;++step) {
    // Dense sweep plus both sides of the two-sample threshold.
    double turn=step<=256?step*(3.141592653589793/256):
     std::min(3.141592653589793,6.283185307179586/cap+(step==257?-1e-6:1e-6));
    Normal a={float(std::cos(start)),float(std::sin(start))};
    Normal b={float(std::cos(start+turn)),float(std::sin(start+turn))};
    Normal expected[64],actual[64];
    int nr=reference(a,b,policy,expected),nc=candidate(a,b,policy,actual);
    check(nr==nc,"arc sample count preserved");
    if(actual[0].x==a.x && actual[0].y==a.y) ++fast;
    for(int i=0;i<nr;++i) {
     double error=std::hypot(double(actual[i].x)-expected[i].x,double(actual[i].y)-expected[i].y);
     maximum=std::max(maximum,error);
     check(error<2e-6,"unit arc error below 2e-6");
    }
    ++cases;
   }
  }
 }
 std::printf("cases=%llu,endpoint_matches=%llu,max_unit_error=%.9g\n",cases,fast,maximum);
}
