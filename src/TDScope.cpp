#include "TemporalDeckExpanderProtocol.hpp"
#include "plugin.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <regex>
#include <sstream>
#include <vector>

namespace {

static bool loadRectFromSvgMm(const std::string &svgPath, const std::string &rectId, math::Rect *outRect) {
  if (!outRect) {
    return false;
  }

  std::ifstream svgFile(svgPath.c_str());
  if (!svgFile.good()) {
    return false;
  }

  std::ostringstream svgBuffer;
  svgBuffer << svgFile.rdbuf();
  const std::string svgText = svgBuffer.str();

  const std::regex rectRegex("<rect\\b[^>]*\\bid\\s*=\\s*\"" + rectId + "\"[^>]*>", std::regex::icase);
  std::smatch rectMatch;
  if (!std::regex_search(svgText, rectMatch, rectRegex)) {
    return false;
  }
  const std::string rectTag = rectMatch.str(0);

  auto parseAttrMm = [&](const char *attr, float *outMm) -> bool {
    if (!outMm) {
      return false;
    }
    const std::regex attrRegex(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
    std::smatch attrMatch;
    if (!std::regex_search(rectTag, attrMatch, attrRegex)) {
      return false;
    }
    // Inkscape panel coordinates are in 1/100 mm (viewBox-scale style).
    *outMm = std::stof(attrMatch.str(1)) * 0.01f;
    return true;
  };

  float xMm = 0.f;
  float yMm = 0.f;
  float wMm = 0.f;
  float hMm = 0.f;
  if (!parseAttrMm("x", &xMm) || !parseAttrMm("y", &yMm) || !parseAttrMm("width", &wMm) ||
      !parseAttrMm("height", &hMm)) {
    return false;
  }

  outRect->pos = Vec(xMm, yMm);
  outRect->size = Vec(wMm, hMm);
  return true;
}

} // namespace

struct TDScope final : Module {
  enum LightId { LINK_LIGHT, PREVIEW_LIGHT, LIGHTS_LEN };
  enum ScopeRangeMode { SCOPE_RANGE_5V = 0, SCOPE_RANGE_10V, SCOPE_RANGE_2V5, SCOPE_RANGE_AUTO, SCOPE_RANGE_COUNT };

  std::array<temporaldeck_expander::HostToDisplay, 2> leftMessages;
  temporaldeck_expander::HostToDisplay uiSnapshot;
  std::atomic<uint32_t> uiSnapshotSeq {0};
  std::atomic<bool> uiLinkActive {false};
  std::atomic<bool> uiPreviewValid {false};
  std::atomic<uint64_t> uiLastPublishSeq {0};
  float uiPublishTimerSec = 0.f;
  uint64_t lastPublishSeq = 0;
  int staleFrames = 0;
  bool previewValid = false;
  int scopeDisplayRangeMode = SCOPE_RANGE_5V;

  static constexpr float kUiPublishIntervalSec = 1.f / 60.f;

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    leftExpander.producerMessage = &leftMessages[0];
    leftExpander.consumerMessage = &leftMessages[1];
    uiSnapshot = temporaldeck_expander::HostToDisplay();
  }

  float scopeDisplayFullScaleVolts() const {
    switch (scopeDisplayRangeMode) {
      case SCOPE_RANGE_10V:
        return 10.f;
      case SCOPE_RANGE_2V5:
        return 2.5f;
      case SCOPE_RANGE_AUTO:
        return 5.f;
      case SCOPE_RANGE_5V:
      default:
        return 5.f;
    }
  }

  json_t *dataToJson() override {
    json_t *root = json_object();
    json_object_set_new(root, "scopeDisplayRangeMode", json_integer(scopeDisplayRangeMode));
    return root;
  }

  void dataFromJson(json_t *root) override {
    if (!root) {
      return;
    }
    json_t *rangeJ = json_object_get(root, "scopeDisplayRangeMode");
    if (rangeJ) {
      scopeDisplayRangeMode = clamp(int(json_integer_value(rangeJ)), SCOPE_RANGE_5V, SCOPE_RANGE_COUNT - 1);
    }
  }

  void publishSnapshotToUi(const temporaldeck_expander::HostToDisplay &msg) {
    uint32_t seq = uiSnapshotSeq.load(std::memory_order_relaxed);
    uiSnapshotSeq.store(seq + 1u, std::memory_order_release); // writer active (odd)
    uiSnapshot = msg;
    uiSnapshotSeq.store(seq + 2u, std::memory_order_release); // writer done (even)
    uiLastPublishSeq.store(msg.publishSeq, std::memory_order_release);
  }

  bool readSnapshotForUi(temporaldeck_expander::HostToDisplay *out) const {
    if (!out) {
      return false;
    }
    for (int i = 0; i < 3; ++i) {
      uint32_t seq0 = uiSnapshotSeq.load(std::memory_order_acquire);
      if ((seq0 & 1u) != 0u) {
        continue;
      }
      *out = uiSnapshot;
      uint32_t seq1 = uiSnapshotSeq.load(std::memory_order_acquire);
      if (seq0 == seq1 && (seq1 & 1u) == 0u) {
        return out->magic == temporaldeck_expander::MAGIC &&
               out->version == temporaldeck_expander::VERSION &&
               out->size == sizeof(temporaldeck_expander::HostToDisplay);
      }
    }
    return false;
  }

  void process(const ProcessArgs &args) override {
    bool validMessage = false;
    previewValid = false;
    const temporaldeck_expander::HostToDisplay *latestMsg = nullptr;
    if (leftExpander.module && leftExpander.consumerMessage) {
      const auto *msg = reinterpret_cast<const temporaldeck_expander::HostToDisplay *>(leftExpander.consumerMessage);
      if (msg->magic == temporaldeck_expander::MAGIC && msg->version == temporaldeck_expander::VERSION &&
          msg->size == sizeof(temporaldeck_expander::HostToDisplay)) {
        validMessage = true;
        latestMsg = msg;
        previewValid = (msg->flags & temporaldeck_expander::FLAG_PREVIEW_VALID) != 0u;
        if (msg->publishSeq != lastPublishSeq) {
          lastPublishSeq = msg->publishSeq;
          staleFrames = 0;
        } else {
          staleFrames++;
        }
      }
    }

    bool linkActive = validMessage && staleFrames < 2048;
    uiLinkActive.store(linkActive, std::memory_order_relaxed);
    uiPreviewValid.store(linkActive && previewValid, std::memory_order_relaxed);
    uiPublishTimerSec += args.sampleTime;
    if (linkActive && latestMsg && uiPublishTimerSec >= kUiPublishIntervalSec) {
      uiPublishTimerSec = std::fmod(uiPublishTimerSec, kUiPublishIntervalSec);
      publishSnapshotToUi(*latestMsg);
    }

    bool ready = linkActive && previewValid;
    lights[LINK_LIGHT].setBrightness(linkActive && !ready ? 1.f : 0.f);
    lights[PREVIEW_LIGHT].setBrightness(ready ? 1.f : 0.f);
  }
};

struct TDScopeDisplayWidget final : Widget {
  TDScope *module = nullptr;
  float autoDisplayFullScaleVolts = 5.f;
  bool autoDisplayScaleInitialized = false;

  void draw(const DrawArgs &args) override {
    bool linkActive = module && module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module && module->uiPreviewValid.load(std::memory_order_relaxed);

    if (!module) {
      return;
    }

    temporaldeck_expander::HostToDisplay msg;
    if (!module->readSnapshotForUi(&msg) || !linkActive || !previewValid) {
      return;
    }

    uint32_t scopeBinCount = std::min(msg.scopeBinCount, temporaldeck_expander::SCOPE_BIN_COUNT);
    if (scopeBinCount == 0u) {
      return;
    }

    const float yInset = 0.75f;
    const float drawTop = yInset;
    const float drawBottom = std::max(drawTop + 1.f, box.size.y - yInset);
    const float drawHeight = std::max(drawBottom - drawTop, 1.f);

    const float centerY = 0.5f * (drawTop + drawBottom);
    const float centerX = box.size.x * 0.5f;
    const float ampHalfWidth = box.size.x * 0.46f;
    const float yDen = std::max(drawHeight - 1.f, 1.f);
    float displayFullScaleVolts = std::max(module->scopeDisplayFullScaleVolts(), 0.001f);
    if (module->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO) {
      int peakQ = 0;
      for (uint32_t i = 0; i < scopeBinCount; ++i) {
        const temporaldeck_expander::ScopeBin &bin = msg.scope[i];
        if (!temporaldeck_expander::isScopeBinValid(bin)) {
          continue;
        }
        peakQ = std::max(peakQ, int(std::abs(int(bin.min))));
        peakQ = std::max(peakQ, int(std::abs(int(bin.max))));
      }
      float peakVolts = (float(peakQ) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
      float targetFullScaleVolts = clamp(peakVolts * 1.08f, 0.25f, temporaldeck_expander::kPreviewQuantizeVolts);
      if (!autoDisplayScaleInitialized) {
        autoDisplayFullScaleVolts = targetFullScaleVolts;
        autoDisplayScaleInitialized = true;
      } else {
        // Smooth autoscale transitions: expand quickly to avoid clipping spikes,
        // contract slowly to prevent visible snapping/pumping.
        float delta = targetFullScaleVolts - autoDisplayFullScaleVolts;
        if (std::fabs(delta) > 1e-4f) {
          constexpr float kAutoScaleAttackAlpha = 0.16f;
          constexpr float kAutoScaleReleaseAlpha = 0.08f;
          float alpha = delta > 0.f ? kAutoScaleAttackAlpha : kAutoScaleReleaseAlpha;
          autoDisplayFullScaleVolts += delta * alpha;
        }
      }
      displayFullScaleVolts = autoDisplayFullScaleVolts;
    } else {
      autoDisplayScaleInitialized = false;
    }
    float scopeNormGain = temporaldeck_expander::kPreviewQuantizeVolts / displayFullScaleVolts;
    float halfWindowSamples = std::max(0.f, msg.scopeHalfWindowMs * 0.001f * std::max(msg.sampleRate, 1.f));
    float windowTopLag = msg.lagSamples + halfWindowSamples;
    float windowBottomLag = msg.lagSamples - halfWindowSamples;
    float scopeBinSpanSamples = std::max(msg.scopeBinSpanSamples, 1e-6f);
    const int rowCount = std::max(1, int(std::ceil(drawHeight)));
    std::vector<float> rowX0(size_t(rowCount), centerX);
    std::vector<float> rowX1(size_t(rowCount), centerX);
    std::vector<float> rowIntensity(size_t(rowCount), 0.f);
    std::vector<uint8_t> rowValid(size_t(rowCount), 0u);

    auto decodeScopeBin = [&](const temporaldeck_expander::ScopeBin &bin, float *minNorm, float *maxNorm) {
      *minNorm = clamp((float(bin.min) / 32767.f) * scopeNormGain, -1.f, 1.f);
      *maxNorm = clamp((float(bin.max) / 32767.f) * scopeNormGain, -1.f, 1.f);
    };

    auto sampleEnvelopeAtT = [&](float t, float *minNormOut, float *maxNormOut) -> bool {
      float lag = windowTopLag + (windowBottomLag - windowTopLag) * clamp(t, 0.f, 1.f);
      float binPos = (msg.scopeStartLagSamples - lag) / scopeBinSpanSamples;
      if (binPos < 0.f || binPos > float(scopeBinCount - 1u)) {
        return false;
      }
      uint32_t binIndex0 = std::min(uint32_t(std::floor(binPos)), scopeBinCount - 1u);
      uint32_t binIndex1 = std::min(binIndex0 + 1u, scopeBinCount - 1u);
      float binFrac = clamp(binPos - float(binIndex0), 0.f, 1.f);

      const temporaldeck_expander::ScopeBin &bin0 = msg.scope[binIndex0];
      const temporaldeck_expander::ScopeBin &bin1 = msg.scope[binIndex1];
      bool valid0 = temporaldeck_expander::isScopeBinValid(bin0);
      bool valid1 = temporaldeck_expander::isScopeBinValid(bin1);
      if (!valid0 && !valid1) {
        return false;
      }

      float minNorm = 0.f;
      float maxNorm = 0.f;
      if (valid0 && valid1) {
        float minNorm0 = 0.f;
        float maxNorm0 = 0.f;
        float minNorm1 = 0.f;
        float maxNorm1 = 0.f;
        decodeScopeBin(bin0, &minNorm0, &maxNorm0);
        decodeScopeBin(bin1, &minNorm1, &maxNorm1);
        if (binFrac <= 0.001f) {
          minNorm = minNorm0;
          maxNorm = maxNorm0;
        } else if (binFrac >= 0.999f) {
          minNorm = minNorm1;
          maxNorm = maxNorm1;
        } else {
          minNorm = std::min(minNorm0, minNorm1);
          maxNorm = std::max(maxNorm0, maxNorm1);
        }
      } else if (valid0) {
        decodeScopeBin(bin0, &minNorm, &maxNorm);
      } else {
        decodeScopeBin(bin1, &minNorm, &maxNorm);
      }
      *minNormOut = minNorm;
      *maxNormOut = maxNorm;
      return true;
    };

    for (int iy = 0; iy < rowCount; ++iy) {
      float y = drawTop + float(iy) + 0.5f;
      float t0 = clamp((y - drawTop - 0.5f) / yDen, 0.f, 1.f);
      float t1 = clamp((y - drawTop + 0.5f) / yDen, 0.f, 1.f);
      float rowMinNorm = 0.f;
      float rowMaxNorm = 0.f;
      bool rowHasData = false;
      constexpr int kRowSupersampleTaps = 3;
      for (int tap = 0; tap < kRowSupersampleTaps; ++tap) {
        float frac = (float(tap) + 0.5f) / float(kRowSupersampleTaps);
        float t = t0 + (t1 - t0) * frac;
        float sampleMinNorm = 0.f;
        float sampleMaxNorm = 0.f;
        if (!sampleEnvelopeAtT(t, &sampleMinNorm, &sampleMaxNorm)) {
          continue;
        }
        if (!rowHasData) {
          rowMinNorm = sampleMinNorm;
          rowMaxNorm = sampleMaxNorm;
          rowHasData = true;
        } else {
          rowMinNorm = std::min(rowMinNorm, sampleMinNorm);
          rowMaxNorm = std::max(rowMaxNorm, sampleMaxNorm);
        }
      }
      if (!rowHasData) {
        continue;
      }

      float x0 = centerX + rowMinNorm * ampHalfWidth;
      float x1 = centerX + rowMaxNorm * ampHalfWidth;
      if (x1 < x0) {
        std::swap(x0, x1);
      }
      rowX0[size_t(iy)] = x0;
      rowX1[size_t(iy)] = x1;
      float peakness = clamp(std::max(std::fabs(rowMinNorm), std::fabs(rowMaxNorm)), 0.f, 1.f);
      float density = clamp(0.5f * (rowMaxNorm - rowMinNorm), 0.f, 1.f);
      rowIntensity[size_t(iy)] = clamp(0.65f * peakness + 0.35f * density, 0.f, 1.f);
      rowValid[size_t(iy)] = 1u;
    }

    auto gradientColorForIntensity = [](float intensity, uint8_t alpha) -> NVGcolor {
      intensity = clamp(intensity, 0.f, 1.f);
      // Inverted mapping: low intensity -> purple, high intensity -> cyan.
      constexpr float lowR = 122.f; // #7a5cff (TD.Scope title gradient stop)
      constexpr float lowG = 92.f;
      constexpr float lowB = 255.f;
      constexpr float highR = 28.f; // #1cccd9 (TD.Scope title gradient stop)
      constexpr float highG = 204.f;
      constexpr float highB = 217.f;
      uint8_t r = uint8_t(std::lround(lowR + (highR - lowR) * intensity));
      uint8_t g = uint8_t(std::lround(lowG + (highG - lowG) * intensity));
      uint8_t b = uint8_t(std::lround(lowB + (highB - lowB) * intensity));
      return nvgRGBA(r, g, b, alpha);
    };

    nvgSave(args.vg);
    nvgScissor(args.vg, 0.f, drawTop, box.size.x, drawBottom - drawTop);
    bool prevDrawn = false;
    float prevX0 = centerX;
    float prevX1 = centerX;
    float prevY = drawTop + 0.5f;
    float prevIntensity = 0.f;
    for (int iy = 0; iy < rowCount; ++iy) {
      if (!rowValid[size_t(iy)]) {
        continue;
      }

      float y = drawTop + float(iy) + 0.5f;
      float x0 = rowX0[size_t(iy)];
      float x1 = rowX1[size_t(iy)];
      float intensity = rowIntensity[size_t(iy)];
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, x0, y);
      nvgLineTo(args.vg, x1, y);
      nvgStrokeColor(args.vg, gradientColorForIntensity(intensity, 210));
      nvgStrokeWidth(args.vg, 1.f);
      nvgStroke(args.vg);

      if (prevDrawn) {
        float connectIntensity = 0.5f * (prevIntensity + intensity);
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, prevX0, prevY);
        nvgLineTo(args.vg, x0, y);
        nvgMoveTo(args.vg, prevX1, prevY);
        nvgLineTo(args.vg, x1, y);
        nvgStrokeColor(args.vg, gradientColorForIntensity(connectIntensity, 150));
        nvgStrokeWidth(args.vg, 0.75f);
        nvgStroke(args.vg);
      }
      prevDrawn = true;
      prevX0 = x0;
      prevX1 = x1;
      prevY = y;
      prevIntensity = intensity;
    }

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, 2.f, centerY);
    nvgLineTo(args.vg, box.size.x - 2.f, centerY);
    nvgStrokeColor(args.vg, nvgRGBA(87, 64, 191, 128));
    nvgStrokeWidth(args.vg, 1.9f);
    nvgStroke(args.vg);
    nvgResetScissor(args.vg);
    nvgRestore(args.vg);
  }
};

struct TDScopeSeamBlendWidget final : Widget {
  TDScope *module = nullptr;

  void draw(const DrawArgs &args) override {
    if (!module || !module->leftExpander.module || module->leftExpander.module->model != modelTemporalDeck) {
      return;
    }

    const float seamW = 1.25f;
    const float featherW = 1.75f;
    NVGcolor seamColor = nvgRGBA(11, 15, 20, 235);

    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, seamW, box.size.y);
    nvgFillColor(args.vg, seamColor);
    nvgFill(args.vg);

    NVGpaint feather = nvgLinearGradient(args.vg, seamW, 0.f, seamW + featherW, 0.f, seamColor, nvgRGBA(11, 15, 20, 0));
    nvgBeginPath(args.vg);
    nvgRect(args.vg, seamW, 0.f, featherW, box.size.y);
    nvgFillPaint(args.vg, feather);
    nvgFill(args.vg);
  }
};

struct TDScopeWidget : ModuleWidget {
  TDScopeWidget(TDScope *module) {
    setModule(module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/tdscope.svg");
    setPanel(createPanel(panelPath));

    auto *seamBlend = new TDScopeSeamBlendWidget;
    seamBlend->module = module;
    seamBlend->box.pos = Vec(0.f, 0.f);
    seamBlend->box.size = box.size;
    addChild(seamBlend);

    auto *display = new TDScopeDisplayWidget;
    display->module = module;
    math::Rect scopeRectMm;
    if (loadRectFromSvgMm(panelPath, "scope", &scopeRectMm)) {
      display->box.pos = mm2px(scopeRectMm.pos);
      display->box.size = mm2px(scopeRectMm.size);
    } else {
      display->box.pos = mm2px(Vec(1.1138f, 10.9404f));
      display->box.size = mm2px(Vec(38.5563f, 109.4206f));
    }
    addChild(display);

    addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::PREVIEW_LIGHT));
  }

  void appendContextMenu(Menu *menu) override {
    ModuleWidget::appendContextMenu(menu);
    TDScope *scopeModule = dynamic_cast<TDScope *>(module);
    if (!scopeModule) {
      return;
    }

    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Scope Range"));
    menu->addChild(createCheckMenuItem(
      "Auto (window peak)", "",
      [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_AUTO; }));
    menu->addChild(createCheckMenuItem(
      "+/-2.5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_2V5; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_2V5; }));
    menu->addChild(createCheckMenuItem(
      "+/-5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_5V; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_5V; }));
    menu->addChild(createCheckMenuItem(
      "+/-10V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_10V; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_10V; }));
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
