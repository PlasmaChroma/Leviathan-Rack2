#include "TemporalDeckSamplePrep.hpp"

#include <algorithm>
#include <cmath>

namespace temporaldeck {

namespace {

static constexpr float kSampleFileVoltageScale = 5.f;

int maxFramesForModeAtSampleRate(int mode, float sampleRate) {
  return std::max(1, int(std::floor(temporaldeck_modes::usableBufferSecondsForMode(mode) * std::max(sampleRate, 1.f))));
}

int resampledFrameCount(int sourceFrames, float sourceRate, float targetRate) {
  if (sourceFrames <= 0 || sourceRate <= 1.f || targetRate <= 1.f) {
    return 0;
  }
  double seconds = double(sourceFrames) / double(sourceRate);
  return std::max(1, int(std::round(seconds * double(targetRate))));
}

void resampleSampleChannel(const std::vector<float> &src, float sourceRate, float targetRate, int outFrames,
                           std::vector<float> *dst, float outputGain = 1.f) {
  dst->assign(std::max(outFrames, 0), 0.f);
  if (src.empty() || outFrames <= 0) {
    return;
  }
  if (src.size() == 1 || std::fabs(sourceRate - targetRate) < 1e-3f) {
    for (int i = 0; i < outFrames; ++i) {
      int srcIndex = std::min<int>(i, src.size() - 1);
      (*dst)[i] = src[srcIndex] * outputGain;
    }
    return;
  }

  double ratio = double(sourceRate) / double(targetRate);
  for (int i = 0; i < outFrames; ++i) {
    double srcPos = double(i) * ratio;
    int i0 = clamp(int(std::floor(srcPos)), 0, int(src.size()) - 1);
    int i1 = clamp(i0 + 1, 0, int(src.size()) - 1);
    float t = float(srcPos - double(i0));
    (*dst)[i] = crossfade(src[i0], src[i1], t) * outputGain;
  }
}

} // namespace

int chooseSampleBufferMode(const DecodedSampleFile &sample) {
  if (sample.channels <= 1) {
    return TemporalDeckEngine::BUFFER_DURATION_10MIN_MONO;
  }
  return TemporalDeckEngine::BUFFER_DURATION_10MIN_STEREO;
}

bool buildPreparedSample(const DecodedSampleFile &decodedSample, float targetSampleRate, int bufferMode,
                         bool autoPlayOnLoad, PreparedSampleData *outPrepared) {
  if (!outPrepared) {
    return false;
  }
  PreparedSampleData prepared;
  if (decodedSample.frames <= 0 || decodedSample.left.empty() || targetSampleRate <= 1.f) {
    *outPrepared = prepared;
    return false;
  }

  prepared.bufferMode = clamp(bufferMode, TemporalDeckEngine::BUFFER_DURATION_10S,
                              TemporalDeckEngine::BUFFER_DURATION_COUNT - 1);
  prepared.sampleRate = targetSampleRate;
  prepared.autoPlayOnLoad = autoPlayOnLoad;
  prepared.monoStorage = temporaldeck_modes::isMonoBufferMode(prepared.bufferMode);

  int outFrames = resampledFrameCount(decodedSample.frames, decodedSample.sampleRate, targetSampleRate);
  int maxFrames = maxFramesForModeAtSampleRate(prepared.bufferMode, targetSampleRate);
  prepared.truncated = decodedSample.truncated;
  if (outFrames > maxFrames) {
    outFrames = maxFrames;
    prepared.truncated = true;
  }
  if (outFrames <= 0) {
    *outPrepared = prepared;
    return false;
  }

  if (prepared.monoStorage) {
    std::vector<float> leftResampled;
    resampleSampleChannel(decodedSample.left, decodedSample.sampleRate, targetSampleRate, outFrames, &leftResampled,
                          kSampleFileVoltageScale);
    if (decodedSample.channels > 1 && !decodedSample.right.empty()) {
      std::vector<float> rightResampled;
      resampleSampleChannel(decodedSample.right, decodedSample.sampleRate, targetSampleRate, outFrames, &rightResampled,
                            kSampleFileVoltageScale);
      for (int i = 0; i < outFrames; ++i) {
        leftResampled[i] = 0.5f * (leftResampled[i] + rightResampled[i]);
      }
    }
    prepared.left = std::move(leftResampled);
    std::vector<float>().swap(prepared.right);
  } else {
    resampleSampleChannel(decodedSample.left, decodedSample.sampleRate, targetSampleRate, outFrames, &prepared.left,
                          kSampleFileVoltageScale);
    if (decodedSample.channels > 1 && !decodedSample.right.empty()) {
      resampleSampleChannel(decodedSample.right, decodedSample.sampleRate, targetSampleRate, outFrames, &prepared.right,
                            kSampleFileVoltageScale);
    } else {
      prepared.right = prepared.left;
    }
  }

  prepared.frames = std::min(outFrames, int(prepared.left.size()));
  prepared.valid = prepared.frames > 0;
  *outPrepared = std::move(prepared);
  return outPrepared->valid;
}

} // namespace temporaldeck
