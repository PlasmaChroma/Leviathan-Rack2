#include "TemporalDeckExpanderProtocol.hpp"
#include "plugin.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>

struct TDScope final : Module {
  enum LightId { LINK_LIGHT, PREVIEW_LIGHT, LIGHTS_LEN };

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

  static constexpr float kUiPublishIntervalSec = 1.f / 60.f;

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    leftExpander.producerMessage = &leftMessages[0];
    leftExpander.consumerMessage = &leftMessages[1];
    uiSnapshot = temporaldeck_expander::HostToDisplay();
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

    lights[LINK_LIGHT].setBrightness(linkActive ? 1.f : 0.f);
    lights[PREVIEW_LIGHT].setBrightness(linkActive && previewValid ? 1.f : 0.f);
  }
};

struct TDScopeDisplayWidget final : Widget {
  TDScope *module = nullptr;
  uint64_t seenSeq = 0;
  float linkPulse = 0.f;
  static constexpr float kWindowMs = 900.f;

  bool previewBinForLag(const temporaldeck_expander::HostToDisplay &msg, float lagSamples,
                        temporaldeck_expander::PreviewBin *outBin) const {
    if (!outBin || msg.previewFilledBins == 0 || msg.samplesPerBin == 0) {
      return false;
    }
    if (lagSamples < 0.f) {
      return false;
    }
    int lagBins = int(std::lround(lagSamples / std::max(1u, msg.samplesPerBin)));
    if (lagBins < 0 || lagBins >= int(msg.previewFilledBins)) {
      return false;
    }
    int newestBin = int(msg.previewWriteIndex + temporaldeck_expander::PREVIEW_BIN_COUNT - 1u) %
                    int(temporaldeck_expander::PREVIEW_BIN_COUNT);
    int idx = newestBin - lagBins;
    while (idx < 0) {
      idx += int(temporaldeck_expander::PREVIEW_BIN_COUNT);
    }
    idx %= int(temporaldeck_expander::PREVIEW_BIN_COUNT);
    *outBin = msg.preview[idx];
    return true;
  }

  void drawLinkStatus(const DrawArgs &args, bool hasHostModule, bool linkActive, bool previewValid) {
    nvgFontSize(args.vg, 12.f);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    NVGcolor statusColor = nvgRGBA(220, 80, 80, 220);
    const char *statusText = hasHostModule ? "STALE" : "NO HOST";
    if (linkActive && previewValid) {
      statusColor = nvgRGBA(92, 224, 132, 235);
      statusText = "LINK OK";
    } else if (linkActive) {
      statusColor = nvgRGBA(230, 186, 86, 235);
      statusText = "LINK / NO DATA";
    }

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 4.f, 4.f, box.size.x - 8.f, 16.f, 4.f);
    nvgFillColor(args.vg, nvgRGBA(12, 18, 26, 190));
    nvgFill(args.vg);

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 4.f, 4.f, box.size.x - 8.f, 16.f, 4.f);
    nvgStrokeColor(args.vg, statusColor);
    nvgStrokeWidth(args.vg, 1.1f);
    nvgStroke(args.vg);

    nvgFillColor(args.vg, statusColor);
    nvgText(args.vg, 8.f, 12.f, statusText, nullptr);

    float dotAlpha = clamp(linkPulse, 0.f, 1.f);
    nvgBeginPath(args.vg);
    nvgCircle(args.vg, box.size.x - 12.f, 12.f, 3.2f);
    nvgFillColor(args.vg, nvgRGBAf(statusColor.r, statusColor.g, statusColor.b, 0.28f + 0.72f * dotAlpha));
    nvgFill(args.vg);
  }

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

    bool hasHostModule = module && module->leftExpander.module;
    bool linkActive = module && module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module && module->uiPreviewValid.load(std::memory_order_relaxed);

    if (module) {
      uint64_t seq = module->uiLastPublishSeq.load(std::memory_order_relaxed);
      if (seq != seenSeq) {
        seenSeq = seq;
        linkPulse = 1.f;
      } else {
        linkPulse *= 0.93f;
      }
    }

    drawLinkStatus(args, hasHostModule, linkActive, previewValid);

    if (!module) {
      return;
    }

    temporaldeck_expander::HostToDisplay msg;
    if (!module->readSnapshotForUi(&msg) || !linkActive || !previewValid) {
      return;
    }

    const float centerY = box.size.y * 0.5f;
    const float centerX = box.size.x * 0.5f;
    const float ampHalfWidth = box.size.x * 0.46f;
    const float halfHeight = box.size.y * 0.5f;
    const bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    const float lagMs = (msg.lagSamples / std::max(msg.sampleRate, 1.f)) * 1000.f;
    const float forwardWindowMs = sampleMode ? kWindowMs : std::min(kWindowMs, std::max(0.f, lagMs));
    const float samplesPerMs = std::max(msg.sampleRate, 1.f) * 0.001f;

    for (int iy = 0; iy < int(std::ceil(box.size.y)); ++iy) {
      float y = float(iy) + 0.5f;
      float rel = (y - centerY) / std::max(halfHeight, 1.f); // top=-1, bottom=+1
      float offsetMs = rel * kWindowMs;
      if (offsetMs > forwardWindowMs) {
        continue;
      }
      float targetLagSamples = msg.lagSamples - offsetMs * samplesPerMs;
      if (targetLagSamples < 0.f || targetLagSamples > msg.accessibleLagSamples) {
        continue;
      }

      temporaldeck_expander::PreviewBin bin;
      if (!previewBinForLag(msg, targetLagSamples, &bin)) {
        continue;
      }

      float minNorm = float(bin.min) / 32767.f;
      float maxNorm = float(bin.max) / 32767.f;
      float x0 = centerX + minNorm * ampHalfWidth;
      float x1 = centerX + maxNorm * ampHalfWidth;
      if (x1 < x0) {
        std::swap(x0, x1);
      }

      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, x0, y);
      nvgLineTo(args.vg, x1, y);
      nvgStrokeColor(args.vg, nvgRGBA(114, 216, 255, 210));
      nvgStrokeWidth(args.vg, 1.f);
      nvgStroke(args.vg);
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
    setPanel(createPanel(asset::plugin(pluginInstance, "res/tdscope.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    auto *display = new TDScopeDisplayWidget;
    display->module = module;
    display->box.pos = mm2px(Vec(8.5f, 23.25f));
    display->box.size = mm2px(Vec(43.96f, 82.0f));
    addChild(display);

    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(9.5f, 58.0f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(9.5f, 70.0f)), module, TDScope::PREVIEW_LIGHT));
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
