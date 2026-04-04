#include "TemporalDeckExpanderProtocol.hpp"
#include "plugin.hpp"

#include <array>
#include <cstdint>

struct TDScope final : Module {
  enum LightId { LINK_LIGHT, PREVIEW_LIGHT, LIGHTS_LEN };

  std::array<temporaldeck_expander::HostToDisplay, 2> leftMessages;
  uint64_t lastPublishSeq = 0;
  int staleFrames = 0;
  bool previewValid = false;

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    leftExpander.producerMessage = &leftMessages[0];
    leftExpander.consumerMessage = &leftMessages[1];
  }

  void process(const ProcessArgs &args) override {
    (void)args;
    bool validMessage = false;
    previewValid = false;
    if (leftExpander.module && leftExpander.consumerMessage) {
      const auto *msg = reinterpret_cast<const temporaldeck_expander::HostToDisplay *>(leftExpander.consumerMessage);
      if (msg->magic == temporaldeck_expander::MAGIC && msg->version == temporaldeck_expander::VERSION &&
          msg->size == sizeof(temporaldeck_expander::HostToDisplay)) {
        validMessage = true;
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
    lights[LINK_LIGHT].setBrightness(linkActive ? 1.f : 0.f);
    lights[PREVIEW_LIGHT].setBrightness(linkActive && previewValid ? 1.f : 0.f);
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

    addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(9.5f, 58.0f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(9.5f, 70.0f)), module, TDScope::PREVIEW_LIGHT));
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");

