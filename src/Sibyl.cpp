#include "plugin.hpp"
#include "SibylControl.hpp"
#include "SibylJSON.hpp"
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

	int m_activeRevision = 0;
	std::vector<std::shared_ptr<const sibyl::Composition>> m_history;
	std::atomic<const sibyl::Composition*> m_activeCompositionPtr{nullptr};

	// Realtime playback state
	double m_globalPhaseBeats = 0.0;
	int m_sceneIndex = 0;
	int m_sceneRepeat = 0;
	double m_scenePhase = 0.0;

	struct TrackState {
		double patternPhaseBeats = 0.0;
		int lastFiredStep = -1;
		float currentPitch = 0.0f;
		float currentGate = 0.0f;
		float currentVel = 0.0f;
		float currentMod = 0.0f;
	};
	TrackState m_trackStates[16];

	SibylModule() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		auto comp = std::make_shared<sibyl::Composition>();
		m_history.push_back(comp);
		m_activeCompositionPtr.store(comp.get(), std::memory_order_release);
	}

	void process(const ProcessArgs& args) override {
		const sibyl::Composition* comp = m_activeCompositionPtr.load(std::memory_order_acquire);
		if (!comp) return;

		double beatDelta = 0.0;
		if (inputs[CLOCK_INPUT].isConnected()) {
			// MVP: fallback to 0 or we could do naive edge detection
			// Wait, let's keep it simple: internal clock if not connected
			// Real external clock estimator requires bounded phase tracking.
		} else {
			double bpm = comp->meta.bpm;
			double beatsPerSecond = bpm / 60.0;
			beatDelta = beatsPerSecond * args.sampleTime;
		}

		if (!comp->transport.running) beatDelta = 0.0;

		m_globalPhaseBeats += beatDelta;

		if (comp->arrangement.empty()) {
			outputs[V_OCT_OUTPUT].setChannels(1);
			outputs[GATE_OUTPUT].setChannels(1);
			outputs[VELOCITY_OUTPUT].setChannels(1);
			outputs[MOD_OUTPUT].setChannels(1);
			outputs[V_OCT_OUTPUT].setVoltage(0.f, 0);
			outputs[GATE_OUTPUT].setVoltage(0.f, 0);
			outputs[VELOCITY_OUTPUT].setVoltage(0.f, 0);
			outputs[MOD_OUTPUT].setVoltage(0.f, 0);
			return;
		}

		if (m_sceneIndex >= (int)comp->arrangement.size()) {
			m_sceneIndex = 0;
			m_sceneRepeat = 0;
			m_scenePhase = 0.0;
		}

		const auto& scene = comp->arrangement[m_sceneIndex];
		m_scenePhase += beatDelta;

		if (m_scenePhase >= scene.lengthBeats) {
			m_scenePhase -= scene.lengthBeats;
			m_sceneRepeat++;
			if (m_sceneRepeat >= scene.repeats) {
				m_sceneRepeat = 0;
				m_sceneIndex++;
				if (m_sceneIndex >= (int)comp->arrangement.size()) {
					if (comp->transport.loop) m_sceneIndex = 0;
					else m_sceneIndex = comp->arrangement.size() - 1; // halt at end MVP
				}
			}
			// Restart tracks on scene boundary (MVP assumption)
			for (int c = 0; c < 16; c++) {
				m_trackStates[c].lastFiredStep = -1;
				m_trackStates[c].patternPhaseBeats = 0.0;
			}
		}

		int maxChannel = -1;
		for (const auto& trackDef : comp->tracks) {
			int ch = trackDef.channel;
			if (ch < 0 || ch > 15) continue;
			maxChannel = std::max(maxChannel, ch);

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

			if (currentStep != m_trackStates[ch].lastFiredStep) {
				m_trackStates[ch].lastFiredStep = currentStep;
				m_trackStates[ch].currentGate = 0.0f; 

				for (const auto& ev : pat.steps) {
					if (ev.step == currentStep) {
						m_trackStates[ch].currentPitch = ev.compiledPitchV;
						m_trackStates[ch].currentGate = 10.0f;
						m_trackStates[ch].currentVel = ev.hasVelocity ? ev.velocity * 10.0f : trackDef.defaultVelocity * 10.0f;
						m_trackStates[ch].currentMod = ev.hasMod ? ev.mod * (trackDef.modRange == sibyl::ModRange::BIPOLAR ? 5.0f : 10.0f) : 0.0f;
						break;
					}
				}
			} else {
				// Gate logic MVP: hold if fraction < gateLength
				double fraction = (stepPhaseBeats - currentStep * pat.resolutionBeats) / pat.resolutionBeats;
				float gateLength = trackDef.defaultGate; 
				for (const auto& ev : pat.steps) {
					if (ev.step == currentStep) {
						if (ev.hasGate) gateLength = ev.gate;
						break;
					}
				}
				if (fraction > gateLength) {
					m_trackStates[ch].currentGate = 0.0f;
				}
			}
		}

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
	}

	bool handleSibylRequest(Operation operation, const std::string& requestJson, std::string& responseJson, std::string& error) override {
		if (operation == Operation::CAPABILITIES) {
			responseJson = "{\"ok\":true,\"capabilities\":{\"sibyl\":{\"apiVersion\":1,\"schemaVersion\":1,\"revision\":" + std::to_string(m_activeRevision) + ",\"operations\":[\"get_composition\",\"validate\",\"edit\",\"get_status\",\"transport\"]}}}";
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
			if (expectedRev != m_activeRevision) {
				json_decref(root);
				error = "Revision conflict";
				// Ideally return structured error as per spec
				responseJson = "{\"ok\":false,\"error\":{\"code\":\"revision_conflict\",\"message\":\"Expected revision " + std::to_string(expectedRev) + " but current is " + std::to_string(m_activeRevision) + "\"}}";
				return false;
			}

			json_t* opsJ = json_object_get(root, "operations");
			if (!opsJ || !json_is_array(opsJ)) {
				json_decref(root);
				error = "Missing operations array";
				return false;
			}

			// Very naive MVP: look for replace_composition
			bool foundReplace = false;
			std::string compJsonStr;
			size_t idx; json_t* opJ;
			json_array_foreach(opsJ, idx, opJ) {
				json_t* opNameJ = json_object_get(opJ, "op");
				if (opNameJ && json_is_string(opNameJ) && std::string(json_string_value(opNameJ)) == "replace_composition") {
					json_t* compJ = json_object_get(opJ, "composition");
					if (compJ) {
						foundReplace = true;
						char* dumped = json_dumps(compJ, 0);
						compJsonStr = dumped;
						free(dumped);
					}
				} else {
					json_decref(root);
					error = "Unsupported operation in MVP";
					return false;
				}
			}

			if (foundReplace) {
				sibyl::ParseResult res = sibyl::parseCompositionJson(compJsonStr, m_activeRevision + 1);
				if (res.valid) {
					m_history.push_back(res.composition);
					m_activeCompositionPtr.store(res.composition.get(), std::memory_order_release);
					m_activeRevision++;
					responseJson = "{\"ok\":true,\"revision\":" + std::to_string(m_activeRevision) + "}";
					json_decref(root);
					return true;
				} else {
					json_decref(root);
					error = "Validation failed: " + (!res.errors.empty() ? res.errors[0].message : "Unknown");
					return false;
				}
			}

			json_decref(root);
			error = "No supported operations found";
			return false;
		}
		error = "Not implemented";
		return false;
	}
};

struct SibylWidget : ModuleWidget {
	SibylWidget(SibylModule* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Sibyl.svg")));
		box.size = mm2px(Vec(50.8, 128.5));

		float xLeft = 10.0f, xRight = 40.8f, ySpace = 12.0f, yStart = 20.0f;

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 0 * ySpace)), module, SibylModule::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 1 * ySpace)), module, SibylModule::RUN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 2 * ySpace)), module, SibylModule::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 3 * ySpace)), module, SibylModule::SCENE_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 4 * ySpace)), module, SibylModule::SCENE_CV_INPUT));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 6 * ySpace)), module, SibylModule::MACRO_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 7 * ySpace)), module, SibylModule::MACRO_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 8 * ySpace)), module, SibylModule::MACRO_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(xLeft, yStart + 9 * ySpace)), module, SibylModule::MACRO_4_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 0 * ySpace)), module, SibylModule::V_OCT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 1 * ySpace)), module, SibylModule::GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 2 * ySpace)), module, SibylModule::VELOCITY_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 3 * ySpace)), module, SibylModule::MOD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 4 * ySpace)), module, SibylModule::CLOCK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 5 * ySpace)), module, SibylModule::SCENE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xRight, yStart + 6 * ySpace)), module, SibylModule::EOC_OUTPUT));
	}
};

Model* modelSibyl = createModel<SibylModule, SibylWidget>("Sibyl");
