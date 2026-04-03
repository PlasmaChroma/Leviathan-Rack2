#pragma once

#include "TemporalDeckSamplePrep.hpp"

#include <atomic>
#include <condition_variable>
#include <cstdint>
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

  void requestAsyncSampleBuild(const AsyncSampleBuildRequest &request);
  bool sampleBuildInProgress() const;
  bool decodedSampleAvailable() const;

  bool consumePendingPreparedSample(temporaldeck::PreparedSampleData *outPrepared);
  bool consumeAllocationFallbackPending();

  void clearDecodedAndPreparedState();
  void setPendingSampleStateApply();
  bool consumePendingSampleStateApply();

  bool sampleAutoPlayOnLoad() const;
  void setSampleAutoPlayOnLoad(bool enabled);
  std::string samplePath() const;
  std::string sampleDisplayName() const;
  void sampleJsonSnapshot(bool *autoPlayOut, std::string *pathOut) const;
  void setSampleSavedPath(const std::string &path);

private:
  void workerLoop();

  mutable std::mutex sampleStateMutex_;
  bool sampleAutoPlayOnLoad_ = true;
  std::string samplePath_;
  std::string sampleDisplayName_;
  temporaldeck::DecodedSampleFile decodedSample_;
  std::atomic<bool> decodedSampleAvailable_{false};

  mutable std::mutex preparedSampleMutex_;
  temporaldeck::PreparedSampleData preparedSample_;
  std::atomic<bool> pendingPreparedSampleInstall_{false};

  std::thread sampleBuildThread_;
  mutable std::mutex sampleBuildMutex_;
  std::condition_variable sampleBuildCv_;
  bool sampleBuildStop_ = false;
  bool sampleBuildHasRequest_ = false;
  AsyncSampleBuildRequest sampleBuildRequest_;
  std::atomic<bool> sampleBuildInProgress_{false};
  std::atomic<uint64_t> sampleBuildRequestSerial_{0};
  std::atomic<bool> allocationFallbackPending_{false};

  std::atomic<bool> pendingSampleStateApply_{false};
};

} // namespace temporaldeck_lifecycle
