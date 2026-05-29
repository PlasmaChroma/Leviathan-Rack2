#pragma once

#include "ChronomawState.hpp"

namespace chronomaw {

json_t* serializeState(const ModuleState& state);
void deserializeState(json_t* rootJ, ModuleState* state);

} // namespace chronomaw

