#include "DebugTerminalTransport.hpp"
#include <cassert>
#include <thread>

int main() {
  using namespace debug_terminal;
  UiTimingRangeAccumulator ui;
  assert(std::isnan(ui.consume().average));
  ui.add(1); ui.add(1); ui.add(10);
  auto r = ui.consume();
  assert(r.min == 1 && r.max == 10 && r.average == 4);
  assert(std::isnan(ui.consume().average));
  ui.add(0);
  assert(ui.consume().average == 0);
  UiCycleTimingAccumulator layers;
  layers.add(100); // Disabled collectors ignore work.
  layers.beginCycle(true);
  layers.add(2); // Shadow.
  assert(std::isnan(layers.consume().average)); // Mid-frame report must not flush.
  layers.add(5); // Light.
  layers.add(3); // Additional pass.
  layers.beginCycle(true); // First completed cycle is 10, not three samples.
  layers.add(4); // A cycle with only one call.
  layers.beginCycle(true);
  layers.beginCycle(true); // No calls: offscreen, no manufactured zero sample.
  r = layers.consume();
  assert(r.min == 4 && r.max == 10 && r.average == 7);
  assert(std::isnan(layers.consume().average));
  layers.add(0);
  layers.beginCycle(true);
  assert(layers.consume().average == 0); // A measured zero is still a sample.
  layers.add(99);
  layers.beginCycle(false); // Drop partial data when disabling debug.
  layers.add(200);
  layers.beginCycle(true);
  assert(std::isnan(layers.consume().average));
  layers.add(6);
  layers.beginCycle(true);
  assert(layers.consume().average == 6);
  // Main widget + detached editor/avatar costs must be paired by cycle.
  UiCycleTimingAccumulator combined;
  combined.beginCycle(true);
  combined.add(10); combined.add(2);
  combined.beginCycle(true);
  combined.add(2); combined.add(10);
  combined.beginCycle(true);
  combined.add(12); // Docked again: editor already included in the parent call.
  combined.beginCycle(true);
  r = combined.consume();
  assert(r.min == 12 && r.max == 12 && r.average == 12);
  AtomicTimingAverage average;
  std::atomic<uint64_t> lo {UINT64_MAX}, hi {0};
  recordAudioProcessTiming(lo, hi, 1000, &average);
  recordAudioProcessTiming(lo, hi, 1000, &average);
  recordAudioProcessTiming(lo, hi, 10000, &average);
  r = consumeAudioProcessTiming(lo, hi, &average);
  assert(r.min == 1 && r.max == 10 && r.average == 4);
  assert(std::isnan(average.consume()));
  std::atomic<bool> done {false};
  std::thread producer([&] {
    for (int i = 0; i < 100000; ++i) average.add(1250);
    done.store(true);
  });
  while (!done.load()) {
    const float mean = average.consume();
    assert(std::isnan(mean) || mean == 1.25f);
  }
  producer.join();
  const float finalMean = average.consume();
  assert(std::isnan(finalMean) || finalMean == 1.25f);
  average.add(5000);
  assert(average.consume() == 5);
}
