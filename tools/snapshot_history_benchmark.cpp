// Offscreen backend comparison. Includes deposit passes and NanoVG flushes,
// excludes Rack widget traversal, allocation warmup and geometry generation.
#define PROC_HEADLESS_TEST 1
#include "../src/Proc.cpp"
#include "../src/visual/SnapshotHistory.hpp"
#include "preview_benchmark_utils.hpp"
using Geometry = ProcPreviewGeometry<Proc>;
using History = visual_assets::SnapshotHistory<128, 6>;
static void begin(NVGcontext* vg, NVGLUframebuffer* fb, int scale) {
 nvgluBindFramebuffer(fb); glViewport(0,0,106*scale,48*scale);
 glClearColor(0,0,0,0); glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 nvgBeginFrame(vg,106,48,float(scale));
}
static std::vector<unsigned char> scenario(NVGcontext* vg, int scale, bool cached) {
 auto* target=nvgluCreateFramebuffer(vg,106*scale,48*scale,0); require(target,"target");
 std::array<NVGLUframebuffer*,6> slots{};
 for(auto& s:slots) { s=nvgluCreateFramebuffer(vg,106*scale,48*scale,0); require(s,"slot"); }
 WavePreviewTracer<128,6> tracer;
 WavePreviewTracerStyle style;
 Geometry geometry;
 History::Path path;
 path.style=style;
 std::vector<double> active,fade,idle;
 std::vector<unsigned char> images;
 Queries queries;
 int deposits=0, passiveDeposits=0;
 for(int frame=0;frame<480;++frame) {
  double now=1.+frame/60.;
  bool capture=frame<360 && frame%3==0;
  int slot=tracer.nextFrame;
  if(capture) {
   geometry.rebuildPoints(Vec(106,48),1.f+.5f*std::sin(frame*.03f),2,.6f,false);
   require(tracer.capture(geometry.points,now,.04f,2).captured,"capture cadence");
  }
  auto* query=queries.begin(frame);
  auto start=Clock::now();
  if(cached && capture) {
   begin(vg,slots[slot],scale);
   path.frame=tracer.frames[slot];
   widget::Widget::DrawArgs args{};args.vg=vg;
   path.draw(args); nvgEndFrame(vg);
   ++deposits; if(frame>=360) ++passiveDeposits;
  }
  begin(vg,target,scale);
  if(cached) {
   for(size_t i=0;i<6;++i) {
    const auto& f=tracer.frames[i];float age=float(now-f.birthSec);
    if(!f.active||age<0||age>=style.fadeSec) continue;
    int alpha=clamp(int(style.maxAlpha*(1-age/style.fadeSec)),0,255);
    if(alpha<=0) continue;
    auto paint=nvgImagePattern(vg,0,0,106,48,0,slots[i]->image,float(alpha)/255.f);
    nvgBeginPath(vg);nvgRect(vg,0,0,106,48);nvgFillPaint(vg,paint);nvgFill(vg);
   }
  } else tracer.draw(vg,now,style);
  nvgEndFrame(vg);
  double elapsed=us(start);queries.end(query);glFlush();
  if(frame>=100) (frame<360?active:frame<380?fade:idle).push_back(elapsed);
  if(frame==350||frame==365||frame==390) {
   size_t offset=images.size();images.resize(offset+106*48*scale*scale*4);
   glReadPixels(0,0,106*scale,48*scale,GL_RGBA,GL_UNSIGNED_BYTE,images.data()+offset);
   uint64_t alpha=0;for(size_t j=offset+3;j<images.size();j+=4)alpha+=images[j];
   require(frame==390 ? alpha==0 : alpha>0,"active/fading/expired pixels");
  }
 }
 queries.finish();require(glGetError()==GL_NO_ERROR,"GL errors");
 char label[120];
 for(int phase=0;phase<3;++phase) {
  std::snprintf(label,sizeof(label),"%dx/%s/%s/CPU",scale,cached?"snapshot":"vector",phase==0?"modulation":phase==1?"fade":"expired");
  report(label,phase==0?active:phase==1?fade:idle);
 }
 std::snprintf(label,sizeof(label),"%dx/%s/all phases/GPU",scale,cached?"snapshot":"vector");report(label,queries.results);
 require(!cached || (deposits==120 && passiveDeposits==0),"deposit count");
 nvgluBindFramebuffer(nullptr);for(auto* s:slots)nvgluDeleteFramebuffer(s);nvgluDeleteFramebuffer(target);
 return images;
}
int main() {
 require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* window=glfwCreateWindow(512,256,"Snapshot benchmark",nullptr,nullptr);require(window,"context");
 glfwMakeContextCurrent(window);glfwSwapInterval(0);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 std::printf("GPU=%s; GL=%s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NanoVG");
 for(int scale:{1,4}) {
  auto direct=scenario(vg,scale,false);auto cached=scenario(vg,scale,true);
  require(direct.size()==cached.size(),"image dimensions");
  uint64_t error=0,alpha=0;int maxError=0;
  for(size_t i=0;i<direct.size();++i) {int d=std::abs(int(direct[i])-int(cached[i]));error+=d;maxError=std::max(maxError,d);if(i%4==3)alpha+=direct[i];}
  double mean=double(error)/direct.size();std::printf("%dx image mean byte error=%.6f max=%d\n",scale,mean,maxError);
  require(alpha>0 && mean<1.,"snapshot visual agreement");
 }
 nvgDeleteGL2(vg);glfwDestroyWindow(window);glfwTerminate();
}
