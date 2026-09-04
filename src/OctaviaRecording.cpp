#include "OctaviaRecording.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <thread>
#include <vector>

namespace octavia {

namespace {

enum class RecordingFailure : uint8_t {
	None,
	SampleRateChanged,
	AnalysisFailed,
	WriteFailed
};

std::string joinPath(const std::string& directory, const std::string& name) {
	if (directory.empty()) return name;
	const char tail = directory.back();
	return directory + ((tail == '/' || tail == '\\') ? "" : "/") + name;
}

std::string safeFileLabel(const std::string& label) {
	std::string result;
	result.reserve(std::min<size_t>(label.size(), 48));
	for (char c : label) {
		if (result.size() >= 48) break;
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9')) result.push_back(c);
		else if ((c == '-' || c == '_' || c == ' ') && !result.empty()
				&& result.back() != '-') result.push_back('-');
	}
	while (!result.empty() && result.back() == '-') result.pop_back();
	return result.empty() ? "capture" : result;
}

std::string jsonString(const std::string& value) {
	std::string result = "\"";
	for (unsigned char c : value) {
		switch (c) {
			case '\\': result += "\\\\"; break;
			case '"': result += "\\\""; break;
			case '\n': result += "\\n"; break;
			case '\r': result += "\\r"; break;
			case '\t': result += "\\t"; break;
			default:
				if (c < 0x20) {
					char escaped[8];
					std::snprintf(escaped, sizeof(escaped), "\\u%04x", c);
					result += escaped;
				} else result.push_back(static_cast<char>(c));
		}
	}
	return result + "\"";
}

void writeLe16(std::ofstream& output, uint16_t value) {
	const char bytes[2] = {
		static_cast<char>(value & 0xffu),
		static_cast<char>((value >> 8) & 0xffu)
	};
	output.write(bytes, sizeof(bytes));
}

void writeLe32(std::ofstream& output, uint32_t value) {
	const char bytes[4] = {
		static_cast<char>(value & 0xffu),
		static_cast<char>((value >> 8) & 0xffu),
		static_cast<char>((value >> 16) & 0xffu),
		static_cast<char>((value >> 24) & 0xffu)
	};
	output.write(bytes, sizeof(bytes));
}

const char* failureText(RecordingFailure failure) {
	switch (failure) {
		case RecordingFailure::SampleRateChanged: return "sample_rate_changed";
		case RecordingFailure::AnalysisFailed: return "capture_analysis_failed";
		case RecordingFailure::WriteFailed: return "recording_write_failed";
		default: return "";
	}
}

} // namespace

struct RecordingEngine::Session {
	struct ControlTransition {
		uint64_t offsetFrames = 0;
		uint8_t port = 0;
		uint8_t channel = 0;
		uint8_t priority = 0;
		float voltage = 0.f;
	};
	uint64_t id = 0;
	uint8_t requestedMask = 0;
	std::array<ObserveChannel, OBSERVATION_CHANNELS> channelOrder{{}};
	size_t channelCount = 0;
	uint64_t targetFrames = 0;
	float sampleRate = 0.f;
	std::string label;
	std::string wavPath;
	std::string metadataPath;
	FrozenObservation observation;
	CaptureDisposition disposition = CaptureDisposition::Record;
	CaptureAnalysisRequest analysisRequest;
	GroupAnalysis groupAnalysis;
	ComparisonAnalysis comparisonAnalysis;
	std::string detailError;
	std::atomic<RecordingState> state{RecordingState::Armed};
	std::atomic<RecordingFailure> failure{RecordingFailure::None};
	std::atomic<uint64_t> writtenFrames{0};
	std::atomic<uint64_t> startFrame{0};
	std::atomic<uint64_t> endFrame{0};
	std::atomic<uint64_t> controlStartFrame{0};
	std::atomic<bool> controlStarted{false};
	std::atomic<uint8_t> allConnectedMask{0};
	std::atomic<uint8_t> anyConnectedMask{0};
	ControlProgram controlProgram;
	std::array<ControlTransition, CONTROL_MAX_EVENTS * 2> controlTransitions{{}};
	size_t controlTransitionCount = 0;
	size_t nextControlTransition = 0;
	std::array<std::array<float, CONTROL_CHANNELS>, CONTROL_PORTS> currentControlVolts{{}};
	uint8_t requestedControlMask = 0;
	std::atomic<uint8_t> allControlConnectedMask{0};
	std::atomic<uint8_t> anyControlConnectedMask{0};
};

const char* recordingStateName(RecordingState state) {
	switch (state) {
		case RecordingState::Armed: return "armed";
		case RecordingState::Capturing: return "capturing";
		case RecordingState::Captured: return "captured";
		case RecordingState::Processing: return "processing";
		case RecordingState::Complete: return "complete";
		case RecordingState::Failed: return "failed";
		default: return "failed";
	}
}

const char* captureDispositionName(CaptureDisposition disposition) {
	switch (disposition) {
		case CaptureDisposition::Analyze: return "analyze";
		case CaptureDisposition::Record: return "record";
		case CaptureDisposition::AnalyzeAndRecord: return "analyze_and_record";
		default: return "record";
	}
}

const char* captureAnalysisKindName(CaptureAnalysisKind kind) {
	switch (kind) {
		case CaptureAnalysisKind::Group: return "group";
		case CaptureAnalysisKind::Comparison: return "comparison";
		default: return "none";
	}
}

RecordingEngine::RecordingEngine(AnalysisEngine* analysisEngine)
	: analysisEngine_(analysisEngine),
	  worker_(&RecordingEngine::runWorker, this) {}

RecordingEngine::~RecordingEngine() {
	stopping_.store(true, std::memory_order_release);
	workerCv_.notify_one();
	if (worker_.joinable()) worker_.join();
}

bool RecordingEngine::busy() const noexcept {
	return active_.load(std::memory_order_acquire) != nullptr
		|| pendingWorkId_.load(std::memory_order_acquire) != 0;
}

bool RecordingEngine::arm(uint8_t requestedMask, double durationSeconds,
		float sampleRate, const std::string& label,
		const std::string& outputDirectory, RecordingStatus* result,
		std::string* error) {
	ControlProgram noControl;
	return armControlled(requestedMask, durationSeconds, sampleRate, label,
		outputDirectory, noControl, result, error);
}

bool RecordingEngine::armControlled(uint8_t requestedMask, double durationSeconds,
		float sampleRate, const std::string& label,
		const std::string& outputDirectory, const ControlProgram& controlProgram,
		RecordingStatus* result, std::string* error) {
	CaptureAnalysisRequest noAnalysis;
	return armCaptureControlled(requestedMask, durationSeconds, sampleRate, label,
		CaptureDisposition::Record, noAnalysis, outputDirectory, controlProgram,
		result, error);
}

bool RecordingEngine::armCapture(uint8_t requestedMask, double durationSeconds,
		float sampleRate, const std::string& label,
		CaptureDisposition disposition,
		const CaptureAnalysisRequest& analysisRequest,
		const std::string& outputDirectory, RecordingStatus* result,
		std::string* error) {
	ControlProgram noControl;
	return armCaptureControlled(requestedMask, durationSeconds, sampleRate, label,
		disposition, analysisRequest, outputDirectory, noControl, result, error);
}

bool RecordingEngine::armCaptureControlled(uint8_t requestedMask,
		double durationSeconds, float sampleRate, const std::string& label,
		CaptureDisposition disposition,
		const CaptureAnalysisRequest& analysisRequest,
		const std::string& outputDirectory, const ControlProgram& controlProgram,
		RecordingStatus* result, std::string* error) {
	const bool analyze = disposition == CaptureDisposition::Analyze
		|| disposition == CaptureDisposition::AnalyzeAndRecord;
	const bool save = disposition == CaptureDisposition::Record
		|| disposition == CaptureDisposition::AnalyzeAndRecord;
	auto groupMask = [](const AnalysisGroup& group) {
		return static_cast<uint8_t>(observeChannelBit(group.first)
			| (group.stereo ? observeChannelBit(group.second) : 0));
	};
	uint8_t analysisMask = 0;
	if (analysisRequest.kind == CaptureAnalysisKind::Group)
		analysisMask = groupMask(analysisRequest.group);
	else if (analysisRequest.kind == CaptureAnalysisKind::Comparison)
		analysisMask = groupMask(analysisRequest.reference) | groupMask(analysisRequest.target);
	if (requestedMask == 0 || (requestedMask & ~uint8_t(0x3f)) != 0
			|| !std::isfinite(durationSeconds)
			|| durationSeconds < RECORDING_MIN_SECONDS
			|| durationSeconds > RECORDING_MAX_SECONDS
			|| !std::isfinite(sampleRate) || sampleRate <= 0.f
			|| label.size() > 256 || (save && outputDirectory.empty())
			|| (analyze && (analysisRequest.kind == CaptureAnalysisKind::None
				|| !analysisEngine_ || (requestedMask & analysisMask) != analysisMask))
			|| (!analyze && analysisRequest.kind != CaptureAnalysisKind::None)) {
		if (error) *error = "invalid_recording_request";
		return false;
	}
	if (busy()) {
		if (error) *error = "recording_busy";
		return false;
	}

	std::shared_ptr<Session> session(new Session());
	session->requestedMask = requestedMask;
	session->disposition = disposition;
	session->analysisRequest = analysisRequest;
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel) {
		if (requestedMask & (1u << channel))
			session->channelOrder[session->channelCount++] =
				static_cast<ObserveChannel>(channel);
	}
	const double frames = std::round(durationSeconds * sampleRate);
	if (frames < 1.0 || frames > static_cast<double>(std::numeric_limits<uint32_t>::max())) {
		if (error) *error = "invalid_recording_duration";
		return false;
	}
	session->targetFrames = static_cast<uint64_t>(frames);
	uint8_t requestedControlMask = 0;
	if (controlProgram.enabled) {
		if (controlProgram.eventCount > CONTROL_MAX_EVENTS) {
			if (error) *error = "invalid_control_program";
			return false;
		}
		for (size_t port = 0; port < CONTROL_PORTS; ++port) {
			if (controlProgram.channels[port] > CONTROL_CHANNELS) {
				if (error) *error = "invalid_control_program";
				return false;
			}
			if (controlProgram.channels[port]) requestedControlMask |= uint8_t(1u << port);
			for (size_t channel = 0; channel < controlProgram.channels[port]; ++channel) {
				const float voltage = controlProgram.staticVolts[port][channel];
				if (!std::isfinite(voltage) || voltage < -10.f || voltage > 10.f) {
					if (error) *error = "invalid_control_program";
					return false;
				}
			}
		}
		for (size_t index = 0; index < controlProgram.eventCount; ++index) {
			const ControlEvent& event = controlProgram.events[index];
			if (event.port >= CONTROL_PORTS || event.channel >= CONTROL_CHANNELS
					|| event.durationFrames == 0
					|| event.offsetFrames >= session->targetFrames
					|| event.durationFrames > session->targetFrames - event.offsetFrames
					|| !std::isfinite(event.voltage)
					|| event.voltage < -10.f || event.voltage > 10.f) {
				if (error) *error = "invalid_control_program";
				return false;
			}
			for (size_t priorIndex = 0; priorIndex < index; ++priorIndex) {
				const ControlEvent& prior = controlProgram.events[priorIndex];
				const bool overlaps = event.port == prior.port && event.channel == prior.channel
					&& event.offsetFrames < prior.offsetFrames + prior.durationFrames
					&& prior.offsetFrames < event.offsetFrames + event.durationFrames;
				if (overlaps) {
					if (error) *error = "overlapping_control_events";
					return false;
				}
			}
			requestedControlMask |= uint8_t(1u << event.port);
		}
	}
	if (session->targetFrames > std::numeric_limits<size_t>::max() / sizeof(float)) {
		if (error) *error = "recording_too_large";
		return false;
	}
	try {
		for (size_t channel = 0; channel < session->channelCount; ++channel) {
			const size_t observed = static_cast<size_t>(session->channelOrder[channel]);
			session->observation.samples[observed].resize(
				static_cast<size_t>(session->targetFrames));
		}
	} catch (...) {
		if (error) *error = "recording_allocation_failed";
		return false;
	}
	session->sampleRate = sampleRate;
	session->label = label;
	session->observation.requestedMask = requestedMask;
	session->observation.sampleRate = sampleRate;
	session->observation.label = label;
	session->allConnectedMask.store(requestedMask, std::memory_order_relaxed);
	session->controlProgram = controlProgram;
	session->currentControlVolts = controlProgram.staticVolts;
	for (size_t index = 0; index < controlProgram.eventCount; ++index) {
		const ControlEvent& event = controlProgram.events[index];
		Session::ControlTransition& begin =
			session->controlTransitions[session->controlTransitionCount++];
		begin.offsetFrames = event.offsetFrames;
		begin.port = event.port;
		begin.channel = event.channel;
		begin.priority = 1;
		begin.voltage = event.voltage;
		Session::ControlTransition& end =
			session->controlTransitions[session->controlTransitionCount++];
		end.offsetFrames = event.offsetFrames + event.durationFrames;
		end.port = event.port;
		end.channel = event.channel;
		end.priority = 0;
		end.voltage = controlProgram.staticVolts[event.port][event.channel];
	}
	std::stable_sort(session->controlTransitions.begin(),
		session->controlTransitions.begin() + session->controlTransitionCount,
		[](const Session::ControlTransition& a, const Session::ControlTransition& b) {
			return a.offsetFrames < b.offsetFrames
				|| (a.offsetFrames == b.offsetFrames && a.priority < b.priority);
		});
	session->requestedControlMask = requestedControlMask;
	session->allControlConnectedMask.store(requestedControlMask,
		std::memory_order_relaxed);

	std::lock_guard<std::mutex> lock(mutex_);
	if (busy()) {
		if (error) *error = "recording_busy";
		return false;
	}
	session->id = nextId_++;
	session->observation.id = session->id;
	if (save) {
		const auto epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
		const std::string stem = "octavia-" + std::to_string(epochMs) + "-"
			+ std::to_string(session->id) + "-" + safeFileLabel(label);
		session->wavPath = joinPath(outputDirectory, stem + ".wav");
		session->metadataPath = joinPath(outputDirectory, stem + ".json");
	}
	while (sessions_.size() >= RECORDING_STATUS_LIMIT) sessions_.pop_front();
	Session* published = session.get();
	sessions_.push_back(session);
	pendingWorkId_.store(published->id, std::memory_order_release);
	active_.store(published, std::memory_order_release);
	fillStatus(*published, result);
	workerCv_.notify_one();
	return true;
}

void RecordingEngine::process(uint64_t frame, float sampleRate,
		const std::array<float, OBSERVATION_CHANNELS>& volts,
		uint8_t connectedMask) noexcept {
	process(frame, sampleRate, volts, connectedMask, 0, nullptr);
}

void RecordingEngine::process(uint64_t frame, float sampleRate,
		const std::array<float, OBSERVATION_CHANNELS>& volts,
		uint8_t connectedMask, uint8_t controlConnectedMask,
		ControlOutputFrame* controlOutput) noexcept {
	if (controlOutput) *controlOutput = ControlOutputFrame{};
	Session* session = active_.load(std::memory_order_acquire);
	if (!session) return;
	RecordingState state = session->state.load(std::memory_order_relaxed);
	if (state == RecordingState::Armed) {
		uint64_t controlStart = session->controlStartFrame.load(std::memory_order_relaxed);
		if (!session->controlStarted.exchange(true, std::memory_order_relaxed)) {
			controlStart = frame;
			session->controlStartFrame.store(frame, std::memory_order_relaxed);
		}
		const uint64_t captureStart = controlStart + session->controlProgram.settleFrames;
		if (frame >= captureStart) {
			session->startFrame.store(frame, std::memory_order_relaxed);
			session->observation.triggerFrame = frame;
			session->observation.startFrame = frame;
			session->state.store(RecordingState::Capturing, std::memory_order_relaxed);
			state = RecordingState::Capturing;
		}
	} else if (state != RecordingState::Capturing) return;

	if (session->controlProgram.enabled && controlOutput) {
		controlOutput->channels = session->controlProgram.channels;
		const uint64_t captureStart = session->startFrame.load(std::memory_order_relaxed);
		if (captureStart && frame >= captureStart) {
			const uint64_t offset = frame - captureStart;
			while (session->nextControlTransition < session->controlTransitionCount
					&& session->controlTransitions[session->nextControlTransition].offsetFrames
						<= offset) {
				const Session::ControlTransition& transition =
					session->controlTransitions[session->nextControlTransition++];
				session->currentControlVolts[transition.port][transition.channel] =
					transition.voltage;
			}
		}
		controlOutput->volts = session->currentControlVolts;
	}
	if (session->requestedControlMask) {
		session->allControlConnectedMask.store(
			session->allControlConnectedMask.load(std::memory_order_relaxed)
				& controlConnectedMask & session->requestedControlMask,
			std::memory_order_relaxed);
			session->anyControlConnectedMask.store(
			session->anyControlConnectedMask.load(std::memory_order_relaxed)
				| (controlConnectedMask & session->requestedControlMask),
			std::memory_order_relaxed);
	}
	if (std::fabs(sampleRate - session->sampleRate) > 0.5f) {
		session->endFrame.store(frame, std::memory_order_relaxed);
		session->failure.store(RecordingFailure::SampleRateChanged, std::memory_order_relaxed);
		session->state.store(RecordingState::Failed, std::memory_order_release);
		active_.store(nullptr, std::memory_order_release);
		return;
	}
	if (state == RecordingState::Armed) return;

	const uint64_t written = session->writtenFrames.load(std::memory_order_relaxed);
	if (written >= session->targetFrames) return;
	for (size_t channel = 0; channel < session->channelCount; ++channel) {
		const size_t observed = static_cast<size_t>(session->channelOrder[channel]);
		session->observation.samples[observed][static_cast<size_t>(written)] = volts[observed];
	}
	session->allConnectedMask.store(
		session->allConnectedMask.load(std::memory_order_relaxed)
			& connectedMask & session->requestedMask,
		std::memory_order_relaxed);
	session->anyConnectedMask.store(
		session->anyConnectedMask.load(std::memory_order_relaxed)
			| (connectedMask & session->requestedMask),
		std::memory_order_relaxed);
	session->writtenFrames.store(written + 1, std::memory_order_release);
	if (written + 1 == session->targetFrames) {
		session->endFrame.store(frame, std::memory_order_relaxed);
		session->observation.endFrame = frame;
		session->observation.allConnectedMask =
			session->allConnectedMask.load(std::memory_order_relaxed);
		session->observation.anyConnectedMask =
			session->anyConnectedMask.load(std::memory_order_relaxed);
		session->state.store(RecordingState::Captured, std::memory_order_release);
		active_.store(nullptr, std::memory_order_release);
	}
}

std::shared_ptr<RecordingEngine::Session> RecordingEngine::find(uint64_t id) const {
	std::lock_guard<std::mutex> lock(mutex_);
	for (const auto& session : sessions_)
		if (session->id == id) return session;
	return {};
}

void RecordingEngine::fillStatus(const Session& session, RecordingStatus* result) {
	if (!result) return;
	result->id = session.id;
	result->state = session.state.load(std::memory_order_acquire);
	result->disposition = session.disposition;
	result->analysisKind = session.analysisRequest.kind;
	result->requestedMask = session.requestedMask;
	result->allConnectedMask = session.allConnectedMask.load(std::memory_order_relaxed);
	result->anyConnectedMask = session.anyConnectedMask.load(std::memory_order_relaxed);
	result->channelOrder = session.channelOrder;
	result->channelCount = session.channelCount;
	result->targetFrames = session.targetFrames;
	result->writtenFrames = session.writtenFrames.load(std::memory_order_acquire);
	result->startFrame = session.startFrame.load(std::memory_order_relaxed);
	result->endFrame = session.endFrame.load(std::memory_order_relaxed);
	result->controlStartFrame = session.controlStartFrame.load(std::memory_order_relaxed);
	result->sampleRate = session.sampleRate;
	result->label = session.label;
	result->wavPath = session.wavPath;
	result->metadataPath = session.metadataPath;
	result->analysisAvailable = result->state == RecordingState::Complete
		&& session.analysisRequest.kind != CaptureAnalysisKind::None;
	result->includeSpectrum = session.analysisRequest.includeSpectrum;
	result->controlProgram = session.controlProgram;
	result->allControlConnectedMask = session.allControlConnectedMask.load(
		std::memory_order_relaxed);
	result->anyControlConnectedMask = session.anyControlConnectedMask.load(
		std::memory_order_relaxed);
	if (result->analysisAvailable) {
		result->groupAnalysis = session.groupAnalysis;
		result->comparisonAnalysis = session.comparisonAnalysis;
	}
	result->error = session.detailError.empty()
		? failureText(session.failure.load(std::memory_order_relaxed))
		: session.detailError;
}

bool RecordingEngine::get(uint64_t id, RecordingStatus* result) const {
	std::shared_ptr<Session> session = find(id);
	if (!session) return false;
	fillStatus(*session, result);
	return true;
}

bool RecordingEngine::writeFiles(Session& session, std::string* error) {
	const uint64_t sampleValues = session.targetFrames * session.channelCount;
	const uint64_t dataBytes64 = sampleValues * sizeof(float);
	if (dataBytes64 > std::numeric_limits<uint32_t>::max()) {
		if (error) *error = "recording_too_large_for_wav";
		return false;
	}
	const uint32_t dataBytes = static_cast<uint32_t>(dataBytes64);
	const uint16_t blockAlign = static_cast<uint16_t>(session.channelCount * sizeof(float));
	const uint32_t sampleRate = static_cast<uint32_t>(std::lround(session.sampleRate));
	std::ofstream wav(session.wavPath.c_str(), std::ios::binary | std::ios::trunc);
	if (!wav) {
		if (error) *error = "could_not_open_recording_wav";
		return false;
	}
	wav.write("RIFF", 4); writeLe32(wav, 36u + dataBytes); wav.write("WAVE", 4);
	wav.write("fmt ", 4); writeLe32(wav, 16); writeLe16(wav, 3);
	writeLe16(wav, static_cast<uint16_t>(session.channelCount));
	writeLe32(wav, sampleRate); writeLe32(wav, sampleRate * blockAlign);
	writeLe16(wav, blockAlign); writeLe16(wav, 32);
	wav.write("data", 4); writeLe32(wav, dataBytes);
	const size_t chunkFrames = 4096;
	std::vector<float> interleaved(chunkFrames * session.channelCount);
	for (uint64_t offset = 0; offset < session.targetFrames; offset += chunkFrames) {
		const size_t frames = static_cast<size_t>(std::min<uint64_t>(
			chunkFrames, session.targetFrames - offset));
		for (size_t frame = 0; frame < frames; ++frame) {
			for (size_t channel = 0; channel < session.channelCount; ++channel) {
				const size_t observed = static_cast<size_t>(session.channelOrder[channel]);
				interleaved[frame * session.channelCount + channel] =
					session.observation.samples[observed][static_cast<size_t>(offset) + frame];
			}
		}
		wav.write(reinterpret_cast<const char*>(interleaved.data()),
			static_cast<std::streamsize>(frames * session.channelCount * sizeof(float)));
	}
	wav.close();
	if (!wav) {
		if (error) *error = "could_not_write_recording_wav";
		return false;
	}

	std::ofstream metadata(session.metadataPath.c_str(), std::ios::out | std::ios::trunc);
	if (!metadata) {
		std::remove(session.wavPath.c_str());
		if (error) *error = "could_not_open_recording_metadata";
		return false;
	}
	metadata << "{\n  \"recordingId\": " << session.id
		<< ",\n  \"label\": " << jsonString(session.label)
		<< ",\n  \"sampleRate\": " << session.sampleRate
		<< ",\n  \"sampleFormat\": \"ieee_float32_rack_volts\""
		<< ",\n  \"startFrame\": " << session.startFrame.load(std::memory_order_relaxed)
		<< ",\n  \"endFrame\": " << session.endFrame.load(std::memory_order_relaxed)
		<< ",\n  \"frames\": " << session.targetFrames
		<< ",\n  \"allConnectedMask\": "
		<< static_cast<unsigned>(session.allConnectedMask.load(std::memory_order_relaxed))
		<< ",\n  \"anyConnectedMask\": "
		<< static_cast<unsigned>(session.anyConnectedMask.load(std::memory_order_relaxed))
		<< ",\n  \"channels\": [";
	for (size_t channel = 0; channel < session.channelCount; ++channel) {
		if (channel) metadata << ", ";
		metadata << jsonString(observeChannelName(session.channelOrder[channel]));
	}
	metadata << "],\n  \"control\": ";
	if (!session.controlProgram.enabled) {
		metadata << "null\n";
	} else {
		metadata << "{\n    \"settleFrames\": " << session.controlProgram.settleFrames
			<< ",\n    \"controlStartFrame\": "
			<< session.controlStartFrame.load(std::memory_order_relaxed)
			<< ",\n    \"captureStartFrame\": "
			<< session.startFrame.load(std::memory_order_relaxed)
			<< ",\n    \"allConnectedMask\": "
			<< static_cast<unsigned>(session.allControlConnectedMask.load(
				std::memory_order_relaxed))
			<< ",\n    \"anyConnectedMask\": "
			<< static_cast<unsigned>(session.anyControlConnectedMask.load(
				std::memory_order_relaxed))
			<< ",\n    \"ports\": {";
		for (size_t port = 0; port < CONTROL_PORTS; ++port) {
			if (port) metadata << ",";
			metadata << "\n      " << jsonString(port == 0 ? "A" : "B") << ": [";
			for (size_t channel = 0; channel < session.controlProgram.channels[port];
					++channel) {
				if (channel) metadata << ", ";
				metadata << session.controlProgram.staticVolts[port][channel];
			}
			metadata << "]";
		}
		metadata << "\n    },\n    \"events\": [";
		const uint64_t captureStart = session.startFrame.load(std::memory_order_relaxed);
		for (size_t index = 0; index < session.controlProgram.eventCount; ++index) {
			const ControlEvent& event = session.controlProgram.events[index];
			if (index) metadata << ",";
			metadata << "\n      {\"port\": " << jsonString(event.port == 0 ? "A" : "B")
				<< ", \"channel\": " << static_cast<unsigned>(event.channel)
				<< ", \"offsetFrames\": " << event.offsetFrames
				<< ", \"durationFrames\": " << event.durationFrames
				<< ", \"voltage\": " << event.voltage
				<< ", \"executedFrame\": " << captureStart + event.offsetFrames << "}";
		}
		if (session.controlProgram.eventCount) metadata << "\n    ";
		metadata << "]\n  }\n";
	}
	metadata << "}\n";
	metadata.close();
	if (!metadata) {
		std::remove(session.wavPath.c_str());
		std::remove(session.metadataPath.c_str());
		if (error) *error = "could_not_write_recording_metadata";
		return false;
	}
	return true;
}

bool RecordingEngine::processIfReady(uint64_t id, std::string* error) {
	std::shared_ptr<Session> session = find(id);
	if (!session) {
		if (error) *error = "recording_not_found";
		return false;
	}
	RecordingState expected = RecordingState::Captured;
	if (session->state.compare_exchange_strong(expected, RecordingState::Processing,
			std::memory_order_acq_rel, std::memory_order_acquire)) {
		const bool analyze = session->disposition == CaptureDisposition::Analyze
			|| session->disposition == CaptureDisposition::AnalyzeAndRecord;
		const bool save = session->disposition == CaptureDisposition::Record
			|| session->disposition == CaptureDisposition::AnalyzeAndRecord;
		if (analyze) {
			std::string analysisError;
			bool analyzed = false;
			for (int attempt = 0; attempt < 2500 && !stopping_.load(std::memory_order_acquire);
					++attempt) {
				if (session->analysisRequest.kind == CaptureAnalysisKind::Group) {
					analyzed = analysisEngine_->tryAnalyze(session->observation,
						session->analysisRequest.group, session->analysisRequest.detailed,
						session->analysisRequest.includeSpectrum, &session->groupAnalysis,
						&analysisError);
				} else {
					analyzed = analysisEngine_->tryCompare(session->observation,
						session->analysisRequest.reference, session->analysisRequest.target,
						session->analysisRequest.detailed,
						session->analysisRequest.includeSpectrum,
						&session->comparisonAnalysis, &analysisError);
				}
				if (analyzed || analysisError != "analysis_busy") break;
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}
			if (!analyzed) {
				session->detailError = analysisError.empty()
					? "capture_analysis_cancelled" : analysisError;
				session->failure.store(RecordingFailure::AnalysisFailed,
					std::memory_order_relaxed);
				session->state.store(RecordingState::Failed, std::memory_order_release);
				if (error) *error = session->detailError;
				return false;
			}
		}
		if (save) {
			std::string writeError;
			if (!writeFiles(*session, &writeError)) {
				session->detailError = writeError;
				session->failure.store(RecordingFailure::WriteFailed,
					std::memory_order_relaxed);
				session->state.store(RecordingState::Failed, std::memory_order_release);
				if (error) *error = writeError;
				return false;
			}
		}
		for (auto& samples : session->observation.samples) {
			samples.clear();
			samples.shrink_to_fit();
		}
		session->state.store(RecordingState::Complete, std::memory_order_release);
	} else if (expected != RecordingState::Complete) {
		if (error) *error = expected == RecordingState::Failed
			? failureText(session->failure.load(std::memory_order_relaxed))
			: "recording_not_ready";
		return false;
	}
	return true;
}

void RecordingEngine::runWorker() {
	std::unique_lock<std::mutex> workerLock(workerMutex_);
	while (!stopping_.load(std::memory_order_acquire)) {
		workerCv_.wait(workerLock, [this] {
			return stopping_.load(std::memory_order_acquire)
				|| pendingWorkId_.load(std::memory_order_acquire) != 0;
		});
		if (stopping_.load(std::memory_order_acquire)) break;
		const uint64_t id = pendingWorkId_.exchange(0, std::memory_order_acq_rel);
		if (!id) continue;
		std::shared_ptr<Session> session = find(id);
		workerLock.unlock();
		while (!stopping_.load(std::memory_order_acquire) && session) {
			const RecordingState state = session->state.load(std::memory_order_acquire);
			if (state != RecordingState::Armed && state != RecordingState::Capturing) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
		if (!stopping_.load(std::memory_order_acquire) && session) {
			std::string ignoredError;
			processIfReady(id, &ignoredError);
		}
		workerLock.lock();
	}
}

} // namespace octavia
