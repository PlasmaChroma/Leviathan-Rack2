#include "PachinkoPlugin.hpp"
#include "PachinkoTimingModule.hpp"
#include "PachinkoWidget.hpp"

// Define plugin information
#define PLUGIN_SLUG "pachinko"

// Model pointer
Model* modelPachinkoTiming;

// Initialize plugin - called from main plugin.cpp
void initPachinkoPlugin() {
    // Register model using correct VCV Rack 2 API
    modelPachinkoTiming = new Model();
    modelPachinkoTiming->slug = PLUGIN_SLUG;
    modelPachinkoTiming->name = "Pachinko";
    modelPachinkoTiming->description = "Pachinko Timing";
    // tagIds is a std::list<int> - empty list for now
}

// Get model pointer
Model* getPachinkoModel() {
    return modelPachinkoTiming;
}
