#pragma once

#include "theme/ThemeTypes.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>

namespace deepcache {

inline bool usesLeviathanTheme(const std::string& pluginSlug) {
    return pluginSlug == "Leviathan"; // Pro previews use their raster artwork.
}

// Hash rendered values, not process-local generation counters or preset names.
// Explicit bytes avoid padding, locale and host-endian differences on disk.
inline std::uint64_t themeVisualIdentity(const leviathan::theme::ThemeSnapshot& snapshot) {
    std::uint64_t hash = UINT64_C(14695981039346656037);
    auto byte = [&](unsigned value) { hash ^= value & 255u; hash *= UINT64_C(1099511628211); };
    auto color = [&](leviathan::theme::ThemeColor c) { byte(c.r); byte(c.g); byte(c.b); };
    byte(1); // Visual identity schema.
    color(snapshot.colors.input); color(snapshot.colors.output);
    color(snapshot.colors.textInput); color(snapshot.colors.textOutput);
    byte(snapshot.colors.backgroundEnabled ? 1 : 0);
    if (snapshot.colors.backgroundEnabled) color(snapshot.colors.background);
    float texture = snapshot.surface.textureAmount;
    if (texture == 0.f) texture = 0.f; // Canonicalize negative zero.
    std::uint32_t bits = 0;
    static_assert(sizeof(bits) == sizeof(texture), "32-bit float required");
    std::memcpy(&bits, &texture, sizeof(bits));
    for (unsigned shift = 0; shift < 32; shift += 8) byte(bits >> shift);
    return hash;
}

class ThemeRefreshDebounce {
public:
    explicit ThemeRefreshDebounce(std::uint64_t identity) : applied_(identity), observed_(identity) {}
    void observe(std::uint64_t identity, double now) {
        if (identity == observed_) return;
        observed_ = identity;
        changedAt_ = now;
    }
    bool pending() const { return observed_ != applied_; }
    // The picker publishes at one-second intervals while dragging. Wait longer
    // than that cadence so intermediate colors do not repeatedly rebuild previews.
    bool applyIfSettled(double now) {
        if (!pending() || !std::isfinite(now) || now - changedAt_ < 3.0) return false;
        applied_ = observed_;
        return true;
    }
    std::uint64_t applied() const { return applied_; }
private:
    std::uint64_t applied_, observed_;
    double changedAt_ = 0;
};

} // namespace deepcache
