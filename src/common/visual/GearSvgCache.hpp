#pragma once
#include "Eclipse2RuntimeBake.hpp"
#include "../render/NvgGraphicsLifecycle.hpp"
#include "../render/GlResourceRetirement.hpp"
#include <array>
#include <map>

namespace gear_svg_cache {
// Bake with Rack's SVG renderer so its gradients match the vector fallback.
// Rotate the resulting image through NanoVG; never rotate a FramebufferWidget.
struct Raster {
    std::shared_ptr<window::Svg> svg;
    eclipse2_bake::Raster pixels;
    struct Image {
        NVGcontext* owner=nullptr;
        NVGcontext* window=nullptr;
        int handle=-1,width=0,height=0;
        gl_lifecycle::ContextLease context;
        void clear() {
            gl_lifecycle::retireObject(context,gl_lifecycle::ObjectKind::NvgImage,handle>0?handle:0);
            nvg_gfx_lifecycle::resetOwnedNvgImage(owner,handle,width,height,nullptr,false);
            context.reset();window=nullptr;
        }
        ~Image(){clear();}
    };
    std::array<Image,8> images;
    explicit Raster(std::shared_ptr<window::Svg> svg):svg(svg) {}
    bool prepare() {
        if(!pixels.rgba.empty())return true;
        if(!svg || !svg->handle)return false;
        const Vec size=svg->getSize();
        if(size.x<1 || size.y<1 || size.x>128 || size.y>128)return false;
        widget::SvgWidget source;source.setSvg(svg);
        return pixels.prepareSized([](const Widget::DrawArgs& args,void* user) {
            static_cast<widget::SvgWidget*>(user)->draw(args);
        },&source,size,4.f);
    }
    int imageFor(NVGcontext* vg) {
        if(!vg || !APP || !APP->window || !prepare())return -1;
        Image* selected=nullptr;
        for(auto& image:images)if(image.owner==vg){selected=&image;break;}
        if(!selected)for(auto& image:images) {
            if(!image.owner || !gl_lifecycle::resourceContextMatches(image.context,image.owner)) {
                image.clear();selected=&image;break;
            }
        }
        if(!selected)return -1;
        auto& image=*selected;
        const Vec size=svg->getSize();
        const int width=int(size.x*4),height=int(size.y*4);
        if(!gl_lifecycle::resourceContextMatches(image.context,vg)) {
            image.clear();image.context=gl_lifecycle::acquireResourceContext(vg);
        }
        if(image.owner==vg && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg,image.handle,width,height))
            return image.handle;
        image.window=APP->window->vg;
        if(!nvg_gfx_lifecycle::updateOwnedNvgImageRgba(image.owner,image.handle,image.width,image.height,
                vg,width,height,NVG_IMAGE_PREMULTIPLIED,pixels.rgba.data()))return -1;
        return image.handle;
    }
    void forgetWindow(NVGcontext* vg) {
        for(auto& image:images)if(image.owner==vg || image.window==vg)image.clear();
        pixels.retryAfterContextCreate();
    }
};
inline std::shared_ptr<Raster> rasterFor(const std::shared_ptr<window::Svg>& svg) {
    static std::map<window::Svg*,std::weak_ptr<Raster>> shared;
    for(auto it=shared.begin();it!=shared.end();) {
        if(it->second.expired())it=shared.erase(it);else ++it;
    }
    if(auto found=shared[svg.get()].lock())return found;
    auto result=std::make_shared<Raster>(svg);shared[svg.get()]=result;return result;
}
struct Layer : widget::SvgWidget {
    bool* enabled=nullptr;
    std::shared_ptr<Raster> raster;
    explicit Layer(bool* enabled):enabled(enabled) {}
    void draw(const DrawArgs& args) override {
        if(enabled && *enabled && svg && box.size==svg->getSize()) {
            if(!raster || raster->svg!=svg)raster=rasterFor(svg);
            const int image=raster->imageFor(args.vg);
            if(image>0) {
                nvgBeginPath(args.vg);nvgRect(args.vg,0,0,box.size.x,box.size.y);
                nvgFillPaint(args.vg,nvgImagePattern(args.vg,0,0,box.size.x,box.size.y,0,image,1));
                nvgFill(args.vg);return;
            }
        }
        SvgWidget::draw(args);
    }
    void onContextCreate(const ContextCreateEvent& e) override {
        if(raster)raster->forgetWindow(e.vg);
        SvgWidget::onContextCreate(e);
    }
    void onContextDestroy(const ContextDestroyEvent& e) override {
        if(raster)raster->forgetWindow(e.vg);
        SvgWidget::onContextDestroy(e);
    }
};
}
