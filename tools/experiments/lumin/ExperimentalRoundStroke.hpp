#pragma once
#include "../../../src/visual/AdaptiveGlSurface.hpp"
#include "PrivateContourEngine.hpp"
namespace leviathan { namespace render {
class ExperimentalRoundStroke {
 visual_assets::AdaptiveGlSurface surface;
 gl_lifecycle::ContextLease lease;
 NVGcontext* owner=nullptr;
 PrivateContourEngine* engine=nullptr;
 const ContourSegment* segments=nullptr;
 int segmentCount=0;
 Vec logicalSize;
 Vec pixelOffset;
 float density=1.f;
 double lastFrame=-1.;
 bool failed=false;
 bool rendered=false;
 static void retire(void* user,int kind,unsigned int name) {
  auto& self=*static_cast<ExperimentalRoundStroke*>(user);
  const gl_lifecycle::ObjectKind kinds[]={gl_lifecycle::ObjectKind::Buffer,
   gl_lifecycle::ObjectKind::Program,gl_lifecycle::ObjectKind::Shader,gl_lifecycle::ObjectKind::Texture};
  gl_lifecycle::retireObject(self.lease,kinds[kind],name);
 }
 static void paint(void* user,Vec active,int viewportY) {
  auto& self=*static_cast<ExperimentalRoundStroke*>(user);
  if(!self.engine)self.engine=createContourEngine(&self,retire);
  if(!self.engine){self.failed=true;return;}
  glViewport(0,viewportY,int(active.x),int(active.y));
  glStencilMask(0xff);glClearStencil(0);glClear(GL_STENCIL_BUFFER_BIT);
  renderContourEngine(self.engine,active.x/self.density,active.y/self.density,self.density,
   self.segments,self.segmentCount,1.4f,self.pixelOffset.x/self.density,self.pixelOffset.y/self.density);
  self.rendered=true;
 }
public:
 ~ExperimentalRoundStroke(){reset();}
 ExperimentalRoundStroke()=default;
 ExperimentalRoundStroke(const ExperimentalRoundStroke&)=delete;
 ExperimentalRoundStroke& operator=(const ExperimentalRoundStroke&)=delete;
 void reset() {
  destroyContourEngine(engine);engine=nullptr;
  surface.reset(false);lease.reset();owner=nullptr;failed=false;lastFrame=-1.;
 }
 bool draw(const Widget::DrawArgs& args,Vec size,const ContourSegment* paths,int count) {
  if(!APP||!APP->window||!APP->scene||args.fb||args.vg!=APP->window->vg||count<1||count>2)return false;
  float t[6];nvgCurrentTransform(args.vg,t);
  if(t[1]!=0.f||t[2]!=0.f||t[0]<=0.f||t[3]!=t[0])return false;
  float nextDensity=t[0]*APP->window->pixelRatio;
  if(!std::isfinite(nextDensity)||nextDensity<.25f||nextDensity>8.f||size.x<=0||size.y<=0||size.x>512||size.y>256)return false;
  double frame=APP->window->getFrameTime();
  if(!std::isfinite(frame))return false;
  if(owner!=args.vg||!gl_lifecycle::resourceContextMatches(lease,args.vg)) {
   reset();owner=args.vg;lease=gl_lifecycle::acquireResourceContext(owner);
  }
  if(failed||!lease||frame==lastFrame)return false;
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
