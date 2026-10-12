#include "PuffyDrawDiagnostics.hpp"

#include "plugin.hpp"

namespace {
thread_local PuffyDrawMetrics gPuffyDrawMetrics;
}

PuffyDrawMetrics& puffyDrawMetricsForUiThread() {
	return gPuffyDrawMetrics;
}

PuffyDrawMetrics consumePuffyDrawMetrics() {
	const PuffyDrawMetrics result = gPuffyDrawMetrics;
	gPuffyDrawMetrics = {};
	return result;
}

bool isPuffyDrawMeasurementEnabled() {
	return isDragonKingDebugEnabled() && isPuffyDrawLoggingEnabled();
}
