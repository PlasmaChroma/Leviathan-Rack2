#pragma once

#include "SibylTypes.hpp"
#include <string>
#include <vector>
#include <jansson.h>

namespace sibyl {

struct ValidationIssue {
    std::string path;
    std::string message;
    std::string code;
};

struct ParseResult {
    bool valid = false;
    CompositionPtr composition;
    std::vector<ValidationIssue> errors;
    std::vector<ValidationIssue> warnings;
};

// Returns a new normalized composition object, or null with errors.
json_t* normalizeComposition(json_t* input, ParseResult& result);
bool normalizeNoteIds(json_t* pattern, const std::string& path, ParseResult& result, json_t* previous = nullptr);

// Parses a complete JSON composition payload and compiles it into an immutable snapshot.
ParseResult parseCompositionJson(const std::string& jsonString, int revision);

// Serializers for SibylControl GET_COMPOSITION views
std::string serializeSummaryJson(const Composition& comp);
std::string serializeFullCompositionJson(const Composition& comp);
std::string serializePatternViewJson(const Composition& comp, const std::string& patternId);
std::string serializeSceneViewJson(const Composition& comp, const std::string& sceneId, bool effectiveExpressions = false);

} // namespace sibyl
