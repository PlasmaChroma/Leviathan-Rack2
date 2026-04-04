#include "../src/TemporalDeckMenuUtils.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

using temporaldeck_menu::ArtBatch;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

std::string joinLabels(const std::vector<ArtBatch> &batches) {
  std::string out;
  for (size_t i = 0; i < batches.size(); ++i) {
    if (i > 0) {
      out += ", ";
    }
    out += batches[i].label;
  }
  return out;
}

TestResult testNoBatchingAtTwelve() {
  std::vector<ArtBatch> batches = temporaldeck_menu::buildArtBatches(12, 12);
  bool pass = batches.empty();
  return {"No batching at 12 entries", pass, "batchCount=" + std::to_string(batches.size())};
}

TestResult testThirteenCreatesSecondBatchWithPlus() {
  std::vector<ArtBatch> batches = temporaldeck_menu::buildArtBatches(13, 12);
  bool pass = batches.size() == 2 && batches[0].label == "Art 1-12" && batches[1].label == "Art 13+" &&
              batches[0].begin == 0 && batches[0].endExclusive == 12 && batches[1].begin == 12 &&
              batches[1].endExclusive == 13;
  return {"13 entries batches as 1-12 and 13+", pass, "labels=[" + joinLabels(batches) + "]"};
}

TestResult testTwentyFourCreatesTwoFullBatches() {
  std::vector<ArtBatch> batches = temporaldeck_menu::buildArtBatches(24, 12);
  bool pass = batches.size() == 2 && batches[0].label == "Art 1-12" && batches[1].label == "Art 13-24";
  return {"24 entries batches as two full groups", pass, "labels=[" + joinLabels(batches) + "]"};
}

TestResult testTwentyFiveCreatesTrailingPlusBatch() {
  std::vector<ArtBatch> batches = temporaldeck_menu::buildArtBatches(25, 12);
  bool pass = batches.size() == 3 && batches[0].label == "Art 1-12" && batches[1].label == "Art 13-24" &&
              batches[2].label == "Art 25+";
  return {"25 entries batches with trailing plus group", pass, "labels=[" + joinLabels(batches) + "]"};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testNoBatchingAtTwelve());
  tests.push_back(testThirteenCreatesSecondBatchWithPlus());
  tests.push_back(testTwentyFourCreatesTwoFullBatches());
  tests.push_back(testTwentyFiveCreatesTrailingPlusBatch());

  int failed = 0;
  std::cout << "TemporalDeck Menu Utils Spec\n";
  std::cout << "----------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "----------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}

