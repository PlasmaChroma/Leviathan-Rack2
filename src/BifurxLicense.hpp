#pragma once

// The normal Leviathan build has no DRM include, symbols, or runtime checks.
#if defined(LEVIATHAN_PRO_DRM) && LEVIATHAN_PRO_DRM
#ifdef DRM_DISABLE
#error "Premium validation and release builds must use real VCV DRM"
#endif
#include <rack.hpp>
#include <drm.hpp>

extern rack::drm::Context* leviathanDrmContext;

namespace bifurx {
inline bool isLicenseVerified() {
	return leviathanDrmContext && leviathanDrmContext->isModelVerified("Bifurx");
}
} // namespace bifurx
#endif
