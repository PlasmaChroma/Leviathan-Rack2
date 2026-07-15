#pragma once

#include "../NautiloidFractal.hpp"
#include "../plugin.hpp"

#include <memory>

namespace visual_assets {

class FractalGlassOverlay : public TransparentWidget {
public:
	explicit FractalGlassOverlay(const std::string& panelPath);
	~FractalGlassOverlay() override;

	void setFramebuffer(widget::FramebufferWidget* framebuffer);
	void setLiveParams(const iris::NautiloidFractalSourceParams* params);
	bool hasFallbackSelection() const;
	bool deleteFallbackSelection();

	void step() override;
	void draw(const DrawArgs& args) override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

FractalGlassOverlay* addFractalGlassOverlay(
	ModuleWidget* parent, const std::string& panelPath);

} // namespace visual_assets
