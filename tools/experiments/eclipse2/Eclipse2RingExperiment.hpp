#pragma once
#include "plugin.hpp"
#include "visual/ApertureBloomMasks.hpp"
#include <array>
namespace eclipse2_ring {
enum class Mode { Baseline, Geometry, Masks, Static, Shader };
inline Mode& requested() { static Mode value=Mode::Baseline;return value; }
inline Mode mode() { return isDragonKingDebugEnabled()?requested():Mode::Baseline; }
inline void setMode(Mode value) { if(isDragonKingDebugEnabled()) requested()=value; }
inline bool cachedGeometry() { return mode()==Mode::Geometry||mode()==Mode::Masks||mode()==Mode::Static; }
inline int& trackHandle() {static int value=-1;return value;}
inline int& nestedTrackHandle() {static int value=-1;return value;}
inline bool drawTrack(const Widget::DrawArgs& args) {
	if(mode()!=Mode::Static) return false;
	int handle=args.vg==APP->window->vg?trackHandle():nestedTrackHandle();
	if(handle<=0) return false;
	nvgBeginPath(args.vg);nvgRect(args.vg,0,0,34,34);
	nvgFillPaint(args.vg,nvgImagePattern(args.vg,0,0,34,34,0,handle,1));nvgFill(args.vg);return true;
}
inline Vec ledPosition(int index,float minAngle,float maxAngle) {
	struct Positions { float min=100.f,max=100.f;std::array<Vec,25> values; };
	static thread_local Positions positions;
	if(positions.min!=minAngle||positions.max!=maxAngle) {
		for(int i=0;i<25;++i) {float a=minAngle+(float(i)/24.f)*(maxAngle-minAngle);positions.values[i]=Vec(std::sin(a),-std::cos(a));}
		positions.min=minAngle;positions.max=maxAngle;
	}
	return positions.values[index];
}
inline bool glow(const Widget::DrawArgs& args,float x,float y,float radius,float bloom) {
	if(mode()!=Mode::Masks&&mode()!=Mode::Static) return false;
	auto* cache=aperture_bloom::findCache(args.vg,true);
	auto* mask=cache?cache->get(args.vg,radius*.4f,radius*3.2f):nullptr;
	if(!mask) return false;
	NVGcolor color=nvgRGBA(255,235,140,255);color.a*=bloom;
	aperture_bloom::draw(args.vg,*mask,x,y,color);return true;
}
}
