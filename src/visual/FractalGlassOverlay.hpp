#pragma once

#include "../NautiloidFractal.hpp"
#include "../plugin.hpp"
#include "../theme/ThemeTypes.hpp"

#include <memory>

namespace visual_assets {

class FractalGlassOverlay : public TransparentWidget {
public:
	FractalGlassOverlay(const std::string& panelPath, const std::string& selectionKey,
		int renderWidth, int renderHeight, bool synchronousFallback,
		const Widget* themePollOwner = nullptr);
	~FractalGlassOverlay() override;

	void setFramebuffer(widget::FramebufferWidget* framebuffer);
	void setLiveParams(const iris::NautiloidFractalSourceParams* params);
	// A local editor may preview texture opacity without publishing a global
	// theme change. NaN clears the preview and resumes the shared setting.
	void setTextureAmountPreview(float amount);
	// A local editor may also preview a semantic glass palette without
	// publishing a library-wide theme change.
	void setColorPreview(
		leviathan::theme::ThemeRole role,
		leviathan::theme::ThemeColor color);
	void clearColorPreview(leviathan::theme::ThemeRole role);
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
