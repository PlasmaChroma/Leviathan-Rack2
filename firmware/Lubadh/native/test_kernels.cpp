#include "recovered_kernels.hpp"
#include <cassert>
#include <iostream>
#include <vector>
using namespace lubadh_research;
static unsigned checks=0;
void near(float a,float b,float tol=2e-6f){++checks;if(std::abs(a-b)>tol){std::cerr<<a<<" != "<<b<<'\n';std::abort();}}
int main(){
    for(float k:{0.f,.0099f,.01f,.0101f,.3f,.9f,1.f}) for(float c:{0.f,.2f,1.f}){
        SoftClipper sc{k,c};near(sc.process(0),0);
        for(int i=1;i<=256;++i){float x=i/64.f;near(sc.process(-x),-sc.process(x));}
    }
    for(int i=0;i<=100;++i){float x=i*.01f;near(wear(x,0),x);near(wear(x,1),x*x);near(wear(-x,1),-x*x);}
    for(float t:{0.f,.1f,.5f,.9f,1.f}){near(cubic(0,1,2,3,t),1+t);}
    for(float a:{0.f,.3f,1.f}){
        TapeDiffuser f;float sum=0;
        for(int i=0;i<1300;++i){float y=f.process(i==0?1.f:0.f,a);sum+=y;if(i>853)near(y,0);}
        near(sum,1.f-.3f*a);
    }
    near(idealHybridFade(0),0);near(idealHybridFade(1),1);
    for(int i=0;i<=100;++i){auto c=reverbControls(i*.01f,1);near(c.wet*c.wet+c.dry*c.dry,1);}
    near(presetVOctSpeed(0),0);near(presetVOctSpeed(1),.25);near(presetVOctSpeed(3),1);near(presetVOctSpeed(5),4);
    near(writeSample(.1f,.2f,.9f,SoftClipper{1,0}),.28f);
    TapeAgeFilter tf; for(int i=0;i<1000;++i)near(tf.process(i==0?1.f:0.f,0),i==0?1.f:0.f);
    std::cout<<"PASS: "<<checks<<" native reference invariant checks (not hardware equivalence).\n";
}
