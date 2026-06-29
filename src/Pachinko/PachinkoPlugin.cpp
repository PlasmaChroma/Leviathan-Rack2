#include "PachinkoPlugin.hpp"
#include "PachinkoTimingModule.hpp"
#include "PachinkoWidget.hpp"

Model* modelPachinkoTiming =
    createModel<PachinkoTimingModule, PachinkoWidget>("Pachinko");
