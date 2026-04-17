#include "bifurx_filter_test_model.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using bifurx_test_model::PreviewModel;
using bifurx_test_model::PreviewState;
using bifurx_test_model::simulateLlRuntimeGainDb;
using bifurx_test_model::makePreviewModel;
using bifurx_test_model::responseDb;

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

  // This threshold is intentionally strict for current tuning work:
  // midpoint should not collapse far below the first-region level.
  const bool pass = (dbMid > -10.f) && (dbMid < dbF1 - 2.f) && (dbMid > dbF2 + 6.f);
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
