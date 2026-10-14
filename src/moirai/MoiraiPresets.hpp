#pragma once

#include "MoiraiTypes.hpp"

namespace moirai {

const std::vector<Program>& factoryPrograms();
const Program* findFactoryProgram(const std::string& id);
Bank makeInitialBank();

} // namespace moirai
