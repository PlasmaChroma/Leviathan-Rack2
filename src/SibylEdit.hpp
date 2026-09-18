#pragma once

#include "SibylJSON.hpp"
#include <jansson.h>
#include <string>
#include <utility>

namespace sibyl {

struct NoteChange {
    size_t operationIndex = 0;
    std::string patternId;
    size_t matched = 0, updated = 0, inserted = 0, deleted = 0, clamped = 0;
    std::vector<std::string> noteIds, displacedIds;
    std::vector<std::pair<std::string, std::string>> createdIds;
    size_t idLimit = 128;
};

struct EditResult {
	bool valid = false;
	CompositionPtr composition;
	std::vector<NoteChange> changes;
	std::vector<ValidationIssue> errors;
	std::vector<ValidationIssue> warnings;
	std::string errorCode;
	std::string errorPath;
	std::string errorMessage;
};

// Applies ordered semantic operations to a private JSON copy, then validates and
// compiles the complete composition exactly once. The base snapshot is untouched.
EditResult applyCompositionEdit(const Composition& base, json_t* operations, int revision);

} // namespace sibyl
