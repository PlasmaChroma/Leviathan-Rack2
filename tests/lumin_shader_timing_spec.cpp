#define LUMIN_FUNCTION_NO_MAIN
#include "lumin_function_curve_spec.cpp"
#undef LUMIN_FUNCTION_NO_MAIN
using Curve=lumin::FunctionCurve;
struct TimingJob {
 Curve renderer;NVGcontext* vg=nullptr;float density=1;int count=7;Curve::Layer layers[7];
 void setFrame(int frame){for(int i=0;i<count;++i){auto& l=layers[i];l.ratio=.5f+.45f*std::sin((frame-i*3)*.027f);l.shape=.95f*std::sin((frame-i*3)*.053f);l.shark=((frame-i*3+1000)/90)%2!=0;l.width=i==count-1?1.4f:1.15f;l.color=l.other=i==count-1?nvgRGBA(230,230,220,255):nvgRGBA(255,190,80,20+i*14);}}
 static void paint(void* user,Vec target,int y){auto& j=*static_cast<TimingJob*>(user);for(int i=0;i<2;++i)require(j.renderer.drawBatch(j.vg,target,y,{106,48},{110.f*i,0},j.density,j.layers,j.count),"timed shader");}
};
static void quality(NVGcontext* vg){
 Curve reference,prepared(Curve::Kernel::Prepared),fast(Curve::Kernel::FastAtan),twoStep(Curve::Kernel::TwoStep);
 int maxPrepared=0,maxFast=0,maxTwoStep=0,cases=0;uint64_t totalDifference=0;
 for(float density:{1.f,1.19f,2.f,4.f,8.f}){
  auto* target=nvgluCreateFramebuffer(vg,int(std::ceil(108*density)),int(std::ceil(50*density)),0);require(target,"quality target");int w=int(std::ceil(108*density)),h=int(std::ceil(50*density));
  for(int scenario=0;scenario<60;++scenario){Curve::Layer layers[7];int count=scenario%3==0?7:1;
   for(int i=0;i<count;++i){auto& l=layers[i];const float ratios[]={.01f,.05f,.5f,.95f,.99f};const float shapes[]={-1.f,-.85f,-.001f,0.f,.001f,.85f,1.f};l.ratio=ratios[(scenario+i)%5];l.shape=shapes[(scenario/5+i)%7];l.shark=(scenario+i)%2;l.highlight=scenario%2;l.width=i==count-1?1.4f:1.15f;l.color=nvgRGBA(28,204,217,i==count-1?255:75);l.other=nvgRGBA(230,230,220,180);}
   std::vector<unsigned char> expected;
   Curve* renderers[]={&reference,&prepared,&fast,&twoStep};
   for(int mode=0;mode<4;++mode){nvgluBindFramebuffer(target);glViewport(0,0,w,h);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
    require(renderers[mode]->drawBatch(vg,{float(w),float(h)},0,{106,48},{.37f,.81f},density,layers,count),"quality shader");std::vector<unsigned char> image(size_t(w)*h*4);glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,image.data());
    if(!mode)expected=image;else for(size_t i=0;i<image.size();++i){int diff=std::abs(int(expected[i])-int(image[i]));if(mode==1)maxPrepared=std::max(maxPrepared,diff);else if(mode==3)maxTwoStep=std::max(maxTwoStep,diff);else{maxFast=std::max(maxFast,diff);totalDifference+=diff;}}
   }++cases;
  }nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
 std::printf("QUALITY cases=%d max_prepared_byte=%d max_fast_byte=%d fast_total_byte_error=%llu\n",cases,maxPrepared,maxFast,(unsigned long long)totalDifference);
 std::printf("TWO-STEP max_byte=%d\n",maxTwoStep);
 require(maxTwoStep<=2,"two iterations must preserve coverage");
 require(maxPrepared<=2&&maxFast<=2,"optimization must stay within two bytes of reference shader");
}
static void timing(NVGcontext* vg){
 for(int pass=0;pass<2;++pass)for(float density:{1.f,2.f})for(int layers:{1,7})for(int repeat=0;repeat<3;++repeat)for(int order=0;order<5;++order){int variant=(repeat+order)%5;
  TimingJob job;job.vg=vg;job.density=density;job.count=layers;job.renderer.setKernel(variant==4?Curve::Kernel::TwoStep:variant==1?Curve::Kernel::Prepared:variant==2?Curve::Kernel::FastAtan:Curve::Kernel::Reference);job.renderer.setSubmitPixels(variant!=3);
  Surface surface;Surface::Update u;u.surface=&surface;u.logicalSize={220,49};u.policy.minDensity=u.policy.maxDensity=density;u.policy.sizeQuantum=1;u.policy.vertexAttributeCount=1;u.callback=TimingJob::paint;u.user=&job;
  auto* target=nvgluCreateFramebuffer(vg,int(220*density),int(54*density),0);require(target,"timing target");Queries queries;std::vector<double> cpu;
  for(int frame=0;frame<460;++frame){++testFrame;for(auto* child:APP->scene->children)child->step();job.setFrame(frame);surface.markDirty();nvgluBindFramebuffer(target);glViewport(0,0,int(220*density),int(54*density));glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
   auto* query=queries.begin(frame);auto start=Clock::now();
   if(!pass)TimingJob::paint(&job,{float(int(220*density)),float(int(54*density))},0);
   else{require(Surface::renderBatch(vg,&u,1),"complete pass");nvgBeginFrame(vg,220,54,density);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect({0,0},{220,48});require(surface.drawAligned(args,{220,48},density,{0,0}),"complete presentation");nvgEndFrame(vg);}
   double elapsed=us(start);queries.end(query);glFlush();if(frame>=100)cpu.push_back(elapsed);
  }queries.finish();require(queries.skipped==0,"all GPU samples collected");char label[180];const char* names[]={"reference","prepared","fast-atan","no-draw-control","two-step"};std::snprintf(label,180,"%s/%s/%.0fx/layers%d/repeat%d/CPU",pass?"complete":"private",names[variant],density,layers,repeat);report(label,cpu);std::snprintf(label,180,"%s/%s/%.0fx/layers%d/repeat%d/GPU",pass?"complete":"private",names[variant],density,layers,repeat);report(label,queries.results);nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 }
}
int main(int argc,char** argv){require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,256,"Shader timing",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);std::printf("GL %s / %s\n",glGetString(GL_VENDOR),glGetString(GL_RENDERER));
 if(argc>1&&std::strcmp(argv[1],"--benchmark")==0)timing(vg);else quality(vg);
 for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}require(glGetError()==GL_NO_ERROR,"final GL errors");nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();return 0;}

