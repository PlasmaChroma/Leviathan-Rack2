#pragma once

#include "OctaviaAnalysis.hpp"
#include "OctaviaObservation.hpp"

#include <array>
#include <atomic>
#include <cstddef>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace octavia {

static constexpr double RECORDING_MIN_SECONDS = 0.1;
static constexpr double RECORDING_MAX_SECONDS = 30.0;
static constexpr size_t RECORDING_STATUS_LIMIT = 16;

enum class RecordingState : uint8_t {
	Armed,
	Capturing,
	Captured,
	Processing,
	Complete,
	Failed
};

const char* recordingStateName(RecordingState state);

enum class CaptureDisposition : uint8_t {
	Analyze,
	Record,
	AnalyzeAndRecord
};

const char* captureDispositionName(CaptureDisposition disposition);

enum class CaptureAnalysisKind : uint8_t {
	None,
	Group,
	Comparison
};

const char* captureAnalysisKindName(CaptureAnalysisKind kind);

struct CaptureAnalysisRequest {
	CaptureAnalysisKind kind = CaptureAnalysisKind::None;
	AnalysisGroup group;
	AnalysisGroup reference;
	AnalysisGroup target;
	bool detailed = true;
	bool includeSpectrum = false;
};

struct RecordingStatus {
	uint64_t id = 0;
	RecordingState state = RecordingState::Failed;
	CaptureDisposition disposition = CaptureDisposition::Record;
	CaptureAnalysisKind analysisKind = CaptureAnalysisKind::None;
	uint8_t requestedMask = 0;
	uint8_t allConnectedMask = 0;
	uint8_t anyConnectedMask = 0;
	std::array<ObserveChannel, OBSERVATION_CHANNELS> channelOrder{{}};
	size_t channelCount = 0;
	uint64_t targetFrames = 0;
	uint64_t writtenFrames = 0;
	uint64_t startFrame = 0;
	uint64_t endFrame = 0;
	float sampleRate = 0.f;
	std::string label;
	std::string wavPath;
	std::string metadataPath;
	bool analysisAvailable = false;
	bool includeSpectrum = false;
	GroupAnalysis groupAnalysis;
	ComparisonAnalysis comparisonAnalysis;
	std::string error;
};

// One bounded capture may be active at a time. Allocation occurs on the
// caller/server thread and analysis/file I/O on a dedicated worker. process()
// performs only atomic state checks and writes into preallocated planar storage.
class RecordingEngine {
public:
	explicit RecordingEngine(AnalysisEngine* analysisEngine = nullptr);
	~RecordingEngine();

	bool arm(uint8_t requestedMask, double durationSeconds, float sampleRate,
		const std::string& label, const std::string& outputDirectory,
		RecordingStatus* result, std::string* error);
	bool armCapture(uint8_t requestedMask, double durationSeconds, float sampleRate,
		const std::string& label, CaptureDisposition disposition,
		const CaptureAnalysisRequest& analysisRequest,
		const std::string& outputDirectory, RecordingStatus* result,
		std::string* error);
	void process(uint64_t frame, float sampleRate,
		const std::array<float, OBSERVATION_CHANNELS>& volts,
		uint8_t connectedMask) noexcept;
	bool get(uint64_t id, RecordingStatus* result) const;
	bool busy() const noexcept;

private:
	struct Session;
	std::shared_ptr<Session> find(uint64_t id) const;
	static void fillStatus(const Session& session, RecordingStatus* result);
	static bool writeFiles(Session& session, std::string* error);
	bool processIfReady(uint64_t id, std::string* error);
	void runWorker();

	mutable std::mutex mutex_;
	std::deque<std::shared_ptr<Session>> sessions_;
	std::atomic<Session*> active_{nullptr};
	std::atomic<uint64_t> pendingWorkId_{0};
	uint64_t nextId_ = 1;
	std::atomic<bool> stopping_{false};
	AnalysisEngine* analysisEngine_ = nullptr;
	std::thread worker_;
	std::mutex workerMutex_;
	std::condition_variable workerCv_;
};

} // namespace octavia
