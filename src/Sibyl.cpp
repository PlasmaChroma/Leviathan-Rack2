#include "plugin.hpp"
#include "SibylControl.hpp"
#include "SibylAdoption.hpp"
#include "SibylEdit.hpp"
#include "SibylJSON.hpp"
#include "SibylTransport.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include <jansson.h>

using namespace rack;

struct SibylModule : Module, SibylControl {
	enum ParamIds { NUM_PARAMS };
	enum InputIds {
		CLOCK_INPUT, RUN_INPUT, RESET_INPUT, SCENE_TRIG_INPUT, SCENE_CV_INPUT,
		MACRO_1_INPUT, MACRO_2_INPUT, MACRO_3_INPUT, MACRO_4_INPUT, NUM_INPUTS
	};
	enum OutputIds {
		V_OCT_OUTPUT, GATE_OUTPUT, VELOCITY_OUTPUT, MOD_OUTPUT,
		CLOCK_OUTPUT, SCENE_OUTPUT, EOC_OUTPUT, NUM_OUTPUTS
	};
	enum LightIds { NUM_LIGHTS };

	int m_acceptedRevision = 0;
	const sibyl::Composition* m_acceptedCompositionPtr = nullptr;
	bool m_seenAuthoritativeLoad = false;
	std::string m_lastError;
	std::vector<sibyl::ValidationIssue> m_lastWarnings;
	std::vector<std::shared_ptr<const sibyl::Composition>> m_history;
	std::atomic<const sibyl::Composition*> m_activeCompositionPtr{nullptr};
	std::atomic<int> m_activeRevision{0};
	std::vector<std::unique_ptr<const sibyl::AdoptionRequest>> m_adoptionHistory;
	std::atomic<const sibyl::AdoptionRequest*> m_pendingAdoptionPtr{nullptr};
	std::vector<std::unique_ptr<const sibyl::TransportRequest>> m_transportHistory;
	std::atomic<const sibyl::TransportRequest*> m_pendingTransportPtr{nullptr};
	std::atomic<bool> m_runtimeRunning{true};
	std::atomic<bool> m_effectiveRunning{true};
	uint64_t m_randomnessEpoch = 0;

	// Realtime playback state
	double m_globalPhaseBeats = 0.0;
	double m_clockBoundaryPhaseBeats = 0.0;
	int m_sceneIndex = 0;
	int m_sceneRepeat = 0;
	double m_scenePhase = 0.0;

	struct TrackState {
		double patternPhaseBeats = 0.0;
		int lastFiredStep = -1;
		float currentPitch = 0.0f;
		float targetPitch = 0.0f;
		float glideRatePerSample = 0.0f;
		float currentGate = 0.0f;
		float currentVel = 0.0f;
		float currentMod = 0.0f;
	};
	TrackState m_trackStates[16];

	// Hardware Schmitt triggers & pulse generators
	dsp::SchmittTrigger m_clockTrigger;
	dsp::SchmittTrigger m_resetTrigger;
	dsp::SchmittTrigger m_sceneTrigger;
	dsp::PulseGenerator m_clockPulse;
	dsp::PulseGenerator m_scenePulse;
	dsp::PulseGenerator m_eocPulse;

	// External clock interval tracking
	double m_lastClockEdgeSampleTime = 0.0;
	double m_clockIntervalSeconds = 0.5; // ~120 BPM default
	double m_timeSinceLastClockEdge = 0.0;

	SibylModule() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

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
		configOutput(MOD_OUTPUT, "Modulation (Polyphonic)");
		configOutput(CLOCK_OUTPUT, "Reconstructed Clock");
		configOutput(SCENE_OUTPUT, "Scene Transition Trigger");
		configOutput(EOC_OUTPUT, "End of Cycle Trigger");

		auto comp = std::make_shared<sibyl::Composition>();
		m_history.push_back(comp);
		m_acceptedCompositionPtr = comp.get();
		m_activeCompositionPtr.store(comp.get(), std::memory_order_release);
	}

	void acceptComposition(const sibyl::CompositionPtr& composition, sibyl::ApplyAt applyAt,
			sibyl::PhasePolicy phasePolicy, const std::vector<sibyl::ValidationIssue>& warnings = {}) {
		const sibyl::Composition* sounding = m_activeCompositionPtr.load(std::memory_order_acquire);
		m_history.push_back(composition);
		std::unique_ptr<sibyl::AdoptionRequest> request(new sibyl::AdoptionRequest());
		request->composition = composition.get();
		request->applyAt = applyAt;
		request->phasePolicy = phasePolicy;
		request->restartChannelMask = sounding
			? sibyl::changedTrackChannelMask(*sounding, *composition) : uint16_t(0xffffu);
		const sibyl::AdoptionRequest* requestPtr = request.get();
		m_adoptionHistory.emplace_back(request.release());
		m_acceptedCompositionPtr = composition.get();
		m_acceptedRevision = composition->revision;
		m_lastError.clear();
		m_lastWarnings = warnings;
		m_pendingAdoptionPtr.store(requestPtr, std::memory_order_release);
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
		uint16_t restartMask = 0;
		if (request->phasePolicy == sibyl::PhasePolicy::RESTART_ALL) restartMask = 0xffffu;
		else if (request->phasePolicy == sibyl::PhasePolicy::RESTART_CHANGED) restartMask = request->restartChannelMask;
		for (int channel = 0; channel < 16; ++channel) {
			if (request->restartChannelMask & (1u << channel)) m_trackStates[channel].currentGate = 0.0f;
			if (!(restartMask & (1u << channel))) continue;
			m_trackStates[channel].patternPhaseBeats = 0.0;
			m_trackStates[channel].lastFiredStep = -1;
			m_trackStates[channel].currentGate = 0.0f;
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
		for (auto& state : m_trackStates) state.currentGate = 0.0f;
	}

	void applyPatternPhase(sibyl::PhaseMode mode) {
		if (mode == sibyl::PhaseMode::CONTINUE) return;
		for (auto& state : m_trackStates) {
			state.patternPhaseBeats = mode == sibyl::PhaseMode::ALIGN_GLOBAL ? m_globalPhaseBeats : 0.0;
			state.lastFiredStep = -1;
		}
	}

	sibyl::PhaseMode scenePhaseMode(const sibyl::Composition& composition, int sceneIndex,
			const sibyl::TransportRequest& request) const {
		if (request.hasPhaseModeOverride) return request.phaseMode;
		if (sceneIndex >= 0 && sceneIndex < (int)composition.arrangement.size())
			return composition.arrangement[sceneIndex].phaseMode;
		return sibyl::PhaseMode::RESTART;
	}

	void enterScene(const sibyl::Composition& composition, int sceneIndex,
			const sibyl::TransportRequest& request) {
		if (composition.arrangement.empty()) return;
		m_sceneIndex = std::max(0, std::min(sceneIndex, (int)composition.arrangement.size() - 1));
		m_sceneRepeat = 0;
		m_scenePhase = 0.0;
		closeAllGates();
		applyPatternPhase(scenePhaseMode(composition, m_sceneIndex, request));
		m_scenePulse.trigger(1e-3f);
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
						break;
					case sibyl::RestartTarget::ARRANGEMENT:
						enterScene(composition, 0, *request);
						m_randomnessEpoch = 0;
						break;
					case sibyl::RestartTarget::PATTERNS:
						closeAllGates();
						applyPatternPhase(sibyl::PhaseMode::RESTART);
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
		json_object_set_new(rootJ, "schemaVersion", json_integer(1));
		json_object_set_new(rootJ, "revision", json_integer(m_acceptedRevision));

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
		const sibyl::Composition* comp = m_activeCompositionPtr.load(std::memory_order_acquire);
		if (!comp) return;

		// --- Transport Run / Pause ---
		bool isRunning = m_runtimeRunning.load(std::memory_order_acquire);
		if (inputs[RUN_INPUT].isConnected()) {
			isRunning = inputs[RUN_INPUT].getVoltage() >= 1.0f;
		}
		m_effectiveRunning.store(isRunning, std::memory_order_release);

		// --- Reset Trigger ---
		if (m_resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
			m_sceneIndex = 0;
			m_sceneRepeat = 0;
			m_scenePhase = 0.0;
			for (int c = 0; c < 16; c++) {
				m_trackStates[c].lastFiredStep = -1;
				m_trackStates[c].patternPhaseBeats = 0.0;
			}
		}

		// --- Clock Processing ---
		double rawBeatDelta = 0.0;
		bool clockTick = false;
		if (inputs[CLOCK_INPUT].isConnected()) {
			m_timeSinceLastClockEdge += args.sampleTime;
			if (m_clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
				if (m_timeSinceLastClockEdge > 0.001) {
					m_clockIntervalSeconds = m_timeSinceLastClockEdge;
				}
				m_timeSinceLastClockEdge = 0.0;
				int ppqn = std::max(1, comp->clock.externalPpqn);
				rawBeatDelta = 1.0 / ppqn;
				clockTick = true;
			} else {
				// Timeout check
				if (m_timeSinceLastClockEdge * 1000.0 > comp->clock.externalTimeoutMs) {
					if (comp->clock.onExternalStop == sibyl::OnExternalStop::INTERNAL) {
						double beatsPerSec = comp->meta.bpm / 60.0;
						rawBeatDelta = beatsPerSec * args.sampleTime;
					} else if (comp->clock.onExternalStop == sibyl::OnExternalStop::FREE_RUN) {
						if (m_clockIntervalSeconds > 0.0) {
							rawBeatDelta = (1.0 / std::max(1, comp->clock.externalPpqn)) * (args.sampleTime / m_clockIntervalSeconds);
						}
					} else {
						// HOLD
						rawBeatDelta = 0.0;
					}
				}
			}
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
		const sibyl::AdoptionRequest* pending = m_pendingAdoptionPtr.load(std::memory_order_acquire);
		adoptPendingIfReady(adoptionBoundary, pending);
		comp = m_activeCompositionPtr.load(std::memory_order_acquire);
		if (!comp) return;
		const sibyl::TransportRequest* pendingTransport = m_pendingTransportPtr.load(std::memory_order_acquire);
		applyPendingTransportIfReady(adoptionBoundary, *comp, pendingTransport);
		isRunning = m_runtimeRunning.load(std::memory_order_acquire);
		if (inputs[RUN_INPUT].isConnected()) isRunning = inputs[RUN_INPUT].getVoltage() >= 1.0f;
		m_effectiveRunning.store(isRunning, std::memory_order_release);
		double beatDelta = isRunning ? rawBeatDelta : 0.0;

		m_clockBoundaryPhaseBeats += rawBeatDelta;
		m_globalPhaseBeats += beatDelta;

		// Clock output pulse (every beat subdivision)
		if (isRunning) {
			int outPpqn = std::max(1, comp->clock.outputPpqn);
			double clockPhase = fmod(m_globalPhaseBeats * outPpqn, 1.0);
			if (clockPhase < (beatDelta * outPpqn) || clockTick) {
				m_clockPulse.trigger(1e-3f);
			}
		}

		if (comp->arrangement.empty()) {
			outputs[V_OCT_OUTPUT].setChannels(1);
			outputs[GATE_OUTPUT].setChannels(1);
			outputs[VELOCITY_OUTPUT].setChannels(1);
			outputs[MOD_OUTPUT].setChannels(1);
			outputs[V_OCT_OUTPUT].setVoltage(0.f, 0);
			outputs[GATE_OUTPUT].setVoltage(0.f, 0);
			outputs[VELOCITY_OUTPUT].setVoltage(0.f, 0);
			outputs[MOD_OUTPUT].setVoltage(0.f, 0);
			outputs[CLOCK_OUTPUT].setVoltage(m_clockPulse.process(args.sampleTime) ? 10.0f : 0.0f);
			outputs[SCENE_OUTPUT].setVoltage(m_scenePulse.process(args.sampleTime) ? 10.0f : 0.0f);
			outputs[EOC_OUTPUT].setVoltage(m_eocPulse.process(args.sampleTime) ? 10.0f : 0.0f);
			return;
		}

		// --- Scene Addressing (Trigger & CV) ---
		bool manualSceneJump = false;
		if (m_sceneTrigger.process(inputs[SCENE_TRIG_INPUT].getVoltage())) {
			m_sceneIndex = (m_sceneIndex + 1) % comp->arrangement.size();
			m_sceneRepeat = 0;
			m_scenePhase = 0.0;
			manualSceneJump = true;
			m_scenePulse.trigger(1e-3f);
		} else if (inputs[SCENE_CV_INPUT].isConnected()) {
			float cvNorm = clamp(inputs[SCENE_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			int targetIdx = clamp(int(cvNorm * comp->arrangement.size()), 0, (int)comp->arrangement.size() - 1);
			if (targetIdx != m_sceneIndex) {
				m_sceneIndex = targetIdx;
				m_sceneRepeat = 0;
				m_scenePhase = 0.0;
				manualSceneJump = true;
				m_scenePulse.trigger(1e-3f);
			}
		}

		if (m_sceneIndex >= (int)comp->arrangement.size()) {
			m_sceneIndex = 0;
			m_sceneRepeat = 0;
			m_scenePhase = 0.0;
		}

		const auto& scene = comp->arrangement[m_sceneIndex];
		if (!manualSceneJump) {
			m_scenePhase += beatDelta;
		}

		// --- Scene Boundary Progression ---
		if (m_scenePhase >= scene.lengthBeats) {
			m_scenePhase -= scene.lengthBeats;
			m_sceneRepeat++;
			if (m_sceneRepeat >= scene.repeats) {
				m_sceneRepeat = 0;
				m_sceneIndex++;
				m_scenePulse.trigger(1e-3f);
				if (m_sceneIndex >= (int)comp->arrangement.size()) {
					if (comp->transport.loop) {
						m_sceneIndex = 0;
						m_eocPulse.trigger(1e-3f);
					} else {
						m_sceneIndex = (int)comp->arrangement.size() - 1;
						m_scenePhase = comp->arrangement.back().lengthBeats;
						m_runtimeRunning.store(false, std::memory_order_release);
						m_effectiveRunning.store(false, std::memory_order_release);
						isRunning = false;
						beatDelta = 0.0;
						closeAllGates();
					}
				}
			}
			// Reset track phases on scene boundary if scene phaseMode == RESTART
			if (scene.phaseMode == sibyl::PhaseMode::RESTART) {
				for (int c = 0; c < 16; c++) {
					m_trackStates[c].lastFiredStep = -1;
					m_trackStates[c].patternPhaseBeats = 0.0;
				}
			}
		}

		// --- Macro Inputs Evaluation (0–10 V) ---
		float globalProbMacro = 0.f;
		float globalVelMacro = 0.f;
		float trackProbMacro[16] = {};
		float trackVelMacro[16] = {};
		float trackGateMacro[16] = {};

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

			auto trackAsgIt = scene.tracks.find(trackDef.id);
			if (trackAsgIt == scene.tracks.end() || trackAsgIt->second.patternId.empty()) {
				m_trackStates[ch].currentGate = 0.0f;
				continue;
			}

			auto patIt = comp->patterns.find(trackAsgIt->second.patternId);
			if (patIt == comp->patterns.end()) continue;
			const auto& pat = patIt->second;

			if (pat.resolutionBeats <= 0) continue;

			m_trackStates[ch].patternPhaseBeats += beatDelta;
			
			double stepPhaseBeats = fmod(m_trackStates[ch].patternPhaseBeats, pat.length * pat.resolutionBeats);
			int currentStep = static_cast<int>(stepPhaseBeats / pat.resolutionBeats);

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

			if (currentStep != m_trackStates[ch].lastFiredStep) {
				m_trackStates[ch].lastFiredStep = currentStep;

				// Find event for this step
				const sibyl::StepEvent* matchedEvent = nullptr;
				for (const auto& ev : pat.steps) {
					if (ev.step == currentStep) {
						matchedEvent = &ev;
						break;
					}
				}

				if (matchedEvent) {
					// Deterministic Probability Check + Macro Modulation
					float baseProb = matchedEvent->hasProbability ? matchedEvent->probability : 1.0f;
					float effProb = clamp(baseProb + globalProbMacro + trackProbMacro[ch], 0.0f, 1.0f);
					uint64_t hash = comp->meta.seed ^ (m_randomnessEpoch * 11400714819323198485ULL) ^
						(m_sceneIndex * 73856093ULL) ^ (ch * 19349663ULL) ^ (currentStep * 83492791ULL);
					float randVal = (float)((hash % 10000ULL) / 10000.0);
					bool play = randVal < effProb;

					if (play) {
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
						m_trackStates[ch].currentMod = matchedEvent->hasMod ? matchedEvent->mod * (trackDef.modRange == sibyl::ModRange::BIPOLAR ? 5.0f : 10.0f) : 0.0f;
					} else {
						m_trackStates[ch].currentGate = 0.0f;
					}
				} else {
					m_trackStates[ch].currentGate = 0.0f; // Rest
				}
			} else {
				// Gate length & ratchet slicing + Gate Macro
				double stepFraction = (stepPhaseBeats - currentStep * pat.resolutionBeats) / pat.resolutionBeats;
				float baseGate = trackDef.defaultGate;
				int ratchets = 1;
				bool tie = false;

				for (const auto& ev : pat.steps) {
					if (ev.step == currentStep) {
						if (ev.hasGate) baseGate = ev.gate;
						ratchets = std::max(1, ev.ratchets);
						tie = ev.tie;
						break;
					}
				}

				float effGate = clamp(baseGate + trackGateMacro[ch], 0.01f, 1.0f);

				if (!tie) {
					if (ratchets > 1) {
						double sliceFraction = fmod(stepFraction * ratchets, 1.0);
						if (sliceFraction > effGate) {
							m_trackStates[ch].currentGate = 0.0f;
						} else {
							m_trackStates[ch].currentGate = 10.0f;
						}
					} else {
						if (stepFraction > effGate) {
							m_trackStates[ch].currentGate = 0.0f;
						}
					}
				}
			}
		}

		// Write Polyphonic outputs
		int numChannels = std::max(1, maxChannel + 1);
		outputs[V_OCT_OUTPUT].setChannels(numChannels);
		outputs[GATE_OUTPUT].setChannels(numChannels);
		outputs[VELOCITY_OUTPUT].setChannels(numChannels);
		outputs[MOD_OUTPUT].setChannels(numChannels);

		for (int c = 0; c < numChannels; c++) {
			outputs[V_OCT_OUTPUT].setVoltage(m_trackStates[c].currentPitch, c);
			outputs[GATE_OUTPUT].setVoltage(m_trackStates[c].currentGate, c);
			outputs[VELOCITY_OUTPUT].setVoltage(m_trackStates[c].currentVel, c);
			outputs[MOD_OUTPUT].setVoltage(m_trackStates[c].currentMod, c);
		}

		// Write Pulse triggers
		outputs[CLOCK_OUTPUT].setVoltage(m_clockPulse.process(args.sampleTime) ? 10.0f : 0.0f);
		outputs[SCENE_OUTPUT].setVoltage(m_scenePulse.process(args.sampleTime) ? 10.0f : 0.0f);
		outputs[EOC_OUTPUT].setVoltage(m_eocPulse.process(args.sampleTime) ? 10.0f : 0.0f);
	}

	bool handleSibylRequest(Operation operation, const std::string& requestJson, std::string& responseJson, std::string& error) override {
		if (operation == Operation::CAPABILITIES) {
			responseJson = "{\"ok\":true,\"capabilities\":{\"sibyl\":{\"apiVersion\":1,\"schemaVersion\":1,\"revision\":" + std::to_string(m_acceptedRevision) + ",\"operations\":[\"get_composition\",\"validate\",\"edit\",\"get_status\",\"transport\"]}}}";
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
			const sibyl::Composition* comp = m_activeCompositionPtr.load(std::memory_order_acquire);
			int rev = m_acceptedRevision;
			int actRev = m_activeRevision.load(std::memory_order_acquire);
			const sibyl::AdoptionRequest* pending = m_pendingAdoptionPtr.load(std::memory_order_acquire);
			const sibyl::TransportRequest* pendingTransport = m_pendingTransportPtr.load(std::memory_order_acquire);
			bool running = m_effectiveRunning.load(std::memory_order_acquire);
			float bpm = comp ? comp->meta.bpm : 120.0f;
			std::string sceneId = "";
			int sceneRepeat = m_sceneRepeat;
			double beat = m_scenePhase;
			if (comp && m_sceneIndex >= 0 && m_sceneIndex < (int)comp->arrangement.size()) {
				sceneId = comp->arrangement[m_sceneIndex].id;
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
			json_object_set_new(stJ, "clockSource", json_string(inputs[CLOCK_INPUT].isConnected() ? "external" : "internal"));
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
			m_transportHistory.emplace_back(request.release());
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

struct SibylWidget : ModuleWidget {
	explicit SibylWidget(SibylModule* module) {
		setModule(module);
		PreviewBuildLogTimer previewTimer("Sibyl", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/Sibyl.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/Sibyl.labels.svg");
		splitPanel.addCompactLeviathanLogoBranding();
		visual_assets::addFractalGlassOverlay(this, panelPath, splitPanel.panelSurfaceEffectWidget());

		// Inputs: Left column (x = 8.5mm), Second column (x = 20.0mm)
		// Row 1: y = 49mm (CLK, RUN)
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(8.5f, 49.0f)), module, SibylModule::CLOCK_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(20.0f, 49.0f)), module, SibylModule::RUN_INPUT));

		// Row 2: y = 64mm (RST, TRIG)
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(8.5f, 64.0f)), module, SibylModule::RESET_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(20.0f, 64.0f)), module, SibylModule::SCENE_TRIG_INPUT));

		// Row 3: y = 79mm (S.CV)
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(8.5f, 79.0f)), module, SibylModule::SCENE_CV_INPUT));

		// Row 4: y = 94mm (M1, M2)
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(8.5f, 94.0f)), module, SibylModule::MACRO_1_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(20.0f, 94.0f)), module, SibylModule::MACRO_2_INPUT));

		// Row 5: y = 109mm (M3, M4)
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(8.5f, 109.0f)), module, SibylModule::MACRO_3_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(20.0f, 109.0f)), module, SibylModule::MACRO_4_INPUT));

		// Outputs: Third column (x = 32.5mm), Fourth column (x = 43.0mm)
		// Row 1: y = 49mm (V/OCT, GATE)
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(32.5f, 49.0f)), module, SibylModule::V_OCT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(43.0f, 49.0f)), module, SibylModule::GATE_OUTPUT));

		// Row 2: y = 64mm (VEL, MOD)
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(32.5f, 64.0f)), module, SibylModule::VELOCITY_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(43.0f, 64.0f)), module, SibylModule::MOD_OUTPUT));

		// Row 3: y = 79mm (CLK, SCENE)
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(32.5f, 79.0f)), module, SibylModule::CLOCK_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(43.0f, 79.0f)), module, SibylModule::SCENE_OUTPUT));

		// Row 4: y = 94mm (EOC)
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(37.75f, 94.0f)), module, SibylModule::EOC_OUTPUT));
	}
};

Model* modelSibyl = createModel<SibylModule, SibylWidget>("Sibyl");
