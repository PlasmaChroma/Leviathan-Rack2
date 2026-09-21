#include "fixture.hpp"
#include "reference.inc"
#include "visual/Eclipse2RetainedCap.hpp"
#include "runtime_checks.inc"

Plugin* pluginInstance = nullptr;
namespace rack { namespace app {
Scene::Scene():internal(nullptr),rackScroll(nullptr),rack(nullptr),menuBar(nullptr),browser(nullptr){}
Scene::~Scene(){}
} }


static void run(NVGcontext* vg){
 Plugin plugin;plugin.path=".";pluginInstance=&plugin;
 app::Scene scene;APP->scene=&scene;
 runtimeRecordingChecks(vg);
 Eclipse2Knob::ShadowWidget shadow;shadow.box.size=Vec(34,34);
 auto shadowSvg=window::Svg::load("res/icon/Eclipse2KnobShadow.svg");require(shadowSvg&&shadowSvg->handle,"shadow SVG");shadow.setSvg(shadowSvg);
 Eclipse2Knob::ProgressLedRingWidget ring;ring.box.size=Vec(34,34);
 EclipseKnob::SvgLayer cap;cap.box.size=Vec(34,34);cap.scaleFactor=.70f;
 auto svg=window::Svg::load("res/icon/Eclipse2Knob.svg");require(svg&&svg->handle,"SVG");cap.setSvg(svg);
 const int pixels=136;std::vector<unsigned char> rgba(pixels*pixels*4);
 // Bake with Rack's own NanoVG SVG renderer: NanoSVG's CPU rasterizer changes
 // this cap's radial-gradient shading. Capture premultiplied pixels once.
 auto* target=nvgluCreateFramebuffer(vg,pixels,pixels,0);require(target,"cap bake target");
 nvgluBindFramebuffer(target);glViewport(0,0,pixels,pixels);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 auto* fvg=APP->window->fbVg;nvgBeginFrame(fvg,34,34,4);nvgTranslate(fvg,17,17);nvgScale(fvg,23.8f/120.f,23.8f/120.f);nvgTranslate(fvg,-60,-60);widget::Widget::DrawArgs bakeArgs;bakeArgs.vg=fvg;bakeArgs.clipBox=Rect::inf();bakeArgs.fb=target;cap.cachedSvgSw->draw(bakeArgs);nvgEndFrame(fvg);
 std::vector<unsigned char> bottomUp(rgba.size());glReadPixels(0,0,pixels,pixels,GL_RGBA,GL_UNSIGNED_BYTE,bottomUp.data());
 for(int y=0;y<pixels;++y)std::copy(bottomUp.data()+size_t(pixels-1-y)*pixels*4,bottomUp.data()+size_t(pixels-y)*pixels*4,rgba.data()+size_t(y)*pixels*4);
 {FILE* file=std::fopen("build/eclipse2-cap-4x.rgba","wb");require(file,"bake output");require(std::fwrite(rgba.data(),1,rgba.size(),file)==rgba.size(),"bake write");std::fclose(file);}
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);glViewport(0,0,256,128);
 int image=nvgCreateImageRGBA(vg,pixels,pixels,NVG_IMAGE_PREMULTIPLIED|NVG_IMAGE_GENERATE_MIPMAPS,rgba.data());require(image>0,"cap image");
 int fbImage=nvgCreateImageRGBA(fvg,pixels,pixels,NVG_IMAGE_PREMULTIPLIED|NVG_IMAGE_GENERATE_MIPMAPS,rgba.data());require(fbImage>0,"framebuffer-context cap image");
 const auto coldStart=Clock::now();require(eclipse2_cap::prepare(svg),"runtime cap bake");
 std::printf("COLD cap bake_us=%.3f\n",us(coldStart));
 require(eclipse2_cap::bakedPixels()==rgba,"runtime bake matches NanoVG reference bake");
 eclipse2_cap::Layer retained;retained.setSvg(svg);retained.bakedSvg=svg;retained.box.size=Vec(34,34);retained.scaleFactor=.70f;
 require(!eclipse2_cap::enabled(),"experiment off by default");
 eclipse2_cap::setEnabled(true);require(eclipse2_cap::enabled(),"enable experiment");
 debugEnabled=false;require(!eclipse2_cap::enabled(),"non-debug uses SVG");debugEnabled=true;
 auto rasterCap=[&](NVGcontext* drawVg){retained.valueNorm=cap.valueNorm;widget::Widget::DrawArgs a;a.vg=drawVg;a.clipBox=Rect::inf();retained.draw(a);};
 widget::Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect::inf();NVGLUframebuffer dummy{};args.fb=&dummy; // Match the dirty outer framebuffer's nested-SVG bypass.
 // Exercise the actual layer's baseline and fallback routes, including rotation
 // and inherited opacity. These must remain pixel-identical to the original SVG.
 for(int fallback=0;fallback<4;++fallback){
  eclipse2_cap::setEnabled(fallback!=0);retained.bakedSvg=fallback==1?nullptr:svg;
  cap.box.size=retained.box.size=fallback==2?Vec(42,42):Vec(34,34);
  cap.scaleFactor=retained.scaleFactor=fallback==3?.8f:.7f;
  std::vector<unsigned char> baseline(256*128*4), candidate(baseline.size());
  for(int mode=0;mode<2;++mode){
   nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,40.37f,20.81f);nvgScale(vg,1.5f,1.5f);nvgGlobalAlpha(vg,.5f);
   cap.valueNorm=retained.valueNorm=.33f;if(mode)retained.draw(args);else cap.draw(args);nvgEndFrame(vg);
   glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,(mode?candidate:baseline).data());
  }
  require(baseline==candidate,"SVG fallback pixel parity");
 }
 eclipse2_cap::setEnabled(true);retained.bakedSvg=svg;cap.box.size=retained.box.size=Vec(34,34);cap.scaleFactor=retained.scaleFactor=.7f;
 const char* names[]={"shadow","ring","svg-cap","raster-cap","all-original","raster-cap-hybrid"};
 std::vector<unsigned char> ref(256*128*4),actual(ref.size());int maximum=0,cases=0;double mean=0;
 for(float scale:{.75f,1.f,1.19f,1.5f,2.f,3.f})for(float value:{0.f,.125f,.33f,.5f,.73f,1.f})for(float alpha:{1.f,.5f}){
  cap.valueNorm=ring.valueNorm=value;
  for(int mode=0;mode<2;++mode){nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,40.37f,20.81f);nvgScale(vg,scale,scale);nvgGlobalAlpha(vg,alpha);shadow.draw(args);ring.draw(args);if(mode==0)cap.draw(args);else rasterCap(vg);nvgEndFrame(vg);glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
   if(mode==0)ref=actual;else{int localMax=0;long long sum=0;for(size_t k=0;k<actual.size();++k){int d=std::abs(int(actual[k])-int(ref[k]));maximum=std::max(maximum,d);localMax=std::max(localMax,d);sum+=d;}mean=std::max(mean,double(sum)/(34*34*scale*scale*4));++cases;if(scale==3.f&&alpha==1.f)std::printf("ANGLE %.3f max=%d mean=%.3f\n",value,localMax,double(sum)/(34*34*scale*scale*4));}
   if(scale==3.f&&value==.33f&&alpha==1.f){FILE* file=std::fopen(mode?"build/eclipse2-raster.rgba":"build/eclipse2-reference.rgba","wb");std::fwrite(actual.data(),1,actual.size(),file);std::fclose(file);}
  }
 }
 std::printf("IMAGE cases=%d max_byte=%d max_knob_mean=%.5f\n",cases,maximum,mean);
 for(int repeat=0;repeat<3;++repeat)for(int order=0;order<6;++order){int mode=(order+repeat*2)%6;std::vector<double> times,total;
  for(int frame=0;frame<700;++frame){
   nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);
   ring.valueNorm=cap.valueNorm=float(frame%101)/100.f;auto start=Clock::now();
   for(int i=0;i<4;++i){nvgSave(vg);nvgTranslate(vg,12+52*i,25);
    if(mode==0||mode>=4)shadow.draw(args);if(mode==1||mode>=4)ring.draw(args);
    if(mode==2||mode==4)cap.draw(args);if(mode==3||mode==5)rasterCap(vg);nvgRestore(vg);
   }
   double submission=us(start);nvgEndFrame(vg);double complete=us(start);glFlush();if(frame>=100){times.push_back(submission);total.push_back(complete);}
  }
  char label[100];std::snprintf(label,sizeof(label),"repeat%d/%s/draw4",repeat,names[mode]);report(label,times);std::snprintf(label,sizeof(label),"repeat%d/%s/flush4",repeat,names[mode]);report(label,total);
 }
 struct Content : Widget {
  std::function<void(const DrawArgs&)> callback;
  void draw(const DrawArgs& a) override{callback(a);}
 };
 widget::FramebufferWidget surfaces[2][4];
 for(int mode=0;mode<2;++mode)for(int i=0;i<4;++i){auto* content=new Content;content->box.size=Vec(34,34);content->callback=[&,mode](const Widget::DrawArgs& a){shadow.draw(a);ring.draw(a);if(mode)rasterCap(a.vg);else cap.draw(a);};surfaces[mode][i].dirtyOnSubpixelChange=false;surfaces[mode][i].box.size=Vec(34,34);surfaces[mode][i].addChild(content);}
 args.fb=nullptr;
 for(int repeat=0;repeat<3;++repeat)for(int order=0;order<2;++order){int mode=(order+repeat)%2;std::vector<double> times;
  for(int frame=0;frame<400;++frame){ring.valueNorm=cap.valueNorm=float(frame%101)/100.f;auto start=Clock::now();
   for(auto& surface:surfaces[mode])surface.render(Vec(1,1),Vec(0,0),Rect::inf());
   nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);
   for(int i=0;i<4;++i){nvgSave(vg);nvgTranslate(vg,12+52*i,25);surfaces[mode][i].draw(args);nvgRestore(vg);}nvgEndFrame(vg);double elapsed=us(start);glFlush();if(frame>=100)times.push_back(elapsed);
  }
  char label[100];std::snprintf(label,sizeof(label),"repeat%d/full-cache-mode%d/update-present4",repeat,mode);report(label,times);
 }
 widget::Widget::ContextDestroyEvent destroy;destroy.vg=vg;for(auto& group:surfaces)for(auto& surface:group)surface.onContextDestroy(destroy);
 // Both draw contexts share one texture each; switching contexts must not rebuild.
 const int hostImage=eclipse2_cap::imageFor(vg), nestedImage=eclipse2_cap::imageFor(fvg);
 require(hostImage>0&&nestedImage>0,"both context images");
 require(eclipse2_cap::imageFor(vg)==hostImage&&eclipse2_cap::imageFor(fvg)==nestedImage,"stable alternating context images");
 retained.onContextDestroy(destroy);
 for(auto& cached:eclipse2_cap::images())require(!cached.owner,"destroy clears main and nested handles");
 widget::Widget::ContextCreateEvent create;create.vg=vg;retained.onContextCreate(create);
 require(eclipse2_cap::imageFor(vg)>0&&eclipse2_cap::imageFor(fvg)>0,"context events rebuild lazily");
 eclipse2_cap::forgetWindow(vg);
 require(eclipse2_cap::prepare(svg)&&eclipse2_cap::raster().builds==1,"cap pixels retained across context events");
 std::puts("LIVE checks passed: asset parity, debug gate, SVG fallbacks, context sharing and event invalidation");
 nvgDeleteImage(fvg,fbImage);nvgDeleteImage(vg,image);require(glGetError()==GL_NO_ERROR,"component GL errors");
 scene.onContextDestroy(destroy);APP->scene=nullptr;
}
#include "main.inc"
