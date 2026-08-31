#pragma once

#include "PhonexTypes.hpp"

namespace phonex {

LpcSequence makeSilenceFixture(std::uint16_t frameCount = 4);
LpcSequence makeVoicedFixture(std::uint16_t frameCount = 8);
LpcSequence makeUnvoicedFixture(std::uint16_t frameCount = 8);

} // namespace phonex
