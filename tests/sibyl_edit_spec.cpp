#include "SibylEdit.hpp"

#include <jansson.h>
#include <iostream>
#include <string>

namespace {

int failures = 0;

void check(bool condition, const std::string& name) {
	if (condition) std::cout << "[PASS] " << name << "\n";
	else { std::cout << "[FAIL] " << name << "\n"; ++failures; }
}

const char* baseJson = R"JSON({
  "meta":{"title":"Base","bpm":120,"root":"C","rootOctave":4,"scale":"chromatic","swing":0,"seed":1},
  "clock":{"externalPpqn":24,"outputPpqn":24,"externalTimeoutMs":2000,"onExternalStop":"hold"},
  "transport":{"running":true,"loop":true,"defaultApplyAt":"nextBeat"},
  "tracks":[{"id":"bass","channel":0,"defaultGate":0.5,"defaultVelocity":0.8,"modRange":"unipolar"}],
  "patterns":{"old":{"length":4,"resolution":"1/16","steps":[{"step":0,"note":"C3"}]}},
  "arrangement":[
    {"id":"verse","name":"Verse","lengthBeats":4,"repeats":1,"phaseMode":"restart","tracks":{"bass":"old"}},
    {"id":"outro","name":"Outro","lengthBeats":4,"repeats":1,"phaseMode":"restart","tracks":{}}
  ],
  "macros":{"1":{"target":"global.probability","amount":0.5,"polarity":"unipolar","clamp":[0,1]}}
})JSON";

sibyl::EditResult edit(const sibyl::Composition& base, const char* operations, int revision = 2) {
	json_error_t error;
	json_t* ops = json_loads(operations, 0, &error);
	if (!ops) return {};
	auto result = sibyl::applyCompositionEdit(base, ops, revision);
	json_decref(ops);
	return result;
}

} // namespace

int main() {
	auto parsed = sibyl::parseCompositionJson(baseJson, 1);
	check(parsed.valid && parsed.composition, "base composition compiles");
	if (!parsed.composition) return 1;

	auto changed = edit(*parsed.composition, R"JSON([
	  {"op":"set_meta","path":"title","value":"Edited"},
	  {"op":"set_clock","path":"outputPpqn","value":12},
	  {"op":"upsert_track","id":"lead","track":{"channel":1,"defaultGate":0.4,"defaultVelocity":0.7,"modRange":"bipolar"}},
	  {"op":"upsert_pattern","id":"leadline","pattern":{"length":8,"resolution":"1/8","steps":[{"step":0,"note":"E4"}]}},
	  {"op":"set_scene_track","scene_id":"verse","track_id":"bass","pattern_id":null},
	  {"op":"delete_pattern","id":"old"},
	  {"op":"set_scene_track","scene_id":"verse","track_id":"lead","pattern_id":"leadline"},
	  {"op":"upsert_scene","id":"bridge","scene":{"name":"Bridge","lengthBeats":2,"repeats":1,"phaseMode":"continue","tracks":{"lead":"leadline"}}},
	  {"op":"reorder_scenes","scene_ids":["bridge","verse","outro"]},
	  {"op":"upsert_macro","id":"2","macro":{"target":"track.lead.mod","amount":0.25,"polarity":"bipolar","clamp":[-1,1]}},
	  {"op":"delete_macro","id":"1"}
	])JSON", 9);
	check(changed.valid && changed.composition && changed.composition->revision == 9, "ordered transaction compiles once with requested revision");
	check(changed.composition && changed.composition->meta.title == "Edited" && changed.composition->clock.outputPpqn == 12,
		"set_meta and set_clock update scalar fields");
	check(changed.composition && changed.composition->tracks.size() == 2 && changed.composition->patterns.count("leadline") == 1 && changed.composition->patterns.count("old") == 0,
		"track and pattern upsert/delete operations apply");
	check(changed.composition && changed.composition->arrangement.size() == 3 && changed.composition->arrangement[0].id == "bridge" &&
		changed.composition->arrangement[1].tracks.at("lead").patternId == "leadline",
		"scene upsert, assignment, and reorder operations apply");
	check(changed.composition && changed.composition->macros.count("1") == 0 && changed.composition->macros.count("2") == 1,
		"macro upsert/delete operations apply");
	auto removed = changed.composition ? edit(*changed.composition, R"JSON([
	  {"op":"delete_macro","id":"2"},
	  {"op":"set_scene_track","scene_id":"bridge","track_id":"lead","pattern_id":null},
	  {"op":"set_scene_track","scene_id":"verse","track_id":"lead","pattern_id":null},
	  {"op":"delete_track","id":"lead"},
	  {"op":"delete_scene","id":"outro"}
	])JSON") : sibyl::EditResult{};
	check(removed.valid && removed.composition && removed.composition->tracks.size() == 1 && removed.composition->arrangement.size() == 2,
		"unreferenced tracks and scenes can be deleted transactionally");

	auto inUse = edit(*parsed.composition, R"([{"op":"delete_pattern","id":"old"}])");
	check(!inUse.valid && inUse.errorCode == "object_in_use" && inUse.errorPath == "operations[0]",
		"referenced pattern deletion reports object_in_use");
	auto missing = edit(*parsed.composition, R"([{"op":"delete_scene","id":"missing"}])");
	check(!missing.valid && missing.errorCode == "object_not_found", "missing object deletion is rejected");
	auto unknown = edit(*parsed.composition, R"([{"op":"transpose_everything"}])");
	check(!unknown.valid && unknown.errorCode == "unknown_operation", "unknown operations are rejected");
	auto invalid = edit(*parsed.composition, R"([{"op":"set_meta","path":"bpm","value":"fast"}])");
	check(!invalid.valid && invalid.errorCode == "validation_failed" && invalid.errorPath == "meta.bpm",
		"full compiler rejects invalid transaction result");
	auto unknownPath = edit(*parsed.composition, R"([{"op":"set_meta","path":"futureTempo","value":90}])");
	check(!unknownPath.valid && unknownPath.errorCode == "invalid_operation" && unknownPath.errorPath == "operations[0].path",
		"set_meta rejects paths outside its semantic contract");
	check(parsed.composition->meta.title == "Base" && parsed.composition->patterns.count("old") == 1,
		"editing never mutates the accepted base snapshot");

	std::cout << "[SUMMARY] sibyl_edit_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures == 0 ? 0 : 1;
}
