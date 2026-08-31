#include "../src/Nautiloid.hpp"
#include "../src/Iris.hpp"

#include <chrono>
#include <cmath>
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

bool cacheGenerationsRemainImmutableAcrossRecenter() {
  Model nautModel;
  nautModel.slug = "Nautiloid";
  modelNautiloid = &nautModel;

  Nautiloid naut;
  naut.model = &nautModel;
  naut.setGpuPreviewAvailable(false, true);

  Nautiloid::DisplayCacheGenerationPtr first;
  const auto firstDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(4);
  while (std::chrono::steady_clock::now() < firstDeadline) {
    first = naut.displayCacheGenerationSnapshot();
    if (first && first->validTileCount() >= 4u) break;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  if (!first || first->validTileCount() < 4u) return false;
  const size_t firstCount = first->validTileCount();

  Nautiloid::DisplayCacheGenerationPtr later;
  const auto laterDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(4);
  while (std::chrono::steady_clock::now() < laterDeadline) {
    later = naut.displayCacheGenerationSnapshot();
    if (later && later != first && later->validTileCount() > firstCount) break;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  if (!later || later == first || later->validTileCount() <= firstCount ||
      first->validTileCount() != firstCount) {
    return false;
  }
  const auto compositeDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < compositeDeadline &&
         naut.displayCacheCompositePublishes.load(std::memory_order_acquire) == 0u) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  if (naut.displayCacheCompositePublishes.load(std::memory_order_acquire) == 0u) {
    return false;
  }

  const uint64_t compositesBeforePan =
    naut.displayCacheCompositePublishes.load(std::memory_order_acquire);
  Nautiloid::FractalState panState = naut.fractalStateSnapshot();
  panState.centerX = 0.1;
  naut.setFractalState(panState);
  naut.requestInteractiveZoomPreview(panState.centerX, panState.centerY);
  const auto panDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < panDeadline &&
         naut.displayCacheCompositePublishes.load(std::memory_order_acquire) ==
           compositesBeforePan) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  if (naut.displayCacheCompositePublishes.load(std::memory_order_acquire) ==
      compositesBeforePan) {
    return false;
  }

  Nautiloid::FractalState state = naut.fractalStateSnapshot();
  state.centerX = 0.25;
  naut.setFractalState(state);
  naut.requestRenderWithCenteredCache();

  Nautiloid::DisplayCacheGenerationPtr retained;
  Nautiloid::DisplayCacheGenerationPtr recentered;
  const auto recenterDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(4);
  while (std::chrono::steady_clock::now() < recenterDeadline) {
    retained = naut.displayCacheGenerationSnapshot(true);
    recentered = naut.displayCacheGenerationSnapshot();
    if (retained && retained->validTileCount() > 0u && recentered &&
        std::fabs(recentered->centerX - state.centerX) <= 1e-9) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return retained && retained->validTileCount() > 0u && recentered &&
    retained != recentered && first->validTileCount() == firstCount;
}

} // namespace

int main() {
  check("Spider reconnect publishes without startup request starvation",
        restoresWithoutFinalRequestStarvation(iris::FRACTAL_SPIDER));
  check("Barnsley reconnect publishes without startup request starvation",
        restoresWithoutFinalRequestStarvation(iris::FRACTAL_BARNSLEY));
  check("CPU fallback retains immutable generations and serves live pans from cache",
        cacheGenerationsRemainImmutableAcrossRecenter());

  if (failures != 0) {
    std::cerr << failures << " Nautiloid/Iris restore checks failed\n";
    return 1;
  }
  std::cout << "Nautiloid/Iris restore checks passed\n";
  return 0;
}
