#pragma once

#include "MoiraiTypes.hpp"

#include <jansson.h>

namespace moirai {

struct EditResult {
	bool valid = false;
	Bank bank;
	CompiledBankPtr compiledBank;
	ApplyAt applyAt = ApplyAt::IMMEDIATE;
	ActiveVoicePolicy activeVoicePolicy = ActiveVoicePolicy::FINISH_CURRENT;
	std::vector<ValidationIssue> errors;
	std::vector<ValidationIssue> warnings;
	std::string errorCode;
	std::string errorPath;
	std::string errorMessage;
	int currentRevision = -1;
};

// Applies a revision-guarded ordered transaction to a private document copy.
// The base bank is never mutated and the complete result is compiled once.
EditResult applyBankEdit(const Bank& base, json_t* request);

} // namespace moirai
