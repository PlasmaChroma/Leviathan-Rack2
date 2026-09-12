#include "plugin.hpp"
#include "../tools/preview_benchmark_utils.hpp"
#include "render/HostStrokeBridge.hpp"
#include "callback_bridge.hpp"
#include "flux_geometry.hpp"
#include <cstring>
#include "UndertowShape.hpp"
#include "WavePreviewSimplifier.hpp"

static lumin::HostStrokeBridge* liveBridge=nullptr;
using Points=std::vector<Vec>;
static std::vector<Points> shapes(int mode) {
 std::vector<Points> result;FluxGeometry g;
 for(int frame=0;frame<460;++frame){
  g.rebuildPoints(.05f+1.9f*(.5f+.5f*std::sin(frame*.037f)),1.f,.85f*std::sin(frame*.023f),
   mode?IntegralFlux::FUNCTION_SHAPE_MATHS:IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);
  result.emplace_back(g.simplifiedFullPath.points.begin(),g.simplifiedFullPath.points.begin()+g.simplifiedFullPath.count);
 }
 return result;
}
static void rect(NVGcontext* vg,float x,float y,float w,float h,NVGcolor c){
 nvgBeginPath(vg);nvgRect(vg,x,y,w,h);nvgFillColor(vg,c);nvgFill(vg);
}
static void scene(NVGcontext* vg,CallbackBridge* bridge,const Points& points,int mode,
 float density,int variant,bool verify=false){
 nvgBeginFrame(vg,224,60,density);
 rect(vg,0,0,224,60,nvgRGBA(20,35,50,160));
 NVGparams original=*nvgInternalParams(vg);
 for(int i=0;i<2;++i){
  nvgSave(vg);nvgTranslate(vg,110.f*i+.37f,.81f);
  if(variant==1)nvgRotate(vg,.08f);
  if(variant==2)nvgScale(vg,.83f,1.07f);
  if(variant==3)nvgSkewX(vg,.13f);
  CallbackStroke input;
  nvgCurrentTransform(vg,input.transform);
  if(variant){
   nvgScissor(vg,7,5,88,36);
   std::memcpy(input.scissor.xform,input.transform,sizeof(input.transform));
   input.scissor.xform[4]=input.transform[0]*51+input.transform[2]*23+input.transform[4];
   input.scissor.xform[5]=input.transform[1]*51+input.transform[3]*23+input.transform[5];
   input.scissor.extent[0]=44;input.scissor.extent[1]=18;
  }
  input.alpha=variant?.57f:1.f;nvgGlobalAlpha(vg,input.alpha);
  input.operation=variant==4?NVG_LIGHTER:NVG_SOURCE_OVER;nvgGlobalCompositeOperation(vg,input.operation);
  input.color=nvgRGBA(230,200-i*90,90+i*100,variant?173:255);
  input.width=variant==4?.25f:variant==3?4.f:1.4f;
  input.cap=NVG_BUTT;
  input.density=density;input.count=int(points.size());
  static_assert(sizeof(Vec)==2*sizeof(float),"packed Vec");input.xy=&points[0].x;
  // Adjacent host draws exercise ordering and preservation of NanoVG state.
  rect(vg,20,10,48,24,nvgRGBA(70,150,80,95));
  if(mode==3){
   nvgStrokeColor(vg,input.color);nvgStrokeWidth(vg,input.width);
   nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,NVG_ROUND);
   require(liveBridge->stroke(vg,points.data(),int(points.size())),"live bridge submitted");
  }else if(mode){
   GLint before[3]{},after[3]{};
   const GLenum bindings[]{GL_FRAMEBUFFER_BINDING,GL_CURRENT_PROGRAM,GL_ARRAY_BUFFER_BINDING};
   if(verify)for(int k=0;k<3;++k)glGetIntegerv(bindings[k],&before[k]);
   submitCallbackStroke(bridge,nvgInternalParams(vg),input,mode==2);
   if(verify){for(int k=0;k<3;++k)glGetIntegerv(bindings[k],&after[k]);
    require(std::memcmp(before,after,sizeof(before))==0,"submission leaves GL bindings unchanged");}
  }
  else {
   nvgBeginPath(vg);for(size_t j=0;j<points.size();++j)(j?nvgLineTo:nvgMoveTo)(vg,points[j].x,points[j].y);
   nvgStrokeColor(vg,input.color);nvgStrokeWidth(vg,input.width);
   nvgLineCap(vg,input.cap);nvgLineJoin(vg,input.join);nvgStroke(vg);
  }
  rect(vg,43,16,18,23,nvgRGBA(30,80,240,110));
  nvgRestore(vg);
 }
 if(verify)require(std::memcmp(&original,nvgInternalParams(vg),sizeof(original))==0,"host callbacks unchanged");
 nvgEndFrame(vg);
}
static void inherited(NVGcontext* vg,CallbackBridge* bridge){
 auto frames=shapes(1);auto* target=nvgluCreateFramebuffer(vg,224,60,0);require(target,"inherited target");
 // scene resets state itself; this additional direct screen keeps state entirely in the host.
 std::vector<unsigned char> reference(224*60*4),actual(reference.size());
 for(int aa=0;aa<2;++aa)for(int frame=100;frame<130;++frame)for(int mode:{0,3}){
  nvgluBindFramebuffer(target);glViewport(0,0,224,60);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  nvgBeginFrame(vg,224,60,1);nvgTranslate(vg,.37f,.81f);nvgRotate(vg,.08f);
  nvgScissor(vg,0,0,100,48);nvgIntersectScissor(vg,7,5,87,37);
  nvgGlobalTint(vg,nvgRGBAf(.65f,.8f,.35f,.47f));nvgGlobalAlpha(vg,.63f);
  nvgGlobalCompositeOperation(vg,NVG_LIGHTER);nvgShapeAntiAlias(vg,aa);
  nvgStrokeWidth(vg,frame%2?.25f:1.4f);nvgStrokeColor(vg,nvgRGBA(205,221,173,117));nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,NVG_ROUND);
  auto before=*nvgInternalParams(vg);
  if(mode)require(liveBridge->stroke(vg,frames[frame].data(),int(frames[frame].size())),"inherited live stroke");
  else {nvgBeginPath(vg);for(size_t j=0;j<frames[frame].size();++j)(j?nvgLineTo:nvgMoveTo)(vg,frames[frame][j].x,frames[frame][j].y);nvgStroke(vg);}
  require(std::memcmp(&before,nvgInternalParams(vg),sizeof(before))==0,"host callback restored");
  rect(vg,23,18,20,20,nvgRGBA(255,120,70,90));nvgEndFrame(vg);glReadPixels(0,0,224,60,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
  if(!mode)reference=actual;else require(reference==actual,"inherited tint alpha intersection and AA exact");
 }
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);std::puts("PASS: 60 inherited-state comparisons exact");
}
static void undertowContours(NVGcontext* vg) {
 int cases=0;
 for(float density:{1.f,1.19f,2.f,4.f}) {
  int w=int(std::ceil(110*density)),h=int(std::ceil(52*density));
  auto* target=nvgluCreateFramebuffer(vg,w,h,0);require(target,"Undertow target");
  std::vector<unsigned char> reference(w*h*4),actual(reference.size());
  for(float shape:{0.f,.1f,.5f,.9f,1.f})for(float hardness:{0.f,.5f,1.f})for(int asym=0;asym<3;++asym) {
   std::array<Vec,256> source;Points contour;
   for(int i=0;i<256;++i){float phase=float(i)/255;
    float v=clamp(undertow_shape::thresholdFold(phase,shape,asym!=0,hardness,asym==2),-1.f,1.f);
    source[i]=Vec(1.625f+phase*102.75f,1.625f+(.5f-.5f*v)*44.75f);
   }
   wave_preview::simplifyPath(source.data(),source.size(),1,.02f,[&](const Vec& p,bool){contour.push_back(p);});
   for(int mode=0;mode<2;++mode){
    nvgluBindFramebuffer(target);glViewport(0,0,w,h);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(vg,110,52,density);nvgTranslate(vg,.37f,.81f);nvgScissor(vg,0,0,106,48);
    nvgStrokeColor(vg,nvgRGBA(230,230,220,255));nvgStrokeWidth(vg,1.25f);nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,NVG_ROUND);
    if(mode)require(liveBridge->stroke(vg,contour.data(),int(contour.size())),"Undertow bridge admitted");
    else{nvgBeginPath(vg);for(size_t i=0;i<contour.size();++i)(i?nvgLineTo:nvgMoveTo)(vg,contour[i].x,contour[i].y);nvgStroke(vg);}
    nvgEndFrame(vg);glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
    if(!mode)reference=actual;else{require(reference==actual,"Undertow contour exact");++cases;}
   }
  }
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
 std::printf("PASS: %d Undertow folded/asymmetric contour comparisons exact\n",cases);
}
int main(){
 require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* window=glfwCreateWindow(256,128,"Callback bridge",nullptr,nullptr);require(window,"window");
 glfwMakeContextCurrent(window);glfwSwapInterval(0);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 std::printf("Rack=%s,GPU=%s,GL=%s\n",rack::APP_VERSION.c_str(),glGetString(GL_RENDERER),glGetString(GL_VERSION));
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"host NanoVG");
 lumin::HostStrokeBridge live;liveBridge=&live;
 auto* bridge=createCallbackBridge();require(bridge,"CPU geometry bridge");
 int cases=0,maxByte=0;double maxRelative=0;
 for(int shapeMode=0;shapeMode<2;++shapeMode){auto frames=shapes(shapeMode);
  for(float density:{1.f,1.19f,2.f,4.f,8.f}){
   int w=int(std::ceil(224*density)),h=int(std::ceil(60*density));auto* target=nvgluCreateFramebuffer(vg,w,h,0);require(target,"test target");
   std::vector<unsigned char> reference(size_t(w)*h*4),pixels(reference.size());
   for(int frame=100;frame<460;frame+=12)for(int variant=0;variant<5;++variant)for(int mode:{0,3}){
    nvgluBindFramebuffer(target);glViewport(0,0,w,h);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    scene(vg,bridge,frames[frame],mode,density,variant,true);
    glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
    if(!mode)reference=pixels;
    else {uint64_t delta=0,alpha=0;int worst=0;for(size_t j=0;j<pixels.size();++j){int d=std::abs(int(pixels[j])-int(reference[j]));delta+=d;worst=std::max(worst,d);if(j%4==3)alpha+=reference[j];}
     if(mode==1)require(delta==0,"unoptimized bridge exactly matches host");
     double relative=double(delta)/(4.*std::max(uint64_t(1),alpha));
     if(worst>16||relative>.001) std::fprintf(stderr,"mismatch shape=%d density=%g frame=%d variant=%d mode=%d byte=%d relative=%g\n",shapeMode,density,frame,variant,mode,worst,relative);
     require(worst<=16&&relative<=.001,"optimized bridge existing image gate");
     maxByte=std::max(maxByte,worst);maxRelative=std::max(maxRelative,relative);++cases;
    }
   }
   require(glGetError()==GL_NO_ERROR,"image GL errors");nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
  }
 }
 std::printf("PASS: image comparisons=%d,max_byte=%d,max_relative=%.9g; host callback table unchanged\n",cases,maxByte,maxRelative);
 for(int shapeMode=0;shapeMode<2;++shapeMode){auto frames=shapes(shapeMode);
  for(float density:{1.f,2.f}){
   int w=int(224*density),h=int(60*density);auto* target=nvgluCreateFramebuffer(vg,w,h,0);require(target,"timing target");
   for(int repeat=0;repeat<3;++repeat)for(int order=0;order<4;++order){int mode=(order+repeat)%4;std::vector<double> cpu;Queries queries;
    for(int frame=0;frame<460;++frame){nvgluBindFramebuffer(target);glViewport(0,0,w,h);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);
     auto* query=queries.begin(frame);auto start=Clock::now();glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
     scene(vg,bridge,frames[frame],mode,density,0);double elapsed=us(start);queries.end(query);glFlush();if(frame>=100)cpu.push_back(elapsed);
    }
    queries.finish();char label[160];std::snprintf(label,sizeof(label),"bridge/shape%d/%.0fx/repeat%d/mode%d/CPU",shapeMode,density,repeat,mode);report(label,cpu);
    std::snprintf(label,sizeof(label),"bridge/shape%d/%.0fx/repeat%d/mode%d/GPU",shapeMode,density,repeat,mode);report(label,queries.results);require(queries.skipped==0,"complete GPU queries");
   }
   nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
  }
 }
 inherited(vg,bridge);
 undertowContours(vg);
 // Reuse the CPU bridge after replacing the host NanoVG context. It must not
 // retain the old backend or own any of its resources.
 nvgDeleteGL2(vg);vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"replacement host");
 auto* target=nvgluCreateFramebuffer(vg,224,60,0);require(target,"replacement target");
 auto frames=shapes(1);std::vector<unsigned char> reference(224*60*4),actual(reference.size());
 for(int mode:{0,3}){nvgluBindFramebuffer(target);glViewport(0,0,224,60);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  scene(vg,bridge,frames[155],mode,1,1,true);glReadPixels(0,0,224,60,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());if(!mode)reference=actual;else require(reference==actual,"replacement host output identical");}
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 destroyCallbackBridge(bridge);nvgDeleteGL2(vg);require(glGetError()==GL_NO_ERROR,"final GL errors");glfwDestroyWindow(window);glfwTerminate();
 std::puts("PASS: replacement host context; no retained host ownership; final GL errors clear");
}
