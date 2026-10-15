#pragma once

#include "ThemeService.hpp"
#include "ThemeUiPoller.hpp"

namespace leviathan {
namespace theme {

// Native panel labels share the SVG text roles without reading theme state in
// draw(). A containing label framebuffer is invalidated only on color changes.
template <class Base = widget::TransparentWidget>
struct ThemedTextWidget : Base {
	widget::FramebufferWidget* textFramebuffer = nullptr;
	NVGcolor inputTextColor;
	NVGcolor outputTextColor;
	ThemeUiPoller textThemePoller;
	uint64_t textColorGeneration = 0u;

	ThemedTextWidget() {
		textThemePoller.setOwner(this);
		refreshTextColors();
	}

	void refreshTextColors() {
		const auto state = read();
		textColorGeneration = state.colorGeneration;
		const auto& colors = state.snapshot.colors;
		inputTextColor = nvgRGB(colors.textInput.r, colors.textInput.g, colors.textInput.b);
		outputTextColor = nvgRGB(colors.textOutput.r, colors.textOutput.g, colors.textOutput.b);
	}

	void step() override {
		if (textThemePoller.shouldPoll() && textColorGeneration != colorGeneration()) {
			refreshTextColors();
			if (textFramebuffer) textFramebuffer->setDirty();
		}
		Base::step();
	}
};

} // namespace theme
} // namespace leviathan
