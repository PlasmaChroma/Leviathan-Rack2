#pragma once

#include <cstdint>
#include <fstream>
#include <string>

namespace bifurx {

struct BifurxCurveDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
	};
	struct BifurxPerfDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
		double lastLogTimeSec = -1.0;
		uint64_t lastAudioSampledCount = 0;
		uint64_t lastAudioProcessNs = 0;
		uint64_t lastAudioControlsNs = 0;
		uint64_t lastAudioCoreNs = 0;
		uint64_t lastAudioPreviewNs = 0;
		uint64_t lastAudioAnalysisNs = 0;
		uint64_t lastUiStepCount = 0;
		uint64_t lastUiStepNs = 0;
		uint64_t lastUiDrawCount = 0;
		uint64_t lastUiDrawNs = 0;
		uint64_t lastUiCurveUpdateCount = 0;
		uint64_t lastUiCurveUpdateNs = 0;
		uint64_t lastUiOverlayUpdateCount = 0;
		uint64_t lastUiOverlayUpdateNs = 0;
		uint64_t lastUiDrawSetupCount = 0;
		uint64_t lastUiDrawSetupNs = 0;
		uint64_t lastUiDrawBackgroundCount = 0;
		uint64_t lastUiDrawBackgroundNs = 0;
		uint64_t lastUiDrawExpectedCount = 0;
		uint64_t lastUiDrawExpectedNs = 0;
		uint64_t lastUiDrawOverlayCount = 0;
		uint64_t lastUiDrawOverlayNs = 0;
		uint64_t lastUiDrawCurveCount = 0;
		uint64_t lastUiDrawCurveNs = 0;
		uint64_t lastUiDrawMarkersCount = 0;
		uint64_t lastUiDrawMarkersNs = 0;
	};

	bool shouldSubmitBifurxDebugPacket(uint32_t instanceId, double nowSec);

} // namespace bifurx
