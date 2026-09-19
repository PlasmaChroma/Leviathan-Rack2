#include "fixture.hpp"
#include "Eclipse2RingShader.hpp"
#include "visual/Eclipse2RingCache.hpp"
#include "reference.inc"
Plugin* pluginInstance=nullptr;
namespace rack { namespace app {
Scene::Scene():internal(nullptr),rackScroll(nullptr),rack(nullptr),menuBar(nullptr),browser(nullptr){}
Scene::~Scene(){}
} }

static void run(NVGcontext* vg) {
 Plugin plugin;plugin.path=".";pluginInstance=&plugin;
 app::Scene scene;APP->scene=&scene;
 eclipse2_ring::Ring rings[4];for(auto& ring:rings)ring.box.size=Vec(34,34);
 eclipse2_ring_cache::Ring liveRings[4];for(auto& ring:liveRings)ring.box.size=Vec(34,34);
 widget::Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect::inf();
 // Bake the static track once, outside all timed rendering and NanoVG frames.
 auto* target=nvgluCreateFramebuffer(vg,136,136,0);require(target,"static track target");
 nvgluBindFramebuffer(target);glViewport(0,0,136,136);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 nvgBeginFrame(APP->window->fbVg,34,34,4);auto bakeArgs=args;bakeArgs.vg=APP->window->fbVg;drawTrackReference(bakeArgs);nvgEndFrame(bakeArgs.vg);
 std::vector<unsigned char> bottomUp(136*136*4),track(bottomUp.size());glReadPixels(0,0,136,136,GL_RGBA,GL_UNSIGNED_BYTE,bottomUp.data());
 for(int y=0;y<136;++y)std::copy(bottomUp.data()+(135-y)*136*4,bottomUp.data()+(136-y)*136*4,track.data()+y*136*4);
 {FILE* file=std::fopen("build/eclipse2-track-4x.rgba","wb");require(file,"track bake");std::fwrite(track.data(),1,track.size(),file);std::fclose(file);}
 const auto coldStart=Clock::now();require(eclipse2_ring_cache::prepare(),"runtime track bake");
 std::printf("COLD track bake_us=%.3f\n",us(coldStart));
 require(eclipse2_ring_cache::bakedPixels()==track,"runtime track matches source bake");
 require(!eclipse2_ring_cache::enabled(),"ring cache starts disabled");
 eclipse2_ring_cache::setEnabled(true);debugEnabled=false;require(!eclipse2_ring_cache::enabled(),"non-debug baseline");debugEnabled=true;
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 eclipse2_ring::trackHandle()=nvgCreateImageRGBA(vg,136,136,NVG_IMAGE_PREMULTIPLIED|NVG_IMAGE_GENERATE_MIPMAPS,track.data());
 eclipse2_ring::nestedTrackHandle()=nvgCreateImageRGBA(APP->window->fbVg,136,136,NVG_IMAGE_PREMULTIPLIED|NVG_IMAGE_GENERATE_MIPMAPS,track.data());
 const eclipse2_ring::Mode modes[]={eclipse2_ring::Mode::Baseline,eclipse2_ring::Mode::Geometry,eclipse2_ring::Mode::Masks,eclipse2_ring::Mode::Static,eclipse2_ring::Mode::Shader,eclipse2_ring::Mode::Shader};
 const char* names[]={"baseline","geometry","masks","static","shader","batched-shader"};
 std::vector<unsigned char> baseline(256*128*4),actual(baseline.size());
 int maximum[5]={};double worstMean[5]={};int cases=0;
 for(float scale:{.75f,1.f,1.5f,2.f,3.f})for(float value:{0.f,.125f,.5f,.73f,1.f})for(float brightness:{0.f,.5f,1.f,1.5f})for(bool bipolar:{false,true}) {
  settings::haloBrightness=brightness;rings[0].valueNorm=value;rings[0].bipolar=bipolar;
  for(int mode=0;mode<5;++mode) {
   eclipse2_ring_cache::setEnabled(mode==3);liveRings[0].valueNorm=value;liveRings[0].bipolar=bipolar;
   eclipse2_ring::setMode(modes[mode]);nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,40.37f,20.81f);nvgScale(vg,scale,scale);if(mode==3)liveRings[0].draw(args);else rings[0].draw(args);nvgEndFrame(vg);
   glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
   if(!mode)baseline=actual;else {
    long long sum=0;for(size_t i=0;i<actual.size();++i){int d=std::abs(int(actual[i])-int(baseline[i]));maximum[mode]=std::max(maximum[mode],d);sum+=d;}
    worstMean[mode]=std::max(worstMean[mode],double(sum)/(34*34*scale*scale*4));
   }
   if(scale==3.f&&value==.73f&&brightness==.5f&&!bipolar) {char fileName[128];std::snprintf(fileName,sizeof(fileName),"build/eclipse2-ring-%s.rgba",names[mode]);FILE* file=std::fopen(fileName,"wb");require(file,"snapshot");std::fwrite(actual.data(),1,actual.size(),file);std::fclose(file);}
  }
  ++cases;
 }
 require(!rings[0].failed&&rings[0].program,"shader compiled and drew");
 require(maximum[1]==0,"geometry exact image parity");
 for(int mode=1;mode<5;++mode)std::printf("IMAGE %s cases=%d max=%d mean=%.4f\n",names[mode],cases,maximum[mode],worstMean[mode]);
 // The live class must take the exact baseline path for unsupported geometry.
 eclipse2_ring::setMode(eclipse2_ring::Mode::Baseline);eclipse2_ring_cache::setEnabled(true);
 for(int fallback=0;fallback<4;++fallback) {
  rings[0].box.size=liveRings[0].box.size=fallback==0?Vec(42,42):Vec(34,34);
  rings[0].numLeds=liveRings[0].numLeds=fallback==1?17:25;
  rings[0].minAngle=liveRings[0].minAngle=fallback==2?-1.f:float(-.83f*M_PI);
  if(fallback==3)eclipse2_ring_cache::setEnabled(false);
  for(int candidate=0;candidate<2;++candidate) {
   nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,40.37f,20.81f);nvgScale(vg,1.5f,1.5f);nvgGlobalAlpha(vg,.5f);
   if(candidate)liveRings[0].draw(args);else rings[0].draw(args);nvgEndFrame(vg);
   glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,(candidate?actual:baseline).data());
  }
  require(actual==baseline,"live ring baseline/fallback pixel parity");
 }
 rings[0].box.size=liveRings[0].box.size=Vec(34,34);rings[0].numLeds=liveRings[0].numLeds=25;
 rings[0].minAngle=liveRings[0].minAngle=float(-.83f*M_PI);
 settings::haloBrightness=.5f;for(auto& ring:rings)ring.bipolar=false;for(auto& ring:liveRings)ring.bipolar=false;
 // Actual nested NanoVG path with all shader preparation included in the timer.
 struct Content:Widget{eclipse2_ring::Ring* ring;eclipse2_ring_cache::Ring* live;void draw(const DrawArgs& args)override{if(eclipse2_ring_cache::enabled())live->draw(args);else ring->draw(args);}};
 widget::FramebufferWidget frames[4];
 for(int i=0;i<4;++i){frames[i].box.size=Vec(34,34);frames[i].dirtyOnSubpixelChange=false;auto* c=new Content;c->ring=&rings[i];c->live=&liveRings[i];c->box.size=Vec(34,34);frames[i].addChild(c);}
 for(int repeat=0;repeat<3;++repeat)for(int order=0;order<6;++order) {
  int mode=(order+repeat)%6;eclipse2_ring::setMode(modes[mode]);std::vector<double> times;
  eclipse2_ring_cache::setEnabled(mode==3);
  for(int frame=0;frame<500;++frame){
   for(auto& ring:rings)ring.valueNorm=float(frame%101)/100.f;
   for(auto& ring:liveRings)ring.valueNorm=float(frame%101)/100.f;
   auto start=Clock::now();
   if(mode==5) {
    visual_assets::AdaptiveGlSurface::Update updates[4];auto prepareArgs=args;prepareArgs.vg=APP->window->fbVg;
    for(int i=0;i<4;++i)require(rings[i].prepareUpdate(prepareArgs,updates[i]),"batch prepare");
    require(visual_assets::AdaptiveGlSurface::renderBatch(prepareArgs.vg,updates,4),"batch render");
   }
   for(auto& f:frames)f.render(Vec(1,1),Vec(0,0),Rect::inf());
   nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);
   for(int i=0;i<4;++i){nvgSave(vg);nvgTranslate(vg,12+52*i,25);frames[i].draw(args);nvgRestore(vg);}nvgEndFrame(vg);
   double elapsed=us(start);glFlush();if(frame>=100)times.push_back(elapsed);
  }
  char label[100];std::snprintf(label,sizeof(label),"repeat%d/%s/update-present4",repeat,names[mode]);report(label,times);
 }
 widget::Widget::ContextDestroyEvent destroy;destroy.vg=vg;
 const int mainHandle=eclipse2_ring_cache::imageFor(vg),nestedHandle=eclipse2_ring_cache::imageFor(APP->window->fbVg);
 require(mainHandle>0&&nestedHandle>0&&eclipse2_ring_cache::imageFor(vg)==mainHandle,"shared independent images");
 nvgDeleteImage(vg,mainHandle);require(eclipse2_ring_cache::imageFor(vg)>0,"invalid image recreated");
 widget::Widget::ContextCreateEvent create;create.vg=vg;liveRings[0].onContextCreate(create);
 for(auto& image:eclipse2_ring_cache::images())require(!image.owner,"context create forgets both contexts");
 require(eclipse2_ring_cache::imageFor(vg)>0,"rebuild after context event");
 require(eclipse2_ring_cache::prepare()&&eclipse2_ring_cache::raster().builds==1,"track pixels retained across context events");
 for(auto& f:frames)f.onContextDestroy(destroy);for(auto& ring:rings)ring.onContextDestroy(destroy);
 for(auto& ring:liveRings)ring.onContextDestroy(destroy);
 nvgDeleteImage(vg,eclipse2_ring::trackHandle());nvgDeleteImage(APP->window->fbVg,eclipse2_ring::nestedTrackHandle());
 scene.onContextDestroy(destroy);APP->scene=nullptr;
 require(glGetError()==GL_NO_ERROR,"ring GL state/lifecycle");
 std::puts("LIVE ring cache checks passed: asset, gate, fallbacks, context sharing and invalidation");
}
#include "main.inc"
