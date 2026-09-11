#define _USE_MATH_DEFINES
#include <app/Scene.hpp>
#include <window/Window.hpp>
#undef PRIVATE
#include "experimental_shader_stroke.hpp"
#include "../../preview_benchmark_utils.hpp"
#include "ExperimentalRoundStroke.hpp"
#include "flux_geometry.hpp"
#include <cstring>

static double testFrame=1.;
namespace rack {
Context::~Context() {}
namespace app {
Scene::Scene():internal(nullptr),rackScroll(nullptr),rack(nullptr),menuBar(nullptr),browser(nullptr){}
Scene::~Scene(){}
}
namespace window {
Window::Window():internal(nullptr){}
Window::~Window(){}
double Window::getFrameTime(){return testFrame;}
}
}
using namespace leviathan::render;
static void legacy(NVGcontext* vg,const ContourSegment* paths,int count){
 for(int j=0;j<count;++j){const auto& p=paths[j];
  nvgBeginPath(vg);nvgMoveTo(vg,p.points[0].x,p.points[0].y);
  for(int i=1;i<p.count;++i)nvgLineTo(vg,p.points[i].x,p.points[i].y);
  nvgStrokeColor(vg,p.color);nvgStrokeWidth(vg,1.4f);nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,NVG_ROUND);nvgStroke(vg);
 }
}
static void batchBenchmark(NVGcontext* vg,rack::window::Window& window,rack::app::Scene& scene){
 using Surface=visual_assets::AdaptiveGlSurface;
 struct Job{ShaderContourEngine engine;NVGcontext* vg=nullptr;ContourSegment path;};
 auto paint=[](void* user,Vec size,int y){auto& job=*static_cast<Job*>(user);require(job.engine.render(job.vg,size,y,1,{0,0},&job.path,1),"shader pass admitted");};
 std::vector<std::vector<ContourPoint>> frames;FluxGeometry g;
 for(int i=0;i<360;++i){g.rebuildPoints(.05f+1.9f*(.5f+.5f*std::sin(i*.037f)),1,.85f*std::sin(i*.023f),IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);
  frames.emplace_back();for(int j=0;j<g.simplifiedFullPath.count;++j){auto p=g.simplifiedFullPath.points[j];frames.back().push_back({p.x,p.y});}}
 window.pixelRatio=1;
 for(int count:{2,8,32})for(int repeat=0;repeat<3;++repeat)for(int order=0;order<3;++order){
  int mode=(order+repeat)%3;Surface surfaces[32];Surface::Update updates[32];Job jobs[32];
  auto* target=nvgluCreateFramebuffer(vg,110*count,54,0);require(target,"batch shader target");
  for(int i=0;i<count;++i){jobs[i].vg=vg;auto& u=updates[i];u.surface=&surfaces[i];u.logicalSize={107,49};u.policy.minDensity=u.policy.maxDensity=1;u.policy.sizeQuantum=1;u.policy.vertexAttributeCount=2;u.callback=paint;u.user=&jobs[i];}
  Queries queries;std::vector<double> cpu;
  for(int frame=0;frame<460;++frame){++testFrame;for(auto* child:scene.children)child->step();
   const auto& pts=frames[size_t(frame)%frames.size()];for(int i=0;i<count;++i){jobs[i].path=ContourSegment(pts.data(),int(pts.size()),nvgRGBA(230,230,220,255));surfaces[i].markDirty();}
   nvgluBindFramebuffer(target);glViewport(0,0,110*count,54);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glStencilMask(0xff);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   auto* query=queries.begin(frame);auto start=Clock::now();
   if(mode==1)for(auto& u:updates){if(!u.surface)break;require(u.surface->renderIfNeeded(vg,u.logicalSize,1,1,u.policy,false,u.callback,u.user),"separate shader");}
   if(mode==2)require(Surface::renderBatch(vg,updates,count),"grouped shader");
   nvgBeginFrame(vg,110*count,54,1);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(106,48));
   for(int i=0;i<count;++i){nvgSave(vg);nvgTranslate(vg,110.f*i,0);
    if(!mode)legacy(vg,&jobs[i].path,1);else require(surfaces[i].drawAligned(args,{106,48},1,{0,0}),"shader present");nvgRestore(vg);}
   nvgEndFrame(vg);double elapsed=us(start);queries.end(query);glFlush();if(frame>=100)cpu.push_back(elapsed);
  }
  queries.finish();require(glGetError()==GL_NO_ERROR,"batch shader GL state");char label[120];const char* name=mode==0?"host-nanovg":mode==1?"separate-shader":"grouped-shader";
  std::snprintf(label,sizeof(label),"%s/count%d/repeat%d/CPU",name,count,repeat);report(label,cpu);
  std::snprintf(label,sizeof(label),"%s/count%d/repeat%d/GPU",name,count,repeat);report(label,queries.results);
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
}
static void benchmark(NVGcontext* vg,rack::window::Window& window,rack::app::Scene& scene){
 // Precompute actual production-pruned geometry outside render timings.
 std::vector<std::vector<ContourPoint>> frames;
 FluxGeometry g;
 for(int frame=0;frame<360;++frame){
  g.rebuildPoints(.05f+1.9f*(.5f+.5f*std::sin(frame*.037f)),1.f,.85f*std::sin(frame*.023f),IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);
  frames.emplace_back();for(int i=0;i<g.simplifiedFullPath.count;++i){auto p=g.simplifiedFullPath.points[i];frames.back().push_back({p.x,p.y});}
 }
 for(float density:{1.f,2.f})for(int repeat=0;repeat<3;++repeat)for(int order=0;order<4;++order){
  int mode=(order+repeat)%4;window.pixelRatio=density;
  auto* target=nvgluCreateFramebuffer(vg,int(220*density),int(54*density),0);require(target,"benchmark target");
  ExperimentalShaderStroke shader[2];ExperimentalRoundStroke privateNvg[2];
  ExperimentalShaderStroke empty0(true),empty1(true);
  std::vector<double> times,submit;Queries queries;
  for(int frame=0;frame<460;++frame){
   ++testFrame;for(auto* child:scene.children)child->step();
   const auto& points=frames[size_t(frame)%frames.size()];ContourSegment path(points.data(),int(points.size()),nvgRGBA(230,230,220,255));
   nvgluBindFramebuffer(target);glViewport(0,0,int(220*density),int(54*density));glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glStencilMask(0xff);
   glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   auto* query=queries.begin(frame);auto start=Clock::now();
   nvgBeginFrame(vg,220,54,density);
   for(int j=0;j<2;++j){nvgSave(vg);nvgTranslate(vg,float(j*110),0);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(106,48));
    if(mode==0)legacy(vg,&path,1);
    else if(mode==1)require(privateNvg[j].draw(args,{106,48},&path,1),"private benchmark admitted");
    else if(mode==2)require(shader[j].draw(args,{106,48},&path,1),"shader benchmark admitted");
    else require((j?empty1:empty0).draw(args,{106,48},&path,1),"empty surface control");
    nvgRestore(vg);
   }
   double submission=us(start);nvgEndFrame(vg);double elapsed=us(start);queries.end(query);glFlush();
   if(frame>=100){times.push_back(elapsed);submit.push_back(submission);}
  }
  queries.finish();require(glGetError()==GL_NO_ERROR,"benchmark GL state");
  char label[128];const char* name=mode==0?"host-nanovg":mode==1?"private-nanovg":mode==2?"shader":"surface-only-control";
  std::snprintf(label,sizeof(label),"%s/%.2fx/repeat%d/CPU-complete",name,density,repeat);report(label,times);
  std::snprintf(label,sizeof(label),"%s/%.2fx/repeat%d/CPU-before-host-flush",name,density,repeat);report(label,submit);
  std::snprintf(label,sizeof(label),"%s/%.2fx/repeat%d/GPU",name,density,repeat);report(label,queries.results);
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
}
int main(int argc,char** argv) {
 require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* native=glfwCreateWindow(256,256,"Stroke surface test",nullptr,nullptr);require(native,"window");
 glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NanoVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;
 ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;
 rack::contextSet(&ctx);
 if(argc>1&&(std::strcmp(argv[1],"--benchmark")==0||std::strcmp(argv[1],"--batch-benchmark")==0)){
  if(std::strcmp(argv[1],"--batch-benchmark")==0)batchBenchmark(vg,window,scene);else benchmark(vg,window,scene);
  for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}
  while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}
  nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);
  glfwDestroyWindow(native);glfwTerminate();return 0;
 }
 const bool groupedImages=argc>1&&std::strcmp(argv[1],"--grouped-images")==0;
 auto* groupSurface=new visual_assets::AdaptiveGlSurface;
 struct GroupJob{ShaderContourEngine engine;NVGcontext* vg=nullptr;float density=1;Vec offset;const ContourSegment* paths=nullptr;int count=0;} groupJob;
 groupJob.vg=vg;
 auto* surface=new ExperimentalShaderStroke;
 ContourPoint points[]={{3,42},{15,30},{30,9},{40,2},{44,8},{65,25},{103,42}};
 ContourSegment path(points,7,nvgRGBA(230,230,220,255));
 double largest=0;int maxByte=0,cases=0,failedImages=0;
 std::vector<std::vector<ContourPoint>> fixtures;
 fixtures.emplace_back(points,points+7);
 FluxGeometry geometry;
 for(auto mode:{IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,IntegralFlux::FUNCTION_SHAPE_MATHS})
 for(float ratio:{.05f,1.f,1.95f})for(float curve:{-.85f,0.f,.85f}){
  geometry.rebuildPoints(ratio,1.f,curve,mode,false);fixtures.emplace_back();
  for(int i=0;i<geometry.simplifiedFullPath.count;++i){auto p=geometry.simplifiedFullPath.points[i];fixtures.back().push_back({p.x,p.y});}
 }
 for(size_t fixture=0;fixture<fixtures.size();++fixture)for(int highlighted=0;highlighted<2;++highlighted)
 for(float scale:{1.f,1.19f,2.f,4.f,8.f})for(float offset:{0.f,.37f,.81f})for(float alpha:{1.f,.45f}) {
  window.pixelRatio=scale;
  const auto& pts=fixtures[fixture];
  ContourSegment paths[2]={{pts.data(),int(pts.size()),nvgRGBA(230,230,220,255)},{}};
  int count=1;
  if(highlighted){
   int peak=1;for(int i=2;i<int(pts.size())-1;++i)if(pts[i].y<pts[peak].y)peak=i;
   paths[0]=ContourSegment(pts.data(),peak+1,nvgRGBA(28,204,217,255));
   paths[1]=ContourSegment(pts.data()+peak,int(pts.size())-peak,nvgRGBA(230,230,220,180));count=2;
  }
  int w=int(std::ceil(112*scale)),h=int(std::ceil(54*scale));
  auto* target=nvgluCreateFramebuffer(vg,w,h,0);require(target,"output framebuffer");
  std::vector<unsigned char> reference;
  for(int mode=0;mode<2;++mode) {
   ++testFrame;for(auto* child:scene.children)child->step();
   nvgluBindFramebuffer(target);glViewport(0,0,w,h);glDisable(GL_SCISSOR_TEST);
   glColorMask(1,1,1,1);glStencilMask(0xff);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   nvgBeginFrame(vg,float(w)/scale,float(h)/scale,scale);
   nvgTranslate(vg,offset,offset);nvgGlobalAlpha(vg,alpha);
   nvgScissor(vg,8,0,94,48);
   Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(106,48));
   if(mode || groupedImages) {
    GLint framebuffer=0;glGetIntegerv(GL_FRAMEBUFFER_BINDING,&framebuffer);
    if(groupedImages&&mode){
     groupJob.density=scale;float physical=offset*scale;groupJob.offset={physical-std::floor(physical),physical-std::floor(physical)};groupJob.paths=paths;groupJob.count=count;
     visual_assets::AdaptiveGlSurface::Update update;update.surface=groupSurface;update.logicalSize={106+1/scale,48+1/scale};
     update.policy.minDensity=update.policy.maxDensity=scale;update.policy.sizeQuantum=1;update.policy.retainPeakCapacity=true;update.policy.vertexAttributeCount=2;
     update.callback=[](void* user,Vec active,int y){auto& j=*static_cast<GroupJob*>(user);require(j.engine.render(j.vg,active,y,j.density,j.offset,j.paths,j.count),"group image render");};update.user=&groupJob;
     groupSurface->markDirty();require(visual_assets::AdaptiveGlSurface::renderBatch(vg,&update,1)&&update.rendered,"group image update");
     require(groupSurface->drawAligned(args,{106,48},scale,groupJob.offset),"group image presentation");
    }else{
     require(surface->draw(args,{106,48},paths,count),"experimental path used");
     require(!surface->draw(args,{106,48},&path,1),"second same-frame write falls back");
    }
    GLint after=0;glGetIntegerv(GL_FRAMEBUFFER_BINDING,&after);require(after==framebuffer,"framebuffer restored");
   } else {
    legacy(vg,paths,count);
   }
   nvgEndFrame(vg);std::vector<unsigned char> image(size_t(w)*h*4);
   glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,image.data());
   require(glGetError()==GL_NO_ERROR,"no render GL error");
   if(fixture==0 && highlighted==0 && scale==1.f && offset==.37f && alpha==1.f){
    auto* f=std::fopen(mode?"work/shader.ppm":"work/reference.ppm","wb");require(f,"image output");
    std::fprintf(f,"P6\n%d %d\n255\n",w,h);
    for(int y=h-1;y>=0;--y)for(int x=0;x<w;++x)std::fwrite(&image[(size_t(y)*w+x)*4],1,3,f);
    std::fclose(f);
   }
   if(!mode)reference=image;
   else {
    uint64_t error=0,mass=0;int maximum=0;
    for(size_t i=0;i<image.size();++i) {
     int delta=std::abs(int(image[i])-int(reference[i]));error+=delta;maximum=std::max(maximum,delta);
     if(i%4==3)mass+=reference[i];
    }
    require(mass>0,"visible reference");double relative=double(error)/(4.*mass);
    std::printf("fixture=%zu,highlight=%d,scale=%.2f,offset=%.2f,alpha=%.2f,error=%.8f,max_byte=%d\n",fixture,highlighted,scale,offset,alpha,relative,maximum);
    if(groupedImages ? error!=0 : !(relative<.05&&maximum<=64))++failedImages;
    largest=std::max(largest,relative);maxByte=std::max(maxByte,maximum);++cases;
   }
  }
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
 groupJob.engine.reset();delete groupSurface;
 Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(106,48));
 nvgBeginFrame(vg,112,54,1);nvgRotate(vg,.1f);++testFrame;
 require(!surface->draw(args,{106,48},&path,1),"rotation falls back");nvgEndFrame(vg);
 auto* fb=nvgluCreateFramebuffer(vg,8,8,0);args.fb=fb;
 nvgBeginFrame(vg,112,54,1);require(!surface->draw(args,{106,48},&path,1),"nested capture falls back");nvgEndFrame(vg);args.fb=nullptr;
 nvgluDeleteFramebuffer(fb);
 auto before=gl_lifecycle::resourceRetirementStats();
 glfwMakeContextCurrent(nullptr);delete surface;
 auto queued=gl_lifecycle::resourceRetirementStats();require(queued.pendingObjects>before.pendingObjects,"private GPU objects deferred without current context");
 glfwMakeContextCurrent(native);
 for(int frame=0;frame<4;++frame){++testFrame;for(auto* child:scene.children)child->step();}
 require(gl_lifecycle::resourceRetirementStats().pendingObjects==0,"deferred private GPU objects drained");
 require(gl_lifecycle::resourceRetirementStats().pendingFramebuffers==0,"deferred images drained");
 // Context-destroy notification invalidates leases even if the NVG pointer is reused.
 surface=new ExperimentalShaderStroke;window.pixelRatio=1;
 nvgBeginFrame(vg,112,54,1);++testFrame;require(surface->draw(args,{106,48},&path,1),"fresh surface after retirement");nvgEndFrame(vg);
 for(auto* child:scene.children){Widget::ContextDestroyEvent event;event.vg=vg;child->onContextDestroy(event);}
 surface->reset();
 for(auto* child:scene.children){Widget::ContextCreateEvent event;event.vg=vg;child->onContextCreate(event);}
 nvgBeginFrame(vg,112,54,1);++testFrame;require(surface->draw(args,{106,48},&path,1),"reused pointer creates new resources");nvgEndFrame(vg);
 delete surface;
 for(int frame=0;frame<4;++frame){++testFrame;for(auto* child:scene.children)child->step();}
 while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}
 require(glGetError()==GL_NO_ERROR,"no lifecycle GL errors");
 nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);
 glfwDestroyWindow(native);glfwTerminate();
 std::printf("IMAGE SUMMARY: %d cases, %d failed, max aggregate %.8f, max byte %d; lifecycle/fallback checks passed\n",cases,failedImages,largest,maxByte);
 require(failedImages==0,groupedImages?"grouped output must be exactly identical":"shader screening image gate (5% aggregate, 64 bytes)");
 std::printf("PASS: %d composed images, max relative %.8f, max byte %d; lifecycle/fallback checks passed\n",cases,largest,maxByte);
 return 0;
}
