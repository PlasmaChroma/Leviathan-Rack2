#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelIntegralFlux;
extern Model* modelProc;
extern Model* modelTemporalDeck;
extern Model* modelTDScope;
extern Model* modelCrownstep;

// Runtime feature flag: enabled when `res/dragonking.txt` exists.
bool isDragonKingDebugEnabled();
void refreshDragonKingDebugEnabled();
