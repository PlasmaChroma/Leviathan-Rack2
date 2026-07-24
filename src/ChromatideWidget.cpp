#include "ChromatideWidget.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include <app/Scene.hpp>

// --- ChromatideToolButton ---

struct ChromatideToolButton final : widget::OpaqueWidget {
    Chromatide* module = nullptr;
    ChromatideTool tool;
    std::string label;

    ChromatideToolButton(Chromatide* module, ChromatideTool tool, const std::string& label)
        : module(module), tool(tool), label(label) {}

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

        if (APP && APP->window && APP->window->uiFont) {
            nvgFontSize(args.vg, 10.0f);
            nvgFontFaceId(args.vg, APP->window->uiFont->handle);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgFillColor(args.vg, active ? nvgRGBA(255, 255, 255, 255) : nvgRGBA(180, 190, 205, 220));
            nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f, label.c_str(), nullptr);
        }
        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
            module->brushState.tool = tool;
            module->params[Chromatide::TOOL_PARAM].setValue(static_cast<float>(tool));
            e.consume(this);
            return;
        }
        OpaqueWidget::onButton(e);
    }
};

// --- ChromatideActionButton ---
struct ChromatideActionButton final : widget::OpaqueWidget {
    std::string label;
    std::function<void()> onClick;

    ChromatideActionButton(const std::string& label, std::function<void()> onClick)
        : label(label), onClick(onClick) {}

    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3.0f);
        nvgFillColor(args.vg, nvgRGBA(20, 26, 38, 220));
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 40));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 1.0f);
        nvgStroke(args.vg);

        if (APP && APP->window && APP->window->uiFont) {
            nvgFontSize(args.vg, 10.0f);
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
            e.consume(this);
            return;
        }
        OpaqueWidget::onButton(e);
    }
};

// --- ChromatidePaletteWidget ---
struct ChromatidePaletteWidget final : widget::OpaqueWidget {
    Chromatide* module = nullptr;

    explicit ChromatidePaletteWidget(Chromatide* module) : module(module) {}

    void draw(const DrawArgs& args) override {
        if (!module) return;

        float swatchWidth = (box.size.x - 7.0f * 4.0f) / 8.0f;
        float swatchHeight = box.size.y;

        for (size_t i = 0; i < module->palette.size(); ++i) {
            float x = i * (swatchWidth + 4.0f);
            const auto& col = module->palette[i];

            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x, 0, swatchWidth, swatchHeight, 3.0f);
            nvgFillColor(args.vg, nvgRGB(col.r, col.g, col.b));
            nvgFill(args.vg);

            bool selected = (static_cast<int>(i) == module->selectedPaletteIndex);
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

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
            float swatchWidth = (box.size.x - 7.0f * 4.0f) / 8.0f;
            for (size_t i = 0; i < module->palette.size(); ++i) {
                float x = i * (swatchWidth + 4.0f);
                if (e.pos.x >= x && e.pos.x <= x + swatchWidth) {
                    module->selectPaletteColor(static_cast<int>(i));
                    e.consume(this);
                    return;
                }
            }
        }
        OpaqueWidget::onButton(e);
    }
};

// --- ChromatideEditorSurface ---
ChromatideEditorSurface::ChromatideEditorSurface(Chromatide* module) : module(module) {
    rgbaBuffer.resize(ChromatideCanvas::WIDTH * ChromatideCanvas::HEIGHT * 4, 255);
}

ChromatideEditorSurface::~ChromatideEditorSurface() {
    if (APP && APP->window && APP->window->vg) {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, APP->window->vg, true);
    } else {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
    }
}

void ChromatideEditorSurface::updateTextureBuffer() {
    if (!module) return;
    if (module->canvas.revision == lastUploadedRevision && !rgbaBuffer.empty()) return;

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
    updateTextureBuffer();

    if (imageContext != args.vg) {
        nvg_gfx_lifecycle::resetOwnedNvgImage(imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
    }

    if (imageHandle <= 0) {
        imageHandle = nvgCreateImageRGBA(args.vg, ChromatideCanvas::WIDTH, ChromatideCanvas::HEIGHT, 0, rgbaBuffer.data());
        imageContext = args.vg;
        uploadedWidth = ChromatideCanvas::WIDTH;
        uploadedHeight = ChromatideCanvas::HEIGHT;
    } else {
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

    if ((hovering || mouseInBounds) && module) {
        float size = module->brushState.size;
        float viewportRadiusY = (size * 0.5f / static_cast<float>(ChromatideCanvas::HEIGHT)) * box.size.y;
        float viewportRadiusX = viewportRadiusY;

        nvgBeginPath(args.vg);
        nvgEllipse(args.vg, localMouse.x, localMouse.y, viewportRadiusX, viewportRadiusY);

        if (module->brushState.tool == ChromatideTool::Eraser) {
            nvgStrokeColor(args.vg, nvgRGBA(255, 59, 48, 220));
        } else if (module->brushState.tool == ChromatideTool::Eyedropper) {
            nvgStrokeColor(args.vg, nvgRGBA(255, 204, 0, 220));
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
    hovering = true;
    hoverPos = e.pos;
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

    std::string panelPath = asset::plugin(pluginInstance, "res/nautiloid.panel.svg");
    visual_assets::SplitPanelRenderer splitPanel(this, panelPath.c_str());

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

    float marginX = 14.0f;
    float contentW = box.size.x - 2.0f * marginX;

    // Row 1: Tool Selector & Action Buttons (y = 282 px)
    float toolRowY = editorDock->box.pos.y + editorDock->box.size.y + 12.0f;
    float btnH = 22.0f;

    // Tool Buttons: Brush, Eraser, Dropper
    float toolBtnW = 44.0f;
    auto* brushBtn = new ChromatideToolButton(module, ChromatideTool::Brush, "BRUSH");
    brushBtn->box = Rect(Vec(marginX, toolRowY), Vec(toolBtnW, btnH));
    addChild(brushBtn);

    auto* eraserBtn = new ChromatideToolButton(module, ChromatideTool::Eraser, "ERASER");
    eraserBtn->box = Rect(Vec(marginX + toolBtnW + 4.0f, toolRowY), Vec(toolBtnW, btnH));
    addChild(eraserBtn);

    auto* dropBtn = new ChromatideToolButton(module, ChromatideTool::Eyedropper, "DROPPER");
    dropBtn->box = Rect(Vec(marginX + (toolBtnW + 4.0f) * 2, toolRowY), Vec(toolBtnW + 4.0f, btnH));
    addChild(dropBtn);

    // Action Buttons: Undo, Redo, Clear, Expand
    float actBtnW = 40.0f;
    float actStartX = box.size.x - marginX - 4.0f * (actBtnW + 4.0f) + 4.0f;

    auto* undoBtn = new ChromatideActionButton("UNDO", [module]() { if (module) module->undo(); });
    undoBtn->box = Rect(Vec(actStartX, toolRowY), Vec(actBtnW, btnH));
    addChild(undoBtn);

    auto* redoBtn = new ChromatideActionButton("REDO", [module]() { if (module) module->redo(); });
    redoBtn->box = Rect(Vec(actStartX + actBtnW + 4.0f, toolRowY), Vec(actBtnW, btnH));
    addChild(redoBtn);

    auto* clearBtn = new ChromatideActionButton("CLEAR", [module]() { if (module) module->clearCanvas(); });
    clearBtn->box = Rect(Vec(actStartX + (actBtnW + 4.0f) * 2, toolRowY), Vec(actBtnW, btnH));
    addChild(clearBtn);

    auto* expandBtn = new ChromatideActionButton("EXPAND", [this]() { openExpandedEditor(); });
    expandBtn->box = Rect(Vec(actStartX + (actBtnW + 4.0f) * 3, toolRowY), Vec(actBtnW + 4.0f, btnH));
    addChild(expandBtn);

    // Row 2: 8 Color Swatch Palette (y = 314 px)
    float palRowY = toolRowY + btnH + 10.0f;
    float palH = 20.0f;
    auto* paletteWidget = new ChromatidePaletteWidget(module);
    paletteWidget->box = Rect(Vec(marginX, palRowY), Vec(contentW, palH));
    addChild(paletteWidget);

    // Row 3: Size & Opacity Knobs (y = 350 px)
    float knobRowY = palRowY + palH + 28.0f;
    float knobLeftX = box.size.x * 0.30f;
    float knobRightX = box.size.x * 0.70f;

    addParam(createParamCentered<BefacoTinyKnobWhite>(Vec(knobLeftX, knobRowY), module, Chromatide::BRUSH_SIZE_PARAM));
    addParam(createParamCentered<BefacoTinyKnobWhite>(Vec(knobRightX, knobRowY), module, Chromatide::BRUSH_OPACITY_PARAM));
}

ChromatideWidget::~ChromatideWidget() {
    closeExpandedEditor();
    if (editorOverlayLink) {
        editorOverlayLink->owner = nullptr;
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
        if (link && link->owner) {
            link->owner->closeExpandedEditor();
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
            ChromatideColor(90, 200, 250),
            ChromatideColor(175, 82, 222)
        };
        chromatideModule->selectPaletteColor(1);
    }));

    menu->addChild(createMenuItem("Re-publish Canvas to Iris", "", [=]() {
        chromatideModule->publishToIris();
    }));

    menu->addChild(createMenuItem("Expand Editor Overlay", "", [=]() {
        openExpandedEditor();
    }));
}

Model* modelChromatide = createModel<Chromatide, ChromatideWidget>("Chromatide");
