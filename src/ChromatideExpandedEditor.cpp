#include "ChromatideExpandedEditor.hpp"
#include "ChromatideWidget.hpp"
#include "visual/VisualAssets.hpp"
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

void ChromatideExpandedEditorOverlay::layoutToScene() {
    if (!APP || !APP->scene || !anchorDock || !editorSurface) return;
    Vec sceneSize = APP->scene->box.size;
    box.pos = Vec(0, 0);
    box.size = sceneSize;

    float margin = 20.0f;
    float top = margin;
    if (APP->scene->menuBar && APP->scene->menuBar->isVisible()) {
        top = std::max(top, APP->scene->menuBar->box.pos.y + APP->scene->menuBar->box.size.y + 10.0f);
    }

    float availableW = std::max(1.0f, sceneSize.x - 2.0f * margin);
    float availableH = std::max(1.0f, sceneSize.y - top - margin);

    // Keep aspect ratio 1.50:1 (3:2)
    float targetAspect = ChromatideCanvas::VIEWPORT_ASPECT_RATIO;
    float w = availableW;
    float h = w / targetAspect;
    if (h > availableH) {
        h = availableH;
        w = h * targetAspect;
    }

    Vec pos((sceneSize.x - w) * 0.5f, top + (availableH - h) * 0.5f);
    editorZoom->box.pos = pos;
    editorZoom->box.size = Vec(w, h);

    editorSurface->box.pos = Vec(0, 0);
    editorSurface->box.size = Vec(w, h);
    editorZoom->setZoom(w / anchorDock->box.size.x);
}

void ChromatideExpandedEditorOverlay::draw(const DrawArgs& args) {
    // Backdrop dim
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGBA(5, 8, 14, 210));
    nvgFill(args.vg);

    OpaqueWidget::draw(args);
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
