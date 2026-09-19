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
  "tracks":[{"id":"bass","channel":0,"defaultGate":0.5,"defaultVelocity":0.8}],
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
	  {"op":"upsert_track","id":"lead","track":{"channel":1,"defaultGate":0.4,"defaultVelocity":0.7}},
	  {"op":"upsert_pattern","id":"leadline","pattern":{"length":8,"resolution":"1/8","steps":[{"step":0,"note":"E4"}]}},
	  {"op":"set_scene_track","scene_id":"verse","track_id":"bass","pattern_id":null},
	  {"op":"delete_pattern","id":"old"},
	  {"op":"set_scene_track","scene_id":"verse","track_id":"lead","pattern_id":"leadline"},
	  {"op":"upsert_scene","id":"bridge","scene":{"name":"Bridge","description":"Strip the bass away and suspend the lead before the return.","lengthBeats":2,"repeats":1,"phaseMode":"continue","tracks":{"lead":"leadline"}}},
	  {"op":"reorder_scenes","scene_ids":["bridge","verse","outro"]},
	  {"op":"upsert_macro","id":"2","macro":{"target":"track.lead.mod2","amount":0.25,"polarity":"bipolar","clamp":[-1,1]}},
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
	check(changed.composition && changed.composition->arrangement[0].description ==
		"Strip the bass away and suspend the lead before the return.",
		"scene upsert preserves descriptive musical intent");
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
	// --- P8C Structural Reuse Tests ---
	// 1. update_pattern: partial property update without retransmitting notes
	auto updPat = edit(*changed.composition, R"JSON([
	  {"op":"update_pattern","id":"leadline","set":{"length":32,"evolution":{"probability":0.25}}}
	])JSON");
	check(updPat.valid && updPat.composition && updPat.composition->patterns.at("leadline").length == 32 &&
		updPat.composition->patterns.at("leadline").steps.size() == changed.composition->patterns.at("leadline").steps.size(),
		"update_pattern updates properties while preserving existing notes");

	// 1b. update_pattern: shrinking length below active note step is rejected
	auto withLateNote = edit(*changed.composition, R"JSON([
	  {"op":"insert_notes","pattern_id":"leadline","notes":[{"step":4,"note":"G4"}]}
	])JSON");
	check(withLateNote.valid, "inserted note at step 4 into leadline");
	auto shrinkBad = withLateNote.composition ? edit(*withLateNote.composition, R"JSON([
	  {"op":"update_pattern","id":"leadline","set":{"length":4}}
	])JSON") : sibyl::EditResult{};
	check(!shrinkBad.valid && shrinkBad.errorCode == "invalid_operation",
		"update_pattern rejects shrinking length below active note step");

	// 2. clone_pattern: clones pattern with overrides and fresh IDs
	auto clonePat = edit(*changed.composition, R"JSON([
	  {"op":"clone_pattern","source_id":"leadline","id":"lead_chorus","overrides":{"length":32}}
	])JSON");
	check(clonePat.valid && clonePat.composition && clonePat.composition->patterns.count("lead_chorus") == 1 &&
		clonePat.composition->patterns.at("lead_chorus").length == 32 &&
		clonePat.composition->patterns.at("lead_chorus").steps.size() == changed.composition->patterns.at("leadline").steps.size() &&
		clonePat.composition->patterns.at("lead_chorus").steps[0].id == "n1",
		"clone_pattern clones pattern with overrides and fresh sequential IDs");

	auto clonePatDup = edit(*changed.composition, R"JSON([
	  {"op":"clone_pattern","source_id":"leadline","id":"leadline"}
	])JSON");
	check(!clonePatDup.valid && clonePatDup.errorCode == "invalid_operation",
		"clone_pattern rejects duplicate destination ID");

	// 3. clone_scene: share vs copy
	for (const char* mapping : {R"({"old":"old"})", R"({"old":42})", R"({"old":null})", R"([])"}) {
        std::string operation = std::string(R"([{"op":"clone_scene","source_id":"verse","id":"copy","patterns":"copy","pattern_id_mapping":)") + mapping + "}]";
        auto rejected = edit(*parsed.composition, operation.c_str());
        check(!rejected.valid, "scene copy rejects occupied and malformed destination mappings");
    }
	auto cloneSceneShare = edit(*changed.composition, R"JSON([
	  {"op":"clone_scene","source_id":"bridge","id":"bridge_var","patterns":"share","track_overrides":{"lead":null},"position":"after_source","repeats":2}
	])JSON");
	check(cloneSceneShare.valid && cloneSceneShare.composition && cloneSceneShare.composition->arrangement.size() == 4 &&
		cloneSceneShare.composition->arrangement[1].id == "bridge_var" &&
		cloneSceneShare.composition->arrangement[1].repeats == 2 &&
		cloneSceneShare.composition->arrangement[1].tracks.count("lead") == 0,
		"clone_scene with patterns:share applies track_overrides and after_source position");

	auto cloneSceneCopy = edit(*changed.composition, R"JSON([
	  {"op":"clone_scene","source_id":"verse","id":"chorus","patterns":"copy"}
	])JSON");
	check(cloneSceneCopy.valid && cloneSceneCopy.composition && cloneSceneCopy.composition->arrangement.back().id == "chorus" &&
		cloneSceneCopy.composition->patterns.count("leadline_chorus") == 1 &&
		cloneSceneCopy.composition->arrangement.back().tracks.at("lead").patternId == "leadline_chorus",
		"clone_scene with patterns:copy clones referenced patterns and remaps track assignments");


	std::cout << "[SUMMARY] sibyl_edit_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures == 0 ? 0 : 1;
}
