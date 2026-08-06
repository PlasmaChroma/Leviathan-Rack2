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

struct PeakPair {
  float minLeft;
  float maxLeft;
  float minRight;
  float maxRight;

  PeakPair() : minLeft(0.f), maxLeft(0.f), minRight(0.f), maxRight(0.f) {}
  PeakPair(float minLeft, float maxLeft, float minRight, float maxRight)
      : minLeft(minLeft), maxLeft(maxLeft), minRight(minRight), maxRight(maxRight) {}
};

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
  static constexpr std::size_t kOverviewPyramidSize = 4096u;

  LongPlayStreamEngine();
  ~LongPlayStreamEngine();

  void requestLoad(const std::string &path);
  void clear();
  void setDesiredFrame(std::uint64_t frame, bool loop);
  bool readFrame(std::uint64_t frame, float *left, float *right) const;
  bool readStereoInterleaved(std::uint64_t startFrame, std::uint32_t count, float *out) const;
  bool isFrameResident(std::uint64_t frame) const;

  bool ready() const;
  bool loading() const;
  std::uint64_t totalFrames() const;
  std::uint32_t sampleRate() const;
  std::uint32_t channels() const;
  std::uint64_t generation() const;
  float absolutePeak() const;
  std::string path() const;
  std::string displayName() const;
  std::string error() const;

  std::vector<PeakPair> overviewPyramid() const;
  bool isOverviewReady() const;
  std::size_t allocatedAudioBytes() const;

private:
  struct Block {
    std::vector<float> stereo;
    std::atomic<std::uint64_t> sequence{0u};
    mutable std::atomic<std::uint32_t> readers{0u};
    std::uint64_t startFrame = 0u;
    std::uint32_t validFrames = 0u;

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
  std::vector<PeakPair> overviewPyramidData;
  std::atomic<bool> overviewReady{false};

  std::atomic<bool> streamReady{false};
  std::atomic<bool> loadInProgress{false};
  std::atomic<std::uint64_t> publishedFrames{0u};
  std::atomic<std::uint32_t> publishedSampleRate{0u};
  std::atomic<std::uint32_t> publishedChannels{0u};
  std::atomic<std::uint64_t> publishedGeneration{0u};
  std::atomic<float> publishedAbsolutePeak{0.f};
  std::atomic<std::uint64_t> desiredFrame{0u};
  std::atomic<bool> desiredLoop{false};

  bool requestSuperseded(std::uint64_t serial) const;
};

} // namespace temporaldeck
