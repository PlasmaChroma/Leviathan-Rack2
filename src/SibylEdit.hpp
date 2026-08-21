#pragma once

#include "SibylJSON.hpp"
#include <jansson.h>
#include <string>

namespace sibyl {

struct EditResult {
	bool valid = false;
	CompositionPtr composition;
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
