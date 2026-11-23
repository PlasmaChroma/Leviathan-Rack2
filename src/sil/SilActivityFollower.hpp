// Program activity follower adapted from Temper's correction guard.
// Tracks a 50 ms mean-square level against a 3 s release peak hold. When the
// level falls more than 30 dB below the hold, activity fades to zero across a
// 6 dB window, with 120 ms smoothing. Sil uses it to scale adaptive lifts so
// breakdowns and fade-outs do not attract boosts. Rack-free for DSP tests.
#pragma once
#include <algorithm>
#include <cmath>

namespace sil {

struct ActivityFollower {
	static constexpr float kFloorDb = -140.f;
	static constexpr float kSilenceDb = -90.f;
	static constexpr float kDropFullDb = 30.f;
	static constexpr float kDropKneeDb = 6.f;

	float energyAlpha = 0.f;
	float holdAlpha = 0.f;
	float activityAlpha = 0.f;
	float energy = 0.f;
	float heldDb = kFloorDb;
	float activity = 1.f;

	// decisionDivision: samples between decide() calls.
	void configure(float sampleRate, int decisionDivision) {
		const float sr = std::max(sampleRate, 1.f);
		const float div = float(std::max(decisionDivision, 1));
		energyAlpha = 1.f - std::exp(-1.f / (0.050f * sr));
		holdAlpha = 1.f - std::exp(-div / (3.f * sr));
		activityAlpha = 1.f - std::exp(-div / (0.120f * sr));
		reset();
	}
	void reset() {
		energy = 0.f;
		heldDb = kFloorDb;
		activity = 1.f;
	}
	// mono: full-scale-normalized amplitude.
	void processSample(float mono) {
		energy += energyAlpha * (mono * mono - energy);
	}
	float decide() {
		const float levelDb = 10.f * std::log10(std::max(energy, 1e-14f));
		if (levelDb >= heldDb) heldDb = levelDb;
		else heldDb += holdAlpha * (levelDb - heldDb);
		const float target = heldDb < kSilenceDb
			? 0.f
			: std::min(1.f, std::max(0.f, (kDropFullDb + kDropKneeDb - (heldDb - levelDb)) / kDropKneeDb));
		activity += activityAlpha * (target - activity);
		return activity;
	}
};

} // namespace sil
