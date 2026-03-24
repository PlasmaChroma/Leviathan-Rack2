#include "codec.hpp"

#include "plugin.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

#define DR_FLAC_IMPLEMENTATION
#include "third_party/dr_flac.h"

#define DR_MP3_IMPLEMENTATION
#include "third_party/dr_mp3.h"

namespace temporaldeck {
namespace {

static uint16_t readLe16(const uint8_t *data) {
  return uint16_t(data[0]) | (uint16_t(data[1]) << 8);
}

static uint32_t readLe32(const uint8_t *data) {
  return uint32_t(data[0]) | (uint32_t(data[1]) << 8) | (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
}

static int32_t signExtend24(uint32_t x) {
  return (x & 0x00800000u) ? int32_t(x | 0xFF000000u) : int32_t(x);
}

static float clampAudio(float x) {
  return std::max(-1.f, std::min(1.f, x));
}

static float decodePcmSample(const uint8_t *src, int bitsPerSample, bool isFloat) {
  if (isFloat && bitsPerSample == 32) {
    float value = 0.f;
    std::memcpy(&value, src, sizeof(float));
    return clampAudio(value);
  }

  switch (bitsPerSample) {
  case 8:
    return (float(src[0]) - 128.f) / 128.f;
  case 16:
    return clampAudio(float(int16_t(readLe16(src))) / 32768.f);
  case 24:
    return clampAudio(float(signExtend24(readLe32(src) & 0x00FFFFFFu)) / 8388608.f);
  case 32:
    return clampAudio(float(int32_t(readLe32(src))) / 2147483648.f);
  default:
    return 0.f;
  }
}

static bool failWith(const std::string &message, std::string *errorOut) {
  if (errorOut) {
    *errorOut = message;
  }
  return false;
}

static bool decodeWaveFile(const std::string &path, DecodedSampleFile *out, std::string *errorOut) {
  if (!out) {
    return false;
  }

  std::vector<uint8_t> data;
  try {
    data = system::readFile(path);
  } catch (const std::exception &e) {
    return failWith(e.what(), errorOut);
  }

  if (data.size() < 44) {
    return failWith("File is too small to be a WAV file", errorOut);
  }
  if (std::memcmp(data.data(), "RIFF", 4) != 0 || std::memcmp(data.data() + 8, "WAVE", 4) != 0) {
    return failWith("WAV file is missing RIFF/WAVE header", errorOut);
  }

  const uint8_t *fmtChunk = nullptr;
  size_t fmtSize = 0;
  const uint8_t *dataChunk = nullptr;
  size_t dataSize = 0;
  size_t offset = 12;
  while (offset + 8 <= data.size()) {
    const uint8_t *chunk = data.data() + offset;
    uint32_t chunkSize = readLe32(chunk + 4);
    size_t payloadOffset = offset + 8;
    size_t paddedChunkSize = (size_t(chunkSize) + 1u) & ~size_t(1u);
    if (payloadOffset + size_t(chunkSize) > data.size()) {
      return failWith("WAV file has a truncated chunk", errorOut);
    }
    if (std::memcmp(chunk, "fmt ", 4) == 0) {
      fmtChunk = data.data() + payloadOffset;
      fmtSize = chunkSize;
    } else if (std::memcmp(chunk, "data", 4) == 0) {
      dataChunk = data.data() + payloadOffset;
      dataSize = chunkSize;
    }
    offset = payloadOffset + paddedChunkSize;
  }

  if (!fmtChunk || fmtSize < 16 || !dataChunk || dataSize == 0) {
    return failWith("WAV file is missing fmt or data chunk", errorOut);
  }

  uint16_t formatTag = readLe16(fmtChunk + 0);
  uint16_t channels = readLe16(fmtChunk + 2);
  uint32_t sampleRate = readLe32(fmtChunk + 4);
  uint16_t blockAlign = readLe16(fmtChunk + 12);
  uint16_t bitsPerSample = readLe16(fmtChunk + 14);
  bool isFloat = false;
  if (formatTag == 3) {
    isFloat = true;
  } else if (formatTag != 1) {
    return failWith("Only PCM and 32-bit float WAV files are supported", errorOut);
  }

  if (channels < 1 || channels > 2) {
    return failWith("Only mono and stereo files are supported", errorOut);
  }
  if (sampleRate == 0 || blockAlign == 0 || bitsPerSample == 0) {
    return failWith("WAV format chunk is invalid", errorOut);
  }

  int bytesPerSample = (bitsPerSample + 7) / 8;
  if (blockAlign < channels * bytesPerSample) {
    return failWith("WAV block alignment is invalid", errorOut);
  }
  int frames = int(dataSize / blockAlign);
  if (frames <= 0) {
    return failWith("WAV file contains no sample frames", errorOut);
  }

  out->left.assign(frames, 0.f);
  if (channels > 1) {
    out->right.assign(frames, 0.f);
  } else {
    out->right.clear();
  }

  for (int i = 0; i < frames; ++i) {
    const uint8_t *frame = dataChunk + size_t(i) * blockAlign;
    out->left[i] = decodePcmSample(frame, bitsPerSample, isFloat);
    if (channels > 1) {
      out->right[i] = decodePcmSample(frame + bytesPerSample, bitsPerSample, isFloat);
    }
  }

  out->channels = channels;
  out->frames = frames;
  out->sampleRate = float(sampleRate);
  out->truncated = false;
  return true;
}

static bool fillFromInterleaved(const float *interleaved, uint64_t frames, uint32_t channels, uint32_t sampleRate,
                                DecodedSampleFile *out, std::string *errorOut) {
  if (!interleaved) {
    return failWith("Decoder returned no sample data", errorOut);
  }
  if (channels < 1 || channels > 2) {
    return failWith("Only mono and stereo files are supported", errorOut);
  }
  if (sampleRate == 0) {
    return failWith("Decoded file has invalid sample rate", errorOut);
  }
  if (frames == 0) {
    return failWith("Decoded file contains no sample frames", errorOut);
  }
  if (frames > uint64_t(std::numeric_limits<int>::max())) {
    return failWith("Decoded file is too long", errorOut);
  }

  int frameCount = int(frames);
  out->left.assign(frameCount, 0.f);
  if (channels > 1) {
    out->right.assign(frameCount, 0.f);
  } else {
    out->right.clear();
  }

  for (int i = 0; i < frameCount; ++i) {
    const float *frame = interleaved + size_t(i) * size_t(channels);
    out->left[i] = clampAudio(frame[0]);
    if (channels > 1) {
      out->right[i] = clampAudio(frame[1]);
    }
  }

  out->channels = int(channels);
  out->frames = frameCount;
  out->sampleRate = float(sampleRate);
  out->truncated = false;
  return true;
}

static bool decodeFlacFile(const std::string &path, DecodedSampleFile *out, std::string *errorOut) {
  unsigned int channels = 0;
  unsigned int sampleRate = 0;
  drflac_uint64 totalFrames = 0;
  float *pcm = drflac_open_file_and_read_pcm_frames_f32(path.c_str(), &channels, &sampleRate, &totalFrames, nullptr);
  if (!pcm) {
    return failWith("Failed to decode FLAC file", errorOut);
  }
  bool ok = fillFromInterleaved(pcm, uint64_t(totalFrames), channels, sampleRate, out, errorOut);
  drflac_free(pcm, nullptr);
  return ok;
}

static bool decodeMp3File(const std::string &path, DecodedSampleFile *out, std::string *errorOut) {
  drmp3_config config{};
  drmp3_uint64 totalFrames = 0;
  float *pcm = drmp3_open_file_and_read_pcm_frames_f32(path.c_str(), &config, &totalFrames, nullptr);
  if (!pcm) {
    return failWith("Failed to decode MP3 file", errorOut);
  }
  bool ok = fillFromInterleaved(pcm, uint64_t(totalFrames), uint32_t(config.channels), uint32_t(config.sampleRate), out,
                                errorOut);
  drmp3_free(pcm, nullptr);
  return ok;
}

} // namespace

bool decodeSampleFile(const std::string &path, DecodedSampleFile *out, std::string *errorOut) {
  if (!out) {
    return false;
  }

  *out = DecodedSampleFile();
  std::string ext = system::getExtension(path);
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return (char)std::tolower(c); });

  if (ext == ".wav" || ext == ".wave") {
    return decodeWaveFile(path, out, errorOut);
  }
  if (ext == ".flac") {
    return decodeFlacFile(path, out, errorOut);
  }
  if (ext == ".mp3") {
    return decodeMp3File(path, out, errorOut);
  }

  if (!ext.empty()) {
    return failWith("Unsupported sample format: " + ext + " (supported: WAV, FLAC, MP3)", errorOut);
  }
  return failWith("Unsupported sample format (supported: WAV, FLAC, MP3)", errorOut);
}

} // namespace temporaldeck
