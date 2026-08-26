#include "MoiraiCompiler.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace moirai {
namespace {

bool finite(float value) {
	return std::isfinite(value);
}

bool validId(const std::string& id) {
	if (id.empty() || id.size() > 64) return false;
	for (char c : id) {
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
	}
	return true;
}

uint32_t stableHash(const std::string& text) {
	uint32_t hash = 2166136261u;
	for (unsigned char c : text) {
		hash ^= c;
		hash *= 16777619u;
	}
	return hash;
}

void error(CompileResult& result, const std::string& code,
		const std::string& path, const std::string& message) {
	result.errors.push_back({code, path, message});
}

bool validateDuration(const Duration& duration, CompileResult& result,
		const std::string& path) {
	if (!finite(duration.value)) {
		error(result, "invalid_duration", path, "duration must be finite");
		return false;
	}
	const float minimum = duration.unit == DurationUnit::SECONDS ? 0.00001f : 1.f / 4096.f;
	const float maximum = duration.unit == DurationUnit::SECONDS ? 600.f : 1024.f;
	if (duration.value < minimum || duration.value > maximum) {
		error(result, "invalid_duration", path,
			duration.unit == DurationUnit::SECONDS
				? "absolute duration must be between 0.01 and 600000 milliseconds"
				: "beat duration must be between 1/4096 and 1024 beats");
		return false;
	}
	return true;
}

void addDurationSummary(const Duration& duration, float& seconds, float& beats) {
	if (duration.unit == DurationUnit::SECONDS) seconds += duration.value;
	else beats += duration.value;
}

bool validateCurve(const Curve& curve, CompileResult& result, const std::string& path) {
	if (!finite(curve.amount) || curve.amount < -1.f || curve.amount > 1.f) {
		error(result, "invalid_curve", path + "/amount", "curve amount must be finite and between -1 and 1");
		return false;
	}
	return true;
}

void compilePath(const std::vector<Stage>& authored, std::vector<CompiledStage>& compiled,
		CompileResult& result, const std::string& path, CompiledProgram& program,
		bool gatePath) {
	if (authored.size() > static_cast<size_t>(kMaxPathSegments)) {
		error(result, "limit_exceeded", path, "path may contain at most 32 segments");
		return;
	}
	compiled.reserve(authored.size());
	int loopStart = -1;
	int loopEnd = -1;
	LoopMode loopMode = LoopMode::NONE;
	int loopCount = 1;
	for (size_t index = 0; index < authored.size(); ++index) {
		const Stage& stage = authored[index];
		const std::string stagePath = path + "/" + std::to_string(index);
		if (!validId(stage.id))
			error(result, "invalid_id", stagePath + "/id", "stage id must match [A-Za-z0-9_-]{1,64}");
		if (!finite(stage.target) || stage.target < 0.f || stage.target > 1.f)
			error(result, "invalid_level", stagePath + "/to", "stage target must be finite and between 0 and 1");
		validateDuration(stage.duration, result, stagePath + "/duration");
		validateCurve(stage.curve, result, stagePath + "/curve");
		if (stage.loopStart) {
			if (!gatePath || loopStart >= 0)
				error(result, "invalid_loop", stagePath + "/loopStart", "exactly one forward loop start is allowed in gatePath");
			else loopStart = static_cast<int>(index);
		}
		if (stage.loopEnd.mode != LoopMode::NONE) {
			if (!gatePath || loopEnd >= 0) {
				error(result, "invalid_loop", stagePath + "/loopEnd", "exactly one forward loop end is allowed in gatePath");
			} else {
				loopEnd = static_cast<int>(index);
				loopMode = stage.loopEnd.mode;
				loopCount = stage.loopEnd.count;
				if (loopMode == LoopMode::COUNTED && (loopCount < 1 || loopCount > 1024))
					error(result, "invalid_loop", stagePath + "/loopEnd/count", "loop count must be between 1 and 1024");
			}
		}
		CompiledStage output;
		output.target = stage.target;
		output.duration = stage.duration;
		output.curve = stage.curve;
		compiled.push_back(output);
		program.authoredPeak = std::max(program.authoredPeak, finite(stage.target) ? stage.target : 0.f);
		if (gatePath) addDurationSummary(stage.duration, program.gateDurationSeconds, program.gateDurationBeats);
		else addDurationSummary(stage.duration, program.releaseDurationSeconds, program.releaseDurationBeats);
	}
	if (gatePath) {
		if ((loopStart >= 0) != (loopEnd >= 0))
			error(result, "invalid_loop", path, "loopStart and loopEnd must be supplied together");
		else if (loopStart >= 0 && loopStart > loopEnd)
			error(result, "invalid_loop", path, "loopStart must not follow loopEnd");
		else if (loopStart >= 0) {
			program.loopStart = loopStart;
			program.loopEnd = loopEnd;
			program.loopMode = loopMode;
			program.loopCount = loopCount;
		}
	}
}

void computeMonotoneTangents(std::vector<CompiledContourPoint>& points) {
	const size_t count = points.size();
	if (count < 2) return;
	std::vector<float> spans(count - 1);
	std::vector<float> slopes(count - 1);
	for (size_t index = 0; index + 1 < count; ++index) {
		spans[index] = points[index + 1].time - points[index].time;
		slopes[index] = spans[index] > 0.f
			? (points[index + 1].value - points[index].value) / spans[index] : 0.f;
	}
	points.front().tangent = slopes.front();
	points.back().tangent = slopes.back();
	for (size_t index = 1; index + 1 < count; ++index) {
		const float before = slopes[index - 1];
		const float after = slopes[index];
		if (before == 0.f || after == 0.f || (before < 0.f) != (after < 0.f)) {
			points[index].tangent = 0.f;
			continue;
		}
		const float firstWeight = 2.f * spans[index] + spans[index - 1];
		const float secondWeight = spans[index] + 2.f * spans[index - 1];
		points[index].tangent = (firstWeight + secondWeight)
			/ (firstWeight / before + secondWeight / after);
	}
}

void compileMacros(const Program& authored, CompiledProgram& compiled,
		CompileResult& result, const std::string& path) {
	if (authored.macroBindings.size() > static_cast<size_t>(kMaxMacroBindings)) {
		error(result, "limit_exceeded", path + "/macroBindings", "program may contain at most 16 macro bindings");
		return;
	}
	bool hasVelocityBinding = false;
	for (size_t index = 0; index < authored.macroBindings.size(); ++index) {
		const MacroBinding& binding = authored.macroBindings[index];
		const std::string bindingPath = path + "/macroBindings/" + std::to_string(index);
		hasVelocityBinding = hasVelocityBinding || binding.source == MacroSource::VELOCITY;
		if (!finite(binding.inputMin) || !finite(binding.inputMax) || binding.inputMax <= binding.inputMin)
			error(result, "invalid_binding", bindingPath + "/inputRange", "input range must be finite and increasing");
		if (!finite(binding.outputMin) || !finite(binding.outputMax))
			error(result, "invalid_binding", bindingPath + "/outputRange", "output range must be finite");
		if ((binding.target == MacroTarget::TIME_SCALE || binding.target == MacroTarget::VARIANT_SELECT)
				&& binding.sampling != MacroSampling::ON_TRIGGER)
			error(result, "invalid_binding", bindingPath + "/sampling", "structural targets must be sampled on trigger");
		if (binding.sampling == MacroSampling::CONTINUOUS &&
				(!finite(binding.smoothingMs) || binding.smoothingMs < 1.f || binding.smoothingMs > 1000.f))
			error(result, "invalid_binding", bindingPath + "/smoothingMs", "continuous smoothing must be between 1 and 1000 milliseconds");
		if (binding.target == MacroTarget::VARIANT_SELECT) {
			if (binding.variants.empty() || binding.variants.size() > static_cast<size_t>(kMaxVariants))
				error(result, "invalid_binding", bindingPath + "/variants", "variant selection requires 1 to 32 program ids");
		} else if (!binding.variants.empty()) {
			error(result, "invalid_binding", bindingPath + "/variants", "variants are only valid for variantSelect");
		}
		CompiledMacroBinding output;
		output.source = binding.source;
		output.target = binding.target;
		output.inputMin = binding.inputMin;
		output.inverseInputSpan = binding.inputMax > binding.inputMin
			? 1.f / (binding.inputMax - binding.inputMin) : 0.f;
		output.outputMin = binding.outputMin;
		output.outputMax = binding.outputMax;
		output.mapping = binding.mapping;
		output.sampling = binding.sampling;
		output.smoothingMs = binding.smoothingMs;
		compiled.macroBindings.push_back(output);
	}
	if (authored.velocityDefault && !hasVelocityBinding) {
		CompiledMacroBinding velocity;
		velocity.source = MacroSource::VELOCITY;
		velocity.target = MacroTarget::LEVEL_SCALE;
		velocity.inputMin = 0.f;
		velocity.inverseInputSpan = 0.1f;
		velocity.outputMin = 0.f;
		velocity.outputMax = 1.f;
		compiled.macroBindings.push_back(velocity);
	}
}

CompiledProgram compileProgram(const std::string& id, const Program& authored,
		CompileResult& result) {
	CompiledProgram compiled;
	compiled.id = id;
	compiled.name = authored.name.empty() ? id : authored.name;
	compiled.stableIdHash = stableHash(id);
	compiled.kind = authored.kind;
	compiled.mode = authored.mode;
	compiled.sustainHold = authored.sustainHold;
	compiled.duration = authored.duration;
	compiled.interpolation = authored.interpolation;
	compiled.retrigger = authored.retrigger;
	compiled.variation = authored.variation;
	const std::string path = "/programs/" + id;
	if (!validId(id))
		error(result, "invalid_id", path, "program id must match [A-Za-z0-9_-]{1,64}");
	if (!authored.id.empty() && authored.id != id)
		error(result, "invalid_id", path + "/id", "stored program id must match its object key");
	if (compiled.name.size() > 64)
		error(result, "limit_exceeded", path + "/name", "program name may contain at most 64 UTF-8 bytes");
	if (!finite(authored.variation.level) || authored.variation.level < 0.f || authored.variation.level > 1.f)
		error(result, "invalid_variation", path + "/variation/level", "level variation must be between 0 and 1");
	if (!finite(authored.variation.time) || authored.variation.time < 0.f || authored.variation.time > 1.f)
		error(result, "invalid_variation", path + "/variation/time", "time variation must be between 0 and 1");

	if (authored.kind == ProgramKind::STAGED) {
		if (authored.mode == ProgramMode::CYCLE)
			error(result, "invalid_program", path + "/mode", "staged programs support gate or oneShot mode");
		if (authored.gatePath.empty())
			error(result, "invalid_program", path + "/gatePath", "staged programs require a nonempty gate path");
		if (authored.mode == ProgramMode::GATE && authored.releasePath.empty())
			error(result, "invalid_program", path + "/releasePath", "staged gate programs require a nonempty release path");
		if (authored.mode == ProgramMode::ONE_SHOT && authored.sustainHold)
			error(result, "invalid_program", path + "/sustain", "staged oneShot programs cannot hold sustain");
		compilePath(authored.gatePath, compiled.gatePath, result, path + "/gatePath", compiled, true);
		compilePath(authored.releasePath, compiled.releasePath, result, path + "/releasePath", compiled, false);
	} else {
		if (authored.mode == ProgramMode::GATE)
			error(result, "invalid_program", path + "/mode", "contour programs support oneShot or cycle mode");
		validateDuration(authored.duration, result, path + "/duration");
		if (authored.points.size() < 2 || authored.points.size() > static_cast<size_t>(kMaxContourPointsPerProgram))
			error(result, "invalid_contour", path + "/points", "contours require 2 to 256 points");
		compiled.points.reserve(authored.points.size());
		for (size_t index = 0; index < authored.points.size(); ++index) {
			const ContourPoint& point = authored.points[index];
			const std::string pointPath = path + "/points/" + std::to_string(index);
			if (!finite(point.time) || point.time < 0.f || point.time > 1.f)
				error(result, "invalid_contour", pointPath + "/t", "point time must be finite and between 0 and 1");
			if (!finite(point.value) || point.value < 0.f || point.value > 1.f)
				error(result, "invalid_contour", pointPath + "/v", "point value must be finite and between 0 and 1");
			if (index > 0 && !(point.time > authored.points[index - 1].time))
				error(result, "invalid_contour", pointPath + "/t", "point times must be strictly increasing");
			compiled.points.push_back({point.time, point.value, 0.f});
			if (finite(point.value)) compiled.authoredPeak = std::max(compiled.authoredPeak, point.value);
		}
		if (!authored.points.empty() && authored.points.front().time != 0.f)
			error(result, "invalid_contour", path + "/points/0/t", "first contour point must be exactly 0");
		if (!authored.points.empty() && authored.points.back().time != 1.f)
			error(result, "invalid_contour", path + "/points/" +
				std::to_string(authored.points.size() - 1) + "/t", "last contour point must be exactly 1");
		if (compiled.points.size() >= 2) computeMonotoneTangents(compiled.points);
	}
	compileMacros(authored, compiled, result, path);
	return compiled;
}

bool programHasVariantBinding(const Program& program) {
	for (const MacroBinding& binding : program.macroBindings)
		if (binding.target == MacroTarget::VARIANT_SELECT) return true;
	return false;
}

} // namespace

CompileResult compileBank(const Bank& authored) {
	CompileResult result;
	std::shared_ptr<CompiledBank> compiled(new CompiledBank());
	compiled->revision = authored.revision;
	compiled->seed = authored.seed;
	compiled->clock = authored.clock;
	if (authored.schemaVersion != 1)
		error(result, "unsupported_schema", "/schemaVersion", "Moirai requires schemaVersion 1");
	if (authored.revision < 0)
		error(result, "invalid_revision", "/revision", "revision must be nonnegative");
	if (authored.programs.empty() || authored.programs.size() > static_cast<size_t>(kMaxPrograms))
		error(result, "limit_exceeded", "/programs", "bank must contain 1 to 32 authored programs");
	if (authored.clock.externalPpqn < 1 || authored.clock.externalPpqn > 96)
		error(result, "invalid_clock", "/clock/externalPpqn", "externalPpqn must be between 1 and 96");
	if (!finite(authored.clock.fallbackBpm) || authored.clock.fallbackBpm < 20.f || authored.clock.fallbackBpm > 400.f)
		error(result, "invalid_clock", "/clock/fallbackBpm", "fallbackBpm must be finite and between 20 and 400");
	if (!finite(authored.clock.lossTimeoutMs) || authored.clock.lossTimeoutMs < 100.f || authored.clock.lossTimeoutMs > 10000.f)
		error(result, "invalid_clock", "/clock/lossTimeoutMs", "lossTimeoutMs must be finite and between 100 and 10000");

	std::vector<std::string> ids;
	ids.reserve(authored.programs.size());
	for (const auto& entry : authored.programs) ids.push_back(entry.first);
	std::sort(ids.begin(), ids.end());
	size_t totalContourPoints = 0;
	for (const std::string& id : ids) {
		const Program& program = authored.programs.at(id);
		totalContourPoints += program.kind == ProgramKind::CONTOUR ? program.points.size() : 0;
		const int index = static_cast<int>(compiled->programs.size());
		compiled->programIndexById[id] = index;
		compiled->programs.push_back(compileProgram(id, program, result));
	}
	if (totalContourPoints > static_cast<size_t>(kMaxContourPointsPerBank))
		error(result, "limit_exceeded", "/programs", "bank may contain at most 8192 contour points");

	for (size_t programIndex = 0; programIndex < ids.size(); ++programIndex) {
		const Program& authoredProgram = authored.programs.at(ids[programIndex]);
		CompiledProgram& compiledProgram = compiled->programs[programIndex];
		for (size_t bindingIndex = 0; bindingIndex < authoredProgram.macroBindings.size(); ++bindingIndex) {
			const MacroBinding& authoredBinding = authoredProgram.macroBindings[bindingIndex];
			if (authoredBinding.target != MacroTarget::VARIANT_SELECT) continue;
			if (bindingIndex >= compiledProgram.macroBindings.size()) continue;
			for (size_t variantIndex = 0; variantIndex < authoredBinding.variants.size(); ++variantIndex) {
				const std::string& variantId = authoredBinding.variants[variantIndex];
				auto found = compiled->programIndexById.find(variantId);
				const std::string path = "/programs/" + ids[programIndex] + "/macroBindings/"
					+ std::to_string(bindingIndex) + "/variants/" + std::to_string(variantIndex);
				if (found == compiled->programIndexById.end()) {
					error(result, "missing_program", path, "variant target does not exist");
					continue;
				}
				if (variantId == ids[programIndex] || programHasVariantBinding(authored.programs.at(variantId))) {
					error(result, "variant_cycle", path, "variant targets cannot self-reference or contain variant selection");
					continue;
				}
				compiledProgram.macroBindings[bindingIndex].variantProgramIndices.push_back(found->second);
			}
		}
	}

	for (int laneIndex = 0; laneIndex < kLaneCount; ++laneIndex) {
		const Lane& lane = authored.lanes[laneIndex];
		CompiledLane& output = compiled->lanes[laneIndex];
		output.outputMode = lane.outputMode;
		output.eocSource = lane.eocSource;
		const std::string lanePath = std::string("/lanes/") + (laneIndex == 0 ? "A" : "B");
		auto defaultProgram = compiled->programIndexById.find(lane.defaultProgram);
		if (defaultProgram == compiled->programIndexById.end())
			error(result, "missing_program", lanePath + "/defaultProgram", "lane default program does not exist");
		else output.defaultProgram = defaultProgram->second;
		for (int channel = 0; channel < kMaxChannels; ++channel) {
			if (lane.channelLabels[channel].size() > 64)
				error(result, "limit_exceeded", lanePath + "/channelLabels/" + std::to_string(channel),
					"channel label may contain at most 64 UTF-8 bytes");
			output.channelLabels[channel] = lane.channelLabels[channel];
			if (lane.assignments[channel].empty()) continue;
			auto assignment = compiled->programIndexById.find(lane.assignments[channel]);
			if (assignment == compiled->programIndexById.end())
				error(result, "missing_program", lanePath + "/assignments/" + std::to_string(channel),
					"assigned program does not exist");
			else output.assignments[channel] = assignment->second;
		}
	}

	result.valid = result.errors.empty();
	if (result.valid) result.bank = compiled;
	return result;
}

} // namespace moirai
