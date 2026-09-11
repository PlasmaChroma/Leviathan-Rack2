#define LUMIN_FUNCTION_NO_MAIN
#include "lumin_function_curve_spec.cpp"
#undef LUMIN_FUNCTION_NO_MAIN
#include "render/FunctionContourPilot.hpp"
static void pilotRun(NVGcontext* vg){
 lumin::FunctionContourPilot pilot;
 auto* target=nvgluCreateFramebuffer(vg,220,110,0);require(target,"pilot target");
 Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect({0,0},{106,48});
 auto begin=[&](){++testFrame;nvgluBindFramebuffer(target);glViewport(0,0,220,110);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);nvgBeginFrame(vg,220,110,1);nvgTranslate(vg,.37f,.81f);};
 auto draw=[&](float shape,bool measure=false){return pilot.draw(args,{106,48},.3f,shape,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true,measure);};
 begin();glEnableVertexAttribArray(3);auto first=draw(.85f,true);GLint untouched=0;glGetVertexAttribiv(3,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&untouched);require(untouched,"undeclared attribute state preserved");glDisableVertexAttribArray(3);require(first.presented&&first.updated&&!first.cacheHit,"initial pilot update");require(first.phases.hostBoundaries==1&&first.phases.callbackNs>0&&first.phases.captureNs>0&&first.phases.restoreNs>0,"CPU phases captured");
 auto hit=draw(.85f);require(hit.presented&&hit.cacheHit&&!hit.updated,"same-frame stable texture reuse");require(!draw(-.85f).presented,"same-frame overwrite rejected");nvgEndFrame(vg);
 begin();require(draw(-.85f).updated,"next-frame changed source updates");nvgEndFrame(vg);
 begin();require(draw(-.85f).cacheHit,"settled curve skips GL update");nvgRotate(vg,.1f);require(!draw(-.85f).presented,"rotation fallback");nvgEndFrame(vg);
 begin();args.fb=target;require(!draw(.85f).presented,"nested framebuffer fallback");args.fb=nullptr;nvgEndFrame(vg);
 for(auto* child:APP->scene->children){Widget::ContextDestroyEvent event;event.vg=vg;child->onContextDestroy(event);}
 for(auto* child:APP->scene->children){Widget::ContextCreateEvent event;event.vg=vg;child->onContextCreate(event);}
 begin();require(draw(-.85f).updated,"context epoch rebuilds same source");nvgEndFrame(vg);
 begin();require(draw(.2f).presented,"aligned pilot image");nvgEndFrame(vg);
 std::vector<unsigned char> actual(220*110*4),expected(actual.size());glReadPixels(0,0,220,110,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());glDisable(GL_SCISSOR_TEST);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
 {lumin::FunctionCurve reference(lumin::FunctionCurve::Kernel::TwoStep);require(reference.draw(vg,{220,110},0,{106,48},{.37f,.81f},1,.3f,.2f,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true),"direct aligned reference");}
 glReadPixels(0,0,220,110,GL_RGBA,GL_UNSIGNED_BYTE,expected.data());int maxByte=0;for(size_t i=0;i<actual.size();++i)maxByte=std::max(maxByte,std::abs(int(actual[i])-int(expected[i])));require(maxByte<=1,"fractional pilot composition matches direct shader");
 // Both previews prepare before either texture is queued in NanoVG.
 lumin::FunctionContourPilot pair[2];
 for(int frame=0;frame<4;++frame){
  begin();visual_assets::AdaptiveGlSurface::Update requests[2];int owners[2],count=0;
  for(int i=0;i<2;++i){nvgSave(vg);nvgTranslate(vg,float(i)*110,0);
   if(auto* u=pair[i].prepare(args,{106,48},.3f,(frame>=2&&i==1)?-.4f:.2f,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true)){requests[count]=*u;owners[count++]=i;}
   nvgRestore(vg);
  }
  visual_assets::AdaptiveGlSurface::BatchStats stats;
  require(visual_assets::AdaptiveGlSurface::renderBatch(vg,requests,count,&stats),"pair batch admitted");
  require(stats.updates==size_t(frame==0?2:frame==2?1:0)&&stats.hostBoundaries==size_t(count>0),"pair initial, cached and single-dirty boundaries");
  for(int j=0;j<count;++j)pair[owners[j]].complete(requests[j].rendered);
  for(int i=0;i<2;++i){nvgSave(vg);nvgTranslate(vg,float(i)*110,0);auto r=pair[i].draw(args,{106,48},.3f,(frame>=2&&i==1)?-.4f:.2f,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true,true);
   require(r.presented&&r.phases.hostBoundaries==0,"pair presentation has no second GL boundary");require(r.updated==(frame==0||(frame==2&&i==1)),"pair update telemetry");nvgRestore(vg);
  }
  nvgEndFrame(vg);
 }
 std::vector<double> capture,setup,allocation,shader,restore,present,flush,total;
 for(int i=0;i<460;++i){begin();auto start=Clock::now();auto result=draw(.85f*std::sin(i*.037f),true);double elapsed=us(start);require(result.presented,"profile pilot presented");auto end=Clock::now();nvgEndFrame(vg);double flushing=us(end);if(i>=100){auto& p=result.phases;capture.push_back(p.captureNs*.001);setup.push_back(p.setupNs*.001);allocation.push_back(p.targetNs*.001);shader.push_back(p.callbackNs*.001);restore.push_back(p.restoreNs*.001);present.push_back(elapsed-(p.captureNs+p.setupNs+p.targetNs+p.callbackNs+p.restoreNs)*.001);flush.push_back(flushing);total.push_back(elapsed+flushing);}glFlush();}
 report("pilot/capture",capture);report("pilot/shader-state-setup",setup);report("pilot/target-ensure-bind-clear",allocation);report("pilot/shader",shader);report("pilot/restore",restore);report("pilot/admission-and-image-submission",present);report("pilot/host-flush",flush);report("pilot/complete",total);
 // Matched two-preview workload: separate/full, grouped/full, grouped/restricted.
 for(int repeat=0;repeat<3;++repeat)for(int order=0;order<3;++order){
  int mode=(order+repeat)%3;
  lumin::FunctionCurve shared(lumin::FunctionCurve::Kernel::TwoStep);
  lumin::FunctionContourPilot previews[2]{lumin::FunctionContourPilot(mode?&shared:nullptr),lumin::FunctionContourPilot(mode?&shared:nullptr)};
  std::vector<double> cpu,state;
  for(int frame=0;frame<460;++frame){begin();auto start=Clock::now();
   visual_assets::AdaptiveGlSurface::Update requests[2];
   float shape=.85f*std::sin(frame*.037f);
   for(int i=0;i<2;++i){nvgSave(vg);nvgTranslate(vg,float(i)*110,0);
    auto* u=previews[i].prepare(args,{106,48},.3f,shape,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true);require(u,"benchmark dirty preparation");requests[i]=*u;requests[i].policy.shaderOnlyState=mode==2;nvgRestore(vg);
   }
   uint64_t stateNs=0;
   if(mode){visual_assets::AdaptiveGlSurface::BatchStats st;require(visual_assets::AdaptiveGlSurface::renderBatch(vg,requests,2,&st),"benchmark grouped");stateNs=st.captureNs+st.restoreNs;}
   else for(auto& u:requests){visual_assets::AdaptiveGlSurface::BatchStats st;require(visual_assets::AdaptiveGlSurface::renderBatch(vg,&u,1,&st),"benchmark separate");stateNs+=st.captureNs+st.restoreNs;}
   for(int i=0;i<2;++i){previews[i].complete(requests[i].rendered);nvgSave(vg);nvgTranslate(vg,float(i)*110,0);require(previews[i].draw(args,{106,48},.3f,shape,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true,false).presented,"benchmark presentation");nvgRestore(vg);}
   nvgEndFrame(vg);double elapsed=us(start);if(frame>=100){cpu.push_back(elapsed);state.push_back(stateNs*.001);}glFlush();
  }
  char label[100];std::snprintf(label,sizeof(label),"pair/mode%d/repeat%d/complete",mode,repeat);report(label,cpu);std::snprintf(label,sizeof(label),"pair/mode%d/repeat%d/state",mode,repeat);report(label,state);
 }
 require(glGetError()==GL_NO_ERROR,"pilot GL errors");nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
 std::puts("PASS: pilot cache, overwrite protection, fallback, context epoch and phase diagnostics");
}
int main(){require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,256,"Function pilot",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);
 pilotRun(vg);for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();return 0;}

