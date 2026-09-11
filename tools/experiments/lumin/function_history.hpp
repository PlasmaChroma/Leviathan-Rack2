#pragma once
#include <array>
#include <cmath>
#include <algorithm>
namespace lumin {
struct FunctionParameters {float ratio=.5f,shape=0;bool shark=false;};
// Same six-slot ring order/capture cadence/fade as Flux. Capture the old shape
// before replacing current parameters. No point arrays or graphics resources.
struct FunctionHistory {
 struct Frame {FunctionParameters parameters;double birth=0;bool active=false;};
 std::array<Frame,6> frames{};
 int next=0;double last=-1;
 bool capture(FunctionParameters parameters,double now){
  if(!std::isfinite(now)||!std::isfinite(parameters.ratio)||parameters.ratio<=0||parameters.ratio>=1||!std::isfinite(parameters.shape)||std::fabs(parameters.shape)>1)return false;
  if(last>0&&now-last<1.f/24.f)return false;
  auto& f=frames[next];f.parameters=parameters;f.birth=now;f.active=true;next=(next+1)%6;last=now;return true;
 }
 void expire(double now){for(auto& f:frames)if(f.active&&now-f.birth>=.333f)f.active=false;}
 void clear(){for(auto& f:frames)f.active=false;next=0;last=-1;}
 static int alpha(const Frame& f,double now){float age=float(now-f.birth);return !f.active||age<0||age>=.333f?0:std::max(0,std::min(255,int(118.f*(1-age/.333f))));}
};
}
