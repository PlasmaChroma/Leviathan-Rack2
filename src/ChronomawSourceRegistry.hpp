#pragma once

#include <string>

namespace chronomaw {

enum class SourceKind {
	Output,
	CvInput,
	ClockInput,
	RunInput,
	ExpanderCv,
	ExpanderButton,
};

struct SourceDescriptor {
	int id = -1;
	std::string label;
	SourceKind kind = SourceKind::CvInput;
	bool audioRate = false;
	bool available = true;
	std::string unavailableReason;
	float nominalMinV = 0.f;
	float nominalMaxV = 5.f;
};

} // namespace chronomaw

