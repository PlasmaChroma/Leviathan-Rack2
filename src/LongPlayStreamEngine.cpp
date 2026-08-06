#include "LongPlayStreamEngine.hpp"

#include "plugin.hpp"
#include "third_party/dr_flac.h"
#include "third_party/dr_mp3.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>

namespace temporaldeck {
namespace {

std::uint16_t readLe16(const unsigned char *data) {
  return std::uint16_t(data[0]) | (std::uint16_t(data[1]) << 8);
}

std::uint32_t readLe32(const unsigned char *data) {
  return std::uint32_t(data[0]) | (std::uint32_t(data[1]) << 8) | (std::uint32_t(data[2]) << 16) |
         (std::uint32_t(data[3]) << 24);
}

std::int32_t signExtend24(std::uint32_t value) {
  return (value & 0x00800000u) ? std::int32_t(value | 0xff000000u) : std::int32_t(value);
}

float decodeWavSample(const unsigned char *source, std::uint16_t bitsPerSample, bool floatingPoint) {
  if (floatingPoint && bitsPerSample == 32u) {
    float value = 0.f;
    std::memcpy(&value, source, sizeof(value));
    return std::isfinite(value) ? std::max(-1.f, std::min(value, 1.f)) : 0.f;
  }
  switch (bitsPerSample) {
  case 8:
    return (float(source[0]) - 128.f) * (1.f / 128.f);
  case 16:
    return float(std::int16_t(readLe16(source))) * (1.f / 32768.f);
  case 24:
    return float(signExtend24(readLe32(source) & 0x00ffffffu)) * (1.f / 8388608.f);
  case 32:
    return float(std::int32_t(readLe32(source))) * (1.f / 2147483648.f);
  default:
    return 0.f;
  }
}

std::string lowercaseExtension(const std::string &path) {
  std::string extension = system::getExtension(path);
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](unsigned char value) { return char(std::tolower(value)); });
  return extension;
}

struct Decoder {
  enum class Kind { None, Wav, Flac, Mp3 };

  Kind kind = Kind::None;
  std::ifstream wav;
  drflac *flac = nullptr;
  drmp3 mp3{};
  bool mp3Open = false;
  std::vector<drmp3_seek_point> mp3SeekPoints;
  std::vector<unsigned char> wavBytes;
  std::vector<float> interleaved;
  std::uint64_t wavDataOffset = 0u;
  std::uint64_t totalFrames = 0u;
  std::uint64_t cursor = 0u;
  std::uint32_t sampleRate = 0u;
  std::uint32_t channels = 0u;
  std::uint16_t wavBlockAlign = 0u;
  std::uint16_t wavBitsPerSample = 0u;
  bool wavFloatingPoint = false;

  ~Decoder() { close(); }

  void close() {
    if (flac) {
      drflac_close(flac);
      flac = nullptr;
    }
    if (mp3Open) {
      drmp3_uninit(&mp3);
      mp3Open = false;
    }
    wav.close();
    mp3SeekPoints.clear();
    kind = Kind::None;
    totalFrames = 0u;
    cursor = 0u;
    sampleRate = 0u;
    channels = 0u;
  }

  bool openWav(const std::string &path, std::string *error) {
    wav.open(path.c_str(), std::ios::binary);
    if (!wav) {
      *error = "Could not open WAV file";
      return false;
    }
    unsigned char header[12]{};
    wav.read(reinterpret_cast<char *>(header), sizeof(header));
    if (wav.gcount() != std::streamsize(sizeof(header)) || std::memcmp(header, "RIFF", 4) != 0 ||
        std::memcmp(header + 8, "WAVE", 4) != 0) {
      *error = "WAV file is missing its RIFF/WAVE header";
      return false;
    }

    bool foundFormat = false;
    bool foundData = false;
    std::uint64_t dataBytes = 0u;
    while (wav && (!foundFormat || !foundData)) {
      unsigned char chunk[8]{};
      wav.read(reinterpret_cast<char *>(chunk), sizeof(chunk));
      if (wav.gcount() != std::streamsize(sizeof(chunk))) {
        break;
      }
      const std::uint32_t chunkSize = readLe32(chunk + 4);
      const std::streamoff payload = wav.tellg();
      if (std::memcmp(chunk, "fmt ", 4) == 0 && chunkSize >= 16u) {
        std::vector<unsigned char> format(chunkSize);
        wav.read(reinterpret_cast<char *>(format.data()), chunkSize);
        if (wav.gcount() != std::streamsize(chunkSize)) {
          break;
        }
        const std::uint16_t tag = readLe16(format.data());
        channels = readLe16(format.data() + 2);
        sampleRate = readLe32(format.data() + 4);
        wavBlockAlign = readLe16(format.data() + 12);
        wavBitsPerSample = readLe16(format.data() + 14);
        wavFloatingPoint = tag == 3u;
        if (tag != 1u && tag != 3u) {
          *error = "Only PCM and 32-bit float WAV files are supported";
          return false;
        }
        foundFormat = true;
      } else if (std::memcmp(chunk, "data", 4) == 0) {
        wavDataOffset = std::uint64_t(payload);
        dataBytes = chunkSize;
        foundData = true;
      }
      wav.clear();
      wav.seekg(payload + std::streamoff((std::uint64_t(chunkSize) + 1u) & ~1u));
    }
    if (!foundFormat || !foundData || sampleRate == 0u || channels < 1u || channels > 2u || wavBlockAlign == 0u) {
      *error = "WAV file has an unsupported or incomplete format";
      return false;
    }
    const std::uint32_t bytesPerSample = (wavBitsPerSample + 7u) / 8u;
    if (bytesPerSample == 0u || wavBlockAlign < channels * bytesPerSample) {
      *error = "WAV block alignment is invalid";
      return false;
    }
    totalFrames = dataBytes / wavBlockAlign;
    if (totalFrames == 0u) {
      *error = "WAV file contains no audio";
      return false;
    }
    wav.clear();
    wav.seekg(std::streamoff(wavDataOffset));
    if (!wav) {
      *error = "Could not seek to WAV audio data";
      return false;
    }
    kind = Kind::Wav;
    cursor = 0u;
    return true;
  }

  bool open(const std::string &path, std::string *error) {
    close();
    const std::string extension = lowercaseExtension(path);
    if (extension == ".wav" || extension == ".wave") {
      return openWav(path, error);
    }
    if (extension == ".flac") {
      flac = drflac_open_file(path.c_str(), nullptr);
      if (!flac || flac->channels < 1u || flac->channels > 2u || flac->sampleRate == 0u ||
          flac->totalPCMFrameCount == 0u) {
        *error = "Could not open stereo or mono FLAC file";
        close();
        return false;
      }
      kind = Kind::Flac;
      channels = flac->channels;
      sampleRate = flac->sampleRate;
      totalFrames = flac->totalPCMFrameCount;
      return true;
    }
    if (extension == ".mp3") {
      if (!drmp3_init_file(&mp3, path.c_str(), nullptr)) {
        *error = "Could not open MP3 file";
        return false;
      }
      mp3Open = true;
      channels = mp3.channels;
      sampleRate = mp3.sampleRate;
      if (channels < 1u || channels > 2u || sampleRate == 0u) {
        *error = "Only stereo and mono MP3 files are supported";
        close();
        return false;
      }
      totalFrames = drmp3_get_pcm_frame_count(&mp3);
      if (totalFrames == 0u) {
        *error = "MP3 file contains no audio";
        close();
        return false;
      }
      drmp3_uint32 seekCount = drmp3_uint32(
          std::max<std::uint64_t>(32u, std::min<std::uint64_t>(4096u, totalFrames / std::max<std::uint64_t>(sampleRate * 2u, 1u))));
      mp3SeekPoints.resize(seekCount);
      if (drmp3_calculate_seek_points(&mp3, &seekCount, mp3SeekPoints.data())) {
        mp3SeekPoints.resize(seekCount);
        drmp3_bind_seek_table(&mp3, seekCount, mp3SeekPoints.data());
      } else {
        mp3SeekPoints.clear();
      }
      drmp3_seek_to_pcm_frame(&mp3, 0u);
      kind = Kind::Mp3;
      cursor = 0u;
      return true;
    }
    *error = "Unsupported file type (use WAV, FLAC, or MP3)";
    return false;
  }

  bool seek(std::uint64_t frame) {
    if (frame == cursor) {
      return true;
    }
    bool ok = false;
    switch (kind) {
    case Kind::Wav:
      wav.clear();
      wav.seekg(std::streamoff(wavDataOffset + frame * wavBlockAlign));
      ok = bool(wav);
      break;
    case Kind::Flac:
      ok = drflac_seek_to_pcm_frame(flac, frame) != DRFLAC_FALSE;
      break;
    case Kind::Mp3:
      ok = drmp3_seek_to_pcm_frame(&mp3, frame) != DRMP3_FALSE;
      break;
    case Kind::None:
      break;
    }
    if (ok) {
      cursor = frame;
    }
    return ok;
  }

  std::uint32_t readStereo(std::uint64_t startFrame, std::uint32_t framesRequested, float *stereoOut) {
    if (!stereoOut || startFrame >= totalFrames || !seek(startFrame)) {
      return 0u;
    }
    const std::uint32_t request =
        std::uint32_t(std::min<std::uint64_t>(framesRequested, totalFrames - startFrame));
    std::uint32_t framesRead = 0u;
    if (kind == Kind::Wav) {
      wavBytes.resize(std::size_t(request) * wavBlockAlign);
      wav.read(reinterpret_cast<char *>(wavBytes.data()), wavBytes.size());
      framesRead = std::uint32_t(std::max<std::streamsize>(0, wav.gcount()) / wavBlockAlign);
      const std::uint32_t bytesPerSample = (wavBitsPerSample + 7u) / 8u;
      for (std::uint32_t i = 0u; i < framesRead; ++i) {
        const unsigned char *source = wavBytes.data() + std::size_t(i) * wavBlockAlign;
        const float left = decodeWavSample(source, wavBitsPerSample, wavFloatingPoint);
        const float right = channels > 1u ? decodeWavSample(source + bytesPerSample, wavBitsPerSample,
                                                           wavFloatingPoint)
                                          : left;
        stereoOut[std::size_t(i) * 2u] = left;
        stereoOut[std::size_t(i) * 2u + 1u] = right;
      }
    } else {
      interleaved.resize(std::size_t(request) * channels);
      if (kind == Kind::Flac) {
        framesRead = std::uint32_t(drflac_read_pcm_frames_f32(flac, request, interleaved.data()));
      } else if (kind == Kind::Mp3) {
        framesRead = std::uint32_t(drmp3_read_pcm_frames_f32(&mp3, request, interleaved.data()));
      }
      for (std::uint32_t i = 0u; i < framesRead; ++i) {
        const float left = interleaved[std::size_t(i) * channels];
        const float right = channels > 1u ? interleaved[std::size_t(i) * channels + 1u] : left;
        stereoOut[std::size_t(i) * 2u] = std::max(-1.f, std::min(left, 1.f));
        stereoOut[std::size_t(i) * 2u + 1u] = std::max(-1.f, std::min(right, 1.f));
      }
    }
    cursor = startFrame + framesRead;
    return framesRead;
  }
};

} // namespace

bool probeLongPlayFile(const std::string &path, LongPlayFileInfo *info,
                       std::string *error) {
  if (!info) {
    if (error) {
      *error = "Missing output metadata";
    }
    return false;
  }
  *info = LongPlayFileInfo();
  Decoder decoder;
  std::string localError;
  if (!decoder.open(path, &localError)) {
    if (error) {
      *error = localError;
    }
    return false;
  }
  info->totalFrames = decoder.totalFrames;
  info->sampleRate = decoder.sampleRate;
  info->channels = decoder.channels;
  return true;
}

LongPlayStreamEngine::Block::Block()
    : stereo(std::size_t(kBlockFrames) * 2u, 0.f) {}

LongPlayStreamEngine::LongPlayStreamEngine() {
  worker = std::thread(&LongPlayStreamEngine::workerLoop, this);
}

LongPlayStreamEngine::~LongPlayStreamEngine() {
  {
    std::lock_guard<std::mutex> lock(requestMutex);
    stopRequested = true;
  }
  requestCv.notify_one();
  if (worker.joinable()) {
    worker.join();
  }
}

void LongPlayStreamEngine::requestLoad(const std::string &pathValue) {
  {
    std::lock_guard<std::mutex> lock(requestMutex);
    requestedPath = pathValue;
    ++requestedSerial;
    loadInProgress.store(true, std::memory_order_release);
  }
  requestCv.notify_one();
}

void LongPlayStreamEngine::clear() { requestLoad(std::string()); }

void LongPlayStreamEngine::setDesiredFrame(std::uint64_t frame, bool loop) {
  desiredFrame.store(frame, std::memory_order_relaxed);
  desiredLoop.store(loop, std::memory_order_relaxed);
}

bool LongPlayStreamEngine::readFrame(std::uint64_t frame, float *left, float *right) const {
  const std::uint64_t blockNumber = frame / kBlockFrames;
  const Block &block = blocks[std::size_t(blockNumber % kBlockCount)];
  const std::uint64_t sequence = block.sequence.load(std::memory_order_acquire);
  if (sequence & 1u) {
    return false;
  }
  block.readers.fetch_add(1u, std::memory_order_acq_rel);
  if (sequence != block.sequence.load(std::memory_order_acquire)) {
    block.readers.fetch_sub(1u, std::memory_order_release);
    return false;
  }
  const std::uint64_t start = block.startFrame;
  const std::uint32_t count = block.validFrames;
  if (frame >= start && frame - start < count) {
    const std::size_t index = std::size_t(frame - start) * 2u;
    if (left)
      *left = block.stereo[index];
    if (right)
      *right = block.stereo[index + 1u];
    block.readers.fetch_sub(1u, std::memory_order_release);
    return true;
  }
  block.readers.fetch_sub(1u, std::memory_order_release);
  return false;
}

bool LongPlayStreamEngine::readStereoInterleaved(std::uint64_t startFrame, std::uint32_t count, float *out) const {
  if (!out)
    return false;
  for (std::uint32_t i = 0u; i < count; ++i) {
    float l = 0.f, r = 0.f;
    if (!readFrame(startFrame + i, &l, &r)) {
      return false;
    }
    out[i * 2u] = l;
    out[i * 2u + 1u] = r;
  }
  return true;
}

bool LongPlayStreamEngine::isFrameResident(std::uint64_t frame) const {
  return readFrame(frame, nullptr, nullptr);
}

bool LongPlayStreamEngine::ready() const {
  return streamReady.load(std::memory_order_acquire);
}

bool LongPlayStreamEngine::loading() const {
  return loadInProgress.load(std::memory_order_acquire);
}

std::uint64_t LongPlayStreamEngine::totalFrames() const {
  return publishedFrames.load(std::memory_order_acquire);
}

std::uint32_t LongPlayStreamEngine::sampleRate() const {
  return publishedSampleRate.load(std::memory_order_acquire);
}

std::uint32_t LongPlayStreamEngine::channels() const {
  return publishedChannels.load(std::memory_order_acquire);
}

std::uint64_t LongPlayStreamEngine::generation() const {
  return publishedGeneration.load(std::memory_order_acquire);
}

float LongPlayStreamEngine::absolutePeak() const {
  return publishedAbsolutePeak.load(std::memory_order_acquire);
}

std::string LongPlayStreamEngine::path() const {
  std::lock_guard<std::mutex> lock(metadataMutex);
  return loadedPath;
}

std::string LongPlayStreamEngine::displayName() const {
  std::lock_guard<std::mutex> lock(metadataMutex);
  return loadedDisplayName;
}

std::string LongPlayStreamEngine::error() const {
  std::lock_guard<std::mutex> lock(metadataMutex);
  return loadError;
}

std::size_t LongPlayStreamEngine::allocatedAudioBytes() const {
  std::size_t bytes = 0u;
  for (const Block &block : blocks) {
    bytes += block.stereo.capacity() * sizeof(float);
  }
  return bytes;
}

bool LongPlayStreamEngine::requestSuperseded(std::uint64_t serial) const {
  std::lock_guard<std::mutex> lock(requestMutex);
  return stopRequested || requestedSerial != serial;
}

void LongPlayStreamEngine::invalidateBlocks() {
  for (Block &block : blocks) {
    block.sequence.fetch_add(1u, std::memory_order_acq_rel);
    while (block.readers.load(std::memory_order_acquire) != 0u) {
      std::this_thread::yield();
    }
    block.startFrame = 0u;
    block.validFrames = 0u;
    block.peak = 0.f;
    block.sequence.fetch_add(1u, std::memory_order_release);
  }
}

void LongPlayStreamEngine::workerLoop() {
  Decoder decoder;
  while (true) {
    std::string pathToLoad;
    std::uint64_t serial = 0u;
    {
      std::unique_lock<std::mutex> lock(requestMutex);
      requestCv.wait_for(lock, std::chrono::milliseconds(2), [this]() {
        return stopRequested || requestedSerial != appliedSerial;
      });
      if (stopRequested) {
        break;
      }
      if (requestedSerial != appliedSerial) {
        pathToLoad = requestedPath;
        serial = requestedSerial;
      }
    }

    if (serial != 0u) {
      streamReady.store(false, std::memory_order_release);
      invalidateBlocks();
      decoder.close();
      std::string openError;
      const bool opened = !pathToLoad.empty() && decoder.open(pathToLoad, &openError);

      {
        std::lock_guard<std::mutex> lock(metadataMutex);
        loadedPath = opened ? pathToLoad : std::string();
        loadedDisplayName = opened ? system::getFilename(pathToLoad) : std::string();
        loadError = pathToLoad.empty() ? std::string() : openError;
      }
      publishedFrames.store(opened ? decoder.totalFrames : 0u, std::memory_order_release);
      publishedSampleRate.store(opened ? decoder.sampleRate : 0u, std::memory_order_release);
      publishedChannels.store(opened ? decoder.channels : 0u, std::memory_order_release);
      publishedAbsolutePeak.store(0.f, std::memory_order_release);
      desiredFrame.store(0u, std::memory_order_relaxed);
      publishedGeneration.fetch_add(1u, std::memory_order_acq_rel);
      streamReady.store(opened, std::memory_order_release);
      loadInProgress.store(false, std::memory_order_release);
      std::lock_guard<std::mutex> lock(requestMutex);
      appliedSerial = serial;
      continue;
    }

    if (!streamReady.load(std::memory_order_acquire) || decoder.totalFrames == 0u) {
      continue;
    }

    // 50/50 Symmetric Window around target center: 16 blocks backward, 16 blocks forward
    const std::uint64_t target = std::min(desiredFrame.load(std::memory_order_relaxed), decoder.totalFrames - 1u);
    const int64_t centerBlockIndex = int64_t(target / kBlockFrames);
    
    // Priority loading sequence: 0 (center), +1, -1, +2, -2, ..., +15, -16
    std::array<std::uint64_t, kBlockCount> wantedStarts{};
    std::size_t wantedCount = 0;
    const int64_t totalBlocks = int64_t(
      (decoder.totalFrames + kBlockFrames - 1u) / kBlockFrames);
    auto appendWantedBlock = [&](int64_t blockIndex) {
      if (wantedCount >= kBlockCount || totalBlocks <= 0) {
        return;
      }
      if (desiredLoop.load(std::memory_order_relaxed)) {
        blockIndex = (blockIndex % totalBlocks + totalBlocks) % totalBlocks;
      } else if (blockIndex < 0 || blockIndex >= totalBlocks) {
        return;
      }
      const std::uint64_t startFrame = std::uint64_t(blockIndex) * kBlockFrames;
      for (std::size_t i = 0; i < wantedCount; ++i) {
        if (wantedStarts[i] == startFrame) {
          return;
        }
      }
      wantedStarts[wantedCount++] = startFrame;
    };
    
    for (int step = 0; step < 16; ++step) {
      // Sixteen blocks on each side of the anchor boundary. The center block
      // belongs to the forward half.
      int offsets[2] = {step, -step - 1};
      for (int offset : offsets) {
        appendWantedBlock(centerBlockIndex + offset);
      }
    }

    // At non-looping file edges, replace unavailable backward/forward blocks
    // with blocks on the usable side so the cache still uses all 32 slots.
    if (!desiredLoop.load(std::memory_order_relaxed) && wantedCount < kBlockCount) {
      const int64_t first = std::max<int64_t>(
        0, std::min<int64_t>(centerBlockIndex - 16, std::max<int64_t>(0, totalBlocks - kBlockCount)));
      const int64_t end = std::min<int64_t>(totalBlocks, first + kBlockCount);
      for (int64_t blockIndex = first; blockIndex < end; ++blockIndex) {
        appendWantedBlock(blockIndex);
      }
    }

    bool filledOne = false;
    for (std::size_t w = 0; w < wantedCount; ++w) {
      const std::uint64_t wantedStart = wantedStarts[w];
      const std::uint64_t blockNumber = wantedStart / kBlockFrames;
      Block *destination = &blocks[std::size_t(blockNumber % kBlockCount)];
      const std::uint64_t sequence = destination->sequence.load(std::memory_order_acquire);
      const bool present = !(sequence & 1u) && destination->validFrames > 0u && destination->startFrame == wantedStart;
      if (present) {
        continue;
      }

      destination->sequence.fetch_add(1u, std::memory_order_acq_rel);
      while (destination->readers.load(std::memory_order_acquire) != 0u) {
        std::this_thread::yield();
      }
      destination->validFrames = 0u;
      const std::uint32_t decoded = decoder.readStereo(wantedStart, kBlockFrames, destination->stereo.data());
      
      float blockPeak = 0.f;
      for (std::size_t i = 0; i < std::size_t(decoded) * 2u; ++i) {
        blockPeak = std::max(blockPeak, std::fabs(destination->stereo[i]));
      }
      destination->peak = blockPeak;

      destination->startFrame = wantedStart;
      destination->validFrames = decoded;
      destination->sequence.fetch_add(1u, std::memory_order_release);
      filledOne = decoded > 0u;
      break;
    }
    if (!filledOne) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    float currentPeak = 0.f;
    for (const Block &block : blocks) {
      if (block.validFrames > 0u) {
        currentPeak = std::max(currentPeak, block.peak);
      }
    }
    publishedAbsolutePeak.store(currentPeak, std::memory_order_release);
  }
  streamReady.store(false, std::memory_order_release);
  invalidateBlocks();
}

} // namespace temporaldeck
