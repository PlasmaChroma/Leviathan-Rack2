#include "SibylTransport.hpp"

#include <jansson.h>
#include <iostream>
#include <string>

namespace {
int failures = 0;
void check(bool condition, const std::string& name) {
	if (condition) std::cout << "[PASS] " << name << "\n";
	else { std::cout << "[FAIL] " << name << "\n"; ++failures; }
}
sibyl::TransportParseResult parse(const char* json) {
	json_error_t error;
	json_t* root = json_loads(json, 0, &error);
	auto result = sibyl::parseTransportRequest(root, sibyl::ApplyAt::NEXT_BEAT);
	if (root) json_decref(root);
	return result;
}
}

int main() {
	auto play = parse(R"({"action":"play"})");
	check(play.valid && play.request.action == sibyl::TransportAction::PLAY && play.request.applyAt == sibyl::ApplyAt::NEXT_BEAT,
		"play uses the composition boundary default");
	auto restart = parse(R"({"action":"restart","target":"scene","apply_at":"nextScene","phase_mode":"continue"})");
	check(restart.valid && restart.request.target == sibyl::RestartTarget::SCENE &&
		restart.request.applyAt == sibyl::ApplyAt::NEXT_SCENE && restart.request.hasPhaseModeOverride &&
		restart.request.phaseMode == sibyl::PhaseMode::CONTINUE, "restart parses target, boundary, and phase override");
	auto reset = parse(R"({"action":"reset","applyAt":"immediate"})");
	check(reset.valid && reset.request.action == sibyl::TransportAction::RESTART &&
		reset.request.target == sibyl::RestartTarget::ARRANGEMENT, "reset normalizes to arrangement restart");
	auto select = parse(R"({"action":"select_scene","scene_id":"chorus","phase_mode":"alignGlobal"})");
	check(select.valid && select.request.sceneId == "chorus" && select.request.phaseMode == sibyl::PhaseMode::ALIGN_GLOBAL,
		"scene selection parses destination and phase mode");
	auto panic = parse(R"({"action":"panic","apply_at":"nextScene"})");
	check(panic.valid && panic.request.applyAt == sibyl::ApplyAt::IMMEDIATE, "panic is always normalized to immediate");
	check(parse(R"({"action":"restart"})").path == "target", "restart requires an explicit target");
	check(parse(R"({"action":"select_scene"})").path == "scene_id", "select_scene requires a scene id");
	check(parse(R"({"action":"pause","target":"scene"})").path == "target", "unrelated target is rejected");
	check(parse(R"({"action":"reseed","phase_mode":"restart"})").path == "phase_mode", "unrelated phase mode is rejected");
	check(parse(R"({"action":"teleport"})").path == "action", "unknown action is rejected");
	check(std::string(sibyl::transportActionName(sibyl::TransportAction::PREVIOUS_SCENE)) == "previous_scene" &&
		std::string(sibyl::restartTargetName(sibyl::RestartTarget::RANDOMNESS)) == "randomness" &&
		std::string(sibyl::phaseModeName(sibyl::PhaseMode::ALIGN_GLOBAL)) == "alignGlobal", "normalized names are stable");

	std::cout << "[SUMMARY] sibyl_transport_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures == 0 ? 0 : 1;
}
