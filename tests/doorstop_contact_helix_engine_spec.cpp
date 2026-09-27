#include "../src/DoorstopContactHelixEngine.hpp"
#include <cmath>
#include <iostream>
int main() {
	for (float rate : {44100.f, 48000.f, 88200.f, 96000.f, 192000.f}) {
		doorstop::ContactHelixEngine e; e.setSampleRate(rate); e.setSpecimenSeed(77); e.strike(0.5f);
		float peak = 0.f; bool finite = true;
		for (int i=0; i<int(rate*2); ++i) { auto f=e.process(1.f/rate); peak=std::max(peak,std::fabs(f.outputVolts)); finite &= std::isfinite(f.outputVolts)&&std::isfinite(f.displacement); }
		if (!finite || peak <= 0.001f || e.getDiagnostics().recoveries) return 1;
	}
	for (float velocity : {0.1f, 0.3f, 0.5f, 1.f}) {
		doorstop::ContactHelixEngine e; e.setSampleRate(48000.f); e.setSpecimenSeed(77); e.strike(velocity);
		float peak = 0.f, travel = 0.f, previousDisplacement = 0.f; double lateEnergy = 0.0; int bendCrossings = 0;
		for (int i=0; i<48000*3; ++i) { auto f=e.process(1.f/48000.f); peak=std::max(peak,std::fabs(f.outputVolts)); travel=std::max(travel,std::fabs(f.displacement)); if (i > 0 && f.displacement * previousDisplacement < 0.f) ++bendCrossings; previousDisplacement=f.displacement; if(i>=48000*2) lateEnergy += double(f.outputVolts)*f.outputVolts; }
		const float lateRms = std::sqrt(float(lateEnergy / 48000.0));
		const float mechanicalBend = e.getDiagnostics().maximumBendMagnitude;
		std::cout << "velocity=" << velocity << " peakV=" << peak << " travel=" << travel << " mechanicalBend=" << mechanicalBend << " lateRms=" << lateRms << " crossings=" << bendCrossings << "\n";
		if (velocity == 0.1f && (peak < 0.35f || travel < 0.25f || mechanicalBend < 0.006f)) return 1;
		if (velocity == 0.3f && (peak < 1.f || travel < 0.65f || mechanicalBend < 0.015f)) return 1;
		if (velocity == 0.5f && (peak < 1.5f || travel < 1.f || mechanicalBend < 0.03f || lateRms < 0.02f || bendCrossings < 50)) return 1;
		if (velocity == 1.f && (peak < 2.5f || travel < 2.f || mechanicalBend < 0.05f || lateRms < 0.04f || bendCrossings < 50)) return 1;
	}
	{
		using namespace doorstop::contact_helix_defaults;
		doorstop::ContactHelixEngine low, high;
		low.setSampleRate(48000.f); high.setSampleRate(48000.f);
		low.setSpecimenSeed(77); high.setSpecimenSeed(77);
		low.setV3BodyTuning(PAIRING, REACTION, LOBES, ATTACK, FLICK_TIME, MID_BODY, 0.7f);
		high.setV3BodyTuning(PAIRING, REACTION, LOBES, ATTACK, FLICK_TIME, MID_BODY, 1.3f);
		low.strike(0.5f); high.strike(0.5f);
		double pitchDifference = 0.0;
		for (int i = 0; i < 48000; ++i) {
			const auto a = low.process(1.f / 48000.f);
			const auto b = high.process(1.f / 48000.f);
			pitchDifference += std::fabs(double(a.outputVolts) - b.outputVolts);
		}
		if (pitchDifference < 100.0) return 1;

		doorstop::ContactHelixEngine dryLow, dryHigh;
		dryLow.setSampleRate(48000.f); dryHigh.setSampleRate(48000.f);
		dryLow.setSpecimenSeed(77); dryHigh.setSpecimenSeed(77);
		dryLow.setV3BodyTuning(0.f, 0.f, LOBES, ATTACK, FLICK_TIME, MID_BODY, 0.7f);
		dryHigh.setV3BodyTuning(0.f, 0.f, LOBES, ATTACK, FLICK_TIME, MID_BODY, 1.3f);
		dryLow.strike(0.5f); dryHigh.strike(0.5f);
		double dryDifference = 0.0;
		for (int i = 0; i < 4800; ++i) {
			const auto a = dryLow.process(1.f / 48000.f);
			const auto b = dryHigh.process(1.f / 48000.f);
			dryDifference += std::fabs(double(a.outputVolts) - b.outputVolts);
		}
		if (dryDifference != 0.0) return 1;
	}
	std::cout << "[PASS] V4 is finite and audible across supported rates\n"; return 0;
}
