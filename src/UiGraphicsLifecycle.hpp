#pragma once

struct NVGcontext;

namespace ui_gfx_lifecycle {

bool resetOwnedNvgImage(NVGcontext*& ownerVg,
                        int& handle,
                        int& cachedWidth,
                        int& cachedHeight,
                        NVGcontext* currentVg,
                        bool deleteCurrentHandle);

bool ownedNvgImageSizeMatches(NVGcontext* currentVg, int handle, int expectedWidth, int expectedHeight);

bool clearCacheOnContextSwitch(NVGcontext* currentVg, NVGcontext*& activeVg, unsigned long long* useCounter);

}  // namespace ui_gfx_lifecycle

