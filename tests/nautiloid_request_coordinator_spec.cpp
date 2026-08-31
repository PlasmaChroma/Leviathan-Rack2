#include "../src/NautiloidRequestCoordinator.hpp"
#include "../src/NautiloidCachePolicy.hpp"
#include "../src/NautiloidFractal.hpp"

#include <iostream>
#include <string>

namespace {

int failures = 0;

void check(const char* name, bool condition) {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << '\n';
  if (!condition) ++failures;
}

bool cancelAfterFirstRow(void* opaque) {
  int* checks = static_cast<int*>(opaque);
  return ++(*checks) > 1;
}

} // namespace

int main() {
  using nautiloid_requests::Coordinator;
  using nautiloid_requests::FallbackLifecyclePolicy;
  using nautiloid_requests::FallbackTransition;
  using nautiloid_requests::InteractionPhase;
  using nautiloid_requests::IrisConsumerDemand;

  Coordinator coordinator;
  const uint64_t display1 = coordinator.allocateDisplaySerial();
  check("no consumer rejects ordinary Iris work",
    coordinator.allocateIrisSerial(InteractionPhase::Final, false) == 0u);
  const uint64_t display2 = coordinator.allocateDisplaySerial();
  check("display serial advances without advancing Iris", display1 == 1u && display2 == 2u);

  check("retained image source is not a consuming Iris",
    !coordinator.setIrisConsumerDemand(IrisConsumerDemand::RetainImageSource) &&
    coordinator.allocateIrisSerial(InteractionPhase::Final, false) == 0u);
  const uint64_t forced = coordinator.allocateIrisSerial(InteractionPhase::Final, true);
  check("forced sync bypasses demand gating", forced == 1u);

  check("becoming a consumer requests an idle sync",
    coordinator.setIrisConsumerDemand(IrisConsumerDemand::AcceptUpdates));
  check("repeating consuming demand does not request another idle sync",
    !coordinator.setIrisConsumerDemand(IrisConsumerDemand::AcceptUpdates));

  const uint64_t preview1 = coordinator.allocateIrisSerial(InteractionPhase::Preview, false);
  check("the worker can activate the first offered preview immediately",
    preview1 == 2u && coordinator.activateIrisPreviewSerial(preview1));
  const uint64_t pendingPreview1 = coordinator.allocateIrisSerial(InteractionPhase::Preview, false);
  const uint64_t pendingPreview2 = coordinator.allocateIrisSerial(InteractionPhase::Preview, false);
  check("new preview offers do not cancel active work",
    pendingPreview1 == 3u && pendingPreview2 == 4u &&
    coordinator.isCurrentIrisSerial(preview1));
  check("the worker activates the newest coalesced preview after completion",
    coordinator.activateIrisPreviewSerial(pendingPreview2) &&
    coordinator.isCurrentIrisSerial(pendingPreview2));
  check("an overwritten pending preview cannot activate later",
    !coordinator.activateIrisPreviewSerial(pendingPreview1));

  const uint64_t finalSerial = coordinator.allocateIrisSerial(InteractionPhase::Final, false);
  check("final interaction immediately receives cancellation authority", finalSerial == 5u);
  check("superseded Iris publications are rejected",
    !coordinator.isCurrentIrisSerial(pendingPreview2) && coordinator.isCurrentIrisSerial(finalSerial));
  check("display and Iris serial spaces remain independent",
    coordinator.allocateDisplaySerial() == 3u &&
    coordinator.allocateIrisSerial(InteractionPhase::Final, false) == 6u);
  const uint64_t activeSerial = coordinator.currentIrisSerial();
  coordinator.setIrisConsumerDemand(IrisConsumerDemand::RetainImageSource);
  check("ending Iris demand cancels an in-flight ordinary request",
    !coordinator.isCurrentIrisSerial(activeSerial));

  const uint64_t activeDisplaySerial = coordinator.allocateDisplaySerial();
  coordinator.invalidateDisplayRequests();
  check("parking fallback invalidates in-flight display work",
    !coordinator.isNewestDisplaySerial(activeDisplaySerial));

  FallbackLifecyclePolicy fallback;
  check("GPU probe pending creates no fallback workers",
    !fallback.hasWorkers() && !fallback.isActive() &&
    fallback.requestParked() == FallbackTransition::None);
  check("first CPU fallback requirement starts workers lazily",
    fallback.requestActive() == FallbackTransition::Start &&
    fallback.hasWorkers() && fallback.isActive());
  check("repeated fallback requirement performs no lifecycle work",
    fallback.requestActive() == FallbackTransition::None);
  check("GPU recovery parks rather than destroys fallback workers",
    fallback.requestParked() == FallbackTransition::Park &&
    fallback.hasWorkers() && !fallback.isActive());
  check("repeated GPU frames do not repeatedly park workers",
    fallback.requestParked() == FallbackTransition::None);
  check("fallback reactivation wakes the existing workers",
    fallback.requestActive() == FallbackTransition::Wake &&
    fallback.hasWorkers() && fallback.isActive());

  nautiloid_cache::CompositePublishPolicy compositePolicy;
  check("partial cache publication waits for useful center coverage",
    !compositePolicy.shouldPublishPartial(3u, 1000));
  check("first useful partial cache generation publishes immediately",
    compositePolicy.shouldPublishPartial(4u, 1000));
  check("partial cache publications are time bounded",
    !compositePolicy.shouldPublishPartial(20u, 1059) &&
    compositePolicy.shouldPublishPartial(21u, 1060));

  int cancellationChecks = 0;
  iris::FractalCancellationToken cancellation;
  cancellation.isCancelled = cancelAfterFirstRow;
  cancellation.context = &cancellationChecks;
  iris::SourceField source;
  std::string error;
  const bool rendered = iris::makeBuiltinFractalSourceSized(
    iris::FRACTAL_MANDELBROT,
    0.f,
    0.0,
    0.0,
    32,
    16,
    1.f,
    &source,
    &error,
    &cancellation);
  check("offline fractal cancellation is checked between rows",
    !rendered && cancellationChecks == 2 && error == "Fractal render cancelled");

  return failures == 0 ? 0 : 1;
}
