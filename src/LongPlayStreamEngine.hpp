#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace temporaldeck {

struct LongPlayFileInfo {
  std::uint64_t totalFrames = 0u;
  std::uint32_t sampleRate = 0u;
  std::uint32_t channels = 0u;
};

bool probeLongPlayFile(const std::string &path, LongPlayFileInfo *info,
                       std::string *error = nullptr);

class LongPlayStreamEngine {
public:
  static constexpr std::uint32_t kBlockFrames = 65536u;
  static constexpr int kBlockCount = 32;

  LongPlayStreamEngine();
  ~LongPlayStreamEngine();

  void requestLoad(const std::string &path);
  void clear();
  void setDesiredFrame(std::uint64_t frame, bool loop);
  void setDesiredWindow(std::uint64_t frame, bool loop,
                        std::uint64_t loopStartFrame,
                        std::uint64_t loopEndFrameExclusive);
  bool readFrame(std::uint64_t frame, float *left, float *right) const;
  bool readStereoInterleaved(std::uint64_t startFrame, std::uint32_t count, float *out) const;
  bool isFrameResident(std::uint64_t frame) const;

  bool ready() const;
  bool loading() const;
  std::uint64_t totalFrames() const;
  std::uint32_t sampleRate() const;
  std::uint32_t channels() const;
  std::uint64_t generation() const;
  std::uint64_t residencyGeneration() const;
  float absolutePeak() const;
  std::string path() const;
  std::string displayName() const;
  std::string error() const;

  std::size_t allocatedAudioBytes() const;

private:
  struct Block {
    std::vector<float> stereo;
    std::atomic<std::uint64_t> sequence{0u};
    mutable std::atomic<std::uint32_t> readers{0u};
    std::uint64_t startFrame = 0u;
    std::uint32_t validFrames = 0u;
    float peak = 0.f;

    Block();
  };

  void workerLoop();
  void invalidateBlocks();

  std::array<Block, kBlockCount> blocks;
  std::thread worker;
  mutable std::mutex requestMutex;
  std::condition_variable requestCv;
  bool stopRequested = false;
  std::string requestedPath;
  std::uint64_t requestedSerial = 0u;
  std::uint64_t appliedSerial = 0u;

  mutable std::mutex metadataMutex;
  std::string loadedPath;
  std::string loadedDisplayName;
  std::string loadError;

  std::atomic<bool> streamReady{false};
  std::atomic<bool> loadInProgress{false};
  std::atomic<std::uint64_t> publishedFrames{0u};
  std::atomic<std::uint32_t> publishedSampleRate{0u};
  std::atomic<std::uint32_t> publishedChannels{0u};
  std::atomic<std::uint64_t> publishedGeneration{0u};
  std::atomic<std::uint64_t> publishedResidencyGeneration{0u};
  std::atomic<float> publishedAbsolutePeak{0.f};
  std::atomic<std::uint64_t> desiredFrame{0u};
  std::atomic<bool> desiredLoop{false};
  std::atomic<std::uint64_t> desiredLoopStartFrame{0u};
  std::atomic<std::uint64_t> desiredLoopEndFrameExclusive{0u};
  std::atomic<std::uint64_t> desiredWindowSequence{0u};

  bool requestSuperseded(std::uint64_t serial) const;
};

} // namespace temporaldeck
