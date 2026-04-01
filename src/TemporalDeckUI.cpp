#include "TemporalDeck.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <limits>
#include <regex>
#include <set>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include <osdialog.h>

static bool isExpandedVinylSyncActive();
static bool isExpandedVinylDownloadRunning();
static std::string expandedVinylSyncLabel();
static bool startExpandedVinylDownloadAsync(std::string *errorOut);
static void pumpExpandedVinylDownloadNotifications();
static std::string temporalDeckUserRootPath();

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
  static constexpr float kLowFpsWarningOffHz = 55.f;
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
  int cachedRenderArtMode = std::numeric_limits<int>::min();
  int cachedRenderInventorySerial = std::numeric_limits<int>::min();
  std::string cachedRenderPath;
  bool cachedRenderValid = false;
  bool cachedRenderSvg = false;
  std::string cachedRenderAbsolutePath;
  std::string cachedRenderLoadPath;

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
  bool showSync = isExpandedVinylSyncActive();
  if ((!showSync && !lowFpsWarningActive) || !APP || !APP->window || !APP->window->uiFont) {
    return;
  }
  (void)center;
  // Place warning above S.GATE, in the gap between platter edge and jack row.
  Vec warningPos = mm2px(Vec(73.0f, 70.0f));
  nvgSave(args.vg);
  nvgFontFaceId(args.vg, APP->window->uiFont->handle);
  nvgFontSize(args.vg, 10.f);
  nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  if (showSync) {
    nvgFillColor(args.vg, nvgRGBA(120, 220, 255, 245));
    std::string syncLabel = expandedVinylSyncLabel();
    nvgText(args.vg, warningPos.x, warningPos.y, syncLabel.c_str(), nullptr);
  } else {
    nvgFillColor(args.vg, nvgRGBA(255, 178, 82, 240));
    nvgText(args.vg, warningPos.x, warningPos.y, string::f("LOW UI FPS: %.0f", uiFpsDisplayHz).c_str(), nullptr);
  }
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
  std::string traceDir = system::join(temporalDeckUserRootPath(), "platter_traces");
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

static std::string trimAsciiWhitespace(std::string text) {
  auto notSpace = [](unsigned char c) { return !std::isspace(c); };
  auto start = std::find_if(text.begin(), text.end(), notSpace);
  auto end = std::find_if(text.rbegin(), text.rend(), notSpace).base();
  if (start >= end) {
    return "";
  }
  return std::string(start, end);
}

static uint64_t fnv1aInit64() {
  return 1469598103934665603ull;
}

static uint64_t fnv1aUpdate64(uint64_t hash, const uint8_t *data, size_t size) {
  constexpr uint64_t kFNVPrime = 1099511628211ull;
  for (size_t i = 0; i < size; ++i) {
    hash ^= data[i];
    hash *= kFNVPrime;
  }
  return hash;
}

static uint64_t fnv1aUpdate64String(uint64_t hash, const std::string &text) {
  return fnv1aUpdate64(hash, reinterpret_cast<const uint8_t *>(text.data()), text.size());
}

static const char *kVinylManifestAlgorithm = "fnv1a64-keyed-v1";
static const char *kVinylManifestDomain = "TemporalDeckVinylInventorySigned-v1";
static const char *kVinylManifestFallbackSecret = "TemporalDeckLocalSigningKey";

static std::string hexU64(uint64_t value) {
  std::ostringstream out;
  out << std::hex << std::setfill('0') << std::setw(16) << value;
  return out.str();
}

static std::string jsonEscape(std::string text) {
  std::ostringstream out;
  for (unsigned char c : text) {
    switch (c) {
    case '"':
      out << "\\\"";
      break;
    case '\\':
      out << "\\\\";
      break;
    case '\b':
      out << "\\b";
      break;
    case '\f':
      out << "\\f";
      break;
    case '\n':
      out << "\\n";
      break;
    case '\r':
      out << "\\r";
      break;
    case '\t':
      out << "\\t";
      break;
    default:
      if (c < 0x20) {
        out << "\\u00" << std::hex << std::setw(2) << std::setfill('0') << int(c);
      } else {
        out << char(c);
      }
      break;
    }
  }
  return out.str();
}

struct VinylSignatureRecord {
  std::string id;
  std::string label;
  std::string file;
  bool menuVisible = true;
  int menuId = -1;
  std::string basePath;
  std::string sourceRootPath;
  std::string absolutePath;
  uint64_t sizeBytes = 0;
  uint64_t contentHash = 0;
};

enum VinylInventorySource {
  VINYL_SOURCE_BUILTIN = 0,
  VINYL_SOURCE_EXPANDED = 1,
};

struct VinylInventoryEntry {
  std::string id;
  std::string label;
  std::string file;
  bool menuVisible = true;
  int menuId = -1;
  int source = VINYL_SOURCE_BUILTIN;
  std::string sourceRootPath;
  std::string basePath;
  std::string absolutePath;
  bool signatureVerified = false;
  std::string signatureError;
};

struct SignedFileInfo {
  std::string id;
  std::string file;
  int menuId = -1;
  uint64_t size = 0;
  uint64_t hash = 0;
};

static uint64_t computeVinylManifestKeyId(const std::string &secret) {
  return fnv1aUpdate64String(fnv1aInit64(), secret);
}

static bool computeVinylManifestSignature(const std::string &secret, const std::string &basePath,
                                          const std::vector<VinylInventoryEntry> &entries,
                                          const std::map<std::string, SignedFileInfo> &signedById, uint64_t *signatureOut) {
  if (!signatureOut) {
    return false;
  }
  uint64_t signature = fnv1aUpdate64String(fnv1aInit64(), kVinylManifestDomain);
  signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  signature = fnv1aUpdate64String(signature, secret);
  signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  signature = fnv1aUpdate64String(signature, basePath);
  signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  for (const VinylInventoryEntry &entry : entries) {
    auto it = signedById.find(entry.id);
    if (it == signedById.end()) {
      return false;
    }
    const SignedFileInfo &signedFile = it->second;
    signature = fnv1aUpdate64String(signature, entry.id);
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, entry.label);
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, entry.file);
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, entry.menuVisible ? "1" : "0");
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, std::to_string(entry.menuId));
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, std::to_string(signedFile.size));
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, hexU64(signedFile.hash));
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  }
  *signatureOut = signature;
  return true;
}

static std::vector<std::string> vinylManifestVerificationSecrets() {
  return {kVinylManifestFallbackSecret};
}

struct VinylInventoryState {
  bool valid = false;
  std::string error;
  bool signaturePresent = false;
  bool signatureVerified = false;
  std::string signatureError;
  int source = VINYL_SOURCE_BUILTIN;
  std::string sourceRootPath;
  std::string sourceLabel;
  std::string basePath = "res/Vinyl";
  int verifiedEntryCount = 0;
  std::vector<VinylInventoryEntry> entries;
};

static void markVinylEntriesUnverified(VinylInventoryState *state, const std::string &reason) {
  if (!state) {
    return;
  }
  state->verifiedEntryCount = 0;
  for (VinylInventoryEntry &entry : state->entries) {
    entry.signatureVerified = false;
    entry.signatureError = reason;
  }
}

static bool hashFileFnv64(const std::string &path, uint64_t *hashOut, uint64_t *sizeOut, std::string *errorOut);

static bool parseHexU64(const std::string &text, uint64_t *valueOut) {
  if (!valueOut) {
    return false;
  }
  std::string trimmed = trimAsciiWhitespace(text);
  if (trimmed.empty()) {
    return false;
  }
  try {
    size_t consumed = 0;
    uint64_t value = std::stoull(trimmed, &consumed, 16);
    if (consumed != trimmed.size()) {
      return false;
    }
    *valueOut = value;
    return true;
  } catch (...) {
    return false;
  }
}

static bool isSafeVinylFileName(const std::string &fileName) {
  if (fileName.empty()) {
    return false;
  }
  return fileName.find("..") == std::string::npos && fileName.find('/') == std::string::npos &&
         fileName.find('\\') == std::string::npos;
}

static std::string normalizeInventoryBasePath(std::string path) {
  path = trimAsciiWhitespace(path);
  std::replace(path.begin(), path.end(), '\\', '/');
  while (!path.empty() && path.back() == '/') {
    path.pop_back();
  }
  return path;
}

static bool isSafeInventoryBasePath(const std::string &basePath) {
  if (basePath.empty()) {
    return false;
  }
  if (basePath.find("..") != std::string::npos) {
    return false;
  }
  if (basePath[0] == '/' || basePath[0] == '\\') {
    return false;
  }
  return true;
}

static std::string joinInventoryRelativePath(const std::string &basePath, const std::string &fileName) {
  if (basePath.empty()) {
    return fileName;
  }
  return basePath + "/" + fileName;
}

static constexpr int kExpandedMenuIdOffset = 10000;
static const char *kVinylExpansionBaseUrl =
  "https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Assets/main/Vinyl";
static std::atomic<int> gExpandedVinylSyncDepth {0};
static std::atomic<bool> gExpandedVinylDownloadRunning {false};
static std::atomic<bool> gExpandedVinylDownloadResultPending {false};
static std::mutex gExpandedVinylDownloadResultMutex;
static std::string gExpandedVinylDownloadResultError;
static std::atomic<int> gExpandedVinylDownloadCurrentIndex {0};
static std::atomic<int> gExpandedVinylDownloadTotalFiles {0};
static std::atomic<uint64_t> gExpandedVinylSyncNonceSeq {0};
static std::atomic<uint64_t> gExpandedVinylLoadSalt {0};

static bool isExpandedVinylSyncActive() {
  return gExpandedVinylSyncDepth.load(std::memory_order_relaxed) > 0;
}

static bool isExpandedVinylDownloadRunning() {
  return gExpandedVinylDownloadRunning.load(std::memory_order_relaxed);
}

static std::string expandedVinylSyncLabel() {
  int current = gExpandedVinylDownloadCurrentIndex.load(std::memory_order_relaxed);
  int total = gExpandedVinylDownloadTotalFiles.load(std::memory_order_relaxed);
  if (current > 0 && total > 0) {
    current = std::min(current, total);
    return string::f("SYNC (%d/%d)", current, total);
  }
  return "SYNC";
}

static std::string builtInVinylInventoryPath() { return asset::plugin(pluginInstance, "res/Vinyl/inventory.json"); }

static std::string temporalDeckUserRootPath() { return system::join(asset::user(), "Leviathan/TemporalDeck"); }

static std::string expandedVinylRootPath() { return system::join(temporalDeckUserRootPath(), "Vinyl"); }

static std::string expandedVinylInventoryPath() { return system::join(expandedVinylRootPath(), "expanded.json"); }

static std::string inventoryAbsolutePath(const VinylInventoryEntry &entry) {
  return system::join(entry.sourceRootPath, joinInventoryRelativePath(entry.basePath, entry.file));
}

static const char *inventoryIdForPlatterArtMode(int mode) {
  switch (mode) {
  case TemporalDeck::PLATTER_ART_BUILTIN_SVG:
    return "static_svg";
  case TemporalDeck::PLATTER_ART_DRAGON_KING:
    return "dragon_king";
  case TemporalDeck::PLATTER_ART_BLANK:
    return "blank";
  case TemporalDeck::PLATTER_ART_TEMPORAL_DECK:
    return "temporal_deck";
  default:
    return nullptr;
  }
}

static bool platterArtModeForInventoryId(const std::string &id, int *modeOut) {
  int mode = -1;
  if (id == "static_svg") {
    mode = TemporalDeck::PLATTER_ART_BUILTIN_SVG;
  } else if (id == "dragon_king") {
    mode = TemporalDeck::PLATTER_ART_DRAGON_KING;
  } else if (id == "blank") {
    mode = TemporalDeck::PLATTER_ART_BLANK;
  } else if (id == "temporal_deck") {
    mode = TemporalDeck::PLATTER_ART_TEMPORAL_DECK;
  } else {
    return false;
  }
  if (modeOut) {
    *modeOut = mode;
  }
  return true;
}

static std::string fallbackVinylRelativePathForPlatterArtMode(int mode) {
  switch (mode) {
  case TemporalDeck::PLATTER_ART_BUILTIN_SVG:
    return "res/Vinyl/Static.svg";
  case TemporalDeck::PLATTER_ART_DRAGON_KING:
    return "res/Vinyl/DragonKingLeviathan.png";
  case TemporalDeck::PLATTER_ART_BLANK:
    return "res/Vinyl/Blank.png";
  case TemporalDeck::PLATTER_ART_TEMPORAL_DECK:
    return "res/Vinyl/TemporalDeck.png";
  default:
    return "";
  }
}

static VinylInventoryState loadVinylInventoryStateFromPath(const std::string &path, const std::string &sourceRootPath,
                                                           const std::string &defaultBasePath, int source,
                                                           const std::string &sourceLabel, bool useFileBasePath = true) {
  VinylInventoryState state;
  state.source = source;
  state.sourceRootPath = sourceRootPath;
  state.sourceLabel = sourceLabel;
  json_error_t error;
  json_t *root = json_load_file(path.c_str(), 0, &error);
  if (!root) {
    state.error = string::f("Failed to parse inventory.json (line %d): %s", error.line, error.text);
    return state;
  }
  if (!json_is_object(root)) {
    state.error = "inventory.json root must be an object";
    json_decref(root);
    return state;
  }

  json_t *basePathJ = json_object_get(root, "basePath");
  if (useFileBasePath && json_is_string(basePathJ)) {
    state.basePath = normalizeInventoryBasePath(json_string_value(basePathJ));
  } else {
    state.basePath = defaultBasePath;
  }
  if (!isSafeInventoryBasePath(state.basePath)) {
    state.error = "inventory.json has invalid basePath";
    json_decref(root);
    return state;
  }

  json_t *vinylJ = json_object_get(root, "vinyl");
  std::set<std::string> seenIds;
  std::set<int> seenMenuIds;
  if (!json_is_array(vinylJ)) {
    state.error = "inventory.json must contain a vinyl[] array";
    json_decref(root);
    return state;
  }
  size_t i = 0;
  json_t *itemJ = nullptr;
  json_array_foreach(vinylJ, i, itemJ) {
    if (!json_is_object(itemJ)) {
      state.error = string::f("vinyl[%zu] must be an object", i);
      state.entries.clear();
      json_decref(root);
      return state;
    }
    json_t *idJ = json_object_get(itemJ, "id");
    json_t *labelJ = json_object_get(itemJ, "label");
    json_t *fileJ = json_object_get(itemJ, "file");
    json_t *menuVisibleJ = json_object_get(itemJ, "menuVisible");
    json_t *menuIdJ = json_object_get(itemJ, "menuId");
    if (!json_is_string(idJ) || !json_is_string(fileJ)) {
      state.error = string::f("vinyl[%zu] requires string id and file", i);
      state.entries.clear();
      json_decref(root);
      return state;
    }

    VinylInventoryEntry entry;
    entry.id = trimAsciiWhitespace(json_string_value(idJ));
    entry.file = trimAsciiWhitespace(json_string_value(fileJ));
    entry.label = json_is_string(labelJ) ? trimAsciiWhitespace(json_string_value(labelJ)) : "";
    entry.menuVisible = !menuVisibleJ || json_is_true(menuVisibleJ);
    if (entry.menuVisible) {
      if (!json_is_integer(menuIdJ)) {
        state.error = string::f("vinyl[%zu] requires integer menuId when menuVisible=true", i);
        state.entries.clear();
        json_decref(root);
        return state;
      }
      entry.menuId = int(json_integer_value(menuIdJ));
      if (entry.menuId < 0) {
        state.error = string::f("vinyl[%zu] has invalid menuId", i);
        state.entries.clear();
        json_decref(root);
        return state;
      }
      if (!seenMenuIds.insert(entry.menuId).second) {
        state.error = string::f("Duplicate menuId in inventory.json: %d", entry.menuId);
        state.entries.clear();
        json_decref(root);
        return state;
      }
    } else if (json_is_integer(menuIdJ)) {
      entry.menuId = int(json_integer_value(menuIdJ));
    }

    if (entry.id.empty() || !isSafeVinylFileName(entry.file)) {
      state.error = string::f("vinyl[%zu] has invalid id/file", i);
      state.entries.clear();
      json_decref(root);
      return state;
    }
    if (!seenIds.insert(entry.id).second) {
      state.error = string::f("Duplicate vinyl id in inventory.json: %s", entry.id.c_str());
      state.entries.clear();
      json_decref(root);
      return state;
    }
    entry.source = source;
    entry.sourceRootPath = sourceRootPath;
    entry.basePath = state.basePath;
    entry.absolutePath = inventoryAbsolutePath(entry);
    state.entries.push_back(entry);
  }

  if (state.entries.empty()) {
    state.error = "inventory.json contains no vinyl entries";
    json_decref(root);
    return state;
  }
  state.valid = true;

  json_t *signedJ = json_object_get(root, "signed");
  if (!json_is_object(signedJ)) {
    state.signaturePresent = false;
    state.signatureVerified = false;
    state.signatureError = "Missing signed block in inventory.json (run Export signed inventory.json...)";
    markVinylEntriesUnverified(&state, "Missing signed block");
    json_decref(root);
    return state;
  }
  state.signaturePresent = true;

  json_t *algorithmJ = json_object_get(signedJ, "algorithm");
  json_t *keyIdJ = json_object_get(signedJ, "keyId");
  json_t *signatureJ = json_object_get(signedJ, "signature");
  json_t *filesJ = json_object_get(signedJ, "files");
  if (!json_is_string(algorithmJ) || !json_is_string(keyIdJ) || !json_is_string(signatureJ) || !json_is_array(filesJ)) {
    state.signatureVerified = false;
    state.signatureError = "Signed block missing required fields (algorithm/keyId/signature/files)";
    markVinylEntriesUnverified(&state, "Signed block missing required fields");
    json_decref(root);
    return state;
  }
  std::string algorithm = trimAsciiWhitespace(json_string_value(algorithmJ));
  if (algorithm != kVinylManifestAlgorithm) {
    state.signatureVerified = false;
    state.signatureError = string::f("Unsupported signature algorithm '%s'", algorithm.c_str());
    markVinylEntriesUnverified(&state, "Unsupported signature algorithm");
    json_decref(root);
    return state;
  }
  uint64_t signedKeyId = 0;
  if (!parseHexU64(json_string_value(keyIdJ), &signedKeyId)) {
    state.signatureVerified = false;
    state.signatureError = "Signed keyId is invalid";
    markVinylEntriesUnverified(&state, "Signed keyId invalid");
    json_decref(root);
    return state;
  }
  uint64_t signedManifestSignature = 0;
  if (!parseHexU64(json_string_value(signatureJ), &signedManifestSignature)) {
    state.signatureVerified = false;
    state.signatureError = "Signed signature is invalid";
    markVinylEntriesUnverified(&state, "Signed signature invalid");
    json_decref(root);
    return state;
  }

  std::map<std::string, SignedFileInfo> signedById;
  size_t fileIndex = 0;
  json_t *fileJ = nullptr;
  json_array_foreach(filesJ, fileIndex, fileJ) {
    if (!json_is_object(fileJ)) {
      state.signatureVerified = false;
      state.signatureError = string::f("signed.files[%zu] must be an object", fileIndex);
      markVinylEntriesUnverified(&state, state.signatureError);
      json_decref(root);
      return state;
    }
    json_t *idJ = json_object_get(fileJ, "id");
    json_t *fileNameJ = json_object_get(fileJ, "file");
    json_t *menuIdJ = json_object_get(fileJ, "menuId");
    json_t *sizeJ = json_object_get(fileJ, "size");
    json_t *hashJ = json_object_get(fileJ, "hash");
    if (!json_is_string(idJ) || !json_is_string(fileNameJ) || !json_is_integer(sizeJ) || !json_is_string(hashJ)) {
      state.signatureVerified = false;
      state.signatureError = string::f("signed.files[%zu] missing id/file/size/hash", fileIndex);
      markVinylEntriesUnverified(&state, state.signatureError);
      json_decref(root);
      return state;
    }
    SignedFileInfo info;
    info.id = trimAsciiWhitespace(json_string_value(idJ));
    info.file = trimAsciiWhitespace(json_string_value(fileNameJ));
    if (json_is_integer(menuIdJ)) {
      info.menuId = int(json_integer_value(menuIdJ));
    }
    info.size = uint64_t(std::max<json_int_t>(0, json_integer_value(sizeJ)));
    if (info.id.empty() || !isSafeVinylFileName(info.file) || !parseHexU64(json_string_value(hashJ), &info.hash)) {
      state.signatureVerified = false;
      state.signatureError = string::f("signed.files[%zu] has invalid id/file/hash", fileIndex);
      markVinylEntriesUnverified(&state, state.signatureError);
      json_decref(root);
      return state;
    }
    if (!signedById.emplace(info.id, info).second) {
      state.signatureVerified = false;
      state.signatureError = string::f("signed.files has duplicate id '%s'", info.id.c_str());
      markVinylEntriesUnverified(&state, state.signatureError);
      json_decref(root);
      return state;
    }
  }

  // Top-level signature verification:
  // A known keyId and matching signature are both required.
  bool topLevelSignatureChecked = false;
  bool topLevelSignatureValid = false;
  for (const std::string &secret : vinylManifestVerificationSecrets()) {
    if (computeVinylManifestKeyId(secret) != signedKeyId) {
      continue;
    }
    topLevelSignatureChecked = true;
    uint64_t expectedSignature = 0;
    if (!computeVinylManifestSignature(secret, state.basePath, state.entries, signedById, &expectedSignature)) {
      break;
    }
    if (expectedSignature == signedManifestSignature) {
      topLevelSignatureValid = true;
      break;
    }
  }
  if (!topLevelSignatureChecked) {
    state.signatureVerified = false;
    state.signatureError = "Unknown signing keyId";
    markVinylEntriesUnverified(&state, "Unknown signing keyId");
    json_decref(root);
    return state;
  }
  if (topLevelSignatureChecked && !topLevelSignatureValid) {
    state.signatureVerified = false;
    state.signatureError = "Top-level signature mismatch";
    markVinylEntriesUnverified(&state, "Top-level signature mismatch");
    json_decref(root);
    return state;
  }

  state.verifiedEntryCount = 0;
  for (VinylInventoryEntry &entry : state.entries) {
    entry.signatureVerified = false;
    entry.signatureError.clear();

    auto it = signedById.find(entry.id);
    if (it == signedById.end()) {
      entry.signatureError = "Missing signed entry";
      continue;
    }

    const SignedFileInfo &signedFile = it->second;
    if (signedFile.file != entry.file) {
      entry.signatureError = "Signed file mismatch";
      continue;
    }
    if (entry.menuVisible && signedFile.menuId != entry.menuId) {
      entry.signatureError = "Signed menuId mismatch";
      continue;
    }

    std::string expectedPath = joinInventoryRelativePath(state.basePath, entry.file);
    uint64_t actualHash = 0;
    uint64_t actualSize = 0;
    std::string hashError;
    if (!hashFileFnv64(system::join(sourceRootPath, expectedPath), &actualHash, &actualSize, &hashError)) {
      entry.signatureError = hashError.empty() ? "Failed to read file for signature check" : hashError;
      continue;
    }
    if (actualSize != signedFile.size || actualHash != signedFile.hash) {
      entry.signatureError = "Signature mismatch";
      continue;
    }

    entry.signatureVerified = true;
    ++state.verifiedEntryCount;
  }

  state.signatureVerified = (state.verifiedEntryCount == int(state.entries.size()));
  if (!state.signatureVerified) {
    if (state.verifiedEntryCount <= 0) {
      state.signatureError = "No signed vinyl entries verified";
    } else {
      state.signatureError = string::f("Only %d/%d vinyl entries verified", state.verifiedEntryCount,
                                       int(state.entries.size()));
    }
  } else {
    state.signatureError.clear();
  }

  json_decref(root);
  return state;
}

static VinylInventoryState loadBuiltInVinylInventoryState() {
  const std::string sourceRootPath = asset::plugin(pluginInstance, "");
  return loadVinylInventoryStateFromPath(builtInVinylInventoryPath(), sourceRootPath, "res/Vinyl", VINYL_SOURCE_BUILTIN,
                                         "Built-in");
}

static VinylInventoryState loadExpandedVinylInventoryState() {
  const std::string sourceRootPath = expandedVinylRootPath();
  const std::string path = expandedVinylInventoryPath();
  if (!system::exists(path)) {
    VinylInventoryState state;
    state.valid = true;
    state.source = VINYL_SOURCE_EXPANDED;
    state.sourceRootPath = sourceRootPath;
    state.sourceLabel = "Expanded";
    state.basePath = ".";
    state.signaturePresent = false;
    state.signatureVerified = false;
    return state;
  }
  return loadVinylInventoryStateFromPath(path, sourceRootPath, ".", VINYL_SOURCE_EXPANDED, "Expanded", false);
}

static VinylInventoryState mergeVinylInventoryStates(const VinylInventoryState &builtIn, const VinylInventoryState &expanded) {
  VinylInventoryState merged;
  merged.source = VINYL_SOURCE_BUILTIN;
  merged.sourceLabel = "Merged";
  merged.basePath = "merged";
  merged.valid = builtIn.valid || expanded.valid;
  if (!merged.valid) {
    merged.error = "Both built-in and expanded inventories are invalid";
    return merged;
  }

  std::set<std::string> seenIds;
  std::set<int> seenMenuIds;
  std::map<std::string, size_t> indexById;

  auto assignMenuId = [&](VinylInventoryEntry *entry, bool offsetExpandedMenuIds) {
    if (!entry || !entry->menuVisible) {
      return;
    }
    int candidateMenuId = entry->menuId;
    if (offsetExpandedMenuIds) {
      candidateMenuId = std::max(kExpandedMenuIdOffset, candidateMenuId + kExpandedMenuIdOffset);
    }
    while (!seenMenuIds.insert(candidateMenuId).second) {
      ++candidateMenuId;
    }
    entry->menuId = candidateMenuId;
  };

  auto appendEntries = [&](const VinylInventoryState &state, bool offsetExpandedMenuIds) {
    if (!state.valid) {
      return;
    }
    for (const VinylInventoryEntry &srcEntry : state.entries) {
      VinylInventoryEntry entry = srcEntry;
      auto existingIt = indexById.find(entry.id);
      if (existingIt != indexById.end()) {
        if (entry.source == VINYL_SOURCE_EXPANDED && entry.signatureVerified) {
          // Signed expanded entries can override built-in IDs in-place.
          VinylInventoryEntry &existing = merged.entries[existingIt->second];
          entry.menuVisible = existing.menuVisible;
          entry.menuId = existing.menuId;
          existing = entry;
          continue;
        }
        if (entry.source == VINYL_SOURCE_EXPANDED) {
          std::string fallbackId = "expanded/" + entry.id;
          int suffix = 1;
          while (!seenIds.insert(fallbackId).second) {
            fallbackId = string::f("expanded/%s_%d", entry.id.c_str(), suffix++);
          }
          entry.id = fallbackId;
        } else {
          continue;
        }
      } else {
        seenIds.insert(entry.id);
      }
      assignMenuId(&entry, offsetExpandedMenuIds);
      indexById[entry.id] = merged.entries.size();
      merged.entries.push_back(entry);
    }
  };

  appendEntries(builtIn, false);
  appendEntries(expanded, true);

  merged.verifiedEntryCount = 0;
  for (const VinylInventoryEntry &entry : merged.entries) {
    if (entry.signatureVerified) {
      ++merged.verifiedEntryCount;
    }
  }

  merged.signaturePresent = builtIn.signaturePresent || expanded.signaturePresent;
  merged.signatureVerified = (merged.verifiedEntryCount == int(merged.entries.size()));
  std::vector<std::string> signatureErrors;
  if (builtIn.valid && !builtIn.signatureVerified) {
    signatureErrors.push_back("Built-in: " + builtIn.signatureError);
  }
  if (expanded.valid && !expanded.entries.empty() && !expanded.signatureVerified) {
    signatureErrors.push_back("Expanded: " + expanded.signatureError);
  }
  if (!signatureErrors.empty()) {
    merged.signatureError = signatureErrors.front();
  } else if (!merged.signatureVerified) {
    merged.signatureError = string::f("Only %d/%d vinyl entries verified", merged.verifiedEntryCount,
                                      int(merged.entries.size()));
  }
  if (!builtIn.valid) {
    merged.error = builtIn.error;
  } else if (!expanded.valid) {
    merged.error = expanded.error;
  }
  return merged;
}

static std::atomic<int> gVinylInventoryCacheSerial {0};

static void invalidateVinylInventoryCache() { gVinylInventoryCacheSerial.fetch_add(1, std::memory_order_relaxed); }

static const VinylInventoryState &getBuiltInVinylInventoryState() {
  static int cachedSerial = -1;
  static VinylInventoryState state;
  int serial = gVinylInventoryCacheSerial.load(std::memory_order_relaxed);
  if (cachedSerial != serial) {
    state = loadBuiltInVinylInventoryState();
    cachedSerial = serial;
  }
  return state;
}

static const VinylInventoryState &getExpandedVinylInventoryState() {
  static int cachedSerial = -1;
  static VinylInventoryState state;
  int serial = gVinylInventoryCacheSerial.load(std::memory_order_relaxed);
  if (cachedSerial != serial) {
    state = loadExpandedVinylInventoryState();
    cachedSerial = serial;
  }
  return state;
}

static const VinylInventoryState &getVinylInventoryState() {
  static int cachedSerial = -1;
  static VinylInventoryState state;
  int serial = gVinylInventoryCacheSerial.load(std::memory_order_relaxed);
  if (cachedSerial != serial) {
    state = mergeVinylInventoryStates(getBuiltInVinylInventoryState(), getExpandedVinylInventoryState());
    cachedSerial = serial;
  }
  static bool warned = false;
  if (warned) {
    return state;
  }
  if (!state.valid) {
    warned = true;
    WARN("TemporalDeck: invalid Vinyl inventory.json: %s", state.error.c_str());
  } else if (!state.signaturePresent) {
    warned = true;
    WARN("TemporalDeck: unsigned vinyl inventory: %s", state.signatureError.c_str());
  } else if (!state.signatureVerified) {
    warned = true;
    WARN("TemporalDeck: vinyl inventory signature check failed: %s", state.signatureError.c_str());
  }
  return state;
}

static std::string vinylInventoryTrustStatusLabel(const VinylInventoryState &state) {
  int total = 0;
  int verified = 0;
  if (state.valid) {
    for (const VinylInventoryEntry &entry : state.entries) {
      if (!entry.menuVisible) {
        continue;
      }
      ++total;
      if (entry.signatureVerified) {
        ++verified;
      }
    }
    if (total <= 0) {
      total = int(state.entries.size());
      verified = state.verifiedEntryCount;
    }
  } else {
    total = int(state.entries.size());
    verified = state.verifiedEntryCount;
  }
  total = std::max(total, 0);
  verified = clamp(verified, 0, total);
  return string::f("(%d/%d verified)", verified, total);
}

static const VinylInventoryEntry *findVinylInventoryEntryById(const std::string &id) {
  const VinylInventoryState &state = getVinylInventoryState();
  if (!state.valid) {
    return nullptr;
  }
  for (const VinylInventoryEntry &entry : state.entries) {
    if (entry.id == id) {
      return &entry;
    }
  }
  return nullptr;
}

static const VinylInventoryEntry *findVinylInventoryEntryForPlatterArtMode(int mode) {
  const char *inventoryId = inventoryIdForPlatterArtMode(mode);
  if (!inventoryId) {
    return nullptr;
  }
  return findVinylInventoryEntryById(inventoryId);
}

static std::vector<int> visiblePlatterArtModesFromInventory();

static bool isInventoryPlatterArtModeVerified(int mode, std::string *errorOut = nullptr) {
  const VinylInventoryState &state = getVinylInventoryState();
  if (!state.valid) {
    if (errorOut) {
      *errorOut = state.error;
    }
    return false;
  }
  const VinylInventoryEntry *entry = findVinylInventoryEntryForPlatterArtMode(mode);
  if (!entry) {
    if (errorOut) {
      *errorOut = "No matching inventory entry";
    }
    return false;
  }
  if (!entry->signatureVerified && errorOut) {
    *errorOut = entry->signatureError;
  }
  return entry->signatureVerified;
}

static const VinylInventoryEntry *defaultPreviewVinylEntryFromInventory() {
  const VinylInventoryState &inventoryState = getVinylInventoryState();
  if (!inventoryState.valid) {
    return nullptr;
  }

  const VinylInventoryEntry *firstMatch = nullptr;
  for (const VinylInventoryEntry &entry : inventoryState.entries) {
    if (!entry.menuVisible || entry.menuId != 1) {
      continue;
    }
    if (!firstMatch) {
      firstMatch = &entry;
    }
    if (entry.signatureVerified) {
      return &entry;
    }
  }
  return firstMatch;
}

static std::string vinylAbsolutePathForPlatterArtMode(int mode) {
  if (const VinylInventoryEntry *entry = findVinylInventoryEntryForPlatterArtMode(mode)) {
    return inventoryAbsolutePath(*entry);
  }
  std::string fallbackRelative = fallbackVinylRelativePathForPlatterArtMode(mode);
  if (fallbackRelative.empty()) {
    return "";
  }
  return asset::plugin(pluginInstance, fallbackRelative);
}

static std::string vinylLabelForPlatterArtMode(int mode) {
  const char *inventoryId = inventoryIdForPlatterArtMode(mode);
  if (inventoryId) {
    if (const VinylInventoryEntry *entry = findVinylInventoryEntryById(inventoryId)) {
      if (!entry->label.empty()) {
        return entry->label;
      }
    }
  }
  return TemporalDeck::platterArtModeLabelFor(mode);
}

static std::vector<int> visiblePlatterArtModesFromInventory() {
  std::vector<std::pair<int, int>> rankedModes;
  std::set<int> seenModes;
  const VinylInventoryState &state = getVinylInventoryState();
  if (!state.valid) {
    return {};
  }
  for (const VinylInventoryEntry &entry : state.entries) {
    if (!entry.menuVisible) {
      continue;
    }
    int mode = -1;
    if (!platterArtModeForInventoryId(entry.id, &mode)) {
      continue;
    }
    if (!seenModes.insert(mode).second) {
      continue;
    }
    rankedModes.push_back(std::make_pair(entry.menuId, mode));
  }
  std::sort(rankedModes.begin(), rankedModes.end(),
            [](const std::pair<int, int> &a, const std::pair<int, int> &b) { return a.first < b.first; });

  std::vector<int> modes;
  modes.reserve(rankedModes.size());
  for (const std::pair<int, int> &p : rankedModes) {
    modes.push_back(p.second);
  }
  return modes;
}

static std::vector<const VinylInventoryEntry *> visibleCustomVinylEntriesFromInventory() {
  const VinylInventoryState &state = getVinylInventoryState();
  if (!state.valid) {
    return {};
  }
  std::vector<const VinylInventoryEntry *> entries;
  for (const VinylInventoryEntry &entry : state.entries) {
    if (!entry.menuVisible) {
      continue;
    }
    int mappedMode = -1;
    if (platterArtModeForInventoryId(entry.id, &mappedMode)) {
      continue;
    }
    entries.push_back(&entry);
  }
  std::sort(entries.begin(), entries.end(), [](const VinylInventoryEntry *a, const VinylInventoryEntry *b) {
    if (a->menuId != b->menuId) {
      return a->menuId < b->menuId;
    }
    return a->label < b->label;
  });
  return entries;
}

static std::string normalizedPathForCompare(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
#if defined ARCH_WIN
  std::transform(path.begin(), path.end(), path.begin(), [](unsigned char c) { return char(std::tolower(c)); });
#endif
  return path;
}

static const VinylInventoryEntry *findExpandedVinylEntryByAbsolutePath(const std::string &absolutePath) {
  const VinylInventoryState &state = getVinylInventoryState();
  if (!state.valid) {
    return nullptr;
  }
  std::string needle = normalizedPathForCompare(absolutePath);
  const VinylInventoryEntry *firstMatch = nullptr;
  for (const VinylInventoryEntry &entry : state.entries) {
    if (entry.source != VINYL_SOURCE_EXPANDED) {
      continue;
    }
    if (normalizedPathForCompare(entry.absolutePath) == needle) {
      if (!firstMatch) {
        firstMatch = &entry;
      }
      if (entry.signatureVerified) {
        return &entry;
      }
    }
  }
  return firstMatch;
}

static const VinylInventoryEntry *findVinylInventoryEntryByAbsolutePath(const std::string &absolutePath) {
  const VinylInventoryState &state = getVinylInventoryState();
  if (!state.valid) {
    return nullptr;
  }
  std::string needle = normalizedPathForCompare(absolutePath);
  const VinylInventoryEntry *firstMatch = nullptr;
  for (const VinylInventoryEntry &entry : state.entries) {
    if (normalizedPathForCompare(entry.absolutePath) == needle) {
      if (!firstMatch) {
        firstMatch = &entry;
      }
      if (entry.signatureVerified) {
        return &entry;
      }
    }
  }
  return firstMatch;
}

static std::string expandedArtLoadPath(const std::string &absolutePath) {
  const VinylInventoryEntry *entry = findExpandedVinylEntryByAbsolutePath(absolutePath);
  if (!entry) {
    return absolutePath;
  }
  uint64_t salt = gExpandedVinylLoadSalt.load(std::memory_order_relaxed);
  if (salt == 0) {
    return absolutePath;
  }
  std::string normalized = absolutePath;
  std::replace(normalized.begin(), normalized.end(), '\\', '/');
  size_t slash = normalized.find_last_of('/');
  if (slash == std::string::npos || slash + 1 >= normalized.size()) {
    return absolutePath;
  }
  std::string dir = normalized.substr(0, slash);
  std::string file = normalized.substr(slash + 1);
  std::string alias = dir;
  std::string saltDigits = std::to_string(salt);
  for (char c : saltDigits) {
    int reps = 1 + (c - '0');
    for (int i = 0; i < reps; ++i) {
      alias += "/.";
    }
  }
  alias += "/";
  alias += file;
  return alias;
}

static bool hashFileFnv64(const std::string &path, uint64_t *hashOut, uint64_t *sizeOut, std::string *errorOut) {
  std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
  if (!in.good()) {
    if (errorOut) {
      *errorOut = string::f("Failed to open file: %s", path.c_str());
    }
    return false;
  }
  uint64_t hash = fnv1aInit64();
  uint64_t sizeBytes = 0;
  char buffer[32 * 1024];
  while (in.good()) {
    in.read(buffer, sizeof(buffer));
    std::streamsize got = in.gcount();
    if (got <= 0) {
      break;
    }
    sizeBytes += uint64_t(got);
    hash = fnv1aUpdate64(hash, reinterpret_cast<const uint8_t *>(buffer), size_t(got));
  }
  if (!in.eof() && in.fail()) {
    if (errorOut) {
      *errorOut = string::f("Read error while hashing file: %s", path.c_str());
    }
    return false;
  }
  *hashOut = hash;
  *sizeOut = sizeBytes;
  return true;
}

static bool collectVinylSignatureRecords(const VinylInventoryState &inventory, int sourceFilter,
                                         std::vector<VinylSignatureRecord> *recordsOut, std::string *errorOut) {
  if (!recordsOut) {
    return false;
  }
  std::vector<VinylSignatureRecord> records;
  std::set<std::string> seenIds;
  if (!inventory.valid) {
    if (errorOut) {
      *errorOut = inventory.error.empty() ? "Failed to load inventory.json" : inventory.error;
    }
    return false;
  }

  std::string commonBasePath;
  for (const VinylInventoryEntry &entry : inventory.entries) {
    if (sourceFilter >= 0 && entry.source != sourceFilter) {
      continue;
    }
    if (!seenIds.insert(entry.id).second) {
      continue;
    }
    VinylSignatureRecord rec;
    rec.id = entry.id;
    rec.label = entry.label;
    rec.file = entry.file;
    rec.menuVisible = entry.menuVisible;
    rec.menuId = entry.menuId;
    rec.basePath = entry.basePath;
    rec.sourceRootPath = entry.sourceRootPath;
    rec.absolutePath = inventoryAbsolutePath(entry);
    if (commonBasePath.empty()) {
      commonBasePath = rec.basePath;
    } else if (commonBasePath != rec.basePath) {
      if (errorOut) {
        *errorOut = "Cannot sign mixed inventories with different basePath values";
      }
      return false;
    }
    records.push_back(rec);
  }
  if (records.empty()) {
    if (errorOut) {
      *errorOut = "No vinyl files listed in inventory.json";
    }
    return false;
  }

  *recordsOut = std::move(records);
  return true;
}

static std::string ensureJsonExtension(std::string path) {
  std::string ext = system::getExtension(path);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return char(std::tolower(c)); });
  if (ext != ".json") {
    path += ".json";
  }
  return path;
}

static std::string lowercaseExtension(const std::string &path);

struct VinylDownloadPlan {
  struct FileItem {
    std::string file;
    bool hasSignature = false;
    uint64_t signedSize = 0;
    uint64_t signedHash = 0;
  };
  std::vector<FileItem> files;
};

static bool loadVinylDownloadPlan(const std::string &inventoryPath, VinylDownloadPlan *planOut, std::string *errorOut) {
  if (!planOut) {
    return false;
  }
  json_error_t error;
  json_t *root = json_load_file(inventoryPath.c_str(), 0, &error);
  if (!root) {
    if (errorOut) {
      *errorOut = string::f("Failed to parse downloaded expanded.json (line %d): %s", error.line, error.text);
    }
    return false;
  }
  if (!json_is_object(root)) {
    if (errorOut) {
      *errorOut = "Downloaded expanded.json root must be an object";
    }
    json_decref(root);
    return false;
  }

  VinylDownloadPlan plan;
  json_t *vinylJ = json_object_get(root, "vinyl");
  if (!json_is_array(vinylJ)) {
    if (errorOut) {
      *errorOut = "Downloaded expanded.json must contain a vinyl[] array";
    }
    json_decref(root);
    return false;
  }

  std::set<std::string> seenFiles;
  size_t i = 0;
  json_t *itemJ = nullptr;
  std::vector<std::string> listedFiles;
  json_array_foreach(vinylJ, i, itemJ) {
    if (!json_is_object(itemJ)) {
      continue;
    }
    json_t *fileJ = json_object_get(itemJ, "file");
    if (!json_is_string(fileJ)) {
      continue;
    }
    std::string file = trimAsciiWhitespace(json_string_value(fileJ));
    if (!isSafeVinylFileName(file)) {
      continue;
    }
    if (!seenFiles.insert(file).second) {
      continue;
    }
    listedFiles.push_back(file);
  }

  struct SignedFileMeta {
    uint64_t size = 0;
    uint64_t hash = 0;
  };
  std::map<std::string, SignedFileMeta> signedByFile;
  json_t *signedJ = json_object_get(root, "signed");
  if (json_is_object(signedJ)) {
    json_t *filesJ = json_object_get(signedJ, "files");
    if (json_is_array(filesJ)) {
      size_t fileIndex = 0;
      json_t *fileJ = nullptr;
      json_array_foreach(filesJ, fileIndex, fileJ) {
        if (!json_is_object(fileJ)) {
          continue;
        }
        json_t *fileNameJ = json_object_get(fileJ, "file");
        json_t *sizeJ = json_object_get(fileJ, "size");
        json_t *hashJ = json_object_get(fileJ, "hash");
        if (!json_is_string(fileNameJ) || !json_is_integer(sizeJ) || !json_is_string(hashJ)) {
          continue;
        }
        std::string file = trimAsciiWhitespace(json_string_value(fileNameJ));
        if (!isSafeVinylFileName(file)) {
          continue;
        }
        uint64_t parsedHash = 0;
        if (!parseHexU64(json_string_value(hashJ), &parsedHash)) {
          continue;
        }
        SignedFileMeta meta;
        meta.size = uint64_t(std::max<json_int_t>(0, json_integer_value(sizeJ)));
        meta.hash = parsedHash;
        signedByFile[file] = meta;
      }
    }
  }

  for (const std::string &file : listedFiles) {
    VinylDownloadPlan::FileItem item;
    item.file = file;
    auto it = signedByFile.find(file);
    if (it != signedByFile.end()) {
      item.hasSignature = true;
      item.signedSize = it->second.size;
      item.signedHash = it->second.hash;
    }
    plan.files.push_back(item);
  }
  json_decref(root);
  if (plan.files.empty()) {
    if (errorOut) {
      *errorOut = "Downloaded inventory contains no usable file entries";
    }
    return false;
  }

  *planOut = std::move(plan);
  return true;
}

static bool downloadExpandedVinylInventory(std::string *errorOut, int *fileCountOut) {
  struct ScopedExpandedVinylSync {
    ScopedExpandedVinylSync() { gExpandedVinylSyncDepth.fetch_add(1, std::memory_order_relaxed); }
    ~ScopedExpandedVinylSync() { gExpandedVinylSyncDepth.fetch_sub(1, std::memory_order_relaxed); }
  } scopedExpandedVinylSync;

  const std::string finalRoot = expandedVinylRootPath();
  const std::string tempRoot = finalRoot + ".tmp";
  long long syncMs = (long long)std::llround(system::getUnixTime() * 1000.0);
  uint64_t syncSeq = gExpandedVinylSyncNonceSeq.fetch_add(1, std::memory_order_relaxed);
  const std::string cacheBuster = string::f("tdcb=%lld_%llu", syncMs, (unsigned long long)syncSeq);
  system::removeRecursively(tempRoot);
  if (!system::createDirectories(tempRoot)) {
    if (errorOut) {
      *errorOut = "Failed to create temporary expansion directory";
    }
    return false;
  }

  const std::string inventoryPath = system::join(tempRoot, "expanded.json");
  const std::string inventoryUrl = std::string(kVinylExpansionBaseUrl) + "/expanded.json?" + cacheBuster;
  if (!network::requestDownload(inventoryUrl, inventoryPath)) {
    system::removeRecursively(tempRoot);
    if (errorOut) {
      *errorOut = "Failed to download expanded.json";
    }
    return false;
  }

  VinylDownloadPlan plan;
  if (!loadVinylDownloadPlan(inventoryPath, &plan, errorOut)) {
    system::removeRecursively(tempRoot);
    return false;
  }
  std::vector<const VinylDownloadPlan::FileItem *> missingFiles;
  std::vector<const VinylDownloadPlan::FileItem *> staleFiles;
  auto queueOrReuse = [&](const VinylDownloadPlan::FileItem &item) -> bool {
    std::string existingPath = system::join(finalRoot, item.file);
    bool exists = system::isFile(existingPath);
    if (item.hasSignature && exists) {
      uint64_t actualHash = 0;
      uint64_t actualSize = 0;
      std::string hashError;
      if (hashFileFnv64(existingPath, &actualHash, &actualSize, &hashError) && actualSize == item.signedSize &&
          actualHash == item.signedHash) {
        std::string dstPath = system::join(tempRoot, item.file);
        if (!system::copy(existingPath, dstPath)) {
          if (errorOut) {
            *errorOut = string::f("Failed to stage cached vinyl file: %s", item.file.c_str());
          }
          return false;
        }
        return true;
      }
    }
    if (!exists) {
      missingFiles.push_back(&item);
    } else {
      staleFiles.push_back(&item);
    }
    return true;
  };

  for (const VinylDownloadPlan::FileItem &item : plan.files) {
    if (!queueOrReuse(item)) {
      system::removeRecursively(tempRoot);
      return false;
    }
  }

  int totalFilesToDownload = int(missingFiles.size() + staleFiles.size());
  gExpandedVinylDownloadTotalFiles.store(totalFilesToDownload, std::memory_order_relaxed);
  gExpandedVinylDownloadCurrentIndex.store(0, std::memory_order_relaxed);
  int currentFetchIndex = 0;

  auto downloadQueued = [&](const std::vector<const VinylDownloadPlan::FileItem *> &queue) -> bool {
    for (const VinylDownloadPlan::FileItem *item : queue) {
      if (!item) {
        continue;
      }
      gExpandedVinylDownloadCurrentIndex.store(++currentFetchIndex, std::memory_order_relaxed);
      std::string encodedFile = network::encodeUrl(item->file);
      std::string fileUrl = std::string(kVinylExpansionBaseUrl) + "/" + encodedFile + "?" + cacheBuster;
      std::string filePath = system::join(tempRoot, item->file);
      if (!network::requestDownload(fileUrl, filePath)) {
        if (errorOut) {
          *errorOut = string::f("Failed to download vinyl file: %s", item->file.c_str());
        }
        return false;
      }
    }
    return true;
  };

  if (!downloadQueued(missingFiles) || !downloadQueued(staleFiles)) {
    system::removeRecursively(tempRoot);
    return false;
  }
  // All files fetched; keep plain SYNC while finalizing install.
  gExpandedVinylDownloadCurrentIndex.store(0, std::memory_order_relaxed);

  system::removeRecursively(finalRoot);
  if (!system::rename(tempRoot, finalRoot)) {
    if (!system::copy(tempRoot, finalRoot)) {
      system::removeRecursively(tempRoot);
      if (errorOut) {
        *errorOut = "Failed to install expanded vinyl files into user folder";
      }
      return false;
    }
    system::removeRecursively(tempRoot);
  }

  if (fileCountOut) {
    *fileCountOut = int(plan.files.size());
  }
  gExpandedVinylLoadSalt.fetch_add(1, std::memory_order_relaxed);
  invalidateVinylInventoryCache();
  return true;
}

static bool startExpandedVinylDownloadAsync(std::string *errorOut) {
  bool expected = false;
  if (!gExpandedVinylDownloadRunning.compare_exchange_strong(expected, true, std::memory_order_relaxed)) {
    if (errorOut) {
      *errorOut = "Vinyl expansion sync already in progress";
    }
    return false;
  }
  gExpandedVinylDownloadResultPending.store(false, std::memory_order_relaxed);
  gExpandedVinylDownloadCurrentIndex.store(0, std::memory_order_relaxed);
  gExpandedVinylDownloadTotalFiles.store(0, std::memory_order_relaxed);
  {
    std::lock_guard<std::mutex> lock(gExpandedVinylDownloadResultMutex);
    gExpandedVinylDownloadResultError.clear();
  }
  std::thread([]() {
    std::string error;
    int fileCount = 0;
    bool ok = downloadExpandedVinylInventory(&error, &fileCount);
    {
      std::lock_guard<std::mutex> lock(gExpandedVinylDownloadResultMutex);
      gExpandedVinylDownloadResultError = ok ? "" : (error.empty() ? "Failed to download Vinyl expansion" : error);
    }
    gExpandedVinylDownloadCurrentIndex.store(0, std::memory_order_relaxed);
    gExpandedVinylDownloadTotalFiles.store(0, std::memory_order_relaxed);
    gExpandedVinylDownloadRunning.store(false, std::memory_order_relaxed);
    gExpandedVinylDownloadResultPending.store(true, std::memory_order_relaxed);
  }).detach();
  return true;
}

static void pumpExpandedVinylDownloadNotifications() {
  if (!gExpandedVinylDownloadResultPending.exchange(false, std::memory_order_relaxed)) {
    return;
  }
  std::string error;
  {
    std::lock_guard<std::mutex> lock(gExpandedVinylDownloadResultMutex);
    error = gExpandedVinylDownloadResultError;
    gExpandedVinylDownloadResultError.clear();
  }
  if (!error.empty()) {
    WARN("TemporalDeck: Vinyl library sync failed: %s", error.c_str());
  }
}

static bool writeSignedVinylManifest(const std::string &path, const VinylInventoryState &inventory, int sourceFilter,
                                     std::string *errorOut, std::string *signatureOut, int *fileCountOut) {
  const std::string secret = kVinylManifestFallbackSecret;

  std::vector<VinylSignatureRecord> records;
  if (!collectVinylSignatureRecords(inventory, sourceFilter, &records, errorOut)) {
    return false;
  }
  for (VinylSignatureRecord &record : records) {
    if (!hashFileFnv64(record.absolutePath, &record.contentHash, &record.sizeBytes, errorOut)) {
      return false;
    }
  }

  std::string basePath = records.front().basePath;
  if (!isSafeInventoryBasePath(basePath)) {
    if (errorOut) {
      *errorOut = "Invalid inventory basePath";
    }
    return false;
  }

  uint64_t keyId = computeVinylManifestKeyId(secret);
  uint64_t signature = fnv1aUpdate64String(fnv1aInit64(), kVinylManifestDomain);
  signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  signature = fnv1aUpdate64String(signature, secret);
  signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  signature = fnv1aUpdate64String(signature, basePath);
  signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  for (const VinylSignatureRecord &record : records) {
    signature = fnv1aUpdate64String(signature, record.id);
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, record.label);
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, record.file);
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, record.menuVisible ? "1" : "0");
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, std::to_string(record.menuId));
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, std::to_string(record.sizeBytes));
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
    signature = fnv1aUpdate64String(signature, hexU64(record.contentHash));
    signature = fnv1aUpdate64(signature, reinterpret_cast<const uint8_t *>("\0"), 1);
  }

  std::ofstream out(path.c_str(), std::ios::out | std::ios::trunc);
  if (!out.good()) {
    if (errorOut) {
      *errorOut = "Failed to open output file for writing";
    }
    return false;
  }

  long long unixTime = (long long)std::llround(system::getUnixTime());
  out << "{\n";
  out << "  \"type\": \"TemporalDeckVinylInventory\",\n";
  out << "  \"version\": 1,\n";
  out << "  \"basePath\": \"" << jsonEscape(basePath) << "\",\n";
  out << "  \"vinyl\": [\n";
  for (size_t i = 0; i < records.size(); ++i) {
    const VinylSignatureRecord &record = records[i];
    std::string ext = lowercaseExtension(record.file);
    std::string format = ext;
    if (!format.empty() && format[0] == '.') {
      format.erase(0, 1);
    }
    out << "    {\n";
    out << "      \"id\": \"" << jsonEscape(record.id) << "\",\n";
    out << "      \"label\": \"" << jsonEscape(record.label) << "\",\n";
    out << "      \"file\": \"" << jsonEscape(record.file) << "\",\n";
    out << "      \"format\": \"" << jsonEscape(format) << "\",\n";
    out << "      \"menuVisible\": " << (record.menuVisible ? "true" : "false") << ",\n";
    out << "      \"menuId\": " << record.menuId << "\n";
    out << "    }";
    if (i + 1 < records.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "  ],\n";
  out << "  \"signed\": {\n";
  out << "    \"algorithm\": \"" << kVinylManifestAlgorithm << "\",\n";
  out << "    \"generatedUnix\": " << unixTime << ",\n";
  out << "    \"keyId\": \"" << hexU64(keyId) << "\",\n";
  out << "    \"signature\": \"" << hexU64(signature) << "\",\n";
  out << "    \"files\": [\n";
  for (size_t i = 0; i < records.size(); ++i) {
    const VinylSignatureRecord &record = records[i];
    out << "      {\n";
    out << "        \"id\": \"" << jsonEscape(record.id) << "\",\n";
    out << "        \"file\": \"" << jsonEscape(record.file) << "\",\n";
    out << "        \"menuId\": " << record.menuId << ",\n";
    out << "        \"size\": " << record.sizeBytes << ",\n";
    out << "        \"hash\": \"" << hexU64(record.contentHash) << "\"\n";
    out << "      }";
    if (i + 1 < records.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ]\n";
  out << "  }\n";
  out << "}\n";

  if (!out.good()) {
    if (errorOut) {
      *errorOut = "Failed while writing signed manifest";
    }
    return false;
  }

  if (signatureOut) {
    *signatureOut = hexU64(signature);
  }
  if (fileCountOut) {
    *fileCountOut = int(records.size());
  }
  return true;
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

static int loadPlatterMipmapImageHandle(NVGcontext *vg, const std::string &path) {
  struct MipmapCache {
    struct Entry {
      int handle = -1;
      uint64_t lastUse = 0;
    };
    NVGcontext *vg = nullptr;
    std::unordered_map<std::string, Entry> entries;
    uint64_t useCounter = 0;
  };
  static MipmapCache cache;
  constexpr size_t kMaxMipmapImageCacheEntries = 16;

  if (!vg || path.empty()) {
    return -1;
  }
  if (cache.vg != vg) {
    cache.vg = vg;
    cache.entries.clear();
    cache.useCounter = 0;
  }
  auto it = cache.entries.find(path);
  if (it != cache.entries.end()) {
    it->second.lastUse = ++cache.useCounter;
    return it->second.handle;
  }

  int handle = nvgCreateImage(vg, path.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
  if (handle >= 0) {
    MipmapCache::Entry entry;
    entry.handle = handle;
    entry.lastUse = ++cache.useCounter;
    cache.entries[path] = entry;
    if (cache.entries.size() > kMaxMipmapImageCacheEntries) {
      auto evictIt = cache.entries.begin();
      for (auto iter = cache.entries.begin(); iter != cache.entries.end(); ++iter) {
        if (iter->second.lastUse < evictIt->second.lastUse) {
          evictIt = iter;
        }
      }
      if (evictIt != cache.entries.end()) {
        nvgDeleteImage(vg, evictIt->second.handle);
        cache.entries.erase(evictIt);
      }
    }
  }
  return handle;
}

static bool drawPlatterImagePath(const Widget::DrawArgs &args, const std::string &path, Vec center,
                                 float platterRadiusPx, float rotation) {
  int handle = loadPlatterMipmapImageHandle(args.vg, path);
  if (handle < 0) {
    return false;
  }
  int imageW = 0;
  int imageH = 0;
  nvgImageSize(args.vg, handle, &imageW, &imageH);
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
  NVGpaint imgPaint = nvgImagePattern(args.vg, -drawW * 0.5f, -drawH * 0.5f, drawW, drawH, 0.f, handle, 1.0f);
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
  constexpr float kLoopIconScale = 1.35f;
  return std::fabs(panelPos.x - c.x) <= 7.2f * kLoopIconScale && std::fabs(panelPos.y - c.y) <= 4.8f * kLoopIconScale;
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
      constexpr float kLoopIconScale = 1.35f;
      float loopHalfW = 4.25f * kLoopIconScale;
      float loopHalfH = 2.55f * kLoopIconScale;
      float neck = 1.7f * kLoopIconScale;
      nvgStrokeColor(args.vg, loopColor);
      nvgStrokeWidth(args.vg, 1.2f * kLoopIconScale);
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
  pumpExpandedVinylDownloadNotifications();
  syncTraceCaptureState();
  pollMiddleQuickSlipTrigger();
  updateUiFpsTracking();
  nvgSave(args.vg);
  float rotation = module ? module->getUiPlatterAngle() : 0.f;
  Vec center = localCenter();
  const VinylInventoryEntry *defaultPreviewEntry = defaultPreviewVinylEntryFromInventory();

  if (module && module->consumePendingInitialPlatterArtSelection()) {
    if (defaultPreviewEntry && defaultPreviewEntry->signatureVerified) {
      int mappedMode = -1;
      if (platterArtModeForInventoryId(defaultPreviewEntry->id, &mappedMode)) {
        module->setPlatterArtMode(mappedMode);
      } else {
        module->setCustomPlatterArtPath(inventoryAbsolutePath(*defaultPreviewEntry));
      }
    } else {
      module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL);
    }
  }

  int artMode = module ? module->getPlatterArtMode() : TemporalDeck::PLATTER_ART_PROCEDURAL;
  if (module && artMode != TemporalDeck::PLATTER_ART_PROCEDURAL && artMode != TemporalDeck::PLATTER_ART_CUSTOM &&
      !isInventoryPlatterArtModeVerified(artMode)) {
    module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL);
    artMode = TemporalDeck::PLATTER_ART_PROCEDURAL;
  }
  if (module && artMode == TemporalDeck::PLATTER_ART_CUSTOM) {
    const VinylInventoryEntry *inventoryEntry = findVinylInventoryEntryByAbsolutePath(module->getCustomPlatterArtPath());
    if (inventoryEntry && !inventoryEntry->signatureVerified) {
      module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL);
      artMode = TemporalDeck::PLATTER_ART_PROCEDURAL;
    }
  }

  // Primary platter render path is selectable: built-in SVG, custom file, or
  // procedural fallback.
  bool drewArt = false;
  float platterDimAlpha = 0.f;
  if (APP && APP->window) {
    int inventorySerial = gVinylInventoryCacheSerial.load(std::memory_order_relaxed);
    std::string currentPath;
    if (!module && defaultPreviewEntry && defaultPreviewEntry->signatureVerified) {
      currentPath = inventoryAbsolutePath(*defaultPreviewEntry);
    } else if (module && artMode == TemporalDeck::PLATTER_ART_CUSTOM) {
      currentPath = module->getCustomPlatterArtPath();
    }
    bool renderKeyChanged = (cachedRenderArtMode != artMode || cachedRenderInventorySerial != inventorySerial ||
                             cachedRenderPath != currentPath);
    if (renderKeyChanged) {
      cachedRenderArtMode = artMode;
      cachedRenderInventorySerial = inventorySerial;
      cachedRenderPath = currentPath;
      cachedRenderValid = false;
      cachedRenderSvg = false;
      cachedRenderAbsolutePath.clear();
      cachedRenderLoadPath.clear();

      auto setCachedPath = [&](const std::string &absolutePath) {
        if (absolutePath.empty()) {
          return;
        }
        std::string ext = lowercaseExtension(absolutePath);
        if (ext != ".svg" && ext != ".png" && ext != ".jpg" && ext != ".jpeg") {
          return;
        }
        cachedRenderValid = true;
        cachedRenderSvg = (ext == ".svg");
        cachedRenderAbsolutePath = absolutePath;
        cachedRenderLoadPath = expandedArtLoadPath(absolutePath);
      };

      if (!module && defaultPreviewEntry && defaultPreviewEntry->signatureVerified) {
        setCachedPath(inventoryAbsolutePath(*defaultPreviewEntry));
      } else if (module && artMode == TemporalDeck::PLATTER_ART_CUSTOM) {
        setCachedPath(currentPath);
      } else if (artMode != TemporalDeck::PLATTER_ART_PROCEDURAL && isInventoryPlatterArtModeVerified(artMode)) {
        setCachedPath(vinylAbsolutePathForPlatterArtMode(artMode));
      }
    }

    // In module-browser preview there is no backing module instance, so pick a
    // deterministic default art instead of falling through to procedural.
    platterDimAlpha = module ? platterDimmingOverlayAlphaForMode(module->getPlatterBrightnessMode()) : 0.f;
    if (cachedRenderValid && !module) {
      try {
        if (cachedRenderSvg) {
          drewArt = drawPlatterSvg(args, APP->window->loadSvg(cachedRenderLoadPath), center, platterRadiusPx, rotation);
        } else {
          drewArt = drawPlatterImagePath(args, cachedRenderLoadPath, center, platterRadiusPx, rotation);
          if (!drewArt) {
            drewArt =
              drawPlatterImage(args, APP->window->loadImage(cachedRenderLoadPath), center, platterRadiusPx, rotation);
          }
        }
      } catch (const std::exception &e) {
        WARN("TemporalDeck: failed to load default preview platter art '%s': %s", cachedRenderAbsolutePath.c_str(),
             e.what());
      }
    } else if (cachedRenderValid && artMode == TemporalDeck::PLATTER_ART_CUSTOM) {
      try {
        if (cachedRenderSvg) {
          drewArt = drawPlatterSvg(args, APP->window->loadSvg(cachedRenderLoadPath), center, platterRadiusPx, rotation);
        } else {
          drewArt = drawPlatterImagePath(args, cachedRenderLoadPath, center, platterRadiusPx, rotation);
          if (!drewArt) {
            drewArt = drawPlatterImage(args, APP->window->loadImage(cachedRenderLoadPath), center, platterRadiusPx,
                                       rotation);
          }
        }
      } catch (const std::exception &e) {
        WARN("TemporalDeck: failed to load custom platter art '%s' (load path '%s'): %s",
             cachedRenderAbsolutePath.c_str(), cachedRenderLoadPath.c_str(), e.what());
      }
    } else if (cachedRenderValid && artMode != TemporalDeck::PLATTER_ART_PROCEDURAL) {
      try {
        if (cachedRenderSvg) {
          drewArt = drawPlatterSvg(args, APP->window->loadSvg(cachedRenderAbsolutePath), center, platterRadiusPx,
                                   rotation);
        } else {
          drewArt = drawPlatterImagePath(args, cachedRenderAbsolutePath, center, platterRadiusPx, rotation);
          if (!drewArt) {
            drewArt =
              drawPlatterImage(args, APP->window->loadImage(cachedRenderAbsolutePath), center, platterRadiusPx, rotation);
          }
        }
      } catch (const std::exception &e) {
        WARN("TemporalDeck: failed to load platter art '%s': %s", cachedRenderAbsolutePath.c_str(), e.what());
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
  // but use a tighter dwell so wheel gestures release sooner.
  float holdSeconds = module->isSlipLatched() ? 0.14f : 0.075f;
  holdSeconds += 0.045f * lowFpsComp;
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
      menu->addChild(createMenuLabel("Advanced"));
      menu->addChild(createSubmenuItem("Vinyl", "", [=](Menu *submenu) {
        bool dragonKingDebug = isDragonKingDebugEnabled();
        const VinylInventoryState &inventoryState = getVinylInventoryState();
        bool downloadRunning = isExpandedVinylDownloadRunning();
        submenu->addChild(createMenuItem("Sync Vinyl Library", downloadRunning ? "Syncing..." : "", [=]() {
          startExpandedVinylDownloadAsync(nullptr);
        }, downloadRunning));
        submenu->addChild(createMenuLabel(string::f("Library: %s", vinylInventoryTrustStatusLabel(inventoryState).c_str())));
        if (!inventoryState.valid) {
          module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL);
          if (!inventoryState.error.empty()) {
            submenu->addChild(createMenuLabel(inventoryState.error));
          }
          submenu->addChild(createMenuLabel("Platter art locked to Procedural"));
          submenu->addChild(createSubmenuItem("Brightness", "", [=](Menu *brightnessMenu) {
            for (int i = 0; i < TemporalDeck::PLATTER_BRIGHTNESS_COUNT; ++i) {
              brightnessMenu->addChild(createCheckMenuItem(
                TemporalDeck::platterBrightnessLabelFor(i), "",
                [=]() { return module->getPlatterBrightnessMode() == i; },
                [=]() { module->setPlatterBrightnessMode(i); }));
            }
          }));
          return;
        }

        std::vector<int> visibleModes = visiblePlatterArtModesFromInventory();
        if (visibleModes.empty()) {
          submenu->addChild(createMenuLabel("No platter art entries marked menuVisible"));
        } else {
          for (int mode : visibleModes) {
            std::string modeLabel = vinylLabelForPlatterArtMode(mode);
            bool modeVerified = isInventoryPlatterArtModeVerified(mode);
            std::string rightText = modeVerified ? "" : "Signiture error";
            submenu->addChild(createCheckMenuItem(
              modeLabel, rightText, [=]() { return module->getPlatterArtMode() == mode; },
              [=]() {
                if (isInventoryPlatterArtModeVerified(mode)) {
                  module->setPlatterArtMode(mode);
                } else {
                  module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL);
                }
              },
              !modeVerified));
          }
        }
        std::vector<const VinylInventoryEntry *> customEntries = visibleCustomVinylEntriesFromInventory();
        if (!customEntries.empty()) {
          for (const VinylInventoryEntry *entry : customEntries) {
            if (!entry) {
              continue;
            }
            std::string absolutePath = inventoryAbsolutePath(*entry);
            bool entryVerified = entry->signatureVerified;
            std::string entryLabel = entry->label.empty() ? entry->id : entry->label;
            std::string rightText;
            if (!entryVerified) {
              rightText = "Signiture error";
            }
            submenu->addChild(createCheckMenuItem(
              entryLabel, rightText,
              [=]() {
                return module->getPlatterArtMode() == TemporalDeck::PLATTER_ART_CUSTOM &&
                       module->getCustomPlatterArtPath() == absolutePath;
              },
              [=]() {
                if (entryVerified) {
                  module->setCustomPlatterArtPath(absolutePath);
                } else {
                  module->setPlatterArtMode(TemporalDeck::PLATTER_ART_PROCEDURAL);
                }
              },
              !entryVerified || !system::isFile(absolutePath)));
          }
        }
        submenu->addChild(new MenuSeparator());
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
      menu->addChild(createSubmenuItem("Scratch interpolation", "", [=](Menu *submenu) {
        for (int i = 0; i < TemporalDeck::SCRATCH_INTERP_COUNT; ++i) {
          submenu->addChild(createCheckMenuItem(
            TemporalDeck::scratchInterpolationLabelFor(i), "",
            [=]() { return module->getScratchInterpolationMode() == i; },
            [=]() { module->setScratchInterpolationMode(i); }));
        }
      }));
      menu->addChild(createSubmenuItem("Gate+Pos mode", "", [=](Menu *submenu) {
        for (int i = 0; i < TemporalDeck::EXTERNAL_GATE_POS_COUNT; ++i) {
          submenu->addChild(createCheckMenuItem(
            TemporalDeck::externalGatePosLabelFor(i), "",
            [=]() { return module->getExternalGatePosMode() == i; },
            [=]() { module->setExternalGatePosMode(i); }));
        }
      }));
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
      // Hidden for now, but keep the trace plumbing available in code in case
      // we need to bring back platter interaction logging for debugging.
      // menu->addChild(createCheckMenuItem("Log platter mouse events", "",
      //                                    [=]() { return module->isPlatterTraceLoggingEnabled(); },
      //                                    [=]() { module->setPlatterTraceLoggingEnabled(!module->isPlatterTraceLoggingEnabled()); }));
      if (isDragonKingDebugEnabled()) {
        menu->addChild(createMenuItem("Export signed inventory.json...", "", [=]() {
          std::string defaultDir = temporalDeckUserRootPath();
          system::createDirectories(defaultDir);
          osdialog_filters *filters = osdialog_filters_parse("JSON:json,JSON");
          char *pathC = osdialog_file(OSDIALOG_SAVE, defaultDir.c_str(), "inventory.signed.json", filters);
          osdialog_filters_free(filters);
          if (!pathC) {
            return;
          }
          std::string path = ensureJsonExtension(pathC);
          std::free(pathC);

          std::string error;
          std::string signature;
          int fileCount = 0;
          if (!writeSignedVinylManifest(path, getBuiltInVinylInventoryState(), VINYL_SOURCE_BUILTIN, &error, &signature,
                                        &fileCount)) {
            std::string message = error.empty() ? "Signed inventory export failed" : error;
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message.c_str());
            return;
          }
          osdialog_message(OSDIALOG_INFO, OSDIALOG_OK,
                           string::f("Saved signed inventory:\n%s\n\nSignature: %s\nFiles: %d",
                                     path.c_str(), signature.c_str(), fileCount)
                             .c_str());
        }));
        menu->addChild(createMenuItem("Export signed expanded.json...", "", [=]() {
          const VinylInventoryState &expandedState = getExpandedVinylInventoryState();
          if (!expandedState.valid || expandedState.entries.empty()) {
            std::string msg = expandedState.valid ? "No expanded vinyl inventory entries to export"
                                                  : (expandedState.error.empty() ? "Expanded vinyl inventory is invalid"
                                                                                 : expandedState.error);
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, msg.c_str());
            return;
          }
          std::string defaultDir = temporalDeckUserRootPath();
          system::createDirectories(defaultDir);
          osdialog_filters *filters = osdialog_filters_parse("JSON:json,JSON");
          char *pathC = osdialog_file(OSDIALOG_SAVE, defaultDir.c_str(), "inventory.expanded.signed.json", filters);
          osdialog_filters_free(filters);
          if (!pathC) {
            return;
          }
          std::string path = ensureJsonExtension(pathC);
          std::free(pathC);

          std::string error;
          std::string signature;
          int fileCount = 0;
          if (!writeSignedVinylManifest(path, expandedState, VINYL_SOURCE_EXPANDED, &error, &signature, &fileCount)) {
            std::string message = error.empty() ? "Expanded inventory signing failed" : error;
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message.c_str());
            return;
          }
          osdialog_message(OSDIALOG_INFO, OSDIALOG_OK,
                           string::f("Saved signed expanded inventory:\n%s\n\nSignature: %s\nFiles: %d",
                                     path.c_str(), signature.c_str(), fileCount)
                             .c_str());
        }));
        menu->addChild(createMenuItem("Export platter SVG...", "", [=]() {
          Vec platterCenter = mm2px(Vec(50.8f, 72.f));
          float platterRadius = mm2px(Vec(29.5f, 0.f)).x;
          loadPlatterAnchor(platterCenter, platterRadius);
          std::string defaultDir = temporalDeckUserRootPath();
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

      menu->addChild(new MenuSeparator());
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
      if (module->hasLoadedSample()) {
        menu->addChild(createSubmenuItem("Sample info", "", [=](Menu *submenu) {
          submenu->addChild(createMenuLabel(loadedSampleName));
          submenu->addChild(
            createMenuLabel(string::f("Length: %.2f s", std::max(0.0, module->getUiSampleDurationSeconds()))));
          if (module->wasLoadedSampleTruncated()) {
            submenu->addChild(createMenuLabel("Truncated to current buffer limit"));
          }
        }));
      }
    }
  }
};

Model *modelTemporalDeck = createModel<TemporalDeck, TemporalDeckWidget>("TemporalDeck");
