#include "MoiraiJSON.hpp"

#include "MoiraiCompiler.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <unordered_set>

namespace moirai {
namespace {

struct Reader {
	JsonResult result;

	void fail(const std::string& code, const std::string& path, const std::string& message) {
		result.errors.push_back({code, path, message});
	}

	bool object(json_t* value, const std::string& path) {
		if (json_is_object(value)) return true;
		fail("invalid_type", path, "expected object");
		return false;
	}

	bool array(json_t* value, const std::string& path) {
		if (json_is_array(value)) return true;
		fail("invalid_type", path, "expected array");
		return false;
	}

	void fields(json_t* value, const std::string& path,
			std::initializer_list<const char*> allowed) {
		if (!json_is_object(value)) return;
		std::unordered_set<std::string> names;
		for (const char* name : allowed) names.insert(name);
		const char* key = nullptr;
		json_t* child = nullptr;
		json_object_foreach(value, key, child) {
			if (!names.count(key)) fail("unknown_field", path + "/" + key, "unknown field in schemaVersion 1");
		}
	}

	json_t* required(json_t* parent, const char* key, const std::string& path) {
		json_t* value = json_object_get(parent, key);
		if (!value) fail("missing_field", path + "/" + key, "required field is missing");
		return value;
	}

	bool number(json_t* value, float& output, const std::string& path) {
		if (!json_is_number(value)) {
			fail("invalid_type", path, "expected number");
			return false;
		}
		const double number = json_number_value(value);
		if (!std::isfinite(number) || number < -std::numeric_limits<float>::max() ||
				number > std::numeric_limits<float>::max()) {
			fail("invalid_number", path, "number must be finite and representable");
			return false;
		}
		output = static_cast<float>(number);
		return true;
	}

	bool integer(json_t* value, int& output, const std::string& path) {
		if (!json_is_integer(value)) {
			fail("invalid_type", path, "expected integer");
			return false;
		}
		const json_int_t number = json_integer_value(value);
		if (number < std::numeric_limits<int>::min() || number > std::numeric_limits<int>::max()) {
			fail("invalid_number", path, "integer is out of range");
			return false;
		}
		output = static_cast<int>(number);
		return true;
	}

	bool string(json_t* value, std::string& output, const std::string& path) {
		if (!json_is_string(value)) {
			fail("invalid_type", path, "expected UTF-8 string");
			return false;
		}
		output = json_string_value(value);
		if (output.size() > 64) fail("limit_exceeded", path, "string may contain at most 64 UTF-8 bytes");
		return true;
	}

	bool boolean(json_t* value, bool& output, const std::string& path) {
		if (!json_is_boolean(value)) {
			fail("invalid_type", path, "expected boolean");
			return false;
		}
		output = json_is_true(value);
		return true;
	}
};

template <typename Enum>
bool enumValue(Reader& reader, json_t* value, Enum& output, const std::string& path,
		std::initializer_list<std::pair<const char*, Enum>> choices) {
	if (!json_is_string(value)) {
		reader.fail("invalid_type", path, "expected string enum");
		return false;
	}
	const char* text = json_string_value(value);
	for (const auto& choice : choices) {
		if (std::strcmp(text, choice.first) == 0) {
			output = choice.second;
			return true;
		}
	}
	reader.fail("invalid_enum", path, "unsupported schemaVersion 1 value");
	return false;
}

void readDuration(Reader& r, json_t* value, Duration& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"ms", "seconds", "beats"});
	int count = 0;
	float parsed = 0.f;
	if (json_t* item = json_object_get(value, "ms")) {
		++count;
		if (r.number(item, parsed, path + "/ms")) {
			output.unit = DurationUnit::SECONDS;
			output.value = parsed * 0.001f;
		}
	}
	if (json_t* item = json_object_get(value, "seconds")) {
		++count;
		if (r.number(item, parsed, path + "/seconds")) {
			output.unit = DurationUnit::SECONDS;
			output.value = parsed;
		}
	}
	if (json_t* item = json_object_get(value, "beats")) {
		++count;
		if (r.number(item, parsed, path + "/beats")) {
			output.unit = DurationUnit::BEATS;
			output.value = parsed;
		}
	}
	if (count != 1) r.fail("invalid_duration", path, "duration must contain exactly one of ms, seconds, or beats");
}

void readCurve(Reader& r, json_t* value, Curve& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"type", "amount"});
	if (json_t* item = r.required(value, "type", path))
		enumValue(r, item, output.type, path + "/type", {
			{"linear", CurveType::LINEAR}, {"smoothstep", CurveType::SMOOTHSTEP},
			{"smootherstep", CurveType::SIGMOID}, {"sigmoid", CurveType::SIGMOID},
			{"hold", CurveType::HOLD}, {"step", CurveType::STEP},
			{"exponential", CurveType::EXPONENTIAL}, {"logarithmic", CurveType::LOGARITHMIC}});
	if (json_t* item = json_object_get(value, "amount")) r.number(item, output.amount, path + "/amount");
}

void readLoopEnd(Reader& r, json_t* value, LoopEnd& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"count", "whileGate"});
	const bool hasCount = json_object_get(value, "count");
	const bool hasWhile = json_object_get(value, "whileGate");
	if (hasCount == hasWhile) {
		r.fail("invalid_loop", path, "loopEnd requires exactly one of count or whileGate");
		return;
	}
	if (hasCount) {
		output.mode = LoopMode::COUNTED;
		r.integer(json_object_get(value, "count"), output.count, path + "/count");
	} else {
		bool enabled = false;
		if (r.boolean(json_object_get(value, "whileGate"), enabled, path + "/whileGate") && enabled)
			output.mode = LoopMode::WHILE_GATE;
		else r.fail("invalid_loop", path + "/whileGate", "whileGate must be true");
	}
}

void readStage(Reader& r, json_t* value, Stage& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"id", "to", "duration", "curve", "loopStart", "loopEnd"});
	if (json_t* item = r.required(value, "id", path)) r.string(item, output.id, path + "/id");
	if (json_t* item = r.required(value, "to", path)) r.number(item, output.target, path + "/to");
	if (json_t* item = r.required(value, "duration", path)) readDuration(r, item, output.duration, path + "/duration");
	if (json_t* item = r.required(value, "curve", path)) readCurve(r, item, output.curve, path + "/curve");
	if (json_t* item = json_object_get(value, "loopStart")) r.boolean(item, output.loopStart, path + "/loopStart");
	if (json_t* item = json_object_get(value, "loopEnd")) readLoopEnd(r, item, output.loopEnd, path + "/loopEnd");
}

void readPath(Reader& r, json_t* value, std::vector<Stage>& output, const std::string& path) {
	if (!r.array(value, path)) return;
	const size_t count = json_array_size(value);
	output.reserve(count);
	for (size_t index = 0; index < count; ++index) {
		Stage stage;
		readStage(r, json_array_get(value, index), stage, path + "/" + std::to_string(index));
		output.push_back(std::move(stage));
	}
}

void readBinding(Reader& r, json_t* value, MacroBinding& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"source", "target", "inputRange", "outputRange", "mapping", "sampling", "smoothingMs", "variants"});
	if (json_t* item = r.required(value, "source", path))
		enumValue(r, item, output.source, path + "/source", {{"velocity", MacroSource::VELOCITY}, {"m1", MacroSource::M1}, {"m2", MacroSource::M2}, {"m3", MacroSource::M3}});
	if (json_t* item = r.required(value, "target", path))
		enumValue(r, item, output.target, path + "/target", {{"timeScale", MacroTarget::TIME_SCALE}, {"curveBias", MacroTarget::CURVE_BIAS}, {"levelScale", MacroTarget::LEVEL_SCALE}, {"levelOffset", MacroTarget::LEVEL_OFFSET}, {"variantSelect", MacroTarget::VARIANT_SELECT}});
	auto range = [&](const char* key, float& low, float& high) {
		json_t* item = r.required(value, key, path);
		const std::string rangePath = path + "/" + key;
		if (!item || !r.array(item, rangePath)) return;
		if (json_array_size(item) != 2) {
			r.fail("invalid_binding", rangePath, "range must contain exactly two numbers");
			return;
		}
		r.number(json_array_get(item, 0), low, rangePath + "/0");
		r.number(json_array_get(item, 1), high, rangePath + "/1");
	};
	range("inputRange", output.inputMin, output.inputMax);
	range("outputRange", output.outputMin, output.outputMax);
	if (json_t* item = json_object_get(value, "mapping")) enumValue(r, item, output.mapping, path + "/mapping", {{"linear", MacroMapping::LINEAR}, {"exponential", MacroMapping::EXPONENTIAL}});
	if (json_t* item = json_object_get(value, "sampling")) enumValue(r, item, output.sampling, path + "/sampling", {{"onTrigger", MacroSampling::ON_TRIGGER}, {"continuous", MacroSampling::CONTINUOUS}});
	if (json_t* item = json_object_get(value, "smoothingMs")) r.number(item, output.smoothingMs, path + "/smoothingMs");
	if (json_t* item = json_object_get(value, "variants")) {
		if (r.array(item, path + "/variants")) {
			for (size_t index = 0; index < json_array_size(item); ++index) {
				std::string id;
				r.string(json_array_get(item, index), id, path + "/variants/" + std::to_string(index));
				output.variants.push_back(std::move(id));
			}
		}
	}
}

void readCommonProgram(Reader& r, json_t* value, Program& output, const std::string& path) {
	if (json_t* item = r.required(value, "name", path)) r.string(item, output.name, path + "/name");
	if (json_t* item = r.required(value, "kind", path)) enumValue(r, item, output.kind, path + "/kind", {{"staged", ProgramKind::STAGED}, {"contour", ProgramKind::CONTOUR}});
	if (json_t* item = r.required(value, "mode", path)) enumValue(r, item, output.mode, path + "/mode", {{"gate", ProgramMode::GATE}, {"oneShot", ProgramMode::ONE_SHOT}, {"cycle", ProgramMode::CYCLE}});
	if (json_t* item = json_object_get(value, "retrigger")) enumValue(r, item, output.retrigger, path + "/retrigger", {{"restart", RetriggerPolicy::RESTART}, {"fromCurrent", RetriggerPolicy::FROM_CURRENT}, {"legato", RetriggerPolicy::LEGATO}, {"ignoreWhileRunning", RetriggerPolicy::IGNORE_WHILE_RUNNING}});
	if (json_t* item = json_object_get(value, "variation")) {
		if (r.object(item, path + "/variation")) {
			r.fields(item, path + "/variation", {"level", "time"});
			if (json_t* field = json_object_get(item, "level")) r.number(field, output.variation.level, path + "/variation/level");
			if (json_t* field = json_object_get(item, "time")) r.number(field, output.variation.time, path + "/variation/time");
		}
	}
	if (json_t* item = json_object_get(value, "velocityDefault")) r.boolean(item, output.velocityDefault, path + "/velocityDefault");
	if (json_t* item = json_object_get(value, "macroBindings")) {
		if (r.array(item, path + "/macroBindings")) {
			for (size_t index = 0; index < json_array_size(item); ++index) {
				MacroBinding binding;
				readBinding(r, json_array_get(item, index), binding, path + "/macroBindings/" + std::to_string(index));
				output.macroBindings.push_back(std::move(binding));
			}
		}
	}
}

void readProgram(Reader& r, const std::string& id, json_t* value, Program& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"name", "kind", "mode", "gatePath", "sustain", "releasePath", "duration", "points", "interpolation", "retrigger", "variation", "macroBindings", "velocityDefault"});
	output.id = id;
	readCommonProgram(r, value, output, path);
	if (output.kind == ProgramKind::STAGED) {
		if (json_t* item = r.required(value, "gatePath", path)) readPath(r, item, output.gatePath, path + "/gatePath");
		if (json_t* item = r.required(value, "releasePath", path)) readPath(r, item, output.releasePath, path + "/releasePath");
		if (json_t* item = json_object_get(value, "sustain")) {
			if (r.object(item, path + "/sustain")) {
				r.fields(item, path + "/sustain", {"mode"});
				std::string mode;
				if (json_t* modeJson = r.required(item, "mode", path + "/sustain")) {
					r.string(modeJson, mode, path + "/sustain/mode");
					if (mode != "hold") r.fail("invalid_enum", path + "/sustain/mode", "only hold is supported in schemaVersion 1");
				}
			}
			output.sustainHold = true;
		} else output.sustainHold = false;
	} else {
		if (json_t* item = r.required(value, "duration", path)) readDuration(r, item, output.duration, path + "/duration");
		if (json_t* item = r.required(value, "points", path)) {
			if (r.array(item, path + "/points")) {
				for (size_t index = 0; index < json_array_size(item); ++index) {
					const std::string pointPath = path + "/points/" + std::to_string(index);
					json_t* point = json_array_get(item, index);
					ContourPoint parsed;
					if (r.object(point, pointPath)) {
						r.fields(point, pointPath, {"t", "v"});
						if (json_t* field = r.required(point, "t", pointPath)) r.number(field, parsed.time, pointPath + "/t");
						if (json_t* field = r.required(point, "v", pointPath)) r.number(field, parsed.value, pointPath + "/v");
					}
					output.points.push_back(parsed);
				}
			}
		}
		if (json_t* item = json_object_get(value, "interpolation")) enumValue(r, item, output.interpolation, path + "/interpolation", {{"linear", Interpolation::LINEAR}, {"monotoneCubic", Interpolation::MONOTONE_CUBIC}});
	}
}

void readSparseStrings(Reader& r, json_t* value, std::array<std::string, kMaxChannels>& output, const std::string& path) {
	if (!r.object(value, path)) return;
	const char* key = nullptr;
	json_t* child = nullptr;
	json_object_foreach(value, key, child) {
		char* end = nullptr;
		const long channel = std::strtol(key, &end, 10);
		if (!end || *end != '\0' || channel < 0 || channel >= kMaxChannels) {
			r.fail("invalid_channel", path + "/" + key, "channel key must be 0 through 15");
			continue;
		}
		r.string(child, output[channel], path + "/" + key);
	}
}

void readLane(Reader& r, json_t* value, Lane& output, const std::string& path) {
	if (!r.object(value, path)) return;
	r.fields(value, path, {"defaultProgram", "outputMode", "eocSource", "assignments", "channelLabels"});
	if (json_t* item = r.required(value, "defaultProgram", path)) r.string(item, output.defaultProgram, path + "/defaultProgram");
	if (json_t* item = r.required(value, "outputMode", path)) enumValue(r, item, output.outputMode, path + "/outputMode", {{"0_10", OutputMode::UNIPOLAR_10}, {"0_5", OutputMode::UNIPOLAR_5}, {"bipolar_5", OutputMode::BIPOLAR_5}});
	if (json_t* item = r.required(value, "eocSource", path)) enumValue(r, item, output.eocSource, path + "/eocSource", {{"program", EocSource::PROGRAM}, {"loop", EocSource::LOOP}});
	if (json_t* item = r.required(value, "assignments", path)) readSparseStrings(r, item, output.assignments, path + "/assignments");
	if (json_t* item = r.required(value, "channelLabels", path)) readSparseStrings(r, item, output.channelLabels, path + "/channelLabels");
}

const char* curveName(CurveType value) { switch (value) { case CurveType::LINEAR:return "linear"; case CurveType::SMOOTHSTEP:return "smoothstep"; case CurveType::SIGMOID:return "sigmoid"; case CurveType::HOLD:return "hold"; case CurveType::STEP:return "step"; case CurveType::EXPONENTIAL:return "exponential"; case CurveType::LOGARITHMIC:return "logarithmic"; } return "linear"; }
const char* retriggerName(RetriggerPolicy value) { switch (value) { case RetriggerPolicy::RESTART:return "restart"; case RetriggerPolicy::FROM_CURRENT:return "fromCurrent"; case RetriggerPolicy::LEGATO:return "legato"; case RetriggerPolicy::IGNORE_WHILE_RUNNING:return "ignoreWhileRunning"; } return "restart"; }

json_t* writeDuration(const Duration& value) {
	json_t* output = json_object();
	json_object_set_new(output, value.unit == DurationUnit::BEATS ? "beats" : "seconds", json_real(value.value));
	return output;
}

json_t* writeCurve(const Curve& value) {
	return json_pack("{s:s,s:f}", "type", curveName(value.type), "amount", value.amount);
}

json_t* writeStage(const Stage& value) {
	json_t* output = json_pack("{s:s,s:f,s:o,s:o}", "id", value.id.c_str(), "to", value.target,
		"duration", writeDuration(value.duration), "curve", writeCurve(value.curve));
	if (value.loopStart) json_object_set_new(output, "loopStart", json_true());
	if (value.loopEnd.mode == LoopMode::COUNTED) json_object_set_new(output, "loopEnd", json_pack("{s:i}", "count", value.loopEnd.count));
	else if (value.loopEnd.mode == LoopMode::WHILE_GATE) json_object_set_new(output, "loopEnd", json_pack("{s:b}", "whileGate", 1));
	return output;
}

json_t* writePath(const std::vector<Stage>& path) {
	json_t* output = json_array();
	for (const Stage& stage : path) json_array_append_new(output, writeStage(stage));
	return output;
}

json_t* writeBinding(const MacroBinding& value) {
	const char* source[] = {"velocity", "m1", "m2", "m3"};
	const char* target[] = {"timeScale", "curveBias", "levelScale", "levelOffset", "variantSelect"};
	json_t* output = json_pack("{s:s,s:s,s:[f,f],s:[f,f],s:s,s:s,s:f}",
		"source", source[static_cast<int>(value.source)], "target", target[static_cast<int>(value.target)],
		"inputRange", value.inputMin, value.inputMax, "outputRange", value.outputMin, value.outputMax,
		"mapping", value.mapping == MacroMapping::LINEAR ? "linear" : "exponential",
		"sampling", value.sampling == MacroSampling::ON_TRIGGER ? "onTrigger" : "continuous",
		"smoothingMs", value.smoothingMs);
	if (!value.variants.empty()) {
		json_t* variants = json_array();
		for (const std::string& id : value.variants) json_array_append_new(variants, json_string(id.c_str()));
		json_object_set_new(output, "variants", variants);
	}
	return output;
}

json_t* writeProgram(const Program& value) {
	const char* mode[] = {"gate", "oneShot", "cycle"};
	json_t* output = json_pack("{s:s,s:s,s:s}", "name", value.name.c_str(),
		"kind", value.kind == ProgramKind::STAGED ? "staged" : "contour", "mode", mode[static_cast<int>(value.mode)]);
	if (value.kind == ProgramKind::STAGED) {
		json_object_set_new(output, "gatePath", writePath(value.gatePath));
		if (value.sustainHold) json_object_set_new(output, "sustain", json_pack("{s:s}", "mode", "hold"));
		json_object_set_new(output, "releasePath", writePath(value.releasePath));
	} else {
		json_object_set_new(output, "duration", writeDuration(value.duration));
		json_t* points = json_array();
		for (const ContourPoint& point : value.points) json_array_append_new(points, json_pack("{s:f,s:f}", "t", point.time, "v", point.value));
		json_object_set_new(output, "points", points);
		json_object_set_new(output, "interpolation", json_string(value.interpolation == Interpolation::LINEAR ? "linear" : "monotoneCubic"));
	}
	json_object_set_new(output, "retrigger", json_string(retriggerName(value.retrigger)));
	json_object_set_new(output, "variation", json_pack("{s:f,s:f}", "level", value.variation.level, "time", value.variation.time));
	json_t* bindings = json_array();
	for (const MacroBinding& binding : value.macroBindings) json_array_append_new(bindings, writeBinding(binding));
	json_object_set_new(output, "macroBindings", bindings);
	json_object_set_new(output, "velocityDefault", json_boolean(value.velocityDefault));
	return output;
}

json_t* writeSparseStrings(const std::array<std::string, kMaxChannels>& values) {
	json_t* output = json_object();
	for (int channel = 0; channel < kMaxChannels; ++channel)
		if (!values[channel].empty()) json_object_set_new(output, std::to_string(channel).c_str(), json_string(values[channel].c_str()));
	return output;
}

json_t* writeLane(const Lane& value) {
	const char* outputMode[] = {"0_10", "0_5", "bipolar_5"};
	json_t* output = json_pack("{s:s,s:s,s:s}", "defaultProgram", value.defaultProgram.c_str(),
		"outputMode", outputMode[static_cast<int>(value.outputMode)], "eocSource", value.eocSource == EocSource::PROGRAM ? "program" : "loop");
	json_object_set_new(output, "assignments", writeSparseStrings(value.assignments));
	json_object_set_new(output, "channelLabels", writeSparseStrings(value.channelLabels));
	return output;
}

} // namespace

JsonResult parseBankJson(json_t* root) {
	Reader r;
	if (!r.object(root, "")) return r.result;
	char* compact = json_dumps(root, JSON_COMPACT);
	if (!compact) {
		r.fail("invalid_json", "", "document could not be serialized");
		return r.result;
	}
	const size_t bytes = std::strlen(compact);
	std::free(compact);
	if (bytes > kMaxDocumentBytes) {
		r.fail("document_too_large", "", "document exceeds the 1 MiB safety limit");
		return r.result;
	}
	r.fields(root, "", {"schemaVersion", "revision", "seed", "clock", "lanes", "programs"});
	Bank& bank = r.result.bank;
	if (json_t* item = r.required(root, "schemaVersion", "")) r.integer(item, bank.schemaVersion, "/schemaVersion");
	if (json_t* item = r.required(root, "revision", "")) r.integer(item, bank.revision, "/revision");
	if (json_t* item = r.required(root, "seed", "")) {
		if (!json_is_integer(item) || json_integer_value(item) < 0) r.fail("invalid_number", "/seed", "seed must be a nonnegative integer");
		else bank.seed = static_cast<uint64_t>(json_integer_value(item));
	}
	if (json_t* clock = r.required(root, "clock", "")) {
		if (r.object(clock, "/clock")) {
			r.fields(clock, "/clock", {"externalPpqn", "fallbackBpm", "lossTimeoutMs", "onClockLoss"});
			if (json_t* item = r.required(clock, "externalPpqn", "/clock")) r.integer(item, bank.clock.externalPpqn, "/clock/externalPpqn");
			if (json_t* item = r.required(clock, "fallbackBpm", "/clock")) r.number(item, bank.clock.fallbackBpm, "/clock/fallbackBpm");
			if (json_t* item = r.required(clock, "lossTimeoutMs", "/clock")) r.number(item, bank.clock.lossTimeoutMs, "/clock/lossTimeoutMs");
			if (json_t* item = r.required(clock, "onClockLoss", "/clock")) enumValue(r, item, bank.clock.onClockLoss, "/clock/onClockLoss", {{"holdTempo", ClockLossPolicy::HOLD_TEMPO}, {"fallback", ClockLossPolicy::FALLBACK}});
		}
	}
	if (json_t* lanes = r.required(root, "lanes", "")) {
		if (r.object(lanes, "/lanes")) {
			r.fields(lanes, "/lanes", {"A", "B"});
			if (json_t* lane = r.required(lanes, "A", "/lanes")) readLane(r, lane, bank.lanes[0], "/lanes/A");
			if (json_t* lane = r.required(lanes, "B", "/lanes")) readLane(r, lane, bank.lanes[1], "/lanes/B");
		}
	}
	if (json_t* programs = r.required(root, "programs", "")) {
		if (r.object(programs, "/programs")) {
			const char* id = nullptr;
			json_t* programJson = nullptr;
			json_object_foreach(programs, id, programJson) {
				Program program;
				readProgram(r, id, programJson, program, "/programs/" + std::string(id));
				bank.programs[id] = std::move(program);
			}
		}
	}
	if (r.result.errors.empty()) {
		CompileResult compiled = compileBank(bank);
		if (!compiled.valid) r.result.errors = std::move(compiled.errors);
	}
	r.result.valid = r.result.errors.empty();
	return r.result;
}

json_t* bankToJson(const Bank& bank) {
	json_t* root = json_object();
	json_object_set_new(root, "schemaVersion", json_integer(bank.schemaVersion));
	json_object_set_new(root, "revision", json_integer(bank.revision));
	json_object_set_new(root, "seed", json_integer(static_cast<json_int_t>(bank.seed)));
	json_object_set_new(root, "clock", json_pack("{s:i,s:f,s:f,s:s}", "externalPpqn", bank.clock.externalPpqn,
		"fallbackBpm", bank.clock.fallbackBpm, "lossTimeoutMs", bank.clock.lossTimeoutMs,
		"onClockLoss", bank.clock.onClockLoss == ClockLossPolicy::HOLD_TEMPO ? "holdTempo" : "fallback"));
	json_t* lanes = json_object();
	json_object_set_new(lanes, "A", writeLane(bank.lanes[0]));
	json_object_set_new(lanes, "B", writeLane(bank.lanes[1]));
	json_object_set_new(root, "lanes", lanes);
	json_t* programs = json_object();
	std::vector<std::string> ids;
	ids.reserve(bank.programs.size());
	for (const auto& entry : bank.programs) ids.push_back(entry.first);
	std::sort(ids.begin(), ids.end());
	for (const std::string& id : ids) json_object_set_new(programs, id.c_str(), writeProgram(bank.programs.at(id)));
	json_object_set_new(root, "programs", programs);
	return root;
}

} // namespace moirai
