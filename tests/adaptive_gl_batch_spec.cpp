// Headless-window integration test. Substitute only Rack's private Scene/Window
// construction and frame clock; use real Widget traversal, NanoVG and OpenGL.
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <app/Scene.hpp>
#include <window/Window.hpp>
#undef PRIVATE
#include "../src/visual/AdaptiveGlSurface.hpp"
#include "../src/GlResourceRetirement.hpp"
#include "../tools/preview_benchmark_utils.hpp"
#include <cstring>
#define NANOVG_GL2
#include <nanovg_gl.h>
#include <cstdio>
#include <cstdlib>
#include <vector>

static double testFrame = 1.;
namespace rack {
// The fixture owns its stack members; the real Context destructor owns heap
// host services and assumes Rack logging/host initialization.
Context::~Context() {}
namespace app {
Scene::Scene() : internal(nullptr), rackScroll(nullptr), rack(nullptr), menuBar(nullptr), browser(nullptr) {}
Scene::~Scene() {}
}
namespace window {
Window::Window() : internal(nullptr) {}
Window::~Window() {}
double Window::getFrameTime() { return testFrame; }
}
}

using Surface=visual_assets::AdaptiveGlSurface;
static int calls=0;
struct Paint {GLuint program=0,vbo=0; GLint tone=-1; bool checkState=false; bool poison=false; float color=.5f;};
static GLint get(GLenum e){GLint value=0;glGetIntegerv(e,&value);return value;}
static GLuint shader(GLenum type,const char* source){
 GLuint s=glCreateShader(type);glShaderSource(s,1,&source,nullptr);glCompileShader(s);
 GLint ok=0;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);require(ok,"shader compiled");return s;
}
static void paint(void* user,Vec size,int y){
 auto& p=*static_cast<Paint*>(user);++calls;
 if(p.checkState){
  require(!glIsEnabled(GL_DEPTH_TEST)&&!glIsEnabled(GL_STENCIL_TEST)&&!glIsEnabled(GL_CULL_FACE)&&!glIsEnabled(GL_SCISSOR_TEST),"neutral enables");
  require(get(GL_CURRENT_PROGRAM)==0&&get(GL_ARRAY_BUFFER_BINDING)==0&&get(GL_UNPACK_ROW_LENGTH)==0,"neutral bindings/upload state");
  require(get(GL_BLEND_SRC_RGB)==GL_ONE&&get(GL_BLEND_DST_RGB)==GL_ONE_MINUS_SRC_ALPHA,"neutral blend");
  for(int i=0;i<4;++i){GLint enabled=1;glGetVertexAttribiv(i,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&enabled);require(!enabled,"neutral attributes");}
 }
 glViewport(0,y,int(size.x),int(size.y));glDisable(GL_SCISSOR_TEST);glDisable(GL_DEPTH_TEST);glDisable(GL_STENCIL_TEST);glDisable(GL_CULL_FACE);
 glDisable(GL_ALPHA_TEST);glEnable(GL_BLEND);glBlendEquation(GL_FUNC_ADD);glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
 glUseProgram(p.program);glUniform1f(p.tone,p.color);
 glBindBuffer(GL_ARRAY_BUFFER,p.vbo);glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,nullptr);glDrawArrays(GL_TRIANGLES,0,3);
 if(p.poison){glEnable(GL_DEPTH_TEST);glEnable(GL_STENCIL_TEST);glEnable(GL_CULL_FACE);glEnable(GL_SCISSOR_TEST);glScissor(0,0,1,1);
  glBlendFunc(GL_ZERO,GL_ZERO);glPixelStorei(GL_UNPACK_ROW_LENGTH,99);glEnableVertexAttribArray(3);}
}
int main(int argc,char** argv){
 require(glfwInit(),"GLFW");glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
 auto* native=glfwCreateWindow(256,256,"Grouped surfaces",nullptr,nullptr);require(native,"window");
 glfwMakeContextCurrent(native);require(glewInit()==GLEW_OK,"GLEW");while(glGetError()!=GL_NO_ERROR){}
 auto* vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);require(vg,"NanoVG");
 rack::Context ctx;rack::widget::EventState events;rack::app::Scene scene;rack::window::Window window;
 ctx.scene=&scene;ctx.window=&window;ctx.event=&events;window.win=native;window.vg=vg;window.pixelRatio=1;rack::contextSet(&ctx);
 auto service=[&](){++testFrame;for(auto* child:scene.children)child->step();};
 GLuint vs=shader(GL_VERTEX_SHADER,"#version 120\nattribute vec2 p;varying vec2 uv;void main(){gl_Position=vec4(p,0,1);uv=p*.5+.5;}");
 GLuint fs=shader(GL_FRAGMENT_SHADER,"#version 120\nvarying vec2 uv;uniform float tone;void main(){gl_FragColor=vec4(uv.x*.5,uv.y*.5,tone*.5,.5);}");
 GLuint program=glCreateProgram();glAttachShader(program,vs);glAttachShader(program,fs);glBindAttribLocation(program,0,"p");glLinkProgram(program);
 GLint linked=0;glGetProgramiv(program,GL_LINK_STATUS,&linked);require(linked,"link");glDeleteShader(vs);glDeleteShader(fs);
 GLuint vbo=0;glGenBuffers(1,&vbo);glBindBuffer(GL_ARRAY_BUFFER,vbo);float triangle[]={-1,-1,3,-1,-1,3};glBufferData(GL_ARRAY_BUFFER,sizeof(triangle),triangle,GL_STATIC_DRAW);glBindBuffer(GL_ARRAY_BUFFER,0);
 if(argc>1&&std::strcmp(argv[1],"--benchmark")==0){
  for(int count:{1,2,8,32,64})for(int repeat=0;repeat<3;++repeat)for(int order=0;order<2;++order){
   bool grouped=(order+repeat)%2;Surface surfaces[64];Surface::Update updates[64];Paint jobs[64];
   auto* target=nvgluCreateFramebuffer(vg,106*count,48,0);require(target,"target");
   for(int i=0;i<count;++i){jobs[i].program=program;jobs[i].vbo=vbo;jobs[i].tone=glGetUniformLocation(program,"tone");jobs[i].color=float(i%3)*.25f;auto& u=updates[i];u.surface=&surfaces[i];u.logicalSize={106,48};u.policy.minDensity=u.policy.maxDensity=1;u.policy.sizeQuantum=1;u.policy.vertexAttributeCount=4;u.callback=paint;u.user=&jobs[i];}
   std::vector<double> cpu;Queries queries;
   for(int frame=0;frame<460;++frame){service();nvgluBindFramebuffer(target);glViewport(0,0,106*count,48);glDisable(GL_SCISSOR_TEST);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
    for(int i=0;i<count;++i)surfaces[i].markDirty();
    auto* query=queries.begin(frame);auto start=Clock::now();
    if(grouped)require(Surface::renderBatch(vg,updates,count),"batch");
    else for(int i=0;i<count;++i){auto& u=updates[i];require(u.surface->renderIfNeeded(vg,u.logicalSize,1,1,u.policy,false,u.callback,u.user),"single");}
    nvgBeginFrame(vg,106*count,48,1);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(106,48));
    for(int i=0;i<count;++i){nvgSave(vg);nvgTranslate(vg,106.f*i,0);require(surfaces[i].drawAligned(args,{106,48},1,{0,0}),"present");nvgRestore(vg);}
    nvgEndFrame(vg);double elapsed=us(start);queries.end(query);glFlush();if(frame>=100)cpu.push_back(elapsed);
   }
   queries.finish();require(glGetError()==GL_NO_ERROR,"benchmark GL state");char label[120];
   std::snprintf(label,sizeof(label),"%s/count%d/repeat%d/CPU",grouped?"grouped":"separate",count,repeat);report(label,cpu);
   std::snprintf(label,sizeof(label),"%s/count%d/repeat%d/GPU",grouped?"grouped":"separate",count,repeat);report(label,queries.results);
   nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
  }
 }else{
  Surface surfaces[2];Surface::Update updates[2];Paint jobs[2];
  for(int i=0;i<2;++i){jobs[i].program=program;jobs[i].vbo=vbo;jobs[i].tone=glGetUniformLocation(program,"tone");jobs[i].poison=i==0;jobs[i].color=float(i)*.6f;
   auto& u=updates[i];u.surface=&surfaces[i];u.logicalSize={106,48};u.policy.minDensity=u.policy.maxDensity=1;u.policy.vertexAttributeCount=4;u.callback=paint;u.user=&jobs[i];}
  auto* target=nvgluCreateFramebuffer(vg,220,54,0);require(target,"image target");
  std::vector<unsigned char> reference;
  for(int mode=0;mode<4;++mode){service();nvgluBindFramebuffer(target);glViewport(0,0,220,54);glDisable(GL_SCISSOR_TEST);glClearColor(0,0,0,0);glClear(GL_COLOR_BUFFER_BIT);
   glUseProgram(program);glBindBuffer(GL_ARRAY_BUFFER,vbo);glPixelStorei(GL_UNPACK_ROW_LENGTH,17);glEnable(GL_SCISSOR_TEST);glScissor(2,3,19,23);
   for(auto& s:surfaces)s.markDirty();
   updates[0].policy.shaderOnlyState=mode>=2;updates[1].policy.shaderOnlyState=mode==2;
   glMatrixMode(GL_PROJECTION);glLoadIdentity();glTranslatef(3,4,0);
   glMatrixMode(GL_TEXTURE);glLoadIdentity();glTranslatef(5,6,0);
   Surface::BatchStats stats;
   if(mode){for(auto& j:jobs)j.checkState=true;require(Surface::renderBatch(vg,updates,2,&stats),"group admitted");require(stats.updates==2&&stats.hostBoundaries==1,"one boundary");}
   else for(auto& u:updates)require(u.surface->renderIfNeeded(vg,u.logicalSize,1,1,u.policy,false,u.callback,u.user),"independent admitted");
   require(get(GL_FRAMEBUFFER_BINDING)==int(target->fbo)&&get(GL_CURRENT_PROGRAM)==int(program)&&get(GL_ARRAY_BUFFER_BINDING)==int(vbo),"host bindings restored");
   require(glIsEnabled(GL_SCISSOR_TEST)&&get(GL_UNPACK_ROW_LENGTH)==17,"host scissor/upload restored");
   GLint scissor[4];glGetIntegerv(GL_SCISSOR_BOX,scissor);require(scissor[0]==2&&scissor[1]==3&&scissor[2]==19&&scissor[3]==23,"scissor box restored");
   require(get(GL_MATRIX_MODE)==GL_TEXTURE,"matrix mode preserved");
   GLfloat matrix[16];glGetFloatv(GL_PROJECTION_MATRIX,matrix);require(matrix[12]==3&&matrix[13]==4,"projection preserved");
   glGetFloatv(GL_TEXTURE_MATRIX,matrix);require(matrix[12]==5&&matrix[13]==6,"texture matrix preserved");
   glPixelStorei(GL_UNPACK_ROW_LENGTH,0);glDisable(GL_SCISSOR_TEST);
   nvgBeginFrame(vg,220,54,1);Widget::DrawArgs args;args.vg=vg;args.clipBox=Rect(Vec(0,0),Vec(106,48));
   nvgGlobalAlpha(vg,.7f);nvgScissor(vg,7,4,203,44);
   for(int i=0;i<2;++i){nvgSave(vg);nvgTranslate(vg,110.f*i+.37f,.37f);require(surfaces[i].drawAligned(args,{106,48},1,{0,0}),"image present");nvgRestore(vg);}
   nvgEndFrame(vg);std::vector<unsigned char> image(220*54*4);glReadPixels(0,0,220,54,GL_RGBA,GL_UNSIGNED_BYTE,image.data());
   if(!mode)reference=image;else require(image==reference,"grouped pixels identical to separate");
  }
  Surface::BatchStats stats;int before=calls;require(Surface::renderBatch(vg,updates,2,&stats),"cached batch admitted");require(stats.hostBoundaries==0&&stats.updates==0&&calls==before,"cached batch no guard or render");
  surfaces[1].markDirty();require(Surface::renderBatch(vg,updates,2,&stats)&&stats.updates==1&&stats.hostBoundaries==1,"independent dirty surface");
  auto duplicate=updates[1];updates[1].surface=updates[0].surface;before=calls;require(!Surface::renderBatch(vg,updates,2)&&calls==before,"duplicate rejected before rendering");updates[1]=duplicate;
  require(!Surface::renderBatch(vg,updates,65),"bounded count");updates[1].logicalSize.x=std::numeric_limits<float>::quiet_NaN();require(!Surface::renderBatch(vg,updates,2),"invalid extent rejected");
  updates[1].logicalSize={106,48};glfwMakeContextCurrent(nullptr);
  require(!Surface::renderBatch(vg,updates,2),"missing current context rejected");glfwMakeContextCurrent(native);
  nvgluBindFramebuffer(nullptr);nvgluDeleteFramebuffer(target);
  std::puts("PASS: identical grouped output; mixed-state isolation/restoration; cached/dirty/invalid admission");
 }
 for(int i=0;i<4;++i)service();
 glUseProgram(0);glBindBuffer(GL_ARRAY_BUFFER,0);glDeleteProgram(program);glDeleteBuffers(1,&vbo);
 while(!scene.children.empty()){auto* child=scene.children.front();scene.removeChild(child);delete child;}
 require(glGetError()==GL_NO_ERROR,"no GL error");nvgDeleteGL2(vg);ctx.scene=nullptr;ctx.window=nullptr;ctx.event=nullptr;rack::contextSet(nullptr);glfwDestroyWindow(native);glfwTerminate();
}
