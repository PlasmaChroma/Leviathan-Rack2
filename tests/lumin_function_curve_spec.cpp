#define main oldProbeMain
#include "../tools/experiments/lumin/flux_shader_surface_spec.cpp"
#undef main
#include "../tools/experiments/lumin/function_curve_candidate.hpp"
#include "proc_shape.hpp"
static bool denseReference=false,includeGeometry=false;
static int imageFailures=0;
using Surface=visual_assets::AdaptiveGlSurface;
struct FunctionJob{
 lumin::FunctionCurve renderer;NVGcontext* vg=nullptr;
 float ratio=.5f,curve=0,density=1;bool shark=false,highlight=false;int count=1;Vec offset;
 NVGcolor color=nvgRGBA(230,230,220,255),other=nvgRGBA(230,230,220,180);
 static void paint(void* user,Vec active,int y){auto& j=*static_cast<FunctionJob*>(user);
  for(int i=0;i<j.count;++i)require(j.renderer.draw(j.vg,active,y,{106,48},{110.f*i+j.offset.x,j.offset.y},j.density,j.ratio,j.curve,j.shark,j.color,j.other,j.highlight),"analytic shader draw");}
};
static void compareModel(){
 FluxGeometry flux;ProcPreviewGeometry<ProcShapeModel> proc;double maxPhase=0;
 for(float c:{-1.f,-.85f,-.1f,0.f,.1f,.85f,1.f})for(float ratio:{.05f,.5f,.95f}){
  flux.rebuildPoints(ratio,1-ratio,c,IntegralFlux::FUNCTION_SHAPE_MATHS,false);proc.rebuildPoints({106,48},ratio,1-ratio,c,false);
  for(int i=0;i<128;++i)require(flux.points[i].x==proc.points[i].x&&flux.points[i].y==proc.points[i].y,"Proc and Flux Maths production geometry identical");
  for(bool rise:{false,true})for(int i=1;i<511;++i){float v=rise?flux.cachedRiseLut[i]:flux.cachedFallLut[i];float phase=lumin::FunctionCurve::integral(v,40*c)/lumin::FunctionCurve::integral(1,40*c);if(!rise)phase=1-phase;
   maxPhase=std::max(maxPhase,double(std::fabs(phase-float(i)/511)));}
 }
 std::printf("MODEL: Proc/Flux Maths exact geometry agreement; max analytic phase vs production LUT %.8f (%.5f px across 102.6 px)\n",maxPhase,maxPhase*102.6);
}
static void run(NVGcontext* vg,bool timing){
 FunctionJob job;job.vg=vg;Surface surface;Surface::Update u;u.surface=&surface;u.policy.minDensity=u.policy.maxDensity=1;u.policy.sizeQuantum=1;u.policy.vertexAttributeCount=1;u.callback=FunctionJob::paint;u.user=&job;
 if(timing){
  std::array<FluxGeometry,32> liveGeometry;
  struct Frame{float ratio,curve;std::vector<ContourPoint> points;};std::vector<Frame> frames;FluxGeometry g;
  for(int i=0;i<360;++i){Frame f;float rise=.05f+1.9f*(.5f+.5f*std::sin(i*.037f));f.ratio=rise/(1+rise);f.curve=.85f*std::sin(i*.023f);g.rebuildPoints(rise,1,f.curve,IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);for(int k=0;k<g.simplifiedFullPath.count;++k)f.points.push_back({g.simplifiedFullPath.points[k].x,g.simplifiedFullPath.points[k].y});frames.push_back(f);}
  for(int count:{2,8,32})for(int repeat=0;repeat<3;++repeat)for(int order=0;order<2;++order){int mode=(repeat+order)%2;job.count=count;job.shark=true;u.logicalSize={float(count*110),49};auto* target=nvgluCreateFramebuffer(vg,count*110,54,0);require(target,"timing target");Queries queries;std::vector<double> cpu;
   for(int frame=0;frame<460;++frame){++testFrame;for(auto* child:APP->scene->children)child->step();auto& f=frames[size_t(frame)%frames.size()];job.ratio=f.ratio;job.curve=f.curve;surface.markDirty();
    nvgluBindFramebuffer(target);glViewport(0,0,count*110,54);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glStencilMask(255);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    auto* query=queries.begin(frame);auto start=Clock::now();
    if(!mode&&includeGeometry)for(int i=0;i<count;++i)liveGeometry[i].rebuildPoints(f.ratio,1-f.ratio,f.curve,IntegralFlux::FUNCTION_SHAPE_SHARK_FIN,false);
    if(mode)require(Surface::renderBatch(vg,&u,1),"function common pass");
    nvgBeginFrame(vg,count*110,54,1);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect({0,0},{float(count*110),48});
    if(mode)require(surface.drawAligned(args,{float(count*110),48},1,{0,0}),"function present");else{ContourSegment p(f.points.data(),int(f.points.size()),job.color);for(int i=0;i<count;++i){nvgSave(vg);nvgTranslate(vg,110.f*i,0);if(includeGeometry){auto& geometry=liveGeometry[i];nvgBeginPath(vg);auto& path=geometry.simplifiedFullPath;nvgMoveTo(vg,path.points[0].x,path.points[0].y);for(int k=1;k<path.count;++k)nvgLineTo(vg,path.points[k].x,path.points[k].y);nvgStrokeColor(vg,job.color);nvgStrokeWidth(vg,1.4f);nvgLineCap(vg,NVG_BUTT);nvgLineJoin(vg,NVG_ROUND);nvgStroke(vg);}else legacy(vg,&p,1);nvgRestore(vg);}}
    nvgEndFrame(vg);double elapsed=us(start);queries.end(query);glFlush();if(frame>=100)cpu.push_back(elapsed);
   }
   queries.finish();char label[120];std::snprintf(label,120,"%s/count%d/repeat%d/CPU",mode?"function-shader":"host-nanovg",count,repeat);report(label,cpu);std::snprintf(label,120,"%s/count%d/repeat%d/GPU",mode?"function-shader":"host-nanovg",count,repeat);report(label,queries.results);nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
  }
 }else{
  int cases=0,failed=0,maxByte=0;double maximum=0;FluxGeometry g;
  for(bool shark:{false,true})for(float ratio:{.05f,.5f,.95f})for(float curve:{-.85f,0.f,.85f})for(float density:{1.f,1.19f,2.f,4.f})for(float offset:{0.f,.37f})for(bool highlight:{false,true}){
   g.rebuildPoints(ratio,1-ratio,curve,shark?IntegralFlux::FUNCTION_SHAPE_SHARK_FIN:IntegralFlux::FUNCTION_SHAPE_MATHS,false);job.shark=shark;job.ratio=ratio;job.curve=curve;job.density=density;job.offset={offset,offset};job.highlight=highlight;job.color=highlight?nvgRGBA(28,204,217,255):nvgRGBA(230,230,220,255);
   std::vector<ContourPoint> full,rise,fall;auto copy=[](auto& out,auto& path){for(int i=0;i<path.count;++i)out.push_back({path.points[i].x,path.points[i].y});};copy(full,g.simplifiedFullPath);copy(rise,g.simplifiedRisePath);copy(fall,g.simplifiedFallPath);
   if(denseReference){
    full.clear();rise.clear();fall.clear();
    float kr=40*(shark?-curve:curve),kf=40*curve;
    for(int i=0;i<=2048;++i){float v=float(i)/2048;float phase=lumin::FunctionCurve::integral(v,kr)/lumin::FunctionCurve::integral(1,kr);rise.push_back({1.7f+102.6f*ratio*phase,46.3f-44.6f*v});}
    for(int i=0;i<=2048;++i){float v=1-float(i)/2048;float phase=lumin::FunctionCurve::integral(v,kf)/lumin::FunctionCurve::integral(1,kf);fall.push_back({1.7f+102.6f*(ratio+(1-ratio)*(1-phase)),46.3f-44.6f*v});}
    full=rise;full.insert(full.end(),fall.begin()+1,fall.end());
   }
   ContourSegment paths[2]={{full.data(),int(full.size()),job.color},{}};if(highlight){paths[0]={rise.data(),int(rise.size()),job.color};paths[1]={fall.data(),int(fall.size()),job.other};}
   int w=int(std::ceil(112*density)),h=int(std::ceil(54*density));auto* target=nvgluCreateFramebuffer(vg,w,h,0);require(target,"image target");std::vector<unsigned char> ref;
   for(int mode=0;mode<2;++mode){++testFrame;for(auto* child:APP->scene->children)child->step();nvgluBindFramebuffer(target);glViewport(0,0,w,h);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glStencilMask(255);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    u.logicalSize={108,50};u.policy.minDensity=u.policy.maxDensity=density;surface.markDirty();if(mode)require(Surface::renderBatch(vg,&u,1),"image private pass");
    nvgBeginFrame(vg,float(w)/density,float(h)/density,density);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect({0,0},{108,50});
    if(mode)require(surface.drawAligned(args,{108,50},density,{0,0}),"image present");else{nvgTranslate(vg,offset,offset);legacy(vg,paths,highlight?2:1);}nvgEndFrame(vg);
    std::vector<unsigned char> pixels(size_t(w)*h*4);glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
    if(!shark&&ratio==.5f&&curve==.85f&&density==1&&offset==.37f&&!highlight){auto* f=std::fopen(mode?"work/function-shader.ppm":"work/function-reference.ppm","wb");require(f,"image file");std::fprintf(f,"P6\n%d %d\n255\n",w,h);for(int y=h-1;y>=0;--y)for(int x=0;x<w;++x)std::fwrite(&pixels[(size_t(y)*w+x)*4],1,3,f);std::fclose(f);}
    if(!mode)ref=pixels;else{uint64_t error=0,mass=0;int byte=0;for(size_t i=0;i<pixels.size();++i){int d=std::abs(int(pixels[i])-int(ref[i]));error+=d;byte=std::max(byte,d);if(i%4==3)mass+=ref[i];}double relative=double(error)/(4*mass);maximum=std::max(maximum,relative);maxByte=std::max(maxByte,byte);if(relative>=.05||byte>64)++failed;++cases;std::printf("image shark=%d ratio=%.2f curve=%.2f density=%.2f offset=%.2f highlight=%d relative=%.8f byte=%d\n",shark,ratio,curve,density,offset,highlight,relative,byte);}
   }nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
  }imageFailures=failed;std::printf("FUNCTION IMAGES: %d cases, %d fail historical thresholds, maximum relative %.8f, max byte %d\n",cases,failed,maximum,maxByte);
 }
 require(glGetError()==GL_NO_ERROR,"function probe GL errors");
}
#ifndef LUMIN_FUNCTION_NO_MAIN
int main(int argc,char** argv){includeGeometry=argc>1&&std::strcmp(argv[1],"--end-to-end")==0;denseReference=argc>1&&std::strcmp(argv[1],"--dense-reference")==0;compareModel();require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,256,"Function shader",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);
 run(vg,includeGeometry||(argc>1&&std::strcmp(argv[1],"--benchmark")==0));
 for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();return imageFailures?1:0;}

#endif
