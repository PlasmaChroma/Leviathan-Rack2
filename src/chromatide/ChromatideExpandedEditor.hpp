#pragma once

#include "../plugin.hpp"
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
    // Open/close run from ChromatideWidget::step(), never inside an event
    // handler: Rack is still iterating the scene's children (and holds
    // hovered/dragged pointers) while it dispatches the event.
    bool openRequested = false;
    bool closeRequested = false;
};

struct ChromatideExpandedEditorOverlay final : widget::OpaqueWidget {
    ChromatideEditorDock* anchorDock = nullptr;
    ChromatideEditorSurface* editorSurface = nullptr;
    widget::ZoomWidget* editorZoom = nullptr;
    std::shared_ptr<ChromatideEditorOverlayLink> link;
    std::function<void()> collapseAction;

    ChromatideExpandedEditorOverlay();
    ~ChromatideExpandedEditorOverlay() override;

    // Screen-space frame padding around the scaled canvas.
    static constexpr float kFramePadPx = 6.0f;
    static constexpr float kExpandScale = 2.0f;

    void layoutToScene();
    void step() override;
    void draw(const DrawArgs& args) override;
    void onButton(const ButtonEvent& e) override;
    void onHoverKey(const HoverKeyEvent& e) override;
};
