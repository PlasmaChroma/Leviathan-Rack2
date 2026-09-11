#pragma once
// Reuse the historical fixtures without changing their reference implementation.
#define ShaderContourEngine ArchivedShaderContourEngine
#include "flux_shader_candidate.hpp"
#undef ShaderContourEngine
#include "../../../src/render/PolylineStroke.hpp"
namespace leviathan { namespace render {
inline std::shared_ptr<lumin::PolylineDevice> probeDevice(){
 static std::weak_ptr<lumin::PolylineDevice> weak;
 auto device=weak.lock();if(!device){device=std::make_shared<lumin::PolylineDevice>();weak=device;}return device;
}
class ShaderContourEngine {
 std::shared_ptr<lumin::PolylineDevice> device=probeDevice();
 std::unique_ptr<lumin::PolylineStroke> strokes[2];
 std::vector<lumin::Point> scratch;
 uint64_t revision=0;
public:
 ShaderContourEngine(){scratch.reserve(1025);for(auto& stroke:strokes)stroke.reset(new lumin::PolylineStroke);}
 void reset(){for(auto& stroke:strokes)stroke.reset(new lumin::PolylineStroke);device=probeDevice();}
 bool render(NVGcontext* vg,Vec active,int y,float density,Vec offset,const ContourSegment* paths,int count){
  if(!paths||count<1||count>2)return false;
  ++revision;
  for(int j=0;j<count;++j){scratch.clear();for(int i=0;i<paths[j].count;++i)scratch.push_back({paths[j].points[i].x,paths[j].points[i].y});
   if(strokes[j]->update(scratch.data(),scratch.size(),revision)!=lumin::StrokeResult::Ok)return false;}
  for(int j=0;j<count;++j)if(strokes[j]->draw(*device,vg,active,y,density,offset,1.4f,paths[j].color)!=lumin::StrokeResult::Ok)return false;
  return true;
 }
};
} }
