#include "../src/LongPlayStreamEngine.hpp"
#include "../src/TemporalDeckEngine.hpp"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

namespace {

struct Result {
  std::string name;
  bool passed = false;
  std::string detail;
};

struct StreamStub {
  bool targetResident = false;
  std::uint64_t desired = 0u;

  static bool read(void *context, std::uint64_t frame, float *left, float *right) {
    StreamStub *stub = static_cast<StreamStub *>(context);
    if (frame >= 700u && !stub->targetResident) return false;
    if (left) *left = float(frame) * 0.001f;
    if (right) *right = -float(frame) * 0.001f;
    return true;
  }

  static void desire(void *context, std::uint64_t frame, bool) {
    static_cast<StreamStub *>(context)->desired = frame;
  }

  static bool resident(void *context, std::uint64_t frame) {
    StreamStub *stub = static_cast<StreamStub *>(context);
    return frame < 700u || stub->targetResident;
  }
};

void writeLe16(std::ofstream &output, unsigned value) {
  const unsigned char bytes[] = {static_cast<unsigned char>(value),
                                 static_cast<unsigned char>(value >> 8)};
  output.write(reinterpret_cast<const char *>(bytes), sizeof(bytes));
}

void writeLe32(std::ofstream &output, unsigned value) {
  const unsigned char bytes[] = {
      static_cast<unsigned char>(value), static_cast<unsigned char>(value >> 8),
      static_cast<unsigned char>(value >> 16),
      static_cast<unsigned char>(value >> 24)};
  output.write(reinterpret_cast<const char *>(bytes), sizeof(bytes));
}

bool writeSparseWav(const std::string &path, unsigned sampleRate,
                    unsigned seconds, bool includeTestImpulse) {
  const unsigned frames = sampleRate * seconds;
  const unsigned dataBytes = frames * 2u;
  std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
  if (!output)
    return false;
  output.write("RIFF", 4);
  writeLe32(output, 36u + dataBytes);
  output.write("WAVEfmt ", 8);
  writeLe32(output, 16u);
  writeLe16(output, 1u);
  writeLe16(output, 1u);
  writeLe32(output, sampleRate);
  writeLe32(output, sampleRate * 2u);
  writeLe16(output, 2u);
  writeLe16(output, 16u);
  output.write("data", 4);
  writeLe32(output, dataBytes);
  if (includeTestImpulse) {
    writeLe16(output, 16384u); // 0.5 sample
  }
  if (dataBytes > (includeTestImpulse ? 2u : 0u)) {
    output.seekp(std::streamoff(44u + dataBytes - 1u));
    output.put('\0');
  }
  return bool(output);
}

bool writeStereoPeakWav(const std::string &path) {
  const unsigned sampleRate = 8000u;
  const unsigned frames = sampleRate;
  const unsigned dataBytes = frames * 4u;
  std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
  if (!output) return false;
  output.write("RIFF", 4);
  writeLe32(output, 36u + dataBytes);
  output.write("WAVEfmt ", 8);
  writeLe32(output, 16u);
  writeLe16(output, 1u);
  writeLe16(output, 2u);
  writeLe32(output, sampleRate);
  writeLe32(output, sampleRate * 4u);
  writeLe16(output, 4u);
  writeLe16(output, 16u);
  output.write("data", 4);
  writeLe32(output, dataBytes);
  writeLe16(output, 24576u); // +0.75 left
  writeLe16(output, 40960u); // -0.75 right
  output.seekp(std::streamoff(44u + dataBytes - 1u));
  output.put('\0');
  return bool(output);
}

bool waitForReady(temporaldeck::LongPlayStreamEngine &engine,
                  int timeoutMs = 10000) {
  const auto deadline = std::chrono::steady_clock::now() +
                         std::chrono::milliseconds(timeoutMs);
  while (std::chrono::steady_clock::now() < deadline) {
    if (engine.ready())
      return true;
    if (!engine.loading() && !engine.error().empty())
      return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return false;
}


bool waitForFrame(temporaldeck::LongPlayStreamEngine &engine,
                  std::uint64_t frame, float *left, float *right,
                  int timeoutMs = 10000) {
  engine.setDesiredFrame(frame, false);
  const auto deadline = std::chrono::steady_clock::now() +
                         std::chrono::milliseconds(timeoutMs);
  while (std::chrono::steady_clock::now() < deadline) {
    if (engine.readFrame(frame, left, right))
      return true;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
  return false;
}

Result testWavStreamingAndSymmetricBlocks() {
  const std::string path = "/tmp/leviathan_tdlongplay_test.wav";
  const bool wrote = writeSparseWav(path, 8000u, 300u, true);
  temporaldeck::LongPlayStreamEngine engine;
  engine.requestLoad(path);
  const bool ready = waitForReady(engine);

  float left = 0.f, right = 0.f;
  const bool read0 = ready && waitForFrame(engine, 0u, &left, &right);
  const bool validImpulse = std::fabs(left - 0.5f) < 1e-3f;

  const std::uint64_t center = 18u * temporaldeck::LongPlayStreamEngine::kBlockFrames;
  engine.setDesiredFrame(center, false);
  const std::uint64_t behind = 2u * temporaldeck::LongPlayStreamEngine::kBlockFrames;
  const std::uint64_t ahead = 33u * temporaldeck::LongPlayStreamEngine::kBlockFrames;
  const auto windowDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
  while (std::chrono::steady_clock::now() < windowDeadline &&
         (!engine.isFrameResident(behind) || !engine.isFrameResident(ahead))) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
  const bool symmetric = engine.isFrameResident(behind) && engine.isFrameResident(ahead);
  const bool boundedMemory = engine.allocatedAudioBytes() <= 32u * 1024u * 1024u;

  std::remove(path.c_str());
  return {"LongPlayStreamEngine streams WAV and populates 50/50 RAM blocks",
          wrote && ready && read0 && validImpulse && symmetric && boundedMemory,
          "wrote=" + std::to_string(wrote) + " ready=" + std::to_string(ready) +
              " read0=" + std::to_string(read0) +
              " sample=" + std::to_string(left) +
              " symmetric=" + std::to_string(symmetric) +
              " bytes=" + std::to_string(engine.allocatedAudioBytes())};
}

Result testEngineDefersColdSeekUntilResident() {
  temporaldeck::TemporalDeckEngine engine;
  engine.buffer.reset(48000.f, 1.f, false);
  engine.sampleRate = 48000.f;
  StreamStub stub;
  engine.installStreamedSample(&stub, &StreamStub::read, &StreamStub::desire,
                               &StreamStub::resident, 1000, 5.f);
  engine.samplePlayhead = 100.0;
  engine.readHead = 100.0;
  engine.requestSampleSeekTarget(800.0);
  const bool deferred = engine.streamedSeekPending && engine.readHead == 100.0 &&
                        stub.desired == 800u;
  stub.targetResident = true;
  engine.servicePendingStreamSeek();
  const bool completed = !engine.streamedSeekPending && engine.readHead == 800.0 &&
                         engine.streamedSeekCrossfadeRemaining > 0;
  return {"Temporal Deck defers cold seeks and arms a 5 ms handoff",
          deferred && completed,
          "deferred=" + std::to_string(deferred) +
              " completed=" + std::to_string(completed)};
}

Result testHourWavSeek() {
  const std::string path = "/tmp/leviathan_tdlongplay_hour.wav";
  const unsigned sampleRate = 8000u;
  const unsigned seconds = 3600u; // 1 Hour
  const bool wrote = writeSparseWav(path, sampleRate, seconds, false);
  temporaldeck::LongPlayStreamEngine engine;
  engine.requestLoad(path);
  const bool ready = waitForReady(engine);


  const std::uint64_t target = std::uint64_t(sampleRate) * 55u * 60u;
  float left = 1.f, right = 1.f;
  const bool seekRead = ready && waitForFrame(engine, target, &left, &right);


  const double duration = engine.sampleRate() > 0u
                              ? double(engine.totalFrames()) / engine.sampleRate()
                              : 0.0;

  std::remove(path.c_str());
  return {"One-Hour WAV seeks through 32-block hot RAM window",
          wrote && ready && seekRead && std::fabs(duration - 3600.0) < 1e-5,
          "duration=" + std::to_string(duration) +
              " seekRead=" + std::to_string(seekRead)};
}

} // namespace

int main() {
  const Result results[] = {testWavStreamingAndSymmetricBlocks(),
                            testEngineDefersColdSeekUntilResident(),
                            testHourWavSeek()};
  int failures = 0;
  for (const Result &result : results) {
    std::cout << (result.passed ? "[PASS] " : "[FAIL] ") << result.name
              << " :: " << result.detail << '\n';
    if (!result.passed)
      ++failures;
  }
  std::cout << "[SUMMARY] temporaldeck_longplay_spec: "
            << (int(sizeof(results) / sizeof(results[0])) - failures) << "/"
            << int(sizeof(results) / sizeof(results[0])) << " passed\n";
  return failures == 0 ? 0 : 1;
}
