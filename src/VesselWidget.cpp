#include "Vessel.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/PlasmaConduit.hpp"
#include "visual/VisualAssets.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <nanosvgrast.h>

namespace {
// Match MALLET_PARAM's persistent ordering.
const char* const vesselMalletPaths[] = {
    "res/Vessel/PureWoodMallet.png", "res/Vessel/Suede.png",
    "res/Vessel/Silicone.png", "res/Vessel/Felt.png"
};
const char* const vesselMalletNames[] = {"Wood", "Suede", "Silicone", "Felt"};

NVGcolor mixPitchTint(NVGcolor a, NVGcolor b, float amount) {
    amount = clamp(amount, 0.f, 1.f);
    return nvgRGBAf(
        a.r + (b.r - a.r) * amount,
        a.g + (b.g - a.g) * amount,
        a.b + (b.b - a.b) * amount,
        1.f);
}

NVGcolor vesselPitchTint(float frequency) {
    // Aparmita's original A4=440 color-derived bands, represented by their
    // geometric centers. Fold by octaves, then interpolate between centers so
    // pitch sweeps remain continuous instead of stepping across seven colors.
    static const float centers[] = {100.25f, 112.50f, 119.32f, 127.25f, 141.72f, 155.71f, 171.21f};
    static const NVGcolor colors[] = {
        nvgRGB(255, 56, 72),   // Root
        nvgRGB(255, 124, 35),  // Sacral
        nvgRGB(255, 220, 50),  // Solar plexus
        nvgRGB(40, 224, 110),  // Heart
        nvgRGB(50, 180, 255),  // Throat
        nvgRGB(80, 90, 245),   // Third eye
        nvgRGB(200, 70, 235)   // Crown
    };
    float folded = std::isfinite(frequency) ? std::max(frequency, 1.f) : 261.625565f;
    while (folded < 92.09f) folded *= 2.f;
    while (folded >= 184.17f) folded *= 0.5f;
    if (folded < centers[0]) {
        const float previousCenter = centers[6] * 0.5f;
        return mixPitchTint(colors[6], colors[0], (folded - previousCenter) / (centers[0] - previousCenter));
    }
    for (int i = 0; i < 6; ++i) {
        if (folded < centers[i + 1])
            return mixPitchTint(colors[i], colors[i + 1], (folded - centers[i]) / (centers[i + 1] - centers[i]));
    }
    const float nextRootCenter = centers[0] * 2.f;
    return mixPitchTint(colors[6], colors[0], (folded - centers[6]) / (nextRootCenter - centers[6]));
}

struct VesselTintMask final : TransparentWidget {
    std::string sourcePath;
    std::vector<std::uint8_t> pixels;
    NVGcontext* imageVg = nullptr;
    int imageHandle = -1;
    int cachedWidth = 0;
    int cachedHeight = 0;
    int sourceWidth = 0;
    int sourceHeight = 0;
    bool decodeAttempted = false;

    VesselTintMask(math::Rect rectMm, const char* path) : sourcePath(path ? path : "") {
        box.pos = mm2px(rectMm.pos);
        box.size = mm2px(rectMm.size);
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(
            imageVg, imageHandle, cachedWidth, cachedHeight, e.vg, imageVg == e.vg);
        TransparentWidget::onContextDestroy(e);
    }

    bool ensurePixels() {
        if (decodeAttempted) return !pixels.empty();
        decodeAttempted = true;
        int width = 0;
        int height = 0;
        if (!visual_assets::decodeRasterRgba8(
                asset::plugin(pluginInstance, sourcePath),
                &pixels, &width, &height)) return false;
        sourceWidth = width;
        sourceHeight = height;
        for (std::size_t i = 0; i + 3 < pixels.size(); i += 4) {
            const int luminance = (54 * int(pixels[i]) + 183 * int(pixels[i + 1])
                + 19 * int(pixels[i + 2]) + 128) >> 8;
            pixels[i] = pixels[i + 1] = pixels[i + 2] = std::uint8_t(luminance);
        }
        return true;
    }

    bool ensureImage(NVGcontext* vg) {
        if (!ensurePixels()) return false;
        if (imageVg == vg && imageHandle > 0
            && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, imageHandle, sourceWidth, sourceHeight)) return true;
        return nvg_gfx_lifecycle::updateOwnedNvgImageRgba(
            imageVg, imageHandle, cachedWidth, cachedHeight, vg,
            sourceWidth, sourceHeight, NVG_IMAGE_GENERATE_MIPMAPS, pixels.data());
    }

    void draw(const DrawArgs& args) override {
        if (!ensureImage(args.vg) || sourceWidth <= 0 || sourceHeight <= 0) return;
        const float aspect = float(sourceWidth) / float(sourceHeight);
        float drawWidth = box.size.x;
        float drawHeight = drawWidth / aspect;
        if (drawHeight > box.size.y) {
            drawHeight = box.size.y;
            drawWidth = drawHeight * aspect;
        }
        const float x = 0.5f * (box.size.x - drawWidth);
        const float y = 0.5f * (box.size.y - drawHeight);
        nvgBeginPath(args.vg);
        nvgRect(args.vg, x, y, drawWidth, drawHeight);
        nvgFillPaint(args.vg, nvgImagePattern(
            args.vg, x, y, drawWidth, drawHeight, 0.f, imageHandle, 1.f));
        nvgFill(args.vg);
    }
};

struct VesselPitchTintLayer final : TransparentWidget {
    Vessel* vessel = nullptr;
    Widget* metalRaster = nullptr;
    Widget* crystalRaster = nullptr;
    bool crystalSelected = false;
    NVGcolor displayedTint = vesselPitchTint(261.625565f);
    bool tintInitialized = false;

    VesselPitchTintLayer(
        Vessel* module,
        math::Rect metalRasterRectMm,
        math::Rect crystalRasterRectMm) : vessel(module) {
        metalRaster = new VesselTintMask(metalRasterRectMm, "res/Vessel/Metal-Crop-Only.png");
        crystalRaster = new VesselTintMask(crystalRasterRectMm, "res/Vessel/Crystal-Crop-Only.png");
        addChild(metalRaster);
        addChild(crystalRaster);
    }

    void step() override {
        const float frequency = vessel
            ? vessel->visualFrequency.load(std::memory_order_relaxed)
            : 261.625565f;
        const NVGcolor targetTint = vesselPitchTint(frequency);
        if (!tintInitialized) {
            displayedTint = targetTint;
            tintInitialized = true;
        }
        else {
            const float frameTime = APP && APP->window
                ? clamp(float(APP->window->getLastFrameDuration()), 0.f, 0.1f)
                : 1.f / 60.f;
            // Match Puffy's character-tint easing: about 90% settled in 0.45s.
            // Blend the color itself so octave jumps don't sweep unrelated hues.
            displayedTint = mixPitchTint(displayedTint, targetTint, 5.f * frameTime);
        }
        crystalSelected = vessel && vessel->params[Vessel::BOWL_PARAM].getValue() >= .5f;
        if (metalRaster) metalRaster->setVisible(!crystalSelected);
        if (crystalRaster) crystalRaster->setVisible(crystalSelected);
        TransparentWidget::step();
    }

    void draw(const DrawArgs& args) override {
        nvgSave(args.vg);
        nvgGlobalCompositeOperation(args.vg, NVG_SOURCE_OVER);
        nvgGlobalAlpha(args.vg, crystalSelected ? 0.20f : 0.38f);
        nvgGlobalTint(args.vg, displayedTint);
        TransparentWidget::draw(args);
        nvgRestore(args.vg);
    }
};

struct VesselMalletPixels {
    struct Image {
        std::vector<std::uint8_t> rgba;
        int width = 0, height = 0;
    };
    std::array<Image, 4> images;

    VesselMalletPixels() {
        window::Svg emblem;
        emblem.loadFile(asset::plugin(pluginInstance, "res/icon/Vahdrim'Keth.svg"));
        if (emblem.handle) {
            for (NSVGshape* shape = emblem.handle->shapes; shape; shape = shape->next) {
                if (shape->fill.type == NSVG_PAINT_COLOR) shape->fill.color = 0xff000000u;
                if (shape->stroke.type == NSVG_PAINT_COLOR) shape->stroke.color = 0xff000000u;
            }
        }
        NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
        for (int i = 0; i < 4; ++i) {
            auto& image = images[i];
            if (!visual_assets::decodeRasterRgba8(asset::plugin(pluginInstance, vesselMalletPaths[i]),
                    &image.rgba, &image.width, &image.height)) continue;
            if (!rasterizer || !emblem.handle || emblem.handle->width <= 0.f) continue;
            const float scale = .7f * image.width / emblem.handle->width;
            const float x = .5f * (image.width - emblem.handle->width * scale);
            const float y = (i == 3 ? .33f : .5f) * image.height - .5f * emblem.handle->height * scale;
            std::vector<unsigned char> rune(image.rgba.size(), 0);
            nsvgRasterize(rasterizer, emblem.handle, x, y, scale,
                rune.data(), image.width, image.height, image.width * 4);
            // Black source-over, retaining straight alpha for NanoVG upload.
            for (std::size_t p = 0; p < rune.size(); p += 4) {
                const unsigned sa = rune[p + 3], da = image.rgba[p + 3];
                const unsigned retained = da * (255 - sa);
                const unsigned alpha = sa * 255 + retained;
                if (!alpha) continue;
                for (int c = 0; c < 3; ++c)
                    image.rgba[p + c] = std::uint8_t((image.rgba[p + c] * retained + alpha / 2) / alpha);
                image.rgba[p + 3] = std::uint8_t((alpha + 127) / 255);
            }
        }
        if (rasterizer) nsvgDeleteRasterizer(rasterizer);
    }
};

struct VesselMalletImages {
    struct Texture {
        NVGcontext* owner = nullptr;
        int handle = -1, width = 0, height = 0;
    };
    // CPU composites are prepared once on the UI thread and survive window recreation.
    const VesselMalletPixels& pixels() const {
        static const VesselMalletPixels cached;
        return cached;
    }
    std::map<NVGcontext*, std::array<Texture, 4>> textures;

    int imageHandle(NVGcontext* vg, int index) {
        const auto& image = pixels().images[index];
        if (image.rgba.empty()) return -1;
        auto& texture = textures[vg][index];
        if (texture.owner == vg && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
                vg, texture.handle, image.width, image.height)) return texture.handle;
        return nvg_gfx_lifecycle::updateOwnedNvgImageRgba(texture.owner, texture.handle,
            texture.width, texture.height, vg, image.width, image.height,
            NVG_IMAGE_GENERATE_MIPMAPS, image.rgba.data()) ? texture.handle : -1;
    }
    void resetContext(NVGcontext* vg, bool destroy) {
        // Rack's main-context event also retires its framebuffer context.
        for (auto& context : textures) for (auto& texture : context.second)
            nvg_gfx_lifecycle::resetOwnedNvgImage(texture.owner, texture.handle,
                texture.width, texture.height, vg, destroy && texture.owner == vg);
        textures.clear();
    }
};

struct VesselMalletLink {
    Vessel* module = nullptr;
    VesselMalletImages images;
    math::Rect orbit;
    math::Rect bowl;
    float strikePadHeight = 0.f;
    // Shared UI envelope keeps the front/back passes in sync through release.
    float rubFade = 0.f;
    // Temporary, UI-only tuning shared by the front/back animation passes.
    std::shared_ptr<float> orbitWidthScale = std::make_shared<float>(1.f);

    VesselMalletLink() { images.pixels(); }
};

struct VesselOrbitWidthQuantity final : Quantity {
    std::shared_ptr<float> scale;
    float defaultWidthMm;

    VesselOrbitWidthQuantity(std::shared_ptr<float> scale, float defaultWidthMm)
        : scale(scale), defaultWidthMm(defaultWidthMm) {}
    void setValue(float value) override {
        if (std::isfinite(value)) *scale = clamp(value, .5f, 1.5f);
    }
    float getValue() override { return *scale; }
    float getMinValue() override { return .5f; }
    float getMaxValue() override { return 1.5f; }
    float getDefaultValue() override { return 1.f; }
    float getDisplayValue() override { return getValue() * defaultWidthMm; }
    void setDisplayValue(float value) override { setValue(value / defaultWidthMm); }
    std::string getDisplayValueString() override { return string::f("%.1f", getDisplayValue()); }
    std::string getLabel() override { return "Orbit width"; }
    std::string getUnit() override { return " mm"; }
};

struct VesselOrbitWidthSlider final : ui::Slider {
    VesselOrbitWidthQuantity widthQuantity;

    VesselOrbitWidthSlider(std::shared_ptr<float> scale, float defaultWidthMm)
        : widthQuantity(scale, defaultWidthMm) {
        quantity = &widthQuantity;
        box.size = Vec(240.f, 24.f);
    }
};

constexpr float kVesselMalletHeightMm = 34.f;
constexpr float kVesselMalletTipFraction = .12f;
constexpr float kVesselMalletTipPenetration = .20f;
constexpr float kVesselMalletContactOffsetMm = 7.9f;

// Offline samples of 1 - (2^(6 * progress) - 1) / 63.
constexpr float kVesselStrikeFade[] = {
    1.00000000f, 0.99779701f, 0.99528826f, 0.99243133f, 0.98917789f, 0.98547291f,
    0.98125372f, 0.97644896f, 0.97097735f, 0.96474634f, 0.95765054f, 0.94956992f,
    0.94036780f, 0.92988854f, 0.91795487f, 0.90436494f, 0.88888889f, 0.87126494f,
    0.85119498f, 0.82833954f, 0.80231202f, 0.77267218f, 0.73891867f, 0.70048056f,
    0.65670767f, 0.60685960f, 0.55009318f, 0.48544824f, 0.41183131f, 0.32799718f,
    0.23252783f, 0.12380843f, 0.00000000f
};

float vesselStrikeFade(float progress) {
    const float position = clamp(progress, 0.f, 1.f) * 32.f;
    const int index = std::min(int(position), 31);
    const float fraction = position - float(index);
    return kVesselStrikeFade[index]
        + fraction * (kVesselStrikeFade[index + 1] - kVesselStrikeFade[index]);
}

struct VesselMalletRenderWidget : TransparentWidget {
    VesselMalletLink* link = nullptr;
    bool frontPass = false;

    VesselMalletRenderWidget(VesselMalletLink* link, bool frontPass)
        : link(link), frontPass(frontPass) {}

    void drawMallet(const DrawArgs& args) {
        if (!link || !link->module || !APP || !APP->window) return;
        const float aftermath = clamp(
            link->module->visualStrikeAftermath.load(std::memory_order_relaxed), 0.f, 1.f);
        const bool rubbing = link->module->visualRubbing.load(std::memory_order_relaxed)
            || (link->rubFade > 0.f && aftermath <= 0.f);
        if (!rubbing && (aftermath <= 0.f || !frontPass)) return;
        float angle = link->module->visualRotationAngle.load(std::memory_order_relaxed);
        if (!std::isfinite(angle)) angle = 0.f;
        const float depth = std::sin(angle);
        if (rubbing && frontPass != (depth >= 0.f)) return;

        const int mallet = clamp(int(std::round(link->module->params[Vessel::MALLET_PARAM].getValue())), 0, 3);
        const int handle = link->images.imageHandle(args.vg, mallet);
        if (handle <= 0) return;
        const auto& image = link->images.pixels().images[mallet];
        const int imageWidth = image.width, imageHeight = image.height;
        if (imageWidth <= 0 || imageHeight <= 0) return;

        const Vec center = link->orbit.pos.plus(link->orbit.size.mult(.5f));
        const Vec radius = link->orbit.size.mult(.5f);
        const Vec contact = center.plus(Vec(
            radius.x * *link->orbitWidthScale * std::cos(angle),
            radius.y * depth + mm2px(kVesselMalletContactOffsetMm)));
        const float drawHeight = mm2px(kVesselMalletHeightMm);
        const float drawWidth = drawHeight * float(imageWidth) / float(imageHeight);
        const float tipPenetration = drawHeight
            * kVesselMalletTipFraction * kVesselMalletTipPenetration;
        Vec renderedTip = contact.plus(Vec(0.f, tipPenetration));
        float tilt = 0.f;
        // Smoothstep needs no transcendental work and reverses cleanly on retrigger.
        float opacity = link->rubFade * link->rubFade * (3.f - 2.f * link->rubFade);
        const float imageY = -drawHeight;
        if (!rubbing) {
            const float elapsed = (1.f - aftermath)
                * (Vessel::strikeApproachSeconds + Vessel::strikeReboundSeconds);
            const float progress = clamp(
                (elapsed - Vessel::strikeApproachSeconds) / Vessel::strikeReboundSeconds,
                0.f, 1.f);
            // A fast initial rebound that slows as the mallet disappears.
            float retreat = progress * (2.f - progress);
            if (elapsed < Vessel::strikeApproachSeconds) {
                // Brief visual anticipation after the audio trigger: accelerate
                // from the raised pose into contact before rebounding.
                const float approach = elapsed / Vessel::strikeApproachSeconds;
                retreat = 1.f - approach * approach;
            }
            renderedTip = Vec(
                link->bowl.pos.x + .08f * link->bowl.size.x,
                link->bowl.pos.y + .56f * link->bowl.size.y + .20f * link->strikePadHeight
                    - mm2px(5.f * retreat));
            tilt = .5877335f - .25f * retreat;
            // An accelerating exponential fade: retain most opacity early,
            // then drop rapidly to zero as the rebound finishes.
            opacity = vesselStrikeFade(progress);
        }

        // Draw the artwork in its normal orientation above the contact endpoint.
        // Only the requested part of the contact tip penetrates below the rim path.
        nvgSave(args.vg);
        nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
        nvgTranslate(args.vg, renderedTip.x, renderedTip.y);
        nvgRotate(args.vg, tilt);
        if (!rubbing) {
            // Flip artwork and rune about their shared center without moving
            // the strike animation's original footprint.
            const float centerY = imageY + .5f * drawHeight;
            nvgTranslate(args.vg, 0.f, centerY);
            nvgScale(args.vg, -1.f, -1.f);
            nvgTranslate(args.vg, 0.f, -centerY);
        }
        const NVGpaint paint = nvgImagePattern(
            args.vg, -.5f * drawWidth, imageY, drawWidth, drawHeight, 0.f, handle, opacity);
        nvgBeginPath(args.vg);
        nvgRect(args.vg, -.5f * drawWidth, imageY, drawWidth, drawHeight);
        nvgFillPaint(args.vg, paint);
        nvgFill(args.vg);
        nvgRestore(args.vg);
    }

    void draw(const DrawArgs& args) override {
        drawMallet(args);
    }
};

struct VesselMalletSelector final : app::ParamWidget {
    int hovered = -1;
    VesselMalletImages* images = nullptr;

    math::Rect cell(int index) const {
        const float gap = mm2px(1.f);
        const Vec size = box.size.minus(Vec(gap, gap)).mult(.5f);
        return math::Rect(Vec((index % 2) * (size.x + gap),
            (index / 2) * (size.y + gap)), size);
    }
    int cellAt(Vec pos) const {
        for (int i = 0; i < 4; ++i)
            if (cell(i).contains(pos)) return i;
        return -1;
    }
    void onHover(const event::Hover& e) override {
        hovered = cellAt(e.pos);
        app::ParamWidget::onHover(e);
    }
    void onLeave(const event::Leave& e) override {
        hovered = -1;
        app::ParamWidget::onLeave(e);
    }
    void onButton(const event::Button& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
            app::ParamWidget::onButton(e);
            return;
        }
        const int selected = cellAt(e.pos);
        if (e.action == GLFW_PRESS && selected >= 0) {
            if (auto* quantity = getParamQuantity()) {
                const float previous = quantity->getValue();
                quantity->setValue(float(selected));
                if (previous != quantity->getValue() && APP && APP->history) {
                    auto* action = new history::ParamChange;
                    action->name = "select mallet";
                    action->moduleId = module->id;
                    action->paramId = paramId;
                    action->oldValue = previous;
                    action->newValue = quantity->getValue();
                    APP->history->push(action);
                }
            }
        }
        e.consume(this);
    }
    // A second click keeps the chosen tile instead of resetting the parameter.
    void onDoubleClick(const event::DoubleClick& e) override { e.consume(this); }
    void draw(const DrawArgs& args) override {
        auto* quantity = getParamQuantity();
        const int selected = quantity ? clamp(int(std::round(quantity->getValue())), 0, 3) : 1;
        for (int i = 0; i < 4; ++i) {
            const auto r = cell(i);
            const bool active = selected == i;
            const bool hover = hovered == i;
            const NVGcolor color = active ? nvgRGB(74, 222, 214) : nvgRGB(163, 113, 245);
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, r.pos.x + 1.f, r.pos.y + 1.f,
                r.size.x - 2.f, r.size.y - 2.f, mm2px(1.2f));
            nvgFillColor(args.vg, nvgTransRGBA(color, active ? 32 : hover ? 26 : 9));
            nvgFill(args.vg);
            if (active || hover) {
                nvgStrokeWidth(args.vg, 4.f);
                nvgStrokeColor(args.vg, nvgTransRGBA(color, hover ? 48 : 30));
                nvgStroke(args.vg);
            }
            nvgStrokeWidth(args.vg, active ? 1.4f : .8f);
            nvgStrokeColor(args.vg, nvgTransRGBA(color, active ? 230 : hover ? 135 : 45));
            nvgStroke(args.vg);
            const int handle = images ? images->imageHandle(args.vg, i) : -1;
            if (handle > 0) {
                const auto& image = images->pixels().images[i];
                const Vec available = r.size.minus(mm2px(Vec(2.f, 4.f)));
                const float scale = std::max(0.f, std::min(available.x / image.width, available.y / image.height));
                const Vec size(image.width * scale, image.height * scale);
                const Vec pos = r.pos.plus(mm2px(Vec(1.f, 1.f))).plus(available.minus(size).mult(.5f));
                nvgBeginPath(args.vg);
                nvgRect(args.vg, pos.x, pos.y, size.x, size.y);
                nvgFillPaint(args.vg, nvgImagePattern(args.vg, pos.x, pos.y, size.x, size.y, 0.f, handle, 1.f));
                nvgFill(args.vg);
            }
            if (APP && APP->window && APP->window->uiFont) {
                nvgFontFaceId(args.vg, APP->window->uiFont->handle);
                nvgFontSize(args.vg, mm2px(1.8f));
                nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
                nvgFillColor(args.vg, active ? nvgRGB(194, 247, 243) : nvgRGB(191, 191, 210));
                nvgText(args.vg, r.pos.x + .5f * r.size.x,
                    r.pos.y + r.size.y - mm2px(1.6f), vesselMalletNames[i], nullptr);
            }
        }
        app::ParamWidget::draw(args);
    }
};

struct VesselPerformanceArea : app::Switch {
    enum class Kind { Strike, Rotate };
    Kind kind;
    ui::Tooltip* padTooltip = nullptr;
    float currentAmount = 1.f;
    float dragY = 0.f;
    bool hovered = false;

    explicit VesselPerformanceArea(Kind kind) : kind(kind) { momentary = true; }
    ~VesselPerformanceArea() override { destroyPadTooltip(); }

    float amountAt(float y) const {
        const float topSaturationMargin = mm2px(3.f);
        const float activeHeight = std::max(box.size.y - topSaturationMargin, 1e-6f);
        const float amount = 1.f - clamp((y - topSaturationMargin) / activeHeight, 0.f, 1.f);
        // Both pads use linear vertical travel; intensity shaping lives in DSP.
        return amount;
    }
    void publishAmount(float y) {
        dragY = clamp(y, 0.f, box.size.y);
        currentAmount = amountAt(dragY);
        if (auto* vessel = dynamic_cast<Vessel*>(module)) {
            if (kind == Kind::Strike)
                vessel->manualStrikeVelocity.store(currentAmount, std::memory_order_relaxed);
            else
                vessel->manualRubIntensity.store(currentAmount, std::memory_order_relaxed);
        }
        refreshPadTooltip();
    }
    std::string tooltipText() const {
        const int percent = int(std::lround(100.f * currentAmount));
        return kind == Kind::Strike
            ? string::f("Strike: %d%% Velocity", percent)
            : string::f("Rub: %d%% Intensity", percent);
    }
    void createPadTooltip() {
        if (!settings::tooltips || padTooltip || !APP || !APP->scene) return;
        padTooltip = new ui::Tooltip();
        padTooltip->text = tooltipText();
        APP->scene->addChild(padTooltip);
    }
    void destroyPadTooltip() {
        if (!padTooltip) return;
        if (padTooltip->parent) padTooltip->parent->removeChild(padTooltip);
        delete padTooltip;
        padTooltip = nullptr;
    }
    void refreshPadTooltip() {
        if (padTooltip) padTooltip->text = tooltipText();
    }
    void onHover(const event::Hover& e) override {
        publishAmount(e.pos.y);
        app::Switch::onHover(e);
    }
    void onButton(const event::Button& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;
        if (e.action == GLFW_PRESS) publishAmount(e.pos.y);
        app::Switch::onButton(e);
    }
    void onDragMove(const event::DragMove& e) override {
        const float zoom = std::max(getAbsoluteZoom(), 1e-6f);
        publishAmount(dragY + e.mouseDelta.y / zoom);
        app::Switch::onDragMove(e);
    }
    void onEnter(const event::Enter& e) override {
        hovered = true;
        app::Switch::onEnter(e);
        app::ParamWidget::destroyTooltip();
        createPadTooltip();
    }
    void onLeave(const event::Leave& e) override {
        hovered = false;
        destroyPadTooltip();
        app::Switch::onLeave(e);
    }
    void step() override {
        app::Switch::step();
        refreshPadTooltip();
    }
    void draw(const DrawArgs& args) override {
        const bool active = module && module->params[paramId].getValue() >= .5f;
        const NVGcolor color = kind == Kind::Strike ? nvgRGB(74, 222, 214) : nvgRGB(163, 113, 245);
        const float inset = mm2px(.35f);
        const float radius = mm2px(2.2f);
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, inset, inset,
            std::max(0.f, box.size.x - 2.f * inset), std::max(0.f, box.size.y - 2.f * inset), radius);
        nvgFillColor(args.vg, nvgTransRGBA(color, active ? 38 : hovered ? 24 : 14));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, active ? 1.35f : 0.9f);
        nvgStrokeColor(args.vg, nvgTransRGBA(color, active ? 130 : hovered ? 90 : 55));
        nvgStroke(args.vg);
    }
};
struct VesselStrikeArea final : VesselPerformanceArea {
    VesselStrikeArea() : VesselPerformanceArea(Kind::Strike) {}
};
struct VesselRotateArea final : VesselPerformanceArea {
    VesselRotateArea() : VesselPerformanceArea(Kind::Rotate) {}
};
struct BowlDisplay : TransparentWidget {
    Vessel* vessel = nullptr;
    void draw(const DrawArgs& args) override {
        const float energy = vessel ? vessel->visualEnergy.load(std::memory_order_relaxed) : .55f;
        const bool ringing = vessel && !vessel->visualSleeping.load(std::memory_order_relaxed)
            && vessel->rawEnergy.load(std::memory_order_relaxed) > 0.f;
        const float barHeight = mm2px(3.f);
        nvgBeginPath(args.vg); nvgRoundedRect(args.vg, 0, 0, box.size.x, barHeight, 2);
        nvgFillColor(args.vg, nvgRGB(7, 11, 19)); nvgFill(args.vg);
        nvgStrokeColor(args.vg, nvgRGBA(176, 141, 216, 125)); nvgStrokeWidth(args.vg, .7f); nvgStroke(args.vg);
        if (energy > 0.f || ringing) {
            // A dim residual-energy marker keeps quiet tails visible without
            // making mere DSP activity look like a fixed minimum energy level.
            const float scaledWidth = (box.size.x-2)*energy;
            const float width = std::max(scaledWidth, ringing ? 2.f : 0.f);
            const int alpha = ringing ? int(70.f+185.f*std::min(1.f,scaledWidth/2.f)) : 255;
            nvgBeginPath(args.vg); nvgRoundedRect(args.vg, 1, 1, width, barHeight-2, 1);
            nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, box.size.x, 0,
                nvgRGBA(163, 113, 245, alpha), nvgRGBA(74, 222, 214, alpha))); nvgFill(args.vg);
        }
        if (!APP || !APP->window || !APP->window->uiFont) return;
        const float hz = vessel ? vessel->visualFrequency.load(std::memory_order_relaxed) : 261.625565f;
        const bool fault = vessel && vessel->visualFault.load(std::memory_order_relaxed);
        const float delta = vessel ? vessel->visualSeparation.load(std::memory_order_relaxed) : 0.f;
        char text[48]; std::snprintf(text, sizeof(text), "L: %.1f Hz  R: %.1f Hz", hz-.5f*delta, hz+.5f*delta);
        nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFontSize(args.vg, 9.f);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg, fault ? nvgRGB(255, 133, 99) : nvgRGB(181, 213, 220));
        nvgText(args.vg, 1, mm2px(5.6f), fault ? "RATE / CONTACT FAULT" : "ENERGY", nullptr);
        if (!fault) {
            nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
            nvgText(args.vg, box.size.x-1, mm2px(5.6f), text, nullptr);
        }
    }
};
}

struct VesselWidget final : ModuleWidget {
    debug_terminal::BaselineWidgetMetrics timing;
    debug_terminal::UiCycleTimingAccumulator layerTiming;
    VesselMalletLink malletLink;
    Widget* metalBowlRaster = nullptr;
    Widget* crystalBowlRaster = nullptr;
    VesselPitchTintLayer* bowlPitchTint = nullptr;
    explicit VesselWidget(Vessel* module) {
        setModule(module);
        {
        visual_assets::SplitPanelRenderer panel(this, "res/Vessel.panel.svg");
        panel.addThemedLabels("res/Vessel.labels.svg", "res/Vessel.theme-text-input.svg", "res/Vessel.theme-text-output.svg");
        panel.addCompactLeviathanLogoBranding();
        panel.addPerfectWaveBranding();
        addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
        addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
        addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        if (widget::FramebufferWidget* conduits =
                visual_assets::createPlasmaConduitLayer(panel.panelPath(), box.size)) {
            addChild(conduits);
        }
        auto point = [&](const char* id, float x, float y) {
            Vec p; return panel_svg::loadPointFromSvgMm(panel.panelPath(), id, &p) ? p : Vec(x, y);
        };
        auto* display = new BowlDisplay(); display->vessel = module;
        math::Rect r(Vec(3.5f, 15.f), Vec(74.28f, 8.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ENERGY_DISPLAY", &r);
        display->box.pos = mm2px(r.pos); display->box.size = mm2px(r.size); addChild(display);
        math::Rect bowlRasterRect(Vec(6.64f, 29.89f), Vec(68.f, 40.22f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "BOWL_RASTER", &bowlRasterRect);
        const float bowlCenterY = bowlRasterRect.pos.y + .5f * bowlRasterRect.size.y;
        auto widthMatchedBowlRect = [&](float sourceAspect) {
            const float height = bowlRasterRect.size.x / sourceAspect;
            return math::Rect(
                Vec(bowlRasterRect.pos.x, bowlCenterY - .5f * height),
                Vec(bowlRasterRect.size.x, height));
        };
        const math::Rect metalBowlRect = widthMatchedBowlRect(1300.f / 770.f);
        const math::Rect crystalBowlRect = widthMatchedBowlRect(1295.f / 778.f);
        metalBowlRaster = visual_assets::createAspectFitRasterImageWidget(
            "res/Vessel/Metal-Crop-Only.png", metalBowlRect);
        crystalBowlRaster = visual_assets::createAspectFitRasterImageWidget(
            "res/Vessel/Crystal-Crop-Only.png", crystalBowlRect);
        math::Rect malletOrbitRect(Vec(3.39f, 29.5f), Vec(74.5f, 9.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "MALLET_ORBIT", &malletOrbitRect);
        malletLink.module = module;
        malletLink.orbit = math::Rect(mm2px(malletOrbitRect.pos), mm2px(malletOrbitRect.size));
        malletLink.bowl = math::Rect(mm2px(bowlRasterRect.pos), mm2px(bowlRasterRect.size));
        addChild(createLightCentered<SmallAperture<AmberGreenApertureLight>>(
            mm2px(point("VTUNE_EXPANDER_LIGHT", 78.08f, 5.8f)), module, Vessel::VTUNE_LINK_LIGHT));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("BINAURAL_PARAM", 12.f, 83.5f)), module, Vessel::BINAURAL_PARAM));
        addParam(createParamCentered<PlasmaSwitch>(mm2px(point("BOWL_PARAM", 27.f, 83.5f)), module, Vessel::BOWL_PARAM));
        addParam(createParamCentered<LeviathanHaloKnob2>(mm2px(point("PITCH_PARAM", 42.5f, 83.5f)), module, Vessel::PITCH_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("FINE_PARAM", 55.25f, 83.5f)), module, Vessel::FINE_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("LEVEL_PARAM", 22.f, 111.7f)), module, Vessel::LEVEL_PARAM));
        math::Rect malletSelectorRect(Vec(59.5f, 79.5f), Vec(18.8f, 36.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "MALLET_SELECTOR", &malletSelectorRect);
        auto* malletSelector = createParam<VesselMalletSelector>(
            mm2px(malletSelectorRect.pos), module, Vessel::MALLET_PARAM);
        malletSelector->box.size = mm2px(malletSelectorRect.size);
        malletSelector->images = &malletLink.images;
        addParam(malletSelector);
        math::Rect strikeRect(Vec(3.5f, 24.5f), Vec(36.74f, 51.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "STRIKE_AREA", &strikeRect);
        malletLink.strikePadHeight = mm2px(strikeRect.size.y);
        auto* strikeArea = createParam<VesselStrikeArea>(mm2px(strikeRect.pos), module, Vessel::STRIKE_PARAM);
        strikeArea->box.size = mm2px(strikeRect.size); addParam(strikeArea);
        math::Rect rotateRect(Vec(41.04f, 24.5f), Vec(36.74f, 51.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ROTATE_AREA", &rotateRect);
        auto* rotateArea = createParam<VesselRotateArea>(mm2px(rotateRect.pos), module, Vessel::ROTATE_PARAM);
        rotateArea->box.size = mm2px(rotateRect.size); addParam(rotateArea);
        bowlPitchTint = new VesselPitchTintLayer(module, metalBowlRect, crystalBowlRect);
        bowlPitchTint->box.size = box.size;
        struct InputPlacement { const char* anchor; int id; float x; float y; };
        const InputPlacement inputs[] = {
            {"STRIKE_INPUT", Vessel::STRIKE_INPUT, 9.f, 98.f},
            {"VELOCITY_INPUT", Vessel::VELOCITY_INPUT, 22.f, 98.f},
            {"INTENSITY_INPUT", Vessel::INTENSITY_INPUT, 39.f, 98.f},
            {"ROTATE_INPUT", Vessel::ROTATE_INPUT, 52.f, 98.f},
            {"VOCT_INPUT", Vessel::VOCT_INPUT, 9.f, 110.5f}
        };
        for (const auto& input : inputs) addInput(createInputCentered<Magitek2InputJack>(
            mm2px(point(input.anchor, input.x, input.y)), module, input.id));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("LEFT_OUTPUT", 39.f, 110.5f)), module, Vessel::LEFT_OUTPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("RIGHT_OUTPUT", 52.f, 110.5f)), module, Vessel::RIGHT_OUTPUT));
        }
        // The scoped panel builder has now appended its labels. Both mallet
        // passes belong above those labels, with the bowl between the passes
        // to occlude the back half using its actual raster silhouette.
        auto* backMallet = new VesselMalletRenderWidget(&malletLink, false);
        backMallet->box.size = box.size;
        addChild(backMallet);
        addChild(metalBowlRaster);
        addChild(crystalBowlRaster);
        addChild(bowlPitchTint);
        auto* frontMallet = new VesselMalletRenderWidget(&malletLink, true);
        frontMallet->box.size = box.size;
        addChild(frontMallet);
    }

    void onContextCreate(const ContextCreateEvent& e) override {
        malletLink.images.resetContext(e.vg, false);
        ModuleWidget::onContextCreate(e);
    }
    void onContextDestroy(const ContextDestroyEvent& e) override {
        malletLink.images.resetContext(e.vg, true);
        ModuleWidget::onContextDestroy(e);
    }
    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        auto* m = static_cast<Vessel*>(module);
        if (!m) return;
        if (isDragonKingDebugEnabled()) {
            menu->addChild(new MenuSeparator);
            menu->addChild(createMenuLabel("Temporary orbit tuning"));
            const float defaultWidthMm = malletLink.orbit.size.x / mm2px(1.f);
            menu->addChild(new VesselOrbitWidthSlider(malletLink.orbitWidthScale, defaultWidthMm));
            menu->addChild(createMenuLabel(string::f("Double-click to reset to %.1f mm.", defaultWidthMm)));
        }
        menu->addChild(new MenuSeparator);
        menu->addChild(createSubmenuItem("Processing quality (performance test)", "", [m](Menu* sub) {
            const char* labels[] = {"48 kHz — Economy", "96 kHz — Balanced", "192 kHz — Reference"};
            for (int i = 0; i < 3; ++i) sub->addChild(createCheckMenuItem(labels[i], "",
                [m, i]() { return m->requestedQuality.load(std::memory_order_relaxed) == i; },
                [m, i]() { m->requestedQuality.store(i, std::memory_order_relaxed); }));
            sub->addChild(new MenuSeparator);
            sub->addChild(createMenuLabel("Switching clears the ringing bowl."));
            sub->addChild(createMenuLabel("Targets follow the host sample rate."));
        }));
        const float rate = m->visualInternalRate.load(std::memory_order_relaxed);
        menu->addChild(createMenuLabel(string::f("Actual internal rate: %.1f kHz%s", rate/1000.f,
            m->visualRateFallback.load(std::memory_order_relaxed) ? " (fallback)" : "")));
    }
    void step() override {
        const bool enabled = isDragonKingDebugEnabled();
        layerTiming.beginCycle(enabled);
        const auto start = debug_terminal::debugTimerStart(enabled);
        auto* vessel = static_cast<Vessel*>(module);
        const float frameTime = APP && APP->window
            ? clamp(float(APP->window->getLastFrameDuration()), 0.f, .1f) : 1.f / 60.f;
        const bool rubbing = vessel && vessel->visualRubbing.load(std::memory_order_relaxed);
        if (rubbing)
            malletLink.rubFade = std::min(1.f, malletLink.rubFade + frameTime / .12f);
        else if (vessel && vessel->visualStrikeAftermath.load(std::memory_order_relaxed) > 0.f)
            malletLink.rubFade = 0.f; // A new strike replaces the released rubbing mallet.
        else
            malletLink.rubFade = std::max(0.f, malletLink.rubFade - frameTime / .20f);
        const bool crystalSelected = vessel && vessel->params[Vessel::BOWL_PARAM].getValue() >= .5f;
        if (metalBowlRaster) metalBowlRaster->setVisible(!crystalSelected);
        if (crystalBowlRaster) crystalBowlRaster->setVisible(crystalSelected);
        ModuleWidget::step();
        if (enabled) timing.recordStep(debug_terminal::elapsedUsSince(start));
    }
    void drawLayer(const DrawArgs& args, int layer) override {
        debug_terminal::ScopedUiCycleTimer timer(&layerTiming);
        ModuleWidget::drawLayer(args, layer);
    }
    void draw(const DrawArgs& args) override {
        const bool enabled = isDragonKingDebugEnabled();
        const auto start = debug_terminal::debugTimerStart(enabled);
        ModuleWidget::draw(args);
        auto* m = static_cast<Vessel*>(module);
        if (enabled) {
            timing.recordDraw(debug_terminal::elapsedUsSince(start));
            if (m && debug_terminal::baselineSubmitDue("Vessel", m->debugMetrics.instanceId, system::getTime()))
                debug_terminal::submitBaselineMetrics("Vessel", m->debugMetrics.instanceId,
                    m->debugMetrics.consumeProcessRange(), timing.consumeStepRange(), timing.consumeDrawRange(), layerTiming.consume());
        }
    }
};
Model* modelVessel = createModel<Vessel, VesselWidget>("Vessel");
