#pragma once

struct NVGcontext;

namespace nvg_gfx_lifecycle {

bool resetOwnedNvgImage(NVGcontext*& ownerVg,
                        int& handle,
                        int& cachedWidth,
                        int& cachedHeight,
                        NVGcontext* currentVg,
                        bool deleteCurrentHandle);

bool ownedNvgImageSizeMatches(NVGcontext* currentVg, int handle, int expectedWidth, int expectedHeight);

bool updateOwnedNvgImageRgba(NVGcontext*& ownerVg,
                             int& handle,
                             int& cachedWidth,
                             int& cachedHeight,
                             NVGcontext* currentVg,
                             int width,
                             int height,
                             int imageFlags,
                             const unsigned char* rgbaPixels);

bool clearCacheOnContextSwitch(NVGcontext* currentVg, NVGcontext*& activeVg, unsigned long long* useCounter);

}  // namespace nvg_gfx_lifecycle
