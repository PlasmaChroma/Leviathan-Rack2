#include "platter_spec_cases.hpp"

#include <cstdlib>
#include <iostream>

int main() {
  const auto tests = spec::collectTests();

  int failed = 0;
  std::cout << "TemporalDeck Platter Spec Harness\n";
  std::cout << "--------------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }

  std::cout << "--------------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
