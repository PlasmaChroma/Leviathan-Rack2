#include "plugin.hpp"
#include "preview_benchmark_utils.hpp"
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
static void run(NVGcontext* vg){
 engine::Module module;module.config(0,0,0,3);
 AmberGreenVioletApertureLight light;light.module=&module;light.firstLightId=0;
 widget::Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(-100,-100),Vec(400,400));
 auto begin=[&](){nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,40,40);};
 auto end=[&](){glViewport(0,0,256,128);nvgEndFrame(vg);};
 module.lights[0].setBrightness(.7f);
 light.staticBackgroundFb->render(Vec(1,1),Vec(0,0),Rect::inf());
 begin();light.drawBackground(args);light.drawLight(args);end();
 require(!light.normalLightFb->getFramebuffer()&&!light.bloomFb->getFramebuffer(),"changing layers allocate no framebuffer");
 light.normalChangedAt-=.05;light.bloomChangedAt-=.05;
 begin();light.drawBackground(args);light.drawLight(args);end();
 require(!light.normalLightFb->getFramebuffer()&&!light.bloomFb->getFramebuffer(),"50ms remains direct");
 light.normalChangedAt-=1;light.bloomChangedAt-=1;
 begin();light.drawBackground(args);light.drawLight(args);end();
 require(light.normalLightFb->getFramebuffer()&&light.bloomFb->getFramebuffer(),"settled layers build caches");
 begin();light.drawBackground(args);light.drawLight(args);end();
 require(!light.normalLightFb->dirty&&!light.bloomFb->dirty&&!light.normalLightFb->bypassed&&!light.bloomFb->bypassed,"settled caches reused");
 module.lights[0].setBrightness(.3f);begin();light.drawBackground(args);light.drawLight(args);end();
 require(light.normalLightFb->bypassed&&light.bloomFb->bypassed&&light.normalLightFb->dirty&&light.bloomFb->dirty,"change returns to direct without stale cache");
 light.normalChangedAt-=1;light.bloomChangedAt-=1;
 begin();light.drawBackground(args);light.drawLight(args);end();
 settings::haloBrightness=.9f;begin();light.drawBackground(args);light.drawLight(args);end();
 require(!light.normalLightFb->bypassed&&light.bloomFb->bypassed,"bloom setting settles independently");
 module.lights[0].setBrightness(0);begin();light.drawBackground(args);light.drawLight(args);end();require(light.bloomCacheGlow<0,"off resets bloom settling");
 int cases=0,maxByte=0;double maxMean=0;
 std::vector<unsigned char> reference(256*128*4),actual(reference.size());
 for(auto size:{ApertureLightSize::Tiny,ApertureLightSize::Small,ApertureLightSize::Medium,ApertureLightSize::Large})
 for(float scale:{.75f,1.f,1.5f,2.f})for(float alpha:{1.f,.5f,.8f})for(float brightness:{.05f,.4f,1.f}){
  light.applySize(size);module.lights[0].setBrightness(brightness);module.lights[1].setBrightness(brightness*.6f);module.lights[2].setBrightness(brightness*.25f);
  light.staticBackgroundFb->render(Vec(scale,scale),Vec(.37f,.81f),Rect::inf());
  // Prime state, then compare direct and settled with identical source/transform.
  begin();nvgTranslate(vg,.37f,.81f);nvgScale(vg,scale,scale);light.drawBackground(args);light.drawLight(args);end();
  for(int cached=0;cached<2;++cached){
   light.normalChangedAt=light.bloomChangedAt=system::getTime()-(cached?1:0);light.normalLightFb->setDirty();light.bloomFb->setDirty();
   begin();nvgTranslate(vg,.37f,.81f);nvgScale(vg,scale,scale);nvgGlobalAlpha(vg,alpha);if(alpha==.8f)nvgGlobalTint(vg,nvgRGBAf(.7f,.85f,.6f,1.f));nvgScissor(vg,0,0,light.box.size.x*.9f,light.box.size.y);
   light.drawBackground(args);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);light.drawLight(args);if(alpha<1)require(!light.normalLightFb->bypassed&&!light.bloomFb->bypassed,"inherited alpha retains grouped cache");end();
   glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
   if(!cached)reference=actual;else {long long error=0;int localMax=0;for(size_t i=0;i<actual.size();++i){int d=std::abs(int(actual[i])-int(reference[i]));error+=d;localMax=std::max(localMax,d);}require(localMax<=6,"settle transition image tolerance");maxByte=std::max(maxByte,localMax);maxMean=std::max(maxMean,double(error)/actual.size());++cases;}
  }
 }
 std::printf("IMAGE cases=%d max_byte=%d max_mean=%.6f\n",cases,maxByte,maxMean);
 widget::Widget::ContextDestroyEvent destroy;destroy.vg=vg;light.onContextDestroy(destroy);require(!light.normalLightFb->getFramebuffer()&&!light.bloomFb->getFramebuffer(),"context destroys caches");
 widget::Widget::ContextCreateEvent create;create.vg=vg;light.onContextCreate(create);
 light.staticBackgroundFb->render(Vec(1,1),Vec(0,0),Rect::inf());begin();light.drawBackground(args);light.drawLight(args);end();require(light.normalLightFb->bypassed&&light.bloomFb->bypassed,"context recreation starts direct");
 require(glGetError()==GL_NO_ERROR,"aperture GL errors");std::puts("PASS: aperture settling, independent invalidation and context lifecycle");
}
int main(){require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,128,"Aperture probe",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);auto* fbvg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg&&fbvg,"NanoVG");rack::Context ctx;rack::widget::EventState events;ctx.event=&events;rack::window::Window window;ctx.window=&window;window.win=native;window.vg=vg;window.fbVg=fbvg;window.pixelRatio=1;rack::contextSet(&ctx);settings::haloBrightness=.5f;
 run(vg);nvgDeleteGL2(fbvg);nvgDeleteGL2(vg);ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();}
