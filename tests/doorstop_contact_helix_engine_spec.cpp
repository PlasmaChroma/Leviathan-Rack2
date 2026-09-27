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
		if (velocity == 1.f && (peak < 2.5f || travel < 2.f || mechanicalBend < 0.06f || lateRms < 0.04f || bendCrossings < 50)) return 1;
	}
	std::cout << "[PASS] V4 is finite and audible across supported rates\n"; return 0;
}
