static void run(NVGcontext* vg) {
 require(rack::APP_VERSION=="2.6.6","probe callback ABI is Rack 2.6.6 only");
 engine::Module module;module.config(0,0,0,3);AmberGreenVioletApertureLight light;light.module=&module;light.firstLightId=0;
 auto begin=[&](){nvgluBindFramebuffer(nullptr);glViewport(0,0,256,128);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);nvgBeginFrame(vg,256,128,1);};
 auto primeMasks=[&](){candidate::mask(vg,0,light.lensRadius*.22f);candidate::mask(vg,std::max(.5f,light.coreRadius*.70f),light.bloomRadius*1.35f);candidate::mask(vg,light.coreRadius*.35f,light.lensRadius*1.7f);};
 std::vector<unsigned char> ref(256*128*4),actual(ref.size());int maxByte[7]{},cases=0;double maxMean[7]{};
 for(auto size:{ApertureLightSize::Tiny,ApertureLightSize::Small,ApertureLightSize::Medium,ApertureLightSize::Large}){
  light.applySize(size);primeMasks();
  for(float scale:{.75f,1.f,1.5f,2.f})for(float brightness:{.05f,.4f,1.f})for(int tint=0;tint<3;++tint)for(float phase:{0.f,.37f,.81f}){
   module.lights[0].setBrightness(brightness);module.lights[1].setBrightness(brightness*.6f);module.lights[2].setBrightness(brightness*.25f);light.refreshLightState();
   for(int mode:{0,1,2,5,6}){
    begin();nvgTranslate(vg,40.f+phase,40.81f-phase);nvgScale(vg,scale,scale);
    if(tint==1)nvgGlobalAlpha(vg,.5f);if(tint==2)nvgGlobalTint(vg,nvgRGBAf(.7f,.85f,.6f,.8f));
    nvgScissor(vg,-20,-20,light.box.size.x+18,light.box.size.y+40);
    light.drawStaticBackground(vg);nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);
    candidate::dynamic(light,vg,mode);nvgEndFrame(vg);glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());
    if(mode==0)ref=actual;else{long long sum=0;for(size_t k=0;k<actual.size();++k){int d=std::abs(int(actual[k])-int(ref[k]));maxByte[mode]=std::max(maxByte[mode],d);sum+=d;}maxMean[mode]=std::max(maxMean[mode],double(sum)/actual.size());}
   }++cases;
  }
 }
 for(int mode:{1,2,5,6})std::printf("IMAGE mode=%d cases=%d max_byte=%d max_mean=%.6f\n",mode,cases,maxByte[mode],maxMean[mode]);
 require(maxByte[2]<=6,"retained circle image tolerance");
 require(maxByte[5]<=6,"bloom-only mask image tolerance");
 // Independent same-appearance caches versus one shared socket image.
 TealApertureLight sockets[8];widget::Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(-100,-100),Vec(400,400));
 for(auto& s:sockets)s.staticBackgroundFb->render(Vec(1,1),Vec(0,0),Rect::inf());
 int socketMax=0;
 for(int shared=0;shared<2;++shared){begin();for(int i=0;i<8;++i){nvgSave(vg);nvgTranslate(vg,8.f+28*i,30);(shared?sockets[0]:sockets[i]).staticBackgroundFb->draw(args);nvgRestore(vg);}nvgEndFrame(vg);glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());if(!shared)ref=actual;else for(size_t k=0;k<actual.size();++k)socketMax=std::max(socketMax,std::abs(int(actual[k])-int(ref[k])));}
 std::printf("SOCKET shared_max_byte=%d referenced_images=8->1\n",socketMax);
 require(socketMax==0,"same-appearance shared socket image");
 light.applySize(ApertureLightSize::Small);primeMasks();
 for(int repeat=0;repeat<4;++repeat)for(int order=0;order<7;++order){int mode=(order+repeat*2)%7;std::vector<double> draw,total;
  for(int frame=0;frame<1100;++frame){module.lights[0].setBrightness(.2f+.6f*float(frame%127)/127);light.refreshLightState();begin();auto start=Clock::now();
   for(int i=0;i<8;++i){nvgSave(vg);nvgTranslate(vg,8.f+28*i,30);
    if(mode<3||mode>=5){nvgGlobalCompositeBlendFunc(vg,NVG_ONE_MINUS_DST_COLOR,NVG_ONE);candidate::dynamic(light,vg,mode);}
    else (mode==4?sockets[0]:sockets[i]).staticBackgroundFb->draw(args);
    nvgRestore(vg);
   }
   double submission=us(start);nvgEndFrame(vg);double complete=us(start);glFlush();if(frame>=100){draw.push_back(submission);total.push_back(complete);}
  }
  char label[80];std::snprintf(label,sizeof(label),"repeat%d/mode%d/draw8",repeat,mode);report(label,draw);std::snprintf(label,sizeof(label),"repeat%d/mode%d/flush8",repeat,mode);report(label,total);
 }
 widget::Widget::ContextDestroyEvent destroy;destroy.vg=vg;for(auto& s:sockets)s.onContextDestroy(destroy);
 candidate::clear();require(glGetError()==GL_NO_ERROR,"candidate GL errors");
}
