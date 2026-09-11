#pragma once
#include "../GlResourceRetirement.hpp"
#include "../GlLifecycleUtils.hpp"
#include <vector>
#include <array>
#include <limits>

namespace lumin {
struct Point { float x, y; };
enum class StrokeResult { Ok, Invalid, Unsupported, ResourceFailure };
enum class Cap { Butt, Round };

// UI thread only. One device is shared by strokes submitted to a private pass.
// The executor owns the target and restores host state. This owns no framebuffer.
class PolylineDevice {
 friend class PolylineStroke;
 GLuint program=0;
 gl_lifecycle::ContextLease lease;
 GLint mapping=-1, sampler=-1, material=-1, radius=-1, count=-1, cap=-1;
 void clear() { gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Program,program);program=0; }
 bool ensure(NVGcontext* vg) {
  if(!gl_lifecycle::resourceContextMatches(lease,vg)){clear();lease=gl_lifecycle::acquireResourceContext(vg);}
  if(!lease||!gl_lifecycle::resourceContextIsCurrent(lease))return false;
  if(program&&glIsProgram(program))return true;
  clear();
  const char* vs=R"GLSL(#version 120
attribute vec2 position;
attribute vec2 range;
uniform vec4 mapping;
varying vec2 point;
varying vec2 indices;
void main(){point=position;indices=range;
 gl_Position=vec4(position*mapping.xy+mapping.zw,0.0,1.0);}
)GLSL";
  const char* fs=R"GLSL(#version 120
uniform sampler2D segments;
uniform vec4 material;
uniform vec2 radius;
uniform float count;
uniform int cap;
varying vec2 point;
varying vec2 indices;
void main(){
 float minimumSquared=1.0e20;
 float endpointField=1.0e20;
 for(int j=0;j<64;++j){
  float i=indices.x+float(j);if(i>indices.y)break;
  vec4 ends=texture2D(segments,vec2((i+0.5)/1024.0,0.5));
  vec2 ab=ends.zw-ends.xy;
  float len2=dot(ab,ab);
  float t=dot(point-ends.xy,ab)/len2;
  vec2 delta=point-(ends.xy+ab*clamp(t,0.0,1.0));
  float squared=dot(delta,delta);
  if(cap==0&&(i<0.5||i>count-1.5)){
   float d=sqrt(squared)-radius.x;
   if(i<0.5)d=max(d,-t*sqrt(len2));
   if(i>count-1.5)d=max(d,(t-1.0)*sqrt(len2));
   endpointField=min(endpointField,d);
  }else minimumSquared=min(minimumSquared,squared);
 }
 float field=min(sqrt(minimumSquared)-radius.x,endpointField);
 float alpha=material.a*clamp(0.5-field*radius.y,0.0,1.0);
 gl_FragColor=vec4(material.rgb*alpha,alpha);
}
)GLSL";
  GLuint shaders[2]={glCreateShader(GL_VERTEX_SHADER),glCreateShader(GL_FRAGMENT_SHADER)};
  bool ok=true;
  for(int i=0;i<2;++i){const char* source=i?fs:vs;glShaderSource(shaders[i],1,&source,nullptr);glCompileShader(shaders[i]);GLint status=0;glGetShaderiv(shaders[i],GL_COMPILE_STATUS,&status);ok=ok&&status;}
  if(ok){program=glCreateProgram();for(auto s:shaders)glAttachShader(program,s);
   glBindAttribLocation(program,0,"position");glBindAttribLocation(program,1,"range");glLinkProgram(program);
   GLint status=0;glGetProgramiv(program,GL_LINK_STATUS,&status);ok=status;}
  for(auto s:shaders)gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Shader,s);
  if(!ok){clear();return false;}
  mapping=glGetUniformLocation(program,"mapping");sampler=glGetUniformLocation(program,"segments");
  material=glGetUniformLocation(program,"material");radius=glGetUniformLocation(program,"radius");
  count=glGetUniformLocation(program,"count");cap=glGetUniformLocation(program,"cap");return true;
 }
public:
 PolylineDevice()=default;
 PolylineDevice(const PolylineDevice&)=delete;
 PolylineDevice& operator=(const PolylineDevice&)=delete;
 ~PolylineDevice(){clear();}
};

// First specialization: open, nondecreasing-X paths, at most 1025 points.
// Round joins are evaluated as a union in disjoint X slabs, never blended twice.
// Geometry is copied on revision changes; rejected updates invalidate old output.
class PolylineStroke {
 struct Vertex {float x,y,first,last;};
 struct Tile {int first=-1,last=-1;float low=0,high=0;};
 std::vector<Point> points;
 std::vector<Vertex> vertices;
 std::vector<Tile> tiles;
 std::array<std::array<float,4>,1024> endpoints{};
 GLuint buffer=0,texture=0;
 gl_lifecycle::ContextLease lease;
 uint64_t revision=0;
 size_t sourceCount=0;
 bool valid=false,geometryDirty=true,coverageDirty=true;
 float preparedWidth=-1,preparedDensity=-1;
 size_t uploaded=0,builds=0;
 void clear(){
  gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Buffer,buffer);
  gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Texture,texture);
  buffer=texture=0;geometryDirty=coverageDirty=true;
 }
 StrokeResult prepare(float width,float density){
  if(!coverageDirty&&width==preparedWidth&&density==preparedDensity)return StrokeResult::Ok;
  coverageDirty=true;vertices.clear();
  if(points.size()<2){coverageDirty=false;preparedWidth=width;preparedDensity=density;return StrokeResult::Ok;}
  const float support=width*.5f+.5f/density,step=4.f/density;
  float origin=std::floor((points.front().x-support)/step)*step;
  double number=std::ceil((points.back().x+support-origin)/step);
  if(number<1||number>4096)return StrokeResult::Unsupported;
  tiles.assign(size_t(number),Tile{});
  for(size_t i=0;i+1<points.size();++i){auto a=points[i],b=points[i+1];
   int first=std::max(0,int(std::floor((a.x-support-origin)/step)));
   int last=std::min(int(tiles.size())-1,int(std::floor((b.x+support-origin)/step)));
   for(int j=first;j<=last;++j){auto& tile=tiles[j];
    // Clip the centerline to the expanded slab before computing its Y support.
    // This bounds fragments near the line even for long, steep segments.
    float lo=0,hi=1;
    if(b.x>a.x){lo=std::max(0.f,std::min(1.f,(origin+j*step-support-a.x)/(b.x-a.x)));
     hi=std::max(0.f,std::min(1.f,(origin+(j+1)*step+support-a.x)/(b.x-a.x)));}
    float y0=a.y+(b.y-a.y)*lo,y1=a.y+(b.y-a.y)*hi;
    float low=std::min(y0,y1)-support,high=std::max(y0,y1)+support;
    if(tile.first<0){tile.first=int(i);tile.low=low;tile.high=high;}
    tile.last=int(i);tile.low=std::min(tile.low,low);tile.high=std::max(tile.high,high);
    if(tile.last-tile.first>=64)return StrokeResult::Unsupported;
   }
  }
  for(size_t i=0;i<tiles.size();++i){auto t=tiles[i];if(t.first<0)continue;
   float x=origin+i*step;Vertex a{x,t.low,float(t.first),float(t.last)},b{x+step,t.low,a.first,a.last},c{x+step,t.high,a.first,a.last},d{x,t.high,a.first,a.last};
   vertices.insert(vertices.end(),{a,b,c,a,c,d});
  }
  preparedWidth=width;preparedDensity=density;coverageDirty=false;++builds;return StrokeResult::Ok;
 }
public:
 PolylineStroke(){points.reserve(1025);vertices.reserve(4096*6);tiles.reserve(4096);}
 PolylineStroke(const PolylineStroke&)=delete;
 PolylineStroke& operator=(const PolylineStroke&)=delete;
 ~PolylineStroke(){clear();}
 StrokeResult update(const Point* source,size_t count,uint64_t nextRevision){
  if(valid&&revision==nextRevision&&sourceCount==count)return StrokeResult::Ok;
  valid=false;geometryDirty=coverageDirty=true;points.clear();
  if((count&&!source)||count>1025)return StrokeResult::Invalid;
  for(size_t i=0;i<count;++i){auto p=source[i];
   if(!std::isfinite(p.x)||!std::isfinite(p.y)||std::fabs(p.x)>1e6f||std::fabs(p.y)>1e6f)return StrokeResult::Invalid;
   if(!points.empty()){
    auto a=points.back();if(p.x<a.x)return StrokeResult::Unsupported;
    if(p.x==a.x&&p.y==a.y)continue;
    float dx=p.x-a.x,dy=p.y-a.y;
    if(dx*dx+dy*dy<1e-12f)return StrokeResult::Unsupported;
   }
   points.push_back(p);
  }
  for(size_t i=0;i+1<points.size();++i)endpoints[i]={{points[i].x,points[i].y,points[i+1].x,points[i+1].y}};
  sourceCount=count;revision=nextRevision;valid=true;return StrokeResult::Ok;
 }
 // Private pass only: texture unit 0, attributes 0/1. Positive uniform scale.
 // Color/translation changes do not rebuild or upload geometry.
 StrokeResult draw(PolylineDevice& device,NVGcontext* vg,Vec target,int viewportY,
                   float density,Vec offset,float width,NVGcolor color,Cap cap=Cap::Butt){
  if(!valid||viewportY<0||!std::isfinite(density)||density<.01f||density>16||!std::isfinite(width)||width<=0||width>1024
    ||!std::isfinite(target.x)||!std::isfinite(target.y)||target.x<1||target.y<1||target.x>16384||target.y>16384
    ||!std::isfinite(offset.x)||!std::isfinite(offset.y)
    ||!std::isfinite(color.r)||!std::isfinite(color.g)||!std::isfinite(color.b)||!std::isfinite(color.a))return StrokeResult::Invalid;
  bool newCoverage=coverageDirty||width!=preparedWidth||density!=preparedDensity;
  auto result=prepare(width,density);if(result!=StrokeResult::Ok)return result;
  if(points.size()<2)return StrokeResult::Ok;
  if(!gl_lifecycle::resourceContextMatches(lease,vg)){clear();lease=gl_lifecycle::acquireResourceContext(vg);newCoverage=true;}
  if(!lease||!gl_lifecycle::resourceContextIsCurrent(lease)||!GLEW_ARB_texture_float||!device.ensure(vg))return StrokeResult::ResourceFailure;
  if(!gl_lifecycle::isValidProgramBufferPair(device.program,buffer)||!gl_lifecycle::areValidTextures({texture})){
   clear();newCoverage=true;glGenBuffers(1,&buffer);glGenTextures(1,&texture);
   glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texture);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F_ARB,1024,1,0,GL_RGBA,GL_FLOAT,nullptr);
  }
  glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texture);glBindBuffer(GL_ARRAY_BUFFER,buffer);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);glPixelStorei(GL_UNPACK_ALIGNMENT,4);glPixelStorei(GL_UNPACK_ROW_LENGTH,0);glPixelStorei(GL_UNPACK_SKIP_ROWS,0);glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
  if(geometryDirty){glTexSubImage2D(GL_TEXTURE_2D,0,0,0,GLsizei(points.size()-1),1,GL_RGBA,GL_FLOAT,endpoints.data());uploaded+=(points.size()-1)*4*sizeof(float);geometryDirty=false;}
  if(newCoverage){glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),vertices.data(),GL_DYNAMIC_DRAW);uploaded+=vertices.size()*sizeof(Vertex);}
  coverageDirty=false;
  glViewport(0,viewportY,int(target.x),int(target.y));glDisable(GL_DEPTH_TEST);glDisable(GL_STENCIL_TEST);glDisable(GL_CULL_FACE);glDisable(GL_SCISSOR_TEST);
  glEnable(GL_BLEND);glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);glBlendFuncSeparate(GL_ONE,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);glColorMask(1,1,1,1);
  glUseProgram(device.program);glUniform4f(device.mapping,2*density/target.x,-2*density/target.y,2*offset.x/target.x-1,1-2*offset.y/target.y);
  glUniform1i(device.sampler,0);glUniform4f(device.material,color.r,color.g,color.b,color.a);
  glUniform2f(device.radius,width*.5f,density);glUniform1f(device.count,float(points.size()-1));glUniform1i(device.cap,cap==Cap::Butt?0:1);
  glEnableVertexAttribArray(0);glEnableVertexAttribArray(1);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(2*sizeof(float)));
  glDrawArrays(GL_TRIANGLES,0,GLsizei(vertices.size()));return StrokeResult::Ok;
 }
 size_t uploadedBytes() const{return uploaded;}
 size_t coverageBuilds() const{return builds;}
};
} // namespace lumin
