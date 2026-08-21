#pragma once

#include "SibylTypes.hpp"
#include <string>
#include <vector>

namespace sibyl {

struct ValidationIssue {
    std::string path;
    std::string message;
};

struct ParseResult {
    bool valid = false;
    CompositionPtr composition;
    std::vector<ValidationIssue> errors;
    std::vector<ValidationIssue> warnings;
};

// Parses a complete JSON composition payload and compiles it into an immutable snapshot.
ParseResult parseCompositionJson(const std::string& jsonString, int revision);

// Validate candidate mutations.
// ParseResult validateOperation(const std::string& opJson, CompositionPtr current);

} // namespace sibyl
