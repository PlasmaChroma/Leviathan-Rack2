#pragma once
#include "Eclipse2RuntimeBake.hpp"
#include "../render/NvgGraphicsLifecycle.hpp"
#include <array>

// Complete premultiplied dot states: no difference overlays over the panel.
// The track and continuous linking glow are drawn separately in their original order.
namespace eclipse2_dots {
constexpr int columns = 7, rows = 4, tile = 38, density = 4;
struct Atlas {
 eclipse2_bake::Raster raster;
 float bloom = -1.f;
 struct Image { NVGcontext* owner = nullptr; NVGcontext* window = nullptr; int handle = -1, width = 0, height = 0; float bloom = -1.f; };
 std::array<Image,8> images;
 bool prepare(bool bipolar) {
  if (bloom == settings::haloBrightness && !raster.rgba.empty()) return true;
  raster.rgba.clear(); raster.attempted = false;
  struct Bake { bool bipolar; } bake{bipolar};
  if (!raster.prepareSized([](const Widget::DrawArgs& args, void* ptr) {
   const bool bipolar = static_cast<Bake*>(ptr)->bipolar;
   Eclipse2Knob::ProgressLedRingWidget ring;
   ring.box.size = Vec(34,34); ring.bipolar = bipolar; ring.dotsOnly = true;
   for(int state=0;state<27;++state) {
    if (!bipolar) ring.valueNorm = state == 0 ? 0.f : std::min(1.f,(state-1+.25f)/24.f);
    else if(state==0) ring.valueNorm=.5f;
    else if(state<=13) ring.valueNorm=std::max(0.f,.5f-(state-1+.25f)/24.f);
    else ring.valueNorm=std::min(1.f,.5f+(state-14+.25f)/24.f);
    nvgSave(args.vg); nvgTranslate(args.vg,(state%columns)*tile+2,(state/columns)*tile+2);
    ring.draw(args); nvgRestore(args.vg);
   }
  },&bake,Vec(columns*tile,rows*tile),density)) return false;
  bloom=settings::haloBrightness; return true;
 }
 int imageFor(NVGcontext* vg) {
  Image* selected=nullptr;
  for(auto& im:images)if(im.owner==vg){selected=&im;break;}
  if(!selected)for(auto& im:images)if(!im.owner){selected=&im;break;}
  if(!selected)return -1;
  auto& im=*selected;
  const int w=columns*tile*density,h=rows*tile*density;
  if(im.bloom==bloom && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg,im.handle,w,h))return im.handle;
  im.window=APP->window->vg;
  if(!nvg_gfx_lifecycle::updateOwnedNvgImageRgba(im.owner,im.handle,im.width,im.height,vg,w,h,NVG_IMAGE_PREMULTIPLIED,raster.rgba.data()))return -1;
  im.bloom=bloom;return im.handle;
 }
};
inline std::array<Atlas,2>& atlases(){static std::array<Atlas,2> value;return value;}
inline void forget(NVGcontext* vg) {
 for(auto& atlas:atlases()) {
  atlas.raster.retryAfterContextCreate();
  for(auto& im:atlas.images)if(im.owner==vg || im.window==vg) {
   nvg_gfx_lifecycle::resetOwnedNvgImage(im.owner,im.handle,im.width,im.height,vg,false);im.window=nullptr;im.bloom=-1.f;
  }
 }
}
inline int stateFor(const Eclipse2Knob::ProgressLedRingWidget& ring) {
 int active=0;
 for(int i=0;i<25;++i) {
  const float led=float(i)/24.f;
  if(ring.bipolar) active += led>=std::min(.5f,ring.valueNorm) && led<=std::max(.5f,ring.valueNorm) && std::fabs(ring.valueNorm-.5f)>.005f;
  else active += ring.valueNorm>0.f && led<=ring.valueNorm;
 }
 if(!active)return 0;
 return ring.bipolar && ring.valueNorm>.5f ? 13+active : active;
}
inline bool draw(const Widget::DrawArgs& args,const Eclipse2Knob::ProgressLedRingWidget& ring) {
 if(ring.bipolar && ring.centerNorm!=.5f)return false;
 auto& atlas=atlases()[ring.bipolar?1:0];
 if(!atlas.prepare(ring.bipolar))return false;
 const int image=atlas.imageFor(args.vg);if(image<=0)return false;
 const int state=stateFor(ring);
 nvgBeginPath(args.vg);nvgRect(args.vg,0,0,34,34);
 nvgFillPaint(args.vg,nvgImagePattern(args.vg,-(state%columns)*tile-2,-(state/columns)*tile-2,columns*tile,rows*tile,0,image,1));nvgFill(args.vg);
 return true;
}
}
