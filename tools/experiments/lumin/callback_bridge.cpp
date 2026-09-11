// Uses the hash-pinned generated private geometry core, not its GL backend.
#include "rename.h"
#define STBTT_STATIC
#define NVG_NO_STB
#include "nanovg.c"
#include "callback_bridge.hpp"

struct CallbackBridge { NVGcontext* geometry=nullptr; };
static int createGeometry(void*,void*) { return 1; }
// The private core initializes a font atlas; this stroke-only adapter never uses
// it. Dummy texture callbacks avoid creating/deleting any host or GL resources.
static int createDummyTexture(void*,int,int,int,int,const unsigned char*) { return 1; }
static int deleteDummyTexture(void*,int) { return 1; }
CallbackBridge* createCallbackBridge() {
 NVGparams params{};
 params.renderCreate=createGeometry;
 params.renderCreateTexture=createDummyTexture;
 params.renderDeleteTexture=deleteDummyTexture;
 auto* bridge=new CallbackBridge;
 bridge->geometry=nvgCreateInternal(&params,nullptr);
 if(!bridge->geometry){delete bridge;return nullptr;}
 return bridge;
}
void destroyCallbackBridge(CallbackBridge* bridge) {
 if(bridge){nvgDeleteInternal(bridge->geometry);delete bridge;}
}
void submitCallbackStroke(CallbackBridge* bridge,const NVGparams* host,
 const CallbackStroke& stroke,bool optimized) {
 auto* ctx=bridge->geometry;
 // Copy only the submission callback. No host viewport/flush/cancel/delete calls.
 ctx->params.userPtr=host->userPtr;
 ctx->params.renderStroke=host->renderStroke;
 ctx->params.edgeAntiAlias=host->edgeAntiAlias;
 nvgReset(ctx);
 nvg__setDevicePixelRatio(ctx,stroke.density);
 nvgTransform(ctx,stroke.transform[0],stroke.transform[1],stroke.transform[2],
  stroke.transform[3],stroke.transform[4],stroke.transform[5]);
 nvg__getState(ctx)->scissor=stroke.scissor; // Private geometry context only.
 nvgGlobalAlpha(ctx,stroke.alpha);
 nvgGlobalCompositeOperation(ctx,stroke.operation);
 nvgStrokeColor(ctx,stroke.color);nvgStrokeWidth(ctx,stroke.width);
 nvgLineCap(ctx,stroke.cap);nvgLineJoin(ctx,stroke.join);
 nvgBeginPath(ctx);
 for(int i=0;i<stroke.count;++i)
  (i?nvgLineTo:nvgMoveTo)(ctx,stroke.xy[2*i],stroke.xy[2*i+1]);
 offlineCandidateEnabled=optimized;
 nvgStroke(ctx);
 // Backend has copied vertices/uniforms. Do not retain a host pointer afterward.
 ctx->params.userPtr=nullptr;ctx->params.renderStroke=nullptr;
}
