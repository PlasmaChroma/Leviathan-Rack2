#include "Moirai.hpp"

#include "MoiraiJSON.hpp"
#include "MoiraiEdit.hpp"

#include <algorithm>
#include <cmath>

using namespace rack;

namespace {
std::string dumpJson(json_t* root) {
	char* text = json_dumps(root, JSON_COMPACT);
	std::string result = text ? text : "{}";
	if (text) free(text);
	return result;
}

void appendIssues(json_t* response, const char* key,
		const std::vector<moirai::ValidationIssue>& issues) {
	json_t* values = json_array();
	for (const auto& issue : issues) {
		json_t* value = json_object();
		json_object_set_new(value, "code", json_string(issue.code.c_str()));
		json_object_set_new(value, "path", json_string(issue.path.c_str()));
		json_object_set_new(value, "message", json_string(issue.message.c_str()));
		json_array_append_new(values, value);
	}
	json_object_set_new(response, key, values);
}

const char* applyAtName(moirai::ApplyAt value) {
	switch (value) {
		case moirai::ApplyAt::IMMEDIATE: return "immediate";
		case moirai::ApplyAt::NEXT_TRIGGER: return "nextTrigger";
		case moirai::ApplyAt::ALL_IDLE: return "allIdle";
		case moirai::ApplyAt::NEXT_CLOCK: return "nextClock";
	}
	return "immediate";
}

const char* voicePolicyName(moirai::ActiveVoicePolicy value) {
	return value == moirai::ActiveVoicePolicy::RESTART_ACTIVE ? "restartActive" : "finishCurrent";
}

json_t* stringArray(std::initializer_list<const char*> values) {
	json_t* result = json_array();
	for (const char* value : values) json_array_append_new(result, json_string(value));
	return result;
}

void setRevisionFields(json_t* response, const Moirai& module) {
	json_object_set_new(response, "acceptedRevision", json_integer(module.authoredBank.revision));
	json_object_set_new(response, "activeRevision", json_integer(module.envelopeEngine.activeRevision()));
	const int pending = module.envelopeEngine.pendingRevision();
	json_object_set_new(response, "pendingRevision", pending < 0 ? json_null() : json_integer(pending));
}
} // namespace

Moirai::Moirai() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configParam(TIME_PARAM, -4.f, 4.f, 0.f, "Time scale", " oct", 2.f);
	configParam(CURVE_PARAM, -1.f, 1.f, 0.f, "Curve bias");
	configParam(LEVEL_PARAM, 0.f, 1.f, 1.f, "Level", "%", 0.f, 100.f);
	configButton(LANE_PARAM, "Select inspected lane");
	configParam(CHANNEL_PARAM, 0.f, 15.f, 0.f, "Inspected channel", "", 0.f, 1.f, 1.f);
	getParamQuantity(CHANNEL_PARAM)->snapEnabled = true;
	configButton(MANUAL_TRIGGER_PARAM, "Manually trigger selected channel");
	configInput(GATE_INPUT, "Polyphonic gate");
	configInput(VELOCITY_INPUT, "Polyphonic velocity");
	configInput(M1_INPUT, "Polyphonic modulation 1");
	configInput(M2_INPUT, "Polyphonic modulation 2");
	configInput(M3_INPUT, "Polyphonic modulation 3");
	configInput(CLOCK_INPUT, "Clock");
	configInput(RESET_INPUT, "Reset");
	configOutput(A_OUTPUT, "Lane A envelope (polyphonic)");
	configOutput(EOC_A_OUTPUT, "Lane A end of cycle (polyphonic)");
	configOutput(B_OUTPUT, "Lane B envelope (polyphonic)");
	configOutput(EOC_B_OUTPUT, "Lane B end of cycle (polyphonic)");

	authoredBank = moirai::makeInitialBank();
	const moirai::CompileResult compiled = moirai::compileBank(authoredBank);
	compiledBank = compiled.bank;
	envelopeEngine.installBank(compiledBank);
	for (auto& mask : telemetryActiveMask) mask.store(0u, std::memory_order_relaxed);
	for (auto& value : telemetrySelectedValue) value.store(0.f, std::memory_order_relaxed);
	for (auto& phase : telemetrySelectedPhase) phase.store(0.f, std::memory_order_relaxed);
	for (auto& stage : telemetrySelectedStage) stage.store(0, std::memory_order_relaxed);
	for (auto& mask : semanticTriggerMask) mask.store(0u, std::memory_order_relaxed);
}

float Moirai::resolvedInputVoltage(InputId inputId, int channel, float neutral) noexcept {
	Input& input = inputs[inputId];
	if (!input.isConnected() || input.getChannels() <= 0) return neutral;
	if (input.getChannels() == 1) return input.getVoltage(0);
	return channel < input.getChannels() ? input.getVoltage(channel) : neutral;
}

void Moirai::resetRuntime() noexcept {
	envelopeEngine.reset();
	for (auto& lane : eocPulses)
		for (dsp::PulseGenerator& pulse : lane) pulse.reset();
	manualChannelFloor = 1;
	clockElapsed = 0.0;
	seenClockEdge = false;
	estimatedBpm = compiledBank ? compiledBank->clock.fallbackBpm : 120.f;
}

void Moirai::onReset(const ResetEvent& e) {
	Module::onReset(e);
	selectedLane.store(0, std::memory_order_relaxed);
	resetRuntime();
}

void Moirai::process(const ProcessArgs& args) {
	const moirai::CompiledBank* activeBank = envelopeEngine.bank();
	if (!activeBank) return;
	if (laneButtonTrigger.process(params[LANE_PARAM].getValue(), 0.f, 1.f))
		selectedLane.store(1 - selectedLane.load(std::memory_order_relaxed), std::memory_order_relaxed);
	const int channelSelection = clamp(static_cast<int>(std::round(params[CHANNEL_PARAM].getValue())), 0, 15);
	selectedChannel.store(channelSelection, std::memory_order_relaxed);
	const bool manualTrigger = manualButtonTrigger.process(
		params[MANUAL_TRIGGER_PARAM].getValue(), 0.f, 1.f);
	if (manualTrigger) manualChannelFloor = std::max(manualChannelFloor, channelSelection + 1);

	const bool reset = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f)
		|| semanticResetRequested.exchange(false, std::memory_order_acq_rel);
	clockElapsed += args.sampleTime;
	const bool clockEdge = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 1.f);
	if (clockEdge) {
		if (seenClockEdge && clockElapsed > 0.0001) {
			const float observed = 60.f / static_cast<float>(clockElapsed * activeBank->clock.externalPpqn);
			if (observed >= 20.f && observed <= 400.f)
				estimatedBpm += 0.2f * (observed - estimatedBpm);
		}
		seenClockEdge = true;
		clockElapsed = 0.0;
	}
	if (!inputs[CLOCK_INPUT].isConnected()) estimatedBpm = activeBank->clock.fallbackBpm;
	else if (clockElapsed * 1000.0 > activeBank->clock.lossTimeoutMs &&
			activeBank->clock.onClockLoss == moirai::ClockLossPolicy::FALLBACK)
		estimatedBpm = activeBank->clock.fallbackBpm;

	if (reset) {
		resetRuntime();
	}

	const int gateChannels = inputs[GATE_INPUT].isConnected()
		? clamp(inputs[GATE_INPUT].getChannels(), 1, 16) : 1;
	int channels = std::max(gateChannels, manualChannelFloor);
	std::array<uint16_t, moirai::kLaneCount> semanticTriggers {};
	for (int lane = 0; lane < moirai::kLaneCount; ++lane) {
		semanticTriggers[lane] = semanticTriggerMask[lane].exchange(0u, std::memory_order_acq_rel);
		for (int channel = 0; channel < moirai::kMaxChannels; ++channel)
			if (semanticTriggers[lane] & uint16_t(1u << channel)) {
				channels = std::max(channels, channel + 1);
				manualChannelFloor = std::max(manualChannelFloor, channel + 1);
			}
	}
	moirai::EngineInputs engineInputs;
	engineInputs.channels = channels;
	engineInputs.sampleTime = args.sampleTime;
	engineInputs.bpm = estimatedBpm;
	engineInputs.panelTimeScale = dsp::exp2_taylor5(params[TIME_PARAM].getValue());
	engineInputs.panelCurveBias = params[CURVE_PARAM].getValue();
	engineInputs.panelLevel = params[LEVEL_PARAM].getValue();
	engineInputs.clockEdge = clockEdge;
	engineInputs.triggerMask = semanticTriggers;
	for (int channel = 0; channel < channels; ++channel) {
		engineInputs.gate[channel] = reset ? 0.f : resolvedInputVoltage(GATE_INPUT, channel, 0.f);
		if (manualTrigger && channel == channelSelection) engineInputs.gate[channel] = 10.f;
		engineInputs.velocity[channel] = resolvedInputVoltage(VELOCITY_INPUT, channel, 10.f);
		engineInputs.m1[channel] = resolvedInputVoltage(M1_INPUT, channel, 0.f);
		engineInputs.m2[channel] = resolvedInputVoltage(M2_INPUT, channel, 0.f);
		engineInputs.m3[channel] = resolvedInputVoltage(M3_INPUT, channel, 0.f);
	}
	moirai::EngineOutputs engineOutputs;
	envelopeEngine.process(engineInputs, engineOutputs);
	outputs[A_OUTPUT].setChannels(channels);
	outputs[EOC_A_OUTPUT].setChannels(channels);
	outputs[B_OUTPUT].setChannels(channels);
	outputs[EOC_B_OUTPUT].setChannels(channels);
	for (int channel = 0; channel < channels; ++channel) {
		outputs[A_OUTPUT].setVoltage(engineOutputs.envelope[0][channel], channel);
		outputs[B_OUTPUT].setVoltage(engineOutputs.envelope[1][channel], channel);
		for (int lane = 0; lane < 2; ++lane)
			if (engineOutputs.eoc[lane][channel]) eocPulses[lane][channel].trigger(0.001f);
		outputs[EOC_A_OUTPUT].setVoltage(eocPulses[0][channel].process(args.sampleTime) ? 10.f : 0.f, channel);
		outputs[EOC_B_OUTPUT].setVoltage(eocPulses[1][channel].process(args.sampleTime) ? 10.f : 0.f, channel);
	}
	if (manualChannelFloor > gateChannels) {
		const bool laneAIdle = !envelopeEngine.voice(0, manualChannelFloor - 1).running;
		const bool laneBIdle = !envelopeEngine.voice(1, manualChannelFloor - 1).running;
		if (laneAIdle && laneBIdle) manualChannelFloor = gateChannels;
	}

	const int lane = selectedLane.load(std::memory_order_relaxed);
	lights[LANE_A_LIGHT].setBrightness(lane == 0 ? 1.f : 0.08f);
	lights[LANE_B_LIGHT].setBrightness(lane == 1 ? 1.f : 0.08f);
	if (--telemetryCountdown <= 0) {
		telemetryCountdown = std::max(1, static_cast<int>(args.sampleRate / 60.f));
		telemetrySequence.fetch_add(1, std::memory_order_acq_rel);
		telemetryAcceptedRevision.store(acceptedRevisionSource.load(std::memory_order_acquire), std::memory_order_relaxed);
		telemetryActiveRevision.store(envelopeEngine.activeRevision(), std::memory_order_relaxed);
		telemetryPendingRevision.store(envelopeEngine.pendingRevision(), std::memory_order_relaxed);
		telemetryChannels.store(channels, std::memory_order_relaxed);
		telemetryBpm.store(estimatedBpm, std::memory_order_relaxed);
		telemetryExternalClock.store(inputs[CLOCK_INPUT].isConnected(), std::memory_order_relaxed);
		for (int telemetryLane = 0; telemetryLane < 2; ++telemetryLane) {
			uint16_t activeMask = 0u;
			for (int channel = 0; channel < channels; ++channel)
				if (envelopeEngine.voice(telemetryLane, channel).running) activeMask |= uint16_t(1u << channel);
			telemetryActiveMask[telemetryLane].store(activeMask, std::memory_order_relaxed);
			telemetrySelectedValue[telemetryLane].store(
				envelopeEngine.voice(telemetryLane, channelSelection).value, std::memory_order_relaxed);
			telemetrySelectedPhase[telemetryLane].store(
				envelopeEngine.voice(telemetryLane, channelSelection).segmentPhase, std::memory_order_relaxed);
			telemetrySelectedStage[telemetryLane].store(
				envelopeEngine.voice(telemetryLane, channelSelection).segment, std::memory_order_relaxed);
		}
		telemetrySequence.fetch_add(1, std::memory_order_release);
	}
}

MoiraiTelemetrySnapshot Moirai::readTelemetry() const noexcept {
	MoiraiTelemetrySnapshot result;
	for (;;) {
		const uint64_t before = telemetrySequence.load(std::memory_order_acquire);
		if (before & 1u) continue;
		result.acceptedRevision = telemetryAcceptedRevision.load(std::memory_order_relaxed);
		result.activeRevision = telemetryActiveRevision.load(std::memory_order_relaxed);
		result.pendingRevision = telemetryPendingRevision.load(std::memory_order_relaxed);
		result.channels = telemetryChannels.load(std::memory_order_relaxed);
		result.estimatedBpm = telemetryBpm.load(std::memory_order_relaxed);
		result.externalClock = telemetryExternalClock.load(std::memory_order_relaxed);
		for (int lane = 0; lane < moirai::kLaneCount; ++lane) {
			result.activeMask[lane] = telemetryActiveMask[lane].load(std::memory_order_relaxed);
			result.selectedValue[lane] = telemetrySelectedValue[lane].load(std::memory_order_relaxed);
			result.selectedPhase[lane] = telemetrySelectedPhase[lane].load(std::memory_order_relaxed);
			result.selectedStage[lane] = telemetrySelectedStage[lane].load(std::memory_order_relaxed);
		}
		const uint64_t after = telemetrySequence.load(std::memory_order_acquire);
		if (before == after) return result;
	}
}

json_t* Moirai::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "selectedLane", json_integer(selectedLane.load(std::memory_order_relaxed)));
	json_object_set_new(root, "bank", moirai::bankToJson(authoredBank));
	return root;
}

bool Moirai::handleSemanticRequest(OctaviaSemanticControl::Operation operation,
		const std::string& requestJson, std::string& responseJson, std::string& error) {
	envelopeEngine.reclaimGenerations();
	if (operation == OctaviaSemanticControl::Operation::CAPABILITIES) {
		json_t* response = json_object();
		json_object_set_new(response, "ok", json_true());
		json_object_set_new(response, "capabilityId", json_string(semanticCapabilityId()));
		json_t* capability = json_object();
		json_object_set_new(capability, "apiVersion", json_integer(1));
		json_object_set_new(capability, "schemaVersion", json_integer(1));
		json_object_set_new(capability, "revision", json_integer(authoredBank.revision));
		json_object_set_new(capability, "maxPrograms", json_integer(moirai::kMaxPrograms));
		json_object_set_new(capability, "maxChannels", json_integer(moirai::kMaxChannels));
		json_object_set_new(capability, "programModes", json_pack("[s,s,s]", "oneShot", "gate", "cycle"));
		json_object_set_new(capability, "retriggerPolicies", json_pack("[s,s,s,s]", "restart", "fromCurrent", "legato", "ignoreWhileRunning"));
		json_object_set_new(capability, "adoptionPolicies", json_pack("[s,s,s,s]", "immediate", "nextTrigger", "allIdle", "nextClock"));
		json_object_set_new(capability, "activeVoicePolicies", json_pack("[s,s]", "finishCurrent", "restartActive"));
		json_object_set_new(capability, "curves", stringArray({"linear", "smoothstep", "sigmoid", "hold", "step", "exponential", "logarithmic"}));
		json_object_set_new(capability, "editOperations", stringArray({"replace_bank", "upsert_program", "delete_program", "clone_program", "apply_preset", "assign_program", "set_channel_label", "set_lane_defaults", "set_macro_binding", "set_output_mode", "set_clock"}));
		json_object_set_new(response, "capabilities", capability);
		responseJson = dumpJson(response); json_decref(response); return true;
	}
	if (operation == OctaviaSemanticControl::Operation::GET_DOCUMENT) {
		json_error_t parseError {};
		json_t* request = requestJson.empty() ? json_object() : json_loads(requestJson.c_str(), 0, &parseError);
		if (!request || !json_is_object(request)) {
			if (request) json_decref(request);
			error = "invalid document request";
			return false;
		}
		std::string view = "summary";
		std::string id;
		if (json_t* value = json_object_get(request, "view")) if (json_is_string(value)) view = json_string_value(value);
		if (json_t* value = json_object_get(request, "id")) if (json_is_string(value)) id = json_string_value(value);
		json_t* response = nullptr;
		bool valid = true;
		json_t* full = moirai::bankToJson(authoredBank);
		if (view == "full" || view == "summary") response = json_deep_copy(full);
		else if (view == "program") {
			json_t* value = json_object_get(json_object_get(full, "programs"), id.c_str());
			if (value) { response = json_deep_copy(value); json_object_set_new(response, "id", json_string(id.c_str())); }
			else valid = false;
		} else if (view == "lane" && (id == "A" || id == "B")) {
			response = json_deep_copy(json_object_get(json_object_get(full, "lanes"), id.c_str()));
			json_object_set_new(response, "id", json_string(id.c_str()));
		} else if (view == "channel") {
			char* end = nullptr; const long channel = std::strtol(id.c_str(), &end, 10);
			if (!id.empty() && end && *end == '\0' && channel >= 0 && channel < moirai::kMaxChannels) {
				response = json_object(); json_object_set_new(response, "channel", json_integer(channel));
				json_t* lanes = json_object();
				for (int lane = 0; lane < moirai::kLaneCount; ++lane) {
					json_t* laneJ = json_object();
					json_object_set_new(laneJ, "assignment", json_string(authoredBank.lanes[lane].assignments[channel].c_str()));
					json_object_set_new(laneJ, "label", json_string(authoredBank.lanes[lane].channelLabels[channel].c_str()));
					json_object_set_new(lanes, lane == 0 ? "A" : "B", laneJ);
				}
				json_object_set_new(response, "lanes", lanes);
			} else valid = false;
		} else valid = false;
		json_decref(full);
		if (!valid) {
			response = json_object();
			json_object_set_new(response, "ok", json_false());
			json_object_set_new(response, "error", json_pack("{s:s,s:s}", "code", "object_not_found", "message", "view target was not found"));
		}
		json_decref(request); responseJson = dumpJson(response); json_decref(response);
		if (!valid) error = "view target was not found";
		return valid;
	}
	if (operation == OctaviaSemanticControl::Operation::VALIDATE) {
		json_error_t parseError {};
		json_t* request = json_loads(requestJson.c_str(), 0, &parseError);
		if (!request || !json_is_object(request)) { if (request) json_decref(request); error = "invalid JSON"; return false; }
		json_t* candidate = json_object_get(request, "candidate");
		moirai::JsonResult parsed = moirai::parseBankJson(candidate ? candidate : request);
		moirai::CompileResult compiled;
		if (parsed.valid) compiled = moirai::compileBank(parsed.bank);
		json_t* response = json_object();
		json_object_set_new(response, "ok", json_true());
		json_object_set_new(response, "revision", json_integer(authoredBank.revision));
		json_object_set_new(response, "valid", json_boolean(parsed.valid && compiled.valid));
		appendIssues(response, "errors", parsed.valid ? compiled.errors : parsed.errors);
		appendIssues(response, "warnings", compiled.warnings);
		json_decref(request); responseJson = dumpJson(response); json_decref(response); return true;
	}
	if (operation == OctaviaSemanticControl::Operation::GET_STATUS) {
		json_t* response = json_object(); json_object_set_new(response, "ok", json_true());
		setRevisionFields(response, *this);
		json_object_set_new(response, "channels", json_integer(telemetryChannels.load(std::memory_order_relaxed)));
		json_object_set_new(response, "estimatedBpm", json_real(telemetryBpm.load(std::memory_order_relaxed)));
		json_object_set_new(response, "clockSource", json_string(telemetryExternalClock.load(std::memory_order_relaxed) ? "external" : "internal"));
		json_object_set_new(response, "lastError", persistenceError.empty() ? json_null() : json_string(persistenceError.c_str()));
		responseJson = dumpJson(response); json_decref(response); return true;
	}
	if (operation == OctaviaSemanticControl::Operation::EDIT) {
		json_error_t parseError {};
		json_t* request = json_loads(requestJson.c_str(), 0, &parseError);
		if (!request) { error = "invalid JSON"; return false; }
		moirai::EditResult edited = moirai::applyBankEdit(authoredBank, request);
		json_decref(request);
		if (!edited.valid) {
			json_t* response = json_object(); json_object_set_new(response, "ok", json_false());
			json_t* issue = json_object();
			json_object_set_new(issue, "code", json_string(edited.errorCode.c_str()));
			json_object_set_new(issue, "path", json_string(edited.errorPath.c_str()));
			json_object_set_new(issue, "message", json_string(edited.errorMessage.c_str()));
			json_object_set_new(response, "error", issue);
			if (edited.errorCode == "revision_conflict") json_object_set_new(response, "currentRevision", json_integer(edited.currentRevision));
			responseJson = dumpJson(response); json_decref(response); error = edited.errorMessage; return false;
		}
		authoredBank = std::move(edited.bank); compiledBank = edited.compiledBank;
		acceptedRevisionSource.store(authoredBank.revision, std::memory_order_release);
		envelopeEngine.acceptBank(compiledBank, edited.applyAt, edited.activeVoicePolicy);
		json_t* response = json_object(); json_object_set_new(response, "ok", json_true());
		setRevisionFields(response, *this);
		json_object_set_new(response, "applyAt", json_string(applyAtName(edited.applyAt)));
		json_object_set_new(response, "activeVoicePolicy", json_string(voicePolicyName(edited.activeVoicePolicy)));
		appendIssues(response, "warnings", edited.warnings);
		responseJson = dumpJson(response); json_decref(response); return true;
	}
	if (operation == OctaviaSemanticControl::Operation::COMMAND) {
		json_error_t parseError {};
		json_t* request = json_loads(requestJson.c_str(), 0, &parseError);
		if (!request || !json_is_object(request)) { if (request) json_decref(request); error = "invalid command"; return false; }
		json_t* actionJ = json_object_get(request, "action");
		const std::string action = json_is_string(actionJ) ? json_string_value(actionJ) : "";
		bool valid = true;
		if (action == "reset") semanticResetRequested.store(true, std::memory_order_release);
		else if (action == "select") {
			json_t* laneJ = json_object_get(request, "lane"); json_t* channelJ = json_object_get(request, "channel");
			if (!json_is_string(laneJ) || !json_is_integer(channelJ) || json_integer_value(channelJ) < 0 || json_integer_value(channelJ) >= moirai::kMaxChannels) valid = false;
			else { const std::string lane = json_string_value(laneJ); if (lane != "A" && lane != "B") valid = false; else { selectedLane.store(lane == "B" ? 1 : 0); selectedChannel.store(json_integer_value(channelJ)); } }
		} else if (action == "trigger") {
			json_t* laneJ = json_object_get(request, "lane"); json_t* channelJ = json_object_get(request, "channel");
			if (!json_is_string(laneJ) || !json_is_integer(channelJ) || json_integer_value(channelJ) < 0 || json_integer_value(channelJ) >= moirai::kMaxChannels) valid = false;
			else { const std::string lane = json_string_value(laneJ); const uint16_t bit = uint16_t(1u << json_integer_value(channelJ)); if (lane == "A" || lane == "both") semanticTriggerMask[0].fetch_or(bit); if (lane == "B" || lane == "both") semanticTriggerMask[1].fetch_or(bit); if (lane != "A" && lane != "B" && lane != "both") valid = false; }
		} else valid = false;
		json_decref(request);
		if (!valid) { error = "invalid command"; responseJson = "{\"ok\":false,\"error\":{\"code\":\"invalid_command\",\"message\":\"unsupported or malformed command\"}}"; return false; }
		responseJson = std::string("{\"ok\":true,\"action\":\"") + action + "\",\"pending\":true}"; return true;
	}
	error = "unsupported semantic operation";
	return false;
}

void Moirai::dataFromJson(json_t* rootJ) {
	json_t* laneJ = json_object_get(rootJ, "selectedLane");
	if (json_is_integer(laneJ)) selectedLane.store(clamp(int(json_integer_value(laneJ)), 0, 1));
	persistenceError.clear();
	if (json_t* bankJ = json_object_get(rootJ, "bank")) {
		moirai::JsonResult parsed = moirai::parseBankJson(bankJ);
		if (parsed.valid) {
			moirai::CompileResult compiled = moirai::compileBank(parsed.bank);
			if (compiled.valid) {
				authoredBank = std::move(parsed.bank);
				acceptedRevisionSource.store(authoredBank.revision, std::memory_order_release);
				compiledBank = compiled.bank;
				envelopeEngine.installBank(compiledBank);
			} else persistenceError = compiled.errors.empty() ? "invalid bank" : compiled.errors.front().message;
		} else persistenceError = parsed.errors.empty() ? "invalid bank" : parsed.errors.front().message;
		if (!persistenceError.empty()) {
			authoredBank = moirai::makeInitialBank();
			acceptedRevisionSource.store(authoredBank.revision, std::memory_order_release);
			moirai::CompileResult fallback = moirai::compileBank(authoredBank);
			compiledBank = fallback.bank;
			envelopeEngine.installBank(compiledBank);
		}
	}
	resetRuntime();
}
