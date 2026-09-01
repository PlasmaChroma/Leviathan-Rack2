#pragma once

#include "PhonexSequenceCompiler.hpp"


namespace phonex {

constexpr std::size_t kMaxSubmittedTextBytes = 256;

struct TextCompileResult {
	CompileStatus status = CompileStatus::Empty;
	bool unsupportedUnicode = false;
};

TextCompileResult compileText(StringView source, LpcSequence& output);

} // namespace phonex
