#include "visual/ApertureBloomMasks.hpp"
static void run(NVGcontext* vg){
 require(isApertureBloomMasksEnabled(),"shared masks enabled at startup");
 pilotDebug=false;setApertureBloomMasksEnabled(false);
 require(isApertureBloomMasksEnabled(),"non-debug renderer cannot be disabled by debug override");
 pilotDebug=true;
 engine::Module module;module.config(0,0,0,3);AmberGreenVioletApertureLight light;light.module=&module;light.firstLightId=0;
 widget::Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(-100,-100),Vec(400,400));
 auto begin=[&](){nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256/APP->window->pixelRatio,128/APP->window->pixelRatio,APP->window->pixelRatio);};
 std::vector<unsigned char> ref(256*128*4),actual(ref.size());int maximum=0,cases=0;
 for(auto size:{ApertureLightSize::Tiny,ApertureLightSize::Small,ApertureLightSize::Medium,ApertureLightSize::Large}){
  light.applySize(size);
  for(float dpi:{1.f,2.f})for(float scale:{.5f,.75f,1.f,1.5f,2.f,3.f})for(float brightness:{.05f,.4f,1.f})for(int tint=0;tint<3;++tint)for(float phase:{0.f,.37f,.81f}){
   APP->window->pixelRatio=dpi;module.lights[0].setBrightness(brightness);module.lights[1].setBrightness(brightness*.6f);module.lights[2].setBrightness(brightness*.25f);
   light.staticBackgroundFb->render(Vec(scale,scale),Vec(0,0),Rect::inf());
   for(int pilot=0;pilot<2;++pilot){setApertureBloomMasksEnabled(pilot);light.normalChangedAt=light.bloomChangedAt=system::getTime();light.normalLightFb->setDirty();light.bloomFb->setDirty();
    begin();nvgTranslate(vg,40.f+phase,40.81f-phase);nvgScale(vg,scale,scale);
    if(tint==1)nvgGlobalAlpha(vg,.5f);if(tint==2)nvgGlobalTint(vg,nvgRGBAf(.7f,.85f,.6f,.8f));
    nvgScissor(vg,-20,-20,light.box.size.x+18,light.box.size.y+40);
    light.drawBackground(args);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);light.drawLight(args);nvgEndFrame(vg);
    glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());if(!pilot)ref=actual;else for(size_t k=0;k<actual.size();++k)maximum=std::max(maximum,std::abs(int(actual[k])-int(ref[k])));
   }++cases;
  }
 }
 std::printf("PILOT IMAGE cases=%d max_byte=%d\n",cases,maximum);require(maximum<=6,"full widget pilot image tolerance");
 auto& cache=*aperture_bloom::findCache(vg,false);require(cache.count==8,"two shared masks per preset");
 auto* second=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(second,"second NanoVG context");
 auto* secondCache=aperture_bloom::findCache(second,true);require(secondCache&&secondCache!=&cache&&secondCache->get(second,1,8),"independent context masks");
 require(aperture_bloom::findCache(vg,false)==&cache&&cache.count==8,"alternating contexts preserve first cache");
 secondCache->clear(second,true);nvgDeleteGL2(second);
 APP->window->pixelRatio=1;const unsigned builds=cache.builds;
 light.applySize(ApertureLightSize::Small);module.lights[0].setBrightness(.6f);
 light.staticBackgroundFb->render(Vec(1,1),Vec(0,0),Rect::inf());
 begin();light.drawBackground(args);light.drawLight(args);nvgEndFrame(vg);require(cache.builds==builds,"same geometry reuses masks");
 light.bloomChangedAt-=1;begin();light.drawLight(args);nvgEndFrame(vg);require(light.bloomFb->getFramebuffer()&&!light.bloomFb->dirty&&!light.bloomFb->bypassed,"pilot still settles to one framebuffer");
 args.fb=light.bloomFb->getFramebuffer();light.bloomChangedAt=system::getTime();begin();light.drawLight(args);nvgEndFrame(vg);require(light.bloomFb->bypassed,"nested framebuffer falls back");args.fb=nullptr;
 light.bloomFb->bypassed=false;begin();nvgRotate(vg,.2f);light.drawLight(args);nvgEndFrame(vg);require(light.bloomFb->bypassed,"rotation falls back");
 setApertureBloomMasksEnabled(false);require(!isApertureBloomMasksEnabled(),"debug baseline override works");
 pilotDebug=false;require(isApertureBloomMasksEnabled(),"leaving debug restores optimized default");
 module.lights[0].setBrightness(.7f);light.bloomChangedAt=system::getTime();
 begin();light.drawLight(args);nvgEndFrame(vg);require(cache.count==8,"non-debug rendering reuses shared masks");
 pilotDebug=true;setApertureBloomMasksEnabled(true);
 settings::haloBrightness=0;begin();light.drawLight(args);nvgEndFrame(vg);require(light.bloomCacheGlow<0,"pilot respects bloom disabled");settings::haloBrightness=.5f;
 module.lights[1].setBrightness(0);module.lights[2].setBrightness(0);
 for(int repeat=0;repeat<4;++repeat)for(int order=0;order<2;++order){int pilot=(order+repeat)%2;setApertureBloomMasksEnabled(pilot);std::vector<double> times;
  for(int frame=0;frame<1100;++frame){module.lights[0].setBrightness(.2f+.6f*float(frame%127)/127.f);begin();auto start=Clock::now();
   for(int i=0;i<8;++i){nvgSave(vg);nvgTranslate(vg,8.f+28*i,30);light.drawBackground(args);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);light.drawLight(args);nvgRestore(vg);}
   double elapsed=us(start);nvgEndFrame(vg);glFlush();if(frame>=100)times.push_back(elapsed);
  }
  char label[80];std::snprintf(label,sizeof(label),"live-pilot/repeat%d/enabled%d/draw8",repeat,pilot);report(label,times);
 }
 setApertureBloomMasksEnabled(true);
 auto* m=cache.get(vg,1,8);require(m,"custom mask");nvgDeleteImage(vg,m->image);const unsigned before=cache.builds;require(cache.get(vg,1,8)&&cache.builds==before+1,"invalid image rebuilt");
 for(int i=0;i<40;++i)cache.get(vg,.1f,10.f+i*.1f);require(cache.count==32,"cache is bounded");require(!cache.get(vg,.2f,25.f),"full cache falls back without eviction");
 widget::Widget::ContextDestroyEvent destroy;destroy.vg=vg;light.onContextDestroy(destroy);require(cache.count==0&&!cache.context,"context destroy clears shared masks");
 widget::Widget::ContextCreateEvent create;create.vg=vg;light.onContextCreate(create);
 light.staticBackgroundFb->render(Vec(1,1),Vec(0,0),Rect::inf());begin();light.drawBackground(args);light.drawLight(args);nvgEndFrame(vg);require(cache.count==2,"context recreation rebuilds lazily");
 light.onContextDestroy(destroy);setApertureBloomMasksEnabled(false);
 require(glGetError()==GL_NO_ERROR,"pilot GL errors");std::puts("PASS: bloom pilot sharing, settlement, invalid handles, bounded cache and lifecycle");
}
