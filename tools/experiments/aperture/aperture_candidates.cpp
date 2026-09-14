#include "plugin.hpp"
#include "preview_benchmark_utils.hpp"
bool isDragonKingDebugEnabled() { return true; }
#define private public
#include "visual/ApertureLight.hpp"
#undef private
// Use Rack's real framebuffer implementation with a minimal offline window.
namespace rack {
Context::~Context() {}
namespace window {
Window::Window():internal(nullptr){}
Window::~Window(){}
double Window::getFrameTime(){return 1.;}
double Window::getFrameDurationRemaining(){return 1.;}
int& Window::fbCount(){static int count=0;return count;}
bool& Window::fbDirtyOnSubpixelChange(){static bool enabled=false;return enabled;}
}
}

#include "candidates.inc"
#include "run.inc"
int main(){require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,128,"Aperture probe",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);auto* fbvg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg&&fbvg,"NanoVG");rack::Context ctx;rack::widget::EventState events;ctx.event=&events;rack::window::Window window;ctx.window=&window;window.win=native;window.vg=vg;window.fbVg=fbvg;window.pixelRatio=1;rack::contextSet(&ctx);settings::haloBrightness=.5f;
 run(vg);nvgDeleteGL2(fbvg);nvgDeleteGL2(vg);ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();}


