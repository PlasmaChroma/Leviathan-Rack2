#pragma once

// Some toolchains don't expose math constants like M_PI by default.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

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
extern Model* modelBifurx;

// Runtime feature flag: enabled when `res/dragonking.txt` exists.
bool isDragonKingDebugEnabled();
void refreshDragonKingDebugEnabled();
