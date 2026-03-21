#pragma once

#include <iosfwd>
#include <string>

namespace spec {

bool replayTraceFile(const std::string &path, std::ostream &out);

} // namespace spec

