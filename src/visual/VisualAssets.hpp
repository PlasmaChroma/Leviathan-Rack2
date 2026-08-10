#pragma once

#include "../plugin.hpp"
#include "ApertureLight.hpp"
#include "PlasmaSwitch.hpp"

#include <cstdint>
#include <functional>
#include <vector>

namespace visual_assets {

std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path);
// Loads a mipmapped raster texture and works around Rack's low-bit indexed
// PNG decoder bug by expanding 1/2/4-bit palette images in memory.
int loadRasterMipmapHandle(
	NVGcontext* vg,
	std::shared_ptr<window::Image> lifecycleImage,
	const std::string& fullPath);
bool decodeRasterRgba8(
	const std::string& fullPath,
	std::vector<std::uint8_t>* rgba,
	int* width,
	int* height);
Widget* createSvgRect3DEffectWidget(math::Rect rectMm);
Widget* createSvgRect3DEffectWidget(math::Rect rectMm, NVGcolor baseColor);
Widget* createSvgRect3DEffectWidget(math::Rect rectMm, NVGcolor baseColor, NVGcolor shadowBaseColor);
enum class PreviewFrameTint {
	Cyan,
	Purple
};
Widget* createPreviewFrameEnhancementWidget(math::Rect rectMm);
Widget* createPreviewFrameEnhancementWidget(math::Rect rectMm, PreviewFrameTint tint);
Widget* createPreviewFrameEnhancementWidget(math::Rect rectMm, NVGcolor highlightColor);
float previewFrameOutsideMarginMm();
Widget* createAspectFitRasterImageWidget(
	const char* imageAssetPath,
	math::Rect rectMm,
	bool flipHorizontal = false,
	float opacity = 1.f);
// Installs normal-left and horizontally mirrored-right raster images at SVG
// rect anchors. Either anchor may be omitted for one-sided art. Returns the
// number of images installed, allowing callers and tooling to verify rollout.
int addMirroredPanelRasterImages(
	Widget* parent,
	const std::string& panelPath,
	const char* imageAssetPath,
	const char* leftAnchorId,
	const char* rightAnchorId,
	float opacity = 1.f);
// Standard Leviathan panel branding convention. Panels opt in with
// BRANDING_WAVE_LEFT_RASTER and/or BRANDING_WAVE_RIGHT_RASTER rect anchors.
int addPerfectWavePanelBranding(
	Widget* parent,
	const std::string& panelPath,
	float opacity = 1.f);
// Standard centered single-mark convention for narrow or intentionally solo
// layouts. Panels opt in with a BRANDING_WAVE_SOLO_RASTER rect anchor.
int addPerfectWaveSoloPanelBranding(
	Widget* parent,
	const std::string& panelPath,
	float opacity = 1.f);
// Standard centered Leviathan raster logo for 8 HP / 40.64 mm panels. An SVG
// rect anchor can override the shared fallback without changing widget code.
int addCompactLeviathanLogoBranding(
	Widget* parent,
	const std::string& panelPath,
	float opacity = 1.f);
Widget* createPanelSurfaceEffectWidget(
	const std::string& svgPath,
	Vec panelSizePx,
	float previewProgressionPhase = -1.f);
Widget* createPanelLabelsWidget(const char* svgPath, Vec panelSizePx, float oversample = 2.0f);

// Installs the standard static layers for modules with split panel and label SVGs.
class SplitPanelRenderer final {
	ModuleWidget* parent_ = nullptr;
	Widget* panelSurfaceEffect_ = nullptr;
	std::string panelPath_;
	std::string labelsAssetPath_;
	float previewProgressionPhase_ = -1.f;
	float leviathanLogoOpacity_ = 1.f;
	bool addLeviathanLogo_ = false;

public:
	SplitPanelRenderer(ModuleWidget* parent, const char* panelAssetPath);
	~SplitPanelRenderer();
	const std::string& panelPath() const;
	Widget* panelSurfaceEffectWidget() const;
	float previewProgressionPhase() const;
	int addPerfectWaveBranding(float opacity = 1.f);
	int addPerfectWaveSoloBranding(float opacity = 1.f);
	void addCompactLeviathanLogoBranding(float opacity = 1.f);
	// Labels are inserted when this scoped renderer is destroyed, after the
	// module constructor has added its controls and dynamic visual layers.
	void addLabels(const char* labelsAssetPath);
};

bool isPanelGlassColorCycleEnabled();
void togglePanelGlassColorCycle();
float panelGlassTintAmount();
NVGcolor panelGlassCrystalGlowColor();
NVGcolor panelGlassCrystalStrokeColor();
float panelGlassCyclePhase();

class ScopedPanelGlassPreviewProgression {
public:
	explicit ScopedPanelGlassPreviewProgression(float normalizedPhase);
	~ScopedPanelGlassPreviewProgression();

	ScopedPanelGlassPreviewProgression(
		const ScopedPanelGlassPreviewProgression&) = delete;
	ScopedPanelGlassPreviewProgression& operator=(
		const ScopedPanelGlassPreviewProgression&) = delete;

private:
	bool active = false;
};

void loadSettings();
void saveSettings();
void resetPanelGlassColorCycle();
int addSvgRect3DEffectWidgets(Widget* parent, const std::string& svgPath, const std::string& idSubstring = "ENHANCE");
void resetEclipseShadowDrawMetrics();
uint64_t eclipseShadowDrawNs();
uint64_t eclipseShadowDrawCount();

struct HaloKnob2DrawMetrics {
	uint64_t glSurfaceFramebufferNs = 0u;
	uint64_t nanoVgSurfaceDrawNs = 0u;
	uint64_t centerFramebufferNs = 0u;
	uint64_t capReflectionFramebufferNs = 0u;
	uint32_t glSurfaceFramebufferDraws = 0u;
	uint32_t nanoVgSurfaceDraws = 0u;
	uint32_t centerFramebufferDraws = 0u;
	uint32_t capReflectionFramebufferDraws = 0u;
};

void resetHaloKnob2DrawMetrics();
HaloKnob2DrawMetrics getHaloKnob2DrawMetrics();

} // namespace visual_assets

struct MagitekInputJack : app::SvgPort {
	MagitekInputJack();
};

struct MagitekOutputJack : app::SvgPort {
	MagitekOutputJack();
};

enum class Magitek2JackAnimationStyle {
	None,
	CounterClockwiseRotation,
	ClockwiseRotation,
	PurpleRingsInward,
	CyanRingsOutward,
};

struct Magitek2RasterJack : app::PortWidget {
	explicit Magitek2RasterJack(const char* imagePath, Magitek2JackAnimationStyle animationStyle = Magitek2JackAnimationStyle::ClockwiseRotation);
	void step() override;
	void onEnter(const event::Enter& e) override;
	void onLeave(const event::Leave& e) override;

	widget::FramebufferWidget* shadowFb = nullptr;
	TransparentWidget* animationOverlay = nullptr;
	float hoverSpinRad = 0.f;
	double ringAnimationSec = 0.0;
	float ringOpacity = 0.f;
	Magitek2JackAnimationStyle animationStyle = Magitek2JackAnimationStyle::ClockwiseRotation;
	double lastSpinUpdateSec = 0.0;
	bool hovered = false;
};

struct Magitek2InputJack : Magitek2RasterJack {
	explicit Magitek2InputJack(Magitek2JackAnimationStyle animationStyle = Magitek2JackAnimationStyle::PurpleRingsInward);
};

struct Magitek2OutputJack : Magitek2RasterJack {
	explicit Magitek2OutputJack(Magitek2JackAnimationStyle animationStyle = Magitek2JackAnimationStyle::CyanRingsOutward);
};

struct TorxScrew : TransparentWidget {
	widget::FramebufferWidget* fb = nullptr;
	widget::SvgWidget* sw = nullptr;

	TorxScrew();
	void draw(const DrawArgs& args) override;
};

struct GlowShimmerWidget : TransparentWidget {
	uint8_t glowR = 0xa8;
	uint8_t glowG = 0x62;
	uint8_t glowB = 0xff;
	uint8_t coreR = 0xd0;
	uint8_t coreG = 0xa0;
	uint8_t coreB = 0xff;
	double shimmerPhaseRad = 0.0;
	float opacity = 0.f;
	float pulse = 1.f;
	bool plasmaOrbStyle = false;

	void draw(const DrawArgs& args) override;
};

struct HoverOrbScrew : OpaqueWidget {
	widget::FramebufferWidget* rotatingFb = nullptr;
	GlowShimmerWidget* glowWidget = nullptr;
	float rotationRad = 0.f;
	float spinDirection = 1.f;
	bool renderRotatingLayer = true;
	bool steadyGlow = false;
	double lastSpinUpdateSec = 0.0;
	bool hovered = false;

	HoverOrbScrew(const char* orbPath, const char* underlayPath, float spinDirection, NVGcolor glowColor);

	void onEnter(const event::Enter& e) override;
	void onLeave(const event::Leave& e) override;
	void step() override;
};

struct PurpleOrbScrew : HoverOrbScrew {
	PurpleOrbScrew();
};

struct CyanOrbScrew : HoverOrbScrew {
	CyanOrbScrew();
};

template <typename TBase = GrayModuleLightWidget>
struct TLeviathanCyanPurpleLight : TBase {
	TLeviathanCyanPurpleLight() {
		this->addBaseColor(nvgRGB(0x00, 0xc6, 0xe4));
		this->addBaseColor(nvgRGB(0xa8, 0x62, 0xff));
	}
};
using LeviathanCyanPurpleLight = TLeviathanCyanPurpleLight<>;

struct LuminSlider : VCVLightSlider<LeviathanCyanPurpleLight> {
	LuminSlider();
	void onHover(const event::Hover& e) override;
	void onHoverScroll(const event::HoverScroll& e) override;
	void onButton(const event::Button& e) override;
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
	float sizePx = 24.f;

	GoldButton();
	explicit GoldButton(float sizePx);
	void step() override;
};

struct SmallGoldButton : app::Switch {
	widget::FramebufferWidget* shadowFb = nullptr;
	widget::FramebufferWidget* staticFb = nullptr;
	widget::FramebufferWidget* faceFb = nullptr;
	float sizePx = 18.f;
	float shadowBleedPx = 12.f;
	float pressAmount = 0.f;
	float lastRenderedPressAmount = -1.f;

	SmallGoldButton();
	explicit SmallGoldButton(float sizePx);
	void step() override;
	void draw(const DrawArgs& args) override;
};

// Standard 24 px loop/cycle control, matching the former GoldButton hit area.
struct LoopGoldButton : SmallGoldButton {
	LoopGoldButton() : SmallGoldButton(24.f) {
	}
};

// Standard Leviathan button body with a centered icon and optional state-driven
// artwork. Modules provide the behavior without reimplementing button visuals.
struct LeviathanIconButton : TL1105 {
	std::shared_ptr<window::Svg> iconSvg;
	std::function<std::shared_ptr<window::Svg>()> iconProvider;
	std::function<void()> buttonAction;
	float iconScale = 0.58f;

	LeviathanIconButton();
	void onButton(const event::Button& e) override;
	void draw(const DrawArgs& args) override;
};

// Standard reset control using the generic Leviathan icon-button treatment.
struct LeviathanResetButton : LeviathanIconButton {
	LeviathanResetButton();
};

struct SmallGoldApertureLight : app::ModuleLightWidget {
	NVGcolor baseColor = nvgRGB(255, 235, 120);
	NVGcolor activeColor = nvgRGB(255, 235, 120);

	SmallGoldApertureLight();
	void setBaseColor(NVGcolor color);
	void drawBackground(const DrawArgs& args) override;
	void drawLight(const DrawArgs& args) override;
	void drawHalo(const DrawArgs& args) override;
};

struct SmallGoldApertureButton : LightButton<SmallGoldButton, SmallGoldApertureLight> {
	bool visualHeld = false;

	SmallGoldApertureButton();
	void step() override;
	void onDragStart(const event::DragStart& e) override;
	void onDragEnd(const event::DragEnd& e) override;
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

		NVGcolor activeColorStart = nvgRGBA(255, 218, 42, 248);
		NVGcolor activeColorEnd = nvgRGBA(255, 250, 205, 255);
		NVGcolor inactiveTrackColor = nvgRGBA(2, 1, 1, 230);
		NVGcolor innerLineColor = nvgRGBA(255, 244, 154, 80);

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
	float lastBloomAmount = -1.f;

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

struct DarkTinyClockworkGearKnob : GearKnobInvertSized {
	DarkTinyClockworkGearKnob();
};

struct BipolarDarkTinyClockworkGearKnob : DarkTinyClockworkGearKnob {
	BipolarDarkTinyClockworkGearKnob();
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

struct LeviathanHaloKnob2 : app::Knob {
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

	struct HaloGlSurface;
	HaloGlSurface* glSurface = nullptr;
	EclipseKnob::SvgLayer* backLayer = nullptr;
	EclipseKnob::SvgLayer* centerLayer = nullptr;
	std::shared_ptr<window::Svg> centerNormalSvg;
	std::shared_ptr<window::Svg> centerLitSvg;
	Config config;
	float lastBloomAmount = -1.f;
	bool hovered = false;
	bool dragging = false;
	bool centerLit = false;

	LeviathanHaloKnob2();
	explicit LeviathanHaloKnob2(Config config);
	void step() override;
	void onChange(const ChangeEvent& e) override;
	void onEnter(const event::Enter& e) override;
	void onLeave(const event::Leave& e) override;
	void onDragStart(const event::DragStart& e) override;
	void onDragEnd(const event::DragEnd& e) override;

	static Config brightOrangeConfig();
	bool isVisualDirty() const;
	float normalizedParamValue();
	void setForceNanoVgLedRenderer(bool force);
	void updateCenterSvg();
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
