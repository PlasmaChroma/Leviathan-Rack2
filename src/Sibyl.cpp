#include "plugin.hpp"
#include "SibylControl.hpp"
#include "SibylAdoption.hpp"
#include "SibylClockEstimator.hpp"
#include "SibylEdit.hpp"
#include "SibylHardwareControl.hpp"
#include "SibylJSON.hpp"
#include "SibylTiming.hpp"
#include "SibylTransport.hpp"
#include "OctaviaObservationBus.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include <jansson.h>
#include <osdialog.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace rack;

struct SibylModule : Module, SibylControl {
	static constexpr std::streamoff kMaxPortableCompositionBytes = 16 * 1024 * 1024;
	enum ParamIds {
		SCENE_TRIG_BUTTON_PARAM, RUN_BUTTON_PARAM, RESET_BUTTON_PARAM, LOOP_BUTTON_PARAM, NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT, RUN_INPUT, RESET_INPUT, SCENE_TRIG_INPUT, SCENE_CV_INPUT,
		MACRO_1_INPUT, MACRO_2_INPUT, MACRO_3_INPUT, MACRO_4_INPUT, NUM_INPUTS
	};
	enum OutputIds {
		V_OCT_OUTPUT, GATE_OUTPUT, VELOCITY_OUTPUT, MOD_OUTPUT,
		CLOCK_OUTPUT, SCENE_OUTPUT, EOC_OUTPUT, MOD_2_OUTPUT, MOD_3_OUTPUT, NUM_OUTPUTS
	};
	enum LightIds { NUM_LIGHTS };

	// Rack patch cables and parameter automation persist numeric IDs. These values are
	// frozen for Sibyl's first public schema-v2 release; append future IDs, never reorder.
	static_assert(SCENE_TRIG_BUTTON_PARAM == 0 && RUN_BUTTON_PARAM == 1 &&
		RESET_BUTTON_PARAM == 2 && LOOP_BUTTON_PARAM == 3 && NUM_PARAMS == 4,
		"Sibyl parameter IDs are compatibility-frozen");
	static_assert(CLOCK_INPUT == 0 && RUN_INPUT == 1 && RESET_INPUT == 2 &&
		SCENE_TRIG_INPUT == 3 && SCENE_CV_INPUT == 4 && MACRO_1_INPUT == 5 &&
		MACRO_2_INPUT == 6 && MACRO_3_INPUT == 7 && MACRO_4_INPUT == 8 && NUM_INPUTS == 9,
		"Sibyl input IDs are compatibility-frozen");
	static_assert(V_OCT_OUTPUT == 0 && GATE_OUTPUT == 1 && VELOCITY_OUTPUT == 2 &&
		MOD_OUTPUT == 3 && CLOCK_OUTPUT == 4 && SCENE_OUTPUT == 5 && EOC_OUTPUT == 6 &&
		MOD_2_OUTPUT == 7 && MOD_3_OUTPUT == 8 && NUM_OUTPUTS == 9,
		"Sibyl output IDs are compatibility-frozen");
	static_assert(NUM_LIGHTS == 0, "Sibyl light IDs are compatibility-frozen");

	int m_acceptedRevision = 0;
	const sibyl::Composition* m_acceptedCompositionPtr = nullptr;
	bool m_seenAuthoritativeLoad = false;
	std::string m_lastError;
	std::vector<sibyl::ValidationIssue> m_lastWarnings;
	std::vector<std::shared_ptr<const sibyl::Composition>> m_compositionOwners;
	std::atomic<const sibyl::Composition*> m_activeCompositionPtr{nullptr};
	std::atomic<const sibyl::Composition*> m_compositionHazard{nullptr};
	std::atomic<const sibyl::Composition*> m_displayCompositionHazard{nullptr};
	std::atomic<int> m_activeRevision{0};
	std::vector<std::unique_ptr<const sibyl::AdoptionRequest>> m_adoptionOwners;
	std::atomic<const sibyl::AdoptionRequest*> m_pendingAdoptionPtr{nullptr};
	std::atomic<const sibyl::AdoptionRequest*> m_adoptionHazard{nullptr};
	std::vector<std::unique_ptr<const sibyl::TransportRequest>> m_transportOwners;
	std::atomic<const sibyl::TransportRequest*> m_pendingTransportPtr{nullptr};
	std::atomic<const sibyl::TransportRequest*> m_transportHazard{nullptr};
	std::atomic<bool> m_runtimeRunning{true};
	std::atomic<bool> m_effectiveRunning{true};
	// -1 follows the composition; 0/1 is an explicit persistent panel override.
	std::atomic<int> m_loopOverride{-1};
	bool m_previousEffectiveRunning = true;
	uint64_t m_randomnessEpoch = 0;

	enum class HardwareAction { NONE, RESET_ARRANGEMENT, SELECT_SCENE };
	HardwareAction m_pendingHardwareAction = HardwareAction::NONE;
	int m_pendingHardwareScene = 0;
	sibyl::ApplyAt m_pendingHardwareApplyAt = sibyl::ApplyAt::NEXT_BEAT;

	// Realtime playback state
	double m_globalPhaseBeats = 0.0;
	double m_clockBoundaryPhaseBeats = 0.0;
	double m_outputClockPhaseBeats = 0.0;
	int m_sceneIndex = 0;
	int m_sceneRepeat = 0;
	double m_scenePhase = 0.0;

	// A sequence-guarded set of atomics gives the control/UI thread one coherent
	// telemetry view without reading mutable DSP state or making the audio thread
	// allocate. Odd sequence values mean that publication is in progress.
	std::atomic<uint64_t> m_telemetrySequence{0};
	std::atomic<int> m_telemetrySceneIndex{0};
	std::atomic<int> m_telemetrySceneRepeat{0};
	std::atomic<double> m_telemetryScenePhase{0.0};
	std::atomic<uint16_t> m_telemetryGateMask{0};
	std::atomic<bool> m_telemetryExternalClock{false};
	std::atomic<double> m_telemetryEstimatedBpm{120.0};
	std::array<std::atomic<double>, 16> m_telemetryTrackPhase;
	int m_telemetryPublishCountdown = 0;

	struct TelemetrySnapshot {
		int sceneIndex = 0;
		int sceneRepeat = 0;
		double scenePhase = 0.0;
		uint16_t gateMask = 0;
		bool externalClock = false;
		double estimatedBpm = 120.0;
		std::array<double, 16> trackPhase {};
	};

	struct DisplaySnapshot {
		static constexpr int kMaxProgressSegments = 256;
		std::string title;
		std::string prompt;
		std::string scene;
		std::string sceneDescription;
		std::string error;
		std::array<float, 16> playhead {};
		std::array<float, kMaxProgressSegments> progressSegmentEnds {};
		uint16_t activeTrackMask = 0;
		uint16_t gateMask = 0;
		int sceneRepeat = 0;
		int sceneRepeats = 1;
		int sceneIndex = 0;
		int sceneCount = 0;
		int acceptedRevision = 0;
		int activeRevision = 0;
		int pendingRevision = -1;
		int warningCount = 0;
		int progressSegmentCount = 0;
		float sceneProgress = 0.f;
		float arrangementProgress = 0.f;
		float bpm = 120.f;
		bool running = true;
		bool looping = true;
		bool loopFollowsComposition = true;
		bool externalClock = false;
	};

	struct TrackState {
		double patternPhaseBeats = 0.0;
		int lastFiredStep = -1;
		float currentPitch = 0.0f;
		float targetPitch = 0.0f;
		float glideRatePerSample = 0.0f;
		float currentGate = 0.0f;
		float currentVel = 0.0f;
		float currentMod[3] {};
		int activeEventStep = -1;
		int64_t activeNominalStep = 0;
		double activeEventOnsetBeats = 0.0;
		float activeEventGate = 0.5f;
		int activeEventRatchets = 1;
		bool activeEventTie = false;
		bool activeEventPlayed = false;
	};
	TrackState m_trackStates[16];

	// Hardware Schmitt triggers & pulse generators
	dsp::SchmittTrigger m_clockTrigger;
	dsp::SchmittTrigger m_resetTrigger;
	dsp::SchmittTrigger m_resetButtonTrigger;
	dsp::SchmittTrigger m_sceneTrigger;
	dsp::SchmittTrigger m_sceneButtonTrigger;
	dsp::SchmittTrigger m_runButtonTrigger;
	dsp::SchmittTrigger m_loopButtonTrigger;
	dsp::PulseGenerator m_clockPulse;
	dsp::PulseGenerator m_scenePulse;
	dsp::PulseGenerator m_eocPulse;

	// External clock prediction remains edge-anchored and allocation-free.
	sibyl::ExternalClockEstimator m_externalClockEstimator;

	SibylModule() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configButton(SCENE_TRIG_BUTTON_PARAM, "Trigger next scene");
		configButton(RUN_BUTTON_PARAM, "Run / Pause");
		configButton(RESET_BUTTON_PARAM, "Reset arrangement");
		configButton(LOOP_BUTTON_PARAM, "Cycle loop mode: Auto / Loop / Once");

		configInput(CLOCK_INPUT, "Clock");
		configInput(RUN_INPUT, "Run");
		configInput(RESET_INPUT, "Reset");
		configInput(SCENE_TRIG_INPUT, "Scene Trigger");
		configInput(SCENE_CV_INPUT, "Scene CV (0–10 V)");
		configInput(MACRO_1_INPUT, "Macro 1 (0–10 V)");
		configInput(MACRO_2_INPUT, "Macro 2 (0–10 V)");
		configInput(MACRO_3_INPUT, "Macro 3 (0–10 V)");
		configInput(MACRO_4_INPUT, "Macro 4 (0–10 V)");

		configOutput(V_OCT_OUTPUT, "1 V/oct Pitch (Polyphonic)");
		configOutput(GATE_OUTPUT, "Gate (Polyphonic)");
		configOutput(VELOCITY_OUTPUT, "Velocity (0–10 V Polyphonic)");
		configOutput(MOD_OUTPUT, "Modulation 1 (Polyphonic)");
		configOutput(CLOCK_OUTPUT, "Reconstructed Clock");
		configOutput(SCENE_OUTPUT, "Scene Transition Trigger");
		configOutput(EOC_OUTPUT, "End of Cycle Trigger");
		configOutput(MOD_2_OUTPUT, "Modulation 2 (Polyphonic)");
		configOutput(MOD_3_OUTPUT, "Modulation 3 (Polyphonic)");

		auto comp = std::make_shared<sibyl::Composition>();
		m_compositionOwners.push_back(comp);
		m_acceptedCompositionPtr = comp.get();
		m_activeCompositionPtr.store(comp.get(), std::memory_order_release);
		for (auto& phase : m_telemetryTrackPhase) phase.store(0.0, std::memory_order_relaxed);
	}

	// Allocation-free insertion point for future composition/event analysis
	// markers. Publication is broadcast and Octavia resolves its own module ID.
	uint64_t publishObservationTrigger(int64_t octaviaModuleId, uint64_t triggerFrame,
			uint32_t preFrames, uint32_t postFrames, uint8_t monitorMask,
			const char* label) noexcept {
		if (octaviaModuleId < 0 || monitorMask == 0 || (monitorMask & ~uint8_t(0x3f)))
			return 0;
		octavia::ObservationTrigger trigger;
		trigger.octaviaModuleId = octaviaModuleId;
		trigger.triggerFrame = triggerFrame;
		trigger.preFrames = preFrames;
		trigger.postFrames = postFrames;
		trigger.monitorMask = monitorMask;
		if (label) {
			size_t index = 0;
			for (; index + 1 < trigger.label.size() && label[index]; ++index)
				trigger.label[index] = label[index];
			trigger.label[index] = '\0';
		}
		return octavia::observationBus().publish(trigger);
	}

	void reclaimPublishedObjects() {
		const sibyl::Composition* active = m_activeCompositionPtr.load(std::memory_order_acquire);
		const sibyl::Composition* hazard = m_compositionHazard.load(std::memory_order_acquire);
		const sibyl::Composition* displayHazard = m_displayCompositionHazard.load(std::memory_order_acquire);
		const sibyl::AdoptionRequest* pendingAdoption = m_pendingAdoptionPtr.load(std::memory_order_acquire);
		m_compositionOwners.erase(std::remove_if(m_compositionOwners.begin(), m_compositionOwners.end(),
			[&](const std::shared_ptr<const sibyl::Composition>& owner) {
			const sibyl::Composition* ptr = owner.get();
			return ptr != m_acceptedCompositionPtr && ptr != active && ptr != hazard && ptr != displayHazard &&
				(!pendingAdoption || pendingAdoption->composition != ptr);
		}), m_compositionOwners.end());

		const sibyl::AdoptionRequest* adoptionHazard = m_adoptionHazard.load(std::memory_order_acquire);
		m_adoptionOwners.erase(std::remove_if(m_adoptionOwners.begin(), m_adoptionOwners.end(),
			[&](const std::unique_ptr<const sibyl::AdoptionRequest>& owner) {
				return owner.get() != pendingAdoption && owner.get() != adoptionHazard;
			}),
			m_adoptionOwners.end());

		const sibyl::TransportRequest* pendingTransport = m_pendingTransportPtr.load(std::memory_order_acquire);
		const sibyl::TransportRequest* transportHazard = m_transportHazard.load(std::memory_order_acquire);
		m_transportOwners.erase(std::remove_if(m_transportOwners.begin(), m_transportOwners.end(),
			[&](const std::unique_ptr<const sibyl::TransportRequest>& owner) {
				return owner.get() != pendingTransport && owner.get() != transportHazard;
			}),
			m_transportOwners.end());
	}

	template <typename T>
	const T* acquirePublished(const std::atomic<const T*>& source, std::atomic<const T*>& hazard) {
		const T* value = nullptr;
		do {
			value = source.load(std::memory_order_acquire);
			hazard.store(value, std::memory_order_release);
		} while (value != source.load(std::memory_order_acquire));
		return value;
	}

	void publishTelemetry(bool externalClock, double estimatedBpm) {
		m_telemetrySequence.fetch_add(1, std::memory_order_acq_rel);
		uint16_t gateMask = 0;
		for (int channel = 0; channel < 16; ++channel)
			if (m_trackStates[channel].currentGate > 0.0f) gateMask |= uint16_t(1u << channel);
		m_telemetrySceneIndex.store(m_sceneIndex, std::memory_order_relaxed);
		m_telemetrySceneRepeat.store(m_sceneRepeat, std::memory_order_relaxed);
		m_telemetryScenePhase.store(m_scenePhase, std::memory_order_relaxed);
		m_telemetryGateMask.store(gateMask, std::memory_order_relaxed);
		m_telemetryExternalClock.store(externalClock, std::memory_order_relaxed);
		m_telemetryEstimatedBpm.store(estimatedBpm, std::memory_order_relaxed);
		for (int channel = 0; channel < 16; ++channel)
			m_telemetryTrackPhase[channel].store(m_trackStates[channel].patternPhaseBeats, std::memory_order_relaxed);
		m_telemetrySequence.fetch_add(1, std::memory_order_release);
	}

	void maybePublishTelemetry(float sampleRate, bool externalClock,
			int externalPpqn, float fallbackBpm) {
		if (m_telemetryPublishCountdown > 0) {
			--m_telemetryPublishCountdown;
			return;
		}
		const float safeSampleRate = std::max(1.f, sampleRate);
		const int publishPeriod = std::max(1, static_cast<int>(safeSampleRate / 60.f));
		m_telemetryPublishCountdown = publishPeriod - 1;
		const double estimatedBpm = externalClock
			? m_externalClockEstimator.estimatedBpm(externalPpqn, fallbackBpm)
			: fallbackBpm;
		publishTelemetry(externalClock, estimatedBpm);
	}

	TelemetrySnapshot readTelemetry() const {
		TelemetrySnapshot snapshot;
		for (;;) {
			uint64_t before = m_telemetrySequence.load(std::memory_order_acquire);
			if (before & 1u) continue;
			snapshot.sceneIndex = m_telemetrySceneIndex.load(std::memory_order_relaxed);
			snapshot.sceneRepeat = m_telemetrySceneRepeat.load(std::memory_order_relaxed);
			snapshot.scenePhase = m_telemetryScenePhase.load(std::memory_order_relaxed);
			snapshot.gateMask = m_telemetryGateMask.load(std::memory_order_relaxed);
			snapshot.externalClock = m_telemetryExternalClock.load(std::memory_order_relaxed);
			snapshot.estimatedBpm = m_telemetryEstimatedBpm.load(std::memory_order_relaxed);
			for (int channel = 0; channel < 16; ++channel)
				snapshot.trackPhase[channel] = m_telemetryTrackPhase[channel].load(std::memory_order_relaxed);
			if (before == m_telemetrySequence.load(std::memory_order_acquire)) return snapshot;
		}
	}

	DisplaySnapshot readDisplaySnapshot() {
		DisplaySnapshot display;
		const TelemetrySnapshot telemetry = readTelemetry();
		display.gateMask = telemetry.gateMask;
		display.externalClock = telemetry.externalClock;
		display.bpm = static_cast<float>(telemetry.estimatedBpm);
		display.running = m_effectiveRunning.load(std::memory_order_acquire);
		display.acceptedRevision = m_acceptedRevision;
		display.activeRevision = m_activeRevision.load(std::memory_order_acquire);
		display.warningCount = static_cast<int>(m_lastWarnings.size());
		display.error = m_lastError;
		const sibyl::AdoptionRequest* pending = m_pendingAdoptionPtr.load(std::memory_order_acquire);
		if (pending && pending->composition && pending->composition->revision != display.activeRevision)
			display.pendingRevision = pending->composition->revision;

		const sibyl::Composition* composition = acquirePublished(m_activeCompositionPtr, m_displayCompositionHazard);
		if (composition) {
			const int loopOverride = m_loopOverride.load(std::memory_order_acquire);
			display.looping = loopOverride >= 0 ? loopOverride != 0 : composition->transport.loop;
			display.loopFollowsComposition = loopOverride < 0;
			display.title = composition->meta.title;
			display.prompt = composition->meta.prompt;
			double arrangementBeats = 0.0;
			for (const sibyl::Scene& arrangedScene : composition->arrangement) {
				arrangementBeats += std::max(0.0, static_cast<double>(arrangedScene.lengthBeats))
					* std::max(1, arrangedScene.repeats);
			}
			if (arrangementBeats > 0.0) {
				double cumulativeBeats = 0.0;
				display.progressSegmentCount = std::min(
					static_cast<int>(composition->arrangement.size()),
					DisplaySnapshot::kMaxProgressSegments);
				for (int index = 0; index < display.progressSegmentCount; ++index) {
					const sibyl::Scene& arrangedScene = composition->arrangement[index];
					cumulativeBeats += std::max(0.0, static_cast<double>(arrangedScene.lengthBeats))
						* std::max(1, arrangedScene.repeats);
					display.progressSegmentEnds[index] = clamp(
						static_cast<float>(cumulativeBeats / arrangementBeats), 0.f, 1.f);
				}
			}
			if (telemetry.sceneIndex >= 0 && telemetry.sceneIndex < static_cast<int>(composition->arrangement.size())) {
				const sibyl::Scene& scene = composition->arrangement[telemetry.sceneIndex];
				display.sceneIndex = telemetry.sceneIndex;
				display.sceneCount = static_cast<int>(composition->arrangement.size());
				display.scene = scene.name.empty() ? scene.id : scene.name;
				display.sceneDescription = scene.description;
				display.sceneRepeat = telemetry.sceneRepeat;
				display.sceneRepeats = std::max(1, scene.repeats);
				display.sceneProgress = scene.lengthBeats > 0.f
					? clamp(static_cast<float>(telemetry.scenePhase / scene.lengthBeats), 0.f, 1.f) : 0.f;
				if (arrangementBeats > 0.0) {
					double completedBeats = 0.0;
					for (int index = 0; index < telemetry.sceneIndex; ++index) {
						const sibyl::Scene& priorScene = composition->arrangement[index];
						completedBeats += std::max(0.0, static_cast<double>(priorScene.lengthBeats))
							* std::max(1, priorScene.repeats);
					}
					const int repeat = clamp(telemetry.sceneRepeat, 0, std::max(1, scene.repeats) - 1);
					completedBeats += static_cast<double>(scene.lengthBeats)
						* (repeat + display.sceneProgress);
					display.arrangementProgress = clamp(
						static_cast<float>(completedBeats / arrangementBeats), 0.f, 1.f);
				}
				for (const sibyl::TrackDef& track : composition->tracks) {
					if (track.channel < 0 || track.channel >= 16) continue;
					auto assignment = scene.tracks.find(track.id);
					if (assignment == scene.tracks.end() || assignment->second.patternId.empty()) continue;
					auto pattern = composition->patterns.find(assignment->second.patternId);
					if (pattern == composition->patterns.end()) continue;
					display.activeTrackMask |= uint16_t(1u << track.channel);
					const double duration = pattern->second.length * pattern->second.resolutionBeats;
					if (duration > 0.0) {
						double phase = std::fmod(telemetry.trackPhase[track.channel], duration);
						if (phase < 0.0) phase += duration;
						display.playhead[track.channel] = static_cast<float>(phase / duration);
					}
				}
			}
		}
		m_displayCompositionHazard.store(nullptr, std::memory_order_release);
		return display;
	}

	void acceptComposition(const sibyl::CompositionPtr& composition, sibyl::ApplyAt applyAt,
			sibyl::PhasePolicy phasePolicy, const std::vector<sibyl::ValidationIssue>& warnings = {}) {
		reclaimPublishedObjects();
		const sibyl::Composition* sounding = m_activeCompositionPtr.load(std::memory_order_acquire);
		m_compositionOwners.push_back(composition);
		std::unique_ptr<sibyl::AdoptionRequest> request(new sibyl::AdoptionRequest());
		request->composition = composition.get();
		request->applyAt = applyAt;
		request->phasePolicy = phasePolicy;
		request->restartChannelMask = sounding
			? sibyl::changedTrackChannelMask(*sounding, *composition) : uint16_t(0xffffu);
		const sibyl::AdoptionRequest* requestPtr = request.get();
		m_adoptionOwners.emplace_back(request.release());
		m_acceptedCompositionPtr = composition.get();
		m_acceptedRevision = composition->revision;
		m_lastError.clear();
		m_lastWarnings = warnings;
		m_pendingAdoptionPtr.store(requestPtr, std::memory_order_release);
	}

	bool saveCompositionToPath(const std::string& path, std::string* errorOut) const {
		if (errorOut) errorOut->clear();
		if (!m_acceptedCompositionPtr) {
			if (errorOut) *errorOut = "Sibyl has no composition to save.";
			return false;
		}

		json_error_t jsonError {};
		const std::string serialized = sibyl::serializeFullCompositionJson(*m_acceptedCompositionPtr);
		json_t* full = json_loads(serialized.c_str(), 0, &jsonError);
		json_t* composition = full ? json_object_get(full, "composition") : nullptr;
		if (!composition || !json_is_object(composition)) {
			if (full) json_decref(full);
			if (errorOut) *errorOut = "Could not serialize the current Sibyl composition.";
			return false;
		}

		json_t* envelope = json_object();
		json_object_set_new(envelope, "format", json_string("Leviathan.SibylComposition"));
		json_object_set_new(envelope, "schemaVersion", json_integer(2));
		json_object_set(envelope, "composition", composition);
		char* pretty = json_dumps(envelope, JSON_INDENT(2));
		json_decref(envelope);
		json_decref(full);
		if (!pretty) {
			if (errorOut) *errorOut = "Could not encode the Sibyl composition as JSON.";
			return false;
		}

		std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
		if (!output) {
			free(pretty);
			if (errorOut) *errorOut = "Could not open the selected file for writing.";
			return false;
		}
		output.write(pretty, static_cast<std::streamsize>(std::strlen(pretty)));
		output.put('\n');
		free(pretty);
		if (!output.good()) {
			if (errorOut) *errorOut = "Writing the Sibyl composition did not complete.";
			return false;
		}
		return true;
	}

	bool loadCompositionFromPath(const std::string& path, std::string* errorOut) {
		if (errorOut) errorOut->clear();
		std::ifstream input(path.c_str(), std::ios::binary | std::ios::ate);
		if (!input) {
			if (errorOut) *errorOut = "Could not open the selected Sibyl composition.";
			return false;
		}
		const std::streamoff size = input.tellg();
		if (size < 0 || size > kMaxPortableCompositionBytes) {
			if (errorOut) *errorOut = "Sibyl composition files must be 16 MB or smaller.";
			return false;
		}
		input.seekg(0, std::ios::beg);
		std::string contents(static_cast<size_t>(size), '\0');
		if (size > 0) input.read(&contents[0], static_cast<std::streamsize>(size));
		if (!input.good() && !input.eof()) {
			if (errorOut) *errorOut = "Could not finish reading the Sibyl composition.";
			return false;
		}

		json_error_t jsonError {};
		json_t* root = json_loads(contents.c_str(), 0, &jsonError);
		if (!root || !json_is_object(root)) {
			if (root) json_decref(root);
			const std::string message = string::f("Invalid JSON at line %d: %s", jsonError.line, jsonError.text);
			m_lastError = message;
			if (errorOut) *errorOut = message;
			return false;
		}
		json_t* formatJ = json_object_get(root, "format");
		const bool hasEnvelope = formatJ != nullptr;
		if (hasEnvelope && (!json_is_string(formatJ) ||
			std::string(json_string_value(formatJ)) != "Leviathan.SibylComposition")) {
			json_decref(root);
			m_lastError = "This JSON file is not a supported Sibyl composition.";
			if (errorOut) *errorOut = m_lastError;
			return false;
		}
		json_t* schemaJ = json_object_get(root, "schemaVersion");
		if (hasEnvelope && (!json_is_integer(schemaJ) || json_integer_value(schemaJ) != 2)) {
			json_decref(root);
			m_lastError = "This Sibyl composition uses an unsupported schema version.";
			if (errorOut) *errorOut = m_lastError;
			return false;
		}

		json_t* composition = json_object_get(root, "composition");
		if (!composition && !hasEnvelope) composition = root;
		if (!json_is_object(composition)) {
			json_decref(root);
			m_lastError = "The Sibyl file's composition field must be a JSON object.";
			if (errorOut) *errorOut = m_lastError;
			return false;
		}
		char* compact = json_dumps(composition, JSON_COMPACT);
		json_decref(root);
		if (!compact) {
			m_lastError = "Could not decode the Sibyl composition.";
			if (errorOut) *errorOut = m_lastError;
			return false;
		}

		const int revision = m_acceptedRevision + 1;
		sibyl::ParseResult parsed = sibyl::parseCompositionJson(compact, revision);
		free(compact);
		if (!parsed.valid || !parsed.composition) {
			m_lastError = !parsed.errors.empty()
				? parsed.errors.front().path + ": " + parsed.errors.front().message
				: "Composition validation failed.";
			if (errorOut) *errorOut = m_lastError;
			return false;
		}

		m_runtimeRunning.store(parsed.composition->transport.running, std::memory_order_release);
		acceptComposition(parsed.composition, sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL, parsed.warnings);
		return true;
	}

	bool crossesActiveStepBoundary(const sibyl::Composition& composition, double beatDelta) const {
		if (beatDelta <= 0.0 || m_sceneIndex < 0 || m_sceneIndex >= (int)composition.arrangement.size()) return false;
		const auto& scene = composition.arrangement[m_sceneIndex];
		for (const auto& track : composition.tracks) {
			if (track.channel < 0 || track.channel >= 16) continue;
			auto assignment = scene.tracks.find(track.id);
			if (assignment == scene.tracks.end() || assignment->second.patternId.empty()) continue;
			auto pattern = composition.patterns.find(assignment->second.patternId);
			if (pattern == composition.patterns.end() || pattern->second.resolutionBeats <= 0.0) continue;
			double before = m_trackStates[track.channel].patternPhaseBeats / pattern->second.resolutionBeats;
			double after = (m_trackStates[track.channel].patternPhaseBeats + beatDelta) / pattern->second.resolutionBeats;
			if (std::floor(before) != std::floor(after)) return true;
		}
		return false;
	}

	void adoptPendingIfReady(const sibyl::BoundaryState& boundary, const sibyl::AdoptionRequest* request) {
		if (!request || !request->composition || !sibyl::adoptionBoundaryReached(request->applyAt, boundary)) return;
		const sibyl::Composition& replacement = *request->composition;
		const sibyl::Scene* destinationScene = m_sceneIndex >= 0 && m_sceneIndex < (int)replacement.arrangement.size()
			? &replacement.arrangement[m_sceneIndex] : nullptr;
		for (int channel = 0; channel < 16; ++channel) {
			bool changed = (request->restartChannelMask & (1u << channel)) != 0;
			sibyl::ChannelAdoptionAction action = sibyl::channelAdoptionAction(request->phasePolicy, changed);
			if (action.closeGate) m_trackStates[channel].currentGate = 0.0f;
			if (action.closeGate) m_trackStates[channel].activeEventPlayed = false;
			if (action.cancelGlide) {
				m_trackStates[channel].targetPitch = m_trackStates[channel].currentPitch;
				m_trackStates[channel].glideRatePerSample = 0.0f;
			}
			if (action.restartPhase) {
				m_trackStates[channel].patternPhaseBeats = 0.0;
				m_trackStates[channel].lastFiredStep = -1;
			} else if (changed && destinationScene) {
				for (const auto& track : replacement.tracks) {
					if (track.channel != channel) continue;
					auto assignment = destinationScene->tracks.find(track.id);
					if (assignment == destinationScene->tracks.end()) break;
					auto pattern = replacement.patterns.find(assignment->second.patternId);
					if (pattern == replacement.patterns.end()) break;
					double duration = pattern->second.length * pattern->second.resolutionBeats;
					m_trackStates[channel].patternPhaseBeats = sibyl::preservedPatternPhase(
						m_trackStates[channel].patternPhaseBeats, duration);
					break;
				}
			}
		}
		m_activeCompositionPtr.store(request->composition, std::memory_order_release);
		m_activeRevision.store(request->composition->revision, std::memory_order_release);
		const sibyl::AdoptionRequest* expected = request;
		m_pendingAdoptionPtr.compare_exchange_strong(expected, nullptr, std::memory_order_acq_rel);
		if (m_sceneIndex >= (int)request->composition->arrangement.size()) {
			m_sceneIndex = 0;
			m_sceneRepeat = 0;
			m_scenePhase = 0.0;
		}
	}

	void closeAllGates() {
		for (auto& state : m_trackStates) {
			state.currentGate = 0.0f;
			state.activeEventPlayed = false;
		}
	}

	void realignOutputClock(bool emitPulse) {
		m_outputClockPhaseBeats = 0.0;
		m_clockPulse.reset();
		if (emitPulse) m_clockPulse.trigger(1e-3f);
	}

	void applyPatternPhase(sibyl::PhaseMode mode) {
		if (mode == sibyl::PhaseMode::CONTINUE) return;
		for (auto& state : m_trackStates) {
			state.patternPhaseBeats = mode == sibyl::PhaseMode::ALIGN_GLOBAL ? m_globalPhaseBeats : 0.0;
			state.lastFiredStep = -1;
		}
	}

	void applyDestinationScenePhases(const sibyl::Composition& composition, int sceneIndex,
			const sibyl::TransportRequest* request = nullptr) {
		if (sceneIndex < 0 || sceneIndex >= (int)composition.arrangement.size()) return;
		const auto& scene = composition.arrangement[sceneIndex];
		for (const auto& track : composition.tracks) {
			if (track.channel < 0 || track.channel >= 16 || !scene.tracks.count(track.id)) continue;
			sibyl::PhaseMode mode = request && request->hasPhaseModeOverride
				? request->phaseMode : sibyl::destinationTrackPhaseMode(scene, track.id);
			if (mode == sibyl::PhaseMode::CONTINUE) continue;
			auto& state = m_trackStates[track.channel];
			state.patternPhaseBeats = mode == sibyl::PhaseMode::ALIGN_GLOBAL ? m_globalPhaseBeats : 0.0;
			state.lastFiredStep = -1;
		}
	}

	void enterScene(const sibyl::Composition& composition, int sceneIndex,
			const sibyl::TransportRequest& request) {
		if (composition.arrangement.empty()) return;
		m_sceneIndex = std::max(0, std::min(sceneIndex, (int)composition.arrangement.size() - 1));
		m_sceneRepeat = 0;
		m_scenePhase = 0.0;
		closeAllGates();
		applyDestinationScenePhases(composition, m_sceneIndex, &request);
		m_scenePulse.trigger(1e-3f);
	}

	void applyPendingHardwareIfReady(const sibyl::BoundaryState& boundary,
			const sibyl::Composition& composition, bool externalClockConnected, bool clockTick) {
		if (m_pendingHardwareAction == HardwareAction::NONE) return;
		bool ready = m_pendingHardwareAction == HardwareAction::RESET_ARRANGEMENT
			? sibyl::hardwareResetBoundaryReached(externalClockConnected, clockTick, boundary)
			: sibyl::hardwareSceneBoundaryReached(m_pendingHardwareApplyAt, boundary);
		if (!ready) return;

		sibyl::TransportRequest request;
		if (m_pendingHardwareAction == HardwareAction::RESET_ARRANGEMENT) {
			enterScene(composition, 0, request);
			m_randomnessEpoch = 0;
			realignOutputClock(true);
		} else {
			enterScene(composition, m_pendingHardwareScene, request);
		}
		m_pendingHardwareAction = HardwareAction::NONE;
	}

	void applyPendingTransportIfReady(const sibyl::BoundaryState& boundary,
			const sibyl::Composition& composition, const sibyl::TransportRequest* request) {
		if (!request || !sibyl::adoptionBoundaryReached(request->applyAt, boundary)) return;
		switch (request->action) {
			case sibyl::TransportAction::PLAY:
				m_runtimeRunning.store(true, std::memory_order_release);
				break;
			case sibyl::TransportAction::PAUSE:
				m_runtimeRunning.store(false, std::memory_order_release);
				closeAllGates();
				break;
			case sibyl::TransportAction::STOP:
				m_runtimeRunning.store(false, std::memory_order_release);
				enterScene(composition, 0, *request);
				m_randomnessEpoch = 0;
				realignOutputClock(false);
				break;
			case sibyl::TransportAction::PANIC:
				closeAllGates();
				m_clockPulse.reset();
				m_scenePulse.reset();
				m_eocPulse.reset();
				break;
			case sibyl::TransportAction::RESEED:
				++m_randomnessEpoch;
				break;
			case sibyl::TransportAction::NEXT_SCENE:
				enterScene(composition, (m_sceneIndex + 1) % std::max(1, (int)composition.arrangement.size()), *request);
				break;
			case sibyl::TransportAction::PREVIOUS_SCENE:
				enterScene(composition, (m_sceneIndex - 1 + std::max(1, (int)composition.arrangement.size())) %
					std::max(1, (int)composition.arrangement.size()), *request);
				break;
			case sibyl::TransportAction::SELECT_SCENE: {
				int target = 0;
				for (size_t i = 0; i < composition.arrangement.size(); ++i)
					if (composition.arrangement[i].id == request->sceneId) { target = int(i); break; }
				enterScene(composition, target, *request);
				break;
			}
			case sibyl::TransportAction::RESTART:
				switch (request->target) {
					case sibyl::RestartTarget::SCENE:
						enterScene(composition, m_sceneIndex, *request);
						realignOutputClock(true);
						break;
					case sibyl::RestartTarget::ARRANGEMENT:
						enterScene(composition, 0, *request);
						m_randomnessEpoch = 0;
						realignOutputClock(true);
						break;
					case sibyl::RestartTarget::PATTERNS:
						closeAllGates();
						applyPatternPhase(sibyl::PhaseMode::RESTART);
						realignOutputClock(true);
						break;
					case sibyl::RestartTarget::RANDOMNESS:
						m_randomnessEpoch = 0;
						break;
					case sibyl::RestartTarget::NONE: break;
				}
				break;
		}
		const sibyl::TransportRequest* expected = request;
		m_pendingTransportPtr.compare_exchange_strong(expected, nullptr, std::memory_order_acq_rel);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "schemaVersion", json_integer(2));
		json_object_set_new(rootJ, "revision", json_integer(m_acceptedRevision));
		json_object_set_new(rootJ, "loopOverride",
			json_integer(m_loopOverride.load(std::memory_order_acquire)));

		const sibyl::Composition* comp = m_acceptedCompositionPtr;
		if (comp) {
			std::string fullJson = sibyl::serializeFullCompositionJson(*comp);
			json_error_t err;
			json_t* compWrapper = json_loads(fullJson.c_str(), 0, &err);
			if (compWrapper) {
				json_t* innerComp = json_object_get(compWrapper, "composition");
				if (innerComp) {
					json_object_set(rootJ, "composition", innerComp);
				}
				json_decref(compWrapper);
			}
		}

		json_t* statusJ = json_object();
		json_object_set_new(statusJ, "acceptedRevision", json_integer(m_acceptedRevision));
		if (m_lastError.empty()) json_object_set_new(statusJ, "lastError", json_null());
		else json_object_set_new(statusJ, "lastError", json_string(m_lastError.c_str()));
		json_t* warningsJ = json_array();
		for (const auto& warning : m_lastWarnings) json_array_append_new(warningsJ, json_string(warning.message.c_str()));
		json_object_set_new(statusJ, "warnings", warningsJ);
		json_object_set_new(rootJ, "status", statusJ);

		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		if (!rootJ || !json_is_object(rootJ)) return;
		json_t* loopOverrideJ = json_object_get(rootJ, "loopOverride");
		if (loopOverrideJ && json_is_integer(loopOverrideJ)) {
			const int loopOverride = static_cast<int>(json_integer_value(loopOverrideJ));
			m_loopOverride.store(loopOverride < 0 ? -1 : (loopOverride != 0 ? 1 : 0),
				std::memory_order_release);
		}

		json_t* compJ = json_object_get(rootJ, "composition");
		json_t* revJ = json_object_get(rootJ, "revision");
		int savedRevision = revJ && json_is_integer(revJ) ? json_integer_value(revJ) : 0;
		int revision = (!m_seenAuthoritativeLoad && m_acceptedRevision == 0)
			? std::max(0, savedRevision) : m_acceptedRevision + 1;
		m_seenAuthoritativeLoad = true;

		if (compJ) {
			char* str = json_dumps(compJ, JSON_COMPACT);
			if (str) {
				sibyl::ParseResult res = sibyl::parseCompositionJson(str, revision);
				free(str);
				if (res.valid && res.composition) {
					m_runtimeRunning.store(res.composition->transport.running, std::memory_order_release);
					acceptComposition(res.composition, sibyl::ApplyAt::IMMEDIATE,
						sibyl::PhasePolicy::RESTART_ALL, res.warnings);
				} else {
					m_lastError = !res.errors.empty() ? res.errors[0].message : "Composition validation failed";
				}
			}
		}
	}

	void process(const ProcessArgs& args) override {
		const sibyl::Composition* comp = acquirePublished(m_activeCompositionPtr, m_compositionHazard);
		if (!comp) return;
		if (m_runButtonTrigger.process(params[RUN_BUTTON_PARAM].getValue())) {
			bool running = m_runtimeRunning.load(std::memory_order_acquire);
			m_runtimeRunning.store(!running, std::memory_order_release);
		}
		if (m_loopButtonTrigger.process(params[LOOP_BUTTON_PARAM].getValue())) {
			const int loopOverride = m_loopOverride.load(std::memory_order_acquire);
			// Cycle all three authored/performance states explicitly:
			// follow composition -> force loop -> force once -> follow composition.
			const int nextLoopOverride = loopOverride < 0 ? 1 : (loopOverride > 0 ? 0 : -1);
			m_loopOverride.store(nextLoopOverride, std::memory_order_release);
		}

		// --- Transport Run / Pause ---
		bool isRunning = m_runtimeRunning.load(std::memory_order_acquire);
		if (inputs[RUN_INPUT].isConnected()) {
			isRunning = inputs[RUN_INPUT].getVoltage() >= sibyl::kHardwareSchmittHighVolts;
		}
		if (sibyl::hardwareRunFallingEdge(m_previousEffectiveRunning, isRunning)) closeAllGates();
		m_effectiveRunning.store(isRunning, std::memory_order_release);
		m_previousEffectiveRunning = isRunning;

		// Hardware requests are captured immediately and applied only at their
		// documented musical boundary below.
		const bool resetEdge = m_resetTrigger.process(inputs[RESET_INPUT].getVoltage()) |
			m_resetButtonTrigger.process(10.f * params[RESET_BUTTON_PARAM].getValue());
		if (resetEdge) {
			m_pendingHardwareAction = HardwareAction::RESET_ARRANGEMENT;
		}
		const bool sceneTriggerEdge = m_sceneTrigger.process(inputs[SCENE_TRIG_INPUT].getVoltage()) |
			m_sceneButtonTrigger.process(10.f * params[SCENE_TRIG_BUTTON_PARAM].getValue());
		if (!comp->arrangement.empty() && sceneTriggerEdge &&
				m_pendingHardwareAction != HardwareAction::RESET_ARRANGEMENT) {
			m_pendingHardwareAction = HardwareAction::SELECT_SCENE;
			m_pendingHardwareScene = (m_sceneIndex + 1) % comp->arrangement.size();
			m_pendingHardwareApplyAt = comp->transport.defaultApplyAt;
		}
		if (!comp->arrangement.empty() && inputs[SCENE_CV_INPUT].isConnected() &&
				m_pendingHardwareAction != HardwareAction::RESET_ARRANGEMENT) {
			int referenceScene = m_pendingHardwareAction == HardwareAction::SELECT_SCENE
				? m_pendingHardwareScene : m_sceneIndex;
			int target = sibyl::sceneIndexFromCv(inputs[SCENE_CV_INPUT].getVoltage(),
				(int)comp->arrangement.size(), referenceScene);
			if (target != referenceScene) {
				if (target == m_sceneIndex) {
					m_pendingHardwareAction = HardwareAction::NONE;
				} else {
					m_pendingHardwareAction = HardwareAction::SELECT_SCENE;
					m_pendingHardwareScene = target;
					m_pendingHardwareApplyAt = comp->transport.defaultApplyAt;
				}
			}
		}

		// --- Clock Processing ---
		double rawBeatDelta = 0.0;
		bool clockTick = false;
		if (inputs[CLOCK_INPUT].isConnected()) {
			clockTick = m_clockTrigger.process(inputs[CLOCK_INPUT].getVoltage());
			sibyl::ClockAdvance advance = m_externalClockEstimator.process(args.sampleTime, clockTick,
				comp->clock.externalPpqn, comp->clock.externalTimeoutMs,
				comp->clock.onExternalStop, comp->meta.bpm);
			rawBeatDelta = advance.beatDelta;
		} else {
			double bpm = comp->meta.bpm;
			double beatsPerSecond = bpm / 60.0;
			rawBeatDelta = beatsPerSecond * args.sampleTime;
		}

		// Accepted revisions remain pending until their requested musical boundary.
		// Boundary detection uses the currently sounding composition; adoption then
		// occurs before this sample advances transport or generates events.
		sibyl::BoundaryState adoptionBoundary;
		adoptionBoundary.beat = rawBeatDelta > 0.0 &&
			std::floor(m_clockBoundaryPhaseBeats) != std::floor(m_clockBoundaryPhaseBeats + rawBeatDelta);
		adoptionBoundary.step = crossesActiveStepBoundary(*comp, rawBeatDelta);
		if (rawBeatDelta > 0.0 && m_sceneIndex >= 0 && m_sceneIndex < (int)comp->arrangement.size()) {
			const auto& currentScene = comp->arrangement[m_sceneIndex];
			adoptionBoundary.scene = m_scenePhase + rawBeatDelta >= currentScene.lengthBeats &&
				m_sceneRepeat + 1 >= currentScene.repeats;
		}
		const sibyl::AdoptionRequest* pending = acquirePublished(m_pendingAdoptionPtr, m_adoptionHazard);
		adoptPendingIfReady(adoptionBoundary, pending);
		m_adoptionHazard.store(nullptr, std::memory_order_release);
		m_compositionHazard.store(nullptr, std::memory_order_release);
		comp = acquirePublished(m_activeCompositionPtr, m_compositionHazard);
		if (!comp) return;
		const sibyl::TransportRequest* pendingTransport = acquirePublished(m_pendingTransportPtr, m_transportHazard);
		applyPendingTransportIfReady(adoptionBoundary, *comp, pendingTransport);
		m_transportHazard.store(nullptr, std::memory_order_release);
		applyPendingHardwareIfReady(adoptionBoundary, *comp, inputs[CLOCK_INPUT].isConnected(), clockTick);
		isRunning = m_runtimeRunning.load(std::memory_order_acquire);
		if (inputs[RUN_INPUT].isConnected())
			isRunning = inputs[RUN_INPUT].getVoltage() >= sibyl::kHardwareSchmittHighVolts;
		if (sibyl::hardwareRunFallingEdge(m_previousEffectiveRunning, isRunning)) closeAllGates();
		m_effectiveRunning.store(isRunning, std::memory_order_release);
		m_previousEffectiveRunning = isRunning;
		double beatDelta = isRunning ? rawBeatDelta : 0.0;

		m_clockBoundaryPhaseBeats += rawBeatDelta;
		m_globalPhaseBeats += beatDelta;
		m_outputClockPhaseBeats += beatDelta;

		// Clock output pulse (every beat subdivision)
		if (isRunning) {
			int outPpqn = std::max(1, comp->clock.outputPpqn);
			double previousOutputTick = (m_outputClockPhaseBeats - beatDelta) * outPpqn;
			double currentOutputTick = m_outputClockPhaseBeats * outPpqn;
			if (beatDelta > 0.0 && std::floor(previousOutputTick) != std::floor(currentOutputTick)) {
				m_clockPulse.trigger(1e-3f);
			}
		}

		if (comp->arrangement.empty()) {
			outputs[V_OCT_OUTPUT].setChannels(1);
			outputs[GATE_OUTPUT].setChannels(1);
			outputs[VELOCITY_OUTPUT].setChannels(1);
			outputs[MOD_OUTPUT].setChannels(1);
			outputs[MOD_2_OUTPUT].setChannels(1);
			outputs[MOD_3_OUTPUT].setChannels(1);
			outputs[V_OCT_OUTPUT].setVoltage(0.f, 0);
			outputs[GATE_OUTPUT].setVoltage(0.f, 0);
			outputs[VELOCITY_OUTPUT].setVoltage(0.f, 0);
			outputs[MOD_OUTPUT].setVoltage(0.f, 0);
			outputs[MOD_2_OUTPUT].setVoltage(0.f, 0);
			outputs[MOD_3_OUTPUT].setVoltage(0.f, 0);
			outputs[CLOCK_OUTPUT].setVoltage(m_clockPulse.process(args.sampleTime) ? 10.0f : 0.0f);
			outputs[SCENE_OUTPUT].setVoltage(m_scenePulse.process(args.sampleTime) ? 10.0f : 0.0f);
			outputs[EOC_OUTPUT].setVoltage(m_eocPulse.process(args.sampleTime) ? 10.0f : 0.0f);
			maybePublishTelemetry(args.sampleRate, inputs[CLOCK_INPUT].isConnected(),
				comp->clock.externalPpqn, comp->meta.bpm);
			m_compositionHazard.store(nullptr, std::memory_order_release);
			return;
		}

		if (m_sceneIndex >= (int)comp->arrangement.size()) {
			m_sceneIndex = 0;
			m_sceneRepeat = 0;
			m_scenePhase = 0.0;
		}

		const auto& boundaryScene = comp->arrangement[m_sceneIndex];
		m_scenePhase += beatDelta;

		// --- Scene Boundary Progression ---
		if (m_scenePhase >= boundaryScene.lengthBeats) {
			m_scenePhase -= boundaryScene.lengthBeats;
			m_sceneRepeat++;
			if (m_sceneRepeat >= boundaryScene.repeats) {
				m_sceneRepeat = 0;
				m_sceneIndex++;
				m_scenePulse.trigger(1e-3f);
				bool enteredScene = true;
				if (m_sceneIndex >= (int)comp->arrangement.size()) {
					const int loopOverride = m_loopOverride.load(std::memory_order_acquire);
					const bool looping = loopOverride >= 0 ? loopOverride != 0 : comp->transport.loop;
					if (looping) {
						m_sceneIndex = 0;
						m_eocPulse.trigger(1e-3f);
					} else {
						enteredScene = false;
						m_sceneIndex = (int)comp->arrangement.size() - 1;
						m_scenePhase = comp->arrangement.back().lengthBeats;
						m_runtimeRunning.store(false, std::memory_order_release);
						m_effectiveRunning.store(false, std::memory_order_release);
						isRunning = false;
						beatDelta = 0.0;
						closeAllGates();
					}
				}
				if (enteredScene) {
					closeAllGates();
					applyDestinationScenePhases(*comp, m_sceneIndex);
				}
			}
		}
		// Scene progression above may have changed m_sceneIndex. Event generation
		// must use the destination scene on this same sample so restarted tracks can
		// emit their destination step-zero events without a one-sample stale scene.
		const auto& playbackScene = comp->arrangement[m_sceneIndex];

		// --- Macro Inputs Evaluation (0–10 V) ---
		float globalProbMacro = 0.f;
		float globalVelMacro = 0.f;
		float globalSwingMacro = 0.f;
		float trackProbMacro[16] = {};
		float trackVelMacro[16] = {};
		float trackGateMacro[16] = {};
		float trackSwingMacro[16] = {};
		float trackModMacro[3][16] = {};

		for (int m = 0; m < 4; m++) {
			int inputId = MACRO_1_INPUT + m;
			if (!inputs[inputId].isConnected()) continue;
			std::string macroKey = std::to_string(m + 1);
			auto it = comp->macros.find(macroKey);
			if (it == comp->macros.end()) continue;

			float rawNorm = clamp(inputs[inputId].getVoltage() / 10.0f, 0.0f, 1.0f);
			float contrib = (it->second.polarity == sibyl::MacroPolarity::BIPOLAR)
				? (rawNorm * 2.0f - 1.0f) * it->second.amount
				: rawNorm * it->second.amount;

			const std::string& target = it->second.target;
			if (target == "global.probability") globalProbMacro += contrib;
			else if (target == "global.velocity") globalVelMacro += contrib;
			else if (target == "global.swing") globalSwingMacro += contrib;
			else if (target.rfind("track.", 0) == 0) {
				size_t secondDot = target.rfind('.');
				if (secondDot != std::string::npos && secondDot > 6) {
					std::string trackId = target.substr(6, secondDot - 6);
					std::string param = target.substr(secondDot + 1);
					for (const auto& tr : comp->tracks) {
						if (tr.id == trackId && tr.channel >= 0 && tr.channel < 16) {
							if (param == "probability") trackProbMacro[tr.channel] += contrib;
							else if (param == "velocity") trackVelMacro[tr.channel] += contrib;
							else if (param == "gate") trackGateMacro[tr.channel] += contrib;
							else if (param == "swing") trackSwingMacro[tr.channel] += contrib;
							else if (param == "mod") trackModMacro[0][tr.channel] += contrib;
							else if (param == "mod2") trackModMacro[1][tr.channel] += contrib;
							else if (param == "mod3") trackModMacro[2][tr.channel] += contrib;
						}
					}
				}
			}
		}

		// --- Polyphonic Track Output Evaluation ---
		int maxChannel = -1;
		for (const auto& trackDef : comp->tracks) {
			int ch = trackDef.channel;
			if (ch < 0 || ch > 15) continue;
			maxChannel = std::max(maxChannel, ch);
			if (!isRunning) {
				m_trackStates[ch].currentGate = 0.0f;
				continue;
			}

			auto trackAsgIt = playbackScene.tracks.find(trackDef.id);
			if (trackAsgIt == playbackScene.tracks.end() || trackAsgIt->second.patternId.empty()) {
				m_trackStates[ch].currentGate = 0.0f;
				continue;
			}

			auto patIt = comp->patterns.find(trackAsgIt->second.patternId);
			if (patIt == comp->patterns.end()) continue;
			const auto& pat = patIt->second;

			if (pat.resolutionBeats <= 0) continue;

			double previousPatternPhase = m_trackStates[ch].patternPhaseBeats;
			m_trackStates[ch].patternPhaseBeats += beatDelta;
			double currentPatternPhase = m_trackStates[ch].patternPhaseBeats;
			double effectiveSwing = clamp(comp->meta.swing + globalSwingMacro + trackSwingMacro[ch], 0.0f, 0.49f);

			// Linear Glide interpolation
			if (m_trackStates[ch].glideRatePerSample != 0.0f) {
				float diff = m_trackStates[ch].targetPitch - m_trackStates[ch].currentPitch;
				if (std::abs(diff) <= std::abs(m_trackStates[ch].glideRatePerSample)) {
					m_trackStates[ch].currentPitch = m_trackStates[ch].targetPitch;
					m_trackStates[ch].glideRatePerSample = 0.0f;
				} else {
					m_trackStates[ch].currentPitch += m_trackStates[ch].glideRatePerSample;
				}
			}

			if (beatDelta > 0.0) {
				int64_t firstNominalStep = static_cast<int64_t>(std::floor(previousPatternPhase / pat.resolutionBeats)) - 1;
				int64_t lastNominalStep = static_cast<int64_t>(std::floor(currentPatternPhase / pat.resolutionBeats)) + 1;
				for (int64_t nominalStep = firstNominalStep; nominalStep <= lastNominalStep; ++nominalStep) {
					int eventStep = sibyl::wrappedStep(nominalStep, pat.length);
					const sibyl::StepEvent* matchedEvent = sibyl::eventAtStep(pat, eventStep);
					if (!matchedEvent) continue;
					double scheduledBeat = sibyl::scheduledEventBeat(pat, nominalStep, *matchedEvent, effectiveSwing);
					if (!sibyl::scheduledEventCrossed(previousPatternPhase, currentPatternPhase, scheduledBeat)) continue;

					m_trackStates[ch].lastFiredStep = eventStep;
					m_trackStates[ch].activeEventStep = eventStep;
					m_trackStates[ch].activeNominalStep = nominalStep;
					m_trackStates[ch].activeEventOnsetBeats = scheduledBeat;
					m_trackStates[ch].activeEventGate = matchedEvent->hasGate ? matchedEvent->gate : trackDef.defaultGate;
					m_trackStates[ch].activeEventRatchets = std::max(1, matchedEvent->ratchets);
					m_trackStates[ch].activeEventTie = matchedEvent->tie;

					// Deterministic Probability Check + Macro Modulation
					float baseProb = matchedEvent->hasProbability ? matchedEvent->probability : 1.0f;
					float effProb = clamp(baseProb + globalProbMacro + trackProbMacro[ch], 0.0f, 1.0f);
					uint64_t hash = comp->meta.seed ^ (m_randomnessEpoch * 11400714819323198485ULL) ^
						(m_sceneIndex * 73856093ULL) ^ (ch * 19349663ULL) ^ (eventStep * 83492791ULL);
					float randVal = (float)((hash % 10000ULL) / 10000.0);
					bool play = randVal < effProb;
					m_trackStates[ch].activeEventPlayed = play;

					if (play) {
						if (matchedEvent->hasObservation) {
							publishObservationTrigger(
								matchedEvent->observation.octaviaModuleId,
								static_cast<uint64_t>(args.frame),
								matchedEvent->observation.preFrames,
								matchedEvent->observation.postFrames,
								matchedEvent->observation.monitorMask,
								matchedEvent->observation.label.c_str());
						}
						if (!matchedEvent->tie) {
							m_trackStates[ch].currentGate = 10.0f;
						}
						// Glide or Instant pitch
						if (matchedEvent->glideMs > 0.0f) {
							m_trackStates[ch].targetPitch = matchedEvent->compiledPitchV;
							float numSamples = (matchedEvent->glideMs * 0.001f) * args.sampleRate;
							if (numSamples > 1.0f) {
								m_trackStates[ch].glideRatePerSample = (m_trackStates[ch].targetPitch - m_trackStates[ch].currentPitch) / numSamples;
							} else {
								m_trackStates[ch].currentPitch = matchedEvent->compiledPitchV;
								m_trackStates[ch].glideRatePerSample = 0.0f;
							}
						} else {
							m_trackStates[ch].currentPitch = matchedEvent->compiledPitchV;
							m_trackStates[ch].targetPitch = matchedEvent->compiledPitchV;
							m_trackStates[ch].glideRatePerSample = 0.0f;
						}

						float baseVel = matchedEvent->hasVelocity ? matchedEvent->velocity : trackDef.defaultVelocity;
						float effVel = clamp(baseVel + globalVelMacro + trackVelMacro[ch], 0.0f, 1.0f);
						m_trackStates[ch].currentVel = effVel * 10.0f;
						const float authoredMod[3] {matchedEvent->mod, matchedEvent->mod2, matchedEvent->mod3};
						const bool hasMod[3] {matchedEvent->hasMod, matchedEvent->hasMod2, matchedEvent->hasMod3};
						for (int lane = 0; lane < 3; ++lane) {
							const float authoredVoltage = hasMod[lane] ? authoredMod[lane] : 0.0f;
							m_trackStates[ch].currentMod[lane] = clamp(
								authoredVoltage + trackModMacro[lane][ch], -10.0f, 10.0f);
						}
					} else {
						m_trackStates[ch].currentGate = 0.0f;
					}
				}
			}

			// Gate and ratchet timing are measured from the shifted event onset,
			// rather than from the unshifted integer grid position.
			if (m_trackStates[ch].activeEventStep >= 0 && m_trackStates[ch].activeEventPlayed &&
					!m_trackStates[ch].activeEventTie) {
				double elapsed = currentPatternPhase - m_trackStates[ch].activeEventOnsetBeats;
				int64_t nextNominalStep = m_trackStates[ch].activeNominalStep + 1;
				const sibyl::StepEvent* nextEvent = sibyl::eventAtStep(pat,
					sibyl::wrappedStep(nextNominalStep, pat.length));
				bool awaitingTie = false;
				if (nextEvent && nextEvent->tie) {
					double tieOnset = sibyl::scheduledEventBeat(pat, nextNominalStep, *nextEvent, effectiveSwing);
					awaitingTie = currentPatternPhase <= tieOnset;
				}
				if (awaitingTie) {
					m_trackStates[ch].currentGate = 10.0f;
				} else if (elapsed >= 0.0 && m_trackStates[ch].activeEventRatchets > 1 &&
						elapsed < pat.resolutionBeats) {
					double eventFraction = elapsed / pat.resolutionBeats;
					double sliceFraction = std::fmod(eventFraction * m_trackStates[ch].activeEventRatchets, 1.0);
					float effectiveGate = clamp(m_trackStates[ch].activeEventGate + trackGateMacro[ch], 0.01f, 1.0f);
					m_trackStates[ch].currentGate = sliceFraction <= effectiveGate ? 10.0f : 0.0f;
				} else if (elapsed >= 0.0 && m_trackStates[ch].activeEventRatchets == 1) {
					float effectiveGate = clamp(m_trackStates[ch].activeEventGate + trackGateMacro[ch], 0.01f, 1024.0f);
					m_trackStates[ch].currentGate = elapsed <= pat.resolutionBeats * effectiveGate ? 10.0f : 0.0f;
				} else if (elapsed >= pat.resolutionBeats) {
					m_trackStates[ch].currentGate = 0.0f;
				}
			}
		}

		// Write Polyphonic outputs
		int numChannels = std::max(1, maxChannel + 1);
		outputs[V_OCT_OUTPUT].setChannels(numChannels);
		outputs[GATE_OUTPUT].setChannels(numChannels);
		outputs[VELOCITY_OUTPUT].setChannels(numChannels);
		outputs[MOD_OUTPUT].setChannels(numChannels);
		outputs[MOD_2_OUTPUT].setChannels(numChannels);
		outputs[MOD_3_OUTPUT].setChannels(numChannels);

		for (int c = 0; c < numChannels; c++) {
			outputs[V_OCT_OUTPUT].setVoltage(m_trackStates[c].currentPitch, c);
			outputs[GATE_OUTPUT].setVoltage(m_trackStates[c].currentGate, c);
			outputs[VELOCITY_OUTPUT].setVoltage(m_trackStates[c].currentVel, c);
			outputs[MOD_OUTPUT].setVoltage(m_trackStates[c].currentMod[0], c);
			outputs[MOD_2_OUTPUT].setVoltage(m_trackStates[c].currentMod[1], c);
			outputs[MOD_3_OUTPUT].setVoltage(m_trackStates[c].currentMod[2], c);
		}

		// Write Pulse triggers
		outputs[CLOCK_OUTPUT].setVoltage(m_clockPulse.process(args.sampleTime) ? 10.0f : 0.0f);
		outputs[SCENE_OUTPUT].setVoltage(m_scenePulse.process(args.sampleTime) ? 10.0f : 0.0f);
		outputs[EOC_OUTPUT].setVoltage(m_eocPulse.process(args.sampleTime) ? 10.0f : 0.0f);
		maybePublishTelemetry(args.sampleRate, inputs[CLOCK_INPUT].isConnected(),
			comp->clock.externalPpqn, comp->meta.bpm);
		m_compositionHazard.store(nullptr, std::memory_order_release);
	}

	bool handleSibylRequest(Operation operation, const std::string& requestJson, std::string& responseJson, std::string& error) override {
		if (operation == Operation::CAPABILITIES) {
			responseJson = "{\"ok\":true,\"capabilities\":{\"sibyl\":{\"apiVersion\":1,\"schemaVersion\":2,\"revision\":" + std::to_string(m_acceptedRevision) + ",\"operations\":[\"get_composition\",\"validate\",\"edit\",\"get_status\",\"transport\"]}}}";
			return true;
		} else if (operation == Operation::GET_COMPOSITION) {
			const sibyl::Composition* comp = m_acceptedCompositionPtr;
			if (!comp) {
				error = "No composition loaded";
				return false;
			}
			std::string view = "summary";
			std::string id = "";
			json_error_t jerror;
			json_t* reqJ = json_loads(requestJson.c_str(), 0, &jerror);
			if (reqJ) {
				json_t* viewJ = json_object_get(reqJ, "view");
				if (viewJ && json_is_string(viewJ)) view = json_string_value(viewJ);
				json_t* idJ = json_object_get(reqJ, "id");
				if (idJ && json_is_string(idJ)) id = json_string_value(idJ);
				json_decref(reqJ);
			}
			if (view == "full") {
				responseJson = sibyl::serializeFullCompositionJson(*comp);
			} else if (view == "pattern") {
				responseJson = sibyl::serializePatternViewJson(*comp, id);
			} else if (view == "scene") {
				responseJson = sibyl::serializeSceneViewJson(*comp, id);
			} else {
				responseJson = sibyl::serializeSummaryJson(*comp);
			}
			return true;
		} else if (operation == Operation::GET_STATUS) {
			reclaimPublishedObjects();
			const sibyl::Composition* comp = m_activeCompositionPtr.load(std::memory_order_acquire);
			TelemetrySnapshot telemetry = readTelemetry();
			int rev = m_acceptedRevision;
			int actRev = m_activeRevision.load(std::memory_order_acquire);
			const sibyl::AdoptionRequest* pending = m_pendingAdoptionPtr.load(std::memory_order_acquire);
			const sibyl::TransportRequest* pendingTransport = m_pendingTransportPtr.load(std::memory_order_acquire);
			bool running = m_effectiveRunning.load(std::memory_order_acquire);
			double bpm = telemetry.estimatedBpm;
			std::string sceneId = "";
			int sceneRepeat = telemetry.sceneRepeat;
			double beat = telemetry.scenePhase;
			if (comp && telemetry.sceneIndex >= 0 && telemetry.sceneIndex < (int)comp->arrangement.size()) {
				sceneId = comp->arrangement[telemetry.sceneIndex].id;
			}
			json_t* stJ = json_object();
			json_object_set_new(stJ, "ok", json_true());
			json_object_set_new(stJ, "revision", json_integer(rev));
			json_object_set_new(stJ, "activeRevision", json_integer(actRev));
			if (pending && pending->composition && pending->composition->revision != actRev) {
				json_object_set_new(stJ, "pendingRevision", json_integer(pending->composition->revision));
				json_object_set_new(stJ, "pendingApplyAt", json_string(sibyl::applyAtName(pending->applyAt)));
				json_object_set_new(stJ, "pendingPhasePolicy", json_string(sibyl::phasePolicyName(pending->phasePolicy)));
			} else {
				json_object_set_new(stJ, "pendingRevision", json_null());
				json_object_set_new(stJ, "pendingApplyAt", json_null());
				json_object_set_new(stJ, "pendingPhasePolicy", json_null());
			}
			if (pendingTransport) {
				json_t* transportJ = json_object();
				json_object_set_new(transportJ, "action", json_string(sibyl::transportActionName(pendingTransport->action)));
				if (pendingTransport->target == sibyl::RestartTarget::NONE) json_object_set_new(transportJ, "target", json_null());
				else json_object_set_new(transportJ, "target", json_string(sibyl::restartTargetName(pendingTransport->target)));
				json_object_set_new(transportJ, "applyAt", json_string(sibyl::applyAtName(pendingTransport->applyAt)));
				if (pendingTransport->sceneId.empty()) json_object_set_new(transportJ, "sceneId", json_null());
				else json_object_set_new(transportJ, "sceneId", json_string(pendingTransport->sceneId.c_str()));
				if (pendingTransport->hasPhaseModeOverride) json_object_set_new(transportJ, "phaseMode", json_string(sibyl::phaseModeName(pendingTransport->phaseMode)));
				else json_object_set_new(transportJ, "phaseMode", json_null());
				json_object_set_new(stJ, "pendingTransport", transportJ);
			} else json_object_set_new(stJ, "pendingTransport", json_null());
			json_object_set_new(stJ, "running", json_boolean(running));
			json_object_set_new(stJ, "clockSource", json_string(telemetry.externalClock ? "external" : "internal"));
			json_object_set_new(stJ, "gateMask", json_integer(telemetry.gateMask));
			json_object_set_new(stJ, "estimatedBpm", json_real(bpm));
			if (!sceneId.empty()) {
				json_object_set_new(stJ, "sceneId", json_string(sceneId.c_str()));
			} else {
				json_object_set_new(stJ, "sceneId", json_null());
			}
			json_object_set_new(stJ, "sceneRepeat", json_integer(sceneRepeat));
			json_object_set_new(stJ, "beat", json_real(beat));
			if (m_lastError.empty()) json_object_set_new(stJ, "lastError", json_null());
			else json_object_set_new(stJ, "lastError", json_string(m_lastError.c_str()));
			json_t* warningsJ = json_array();
			for (const auto& warning : m_lastWarnings) json_array_append_new(warningsJ, json_string(warning.message.c_str()));
			json_object_set_new(stJ, "warnings", warningsJ);

			char* dumped = json_dumps(stJ, JSON_COMPACT);
			responseJson = dumped ? dumped : "{}";
			if (dumped) free(dumped);
			json_decref(stJ);
			return true;
		} else if (operation == Operation::VALIDATE) {
			json_error_t jerror;
			json_t* root = json_loads(requestJson.c_str(), 0, &jerror);
			if (!root) {
				error = std::string("Invalid JSON: ") + jerror.text;
				return false;
			}
			json_t* candJ = json_object_get(root, "candidate");
			char* candStr = json_dumps(candJ ? candJ : root, JSON_COMPACT);
			sibyl::ParseResult res = sibyl::parseCompositionJson(candStr ? candStr : "{}", m_acceptedRevision);
			if (candStr) free(candStr);
			json_decref(root);

			json_t* respJ = json_object();
			json_object_set_new(respJ, "ok", json_true());
			json_object_set_new(respJ, "revision", json_integer(m_acceptedRevision));
			json_object_set_new(respJ, "valid", json_boolean(res.valid));
			json_t* errsJ = json_array();
			for (const auto& issue : res.errors) {
				json_t* issueJ = json_object();
				json_object_set_new(issueJ, "path", json_string(issue.path.c_str()));
				json_object_set_new(issueJ, "message", json_string(issue.message.c_str()));
				json_array_append_new(errsJ, issueJ);
			}
			json_object_set_new(respJ, "errors", errsJ);
			json_object_set_new(respJ, "warnings", json_array());
			char* dumped = json_dumps(respJ, JSON_COMPACT);
			responseJson = dumped ? dumped : "{}";
			if (dumped) free(dumped);
			json_decref(respJ);
			return true;
		} else if (operation == Operation::TRANSPORT) {
			reclaimPublishedObjects();
			json_error_t jerror;
			json_t* root = json_loads(requestJson.c_str(), 0, &jerror);
			if (!root) {
				error = std::string("Invalid JSON: ") + jerror.text;
				return false;
			}
			const sibyl::Composition* accepted = m_acceptedCompositionPtr;
			sibyl::ApplyAt defaultApplyAt = accepted ? accepted->transport.defaultApplyAt : sibyl::ApplyAt::NEXT_BEAT;
			sibyl::TransportParseResult parsed = sibyl::parseTransportRequest(root, defaultApplyAt);
			json_decref(root);
			if (!parsed.valid) {
				json_t* respJ = json_object();
				json_object_set_new(respJ, "ok", json_false());
				json_t* errorJ = json_object();
				json_object_set_new(errorJ, "code", json_string(parsed.code.c_str()));
				json_object_set_new(errorJ, "path", json_string(parsed.path.c_str()));
				json_object_set_new(errorJ, "message", json_string(parsed.message.c_str()));
				json_object_set_new(respJ, "error", errorJ);
				char* dumped = json_dumps(respJ, JSON_COMPACT);
				responseJson = dumped ? dumped : "{}";
				if (dumped) free(dumped);
				json_decref(respJ);
				error = parsed.message;
				return false;
			}
			if (parsed.request.action == sibyl::TransportAction::SELECT_SCENE) {
				bool found = false;
				if (accepted) for (const auto& scene : accepted->arrangement) if (scene.id == parsed.request.sceneId) { found = true; break; }
				if (!found) {
					error = "Scene not found: " + parsed.request.sceneId;
				responseJson = "{\"ok\":false,\"error\":{\"code\":\"object_not_found\",\"path\":\"scene_id\",\"message\":\"Scene not found\"}}";
					return false;
				}
			}
			std::unique_ptr<sibyl::TransportRequest> request(new sibyl::TransportRequest(parsed.request));
			const sibyl::TransportRequest* requestPtr = request.get();
			m_transportOwners.emplace_back(request.release());
			m_pendingTransportPtr.store(requestPtr, std::memory_order_release);
			json_t* respJ = json_object();
			json_object_set_new(respJ, "ok", json_true());
			json_object_set_new(respJ, "action", json_string(sibyl::transportActionName(requestPtr->action)));
			if (requestPtr->target == sibyl::RestartTarget::NONE) json_object_set_new(respJ, "target", json_null());
			else json_object_set_new(respJ, "target", json_string(sibyl::restartTargetName(requestPtr->target)));
			json_object_set_new(respJ, "applyAt", json_string(sibyl::applyAtName(requestPtr->applyAt)));
			json_object_set_new(respJ, "pending", json_true());
			if (requestPtr->sceneId.empty()) json_object_set_new(respJ, "pendingScene", json_null());
			else json_object_set_new(respJ, "pendingScene", json_string(requestPtr->sceneId.c_str()));
			if (requestPtr->hasPhaseModeOverride) json_object_set_new(respJ, "phaseMode", json_string(sibyl::phaseModeName(requestPtr->phaseMode)));
			else json_object_set_new(respJ, "phaseMode", json_null());
			char* dumped = json_dumps(respJ, JSON_COMPACT);
			responseJson = dumped ? dumped : "{}";
			if (dumped) free(dumped);
			json_decref(respJ);
			return true;
		} else if (operation == Operation::EDIT) {
			json_error_t jerror;
			json_t* root = json_loads(requestJson.c_str(), 0, &jerror);
			if (!root) {
				error = std::string("Invalid JSON: ") + jerror.text;
				return false;
			}
			json_t* expectedRevJ = json_object_get(root, "expected_revision");
			if (!expectedRevJ || !json_is_integer(expectedRevJ)) {
				json_decref(root);
				error = "Missing or invalid expected_revision";
				return false;
			}
			int expectedRev = json_integer_value(expectedRevJ);
			if (expectedRev != m_acceptedRevision) {
				json_decref(root);
				error = "Revision conflict";
				responseJson = "{\"ok\":false,\"error\":{\"code\":\"revision_conflict\",\"message\":\"Expected revision " + std::to_string(expectedRev) + " but current is " + std::to_string(m_acceptedRevision) + "\"}}";
				return false;
			}

			sibyl::ApplyAt applyAt = sibyl::ApplyAt::NEXT_BEAT;
			json_t* applyAtJ = json_object_get(root, "apply_at");
			if (!applyAtJ) applyAtJ = json_object_get(root, "applyAt");
			if (applyAtJ) {
				if (!json_is_string(applyAtJ) || !sibyl::parseApplyAtName(json_string_value(applyAtJ), applyAt)) {
					json_decref(root);
					error = "Invalid apply_at";
					responseJson = "{\"ok\":false,\"error\":{\"code\":\"invalid_request\",\"path\":\"apply_at\",\"message\":\"Unsupported apply boundary\"}}";
					return false;
				}
			}
			sibyl::PhasePolicy phasePolicy = sibyl::PhasePolicy::PRESERVE;
			json_t* phasePolicyJ = json_object_get(root, "phase_policy");
			if (!phasePolicyJ) phasePolicyJ = json_object_get(root, "phasePolicy");
			if (phasePolicyJ && (!json_is_string(phasePolicyJ) ||
					!sibyl::parsePhasePolicyName(json_string_value(phasePolicyJ), phasePolicy))) {
				json_decref(root);
				error = "Invalid phase_policy";
				responseJson = "{\"ok\":false,\"error\":{\"code\":\"invalid_request\",\"path\":\"phase_policy\",\"message\":\"Unsupported phase policy\"}}";
				return false;
			}

			json_t* opsJ = json_object_get(root, "operations");
			if (!opsJ || !json_is_array(opsJ)) {
				json_decref(root);
				error = "Missing operations array";
				return false;
			}

			if (!m_acceptedCompositionPtr) {
				json_decref(root);
				error = "No composition loaded";
				return false;
			}
			sibyl::EditResult edit = sibyl::applyCompositionEdit(
				*m_acceptedCompositionPtr, opsJ, m_acceptedRevision + 1);
			if (!edit.valid || !edit.composition) {
				json_t* respJ = json_object();
				json_object_set_new(respJ, "ok", json_false());
				json_t* errorJ = json_object();
				json_object_set_new(errorJ, "code", json_string(edit.errorCode.empty() ? "validation_failed" : edit.errorCode.c_str()));
				json_object_set_new(errorJ, "message", json_string(edit.errorMessage.empty() ? "Composition edit failed." : edit.errorMessage.c_str()));
				if (edit.errorPath.empty()) json_object_set_new(errorJ, "path", json_null());
				else json_object_set_new(errorJ, "path", json_string(edit.errorPath.c_str()));
				json_object_set_new(respJ, "error", errorJ);
				char* dumped = json_dumps(respJ, JSON_COMPACT);
				responseJson = dumped ? dumped : "{}";
				if (dumped) free(dumped);
				json_decref(respJ);
				json_decref(root);
				error = edit.errorMessage.empty() ? "Composition edit failed" : edit.errorMessage;
				return false;
			}
			if (!applyAtJ) applyAt = edit.composition->transport.defaultApplyAt;
			acceptComposition(edit.composition, applyAt, phasePolicy, edit.warnings);
			int activeRevision = m_activeRevision.load(std::memory_order_acquire);
			json_t* respJ = json_object();
			json_object_set_new(respJ, "ok", json_true());
			json_object_set_new(respJ, "revision", json_integer(m_acceptedRevision));
			json_object_set_new(respJ, "activeRevision", json_integer(activeRevision));
			json_object_set_new(respJ, "pendingRevision", json_integer(m_acceptedRevision));
			json_object_set_new(respJ, "applyAt", json_string(sibyl::applyAtName(applyAt)));
			json_object_set_new(respJ, "phasePolicy", json_string(sibyl::phasePolicyName(phasePolicy)));
			json_t* warningsJ = json_array();
			for (const auto& warning : edit.warnings) {
				json_t* issueJ = json_object();
				json_object_set_new(issueJ, "path", json_string(warning.path.c_str()));
				json_object_set_new(issueJ, "message", json_string(warning.message.c_str()));
				json_array_append_new(warningsJ, issueJ);
			}
			json_object_set_new(respJ, "warnings", warningsJ);
			char* dumped = json_dumps(respJ, JSON_COMPACT);
			responseJson = dumped ? dumped : "{}";
			if (dumped) free(dumped);
			json_decref(respJ);
			json_decref(root);
			return true;
		}
		error = "Not implemented";
		return false;
	}
};

#ifndef SIBYL_MODULE_TEST
struct SibylOracleDisplay final : TransparentWidget {
	SibylModule* module = nullptr;

	explicit SibylOracleDisplay(SibylModule* module) : module(module) {}

	static void text(const DrawArgs& args, float x, float y, float size, int align,
			NVGcolor color, const std::string& value) {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, size);
		nvgTextAlign(args.vg, align);
		nvgFillColor(args.vg, color);
		nvgText(args.vg, x, y, value.c_str(), nullptr);
	}

	static std::string fittedText(const DrawArgs& args, const std::string& source,
			float fontSize, float maxWidth) {
		if (!APP || !APP->window || !APP->window->uiFont || source.empty()) return source;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, fontSize);
		float bounds[4] {};
		if (nvgTextBounds(args.vg, 0.f, 0.f, source.c_str(), nullptr, bounds) <= maxWidth) return source;
		std::string fitted = source;
		while (!fitted.empty()) {
			fitted.pop_back();
			const std::string candidate = fitted + "...";
			if (nvgTextBounds(args.vg, 0.f, 0.f, candidate.c_str(), nullptr, bounds) <= maxWidth)
				return candidate;
		}
		return "...";
	}

	static float measuredTextWidth(const DrawArgs& args, const std::string& value, float fontSize) {
		if (!APP || !APP->window || !APP->window->uiFont || value.empty()) return 0.f;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, fontSize);
		float bounds[4] {};
		return nvgTextBounds(args.vg, 0.f, 0.f, value.c_str(), nullptr, bounds);
	}

	void draw(const DrawArgs& args) override {
		const float w = box.size.x;
		const float h = box.size.y;
		if (w <= 1.f || h <= 1.f) return;
		SibylModule::DisplaySnapshot state;
		if (module) {
			state = module->readDisplaySnapshot();
		} else {
			state.title = "THE ORACLE AWAKENS";
			state.prompt = "Awaiting a composition from beyond the rack...";
			state.scene = "PREMONITION";
			state.sceneDescription = "A distant pattern gathers around the active scene.";
			state.activeTrackMask = 0x00ffu;
			state.gateMask = 0x0029u;
			state.sceneRepeats = 2;
			state.sceneIndex = 1;
			state.sceneCount = 4;
			state.looping = true;
			state.sceneProgress = 0.37f;
			state.arrangementProgress = 0.34f;
			state.progressSegmentCount = 4;
			state.progressSegmentEnds[0] = 0.18f;
			state.progressSegmentEnds[1] = 0.45f;
			state.progressSegmentEnds[2] = 0.72f;
			state.progressSegmentEnds[3] = 1.f;
			state.acceptedRevision = state.activeRevision = 1;
			for (int i = 0; i < 16; ++i) state.playhead[i] = std::fmod(0.11f * i + 0.18f, 1.f);
		}

		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, w, h);
		NVGpaint glass = nvgLinearGradient(args.vg, 0.f, 0.f, 0.f, h,
			nvgRGBA(5, 14, 25, 255), nvgRGBA(1, 4, 10, 255));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, w, h, 4.f);
		nvgFillPaint(args.vg, glass);
		nvgFill(args.vg);

		// A slow scene-phase aura makes the display read as an instrument rather
		// than a status terminal. It is pure UI work and never feeds the engine.
		const float auraX = w * (0.18f + 0.64f * state.sceneProgress);
		NVGpaint aura = nvgRadialGradient(args.vg, auraX, h * 0.48f, 1.f, w * 0.42f,
			nvgRGBA(51, 224, 232, 42), nvgRGBA(71, 42, 155, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, w, h);
		nvgFillPaint(args.vg, aura);
		nvgFill(args.vg);

		const float pad = 6.f;
		const NVGcolor cyan = nvgRGBA(80, 232, 238, 255);
		const NVGcolor violet = nvgRGBA(167, 139, 250, 255);
		const NVGcolor dim = nvgRGBA(126, 151, 174, 220);
		const NVGcolor white = nvgRGBA(226, 241, 247, 255);
		const NVGcolor red = nvgRGBA(255, 91, 119, 255);

		std::string runLabel = state.running ? "RUN" : "HOLD";
		const float runWidth = measuredTextWidth(args, runLabel, 7.1f);
		text(args, pad, 5.f, 8.8f, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, white,
			fittedText(args, state.title.empty() ? "UNTITLED" : state.title, 8.8f,
				std::max(1.f, w - pad * 2.f - runWidth - 7.f)));
		text(args, w - pad, 5.f, 7.1f, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP,
			state.running ? cyan : red, runLabel);

		const float sceneY = 20.f;
		const std::string scenePosition = state.sceneCount > 0
			? "[" + std::to_string(state.sceneIndex + 1) + "/" + std::to_string(state.sceneCount) + "] "
			: "[0/0] ";
		const std::string sceneLabel = scenePosition + (state.scene.empty() ? "NO SCENE" : state.scene);
		char repeatText[24];
		std::snprintf(repeatText, sizeof(repeatText), "%d/%d", state.sceneRepeat + 1, state.sceneRepeats);
		const float repeatWidth = measuredTextWidth(args, repeatText, 7.2f);
		text(args, pad, sceneY, 10.2f, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, cyan,
			fittedText(args, sceneLabel, 10.2f,
				std::max(1.f, w - pad * 2.f - repeatWidth - 7.f)));
		text(args, w - pad, sceneY + 1.f, 7.2f, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP, violet, repeatText);

		// One duration-weighted segment per scene makes the arrangement readable at
		// a glance. Repeats contribute to their scene's width, while the continuous
		// fill includes completed repeats and the active scene phase.
		const float progressX = pad;
		const float progressY = 33.5f;
		const float progressW = w - pad * 2.f;
		const float progressH = 6.5f;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, progressX, progressY, progressW, progressH, progressH * 0.5f);
		nvgFillColor(args.vg, nvgRGBA(24, 39, 55, 225));
		nvgFill(args.vg);
		const float filledW = progressW * clamp(state.arrangementProgress, 0.f, 1.f);
		if (filledW > 0.f) {
			NVGpaint progressPaint = nvgLinearGradient(args.vg, progressX, progressY,
				progressX + progressW, progressY, violet, cyan);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, progressX, progressY, filledW, progressH, progressH * 0.5f);
			nvgFillPaint(args.vg, progressPaint);
			nvgFill(args.vg);
		}
		float lastDividerX = progressX;
		for (int index = 0; index + 1 < state.progressSegmentCount; ++index) {
			const float dividerX = progressX
				+ progressW * clamp(state.progressSegmentEnds[index], 0.f, 1.f);
			// Dense arrangements retain their proportional fill without collapsing
			// into an opaque picket fence at compact display scale.
			if (dividerX - lastDividerX < 2.f) continue;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, dividerX, progressY + 0.5f);
			nvgLineTo(args.vg, dividerX, progressY + progressH - 0.5f);
			nvgStrokeWidth(args.vg, 0.8f);
			nvgStrokeColor(args.vg, nvgRGBA(3, 9, 17, 210));
			nvgStroke(args.vg);
			lastDividerX = dividerX;
		}

		// Expand the active scene into its own strip. Each equal-width cell is one
		// repeat, so the current repeat and its local phase remain legible even when
		// the arrangement-level scene segment is narrow.
		const float sceneProgressY = progressY + progressH + 4.f;
		const float sceneProgressH = 4.f;
		const int sceneRepeats = std::max(1, state.sceneRepeats);
		const float repeatedSceneProgress = clamp(
			(state.sceneRepeat + clamp(state.sceneProgress, 0.f, 1.f)) / sceneRepeats,
			0.f, 1.f);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, progressX, sceneProgressY, progressW, sceneProgressH,
			sceneProgressH * 0.5f);
		nvgFillColor(args.vg, nvgRGBA(20, 31, 49, 225));
		nvgFill(args.vg);
		const float filledSceneW = progressW * repeatedSceneProgress;
		if (filledSceneW > 0.f) {
			NVGpaint scenePaint = nvgLinearGradient(args.vg, progressX, sceneProgressY,
				progressX + progressW, sceneProgressY, nvgRGBA(125, 95, 224, 255), violet);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, progressX, sceneProgressY, filledSceneW,
				sceneProgressH, sceneProgressH * 0.5f);
			nvgFillPaint(args.vg, scenePaint);
			nvgFill(args.vg);
		}
		if (sceneRepeats > 1) {
			const float repeatW = progressW / sceneRepeats;
			float lastRepeatDividerX = progressX;
			for (int repeat = 1; repeat < sceneRepeats; ++repeat) {
				const float dividerX = progressX + repeatW * repeat;
				if (dividerX - lastRepeatDividerX < 2.f) continue;
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, dividerX, sceneProgressY + 0.5f);
				nvgLineTo(args.vg, dividerX, sceneProgressY + sceneProgressH - 0.5f);
				nvgStrokeWidth(args.vg, 0.8f);
				nvgStrokeColor(args.vg, nvgRGBA(3, 9, 17, 210));
				nvgStroke(args.vg);
				lastRepeatDividerX = dividerX;
			}
		}

		// Give both eight-channel banks equal breathing room. The lower bank used
		// to surrender too much height to the message/revision footer, which was
		// easy to miss in patches that only populated channels 1-8.
		const float gridTop = 54.f;
		const float gridBottom = h - 42.f;
		const float cellW = (w - pad * 2.f) / 8.f;
		const float rowH = std::max(8.f, (gridBottom - gridTop) * 0.5f);
		for (int channel = 0; channel < 16; ++channel) {
			const int column = channel & 7;
			const int row = channel >> 3;
			const float x0 = pad + column * cellW + 2.f;
			const float x1 = pad + (column + 1) * cellW - 3.f;
			const float y = gridTop + (row + 0.5f) * rowH;
			const bool active = (state.activeTrackMask & uint16_t(1u << channel)) != 0;
			const bool gated = (state.gateMask & uint16_t(1u << channel)) != 0;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x0, y);
			nvgLineTo(args.vg, x1, y);
			nvgStrokeWidth(args.vg, gated ? 1.4f : 0.75f);
			nvgStrokeColor(args.vg, active ? nvgRGBA(64, 124, 150, 205) : nvgRGBA(35, 49, 65, 155));
			nvgStroke(args.vg);
			if (active) {
				const float px = x0 + clamp(state.playhead[channel], 0.f, 1.f) * (x1 - x0);
				if (gated) {
					NVGpaint glow = nvgRadialGradient(args.vg, px, y, 0.4f, 5.f,
						nvgRGBA(74, 247, 239, 190), nvgRGBA(74, 247, 239, 0));
					nvgBeginPath(args.vg);
					nvgCircle(args.vg, px, y, 5.f);
					nvgFillPaint(args.vg, glow);
					nvgFill(args.vg);
				}
				nvgBeginPath(args.vg);
				nvgCircle(args.vg, px, y, gated ? 2.2f : 1.25f);
				nvgFillColor(args.vg, gated ? cyan : violet);
				nvgFill(args.vg);
			}
			char channelText[4];
			std::snprintf(channelText, sizeof(channelText), "%d", channel + 1);
			text(args, x0, y - 5.6f, 6.5f, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
				active ? dim : nvgRGBA(55, 66, 80, 160), channelText);
		}

		const float messageTop = h - 43.f;
		const std::string musicalContext = state.sceneDescription.empty() ? state.prompt : state.sceneDescription;
		const std::string message = !state.error.empty() ? "! " + state.error
			: (state.warningCount > 0 ? "! " + std::to_string(state.warningCount) + " WARNING"
			: (musicalContext.empty() ? "THE MACHINE IS LISTENING" : musicalContext));
		if (APP && APP->window && APP->window->uiFont) {
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 7.2f);
			nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			nvgFillColor(args.vg, state.error.empty() ? dim : red);
			NVGtextRow rows[3] {};
			const int rowCount = nvgTextBreakLines(args.vg, message.c_str(), nullptr,
				w - pad * 2.f, rows, 3);
			for (int row = 0; row < rowCount; ++row) {
				const bool hasMore = row == rowCount - 1 && rows[row].next && *rows[row].next != '\0';
				if (hasMore) {
					std::string lastLine(rows[row].start, rows[row].end);
					lastLine = fittedText(args, lastLine + "...", 7.2f, w - pad * 2.f);
					nvgText(args.vg, pad, messageTop + row * 8.2f, lastLine.c_str(), nullptr);
				} else {
					nvgText(args.vg, pad, messageTop + row * 8.2f, rows[row].start, rows[row].end);
				}
			}
		}

		char footer[80];
		const char* loopLabel = state.loopFollowsComposition
			? (state.looping ? "AUTO LOOP" : "AUTO ONCE")
			: (state.looping ? "LOOP" : "ONCE");
		if (state.pendingRevision >= 0) {
			std::snprintf(footer, sizeof(footer), "%s · %s · %5.1f · R%d>%d · P%d",
				loopLabel, state.externalClock ? "EXT" : "INT", state.bpm,
				state.activeRevision, state.acceptedRevision, state.pendingRevision);
		} else {
			std::snprintf(footer, sizeof(footer), "%s · %s · %5.1f BPM · REV %d",
				loopLabel, state.externalClock ? "EXT" : "INT",
				state.bpm, state.activeRevision);
		}
		text(args, w - pad, h - 4.f, 7.0f, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM, violet, footer);

		// Retain a bright inner edge around the Oracle glass.
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.5f, 0.5f, w - 1.f, h - 1.f, 4.f);
		nvgStrokeWidth(args.vg, 1.f);
		nvgStrokeColor(args.vg, nvgRGBA(64, 224, 230, 145));
		nvgStroke(args.vg);
		nvgResetScissor(args.vg);
		nvgRestore(args.vg);
	}
};

struct SibylWidget : ModuleWidget {
	static std::string portableDirectory() {
		return asset::user("Sibyl");
	}

	static std::string portableFilename(const SibylModule* module) {
		std::string title = "sibyl-composition";
		if (module && module->m_acceptedCompositionPtr && !module->m_acceptedCompositionPtr->meta.title.empty()) {
			title = module->m_acceptedCompositionPtr->meta.title;
		}
		for (char& c : title) {
			const bool safe = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') || c == '-' || c == '_';
			if (!safe) c = '-';
		}
		while (!title.empty() && title.back() == '-') title.pop_back();
		if (title.empty()) title = "sibyl-composition";
		return title + ".sibyl.json";
	}

	static std::string ensureJsonExtension(std::string path) {
		std::string suffix = path.size() >= 5 ? path.substr(path.size() - 5) : std::string();
		std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		if (suffix != ".json") path += ".json";
		return path;
	}

	explicit SibylWidget(SibylModule* module) {
		setModule(module);
		PreviewBuildLogTimer previewTimer("Sibyl", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/Sibyl.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/Sibyl.labels.svg");
		splitPanel.addCompactLeviathanLogoBranding();
		visual_assets::addFractalGlassOverlay(this, panelPath, splitPanel.panelSurfaceEffectWidget());

		math::Rect displayMm(Vec(2.4f, 13.f), Vec(46.f, 24.f));
		panel_svg::loadRectFromSvgMm(panelPath, "SIBYL_DISPLAY", &displayMm);
		auto* display = new SibylOracleDisplay(module);
		display->box.pos = mm2px(displayMm.pos);
		display->box.size = mm2px(displayMm.size);
		addChild(display);
		auto anchor = [&](const char* id, Vec fallbackMm) {
			Vec pointMm = fallbackMm;
			panel_svg::loadPointFromSvgMm(panelPath, id, &pointMm);
			return mm2px(pointMm);
		};

		addInput(createInputCentered<Magitek2InputJack>(anchor("CLOCK_INPUT", Vec(8.5f, 69.5f)), module, SibylModule::CLOCK_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("RUN_INPUT", Vec(20.f, 55.f)), module, SibylModule::RUN_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("RESET_INPUT", Vec(31.5f, 55.f)), module, SibylModule::RESET_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("SCENE_TRIG_INPUT", Vec(8.5f, 55.f)), module, SibylModule::SCENE_TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("SCENE_CV_INPUT", Vec(8.5f, 84.f)), module, SibylModule::SCENE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("MACRO_1_INPUT", Vec(8.5f, 98.5f)), module, SibylModule::MACRO_1_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("MACRO_2_INPUT", Vec(20.f, 98.5f)), module, SibylModule::MACRO_2_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("MACRO_3_INPUT", Vec(8.5f, 113.f)), module, SibylModule::MACRO_3_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(anchor("MACRO_4_INPUT", Vec(20.f, 113.f)), module, SibylModule::MACRO_4_INPUT));

		addParam(createParamCentered<SmallGoldButton>(anchor("SCENE_TRIG_BUTTON", Vec(8.5f, 63.5f)), module,
			SibylModule::SCENE_TRIG_BUTTON_PARAM));
		addParam(createParamCentered<SmallGoldButton>(anchor("RUN_BUTTON", Vec(20.f, 63.5f)), module,
			SibylModule::RUN_BUTTON_PARAM));
		addParam(createParamCentered<SmallGoldButton>(anchor("RESET_BUTTON", Vec(31.5f, 63.5f)), module,
			SibylModule::RESET_BUTTON_PARAM));
		addParam(createParamCentered<SmallGoldButton>(anchor("LOOP_BUTTON", Vec(8.5f, 78.f)), module,
			SibylModule::LOOP_BUTTON_PARAM));

		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("V_OCT_OUTPUT", Vec(32.5f, 55.f)), module, SibylModule::V_OCT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("GATE_OUTPUT", Vec(43.f, 55.f)), module, SibylModule::GATE_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("VELOCITY_OUTPUT", Vec(32.5f, 69.5f)), module, SibylModule::VELOCITY_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("MOD_OUTPUT", Vec(32.5f, 69.5f)), module, SibylModule::MOD_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("MOD_2_OUTPUT", Vec(37.75f, 69.5f)), module, SibylModule::MOD_2_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("MOD_3_OUTPUT", Vec(43.f, 69.5f)), module, SibylModule::MOD_3_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("CLOCK_OUTPUT", Vec(32.5f, 84.f)), module, SibylModule::CLOCK_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("SCENE_OUTPUT", Vec(37.75f, 84.f)), module, SibylModule::SCENE_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor("EOC_OUTPUT", Vec(43.f, 84.f)), module, SibylModule::EOC_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* sibylModule = dynamic_cast<SibylModule*>(module);
		if (!sibylModule) return;

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Composition"));
		menu->addChild(createMenuItem("Load composition…", "JSON", [sibylModule]() {
			system::createDirectories(portableDirectory());
			osdialog_filters* filters = osdialog_filters_parse("Sibyl composition:json,JSON");
			char* pathC = osdialog_file(OSDIALOG_OPEN, portableDirectory().c_str(), nullptr, filters);
			osdialog_filters_free(filters);
			if (!pathC) return;
			const std::string path(pathC);
			std::free(pathC);
			std::string error;
			if (!sibylModule->loadCompositionFromPath(path, &error)) {
				osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
					(error.empty() ? "Sibyl composition load failed." : error.c_str()));
			}
		}));
		menu->addChild(createMenuItem("Save composition…", "JSON", [sibylModule]() {
			const std::string directory = portableDirectory();
			system::createDirectories(directory);
			const std::string filename = portableFilename(sibylModule);
			osdialog_filters* filters = osdialog_filters_parse("Sibyl composition:json,JSON");
			char* pathC = osdialog_file(OSDIALOG_SAVE, directory.c_str(), filename.c_str(), filters);
			osdialog_filters_free(filters);
			if (!pathC) return;
			const std::string path = ensureJsonExtension(pathC);
			std::free(pathC);
			std::string error;
			if (!sibylModule->saveCompositionToPath(path, &error)) {
				osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
					(error.empty() ? "Sibyl composition save failed." : error.c_str()));
			}
		}));
	}
};

Model* modelSibyl = createModel<SibylModule, SibylWidget>("Sibyl");
#endif
