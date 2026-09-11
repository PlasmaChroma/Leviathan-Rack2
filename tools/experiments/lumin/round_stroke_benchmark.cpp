#include "plugin.hpp"
#include "../../preview_benchmark_utils.hpp"
#include "private_stroke_api.hpp"
#include "flux_geometry.hpp"
#include <cstring>
#include <fstream>

using Points=std::vector<Vec>;
struct Capture {std::vector<NVGvertex> vertices;std::vector<int> counts;};
static Capture* activeCapture=nullptr;
static decltype(NVGparams::renderStroke) originalStroke=nullptr;
static void captureStroke(void* user,NVGpaint* paint,NVGcompositeOperationState composite,
 NVGscissor* scissor,float fringe,float width,const NVGpath* paths,int count) {
 if(activeCapture) for(int i=0;i<count;++i) {
  activeCapture->counts.push_back(paths[i].nstroke);
  activeCapture->vertices.insert(activeCapture->vertices.end(),paths[i].stroke,paths[i].stroke+paths[i].nstroke);
 }
 originalStroke(user,paint,composite,scissor,fringe,width,paths,count);
}
struct Renderer {
 PrivateStrokeApi api;NVGcontext* vg;decltype(NVGparams::renderStroke) backend;
 Renderer(PrivateStrokeApi a):api(a),vg(api.create(NVG_ANTIALIAS|NVG_STENCIL_STROKES)) {
  require(vg,"renderer context");backend=api.params(vg)->renderStroke;api.params(vg)->renderStroke=captureStroke;
 }
 ~Renderer(){api.params(vg)->renderStroke=backend;api.destroy(vg);}
};
struct Result {std::vector<unsigned char> pixels;Capture mesh;};
static void draw(Renderer& r,NVGLUframebuffer* target,int w,int h,float scale,
 const Points& points,float width,int count,bool candidate,bool capture) {
 privateStrokeCandidate(candidate);originalStroke=r.backend;
 nvgluBindFramebuffer(target);glViewport(0,0,w,h);
 glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 r.api.begin(r.vg,float(w)/scale,float(h)/scale,scale);
 for(int j=0;j<count;++j) {
  r.api.path(r.vg);
  for(size_t i=0;i<points.size();++i) {
   float x=points[i].x+j*110.f;
   (i?r.api.line:r.api.move)(r.vg,x,points[i].y);
  }
  r.api.color(r.vg,nvgRGBA(230,230,220,255));r.api.width(r.vg,width);
  r.api.cap(r.vg,NVG_BUTT);r.api.join(r.vg,NVG_ROUND);r.api.stroke(r.vg);
 }
 r.api.end(r.vg);
 (void)capture;
}
static Result render(Renderer& r,NVGLUframebuffer* target,int w,int h,float scale,
 const Points& points,float width,bool candidate) {
 Result result;activeCapture=&result.mesh;
 draw(r,target,w,h,scale,points,width,1,candidate,true);activeCapture=nullptr;
 result.pixels.resize(size_t(w)*h*4);glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,result.pixels.data());
 require(glGetError()==GL_NO_ERROR,"validation GL errors");return result;
}
struct Errors {double mesh=0,relative=0;int byte=0;size_t cases=0,changed=0;};
static void compare(const Result& reference,const Result& candidate,bool exact,Errors& errors) {
 require(reference.mesh.counts==candidate.mesh.counts,"same stroke topology");
 require(reference.mesh.vertices.size()==candidate.mesh.vertices.size(),"same vertex count");
 double mesh=0;
 for(size_t i=0;i<reference.mesh.vertices.size();++i) {
  const auto& a=reference.mesh.vertices[i];const auto& b=candidate.mesh.vertices[i];
  require(std::isfinite(b.x)&&std::isfinite(b.y),"finite mesh");
  require(a.u==b.u&&a.v==b.v,"same antialias coordinates");
  mesh=std::max(mesh,double(std::hypot(a.x-b.x,a.y-b.y)));
 }
 require(mesh<2e-4,"mesh position gate");
 require(reference.pixels.size()==candidate.pixels.size(),"image dimensions");
 uint64_t error=0,alpha=0;int maximum=0;
 for(size_t i=0;i<reference.pixels.size();++i) {
  int delta=std::abs(int(reference.pixels[i])-int(candidate.pixels[i]));
  error+=delta;maximum=std::max(maximum,delta);
  if(i%4==3) alpha+=reference.pixels[i];
 }
 require(alpha>0,"nonempty reference image");
 double relative=double(error)/(4.*alpha);
 if(exact && error!=0) {
  std::fprintf(stderr,"baseline difference: mesh=%.9g,rgba_sum=%llu,relative=%.9g,max_byte=%d\n",mesh,(unsigned long long)error,relative,maximum);
 }
 if(exact) require(error==0,"private baseline pixels exactly match installed Rack");
 require(relative<=.001&&maximum<=16,"candidate image gate: <=0.1 percent, max byte <=16");
 errors.mesh=std::max(errors.mesh,mesh);errors.relative=std::max(errors.relative,relative);
 errors.byte=std::max(errors.byte,maximum);++errors.cases;errors.changed+=error!=0;
}
static std::vector<Points> shapes(int mode) {
 std::vector<Points> result;
 FluxGeometry g;
 for(int frame=0;frame<360;++frame) {
  g.rebuildPoints(.05f+1.9f*(.5f+.5f*std::sin(frame*.037f)),1.f,.85f*std::sin(frame*.023f),
   mode?IntegralFlux::FUNCTION_SHAPE_MATHS:IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);
  result.emplace_back(g.simplifiedFullPath.points.begin(),g.simplifiedFullPath.points.begin()+g.simplifiedFullPath.count);
 }
 return result;
}
static void timing(Renderer& renderer,Renderer& host,const std::vector<Points>& frames,float scale,
 int count,bool candidate,const char* label,int mode,int iterations=360) {
 int w=int(std::ceil(110*count*scale)),h=int(std::ceil(52*scale));
 auto* target=nvgluCreateFramebuffer(host.vg,w,h,0);require(target,"timing framebuffer");
 Queries queries;std::vector<double> cpu;
 for(int frame=0;frame<iterations;++frame) {
  auto* q=queries.begin(frame);auto start=Clock::now();
  draw(renderer,target,w,h,scale,frames[size_t(frame)%frames.size()],1.4f,count,candidate,false);
  double elapsed=us(start);queries.end(q);glFlush();
  if(frame>=100)cpu.push_back(elapsed);
 }
 queries.finish();require(glGetError()==GL_NO_ERROR,"timing GL errors");
 char name[180];std::snprintf(name,sizeof(name),"full-stroke/%s/mode%d/%.2fx/%d/CPU",label,mode,scale,count);report(name,cpu);
 std::snprintf(name,sizeof(name),"full-stroke/%s/mode%d/%.2fx/%d/GPU",label,mode,scale,count);report(name,queries.results);
 std::printf("gpu_missing=%d\n",queries.skipped);
 nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
}
static void sheet(const Result& a,const Result& b,int w,int h) {
 std::ofstream out("build/tools/round-stroke-comparison.ppm",std::ios::binary);
 out<<"P6\n"<<w*3<<" "<<h<<"\n255\n";
 for(int y=h-1;y>=0;--y)for(int column=0;column<3;++column)for(int x=0;x<w;++x) {
  size_t i=size_t(y*w+x)*4;
  for(int c=0;c<3;++c) {
   int v=column==2?std::min(255,16*std::abs(int(a.pixels[i+c])-int(b.pixels[i+c]))):
    int((column?b:a).pixels[i+c])+24*(255-int((column?b:a).pixels[i+3]))/255;
   out.put(char(std::min(255,v)));
  }
 }
 require(bool(out),"image sheet write");
}
int main(int argc,char** argv) {
 bool reverse=argc==2&&std::strcmp(argv[1],"--reverse")==0;
 bool tails=argc==2&&std::strcmp(argv[1],"--tail-check")==0;
 require(argc==1||reverse||tails,"usage: round_stroke_benchmark [--reverse|--tail-check]");
 require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* window=glfwCreateWindow(512,256,"Offline round stroke",nullptr,nullptr);require(window,"GL context");
 glfwMakeContextCurrent(window);glfwSwapInterval(0);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 std::printf("Rack=%s,GPU=%s,GL=%s\n",rack::APP_VERSION.c_str(),glGetString(GL_RENDERER),glGetString(GL_VERSION));
 {
 Renderer host({nvgCreateGL2,nvgDeleteGL2,nvgBeginFrame,nvgEndFrame,nvgBeginPath,nvgMoveTo,nvgLineTo,
  nvgStrokeWidth,nvgStrokeColor,nvgLineCap,nvgLineJoin,nvgStroke,nvgInternalParams});
 Renderer local(privateStrokeApi());Errors errors,baseline;
 if(tails) {
  for(int mode=0;mode<2;++mode) {
  auto frames=shapes(mode);
  for(int repeat=0;repeat<6;++repeat)for(float scale:{1.f,1.19f,2.f,4.f,8.f}) {
   std::printf("tail_repeat=%d\n",repeat);
   for(int order=0;order<2;++order) {
    bool candidate=(repeat%2)?order==0:order==1;
    timing(local,host,frames,scale,1,candidate,candidate?"candidate":"private-reference",mode,2100);
   }
  }
  }
 } else {
 for(int mode=0;mode<2;++mode) {
  auto frames=shapes(mode);
  for(float scale:{1.f,1.19f,2.f,4.f,8.f}) {
   int w=int(std::ceil(110*scale)),h=int(std::ceil(52*scale));
   auto* target=nvgluCreateFramebuffer(host.vg,w,h,0);require(target,"validation framebuffer");
   for(int frame=100;frame<360;++frame) {
    auto a=render(host,target,w,h,scale,frames[size_t(frame)],1.4f,false);
    auto b=render(local,target,w,h,scale,frames[size_t(frame)],1.4f,false);
    auto c=render(local,target,w,h,scale,frames[size_t(frame)],1.4f,true);
    compare(a,b,true,baseline);compare(b,c,false,errors);
    if(mode==1&&scale==8&&frame==240)sheet(b,c,w,h);
   }
   nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
   for(int count:{1,16})for(int order=0;order<3;++order) {
    int which=reverse?2-order:order;
    timing(which==0?host:local,host,frames,scale,count,which==2,
     which==0?"installed":which==1?"private-reference":"candidate",mode);
   }
  }
 }
 // Sharp, reversed, duplicate, near-collinear and self-overlapping open paths.
 std::vector<Points> corners={{{5,40},{50,3},{100,40}},{{5,25},{50,25},{100,25.00001f}},
  {{5,40},{50,4},{50,4},{100,40}},{{5,25},{100,25},{5.001f,25.001f}},
  {{5,5},{100,45},{5,45},{100,5}},{{5,40},{49.999f,4},{50,40},{100,4}}};
 for(float scale:{1.f,1.19f,2.f,4.f,8.f})for(float width:{.25f,1.15f,1.4f,4.f,12.f})for(auto points:corners)for(int direction=0;direction<2;++direction) {
  if(direction)std::reverse(points.begin(),points.end());
  int w=int(std::ceil(110*scale)),h=int(std::ceil(52*scale));
  auto* target=nvgluCreateFramebuffer(host.vg,w,h,0);require(target,"corner framebuffer");
  auto a=render(host,target,w,h,scale,points,width,false),b=render(local,target,w,h,scale,points,width,false),c=render(local,target,w,h,scale,points,width,true);
  compare(a,b,true,baseline);compare(b,c,false,errors);
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
 std::printf("validation_pairs=%zu,baseline_exact=%zu,changed_images=%zu,max_mesh_error=%.9g,max_relative_error=%.9g,max_byte_error=%d\n",
  errors.cases,baseline.cases,errors.changed,errors.mesh,errors.relative,errors.byte);
 }
 }
 glfwDestroyWindow(window);glfwTerminate();
}
