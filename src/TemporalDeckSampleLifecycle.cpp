#include "TemporalDeckSampleLifecycle.hpp"

#include "codec.hpp"
#include "plugin.hpp"

#include <new>
#include <utility>

namespace temporaldeck_lifecycle {

using temporaldeck::buildPreparedSample;
using temporaldeck::chooseSampleBufferMode;
using temporaldeck::decodeSampleFile;
using temporaldeck::DecodedSampleFile;
using temporaldeck::PreparedSampleData;

TemporalDeckSampleLifecycle::~TemporalDeckSampleLifecycle() {
  stopWorker();
}

void TemporalDeckSampleLifecycle::startWorker() {
  if (sampleBuildThread_.joinable()) {
    return;
  }
  sampleBuildStop_ = false;
  sampleBuildThread_ = std::thread([this]() { workerLoop(); });
}

void TemporalDeckSampleLifecycle::stopWorker() {
  {
    std::lock_guard<std::mutex> lock(sampleBuildMutex_);
    sampleBuildStop_ = true;
    sampleBuildHasRequest_ = false;
  }
  sampleBuildCv_.notify_all();
  if (sampleBuildThread_.joinable()) {
    sampleBuildThread_.join();
  }
}

void TemporalDeckSampleLifecycle::requestAsyncSampleBuild(const AsyncSampleBuildRequest &request) {
  {
    std::lock_guard<std::mutex> lock(sampleBuildMutex_);
    sampleBuildRequest_ = request;
    sampleBuildHasRequest_ = true;
    sampleBuildRequestSerial_.fetch_add(1, std::memory_order_relaxed);
    sampleBuildInProgress_.store(true, std::memory_order_relaxed);
  }
  sampleBuildCv_.notify_one();
}

bool TemporalDeckSampleLifecycle::sampleBuildInProgress() const {
  return sampleBuildInProgress_.load(std::memory_order_relaxed);
}

bool TemporalDeckSampleLifecycle::decodedSampleAvailable() const {
  return decodedSampleAvailable_.load(std::memory_order_relaxed);
}

bool TemporalDeckSampleLifecycle::consumePendingPreparedSample(PreparedSampleData *outPrepared) {
  if (!outPrepared || !pendingPreparedSampleInstall_.exchange(false, std::memory_order_relaxed)) {
    return false;
  }
  std::lock_guard<std::mutex> lock(preparedSampleMutex_);
  if (!preparedSample_.valid) {
    return false;
  }
  *outPrepared = std::move(preparedSample_);
  preparedSample_ = PreparedSampleData();
  return true;
}

bool TemporalDeckSampleLifecycle::consumeAllocationFallbackPending() {
  return allocationFallbackPending_.exchange(false, std::memory_order_relaxed);
}

void TemporalDeckSampleLifecycle::clearDecodedAndPreparedState() {
  {
    std::lock_guard<std::mutex> lock(sampleStateMutex_);
    samplePath_.clear();
    sampleDisplayName_.clear();
    decodedSample_ = DecodedSampleFile();
  }
  decodedSampleAvailable_.store(false, std::memory_order_relaxed);
  pendingPreparedSampleInstall_.store(false, std::memory_order_relaxed);
  {
    std::lock_guard<std::mutex> lock(preparedSampleMutex_);
    preparedSample_ = PreparedSampleData();
  }
}

void TemporalDeckSampleLifecycle::setPendingSampleStateApply() {
  pendingSampleStateApply_.store(true, std::memory_order_relaxed);
}

bool TemporalDeckSampleLifecycle::consumePendingSampleStateApply() {
  return pendingSampleStateApply_.exchange(false, std::memory_order_relaxed);
}

bool TemporalDeckSampleLifecycle::sampleAutoPlayOnLoad() const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  return sampleAutoPlayOnLoad_;
}

void TemporalDeckSampleLifecycle::setSampleAutoPlayOnLoad(bool enabled) {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  sampleAutoPlayOnLoad_ = enabled;
}

std::string TemporalDeckSampleLifecycle::samplePath() const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  return samplePath_;
}

std::string TemporalDeckSampleLifecycle::sampleDisplayName() const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  return sampleDisplayName_;
}

void TemporalDeckSampleLifecycle::sampleJsonSnapshot(bool *autoPlayOut, std::string *pathOut) const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  if (autoPlayOut) {
    *autoPlayOut = sampleAutoPlayOnLoad_;
  }
  if (pathOut) {
    *pathOut = samplePath_;
  }
}

void TemporalDeckSampleLifecycle::workerLoop() {
  while (true) {
    AsyncSampleBuildRequest request;
    uint64_t requestSerial = 0;
    {
      std::unique_lock<std::mutex> lock(sampleBuildMutex_);
      sampleBuildCv_.wait(lock, [this]() { return sampleBuildStop_ || sampleBuildHasRequest_; });
      if (sampleBuildStop_) {
        break;
      }
      request = sampleBuildRequest_;
      sampleBuildHasRequest_ = false;
      requestSerial = sampleBuildRequestSerial_.load(std::memory_order_relaxed);
    }

    DecodedSampleFile decoded;
    bool autoPlayOnLoad = true;
    bool validDecoded = false;

    if (request.type == AsyncSampleBuildRequest::LOAD_PATH) {
      std::string decodeError;
      bool decodeOk = false;
      try {
        decodeOk = decodeSampleFile(request.path, &decoded, &decodeError);
      } catch (const std::bad_alloc &) {
        WARN("TemporalDeck: sample decode allocation failed, falling back to 10s live mode");
        allocationFallbackPending_.store(true, std::memory_order_relaxed);
        pendingSampleStateApply_.store(true, std::memory_order_relaxed);
        sampleBuildInProgress_.store(false, std::memory_order_relaxed);
        continue;
      }
      if (!decodeOk) {
        WARN("TemporalDeck: sample decode failed for '%s': %s", request.path.c_str(), decodeError.c_str());
        sampleBuildInProgress_.store(false, std::memory_order_relaxed);
        continue;
      }
      {
        std::lock_guard<std::mutex> lock(sampleStateMutex_);
        samplePath_ = request.path;
        sampleDisplayName_ = system::getFilename(request.path);
        decodedSample_ = decoded;
        autoPlayOnLoad = sampleAutoPlayOnLoad_;
        decodedSampleAvailable_.store(decodedSample_.frames > 0 && !decodedSample_.left.empty(), std::memory_order_relaxed);
      }
      validDecoded = decoded.frames > 0 && !decoded.left.empty();
    } else if (request.type == AsyncSampleBuildRequest::REBUILD_FROM_DECODED) {
      std::lock_guard<std::mutex> lock(sampleStateMutex_);
      decoded = decodedSample_;
      autoPlayOnLoad = sampleAutoPlayOnLoad_;
      validDecoded = decoded.frames > 0 && !decoded.left.empty();
    }

    if (!validDecoded) {
      sampleBuildInProgress_.store(false, std::memory_order_relaxed);
      continue;
    }

    int targetMode = request.requestedBufferMode;
    if (request.type == AsyncSampleBuildRequest::LOAD_PATH) {
      targetMode = chooseSampleBufferMode(decoded);
    }

    PreparedSampleData prepared;
    try {
      if (buildPreparedSample(decoded, request.targetSampleRate, targetMode, autoPlayOnLoad, &prepared)) {
        if (requestSerial == sampleBuildRequestSerial_.load(std::memory_order_relaxed)) {
          std::lock_guard<std::mutex> lock(preparedSampleMutex_);
          preparedSample_ = std::move(prepared);
          pendingPreparedSampleInstall_.store(true, std::memory_order_relaxed);
        }
      }
    } catch (const std::bad_alloc &) {
      WARN("TemporalDeck: sample prep allocation failed, falling back to 10s live mode");
      allocationFallbackPending_.store(true, std::memory_order_relaxed);
      pendingSampleStateApply_.store(true, std::memory_order_relaxed);
    }
    sampleBuildInProgress_.store(false, std::memory_order_relaxed);
  }
}

} // namespace temporaldeck_lifecycle
