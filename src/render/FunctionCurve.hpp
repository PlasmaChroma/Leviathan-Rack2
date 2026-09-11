#pragma once
#include "../GlResourceRetirement.hpp"
#include "../GlLifecycleUtils.hpp"
#include <string>
// Experimental analytic function family; Flux live pilot is opt-in.
// No sampled curve texture, per-shape vertices, or polyline segment search.
namespace lumin {
class FunctionCurve {
public:
 enum class Kernel { Reference, Prepared, FastAtan, TwoStep };
private:
 Kernel kernel=Kernel::Reference;
 bool submitPixels=true;
 GLuint program=0,buffer=0;
 gl_lifecycle::ContextLease lease;
 GLint mapLoc=-1,boxLoc=-1,shapeLoc=-1,colorLoc=-1,otherLoc=-1,styleLoc=-1,countLoc=-1,constantsLoc=-1;
 void reset(){gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Program,program);gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Buffer,buffer);program=buffer=0;}
 bool ensure(NVGcontext* vg){
  if(!gl_lifecycle::resourceContextMatches(lease,vg)){reset();lease=gl_lifecycle::acquireResourceContext(vg);}
  if(!lease||!gl_lifecycle::resourceContextIsCurrent(lease))return false;
  if(gl_lifecycle::isValidProgramBufferPair(program,buffer))return true;
  reset();
  const char* vs=R"GLSL(#version 120
attribute vec2 position;
uniform vec4 mapping;
uniform vec4 box;
varying vec2 pixel;
void main(){pixel=position*box.xy+box.zw;
 gl_Position=vec4(pixel*mapping.xy+mapping.zw,0.0,1.0);}
)GLSL";
  const char* fs=R"GLSL(#version 120
uniform vec4 curveShapes[7];
uniform vec4 curveStyles[7];
uniform vec4 curveColors[7];
uniform vec4 curveOthers[7];
uniform int curveCount;
uniform vec4 curveConstants[7];
vec4 constants;
vec4 shape;
vec4 style;
uniform vec4 box;
vec4 color;
vec4 other;
varying vec2 pixel;
// Range reduction bounds the Taylor input to |z| <= tan(pi/8).
float fastAtanPositive(float x){
 bool reciprocal=x>1.0;float z=reciprocal?1.0/x:x;
 bool shifted=z>0.414213562373095;z=shifted?(z-1.0)/(z+1.0):z;
 float z2=z*z;
 float p=z*(1.0+z2*(-0.333333333333333+z2*(0.2+z2*(-0.142857142857143+z2*(0.111111111111111-z2*0.090909090909091)))));
 p+=shifted?0.785398163397448:0.0;
 return reciprocal?1.570796326794897-p:p;
}
vec2 phase(float v,float k,float norm,float root,float inverseRootNorm){
 if(abs(k)<0.00001)return vec2(v,1.0);
#if PREPARED
 if(k<0.0)return vec2((v-k*v*v*v/3.0)*norm,(1.0-k*v*v)*norm);
 #if FAST_ATAN
 float angle=fastAtanPositive(root*v);
 #else
 float angle=atan(root*v);
 #endif
 return vec2(angle*inverseRootNorm,norm/(1.0+k*v*v));
#else
 if(k<0.0)return vec2((v-k*v*v*v/3.0)/norm,(1.0-k*v*v)/norm);
 root=sqrt(k);return vec2(atan(root*v)/(root*norm),1.0/((1.0+k*v*v)*norm));
#endif
}
float side(vec2 p,float start,float span,float height,float k,float norm,bool rise,bool split){
 float root=rise?constants.x:constants.y,inverseRootNorm=rise?constants.z:constants.w;
 float support=style.z*0.5+0.5/style.y;
 if(p.x<start-support||p.x>start+span+support||p.y< -support||p.y>height+support)return 1.0e20;
 float v=clamp(1.0-p.y/height,0.0,1.0);
 // Any covered point must lie inside this output interval. Monotonicity
 // gives a conservative horizontal bound before the closest-point solve.
 float lo=clamp(1.0-(p.y+support)/height,0.0,1.0);
 float hi=clamp(1.0-(p.y-support)/height,0.0,1.0);
 float a=phase(lo,k,norm,root,inverseRootNorm).x,b=phase(hi,k,norm,root,inverseRootNorm).x;
 float xlo=start+span*(rise?a:1.0-b),xhi=start+span*(rise?b:1.0-a);
 if(p.x<xlo-support||p.x>xhi+support)return 1.0e20;
 // Closest-point Gauss-Newton with bounded iterations; width uses Euclidean distance.
 for(int j=0;j<SOLVE_STEPS;++j){vec2 f=phase(v,k,norm,root,inverseRootNorm);
  vec2 q=vec2(start+span*(rise?f.x:1.0-f.x),height*(1.0-v));
  vec2 tangent=vec2((rise?span:-span)*f.y,-height);
  v=clamp(v+dot(p-q,tangent)/dot(tangent,tangent),0.0,1.0);
 }
 vec2 f=phase(v,k,norm,root,inverseRootNorm);
 vec2 q=vec2(start+span*(rise?f.x:1.0-f.x),height*(1.0-v));
 float d=length(p-q)-style.z*0.5;
 // Outer baseline caps are butt. The continuous peak has a round union;
 // independently colored sides use butt caps at the shared endpoint.
 vec2 f0=phase(0.0,k,norm,root,inverseRootNorm),f1=phase(1.0,k,norm,root,inverseRootNorm);
 vec2 t0=normalize(vec2((rise?span:-span)*f0.y,-height));
 vec2 t1=normalize(vec2((rise?span:-span)*f1.y,-height));
 vec2 q0=vec2(start+(rise?0.0:span),height),q1=vec2(start+(rise?span:0.0),0.0);
 d=max(d,-dot(p-q0,t0));if(split)d=max(d,dot(p-q1,t1));
 return d;
}
vec4 paint(float d,vec4 c){float a=c.a*clamp(0.5-d*style.y,0.0,1.0);return vec4(c.rgb*a,a);}
vec4 shadeCurve(){
 vec2 size=box.xy-vec2(3.4);vec2 p=pixel-box.zw-vec2(1.7);
 float peak=size.x*style.x;
 float a=side(p,0.0,peak,size.y,shape.x,shape.z,true,style.w>0.5);
 float b=side(p,peak,size.x-peak,size.y,shape.y,shape.w,false,style.w>0.5);
 if(style.w<0.5)return paint(min(a,b),color);
 else{vec4 ca=paint(a,color),cb=paint(b,other);return cb+ca*(1.0-cb.a);}
}
void main(){
 vec4 result=vec4(0.0);
 for(int i=0;i<7;++i){if(i>=curveCount)break;
  constants=curveConstants[i];shape=curveShapes[i];style=curveStyles[i];color=curveColors[i];other=curveOthers[i];
  vec4 next=shadeCurve();result=next+result*(1.0-next.a);
 }
 gl_FragColor=result;
}
)GLSL";
  std::string fragment(fs);fragment.insert(fragment.find("\n")+1,std::string("#define PREPARED ")+(kernel==Kernel::Reference?"0":"1")+"\n#define FAST_ATAN "+(kernel==Kernel::FastAtan?"1":"0")+"\n#define SOLVE_STEPS "+(kernel==Kernel::TwoStep?"2":"4")+"\n");
  GLuint shaders[2]={glCreateShader(GL_VERTEX_SHADER),glCreateShader(GL_FRAGMENT_SHADER)};bool ok=true;
  for(int i=0;i<2;++i){const char* source=i?fragment.c_str():vs;glShaderSource(shaders[i],1,&source,nullptr);glCompileShader(shaders[i]);GLint status=0;glGetShaderiv(shaders[i],GL_COMPILE_STATUS,&status);if(!status){char log[2048]{};glGetShaderInfoLog(shaders[i],2048,nullptr,log);std::fprintf(stderr,"%s\n",log);}ok=ok&&status;}
  if(ok){program=glCreateProgram();for(auto s:shaders)glAttachShader(program,s);glBindAttribLocation(program,0,"position");glLinkProgram(program);GLint status=0;glGetProgramiv(program,GL_LINK_STATUS,&status);ok=status;}
  for(auto s:shaders)gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Shader,s);
  if(!ok){reset();return false;}
  mapLoc=glGetUniformLocation(program,"mapping");boxLoc=glGetUniformLocation(program,"box");shapeLoc=glGetUniformLocation(program,"curveShapes[0]");colorLoc=glGetUniformLocation(program,"curveColors[0]");otherLoc=glGetUniformLocation(program,"curveOthers[0]");styleLoc=glGetUniformLocation(program,"curveStyles[0]");
  constantsLoc=glGetUniformLocation(program,"curveConstants[0]");
  countLoc=glGetUniformLocation(program,"curveCount");
  const float quad[]={0,0,1,0,1,1,0,0,1,1,0,1};glGenBuffers(1,&buffer);glBindBuffer(GL_ARRAY_BUFFER,buffer);glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);return true;
 }
public:
 explicit FunctionCurve(Kernel value=Kernel::Reference):kernel(value){}
 void setKernel(Kernel value){if(value!=kernel){reset();kernel=value;}}
 void setSubmitPixels(bool value){submitPixels=value;}FunctionCurve(const FunctionCurve&)=delete;FunctionCurve& operator=(const FunctionCurve&)=delete;
 ~FunctionCurve(){reset();}
 static float integral(float v,float k){if(std::fabs(k)<1e-5f)return v;return k<0?v-k*v*v*v/3.f:std::atan(std::sqrt(k)*v)/std::sqrt(k);}
 struct Layer {float ratio=.5f,shape=0,width=1.4f;bool shark=false,highlight=false;NVGcolor color{},other{};};
 // Up to six history layers followed by the current curve, preserving order.
 bool drawBatch(NVGcontext* vg,Vec target,int y,Vec size,Vec origin,float density,const Layer* layers,int count){
  if(!layers||count<1||count>7||!std::isfinite(density)||density<.5f||density>8||!std::isfinite(target.x)||!std::isfinite(target.y)||target.x<1||target.y<1||target.x>16384||target.y>16384||!std::isfinite(size.x)||!std::isfinite(size.y)||!std::isfinite(origin.x)||!std::isfinite(origin.y)||size.x<=3.4f||size.y<=3.4f)return false;
  float prepared[28]{},shapes[28]{},styles[28]{},colors[28]{},others[28]{};
  for(int i=0;i<count;++i){auto& l=layers[i];if(!std::isfinite(l.ratio)||l.ratio<=0||l.ratio>=1||!std::isfinite(l.shape)||std::fabs(l.shape)>1||!std::isfinite(l.width)||l.width<=0||l.width>1.4f)return false;
   float k=40*l.shape,rise=l.shark?-k:k;int j=4*i;shapes[j]=rise;shapes[j+1]=k;shapes[j+2]=integral(1,rise);shapes[j+3]=integral(1,k);styles[j]=l.ratio;styles[j+1]=density;styles[j+2]=l.width;styles[j+3]=l.highlight?1:0;
   if(kernel!=Kernel::Reference){
    shapes[j+2]=1.f/shapes[j+2];shapes[j+3]=1.f/shapes[j+3];
    prepared[j]=rise>0?std::sqrt(rise):0;prepared[j+1]=k>0?std::sqrt(k):0;
    prepared[j+2]=rise>0?shapes[j+2]/prepared[j]:0;prepared[j+3]=k>0?shapes[j+3]/prepared[j+1]:0;
   }
   for(int c=0;c<4;++c){if(!std::isfinite(l.color.rgba[c])||!std::isfinite(l.other.rgba[c]))return false;colors[j+c]=l.color.rgba[c];others[j+c]=l.other.rgba[c];}
  }
  if(!ensure(vg))return false;
  glViewport(0,y,int(target.x),int(target.y));glDisable(GL_SCISSOR_TEST);glDisable(GL_STENCIL_TEST);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glEnable(GL_BLEND);glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);glBlendFuncSeparate(GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);glColorMask(1,1,1,1);
  glUseProgram(program);glUniform4f(mapLoc,2*density/target.x,-2*density/target.y,-1,1);glUniform4f(boxLoc,size.x,size.y,origin.x,origin.y);
  if(constantsLoc>=0)glUniform4fv(constantsLoc,count,prepared);
  glUniform1i(countLoc,count);glUniform4fv(shapeLoc,count,shapes);glUniform4fv(styleLoc,count,styles);glUniform4fv(colorLoc,count,colors);glUniform4fv(otherLoc,count,others);
  glBindBuffer(GL_ARRAY_BUFFER,buffer);glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),nullptr);if(submitPixels)glDrawArrays(GL_TRIANGLES,0,6);return true;
 }
 bool draw(NVGcontext* vg,Vec target,int y,Vec size,Vec origin,float density,float ratio,float signedShape,bool shark,NVGcolor color,NVGcolor other,bool highlighted,float width=1.4f){
  Layer layer;layer.ratio=ratio;layer.shape=signedShape;layer.shark=shark;layer.color=color;layer.other=other;layer.highlight=highlighted;layer.width=width;
  return drawBatch(vg,target,y,size,origin,density,&layer,1);
 }
};
}
