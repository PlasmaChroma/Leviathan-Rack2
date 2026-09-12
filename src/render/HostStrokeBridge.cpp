#include "HostStrokeBridge.hpp"
#include <cmath>

// Preserve host entry points before the private geometry core's symbol renaming.
namespace {
const auto hostParams=nvgInternalParams;
const auto hostTransform=nvgCurrentTransform;
const auto hostBeginPath=nvgBeginPath;
const auto hostMove=nvgMoveTo;
const auto hostLine=nvgLineTo;
const auto hostStroke=nvgStroke;
}
#include "../../tools/experiments/lumin/vendor/nanovg/rename.h"
#include "../../tools/experiments/lumin/vendor/nanovg/declarations.inc"
#define STBTT_STATIC
#define NVG_NO_STB
// Fontstash includes the full static stb_truetype API; unused entry points are
// expected. Keep this warning suppression local to the bundled implementation.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "../../tools/experiments/lumin/vendor/nanovg/nanovg.inc"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace lumin {
namespace {
int createGeometry(void*,void*) { return 1; }
int createDummyTexture(void*,int,int,int,int,const unsigned char*) { return 1; }
int deleteDummyTexture(void*,int) { return 1; }
struct Submission {
 NVGcontext* geometry;
 decltype(NVGparams::renderStroke) original;
 bool submitted=false;
};
thread_local Submission* pending=nullptr;
void capture(void* user,NVGpaint* paint,NVGcompositeOperationState composite,
 NVGscissor* scissor,float fringe,float width,const NVGpath* sentinel,int count) {
 auto* job=pending;
 if(!job||count!=1||sentinel[0].nstroke<2||!std::isfinite(fringe)||fringe<=0)return;
 auto* ctx=job->geometry;
 nvg__setDevicePixelRatio(ctx,1.f/fringe);
 nvg__flattenPaths(ctx);
 // The two-point host stroke tells us whether its effective AA was enabled.
 const float aa=sentinel[0].stroke[0].u==.5f?0.f:fringe;
 offlineCandidateEnabled=1;
 if(!nvg__expandStroke(ctx,width*.5f,aa,NVG_BUTT,NVG_ROUND,10.f))return;
 job->original(user,paint,composite,scissor,fringe,width,ctx->cache->paths,ctx->cache->npaths);
 job->submitted=true;
}
struct CaptureScope {
 NVGparams* params;
 decltype(NVGparams::renderStroke) original;
 CaptureScope(NVGparams* p,Submission* job):params(p),original(p->renderStroke){pending=job;params->renderStroke=capture;}
 ~CaptureScope(){params->renderStroke=original;pending=nullptr;}
};
}
struct HostStrokeBridge::Impl { NVGcontext* geometry=nullptr; };
HostStrokeBridge::HostStrokeBridge() {
 NVGparams params{};params.renderCreate=createGeometry;
 params.renderCreateTexture=createDummyTexture;params.renderDeleteTexture=deleteDummyTexture;
 impl=new Impl;impl->geometry=nvgCreateInternal(&params,nullptr);
}
HostStrokeBridge::~HostStrokeBridge(){if(impl){nvgDeleteInternal(impl->geometry);delete impl;}}
bool HostStrokeBridge::stroke(NVGcontext* host,const rack::math::Vec* points,int count){
 // Backend POD/callback ABI has only been validated against this Rack release.
 if(rack::APP_VERSION!="2.6.6"||!host||!impl||!impl->geometry||!points||count<2||count>2048||pending)return false;
 auto* params=hostParams(host);if(!params||!params->renderStroke)return false;
 float transform[6];hostTransform(host,transform);
 for(float value:transform)if(!std::isfinite(value))return false;
 for(int i=0;i<count;++i)if(!std::isfinite(points[i].x)||!std::isfinite(points[i].y))return false;
 auto* ctx=impl->geometry;nvgReset(ctx);
 nvgTransform(ctx,transform[0],transform[1],transform[2],transform[3],transform[4],transform[5]);
 nvgBeginPath(ctx);for(int i=0;i<count;++i)(i?nvgLineTo:nvgMoveTo)(ctx,points[i].x,points[i].y);
 Submission job;job.geometry=ctx;job.original=params->renderStroke;
 {
  CaptureScope scope(params,&job);
  // No GL work: host derives its exact effective stroke state for this cheap
  // sentinel, whose geometry is discarded by capture(). No host context layout
  // is read, and its backend/lifecycle/user pointer are never replaced.
  hostBeginPath(host);hostMove(host,0,0);hostLine(host,16,0);hostStroke(host);
 }
 return job.submitted;
}
}
