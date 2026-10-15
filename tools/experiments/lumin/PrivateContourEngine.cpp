#include <GL/glew.h>
#include "PrivateContourEngine.hpp"

namespace {
thread_local void* retirementUser=nullptr;
thread_local leviathan::render::RetireContourObject retirementCallback=nullptr;
void queueNames(int kind,int count,const GLuint* names) {
 for(int i=0;i<count;++i)if(names[i]&&retirementCallback)
  retirementCallback(retirementUser,kind,names[i]);
}
void queueBuffers(GLsizei count,const GLuint* names){queueNames(0,count,names);}
void queueProgram(GLuint name){queueNames(1,1,&name);}
void queueShader(GLuint name){queueNames(2,1,&name);}
void queueTextures(GLsizei count,const GLuint* names){queueNames(3,count,names);}
struct RetirementScope {
 void* previousUser=retirementUser;
 leviathan::render::RetireContourObject previousCallback=retirementCallback;
 RetirementScope(void* user,leviathan::render::RetireContourObject callback) {
  retirementUser=user;retirementCallback=callback;
 }
 ~RetirementScope(){retirementUser=previousUser;retirementCallback=previousCallback;}
};
}

// The core header was included for shared POD types above. Its renamed public
// declarations are materialized separately, without redefining those types.
#include "vendor/nanovg/rename.h"
#include "vendor/nanovg/declarations.inc"
#define STBTT_STATIC
#define NVG_NO_STB
#include "vendor/nanovg/nanovg.inc"
#undef glDeleteBuffers
#undef glDeleteProgram
#undef glDeleteShader
#undef glDeleteTextures
#define glDeleteBuffers queueBuffers
#define glDeleteProgram queueProgram
#define glDeleteShader queueShader
#define glDeleteTextures queueTextures
#define NANOVG_GL2_IMPLEMENTATION
#include "vendor/nanovg/nanovg_gl.h"

namespace leviathan { namespace render {
struct PrivateContourEngine {
 NVGcontext* vg=nullptr;void* user=nullptr;RetireContourObject retire=nullptr;
};
PrivateContourEngine* createContourEngine(void* user,RetireContourObject retire) {
 RetirementScope scope(user,retire);
 auto* result=new PrivateContourEngine;
 result->user=user;result->retire=retire;
 result->vg=nvgCreateGL2(NVG_ANTIALIAS|NVG_STENCIL_STROKES);
 if(!result->vg){delete result;return nullptr;}
 return result;
}
void destroyContourEngine(PrivateContourEngine* engine) {
 if(!engine)return;
 RetirementScope scope(engine->user,engine->retire);
 nvgDeleteGL2(engine->vg);delete engine;
}
void renderContourEngine(PrivateContourEngine* engine,float width,float height,float density,
 const ContourSegment* segments,int count,float lineWidth,float offsetX,float offsetY) {
 RetirementScope scope(engine->user,engine->retire);
 offlineCandidateEnabled=1;
 nvgBeginFrame(engine->vg,width,height,density);
 nvgTranslate(engine->vg,offsetX,offsetY);
 for(int j=0;j<count;++j) {
  const auto& segment=segments[j];if(segment.count<2)continue;
  nvgBeginPath(engine->vg);nvgMoveTo(engine->vg,segment.points[0].x,segment.points[0].y);
  for(int i=1;i<segment.count;++i)nvgLineTo(engine->vg,segment.points[i].x,segment.points[i].y);
  nvgStrokeColor(engine->vg,segment.color);nvgStrokeWidth(engine->vg,lineWidth);
  nvgLineCap(engine->vg,NVG_BUTT);nvgLineJoin(engine->vg,NVG_ROUND);nvgStroke(engine->vg);
 }
 nvgEndFrame(engine->vg);
}
} }
