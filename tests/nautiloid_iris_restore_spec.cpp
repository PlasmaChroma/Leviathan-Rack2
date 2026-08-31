#include "../src/Nautiloid.hpp"
#include "../src/Iris.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#define QOI_IMPLEMENTATION
#include "../src/third_party/qoi.h"

Model* modelNautiloid = nullptr;
Model* modelIris = nullptr;
Model* modelIntegralFlux = nullptr;
Model* modelChromatide = nullptr;

bool isDragonKingDebugEnabled() {
  return false;
}

namespace {

int failures = 0;

void check(const std::string& name, bool condition) {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
  if (!condition) ++failures;
}

bool restoresWithoutFinalRequestStarvation(int mode) {
  Model nautModel;
  nautModel.slug = "Nautiloid";
  Model irisModel;
  irisModel.slug = "Iris";
  modelNautiloid = &nautModel;
  modelIris = &irisModel;

  Nautiloid naut;
  Iris irisModule;
  naut.model = &nautModel;
  irisModule.model = &irisModel;
  naut.rightExpander.module = &irisModule;
  irisModule.leftExpander.module = &naut;

  json_t* nautState = json_object();
  json_object_set_new(nautState, "fractalMode", json_integer(mode));
  json_object_set_new(nautState, "fractalZoom", json_real(3.0));
  naut.dataFromJson(nautState);
  json_decref(nautState);

  json_t* irisState = json_object();
  json_object_set_new(
    irisState, "sourceKind", json_integer(iris::SOURCE_NAUTILOID_FRACTAL));
  json_object_set_new(irisState, "sourceMode", json_string("nautiloid"));
  json_object_set_new(irisState, "sourceModeNautiloidAttached", json_true());
  json_object_set_new(irisState, "nautiloidFractalMode", json_integer(mode));
  json_object_set_new(irisState, "nautiloidFractalZoom", json_real(3.0));
  json_object_set_new(irisState, "nautiloidFractalGeneration", json_integer(7));
  irisModule.dataFromJson(irisState);
  json_decref(irisState);

  engine::Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
  while (std::chrono::steady_clock::now() < deadline &&
         naut.irisExpanderPublishes.load(std::memory_order_acquire) == 0u) {
    // Approximate one 60 Hz UI service interval. Before the regression fix,
    // every interval submitted a new authoritative request and cancelled the
    // slower Spider/Barnsley render before it could publish generation one.
    for (int sample = 0; sample < 800; ++sample) {
      naut.process(args);
      irisModule.process(args);
    }
    naut.serviceIrisConsumerDemand();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  return naut.irisPreviewGeneration.load(std::memory_order_acquire) != 0u &&
    naut.irisExpanderPublishes.load(std::memory_order_acquire) != 0u &&
    naut.irisRendersCompleted.load(std::memory_order_acquire) != 0u &&
    naut.irisRequestsSubmitted.load(std::memory_order_acquire) == 1u;
}

} // namespace

int main() {
  check("Spider reconnect publishes without startup request starvation",
        restoresWithoutFinalRequestStarvation(iris::FRACTAL_SPIDER));
  check("Barnsley reconnect publishes without startup request starvation",
        restoresWithoutFinalRequestStarvation(iris::FRACTAL_BARNSLEY));

  if (failures != 0) {
    std::cerr << failures << " Nautiloid/Iris restore checks failed\n";
    return 1;
  }
  std::cout << "Nautiloid/Iris restore checks passed\n";
  return 0;
}
