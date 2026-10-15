#include "../src/sil/SilLowBandRecovery.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {
constexpr double pi = 3.14159265358979323846;
void require(bool ok, const char* message) {
	if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
double db(double ratio) { return 20.0 * std::log10(std::max(ratio, 1e-20)); }
struct Measurement {
	double mid = 0, side = 0, left = 0, right = 0, lowMid = 0;
	float amount = 0;
	float feedback = 0;
};
Measurement tone(float sr, float hz, float phase, float amplitudeL = 1.f, float amplitudeR = 1.f) {
	sil::LowBandRecovery recovery;
	recovery.configure(sr);
	Measurement m;
	const int count = int(sr * 4);
	for (int i = 0; i < count; ++i) {
		const double angle = 2 * pi * hz * i / sr;
		const float l = amplitudeL * std::sin(angle);
		const float r = amplitudeR * (phase == 180.f ? -std::sin(angle) : std::sin(angle + phase * pi / 180));
		const auto out = recovery.process(l, r);
		require(std::isfinite(out.left) && std::isfinite(out.right), "Nonfinite tone output");
		if (i >= count - int(sr)) {
			const double mid = 0.5 * (out.left + out.right), side = 0.5 * (out.left - out.right);
			m.mid += mid * mid;
			m.lowMid += double(out.lowMid) * out.lowMid;
			m.side += side * side;
			m.left += double(out.left) * out.left;
			m.right += double(out.right) * out.right;
		}
	}
	m.lowMid = std::sqrt(m.lowMid / sr);
	m.mid = std::sqrt(m.mid / sr); m.side = std::sqrt(m.side / sr);
	m.left = std::sqrt(m.left / sr); m.right = std::sqrt(m.right / sr);
	m.amount = recovery.recoveryAmount();
	m.feedback = recovery.feedbackAmount();
	return m;
}

// Independent bilinear LR4 transfer magnitude, including prewarping.
double lowMagnitude(float sr, float hz) {
	const double ratio = std::tan(pi * hz / sr) / std::tan(pi * 120 / sr);
	return 1.0 / (1.0 + std::pow(ratio, 4));
}
void frequencyTests(float sr) {
	const double sineRms = std::sqrt(0.5);
	for (float hz : {10.f, 20.f, 40.f, 60.f, 120.f, 240.f, 1000.f, 10000.f}) {
		const auto mono = tone(sr, hz, 0.f);
		require(std::fabs(db(mono.left / sineRms)) < 0.02, "Crossover changes mono magnitude");
		require(mono.side < 1e-7, "Mono input creates Side");
		require(mono.amount < 0.001, "Healthy mono engages recovery");
		require(mono.feedback < 0.001, "Healthy mono reports correction power");
		// Positive correlation with unequal channel levels must preserve stereo balance.
		const auto coherent = tone(sr, hz, 0.f, 1.f, 0.5f);
		require(std::fabs(db(coherent.left / sineRms)) < 0.02, "Coherent left magnitude changed");
		require(std::fabs(db(coherent.right / (0.5 * sineRms))) < 0.02, "Coherent right magnitude changed");
		if (hz <= 240.f) {
			const auto inverted = tone(sr, hz, 180.f);
			const double low = lowMagnitude(sr, hz);
			require(inverted.amount > 0.999, "Inverted bass did not fully engage recovery");
			require(std::fabs(inverted.feedback - low * low) < 0.005,
				"Feedback does not represent low-band Side power removed");
			require(std::fabs(db(inverted.mid / (sineRms * low))) < 0.04, "Canceled bass energy was not restored");
			require(std::fabs(inverted.side / sineRms - (1 - low)) < 0.0002, "Side does not follow matched high-pass");
			if (hz == 60.f) std::printf("  %.0f Hz sample rate: 60 Hz mono recovery %.2f dB, residual Side %.2f dB\n",
				sr, db(inverted.mid / sineRms), db(inverted.side / sineRms));
		}
	}
	const auto quarterCycle = tone(sr, 40.f, 90.f);
	require(quarterCycle.amount > 0.97f, "Uncorrelated bass not collapsed");
	require(quarterCycle.mid > 0.68, "Uncorrelated bass lost mono power");
	require(quarterCycle.feedback > 0.47f && quarterCycle.feedback < 0.51f,
		"Uncorrelated bass feedback saturates instead of showing affected power");
	const auto healthyPhase = tone(sr, 40.f, 30.f);
	require(healthyPhase.amount < 0.001f, "Healthy phase-offset stereo engages recovery");
	require(std::fabs(db(healthyPhase.mid / (sineRms * std::cos(pi / 12)))) < 0.02
		&& std::fabs(db(healthyPhase.side / (sineRms * std::sin(pi / 12)))) < 0.02,
		"Healthy phase-offset stereo balance changed");
	const auto partial = tone(sr, 40.f, 60.f);
	require(partial.amount > 0.2f && partial.amount < 0.4f, "Intermediate correlation is not adaptive");
	require(partial.feedback > 0.10f && partial.feedback < 0.15f, "Partial correction feedback has wrong scale");
	const auto unequalInverted = tone(sr, 40.f, 180.f, 0.1f, 1.f);
	require(std::fabs(db(unequalInverted.lowMid / (std::sqrt(0.505) * sineRms * lowMagnitude(sr, 40.f)))) < 0.04,
		"Right-dominant inverted bass recovery has wrong power");
	for (float hz : {1000.f, 10000.f}) {
		const auto high = tone(sr, hz, 180.f);
		require(high.amount < 0.001, "Treble-only signal activates recovery indicator");
		require(std::fabs(db(high.side / sineRms)) < 0.02 && high.mid < 1e-5,
			"High-frequency stereo was collapsed");
	}
	for (bool rightOnly : {false, true}) {
		const auto pan = tone(sr, 40.f, 0.f, rightOnly ? 0.f : 1.f, rightOnly ? 1.f : 0.f);
		require(std::fabs(db(pan.lowMid / (0.5 * lowMagnitude(sr, 40.f)))) < 0.04,
			"Single-channel bass not centered at equal-power level");
	}
}

void feedbackTests() {
	constexpr float sr = 48000;
	float readings[3] = {};
	for (int scenario = 0; scenario < 3; ++scenario) {
		sil::LowBandRecovery recovery;
		recovery.configure(sr);
		const float bassAmplitude = scenario == 0 ? 0.1f : 1.f;
		const float overallGain = scenario == 2 ? 0.1f : 1.f;
		for (int i = 0; i < int(4 * sr); ++i) {
			const float bass = bassAmplitude * std::sin(2 * pi * 40 * i / sr);
			const float treble = std::sin(2 * pi * 3000 * i / sr);
			recovery.process(overallGain * (bass + treble), overallGain * (-bass + treble));
		}
		readings[scenario] = recovery.feedbackAmount();
		require(recovery.recoveryAmount() > 0.999f, "Meter test did not fully engage the fold");
		const double low = lowMagnitude(sr, 40.f);
		const double expected = low * low * bassAmplitude * bassAmplitude / (1 + bassAmplitude * bassAmplitude);
		require(std::fabs(readings[scenario] - expected) < 0.005, "Feedback ignores the affected share of the mix");
		for (int i = 0; i < int(5 * sr); ++i) recovery.process(0.f, 0.f);
		require(recovery.feedbackAmount() < 0.001f, "Feedback does not release to zero on silence");
		recovery.reset();
		require(recovery.feedbackAmount() == 0.f, "Reset leaves stale feedback");
	}
	require(readings[0] < 0.02f && readings[1] > 0.45f, "Weak and strong bass show the same amount");
	require(std::fabs(readings[1] - readings[2]) < 0.001f, "Feedback depends on overall volume");
	std::printf("  feedback: weak bass %.1f%%, strong bass %.1f%%, quieter mix %.1f%%\n",
		100 * readings[0], 100 * readings[1], 100 * readings[2]);
}

void mixedAndTransitionTests() {
	constexpr float sr = 48000;
	sil::LowBandRecovery recovery;
	recovery.configure(sr);
	double bassMid = 0, trebleSideSin = 0, trebleSideCos = 0;
	// Opposite-polarity bass mixed with a separate high-frequency stereo tone.
	for (int i = 0; i < int(4 * sr); ++i) {
		const double bassPhase = 2 * pi * 40 * i / sr, highPhase = 2 * pi * 3000 * i / sr;
		const float input = std::sin(bassPhase) + 0.5 * std::sin(highPhase);
		const auto out = recovery.process(input, -input);
		if (i >= int(3 * sr)) {
			const double mid = 0.5 * (out.left + out.right), side = 0.5 * (out.left - out.right);
			bassMid += mid * mid;
			trebleSideSin += side * std::sin(highPhase);
			trebleSideCos += side * std::cos(highPhase);
		}
	}
	require(std::sqrt(bassMid / sr) > 0.69, "Mixed program did not recover canceled bass");
	const double highAmplitude = 2 * std::hypot(trebleSideSin, trebleSideCos) / sr;
	require(std::fabs(db(highAmplitude / 0.5)) < 0.02, "Mixed program altered high stereo magnitude");

	// Slowly exchange the louder channel while retaining negative correlation.
	// Channel handoff must not cancel the recovered mono waveform or click.
	recovery.configure(sr);
	float previous = 0, maxStep = 0;
	double energy = 0;
	for (int i = 0; i < int(8 * sr); ++i) {
		const float balance = float(i) / (8 * sr);
		const float l = (1.f - 0.95f * balance) * std::sin(2 * pi * 40 * i / sr);
		const float r = -(0.05f + 0.95f * balance) * std::sin(2 * pi * 40 * i / sr);
		const auto out = recovery.process(l, r);
		require(std::fabs(out.lowMid) < 1.5f, "Reference handoff creates excessive gain");
		maxStep = std::max(maxStep, std::fabs(out.lowMid - previous)); previous = out.lowMid;
		if (i >= int(7 * sr)) energy += double(out.lowMid) * out.lowMid;
	}
	require(maxStep < 0.02f, "Reference handoff creates a discontinuity");
	require(std::sqrt(energy / sr) > 0.4, "Reference handoff cancels bass");
	// Release after an inverted segment, then reset/sample-rate reconfiguration.
	for (int i = 0; i < int(4 * sr); ++i) {
		float input = std::sin(2 * pi * 40 * i / sr);
		recovery.process(input, input);
	}
	require(recovery.recoveryAmount() < 0.001f, "Recovery does not release on healthy bass");
	for (float rate : {44100.f, 192000.f, 48000.f}) {
		recovery.configure(rate);
		for (int i = 0; i < 10000; ++i) {
			const auto out = recovery.process(0.f, 0.f);
			require(out.left == 0 && out.right == 0 && recovery.recoveryAmount() == 0,
				"Silence/reset leaves stale audio or recovery indication");
		}
	}
}
}

int main() {
	for (float sr : {44100.f, 48000.f, 96000.f, 192000.f}) frequencyTests(sr);
	mixedAndTransitionTests();
	feedbackTests();
	std::puts("Sil low-band recovery tests passed");
}
