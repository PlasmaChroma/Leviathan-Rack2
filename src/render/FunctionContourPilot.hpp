#pragma once
#include "FunctionCurve.hpp"
#include "../visual/AdaptiveGlSurface.hpp"
#include <array>
namespace lumin {
// Opt-in current contour only. Existing point/history/marker paths stay intact.
class FunctionContourPilot {
 visual_assets::AdaptiveGlSurface surface;
 FunctionCurve renderer{FunctionCurve::Kernel::TwoStep};
 FunctionCurve* sharedRenderer=nullptr;
 gl_lifecycle::ContextLease lease;
 std::array<float,17> saved{};
 bool valid=false,painted=false,ready=false,reportUpdate=false;
 std::array<float,17> pending{};
 visual_assets::AdaptiveGlSurface::Update request;
 double lastWrite=-1;
 NVGcontext* owner=nullptr;
 Vec size,offset;
 float density=1;
 FunctionCurve::Layer layer;
 static void paint(void* user,Vec active,int y){auto& self=*static_cast<FunctionContourPilot*>(user);self.painted=(self.sharedRenderer?*self.sharedRenderer:self.renderer).drawBatch(self.owner,active,y,self.size,self.offset*(1.f/self.density),self.density,&self.layer,1);}
public:
 explicit FunctionContourPilot(FunctionCurve* shared=nullptr):sharedRenderer(shared){}
 struct Result {bool presented=false,updated=false,cacheHit=false;visual_assets::AdaptiveGlSurface::BatchStats phases;};
 visual_assets::AdaptiveGlSurface::Update* prepare(const Widget::DrawArgs& args,Vec extent,float ratio,float shape,bool shark,NVGcolor rise,NVGcolor fall,bool split){
  ready=false;
  if(!APP||!APP->window||!APP->scene||!APP->window->win||args.fb||args.vg!=APP->window->vg||glfwGetCurrentContext()!=APP->window->win)return nullptr;
  float t[6];nvgCurrentTransform(args.vg,t);
  if(t[1]!=0||t[2]!=0||t[0]<=0||t[3]!=t[0]||!std::isfinite(extent.x)||!std::isfinite(extent.y)||extent.x<=3.4f||extent.y<=3.4f||extent.x>512||extent.y>256)return nullptr;
  float d=t[0]*APP->window->pixelRatio;double frame=APP->window->getFrameTime();
  if(!std::isfinite(d)||d<.5f||d>8||!std::isfinite(frame)||!std::isfinite(t[4])||!std::isfinite(t[5])||!std::isfinite(ratio)||ratio<=0||ratio>=1||!std::isfinite(shape)||std::fabs(shape)>1)return nullptr;
  float x=t[4]*APP->window->pixelRatio,y=t[5]*APP->window->pixelRatio;
  if(!std::isfinite(x)||!std::isfinite(y))return nullptr;
  Vec phase{x-std::floor(x),y-std::floor(y)};
  std::array<float,17> key{{extent.x,extent.y,ratio,shape,float(shark),float(split),d,phase.x,phase.y,rise.r,rise.g,rise.b,rise.a,fall.r,fall.g,fall.b,fall.a}};
  if(!gl_lifecycle::resourceContextMatches(lease,args.vg)){surface.reset(false);lease=gl_lifecycle::acquireResourceContext(args.vg);valid=false;lastWrite=-1;}
  if(!lease)return nullptr;
  bool update=!valid||saved!=key||!surface.imageValid();
  if(update){
   // Never overwrite a texture already queued for presentation this host frame.
   if(lastWrite==frame)return nullptr;
   lastWrite=frame;valid=false;painted=false;owner=args.vg;size=extent;offset=phase;density=d;
   layer.ratio=ratio;layer.shape=shape;layer.shark=shark;layer.highlight=split;layer.color=rise;layer.other=fall;
   request={};request.surface=&surface;request.logicalSize=extent+Vec(1.f/d,1.f/d);request.policy.minDensity=request.policy.maxDensity=d;request.policy.sizeQuantum=1;request.policy.vertexAttributeCount=1;request.policy.retainPeakCapacity=true;request.policy.shaderOnlyState=true;request.callback=paint;request.user=this;request.validate=true;
   surface.markDirty();
   pending=key;return &request;
  }
  ready=true;return nullptr;
 }
 void complete(bool rendered){
  if(rendered&&painted){saved=pending;valid=true;ready=true;reportUpdate=true;}
 }
 Result draw(const Widget::DrawArgs& args,Vec extent,float ratio,float shape,bool shark,NVGcolor rise,NVGcolor fall,bool split,bool measure){
  Result result;
  if(auto* update=prepare(args,extent,ratio,shape,shark,rise,fall,split)){
   if(visual_assets::AdaptiveGlSurface::renderBatch(args.vg,update,1,measure?&result.phases:nullptr))complete(update->rendered);
  }
  if(!ready)return result;
  result.presented=surface.drawAligned(args,extent,density,offset);
  result.updated=reportUpdate;result.cacheHit=!reportUpdate;reportUpdate=false;
  return result;
 }
};
}
