#pragma once
#include <nanovg.h>
namespace leviathan { namespace render {
struct ContourPoint {float x,y;};
struct ContourSegment {
 const ContourPoint* points=nullptr;int count=0;NVGcolor color{};
 ContourSegment()=default;
 ContourSegment(const ContourPoint* p,int n,NVGcolor c):points(p),count(n),color(c){}
};
// Delete callbacks enqueue names against the owner's context lease; they never
// issue GL calls from widget destruction or after an editor context is gone.
using RetireContourObject=void(*)(void*,int,unsigned int);
struct PrivateContourEngine;
PrivateContourEngine* createContourEngine(void* user,RetireContourObject retire);
void destroyContourEngine(PrivateContourEngine* engine);
void renderContourEngine(PrivateContourEngine* engine,float width,float height,float density,
                         const ContourSegment* segments,int count,float lineWidth,float offsetX=0,float offsetY=0);
} }
