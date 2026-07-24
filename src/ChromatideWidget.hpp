#pragma once

#include "Chromatide.hpp"
#include "ChromatideExpandedEditor.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include <rack.hpp>

struct ChromatideEditorSurface final : widget::OpaqueWidget {
    Chromatide* module = nullptr;
    NVGcontext* imageContext = nullptr;
    int imageHandle = -1;
    int uploadedWidth = 0;
    int uploadedHeight = 0;
    std::vector<uint8_t> rgbaBuffer;
    uint64_t lastUploadedRevision = static_cast<uint64_t>(-1);

    bool hovering = false;
    Vec hoverPos;

    explicit ChromatideEditorSurface(Chromatide* module);
    ~ChromatideEditorSurface() override;

    Vec currentLocalMousePos() const;
    void updateTextureBuffer();
    void draw(const DrawArgs& args) override;


    void onButton(const ButtonEvent& e) override;
    void onHover(const HoverEvent& e) override;
    void onDragStart(const DragStartEvent& e) override;
    void onDragMove(const DragMoveEvent& e) override;
    void onDragEnd(const DragEndEvent& e) override;
};

struct ChromatideWidget final : ModuleWidget {
    ChromatideEditorDock* editorDock = nullptr;
    ChromatideEditorSurface* editorSurface = nullptr;
    std::shared_ptr<ChromatideEditorOverlayLink> editorOverlayLink;

    double lastClearClickTime = -10.0;

    explicit ChromatideWidget(Chromatide* module);
    ~ChromatideWidget() override;

    void appendContextMenu(Menu* menu) override;
    void openExpandedEditor();
    void closeExpandedEditor();
    bool isEditorExpanded() const;
};
