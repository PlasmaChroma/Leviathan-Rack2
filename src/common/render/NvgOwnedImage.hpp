#pragma once

#include "NvgGraphicsLifecycle.hpp"
#include "GlResourceRetirement.hpp"

namespace nvg_gfx_lifecycle {

// UI-thread ownership. Destruction queues retirement; only the scene service
// touches the owning context, after the frame that may still reference the image.
class OwnedImage {
    gl_lifecycle::ContextLease lease_;
    NVGcontext* owner_ = nullptr;
    int handle_ = -1, width_ = 0, height_ = 0;
public:
    OwnedImage() = default;
    OwnedImage(const OwnedImage&) = delete;
    OwnedImage& operator=(const OwnedImage&) = delete;
    ~OwnedImage() { reset(); }

    void reset() {
        if (handle_ > 0)
            gl_lifecycle::retireObject(lease_, gl_lifecycle::ObjectKind::NvgImage, GLuint(handle_));
        resetOwnedNvgImage(owner_, handle_, width_, height_, nullptr, false);
        lease_.reset();
    }

    int ensure(NVGcontext* vg, int width, int height, int flags, const unsigned char* pixels) {
        if (!vg || !pixels || width <= 0 || height <= 0) return -1;
        if (gl_lifecycle::resourceContextMatches(lease_, vg)
            && width_ == width && height_ == height
            && ownedNvgImageSizeMatches(vg, handle_, width, height)) return handle_;
        reset();
        lease_ = gl_lifecycle::acquireResourceContext(vg);
        if (!lease_) return -1;
        return updateOwnedNvgImageRgba(owner_, handle_, width_, height_, vg,
            width, height, flags, pixels) ? handle_ : -1;
    }
};
} // namespace nvg_gfx_lifecycle
