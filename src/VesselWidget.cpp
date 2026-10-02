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

namespace {
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

struct VesselMalletLink {
    Vessel* module = nullptr;
    math::Rect orbit;
    math::Rect bowl;
    float strikePadHeight = 0.f;
    // Temporary, UI-only tuning shared by the front/back animation passes.
    std::shared_ptr<float> orbitWidthScale = std::make_shared<float>(1.f);
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

    void onContextCreate(const ContextCreateEvent& e) override {
        visual_assets::onRasterContextCreate(e.vg);
        TransparentWidget::onContextCreate(e);
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        visual_assets::onRasterContextDestroy(e.vg);
        TransparentWidget::onContextDestroy(e);
    }

    void drawMallet(const DrawArgs& args) {
        if (!link || !link->module || !APP || !APP->window) return;
        const bool rubbing = link->module->visualRubbing.load(std::memory_order_relaxed);
        const float aftermath = clamp(
            link->module->visualStrikeAftermath.load(std::memory_order_relaxed), 0.f, 1.f);
        if (!rubbing && (aftermath <= 0.f || !frontPass)) return;
        float angle = link->module->visualRotationAngle.load(std::memory_order_relaxed);
        if (!std::isfinite(angle)) angle = 0.f;
        const float depth = std::sin(angle);
        if (rubbing && frontPass != (depth >= 0.f)) return;

        const std::string fullPath = asset::plugin(pluginInstance, "res/Vessel/PureWoodMallet.png");
        std::shared_ptr<window::Image> source = APP->window->loadImage(fullPath);
        if (!source || source->handle < 0) return;
        int handle = visual_assets::loadRasterMipmapHandle(args.vg, source, fullPath);
        if (handle < 0) handle = source->handle;
        int imageWidth = 0, imageHeight = 0;
        nvgImageSize(args.vg, handle, &imageWidth, &imageHeight);
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
        float opacity = 1.f;
        float imageY = 0.f;
        float rotation = float(M_PI);
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
            // Flip the strike artwork around its center while retaining the
            // contact endpoint: the source's bottom end now meets the front lip,
            // with the handle extending diagonally up and right across the bowl.
            rotation = 0.f;
            imageY = -drawHeight;
        }

        // Preserve the raster's intended 180-degree playing orientation, but
        // draw it downwards in local space so that after rotation its length
        // still extends above the rim. Only the requested part of the contact
        // tip penetrates below the rim path.
        nvgSave(args.vg);
        nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
        nvgTranslate(args.vg, renderedTip.x, renderedTip.y);
        nvgRotate(args.vg, rotation + tilt);
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
        return 1.f - clamp((y - topSaturationMargin) / activeHeight, 0.f, 1.f);
    }
    void publishAmount(float y) {
        dragY = clamp(y, 0.f, box.size.y);
        currentAmount = amountAt(dragY);
        if (auto* vessel = dynamic_cast<Vessel*>(module)) {
            if (kind == Kind::Strike)
                vessel->manualStrikeVelocity.store(currentAmount, std::memory_order_relaxed);
            else
                vessel->manualRotateSpeedScale.store(currentAmount, std::memory_order_relaxed);
        }
        refreshPadTooltip();
    }
    std::string tooltipText() const {
        const int percent = int(std::lround(100.f * currentAmount));
        return kind == Kind::Strike
            ? string::f("Strike: %d%% Velocity", percent)
            : string::f("Rub: %d%% Speed", percent);
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
        const float barHeight = mm2px(3.f);
        nvgBeginPath(args.vg); nvgRoundedRect(args.vg, 0, 0, box.size.x, barHeight, 2);
        nvgFillColor(args.vg, nvgRGB(7, 11, 19)); nvgFill(args.vg);
        nvgStrokeColor(args.vg, nvgRGBA(176, 141, 216, 125)); nvgStrokeWidth(args.vg, .7f); nvgStroke(args.vg);
        if (energy > 0.f) {
            nvgBeginPath(args.vg); nvgRoundedRect(args.vg, 1, 1, (box.size.x-2)*energy, barHeight-2, 1);
            nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, box.size.x, 0,
                nvgRGB(163, 113, 245), nvgRGB(74, 222, 214))); nvgFill(args.vg);
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
        math::Rect malletOrbitRect(Vec(2.f, 29.5f), Vec(77.28f, 9.f));
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
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("MALLET_PARAM", 71.f, 83.5f)), module, Vessel::MALLET_PARAM));
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
            {"SPEED_INPUT", Vessel::SPEED_INPUT, 39.f, 98.f},
            {"ROTATE_INPUT", Vessel::ROTATE_INPUT, 52.f, 98.f},
            {"VOCT_INPUT", Vessel::VOCT_INPUT, 9.f, 110.5f},
            {"PRESSURE_INPUT", Vessel::PRESSURE_INPUT, 22.f, 110.5f}
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
