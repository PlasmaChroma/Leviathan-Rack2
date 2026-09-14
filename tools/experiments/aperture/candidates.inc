// Offline probes only. No production dispatch or persistent GL ownership.
#include <map>
#include <tuple>
#include <array>

namespace candidate {
struct Geometry {
 std::vector<NVGvertex> fill,stroke;
 NVGpath path{};
 std::array<float,4> bounds{};
 float x=0,y=0;
};
using Key=std::tuple<float,float,float,float,float,float>;
std::map<Key,Geometry> geometry;
Geometry* pending=nullptr;
decltype(NVGparams::renderFill) original=nullptr;
float originX=0,originY=0;
void capture(void* user,NVGpaint* paint,NVGcompositeOperationState composite,
 NVGscissor* scissor,float fringe,const float* bounds,const NVGpath* paths,int count) {
 require(pending&&count==1,"single circle capture");
 auto& g=*pending;
 if(g.fill.empty()) {
  g.path=paths[0];g.fill.assign(paths[0].fill,paths[0].fill+paths[0].nfill);
  if(paths[0].nstroke)g.stroke.assign(paths[0].stroke,paths[0].stroke+paths[0].nstroke);
  std::copy(bounds,bounds+4,g.bounds.begin());g.x=originX;g.y=originY;
 }
 const float dx=originX-g.x,dy=originY-g.y;
 for(auto& v:g.fill){v.x+=dx;v.y+=dy;}for(auto& v:g.stroke){v.x+=dx;v.y+=dy;}
 g.bounds[0]+=dx;g.bounds[2]+=dx;g.bounds[1]+=dy;g.bounds[3]+=dy;g.x=originX;g.y=originY;
 g.path.fill=g.fill.data();g.path.stroke=g.stroke.data();
 original(user,paint,composite,scissor,fringe,g.bounds.data(),&g.path,1);
}
void retainedCircle(NVGcontext* vg,float cx,float cy,float radius) {
 float t[6];nvgCurrentTransform(vg,t);
 // Probe admits only axis-aligned uniform scale and fixed device density/AA.
 require(t[1]==0&&t[2]==0&&t[0]==t[3]&&t[0]>0,"retained transform scope");
 originX=t[0]*cx+t[4];originY=t[3]*cy+t[5];
 auto key=Key(radius,t[0],t[3],0,0,0);
 auto& g=geometry[key];auto* params=nvgInternalParams(vg);
 original=params->renderFill;pending=&g;params->renderFill=capture;
 nvgBeginPath(vg);
 if(g.fill.empty())nvgCircle(vg,cx,cy,radius);else nvgRect(vg,cx,cy,2,2);
 nvgFill(vg);
 params->renderFill=original;pending=nullptr;original=nullptr;
}
struct Mask { NVGLUframebuffer* fb=nullptr;float extent=0;int pixels=0; };
std::map<std::tuple<float,float>,Mask> masks;
Mask& mask(NVGcontext* vg,float inner,float outer) {
 auto key=std::make_tuple(inner,outer);auto& m=masks[key];if(m.fb)return m;
 // Four samples per Rack logical pixel; include padding beyond circle AA.
 constexpr float density=4;
 m.extent=std::ceil(outer+2);m.pixels=int(2*m.extent*density);
 m.fb=nvgluCreateFramebuffer(vg,m.pixels,m.pixels,0);require(m.fb,"mask framebuffer");
 nvgluBindFramebuffer(m.fb);glViewport(0,0,m.pixels,m.pixels);glDisable(GL_SCISSOR_TEST);
 glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 auto* fvg=APP->window->fbVg;nvgBeginFrame(fvg,2*m.extent,2*m.extent,density);
 nvgBeginPath(fvg);nvgCircle(fvg,m.extent,m.extent,outer);
 nvgFillPaint(fvg,nvgRadialGradient(fvg,m.extent,m.extent,inner,outer,nvgRGBAf(1,1,1,1),nvgRGBAf(1,1,1,0)));nvgFill(fvg);nvgEndFrame(fvg);
 nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);return m;
}
void maskedCircle(NVGcontext* vg,float cx,float cy,float inner,float outer,NVGcolor color) {
 auto& m=mask(vg,inner,outer);
 NVGpaint p=nvgImagePattern(vg,cx-m.extent,cy-m.extent,2*m.extent,2*m.extent,0,m.fb->image,1);
 p.innerColor=p.outerColor=color;
 nvgBeginPath(vg);nvgRect(vg,cx-m.extent,cy-m.extent,2*m.extent,2*m.extent);nvgFillPaint(vg,p);nvgFill(vg);
}
void circle(NVGcontext* vg,float cx,float cy,float radius,int mode) {
 if(mode==2)retainedCircle(vg,cx,cy,radius);
 else{nvgBeginPath(vg);nvgCircle(vg,cx,cy,radius);nvgFill(vg);}
}
void radial(NVGcontext* vg,float cx,float cy,float inner,float outer,NVGcolor color,int mode) {
 if(mode==1){maskedCircle(vg,cx,cy,inner,outer,color);return;}
 NVGcolor transparent=color;transparent.a=0;
 nvgFillPaint(vg,nvgRadialGradient(vg,cx,cy,inner,outer,color,transparent));circle(vg,cx,cy,outer,mode);
}
void dynamic(LeviathanApertureLight& l,NVGcontext* vg,int mode) {
 if(mode==0){l.drawNormalLight(vg);l.drawNormalSpecular(vg);l.drawBloom(vg,l.box.size.x/2,l.box.size.y/2,l.lightGlow*1.5f);return;}
 float cx=l.box.size.x/2,cy=l.box.size.y/2;
 NVGcolor center=l.activeColor,edge=l.activeColor,hot=l.activeColor;
 center.r+=(1-center.r)*.58f;center.g+=(1-center.g)*.58f;center.b+=(1-center.b)*.58f;center.a=.35f*l.lightCore+.35f*l.lightHot;
 edge.a=.82f*l.lightCore;
 nvgFillPaint(vg,nvgRadialGradient(vg,cx-l.lensRadius*.10f,cy-l.lensRadius*.14f,std::max(.1f,l.coreRadius*.10f),l.coreRadius+l.lensRadius*.38f,center,edge));
 circle(vg,cx,cy,l.coreRadius+l.lensRadius*.32f,mode);
 hot.r+=(1-hot.r)*.34f;hot.g+=(1-hot.g)*.34f;hot.b+=(1-hot.b)*.34f;hot.a=.18f*l.lightCore+.20f*l.lightHot;
 nvgFillColor(vg,hot);circle(vg,cx,cy,l.coreRadius,mode);
 if(l.lightHot>.001f)radial(vg,cx-l.lensRadius*.36f,cy-l.lensRadius*.36f,0,l.lensRadius*.22f,nvgRGBAf(1,1,1,.72f*l.lightHot),mode==5?0:mode==6?1:mode);
 NVGcolor bloom=l.activeColor;bloom.a=clamp(l.bloomAlpha*.95f*l.lightGlow*1.5f,0.f,1.f);
 radial(vg,cx,cy,std::max(.5f,l.coreRadius*.70f),l.bloomRadius*1.35f,bloom,mode==5?1:mode==6?0:mode);
 bloom.a=clamp(l.bloomAlpha*.34f*l.lightGlow*1.5f,0.f,1.f);
 radial(vg,cx,cy,l.coreRadius*.35f,l.lensRadius*1.7f,bloom,mode==5?1:mode==6?0:mode);
}
void clear() {
 for(auto& entry:masks)nvgluDeleteFramebuffer(entry.second.fb);
 masks.clear();geometry.clear();
}
}
