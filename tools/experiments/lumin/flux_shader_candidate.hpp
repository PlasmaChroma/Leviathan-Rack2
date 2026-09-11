#pragma once
#include "PrivateContourEngine.hpp"
#include "../../../src/visual/AdaptiveGlSurface.hpp"
#include "../../../src/GlLifecycleUtils.hpp"
#include <array>
#include <vector>

// Offline candidate. Adapted from WYRM's segment-distance shader and disjoint
// body tiles. Unlike WYRM, lookup uses the actual nonuniform retained points.
// No NanoVG stroke calls, CPU join expansion, or resampling in this renderer.
namespace leviathan { namespace render {
class ShaderContourEngine {
 struct Vertex {float x,y,first,last;};
 static constexpr int maxSegments=1024;
 static constexpr int maxTileSegments=64;
 struct Tile {int first=-1,last=-1;float low=0,high=0;};
 GLuint program=0,buffer=0,texture=0;
 gl_lifecycle::ContextLease lease;
 GLint sizeLoc=-1,textureLoc=-1,radiusLoc=-1,colorLoc=-1,countLoc=-1;
 std::array<std::array<float,4>,maxSegments> endpoints{};
 std::vector<Vertex> vertices;
 std::vector<Tile> tiles;
 bool ensure() {
  if(gl_lifecycle::isValidProgramBufferPair(program,buffer)
     &&gl_lifecycle::areValidTextures({texture}))return true;
  resetObjects();
  const char* vs=R"GLSL(#version 120
attribute vec2 position;
attribute vec2 range;
uniform vec2 size;
varying vec2 pixel;
varying vec2 indices;
void main(){pixel=position;indices=range;
 gl_Position=vec4(2.0*position.x/size.x-1.0,1.0-2.0*position.y/size.y,0.0,1.0);}
)GLSL";
  const char* fs=R"GLSL(#version 120
uniform sampler2D segments;
uniform float radius;
uniform float count;
uniform vec4 color;
varying vec2 pixel;
varying vec2 indices;
void main(){
 float field=1.0e20;
 for(int j=0;j<64;++j){
  float i=indices.x+float(j);
  if(i>indices.y)break;
  vec4 ends=texture2D(segments,vec2((i+0.5)/1024.0,0.5));
  vec2 ab=ends.zw-ends.xy;
  float len2=dot(ab,ab);
  if(len2<0.00000001)continue;
  float t=dot(pixel-ends.xy,ab)/len2;
  vec2 delta=pixel-(ends.xy+ab*clamp(t,0.0,1.0));
  float d=sqrt(dot(delta,delta))-radius;
  // Round interior joins, butt caps only at the ends of each colored path.
  if(i<0.5)d=max(d,-t*sqrt(len2));
  if(i>count-1.5)d=max(d,(t-1.0)*sqrt(len2));
  field=min(field,d);
 }
 float coverage=clamp(0.5-field,0.0,1.0);
 float alpha=color.a*coverage;
 gl_FragColor=vec4(color.rgb*alpha,alpha);
}
)GLSL";
  GLuint shaders[2]={glCreateShader(GL_VERTEX_SHADER),glCreateShader(GL_FRAGMENT_SHADER)};
  bool ok=true;
  for(int i=0;i<2;++i){
   const char* source=i?fs:vs;glShaderSource(shaders[i],1,&source,nullptr);glCompileShader(shaders[i]);
   GLint status=0;glGetShaderiv(shaders[i],GL_COMPILE_STATUS,&status);
   if(!status){char log[2048]{};glGetShaderInfoLog(shaders[i],sizeof(log),nullptr,log);std::fprintf(stderr,"Flux shader: %s\n",log);ok=false;}
  }
  if(ok){
   program=glCreateProgram();for(auto s:shaders)glAttachShader(program,s);
   glBindAttribLocation(program,0,"position");glBindAttribLocation(program,1,"range");glLinkProgram(program);
   GLint linked=0;glGetProgramiv(program,GL_LINK_STATUS,&linked);ok=linked!=0;
  }
  for(auto s:shaders)gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Shader,s);
  if(!ok){resetObjects();return false;}
  sizeLoc=glGetUniformLocation(program,"size");textureLoc=glGetUniformLocation(program,"segments");
  radiusLoc=glGetUniformLocation(program,"radius");colorLoc=glGetUniformLocation(program,"color");countLoc=glGetUniformLocation(program,"count");
  glGenBuffers(1,&buffer);glGenTextures(1,&texture);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F_ARB,maxSegments,1,0,GL_RGBA,GL_FLOAT,nullptr);
  return program&&buffer&&texture;
 }
 void resetObjects(){
  gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Program,program);
  gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Buffer,buffer);
  gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Texture,texture);
  program=buffer=texture=0;
 }
 bool prepare(const ContourSegment& path,Vec active,float density,Vec offset){
  if(!path.points||path.count<2||path.count>maxSegments+1)return false;
  const float support=.7f*density+.5f;
  const float tileWidth=16.f;
  tiles.assign(size_t(std::ceil(active.x/tileWidth)),Tile{});
  for(int i=0;i<path.count-1;++i){
   auto a=path.points[i],b=path.points[i+1];
   if(!std::isfinite(a.x)||!std::isfinite(a.y)||!std::isfinite(b.x)||!std::isfinite(b.y)||b.x<a.x
      ||std::fabs(a.x)>1e6f||std::fabs(a.y)>1e6f||std::fabs(b.x)>1e6f||std::fabs(b.y)>1e6f)return false;
   float ax=a.x*density+offset.x,ay=a.y*density+offset.y,bx=b.x*density+offset.x,by=b.y*density+offset.y;
   endpoints[i]={{ax,ay,bx,by}};
   int first=std::max(0,int(std::floor((ax-support)/tileWidth)));
   int last=std::min(int(tiles.size())-1,int(std::floor((bx+support)/tileWidth)));
   for(int j=first;j<=last;++j){auto& tile=tiles[j];
    if(tile.first<0){tile.first=i;tile.low=std::min(ay,by)-support;tile.high=std::max(ay,by)+support;}
    tile.last=i;tile.low=std::min(tile.low,std::min(ay,by)-support);tile.high=std::max(tile.high,std::max(ay,by)+support);
    if(tile.last-tile.first+1>maxTileSegments)return false;
   }
  }
  vertices.clear();vertices.reserve(tiles.size()*6);
  for(size_t j=0;j<tiles.size();++j){const auto& tile=tiles[j];if(tile.first<0)continue;
   float x0=j*tileWidth,x1=std::min(active.x,(j+1)*tileWidth),y0=std::max(0.f,tile.low),y1=std::min(active.y,tile.high);
   if(y1<=y0)continue;
   Vertex a{x0,y0,float(tile.first),float(tile.last)},b{x1,y0,a.first,a.last},c{x1,y1,a.first,a.last},d{x0,y1,a.first,a.last};
   vertices.insert(vertices.end(),{a,b,c,a,c,d});
  }
  return true;
 }
public:
 ShaderContourEngine()=default;
 ShaderContourEngine(const ShaderContourEngine&)=delete;
 ShaderContourEngine& operator=(const ShaderContourEngine&)=delete;
 ~ShaderContourEngine(){resetObjects();}
 void reset(){resetObjects();lease.reset();}
 // Must be called under AdaptiveGlSurface's state guard. Inputs already pruned.
 bool render(NVGcontext* vg,Vec active,int viewportY,float density,Vec offset,const ContourSegment* paths,int count){
  if(!paths||count<1||count>2)return false;
  if(!gl_lifecycle::resourceContextMatches(lease,vg)){reset();lease=gl_lifecycle::acquireResourceContext(vg);}
  if(!lease||!GLEW_ARB_texture_float||!ensure())return false;
  glViewport(0,viewportY,int(active.x),int(active.y));
  glDisable(GL_DEPTH_TEST);glDisable(GL_STENCIL_TEST);glDisable(GL_CULL_FACE);glDisable(GL_SCISSOR_TEST);
  glEnable(GL_BLEND);glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);glBlendFuncSeparate(GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glUseProgram(program);glUniform2f(sizeLoc,active.x,active.y);glUniform1i(textureLoc,0);glUniform1f(radiusLoc,.7f*density);
  glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texture);glBindBuffer(GL_ARRAY_BUFFER,buffer);
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS,0);glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
  glEnableVertexAttribArray(0);glEnableVertexAttribArray(1);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(2*sizeof(float)));
  for(int i=0;i<count;++i){const auto& path=paths[i];
   if(!prepare(path,active,density,offset))return false;
   glTexSubImage2D(GL_TEXTURE_2D,0,0,0,path.count-1,1,GL_RGBA,GL_FLOAT,endpoints.data());
   glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),vertices.data(),GL_STREAM_DRAW);
   glUniform1f(countLoc,float(path.count-1));glUniform4f(colorLoc,path.color.r,path.color.g,path.color.b,path.color.a);
   glDrawArrays(GL_TRIANGLES,0,GLsizei(vertices.size()));
  }
  return true;
 }
};
} }
