#define PROC_HEADLESS_TEST 1
#include "../src/Proc.cpp"
#include "PhosphorPreviewBaseline.hpp"
#include "preview_benchmark_utils.hpp"

using Trail = visual_assets::PhosphorPreview<128>;
using Baseline = baseline_phosphor::PhosphorPreview<128>;
using Geometry = ProcPreviewGeometry<Proc>;

template<typename T> static void releaseHistory(T& trail) {
 glDeleteTextures(2,trail.textures); glDeleteFramebuffers(2,trail.framebuffers);
 trail.forgetResources();
}
static std::vector<unsigned char> readTarget(NVGLUframebuffer* target,int w,int h) {
 nvgluBindFramebuffer(target);
 std::vector<unsigned char> pixels(size_t(w*h*4));
 glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
 return pixels;
}
static uint64_t alphaSum(const std::vector<unsigned char>& pixels) {
 uint64_t total=0; for(size_t i=3;i<pixels.size();i+=4) total+=pixels[i]; return total;
}
static void contracts(NVGcontext* vg,const std::array<Vec,128>& curve) {
 auto* target=nvgluCreateFramebuffer(vg,212,96,0); require(target,"contract target");
 Trail trail; trail.box.size=Vec(106,48); trail.setEnabled(true);
 auto draw=[&](double now,Vec size=Vec(212,96)) {
  nvgluBindFramebuffer(target); trail.renderHistory(vg,size,now);
  GLint binding=0; glGetIntegerv(GL_FRAMEBUFFER_BINDING,&binding);
  require(GLuint(binding)==target->fbo,"presentation target restored");
  require(glGetError()==GL_NO_ERROR,"no GL errors");
  return readTarget(target,int(size.x),int(size.y));
 };
 require(trail.capture(curve,1.0),"initial capture accepted");
 auto initial=draw(1.0); require(alphaSum(initial)>0,"visible initial deposit");
 const int front=trail.front;
 glBindTexture(GL_TEXTURE_2D,trail.textures[front]);
 std::vector<unsigned char> stored(212*96*4),after(stored.size());
 glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,stored.data());
 require(!trail.capture(curve,1.01),"capture throttling retained");
 for(int i=1;i<=6;++i) draw(1.0+i/60.0);
 auto faded=draw(1.1);
 require(trail.front==front,"fade-only never swaps history");
 glBindTexture(GL_TEXTURE_2D,trail.textures[front]);
 glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,after.data());
 require(stored==after,"fade-only history bytes unchanged");
 require(alphaSum(faded)>0 && alphaSum(faded)<alphaSum(initial),"presentation fades");
 trail.persistence=1.2f;
 require(draw(1.1)==faded,"persistence change has no instantaneous brightness jump");
 require(alphaSum(draw(1.2))<alphaSum(faded),"new persistence continues decay");
 require(trail.capture(curve,1.25),"second deposit accepted");
 require(alphaSum(draw(1.25))>alphaSum(faded),"accumulation folds in decayed history");
 require(trail.front!=front,"deposit swaps history");
 require(alphaSum(draw(4.0))==0 && !trail.history,"expired trail disappears");
 trail.setEnabled(false); trail.setEnabled(true);
 require(alphaSum(draw(4.1))==0,"toggle discards old history");
 trail.capture(curve,4.2);
 require(alphaSum(draw(4.2,Vec(106,48)))>0 && trail.width==106,"resize recreates history");
 trail.additive=false; trail.capture(curve,4.3);
 require(alphaSum(draw(4.3,Vec(106,48)))>0,"source-over deposition works");
 trail.capture(curve,4.4); draw(4.6,Vec(106,48));
 require(!trail.pending && trail.lastDeposit==4.3,"stale pending capture discarded");
 std::vector<unsigned char> oneStep;
 for(int steps : {1,9,18,43}) {
  trail.setEnabled(false); trail.setEnabled(true); trail.persistence=.6f;
  trail.capture(curve,5.0); draw(5.0);
  std::vector<unsigned char> image;
  for(int i=1;i<=steps;++i) image=draw(5.0+.3*double(i)/steps);
  if(oneStep.empty()) oneStep=image;
  else for(size_t i=0;i<image.size();++i)
   require(std::abs(int(image[i])-int(oneStep[i]))<=1,"fade independent of presentation cadence within one byte");
 }
 releaseHistory(trail);
 require(alphaSum(draw(5.7))==0,"resource reset discards history");
 releaseHistory(trail); nvgluBindFramebuffer(nullptr); nvgluDeleteFramebuffer(target);
 std::puts("PASS: lazy history, presentation fade, persistence continuity, capture, expiry, resize, reset, blend and GL-state contracts");
}

template<typename T> static void scenario(NVGcontext* vg,const std::array<Vec,128>& curve,
 const char* name,int scale,int count,bool depositing) {
 auto* target=nvgluCreateFramebuffer(vg,106*scale,48*scale,0); require(target,"benchmark target");
 std::vector<T*> trails;
 for(int i=0;i<count;++i) { auto* t=new T; t->box.size=Vec(106,48); t->persistence=1.2f; t->setEnabled(true); trails.push_back(t); }
 Queries queries; std::vector<double> cpu; int writes=0;
 for(int frame=0;frame<600;++frame) {
  double now=1.0+frame/60.0;
  // Seed each fade-only interval outside timing, so visible history lasts through
  // all measured frames without counting deposits in the fade-only workload.
  if(!depositing && frame%60==0) for(auto* t:trails) {
   t->capture(curve,now); nvgluBindFramebuffer(target); t->renderHistory(vg,Vec(106*scale,48*scale),now);
  }
  if(depositing && frame%3==0) for(auto* t:trails) t->capture(curve,now);
  auto* query=queries.begin(frame); auto start=Clock::now();
  for(auto* t:trails) {
   int previous=t->front;
   nvgluBindFramebuffer(target); t->renderHistory(vg,Vec(106*scale,48*scale),now);
   if(frame>=100) writes+=(previous!=t->front);
  }
  double elapsed=us(start); queries.end(query); glFlush();
  if(frame>=100) cpu.push_back(elapsed);
 }
 queries.finish(); require(glGetError()==GL_NO_ERROR,"benchmark GL errors");
 char label[160]; std::snprintf(label,sizeof(label),"%s/%s/%dx/%d instances/CPU",name,depositing?"20Hz-deposits":"fade-only",scale,count); report(label,cpu);
 std::snprintf(label,sizeof(label),"%s/%s/%dx/%d instances/GPU",name,depositing?"20Hz-deposits":"fade-only",scale,count); report(label,queries.results);
 std::printf("history_swaps=%d,gpu_missing=%d\n",writes,queries.skipped);
 if(std::string(name)=="lazy" && !depositing) require(writes==0,"zero fade-only history updates");
 for(auto* t:trails) { releaseHistory(*t); delete t; }
 nvgluBindFramebuffer(nullptr); nvgluDeleteFramebuffer(target);
}
int main() {
 require(glfwInit(),"GLFW"); glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* window=glfwCreateWindow(512,256,"Phosphor benchmark",nullptr,nullptr);
 require(window,"hidden context"); glfwMakeContextCurrent(window); glfwSwapInterval(0);
 require(glewInit()==GLEW_OK,"GLEW"); while(glGetError()!=GL_NO_ERROR) {}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS); require(vg,"NanoVG");
 std::printf("GPU=%s; GL=%s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));
 Geometry g; g.rebuildPoints(Vec(106,48),1,2,.6f,false);
 contracts(vg,g.points);
 for(int scale:{1,4}) for(int count:{1,16}) for(bool depositing:{false,true}) {
  scenario<Baseline>(vg,g.points,"baseline",scale,count,depositing);
  scenario<Trail>(vg,g.points,"lazy",scale,count,depositing);
 }
 nvgDeleteGL2(vg); glfwDestroyWindow(window); glfwTerminate();
}
