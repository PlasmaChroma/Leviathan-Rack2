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

    VesselPitchTintLayer(Vessel* module, math::Rect rasterRectMm) : vessel(module) {
        metalRaster = new VesselTintMask(rasterRectMm, "res/Vessel/Metal-Crop-Only.png");
        crystalRaster = new VesselTintMask(rasterRectMm, "res/Vessel/Crystal-Crop-Only.png");
        addChild(metalRaster);
        addChild(crystalRaster);
    }

    void step() override {
        crystalSelected = vessel && vessel->params[Vessel::BOWL_PARAM].getValue() >= .5f;
        if (metalRaster) metalRaster->setVisible(!crystalSelected);
        if (crystalRaster) crystalRaster->setVisible(crystalSelected);
        TransparentWidget::step();
    }

    void draw(const DrawArgs& args) override {
        const float frequency = vessel
            ? vessel->visualFrequency.load(std::memory_order_relaxed)
            : 261.625565f;
        const NVGcolor tint = vesselPitchTint(frequency);
        nvgSave(args.vg);
        nvgGlobalCompositeOperation(args.vg, NVG_SOURCE_OVER);
        nvgGlobalAlpha(args.vg, crystalSelected ? 0.20f : 0.38f);
        nvgGlobalTint(args.vg, tint);
        TransparentWidget::draw(args);
        nvgRestore(args.vg);
    }
};

struct VesselMalletOverlay;

struct VesselMalletOverlayLink {
    Widget* owner = nullptr;
    Vessel* module = nullptr;
    math::Rect orbit;
    math::Rect bowl;
    Widget* metalBowlRaster = nullptr;
    Widget* crystalBowlRaster = nullptr;
    VesselPitchTintLayer* bowlPitchTint = nullptr;
    VesselMalletOverlay* overlay = nullptr;
    debug_terminal::UiCycleTimingAccumulator* drawLayerTiming = nullptr;
};

constexpr float kVesselMalletHeightMm = 34.f;
constexpr float kVesselMalletOverflowPadMm = 40.f;
constexpr float kVesselMalletTipFraction = .12f;
constexpr float kVesselMalletTipPenetration = .20f;
constexpr float kVesselMalletContactOffsetMm = 4.5f;

struct VesselMalletRenderWidget : TransparentWidget {
    std::shared_ptr<VesselMalletOverlayLink> link;
    Vec moduleOrigin;

    explicit VesselMalletRenderWidget(std::shared_ptr<VesselMalletOverlayLink> link)
        : link(std::move(link)) {}

    void onContextCreate(const ContextCreateEvent& e) override {
        visual_assets::onRasterContextCreate(e.vg);
        TransparentWidget::onContextCreate(e);
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        visual_assets::onRasterContextDestroy(e.vg);
        TransparentWidget::onContextDestroy(e);
    }

    void draw(const DrawArgs& args) override {
        if (!link || !link->module || !link->module->visualRubbing.load(std::memory_order_relaxed)
            || !APP || !APP->window) return;
        float angle = link->module->visualRotationAngle.load(std::memory_order_relaxed);
        if (!std::isfinite(angle)) angle = 0.f;
        const float depth = std::sin(angle);

        const std::string fullPath = asset::plugin(pluginInstance, "res/Vessel/WoodSmall.png");
        std::shared_ptr<window::Image> source = APP->window->loadImage(fullPath);
        if (!source || source->handle < 0) return;
        int handle = visual_assets::loadRasterMipmapHandle(args.vg, source, fullPath);
        if (handle < 0) handle = source->handle;
        int imageWidth = 0, imageHeight = 0;
        nvgImageSize(args.vg, handle, &imageWidth, &imageHeight);
        if (imageWidth <= 0 || imageHeight <= 0) return;

        const Vec center = moduleOrigin.plus(link->orbit.pos).plus(link->orbit.size.mult(.5f));
        const Vec radius = link->orbit.size.mult(.5f);
        const Vec contact = center.plus(Vec(
            radius.x * std::cos(angle),
            radius.y * depth + mm2px(kVesselMalletContactOffsetMm)));
        const float drawHeight = mm2px(kVesselMalletHeightMm);
        const float drawWidth = drawHeight * float(imageWidth) / float(imageHeight);
        const float tipPenetration = drawHeight
            * kVesselMalletTipFraction * kVesselMalletTipPenetration;
        const Vec renderedTip = contact.plus(Vec(0.f, tipPenetration));

        // Preserve the raster's intended 180-degree playing orientation, but
        // draw it downwards in local space so that after rotation its length
        // still extends above the rim. Only the requested part of the contact
        // tip penetrates below the rim path.
        auto drawImage = [&](float clipTop, float clipHeight) {
            if (clipHeight <= 0.f) return;
            nvgSave(args.vg);
            nvgScissor(args.vg, 0.f, clipTop, box.size.x, clipHeight);
            nvgTranslate(args.vg, renderedTip.x, renderedTip.y);
            nvgRotate(args.vg, float(M_PI));
            const NVGpaint paint = nvgImagePattern(
                args.vg, -.5f * drawWidth, 0.f, drawWidth, drawHeight, 0.f, handle, 1.f);
            nvgBeginPath(args.vg);
            nvgRect(args.vg, -.5f * drawWidth, 0.f, drawWidth, drawHeight);
            nvgFillPaint(args.vg, paint);
            nvgFill(args.vg);
            nvgRestore(args.vg);
        };

        if (depth >= 0.f) {
            drawImage(0.f, box.size.y);
            return;
        }

        // Both halves stay in one detached rack-level pass, which preserves the
        // off-module motion. On the back half, replay the module's actual bowl
        // and pitch-tint widgets over the complete mallet. This uses the raster's
        // alpha as the occlusion silhouette instead of cutting a geometric slice.
        drawImage(0.f, box.size.y);
        Widget* bowlRaster = link->module->params[Vessel::BOWL_PARAM].getValue() >= .5f
            ? link->crystalBowlRaster : link->metalBowlRaster;
        if (bowlRaster) {
            nvgSave(args.vg);
            nvgTranslate(args.vg,
                moduleOrigin.x + bowlRaster->box.pos.x,
                moduleOrigin.y + bowlRaster->box.pos.y);
            bowlRaster->draw(args);
            nvgRestore(args.vg);
        }
        if (link->bowlPitchTint) {
            nvgSave(args.vg);
            nvgTranslate(args.vg,
                moduleOrigin.x + link->bowlPitchTint->box.pos.x,
                moduleOrigin.y + link->bowlPitchTint->box.pos.y);
            link->bowlPitchTint->draw(args);
            nvgRestore(args.vg);
        }
    }
};

struct VesselMalletOverlay final : TransparentWidget {
    std::shared_ptr<VesselMalletOverlayLink> link;
    VesselMalletRenderWidget* renderer = nullptr;

    explicit VesselMalletOverlay(std::shared_ptr<VesselMalletOverlayLink> link)
        : link(std::move(link)) {
        renderer = new VesselMalletRenderWidget(this->link);
        renderer->moduleOrigin = Vec(mm2px(kVesselMalletOverflowPadMm), mm2px(kVesselMalletOverflowPadMm));
        renderer->box.size = box.size;
        addChild(renderer);
    }

    ~VesselMalletOverlay() override {
        if (link && link->overlay == this) link->overlay = nullptr;
    }

    void step() override {
        TransparentWidget::step();
        if (!link || !link->owner || !link->module || !APP || !APP->scene || !APP->scene->rack
            || !link->owner->isDescendantOf(APP->scene->rack)) {
            requestDelete();
            return;
        }
        const float pad = mm2px(kVesselMalletOverflowPadMm);
        box.pos = link->owner->getRelativeOffset(Vec(), APP->scene->rack).minus(Vec(pad, pad));
        box.size = link->owner->box.size.plus(Vec(2.f * pad, 2.f * pad));
        if (renderer) renderer->box.size = box.size;
    }

    void draw(const DrawArgs&) override {}

    void drawLayer(const DrawArgs& args, int layer) override {
        auto* timing = link ? link->drawLayerTiming : nullptr;
        const bool measurePerf = timing && timing->enabled;
        const auto start = debug_terminal::debugTimerStart(measurePerf);
        if (layer == 1) TransparentWidget::draw(args);
        TransparentWidget::drawLayer(args, layer);
        if (measurePerf) timing->add(debug_terminal::elapsedUsSince(start));
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
    std::shared_ptr<VesselMalletOverlayLink> malletOverlayLink;
    Widget* metalBowlRaster = nullptr;
    Widget* crystalBowlRaster = nullptr;
    VesselPitchTintLayer* bowlPitchTint = nullptr;
    explicit VesselWidget(Vessel* module) {
        setModule(module);
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
        metalBowlRaster = visual_assets::createAspectFitRasterImageWidget(
            "res/Vessel/Metal-Crop-Only.png", bowlRasterRect);
        crystalBowlRaster = visual_assets::createAspectFitRasterImageWidget(
            "res/Vessel/Crystal-Crop-Only.png", bowlRasterRect);
        math::Rect malletOrbitRect(Vec(2.f, 29.5f), Vec(77.28f, 9.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "MALLET_ORBIT", &malletOrbitRect);
        malletOverlayLink = std::make_shared<VesselMalletOverlayLink>();
        malletOverlayLink->owner = this;
        malletOverlayLink->module = module;
        malletOverlayLink->orbit = math::Rect(mm2px(malletOrbitRect.pos), mm2px(malletOrbitRect.size));
        malletOverlayLink->bowl = math::Rect(mm2px(bowlRasterRect.pos), mm2px(bowlRasterRect.size));
        malletOverlayLink->drawLayerTiming = &layerTiming;
        addChild(createLightCentered<SmallAperture<AmberGreenApertureLight>>(
            mm2px(point("VTUNE_EXPANDER_LIGHT", 78.08f, 5.8f)), module, Vessel::VTUNE_LINK_LIGHT));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("BINAURAL_PARAM", 12.f, 83.5f)), module, Vessel::BINAURAL_PARAM));
        addParam(createParamCentered<PlasmaSwitch>(mm2px(point("BOWL_PARAM", 27.f, 83.5f)), module, Vessel::BOWL_PARAM));
        addParam(createParamCentered<LeviathanHaloKnob2>(mm2px(point("PITCH_PARAM", 42.5f, 83.5f)), module, Vessel::PITCH_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("FINE_PARAM", 55.25f, 83.5f)), module, Vessel::FINE_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("MALLET_PARAM", 71.f, 83.5f)), module, Vessel::MALLET_PARAM));
        math::Rect strikeRect(Vec(3.5f, 24.5f), Vec(36.74f, 51.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "STRIKE_AREA", &strikeRect);
        auto* strikeArea = createParam<VesselStrikeArea>(mm2px(strikeRect.pos), module, Vessel::STRIKE_PARAM);
        strikeArea->box.size = mm2px(strikeRect.size); addParam(strikeArea);
        math::Rect rotateRect(Vec(41.04f, 24.5f), Vec(36.74f, 51.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ROTATE_AREA", &rotateRect);
        auto* rotateArea = createParam<VesselRotateArea>(mm2px(rotateRect.pos), module, Vessel::ROTATE_PARAM);
        rotateArea->box.size = mm2px(rotateRect.size); addParam(rotateArea);
        // Keep the bowl visually above the pad surfaces. These are transparent,
        // non-interactive raster widgets, so the underlying pad controls remain usable.
        addChild(metalBowlRaster);
        addChild(crystalBowlRaster);
        bowlPitchTint = new VesselPitchTintLayer(module, bowlRasterRect);
        bowlPitchTint->box.size = box.size;
        addChild(bowlPitchTint);
        malletOverlayLink->metalBowlRaster = metalBowlRaster;
        malletOverlayLink->crystalBowlRaster = crystalBowlRaster;
        malletOverlayLink->bowlPitchTint = bowlPitchTint;
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

    ~VesselWidget() override {
        destroyMalletOverlay();
        if (malletOverlayLink) {
            malletOverlayLink->owner = nullptr;
            malletOverlayLink->module = nullptr;
            malletOverlayLink->metalBowlRaster = nullptr;
            malletOverlayLink->crystalBowlRaster = nullptr;
            malletOverlayLink->bowlPitchTint = nullptr;
            malletOverlayLink->drawLayerTiming = nullptr;
        }
    }

    bool validMalletOverlayContext() const {
        return module && APP && APP->scene && APP->scene->rack
            && parent == APP->scene->rack->getModuleContainer();
    }

    void createMalletOverlay() {
        if (!malletOverlayLink || malletOverlayLink->overlay || !validMalletOverlayContext()) return;
        auto* rack = APP->scene->rack;
        auto* cableContainer = rack->getCableContainer();
        if (!cableContainer || !rack->hasChild(cableContainer)) return;
        auto* overlay = new VesselMalletOverlay(malletOverlayLink);
        malletOverlayLink->overlay = overlay;
        rack->addChildBelow(overlay, cableContainer);
    }

    void destroyMalletOverlay() {
        if (!malletOverlayLink || !malletOverlayLink->overlay) return;
        VesselMalletOverlay* overlay = malletOverlayLink->overlay;
        malletOverlayLink->overlay = nullptr;
        if (overlay->parent) overlay->requestDelete();
        else delete overlay;
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        destroyMalletOverlay();
        ModuleWidget::onContextDestroy(e);
    }
    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        auto* m = static_cast<Vessel*>(module);
        if (!m) return;
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
        if (validMalletOverlayContext()) createMalletOverlay();
        else destroyMalletOverlay();
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
