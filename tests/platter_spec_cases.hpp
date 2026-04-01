#pragma once

#include <string>
#include <vector>

namespace spec {

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

std::vector<TestResult> collectTests();

} // namespace spec
