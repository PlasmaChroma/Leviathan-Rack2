#include "../src/DoorstopContactHelixEngine.hpp"
#include <cmath>
#include <iostream>
int main() {
	doorstop::ContactHelixEngine e; e.setSampleRate(48000); e.strike(1.f);
	float maxEnergy=0, maxForce=0; unsigned contacts=0;
	for(int i=0;i<48000;++i){e.process(1.f/48000.f); const auto& d=e.getDiagnostics(); maxEnergy=std::max(maxEnergy,d.mechanicalEnergy); maxForce=std::max(maxForce,d.maximumContactForce); contacts=std::max(contacts,d.activeContacts);}
	if (!(maxEnergy>0 && maxForce>0 && contacts>0 && e.getDiagnostics().dissipatedWork>=0 && e.getDiagnostics().appliedImpulse>0)) return 1;
	std::cout << "[PASS] V4 contact participates in the mechanical energy/work ledger\n"; return 0;
}
