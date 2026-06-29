#pragma once

#include "../plugin.hpp"
#include "PachinkoTimingModule.hpp"
#include "PachinkoWidget.hpp"

// Initialize plugin - called from main plugin.cpp
void initPachinkoPlugin();

// Get model pointer
Model* getPachinkoModel();
