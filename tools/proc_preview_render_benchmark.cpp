// Uses the live Proc geometry/stroke implementation, with its actual DSP math.
#define PROC_HEADLESS_TEST 1
#include "../src/Proc.cpp"
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdlib>

using Geometry = ProcPreviewGeometry<Proc>;
#include "preview_benchmark_utils.hpp"
#include "experiments/lumin/BoundedPolyline.hpp"
#include <cstring>
#include "experiments/lumin/round_join_candidate.hpp"

static void roundArcCandidateBench(bool reverse) {
 using namespace round_join_candidate;
 struct Join {Normal a,b;};
 for(float scale:{1.f,1.19f,2.f,4.f,8.f}) {
  Policy policy(std::max(2,int(std::ceil(pi/(2*std::acos(.7f/(.7f+.25f/scale)))))));
  std::vector<std::vector<Join>> frames(260);
  for(int frame=100;frame<360;++frame) {
   Geometry g;g.rebuildPoints(Vec(106,48),.05f+1.9f*(.5f+.5f*std::sin(frame*.037f)),
    1.f,.85f*std::sin(frame*.023f),false);
   const auto& path=g.simplifiedFullPath;
   for(int i=1;i+1<path.count;++i) {
    auto p=path.points[size_t(i-1)],q=path.points[size_t(i)],r=path.points[size_t(i+1)];
    float ax=q.x-p.x,ay=q.y-p.y,bx=r.x-q.x,by=r.y-q.y;
    float al=std::hypot(ax,ay),bl=std::hypot(bx,by);
    if(al<=1e-6f||bl<=1e-6f) continue;
    Normal a={ay/al,-ax/al},b={by/bl,-bx/bl};
    if(a.x*b.y-a.y*b.x<0) std::swap(a,b);
    frames[size_t(frame-100)].push_back({a,b});
   }
  }
  double maxError=0;size_t samples=0,joins=0;
  for(const auto& frame:frames) for(const auto& j:frame) {
   Normal ref[64],test[64];int nr=reference(j.a,j.b,policy,ref),nc=candidate(j.a,j.b,policy,test);
   require(nr==nc,"candidate arc sample counts");
   for(int i=0;i<nr;++i) maxError=std::max(maxError,double(std::hypot(ref[i].x-test[i].x,ref[i].y-test[i].y)));
   samples+=nc;++joins;
  }
  require(maxError<2e-6,"candidate arc position error");
  for(int order=0;order<2;++order) {
   const bool optimized=reverse?order==0:order==1;
   std::vector<double> times;
   for(int repeat=0;repeat<6;++repeat) for(const auto& frame:frames) {
    auto start=Clock::now();
    for(int contourIndex=0;contourIndex<16;++contourIndex) for(const auto& j:frame) {
     Normal out[64];int n=optimized?candidate(j.a,j.b,policy,out):reference(j.a,j.b,policy,out);
     asm volatile("" : : "g"(out),"g"(n) : "memory");
    }
    if(repeat>0) times.push_back(us(start));
   }
   char label[160];std::snprintf(label,sizeof(label),"round-arc/%s/%.2fx/16/CPU",optimized?"candidate":"reference",scale);
   report(label,times);
  }
  std::printf("round-arc/%.2fx/ncap=%d,joins=%zu,samples=%zu,max_unit_error=%.9g\n",scale,policy.ncap,joins,samples,maxError);
 }
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
// Representative stroke experiment using the existing Proc geometry fixture.
// It measures the same reduction helper proposed for Flux, not Flux host timing.
static std::vector<unsigned char> reductionScenario(NVGcontext* vg, float scale, int count, bool reduced, bool activeOnly) {
 const int w=int(std::ceil(106*scale)),h=int(std::ceil(48*scale));
 auto* target=nvgluCreateFramebuffer(vg,w*count,h,0); require(target,"reduction target");
 std::vector<Geometry> geometry(count);
 std::vector<double> cpu, preparation;
 std::vector<unsigned char> images;
 Queries queries;
 uint64_t pointsBefore=0,pointsAfter=0;
 for(int frame=0;frame<360;++frame) {
  for(int j=0;j<count;++j) geometry[j].rebuildPoints(Vec(106,48),
   .05f+1.9f*(.5f+.5f*std::sin(frame*.037f)),
   1.f, .85f*std::sin(frame*.023f), false);
  auto start=Clock::now();
  for(auto& g:geometry) {
   if(frame>=100) pointsBefore+=g.simplifiedFullPath.count;
   if(reduced) {
    auto reduce=[](Geometry::SimplifiedPreviewPath& path) {
     path.count=int(leviathan::render::reducePreviewPolyline(path.points,size_t(path.count)));
    };
    reduce(g.simplifiedFullPath);
    if(!activeOnly) {reduce(g.simplifiedRisePath);reduce(g.simplifiedFallPath);}
   }
   if(frame>=100) pointsAfter+=g.simplifiedFullPath.count;
  }
  const double prep=us(start);
  auto* query=queries.begin(frame);
  start=Clock::now();
  beginFrame(vg,target,w*count,h,scale);
  for(int j=0;j<count;++j) {
   nvgSave(vg);nvgTranslate(vg,float(j*w)/scale,0);contour(vg,geometry[j]);nvgRestore(vg);
  }
  nvgEndFrame(vg);
  const double submit=us(start);
  queries.end(query);glFlush();
  if(frame>=100) { cpu.push_back(prep+submit);preparation.push_back(prep); }
  if(frame==120||frame==240||frame==359) {
   size_t offset=images.size();images.resize(offset+size_t(w*count*h*4));
   glReadPixels(0,0,w*count,h,GL_RGBA,GL_UNSIGNED_BYTE,images.data()+offset);
  }
 }
 queries.finish();require(glGetError()==GL_NO_ERROR,"reduction GL errors");
 char label[160];
 const char* variant=reduced?(activeOnly?"C-active-only":"C"):"B";
 std::snprintf(label,sizeof(label),"reduction/%s/%.2fx/%d/CPU including added preparation",variant,scale,count);report(label,cpu);
 std::snprintf(label,sizeof(label),"reduction/%s/%.2fx/%d/added preparation",variant,scale,count);report(label,preparation);
 std::snprintf(label,sizeof(label),"reduction/%s/%.2fx/%d/GPU",variant,scale,count);report(label,queries.results);
 std::printf("vertices_before=%llu,vertices_after=%llu,gpu_missing=%d\n",
  (unsigned long long)pointsBefore,(unsigned long long)pointsAfter,queries.skipped);
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 return images;
}
static void reductionBench(NVGcontext* vg, bool activeOnly, bool reverse) {
 bool visualPass=true;
 for(float scale:{1.f,1.19f,2.f,4.f,8.f}) for(int count:{1,16}) {
  std::vector<unsigned char> baseline,candidate;
  if(reverse) {
   candidate=reductionScenario(vg,scale,count,true,activeOnly);
   baseline=reductionScenario(vg,scale,count,false,activeOnly);
  } else {
   baseline=reductionScenario(vg,scale,count,false,activeOnly);
   candidate=reductionScenario(vg,scale,count,true,activeOnly);
  }
  require(baseline.size()==candidate.size(),"reduction image extents");
  uint64_t error=0,alpha=0;int maximum=0;
  for(size_t i=0;i<baseline.size();i+=4) {
   alpha+=baseline[i+3];
   for(size_t c=0;c<4;++c) {int d=std::abs(int(baseline[i+c])-int(candidate[i+c]));error+=d;maximum=std::max(maximum,d);}
  }
  const double relative=alpha?double(error)/(4.*alpha):1.;
  std::printf("reduction/%.2fx/%d/image_relative_error=%.6f,max_byte_error=%d\n",scale,count,relative,maximum);
  visualPass &= alpha>0 && relative<=.03 && maximum<=64;
 }
 require(visualPass,"reduction image gate: <=3% alpha-normalized RGBA error, max byte error <=64");
}

// Offline-only instrumentation of this benchmark's own NanoVG context. Never
// replace callbacks on a Rack-owned context in the plugin. The original backend
// receives the original user pointer and every argument unchanged.
struct StrokeProbe {
 using Callback=decltype(NVGparams::renderStroke);
 NVGparams* params=nullptr;
 Callback original=nullptr;
 double backendUs=0.;
 uint64_t calls=0,paths=0,vertices=0;
 static StrokeProbe* active;
 explicit StrokeProbe(NVGcontext* vg) : params(nvgInternalParams(vg)) {
  require(params && params->renderStroke && !active,"exclusive stroke probe");
  original=params->renderStroke;active=this;params->renderStroke=invoke;
 }
 ~StrokeProbe() { params->renderStroke=original;active=nullptr; }
 void reset() {backendUs=0.;calls=paths=vertices=0;}
 static void invoke(void* user,NVGpaint* paint,NVGcompositeOperationState composite,
                    NVGscissor* scissor,float fringe,float width,const NVGpath* paths,int count) {
  auto& probe=*active;
  auto start=Clock::now();
  probe.original(user,paint,composite,scissor,fringe,width,paths,count);
  probe.backendUs+=us(start);
  ++probe.calls;probe.paths+=uint64_t(count);
  for(int i=0;i<count;++i) probe.vertices+=uint64_t(paths[i].nstroke);
 }
};
StrokeProbe* StrokeProbe::active=nullptr;

static std::vector<unsigned char> strokeProfileScenario(NVGcontext* vg,float scale,int count,bool instrumented,
                                                       int join=NVG_ROUND,const char* label=nullptr) {
 const int w=int(std::ceil(106*scale)),h=int(std::ceil(48*scale));
 auto* target=nvgluCreateFramebuffer(vg,w*count,h,0);require(target,"stroke profile target");
 std::vector<Geometry> geometry(count);
 std::vector<double> total,frameSetup,pathCommands,style,strokeInclusive,backend,frontend,endFrame,other,frontendShare;
 std::vector<unsigned char> images;
 Queries queries;
 uint64_t inputPoints=0,outputVertices=0,strokeCalls=0,outputPaths=0;
 // Install once outside measured frames. The uninstrumented control uses the
 // live geometry draw method and the original unwrapped backend throughout.
 StrokeProbe* probe=instrumented?new StrokeProbe(vg):nullptr;
 for(int frame=0;frame<360;++frame) {
  for(auto& g:geometry) g.rebuildPoints(Vec(106,48),
   .05f+1.9f*(.5f+.5f*std::sin(frame*.037f)),1.f,.85f*std::sin(frame*.023f),false);
  if(probe) probe->reset();
  auto* query=queries.begin(frame);
  double commandsUs=0.,styleUs=0.,strokeUs=0.,setupUs=0.,flushUs=0.;
  auto start=Clock::now();
  beginFrame(vg,target,w*count,h,scale);
  if(instrumented) setupUs=us(start);
  for(int j=0;j<count;++j) {
   nvgSave(vg);nvgTranslate(vg,float(j*w)/scale,0);
   auto& g=geometry[j];
   if(instrumented) {
    const auto& path=g.simplifiedFullPath;
    auto part=Clock::now();
    nvgBeginPath(vg);nvgMoveTo(vg,path.points[0].x,path.points[0].y);
    for(int i=1;i<path.count;++i) nvgLineTo(vg,path.points[size_t(i)].x,path.points[size_t(i)].y);
    commandsUs+=us(part);
    part=Clock::now();
    nvgStrokeColor(vg,nvgRGBA(230,230,220,255));nvgStrokeWidth(vg,Geometry::WAVE_LINE_WIDTH);
    nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,join);
    styleUs+=us(part);
    part=Clock::now();nvgStroke(vg);strokeUs+=us(part);
   } else contour(vg,g);
   nvgRestore(vg);
  }
  const auto beforeEnd=instrumented?Clock::now():Clock::time_point();
  nvgEndFrame(vg);
  if(instrumented) flushUs=us(beforeEnd);
  const double elapsed=us(start);
  queries.end(query);glFlush();
  if(frame>=100) {
   total.push_back(elapsed);
   for(const auto& g:geometry) inputPoints+=g.simplifiedFullPath.count;
   if(probe) {
    require(strokeUs+1e-6>=probe->backendUs,"nested callback timing fits enclosing stroke");
    frameSetup.push_back(setupUs);pathCommands.push_back(commandsUs);style.push_back(styleUs);
    strokeInclusive.push_back(strokeUs);backend.push_back(probe->backendUs);
    frontend.push_back(strokeUs-probe->backendUs);endFrame.push_back(flushUs);
    other.push_back(elapsed-setupUs-commandsUs-styleUs-strokeUs-flushUs);
    frontendShare.push_back(strokeUs>0?100.*(strokeUs-probe->backendUs)/strokeUs:0.);
    outputVertices+=probe->vertices;strokeCalls+=probe->calls;outputPaths+=probe->paths;
   }
  }
  // Strict image equality with the live draw method guards the instrumented
  // command sequence and callback forwarding. Readback is outside all timers.
  if(frame==120||frame==240||frame==359) {
   const size_t offset=images.size();images.resize(offset+size_t(w*count*h*4));
   glReadPixels(0,0,w*count,h,GL_RGBA,GL_UNSIGNED_BYTE,images.data()+offset);
  }
 }
 queries.finish();require(glGetError()==GL_NO_ERROR,"stroke profile GL errors");
 delete probe; // Restore before any context teardown or next scenario.
 const char* mode=label?label:(instrumented?"instrumented":"control");
 auto emit=[&](const char* metric,const std::vector<double>& samples) {
  char label[180];std::snprintf(label,sizeof(label),"stroke-profile/%s/%.2fx/%d/%s",mode,scale,count,metric);report(label,samples);
 };
 emit("total CPU",total);emit("GPU",queries.results);
 if(instrumented) {
  emit("frame setup CPU",frameSetup);emit("path commands CPU",pathCommands);emit("style CPU",style);
  emit("nvgStroke inclusive CPU",strokeInclusive);emit("backend callback CPU",backend);
  emit("stroke frontend remainder CPU",frontend);emit("nvgEndFrame CPU",endFrame);
  emit("placement and timer overhead CPU",other);
  std::sort(frontendShare.begin(),frontendShare.end());
  std::printf("stroke-profile/%s/%.2fx/%d/frontend_share_of_stroke,n=%zu,median_percent=%.3f,p95_percent=%.3f\n",
   mode,scale,count,frontendShare.size(),frontendShare[frontendShare.size()/2],frontendShare[size_t((frontendShare.size()-1)*.95)]);
 }
 std::printf("stroke-profile/%s/%.2fx/%d/input_points=%llu,stroke_calls=%llu,backend_paths=%llu,expanded_vertices=%llu,gpu_missing=%d\n",
  mode,scale,count,(unsigned long long)inputPoints,(unsigned long long)strokeCalls,
  (unsigned long long)outputPaths,(unsigned long long)outputVertices,queries.skipped);
 if(instrumented) require(strokeCalls==uint64_t(260*count) && outputPaths==strokeCalls && outputVertices>0,"one backend stroke path per contour");
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 return images;
}
static void strokeProfileBench(NVGcontext* vg,bool reverse) {
 for(float scale:{1.f,1.19f,4.f}) for(int count:{1,16}) {
  std::vector<unsigned char> control,instrumented;
  if(reverse) {
   instrumented=strokeProfileScenario(vg,scale,count,true);
   control=strokeProfileScenario(vg,scale,count,false);
  } else {
   control=strokeProfileScenario(vg,scale,count,false);
   instrumented=strokeProfileScenario(vg,scale,count,true);
  }
  require(control==instrumented,"profiled paths exactly match live fixture rendering");
  uint64_t alpha=0;for(size_t i=3;i<control.size();i+=4) alpha+=control[i];
  require(alpha>0,"nonempty stroke profile images");
  std::printf("stroke-profile/%.2fx/%d/image_match=exact,alpha_sum=%llu\n",scale,count,(unsigned long long)alpha);
 }
}
// Join alternatives are diagnostic controls, never production style changes.
static void joinProfileBench(NVGcontext* vg,bool reverse) {
 for(float scale:{1.f,1.19f,4.f}) for(int count:{1,16}) {
  std::vector<unsigned char> images[3],control;
  const int joins[]={NVG_ROUND,NVG_BEVEL,NVG_MITER};
  const char* names[]={"round","bevel","miter"};
  if(!reverse) control=strokeProfileScenario(vg,scale,count,false);
  for(int k=0;k<3;++k) {
   const int j=reverse?2-k:k;
   images[j]=strokeProfileScenario(vg,scale,count,true,joins[j],names[j]);
  }
  if(reverse) control=strokeProfileScenario(vg,scale,count,false);
  require(control==images[0],"round join matches live geometry exactly");
  uint64_t alpha=0;for(size_t i=3;i<control.size();i+=4) alpha+=control[i];
  require(alpha>0,"nonempty join reference");
  for(int j=1;j<3;++j) {
   require(images[j].size()==control.size(),"join image dimensions");
   uint64_t error=0,changed=0;int maximum=0;
   for(size_t i=0;i<control.size();i+=4) {
    bool different=false;
    for(size_t c=0;c<4;++c) {
     const int delta=std::abs(int(images[j][i+c])-int(control[i+c]));
     error+=delta;maximum=std::max(maximum,delta);different|=delta!=0;
    }
    changed+=different;
   }
   std::printf("join-image/%s/%.2fx/%d/alpha_normalized_error=%.8f,max_byte_error=%d,changed_pixels=%llu\n",
    names[j],scale,count,double(error)/(4.*alpha),maximum,(unsigned long long)changed);
  }
  // Contact sheet: rows are sampled frames, columns round/bevel/miter.
  // Composite premultiplied readback over dark gray, flip GL's bottom-up rows.
  if(count==1) {
   const int w=int(std::ceil(106*scale)),h=int(std::ceil(48*scale));
   char path[160];std::snprintf(path,sizeof(path),"build/tools/join-profile-%.2fx.ppm",scale);
   FILE* file=std::fopen(path,"wb");require(file,"join contact sheet");
   std::fprintf(file,"P6\n%d %d\n255\n",w*3,h*3);
   for(int frame=0;frame<3;++frame) for(int y=h-1;y>=0;--y)
    for(int j=0;j<3;++j) for(int x=0;x<w;++x) {
     const size_t offset=size_t((frame*h+y)*w+x)*4;
     for(int c=0;c<3;++c) {
      const unsigned char value=static_cast<unsigned char>(std::min(255,
       int(images[j][offset+c])+24*(255-int(images[j][offset+3]))/255));
      require(std::fwrite(&value,1,1,file)==1,"write join contact sheet");
     }
    }
   require(std::fclose(file)==0,"close join contact sheet");
  }
 }
}
int main(int argc,char** argv) {
 if(argc>=2 && std::strcmp(argv[1],"--round-arc-candidate")==0) {
  require(argc==2||(argc==3 && std::strcmp(argv[2],"--reverse")==0),"round arc arguments");
  roundArcCandidateBench(argc==3);return 0;
 }
 const bool activeOnly=argc>=2 && std::strcmp(argv[1],"--bounded-reduction-active-only")==0;
 const bool reduction=activeOnly || (argc>=2 && std::strcmp(argv[1],"--bounded-reduction")==0);
 const bool strokeProfile=argc>=2 && std::strcmp(argv[1],"--stroke-profile")==0;
 const bool joinProfile=argc>=2 && std::strcmp(argv[1],"--join-profile")==0;
 const bool reverse=argc==3 && std::strcmp(argv[2],"--reverse")==0;
 require(argc==1||((reduction||strokeProfile||joinProfile)&&(argc==2||reverse)),"usage: proc_preview_render_benchmark [--bounded-reduction|--bounded-reduction-active-only|--stroke-profile|--join-profile] [--reverse]");
 if(!reduction&&!strokeProfile&&!joinProfile) geometryBench();
 require(glfwInit(),"GLFW"); glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* window=glfwCreateWindow(512,256,"Proc preview benchmark",nullptr,nullptr);
 require(window,"hidden context"); glfwMakeContextCurrent(window); glfwSwapInterval(0);
 require(glewInit()==GLEW_OK,"GLEW"); while(glGetError()!=GL_NO_ERROR) {}
 std::printf("GPU=%s; GL=%s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES); require(vg,"NanoVG");
 if(joinProfile) joinProfileBench(vg,reverse);
 else if(strokeProfile) strokeProfileBench(vg,reverse);
 else if(reduction) reductionBench(vg,activeOnly,reverse);
 else for(int scale:{1,4}) for(int count:{1,16}) for(bool changing:{false,true}) for(bool cached:{false,true}) scenario(vg,scale,count,cached,changing);
 nvgDeleteGL2(vg); glfwDestroyWindow(window); glfwTerminate();
}
