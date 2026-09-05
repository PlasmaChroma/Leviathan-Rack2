// Uses the live Proc geometry/stroke implementation, with its actual DSP math.
#define PROC_HEADLESS_TEST 1
#include "../src/Proc.cpp"
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdlib>

using Geometry = ProcPreviewGeometry<Proc>;
using Clock = std::chrono::steady_clock;
static void require(bool ok, const char* why) {
 if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
static double us(Clock::time_point start) {
 return std::chrono::duration<double, std::micro>(Clock::now() - start).count();
}
static void report(const char* label, std::vector<double> values) {
 if (values.empty()) { std::printf("%s,unavailable\n", label); return; }
 std::sort(values.begin(), values.end());
 std::printf("%s,n=%zu,median_us=%.3f,p95_us=%.3f\n", label, values.size(), values[values.size()/2], values[size_t((values.size()-1)*.95)]);
}
static void geometryBench() {
 Geometry geometry;
 geometry.rebuildPoints(Vec(106,48), 1, 2, .6f, false);
 for (int mode=0; mode<3; ++mode) {
  std::vector<double> times;
  for (int batch=0; batch<55; ++batch) {
   auto start=Clock::now();
   for (int i=0; i<1000; ++i) {
    if (mode==0) geometry.rebuildSimplifiedPaths();
    else geometry.rebuildPoints(Vec(106,48), 1.f+i*.001f, 2, mode==1 ? .6f : .2f+i*.0006f, false);
    asm volatile("" : : "g"(geometry.points.data()) : "memory");
   }
   if(batch>=5) times.push_back(us(start)/1000);
  }
  report(mode==0?"geometry/simplify only":mode==1?"geometry/ratio rebuild warm LUT":"geometry/shape rebuild including LUT",times);
 }
}
struct Queries {
 struct Slot { GLuint id=0; bool pending=false; bool measured=false; };
 Slot slots[640];
 bool supported=GLEW_ARB_timer_query;
 std::vector<double> results;
 int skipped=0;
 Queries() { if(supported) for(auto& s:slots) glGenQueries(1,&s.id); }
 void collect() {
  if(!supported) return;
  for(auto& s:slots) if(s.pending) {
   GLint ready=0; glGetQueryObjectiv(s.id,GL_QUERY_RESULT_AVAILABLE,&ready);
   if(ready) {
    GLuint64 ns=0; glGetQueryObjectui64v(s.id,GL_QUERY_RESULT,&ns);
    if(s.measured) results.push_back(double(ns)*.001);
    s.pending=false;
   }
  }
 }
 Slot* begin(int frame) {
  if(!supported) return nullptr;
  collect(); auto& s=slots[frame%640];
  if(s.pending) { if(frame>=100) ++skipped; return nullptr; }
  s.measured=frame>=100; glBeginQuery(GL_TIME_ELAPSED,s.id); return &s;
 }
 void end(Slot* s) { if(s) { glEndQuery(GL_TIME_ELAPSED); s->pending=true; } }
 void finish() {
  glFlush(); auto start=Clock::now();
  while(us(start)<5000000) {
   collect(); bool pending=false; for(auto& s:slots) pending|=s.pending;
   if(!pending) break;
   std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for(auto& s:slots) if(s.pending && s.measured) ++skipped;
 }
 ~Queries() { if(supported) for(auto& s:slots) glDeleteQueries(1,&s.id); }
};
static void beginFrame(NVGcontext* vg,NVGLUframebuffer* f,int w,int h,float scale) {
 nvgluBindFramebuffer(f); glViewport(0,0,w,h);
 glClearColor(0,0,0,0); glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 nvgBeginFrame(vg,float(w)/scale,float(h)/scale,scale);
}
static void contour(NVGcontext* vg,Geometry& g) {
 g.drawWaveSegment(vg,0,Geometry::POINT_COUNT-1,nvgRGBA(230,230,220,255));
}
static void scenario(NVGcontext* vg,int scale,int count,bool cached,bool changing) {
 const int w=106*scale,h=48*scale;
 auto* target=nvgluCreateFramebuffer(vg,w*count,h,0);
 require(target,"output framebuffer");
 std::vector<NVGLUframebuffer*> caches;
 std::vector<Geometry> geometry(count);
 for(int j=0;j<count;++j) {
  geometry[j].rebuildPoints(Vec(106,48),1,2,.6f,false);
  if(cached) { caches.push_back(nvgluCreateFramebuffer(vg,w,h,0)); require(caches.back(),"contour framebuffer"); }
 }
 Queries queries;
 std::vector<double> cpu;
 for(int frame=0;frame<600;++frame) {
  // Geometry cost measured separately, excluded from rendering submission time.
  if(changing) for(auto& g:geometry) g.rebuildPoints(Vec(106,48),1.f+(frame%100)*.01f,2,.6f,false);
  auto* query=queries.begin(frame);
  auto start=Clock::now();
  if(cached && (frame==0||changing)) for(int j=0;j<count;++j) {
   beginFrame(vg,caches[j],w,h,float(scale)); contour(vg,geometry[j]); nvgEndFrame(vg);
  }
  beginFrame(vg,target,w*count,h,float(scale));
  for(int j=0;j<count;++j) {
   nvgSave(vg); nvgTranslate(vg,float(j)*106,0);
   if(cached) {
    auto paint=nvgImagePattern(vg,0,0,106,48,0,caches[j]->image,1);
    nvgBeginPath(vg); nvgRect(vg,0,0,106,48); nvgFillPaint(vg,paint); nvgFill(vg);
   } else contour(vg,geometry[j]);
   nvgRestore(vg);
  }
  nvgEndFrame(vg);
  double elapsed=us(start);
  queries.end(query); glFlush();
  if(frame>=100) cpu.push_back(elapsed);
 }
 queries.finish();
 require(glGetError()==GL_NO_ERROR,"GL render errors");
 // Outside timings: ensure rendering produced visible content.
 nvgluBindFramebuffer(target);
 std::vector<unsigned char> pixels(size_t(w*count*h*4));
 glReadPixels(0,0,w*count,h,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
 uint64_t alpha=0; for(size_t i=3;i<pixels.size();i+=4) alpha+=pixels[i];
 static std::vector<unsigned char> reference;
 if(!cached) reference=pixels;
 else {
  require(reference.size()==pixels.size(),"matching direct image dimensions");
  uint64_t error=0; int maximum=0;
  for(size_t i=0;i<pixels.size();++i) {
   int delta=std::abs(int(pixels[i])-int(reference[i])); error+=delta; maximum=std::max(maximum,delta);
  }
  const double mean=double(error)/pixels.size();
  std::printf("cached_vs_direct_mean_byte_error=%.6f,max_byte_error=%d\n",mean,maximum);
  require(mean<1.0,"cached contour matches direct output");
 }
 require(alpha>0,"nonempty contour output");
 char label[160];
 std::snprintf(label,sizeof(label),"%s/%s/%dx/%d contours/CPU",cached?"cached":"direct",changing?"changing":"stationary",scale,count); report(label,cpu);
 std::snprintf(label,sizeof(label),"%s/%s/%dx/%d contours/GPU",cached?"cached":"direct",changing?"changing":"stationary",scale,count); report(label,queries.results);
 std::printf("gpu_missing=%d,alpha_sum=%llu\n",queries.skipped,(unsigned long long)alpha);
 nvgluBindFramebuffer(nullptr);
 for(auto* f:caches) nvgluDeleteFramebuffer(f);
 nvgluDeleteFramebuffer(target);
}
int main() {
 geometryBench();
 require(glfwInit(),"GLFW"); glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* window=glfwCreateWindow(512,256,"Proc preview benchmark",nullptr,nullptr);
 require(window,"hidden context"); glfwMakeContextCurrent(window); glfwSwapInterval(0);
 require(glewInit()==GLEW_OK,"GLEW"); while(glGetError()!=GL_NO_ERROR) {}
 std::printf("GPU=%s; GL=%s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES); require(vg,"NanoVG");
 for(int scale:{1,4}) for(int count:{1,16}) for(bool changing:{false,true}) for(bool cached:{false,true}) scenario(vg,scale,count,cached,changing);
 nvgDeleteGL2(vg); glfwDestroyWindow(window); glfwTerminate();
}
