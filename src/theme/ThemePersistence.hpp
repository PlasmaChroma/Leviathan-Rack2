#pragma once

#include "ThemeTypes.hpp"

#include <array>
#include <string>

namespace leviathan {
namespace theme {
namespace persistence {

struct UserPresetSlot {
	bool occupied = false;
	std::string name;
	ThemeSnapshot theme = canonicalDefault();
};

struct ThemeDocument {
	std::uint32_t schemaVersion = kThemeSchemaVersion;
	std::string activePreset = "factory:leviathan";
	ThemeSnapshot active = canonicalDefault();
	std::array<UserPresetSlot, 8> userPresets;
};

enum class LoadStatus {
	Loaded,
	Missing,
	Invalid,
	FutureSchema
};

ThemeDocument defaultDocument();
LoadStatus loadDocument(const std::string& path, ThemeDocument* document);
bool saveDocumentAtomic(const std::string& path, const ThemeDocument& document);

void initializeFromUserStorage();
// Leviathan Pro follows the family-wide theme document because it has a
// separately linked ThemeService. Calls are globally rate-limited and inspect
// file metadata only; the document is parsed only after it changes.
void refreshExternalThemeIfChanged();
void saveToUserStorage();
bool applyFactoryPresetAndSave(const char* presetId);
void resetToCanonicalAndSave();

} // namespace persistence
} // namespace theme
} // namespace leviathan
