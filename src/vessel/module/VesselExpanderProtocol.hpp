#pragma once

#include <cstdint>

namespace vessel_expander {

constexpr std::uint32_t kMagic = 0x5654554eu; // "VTUN"
constexpr std::uint32_t kVersion = 1u;

struct TuneMessage {
    std::uint32_t magic = 0; // Valid only after the sender publishes.
    std::uint32_t version = kVersion;
    float velocity = 0.5f;
    float speed = 0.2f;
    float pressure = 2.5f;
    float sustain = 0.f;
    float imperfection = 1.f;
    float width = 0.7f;
    float level = 1.f;
};

inline bool isValid(const TuneMessage& message) {
    return message.magic == kMagic && message.version == kVersion;
}

} // namespace vessel_expander
