#pragma once
#include <nanovg.h>

// Offline-only, solid-stroke bridge. All inherited drawing state is explicit.
// The host must be recording a frame. Never owns, flushes or modifies its backend.
struct CallbackBridge;
struct CallbackStroke {
 const float* xy=nullptr;
 int count=0;
 float transform[6]{1,0,0,1,0,0};
 NVGscissor scissor{{1,0,0,1,0,0},{-1,-1}};
 NVGcolor color{};
 float width=1.4f, alpha=1, density=1;
 int cap=NVG_BUTT, join=NVG_ROUND, operation=NVG_SOURCE_OVER;
};
CallbackBridge* createCallbackBridge();
void destroyCallbackBridge(CallbackBridge* bridge);
void submitCallbackStroke(CallbackBridge* bridge, const NVGparams* host,
 const CallbackStroke& stroke, bool optimized);
