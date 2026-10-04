#pragma once
#include "../plugin.hpp"
#include "BodyMapCore.hpp"
#include <array>
#include <string>

struct VTune;
namespace vtune_body {

// Replace the existing static body widget with this widget, preserving its
// rectangle. It owns the background -> highlights -> outline drawing order.
class BodyMapWidget final : public rack::widget::TransparentWidget {
    VTune* module_ = nullptr; // Only this widget's module; never its neighbor.
    Animation animation_;
    Weights target_{};
    float lastFrequency_ = -1.f;
    Mode lastMode_ = Mode::Off;
    double lastFrameTime_ = -1.0;
    float opacity_ = kDefaultOpacity;
    std::array<std::string, kLayerCount + 2> paths_;
    std::array<bool, kLayerCount + 2> failed_{};
    std::array<rack::app::ModuleLightWidget*, 7> chakraLights_{};
    void drawImage(const DrawArgs& args, std::size_t slot,
                   const FitRect& rect, float opacity);
public:
    explicit BodyMapWidget(VTune* module);
    bool blackBodyBacking = true;
    void step() override;
    void draw(const DrawArgs& args) override;
    void onContextCreate(const ContextCreateEvent& e) override;
    void onContextDestroy(const ContextDestroyEvent& e) override;
};

// Append to VTuneWidget::appendContextMenu(). The module browser passes nullptr.
void appendBodyMapMenu(rack::ui::Menu* menu, VTune* module);

} // namespace vtune_body
