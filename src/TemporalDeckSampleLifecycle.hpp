#pragma once

#include "TemporalDeckSamplePrep.hpp"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstddef>
#include <mutex>
#include <string>
#include <thread>

namespace temporaldeck_lifecycle {

struct TemporalDeckSampleLifecycle {
  struct AsyncSampleBuildRequest {
    enum Type {
      NONE = 0,
      LOAD_PATH = 1,
      REBUILD_FROM_DECODED = 2,
      BUILD_EMPTY_BUFFER = 3,
    };
    int type = NONE;
    std::string path;
    float targetSampleRate = 44100.f;
    int requestedBufferMode = temporaldeck::TemporalDeckEngine::BUFFER_DURATION_10S;
  };

  TemporalDeckSampleLifecycle() = default;
  ~TemporalDeckSampleLifecycle();

  void startWorker();
  void stopWorker();

  uint64_t requestAsyncSampleBuild(const AsyncSampleBuildRequest &request);
  uint64_t requestAsyncRuntimeBuild(int type, float targetSampleRate, int requestedBufferMode);
  void requestClearDecodedAndPreparedStateFromAudio();
  bool sampleBuildInProgress() const;
  bool decodedSampleAvailable() const;

  temporaldeck::PreparedSampleData *consumePendingPreparedSample();
  void retirePreparedSampleFromAudio(temporaldeck::PreparedSampleData *prepared);
  bool consumeAllocationFallbackPending();

  void clearDecodedAndPreparedState();
  void setPendingSampleStateApply();
  bool consumePendingSampleStateApply();

  std::string samplePath() const;
  std::string sampleDisplayName() const;
  void sampleJsonSnapshot(std::string *pathOut) const;
  void setSampleSavedPath(const std::string &path);
  void sampleMemorySnapshot(size_t *decodedBytesOut, size_t *preparedBytesOut) const;

private:
  void workerLoop();

  mutable std::mutex sampleStateMutex_;
  std::string samplePath_;
  std::string sampleDisplayName_;
  temporaldeck::DecodedSampleFile decodedSample_;
  std::atomic<bool> decodedSampleAvailable_{false};

  std::atomic<temporaldeck::PreparedSampleData *> pendingPreparedSample_{nullptr};
  std::atomic<temporaldeck::PreparedSampleData *> retiredPreparedSample_{nullptr};
  std::atomic<size_t> pendingPreparedBytes_{0u};

  std::thread sampleBuildThread_;
  mutable std::mutex sampleBuildMutex_;
  std::condition_variable sampleBuildCv_;
  bool sampleBuildStop_ = false;
  bool sampleBuildHasRequest_ = false;
  AsyncSampleBuildRequest sampleBuildRequest_;
  std::atomic<bool> sampleBuildInProgress_{false};
  std::atomic<uint64_t> sampleBuildRequestSerial_{0};
  std::atomic<bool> allocationFallbackPending_{false};
  std::atomic<bool> runtimeBuildPending_{false};
  std::atomic<int> runtimeBuildType_{AsyncSampleBuildRequest::NONE};
  std::atomic<float> runtimeBuildSampleRate_{44100.f};
  std::atomic<int> runtimeBuildBufferMode_{temporaldeck::TemporalDeckEngine::BUFFER_DURATION_10S};
  std::atomic<bool> clearStateRequested_{false};

  std::atomic<bool> pendingSampleStateApply_{false};
};

} // namespace temporaldeck_lifecycle
