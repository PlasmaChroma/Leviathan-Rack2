#include "bifurx_filter_test_model.hpp"

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
using bifurx_test_model::responseDb;
using bifurx_test_model::simulateHhRuntimeGainDb;
using bifurx_test_model::simulateLlRuntimeGainDb;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

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
  float sumAbsDiff = 0.f;
  float maxAbsDiff = 0.f;
  int points = 0;
  for (float r : ratios) {
    const float lowHz = center / r;
    const float highHz = center * r;
    const float llDb = responseDb(mLL, lowHz);
    const float hhDb = responseDb(mHH, highHz);
    const float absDiff = std::fabs(llDb - hhDb);
    sumAbsDiff += absDiff;
    maxAbsDiff = std::max(maxAbsDiff, absDiff);
    points++;
  }

  MirrorStats stats;
  stats.meanAbsDiff = sumAbsDiff / std::max(1, points);
  stats.maxAbsDiff = maxAbsDiff;
  return stats;
}

MirrorStats computeLlHhRuntimeMirrorStats(float spanNorm, float q) {
  const float sampleRate = 48000.f;
  const MirrorBand band = makeMirrorBand(sampleRate, 900.f, spanNorm);
  const float damping = 1.f / std::max(q, 0.2f);
  const float ratios[] = {1.08f, 1.16f, 1.28f, 1.45f, 1.70f, 2.0f, 2.4f};
  float sumAbsDiff = 0.f;
  float maxAbsDiff = 0.f;
  int points = 0;
  for (float r : ratios) {
    const float lowHz = band.centerHz / r;
    const float highHz = band.centerHz * r;
    const float llDb = simulateLlRuntimeGainDb(
      sampleRate, lowHz, 0.25f, 0.5f, band.cutoffA, band.cutoffB, damping, damping
    );
    const float hhDb = simulateHhRuntimeGainDb(
      sampleRate, highHz, 0.25f, 0.5f, band.cutoffA, band.cutoffB, damping, damping, band.wideMorph
    );
    const float absDiff = std::fabs(llDb - hhDb);
    sumAbsDiff += absDiff;
    maxAbsDiff = std::max(maxAbsDiff, absDiff);
    points++;
  }
  MirrorStats stats;
  stats.meanAbsDiff = sumAbsDiff / std::max(1, points);
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

TestResult testPreviewCircuitSelectionCollapsedForSvfTuning() {
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

  const bool same = sumAbs < 1e-3f && a.circuitMode == 0 && b.circuitMode == 0;
  return {
    "Preview circuit voicing is pinned to SVF while tuning",
    same,
    "sumAbs=" + std::to_string(sumAbs) + " circuitA=" + std::to_string(a.circuitMode) +
      " circuitB=" + std::to_string(b.circuitMode)
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
    testLowLowAndHighHighPreviewMirrorLowSpan(),
    testLowLowAndHighHighPreviewMirrorMidSpan(),
    testLowLowAndHighHighPreviewMirrorHighSpan(),
    testLowLowAndHighHighRuntimeMirrorAcrossSpans(),
    testBandBandHasTwoLocalPeaksNearMarkers(),
    testModesRemainDistinctAtReferenceState(),
    testPreviewCircuitSelectionCollapsedForSvfTuning(),
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
