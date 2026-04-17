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
