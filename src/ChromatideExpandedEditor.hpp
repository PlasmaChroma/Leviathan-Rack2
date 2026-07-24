#pragma once

#include "plugin.hpp"
#include <memory>
#include <functional>

struct ChromatideWidget;
struct ChromatideEditorSurface;
struct ChromatideExpandedEditorOverlay;

struct ChromatideEditorDock final : widget::OpaqueWidget {
    bool expanded = false;
};

struct ChromatideEditorOverlayLink {
    ChromatideWidget* owner = nullptr;
    ChromatideExpandedEditorOverlay* overlay = nullptr;
};

struct ChromatideExpandedEditorOverlay final : widget::OpaqueWidget {
    ChromatideEditorDock* anchorDock = nullptr;
    ChromatideEditorSurface* editorSurface = nullptr;
    widget::ZoomWidget* editorZoom = nullptr;
    std::shared_ptr<ChromatideEditorOverlayLink> link;
    std::function<void()> collapseAction;

    ChromatideExpandedEditorOverlay();
    ~ChromatideExpandedEditorOverlay() override;

    void layoutToScene();
    void draw(const DrawArgs& args) override;
    void onButton(const ButtonEvent& e) override;
    void onHoverKey(const HoverKeyEvent& e) override;
};
