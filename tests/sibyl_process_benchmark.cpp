// Compile the identical harness against retained and current Sibyl sources.
// Measurements include the process call and timer overhead, excluding UI/audio devices.
#define SIBYL_MODULE_TEST
#include "Sibyl.cpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <new>
Plugin *pluginInstance = nullptr;
namespace {
bool track = false;
size_t allocations = 0, deallocations = 0;
} // namespace
void *operator new(size_t n) {
  if (track)
    ++allocations;
  if (void *p = std::malloc(n))
    return p;
  throw std::bad_alloc();
}
void *operator new[](size_t n) { return ::operator new(n); }
void operator delete(void *p) noexcept {
  if (track && p)
    ++deallocations;
  std::free(p);
}
void operator delete[](void *p) noexcept { ::operator delete(p); }
void operator delete(void *p, size_t) noexcept { ::operator delete(p); }
void operator delete[](void *p, size_t) noexcept { ::operator delete(p); }
int main(int argc, char **argv) {
  if (argc < 2)
    return 2;
  std::ifstream file(argv[1]);
  std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  auto parsed = sibyl::parseCompositionJson(text, 1);
  if (!parsed.valid) {
    for (const auto &e : parsed.errors)
      std::cerr << e.path << ": " << e.message << "\n";
    return 3;
  }
  const int rate = argc > 2 ? std::atoi(argv[2]) : 48000;
  SibylModule module;
  module.acceptComposition(parsed.composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
  rack::engine::Module::ProcessArgs args{};
  args.sampleRate = float(rate);
  args.sampleTime = 1.f / rate;
  for (int i = 0; i < rate; ++i) {
    args.frame = i;
    module.process(args);
  }
  using Clock = std::chrono::steady_clock;
  std::vector<int64_t> times(size_t(rate) * 8);
  track = true;
  for (size_t i = 0; i < times.size(); ++i) {
    args.frame = rate + i;
    auto begin = Clock::now();
    module.process(args);
    times[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - begin).count();
  }
  track = false;
  std::sort(times.begin(), times.end());
  auto percentile = [&](double q) { return times[size_t(q * (times.size() - 1))]; };
  std::cout << "{\"rate\":" << rate << ",\"samples\":" << times.size()
            << ",\"tracks\":" << parsed.composition->tracks.size() << ",\"median_ns\":" << percentile(.5)
            << ",\"p95_ns\":" << percentile(.95) << ",\"p99_ns\":" << percentile(.99)
            << ",\"worst_ns\":" << times.back() << ",\"allocations\":" << allocations
            << ",\"deallocations\":" << deallocations << "}\n";
  return allocations || deallocations ? 4 : 0;
}
