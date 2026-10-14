#include "SibylTransport.hpp"
#include "SibylAdoption.hpp"

namespace sibyl {
namespace {

TransportParseResult fail(const std::string& path, const std::string& message) {
	TransportParseResult result;
	result.code = "invalid_request";
	result.path = path;
	result.message = message;
	return result;
}

bool parseAction(const std::string& value, TransportAction& action) {
	if (value == "play") action = TransportAction::PLAY;
	else if (value == "pause") action = TransportAction::PAUSE;
	else if (value == "stop") action = TransportAction::STOP;
	else if (value == "restart" || value == "reset") action = TransportAction::RESTART;
	else if (value == "panic") action = TransportAction::PANIC;
	else if (value == "next_scene") action = TransportAction::NEXT_SCENE;
	else if (value == "previous_scene") action = TransportAction::PREVIOUS_SCENE;
	else if (value == "select_scene") action = TransportAction::SELECT_SCENE;
	else if (value == "reseed") action = TransportAction::RESEED;
	else return false;
	return true;
}

bool parseTarget(const std::string& value, RestartTarget& target) {
	if (value == "scene") target = RestartTarget::SCENE;
	else if (value == "arrangement") target = RestartTarget::ARRANGEMENT;
	else if (value == "patterns") target = RestartTarget::PATTERNS;
	else if (value == "randomness") target = RestartTarget::RANDOMNESS;
	else return false;
	return true;
}

bool parsePhaseMode(const std::string& value, PhaseMode& mode) {
	if (value == "restart") mode = PhaseMode::RESTART;
	else if (value == "continue") mode = PhaseMode::CONTINUE;
	else if (value == "alignGlobal") mode = PhaseMode::ALIGN_GLOBAL;
	else return false;
	return true;
}

} // namespace

TransportParseResult parseTransportRequest(json_t* root, ApplyAt defaultApplyAt) {
	if (!root || !json_is_object(root)) return fail("$", "Transport request must be an object.");
	json_t* actionJ = json_object_get(root, "action");
	if (!actionJ || !json_is_string(actionJ)) return fail("action", "Transport action is required.");
	TransportParseResult result;
	std::string actionName = json_string_value(actionJ);
	if (!parseAction(actionName, result.request.action)) return fail("action", "Unsupported transport action '" + actionName + "'.");
	result.request.applyAt = defaultApplyAt;
	json_t* applyJ = json_object_get(root, "apply_at");
	if (!applyJ) applyJ = json_object_get(root, "applyAt");
	if (applyJ && (!json_is_string(applyJ) || !parseApplyAtName(json_string_value(applyJ), result.request.applyAt)))
		return fail("apply_at", "Unsupported apply boundary.");
	if (result.request.action == TransportAction::PANIC) result.request.applyAt = ApplyAt::IMMEDIATE;

	json_t* targetJ = json_object_get(root, "target");
	if (actionName == "reset") result.request.target = RestartTarget::ARRANGEMENT;
	else if (result.request.action == TransportAction::RESTART) {
		if (!targetJ || !json_is_string(targetJ) || !parseTarget(json_string_value(targetJ), result.request.target))
			return fail("target", "restart requires target scene, arrangement, patterns, or randomness.");
	} else if (targetJ) return fail("target", "target is only valid for restart.");

	json_t* sceneJ = json_object_get(root, "scene_id");
	if (!sceneJ) sceneJ = json_object_get(root, "sceneId");
	if (result.request.action == TransportAction::SELECT_SCENE) {
		if (!sceneJ || !json_is_string(sceneJ) || !*json_string_value(sceneJ)) return fail("scene_id", "select_scene requires scene_id.");
		result.request.sceneId = json_string_value(sceneJ);
	} else if (sceneJ) return fail("scene_id", "scene_id is only valid for select_scene.");

	json_t* phaseJ = json_object_get(root, "phase_mode");
	if (!phaseJ) phaseJ = json_object_get(root, "phaseMode");
	const bool sceneChanging = result.request.action == TransportAction::NEXT_SCENE ||
		result.request.action == TransportAction::PREVIOUS_SCENE || result.request.action == TransportAction::SELECT_SCENE ||
		(result.request.action == TransportAction::RESTART &&
			(result.request.target == RestartTarget::SCENE || result.request.target == RestartTarget::ARRANGEMENT));
	if (phaseJ) {
		if (!sceneChanging) return fail("phase_mode", "phase_mode is only valid for scene-changing actions.");
		if (!json_is_string(phaseJ) || !parsePhaseMode(json_string_value(phaseJ), result.request.phaseMode))
			return fail("phase_mode", "Unsupported phase mode.");
		result.request.hasPhaseModeOverride = true;
	}
	result.valid = true;
	return result;
}

const char* transportActionName(TransportAction action) {
	switch (action) {
		case TransportAction::PLAY: return "play";
		case TransportAction::PAUSE: return "pause";
		case TransportAction::STOP: return "stop";
		case TransportAction::RESTART: return "restart";
		case TransportAction::PANIC: return "panic";
		case TransportAction::NEXT_SCENE: return "next_scene";
		case TransportAction::PREVIOUS_SCENE: return "previous_scene";
		case TransportAction::SELECT_SCENE: return "select_scene";
		case TransportAction::RESEED: return "reseed";
	}
	return "play";
}

const char* restartTargetName(RestartTarget target) {
	switch (target) {
		case RestartTarget::SCENE: return "scene";
		case RestartTarget::ARRANGEMENT: return "arrangement";
		case RestartTarget::PATTERNS: return "patterns";
		case RestartTarget::RANDOMNESS: return "randomness";
		case RestartTarget::NONE: return "none";
	}
	return "none";
}

const char* phaseModeName(PhaseMode mode) {
	switch (mode) {
		case PhaseMode::RESTART: return "restart";
		case PhaseMode::CONTINUE: return "continue";
		case PhaseMode::ALIGN_GLOBAL: return "alignGlobal";
	}
	return "restart";
}

} // namespace sibyl
