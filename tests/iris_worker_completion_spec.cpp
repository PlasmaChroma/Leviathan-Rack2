#include "../src/IrisWorkerCompletion.hpp"

#include <iostream>
#include <string>

namespace {

int failures = 0;

void check(const std::string& name, bool condition) {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
  if (!condition) ++failures;
}

} // namespace

int main() {
  const auto failure = iris_worker::classifyCompletion(false, "read failed");
  check("failed work preserves the published state by refusing publication",
        !iris_worker::shouldPublish(failure));
  check("failed work reports an error", iris_worker::shouldReportFailure(failure));

  const auto partial = iris_worker::classifyCompletion(true, "reload read failed");
  check("reload fallback publishes its rebuilt retained source",
        iris_worker::shouldPublish(partial));
  check("reload fallback keeps the reload error visible",
        iris_worker::shouldReportFailure(partial));

  const auto success = iris_worker::classifyCompletion(true, {});
  check("ordinary success publishes without an error",
        iris_worker::shouldPublish(success) && !iris_worker::shouldReportFailure(success));

  if (failures != 0) {
    std::cerr << failures << " Iris worker completion checks failed\n";
    return 1;
  }
  std::cout << "Iris worker completion checks passed\n";
  return 0;
}
