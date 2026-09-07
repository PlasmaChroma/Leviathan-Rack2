#include "../src/Nautiloid.hpp"
#include "../src/Iris.hpp"
#include "../src/Chromatide.hpp"
#include "../src/UiExpanderUtils.hpp"

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
    const auto publishesBeforeAudio = naut.irisExpanderPublishes.load(std::memory_order_acquire);
    for (int sample = 0; sample < 800; ++sample) {
      naut.process(args);
      irisModule.process(args);
    }
    if (naut.irisExpanderPublishes.load(std::memory_order_acquire) != publishesBeforeAudio) return false;
    naut.serviceIrisSource();
    naut.serviceIrisConsumerDemand();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  return naut.irisPreviewGeneration.load(std::memory_order_acquire) != 0u &&
    naut.irisExpanderPublishes.load(std::memory_order_acquire) != 0u &&
    naut.irisRendersCompleted.load(std::memory_order_acquire) != 0u &&
    naut.irisRequestsSubmitted.load(std::memory_order_acquire) == 1u;
}

bool chromatideDeliveryRunsOnUiAndReconnects() {
  Model canvasModel, irisModel;
  canvasModel.slug = "Chromatide";
  irisModel.slug = "Iris";
  modelChromatide = &canvasModel;
  modelIris = &irisModel;
  Chromatide canvas;
  Iris irisModule;
  canvas.model = &canvasModel;
  irisModule.model = &irisModel;
  canvas.rightExpander.module = &irisModule;
  irisModule.leftExpander.module = &canvas;
  engine::Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;
  for (int i = 0; i < 2000; ++i) canvas.process(args);
  if (canvas.lastExpanderGenerationSent != 0u) return false;
  canvas.serviceIrisSource();
  if (canvas.lastExpanderGenerationSent != canvas.irisPreviewGeneration.load()) return false;

  // A paused UI does not submit or release new canvas ownership from audio.
  canvas.publishToIris();
  const auto sent = canvas.lastExpanderGenerationSent;
  for (int i = 0; i < 2000; ++i) canvas.process(args);
  if (canvas.lastExpanderGenerationSent != sent) return false;
  canvas.serviceIrisSource();
  if (canvas.lastExpanderGenerationSent != canvas.irisPreviewGeneration.load()) return false;

  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
  while (std::chrono::steady_clock::now() < deadline && irisModule.loading.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  if (irisModule.sourceKind() != iris::SOURCE_EXPANDER_IMAGE) return false;

  // Attach the same canvas generation to a different Iris instance.
  Iris replacement;
  replacement.model = &irisModel;
  irisModule.leftExpander.module = nullptr;
  canvas.rightExpander.module = &replacement;
  replacement.leftExpander.module = &canvas;
  engine::Module::ExpanderChangeEvent event;
  event.side = 1;
  canvas.onExpanderChange(event);
  canvas.serviceIrisSource();
  const auto reconnectDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
  while (std::chrono::steady_clock::now() < reconnectDeadline && replacement.loading.load())
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  return replacement.sourceKind() == iris::SOURCE_EXPANDER_IMAGE;
}

bool uiLookupUsesIdsRatherThanEnginePointers() {
  engine::Module left, right;
  left.id = 1;
  right.id = 2;
  bool rightRegistered = true;
  // Rack's Engine constructor is private API. Substitute just its getModule
  // boundary while testing the same ID/reciprocity logic used in production.
  auto lookup = [&](int64_t id) -> engine::Module* {
    if (id == left.id) return &left;
    if (id == right.id && rightRegistered) return &right;
    return nullptr;
  };
  left.rightExpander.moduleId = right.id;
  right.leftExpander.moduleId = left.id;
  // Engine-side pointer updates have not run yet, as can happen during a UI move.
  bool pass = ui_expander::resolveById(&left, true, lookup) == &right &&
    ui_expander::resolveById(&right, false, lookup) == &left;
  right.leftExpander.moduleId = -1;
  pass = pass && ui_expander::resolveById(&left, true, lookup) == nullptr;
  right.leftExpander.moduleId = left.id;
  rightRegistered = false;
  pass = pass && ui_expander::resolveById(&left, true, lookup) == nullptr;
  return pass;
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

  const uint64_t compositesBeforeZoom =
    naut.displayCacheCompositePublishes.load(std::memory_order_acquire);
  Nautiloid::FractalState zoomState = naut.fractalStateSnapshot();
  zoomState.zoom = 0.2f;
  naut.setFractalState(zoomState);
  naut.requestInteractiveZoomPreview(zoomState.centerX, zoomState.centerY);
  const auto zoomDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (std::chrono::steady_clock::now() < zoomDeadline &&
         naut.displayCacheCompositePublishes.load(std::memory_order_acquire) ==
           compositesBeforeZoom) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  if (naut.displayCacheCompositePublishes.load(std::memory_order_acquire) ==
      compositesBeforeZoom) {
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
  check("UI adjacency resolves registered IDs, rejects one-way links, and survives removal",
        uiLookupUsesIdsRatherThanEnginePointers());
  check("Spider reconnect publishes without startup request starvation",
        restoresWithoutFinalRequestStarvation(iris::FRACTAL_SPIDER));
  check("Barnsley reconnect publishes without startup request starvation",
        restoresWithoutFinalRequestStarvation(iris::FRACTAL_BARNSLEY));
  check("CPU fallback serves live pan and zoom previews from immutable cache generations",
        cacheGenerationsRemainImmutableAcrossRecenter());
  check("Chromatide delivers on UI service, keeps audio free of submissions, and reconnects",
        chromatideDeliveryRunsOnUiAndReconnects());

  if (failures != 0) {
    std::cerr << failures << " Nautiloid/Iris restore checks failed\n";
    return 1;
  }
  std::cout << "Nautiloid/Iris restore checks passed\n";
  return 0;
}
