#include "TemporalDeckSampleLifecycle.hpp"

#include "codec.hpp"
#include "plugin.hpp"

#include <chrono>
#include <new>
#include <utility>

namespace temporaldeck_lifecycle {

using temporaldeck::buildPreparedSample;
using temporaldeck::chooseSampleBufferMode;
using temporaldeck::decodeSampleFile;
using temporaldeck::DecodedSampleFile;
using temporaldeck::PreparedSampleData;

namespace {

double lifecycleElapsedMs(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() * 1e-3;
}

} // namespace

TemporalDeckSampleLifecycle::~TemporalDeckSampleLifecycle() {
  stopWorker();
  delete pendingPreparedSample_.exchange(nullptr, std::memory_order_acq_rel);
  delete retiredPreparedSample_.exchange(nullptr, std::memory_order_acq_rel);
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

uint64_t TemporalDeckSampleLifecycle::requestAsyncSampleBuild(const AsyncSampleBuildRequest &request) {
  uint64_t requestSerial = 0u;
  {
    std::lock_guard<std::mutex> lock(sampleBuildMutex_);
    sampleBuildRequest_ = request;
    sampleBuildHasRequest_ = true;
    requestSerial = sampleBuildRequestSerial_.fetch_add(1, std::memory_order_relaxed) + 1u;
    sampleBuildInProgress_.store(true, std::memory_order_relaxed);
  }
  sampleBuildCv_.notify_one();
  return requestSerial;
}

uint64_t TemporalDeckSampleLifecycle::requestAsyncRuntimeBuild(
  int type, float targetSampleRate, int requestedBufferMode) {
  runtimeBuildType_.store(type, std::memory_order_relaxed);
  runtimeBuildSampleRate_.store(targetSampleRate, std::memory_order_relaxed);
  runtimeBuildBufferMode_.store(requestedBufferMode, std::memory_order_relaxed);
  const uint64_t serial = sampleBuildRequestSerial_.fetch_add(1, std::memory_order_acq_rel) + 1u;
  sampleBuildInProgress_.store(true, std::memory_order_release);
  runtimeBuildPending_.store(true, std::memory_order_release);
  sampleBuildCv_.notify_one();
  return serial;
}

void TemporalDeckSampleLifecycle::requestClearDecodedAndPreparedStateFromAudio() {
  clearStateRequested_.store(true, std::memory_order_release);
  sampleBuildCv_.notify_one();
}

bool TemporalDeckSampleLifecycle::sampleBuildInProgress() const {
  return sampleBuildInProgress_.load(std::memory_order_relaxed) ||
         pendingPreparedSample_.load(std::memory_order_acquire) != nullptr;
}

bool TemporalDeckSampleLifecycle::decodedSampleAvailable() const {
  return decodedSampleAvailable_.load(std::memory_order_relaxed);
}

PreparedSampleData *TemporalDeckSampleLifecycle::consumePendingPreparedSample() {
  // Do not consume another install until the worker has reclaimed the prior
  // engine buffers. This bounds ownership to one retired allocation set and
  // keeps the audio path free of delete/free operations.
  if (retiredPreparedSample_.load(std::memory_order_acquire) != nullptr) {
    return nullptr;
  }
  PreparedSampleData *prepared = pendingPreparedSample_.exchange(nullptr, std::memory_order_acq_rel);
  if (prepared) {
    pendingPreparedBytes_.store(0u, std::memory_order_release);
  }
  return prepared;
}

void TemporalDeckSampleLifecycle::retirePreparedSampleFromAudio(PreparedSampleData *prepared) {
  if (!prepared) {
    return;
  }
  PreparedSampleData *expected = nullptr;
  if (!retiredPreparedSample_.compare_exchange_strong(
        expected, prepared, std::memory_order_release, std::memory_order_relaxed)) {
    // consumePendingPreparedSample() prevents this state. Keep ownership in
    // the pending slot as a fail-safe rather than deleting on the audio thread.
    pendingPreparedSample_.store(prepared, std::memory_order_release);
    return;
  }
  sampleBuildCv_.notify_one();
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
  delete pendingPreparedSample_.exchange(nullptr, std::memory_order_acq_rel);
  pendingPreparedBytes_.store(0u, std::memory_order_release);
}

void TemporalDeckSampleLifecycle::setPendingSampleStateApply() {
  pendingSampleStateApply_.store(true, std::memory_order_relaxed);
}

bool TemporalDeckSampleLifecycle::consumePendingSampleStateApply() {
  return pendingSampleStateApply_.exchange(false, std::memory_order_relaxed);
}

std::string TemporalDeckSampleLifecycle::samplePath() const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  return samplePath_;
}

std::string TemporalDeckSampleLifecycle::sampleDisplayName() const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  return sampleDisplayName_;
}

void TemporalDeckSampleLifecycle::sampleJsonSnapshot(std::string *pathOut) const {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  if (pathOut) {
    *pathOut = samplePath_;
  }
}

void TemporalDeckSampleLifecycle::setSampleSavedPath(const std::string &path) {
  std::lock_guard<std::mutex> lock(sampleStateMutex_);
  samplePath_ = path;
  sampleDisplayName_ = path.empty() ? std::string() : system::getFilename(path);
}

void TemporalDeckSampleLifecycle::sampleMemorySnapshot(size_t *decodedBytesOut, size_t *preparedBytesOut) const {
  if (decodedBytesOut) {
    std::lock_guard<std::mutex> lock(sampleStateMutex_);
    *decodedBytesOut = (decodedSample_.left.capacity() + decodedSample_.right.capacity()) * sizeof(float);
  }
  if (preparedBytesOut) {
    *preparedBytesOut = pendingPreparedBytes_.load(std::memory_order_acquire);
  }
}

void TemporalDeckSampleLifecycle::workerLoop() {
  while (true) {
    if (PreparedSampleData *retired = retiredPreparedSample_.exchange(nullptr, std::memory_order_acq_rel)) {
      delete retired;
    }
    if (clearStateRequested_.exchange(false, std::memory_order_acq_rel)) {
      clearDecodedAndPreparedState();
    }
    AsyncSampleBuildRequest request;
    uint64_t requestSerial = 0;
    {
      std::unique_lock<std::mutex> lock(sampleBuildMutex_);
      sampleBuildCv_.wait(lock, [this]() {
        return sampleBuildStop_ || sampleBuildHasRequest_ || runtimeBuildPending_.load(std::memory_order_acquire) ||
               clearStateRequested_.load(std::memory_order_acquire) ||
               retiredPreparedSample_.load(std::memory_order_acquire) != nullptr;
      });
      if (sampleBuildStop_) {
        break;
      }
      if (sampleBuildHasRequest_) {
        request = sampleBuildRequest_;
        sampleBuildHasRequest_ = false;
        requestSerial = sampleBuildRequestSerial_.load(std::memory_order_relaxed);
      } else if (runtimeBuildPending_.exchange(false, std::memory_order_acq_rel)) {
        request.type = runtimeBuildType_.load(std::memory_order_relaxed);
        request.targetSampleRate = runtimeBuildSampleRate_.load(std::memory_order_relaxed);
        request.requestedBufferMode = runtimeBuildBufferMode_.load(std::memory_order_relaxed);
        requestSerial = sampleBuildRequestSerial_.load(std::memory_order_relaxed);
      } else {
        continue;
      }
    }

    DecodedSampleFile decoded;
    bool validDecoded = false;
    const auto buildStart = std::chrono::steady_clock::now();
    double decodeMs = 0.0;

    if (request.type == AsyncSampleBuildRequest::BUILD_EMPTY_BUFFER) {
      validDecoded = true;
    } else if (request.type == AsyncSampleBuildRequest::LOAD_PATH) {
      std::string decodeError;
      bool decodeOk = false;
      try {
        const auto decodeStart = std::chrono::steady_clock::now();
        decodeOk = decodeSampleFile(request.path, &decoded, &decodeError);
        decodeMs = lifecycleElapsedMs(decodeStart, std::chrono::steady_clock::now());
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
        decodedSampleAvailable_.store(decodedSample_.frames > 0 && !decodedSample_.left.empty(), std::memory_order_relaxed);
      }
      validDecoded = decoded.frames > 0 && !decoded.left.empty();
    } else if (request.type == AsyncSampleBuildRequest::REBUILD_FROM_DECODED) {
      const auto decodeStart = std::chrono::steady_clock::now();
      std::lock_guard<std::mutex> lock(sampleStateMutex_);
      decoded = decodedSample_;
      decodeMs = lifecycleElapsedMs(decodeStart, std::chrono::steady_clock::now());
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
      const auto prepStart = std::chrono::steady_clock::now();
      const bool built = request.type == AsyncSampleBuildRequest::BUILD_EMPTY_BUFFER
        ? buildPreparedEmptyBuffer(request.targetSampleRate, targetMode, &prepared)
        : buildPreparedSample(decoded, request.targetSampleRate, targetMode, &prepared);
      if (built) {
        prepared.buildSerial = requestSerial;
        prepared.buildRequestType = request.type;
        prepared.sourceFrames = decoded.frames;
        prepared.sourceChannels = decoded.channels;
        prepared.workerDecodeMs = decodeMs;
        prepared.workerPrepMs = lifecycleElapsedMs(prepStart, std::chrono::steady_clock::now());
        prepared.workerTotalMs = lifecycleElapsedMs(buildStart, std::chrono::steady_clock::now());
        if (requestSerial == sampleBuildRequestSerial_.load(std::memory_order_relaxed)) {
          PreparedSampleData *published = new PreparedSampleData(std::move(prepared));
          pendingPreparedBytes_.store(
            (published->left.capacity() + published->right.capacity()) * sizeof(float),
            std::memory_order_release);
          PreparedSampleData *superseded = pendingPreparedSample_.exchange(published, std::memory_order_acq_rel);
          delete superseded;
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
