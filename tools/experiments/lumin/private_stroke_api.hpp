#pragma once
#include <nanovg.h>
struct PrivateStrokeApi {
 NVGcontext* (*create)(int);
 void (*destroy)(NVGcontext*);
 void (*begin)(NVGcontext*,float,float,float);
 void (*end)(NVGcontext*);
 void (*path)(NVGcontext*);
 void (*move)(NVGcontext*,float,float);
 void (*line)(NVGcontext*,float,float);
 void (*width)(NVGcontext*,float);
 void (*color)(NVGcontext*,NVGcolor);
 void (*cap)(NVGcontext*,int);
 void (*join)(NVGcontext*,int);
 void (*stroke)(NVGcontext*);
 NVGparams* (*params)(NVGcontext*);
};
PrivateStrokeApi privateStrokeApi();
void privateStrokeCandidate(bool enabled);
