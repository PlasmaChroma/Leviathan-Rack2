#include "plugin.hpp"
#include "SibylControl.hpp"
#include "SibylJSON.hpp"
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
		} else if (operation == Operation::GET_COMPOSITION) {
			const sibyl::Composition* comp = m_activeCompositionPtr.load(std::memory_order_acquire);
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
			int rev = m_activeRevision;
			int actRev = comp ? comp->revision : 0;
			bool running = comp ? comp->transport.running : true;
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
			json_object_set_new(stJ, "pendingRevision", json_null());
			json_object_set_new(stJ, "pendingApplyAt", json_null());
			json_object_set_new(stJ, "pendingPhasePolicy", json_null());
			json_object_set_new(stJ, "pendingTransport", json_null());
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
			json_object_set_new(stJ, "lastError", json_null());
			json_object_set_new(stJ, "warnings", json_array());

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
			sibyl::ParseResult res = sibyl::parseCompositionJson(candStr ? candStr : "{}", m_activeRevision);
			if (candStr) free(candStr);
			json_decref(root);

			json_t* respJ = json_object();
			json_object_set_new(respJ, "ok", json_true());
			json_object_set_new(respJ, "revision", json_integer(m_activeRevision));
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
			if (root) {
				json_t* actJ = json_object_get(root, "action");
				if (actJ && json_is_string(actJ)) {
					std::string action = json_string_value(actJ);
					if (action == "restart" || action == "reset") {
						m_sceneIndex = 0;
						m_sceneRepeat = 0;
						m_scenePhase = 0.0;
						for (int c = 0; c < 16; c++) {
							m_trackStates[c].lastFiredStep = -1;
							m_trackStates[c].patternPhaseBeats = 0.0;
						}
					}
				}
				json_decref(root);
			}
			responseJson = "{\"ok\":true}";
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
