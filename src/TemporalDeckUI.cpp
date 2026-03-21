#include "TemporalDeck.hpp"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

struct TemporalDeckDisplayWidget : Widget {
  TemporalDeck *module = nullptr;
  Vec centerMm = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;

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
  bool cursorLocked = false;
  Vec onButtonPos;
  float contactAngle = 0.f;
  float contactRadiusPx = 0.f;
  float localLagSamples = 0.f;
  float filteredGestureVelocity = 0.f;
  double lastMoveTimeSec = 0.0;
  double recentGestureDtSec = kDefaultGestureDtSec;
  InteractionTraceRecorder traceRecorder;

  Vec localCenter() const { return centerPx.minus(box.pos); }

  bool isWithinPlatter(Vec panelPos) const {
    Vec local = panelPos.minus(localCenter());
    float radius = local.norm();
    return radius <= platterRadiusPx;
  }

  void updateScratchFromLocal(Vec local, Vec mouseDelta);
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

void TemporalDeckPlatterWidget::syncTraceCaptureState() {
  if (!module) {
    stopTraceCapture();
    return;
  }
  if (!module->isFreezeTraceLoggingEnabled()) {
    stopTraceCapture();
    return;
  }
  bool freezeLatched = module->isUiFreezeLatched();
  if (freezeLatched) {
    startTraceCapture();
  } else {
    stopTraceCapture();
  }
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
  traceRecorder.file << "# TemporalDeck platter interaction trace v1\n";
  traceRecorder.file << "# Start when freeze latch turns on, stop when freeze latch turns off\n";
  traceRecorder.file
    << "seq,t_sec,event,freeze,dragging,cursor_locked,x,y,dx,dy,scroll,delta_angle,lag_delta,live_lag,local_lag,"
       "velocity,sensitivity,sample_rate\n";
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
  double tSec = std::max(0.0, system::getTime() - traceRecorder.startTimeSec);
  traceRecorder.file << traceRecorder.sequence++ << "," << tSec << "," << eventName << ","
                     << (module->isUiFreezeLatched() ? 1 : 0) << "," << (dragging ? 1 : 0) << ","
                     << (cursorLocked ? 1 : 0) << "," << local.x << "," << local.y << "," << mouseDelta.x << ","
                     << mouseDelta.y << "," << scroll << "," << deltaAngle << "," << lagDelta << "," << liveLag
                     << "," << localLag << "," << velocity << "," << module->scratchSensitivity() << ","
                     << module->getUiSampleRate() << "\n";
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
    double lagMs = 1000.0 * lag / std::max(module->getUiSampleRate(), 1.f);
    char text[32];
    std::snprintf(text, sizeof(text), "%.0f ms", lagMs);
    Vec textPos = centerMm.plus(Vec(arcRadius + mm2px(Vec(8.0f, 0.f)).x, -arcRadius * 0.86f));

    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 11.5f);
    nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgText(args.vg, textPos.x, textPos.y, text, nullptr);
  }
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
  nvgSave(args.vg);
  float rotation = module ? module->getUiPlatterAngle() : 0.f;
  Vec center = localCenter();

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

    for (int i = 0; i < 16; ++i) {
      float grooveRadius = platterRadiusPx * (0.24f + 0.047f * i);
      float alpha = (i % 2 == 0) ? 34.f : 18.f;
      float wobbleAmp = 0.55f + 0.05f * float(i % 4);
      float wobblePhase = 0.47f * float(i) + 0.061f * float(i * i);
      float wobbleFreq = 3.1f + 0.23f * float((i * 2 + 1) % 5);
      float ringRotation = 0.19f * float(i) + 0.043f * float(i * i);

      nvgBeginPath(args.vg);
      constexpr int kSteps = 64; // Reduced from 96 for performance
      for (int step = 0; step <= kSteps; ++step) {
        float t = 2.f * float(M_PI) * float(step) / float(kSteps) + ringRotation;
        // Simplified wobble: removed expensive pow and copysign
        float wobble = std::sin(t * wobbleFreq + wobblePhase);
        float radius = grooveRadius + wobbleAmp * wobble;
        float x = std::cos(t) * radius;
        float y = std::sin(t) * radius;
        if (step == 0)
          nvgMoveTo(args.vg, x, y);
        else
          nvgLineTo(args.vg, x, y);
      }
      nvgStrokeColor(args.vg, nvgRGBA(210, 218, 228, (unsigned char)alpha));
      nvgStrokeWidth(args.vg, 0.7f);
      nvgStroke(args.vg);
    }
    nvgRestore(args.vg);
  }

  float labelRadius = platterRadiusPx * 0.33f;
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

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, labelRadius * 0.12f);
  nvgFillColor(args.vg, nvgRGB(222, 228, 235));
  nvgFill(args.vg);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
  nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 32));
  nvgStrokeWidth(args.vg, 1.1f);
  nvgStroke(args.vg);

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
  bool useCursorAngle = !cursorLocked && localRadius > std::max(deadZonePx, platterRadiusPx * 0.16f);
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
  bool freezeLatched = module->isUiFreezeLatched();
  float sensitivity = module->scratchSensitivity();
  float lagDelta = platter_interaction::lagDeltaFromAngle(deltaAngle, module->getUiSampleRate(), sensitivity,
                                                          TemporalDeck::kMouseScratchTravelScale,
                                                          TemporalDeck::kNominalPlatterRpm);
  if (!freezeLatched) {
    localLagSamples = platter_interaction::rebaseLagTarget(localLagSamples, liveLag, lagDelta);
  }
  localLagSamples = clamp(localLagSamples - lagDelta, 0.f, accessibleLag);

  float measuredVelocity = lagDelta / float(dtSec);
  float velocityAlpha = 1.f - std::exp(-2.f * float(M_PI) * 30.f * float(dtSec));
  filteredGestureVelocity += (measuredVelocity - filteredGestureVelocity) * velocityAlpha;
  module->setPlatterScratch(true, localLagSamples, filteredGestureVelocity);
  logTraceEvent("SCRATCH_APPLY", local, mouseDelta, 0.f, deltaAngle, lagDelta, liveLag, localLagSamples,
                filteredGestureVelocity);

  int motionFreshSamples = int(std::round(module->getUiSampleRate() * float(dtSec) * 1.35f));
  motionFreshSamples = clamp(motionFreshSamples, 1, int(std::round(module->getUiSampleRate() * 0.025f)));
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

  float scroll = -e.scrollDelta.y;
  if (std::fabs(scroll) < 1e-4f) {
    OpaqueWidget::onHoverScroll(e);
    return;
  }

  float maxLag = float(module->getUiAccessibleLagSamples());
  if (maxLag <= 0.f) {
    e.consume(this);
    return;
  }

  float sampleRate = module->getUiSampleRate();
  float scrollAbs = std::fabs(scroll);
  float scrollShaped = (scroll >= 0.f ? 1.f : -1.f) * (scrollAbs + 0.65f * scrollAbs * scrollAbs);
  float samplesPerNotch =
    sampleRate * 0.018f * TemporalDeck::kWheelScratchTravelScale * module->scratchSensitivity();
  float lagDelta = scrollShaped * samplesPerNotch;
  float holdSeconds = module->isSlipLatched() ? 0.16f : 0.03f;
  int holdSamples = std::max(1, int(std::round(sampleRate * holdSeconds)));

  module->addPlatterWheelDelta(lagDelta, holdSamples);
  Vec local = e.pos.minus(localCenter());
  logTraceEvent("WHEEL", local, Vec(0.f, 0.f), scroll, 0.f, lagDelta, float(module->getUiLagSamples()), localLagSamples,
                filteredGestureVelocity);
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
  if (!cursorLocked && module->isPlatterCursorLockEnabled() && settings::allowCursorLock && APP && APP->window) {
    APP->window->cursorLock();
    cursorLocked = true;
  }
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
    logTraceEvent("DRAG_CANCEL", Vec(0.f, 0.f), e.mouseDelta, 0.f, 0.f, 0.f, float(module ? module->getUiLagSamples() : 0.f),
                  localLagSamples, filteredGestureVelocity);
    if (cursorLocked && APP && APP->window) {
      APP->window->cursorUnlock();
      cursorLocked = false;
    }
    dragHasTiming = false;
    filteredGestureVelocity = 0.f;
    return;
  }
  Vec local = APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos).minus(localCenter());
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
    logTraceEvent("DRAG_END", Vec(0.f, 0.f), Vec(0.f, 0.f), 0.f, 0.f, 0.f, float(module ? module->getUiLagSamples() : 0.f),
                  localLagSamples, filteredGestureVelocity);
    if (cursorLocked && APP && APP->window) {
      APP->window->cursorUnlock();
      cursorLocked = false;
    }
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

    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.408, 17.086)), module, TemporalDeck::BUFFER_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(24.39, 99.026)), module, TemporalDeck::RATE_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(78.482, 98.872)), module, TemporalDeck::MIX_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(78.482, 112.996)), module, TemporalDeck::FEEDBACK_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.459, 84.07)), module,
                                                 TemporalDeck::SCRATCH_SENSITIVITY_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(62.1, 101.1)), module, TemporalDeck::FREEZE_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(50.2, 101.1)), module, TemporalDeck::REVERSE_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(37.8, 101.1)), module, TemporalDeck::SLIP_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.965, 112.9)), module, TemporalDeck::POSITION_CV_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.405, 112.9)), module, TemporalDeck::RATE_CV_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.837, 99.012)), module, TemporalDeck::INPUT_L_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.878, 112.9)), module, TemporalDeck::INPUT_R_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.703, 112.9)), module, TemporalDeck::SCRATCH_GATE_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.1, 112.9)), module, TemporalDeck::FREEZE_GATE_INPUT));

    addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(94.041, 99.012)), module, TemporalDeck::OUTPUT_L_OUTPUT));
    addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(94.0, 113.146)), module, TemporalDeck::OUTPUT_R_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(62.1, 95.3)), module, TemporalDeck::FREEZE_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(50.2, 95.3)), module, TemporalDeck::REVERSE_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(35.4, 95.3)), module, TemporalDeck::SLIP_SLOW_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(37.8, 95.3)), module, TemporalDeck::SLIP_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(40.2, 95.3)), module, TemporalDeck::SLIP_FAST_LIGHT));

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
      menu->addChild(createMenuLabel("Advanced"));
      menu->addChild(createSubmenuItem("Buffer range", "", [=](Menu *submenu) {
        for (int i = 0; i < TemporalDeck::BUFFER_DURATION_COUNT; ++i) {
          submenu->addChild(createCheckMenuItem(TemporalDeck::bufferDurationLabelFor(i), "",
                                                [=]() { return module->getBufferDurationMode() == i; },
                                                [=]() { module->applyBufferDurationMode(i); }));
        }
      }));
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
      menu->addChild(createCheckMenuItem("Cursor lock on platter drag", "",
                                         [=]() { return module->isPlatterCursorLockEnabled(); },
                                         [=]() { module->setPlatterCursorLockEnabled(!module->isPlatterCursorLockEnabled()); }));
      menu->addChild(createCheckMenuItem("Debug trace on freeze", "",
                                         [=]() { return module->isFreezeTraceLoggingEnabled(); },
                                         [=]() { module->setFreezeTraceLoggingEnabled(!module->isFreezeTraceLoggingEnabled()); }));
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
