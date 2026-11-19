#include <engine/Engine.hpp>
#include <system.hpp>
#undef PRIVATE
#include "plugin.hpp"
#include "preview_benchmark_utils.hpp"
#include <system.hpp>
#include "common/visual/VisualAssets.hpp"
#include "common/visual/GoldApertureCache.hpp"
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

static void run(NVGcontext* vg) {
 engine::Module module;module.config(0,0,0,2);
 SmallGoldApertureLight light;light.module=&module;light.firstLightId=0;
 auto& cache=*static_cast<GoldApertureCache*>(light.children.front());
 widget::Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(-100,-100),Vec(400,400));
 auto begin=[&](){nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,40.37f,30.81f);};
 auto end=[&](){glViewport(0,0,256,128);nvgEndFrame(vg);};
 auto draw=[&](){light.drawBackground(args);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);light.drawLight(args);};
 begin();draw();end();require(cache.backgroundFb->getFramebuffer(),"off background cached");
 require(!cache.illuminationFb->getFramebuffer(),"off illumination allocates no texture");
 module.lights[0].setBrightness(.7f);begin();draw();end();
 require(cache.illuminationFb->bypassed&&!cache.illuminationFb->getFramebuffer(),"changing light stays direct");
 cache.illuminationChangedAt-=.05;begin();draw();end();require(cache.illuminationFb->bypassed,"50ms stays direct");
 cache.illuminationChangedAt-=1.;begin();draw();end();
 require(cache.illuminationFb->getFramebuffer()&&!cache.illuminationFb->dirty&&!cache.illuminationFb->bypassed,"settled light cached");
 auto* saved=cache.illuminationFb->getFramebuffer();begin();draw();end();
 require(cache.illuminationFb->getFramebuffer()==saved&&!cache.illuminationFb->dirty,"steady cache reused");
 module.lights[0].setBrightness(.3f);begin();draw();end();require(cache.illuminationFb->bypassed&&cache.illuminationFb->dirty,"changed brightness never draws stale image");
 cache.illuminationChangedAt-=1.;begin();draw();end();settings::haloBrightness=0;
 begin();draw();end();require(cache.illuminationFb->bypassed&&cache.cachedBloom==0,"halo setting invalidates immediately");
 settings::haloBrightness=.5f;light.setBaseColor(nvgRGB(255,118,24));begin();draw();end();require(cache.illuminationFb->bypassed,"color invalidates illumination");
 module.lights[0].setBrightness(0);begin();draw();end();cache.illuminationChangedAt-=1.;begin();draw();end();
 require(cache.illuminationFb->dirty,"off leaves existing dynamic cache unbuilt");
 light.module=nullptr;light.previewBrightness=.6f;begin();draw();end();require(cache.brightness==.6f,"browser preview brightness");light.module=&module;
 // Same peak brightness, different channel mixture must invalidate color.
 light.addBaseColor(nvgRGB(42,246,255));module.lights[0].setBrightness(.6f);module.lights[1].setBrightness(0);
 begin();draw();end();cache.illuminationChangedAt-=1.;begin();draw();end();
 const auto oldColor=light.activeColor;module.lights[1].setBrightness(.6f);
 begin();draw();end();require(cache.illuminationFb->bypassed&&light.activeColor.b!=oldColor.b,"mixed color invalidates at unchanged peak brightness");
 module.lights[1].setBrightness(0);
 std::vector<unsigned char> reference(256*128*4),actual(reference.size());int cases=0,maximum=0;double maxMean=0;
 // Compare cached and original primitive rendering, including press-like
 // fractional translations, clipping, halo settings, RGB and inherited alpha.
 for(float size:{10.f,13.f,18.f})for(float scale:{.75f,1.f,1.5f,2.f,4.f})
 for(float alpha:{1.f,.5f,.8f})for(float brightness:{0.f,.05f,.4f,1.f})for(int color=0;color<2;++color){
  light.box.size=Vec(size,size);light.setBaseColor(color?nvgRGB(255,118,24):nvgRGB(255,235,120));module.lights[0].setBrightness(brightness);
  begin();nvgScale(vg,scale,scale);draw();end();cache.illuminationChangedAt-=1.;
  for(int cached=0;cached<2;++cached){
   begin();nvgScale(vg,scale,scale);nvgGlobalAlpha(vg,alpha);if(alpha==.8f)nvgGlobalTint(vg,nvgRGBAf(.7f,.85f,.6f,1.f));
   nvgScissor(vg,-6,-6,size+8,size+9);
   if(cached)draw();else {cache.paintBackground(args);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);cache.paintIllumination(args);}
   end();glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
   if(!cached)reference=actual;else {
    long long sum=0;int local=0;for(size_t i=0;i<actual.size();++i){int d=std::abs(int(actual[i])-int(reference[i]));sum+=d;local=std::max(local,d);}
    if(local>8)std::printf("MISMATCH size=%g scale=%g alpha=%g brightness=%g color=%d max=%d\n",size,scale,alpha,brightness,color,local);
    maximum=std::max(maximum,local);maxMean=std::max(maxMean,double(sum)/actual.size());++cases;
   }
  }
 }
 std::printf("IMAGE cases=%d max_byte=%d max_mean=%.6f\n",cases,maximum,maxMean);require(maximum<=8,"cached/direct image parity");
 // Rotation and nested rendering must use the original primitives.
 begin();nvgRotate(vg,.2f);draw();end();require(cache.backgroundFb->bypassed&&cache.illuminationFb->bypassed,"rotation bypass");
 args.fb=cache.backgroundFb->getFramebuffer();begin();draw();end();args.fb=nullptr;require(cache.backgroundFb->bypassed&&cache.illuminationFb->bypassed,"nested framebuffer bypass");
 widget::Widget::ContextDestroyEvent destroy;destroy.vg=vg;light.onContextDestroy(destroy);
 require(!cache.backgroundFb->getFramebuffer()&&!cache.illuminationFb->getFramebuffer(),"context destroys both caches");
 widget::Widget::ContextCreateEvent create;create.vg=vg;light.onContextCreate(create);
 begin();draw();end();require(cache.backgroundFb->getFramebuffer()&&cache.illuminationFb->bypassed,"context recreation rebuilds lazily");
 cache.illuminationChangedAt-=1.;begin();draw();end();require(cache.illuminationFb->getFramebuffer(),"recreated illumination settles");
 light.onContextDestroy(destroy);require(glGetError()==GL_NO_ERROR,"GL errors");
 std::puts("PASS gold button light caching, settings, preview, resize, tint, nested draw and context lifecycle");
}
int main(){system::init();require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,128,"Gold light cache",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);auto* fbvg=nvgCreateSharedGL2(vg,NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg&&fbvg,"NanoVG");rack::Context ctx;rack::widget::EventState events;ctx.event=&events;rack::window::Window window;ctx.window=&window;window.win=native;window.vg=vg;window.fbVg=fbvg;window.pixelRatio=1;rack::contextSet(&ctx);settings::haloBrightness=.5f;
 run(vg);nvgDeleteGL2(fbvg);nvgDeleteGL2(vg);ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();}

