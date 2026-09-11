#define _USE_MATH_DEFINES
#include <app/Scene.hpp>
#include <window/Window.hpp>
#undef PRIVATE
#include "ExperimentalRoundStroke.hpp"
#include "../../preview_benchmark_utils.hpp"

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
int main() {
 require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* native=glfwCreateWindow(256,256,"Stroke surface test",nullptr,nullptr);require(native,"window");
 glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NanoVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;
 ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;
 rack::contextSet(&ctx);
 auto* surface=new ExperimentalRoundStroke;
 ContourPoint points[]={{3,42},{15,30},{30,9},{40,2},{44,8},{65,25},{103,42}};
 ContourSegment path(points,7,nvgRGBA(230,230,220,255));
 double largest=0;int maxByte=0,cases=0;
 for(float scale:{1.f,1.19f,2.f,4.f,8.f})for(float offset:{0.f,.37f,.81f})for(float alpha:{1.f,.45f}) {
  window.pixelRatio=scale;
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
   if(mode) {
    GLint framebuffer=0;glGetIntegerv(GL_FRAMEBUFFER_BINDING,&framebuffer);
    require(surface->draw(args,{106,48},&path,1),"experimental path used");
    GLint after=0;glGetIntegerv(GL_FRAMEBUFFER_BINDING,&after);require(after==framebuffer,"framebuffer restored");
    require(!surface->draw(args,{106,48},&path,1),"second same-frame write falls back");
   } else {
    nvgBeginPath(vg);nvgMoveTo(vg,points[0].x,points[0].y);
    for(int i=1;i<7;++i)nvgLineTo(vg,points[i].x,points[i].y);
    nvgStrokeColor(vg,path.color);nvgStrokeWidth(vg,1.4f);nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,NVG_ROUND);nvgStroke(vg);
   }
   nvgEndFrame(vg);std::vector<unsigned char> image(size_t(w)*h*4);
   glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,image.data());
   require(glGetError()==GL_NO_ERROR,"no render GL error");
   if(!mode)reference=image;
   else {
    uint64_t error=0,mass=0;int maximum=0;
    for(size_t i=0;i<image.size();++i) {
     int delta=std::abs(int(image[i])-int(reference[i]));error+=delta;maximum=std::max(maximum,delta);
     if(i%4==3)mass+=reference[i];
    }
    require(mass>0,"visible reference");double relative=double(error)/(4.*mass);
    std::printf("scale=%.2f,offset=%.2f,alpha=%.2f,error=%.8f,max_byte=%d\n",scale,offset,alpha,relative,maximum);
    require(relative<.01&&maximum<=32,"composed surface image gate");
    largest=std::max(largest,relative);maxByte=std::max(maxByte,maximum);++cases;
   }
  }
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
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
 surface=new ExperimentalRoundStroke;window.pixelRatio=1;
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
 std::printf("PASS: %d composed images, max relative %.8f, max byte %d; lifecycle/fallback checks passed\n",cases,largest,maxByte);
}
