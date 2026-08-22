#pragma once

#include "SibylTypes.hpp"
#include <jansson.h>
#include <string>

namespace sibyl {

enum class TransportAction {
	PLAY, PAUSE, STOP, RESTART, PANIC,
	NEXT_SCENE, PREVIOUS_SCENE, SELECT_SCENE, RESEED
};

enum class RestartTarget { NONE, SCENE, ARRANGEMENT, PATTERNS, RANDOMNESS };

struct TransportRequest {
	TransportAction action = TransportAction::PLAY;
	RestartTarget target = RestartTarget::NONE;
	ApplyAt applyAt = ApplyAt::NEXT_BEAT;
	bool hasPhaseModeOverride = false;
	PhaseMode phaseMode = PhaseMode::RESTART;
	std::string sceneId;
};

struct TransportParseResult {
	bool valid = false;
	TransportRequest request;
	std::string code;
	std::string path;
	std::string message;
};

TransportParseResult parseTransportRequest(json_t* root, ApplyAt defaultApplyAt);
const char* transportActionName(TransportAction action);
const char* restartTargetName(RestartTarget target);
const char* phaseModeName(PhaseMode mode);

} // namespace sibyl
