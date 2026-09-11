#define LUMIN_FUNCTION_NO_MAIN
#include "lumin_function_curve_spec.cpp"
#undef LUMIN_FUNCTION_NO_MAIN
#include "../tools/experiments/lumin/function_history.hpp"
#include "visual/SnapshotHistory.hpp"
static bool useTwoStep=false;
static void historyContracts(){
 lumin::FunctionHistory h;WavePreviewTracer<128,6> old;std::array<Vec,128> points;for(int i=0;i<128;++i)points[i]={float(i),float(i%5)};
 for(int i=0;i<120;++i){double now=1.+i/60.;lumin::FunctionParameters p;p.ratio=.5f;p.shape=.85f*std::sin(i*.1f);p.shark=i>60;
  h.expire(now);old.expire(now,.333f);bool captured=h.capture(p,now);require(captured==old.capture(points,now,1.f/24.f,1).captured,"capture cadence agreement");
  for(int j=0;j<6;++j){auto& a=h.frames[j];auto& b=old.frames[j];require(a.active==b.active&&a.birth==b.birthSec,"ring order and expiry agreement");float age=float(now-b.birthSec);int alpha=(!b.active||age<0||age>=.333f)?0:clamp(int(118.f*(1-age/.333f)),0,255);require(lumin::FunctionHistory::alpha(a,now)==alpha,"fade agreement");}
 }
 h.expire(10);for(auto& f:h.frames)require(!f.active,"expire all");h.clear();require(h.next==0&&h.last==-1,"clear resets ring");
 std::printf("HISTORY CONTRACTS passed; parameter history=%zu bytes, point history=%zu bytes\n",sizeof(h),sizeof(old));
}
static void historyRun(NVGcontext* vg,bool images){
 struct Job{
  lumin::FunctionCurve renderer;lumin::FunctionHistory histories[2];lumin::FunctionParameters current[2];NVGcontext* vg;double now=1;float density=1;bool includeTrails=true;
  static void paint(void* user,Vec active,int y){auto& j=*static_cast<Job*>(user);for(int i=0;i<2;++i){
   lumin::FunctionCurve::Layer layers[7];int count=0;
   if(j.includeTrails)for(auto& f:j.histories[i].frames){int alpha=lumin::FunctionHistory::alpha(f,j.now);if(!alpha)continue;auto& layer=layers[count++];layer.ratio=f.parameters.ratio;layer.shape=f.parameters.shape;layer.shark=f.parameters.shark;layer.width=1.15f;layer.color=layer.other=nvgRGBA(255,190,80,alpha);}
   auto& layer=layers[count++];auto p=j.current[i];layer.ratio=p.ratio;layer.shape=p.shape;layer.shark=p.shark;layer.color=layer.other=nvgRGBA(230,230,220,255);
   require(j.renderer.drawBatch(j.vg,active,y,{106,48},{110.f*i,0},j.density,layers,count),"batched parameter trails and current curve");
  }}
 };
 for(float density:images?std::vector<float>{1,2}:std::vector<float>{1,2})for(int repeat=0;repeat<(images?1:3);++repeat)for(int order=0;order<3;++order){int mode=(repeat+order)%3;Job job;job.renderer.setKernel(useTwoStep?lumin::FunctionCurve::Kernel::TwoStep:lumin::FunctionCurve::Kernel::Reference);job.vg=vg;job.density=density;job.includeTrails=mode!=2;APP->window->pixelRatio=density;
  Surface surface;Surface::Update u;u.surface=&surface;u.logicalSize={220,49};u.policy.minDensity=u.policy.maxDensity=density;u.policy.sizeQuantum=1;u.policy.vertexAttributeCount=1;u.callback=Job::paint;u.user=&job;
  struct CachedJob {lumin::FunctionCurve* renderer=nullptr;NVGcontext* vg=nullptr;float density=1;lumin::FunctionParameters p;};
  std::array<Surface,12> cached;std::array<CachedJob,12> cachedJobs;std::array<double,12> births;births.fill(-1);
  std::array<Surface::Update,13> updates;
  FluxGeometry geometry[2];WavePreviewTracer<128,6> oldHistory[2];visual_assets::SnapshotHistory<128,6>* snapshots[2];
  for(int i=0;i<2;++i){snapshots[i]=new visual_assets::SnapshotHistory<128,6>;snapshots[i]->box.size={106,48};APP->scene->addChild(snapshots[i]);}
  WavePreviewTracerStyle style;WavePreviewTracerDrawStats stats;
  auto* target=nvgluCreateFramebuffer(vg,int(220*density),int(54*density),0);require(target,"history output");Queries queries;std::vector<double> total,step,draw;
  for(int frame=0;frame<(images?180:460);++frame){++testFrame;job.now=1.+frame/60.;
   nvgluBindFramebuffer(target);glViewport(0,0,int(220*density),int(54*density));glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glStencilMask(255);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   auto start=Clock::now();
   for(auto* child:APP->scene->children)child->step();
   for(int i=0;i<2;++i){lumin::FunctionParameters p;p.ratio=.5f+.45f*std::sin(frame*.027f+i*.5f);p.shape=.95f*std::sin(frame*.053f+i);p.shark=(frame/90)%2!=0;
    if(mode){job.histories[i].expire(job.now);if(frame)job.histories[i].capture(job.current[i],job.now);job.current[i]=p;}
    else{oldHistory[i].expire(job.now,.333f);if(frame)oldHistory[i].capture(geometry[i].points,job.now,1.f/24.f,1);geometry[i].rebuildPoints(p.ratio,1-p.ratio,p.shape,p.shark?IntegralFlux::FUNCTION_SHAPE_SHARK_FIN:IntegralFlux::FUNCTION_SHAPE_MATHS,false);}
   }
   double stepping=us(start);auto drawStart=Clock::now();auto* query=queries.begin(frame);
   if(mode){surface.markDirty();updates[0]=u;int updateCount=1;
    if(mode==2)for(int i=0;i<2;++i)for(int k=0;k<6;++k){auto& frame=job.histories[i].frames[k];if(!lumin::FunctionHistory::alpha(frame,job.now))continue;int slot=i*6+k;if(births[slot]==frame.birth)continue;births[slot]=frame.birth;auto& c=cachedJobs[slot];c.renderer=&job.renderer;c.vg=vg;c.density=density;c.p=frame.parameters;
     auto& update=updates[updateCount++];update=Surface::Update{};update.surface=&cached[slot];update.logicalSize={107,49};update.policy.minDensity=update.policy.maxDensity=density;update.policy.sizeQuantum=1;update.policy.vertexAttributeCount=1;update.user=&c;update.callback=[](void* user,Vec size,int y){auto& c=*static_cast<CachedJob*>(user);auto color=nvgRGBA(255,190,80,255);require(c.renderer->draw(c.vg,size,y,{106,48},{0,0},c.density,c.p.ratio,c.p.shape,c.p.shark,color,color,false,1.15f),"cached analytic snapshot");};cached[slot].markDirty();
    }
    require(Surface::renderBatch(vg,updates.data(),updateCount),"history shared pass");}
   nvgBeginFrame(vg,220,54,density);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect({0,0},{220,48});
   if(mode){
    if(mode==2)for(int i=0;i<2;++i)for(int k=0;k<6;++k){int alpha=lumin::FunctionHistory::alpha(job.histories[i].frames[k],job.now);if(!alpha)continue;nvgSave(vg);nvgTranslate(vg,110.f*i,0);nvgAlpha(vg,float(alpha)/255.f);Widget::DrawArgs local=args;local.clipBox=Rect({0,0},{106,48});require(cached[i*6+k].drawAligned(local,{106,48},density,{0,0}),"cached analytic trail presentation");nvgRestore(vg);}
    require(surface.drawAligned(args,{220,48},density,{0,0}),"history shader image");}else for(int i=0;i<2;++i){nvgSave(vg);nvgTranslate(vg,110.f*i,0);args.clipBox=Rect({0,0},{106,48});snapshots[i]->drawHistory(args,oldHistory[i],job.now,style,&stats);
    auto& path=geometry[i].simplifiedFullPath;nvgBeginPath(vg);nvgMoveTo(vg,path.points[0].x,path.points[0].y);for(int k=1;k<path.count;++k)nvgLineTo(vg,path.points[k].x,path.points[k].y);nvgStrokeWidth(vg,1.4f);nvgStrokeColor(vg,nvgRGBA(230,230,220,255));nvgLineJoin(vg,NVG_ROUND);nvgLineCap(vg,NVG_BUTT);nvgStroke(vg);nvgRestore(vg);}
   nvgEndFrame(vg);double rendering=us(drawStart),elapsed=us(start);queries.end(query);glFlush();if(frame>=100){step.push_back(stepping);draw.push_back(rendering);total.push_back(elapsed);}
   if(images&&(frame==89||frame==95||frame==150||frame==179)){
    std::vector<unsigned char> pixels(size_t(220*density)*size_t(54*density)*4);glReadPixels(0,0,int(220*density),int(54*density),GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());char name[160];std::snprintf(name,160,"work/history-%s-%dx-%d.ppm",mode==2?"cached-shader":mode?"shader":"reference",int(density),frame);auto* f=std::fopen(name,"wb");require(f,"history image");std::fprintf(f,"P6\n%d %d\n255\n",int(220*density),int(54*density));for(int y=int(54*density)-1;y>=0;--y)for(int x=0;x<int(220*density);++x)std::fwrite(&pixels[(size_t(y)*size_t(220*density)+x)*4],1,3,f);std::fclose(f);
   }
  }
  queries.finish();char label[160];const char* name=mode==2?"cached-function-history":mode?"function-history":"snapshot-history";
  auto reportMetric=[&](const char* metric,const std::vector<double>& values){std::snprintf(label,160,"%s/%.0fx/repeat%d/%s",name,density,repeat,metric);report(label,values);};reportMetric("CPU-total",total);reportMetric("CPU-preview-step",step);reportMetric("CPU-preview-draw",draw);reportMetric("GPU",queries.results);
  if(!mode){require(stats.rasterizations>0,"production history rasterized");require(stats.rasterizations<stats.trails,"cached history reused");std::printf("CACHE rasterizations=%zu trails=%zu\n",stats.rasterizations,stats.trails);}
  require(glGetError()==GL_NO_ERROR,"history GL state");nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);for(auto* snapshot:snapshots){APP->scene->removeChild(snapshot);delete snapshot;}
 }
}
int main(int argc,char** argv){useTwoStep=argc>1&&std::strcmp(argv[1],"--two-step")==0;historyContracts();require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,256,"History shader",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NVG");auto* fbVg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(fbVg,"framebuffer NVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.fbVg=fbVg;window.pixelRatio=1;rack::contextSet(&ctx);
 historyRun(vg,argc>1&&std::strcmp(argv[1],"--images")==0);
 for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}nvgDeleteGL2(fbVg);nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();return 0;}
