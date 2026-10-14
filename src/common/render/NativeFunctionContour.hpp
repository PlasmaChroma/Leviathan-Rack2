#pragma once
#include "FunctionCurve.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include <array>
#include <chrono>

namespace lumin {
// Rack owns this widget's FBO allocation, binding and image presentation.
// Kept in the child tree for normal context lifecycle, drawn explicitly at the
// contour slot. No AdaptiveGlSurface or incoming-state snapshot is involved.
struct NativeFunctionContour : widget::OpenGlWidget {
 FunctionCurve* renderer;
 FunctionCurve::Layer layer;
 std::array<float,17> saved{};
 bool valid=false,painted=false,clearOnly=false;
 NVGcontext* owner=nullptr;
 Vec origin;
 float density=1;
 double lastWrite=-1;
 explicit NativeFunctionContour(FunctionCurve* value):renderer(value){dirtyOnSubpixelChange=false;}
 void step() override { widget::FramebufferWidget::step(); }
 void draw(const DrawArgs&) override {}
 void onContextCreate(const ContextCreateEvent& e) override {
  valid=false;owner=nullptr;lastWrite=-1;widget::OpenGlWidget::onContextCreate(e);
 }
 void onContextDestroy(const ContextDestroyEvent& e) override {
  valid=false;owner=nullptr;lastWrite=-1;widget::OpenGlWidget::onContextDestroy(e);
 }
 void drawFramebuffer() override {
  const Vec target=getFramebufferSize();
  // Canonical shader preconditions, independent of preceding widget state.
  glUseProgram(0);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
  glDisable(GL_SCISSOR_TEST);glDisable(GL_STENCIL_TEST);glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);glDisable(GL_ALPHA_TEST);glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,int(target.x),int(target.y));glColorMask(1,1,1,1);
  glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
  painted=clearOnly||renderer->drawBatch(owner,target,0,box.size,origin,density,&layer,1);
  // NanoVG's next submission establishes its own draw state. Leave the inputs
  // and texture unit this callback touched in a canonical, unbound condition.
  glDisableVertexAttribArray(0);glBindBuffer(GL_ARRAY_BUFFER,0);glUseProgram(0);
  glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,0);
 }
 struct Result {bool presented=false,updated=false,cacheHit=false;uint64_t updateNs=0;};
 Result drawContour(const DrawArgs& args,Vec extent,float ratio,float shape,bool shark,
  NVGcolor rise,NVGcolor fall,bool split,bool measure){
  Result result;
  if(!APP||!APP->window||!APP->window->win||args.fb||args.vg!=APP->window->vg
   ||glfwGetCurrentContext()!=APP->window->win)return result;
  const float pr=APP->window->pixelRatio;
  // Rack rounds its framebuffer pixel ratio down; keep identical density to
  // the existing candidate by admitting integer pixel ratios only for now.
  float t[6];nvgCurrentTransform(args.vg,t);
  for(float v:t)if(!std::isfinite(v))return result;
  if(!std::isfinite(pr)||pr<1||pr!=std::floor(pr)||t[1]!=0||t[2]!=0||t[0]<=0||t[3]!=t[0])return result;
  const float d=t[0]*pr;
  if(d<.5f||d>8||!std::isfinite(extent.x)||!std::isfinite(extent.y)||extent.x<=3.4f||extent.y<=3.4f||extent.x>512||extent.y>256
   ||!std::isfinite(ratio)||ratio<=0||ratio>=1||!std::isfinite(shape)||std::fabs(shape)>1)return result;
  const Vec phase(t[4]-std::floor(t[4]),t[5]-std::floor(t[5]));
  const double frame=APP->window->getFrameTime();if(!std::isfinite(frame))return result;
  std::array<float,17> key{{extent.x,extent.y,ratio,shape,float(shark),float(split),d,phase.x,phase.y,
   rise.r,rise.g,rise.b,rise.a,fall.r,fall.g,fall.b,fall.a}};
  // Normal Rack events delete the old FBO. Do not touch a surviving resource
  // from a different context if those events were not delivered.
  if(auto* fb=getFramebuffer()){
   if(owner!=args.vg||fb->ctx!=args.vg)return result;
   const Vec size=getFramebufferSize();
   if(!nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg,fb->image,int(size.x),int(size.y))){
    deleteFramebuffer();valid=false;
   }
  }
  if(!valid||saved!=key||!getFramebuffer()){
   if(lastWrite==frame)return result;
   lastWrite=frame;valid=false;painted=false;owner=args.vg;box.size=extent;density=d;origin=phase*(1.f/t[0]);
   layer.ratio=ratio;layer.shape=shape;layer.shark=shark;layer.highlight=split;layer.color=rise;layer.other=fall;
   using Clock=std::chrono::steady_clock;auto start=measure?Clock::now():Clock::time_point();
   // Execute explicitly rather than accepting Rack's frame-budget deferral.
   // Otherwise apparent savings could merely mean fewer contour updates.
   render(Vec(t[0],t[3]),phase,Rect::inf());
   if(measure)result.updateNs=uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-start).count());
   if(!painted||!getFramebuffer())return result;
   saved=key;valid=true;result.updated=true;
  }else result.cacheHit=true;
  widget::FramebufferWidget::draw(args);result.presented=true;return result;
 }
};
}
