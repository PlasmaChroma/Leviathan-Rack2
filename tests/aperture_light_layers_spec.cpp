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
static void oldCrescent(NVGcontext* vg, float cx, float cy, float amount, float lensRadius) {
	const float alpha = 0.20f * clamp(amount, 0.f, 1.f);
	if (alpha <= 0.001f) {
		return;
	}
	nvgBeginPath(vg);
	nvgCircle(vg, cx + lensRadius * 0.20f, cy + lensRadius * 0.20f, lensRadius * 0.88f);
	nvgCircle(vg, cx - lensRadius * 0.02f, cy - lensRadius * 0.04f, lensRadius * 0.86f);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, nvgRGBAf(0.f, 0.f, 0.f, alpha));
	nvgFill(vg);
}

static void run(NVGcontext* vg){
 engine::Module module;module.config(0,0,0,3);AmberGreenVioletApertureLight light;light.module=&module;light.firstLightId=0;
 auto render=[&](int mode){float cx=light.box.size.x*.5f,cy=light.box.size.y*.5f;
  nvgSave(vg);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);
  if(mode==0 && light.lightBrightness>.001f){light.drawCore(vg,cx,cy,light.lightCore,light.lightHot);oldCrescent(vg,cx,cy,light.lightCore,light.lensRadius);}else if(mode==1)light.drawNormalLight(vg);
  light.drawNormalSpecular(vg);nvgRestore(vg);
 };
 auto begin=[&](){nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);};
 std::vector<unsigned char> ref(256*128*4),actual(ref.size());int cases=0,maximum=0;
 for(auto size:{ApertureLightSize::Tiny,ApertureLightSize::Small,ApertureLightSize::Medium,ApertureLightSize::Large})for(float scale:{.75f,1.f,1.5f,2.f})for(float brightness:{.01f,.1f,.4f,.7f,1.f})for(int color=0;color<3;++color)for(float alpha:{1.f,.5f})for(int cached=0;cached<2;++cached){
  light.applySize(size);for(int j=0;j<3;++j)module.lights[j].setBrightness(j==color?brightness:brightness*.25f);light.refreshLightState();
  for(int mode=0;mode<2;++mode){begin();nvgTranslate(vg,30.37f,30.81f);nvgScale(vg,scale,scale);nvgGlobalAlpha(vg,alpha);light.drawStaticBackground(vg);
   if(cached){
    auto* fb=nvgluCreateFramebuffer(vg,128,128,0);require(fb,"cached target");
    nvgluBindFramebuffer(fb);glViewport(0,0,128,128);glDisable(GL_SCISSOR_TEST);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    auto* fvg=APP->window->fbVg;nvgBeginFrame(fvg,128,128,1);nvgScale(fvg,scale,scale);float cx=light.box.size.x*.5f,cy=light.box.size.y*.5f;
    nvgGlobalCompositeBlendFunc(fvg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);
    if(mode==0){light.drawCore(fvg,cx,cy,light.lightCore,light.lightHot);oldCrescent(fvg,cx,cy,light.lightCore,light.lensRadius);}else light.drawNormalLight(fvg);
    nvgEndFrame(fvg);nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);
    nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);nvgBeginPath(vg);nvgRect(vg,0,0,128/scale,128/scale);nvgFillPaint(vg,nvgImagePattern(vg,0,0,128/scale,128/scale,0,fb->image,1));nvgFill(vg);light.drawNormalSpecular(vg);
    nvgEndFrame(vg);nvgluDeleteFramebuffer(fb);
   }else{render(mode);nvgEndFrame(vg);}glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());if(!mode)ref=actual;else{for(size_t k=0;k<actual.size();++k)maximum=std::max(maximum,std::abs(int(actual[k])-int(ref[k])));++cases;}}
 }
 std::printf("IMAGE cases=%d max_byte=%d\n",cases,maximum);require(maximum==0,"removing black screen crescent preserves final pixels");
 light.applySize(ApertureLightSize::Small);
 for(int repeat=0;repeat<4;++repeat)for(int order=0;order<2;++order){int mode=(order+repeat)%2;std::vector<double> draw,total;
  for(int frame=0;frame<700;++frame){begin();auto start=Clock::now();
   for(int i=0;i<8;++i){for(int j=0;j<3;++j)module.lights[j].setBrightness(j==0?.5f+.49f*std::sin(frame*.037f+i):0);light.refreshLightState();nvgSave(vg);nvgTranslate(vg,8.f+28*i,30);render(mode);nvgRestore(vg);}
   double submission=us(start);nvgEndFrame(vg);double complete=us(start);glFlush();if(frame>=100){draw.push_back(submission);total.push_back(complete);}
  }
  char label[100];std::snprintf(label,sizeof(label),"repeat%d/mode%d/draw8",repeat,mode);report(label,draw);std::snprintf(label,sizeof(label),"repeat%d/mode%d/with-flush",repeat,mode);report(label,total);
 }
 require(glGetError()==GL_NO_ERROR,"GL errors");
}
int main(){require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,128,"Aperture probe",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);auto* fbvg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg&&fbvg,"NanoVG");rack::Context ctx;rack::widget::EventState events;ctx.event=&events;rack::window::Window window;ctx.window=&window;window.win=native;window.vg=vg;window.fbVg=fbvg;window.pixelRatio=1;rack::contextSet(&ctx);settings::haloBrightness=.5f;
 run(vg);nvgDeleteGL2(fbvg);nvgDeleteGL2(vg);ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();}
