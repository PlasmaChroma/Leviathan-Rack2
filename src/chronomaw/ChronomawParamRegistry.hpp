#pragma once

#include "ChronomawState.hpp"

namespace chronomaw {

enum class ParityStatus {
	FrozenChronomawV1,
	ManualDerived,
	Approximation,
	NeedsGoldenTest,
	Deferred,
};

struct ParamMeta {
	const char* label = "";
	ParityStatus parity = ParityStatus::FrozenChronomawV1;
};

} // namespace chronomaw

