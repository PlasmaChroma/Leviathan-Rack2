#include "bifurx_filter_test_model.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using bifurx_test_model::PreviewModel;
using bifurx_test_model::PreviewState;
using bifurx_test_model::cascadeWideMorph;
using bifurx_test_model::clampf;
using bifurx_test_model::clamp01;
using bifurx_test_model::makePreviewModel;
using bifurx_test_model::makeLlRuntimeSweep;
using bifurx_test_model::responseDb;
using bifurx_test_model::LlRuntimeSweepPoint;
using bifurx_test_model::simulateHhRuntimeGainDb;
using bifurx_test_model::simulateLlRuntimeGainDb;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

std::vector<float> llSweepFrequenciesHz() {
  return {30.f, 40.f, 55.f, 75.f, 100.f, 140.f, 200.f, 300.f, 450.f, 650.f, 900.f, 1300.f, 1800.f};
}

TestResult testLowLowMaintainsDoubleSlopeOrdering() {
  PreviewState s;
  s.sampleRate = 48000.f;
  s.mode = 0;      // Low + Low
  s.freqA = 140.f;
  s.freqB = 950.f;
  s.qA = 0.95f;
  s.qB = 0.95f;
  s.spanNorm = 0.55f;
  const PreviewModel m = makePreviewModel(s);

  const float dbSub = responseDb(m, 30.f);
  const float dbMid = responseDb(m, 250.f);
  const float dbUpper = responseDb(m, 1800.f);
  const float dbHigh = responseDb(m, 9000.f);
  const bool ordered = (dbSub > dbMid) && (dbMid > dbUpper) && (dbUpper > dbHigh);
  const bool meaningfulDrop = (dbSub - dbUpper) > 8.f && (dbUpper - dbHigh) > 6.f;
  return {
    "LL profile keeps low-end > mid > upper > high",
    ordered && meaningfulDrop,
    "dbSub=" + std::to_string(dbSub) + " dbMid=" + std::to_string(dbMid) +
      " dbUpper=" + std::to_string(dbUpper) + " dbHigh=" + std::to_string(dbHigh)
  };
}

TestResult testLowLowMidpointDipNotOverlyDeep() {
  // Spec target (manual-inspired): LL should transition with a two-slope shape,
  // not carve an extreme "hole" between the two characteristic frequencies.
  PreviewState s;
  s.sampleRate = 48000.f;
  s.mode = 0;      // Low + Low
  s.freqA = 140.f;
  s.freqB = 950.f;
  s.qA = 1.1f;
  s.qB = 1.1f;
  s.spanNorm = 0.45f;
  const PreviewModel m = makePreviewModel(s);

  const float f1 = s.freqA;
  const float f2 = s.freqB;
  const float midpoint = std::sqrt(f1 * f2);
  const float dbF1 = responseDb(m, f1);
  const float dbMid = responseDb(m, midpoint);
  const float dbF2 = responseDb(m, f2);

  // Keep the middle region clearly below the first shoulder, but avoid
  // an extreme hole in the transition.
  const bool pass = (dbMid > -18.f) && (dbMid < dbF1 - 3.f) && (dbMid > dbF2 + 8.f);
  return {
    "LL midpoint dip remains controlled between f1 and f2",
    pass,
    "dbF1=" + std::to_string(dbF1) + " dbMid=" + std::to_string(dbMid) + " dbF2=" + std::to_string(dbF2)
  };
}

TestResult testLowLowRuntimeRetainsPrePeakLowBand() {
  // Runtime-path check: below first peak, LL should not collapse low-band level.
  const float sampleRate = 48000.f;
  const float cutoffA = 140.f;
  const float cutoffB = 950.f;
  const float dampingA = 1.f / 1.1f;
  const float dampingB = 1.f / 1.1f;

  const float gain40 = simulateLlRuntimeGainDb(
    sampleRate, 40.f, 0.25f, 0.5f, cutoffA, cutoffB, dampingA, dampingB
  );
  const float gain80 = simulateLlRuntimeGainDb(
    sampleRate, 80.f, 0.25f, 0.5f, cutoffA, cutoffB, dampingA, dampingB
  );
  const float gain140 = simulateLlRuntimeGainDb(
    sampleRate, 140.f, 0.25f, 0.5f, cutoffA, cutoffB, dampingA, dampingB
  );

  const bool pass = (gain40 > -3.f) && (gain80 > -3.f) && (gain140 > -6.f);
  return {
    "LL runtime keeps low band before first peak",
    pass,
    "g40=" + std::to_string(gain40) + " g80=" + std::to_string(gain80) + " g140=" + std::to_string(gain140)
  };
}

TestResult testLowLowRuntimeSemanticExportsTrackSvfBaseline() {
  const float sampleRate = 48000.f;
  const float cutoffA = 140.f;
  const float cutoffB = 950.f;
  const float dampingA = 1.f / 1.1f;
  const float dampingB = 1.f / 1.1f;

  const float svf = simulateLlRuntimeGainDb(
    sampleRate, 40.f, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, 0
  );
  const float dfm = simulateLlRuntimeGainDb(
    sampleRate, 40.f, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, 1
  );
  const float ms2 = simulateLlRuntimeGainDb(
    sampleRate, 40.f, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, 2
  );
  const float prd = simulateLlRuntimeGainDb(
    sampleRate, 40.f, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, 3
  );

  const float maxDelta = std::max(
    std::max(std::fabs(dfm - svf), std::fabs(ms2 - svf)),
    std::fabs(prd - svf)
  );
  const bool pass = maxDelta < 5.0f;
  return {
    "LL semantic exports keep alternate circuits near SVF baseline",
    pass,
    "svf=" + std::to_string(svf) + " dfm=" + std::to_string(dfm) +
      " ms2=" + std::to_string(ms2) + " prd=" + std::to_string(prd) +
      " maxDelta=" + std::to_string(maxDelta)
  };
}

TestResult testLowLowSweepDatasetHasStableShape() {
  const float sampleRate = 48000.f;
  const float cutoffA = 140.f;
  const float cutoffB = 950.f;
  const float dampingA = 1.f / 1.1f;
  const float dampingB = 1.f / 1.1f;
  const std::vector<float> freqs = llSweepFrequenciesHz();
  bool pass = true;

  for (int circuit = 0; circuit <= 3; ++circuit) {
    const std::vector<LlRuntimeSweepPoint> sweep = makeLlRuntimeSweep(
      sampleRate, freqs, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, circuit
    );
    if (sweep.size() != freqs.size()) {
      pass = false;
      break;
    }
    for (size_t i = 0; i < sweep.size(); ++i) {
      const LlRuntimeSweepPoint& p = sweep[i];
      if (!(p.freqHz > 0.f) || (i > 0 && !(p.freqHz > sweep[i - 1].freqHz))) {
        pass = false;
        break;
      }
      if (!std::isfinite(p.telemetry.inputRms) || !std::isfinite(p.telemetry.stageALpRms)
          || !std::isfinite(p.telemetry.stageBLpRms) || !std::isfinite(p.telemetry.outputRms)
          || !std::isfinite(p.telemetry.stageBOverADb) || !std::isfinite(p.telemetry.outputOverInputDb)) {
        pass = false;
        break;
      }
    }
    if (!pass) {
      break;
    }
  }

  return {
    "LL sweep dataset is finite, ordered, and complete for all circuits",
    pass,
    "freq_count=" + std::to_string(freqs.size())
  };
}

TestResult testLowLowSweepContractEnvelopeAgainstSvf() {
  const float sampleRate = 48000.f;
  const float cutoffA = 140.f;
  const float cutoffB = 950.f;
  const float dampingA = 1.f / 1.1f;
  const float dampingB = 1.f / 1.1f;
  const std::vector<float> freqs = llSweepFrequenciesHz();
  const std::vector<LlRuntimeSweepPoint> svf = makeLlRuntimeSweep(
    sampleRate, freqs, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, 0
  );

  bool pass = true;
  float worstOutputDeltaDb = 0.f;
  float worstStageDeltaDb = 0.f;

  for (int circuit = 1; circuit <= 3; ++circuit) {
    const std::vector<LlRuntimeSweepPoint> alt = makeLlRuntimeSweep(
      sampleRate, freqs, 0.10f, 0.5f, cutoffA, cutoffB, dampingA, dampingB, circuit
    );
    for (size_t i = 0; i < alt.size(); ++i) {
      const float outputDelta = alt[i].telemetry.outputOverInputDb - svf[i].telemetry.outputOverInputDb;
      const float stageDelta = alt[i].telemetry.stageBOverADb - svf[i].telemetry.stageBOverADb;
      worstOutputDeltaDb = std::max(worstOutputDeltaDb, std::fabs(outputDelta));
      worstStageDeltaDb = std::max(worstStageDeltaDb, std::fabs(stageDelta));

      // Contract guardrails: broad enough for nonlinear behavior, narrow enough
      // to flag collapse/blow-up regressions.
      if (outputDelta < -12.f || outputDelta > 8.f) {
        pass = false;
      }
      if (stageDelta < -6.f || stageDelta > 6.f) {
        pass = false;
      }
    }
  }

  return {
    "LL sweep contract remains within broad SVF-relative envelopes",
    pass,
    "worstOutputDeltaDb=" + std::to_string(worstOutputDeltaDb) +
      " worstStageDeltaDb=" + std::to_string(worstStageDeltaDb)
  };
}

struct MirrorStats {
  float meanAbsDiff = 0.f;
  float maxAbsDiff = 0.f;
};

struct MirrorBand {
  float centerHz = 900.f;
  float cutoffA = 900.f;
  float cutoffB = 900.f;
  float wideMorph = 0.f;
};

MirrorBand makeMirrorBand(float sampleRate, float centerHz, float spanNorm) {
  MirrorBand band;
  band.centerHz = clampf(centerHz, 4.f, 0.46f * sampleRate);
  const float spanOct = 8.f * std::pow(clamp01(spanNorm), 1.45f);
  const float maxShiftUp = std::max(0.f, std::log2((0.46f * sampleRate) / band.centerHz));
  const float maxShiftDown = std::max(0.f, std::log2(band.centerHz / 4.f));
  const float halfSpanOct = std::min(0.5f * spanOct, std::min(maxShiftUp, maxShiftDown));
  band.cutoffA = band.centerHz * std::pow(2.f, -halfSpanOct);
  band.cutoffB = band.centerHz * std::pow(2.f, halfSpanOct);
  band.wideMorph = cascadeWideMorph(spanNorm);
  return band;
}

MirrorStats computeLlHhPreviewMirrorStats(float spanNorm, float q) {
  const MirrorBand band = makeMirrorBand(48000.f, 900.f, spanNorm);
  PreviewState ll;
  ll.sampleRate = 48000.f;
  ll.mode = 0;      // Low + Low
  ll.freqA = band.cutoffA;
  ll.freqB = band.cutoffB;
  ll.qA = q;
  ll.qB = q;
  ll.spanNorm = spanNorm;
  ll.balance = 0.f;

  PreviewState hh = ll;
  hh.mode = 9;      // High + High

  const PreviewModel mLL = makePreviewModel(ll);
  const PreviewModel mHH = makePreviewModel(hh);
  const float center = band.centerHz;

  const float ratios[] = {1.08f, 1.16f, 1.28f, 1.45f, 1.70f, 2.0f, 2.4f};
  std::vector<float> diffs;
  diffs.reserve(16);
  for (float r : ratios) {
    const float lowHz = center / r;
    const float highHz = center * r;
    const float llDb = responseDb(mLL, lowHz);
    const float hhDb = responseDb(mHH, highHz);
    diffs.push_back(llDb - hhDb);
  }

  float meanDiff = 0.f;
  for (float d : diffs) {
    meanDiff += d;
  }
  meanDiff /= std::max<size_t>(1, diffs.size());

  float sumAbsDiff = 0.f;
  float maxAbsDiff = 0.f;
  for (float d : diffs) {
    const float centered = d - meanDiff;
    const float absDiff = std::fabs(centered);
    sumAbsDiff += absDiff;
    maxAbsDiff = std::max(maxAbsDiff, absDiff);
  }

  MirrorStats stats;
  stats.meanAbsDiff = sumAbsDiff / std::max<size_t>(1, diffs.size());
  stats.maxAbsDiff = maxAbsDiff;
  return stats;
}

MirrorStats computeLlHhRuntimeMirrorStats(float spanNorm, float q) {
  const float sampleRate = 48000.f;
  const MirrorBand band = makeMirrorBand(sampleRate, 900.f, spanNorm);
  const float damping = 1.f / std::max(q, 0.2f);
  const float ratios[] = {1.08f, 1.16f, 1.28f, 1.45f, 1.70f, 2.0f, 2.4f};
  std::vector<float> diffs;
  diffs.reserve(16);
  for (float r : ratios) {
    const float lowHz = band.centerHz / r;
    const float highHz = band.centerHz * r;
    const float llDb = simulateLlRuntimeGainDb(
      sampleRate, lowHz, 0.25f, 0.5f, band.cutoffA, band.cutoffB, damping, damping
    );
    const float hhDb = simulateHhRuntimeGainDb(
      sampleRate, highHz, 0.25f, 0.5f, band.cutoffA, band.cutoffB, damping, damping, band.wideMorph
    );
    diffs.push_back(llDb - hhDb);
  }
  float meanDiff = 0.f;
  for (float d : diffs) {
    meanDiff += d;
  }
  meanDiff /= std::max<size_t>(1, diffs.size());

  float sumAbsDiff = 0.f;
  float maxAbsDiff = 0.f;
  for (float d : diffs) {
    const float centered = d - meanDiff;
    const float absDiff = std::fabs(centered);
    sumAbsDiff += absDiff;
    maxAbsDiff = std::max(maxAbsDiff, absDiff);
  }

  MirrorStats stats;
  stats.meanAbsDiff = sumAbsDiff / std::max<size_t>(1, diffs.size());
  stats.maxAbsDiff = maxAbsDiff;
  return stats;
}

TestResult testLowLowAndHighHighPreviewMirrorLowSpan() {
  const MirrorStats stats = computeLlHhPreviewMirrorStats(0.35f, 1.1f);
  const bool pass = (stats.meanAbsDiff < 0.12f) && (stats.maxAbsDiff < 0.20f);
  return {
    "LL/HH preview mirror at low span",
    pass,
    "meanAbsDiff=" + std::to_string(stats.meanAbsDiff) + " maxAbsDiff=" + std::to_string(stats.maxAbsDiff)
  };
}

TestResult testLowLowAndHighHighPreviewMirrorMidSpan() {
  const MirrorStats stats = computeLlHhPreviewMirrorStats(0.65f, 1.1f);
  const bool pass = (stats.meanAbsDiff < 0.12f) && (stats.maxAbsDiff < 0.20f);
  return {
    "LL/HH preview mirror at mid span",
    pass,
    "meanAbsDiff=" + std::to_string(stats.meanAbsDiff) + " maxAbsDiff=" + std::to_string(stats.maxAbsDiff)
  };
}

TestResult testLowLowAndHighHighPreviewMirrorHighSpan() {
  const MirrorStats stats = computeLlHhPreviewMirrorStats(0.95f, 1.1f);
  const bool pass = (stats.meanAbsDiff < 0.12f) && (stats.maxAbsDiff < 0.20f);
  return {
    "LL/HH preview mirror at high span",
    pass,
    "meanAbsDiff=" + std::to_string(stats.meanAbsDiff) + " maxAbsDiff=" + std::to_string(stats.maxAbsDiff)
  };
}

TestResult testLowLowAndHighHighRuntimeMirrorAcrossSpans() {
  const MirrorStats low = computeLlHhRuntimeMirrorStats(0.35f, 1.1f);
  const MirrorStats mid = computeLlHhRuntimeMirrorStats(0.65f, 1.1f);
  const MirrorStats high = computeLlHhRuntimeMirrorStats(0.95f, 1.1f);
  const bool pass = (low.meanAbsDiff < 0.15f) && (low.maxAbsDiff < 0.25f)
    && (mid.meanAbsDiff < 0.15f) && (mid.maxAbsDiff < 0.25f)
    && (high.meanAbsDiff < 0.15f) && (high.maxAbsDiff < 0.25f);
  return {
    "LL/HH runtime mirror remains aligned at low/mid/high span",
    pass,
    "low(mean,max)=(" + std::to_string(low.meanAbsDiff) + "," + std::to_string(low.maxAbsDiff) + ") "
      "mid=(" + std::to_string(mid.meanAbsDiff) + "," + std::to_string(mid.maxAbsDiff) + ") "
      "high=(" + std::to_string(high.meanAbsDiff) + "," + std::to_string(high.maxAbsDiff) + ")"
  };
}

float computeCenterHzFromKnobAndVoct(float freqParamNorm, float voctCv, float fmSum) {
  constexpr float kFreqMinHz = 4.f;
  constexpr float kFreqLog2Span = 12.7731392f;  // log2(28000 / 4)
  const float clampedKnob = clamp01(freqParamNorm);
  const float octaveShift = kFreqLog2Span * clampedKnob + voctCv + fmSum;
  return kFreqMinHz * std::pow(2.f, octaveShift);
}

TestResult testSvfFrequencyKnobMappingIsMonotonic() {
  const float voct = 0.f;
  const float fm = 0.f;
  const float h0 = computeCenterHzFromKnobAndVoct(0.10f, voct, fm);
  const float h1 = computeCenterHzFromKnobAndVoct(0.30f, voct, fm);
  const float h2 = computeCenterHzFromKnobAndVoct(0.50f, voct, fm);
  const float h3 = computeCenterHzFromKnobAndVoct(0.70f, voct, fm);
  const float h4 = computeCenterHzFromKnobAndVoct(0.90f, voct, fm);
  const bool pass = (h0 < h1) && (h1 < h2) && (h2 < h3) && (h3 < h4);
  return {
    "SVF center frequency mapping is monotonic over FREQ knob",
    pass,
    "hz=" + std::to_string(h0) + "," + std::to_string(h1) + "," + std::to_string(h2) +
      "," + std::to_string(h3) + "," + std::to_string(h4)
  };
}

TestResult testSvfVoctTrackingIsMonotonicPerVolt() {
  const float freqNorm = 0.58f;
  const float hm1 = computeCenterHzFromKnobAndVoct(freqNorm, -1.f, 0.f);
  const float h0 = computeCenterHzFromKnobAndVoct(freqNorm, 0.f, 0.f);
  const float h1 = computeCenterHzFromKnobAndVoct(freqNorm, 1.f, 0.f);
  const float upRatio = h1 / std::max(h0, 1e-6f);
  const float downRatio = h0 / std::max(hm1, 1e-6f);
  const bool monotonic = (hm1 < h0) && (h0 < h1);
  const bool nearOctave = std::fabs(upRatio - 2.f) < 0.05f && std::fabs(downRatio - 2.f) < 0.05f;
  return {
    "SVF V/OCT mapping is monotonic and near 2x per volt",
    monotonic && nearOctave,
    "hm1=" + std::to_string(hm1) + " h0=" + std::to_string(h0) + " h1=" + std::to_string(h1) +
      " upRatio=" + std::to_string(upRatio) + " downRatio=" + std::to_string(downRatio)
  };
}

TestResult testSvfBandBandResonanceLiftIncreasesWithQ() {
  PreviewState s;
  s.sampleRate = 48000.f;
  s.mode = 5;      // Band + Band
  s.freqA = 320.f;
  s.freqB = 1400.f;
  s.balance = 0.f;
  s.spanNorm = 0.52f;

  auto localSharpnessDb = [&](float q) {
    s.qA = q;
    s.qB = q;
    const PreviewModel m = makePreviewModel(s);
    const float center = responseDb(m, s.freqA);
    const float shoulderLo = responseDb(m, s.freqA * 0.84f);
    const float shoulderHi = responseDb(m, s.freqA * 1.20f);
    return center - 0.5f * (shoulderLo + shoulderHi);
  };

  const float lowQSharpness = localSharpnessDb(0.8f);
  const float midQSharpness = localSharpnessDb(1.4f);
  const float highQSharpness = localSharpnessDb(2.2f);

  const bool pass = (lowQSharpness < midQSharpness) && (midQSharpness < highQSharpness)
    && ((highQSharpness - lowQSharpness) > 1.5f);
  return {
    "SVF resonance sharpness around marker increases with Q",
    pass,
    "lowQ=" + std::to_string(lowQSharpness) + " midQ=" + std::to_string(midQSharpness) +
      " highQ=" + std::to_string(highQSharpness)
  };
}

TestResult testSvfLowLowRuntimeDoesNotCollapseNearLowPeak() {
  // Regression guard for the reported 40Hz tone with first LL peak near 53.9Hz.
  const float sampleRate = 48000.f;
  const float gain40 = simulateLlRuntimeGainDb(
    sampleRate, 40.f, 0.25f, 0.5f, 53.9f, 114.f, 1.f / 1.2f, 1.f / 1.2f
  );
  const float gain54 = simulateLlRuntimeGainDb(
    sampleRate, 53.9f, 0.25f, 0.5f, 53.9f, 114.f, 1.f / 1.2f, 1.f / 1.2f
  );
  // 40Hz can be below the first shoulder, but should not vanish.
  const bool pass = (gain40 > -10.f) && (gain54 > gain40 - 4.f);
  return {
    "SVF LL runtime keeps low tone energy near first low peak",
    pass,
    "g40=" + std::to_string(gain40) + " g53.9=" + std::to_string(gain54)
  };
}

TestResult testSvfLowLowPreviewRuntimeTrendAgreement() {
  const float sampleRate = 48000.f;
  PreviewState s;
  s.sampleRate = sampleRate;
  s.mode = 0;      // Low + Low
  s.freqA = 140.f;
  s.freqB = 950.f;
  s.qA = 1.1f;
  s.qB = 1.1f;
  s.balance = 0.f;
  s.spanNorm = 0.45f;
  const PreviewModel m = makePreviewModel(s);

  const float runtime40 = simulateLlRuntimeGainDb(sampleRate, 40.f, 0.10f, 0.18f, s.freqA, s.freqB, 1.f / s.qA, 1.f / s.qB);
  const float runtime300 = simulateLlRuntimeGainDb(sampleRate, 300.f, 0.10f, 0.18f, s.freqA, s.freqB, 1.f / s.qA, 1.f / s.qB);
  const float runtime1800 = simulateLlRuntimeGainDb(sampleRate, 1800.f, 0.10f, 0.18f, s.freqA, s.freqB, 1.f / s.qA, 1.f / s.qB);

  const float preview40 = responseDb(m, 40.f);
  const float preview300 = responseDb(m, 300.f);
  const float preview1800 = responseDb(m, 1800.f);

  const bool previewOrdered = (preview40 > preview300) && (preview300 > preview1800);
  const bool runtimeOrdered = (runtime40 > runtime300) && (runtime300 > runtime1800);
  return {
    "SVF LL preview and runtime share low>mid>high trend",
    previewOrdered && runtimeOrdered,
    "preview(40,300,1800)=(" + std::to_string(preview40) + "," + std::to_string(preview300) + "," + std::to_string(preview1800) + ") "
      "runtime=(" + std::to_string(runtime40) + "," + std::to_string(runtime300) + "," + std::to_string(runtime1800) + ")"
  };
}

TestResult testSpanIncreasesMarkerSeparationAtFixedCenter() {
  const float sampleRate = 48000.f;
  const float centerHz = 900.f;
  const MirrorBand narrow = makeMirrorBand(sampleRate, centerHz, 0.20f);
  const MirrorBand medium = makeMirrorBand(sampleRate, centerHz, 0.55f);
  const MirrorBand wide = makeMirrorBand(sampleRate, centerHz, 0.90f);

  const float sepNarrow = narrow.cutoffB / std::max(narrow.cutoffA, 1e-6f);
  const float sepMedium = medium.cutoffB / std::max(medium.cutoffA, 1e-6f);
  const float sepWide = wide.cutoffB / std::max(wide.cutoffA, 1e-6f);
  const bool pass = (sepNarrow < sepMedium) && (sepMedium < sepWide);
  return {
    "SPAN increases A/B marker separation at fixed center",
    pass,
    "sep(n,m,w)=(" + std::to_string(sepNarrow) + "," + std::to_string(sepMedium) + "," + std::to_string(sepWide) + ")"
  };
}

TestResult testSpanWidensBandBandDualPeakSpacing() {
  const float sampleRate = 48000.f;
  PreviewState s;
  s.sampleRate = sampleRate;
  s.mode = 5;      // Band + Band
  s.qA = 4.5f;
  s.qB = 4.5f;
  s.balance = 0.f;

  const MirrorBand narrowBand = makeMirrorBand(sampleRate, 900.f, 0.20f);
  s.freqA = narrowBand.cutoffA;
  s.freqB = narrowBand.cutoffB;
  s.spanNorm = 0.20f;
  const PreviewModel narrow = makePreviewModel(s);

  const MirrorBand wideBand = makeMirrorBand(sampleRate, 900.f, 0.90f);
  s.freqA = wideBand.cutoffA;
  s.freqB = wideBand.cutoffB;
  s.spanNorm = 0.90f;
  const PreviewModel wide = makePreviewModel(s);

  const float narrowBridge = responseDb(narrow, 900.f);
  const float wideBridge = responseDb(wide, 900.f);
  // With wider span, the center bridge between the two BP peaks should drop.
  const bool pass = wideBridge < (narrowBridge - 2.f);
  return {
    "SPAN widening deepens BB center bridge between peaks",
    pass,
    "bridgeDb(narrow,wide)=(" + std::to_string(narrowBridge) + "," + std::to_string(wideBridge) + ")"
  };
}

TestResult testBalanceShiftsBandBandEnergyTowardSelectedSide() {
  PreviewState s;
  s.sampleRate = 48000.f;
  s.mode = 5;      // Band + Band
  s.freqA = 340.f;
  s.freqB = 1500.f;
  s.qA = 4.0f;
  s.qB = 4.0f;
  s.spanNorm = 0.50f;

  s.balance = -0.85f;
  const PreviewModel leftTilt = makePreviewModel(s);
  const float leftLow = responseDb(leftTilt, s.freqA);
  const float leftHigh = responseDb(leftTilt, s.freqB);

  s.balance = 0.85f;
  const PreviewModel rightTilt = makePreviewModel(s);
  const float rightLow = responseDb(rightTilt, s.freqA);
  const float rightHigh = responseDb(rightTilt, s.freqB);

  const bool lowFavoredWhenNegative = leftLow > (leftHigh + 1.f);
  const bool highFavoredWhenPositive = rightHigh > (rightLow + 1.f);
  return {
    "BALANCE steers BB emphasis toward low/high side",
    lowFavoredWhenNegative && highFavoredWhenPositive,
    "negBal(low,high)=(" + std::to_string(leftLow) + "," + std::to_string(leftHigh) + ") "
      "posBal(low,high)=(" + std::to_string(rightLow) + "," + std::to_string(rightHigh) + ")"
  };
}

TestResult testBandBandHasTwoLocalPeaksNearMarkers() {
  PreviewState s;
  s.sampleRate = 48000.f;
  s.mode = 5;      // Band + Band
  s.freqA = 380.f;
  s.freqB = 1600.f;
  s.qA = 5.0f;
  s.qB = 5.0f;
  const PreviewModel m = makePreviewModel(s);

  const float a = m.markerFreqA;
  const float b = m.markerFreqB;
  const float aDb = responseDb(m, a);
  const float bDb = responseDb(m, b);
  const float aLeft = responseDb(m, a * 0.82f);
  const float aRight = responseDb(m, a * 1.22f);
  const float bLeft = responseDb(m, b * 0.82f);
  const float bRight = responseDb(m, b * 1.22f);
  const bool aPeakish = aDb > aLeft && aDb > aRight;
  const bool bPeakish = bDb > bLeft && bDb > bRight;
  return {
    "BB markers sit near local resonant maxima",
    aPeakish && bPeakish,
    "aDb=" + std::to_string(aDb) + " aLeft=" + std::to_string(aLeft) + " aRight=" + std::to_string(aRight) +
      " bDb=" + std::to_string(bDb) + " bLeft=" + std::to_string(bLeft) + " bRight=" + std::to_string(bRight)
  };
}

TestResult testModesRemainDistinctAtReferenceState() {
  PreviewState base;
  base.sampleRate = 48000.f;
  base.freqA = 250.f;
  base.freqB = 1100.f;
  base.qA = 1.25f;
  base.qB = 1.25f;
  base.spanNorm = 0.42f;

  PreviewState m0s = base;
  PreviewState m3s = base;
  PreviewState m5s = base;
  m0s.mode = 0;
  m3s.mode = 3;
  m5s.mode = 5;

  const PreviewModel m0 = makePreviewModel(m0s);
  const PreviewModel m3 = makePreviewModel(m3s);
  const PreviewModel m5 = makePreviewModel(m5s);

  const float hz[6] = {40.f, 120.f, 300.f, 700.f, 1800.f, 6000.f};
  float d03 = 0.f;
  float d05 = 0.f;
  float d35 = 0.f;
  for (float f : hz) {
    const float a = responseDb(m0, f);
    const float b = responseDb(m3, f);
    const float c = responseDb(m5, f);
    d03 += std::fabs(a - b);
    d05 += std::fabs(a - c);
    d35 += std::fabs(b - c);
  }
  const bool distinct = d03 > 20.f && d05 > 20.f && d35 > 20.f;
  return {
    "Reference mode curves are not collapsing into one shape",
    distinct,
    "d03=" + std::to_string(d03) + " d05=" + std::to_string(d05) + " d35=" + std::to_string(d35)
  };
}

TestResult testPreviewCircuitSelectionRespectsCircuitMode() {
  PreviewState svf;
  PreviewState dfm;
  svf.sampleRate = 48000.f;
  svf.mode = 4;
  svf.freqA = 300.f;
  svf.freqB = 1800.f;
  svf.qA = 1.5f;
  svf.qB = 1.7f;
  svf.circuitMode = 0;
  dfm = svf;
  dfm.circuitMode = 1;

  const PreviewModel a = makePreviewModel(svf);
  const PreviewModel b = makePreviewModel(dfm);
  float sumAbs = 0.f;
  const float hz[5] = {80.f, 200.f, 600.f, 1700.f, 4500.f};
  for (float f : hz) {
    sumAbs += std::fabs(responseDb(a, f) - responseDb(b, f));
  }

  const bool same = sumAbs > 0.25f && a.circuitMode == 0 && b.circuitMode == 1;
  return {
    "Preview circuit selection respects circuit mode",
    same,
    "sumAbs=" + std::to_string(sumAbs) + " circuitA=" + std::to_string(a.circuitMode) +
      " circuitB=" + std::to_string(b.circuitMode)
  };
}

std::vector<float> qualifierSweepFrequenciesHz() {
  return {
    30.f, 40.f, 55.f, 75.f, 100.f, 135.f, 180.f, 240.f, 320.f, 430.f,
    580.f, 780.f, 1050.f, 1420.f, 1910.f, 2580.f, 3480.f, 4700.f, 6350.f, 8600.f
  };
}

float percentileValue(const std::vector<float>& values, float p) {
  if (values.empty()) {
    return 0.f;
  }
  std::vector<float> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  const float clampedP = clampf(p, 0.f, 1.f);
  const size_t idx = std::min(sorted.size() - 1, size_t(std::floor(clampedP * float(sorted.size() - 1))));
  return sorted[idx];
}

struct SvfQualifierMetrics {
  float meanSignedDeltaDb = 0.f;
  float meanAbsDeltaDb = 0.f;
  float p95AbsDeltaDb = 0.f;
  float maxAbsDeltaDb = 0.f;
  float meanCenteredAbsDeltaDb = 0.f;
  float p95CenteredAbsDeltaDb = 0.f;
  float meanSlopeAbsDeltaDb = 0.f;
  float maxSlopeAbsDeltaDb = 0.f;
  float lowBandBiasDb = 0.f;
  float midBandBiasDb = 0.f;
  float highBandBiasDb = 0.f;
  float worstFreqHz = 0.f;
  float worstDeltaDb = 0.f;
};

struct SvfQualifierPairResult {
  int mode = -1;
  int circuit = -1;
  float score = 0.f;
  SvfQualifierMetrics metrics;
};

struct LlQualifierScenario {
  float freqA = 0.f;
  float freqB = 0.f;
  float qA = 1.f;
  float qB = 1.f;
  float spanNorm = 0.f;
  float balance = 0.f;
};

std::vector<LlQualifierScenario> llQualifierScenarios() {
  return {
    {80.f, 420.f, 1.10f, 1.10f, 0.35f, 0.f},
    {140.f, 950.f, 1.10f, 1.10f, 0.45f, 0.f},
    {220.f, 1400.f, 1.25f, 1.25f, 0.55f, 0.f},
    {320.f, 2200.f, 1.40f, 1.40f, 0.70f, 0.f},
    {500.f, 4200.f, 1.60f, 1.60f, 0.85f, 0.f},
  };
}

SvfQualifierMetrics qualifyModeAgainstSvf(int mode, int circuitMode) {
  const std::vector<float> freqs = qualifierSweepFrequenciesHz();

  PreviewState base;
  base.sampleRate = 48000.f;
  base.freqA = 220.f;
  base.freqB = 1400.f;
  base.qA = 1.25f;
  base.qB = 1.25f;
  base.spanNorm = 0.55f;
  base.balance = 0.f;

  PreviewState svfState = base;
  svfState.mode = mode;
  svfState.circuitMode = 0;
  const PreviewModel svfModel = makePreviewModel(svfState);

  PreviewState altState = svfState;
  altState.circuitMode = circuitMode;
  const PreviewModel altModel = makePreviewModel(altState);

  std::vector<float> deltasDb;
  std::vector<float> absDeltasDb;
  std::vector<float> centeredAbsDeltasDb;
  std::vector<float> slopeAbsDeltasDb;
  deltasDb.reserve(freqs.size());
  absDeltasDb.reserve(freqs.size());
  centeredAbsDeltasDb.reserve(freqs.size());
  slopeAbsDeltasDb.reserve(freqs.size());

  float lowBiasSum = 0.f;
  float midBiasSum = 0.f;
  float highBiasSum = 0.f;
  int lowBiasCount = 0;
  int midBiasCount = 0;
  int highBiasCount = 0;

  SvfQualifierMetrics metrics;
  for (float hz : freqs) {
    const float svfDb = responseDb(svfModel, hz);
    const float altDb = responseDb(altModel, hz);
    const float deltaDb = altDb - svfDb;
    const float absDeltaDb = std::fabs(deltaDb);
    deltasDb.push_back(deltaDb);
    absDeltasDb.push_back(absDeltaDb);
    metrics.meanSignedDeltaDb += deltaDb;
    metrics.meanAbsDeltaDb += absDeltaDb;
    if (absDeltaDb > metrics.maxAbsDeltaDb) {
      metrics.maxAbsDeltaDb = absDeltaDb;
      metrics.worstFreqHz = hz;
      metrics.worstDeltaDb = deltaDb;
    }

    if (hz <= 180.f) {
      lowBiasSum += deltaDb;
      lowBiasCount++;
    } else if (hz <= 1400.f) {
      midBiasSum += deltaDb;
      midBiasCount++;
    } else {
      highBiasSum += deltaDb;
      highBiasCount++;
    }
  }

  metrics.meanSignedDeltaDb /= std::max<size_t>(1, deltasDb.size());
  metrics.meanAbsDeltaDb /= std::max<size_t>(1, absDeltasDb.size());
  metrics.p95AbsDeltaDb = percentileValue(absDeltasDb, 0.95f);

  for (float deltaDb : deltasDb) {
    centeredAbsDeltasDb.push_back(std::fabs(deltaDb - metrics.meanSignedDeltaDb));
  }
  metrics.meanCenteredAbsDeltaDb = 0.f;
  for (float v : centeredAbsDeltasDb) {
    metrics.meanCenteredAbsDeltaDb += v;
  }
  metrics.meanCenteredAbsDeltaDb /= std::max<size_t>(1, centeredAbsDeltasDb.size());
  metrics.p95CenteredAbsDeltaDb = percentileValue(centeredAbsDeltasDb, 0.95f);

  for (size_t i = 1; i < freqs.size(); ++i) {
    const float svfSlope = responseDb(svfModel, freqs[i]) - responseDb(svfModel, freqs[i - 1]);
    const float altSlope = responseDb(altModel, freqs[i]) - responseDb(altModel, freqs[i - 1]);
    const float slopeAbsDeltaDb = std::fabs(altSlope - svfSlope);
    slopeAbsDeltasDb.push_back(slopeAbsDeltaDb);
    metrics.meanSlopeAbsDeltaDb += slopeAbsDeltaDb;
    metrics.maxSlopeAbsDeltaDb = std::max(metrics.maxSlopeAbsDeltaDb, slopeAbsDeltaDb);
  }
  metrics.meanSlopeAbsDeltaDb /= std::max<size_t>(1, slopeAbsDeltasDb.size());

  metrics.lowBandBiasDb = lowBiasSum / std::max(1, lowBiasCount);
  metrics.midBandBiasDb = midBiasSum / std::max(1, midBiasCount);
  metrics.highBandBiasDb = highBiasSum / std::max(1, highBiasCount);
  return metrics;
}

SvfQualifierMetrics qualifyLowLowAgainstSvfAcrossScenarios(int circuitMode) {
  const std::vector<float> freqs = qualifierSweepFrequenciesHz();
  const std::vector<LlQualifierScenario> scenarios = llQualifierScenarios();

  std::vector<float> deltasDb;
  std::vector<float> absDeltasDb;
  std::vector<float> centeredAbsDeltasDb;
  std::vector<float> slopeAbsDeltasDb;

  float lowBiasSum = 0.f;
  float midBiasSum = 0.f;
  float highBiasSum = 0.f;
  int lowBiasCount = 0;
  int midBiasCount = 0;
  int highBiasCount = 0;

  SvfQualifierMetrics metrics;
  for (size_t si = 0; si < scenarios.size(); ++si) {
    const LlQualifierScenario& scenario = scenarios[si];

    PreviewState svfState;
    svfState.sampleRate = 48000.f;
    svfState.mode = 0;
    svfState.circuitMode = 0;
    svfState.freqA = scenario.freqA;
    svfState.freqB = scenario.freqB;
    svfState.qA = scenario.qA;
    svfState.qB = scenario.qB;
    svfState.spanNorm = scenario.spanNorm;
    svfState.balance = scenario.balance;
    const PreviewModel svfModel = makePreviewModel(svfState);

    PreviewState altState = svfState;
    altState.circuitMode = circuitMode;
    const PreviewModel altModel = makePreviewModel(altState);

    for (float hz : freqs) {
      const float svfDb = responseDb(svfModel, hz);
      const float altDb = responseDb(altModel, hz);
      const float deltaDb = altDb - svfDb;
      const float absDeltaDb = std::fabs(deltaDb);
      deltasDb.push_back(deltaDb);
      absDeltasDb.push_back(absDeltaDb);
      metrics.meanSignedDeltaDb += deltaDb;
      metrics.meanAbsDeltaDb += absDeltaDb;
      if (absDeltaDb > metrics.maxAbsDeltaDb) {
        metrics.maxAbsDeltaDb = absDeltaDb;
        metrics.worstFreqHz = hz;
        metrics.worstDeltaDb = deltaDb;
      }

      if (hz <= 180.f) {
        lowBiasSum += deltaDb;
        lowBiasCount++;
      } else if (hz <= 1400.f) {
        midBiasSum += deltaDb;
        midBiasCount++;
      } else {
        highBiasSum += deltaDb;
        highBiasCount++;
      }
    }

    for (size_t i = 1; i < freqs.size(); ++i) {
      const float svfSlope = responseDb(svfModel, freqs[i]) - responseDb(svfModel, freqs[i - 1]);
      const float altSlope = responseDb(altModel, freqs[i]) - responseDb(altModel, freqs[i - 1]);
      const float slopeAbsDeltaDb = std::fabs(altSlope - svfSlope);
      slopeAbsDeltasDb.push_back(slopeAbsDeltaDb);
      metrics.meanSlopeAbsDeltaDb += slopeAbsDeltaDb;
      metrics.maxSlopeAbsDeltaDb = std::max(metrics.maxSlopeAbsDeltaDb, slopeAbsDeltaDb);
    }
  }

  metrics.meanSignedDeltaDb /= std::max<size_t>(1, deltasDb.size());
  metrics.meanAbsDeltaDb /= std::max<size_t>(1, absDeltasDb.size());
  metrics.p95AbsDeltaDb = percentileValue(absDeltasDb, 0.95f);

  for (float deltaDb : deltasDb) {
    centeredAbsDeltasDb.push_back(std::fabs(deltaDb - metrics.meanSignedDeltaDb));
  }
  for (float v : centeredAbsDeltasDb) {
    metrics.meanCenteredAbsDeltaDb += v;
  }
  metrics.meanCenteredAbsDeltaDb /= std::max<size_t>(1, centeredAbsDeltasDb.size());
  metrics.p95CenteredAbsDeltaDb = percentileValue(centeredAbsDeltasDb, 0.95f);
  metrics.meanSlopeAbsDeltaDb /= std::max<size_t>(1, slopeAbsDeltasDb.size());

  metrics.lowBandBiasDb = lowBiasSum / std::max(1, lowBiasCount);
  metrics.midBandBiasDb = midBiasSum / std::max(1, midBiasCount);
  metrics.highBandBiasDb = highBiasSum / std::max(1, highBiasCount);
  return metrics;
}

TestResult testLowLowMeetsDedicatedSvfTuningQualifier() {
  bool pass = true;
  std::string detail;
  for (int circuit = 1; circuit <= 3; ++circuit) {
    const SvfQualifierMetrics metrics = qualifyLowLowAgainstSvfAcrossScenarios(circuit);
    const bool circuitPass =
      metrics.p95AbsDeltaDb <= 1.5f &&
      metrics.maxAbsDeltaDb <= 2.0f &&
      metrics.meanSlopeAbsDeltaDb <= 0.45f &&
      metrics.maxSlopeAbsDeltaDb <= 1.2f;
    pass = pass && circuitPass;

    if (!detail.empty()) {
      detail += " ";
    }
    detail +=
      "[c" + std::to_string(circuit) +
      (circuitPass ? " PASS" : " FAIL") +
      " p95Abs=" + std::to_string(metrics.p95AbsDeltaDb) +
      " maxAbs=" + std::to_string(metrics.maxAbsDeltaDb) +
      " meanSlope=" + std::to_string(metrics.meanSlopeAbsDeltaDb) +
      " maxSlope=" + std::to_string(metrics.maxSlopeAbsDeltaDb) + "]";
  }

  return {
    "LL dedicated SVF tuning qualifier holds across representative states",
    pass,
    detail
  };
}

TestResult testAllModesMeetSvfTuningQualifier() {
  bool pass = true;
  int violations = 0;
  float worstScore = -1.f;
  int worstMode = -1;
  int worstCircuit = -1;
  SvfQualifierMetrics worstMetrics;
  std::vector<SvfQualifierPairResult> offenders;
  std::vector<SvfQualifierPairResult> llPairs;

  for (int mode = 0; mode <= 9; ++mode) {
    for (int circuit = 1; circuit <= 3; ++circuit) {
      const SvfQualifierMetrics metrics = qualifyModeAgainstSvf(mode, circuit);
      float score = metrics.p95CenteredAbsDeltaDb + 0.75f * metrics.meanSlopeAbsDeltaDb + 0.15f * metrics.p95AbsDeltaDb;
      if (mode == 0) {
        llPairs.push_back({mode, circuit, score, metrics});
      }
      bool pairPass = false;
      if (circuit == 2) {
        pairPass =
          metrics.maxAbsDeltaDb <= 12.f &&
          metrics.p95AbsDeltaDb <= 8.f &&
          metrics.p95CenteredAbsDeltaDb <= 3.f &&
          metrics.meanSlopeAbsDeltaDb <= 1.5f &&
          metrics.maxSlopeAbsDeltaDb <= 4.f;
      }
      else {
        pairPass =
          metrics.maxAbsDeltaDb <= 14.f &&
          metrics.p95AbsDeltaDb <= 9.f;
      }
      if (!pairPass) {
        pass = false;
        violations++;
        offenders.push_back({mode, circuit, score, metrics});
      }

      if (score > worstScore) {
        worstScore = score;
        worstMode = mode;
        worstCircuit = circuit;
        worstMetrics = metrics;
      }
    }
  }

  std::sort(
    offenders.begin(), offenders.end(),
    [](const SvfQualifierPairResult& a, const SvfQualifierPairResult& b) { return a.score > b.score; }
  );
  std::sort(
    llPairs.begin(), llPairs.end(),
    [](const SvfQualifierPairResult& a, const SvfQualifierPairResult& b) { return a.circuit < b.circuit; }
  );

  std::string offenderSummary;
  const size_t offenderLimit = offenders.size();
  for (size_t i = 0; i < offenderLimit; ++i) {
    const SvfQualifierPairResult& offender = offenders[i];
    if (!offenderSummary.empty()) {
      offenderSummary += " ";
    }
    offenderSummary +=
      "[m" + std::to_string(offender.mode) +
      " c" + std::to_string(offender.circuit) +
      " score=" + std::to_string(offender.score) +
      " p95c=" + std::to_string(offender.metrics.p95CenteredAbsDeltaDb) +
      " slope=" + std::to_string(offender.metrics.meanSlopeAbsDeltaDb) + "]";
  }

  std::string llSummary;
  for (size_t i = 0; i < llPairs.size(); ++i) {
    const SvfQualifierPairResult& ll = llPairs[i];
    if (!llSummary.empty()) {
      llSummary += " ";
    }
    llSummary +=
      "[c" + std::to_string(ll.circuit) +
      " score=" + std::to_string(ll.score) +
      " p95c=" + std::to_string(ll.metrics.p95CenteredAbsDeltaDb) +
      " slope=" + std::to_string(ll.metrics.meanSlopeAbsDeltaDb) +
      " p95Abs=" + std::to_string(ll.metrics.p95AbsDeltaDb) +
      " maxAbs=" + std::to_string(ll.metrics.maxAbsDeltaDb) + "]";
  }

  return {
    "Character preview curves stay within mode-appropriate SVF-relative bounds",
    pass,
    "violations=" + std::to_string(violations) +
      " worstScore=" + std::to_string(worstScore) +
      " mode=" + std::to_string(worstMode) +
      " circuit=" + std::to_string(worstCircuit) +
      " p95Centered=" + std::to_string(worstMetrics.p95CenteredAbsDeltaDb) +
      " meanSlope=" + std::to_string(worstMetrics.meanSlopeAbsDeltaDb) +
      " p95Abs=" + std::to_string(worstMetrics.p95AbsDeltaDb) +
      " maxAbs=" + std::to_string(worstMetrics.maxAbsDeltaDb) +
      " worstFreqHz=" + std::to_string(worstMetrics.worstFreqHz) +
      " worstDeltaDb=" + std::to_string(worstMetrics.worstDeltaDb) +
      " LL=" + llSummary +
      " offenders=" + offenderSummary
  };
}

} // namespace

int main() {
  const std::vector<TestResult> tests = {
    testSvfFrequencyKnobMappingIsMonotonic(),
    testSvfVoctTrackingIsMonotonicPerVolt(),
    testSvfBandBandResonanceLiftIncreasesWithQ(),
    testSvfLowLowRuntimeDoesNotCollapseNearLowPeak(),
    testSvfLowLowPreviewRuntimeTrendAgreement(),
    testSpanIncreasesMarkerSeparationAtFixedCenter(),
    testSpanWidensBandBandDualPeakSpacing(),
    testBalanceShiftsBandBandEnergyTowardSelectedSide(),
    testLowLowMaintainsDoubleSlopeOrdering(),
    testLowLowMidpointDipNotOverlyDeep(),
    testLowLowRuntimeRetainsPrePeakLowBand(),
    testLowLowRuntimeSemanticExportsTrackSvfBaseline(),
    testLowLowSweepDatasetHasStableShape(),
    testLowLowSweepContractEnvelopeAgainstSvf(),
    testLowLowMeetsDedicatedSvfTuningQualifier(),
    testLowLowAndHighHighPreviewMirrorLowSpan(),
    testLowLowAndHighHighPreviewMirrorMidSpan(),
    testLowLowAndHighHighPreviewMirrorHighSpan(),
    testLowLowAndHighHighRuntimeMirrorAcrossSpans(),
    testBandBandHasTwoLocalPeaksNearMarkers(),
    testModesRemainDistinctAtReferenceState(),
    testPreviewCircuitSelectionRespectsCircuitMode(),
    testAllModesMeetSvfTuningQualifier(),
  };

  int fails = 0;
  for (const TestResult& t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name;
    if (!t.detail.empty()) {
      std::cout << " :: " << t.detail;
    }
    std::cout << "\n";
    if (!t.pass) {
      fails++;
    }
  }

  if (fails > 0) {
    std::cout << "[SUMMARY] bifurx_filter_spec failed " << fails << " / " << tests.size() << " tests\n";
    return 1;
  }
  std::cout << "[SUMMARY] bifurx_filter_spec passed " << tests.size() << " tests\n";
  return 0;
}
