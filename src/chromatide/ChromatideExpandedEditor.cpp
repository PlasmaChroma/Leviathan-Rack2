#include "ChromatideExpandedEditor.hpp"
#include "ChromatideWidget.hpp"
#include "../common/visual/VisualAssets.hpp"
#include <algorithm>

ChromatideExpandedEditorOverlay::ChromatideExpandedEditorOverlay() {
    editorZoom = new widget::ZoomWidget();
    addChild(editorZoom);
}

ChromatideExpandedEditorOverlay::~ChromatideExpandedEditorOverlay() {
    if (editorSurface && anchorDock) {
        if (editorZoom && editorSurface->parent == editorZoom) {
            editorZoom->removeChild(editorSurface);
        }
        if (!editorSurface->parent) {
            anchorDock->addChild(editorSurface);
            editorSurface->setPosition(Vec(0, 0));
            editorSurface->box.size = anchorDock->box.size;
        }
        anchorDock->expanded = false;
    }
    if (link && link->overlay == this) {
        link->overlay = nullptr;
    }
}

// Like Wyrm's expanded editor, the overlay tracks the panel rather than
// covering the scene: the canvas scales up from the dock's bottom-right
// corner, growing left and up, so the toolbar and a right-hand Iris stay
// visible. Re-run every step so it follows rack pan and zoom.
void ChromatideExpandedEditorOverlay::layoutToScene() {
    if (!APP || !APP->scene || !anchorDock || !anchorDock->parent || !editorSurface) return;
    const float margin = 4.0f;
    float top = margin;
    if (APP->scene->menuBar && APP->scene->menuBar->isVisible()) {
        top = std::max(top, APP->scene->menuBar->box.pos.y + APP->scene->menuBar->box.size.y + margin);
    }

    const float dockZoom = std::max(anchorDock->getAbsoluteZoom(), 1e-6f);
    const Vec dockSize = anchorDock->box.size;
    const Vec dockOrigin = anchorDock->getAbsoluteOffset(Vec());
    const Vec dockBottomRight = dockOrigin.plus(dockSize.mult(dockZoom));
    const float pad = kFramePadPx;

    // Shrink toward 1x when the scene lacks room above/left of the dock.
    const float roomW = dockBottomRight.x + pad - margin - 2.0f * pad;
    const float roomH = dockBottomRight.y + pad - top - 2.0f * pad;
    float scale = kExpandScale;
    scale = std::min(scale, roomW / std::max(dockSize.x * dockZoom, 1e-6f));
    scale = std::min(scale, roomH / std::max(dockSize.y * dockZoom, 1e-6f));
    scale = std::max(scale, 1.0f);

    const Vec canvasScreen = dockSize.mult(dockZoom * scale);
    const Vec frameSize = canvasScreen.plus(Vec(2.0f * pad, 2.0f * pad));
    const Vec framePos = dockBottomRight.plus(Vec(pad, pad)).minus(frameSize);

    // Scene children are laid out in scene coordinates (scene zoom is 1).
    box.pos = framePos;
    box.size = frameSize;
    editorZoom->box.pos = Vec(pad, pad);
    editorZoom->box.size = canvasScreen;
    editorZoom->setZoom(dockZoom * scale);
    // ZoomWidget scales its children, so keep the surface at dock size;
    // strokes use normalized coordinates and need no remapping.
    editorSurface->box.pos = Vec(0, 0);
    editorSurface->box.size = dockSize;
}

void ChromatideExpandedEditorOverlay::step() {
    layoutToScene();
    OpaqueWidget::step();
}

void ChromatideExpandedEditorOverlay::draw(const DrawArgs& args) {
    const float radius = 4.0f;
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, radius);
    nvgFillColor(args.vg, nvgRGBA(4, 10, 16, 255));
    nvgFill(args.vg);

    OpaqueWidget::draw(args);

    // Cyan frame to match Chromatide's preview tint: outer border plus a
    // thin inner line hugging the canvas.
    const float borderWidth = 2.0f;
    const float inset = 0.5f * borderWidth;
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, inset, inset,
        std::max(0.0f, box.size.x - 2.0f * inset),
        std::max(0.0f, box.size.y - 2.0f * inset), radius);
    nvgStrokeColor(args.vg, nvgRGBA(64, 214, 232, 235));
    nvgStrokeWidth(args.vg, borderWidth);
    nvgStroke(args.vg);

    const Rect canvas(editorZoom->box.pos, editorZoom->box.size);
    nvgBeginPath(args.vg);
    nvgRect(args.vg, canvas.pos.x - 0.5f, canvas.pos.y - 0.5f, canvas.size.x + 1.0f, canvas.size.y + 1.0f);
    nvgStrokeColor(args.vg, nvgRGBA(64, 214, 232, 90));
    nvgStrokeWidth(args.vg, 1.0f);
    nvgStroke(args.vg);
}

void ChromatideExpandedEditorOverlay::onButton(const ButtonEvent& e) {
    if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (collapseAction) collapseAction();
        e.consume(this);
        return;
    }
    OpaqueWidget::onButton(e);
}

void ChromatideExpandedEditorOverlay::onHoverKey(const HoverKeyEvent& e) {
    if (e.action == GLFW_PRESS && e.key == GLFW_KEY_ESCAPE) {
        if (collapseAction) collapseAction();
        e.consume(this);
        return;
    }
    OpaqueWidget::onHoverKey(e);
}
