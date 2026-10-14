#include "../tools/experiments/lumin/retained_polyline_adapter.hpp"
#define main archivedFixtureMain
#include "../tools/experiments/lumin/flux_shader_surface_spec.cpp"
#undef main
static void contracts(){
 using namespace lumin;
 std::unique_ptr<PolylineStroke> strokeOwner(new PolylineStroke);
 auto& stroke=*strokeOwner;
 Point points[]={{2,30},{20,2},{20,2},{80,30}};
 require(stroke.update(points,4,1)==StrokeResult::Ok,"copy and normalize repeated points");
 points[0].x=999; // The retained source owns its copy.
 require(glfwInit(),"contract GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* native=glfwCreateWindow(128,64,"Polyline contracts",nullptr,nullptr);require(native,"contract window");
 glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"contract GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"contract NVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;
 ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);
 {
  PolylineDevice device;
  visual_assets::AdaptiveGlSurface surface;
  struct Job{PolylineStroke* stroke;PolylineDevice* device;NVGcontext* vg;} job{&stroke,&device,vg};
  visual_assets::AdaptiveGlSurface::Update u;u.surface=&surface;u.logicalSize={106,48};u.policy.minDensity=u.policy.maxDensity=1;u.policy.vertexAttributeCount=2;u.user=&job;
  u.callback=[](void* user,Vec size,int y){auto& j=*static_cast<Job*>(user);
   auto draw=[&](float width,Cap cap){return j.stroke->draw(*j.device,j.vg,size,y,1,{.37f,.81f},width,nvgRGBA(28,204,217,120),cap);};
   require(draw(1.4f,Cap::Butt)==StrokeResult::Ok,"first retained draw");
   std::vector<unsigned char> pixels(size_t(size.x)*size.y*4);
   glReadPixels(0,y,int(size.x),int(size.y),GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
   Point reference[]={{2,30},{20,2},{80,30}};
   for(int row=0;row<int(size.y);++row)for(int col=0;col<int(size.x);++col){
    double px=col+.5-.37,py=size.y-row-.5-.81,field=1e20;
    for(int k=0;k<2;++k){auto a=reference[k],b=reference[k+1];double dx=b.x-a.x,dy=b.y-a.y,len2=dx*dx+dy*dy;
     double t=((px-a.x)*dx+(py-a.y)*dy)/len2,c=std::max(0.,std::min(1.,t));
     double ex=px-a.x-c*dx,ey=py-a.y-c*dy,d=std::sqrt(ex*ex+ey*ey)-.7;
     if(k==0)d=std::max(d,-t*std::sqrt(len2));
     if(k==1)d=std::max(d,(t-1)*std::sqrt(len2));
     field=std::min(field,d);
    }
    int expected=int(std::round(120*std::max(0.,std::min(1.,.5-field))));
    require(std::abs(int(pixels[(size_t(row)*size_t(size.x)+col)*4+3])-expected)<=1,"coverage matches CPU union without tile seams");
   }
   auto bytes=j.stroke->uploadedBytes(),builds=j.stroke->coverageBuilds();
   require(bytes>0&&builds==1,"first geometry upload");
   require(draw(1.4f,Cap::Round)==StrokeResult::Ok,"round cap material");
   require(j.stroke->draw(*j.device,j.vg,size,y,1,{.12f,.23f},1.4f,nvgRGBA(255,0,0,80))==StrokeResult::Ok,"color and translation material update");
   require(j.stroke->uploadedBytes()==bytes&&j.stroke->coverageBuilds()==builds,"warm draw no rebuild or upload");
   require(draw(2.f,Cap::Butt)==StrokeResult::Ok&&j.stroke->coverageBuilds()==builds+1,"width coverage invalidation");
   Point reverse[]={{5,0},{1,3}};require(j.stroke->update(reverse,2,2)==StrokeResult::Unsupported,"reversal rejected");
   require(draw(1.4f,Cap::Butt)==StrokeResult::Invalid,"rejected geometry cannot draw stale contour");
   Point empty{};require(j.stroke->update(&empty,0,3)==StrokeResult::Ok&&draw(1.4f,Cap::Round)==StrokeResult::Ok,"empty path no marker");
  };
  require(visual_assets::AdaptiveGlSurface::renderBatch(vg,&u,1),"contract shared pass");
 }
 strokeOwner.reset();
 for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}
 while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}
 require(glGetError()==GL_NO_ERROR,"contract GL errors");nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();
 std::puts("PASS: retained source, warm uploads, cap/width invalidation, rejected-source fallback contracts");
}

static void sharedBenchmark(NVGcontext* vg,rack::window::Window& window,rack::app::Scene& scene){
 using Surface=visual_assets::AdaptiveGlSurface;
 struct Job{ShaderContourEngine engine;ContourSegment path;};
 struct Batch{Job* jobs;NVGcontext* vg;int count;};
 std::vector<std::vector<ContourPoint>> frames;FluxGeometry g;
 for(int i=0;i<360;++i){g.rebuildPoints(.05f+1.9f*(.5f+.5f*std::sin(i*.037f)),1,.85f*std::sin(i*.023f),IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);
  frames.emplace_back();for(int j=0;j<g.simplifiedFullPath.count;++j){auto p=g.simplifiedFullPath.points[j];frames.back().push_back({p.x,p.y});}}
 window.pixelRatio=1;
 for(int count:{2,8,32})for(int repeat=0;repeat<3;++repeat)for(int order=0;order<2;++order){
  int mode=(order+repeat)%2;Surface surface;Job jobs[32];Batch batch{jobs,vg,count};
  auto* target=nvgluCreateFramebuffer(vg,110*count,54,0);require(target,"shared canvas target");
  Surface::Update update;update.surface=&surface;update.logicalSize={float(110*count),49};update.policy.minDensity=update.policy.maxDensity=1;update.policy.sizeQuantum=1;update.policy.vertexAttributeCount=2;update.user=&batch;
  update.callback=[](void* user,Vec size,int y){auto& b=*static_cast<Batch*>(user);for(int i=0;i<b.count;++i)require(b.jobs[i].engine.render(b.vg,size,y,1,{float(110*i),0},&b.jobs[i].path,1),"shared canvas stroke");};
  Queries queries;std::vector<double> cpu;
  for(int frame=0;frame<460;++frame){++testFrame;for(auto* child:scene.children)child->step();
   const auto& pts=frames[size_t(frame)%frames.size()];for(int i=0;i<count;++i)jobs[i].path=ContourSegment(pts.data(),int(pts.size()),nvgRGBA(230,230,220,255));surface.markDirty();
   nvgluBindFramebuffer(target);glViewport(0,0,110*count,54);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glStencilMask(0xff);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   auto* query=queries.begin(frame);auto start=Clock::now();
   if(mode)require(Surface::renderBatch(vg,&update,1),"shared canvas pass");
   nvgBeginFrame(vg,110*count,54,1);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(float(110*count),48));
   if(mode)require(surface.drawAligned(args,{float(110*count),48},1,{0,0}),"shared canvas present");
   else for(int i=0;i<count;++i){nvgSave(vg);nvgTranslate(vg,float(110*i),0);legacy(vg,&jobs[i].path,1);nvgRestore(vg);}
   nvgEndFrame(vg);double elapsed=us(start);queries.end(query);glFlush();if(frame>=100)cpu.push_back(elapsed);
  }
  queries.finish();require(glGetError()==GL_NO_ERROR,"shared canvas GL");char label[120];const char* name=mode?"shared-canvas-shader":"host-nanovg";
  std::snprintf(label,sizeof(label),"%s/count%d/repeat%d/CPU",name,count,repeat);report(label,cpu);
  std::snprintf(label,sizeof(label),"%s/count%d/repeat%d/GPU",name,count,repeat);report(label,queries.results);
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
}
static int runShared(){
 require(glfwInit(),"shared GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,256,"Shared canvas",nullptr,nullptr);require(native,"shared window");
 glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"shared GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"shared NanoVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;
 ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);
 sharedBenchmark(vg,window,scene);
 for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}
 while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}
 nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();return 0;
}
int main(int argc,char** argv){contracts();if(argc>1&&std::strcmp(argv[1],"--contracts")==0)return 0;if(argc>1&&std::strcmp(argv[1],"--shared-benchmark")==0)return runShared();return archivedFixtureMain(argc,argv);}
