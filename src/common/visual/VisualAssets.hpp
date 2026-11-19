#pragma once
#include "HaloKnob2Metrics.hpp"

#include "../../plugin.hpp"
#include "../../theme/ThemeTypes.hpp"
#include "ApertureLight.hpp"
#include "PlasmaSwitch.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <vector>

namespace visual_assets {

// Effective dark-panel state: Rack's global setting, optionally flipped per
// module (for example a module's "opposite theme" context-menu override).
inline bool panelThemeDark(const std::atomic<bool>* invert) {
	return settings::preferDarkPanels != (invert && invert->load(std::memory_order_relaxed));
}

std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path);
// Loads a mipmapped raster texture and works around Rack's low-bit indexed
// PNG decoder bug by expanding 1/2/4-bit palette images in memory.
// The returned handle is owned by vg. Callers that retain it must track that
// context and must not delete it after the context has been destroyed.
int createContextOwnedRasterMipmapHandle(
	NVGcontext* vg,
	const std::string& fullPath);
// Borrowed shared texture, cached separately for each live NanoVG context on
// the UI thread. Window-owned lifecycleImage identifies the context lifetime;
// callers forward context events and never delete this handle themselves.
int loadRasterMipmapHandle(
	NVGcontext* vg,
	std::shared_ptr<window::Image> lifecycleImage,
	const std::string& fullPath);
void onRasterContextDestroy(NVGcontext* vg);
void onRasterContextCreate(NVGcontext* vg);
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
float neonBarSliderAssetHeight(float widthPx);
float neonBarSliderSelectorX(float widthPx, float value);
float neonBarSliderValueFromX(float xPx, float widthPx);
void drawNeonBarSlider(
	const Widget::DrawArgs& args,
	Vec sizePx,
	float value,
	bool hovered,
	const char* label,
	float secondaryValue = -1.f);
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
	const Widget* themePollOwner = nullptr);
void setPanelSurfaceColorPreview(
	Widget* panelSurface,
	leviathan::theme::ThemeRole role,
	leviathan::theme::ThemeColor color);
Widget* createPanelLabelsWidget(const char* svgPath, Vec panelSizePx, float oversample = 2.0f);
Widget* createThemedPanelLabelsWidget(
	const char* labelsAssetPath,
	const char* themeTextInputAssetPath,
	const char* themeTextOutputAssetPath,
	Vec panelSizePx,
	const Widget* themePollOwner = nullptr);

// panelPath is absolute; its sibling .background.svg supplies the black base.
// Use this for modules that assemble their own foreground layers.
app::SvgPanel* createThemedPanel(const std::string& panelPath, const Widget* owner);

// Installs the standard static layers for modules with split panel and label SVGs.
class SplitPanelRenderer final {
	ModuleWidget* parent_ = nullptr;
	Widget* panelSurfaceEffect_ = nullptr;
	std::string panelPath_;
	std::string labelsAssetPath_;
	std::string themeTextInputAssetPath_;
	std::string themeTextOutputAssetPath_;
	float leviathanLogoOpacity_ = 1.f;
	bool addLeviathanLogo_ = false;
	bool rasterBackgrounds_ = false; // labels follow the light/dark raster panel
	const std::atomic<bool>* themeInvert_ = nullptr;

public:
	SplitPanelRenderer(ModuleWidget* parent, const char* panelAssetPath,
		const char* originalBackgroundAssetPath = nullptr);
	~SplitPanelRenderer();
	const std::string& panelPath() const;
	Widget* panelSurfaceEffectWidget() const;
	// Replace the SVG panel background with per-theme raster art (light/dark
	// follows Rack's "prefer dark panels"). Either path may be null to reuse the other.
	void setRasterBackgrounds(const char* lightAssetPath, const char* darkAssetPath);
	// Per-theme transparent overlay drawn over the raster background at rectPx
	// (panel pixels). Only shown while a raster background is active.
	void setRasterOverlay(const char* lightAssetPath, const char* darkAssetPath, math::Rect rectPx);
	// Black shade (0-1 alpha) per theme over the raster background, under the overlay.
	void setRasterShade(float lightAlpha, float darkAlpha = 0.f);
	// Optional per-module flag that flips light/dark for the raster panel and
	// theme-following labels. Must outlive the widget (module-owned); may be null.
	void setThemeInvert(const std::atomic<bool>* invert);
	int addPerfectWaveBranding(float opacity = 1.f);
	int addPerfectWaveSoloBranding(float opacity = 1.f);
	void addCompactLeviathanLogoBranding(float opacity = 1.f);
	// Labels are inserted when this scoped renderer is destroyed, after the
	// module constructor has added its controls and dynamic visual layers.
	void addLabels(const char* labelsAssetPath);
	void addThemedLabels(const char* labelsAssetPath, const char* inputTextAssetPath, const char* outputTextAssetPath);
};

int addSvgRect3DEffectWidgets(Widget* parent, const std::string& svgPath, const std::string& idSubstring = "ENHANCE");
void resetEclipseShadowDrawMetrics();
uint64_t eclipseShadowDrawNs();
uint64_t eclipseShadowDrawCount();


void resetHaloKnob2DrawMetrics();
HaloKnob2DrawMetrics getHaloKnob2DrawMetrics();

} // namespace visual_assets

enum class Magitek2JackAnimationStyle {
	None,
	CounterClockwiseRotation,
	ClockwiseRotation,
	PurpleRingsInward,
	CyanRingsOutward,
};

struct Magitek2RasterJack : app::PortWidget {
	explicit Magitek2RasterJack(const char* imagePath, const char* shadowBakedImagePath,
		Magitek2JackAnimationStyle animationStyle = Magitek2JackAnimationStyle::ClockwiseRotation);
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
	// UI-thread palette assignment; legacy gold is retained until setColor().
	struct Palette {
		NVGcolor socket = nvgRGB(32, 25, 18);
		NVGcolor faceLight = nvgRGB(255, 239, 146);
		NVGcolor faceDark = nvgRGB(156, 86, 20);
		NVGcolor rimLight = nvgRGB(255, 248, 186);
		NVGcolor rimDark = nvgRGB(80, 36, 8);
		NVGcolor glint = nvgRGB(255, 255, 230);
		NVGcolor glintFade = nvgRGB(255, 226, 120);
	};
	widget::FramebufferWidget* shadowFb = nullptr;
	widget::FramebufferWidget* staticFb = nullptr;
	widget::FramebufferWidget* faceFb = nullptr;
	float sizePx = 18.f;
	float shadowBleedPx = 12.f;
	float pressAmount = 0.f;
	float lastRenderedPressAmount = -1.f;

	SmallGoldButton();
	explicit SmallGoldButton(float sizePx);
	// Opaque cap color; shading is derived once, not on every frame.
	void setColor(NVGcolor color);
	void resetColor();
	const Palette& getPalette() const { return palette; }
	void step() override;
	void draw(const DrawArgs& args) override;

private:
	Palette palette;
	NVGcolor assignedColor = {};
	bool customColor = false;
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
	// Opt-in brightness for module-browser previews; live modules ignore this.
	float previewBrightness = 0.f;
	NVGcolor baseColor = nvgRGB(255, 235, 120);
	NVGcolor activeColor = nvgRGB(255, 235, 120);

	SmallGoldApertureLight();
	void setBaseColor(NVGcolor color);
	void onHover(const HoverEvent& e) override;
	void onEnter(const EnterEvent& e) override;
	void onLeave(const LeaveEvent& e) override;
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
		bool retainedRenderer = false;
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
	bool retainedRenderer = false;
	unsigned stableVisualSteps = 0;
	float lastVisualValue = -1.f;

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
		void onContextDestroy(const ContextDestroyEvent& e) override;
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
		void onContextDestroy(const ContextDestroyEvent& e) override;
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
		std::shared_ptr<window::Svg> svg;
		float valueNorm = 0.5f;
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;

		void setSvg(std::shared_ptr<window::Svg> svg) { this->svg = svg; }
		void draw(const DrawArgs& args) override;
		void onContextDestroy(const ContextDestroyEvent& e) override;
	};

	struct ProgressLedRingWidget : TransparentWidget {
		bool dotsOnly = false; // Internal atlas bake; normal rendering includes track/glow.
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
	bool lastRetainedCapEnabled = false;
	bool lastRingCacheEnabled = false;
	// Optional normalized live ring value. The owner must outlive this widget;
	// nullptr restores parameter following. The cap always follows the parameter.
	const std::atomic<float>* ringSource = nullptr;
	bool retainedRenderer = true;
	unsigned stableVisualSteps = 0;
	float lastCapValue = -1.f;
	float lastRingValue = -1.f;

	Eclipse2Knob();
	void step() override;
	void draw(const DrawArgs& args) override;
	void onChange(const ChangeEvent& e) override;
	void setRingSource(const std::atomic<float>* source);
	void setRetainedRenderer(bool enabled);

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
		bool bipolar = false;
		// Opt-in for modules with live arcs: pause GL updates after leaving the
		// viewport, retaining the last surface until an actual draw resumes it.
		bool suspendWhenUndrawn = false;
		// Widget diameter in Rack px. Every layer, the shader and the cap scale
		// from it; 46 is the standard size.
		float diameter = 46.f;
		// UI-only mapping for controls whose semantic zero spans a raw-value plateau.
		float (*visualValue)(engine::ParamQuantity*, float) = nullptr;
	};

	struct GlowArcWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		bool foreground = false;
		bool bipolar = false;
		BloomConfig config;

		void draw(const DrawArgs& args) override;
	};

	struct LightArcWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		bool bipolar = false;
		LedArcConfig config;
		BloomConfig bloomConfig;

		void draw(const DrawArgs& args) override;
	};

	struct CapReflectionWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;
		bool bipolar = false;
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
	float lastVisualValue = -1.f;
	// Optional live source for the LED arc fill, normalized 0..1 (for example
	// a module's effective level). The cap keeps following the knob. Null: the arc follows the knob as before. Written by the
	// audio thread, read once per UI step with relaxed ordering.
	const std::atomic<float>* arcSource = nullptr;
	// Live arc redraw limits: changes smaller than arcQuantum are ignored and
	// live updates are spaced at least arcInterval seconds apart.
	float arcQuantum = 1.f / 512.f;
	float arcInterval = 1.f / 30.f;
	float lastArcValue = -1.f;
	double lastArcTime = 0.0;
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
	// Rendering audit/reference override; the shared texture path is the default.
	void setTextureArcRenderer(bool enabled);
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
