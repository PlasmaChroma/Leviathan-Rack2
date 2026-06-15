#pragma once

#include "plugin.hpp"

namespace visual_assets {

std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path);
void resetEclipseShadowDrawMetrics();
uint64_t eclipseShadowDrawNs();
uint64_t eclipseShadowDrawCount();

} // namespace visual_assets

struct MagitekInputJack : app::SvgPort {
	MagitekInputJack();
};

struct MagitekOutputJack : app::SvgPort {
	MagitekOutputJack();
};

struct Magitek2InputJack : app::PortWidget {
	Magitek2InputJack();
};

struct Magitek2OutputJack : app::PortWidget {
	Magitek2OutputJack();
};

struct TorxScrew : TransparentWidget {
	widget::FramebufferWidget* fb = nullptr;
	widget::SvgWidget* sw = nullptr;

	TorxScrew();
	void draw(const DrawArgs& args) override;
};

template <typename TBase = GrayModuleLightWidget>
struct TLeviathanCyanPurpleLight : TBase {
	TLeviathanCyanPurpleLight() {
		this->addBaseColor(nvgRGB(0x00, 0xc6, 0xe4));
		this->addBaseColor(nvgRGB(0xa8, 0x62, 0xff));
	}
};
using LeviathanCyanPurpleLight = TLeviathanCyanPurpleLight<>;

struct LeviathanSlider : VCVLightSlider<LeviathanCyanPurpleLight> {
	LeviathanSlider();
};

namespace levi_jack {

NVGcolor input();
NVGcolor output();

} // namespace levi_jack

struct DynamicRingJack : app::SvgPort {
	enum CoreStyle {
		INPUT_CORE,
		OUTPUT_CORE
	};

	NVGcolor ringColor = nvgRGB(0x00, 0xe6, 0xff);
	float glowAmount = 0.42f;
	float ringAmount = 0.82f;
	bool connectedGlow = true;
	widget::FramebufferWidget* ringFb = nullptr;
	TransparentWidget* ringLayer = nullptr;
	bool cachedConnected = false;

	DynamicRingJack();
	explicit DynamicRingJack(const char* coreSvgPath);
	explicit DynamicRingJack(CoreStyle coreStyle);
	void setRingColor(NVGcolor color);
	void setGlowAmount(float amount);
	void setCoreStyle(CoreStyle coreStyle);
	void markRingDirty();
	void drawRingLayer(const DrawArgs& args);
	void step() override;
};

struct DynamicRingInputJack : DynamicRingJack {
	DynamicRingInputJack();
};

struct DynamicRingOutputJack : DynamicRingJack {
	DynamicRingOutputJack();
};

struct GoldButton : app::SvgSwitch {
	TransformWidget* faceTransform = nullptr;
	TransparentWidget* pressOverlay = nullptr;
	widget::FramebufferWidget* pressOverlayFb = nullptr;
	TransparentWidget* dropShadow = nullptr;
	widget::FramebufferWidget* dropShadowFb = nullptr;
	TransparentWidget* fixedBezel = nullptr;
	widget::FramebufferWidget* fixedBezelFb = nullptr;
	float pressAmount = 0.f;

	GoldButton();
	void step() override;
};

struct GearKnobInvertSized : app::SvgKnob {
	struct ActiveRingWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		float centerPx = 23.f;
		float sourceDiameterPx = 46.f;
		float sourceViewBoxPx = 56.f;
		float ringRadiusSourcePx = 18.9f;
		float ringWidthSourcePx = 5.0f;
		float activeRingWidthSourcePx = 3.2f;
		float innerLineWidthSourcePx = 0.55f;
		bool bipolar = false;
		float centerNorm = 0.5f;
		double liquidShimmerUntil = 0.0;

		void draw(const DrawArgs& args) override;
	};

	struct ShadowWidget : TransparentWidget {
		std::shared_ptr<window::Svg> svg;
		widget::FramebufferWidget* cachedSvgFb = nullptr;
		widget::SvgWidget* cachedSvgSw = nullptr;
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;

		ShadowWidget();
		void setSvg(std::shared_ptr<window::Svg> svg);
		void draw(const DrawArgs& args) override;
	};

	ActiveRingWidget* activeRing = nullptr;
	ShadowWidget* shadowLayer = nullptr;
	widget::FramebufferWidget* cachedSvgFb = nullptr;
	widget::SvgWidget* cachedSvgSw = nullptr;
	int dragMoveFrame = 0;
	uint64_t dragLogGestureId = 0;

	GearKnobInvertSized();
	void draw(const DrawArgs& args) override;
	void step() override;
	void onChange(const ChangeEvent& e) override;
	void onDragStart(const DragStartEvent& e) override;
	void onDragEnd(const DragEndEvent& e) override;
	void onDragMove(const DragMoveEvent& e) override;

	void setCachedSvg(std::shared_ptr<window::Svg> svg);
	float normalizedParamValue();
};

struct TinyClockworkGearKnob : GearKnobInvertSized {
	TinyClockworkGearKnob();
};

struct BipolarTinyClockworkGearKnob : TinyClockworkGearKnob {
	BipolarTinyClockworkGearKnob();
};

struct EclipseKnob : app::SvgKnob {
	struct ProgressRingWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		float centerNorm = 0.5f;
		bool bipolar = false;

		void draw(const DrawArgs& args) override;
	};

	struct SvgLayer : TransparentWidget {
		std::shared_ptr<window::Svg> svg;
		widget::FramebufferWidget* cachedSvgFb = nullptr;
		widget::SvgWidget* cachedSvgSw = nullptr;
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		bool rotateWithValue = true;
		float scaleFactor = 1.0f;

		SvgLayer();
		void setSvg(std::shared_ptr<window::Svg> svg);
		void draw(const DrawArgs& args) override;
	};

	struct ShadowWidget : TransparentWidget {
		std::shared_ptr<window::Svg> svg;
		widget::FramebufferWidget* cachedSvgFb = nullptr;
		widget::SvgWidget* cachedSvgSw = nullptr;
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		float scaleFactor = 1.0f;

		ShadowWidget();
		void setSvg(std::shared_ptr<window::Svg> svg);
		void draw(const DrawArgs& args) override;
	};

	ProgressRingWidget* progressRing = nullptr;
	ShadowWidget* shadowLayer = nullptr;
	SvgLayer* backLayer = nullptr;
	SvgLayer* pointerLayer = nullptr;

	EclipseKnob();
	void onChange(const ChangeEvent& e) override;

	void setBackSvg(std::shared_ptr<window::Svg> svg);
	void setPointerSvg(std::shared_ptr<window::Svg> svg);
	void setProgressRingBipolar(bool bipolar, float centerNorm = 0.5f);
	float normalizedParamValue();
};

struct Eclipse2Knob : app::SvgKnob {
	struct ShadowWidget : TransparentWidget {
		float valueNorm = 0.5f;
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;

		void draw(const DrawArgs& args) override;
	};

	struct ProgressLedRingWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		float centerNorm = 0.5f;
		bool bipolar = false;
		int numLeds = 25;

		void draw(const DrawArgs& args) override;
	};

	ProgressLedRingWidget* progressRing = nullptr;
	ShadowWidget* shadowLayer = nullptr;
	EclipseKnob::SvgLayer* backLayer = nullptr;
	float lastBloomAmount = -1.f;

	Eclipse2Knob();
	void step() override;
	void onChange(const ChangeEvent& e) override;

	void setBackSvg(std::shared_ptr<window::Svg> svg);
	void setProgressRingBipolar(bool bipolar, float centerNorm = 0.5f);
	float normalizedParamValue();
};


struct LeviathanHaloKnob : app::SvgKnob {
	struct GlowArcWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;

		void draw(const DrawArgs& args) override;
	};

	struct LightArcWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;

		void draw(const DrawArgs& args) override;
	};

	struct RimHighlightWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;

		void draw(const DrawArgs& args) override;
	};

	EclipseKnob::ShadowWidget* shadowLayer = nullptr;
	GlowArcWidget* glowArc = nullptr;
	EclipseKnob::SvgLayer* backLayer = nullptr;
	EclipseKnob::SvgLayer* centerLayer = nullptr;
	LightArcWidget* lightArc = nullptr;
	RimHighlightWidget* rimHighlight = nullptr;

	LeviathanHaloKnob();
	void onChange(const ChangeEvent& e) override;

	float normalizedParamValue();
};

struct LeviathanHaloKnob2 : app::SvgKnob {
	struct LedArcConfig {
		NVGcolor activeColor = nvgRGBA(26, 249, 252, 236);
		NVGcolor activeHighlightColor = nvgRGBA(122, 252, 255, 188);
		NVGcolor inactiveColor = nvgRGBA(140, 99, 250, 216);
		NVGcolor inactiveHighlightColor = nvgRGBA(0xc0, 0x7b, 0xff, 168);
	};

	struct BloomConfig {
		NVGcolor backgroundOuterActiveColor = nvgRGBA(0, 210, 255, 34);
		NVGcolor backgroundOuterInactiveColor = nvgRGBA(126, 70, 230, 30);
		NVGcolor backgroundMidActiveColor = nvgRGBA(0, 225, 255, 58);
		NVGcolor backgroundMidInactiveColor = nvgRGBA(154, 84, 245, 50);
		NVGcolor backgroundInnerActiveColor = nvgRGBA(30, 245, 255, 88);
		NVGcolor backgroundInnerInactiveColor = nvgRGBA(192, 123, 255, 72);
		NVGcolor foregroundOuterActiveColor = nvgRGBA(70, 250, 255, 48);
		NVGcolor foregroundOuterInactiveColor = nvgRGBA(192, 123, 255, 44);
		NVGcolor foregroundInnerActiveColor = nvgRGBA(190, 255, 255, 36);
		NVGcolor foregroundInnerInactiveColor = nvgRGBA(226, 190, 255, 32);
		NVGcolor reflectionOuterActiveColor = nvgRGBA(18, 176, 196, 48);
		NVGcolor reflectionOuterInactiveColor = nvgRGBA(132, 76, 226, 68);
		NVGcolor reflectionInnerActiveColor = nvgRGBA(124, 246, 255, 38);
		NVGcolor reflectionInnerInactiveColor = nvgRGBA(204, 156, 255, 48);
		NVGcolor guideOuterColor = nvgRGBA(126, 194, 225, 62);
		NVGcolor guideMidColor = nvgRGBA(150, 94, 230, 58);
		NVGcolor guideInnerColor = nvgRGBA(185, 218, 240, 44);
		NVGcolor capReflectionOuterActiveColor = nvgRGBA(58, 210, 230, 68);
		NVGcolor capReflectionOuterInactiveColor = nvgRGBA(168, 106, 255, 82);
		NVGcolor capReflectionInnerActiveColor = nvgRGBA(192, 255, 255, 44);
		NVGcolor capReflectionInnerInactiveColor = nvgRGBA(222, 188, 255, 54);
	};

	struct Config {
		LedArcConfig ledArc;
		BloomConfig bloom;
	};

	struct GlowArcWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		bool foreground = false;
		BloomConfig config;

		void draw(const DrawArgs& args) override;
	};

	struct LightArcWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		LedArcConfig config;
		BloomConfig bloomConfig;

		void draw(const DrawArgs& args) override;
	};

	struct CapReflectionWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		BloomConfig config;

		void draw(const DrawArgs& args) override;
	};

	EclipseKnob::ShadowWidget* shadowLayer = nullptr;
	GlowArcWidget* glowArc = nullptr;
	GlowArcWidget* foregroundGlowArc = nullptr;
	EclipseKnob::SvgLayer* backLayer = nullptr;
	EclipseKnob::SvgLayer* centerLayer = nullptr;
	LightArcWidget* lightArc = nullptr;
	CapReflectionWidget* capReflection = nullptr;
	Config config;
	float lastBloomAmount = -1.f;

	LeviathanHaloKnob2();
	explicit LeviathanHaloKnob2(Config config);
	void step() override;
	void onChange(const ChangeEvent& e) override;

	static Config brightOrangeConfig();
	float normalizedParamValue();
};

struct ClockworkGearKnob : GearKnobInvertSized {
	struct CogwheelWidget : TransparentWidget {
		std::shared_ptr<window::Svg> svg;
		widget::FramebufferWidget* cachedSvgFb = nullptr;
		widget::SvgWidget* cachedSvgSw = nullptr;
		Vec center;
		float diameterPx = 1.f;
		float angleRad = 0.f;

		CogwheelWidget();
		void setSvg(std::shared_ptr<window::Svg> svg);
		void draw(const DrawArgs& args) override;
	};

	CogwheelWidget* primaryCogwheel = nullptr;
	CogwheelWidget* secondaryCogwheel = nullptr;

	ClockworkGearKnob();
	void draw(const DrawArgs& args) override;
	void onChange(const ChangeEvent& e) override;

	void updateCogwheelGeometry();
};

using BigClockworkGearKnob = ClockworkGearKnob;
