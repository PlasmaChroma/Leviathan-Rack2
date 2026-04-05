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
  enum ScopeRangeMode { SCOPE_RANGE_5V = 0, SCOPE_RANGE_10V, SCOPE_RANGE_COUNT };

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

  float scopeDisplayFullScaleVolts() const { return scopeDisplayRangeMode == SCOPE_RANGE_10V ? 10.f : 5.f; }

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

  void draw(const DrawArgs &args) override {
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 2.5f);
    nvgFillColor(args.vg, nvgRGBA(8, 14, 22, 210));
    nvgFill(args.vg);

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 2.5f);
    nvgStrokeColor(args.vg, nvgRGBA(66, 87, 108, 210));
    nvgStrokeWidth(args.vg, 1.f);
    nvgStroke(args.vg);

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

    const float centerY = box.size.y * 0.5f;
    const float centerX = box.size.x * 0.5f;
    const float ampHalfWidth = box.size.x * 0.46f;
    const float yDen = std::max(box.size.y - 1.f, 1.f);
    float displayFullScaleVolts = std::max(module->scopeDisplayFullScaleVolts(), 0.001f);
    float scopeNormGain = temporaldeck_expander::kPreviewQuantizeVolts / displayFullScaleVolts;
    const int rowCount = std::max(1, int(std::ceil(box.size.y)));
    std::vector<float> rowX0(size_t(rowCount), centerX);
    std::vector<float> rowX1(size_t(rowCount), centerX);
    std::vector<uint8_t> rowValid(size_t(rowCount), 0u);

    auto decodeScopeBin = [&](const temporaldeck_expander::ScopeBin &bin, float *minNorm, float *maxNorm) {
      *minNorm = clamp((float(bin.min) / 32767.f) * scopeNormGain, -1.f, 1.f);
      *maxNorm = clamp((float(bin.max) / 32767.f) * scopeNormGain, -1.f, 1.f);
    };

    for (int iy = 0; iy < rowCount; ++iy) {
      float y = float(iy) + 0.5f;
      float t = clamp(y / yDen, 0.f, 1.f);
      float binPos = t * float(scopeBinCount - 1u);
      uint32_t binIndex0 = uint32_t(std::floor(binPos));
      binIndex0 = std::min(binIndex0, scopeBinCount - 1u);
      uint32_t binIndex1 = std::min(binIndex0 + 1u, scopeBinCount - 1u);
      float binFrac = clamp(binPos - float(binIndex0), 0.f, 1.f);

      const temporaldeck_expander::ScopeBin &bin0 = msg.scope[binIndex0];
      const temporaldeck_expander::ScopeBin &bin1 = msg.scope[binIndex1];
      bool valid0 = temporaldeck_expander::isScopeBinValid(bin0);
      bool valid1 = temporaldeck_expander::isScopeBinValid(bin1);
      if (!valid0 && !valid1) {
        continue;
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
        minNorm = minNorm0 + (minNorm1 - minNorm0) * binFrac;
        maxNorm = maxNorm0 + (maxNorm1 - maxNorm0) * binFrac;
      } else if (valid0) {
        decodeScopeBin(bin0, &minNorm, &maxNorm);
      } else {
        decodeScopeBin(bin1, &minNorm, &maxNorm);
      }

      float x0 = centerX + minNorm * ampHalfWidth;
      float x1 = centerX + maxNorm * ampHalfWidth;
      if (x1 < x0) {
        std::swap(x0, x1);
      }
      rowX0[size_t(iy)] = x0;
      rowX1[size_t(iy)] = x1;
      rowValid[size_t(iy)] = 1u;
    }

    NVGcolor waveColor = nvgRGBA(114, 216, 255, 210);
    NVGcolor connectColor = nvgRGBA(114, 216, 255, 150);
    for (int iy = 0; iy < rowCount; ++iy) {
      if (!rowValid[size_t(iy)]) {
        continue;
      }
      float y = float(iy) + 0.5f;
      float x0 = rowX0[size_t(iy)];
      float x1 = rowX1[size_t(iy)];
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, x0, y);
      nvgLineTo(args.vg, x1, y);
      nvgStrokeColor(args.vg, waveColor);
      nvgStrokeWidth(args.vg, 1.f);
      nvgStroke(args.vg);

      int prev = iy - 1;
      if (prev >= 0 && rowValid[size_t(prev)]) {
        float prevY = float(prev) + 0.5f;
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, rowX0[size_t(prev)], prevY);
        nvgLineTo(args.vg, x0, y);
        nvgMoveTo(args.vg, rowX1[size_t(prev)], prevY);
        nvgLineTo(args.vg, x1, y);
        nvgStrokeColor(args.vg, connectColor);
        nvgStrokeWidth(args.vg, 0.75f);
        nvgStroke(args.vg);
      }
    }

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, 2.f, centerY);
    nvgLineTo(args.vg, box.size.x - 2.f, centerY);
    nvgStrokeColor(args.vg, nvgRGBA(208, 84, 255, 245));
    nvgStrokeWidth(args.vg, 1.9f);
    nvgStroke(args.vg);
  }
};

struct TDScopeWidget : ModuleWidget {
  TDScopeWidget(TDScope *module) {
    setModule(module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/tdscope.svg");
    setPanel(createPanel(panelPath));

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
      "+/-5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_5V; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_5V; }));
    menu->addChild(createCheckMenuItem(
      "+/-10V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_10V; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_10V; }));
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
