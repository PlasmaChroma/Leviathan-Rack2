#pragma once

#include "../plugin.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include <array>
#include <cmath>
#include <vector>

namespace aperture_bloom {
// UI-thread cache. Never evict an image while the host may have queued its quad.
struct Mask {
 NVGcontext* owner = nullptr;
 int image = -1, width = 0, height = 0;
 float inner = 0, outer = 0, extent = 0;
};
struct Cache {
 std::array<Mask, 32> masks;
 NVGcontext* context = nullptr;
 size_t count = 0;
 unsigned builds = 0;
 void clear(NVGcontext* vg, bool destroy) {
  for (auto& m : masks)
   nvg_gfx_lifecycle::resetOwnedNvgImage(m.owner,m.image,m.width,m.height,vg,destroy);
  count=0;context=nullptr;
 }
 Mask* get(NVGcontext* vg, float inner, float outer) {
  if (!std::isfinite(inner)||!std::isfinite(outer)||inner<0||outer<=inner||outer>32) return nullptr;
  if(context!=vg){clear(vg,false);context=vg;}
  size_t index=0;
  for(;index<count;++index)if(masks[index].inner==inner&&masks[index].outer==outer)break;
  if(index==count){if(count==masks.size())return nullptr;++count;}
  auto& m=masks[index];m.inner=inner;m.outer=outer;
  if(m.owner==vg&&nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg,m.image,m.width,m.height))return &m;
  m.extent=std::ceil(outer+2.f);
  const int pixels=int(m.extent*8.f);
  std::vector<unsigned char> rgba(size_t(pixels)*pixels*4);
  const float feather=std::max(1.f,outer-inner),radius=(inner+outer)*.5f;
  // One-time radial mask generation, four samples per logical pixel. Bloom's
  // transparent edge needs no baked circle fringe. No GL framebuffer switch.
  for(int y=0;y<pixels;++y)for(int x=0;x<pixels;++x){
   const float dx=(x+.5f)*.25f-m.extent,dy=(y+.5f)*.25f-m.extent;
   const float distance=std::sqrt(dx*dx+dy*dy);
   const float alpha=clamp((radius+feather*.5f-distance)/feather,0.f,1.f);
   const size_t p=(size_t(y)*pixels+x)*4;
   rgba[p]=rgba[p+1]=rgba[p+2]=255;rgba[p+3]=static_cast<unsigned char>(alpha*255.f+.5f);
  }
  if(!nvg_gfx_lifecycle::updateOwnedNvgImageRgba(m.owner,m.image,m.width,m.height,vg,pixels,pixels,0,rgba.data()))return nullptr;
  ++builds;return &m;
 }
};
inline std::array<Cache,8>& caches(){static std::array<Cache,8> value;return value;}
inline Cache* findCache(NVGcontext* vg,bool create){
 if(!vg)return nullptr;
 for(auto& c:caches())if(c.context==vg)return &c;
 if(create)for(auto& c:caches())if(!c.context){c.context=vg;return &c;}
 // Several DAW windows may alternate contexts. Keep their images independent;
 // never discard an old context's handles merely because another window drew.
 return nullptr;
}
inline unsigned imageCount(){unsigned total=0;for(auto& c:caches())total+=unsigned(c.count);return total;}
inline void draw(NVGcontext* vg,const Mask& mask,float cx,float cy,NVGcolor color){
 NVGpaint p=nvgImagePattern(vg,cx-mask.extent,cy-mask.extent,2*mask.extent,2*mask.extent,0,mask.image,1);
 p.innerColor=p.outerColor=color;
 nvgBeginPath(vg);nvgRect(vg,cx-mask.extent,cy-mask.extent,2*mask.extent,2*mask.extent);
 nvgFillPaint(vg,p);nvgFill(vg);
}
}
