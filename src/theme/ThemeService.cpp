#include "ThemeService.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>

namespace leviathan {
namespace theme {
namespace {

std::mutex gThemeMutex;
ThemeState gThemeState = []() {
	ThemeState state;
	state.snapshot = canonicalDefault();
	state.generation = 1u;
	state.colorGeneration = 1u;
	state.surfaceGeneration = 1u;
	state.presetGeneration = 1u;
	return state;
}();

std::atomic<std::uint64_t> gPublishedGeneration {1u};
std::atomic<std::uint64_t> gPublishedColorGeneration {1u};
std::atomic<std::uint64_t> gPublishedSurfaceGeneration {1u};
std::atomic<std::uint64_t> gPublishedPresetGeneration {1u};

void increment(std::uint64_t* value, std::atomic<std::uint64_t>* published) {
	++(*value);
	published->store(*value, std::memory_order_release);
}

ThemeChange applyLocked(const ThemeSnapshot& candidate) {
	const ThemeSnapshot normalized = canonicalize(candidate);
	const bool colorsChanged = normalized.colors != gThemeState.snapshot.colors;
	const bool surfaceChanged = normalized.surface != gThemeState.snapshot.surface;
	if (!colorsChanged && !surfaceChanged) {
		return ChangeNone;
	}

	gThemeState.snapshot = normalized;
	increment(&gThemeState.generation, &gPublishedGeneration);
	std::uint32_t changed = ChangeNone;
	if (colorsChanged) {
		increment(&gThemeState.colorGeneration, &gPublishedColorGeneration);
		changed |= ChangeColors;
	}
	if (surfaceChanged) {
		increment(&gThemeState.surfaceGeneration, &gPublishedSurfaceGeneration);
		changed |= ChangeSurface;
	}
	return static_cast<ThemeChange>(changed);
}

} // namespace

ThemeSnapshot canonicalize(ThemeSnapshot snapshot) {
	snapshot.schemaVersion = kThemeSchemaVersion;
	if (!std::isfinite(snapshot.surface.textureAmount)) {
		snapshot.surface.textureAmount = canonicalDefault().surface.textureAmount;
	}
	snapshot.surface.textureAmount = std::max(0.f, std::min(2.f, snapshot.surface.textureAmount));
	return snapshot;
}

ThemeState read() {
	std::lock_guard<std::mutex> lock(gThemeMutex);
	return gThemeState;
}

std::uint64_t generation() {
	return gPublishedGeneration.load(std::memory_order_acquire);
}

std::uint64_t colorGeneration() {
	return gPublishedColorGeneration.load(std::memory_order_acquire);
}

std::uint64_t surfaceGeneration() {
	return gPublishedSurfaceGeneration.load(std::memory_order_acquire);
}

std::uint64_t presetGeneration() {
	return gPublishedPresetGeneration.load(std::memory_order_acquire);
}

ThemeColor color(ThemeRole role) {
	const ThemeState state = read();
	switch (role) {
		case ThemeRole::Input: return state.snapshot.colors.input;
		case ThemeRole::Output: return state.snapshot.colors.output;
		case ThemeRole::Accent: return state.snapshot.colors.accent;
		case ThemeRole::None:
		default: return {};
	}
}

ThemeChange setColor(ThemeRole role, ThemeColor value) {
	std::lock_guard<std::mutex> lock(gThemeMutex);
	ThemeSnapshot candidate = gThemeState.snapshot;
	switch (role) {
		case ThemeRole::Input: candidate.colors.input = value; break;
		case ThemeRole::Output: candidate.colors.output = value; break;
		case ThemeRole::Accent: candidate.colors.accent = value; break;
		case ThemeRole::None:
		default: return ChangeNone;
	}
	return applyLocked(candidate);
}

ThemeChange setTextureAmount(float amount) {
	std::lock_guard<std::mutex> lock(gThemeMutex);
	ThemeSnapshot candidate = gThemeState.snapshot;
	candidate.surface.textureAmount = amount;
	return applyLocked(candidate);
}

ThemeChange apply(const ThemeSnapshot& snapshot) {
	std::lock_guard<std::mutex> lock(gThemeMutex);
	return applyLocked(snapshot);
}

ThemeChange resetToDefault() {
	return apply(canonicalDefault());
}

void initialize(const ThemeSnapshot& snapshot) {
	std::lock_guard<std::mutex> lock(gThemeMutex);
	const ThemeSnapshot normalized = canonicalize(snapshot);
	if (normalized == gThemeState.snapshot) {
		return;
	}
	gThemeState.snapshot = normalized;
	increment(&gThemeState.generation, &gPublishedGeneration);
	increment(&gThemeState.colorGeneration, &gPublishedColorGeneration);
	increment(&gThemeState.surfaceGeneration, &gPublishedSurfaceGeneration);
}

} // namespace theme
} // namespace leviathan
