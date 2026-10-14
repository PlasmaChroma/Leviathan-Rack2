#pragma once

#include "MoiraiTypes.hpp"

#include <jansson.h>

namespace moirai {

struct JsonResult {
	bool valid = false;
	Bank bank;
	std::vector<ValidationIssue> errors;
};

JsonResult parseBankJson(json_t* root);
json_t* bankToJson(const Bank& bank);

} // namespace moirai
