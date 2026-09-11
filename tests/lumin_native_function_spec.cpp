#define LUMIN_FUNCTION_NO_MAIN
#include "lumin_function_curve_spec.cpp"
#undef LUMIN_FUNCTION_NO_MAIN
#include "render/NativeFunctionContour.hpp"
#include "render/FunctionContourPilot.hpp"
static void nativeRun(NVGcontext* vg){
 lumin::FunctionCurve shader(lumin::FunctionCurve::Kernel::TwoStep);
 lumin::NativeFunctionContour native(&shader);
 Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect({0,0},{256,256});
 auto begin=[&](){++testFrame;nvgluBindFramebuffer(nullptr);glViewport(0,0,256,256);glDisable(GL_SCISSOR_TEST);glColorMask(1,1,1,1);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);nvgBeginFrame(vg,256,256,1);nvgTranslate(vg,.37f,.81f);};
 auto draw=[&](float shape){return native.drawContour(args,{106,48},.3f,shape,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true,true);};
 begin();auto r=draw(.2f);require(r.presented&&r.updated&&r.updateNs>0,"native initial render");require(draw(.2f).cacheHit,"native cache hit");require(!draw(.3f).presented,"same-frame overwrite rejected");glViewport(0,0,256,256);nvgEndFrame(vg);
 begin();require(draw(.3f).updated,"native changed render");glViewport(0,0,256,256);nvgEndFrame(vg);
 begin();require(draw(.3f).cacheHit,"next-frame cache");nvgRotate(vg,.1f);require(!draw(.3f).presented,"rotation fallback");glViewport(0,0,256,256);nvgEndFrame(vg);
 Widget::ContextDestroyEvent destroy;destroy.vg=vg;native.onContextDestroy(destroy);
 Widget::ContextCreateEvent create;create.vg=vg;native.onContextCreate(create);
 begin();require(draw(.3f).updated,"context rebuild");glViewport(0,0,256,256);nvgEndFrame(vg);
 for(float scale:{.75f,1.f,1.5f,2.f}){
  begin();nvgScale(vg,scale,scale);require(draw(.2f).presented,"native aligned render");glViewport(0,0,256,256);nvgEndFrame(vg);
  std::vector<unsigned char> actual(256*256*4),expected(actual.size());glReadPixels(0,0,256,256,GL_RGBA,GL_UNSIGNED_BYTE,actual.data());glDisable(GL_SCISSOR_TEST);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
  require(shader.draw(vg,{256,256},0,{106,48},{.37f/scale,.81f/scale},scale,.3f,.2f,true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true),"direct reference");
  glReadPixels(0,0,256,256,GL_RGBA,GL_UNSIGNED_BYTE,expected.data());int maxByte=0;for(size_t i=0;i<actual.size();++i)maxByte=std::max(maxByte,std::abs(int(actual[i])-int(expected[i])));std::printf("scale %.2f max byte %d\n",scale,maxByte);require(maxByte<=1,"native image alignment");
 }
 begin();
 auto rect=[&](float x,NVGcolor c){nvgBeginPath(vg);nvgRect(vg,x,80,10,10);nvgFillColor(vg,c);nvgFill(vg);};
 rect(10,nvgRGB(255,0,0));nvgSave(vg);nvgScissor(vg,0,0,60,48);nvgGlobalAlpha(vg,.5f);
 require(draw(.4f).updated,"tinted clipped native update");nvgRestore(vg);rect(30,nvgRGB(0,255,0));glViewport(0,0,256,256);nvgEndFrame(vg);
 unsigned char pixel[4];glReadPixels(15,170,1,1,GL_RGBA,GL_UNSIGNED_BYTE,pixel);require(pixel[0]==255&&pixel[1]==0,"host draw before callback");glReadPixels(35,170,1,1,GL_RGBA,GL_UNSIGNED_BYTE,pixel);require(pixel[0]==0&&pixel[1]==255,"host draw after callback");
 auto* nested=nvgluCreateFramebuffer(vg,16,16,0);begin();args.fb=nested;require(!draw(.5f).presented,"nested fallback");args.fb=nullptr;glViewport(0,0,256,256);nvgEndFrame(vg);nvgluDeleteFramebuffer(nested);
 APP->window->pixelRatio=1.5f;begin();require(!draw(.5f).presented,"fractional pixel ratio fallback");glViewport(0,0,256,256);nvgEndFrame(vg);APP->window->pixelRatio=1;
 lumin::FunctionContourPilot guarded(&shader);
 for(int repeat=0;repeat<3;++repeat)for(int order=0;order<3;++order){int mode=(order+repeat)%3;native.valid=false;native.clearOnly=mode==0;std::vector<double> cpu;
  for(int i=0;i<360;++i){begin();auto start=Clock::now();if(mode==2){auto r=guarded.draw(args,{106,48},.3f,.8f*std::sin(i*.037f),true,nvgRGBA(28,204,217,255),nvgRGBA(230,230,220,255),true,true);require(r.updated,"guarded timing update");}else require(draw(.8f*std::sin(i*.037f)).updated,"timing update");glViewport(0,0,256,256);nvgEndFrame(vg);if(i>=60)cpu.push_back(us(start));glFlush();}
  report(mode==2?"guarded/shader-and-host-flush":mode?"native/shader-and-host-flush":"native/clear-and-host-flush",cpu);
 }
 require(glGetError()==GL_NO_ERROR,"native GL errors");std::puts("PASS: native render, cache, context, fallback, alignment");
}
int main(){require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);auto* native=glfwCreateWindow(256,256,"Function pilot",nullptr,nullptr);require(native,"window");glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);
 nativeRun(vg);for(int i=0;i<4;++i){++testFrame;for(auto* child:scene.children)child->step();}while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();return 0;}
