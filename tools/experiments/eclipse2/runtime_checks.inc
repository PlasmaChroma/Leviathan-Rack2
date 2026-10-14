// Compare an uninterrupted host recording with a first-use bake inserted between
// its draws, both on the main target and inside a nested framebuffer. Also leave
// non-default pixel-pack state for the baker to preserve.
static void runtimeRecordingChecks(NVGcontext* mainVg) {
 for(bool nested:{false,true}) {
  auto* target=nested?nvgluCreateFramebuffer(mainVg,256,128,0):nullptr;
  require(!nested||target,"recording test FBO");
  auto* vg=nested?APP->window->fbVg:mainVg;
  std::vector<unsigned char> before(256*128*4),after(before.size());
  for(int mode=0;mode<2;++mode) {
   nvgluBindFramebuffer(target);glViewport(0,0,256,128);glClearColor(.12f,.09f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   nvgBeginFrame(vg,256,128,1);nvgTranslate(vg,7.5f,4.25f);nvgGlobalAlpha(vg,.6f);nvgScissor(vg,0,0,170,90);
   nvgBeginPath(vg);nvgCircle(vg,40,40,29);nvgFillColor(vg,nvgRGB(170,80,190));nvgFill(vg);
   if(mode) {
    glPixelStorei(GL_PACK_ALIGNMENT,8);glPixelStorei(GL_PACK_ROW_LENGTH,17);
    glPixelStorei(GL_PACK_SKIP_ROWS,2);glPixelStorei(GL_PACK_SKIP_PIXELS,3);
    eclipse2_bake::Raster raster;
    auto callback=[](const Widget::DrawArgs& a,void*) {nvgBeginPath(a.vg);nvgRect(a.vg,0,0,34,34);nvgFillColor(a.vg,nvgRGBA(255,100,20,160));nvgFill(a.vg);};
    require(raster.prepare(callback,nullptr),"bake during active recording");
    require(raster.prepare(callback,nullptr)&&raster.builds==1,"one-time CPU bake");
    GLint value=0;glGetIntegerv(GL_PACK_ALIGNMENT,&value);require(value==8,"pack alignment restored");
    glGetIntegerv(GL_PACK_ROW_LENGTH,&value);require(value==17,"pack row length restored");
    glGetIntegerv(GL_PACK_SKIP_ROWS,&value);require(value==2,"pack rows restored");
    glGetIntegerv(GL_PACK_SKIP_PIXELS,&value);require(value==3,"pack pixels restored");
    glPixelStorei(GL_PACK_ALIGNMENT,4);glPixelStorei(GL_PACK_ROW_LENGTH,0);glPixelStorei(GL_PACK_SKIP_ROWS,0);glPixelStorei(GL_PACK_SKIP_PIXELS,0);
   }
   nvgBeginPath(vg);nvgRoundedRect(vg,30,30,120,50,12);nvgFillColor(vg,nvgRGB(30,210,120));nvgFill(vg);nvgEndFrame(vg);
   glReadPixels(0,0,256,128,GL_RGBA,GL_UNSIGNED_BYTE,(mode?after:before).data());
  }
  require(before==after,"host recording survives cold runtime bake pixel-exactly");
  nvgluBindFramebuffer(nullptr);if(target)nvgluDeleteFramebuffer(target);
 }
 require(glGetError()==GL_NO_ERROR,"runtime bake driver state");
 std::puts("Runtime bake active-recording and GL-state checks passed");
}
