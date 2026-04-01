#include "platter_spec_cases.hpp"
#include "platter_trace_replay.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc == 3 && std::string(argv[1]) == "--replay") {
    bool ok = spec::replayTraceFile(argv[2], std::cout);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
  }
  if (argc != 1) {
    std::cerr << "Usage:\n";
    std::cerr << "  platter_spec_harness\n";
    std::cerr << "  platter_spec_harness --replay <trace.csv>\n";
    return EXIT_FAILURE;
  }

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
