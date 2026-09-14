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
