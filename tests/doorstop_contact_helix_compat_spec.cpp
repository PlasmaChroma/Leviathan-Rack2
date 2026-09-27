#include "../src/DoorstopEngineRouter.hpp"
#include <iostream>
int main(){
	static_assert(int(doorstop::EngineMode::ReferenceV1)==0,"V1 changed"); static_assert(int(doorstop::EngineMode::Legacy)==1,"legacy changed"); static_assert(int(doorstop::EngineMode::ReferenceV2)==2,"V2 changed"); static_assert(int(doorstop::EngineMode::ReferenceV3)==3,"V3 changed"); static_assert(int(doorstop::EngineMode::ReferenceV4)==4,"V4 not appended");
	doorstop::DoorstopEngineRouter r; r.setEngineMode(doorstop::EngineMode::ReferenceV4); for(int i=0;i<800;++i)r.process(1.f/48000.f); r.strike(.5f); if(r.getEngineMode()!=doorstop::EngineMode::ReferenceV4||r.isSleeping())return 1;
	std::cout<<"[PASS] V4 appends identifiers and routes independently\n"; return 0;
}
