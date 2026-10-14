#include "MoiraiPresets.hpp"

#include <algorithm>
#include <cmath>

namespace moirai {
namespace {

Duration milliseconds(float value) {
	Duration duration;
	duration.unit = DurationUnit::SECONDS;
	duration.value = value * 0.001f;
	return duration;
}

Duration beats(float value) {
	Duration duration;
	duration.unit = DurationUnit::BEATS;
	duration.value = value;
	return duration;
}

Stage stage(const char* id, float target, float durationMs,
		CurveType curve = CurveType::EXPONENTIAL, float amount = 0.f) {
	Stage output;
	output.id = id;
	output.target = target;
	output.duration = milliseconds(durationMs);
	output.curve.type = curve;
	output.curve.amount = amount;
	return output;
}

Program staged(const char* id, const char* name, ProgramMode mode,
		std::initializer_list<Stage> gatePath, std::initializer_list<Stage> releasePath,
		RetriggerPolicy retrigger = RetriggerPolicy::FROM_CURRENT) {
	Program program;
	program.id = id;
	program.name = name;
	program.kind = ProgramKind::STAGED;
	program.mode = mode;
	program.gatePath.assign(gatePath.begin(), gatePath.end());
	program.releasePath.assign(releasePath.begin(), releasePath.end());
	program.sustainHold = mode == ProgramMode::GATE;
	program.retrigger = retrigger;
	return program;
}

Program contour(const char* id, const char* name, ProgramMode mode,
		Duration duration, std::initializer_list<ContourPoint> points) {
	Program program;
	program.id = id;
	program.name = name;
	program.kind = ProgramKind::CONTOUR;
	program.mode = mode;
	program.duration = duration;
	program.points.assign(points.begin(), points.end());
	program.interpolation = Interpolation::MONOTONE_CUBIC;
	program.retrigger = RetriggerPolicy::RESTART;
	return program;
}

float smooth01(float value) {
	const float t = std::max(0.f, std::min(1.f, value));
	return t * t * (3.f - 2.f * t);
}

float bell(float phase, float center, float width) {
	const float x = (phase - center) / std::max(width, 0.0001f);
	return std::exp(-0.5f * x * x);
}

float wyrmShapeValue(int shape, float phase) {
	const float p = std::max(0.f, std::min(1.f, phase));
	switch (shape) {
		case 1: return 1.f - smooth01(p);
		case 2: {
			const float attackEnd = 0.15625f;
			if (p <= attackEnd) {
				const float t = p / attackEnd;
				return t + 0.35f * t * (1.f - t);
			}
			const float tail = 1.f - (p - attackEnd) / (1.f - attackEnd);
			return tail * tail;
		}
		case 3:
			return smooth01(p / 0.055f) * (p <= 0.30f ? 1.f : 1.f - smooth01((p - 0.30f) / 0.70f));
		case 4: {
			const float attack = smooth01(p / 0.11f);
			const float decay = 1.f - smooth01((p - 0.11f) / 0.89f);
			return attack * decay - 0.24f * bell(p, 0.27f, 0.035f) + 0.14f * bell(p, 0.36f, 0.055f);
		}
		case 5:
			return smooth01(p / 0.055f) * std::max(bell(p, 0.22f, 0.075f), 0.96f * bell(p, 0.48f, 0.11f));
		case 6: {
			const float body = std::max(bell(p, 0.20f, 0.13f), 0.84f * bell(p, 0.56f, 0.16f));
			return p < 0.20f ? smooth01(p / 0.20f) : body;
		}
		case 7: {
			const float body = std::max(bell(p, 0.20f, 0.12f), 0.30f * bell(p, 0.79f, 0.075f));
			return p < 0.20f ? smooth01(p / 0.20f) : body;
		}
		case 8:
			return smooth01(p / 0.12f) * (1.f - 0.40f * smooth01((p - 0.12f) / 0.17f))
				* (1.f - smooth01((p - 0.60f) / 0.40f));
		case 9:
			return smooth01(p / 0.12f) * (1.f - smooth01((p - 0.67f) / 0.33f));
		case 10:
			return smooth01(p / 0.08f) * (1.f - 0.38f * smooth01((p - 0.38f) / 0.08f))
				* (1.f - smooth01((p - 0.69f) / 0.31f));
	}
	return 0.f;
}

Program wyrmContour(int shape, const std::string& id, const std::string& name) {
	Program program;
	program.id = id;
	program.name = name;
	program.kind = ProgramKind::CONTOUR;
	program.mode = ProgramMode::ONE_SHOT;
	program.duration = milliseconds(500.f);
	program.interpolation = Interpolation::MONOTONE_CUBIC;
	program.retrigger = RetriggerPolicy::RESTART;
	// Sample the authored Wyrm D-shape formulas once into an independent Moirai
	// contour. Moirai never calls Wyrm or evaluates these formulas in its DSP path.
	constexpr int pointCount = 33;
	for (int index = 0; index < pointCount; ++index) {
		const float time = static_cast<float>(index) / (pointCount - 1);
		program.points.push_back({time,
			std::max(0.f, std::min(1.f, wyrmShapeValue(shape, time)))});
	}
	return program;
}

std::vector<Program> buildFactoryPrograms() {
	std::vector<Program> programs;
	programs.push_back(staged("factory_ad_percussive", "AD Percussive", ProgramMode::ONE_SHOT,
		{stage("attack", 1.f, 4.f, CurveType::EXPONENTIAL, 0.65f)},
		{stage("decay", 0.f, 180.f, CurveType::EXPONENTIAL, -0.25f)}, RetriggerPolicy::RESTART));
	programs.push_back(staged("factory_ar", "AR", ProgramMode::GATE,
		{stage("attack", 1.f, 12.f, CurveType::EXPONENTIAL, 0.35f)},
		{stage("release", 0.f, 220.f, CurveType::EXPONENTIAL, 0.25f)}));
	programs.push_back(staged("factory_adsr", "ADSR", ProgramMode::GATE,
		{stage("attack", 1.f, 8.f, CurveType::EXPONENTIAL, 0.55f),
		 stage("decay", 0.62f, 95.f, CurveType::EXPONENTIAL, -0.2f)},
		{stage("release", 0.f, 180.f, CurveType::EXPONENTIAL, 0.35f)}));
	programs.push_back(staged("factory_ahr", "AHR", ProgramMode::ONE_SHOT,
		{stage("attack", 1.f, 18.f, CurveType::SMOOTHSTEP),
		 stage("hold", 1.f, 90.f, CurveType::HOLD)},
		{stage("release", 0.f, 240.f, CurveType::EXPONENTIAL, 0.25f)}));
	programs.push_back(staged("factory_dadsr", "DADSR", ProgramMode::GATE,
		{stage("delay", 0.f, 40.f, CurveType::HOLD),
		 stage("attack", 1.f, 20.f, CurveType::SMOOTHSTEP),
		 stage("decay", 0.58f, 140.f, CurveType::EXPONENTIAL, -0.15f)},
		{stage("release", 0.f, 260.f, CurveType::EXPONENTIAL, 0.25f)}));
	programs.push_back(staged("factory_trapezoid", "Trapezoid", ProgramMode::ONE_SHOT,
		{stage("attack", 1.f, 35.f, CurveType::LINEAR), stage("hold", 1.f, 160.f, CurveType::HOLD)},
		{stage("release", 0.f, 35.f, CurveType::LINEAR)}));
	programs.push_back(staged("factory_pluck", "Pluck", ProgramMode::ONE_SHOT,
		{stage("attack", 1.f, 2.f, CurveType::EXPONENTIAL, 0.8f)},
		{stage("decay", 0.f, 115.f, CurveType::EXPONENTIAL, -0.45f)}, RetriggerPolicy::RESTART));
	programs.push_back(staged("factory_pad", "Pad", ProgramMode::GATE,
		{stage("attack", 1.f, 850.f, CurveType::SIGMOID), stage("decay", 0.78f, 500.f, CurveType::SMOOTHSTEP)},
		{stage("release", 0.f, 1400.f, CurveType::SIGMOID)}, RetriggerPolicy::FROM_CURRENT));
	programs.push_back(staged("factory_swell", "Swell", ProgramMode::ONE_SHOT,
		{stage("rise", 1.f, 900.f, CurveType::SIGMOID)},
		{stage("fall", 0.f, 900.f, CurveType::SIGMOID)}, RetriggerPolicy::FROM_CURRENT));
	programs.push_back(contour("factory_duck", "Duck", ProgramMode::ONE_SHOT, milliseconds(420.f),
		{{0.f, 1.f}, {0.03f, 0.f}, {0.55f, 0.f}, {1.f, 1.f}}));
	programs.push_back(contour("factory_cycle_triangle", "Cycle Triangle", ProgramMode::CYCLE, beats(1.f),
		{{0.f, 0.f}, {0.5f, 1.f}, {1.f, 0.f}}));
	Program sine = contour("factory_cycle_sine", "Cycle Sine", ProgramMode::CYCLE, beats(1.f), {});
	for (int index = 0; index <= 16; ++index) {
		const float time = static_cast<float>(index) / 16.f;
		const float value = 0.5f - 0.5f * std::cos(6.2831853071795864769f * time);
		sine.points.push_back({time, value});
	}
	programs.push_back(sine);
	programs.push_back(wyrmContour(2, "factory_wyrm_ar", "Wyrm AR"));
	for (int shape = 1; shape <= 10; ++shape) {
		programs.push_back(wyrmContour(shape, "factory_wyrm_d" + std::to_string(shape),
			"Wyrm D" + std::to_string(shape)));
	}
	return programs;
}

} // namespace

const std::vector<Program>& factoryPrograms() {
	static const std::vector<Program> programs = buildFactoryPrograms();
	return programs;
}

const Program* findFactoryProgram(const std::string& id) {
	const std::vector<Program>& programs = factoryPrograms();
	for (const Program& program : programs)
		if (program.id == id) return &program;
	return nullptr;
}

Bank makeInitialBank() {
	Bank bank;
	bank.schemaVersion = 1;
	bank.revision = 0;
	bank.seed = 0;
	const Program* adsr = findFactoryProgram("factory_adsr");
	if (adsr) bank.programs[adsr->id] = *adsr;
	for (Lane& lane : bank.lanes) lane.defaultProgram = "factory_adsr";
	return bank;
}

} // namespace moirai
