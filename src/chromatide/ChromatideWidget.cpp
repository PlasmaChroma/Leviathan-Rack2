#include "ChromatideWidget.hpp"
#include "../common/PanelSvgUtils.hpp"
#include "../common/visual/VisualAssets.hpp"
#include "../common/visual/HsvColorPicker.hpp"
#include "../common/visual/FractalGlassOverlay.hpp"
#include "../theme/ThemeColorClipboard.hpp"
#include "../theme/ThemedTextWidget.hpp"
#include <cstdio>
#include <cstdlib>
#include <osdialog.h>
#include <memory>
#include <app/Scene.hpp>

// --- ChromatideToolButton ---

namespace chromatide_icons {
// Icons draw in a 22x22 design grid centered in `size`.
inline void beginGrid(NVGcontext* vg, Vec size) {
    const float scale = std::min(size.x, size.y) / 22.f;
    nvgSave(vg);
    nvgTranslate(vg, 0.5f * (size.x - 22.f * scale), 0.5f * (size.y - 22.f * scale));
    nvgScale(vg, scale, scale);
}

// Pipette: squeeze bulb top-right, collar, glass body, fine tip bottom-left.
inline void drawDropper(NVGcontext* vg, Vec size, NVGcolor color) {
    beginGrid(vg, size);
    nvgLineCap(vg, NVG_ROUND);
    nvgBeginPath(vg);
    nvgCircle(vg, 16.4f, 5.6f, 2.9f);
    nvgFillColor(vg, color);
    nvgFill(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, 11.2f, 6.9f);
    nvgLineTo(vg, 15.1f, 10.8f);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, 13.2f, 8.8f);
    nvgLineTo(vg, 6.6f, 15.4f);
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, 6.6f, 15.4f);
    nvgLineTo(vg, 4.0f, 18.0f);
    nvgStrokeWidth(vg, 1.3f);
    nvgStroke(vg);
    nvgRestore(vg);
}

// Arc'd arrow. Undo traces counterclockwise and ends pointing left;
// redo is its mirror image (clockwise, pointing right).
inline void drawHistoryArrow(NVGcontext* vg, Vec size, NVGcolor color, bool redo) {
    beginGrid(vg, size);
    if (redo) {
        nvgTranslate(vg, 22.f, 0.f);
        nvgScale(vg, -1.f, 1.f);
    }
    const float cx = 11.8f, cy = 12.6f, r = 5.6f;
    nvgBeginPath(vg);
    // Screen angles: lower right -> right -> top, i.e. counterclockwise.
    nvgArc(vg, cx, cy, r, 1.25f, -1.5708f, NVG_CCW);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 2.2f);
    nvgLineCap(vg, NVG_ROUND);
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, cx - 4.4f, cy - r);
    nvgLineTo(vg, cx + 0.4f, cy - r - 3.6f);
    nvgLineTo(vg, cx + 0.4f, cy - r + 3.6f);
    nvgClosePath(vg);
    nvgFillColor(vg, color);
    nvgFill(vg);
    nvgRestore(vg);
}
// Fullscreen-style corner brackets: elbows on the outer corners to expand,
// pulled in toward the center to contract.
inline void drawExpand(NVGcontext* vg, Vec size, NVGcolor color, bool contract) {
    beginGrid(vg, size);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 2.0f);
    nvgLineCap(vg, NVG_ROUND);
    nvgLineJoin(vg, NVG_ROUND);
    const float lo = 4.5f, hi = 17.5f, arm = 4.6f;
    for (int corner = 0; corner < 4; ++corner) {
        const float sx = (corner & 1) ? -1.f : 1.f;   // +1 = left side
        const float sy = (corner & 2) ? -1.f : 1.f;   // +1 = top side
        const float cx = sx > 0 ? lo : hi;
        const float cy = sy > 0 ? lo : hi;
        nvgBeginPath(vg);
        if (!contract) {
            nvgMoveTo(vg, cx, cy + sy * arm);
            nvgLineTo(vg, cx, cy);
            nvgLineTo(vg, cx + sx * arm, cy);
        } else {
            const float ex = cx + sx * arm, ey = cy + sy * arm;
            nvgMoveTo(vg, cx, ey);
            nvgLineTo(vg, ex, ey);
            nvgLineTo(vg, ex, cy);
        }
        nvgStroke(vg);
    }
    nvgRestore(vg);
}
// Small glyph of a brush head's footprint, used by the head dropdown.
inline void drawHeadGlyph(NVGcontext* vg, float cx, float cy, float r, ChromatideBrushHead head, NVGcolor color,
    float chiselAngle = 0.7853982f, float chiselThickness = 0.32f) {
    nvgBeginPath(vg);
    switch (head) {
        case ChromatideBrushHead::Square:
            nvgRect(vg, cx - 0.82f * r, cy - 0.82f * r, 1.64f * r, 1.64f * r);
            break;
        case ChromatideBrushHead::Chisel:
            nvgSave(vg);
            nvgTranslate(vg, cx, cy);
            nvgRotate(vg, chiselAngle);  // same rotation as the engine's dab
            nvgEllipse(vg, 0, 0, r, std::max(0.12f, chiselThickness) * r);
            nvgRestore(vg);
            break;
        case ChromatideBrushHead::Spray: {
            static const float dots[][2] = {
                {0.f, 0.f}, {0.55f, -0.3f}, {-0.5f, -0.45f}, {-0.2f, 0.6f},
                {0.45f, 0.55f}, {-0.75f, 0.15f}, {0.1f, -0.8f}, {0.8f, 0.2f}};
            for (const auto& d : dots) nvgCircle(vg, cx + d[0] * r, cy + d[1] * r, 0.16f * r);
            break;
        }
        default:
            nvgCircle(vg, cx, cy, 0.9f * r);
            break;
    }
    nvgFillColor(vg, color);
    nvgFill(vg);
}
// Smudge: a fingertip pressed into a smear that trails off to the left.
inline void drawSmudge(NVGcontext* vg, Vec size, NVGcolor color) {
    beginGrid(vg, size);
    nvgLineCap(vg, NVG_ROUND);
    // Smear: three tapering streaks.
    const float ys[3] = {12.6f, 15.2f, 17.8f};
    const float x0s[3] = {3.6f, 2.6f, 4.4f};
    for (int i = 0; i < 3; ++i) {
        nvgBeginPath(vg);
        nvgMoveTo(vg, x0s[i], ys[i]);
        nvgLineTo(vg, 12.5f, ys[i]);
        nvgStrokePaint(vg, nvgLinearGradient(vg, x0s[i], 0, 12.5f, 0, nvgTransRGBAf(color, 0.f), color));
        nvgStrokeWidth(vg, i == 1 ? 2.2f : 1.5f);
        nvgStroke(vg);
    }
    // Fingertip.
    nvgBeginPath(vg);
    nvgMoveTo(vg, 11.0f, 18.6f);
    nvgBezierTo(vg, 10.6f, 14.0f, 12.4f, 11.2f, 15.0f, 11.6f);
    nvgLineTo(vg, 19.4f, 3.6f);
    nvgStrokeColor(vg, color);
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, 11.0f, 18.6f);
    nvgBezierTo(vg, 15.6f, 19.4f, 18.2f, 16.4f, 17.6f, 13.0f);
    nvgLineTo(vg, 21.0f, 6.6f);
    nvgStroke(vg);
    nvgRestore(vg);
}
} // namespace chromatide_icons

// Hover highlight and click flash shared by the toolbar row.
struct ChromatideButtonFeedback {
    static constexpr double kFlashSeconds = 0.28;
    double flashStart = -10.0;

    void flash() { flashStart = system::getTime(); }
    static bool isHovered(const widget::Widget* w) {
        return APP && APP->event && APP->event->hoveredWidget == w;
    }
    // Draw over the button body, under its content.
    void draw(NVGcontext* vg, const widget::Widget* w) const {
        const Vec size = w->box.size;
        if (isHovered(w)) {
            nvgBeginPath(vg);
            nvgRoundedRect(vg, 0.5f, 0.5f, size.x - 1.f, size.y - 1.f, 3.0f);
            nvgFillColor(vg, nvgRGBA(255, 255, 255, 18));
            nvgFill(vg);
            nvgStrokeColor(vg, nvgRGBA(120, 228, 240, 150));
            nvgStrokeWidth(vg, 1.0f);
            nvgStroke(vg);
        }
        const double age = system::getTime() - flashStart;
        if (age >= 0.0 && age < kFlashSeconds) {
            const float t = 1.f - float(age / kFlashSeconds);
            nvgBeginPath(vg);
            nvgRoundedRect(vg, 0, 0, size.x, size.y, 3.0f);
            nvgFillColor(vg, nvgRGBA(140, 240, 250, uint8_t(150.f * t * t)));
            nvgFill(vg);
        }
    }
};

struct ChromatideToolButton final : widget::OpaqueWidget {
    Chromatide* module = nullptr;
    ChromatideTool tool;
    std::string label;
    bool iconOnly = false;

    ChromatideToolButton(
        Chromatide* module,
        ChromatideTool tool,
        const std::string& label,
        bool iconOnly = false)
        : module(module), tool(tool), label(label), iconOnly(iconOnly) {}

    ChromatideButtonFeedback feedback;

    void drawBrushIcon(NVGcontext* vg, NVGcolor color) {
        const float scale = std::min(box.size.x, box.size.y) / 22.f;
        nvgSave(vg);
        nvgTranslate(vg, 0.5f * (box.size.x - 22.f * scale), 0.5f * (box.size.y - 22.f * scale));
        nvgScale(vg, scale, scale);

        nvgBeginPath(vg);
        nvgMoveTo(vg, 7.2f, 15.2f);
        nvgLineTo(vg, 16.8f, 5.6f);
        nvgStrokeColor(vg, color);
        nvgStrokeWidth(vg, 3.2f);
        nvgLineCap(vg, NVG_ROUND);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgMoveTo(vg, 7.8f, 13.8f);
        nvgBezierTo(vg, 7.2f, 17.6f, 5.2f, 18.7f, 2.9f, 18.2f);
        nvgBezierTo(vg, 4.0f, 16.8f, 3.6f, 14.5f, 7.8f, 13.8f);
        nvgFillColor(vg, color);
        nvgFill(vg);
        nvgRestore(vg);
    }

    void drawEraserIcon(NVGcontext* vg, NVGcolor color) {
        const float scale = std::min(box.size.x, box.size.y) / 22.f;
        nvgSave(vg);
        nvgTranslate(vg, 0.5f * (box.size.x - 22.f * scale), 0.5f * (box.size.y - 22.f * scale));
        nvgScale(vg, scale, scale);

        nvgBeginPath(vg);
        nvgMoveTo(vg, 5.0f, 14.0f);
        nvgLineTo(vg, 12.7f, 6.3f);
        nvgLineTo(vg, 18.1f, 11.7f);
        nvgLineTo(vg, 10.4f, 19.4f);
        nvgLineTo(vg, 7.2f, 19.4f);
        nvgLineTo(vg, 3.0f, 15.2f);
        nvgClosePath(vg);
        nvgFillColor(vg, color);
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgMoveTo(vg, 10.6f, 8.4f);
        nvgLineTo(vg, 16.0f, 13.8f);
        nvgStrokeColor(vg, nvgRGBA(7, 11, 18, 150));
        nvgStrokeWidth(vg, 1.2f);
        nvgStroke(vg);
        nvgRestore(vg);
    }

    void draw(const DrawArgs& args) override {
        bool active = (module && module->brushState.tool == tool);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.0f);
        if (active) {
            nvgFillColor(args.vg, nvgRGBA(28, 204, 217, 90));
            nvgStrokeColor(args.vg, nvgRGBA(28, 204, 217, 255));
        } else {
            nvgFillColor(args.vg, nvgRGBA(20, 26, 38, 220));
            nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 40));
        }
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, active ? 1.5f : 1.0f);
        nvgStroke(args.vg);
        feedback.draw(args.vg, this);

        const NVGcolor contentColor = active
            ? nvgRGBA(255, 255, 255, 255)
            : nvgRGBA(180, 190, 205, 220);
        if (iconOnly && tool == ChromatideTool::Brush) {
            drawBrushIcon(args.vg, contentColor);
        }
        else if (iconOnly && tool == ChromatideTool::Eraser) {
            drawEraserIcon(args.vg, contentColor);
        }
        else if (iconOnly && tool == ChromatideTool::Eyedropper) {
            chromatide_icons::drawDropper(args.vg, box.size, contentColor);
        }
        else if (iconOnly && tool == ChromatideTool::Smudge) {
            chromatide_icons::drawSmudge(args.vg, box.size, contentColor);
        }
        else if (APP && APP->window && APP->window->uiFont) {
            nvgFontSize(args.vg, 8.5f);
            nvgFontFaceId(args.vg, APP->window->uiFont->handle);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgFillColor(args.vg, contentColor);
            nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f, label.c_str(), nullptr);
        }
        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
            module->brushState.tool = tool;
            module->params[Chromatide::TOOL_PARAM].setValue(static_cast<float>(tool));
            feedback.flash();
            e.consume(this);
            return;
        }
        OpaqueWidget::onButton(e);
    }
};

// --- ChromatideActionButton ---
struct ChromatideActionButton final : widget::OpaqueWidget {
    enum class Icon { None, Undo, Redo, Expand };
    std::string label;
    std::function<void()> onClick;
    Icon icon = Icon::None;
    std::function<bool()> expandedProvider;  // Expand icon flips to contract when true
    ChromatideButtonFeedback feedback;

    ChromatideActionButton(const std::string& label, std::function<void()> onClick, Icon icon = Icon::None)
        : label(label), onClick(onClick), icon(icon) {}

    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.0f);
        nvgFillColor(args.vg, nvgRGBA(20, 26, 38, 220));
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 40));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 1.0f);
        nvgStroke(args.vg);
        feedback.draw(args.vg, this);

        const NVGcolor iconColor = nvgRGBA(180, 190, 205, 220);
        if (icon == Icon::Expand) {
            chromatide_icons::drawExpand(args.vg, box.size, iconColor, expandedProvider && expandedProvider());
        }
        else if (icon != Icon::None) {
            chromatide_icons::drawHistoryArrow(args.vg, box.size, iconColor, icon == Icon::Redo);
        }
        else if (APP && APP->window && APP->window->uiFont) {
            nvgFontSize(args.vg, 8.5f);
            nvgFontFaceId(args.vg, APP->window->uiFont->handle);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgFillColor(args.vg, nvgRGBA(180, 190, 205, 220));
            nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f, label.c_str(), nullptr);
        }
        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            if (onClick) onClick();
            feedback.flash();
            e.consume(this);
            return;
        }
        OpaqueWidget::onButton(e);
    }
};

// --- ChromatideBrushHeadButton ---
// Narrow dropdown beside the Brush tool: shows the current head and a chevron;
// choosing a head also selects the Brush tool.
struct ChromatideBrushHeadButton final : widget::OpaqueWidget {
    Chromatide* module = nullptr;
    ChromatideButtonFeedback feedback;

    explicit ChromatideBrushHeadButton(Chromatide* module) : module(module) {}

    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.0f);
        nvgFillColor(args.vg, nvgRGBA(20, 26, 38, 220));
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 40));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 1.0f);
        nvgStroke(args.vg);
        feedback.draw(args.vg, this);

        const NVGcolor color = nvgRGBA(180, 190, 205, 220);
        const ChromatideBrushHead head = module ? module->brushState.head : ChromatideBrushHead::Round;
        const float r = std::min(0.24f * box.size.y, 0.3f * box.size.x);
        chromatide_icons::drawHeadGlyph(args.vg, 0.5f * box.size.x, 0.4f * box.size.y, r, head, color,
            module ? module->brushState.angle : 0.7853982f,
            module ? module->brushState.thickness : 0.32f);

        // Chevron under the glyph.
        const float cx = 0.5f * box.size.x, cy = 0.8f * box.size.y, w = std::min(3.0f, 0.22f * box.size.x);
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, cx - w, cy - 0.5f * w);
        nvgLineTo(args.vg, cx, cy + 0.5f * w);
        nvgLineTo(args.vg, cx + w, cy - 0.5f * w);
        nvgStrokeColor(args.vg, color);
        nvgStrokeWidth(args.vg, 1.2f);
        nvgLineCap(args.vg, NVG_ROUND);
        nvgLineJoin(args.vg, NVG_ROUND);
        nvgStroke(args.vg);
        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
            feedback.flash();
            Chromatide* m = module;
            ui::Menu* menu = createMenu();
            menu->addChild(createMenuLabel("Brush head"));
            for (int i = 0; i < static_cast<int>(ChromatideBrushHead::Count); ++i) {
                const ChromatideBrushHead head = static_cast<ChromatideBrushHead>(i);
                menu->addChild(createCheckMenuItem(chromatideBrushHeadName(head), "",
                    [m, head]() { return m->brushState.head == head; },
                    [m, head]() {
                        m->brushState.head = head;
                        if (m->brushState.tool != ChromatideTool::Brush
                            && m->brushState.tool != ChromatideTool::Eraser) {
                            m->brushState.tool = ChromatideTool::Brush;
                            m->params[Chromatide::TOOL_PARAM].setValue(static_cast<float>(ChromatideTool::Brush));
                        }
                    }));
            }
            e.consume(this);
            return;
        }
        OpaqueWidget::onButton(e);
    }
};

// --- ChromatideKnobLabels ---
// Knob captions are drawn here rather than in the panel SVG because they
// follow the selected tool. Unused slots show a dimmed dash.
struct ChromatideKnobLabels final : leviathan::theme::ThemedTextWidget<widget::TransparentWidget> {
    Chromatide* module = nullptr;
    std::array<Vec, 1 + kChromatideKnobSlots> centersPx {};  // Size, then slots

    void draw(const DrawArgs& args) override {
        if (!APP || !APP->window || !APP->window->uiFont) return;
        const ChromatideKnobProfile profile = module ? module->activeKnobProfile : ChromatideKnobProfile::Round;
        nvgFontFaceId(args.vg, APP->window->uiFont->handle);
        nvgFontSize(args.vg, mm2px(2.6f));
        nvgTextLetterSpacing(args.vg, 0.f);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        for (size_t i = 0; i < centersPx.size(); ++i) {
            const char* text = i == 0 ? "SIZE" : chromatideKnobSlotLabel(profile, int(i) - 1);
            NVGcolor color = inputTextColor;
            if (!text) {
                text = "-";
                color.a = 0.35f;
            }
            nvgFillColor(args.vg, color);
            nvgText(args.vg, centersPx[i].x - box.pos.x, centersPx[i].y - box.pos.y, text, nullptr);
        }
    }
};

// --- ChromatideHoldButton ---
// Destructive-action guard: the press must be held for kHoldSeconds while a
// bar fills left to right. Releasing early cancels; after firing, a fresh
// press is required before it can fire again.
struct ChromatideHoldButton final : widget::OpaqueWidget {
    static constexpr double kHoldSeconds = 3.0;
    std::string label;
    std::function<void()> onComplete;
    ChromatideButtonFeedback feedback;
    bool holding = false;
    bool fired = false;
    double holdStart = 0.0;

    ChromatideHoldButton(const std::string& label, std::function<void()> onComplete)
        : label(label), onComplete(onComplete) {}

    void step() override {
        if (holding && !fired && system::getTime() - holdStart >= kHoldSeconds) {
            fired = true;
            if (onComplete) onComplete();
            feedback.flash();
        }
        OpaqueWidget::step();
    }

    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.0f);
        nvgFillColor(args.vg, nvgRGBA(20, 26, 38, 220));
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 40));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 1.0f);
        nvgStroke(args.vg);

        const float p = holding
            ? clamp(float((system::getTime() - holdStart) / kHoldSeconds), 0.f, 1.f)
            : 0.f;
        if (p > 0.f) {
            nvgSave(args.vg);
            nvgIntersectScissor(args.vg, 0, 0, box.size.x * p, box.size.y);
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.0f);
            nvgFillColor(args.vg, fired ? nvgRGBA(240, 40, 48, 220) : nvgRGBA(210, 36, 44, 175));
            nvgFill(args.vg);
            nvgRestore(args.vg);
        }
        feedback.draw(args.vg, this);

        if (APP && APP->window && APP->window->uiFont) {
            nvgFontSize(args.vg, 8.5f);
            nvgFontFaceId(args.vg, APP->window->uiFont->handle);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
            nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f,
                holding && !fired ? "HOLD" : label.c_str(), nullptr);
        }
        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            holding = true;
            fired = false;
            holdStart = system::getTime();
            e.consume(this);  // becomes the dragged widget, so release arrives as onDragEnd
            return;
        }
        OpaqueWidget::onButton(e);
    }

    void onDragEnd(const DragEndEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
            holding = false;
            fired = false;
        }
        OpaqueWidget::onDragEnd(e);
    }
};

// --- ChromatidePaletteWidget ---
struct ChromatidePaletteWidget final : widget::OpaqueWidget {
    Chromatide* module = nullptr;

    explicit ChromatidePaletteWidget(Chromatide* module) : module(module) {}

    void draw(const DrawArgs& args) override {
        static const std::array<ChromatideColor, Chromatide::PALETTE_SIZE> previewPalette {{
            {0, 0, 0}, {255, 255, 255}, {255, 59, 48}, {255, 149, 0},
            {255, 204, 0}, {52, 199, 89}, {0, 122, 255}, {88, 86, 214}, {175, 82, 222}
        }};
        const auto& palette = module ? module->palette : previewPalette;
        const int selectedPaletteIndex = module ? module->selectedPaletteIndex : 1;

        float swatchWidth = (box.size.x - float(Chromatide::PALETTE_SIZE - 1) * 4.0f) / float(Chromatide::PALETTE_SIZE);
        float swatchHeight = box.size.y;

        for (size_t i = 0; i < palette.size(); ++i) {
            float x = i * (swatchWidth + 4.0f);
            const auto& col = palette[i];

            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x, 0, swatchWidth, swatchHeight, 3.0f);
            nvgFillColor(args.vg, nvgRGB(col.r, col.g, col.b));
            nvgFill(args.vg);

            bool selected = (static_cast<int>(i) == selectedPaletteIndex);
            if (selected) {
                nvgStrokeColor(args.vg, nvgRGBA(28, 204, 217, 255));
                nvgStrokeWidth(args.vg, 2.0f);
            } else {
                nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 40));
                nvgStrokeWidth(args.vg, 1.0f);
            }
            nvgStroke(args.vg);
        }
        OpaqueWidget::draw(args);
    }

    void openSwatchMenu(int index) {
        if (!module) return;
        const int64_t moduleId = module->id;
        auto resolve = [moduleId]() -> Chromatide* {
            return (APP && APP->engine) ? dynamic_cast<Chromatide*>(APP->engine->getModule(moduleId)) : nullptr;
        };
        const ChromatideColor current = module->palette[static_cast<size_t>(index)];
        // Don't select here: "Set from current brush color" must still see an
        // eyedropper pick.

        ui::Menu* menu = createMenu();
        menu->addChild(createMenuLabel(string::f("Swatch %d", index + 1)));
        auto* picker = new leviathan::visual::HsvColorPicker();
        picker->setColor(leviathan::theme::ThemeColor(current.r, current.g, current.b), true);
        picker->onColorChange = [resolve, index](leviathan::theme::ThemeColor c, bool) {
            if (Chromatide* m = resolve()) m->setPaletteColor(index, ChromatideColor(c.r, c.g, c.b));
        };
        menu->addChild(picker);
        menu->addChild(new MenuSeparator());
        menu->addChild(createMenuItem("Copy hex", "", [current, resolve, index]() {
            ChromatideColor c = current;
            if (Chromatide* m = resolve()) c = m->palette[static_cast<size_t>(index)];
            if (APP && APP->window) {
                glfwSetClipboardString(APP->window->win, string::f("#%02X%02X%02X", c.r, c.g, c.b).c_str());
            }
        }));
        menu->addChild(createMenuItem("Paste color", "", [resolve, index]() {
            if (!APP || !APP->window) return;
            const char* text = glfwGetClipboardString(APP->window->win);
            leviathan::theme::ThemeColor parsed;
            if (!text || !leviathan::theme::parseClipboardColor(text, &parsed)) return;
            if (Chromatide* m = resolve()) m->setPaletteColor(index, ChromatideColor(parsed.r, parsed.g, parsed.b));
        }));
        menu->addChild(createMenuItem("Set from current brush color", "", [resolve, index]() {
            if (Chromatide* m = resolve()) m->setPaletteColor(index, m->brushState.foreground);
        }));
    }

    int swatchIndexAt(Vec pos) const {
        if (!module) return -1;
        float swatchWidth = (box.size.x - float(Chromatide::PALETTE_SIZE - 1) * 4.0f) / float(Chromatide::PALETTE_SIZE);
        for (size_t i = 0; i < module->palette.size(); ++i) {
            float x = i * (swatchWidth + 4.0f);
            if (pos.x >= x && pos.x <= x + swatchWidth) return static_cast<int>(i);
        }
        return -1;
    }

    void onButton(const ButtonEvent& e) override {
        if (e.action == GLFW_PRESS && module) {
            const int index = swatchIndexAt(e.pos);
            if (index >= 0 && e.button == GLFW_MOUSE_BUTTON_LEFT) {
                module->selectPaletteColor(index);
                e.consume(this);
                return;
            }
            if (index >= 0 && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
                openSwatchMenu(index);
                e.consume(this);
                return;
            }
        }
        OpaqueWidget::onButton(e);
    }

    void onDoubleClick(const DoubleClickEvent& e) override {
        const int index = swatchIndexAt(currentLocal());
        if (index >= 0) {
            openSwatchMenu(index);
            e.consume(this);
            return;
        }
        OpaqueWidget::onDoubleClick(e);
    }

    Vec currentLocal() {
        if (!APP || !APP->scene) return Vec();
        return APP->scene->getMousePos().minus(getAbsoluteOffset(Vec())).div(std::max(getAbsoluteZoom(), 1e-6f));
    }
};

// --- ChromatideBrushPreview ---
// Renders a single brush tip with the real brush engine into a private canvas
// so size, hardness, flow and opacity read exactly as one dab will paint.
struct ChromatideBrushPreview final : widget::TransparentWidget {
    Chromatide* module = nullptr;
    std::unique_ptr<ChromatideCanvas> scratch;
    ChromatideStrokeBuffer strokeBuffer;
    std::vector<uint8_t> rgba;
    int cropW = 0, cropH = 0;

    struct Key {
        float size = -1.f, flow = -1.f, opacity = -1.f, hardness = -1.f;
        ChromatideColor fg, bg;
        int tool = -1, head = -1;
        float angle = -1.f, density = -1.f, thickness = -1.f;
        float w = 0.f, h = 0.f;
        bool operator==(const Key& o) const {
            return size == o.size && flow == o.flow && opacity == o.opacity && hardness == o.hardness
                && fg == o.fg && bg == o.bg && tool == o.tool && head == o.head
                && angle == o.angle && density == o.density && thickness == o.thickness && w == o.w && h == o.h;
        }
    } lastKey;
    bool dirty = true;

    NVGcontext* imageContext = nullptr;
    int imageHandle = -1;
    int uploadedWidth = 0;
    int uploadedHeight = 0;

    explicit ChromatideBrushPreview(Chromatide* module) : module(module) {}

    ~ChromatideBrushPreview() override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
    }
    void onContextDestroy(const ContextDestroyEvent& e) override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
        TransparentWidget::onContextDestroy(e);
    }
    void onContextCreate(const ContextCreateEvent& e) override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
        TransparentWidget::onContextCreate(e);
    }

    ChromatideBrushState currentBrush() const {
        ChromatideBrushState brush;
        if (module) {
            brush = module->brushState;
        }
        else {
            brush.foreground = ChromatideColor(90, 200, 250);
        }
        if (brush.tool == ChromatideTool::Eyedropper || brush.tool == ChromatideTool::Smudge) {
            brush.tool = ChromatideTool::Brush;
        }
        return brush;
    }

    void render(const ChromatideBrushState& brush) {
        if (!scratch) scratch.reset(new ChromatideCanvas());
        // Match the main display's raster-pixel aspect so the dab shape and
        // size are what the canvas will show, zooming out for large brushes.
        const float pxPerUnitX = float(ChromatideCanvas::WIDTH) / 98.0f;
        const float pxPerUnitY = float(ChromatideCanvas::HEIGHT) / 65.27f;
        const float boxAspect = box.size.x / std::max(box.size.y, 1.f);
        float h = clampVal(brush.size * 1.3f, 20.f, float(ChromatideCanvas::HEIGHT));
        float w = h * boxAspect * pxPerUnitX / pxPerUnitY;
        if (w > float(ChromatideCanvas::WIDTH)) {
            h *= float(ChromatideCanvas::WIDTH) / w;
            w = float(ChromatideCanvas::WIDTH);
        }
        cropW = std::max(8, int(w));
        cropH = std::max(8, int(h));

        for (int y = 0; y < cropH; ++y) {
            for (int x = 0; x < cropW; ++x) {
                const bool check = ((x / 8) + (y / 8)) & 1;
                const uint8_t v = check ? 26 : 14;
                scratch->setPixel(x, y, v, v, uint8_t(v + 6));
            }
        }

        // One brush tip, centred. Painted through a stroke buffer so it is the
        // same dab the canvas lays down first (flow x opacity, hardness falloff).
        scratch->beginStrokeBuffer(strokeBuffer);
        scratch->stampAtRaster(0.5f * float(cropW), 0.5f * float(cropH), brush, nullptr, &strokeBuffer);
        strokeBuffer.active = false;

        rgba.resize(size_t(cropW) * cropH * 4u);
        for (int y = 0; y < cropH; ++y) {
            for (int x = 0; x < cropW; ++x) {
                const size_t src = ChromatideCanvas::pixelOffset(x, y);
                const size_t dst = (size_t(y) * cropW + x) * 4u;
                rgba[dst + 0] = scratch->pixels[src + 0];
                rgba[dst + 1] = scratch->pixels[src + 1];
                rgba[dst + 2] = scratch->pixels[src + 2];
                rgba[dst + 3] = 255;
            }
        }
        dirty = true;
    }

    void draw(const DrawArgs& args) override {
        const ChromatideBrushState brush = currentBrush();
        Key key;
        key.size = brush.size; key.flow = brush.flow; key.opacity = brush.opacity; key.hardness = brush.hardness;
        key.fg = brush.foreground; key.bg = brush.background; key.tool = int(brush.tool); key.head = int(brush.head);
        key.angle = brush.angle; key.density = brush.density; key.thickness = brush.thickness;
        key.w = box.size.x; key.h = box.size.y;
        if (!(key == lastKey) || rgba.empty()) {
            render(brush);
            lastKey = key;
        }

        if (imageContext != args.vg) {
            nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
        }
        if (imageHandle > 0 && (uploadedWidth != cropW || uploadedHeight != cropH)) {
            nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
        }
        if (imageHandle <= 0) {
            imageHandle = nvgCreateImageRGBA(args.vg, cropW, cropH, 0, rgba.data());
            imageContext = args.vg;
            uploadedWidth = cropW;
            uploadedHeight = cropH;
            dirty = false;
        }
        else if (dirty) {
            nvgUpdateImage(args.vg, imageHandle, rgba.data());
            dirty = false;
        }

        nvgSave(args.vg);
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.f);
        if (imageHandle > 0) {
            nvgFillPaint(args.vg, nvgImagePattern(args.vg, 0, 0, box.size.x, box.size.y, 0.f, imageHandle, 1.f));
            nvgFill(args.vg);
        }
        nvgStrokeColor(args.vg, nvgRGBA(28, 204, 217, 140));
        nvgStrokeWidth(args.vg, 1.f);
        nvgStroke(args.vg);

        if (APP && APP->window && APP->window->uiFont) {
            nvgFontFaceId(args.vg, APP->window->uiFont->handle);
            nvgFontSize(args.vg, 8.f);
            nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
            nvgFillColor(args.vg, nvgRGBA(235, 240, 247, 200));
            char label[48];
            std::snprintf(label, sizeof(label), "%dpx", int(std::round(brush.size)));
            nvgText(args.vg, 4.f, 3.f, label, nullptr);
            const ChromatideTool shown = module ? module->brushState.tool : ChromatideTool::Brush;
            const char* badge = shown == ChromatideTool::Eraser ? "ERASE"
                : shown == ChromatideTool::Smudge ? "SMUDGE" : nullptr;
            if (badge) {
                nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
                nvgText(args.vg, box.size.x - 4.f, 3.f, badge, nullptr);
            }
        }
        nvgRestore(args.vg);
        TransparentWidget::draw(args);
    }
};

// --- ChromatideEditorSurface ---
ChromatideEditorSurface::ChromatideEditorSurface(Chromatide* module) : module(module) {
    rgbaBuffer.resize(ChromatideCanvas::WIDTH * ChromatideCanvas::HEIGHT * 4, 0);
    for (size_t i = 3; i < rgbaBuffer.size(); i += 4) {
        rgbaBuffer[i] = 255;
    }
}

ChromatideEditorSurface::~ChromatideEditorSurface() {
	// Window is destroyed before Scene during Rack shutdown.
	nvg_gfx_lifecycle::resetOwnedNvgImage(
		imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
}

void ChromatideEditorSurface::onContextDestroy(const ContextDestroyEvent& e) {
	nvg_gfx_lifecycle::resetOwnedNvgImage(
		imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
	OpaqueWidget::onContextDestroy(e);
}

void ChromatideEditorSurface::onContextCreate(const ContextCreateEvent& e) {
	nvg_gfx_lifecycle::resetOwnedNvgImage(
		imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
	OpaqueWidget::onContextCreate(e);
}

bool ChromatideEditorSurface::updateTextureBuffer() {
    if (!module) return false;
    if (module->canvas.revision == lastUploadedRevision && !rgbaBuffer.empty()) return false;

    for (int y = 0; y < ChromatideCanvas::HEIGHT; ++y) {
        for (int x = 0; x < ChromatideCanvas::WIDTH; ++x) {
            size_t srcOff = ChromatideCanvas::pixelOffset(x, y);
            size_t dstOff = (static_cast<size_t>(y) * ChromatideCanvas::WIDTH + static_cast<size_t>(x)) * 4u;
            rgbaBuffer[dstOff + 0] = module->canvas.pixels[srcOff + 0];
            rgbaBuffer[dstOff + 1] = module->canvas.pixels[srcOff + 1];
            rgbaBuffer[dstOff + 2] = module->canvas.pixels[srcOff + 2];
            rgbaBuffer[dstOff + 3] = 255;
        }
    }
    lastUploadedRevision = module->canvas.revision;
    return true;
}

Vec ChromatideEditorSurface::currentLocalMousePos() const {
    if (!APP || !APP->scene) return Vec();
    auto* self = const_cast<ChromatideEditorSurface*>(this);
    Vec absoluteOrigin = self->getAbsoluteOffset(Vec());
    float absoluteZoom = std::max(self->getAbsoluteZoom(), 1e-6f);
    return APP->scene->getMousePos().minus(absoluteOrigin).div(absoluteZoom);
}

void ChromatideEditorSurface::draw(const DrawArgs& args) {
    if (module) {
        module->syncBrushFromParams();
    }
    const bool pixelsChanged = updateTextureBuffer();

    if (imageContext != args.vg) {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
    }

    if (imageHandle <= 0) {
        imageHandle = nvgCreateImageRGBA(args.vg, ChromatideCanvas::WIDTH, ChromatideCanvas::HEIGHT, 0, rgbaBuffer.data());
        imageContext = args.vg;
        uploadedWidth = ChromatideCanvas::WIDTH;
        uploadedHeight = ChromatideCanvas::HEIGHT;
    } else if (pixelsChanged) {
        nvgUpdateImage(args.vg, imageHandle, rgbaBuffer.data());
    }

    // Render bitmap texture
    if (imageHandle > 0) {
        NVGpaint imgPaint = nvgImagePattern(args.vg, 0, 0, box.size.x, box.size.y, 0.0f, imageHandle, 1.0f);
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, imgPaint);
        nvgFill(args.vg);
    }

    // Hover brush outline cursor

    Vec localMouse = currentLocalMousePos();
    bool mouseInBounds = (localMouse.x >= 0.0f && localMouse.x <= box.size.x && localMouse.y >= 0.0f && localMouse.y <= box.size.y);

    if (mouseInBounds && module) {
        float size = module->brushState.size;
        float viewportRadiusY = (size * 0.5f / static_cast<float>(ChromatideCanvas::HEIGHT)) * box.size.y;
        float viewportRadiusX = viewportRadiusY;

        nvgBeginPath(args.vg);
        nvgEllipse(args.vg, localMouse.x, localMouse.y, viewportRadiusX, viewportRadiusY);

        if (module->brushState.tool == ChromatideTool::Eraser) {
            nvgStrokeColor(args.vg, nvgRGBA(255, 59, 48, 220));
        } else if (module->brushState.tool == ChromatideTool::Eyedropper) {
            nvgStrokeColor(args.vg, nvgRGBA(255, 204, 0, 220));
        } else if (module->brushState.tool == ChromatideTool::Smudge) {
            nvgStrokeColor(args.vg, nvgRGBA(120, 228, 240, 220));
        } else {
            nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 220));
        }
        nvgStrokeWidth(args.vg, 1.2f);
        nvgStroke(args.vg);
    }

    OpaqueWidget::draw(args);
}

void ChromatideEditorSurface::onHover(const HoverEvent& e) {
    OpaqueWidget::onHover(e);
}

void ChromatideEditorSurface::onButton(const ButtonEvent& e) {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
        if (e.action == GLFW_PRESS && module) {
            Vec localPos = currentLocalMousePos();
            float u = clampVal(localPos.x / box.size.x, 0.0f, 1.0f);
            float v = clampVal(localPos.y / box.size.y, 0.0f, 1.0f);
            module->beginStroke(u, v);
            e.consume(this);
            return;
        }
        if (e.action == GLFW_RELEASE && module) {
            module->endStroke();
            e.consume(this);
            return;
        }
    }
    OpaqueWidget::onButton(e);
}

void ChromatideEditorSurface::onDragStart(const DragStartEvent& e) {
    OpaqueWidget::onDragStart(e);
}

void ChromatideEditorSurface::onDragMove(const DragMoveEvent& e) {
    if (module && module->strokeActive) {
        Vec localPos = currentLocalMousePos();
        float u = clampVal(localPos.x / box.size.x, 0.0f, 1.0f);
        float v = clampVal(localPos.y / box.size.y, 0.0f, 1.0f);
        module->updateStroke(u, v);
    }
    OpaqueWidget::onDragMove(e);
}

void ChromatideEditorSurface::onDragEnd(const DragEndEvent& e) {
    if (module && module->strokeActive) {
        module->endStroke();
    }
    OpaqueWidget::onDragEnd(e);
}


// --- ChromatideWidget ---
ChromatideWidget::ChromatideWidget(Chromatide* module) {
    setModule(module);

    visual_assets::SplitPanelRenderer splitPanel(this, "res/Chromatide.panel.svg");
    const std::string& panelPath = splitPanel.panelPath();
    splitPanel.addThemedLabels("res/Chromatide.labels.svg",
            "res/Chromatide.theme-text-input.svg", nullptr);
    splitPanel.addPerfectWaveBranding();
    splitPanel.addCompactLeviathanLogoBranding();
    visual_assets::addFractalGlassOverlay(
      this, panelPath, splitPanel.panelSurfaceEffectWidget());

    // Panel Screws
    addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.0f)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.0f * RACK_GRID_WIDTH, 0.0f)));
    addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.0f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    auto rectMm = [&](const char* id, math::Rect fallback) {
        math::Rect rect = fallback;
        panel_svg::loadRectFromSvgMm(panelPath, id, &rect);
        return rect;
    };

    // Compact canvas display geometry
    math::Rect displayRectMm = rectMm("DISPLAY", math::Rect(Vec(1.8f, 6.5f), Vec(98.0f, 65.27f)));
    addChild(visual_assets::createPreviewFrameEnhancementWidget(displayRectMm, visual_assets::PreviewFrameTint::Cyan));

    editorDock = new ChromatideEditorDock();
    editorDock->box.pos = mm2px(displayRectMm.pos.plus(Vec(0.4f, 0.4f)));
    editorDock->box.size = mm2px(displayRectMm.size.minus(Vec(0.8f, 0.8f)));
    addChild(editorDock);

    editorSurface = new ChromatideEditorSurface(module);
    editorSurface->box.pos = Vec(0, 0);
    editorSurface->box.size = editorDock->box.size;
    editorDock->addChild(editorSurface);

    editorOverlayLink = std::make_shared<ChromatideEditorOverlayLink>();
    editorOverlayLink->owner = this;

    auto mm2pxRect = [](const math::Rect& r) { return Rect(mm2px(r.pos), mm2px(r.size)); };

    // Toolbar row: every control is an icon except the hold-to-clear bar.
    // Buttons are laid out proportionally across the TOOL_ROW anchor.
    const Rect toolRow = mm2pxRect(rectMm("TOOL_ROW", math::Rect(Vec(4.4f, 76.8f), Vec(92.8f, 7.45f))));
    struct ToolSlot { float weight; widget::Widget* w; };
    std::vector<ToolSlot> slots = {
        {1.0f, new ChromatideToolButton(module, ChromatideTool::Brush, "", true)},
        {0.55f, new ChromatideBrushHeadButton(module)},
        {1.0f, new ChromatideToolButton(module, ChromatideTool::Eraser, "", true)},
        {1.0f, new ChromatideToolButton(module, ChromatideTool::Smudge, "", true)},
        {1.0f, new ChromatideToolButton(module, ChromatideTool::Eyedropper, "", true)},
        {1.0f, new ChromatideActionButton("", [module]() { if (module) module->undo(); },
            ChromatideActionButton::Icon::Undo)},
        {1.0f, new ChromatideActionButton("", [module]() { if (module) module->redo(); },
            ChromatideActionButton::Icon::Redo)},
        {1.6f, new ChromatideHoldButton("CLEAR", [module]() { if (module) module->clearCanvas(); })},
        {1.0f, [this]() {
            auto* expand = new ChromatideActionButton("", [this]() { requestExpandedEditor(!isEditorExpanded()); },
                ChromatideActionButton::Icon::Expand);
            expand->expandedProvider = [this]() { return isEditorExpanded(); };
            return expand;
        }()},
    };
    const float gap = 4.0f;
    float totalWeight = 0.0f;
    for (const auto& slot : slots) totalWeight += slot.weight;
    const float unit = (toolRow.size.x - gap * float(slots.size() - 1)) / totalWeight;
    float x = toolRow.pos.x;
    for (const auto& slot : slots) {
        slot.w->box = Rect(Vec(x, toolRow.pos.y), Vec(slot.weight * unit, toolRow.size.y));
        addChild(slot.w);
        x += slot.weight * unit + gap;
    }

    auto* paletteWidget = new ChromatidePaletteWidget(module);
    paletteWidget->box = mm2pxRect(rectMm("PALETTE", math::Rect(Vec(4.4f, 86.4f), Vec(92.8f, 6.8f))));
    addChild(paletteWidget);

    // Krita-style brush engine controls. Size is universal; the other four
    // knobs change meaning (and remember their values) per tool profile.
    auto* knobLabels = new ChromatideKnobLabels();
    knobLabels->module = module;
    int knobIndex = 0;
    auto knobAt = [&](const char* id, Vec fallbackMm, int paramId) {
        Vec posMm = fallbackMm;
        panel_svg::loadPointFromSvgMm(panelPath, id, &posMm);
        addParam(createParamCentered<Eclipse2Knob>(mm2px(posMm), module, paramId));
        knobLabels->centersPx[size_t(knobIndex++)] = mm2px(Vec(posMm.x, 114.7f));
    };
    {
        Vec lightMm(98.4f, 5.8f);
        panel_svg::loadPointFromSvgMm(panelPath, "IRIS_EXPANDER_LIGHT", &lightMm);
        addChild(createLightCentered<SmallAperture<AmberGreenVioletApertureLight>>(
            mm2px(lightMm), module, Chromatide::IRIS_LINK_LIGHT));
    }

    auto* preview = new ChromatideBrushPreview(module);
    preview->box = mm2pxRect(rectMm("BRUSH_PREVIEW", math::Rect(Vec(4.03f, 97.3f), Vec(18.0f, 18.0f))));
    addChild(preview);

    knobAt("BRUSH_SIZE_PARAM", Vec(29.0f, 106.1f), Chromatide::BRUSH_SIZE_PARAM);
    knobAt("BRUSH_FLOW_PARAM", Vec(44.75f, 106.1f), Chromatide::BRUSH_FLOW_PARAM);
    knobAt("BRUSH_OPACITY_PARAM", Vec(60.5f, 106.1f), Chromatide::BRUSH_OPACITY_PARAM);
    knobAt("BRUSH_HARDNESS_PARAM", Vec(76.25f, 106.1f), Chromatide::BRUSH_HARDNESS_PARAM);
    knobAt("BRUSH_JITTER_PARAM", Vec(92.0f, 106.1f), Chromatide::BRUSH_JITTER_PARAM);
    knobLabels->box = Rect(Vec(0, mm2px(110.f)), Vec(box.size.x, mm2px(9.f)));
    addChild(knobLabels);
}

ChromatideWidget::~ChromatideWidget() {
    closeExpandedEditor();
    if (editorOverlayLink) {
        editorOverlayLink->owner = nullptr;
    }
}

void ChromatideWidget::requestExpandedEditor(bool open) {
    if (!editorOverlayLink) return;
    editorOverlayLink->openRequested = open;
    editorOverlayLink->closeRequested = !open;
}

void ChromatideWidget::step() {
    if (auto* m = static_cast<Chromatide*>(module)) {
        m->updateKnobProfile();
        m->dwellStroke(system::getTime());
    }
    if (editorOverlayLink) {
        if (editorOverlayLink->closeRequested) {
            editorOverlayLink->closeRequested = false;
            closeExpandedEditor();
        } else if (editorOverlayLink->openRequested) {
            editorOverlayLink->openRequested = false;
            openExpandedEditor();
        }
    }
    ModuleWidget::step();
    if (auto* canvasModule = static_cast<Chromatide*>(module)) {
        canvasModule->serviceIrisSource();
    }
}

void ChromatideWidget::openExpandedEditor() {
    if (isEditorExpanded() || !module || !APP || !APP->scene || !editorDock || !editorSurface) {
        return;
    }
    auto* overlay = new ChromatideExpandedEditorOverlay();
    overlay->anchorDock = editorDock;
    overlay->editorSurface = editorSurface;
    overlay->link = editorOverlayLink;
    const std::shared_ptr<ChromatideEditorOverlayLink> link = editorOverlayLink;
    overlay->collapseAction = [link]() {
        if (link) {
            link->openRequested = false;
            link->closeRequested = true;
        }
    };
    editorOverlayLink->overlay = overlay;

    editorDock->removeChild(editorSurface);
    editorDock->expanded = true;
    overlay->editorZoom->addChild(editorSurface);
    overlay->layoutToScene();
    if (APP->scene->menuBar && APP->scene->hasChild(APP->scene->menuBar)) {
        APP->scene->addChildBelow(overlay, APP->scene->menuBar);
    } else {
        APP->scene->addChild(overlay);
    }
}

void ChromatideWidget::closeExpandedEditor() {
    ChromatideExpandedEditorOverlay* overlay = editorOverlayLink ? editorOverlayLink->overlay : nullptr;
    if (!overlay) return;
    if (editorSurface && overlay->editorZoom && editorSurface->parent == overlay->editorZoom) {
        overlay->editorZoom->removeChild(editorSurface);
    }
    if (editorSurface && editorDock && !editorSurface->parent) {
        editorDock->addChild(editorSurface);
        editorSurface->setPosition(Vec(0, 0));
        editorSurface->box.size = editorDock->box.size;
        editorDock->expanded = false;
    }
    if (APP && APP->scene && APP->scene->hasChild(overlay)) {
        APP->scene->removeChild(overlay);
    }
    editorOverlayLink->overlay = nullptr;
    delete overlay;
}

bool ChromatideWidget::isEditorExpanded() const {
    return editorDock && editorDock->expanded;
}

void ChromatideWidget::appendContextMenu(Menu* menu) {
    ModuleWidget::appendContextMenu(menu);

    auto* chromatideModule = dynamic_cast<Chromatide*>(module);
    if (!chromatideModule) return;

    menu->addChild(new MenuSeparator());

    menu->addChild(createMenuItem("Save Canvas as PNG...", "", [=]() {
        osdialog_filters* filters = osdialog_filters_parse("PNG image:png,PNG");
        char* pathC = osdialog_file(OSDIALOG_SAVE, nullptr, "Chromatide.png", filters);
        osdialog_filters_free(filters);
        if (!pathC) return;
        std::string path = pathC;
        std::free(pathC);
        const std::string ext = string::lowercase(system::getExtension(path));
        if (ext != ".png") path += ".png";
        if (!chromatideModule->savePng(path)) {
            osdialog_message(OSDIALOG_WARNING, OSDIALOG_OK,
                string::f("Could not save %s", path.c_str()).c_str());
        }
    }));

    menu->addChild(createMenuItem("Load Image into Canvas...", "", [=]() {
        osdialog_filters* filters = osdialog_filters_parse(
            "Images:png,PNG,jpg,JPG,jpeg,JPEG,bmp,BMP,tga,TGA,gif,GIF");
        char* pathC = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
        osdialog_filters_free(filters);
        if (!pathC) return;
        const std::string path = pathC;
        std::free(pathC);
        if (!chromatideModule->loadImage(path)) {
            osdialog_message(OSDIALOG_WARNING, OSDIALOG_OK,
                string::f("Could not load %s", path.c_str()).c_str());
        }
    }));

    menu->addChild(createMenuItem("Reset Canvas to Black", "", [=]() {
        chromatideModule->clearCanvas();
    }));

    menu->addChild(createMenuItem("Reset Palette to Defaults", "", [=]() {
        chromatideModule->palette = {
            ChromatideColor(0, 0, 0),
            ChromatideColor(255, 255, 255),
            ChromatideColor(255, 59, 48),
            ChromatideColor(255, 149, 0),
            ChromatideColor(255, 204, 0),
            ChromatideColor(52, 199, 89),
            ChromatideColor(0, 122, 255),
            ChromatideColor(88, 86, 214),
            ChromatideColor(175, 82, 222)
        };
        chromatideModule->selectPaletteColor(1);
    }));

    menu->addChild(createMenuItem(isEditorExpanded() ? "Collapse Editor Overlay" : "Expand Editor Overlay", "", [=]() {
        requestExpandedEditor(!isEditorExpanded());
    }));
}

Model* modelChromatide = createModel<Chromatide, ChromatideWidget>("Chromatide");
