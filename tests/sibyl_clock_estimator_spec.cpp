#include "SibylClockEstimator.hpp"

#include <cmath>
#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	const double dt = 0.001;
	{
		sibyl::ExternalClockEstimator estimator;
		double total = estimator.process(dt, true, 4, 2000, sibyl::OnExternalStop::HOLD, 120).beatDelta;
		for (int i = 0; i < 124; ++i) total += estimator.process(dt, false, 4, 2000, sibyl::OnExternalStop::HOLD, 120).beatDelta;
		total += estimator.process(dt, true, 4, 2000, sibyl::OnExternalStop::HOLD, 120).beatDelta;
		check(std::abs(total - 0.5) < 1e-9, "detected edges remain exact quarter-PPQN anchors");
		check(std::abs(estimator.estimatedBpm(4, 0) - 120.0) < 1.0, "stable pulse interval estimates tempo");
		double between = 0.0;
		for (int i = 0; i < 20; ++i) between += estimator.process(dt, false, 4, 2000, sibyl::OnExternalStop::HOLD, 120).beatDelta;
		check(between > 0.0, "learned external tempo interpolates between edges");
	}
	{
		sibyl::ExternalClockEstimator estimator;
		estimator.process(dt, true, 24, 10, sibyl::OnExternalStop::HOLD, 120);
		for (int i = 0; i < 21; ++i) estimator.process(dt, false, 24, 10, sibyl::OnExternalStop::HOLD, 120);
		estimator.process(dt, true, 24, 10, sibyl::OnExternalStop::HOLD, 120);
		for (int i = 0; i < 11; ++i) estimator.process(dt, false, 24, 10, sibyl::OnExternalStop::HOLD, 120);
		double held = 0.0;
		for (int i = 0; i < 10; ++i) held += estimator.process(dt, false, 24, 10, sibyl::OnExternalStop::HOLD, 120).beatDelta;
		check(held == 0.0, "hold timeout stops phase");
	}
	{
		sibyl::ExternalClockEstimator freeRun;
		freeRun.process(dt, true, 4, 10, sibyl::OnExternalStop::FREE_RUN, 90);
		for (int i = 0; i < 125; ++i) freeRun.process(dt, false, 4, 10, sibyl::OnExternalStop::FREE_RUN, 90);
		freeRun.process(dt, true, 4, 10, sibyl::OnExternalStop::FREE_RUN, 90);
		double advanced = 0.0;
		for (int i = 0; i < 20; ++i) advanced += freeRun.process(dt, false, 4, 10, sibyl::OnExternalStop::FREE_RUN, 90).beatDelta;
		check(advanced > 0.0, "freeRun timeout continues from learned tempo");
	}
	{
		sibyl::ExternalClockEstimator internal;
		internal.process(dt, true, 4, 10, sibyl::OnExternalStop::INTERNAL, 90);
		for (int i = 1; i < 125; ++i) internal.process(dt, false, 4, 10, sibyl::OnExternalStop::INTERNAL, 90);
		internal.process(dt, true, 4, 10, sibyl::OnExternalStop::INTERNAL, 90);
		// The estimator is at beat phase 0.5. After timeout it continues at the
		// learned tempo to phase 1.0 before selecting the internal 90 BPM rate.
		for (int i = 0; i < 250; ++i) internal.process(dt, false, 4, 10, sibyl::OnExternalStop::INTERNAL, 90);
		double delta = 0.0;
		for (int i = 0; i < 10; ++i) delta += internal.process(dt, false, 4, 5, sibyl::OnExternalStop::INTERNAL, 90).beatDelta;
		check(std::abs(delta - 90.0 / 60.0 * 0.010) < 1e-6, "internal timeout transitions at the next beat to composition BPM");
	}
	{
		sibyl::ExternalClockEstimator jittered;
		jittered.process(dt, true, 4, 2000, sibyl::OnExternalStop::HOLD, 120);
		const int intervals[] = {125, 121, 129, 123, 127, 124, 126, 122, 128};
		for (int samples : intervals) {
			for (int i = 1; i < samples; ++i)
				jittered.process(dt, false, 4, 2000, sibyl::OnExternalStop::HOLD, 120);
			jittered.process(dt, true, 4, 2000, sibyl::OnExternalStop::HOLD, 120);
		}
		double beforeOutlier = jittered.estimatedBpm(4, 0.0);
		for (int i = 1; i < 50; ++i)
			jittered.process(dt, false, 4, 2000, sibyl::OnExternalStop::HOLD, 120);
		jittered.process(dt, true, 4, 2000, sibyl::OnExternalStop::HOLD, 120);
		double afterOutlier = jittered.estimatedBpm(4, 0.0);
		check(std::abs(beforeOutlier - 120.0) < 3.0, "bounded estimator remains stable under alternating edge jitter");
		check(afterOutlier < 140.0, "single fast outlier cannot abruptly replace the learned tempo");
	}

	std::cout << "[SUMMARY] sibyl_clock_estimator_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
