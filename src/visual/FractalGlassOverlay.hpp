#pragma once

#include "../NautiloidFractal.hpp"
#include "../plugin.hpp"

#include <memory>

namespace visual_assets {

class FractalGlassOverlay : public TransparentWidget {
public:
	FractalGlassOverlay(const std::string& panelPath, const std::string& selectionKey,
		int renderWidth, int renderHeight, bool synchronousFallback);
	~FractalGlassOverlay() override;

	void setFramebuffer(widget::FramebufferWidget* framebuffer);
	void setLiveParams(const iris::NautiloidFractalSourceParams* params);
	// Module-preview capture must wait until the asynchronous field has either
	// produced pixels or established that no fallback selection is available.
	bool isReadyForCapture() const;
	bool hasFallbackSelection() const;
	bool deleteFallbackSelection();

	void step() override;
	void draw(const DrawArgs& args) override;
	void onContextDestroy(const ContextDestroyEvent& e) override;
	void onContextCreate(const ContextCreateEvent& e) override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
	void abandonImages();
};

FractalGlassOverlay* addFractalGlassOverlay(
	ModuleWidget* parent, const std::string& panelPath,
	Widget* upperSibling = nullptr);

} // namespace visual_assets
