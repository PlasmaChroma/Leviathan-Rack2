#include "vtune/BodyMapWidget.hpp"
#include "VTune.hpp"
#include "visual/VisualAssets.hpp"
#include <cmath>
#include <cstdio>

namespace vtune_body {
namespace {
constexpr std::size_t kBackingSlot = kLayerCount;
constexpr std::size_t kOutlineSlot = kLayerCount + 1;
constexpr float kDrawEpsilon = 1.f / 1024.f;
const char* modeName(Mode mode) {
    switch (mode) {
        case Mode::Report: return "Report regions (illustrative)";
        case Mode::Symbolic: return "Solfeggio (symbolic)";
        case Mode::Combined: return "Combined (illustrative + symbolic)";
        case Mode::Off: return "Off (outline only)";
    }
    return "Report regions (illustrative)";
}
}

BodyMapWidget::BodyMapWidget(VTune* module) : module_(module) {
    for (std::size_t i = 0; i < kBandCount; ++i)
        paths_[i] = rack::asset::plugin(pluginInstance, kBands[i].asset);
    for (std::size_t i = 0; i < kToneCount; ++i)
        paths_[kBandCount + i] = rack::asset::plugin(pluginInstance, kTones[i].asset);
    paths_[kBackingSlot] = rack::asset::plugin(pluginInstance, "res/VTune/body-map/body_backing.png");
    paths_[kOutlineSlot] = rack::asset::plugin(pluginInstance, "res/VTune/body-map/body_outline.png");
}

void BodyMapWidget::step() {
    rack::widget::TransparentWidget::step();
    const double now = APP && APP->window ? APP->window->getFrameTime() : -1.0;
    double dt = 1.0 / 60.0;
    if (std::isfinite(now) && now >= 0.0) {
        if (lastFrameTime_ >= 0.0 && now >= lastFrameTime_)
            dt = std::min(0.25, now - lastFrameTime_);
        lastFrameTime_ = now;
    }
    else lastFrameTime_ = -1.0;
    float frequency = 0.f;
    Mode mode = Mode::Off;
    if (module_) {
        frequency = module_->bodyFrequencyHz.load(std::memory_order_relaxed);
        mode = sanitizeMode(module_->bodyMapMode.load(std::memory_order_relaxed));
        opacity_ = unit(module_->bodyMapOpacity.load(std::memory_order_relaxed));
    }
    animation_.advance(evaluate(frequency, mode), dt);
}

void BodyMapWidget::drawImage(const DrawArgs& args, std::size_t slot,
                             const FitRect& rect, float opacity) {
    if (opacity <= kDrawEpsilon || failed_[slot] || !APP || !APP->window)
        return;
    // Window owns/caches this reference. Obtain it afresh for the current frame;
    // do not retain an Image/shared GL handle across a window/context lifetime.
    auto lifecycleImage = APP->window->loadImage(paths_[slot]);
    if (!lifecycleImage) {
        failed_[slot] = true; // Window already logs load failures; avoid spam.
        return;
    }
    // Leviathan's helper returns a borrowed, shared, per-context mipmapped image.
    // It works both in the main context and in Rack's framebuffer/preview context.
    const int handle = visual_assets::loadRasterMipmapHandle(args.vg, lifecycleImage, paths_[slot]);
    if (handle <= 0) return;
    const NVGpaint paint = nvgImagePattern(args.vg, rect.x, rect.y,
        rect.width, rect.height, 0.f, handle, unit(opacity));
    nvgBeginPath(args.vg);
    nvgRect(args.vg, rect.x, rect.y, rect.width, rect.height);
    nvgFillPaint(args.vg, paint);
    nvgFill(args.vg);
}

void BodyMapWidget::draw(const DrawArgs& args) {
    const FitRect rect = aspectFit(box.size.x, box.size.y);
    if (args.vg && rect.width > 0.f && rect.height > 0.f) {
        nvgSave(args.vg);
        // Rect scissor only clips to this widget. Body-shaped clipping is baked
        // into the RGBA masks; NanoVG scissoring is not a silhouette clip.
        nvgIntersectScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
        // Keep this a soft, ordinary panel illustration. No additive blending,
        // per-frame blur or framebuffer. Images are loaded lazily into the
        // shared texture cache; subsequent frames reuse those textures.
        if (blackBodyBacking) drawImage(args, kBackingSlot, rect, 1.f);
        const auto& weights = animation_.weights();
        for (std::size_t i = 0; i < kLayerCount; ++i)
            drawImage(args, i, rect, opacity_ * weights[i]);
        drawImage(args, kOutlineSlot, rect, 1.f);
        nvgRestore(args.vg);
    }
    rack::widget::TransparentWidget::draw(args);
}

void BodyMapWidget::onContextCreate(const ContextCreateEvent& e) {
    visual_assets::onRasterContextCreate(e.vg);
    failed_.fill(false);
    lastFrameTime_ = -1.0;
    rack::widget::TransparentWidget::onContextCreate(e);
}
void BodyMapWidget::onContextDestroy(const ContextDestroyEvent& e) {
    visual_assets::onRasterContextDestroy(e.vg);
    failed_.fill(false);
    lastFrameTime_ = -1.0;
    // Never delete a borrowed texture or keep its integer handle ourselves.
    rack::widget::TransparentWidget::onContextDestroy(e);
}

namespace {
struct ModeItem final : rack::ui::MenuItem {
    VTune* module = nullptr;
    Mode choice = Mode::Report;
    void onAction(const ActionEvent&) override {
        if (module) module->bodyMapMode.store(int(choice), std::memory_order_relaxed);
    }
    void step() override {
        rightText = module && sanitizeMode(module->bodyMapMode.load(std::memory_order_relaxed)) == choice ? "✓" : "";
        rack::ui::MenuItem::step();
    }
};
struct OpacityItem final : rack::ui::MenuItem {
    VTune* module = nullptr;
    float choice = kDefaultOpacity;
    void onAction(const ActionEvent&) override {
        if (module) module->bodyMapOpacity.store(choice, std::memory_order_relaxed);
    }
    void step() override {
        rightText = module && std::abs(module->bodyMapOpacity.load(std::memory_order_relaxed) - choice) < .005f ? "✓" : "";
        rack::ui::MenuItem::step();
    }
};
struct BodyMenu final : rack::ui::MenuItem {
    VTune* module = nullptr;
    rack::ui::Menu* createChildMenu() override {
        auto* menu = new rack::ui::Menu;
        for (Mode choice : {Mode::Report, Mode::Symbolic, Mode::Combined, Mode::Off}) {
            auto* item = new ModeItem;
            item->module = module; item->choice = choice; item->text = modeName(choice);
            menu->addChild(item);
        }
        menu->addChild(new rack::ui::MenuSeparator);
        for (float value : {.35f, kDefaultOpacity, .85f}) {
            auto* item = new OpacityItem;
            item->module = module; item->choice = value;
            item->text = value < .5f ? "Soft intensity" : value > .7f ? "Strong intensity" : "Normal intensity";
            menu->addChild(item);
        }
        menu->addChild(new rack::ui::MenuSeparator);
        menu->addChild(rack::createMenuLabel("Sound+Body associations, not measured organ activity"));
        const float hz = module ? module->bodyFrequencyHz.load(std::memory_order_relaxed) : 0.f;
        if (validFrequency(hz)) {
            char text[96];
            std::snprintf(text, sizeof(text), "Vessel target center: %.2f Hz", double(hz));
            menu->addChild(rack::createMenuLabel(text));
            const auto values = evaluate(hz, sanitizeMode(module->bodyMapMode.load(std::memory_order_relaxed)));
            for (std::size_t i = 0; i < kLayerCount; ++i) {
                if (values[i] < .05f) continue;
                const char* label = i < kBandCount ? kBands[i].label : kTones[i-kBandCount].label;
                menu->addChild(rack::createMenuLabel(label));
            }
        }
        else menu->addChild(rack::createMenuLabel("No linked frequency in the 20–2000 Hz report range"));
        return menu;
    }
};
}

void appendBodyMapMenu(rack::ui::Menu* menu, VTune* module) {
    if (!menu || !module) return;
    menu->addChild(new rack::ui::MenuSeparator);
    auto* item = new BodyMenu;
    item->module = module;
    item->text = "Body map";
    item->rightText = "▸";
    menu->addChild(item);
}
} // namespace vtune_body
