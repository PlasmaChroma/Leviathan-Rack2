#include "../src/NautiloidRequestCoordinator.hpp"
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
  using nautiloid_requests::InteractionPhase;
  using nautiloid_requests::IrisConsumerDemand;

  Coordinator coordinator;
  const uint64_t display1 = coordinator.allocateDisplaySerial();
  check("no consumer rejects ordinary Iris work",
    coordinator.allocateIrisSerial(InteractionPhase::Final, false, 1000) == 0u);
  const uint64_t display2 = coordinator.allocateDisplaySerial();
  check("display serial advances without advancing Iris", display1 == 1u && display2 == 2u);

  check("retained image source is not a consuming Iris",
    !coordinator.setIrisConsumerDemand(IrisConsumerDemand::RetainImageSource) &&
    coordinator.allocateIrisSerial(InteractionPhase::Final, false, 1000) == 0u);
  const uint64_t forced = coordinator.allocateIrisSerial(InteractionPhase::Final, true, 1000);
  check("forced sync bypasses demand gating", forced == 1u);

  check("becoming a consumer requests an idle sync",
    coordinator.setIrisConsumerDemand(IrisConsumerDemand::AcceptUpdates));
  check("repeating consuming demand does not request another idle sync",
    !coordinator.setIrisConsumerDemand(IrisConsumerDemand::AcceptUpdates));

  const uint64_t preview1 = coordinator.allocateIrisSerial(InteractionPhase::Preview, false, 2000);
  const uint64_t suppressedPreview = coordinator.allocateIrisSerial(InteractionPhase::Preview, false, 2100);
  const uint64_t preview2 = coordinator.allocateIrisSerial(InteractionPhase::Preview, false, 2140);
  check("interactive Iris previews use the centralized cadence",
    preview1 == 2u && suppressedPreview == 0u && preview2 == 3u);

  const uint64_t finalSerial = coordinator.allocateIrisSerial(InteractionPhase::Final, false, 2140);
  check("final interaction always receives an authoritative serial", finalSerial == 4u);
  check("superseded Iris publications are rejected",
    !coordinator.isNewestIrisSerial(preview2) && coordinator.isNewestIrisSerial(finalSerial));
  check("display and Iris serial spaces remain independent",
    coordinator.allocateDisplaySerial() == 3u &&
    coordinator.allocateIrisSerial(InteractionPhase::Final, false, 2140) == 5u);
  const uint64_t activeSerial = coordinator.newestIrisSerial();
  coordinator.setIrisConsumerDemand(IrisConsumerDemand::RetainImageSource);
  check("ending Iris demand cancels an in-flight ordinary request",
    !coordinator.isNewestIrisSerial(activeSerial));

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
