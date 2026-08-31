#include "plugin.hpp"
#include "NvgGraphicsLifecycle.hpp"

namespace nvg_gfx_lifecycle {

bool resetOwnedNvgImage(NVGcontext*& ownerVg,
                        int& handle,
                        int& cachedWidth,
                        int& cachedHeight,
                        NVGcontext* currentVg,
                        bool deleteCurrentHandle) {
  if (deleteCurrentHandle && currentVg && ownerVg == currentVg && handle >= 0) {
    nvgDeleteImage(currentVg, handle);
  }
  ownerVg = nullptr;
  handle = -1;
  cachedWidth = 0;
  cachedHeight = 0;
  return true;
}

bool ownedNvgImageSizeMatches(NVGcontext* currentVg, int handle, int expectedWidth, int expectedHeight) {
  if (!currentVg || handle < 0) {
    return false;
  }
  int currentW = 0;
  int currentH = 0;
  nvgImageSize(currentVg, handle, &currentW, &currentH);
  return currentW == expectedWidth && currentH == expectedHeight;
}

bool updateOwnedNvgImageRgba(NVGcontext*& ownerVg,
                             int& handle,
                             int& cachedWidth,
                             int& cachedHeight,
                             NVGcontext* currentVg,
                             int width,
                             int height,
                             int imageFlags,
                             const unsigned char* rgbaPixels) {
  if (!currentVg || width <= 0 || height <= 0 || !rgbaPixels) {
    resetOwnedNvgImage(
      ownerVg, handle, cachedWidth, cachedHeight, currentVg,
      currentVg && ownerVg == currentVg);
    return false;
  }
  if (ownerVg != currentVg) {
    resetOwnedNvgImage(ownerVg, handle, cachedWidth, cachedHeight, currentVg, false);
    ownerVg = currentVg;
  }
  if (handle >= 0 && cachedWidth == width && cachedHeight == height &&
      ownedNvgImageSizeMatches(currentVg, handle, width, height)) {
    nvgUpdateImage(currentVg, handle, rgbaPixels);
    return true;
  }
  resetOwnedNvgImage(ownerVg, handle, cachedWidth, cachedHeight, currentVg, true);
  ownerVg = currentVg;
  handle = nvgCreateImageRGBA(currentVg, width, height, imageFlags, rgbaPixels);
  if (handle < 0) return false;
  cachedWidth = width;
  cachedHeight = height;
  return true;
}

bool clearCacheOnContextSwitch(NVGcontext* currentVg, NVGcontext*& activeVg, unsigned long long* useCounter) {
  if (activeVg == currentVg) {
    return false;
  }
  activeVg = currentVg;
  if (useCounter) {
    *useCounter = 0ull;
  }
  return true;
}

}  // namespace nvg_gfx_lifecycle
