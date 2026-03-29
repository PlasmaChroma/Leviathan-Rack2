#include "TemporalDeck.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

#include <osdialog.h>

struct TemporalDeckDisplayWidget : Widget {
  TemporalDeck *module = nullptr;
  Vec centerMm = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;
  bool arcScrubbing = false;

  bool isWithinSampleSeekArc(Vec panelPos) const;
  void seekSampleFromArcPosition(Vec panelPos);
  Vec currentPanelMousePos() const;
  Vec sampleLoopIconCenter() const;
  bool isWithinSampleLoopIcon(Vec panelPos) const;

  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
  void onDragStart(const event::DragStart &e) override;
  void onDragMove(const event::DragMove &e) override;
  void onDragEnd(const event::DragEnd &e) override;
};

struct TemporalDeckBufferModeWidget : Widget {
  TemporalDeck *module = nullptr;
  Vec labelPosPx;

  void draw(const DrawArgs &args) override;
};

struct TemporalDeckTonearmWidget : Widget {
  TemporalDeck *module = nullptr;
  Vec centerPx = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;

  void draw(const DrawArgs &args) override;
};

struct TemporalDeckPlatterWidget : OpaqueWidget {
  static constexpr double kDefaultGestureDtSec = 1.0 / 60.0;
  static constexpr double kMinGestureDtSec = 1.0 / 240.0;
  static constexpr double kMaxGestureDtSec = 1.0 / 20.0;
  static constexpr float kLowFpsCompStartHz = 90.f;
  static constexpr float kLowFpsCompFullHz = 45.f;
  static constexpr float kLowFpsWarningOnHz = 55.f;
  static constexpr float kLowFpsWarningOffHz = 60.f;
  static constexpr float kLowFpsWarningSettleSec = 1.0f;
  static constexpr float kLowFpsWarningClearSec = 0.25f;

  struct InteractionTraceRecorder {
    bool active = false;
    std::ofstream file;
    std::string path;
    double startTimeSec = 0.0;
    uint64_t sequence = 0;
  };

  TemporalDeck *module = nullptr;
  Vec centerPx = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;
  float deadZonePx = 0.f;
  bool dragging = false;
  bool dragHasTiming = false;
  Vec onButtonPos;
  float contactAngle = 0.f;
  float contactRadiusPx = 0.f;
  float localLagSamples = 0.f;
  float filteredGestureVelocity = 0.f;
  double lastMoveTimeSec = 0.0;
  double recentGestureDtSec = kDefaultGestureDtSec;
  bool middleQuickSlipDown = false;
  float uiFpsEstimateHz = kLowFpsCompStartHz;
  float uiFpsDisplayHz = kLowFpsCompStartHz;
  bool lowFpsWarningActive = false;
  float lowFpsWarningLowAccumSec = 0.f;
  float lowFpsWarningHighAccumSec = 0.f;
  float uiFpsDisplayAccumSec = 0.f;
  double lastUiFpsUpdateSec = 0.0;
  InteractionTraceRecorder traceRecorder;

  Vec localCenter() const { return centerPx.minus(box.pos); }
  Vec currentLocalMousePos() const;
  float wheelRadiusGainForLocal(Vec local) const;
  float normalizeWheelScrollDelta(float rawScroll) const;
  void updateUiFpsTracking();
  float lowFpsCompensationFactor() const;
  void drawLowFpsWarning(const DrawArgs &args, Vec center) const;

  bool isWithinPlatter(Vec panelPos) const {
    Vec local = panelPos.minus(localCenter());
    float radius = local.norm();
    return radius <= platterRadiusPx;
  }

  void updateScratchFromLocal(Vec local, Vec mouseDelta);
  void pollMiddleQuickSlipTrigger();
  void syncTraceCaptureState();
  void startTraceCapture();
  void stopTraceCapture();
  void logTraceEvent(const char *eventName, Vec local, Vec mouseDelta, float scroll, float deltaAngle, float lagDelta,
                     float liveLag, float localLag, float velocity);

  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
  void onHoverScroll(const event::HoverScroll &e) override;
  void onDragMove(const event::DragMove &e) override;
  void onDragStart(const event::DragStart &e) override;
  void onDragEnd(const event::DragEnd &e) override;
  ~TemporalDeckPlatterWidget() override { stopTraceCapture(); }
};

static bool loadSvgCircleMm(const std::string &svgPath, const std::string &circleId, Vec *outCenterMm,
                            float *outRadiusMm) {
  std::ifstream svgFile(svgPath);
  if (!svgFile.good()) {
    return false;
  }
  std::ostringstream svgBuffer;
  svgBuffer << svgFile.rdbuf();
  const std::string svgText = svgBuffer.str();

  const std::regex tagRe("<circle\\b[^>]*\\bid\\s*=\\s*\"" + circleId + "\"[^>]*/?>", std::regex::icase);
  std::smatch tagMatch;
  if (!std::regex_search(svgText, tagMatch, tagRe) || tagMatch.empty()) {
    return false;
  }

  const std::string tag = tagMatch.str(0);
  auto parseAttr = [&](const char *attr, float *out) {
    const std::regex attrRe(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
    std::smatch attrMatch;
    if (!std::regex_search(tag, attrMatch, attrRe)) {
      return false;
    }
    *out = std::stof(attrMatch.str(1));
    return true;
  };

  float cxMm = 0.f;
  float cyMm = 0.f;
  float radiusMm = 0.f;
  if (!parseAttr("cx", &cxMm) || !parseAttr("cy", &cyMm) || !parseAttr("r", &radiusMm)) {
    return false;
  }

  *outCenterMm = Vec(cxMm, cyMm);
  *outRadiusMm = radiusMm;
  return true;
}

static bool loadPlatterAnchor(Vec &centerPx, float &radiusPx) {
  Vec centerMm;
  float radiusMm = 0.f;
  if (!loadSvgCircleMm(asset::plugin(pluginInstance, "res/deck.svg"), "PLATTER_AREA", &centerMm, &radiusMm)) {
    return false;
  }
  centerPx = mm2px(centerMm);
  radiusPx = mm2px(Vec(radiusMm, 0.f)).x;
  return true;
}

static bool isLeftMouseDown() {
  return APP && APP->window && APP->window->win &&
         glfwGetMouseButton(APP->window->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

static bool isMiddleMouseDown() {
  return APP && APP->window && APP->window->win &&
         glfwGetMouseButton(APP->window->win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
}

Vec TemporalDeckPlatterWidget::currentLocalMousePos() const {
  if (!parent || !APP || !APP->scene || !APP->scene->rack) {
    return Vec(0.f, 0.f);
  }
  return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos).minus(localCenter());
}

float TemporalDeckPlatterWidget::wheelRadiusGainForLocal(Vec local) const {
  float radiusNorm = clamp(local.norm() / std::max(platterRadiusPx, 1e-3f), 0.f, 1.f);
  constexpr float kWheelCenterGain = 1.75f;
  constexpr float kWheelEdgeGain = 0.72f;
  return crossfade(kWheelCenterGain, kWheelEdgeGain, std::sqrt(radiusNorm));
}

float TemporalDeckPlatterWidget::normalizeWheelScrollDelta(float rawScroll) const {
  float absRaw = std::fabs(rawScroll);
  if (absRaw >= 8.f) {
    // Some OS/device combinations deliver wheel motion in coarse line-based
    // units (e.g. +/-50 per notch) instead of notch-like +/-1 steps.
    rawScroll /= 50.f;
  }
  return clamp(rawScroll, -4.f, 4.f);
}

void TemporalDeckPlatterWidget::updateUiFpsTracking() {
  double nowSec = system::getTime();
  double dtSec = 0.0;
  if (lastUiFpsUpdateSec > 0.0) {
    dtSec = nowSec - lastUiFpsUpdateSec;
  }
  lastUiFpsUpdateSec = nowSec;
  dtSec = std::max(0.0, std::min(dtSec, 0.25));

  double frameDuration = (APP && APP->window) ? APP->window->getLastFrameDuration() : NAN;
  if (std::isfinite(frameDuration) && frameDuration > 1e-4) {
    float instFps = clamp(float(1.0 / frameDuration), 1.f, 480.f);
    constexpr float kFpsEmaAlpha = 0.18f;
    uiFpsEstimateHz += (instFps - uiFpsEstimateHz) * kFpsEmaAlpha;
  }

  if (dtSec > 0.0) {
    uiFpsDisplayAccumSec += float(dtSec);
    if (uiFpsDisplayAccumSec >= 1.f) {
      uiFpsDisplayHz = uiFpsEstimateHz;
      uiFpsDisplayAccumSec = 0.f;
    }
  }

  float fps = std::max(uiFpsEstimateHz, 1.f);
  if (dtSec > 0.0) {
    if (fps < kLowFpsWarningOnHz) {
      lowFpsWarningLowAccumSec += float(dtSec);
      lowFpsWarningHighAccumSec = 0.f;
      if (lowFpsWarningLowAccumSec >= kLowFpsWarningSettleSec) {
        lowFpsWarningActive = true;
      }
    } else if (fps > kLowFpsWarningOffHz) {
      lowFpsWarningHighAccumSec += float(dtSec);
      lowFpsWarningLowAccumSec = 0.f;
      if (lowFpsWarningHighAccumSec >= kLowFpsWarningClearSec) {
        lowFpsWarningActive = false;
      }
    }
  }
}

float TemporalDeckPlatterWidget::lowFpsCompensationFactor() const {
  float denom = std::max(kLowFpsCompStartHz - kLowFpsCompFullHz, 1.f);
  return clamp((kLowFpsCompStartHz - uiFpsEstimateHz) / denom, 0.f, 1.f);
}

void TemporalDeckPlatterWidget::drawLowFpsWarning(const DrawArgs &args, Vec center) const {
  if (!lowFpsWarningActive || !APP || !APP->window || !APP->window->uiFont) {
    return;
  }
  (void)center;
  // Place warning above S.GATE, in the gap between platter edge and jack row.
  Vec warningPos = mm2px(Vec(73.0f, 70.0f));
  nvgSave(args.vg);
  nvgFontFaceId(args.vg, APP->window->uiFont->handle);
  nvgFontSize(args.vg, 10.f);
  nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgFillColor(args.vg, nvgRGBA(255, 178, 82, 240));
  nvgText(args.vg, warningPos.x, warningPos.y, string::f("LOW UI FPS: %.0f", uiFpsDisplayHz).c_str(), nullptr);
  nvgRestore(args.vg);
}

void TemporalDeckPlatterWidget::pollMiddleQuickSlipTrigger() {
  bool middleDown = isMiddleMouseDown();
  if (!middleDown) {
    middleQuickSlipDown = false;
    return;
  }
  if (middleQuickSlipDown || !module || !parent || !APP || !APP->scene || !APP->scene->rack) {
    return;
  }

  Vec panelPos = APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
  if (isWithinPlatter(panelPos)) {
    module->triggerQuickSlipReturn();
    middleQuickSlipDown = true;
  }
}

void TemporalDeckPlatterWidget::syncTraceCaptureState() {
  if (!module) {
    stopTraceCapture();
    return;
  }
  if (!module->isPlatterTraceLoggingEnabled()) {
    stopTraceCapture();
    return;
  }
  startTraceCapture();
}

void TemporalDeckPlatterWidget::startTraceCapture() {
  if (!module || traceRecorder.active) {
    return;
  }
  std::string traceDir = system::join(asset::user(), "TemporalDeck/platter_traces");
  system::createDirectories(traceDir);
  long long stampMs = (long long)std::llround(system::getUnixTime() * 1000.0);
  std::string filename = "platter_trace_" + std::to_string(stampMs) + ".csv";
  traceRecorder.path = system::join(traceDir, filename);
  traceRecorder.file.open(traceRecorder.path.c_str(), std::ios::out | std::ios::trunc);
  if (!traceRecorder.file.good()) {
    WARN("TemporalDeck: failed to open platter trace file: %s", traceRecorder.path.c_str());
    traceRecorder.path.clear();
    return;
  }
  traceRecorder.file.setf(std::ios::fixed);
  traceRecorder.file << std::setprecision(6);
  traceRecorder.file << "# TemporalDeck platter interaction trace v2\n";
  traceRecorder.file << "# Start when platter trace logging is enabled, stop when it is disabled\n";
  traceRecorder.file
    << "seq,t_sec,event,freeze,sample_mode,sample_loop,sample_playing,dragging,x,y,radius_px,radius_norm,"
       "mouse_angle,radius_gain,dx,dy,scroll,delta_angle,lag_delta,live_lag,local_lag,accessible_lag,velocity,"
       "sample_playhead_sec,sample_progress,ui_platter_angle,sensitivity,sample_rate\n";
  traceRecorder.startTimeSec = system::getTime();
  traceRecorder.sequence = 0;
  traceRecorder.active = true;
  INFO("TemporalDeck: platter trace capture started: %s", traceRecorder.path.c_str());
}

void TemporalDeckPlatterWidget::stopTraceCapture() {
  if (!traceRecorder.active) {
    return;
  }
  if (traceRecorder.file.good()) {
    traceRecorder.file.flush();
    traceRecorder.file.close();
  }
  INFO("TemporalDeck: platter trace capture saved: %s", traceRecorder.path.c_str());
  traceRecorder.active = false;
  traceRecorder.startTimeSec = 0.0;
  traceRecorder.sequence = 0;
  traceRecorder.path.clear();
}

void TemporalDeckPlatterWidget::logTraceEvent(const char *eventName, Vec local, Vec mouseDelta, float scroll,
                                              float deltaAngle, float lagDelta, float liveLag, float localLag,
                                              float velocity) {
  if (!module || !traceRecorder.active || !traceRecorder.file.good()) {
    return;
  }
  float radiusPx = local.norm();
  float radiusNorm = clamp(radiusPx / std::max(platterRadiusPx, 1e-3f), 0.f, 1.f);
  float mouseAngle = (radiusPx > 1e-4f) ? std::atan2(local.y, local.x) : 0.f;
  bool sampleMode = module->isSampleModeEnabled() && module->hasLoadedSample();
  float radiusGain = wheelRadiusGainForLocal(local);
  double tSec = std::max(0.0, system::getTime() - traceRecorder.startTimeSec);
  traceRecorder.file << traceRecorder.sequence++ << "," << tSec << "," << eventName << ","
                     << (module->isUiFreezeLatched() ? 1 : 0) << "," << (sampleMode ? 1 : 0) << ","
                     << (module->isSampleLoopEnabled() ? 1 : 0) << ","
                     << (module->isSampleTransportPlaying() ? 1 : 0) << "," << (dragging ? 1 : 0) << ","
                     << local.x << "," << local.y << "," << radiusPx << "," << radiusNorm << "," << mouseAngle
                     << "," << radiusGain << "," << mouseDelta.x << "," << mouseDelta.y << "," << scroll << ","
                     << deltaAngle << "," << lagDelta << "," << liveLag << "," << localLag << ","
                     << module->getUiAccessibleLagSamples() << "," << velocity << ","
                     << module->getUiSamplePlayheadSeconds() << "," << module->getUiSampleProgress() << ","
                     << module->getUiPlatterAngle() << "," << module->scratchSensitivity() << ","
                     << module->getUiSampleRate() << "\n";
}

static std::string formatSecondsPrecise(double seconds) {
  seconds = std::max(0.0, seconds);
  return string::f("%.3f s", seconds);
}

static bool writePlatterSvgSnapshot(const std::string &path, float platterRadiusPx, float rotationRad,
                                    std::string *errorOut) {
  std::ofstream out(path.c_str(), std::ios::out | std::ios::trunc);
  if (!out.good()) {
    if (errorOut) {
      *errorOut = "Failed to open output file for writing";
    }
    return false;
  }
  out.setf(std::ios::fixed);
  out << std::setprecision(3);

  float margin = 2.0f;
  float width = platterRadiusPx * 2.f + margin * 2.f;
  float height = width;
  float cx = width * 0.5f;
  float cy = height * 0.5f;
  float labelRadius = platterRadiusPx * 0.33f;
  float postRadius = labelRadius * 0.12f;
  float rotationDeg = rotationRad * (180.f / float(M_PI));

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height
      << "\" viewBox=\"0 0 " << width << " " << height << "\">\n";
  out << "  <defs>\n";
  out << "    <radialGradient id=\"vinylGrad\" gradientUnits=\"userSpaceOnUse\" cx=\"" << (cx - platterRadiusPx * 0.08f)
      << "\" cy=\"" << (cy - platterRadiusPx * 0.10f) << "\" r=\"" << (platterRadiusPx * 1.02f)
      << "\" fx=\"" << (cx - platterRadiusPx * 0.08f) << "\" fy=\"" << (cy - platterRadiusPx * 0.10f) << "\">\n";
  out << "      <stop offset=\"0%\" stop-color=\"rgb(38,41,46)\" stop-opacity=\"" << (208.f / 255.f) << "\"/>\n";
  out << "      <stop offset=\"100%\" stop-color=\"rgb(24,26,30)\" stop-opacity=\"1\"/>\n";
  out << "    </radialGradient>\n";
  out << "    <linearGradient id=\"spindleGrad\" gradientUnits=\"userSpaceOnUse\" x1=\"" << cx << "\" y1=\""
      << (cy - postRadius) << "\" x2=\"" << cx << "\" y2=\"" << (cy + postRadius) << "\">\n";
  out << "      <stop offset=\"0%\" stop-color=\"rgb(218,223,229)\"/>\n";
  out << "      <stop offset=\"45%\" stop-color=\"rgb(168,174,183)\"/>\n";
  out << "      <stop offset=\"100%\" stop-color=\"rgb(92,98,107)\"/>\n";
  out << "    </linearGradient>\n";
  out << "  </defs>\n";

  out << "  <circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << platterRadiusPx << "\" fill=\"url(#vinylGrad)\"/>\n";

  out << "  <g transform=\"translate(" << cx << " " << cy << ") rotate(" << rotationDeg << ")\">\n";
  constexpr int kGrooveCount = 20;
  for (int i = 0; i < kGrooveCount; ++i) {
    float tNorm = float(i) / float(kGrooveCount - 1);
    float grooveRadius = platterRadiusPx * (0.24f + 0.705f * tNorm);
    float alpha = (i % 2 == 0) ? 34.f : 18.f;
    float wobbleAmp = 0.55f + 0.05f * float(i % 4);
    float wobblePhase = 0.47f * float(i) + 0.061f * float(i * i);
    float wobbleFreq = float(3 + ((i * 2 + 1) % 5)); // Integer harmonic for seamless closure.
    float ringRotation = 0.19f * float(i) + 0.043f * float(i * i);
    out << "    <path d=\"";
    constexpr int kSteps = 64;
    for (int step = 0; step < kSteps; ++step) {
      float t = 2.f * float(M_PI) * float(step) / float(kSteps) + ringRotation;
      float wobble = std::sin(t * wobbleFreq + wobblePhase);
      float radius = grooveRadius + wobbleAmp * wobble;
      float x = std::cos(t) * radius;
      float y = std::sin(t) * radius;
      out << (step == 0 ? "M " : " L ") << x << " " << y;
    }
    out << " Z\" fill=\"none\" stroke=\"rgb(210,218,228)\" stroke-opacity=\"" << (alpha / 255.f)
        << "\" stroke-width=\"0.7\" stroke-linejoin=\"round\" stroke-linecap=\"round\"/>\n";
  }
  out << "  </g>\n";

  out << "  <circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << labelRadius
      << "\" fill=\"rgb(90,178,187)\"/>\n";

  out << "  <g transform=\"translate(" << cx << " " << cy << ") rotate(" << rotationDeg << ")\">\n";
  out << "    <circle cx=\"0\" cy=\"0\" r=\"" << (labelRadius * 0.74f) << "\" fill=\"rgb(12,41,45)\"/>\n";
  for (int i = 0; i < 3; ++i) {
    float angle = 2.f * float(M_PI) * float(i) / 3.f;
    float ax = std::cos(angle) * labelRadius * 0.22f;
    float ay = std::sin(angle) * labelRadius * 0.22f;
    float bx = std::cos(angle) * labelRadius * 0.62f;
    float by = std::sin(angle) * labelRadius * 0.62f;
    out << "    <line x1=\"" << ax << "\" y1=\"" << ay << "\" x2=\"" << bx << "\" y2=\"" << by
        << "\" stroke=\"rgb(90,178,187)\" stroke-width=\"1.2\"/>\n";
  }
  out << "    <rect x=\"" << (-labelRadius * 0.42f) << "\" y=\"" << (-labelRadius * 0.055f) << "\" width=\""
      << (labelRadius * 0.84f) << "\" height=\"" << (labelRadius * 0.11f)
      << "\" rx=\"1.2\" ry=\"1.2\" fill=\"rgb(90,178,187)\" fill-opacity=\"" << (120.f / 255.f) << "\"/>\n";
  out << "  </g>\n";

  out << "  <circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << postRadius
      << "\" fill=\"url(#spindleGrad)\" stroke=\"rgb(56,61,68)\" stroke-opacity=\"" << (220.f / 255.f)
      << "\" stroke-width=\"0.9\"/>\n";
  out << "  <circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << (postRadius * 0.62f)
      << "\" fill=\"rgb(236,241,246)\" fill-opacity=\"" << (92.f / 255.f) << "\"/>\n";
  out << "  <circle cx=\"" << (cx - postRadius * 0.32f) << "\" cy=\"" << (cy - postRadius * 0.34f) << "\" r=\""
      << (postRadius * 0.20f) << "\" fill=\"rgb(255,255,255)\" fill-opacity=\"" << (132.f / 255.f) << "\"/>\n";
  out << "  <circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << platterRadiusPx
      << "\" fill=\"none\" stroke=\"rgb(255,255,255)\" stroke-opacity=\"" << (32.f / 255.f)
      << "\" stroke-width=\"1.1\"/>\n";
  out << "</svg>\n";
  if (!out.good()) {
    if (errorOut) {
      *errorOut = "Failed while writing SVG data";
    }
    return false;
  }
  return true;
}

static std::string ensureSvgExtension(std::string path) {
  std::string ext = system::getExtension(path);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return char(std::tolower(c)); });
  if (ext != ".svg") {
    path += ".svg";
  }
  return path;
}

static std::string lowercaseExtension(const std::string &path) {
  std::string ext = system::getExtension(path);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return char(std::tolower(c)); });
  return ext;
}

static bool isSupportedPlatterArtPath(const std::string &path) {
  std::string ext = lowercaseExtension(path);
  return ext == ".svg" || ext == ".png" || ext == ".jpg" || ext == ".jpeg";
}

static float platterDimmingOverlayAlphaForMode(int mode) {
  switch (mode) {
  case TemporalDeck::PLATTER_BRIGHTNESS_LOW:
    return 0.46f;
  case TemporalDeck::PLATTER_BRIGHTNESS_MEDIUM:
    return 0.28f;
  case TemporalDeck::PLATTER_BRIGHTNESS_FULL:
  default:
    return 0.f;
  }
}

static bool drawPlatterSvg(const Widget::DrawArgs &args, std::shared_ptr<window::Svg> svg, Vec center,
                           float platterRadiusPx, float rotation) {
  if (!svg) {
    return false;
  }
  Vec svgSize = svg->getSize();
  if (svgSize.x <= 1.f || svgSize.y <= 1.f) {
    return false;
  }
  constexpr float kPlatterSvgMarginPx = 2.0f;
  float sourceRadius = std::max(1.f, 0.5f * std::min(svgSize.x, svgSize.y) - kPlatterSvgMarginPx);
  float scale = platterRadiusPx / sourceRadius;

  nvgSave(args.vg);
  nvgTranslate(args.vg, center.x, center.y);
  nvgRotate(args.vg, rotation);
  nvgScale(args.vg, scale, scale);
  nvgTranslate(args.vg, -svgSize.x * 0.5f, -svgSize.y * 0.5f);
  svg->draw(args.vg);
  nvgRestore(args.vg);
  return true;
}

static bool drawPlatterImage(const Widget::DrawArgs &args, std::shared_ptr<window::Image> image, Vec center,
                             float platterRadiusPx, float rotation) {
  if (!image || image->handle < 0) {
    return false;
  }
  int imageW = 0;
  int imageH = 0;
  nvgImageSize(args.vg, image->handle, &imageW, &imageH);
  if (imageW <= 0 || imageH <= 0) {
    return false;
  }
  float diameter = platterRadiusPx * 2.f;
  float scale = diameter / std::max(1.f, float(std::min(imageW, imageH)));
  float drawW = float(imageW) * scale;
  float drawH = float(imageH) * scale;

  nvgSave(args.vg);
  nvgTranslate(args.vg, center.x, center.y);
  nvgRotate(args.vg, rotation);
  NVGpaint imgPaint = nvgImagePattern(args.vg, -drawW * 0.5f, -drawH * 0.5f, drawW, drawH, 0.f, image->handle, 1.0f);
  nvgBeginPath(args.vg);
  nvgCircle(args.vg, 0.f, 0.f, platterRadiusPx);
  nvgFillPaint(args.vg, imgPaint);
  nvgFill(args.vg);
  nvgRestore(args.vg);
  return true;
}

static void drawPlatterDimmingOverlay(const Widget::DrawArgs &args, Vec center, float platterRadiusPx, float overlayAlpha) {
  overlayAlpha = clamp(overlayAlpha, 0.f, 1.f);
  if (overlayAlpha <= 1e-4f) {
    return;
  }
  unsigned char innerA = (unsigned char)std::round(clamp(overlayAlpha * 0.80f, 0.f, 1.f) * 255.f);
  unsigned char outerA = (unsigned char)std::round(clamp(overlayAlpha * 1.05f, 0.f, 1.f) * 255.f);
  NVGpaint dimPaint =
    nvgRadialGradient(args.vg, center.x - platterRadiusPx * 0.08f, center.y - platterRadiusPx * 0.10f,
                      platterRadiusPx * 0.10f, platterRadiusPx * 1.04f, nvgRGBA(0, 0, 0, innerA),
                      nvgRGBA(0, 0, 0, outerA));
  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
  nvgFillPaint(args.vg, dimPaint);
  nvgFill(args.vg);
}

static bool topArcAngleFromLocal(Vec local, float *angleOut) {
  float angle = std::atan2(local.y, local.x);
  constexpr float kEndpointEpsilon = 0.10f;
  if (angle > 0.f) {
    // atan2 can report near +pi / +0 at endpoints depending on tiny y jitter.
    if (angle >= float(M_PI) - kEndpointEpsilon) {
      angle = -float(M_PI);
    } else if (angle <= kEndpointEpsilon) {
      angle = 0.f;
    } else {
      return false;
    }
  }
  if (angleOut) {
    *angleOut = angle;
  }
  return angle <= 0.f && angle >= -float(M_PI);
}

bool TemporalDeckDisplayWidget::isWithinSampleSeekArc(Vec panelPos) const {
  Vec local = panelPos.minus(centerMm);
  float arcRadius = platterRadiusPx + mm2px(Vec(3.5f, 0.f)).x;
  float arcHalfWidth = mm2px(Vec(3.2f, 0.f)).x;
  float r = local.norm();
  if (std::fabs(r - arcRadius) > arcHalfWidth) {
    return false;
  }
  float angle = 0.f;
  return topArcAngleFromLocal(local, &angle);
}

void TemporalDeckDisplayWidget::seekSampleFromArcPosition(Vec panelPos) {
  if (!module || !module->isSampleModeEnabled() || !module->hasLoadedSample()) {
    return;
  }
  Vec local = panelPos.minus(centerMm);
  float angle = 0.f;
  if (!topArcAngleFromLocal(local, &angle)) {
    return;
  }
  float arcT = clamp(-angle / float(M_PI), 0.f, 1.f); // right->left along top arc
  float seekNorm = 1.f - arcT;                         // sample mode maps left->right as start->end
  module->seekSampleByNormalizedPosition(seekNorm);
}

Vec TemporalDeckDisplayWidget::currentPanelMousePos() const {
  if (!parent || !APP || !APP->scene || !APP->scene->rack) {
    return Vec();
  }
  return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
}

Vec TemporalDeckDisplayWidget::sampleLoopIconCenter() const {
  float arcRadius = platterRadiusPx + mm2px(Vec(3.5f, 0.f)).x;
  float textX = centerMm.x + arcRadius + mm2px(Vec(8.0f, 0.f)).x;
  float topY = centerMm.y - arcRadius * 1.02f;
  float bottomY = topY + 12.4f;
  return Vec(textX - 4.3f, bottomY + 10.4f);
}

bool TemporalDeckDisplayWidget::isWithinSampleLoopIcon(Vec panelPos) const {
  Vec c = sampleLoopIconCenter();
  return std::fabs(panelPos.x - c.x) <= 7.2f && std::fabs(panelPos.y - c.y) <= 4.8f;
}

void TemporalDeckDisplayWidget::draw(const DrawArgs &args) {
  if (!module) {
    return;
  }
  double accessibleLag = std::max(1.0, module->getUiAccessibleLagSamples());
  double lag = std::max(0.0, std::min(module->getUiLagSamples(), accessibleLag));
  nvgSave(args.vg);
  float arcRadius = platterRadiusPx + mm2px(Vec(3.5f, 0.f)).x;

  if (APP && APP->window && APP->window->uiFont) {
    bool sampleDisplay = module->isSampleModeEnabled() && module->hasLoadedSample();
    if (!sampleDisplay) {
      std::string displayText;
      double lagMs = 1000.0 * lag / std::max(module->getUiSampleRate(), 1.f);
      displayText = string::f("%.0f ms", lagMs);
      // Keep readouts above the arc LED strip so they don't visually collide.
      Vec textPos = centerMm.plus(Vec(arcRadius + mm2px(Vec(8.0f, 0.f)).x, -arcRadius * 1.02f));
      nvgFontFaceId(args.vg, APP->window->uiFont->handle);
      nvgFontSize(args.vg, 11.5f);
      nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
      nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
      nvgText(args.vg, textPos.x, textPos.y, displayText.c_str(), nullptr);
    } else {
      // Sample mode: show current/total as a stacked fraction in seconds.
      float textX = centerMm.x + arcRadius + mm2px(Vec(8.0f, 0.f)).x;
      // Align "current" to the same baseline as live-mode millisecond readout.
      float topY = centerMm.y - arcRadius * 1.02f;
      float dividerY = topY + 6.2f;
      float bottomY = dividerY + 6.2f;
      std::string currentText = formatSecondsPrecise(module->getUiSamplePlayheadSeconds());
      std::string totalText = formatSecondsPrecise(module->getUiSampleDurationSeconds());

      nvgFontFaceId(args.vg, APP->window->uiFont->handle);
      nvgFontSize(args.vg, 10.4f);
      nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
      nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
      nvgText(args.vg, textX, topY, currentText.c_str(), nullptr);
      nvgText(args.vg, textX, bottomY, totalText.c_str(), nullptr);

      float boundsTop[4] = {};
      float boundsBottom[4] = {};
      float widthTop = nvgTextBounds(args.vg, textX, topY, currentText.c_str(), nullptr, boundsTop);
      float widthBottom = nvgTextBounds(args.vg, textX, bottomY, totalText.c_str(), nullptr, boundsBottom);
      float lineWidth = std::max(widthTop, widthBottom) + 2.0f;
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, textX - lineWidth, dividerY);
      nvgLineTo(args.vg, textX + 0.8f, dividerY);
      nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 210));
      nvgStrokeWidth(args.vg, 1.0f);
      nvgStroke(args.vg);

      bool loopEnabled = module->isSampleLoopEnabled();
      NVGcolor loopColor = loopEnabled ? nvgRGBA(255, 255, 255, 255) : nvgRGBA(120, 120, 120, 255);
      Vec loopCenter = sampleLoopIconCenter();
      float loopHalfW = 4.25f;
      float loopHalfH = 2.55f;
      float neck = 1.7f;
      nvgStrokeColor(args.vg, loopColor);
      nvgStrokeWidth(args.vg, 1.2f);
      nvgLineCap(args.vg, NVG_ROUND);
      nvgLineJoin(args.vg, NVG_ROUND);
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, loopCenter.x - loopHalfW, loopCenter.y);
      nvgBezierTo(args.vg, loopCenter.x - loopHalfW, loopCenter.y - loopHalfH, loopCenter.x - neck, loopCenter.y - loopHalfH,
                  loopCenter.x, loopCenter.y);
      nvgBezierTo(args.vg, loopCenter.x + neck, loopCenter.y + loopHalfH, loopCenter.x + loopHalfW, loopCenter.y + loopHalfH,
                  loopCenter.x + loopHalfW, loopCenter.y);
      nvgBezierTo(args.vg, loopCenter.x + loopHalfW, loopCenter.y - loopHalfH, loopCenter.x + neck, loopCenter.y - loopHalfH,
                  loopCenter.x, loopCenter.y);
      nvgBezierTo(args.vg, loopCenter.x - neck, loopCenter.y + loopHalfH, loopCenter.x - loopHalfW, loopCenter.y + loopHalfH,
                  loopCenter.x - loopHalfW, loopCenter.y);
      nvgStroke(args.vg);

    }
  }
  nvgRestore(args.vg);
}

void TemporalDeckDisplayWidget::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (e.action == GLFW_PRESS && module && module->isSampleModeEnabled() && module->hasLoadedSample() &&
        isWithinSampleLoopIcon(e.pos)) {
      module->setSampleLoopEnabled(!module->isSampleLoopEnabled());
      e.consume(this);
      return;
    }
    if (e.action == GLFW_PRESS && module && module->isSampleModeEnabled() && module->hasLoadedSample() &&
        isWithinSampleSeekArc(e.pos)) {
      arcScrubbing = true;
      seekSampleFromArcPosition(e.pos);
      e.consume(this);
      return;
    }
    if (e.action == GLFW_RELEASE && arcScrubbing) {
      arcScrubbing = false;
      e.consume(this);
      return;
    }
  }
  Widget::onButton(e);
}

void TemporalDeckDisplayWidget::onDragStart(const event::DragStart &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && arcScrubbing) {
    seekSampleFromArcPosition(currentPanelMousePos());
    e.consume(this);
    return;
  }
  Widget::onDragStart(e);
}

void TemporalDeckDisplayWidget::onDragMove(const event::DragMove &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && arcScrubbing) {
    seekSampleFromArcPosition(currentPanelMousePos());
    e.consume(this);
    return;
  }
  Widget::onDragMove(e);
}

void TemporalDeckDisplayWidget::onDragEnd(const event::DragEnd &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && arcScrubbing) {
    arcScrubbing = false;
    e.consume(this);
    return;
  }
  Widget::onDragEnd(e);
}

void TemporalDeckBufferModeWidget::draw(const DrawArgs &args) {
  if (!module || !module->isBufferModeMono()) {
    return;
  }
  if (!APP || !APP->window || !APP->window->uiFont) {
    return;
  }
  nvgSave(args.vg);
  nvgFontFaceId(args.vg, APP->window->uiFont->handle);
  nvgFontSize(args.vg, 10.0f);
  nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgFillColor(args.vg, nvgRGBA(206, 86, 255, 245));
  nvgText(args.vg, labelPosPx.x, labelPosPx.y, "mono", nullptr);
  nvgRestore(args.vg);
}

void TemporalDeckTonearmWidget::draw(const DrawArgs &args) {
  nvgSave(args.vg);
  Vec center = centerPx;
  Vec armPivot = center.plus(Vec(platterRadiusPx * 1.12f, platterRadiusPx * 0.34f));
  Vec stylusTip = center.plus(Vec(platterRadiusPx * 0.62f, platterRadiusPx * 0.64f));
  float platterPhase = module ? module->getUiPlatterAngle() : 0.f;
  float wobbleAngle = 0.012f * std::sin(platterPhase * 0.31f) + 0.006f * std::sin(platterPhase * 0.77f + 0.8f);
  auto rotateAroundPivot = [&](Vec p) {
    Vec rel = p.minus(armPivot);
    float c = std::cos(wobbleAngle);
    float s = std::sin(wobbleAngle);
    return armPivot.plus(Vec(rel.x * c - rel.y * s, rel.x * s + rel.y * c));
  };
  stylusTip = rotateAroundPivot(stylusTip);
  Vec armDir = stylusTip.minus(armPivot);
  float armLen = armDir.norm();
  if (armLen > 1e-4f) {
    armDir = armDir.div(armLen);
    Vec armNormal(-armDir.y, armDir.x);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, armPivot.x, armPivot.y, mm2px(Vec(5.3f, 0.f)).x);
    nvgFillColor(args.vg, nvgRGBA(28, 31, 36, 236));
    nvgFill(args.vg);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, armPivot.x, armPivot.y, mm2px(Vec(3.0f, 0.f)).x);
    nvgFillColor(args.vg, nvgRGBA(142, 148, 156, 235));
    nvgFill(args.vg);

    Vec counterweight = armPivot.minus(armDir.mult(mm2px(Vec(4.2f, 0.f)).x));
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, counterweight.x - mm2px(Vec(2.2f, 0.f)).x, counterweight.y - mm2px(Vec(1.6f, 0.f)).x,
                   mm2px(Vec(4.4f, 0.f)).x, mm2px(Vec(3.2f, 0.f)).x, 1.2f);
    nvgFillColor(args.vg, nvgRGBA(74, 78, 86, 255));
    nvgFill(args.vg);

    Vec armStart = armPivot;
    Vec shellBack = stylusTip.minus(armDir.mult(mm2px(Vec(4.0f, 0.f)).x));
    Vec armEnd = shellBack;
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, armStart.x, armStart.y);
    nvgLineTo(args.vg, armEnd.x, armEnd.y);
    nvgStrokeColor(args.vg, nvgRGBA(74, 80, 88, 236));
    nvgStrokeWidth(args.vg, 3.0f);
    nvgStroke(args.vg);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, armStart.x, armStart.y);
    nvgLineTo(args.vg, armEnd.x, armEnd.y);
    nvgStrokeColor(args.vg, nvgRGBA(186, 194, 204, 170));
    nvgStrokeWidth(args.vg, 0.9f);
    nvgStroke(args.vg);

    // Vaguely DJ-style headshell/cart assembly with mounting holes.
    Vec shellFront = stylusTip.minus(armDir.mult(mm2px(Vec(0.8f, 0.f)).x));
    Vec headshellA = shellBack.plus(armNormal.mult(mm2px(Vec(1.25f, 0.f)).x));
    Vec headshellB = shellBack.minus(armNormal.mult(mm2px(Vec(1.25f, 0.f)).x));
    Vec headshellC = shellFront.minus(armNormal.mult(mm2px(Vec(1.95f, 0.f)).x));
    Vec headshellD = shellFront.plus(armNormal.mult(mm2px(Vec(1.95f, 0.f)).x));
    CartridgeVisualStyle cartStyle = TemporalDeck::cartridgeVisualStyleFor(module ? module->getCartridgeCharacter() : 0);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, headshellA.x, headshellA.y);
    nvgLineTo(args.vg, headshellD.x, headshellD.y);
    nvgLineTo(args.vg, headshellC.x, headshellC.y);
    nvgLineTo(args.vg, headshellB.x, headshellB.y);
    nvgClosePath(args.vg);
    nvgFillColor(args.vg, cartStyle.shellFill);
    nvgFill(args.vg);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, headshellA.x, headshellA.y);
    nvgLineTo(args.vg, headshellD.x, headshellD.y);
    nvgLineTo(args.vg, headshellC.x, headshellC.y);
    nvgLineTo(args.vg, headshellB.x, headshellB.y);
    nvgClosePath(args.vg);
    nvgStrokeColor(args.vg, cartStyle.shellStroke);
    nvgStrokeWidth(args.vg, 0.85f);
    nvgStroke(args.vg);

    auto drawHole = [&](Vec p, float rMm) {
      nvgBeginPath(args.vg);
      nvgCircle(args.vg, p.x, p.y, mm2px(Vec(rMm, 0.f)).x);
      nvgFillColor(args.vg, cartStyle.holeFill);
      nvgFill(args.vg);
    };
    drawHole(shellBack.plus(armDir.mult(mm2px(Vec(1.2f, 0.f)).x)), 0.38f);
    drawHole(shellBack.plus(armDir.mult(mm2px(Vec(2.2f, 0.f)).x)).plus(armNormal.mult(mm2px(Vec(0.72f, 0.f)).x)), 0.32f);
    drawHole(shellBack.plus(armDir.mult(mm2px(Vec(2.2f, 0.f)).x)).minus(armNormal.mult(mm2px(Vec(0.72f, 0.f)).x)), 0.32f);
  }
  if (module && APP && APP->window && APP->window->uiFont) {
    Vec labelPos = armPivot.plus(Vec(0.f, mm2px(Vec(7.8f, 0.f)).x));
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 11.f);
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgText(args.vg, labelPos.x, labelPos.y, TemporalDeck::cartridgeLabelFor(module->getCartridgeCharacter()), nullptr);
  }
  nvgRestore(args.vg);
}

void TemporalDeckPlatterWidget::draw(const DrawArgs &args) {
  syncTraceCaptureState();
  pollMiddleQuickSlipTrigger();
  updateUiFpsTracking();
  nvgSave(args.vg);
  float rotation = module ? module->getUiPlatterAngle() : 0.f;
  Vec center = localCenter();

  // Primary platter render path is selectable: built-in SVG, custom file, or
  // procedural fallback.
  bool drewArt = false;
  float platterDimAlpha = 0.f;
  if (APP && APP->window) {
    // In module-browser preview there is no backing module instance, so pick a
    // deterministic default art instead of falling through to procedural.
    int artMode = module ? module->getPlatterArtMode() : TemporalDeck::PLATTER_ART_DRAGON_KING;
    platterDimAlpha = module ? platterDimmingOverlayAlphaForMode(module->getPlatterBrightnessMode()) : 0.f;
    if (artMode == TemporalDeck::PLATTER_ART_BUILTIN_SVG) {
      try {
        drewArt = drawPlatterSvg(args, APP->window->loadSvg(asset::plugin(pluginInstance, "res/Vinyl/Static.svg")),
                                 center, platterRadiusPx, rotation);
      } catch (const std::exception &e) {
        WARN("TemporalDeck: failed to load built-in platter SVG asset: %s", e.what());
      }
    } else if (artMode == TemporalDeck::PLATTER_ART_DRAGON_KING) {
      try {
        drewArt = drawPlatterImage(args, APP->window->loadImage(asset::plugin(pluginInstance, "res/Vinyl/DragonKingPlatter.png")),
                                   center, platterRadiusPx, rotation);
      } catch (const std::exception &e) {
        WARN("TemporalDeck: failed to load Dragon King platter PNG asset: %s", e.what());
      }
    } else if (artMode == TemporalDeck::PLATTER_ART_BLANK) {
      try {
        drewArt = drawPlatterImage(args, APP->window->loadImage(asset::plugin(pluginInstance, "res/Vinyl/Blank.png")),
                                   center, platterRadiusPx, rotation);
      } catch (const std::exception &e) {
        WARN("TemporalDeck: failed to load Blank platter PNG asset: %s", e.what());
      }
    } else if (artMode == TemporalDeck::PLATTER_ART_CUSTOM) {
      if (module) {
        std::string customPath = module->getCustomPlatterArtPath();
        std::string ext = lowercaseExtension(customPath);
        try {
          if (ext == ".svg") {
            drewArt = drawPlatterSvg(args, APP->window->loadSvg(customPath), center, platterRadiusPx, rotation);
          } else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
            drewArt = drawPlatterImage(args, APP->window->loadImage(customPath), center, platterRadiusPx, rotation);
          }
        } catch (const std::exception &e) {
          WARN("TemporalDeck: failed to load custom platter art '%s': %s", customPath.c_str(), e.what());
        }
      }
    }
    if (drewArt) {
      drawPlatterDimmingOverlay(args, center, platterRadiusPx, platterDimAlpha);
      drawLowFpsWarning(args, center);
      nvgRestore(args.vg);
      Widget::draw(args);
      return;
    }
  }

  NVGcolor outerDark = nvgRGB(20, 22, 26);
  NVGpaint vinylGrad =
    nvgRadialGradient(args.vg, center.x - platterRadiusPx * 0.18f, center.y - platterRadiusPx * 0.22f,
                      platterRadiusPx * 0.15f, platterRadiusPx * 1.05f, nvgRGBA(52, 56, 64, 220), outerDark);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
  nvgFillPaint(args.vg, vinylGrad);
  nvgFill(args.vg);

  // Optimization: Skip complex grooves if platter is too small to see them
  // clearly
  if (platterRadiusPx > 10.f) {
    nvgSave(args.vg);
    nvgTranslate(args.vg, center.x, center.y);
    nvgRotate(args.vg, rotation);

    constexpr int kGrooveCount = 20;
    for (int i = 0; i < kGrooveCount; ++i) {
      float tNorm = float(i) / float(kGrooveCount - 1);
      float grooveRadius = platterRadiusPx * (0.24f + 0.705f * tNorm);
      float alpha = (i % 2 == 0) ? 34.f : 18.f;
      float wobbleAmp = 0.55f + 0.05f * float(i % 4);
      float wobblePhase = 0.47f * float(i) + 0.061f * float(i * i);
      float wobbleFreq = float(3 + ((i * 2 + 1) % 5)); // Integer harmonic for seamless closure.
      float ringRotation = 0.19f * float(i) + 0.043f * float(i * i);

      nvgBeginPath(args.vg);
      constexpr int kSteps = 64;
      for (int step = 0; step < kSteps; ++step) {
        float t = 2.f * float(M_PI) * float(step) / float(kSteps) + ringRotation;
        float wobble = std::sin(t * wobbleFreq + wobblePhase);
        float radius = grooveRadius + wobbleAmp * wobble;
        float x = std::cos(t) * radius;
        float y = std::sin(t) * radius;
        if (step == 0)
          nvgMoveTo(args.vg, x, y);
        else
          nvgLineTo(args.vg, x, y);
      }
      nvgClosePath(args.vg);
      nvgLineJoin(args.vg, NVG_ROUND);
      nvgLineCap(args.vg, NVG_ROUND);
      nvgStrokeColor(args.vg, nvgRGBA(210, 218, 228, (unsigned char)alpha));
      nvgStrokeWidth(args.vg, 0.7f);
      nvgStroke(args.vg);
    }

    nvgRestore(args.vg);
  }

  float labelRadius = platterRadiusPx * 0.33f;
  float postRadius = labelRadius * 0.12f;
  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, labelRadius);
  nvgFillColor(args.vg, nvgRGB(90, 178, 187));
  nvgFill(args.vg);

  nvgSave(args.vg);
  nvgTranslate(args.vg, center.x, center.y);
  nvgRotate(args.vg, rotation);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, 0.f, 0.f, labelRadius * 0.74f);
  nvgFillColor(args.vg, nvgRGB(12, 41, 45));
  nvgFill(args.vg);

  for (int i = 0; i < 3; ++i) {
    float angle = 2.f * float(M_PI) * float(i) / 3.f;
    Vec a(std::cos(angle) * labelRadius * 0.22f, std::sin(angle) * labelRadius * 0.22f);
    Vec b(std::cos(angle) * labelRadius * 0.62f, std::sin(angle) * labelRadius * 0.62f);
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, a.x, a.y);
    nvgLineTo(args.vg, b.x, b.y);
    nvgStrokeColor(args.vg, nvgRGBA(90, 178, 187, 255));
    nvgStrokeWidth(args.vg, 1.2f);
    nvgStroke(args.vg);
  }

  nvgBeginPath(args.vg);
  nvgRoundedRect(args.vg, -labelRadius * 0.42f, -labelRadius * 0.055f, labelRadius * 0.84f, labelRadius * 0.11f, 1.2f);
  nvgFillColor(args.vg, nvgRGBA(90, 178, 187, 120));
  nvgFill(args.vg);

  nvgRestore(args.vg);

  NVGpaint postPaint = nvgLinearGradient(args.vg, center.x, center.y - postRadius, center.x, center.y + postRadius,
                                         nvgRGB(218, 223, 229), nvgRGB(92, 98, 107));
  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, postRadius);
  nvgFillPaint(args.vg, postPaint);
  nvgFill(args.vg);
  nvgStrokeColor(args.vg, nvgRGBA(56, 61, 68, 220));
  nvgStrokeWidth(args.vg, 0.9f);
  nvgStroke(args.vg);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, postRadius * 0.62f);
  nvgFillColor(args.vg, nvgRGBA(236, 241, 246, 92));
  nvgFill(args.vg);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x - postRadius * 0.32f, center.y - postRadius * 0.34f, postRadius * 0.20f);
  nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 132));
  nvgFill(args.vg);

  drawPlatterDimmingOverlay(args, center, platterRadiusPx, platterDimAlpha);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
  nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 32));
  nvgStrokeWidth(args.vg, 1.1f);
  nvgStroke(args.vg);

  drawLowFpsWarning(args, center);

  nvgRestore(args.vg);
  Widget::draw(args);
}

void TemporalDeckPlatterWidget::updateScratchFromLocal(Vec local, Vec mouseDelta) {
  if (!module || !dragging) {
    return;
  }
  syncTraceCaptureState();
  // Physical screen-space model:
  // lock a contact radius at drag start and only use tangential motion to move
  // the platter. Radial cursor drift is ignored instead of redefining the
  // platter angle directly from the current mouse position.
  constexpr float kScratchMoveThresholdPx = 0.05f;
  float effectiveRadius = std::max(contactRadiusPx, platterRadiusPx * 0.32f);
  if (effectiveRadius <= 1e-3f) {
    return;
  }
  double nowSec = system::getTime();
  double dtSec = dragHasTiming ? (nowSec - lastMoveTimeSec) : recentGestureDtSec;
  dtSec = std::max(kMinGestureDtSec, std::min(dtSec, kMaxGestureDtSec));
  recentGestureDtSec = dtSec;
  lastMoveTimeSec = nowSec;
  dragHasTiming = true;

  float localRadius = local.norm();
  bool useCursorAngle = localRadius > std::max(deadZonePx, platterRadiusPx * 0.16f);
  float deltaAngle = 0.f;
  if (useCursorAngle) {
    float localAngle = std::atan2(local.y, local.x);
    deltaAngle = platter_interaction::wrapSignedAngle(localAngle - contactAngle);
    float minAngleMotion = kScratchMoveThresholdPx / std::max(effectiveRadius, 1e-3f);
    if (std::fabs(deltaAngle) < minAngleMotion) {
      float settleAlpha = 1.f - std::exp(-2.f * float(M_PI) * 45.f * float(dtSec));
      filteredGestureVelocity += (0.f - filteredGestureVelocity) * settleAlpha;
      if (std::fabs(filteredGestureVelocity) < 1.f) {
        filteredGestureVelocity = 0.f;
      }
      module->setPlatterScratch(true, localLagSamples, filteredGestureVelocity);
      module->setPlatterMotionFreshSamples(0);
      logTraceEvent("SCRATCH_SETTLE", local, mouseDelta, 0.f, deltaAngle, 0.f, float(module->getUiLagSamples()),
                    localLagSamples, filteredGestureVelocity);
      return;
    }
    contactAngle = localAngle;
  } else {
    Vec radial(std::cos(contactAngle), std::sin(contactAngle));
    Vec tangent(-radial.y, radial.x);
    float tangentialPx = mouseDelta.x * tangent.x + mouseDelta.y * tangent.y;
    if (std::fabs(tangentialPx) < kScratchMoveThresholdPx) {
      float settleAlpha = 1.f - std::exp(-2.f * float(M_PI) * 45.f * float(dtSec));
      filteredGestureVelocity += (0.f - filteredGestureVelocity) * settleAlpha;
      if (std::fabs(filteredGestureVelocity) < 1.f) {
        filteredGestureVelocity = 0.f;
      }
      module->setPlatterScratch(true, localLagSamples, filteredGestureVelocity);
      module->setPlatterMotionFreshSamples(0);
      logTraceEvent("SCRATCH_SETTLE", local, mouseDelta, 0.f, 0.f, 0.f, float(module->getUiLagSamples()),
                    localLagSamples, filteredGestureVelocity);
      return;
    }
    deltaAngle = tangentialPx / effectiveRadius;
    contactAngle += deltaAngle;
  }
  // Always apply drag deltas to the engine's latest lag, not the last UI
  // event's cached lag. Use direction-aware rebasing so we don't collapse
  // accumulated hand motion when DSP smoothing lags behind dense UI events.
  double accessibleLag = module->getUiAccessibleLagSamples();
  float liveLag = clamp(float(module->getUiLagSamples()), 0.f, float(accessibleLag));
  bool freezeLikeDrag = true;
  float lowFpsComp = lowFpsCompensationFactor();
  float sensitivity = module->scratchSensitivity();
  float lagDelta = platter_interaction::lagDeltaFromAngle(deltaAngle, module->getUiSampleRate(), sensitivity,
                                                          TemporalDeck::kMouseScratchTravelScale,
                                                          TemporalDeck::kNominalPlatterRpm);
  if (!freezeLikeDrag) {
    localLagSamples = platter_interaction::rebaseLagTarget(localLagSamples, liveLag, lagDelta);
  }
  int substeps = clamp(1 + int(std::round(lowFpsComp * 6.f)), 1, 7);
  float lagDeltaStep = lagDelta / float(substeps);
  double stepDtSec = std::max(kMinGestureDtSec, dtSec / double(substeps));
  for (int i = 0; i < substeps; ++i) {
    if (module->isSampleModeEnabled() && module->hasLoadedSample() && module->isSampleLoopEnabled() && accessibleLag > 0.0) {
      double wrappedLag = std::fmod(double(localLagSamples - lagDeltaStep), accessibleLag + 1.0);
      if (wrappedLag < 0.0) {
        wrappedLag += accessibleLag + 1.0;
      }
      localLagSamples = float(wrappedLag);
    } else {
      localLagSamples = clamp(localLagSamples - lagDeltaStep, 0.f, accessibleLag);
    }

    float measuredVelocity = lagDeltaStep / float(stepDtSec);
    float velocityAlpha = 1.f - std::exp(-2.f * float(M_PI) * 30.f * float(stepDtSec));
    filteredGestureVelocity += (measuredVelocity - filteredGestureVelocity) * velocityAlpha;
  }
  module->setPlatterScratch(true, localLagSamples, filteredGestureVelocity);
  logTraceEvent("SCRATCH_APPLY", local, mouseDelta, 0.f, deltaAngle, lagDelta, liveLag, localLagSamples,
                filteredGestureVelocity);

  int motionFreshSamples = int(std::round(module->getUiSampleRate() * float(dtSec) * 1.35f));
  int minHoldSamples = int(std::round(module->getUiSampleRate() * crossfade(0.022f, 0.080f, lowFpsComp)));
  int maxHoldSamples = int(std::round(module->getUiSampleRate() * 0.090f));
  motionFreshSamples = std::max(motionFreshSamples, minHoldSamples);
  motionFreshSamples = clamp(motionFreshSamples, 1, maxHoldSamples);
  module->setPlatterMotionFreshSamples(motionFreshSamples);
  if (contactAngle > M_PI) {
    contactAngle -= 2.f * M_PI;
  }
  if (contactAngle < -M_PI) {
    contactAngle += 2.f * M_PI;
  }
}

void TemporalDeckPlatterWidget::onButton(const event::Button &e) {
  syncTraceCaptureState();
  onButtonPos = e.pos;
  if (e.button == GLFW_MOUSE_BUTTON_MIDDLE && isWithinPlatter(e.pos)) {
    if (e.action == GLFW_PRESS && module) {
      module->triggerQuickSlipReturn();
      middleQuickSlipDown = true;
    } else if (e.action == GLFW_RELEASE) {
      middleQuickSlipDown = false;
    }
    e.consume(this);
    return;
  }
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && isWithinPlatter(e.pos)) {
    Vec local = e.pos.minus(localCenter());
    if (e.action == GLFW_PRESS) {
      logTraceEvent("BUTTON_PRESS", local, Vec(0.f, 0.f), 0.f, 0.f, 0.f, float(module ? module->getUiLagSamples() : 0.f),
                    localLagSamples, filteredGestureVelocity);
      if (module) {
        localLagSamples = module->getUiLagSamples();
        filteredGestureVelocity = 0.f;
        dragHasTiming = false;
        recentGestureDtSec = kDefaultGestureDtSec;
        module->setPlatterScratch(true, localLagSamples, 0.f);
        module->setPlatterMotionFreshSamples(0);
      }
      e.consume(this);
      return;
    }
    if (e.action == GLFW_RELEASE && !dragging) {
      logTraceEvent("BUTTON_RELEASE", local, Vec(0.f, 0.f), 0.f, 0.f, 0.f,
                    float(module ? module->getUiLagSamples() : 0.f), localLagSamples, filteredGestureVelocity);
      if (module) {
        module->setPlatterScratch(false, localLagSamples, 0.f);
        module->setPlatterMotionFreshSamples(0);
      }
      e.consume(this);
      return;
    }
  }
  Widget::onButton(e);
}

void TemporalDeckPlatterWidget::onHoverScroll(const event::HoverScroll &e) {
  syncTraceCaptureState();
  if (!module || !isWithinPlatter(e.pos)) {
    OpaqueWidget::onHoverScroll(e);
    return;
  }
  int mods = (APP && APP->window) ? APP->window->getMods() : 0;
  if ((mods & RACK_MOD_CTRL) != 0) {
    // Let Rack handle Ctrl/Cmd+wheel (e.g. zoom) when hovering the platter.
    OpaqueWidget::onHoverScroll(e);
    return;
  }

  float rawScroll = -e.scrollDelta.y;
  if (std::fabs(rawScroll) < 1e-4f) {
    OpaqueWidget::onHoverScroll(e);
    return;
  }

  float maxLag = float(module->getUiAccessibleLagSamples());
  if (maxLag <= 0.f) {
    e.consume(this);
    return;
  }

  float sampleRate = module->getUiSampleRate();
  Vec local = e.pos.minus(localCenter());
  float radiusGain = wheelRadiusGainForLocal(local);
  float lowFpsComp = lowFpsCompensationFactor();
  float scroll = normalizeWheelScrollDelta(rawScroll);
  float scrollAbs = std::fabs(scroll);
  float scrollShaped = (scroll >= 0.f ? 1.f : -1.f) * (scrollAbs + 0.65f * scrollAbs * scrollAbs);
  float samplesPerNotch =
    sampleRate * 0.018f * TemporalDeck::kWheelScratchTravelScale * module->scratchSensitivity();
  float wheelGain = 1.f + 0.50f * lowFpsComp;
  float lagDelta = scrollShaped * samplesPerNotch * radiusGain * wheelGain;
  // Keep wheel scratch active long enough for Hybrid wheel impulses to settle,
  // otherwise small forward ticks can drop out before they noticeably reduce lag.
  float holdSeconds = module->isSlipLatched() ? 0.16f : 0.09f;
  holdSeconds += 0.06f * lowFpsComp;
  int holdSamples = std::max(1, int(std::round(sampleRate * holdSeconds)));

  module->addPlatterWheelDelta(lagDelta, holdSamples);
  logTraceEvent("WHEEL", local, Vec(0.f, 0.f), rawScroll, 0.f, lagDelta, float(module->getUiLagSamples()),
                localLagSamples, filteredGestureVelocity);
  e.consume(this);
}

void TemporalDeckPlatterWidget::onDragStart(const event::DragStart &e) {
  syncTraceCaptureState();
  if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || !isWithinPlatter(onButtonPos)) {
    return;
  }
  Vec local = onButtonPos.minus(localCenter());
  dragging = true;
  dragHasTiming = false;
  lastMoveTimeSec = system::getTime();
  recentGestureDtSec = kDefaultGestureDtSec;
  filteredGestureVelocity = 0.f;
  contactAngle = std::atan2(local.y, local.x);
  contactRadiusPx = clamp(local.norm(), platterRadiusPx * 0.32f, platterRadiusPx * 0.98f);
  localLagSamples = float(module->getUiLagSamples());
  module->setPlatterScratch(true, localLagSamples, 0.f);
  module->setPlatterMotionFreshSamples(0);
  logTraceEvent("DRAG_START", local, Vec(0.f, 0.f), 0.f, 0.f, 0.f, localLagSamples, localLagSamples, 0.f);
  e.consume(this);
}

void TemporalDeckPlatterWidget::onDragMove(const event::DragMove &e) {
  syncTraceCaptureState();
  if (!dragging || e.button != GLFW_MOUSE_BUTTON_LEFT) {
    return;
  }
  if (!isLeftMouseDown()) {
    dragging = false;
    if (module) {
      module->setPlatterScratch(false, localLagSamples, 0.f);
      module->setPlatterMotionFreshSamples(0);
    }
    Vec local = currentLocalMousePos();
    logTraceEvent("DRAG_CANCEL", local, e.mouseDelta, 0.f, 0.f, 0.f, float(module ? module->getUiLagSamples() : 0.f),
                  localLagSamples, filteredGestureVelocity);
    dragHasTiming = false;
    filteredGestureVelocity = 0.f;
    return;
  }
  Vec local = currentLocalMousePos();
  logTraceEvent("DRAG_MOVE", local, e.mouseDelta, 0.f, 0.f, 0.f, float(module->getUiLagSamples()), localLagSamples,
                filteredGestureVelocity);
  updateScratchFromLocal(local, e.mouseDelta);
  e.consume(this);
}

void TemporalDeckPlatterWidget::onDragEnd(const event::DragEnd &e) {
  syncTraceCaptureState();
  if (dragging && e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (isLeftMouseDown()) {
      e.consume(this);
      return;
    }
    dragging = false;
    if (module) {
      module->setPlatterScratch(false, localLagSamples, 0.f);
      module->setPlatterMotionFreshSamples(0);
    }
    Vec local = currentLocalMousePos();
    logTraceEvent("DRAG_END", local, Vec(0.f, 0.f), 0.f, 0.f, 0.f, float(module ? module->getUiLagSamples() : 0.f),
                  localLagSamples, filteredGestureVelocity);
    dragHasTiming = false;
    filteredGestureVelocity = 0.f;
    e.consume(this);
  }
}

struct BananutBlack : app::SvgPort {
  BananutBlack() { setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg"))); }
};

struct TemporalDeckWidget : ModuleWidget {
  TemporalDeckWidget(TemporalDeck *module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/deck.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    Vec bufferKnobPos = mm2px(Vec(8.408f, 17.086f));
    addParam(createParamCentered<RoundBlackKnob>(bufferKnobPos, module, TemporalDeck::BUFFER_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.889, 99.226)), module, TemporalDeck::RATE_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(70.982, 98.872)), module, TemporalDeck::MIX_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(70.982, 112.996)), module, TemporalDeck::FEEDBACK_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.459, 84.07)), module,
                                                 TemporalDeck::SCRATCH_SENSITIVITY_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(57.5, 101.1)), module, TemporalDeck::FREEZE_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(45.6, 101.1)), module, TemporalDeck::REVERSE_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(33.2, 101.1)), module, TemporalDeck::SLIP_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.665, 112.9)), module, TemporalDeck::POSITION_CV_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.905, 112.9)), module, TemporalDeck::RATE_CV_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.437, 99.012)), module, TemporalDeck::INPUT_L_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.478, 112.9)), module, TemporalDeck::INPUT_R_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.403, 112.9)), module, TemporalDeck::SCRATCH_GATE_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.5, 112.9)), module, TemporalDeck::FREEZE_GATE_INPUT));

    addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(94.241, 99.012)), module, TemporalDeck::OUTPUT_L_OUTPUT));
    addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(83.037, 99.135)), module, TemporalDeck::S_GATE_O_OUTPUT));
    addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(94.2, 113.146)), module, TemporalDeck::OUTPUT_R_OUTPUT));
    addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(82.996, 113.269)), module, TemporalDeck::S_POS_O_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.5, 95.3)), module, TemporalDeck::FREEZE_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.6, 95.3)), module, TemporalDeck::REVERSE_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(30.8, 95.3)), module, TemporalDeck::SLIP_SLOW_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(33.2, 95.3)), module, TemporalDeck::SLIP_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(35.6, 95.3)), module, TemporalDeck::SLIP_FAST_LIGHT));

    auto *bufferMode = new TemporalDeckBufferModeWidget;
    bufferMode->module = module;
    bufferMode->box.pos = Vec(0.f, 0.f);
    bufferMode->box.size = box.size;
    bufferMode->labelPosPx = bufferKnobPos.plus(Vec(mm2px(Vec(10.4f, 0.f)).x, 0.f));
    addChild(bufferMode);

    Vec platterCenter = mm2px(Vec(50.8f, 72.f));
    float platterRadius = mm2px(Vec(29.5f, 0.f)).x;
    loadPlatterAnchor(platterCenter, platterRadius);
    Vec tonearmPivot = platterCenter.plus(Vec(platterRadius * 1.12f, platterRadius * 0.34f));

    float arcRadius = platterRadius + mm2px(Vec(3.5f, 0.f)).x;
    for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
      float t = float(i) / float(TemporalDeck::kArcLightCount - 1);
      float angle = -float(M_PI) * t;
      Vec ledPos = platterCenter.plus(Vec(std::cos(angle), std::sin(angle)).mult(arcRadius));
      addChild(createLightCentered<MediumLight<RedLight>>(ledPos, module, TemporalDeck::ARC_MAX_LIGHT_START + i));
      addChild(createLightCentered<MediumLight<YellowLight>>(ledPos, module, TemporalDeck::ARC_LIGHT_START + i));
    }

    auto display = new TemporalDeckDisplayWidget();
    display->module = module;
    display->centerMm = platterCenter;
    display->platterRadiusPx = platterRadius;
    display->box.size = box.size;
    addChild(display);

    auto platter = new TemporalDeckPlatterWidget();
    platter->module = module;
    platter->centerPx = platterCenter;
    platter->platterRadiusPx = platterRadius;
    platter->deadZonePx = platterRadius * 0.08f;
    platter->box.pos = platterCenter.minus(Vec(platterRadius, platterRadius));
    platter->box.size = Vec(platterRadius * 2.f, platterRadius * 2.f);
    addChild(platter);

    auto *tonearm = new TemporalDeckTonearmWidget;
    tonearm->module = module;
    tonearm->centerPx = platterCenter;
    tonearm->platterRadiusPx = platterRadius;
    tonearm->box.pos = Vec(0.f, 0.f);
    tonearm->box.size = box.size;
    addChild(tonearm);

    // Add after platter/tonearm so this control is visible on top.
    addParam(createParamCentered<LEDButton>(tonearmPivot, module, TemporalDeck::CARTRIDGE_CYCLE_PARAM));
  }

  void appendContextMenu(Menu *menu) override {
    TemporalDeck *module = dynamic_cast<TemporalDeck *>(this->module);
    assert(menu);
    menu->addChild(new MenuSeparator());
    if (module) {
      menu->addChild(createMenuLabel("Sample"));
      std::string loadedSampleName = module->getLoadedSampleDisplayName();
      std::string loadedSampleRight = loadedSampleName.empty() ? "WAV/FLAC/MP3" : "Loaded";
      menu->addChild(createMenuItem("Load sample...", loadedSampleRight, [=]() {
        osdialog_filters *filters = osdialog_filters_parse("Audio:wav,WAV,flac,FLAC,mp3,MP3");
        char *pathC = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
        osdialog_filters_free(filters);
        if (!pathC) {
          return;
        }
        std::string path = pathC;
        std::free(pathC);
        std::string error;
        if (!module->loadSampleFromPath(path, &error)) {
          std::string message = error.empty() ? "Sample load failed" : error;
          osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message.c_str());
        }
      }));
      menu->addChild(createMenuItem("Clear sample", "", [=]() { module->clearLoadedSample(); }, !module->hasLoadedSample()));
      menu->addChild(createCheckMenuItem("Auto-play on load", "",
                                         [=]() { return module->isSampleAutoPlayOnLoadEnabled(); },
                                         [=]() { module->setSampleAutoPlayOnLoadEnabled(!module->isSampleAutoPlayOnLoadEnabled()); }));
      menu->addChild(createCheckMenuItem("Loop sample", "", [=]() { return module->isSampleLoopEnabled(); },
                                         [=]() { module->setSampleLoopEnabled(!module->isSampleLoopEnabled()); },
                                         !module->hasLoadedSample()));
      if (module->hasLoadedSample()) {
        menu->addChild(createSubmenuItem("Sample info", "", [=](Menu *submenu) {
          submenu->addChild(createMenuLabel(loadedSampleName));
          submenu->addChild(createMenuLabel(
            string::f("Length: %.2f s", std::max(0.0, module->getUiSampleDurationSeconds()))));
          if (module->wasLoadedSampleTruncated()) {
            submenu->addChild(createMenuLabel("Truncated to current buffer limit"));
          }
        }));
      }
      menu->addChild(new MenuSeparator());
      menu->addChild(createMenuLabel("Advanced"));
      if (!module->isSampleModeEnabled()) {
        menu->addChild(createSubmenuItem("Buffer range", "", [=](Menu *submenu) {
          auto bufferModeMenuLabel = [=](int mode) {
            std::string label = TemporalDeck::bufferDurationLabelFor(mode);
            if (mode != TemporalDeck::BUFFER_DURATION_10M_STEREO && mode != TemporalDeck::BUFFER_DURATION_10M_MONO) {
              return label;
            }
            // 10-minute modes use 601s internal allocation (extra 1s headroom).
            float sr = std::max(module->getUiSampleRate(), 1.f);
            float channels = (mode == TemporalDeck::BUFFER_DURATION_10M_MONO) ? 1.f : 2.f;
            double bytes = double(sr) * 601.0 * double(channels) * double(sizeof(float));
            double mib = bytes / (1024.0 * 1024.0);
            return string::f("%s (~%.0f MiB @ %.1fk)", label.c_str(), mib, sr / 1000.f);
          };
          for (int i = 0; i < TemporalDeck::BUFFER_DURATION_COUNT; ++i) {
            submenu->addChild(createCheckMenuItem(bufferModeMenuLabel(i), "",
                                                  [=]() { return module->getBufferDurationMode() == i; },
                                                  [=]() { module->applyBufferDurationMode(i); }));
          }
        }));
      }
      menu->addChild(createSubmenuItem("Slip return speed", "", [=](Menu *submenu) {
        submenu->addChild(createCheckMenuItem("None", "", [=]() { return !module->isSlipLatched(); },
                                              [=]() { module->setSlipLatched(false); }));
        for (int i = 0; i < TemporalDeck::SLIP_RETURN_COUNT; ++i) {
          submenu->addChild(createCheckMenuItem(
            TemporalDeck::slipReturnLabelFor(i), "",
            [=]() { return module->isSlipLatched() && module->getSlipReturnMode() == i; },
            [=]() {
              module->setSlipReturnMode(i);
              module->setSlipLatched(true);
            }));
        }
      }));
      menu->addChild(createSubmenuItem("Platter art", "", [=](Menu *submenu) {
        bool dragonKingDebug = isDragonKingDebugEnabled();
        submenu->addChild(createCheckMenuItem(
          TemporalDeck::platterArtModeLabelFor(TemporalDeck::PLATTER_ART_BUILTIN_SVG), "",
          [=]() { return module->getPlatterArtMode() == TemporalDeck::PLATTER_ART_BUILTIN_SVG; },
          [=]() { module->setPlatterArtMode(TemporalDeck::PLATTER_ART_BUILTIN_SVG); }));
        submenu->addChild(createCheckMenuItem(
          TemporalDeck::platterArtModeLabelFor(TemporalDeck::PLATTER_ART_DRAGON_KING), "",
          [=]() { return module->getPlatterArtMode() == TemporalDeck::PLATTER_ART_DRAGON_KING; },
          [=]() { module->setPlatterArtMode(TemporalDeck::PLATTER_ART_DRAGON_KING); }));
        submenu->addChild(createCheckMenuItem(
          TemporalDeck::platterArtModeLabelFor(TemporalDeck::PLATTER_ART_BLANK), "",
          [=]() { return module->getPlatterArtMode() == TemporalDeck::PLATTER_ART_BLANK; },
          [=]() { module->setPlatterArtMode(TemporalDeck::PLATTER_ART_BLANK); }));
        submenu->addChild(createCheckMenuItem(
          TemporalDeck::platterArtModeLabelFor(TemporalDeck::PLATTER_ART_PROCEDURAL), "",
          [=]() { return module->getPlatterArtMode() == TemporalDeck::PLATTER_ART_PROCEDURAL; },
          [=]() { module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL); }));
        submenu->addChild(createSubmenuItem("Brightness", "", [=](Menu *brightnessMenu) {
          for (int i = 0; i < TemporalDeck::PLATTER_BRIGHTNESS_COUNT; ++i) {
            brightnessMenu->addChild(createCheckMenuItem(
              TemporalDeck::platterBrightnessLabelFor(i), "",
              [=]() { return module->getPlatterBrightnessMode() == i; },
              [=]() { module->setPlatterBrightnessMode(i); }));
          }
        }));
        if (dragonKingDebug) {
          int currentMode = module->getPlatterArtMode();
          std::string customPath = module->getCustomPlatterArtPath();
          submenu->addChild(createCheckMenuItem(
            TemporalDeck::platterArtModeLabelFor(TemporalDeck::PLATTER_ART_CUSTOM),
            customPath.empty() ? "No file" : "Loaded",
            [=]() { return module->getPlatterArtMode() == TemporalDeck::PLATTER_ART_CUSTOM; },
            [=]() {
              if (!module->getCustomPlatterArtPath().empty()) {
                module->setPlatterArtMode(TemporalDeck::PLATTER_ART_CUSTOM);
              }
            },
            customPath.empty()));
          submenu->addChild(new MenuSeparator());
          if (!customPath.empty()) {
            submenu->addChild(createMenuLabel(system::getFilename(customPath)));
          }
          submenu->addChild(createMenuItem("Load custom art...", "", [=]() {
            osdialog_filters *filters = osdialog_filters_parse("Image:svg,SVG,png,PNG,jpg,JPG,jpeg,JPEG");
            char *pathC = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
            osdialog_filters_free(filters);
            if (!pathC) {
              return;
            }
            std::string path = pathC;
            std::free(pathC);
            if (!isSupportedPlatterArtPath(path)) {
              osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Supported platter art formats are SVG, PNG, and JPG/JPEG.");
              return;
            }
            module->setCustomPlatterArtPath(path);
          }));
          submenu->addChild(createMenuItem("Clear custom art", "", [=]() { module->clearCustomPlatterArtPath(); },
                                           customPath.empty() && currentMode != TemporalDeck::PLATTER_ART_CUSTOM));
        }
      }));
      // Hidden for now, but keep the trace plumbing available in code in case
      // we need to bring back platter interaction logging for debugging.
      // menu->addChild(createCheckMenuItem("Log platter mouse events", "",
      //                                    [=]() { return module->isPlatterTraceLoggingEnabled(); },
      //                                    [=]() { module->setPlatterTraceLoggingEnabled(!module->isPlatterTraceLoggingEnabled()); }));
      if (isDragonKingDebugEnabled()) {
        menu->addChild(createMenuItem("Export platter SVG...", "", [=]() {
          Vec platterCenter = mm2px(Vec(50.8f, 72.f));
          float platterRadius = mm2px(Vec(29.5f, 0.f)).x;
          loadPlatterAnchor(platterCenter, platterRadius);
          std::string defaultDir = system::join(asset::user(), "TemporalDeck");
          system::createDirectories(defaultDir);
          osdialog_filters *filters = osdialog_filters_parse("SVG:svg,SVG");
          char *pathC = osdialog_file(OSDIALOG_SAVE, defaultDir.c_str(), "temporaldeck_platter.svg", filters);
          osdialog_filters_free(filters);
          if (!pathC) {
            return;
          }
          std::string path = ensureSvgExtension(pathC);
          std::free(pathC);

          std::string error;
          if (!writePlatterSvgSnapshot(path, platterRadius, module->getUiPlatterAngle(), &error)) {
            std::string message = error.empty() ? "Platter SVG export failed" : error;
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message.c_str());
            return;
          }
          osdialog_message(OSDIALOG_INFO, OSDIALOG_OK, string::f("Saved platter SVG:\n%s", path.c_str()).c_str());
        }));
      }
    }
    if (module) {
      menu->addChild(createSubmenuItem("Scratch interpolation", "", [=](Menu *submenu) {
        for (int i = 0; i < TemporalDeck::SCRATCH_INTERP_COUNT; ++i) {
          submenu->addChild(createCheckMenuItem(
            TemporalDeck::scratchInterpolationLabelFor(i), "",
            [=]() { return module->getScratchInterpolationMode() == i; },
            [=]() { module->setScratchInterpolationMode(i); }));
        }
      }));
    }
  }
};

Model *modelTemporalDeck = createModel<TemporalDeck, TemporalDeckWidget>("TemporalDeck");
