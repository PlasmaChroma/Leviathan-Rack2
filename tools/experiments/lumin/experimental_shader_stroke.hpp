#pragma once
#include "../../../src/visual/AdaptiveGlSurface.hpp"
#include "flux_shader_candidate.hpp"
namespace leviathan { namespace render {
class ExperimentalShaderStroke {
 visual_assets::AdaptiveGlSurface surface;
 gl_lifecycle::ContextLease lease;
 NVGcontext* owner=nullptr;
 ShaderContourEngine engine;
 const ContourSegment* segments=nullptr;
 int segmentCount=0;
 Vec logicalSize;
 Vec pixelOffset;
 float density=1.f;
 double lastFrame=-1.;
 bool rendered=false;
 bool surfaceOnly=false;
 static void paint(void* user,Vec active,int viewportY) {
  auto& self=*static_cast<ExperimentalShaderStroke*>(user);
  if(self.surfaceOnly){self.rendered=true;return;}
  self.rendered=self.engine.render(self.owner,active,viewportY,self.density,self.pixelOffset,self.segments,self.segmentCount);
 }
public:
 ~ExperimentalShaderStroke(){reset();}
 explicit ExperimentalShaderStroke(bool surfaceOnlyControl=false):surfaceOnly(surfaceOnlyControl){}
 ExperimentalShaderStroke(const ExperimentalShaderStroke&)=delete;
 ExperimentalShaderStroke& operator=(const ExperimentalShaderStroke&)=delete;
 void reset() {
  engine.reset();
  surface.reset(false);lease.reset();owner=nullptr;lastFrame=-1.;
 }
 bool draw(const Widget::DrawArgs& args,Vec size,const ContourSegment* paths,int count) {
  if(!paths||!APP||!APP->window||!APP->scene||args.fb||args.vg!=APP->window->vg||count<1||count>2)return false;
  float t[6];nvgCurrentTransform(args.vg,t);
  if(t[1]!=0.f||t[2]!=0.f||t[0]<=0.f||t[3]!=t[0])return false;
  float nextDensity=t[0]*APP->window->pixelRatio;
  if(!std::isfinite(nextDensity)||!std::isfinite(size.x)||!std::isfinite(size.y)||nextDensity<.25f||nextDensity>8.f||size.x<=0||size.y<=0||size.x>512||size.y>256)return false;
  double frame=APP->window->getFrameTime();
  if(!std::isfinite(frame))return false;
  if(owner!=args.vg||!gl_lifecycle::resourceContextMatches(lease,args.vg)) {
   reset();owner=args.vg;lease=gl_lifecycle::acquireResourceContext(owner);
  }
  if(!lease||frame==lastFrame)return false;
  // At most one write per host frame: an earlier queued image cannot be changed
  // by repeated presentation. Nested/capture destinations use the stock path.
  lastFrame=frame;segments=paths;segmentCount=count;logicalSize=size;density=nextDensity;rendered=false;
  const float screenX=t[4]*APP->window->pixelRatio,screenY=t[5]*APP->window->pixelRatio;
  if(!std::isfinite(screenX)||!std::isfinite(screenY))return false;
  pixelOffset=Vec(screenX-std::floor(screenX),screenY-std::floor(screenY));
  visual_assets::AdaptiveGlSurfacePolicy policy;
  policy.minDensity=policy.maxDensity=density;policy.sizeQuantum=1;
  policy.vertexAttributeCount=2;policy.retainPeakCapacity=true;
  surface.markDirty();
  surface.renderIfNeeded(args.vg,size+Vec(1.f/density,1.f/density),density,1.f,policy,false,paint,this);
  segments=nullptr;
  return rendered&&surface.drawAligned(args,size,density,pixelOffset);
 }
};
} }
