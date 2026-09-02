#include "OctaviaAnalysis.hpp"

#include <cmath>
#include <iostream>

namespace {
int failures = 0;
void check(bool pass, const char* name) {
	std::cout << (pass ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!pass) failures++;
}

octavia::FrozenObservation fixture() {
	octavia::FrozenObservation snapshot;
	snapshot.id = 7;
	snapshot.sampleRate = 48000.f;
	snapshot.startFrame = 100;
	snapshot.endFrame = 4195;
	snapshot.requestedMask = 0x3f;
	snapshot.anyConnectedMask = 0x3f;
	snapshot.allConnectedMask = 0x3f;
	for (size_t i = 0; i < 4096; ++i) {
		const float phase = 2.f * 3.14159265358979323846f * 1000.f * float(i) / 48000.f;
		const float sine = std::sin(phase);
		snapshot.samples[2].push_back(sine);
		snapshot.samples[3].push_back(2.f * sine);
		snapshot.samples[4].push_back(sine);
		snapshot.samples[5].push_back(-sine);
	}
	return snapshot;
}
}

int main() {
	octavia::FrozenObservation snapshot = fixture();
	octavia::AnalysisEngine engine;
	std::string error;

	octavia::AnalysisGroup mono;
	mono.first = octavia::ObserveChannel::A;
	octavia::GroupAnalysis basic;
	check(engine.tryAnalyze(snapshot, mono, false, false, &basic, &error),
		"named mono snapshot analysis succeeds");
	check(std::fabs(basic.mono.rms - std::sqrt(0.5f)) < 0.01f
		&& basic.mono.peak > 0.99f && basic.mono.frames == 4096,
		"basic RMS, peak, and frame metadata are correct");

	octavia::GroupAnalysis detailed;
	check(engine.tryAnalyze(snapshot, mono, true, true, &detailed, &error)
		&& !detailed.mono.spectrum.empty(), "detailed analysis uses FFT spectrum");
	float strongestHz = 0.f, strongestDb = -999.f;
	for (const auto& bin : detailed.mono.spectrum)
		if (bin.db > strongestDb) { strongestDb = bin.db; strongestHz = bin.hz; }
	check(std::fabs(strongestHz - 1000.f) < 15.f,
		"FFT identifies the fixture's dominant frequency");

	octavia::AnalysisGroup stereo;
	stereo.stereo = true;
	stereo.first = octavia::ObserveChannel::C;
	stereo.second = octavia::ObserveChannel::D;
	octavia::GroupAnalysis stereoResult;
	check(engine.tryAnalyze(snapshot, stereo, false, false, &stereoResult, &error)
		&& stereoResult.stereo.correlation < -0.99f
		&& stereoResult.stereo.sideToMidDb > 100.f,
		"stereo groups report correlation and mid/side width");

	octavia::AnalysisGroup target;
	target.first = octavia::ObserveChannel::B;
	octavia::ComparisonAnalysis comparison;
	check(engine.tryCompare(snapshot, mono, target, true, false, &comparison, &error),
		"arbitrary named groups compare from one frozen snapshot");
	check(std::fabs(comparison.rmsDeltaDb - 6.0206f) < 0.05f
		&& std::fabs(comparison.peakDeltaDb - 6.0206f) < 0.05f,
		"comparison reports explicit target-minus-reference level deltas");
	check(std::fabs(comparison.normalizedSpectralDeltaDb[3]) < 0.1f,
		"level-normalized spectral delta removes a pure gain change");

	octavia::FrozenObservation diagnostic;
	diagnostic.sampleRate = 48000.f;
	diagnostic.requestedMask = diagnostic.anyConnectedMask = diagnostic.allConnectedMask = 1u << 2;
	for (size_t i = 0; i < 16384; ++i) {
		const float t = float(i) / diagnostic.sampleRate;
		diagnostic.samples[2].push_back(1.2f * std::sin(2.f * 3.14159265358979323846f * 60.f * t)
			+ 0.7f * std::sin(2.f * 3.14159265358979323846f * 120.f * t));
	}
	octavia::GroupAnalysis diagnostics;
	check(engine.tryAnalyze(diagnostic, mono, true, false, &diagnostics, &error)
		&& diagnostics.mono.temporalSeparationFrames == 5760,
		"temporal stability uses deterministic 120 ms windows from one frozen snapshot");
	check(diagnostics.mono.hum.detected60 && !diagnostics.mono.hum.detected50,
		"targeted Goertzel probes distinguish stable 60 Hz hum and harmonics");
	bool stableResonance = false;
	for (const auto& resonance : diagnostics.mono.resonances)
		stableResonance |= resonance.stable;
	check(stableResonance, "FFT resonance candidates carry frozen-window stability");

	octavia::FrozenObservation extended;
	extended.sampleRate = 48000.f;
	extended.requestedMask = extended.anyConnectedMask = extended.allConnectedMask = 1u << 2;
	for (size_t i = 0; i < 32768; ++i) {
		const float hz = i < 16384 ? 1000.f : 3000.f;
		extended.samples[2].push_back(std::sin(
			2.f * 3.14159265358979323846f * hz * float(i) / extended.sampleRate));
	}
	octavia::GroupAnalysis extendedResult;
	check(engine.tryAnalyze(extended, mono, true, true, &extendedResult, &error),
		"long-window detailed analysis succeeds");
	float earlyToneDb = -140.f, lateToneDb = -140.f;
	for (const auto& bin : extendedResult.mono.spectrum) {
		if (std::fabs(bin.hz - 1000.f) < 15.f) earlyToneDb = std::max(earlyToneDb, bin.db);
		if (std::fabs(bin.hz - 3000.f) < 15.f) lateToneDb = std::max(lateToneDb, bin.db);
	}
	check(earlyToneDb > -30.f && lateToneDb > -30.f,
		"multi-window spectrum represents events from both halves of an extended capture");

	std::cout << "[TEST SUMMARY] failures=" << failures << "\n";
	return failures ? 1 : 0;
}
