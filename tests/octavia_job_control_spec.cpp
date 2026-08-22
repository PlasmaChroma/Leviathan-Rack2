#include "OctaviaJobControl.hpp"

#include <atomic>
#include <iostream>
#include <memory>

namespace {
struct Job { std::atomic<bool> done{false}, cancelled{false}; };
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	auto queued = std::make_shared<Job>();
	octavia::cancelTimedOutJob(queued);
	check(queued->cancelled.load(), "timeout marks queued job cancelled");
	check(!octavia::beginQueuedJob(queued) && queued->done.load(),
		"UI processor skips a cancelled job and completes its lifecycle");
	auto live = std::make_shared<Job>();
	check(octavia::beginQueuedJob(live), "non-cancelled job may execute");
	live->done = true;
	octavia::cancelTimedOutJob(live);
	check(!live->cancelled.load(), "completed job is not retroactively cancelled");
	std::cout << "[SUMMARY] octavia_job_control_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
