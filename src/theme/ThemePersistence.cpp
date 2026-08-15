#include "ThemePersistence.hpp"

#include "ThemeService.hpp"
#include "../plugin.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <jansson.h>

namespace leviathan {
namespace theme {
namespace persistence {
namespace {

ThemeDocument gDocument = defaultDocument();
bool gMayOverwriteDocument = true;

std::string userThemePath() {
	return system::join(asset::user(), "Leviathan/theme.json");
}

int hexNibble(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return 10 + c - 'a';
	if (c >= 'A' && c <= 'F') return 10 + c - 'A';
	return -1;
}

bool parseColor(json_t* value, ThemeColor* color) {
	if (!color || !json_is_string(value)) return false;
	const char* raw = json_string_value(value);
	if (!raw) return false;
	std::string text(raw);
	if (!text.empty() && text[0] == '#') text.erase(text.begin());
	if (text.size() != 6u) return false;
	int digits[6];
	for (int i = 0; i < 6; ++i) {
		digits[i] = hexNibble(text[std::size_t(i)]);
		if (digits[i] < 0) return false;
	}
	color->r = std::uint8_t(digits[0] * 16 + digits[1]);
	color->g = std::uint8_t(digits[2] * 16 + digits[3]);
	color->b = std::uint8_t(digits[4] * 16 + digits[5]);
	return true;
}

std::string colorText(ThemeColor color) {
	char buffer[8];
	std::snprintf(buffer, sizeof(buffer), "#%02X%02X%02X", color.r, color.g, color.b);
	return buffer;
}

void readSnapshot(json_t* object, ThemeSnapshot* snapshot) {
	if (!snapshot || !json_is_object(object)) return;
	ThemeSnapshot candidate = *snapshot;
	parseColor(json_object_get(object, "input"), &candidate.colors.input);
	parseColor(json_object_get(object, "output"), &candidate.colors.output);
	parseColor(json_object_get(object, "accent"), &candidate.colors.accent);
	json_t* texture = json_object_get(object, "textureAmount");
	if (json_is_number(texture)) candidate.surface.textureAmount = float(json_number_value(texture));
	*snapshot = canonicalize(candidate);
}

json_t* snapshotToJson(const ThemeSnapshot& snapshot) {
	const ThemeSnapshot normalized = canonicalize(snapshot);
	json_t* object = json_object();
	json_object_set_new(object, "input", json_string(colorText(normalized.colors.input).c_str()));
	json_object_set_new(object, "output", json_string(colorText(normalized.colors.output).c_str()));
	json_object_set_new(object, "accent", json_string(colorText(normalized.colors.accent).c_str()));
	json_object_set_new(object, "textureAmount", json_real(normalized.surface.textureAmount));
	return object;
}

bool isKnownActivePreset(const std::string& value) {
	if (value == "modified" || value == "factory:leviathan" || value == "factory:abyssal"
		|| value == "factory:monochrome" || value == "factory:ultraviolet") return true;
	if (value.size() == 6u && value.compare(0u, 5u, "user:") == 0)
		return value[5] >= '1' && value[5] <= '8';
	return false;
}

} // namespace

ThemeDocument defaultDocument() {
	return ThemeDocument{};
}

LoadStatus loadDocument(const std::string& path, ThemeDocument* document) {
	if (!document) return LoadStatus::Invalid;
	FILE* file = std::fopen(path.c_str(), "rb");
	if (!file) return errno == ENOENT ? LoadStatus::Missing : LoadStatus::Invalid;
	json_error_t error;
	json_t* root = json_loadf(file, 0, &error);
	std::fclose(file);
	if (!json_is_object(root)) {
		if (root) json_decref(root);
		return LoadStatus::Invalid;
	}

	json_t* schema = json_object_get(root, "schemaVersion");
	if (json_is_integer(schema) && json_integer_value(schema) > kThemeSchemaVersion) {
		json_decref(root);
		return LoadStatus::FutureSchema;
	}

	ThemeDocument candidate = defaultDocument();
	json_t* activePreset = json_object_get(root, "activePreset");
	if (json_is_string(activePreset)) {
		const std::string value = json_string_value(activePreset);
		if (isKnownActivePreset(value)) candidate.activePreset = value;
	}
	readSnapshot(json_object_get(root, "active"), &candidate.active);

	json_t* presets = json_object_get(root, "userPresets");
	if (json_is_array(presets)) {
		const std::size_t count = std::min<std::size_t>(candidate.userPresets.size(), json_array_size(presets));
		for (std::size_t i = 0; i < count; ++i) {
			json_t* preset = json_array_get(presets, i);
			if (!json_is_object(preset)) continue;
			json_t* name = json_object_get(preset, "name");
			if (!json_is_string(name)) continue;
			const std::string presetName = json_string_value(name);
			if (presetName.empty()) continue;
			candidate.userPresets[i].occupied = true;
			candidate.userPresets[i].name = presetName;
			readSnapshot(preset, &candidate.userPresets[i].theme);
		}
	}
	json_decref(root);
	*document = candidate;
	return LoadStatus::Loaded;
}

bool saveDocumentAtomic(const std::string& path, const ThemeDocument& document) {
	const std::string directory = system::getDirectory(path);
	if (!system::createDirectories(directory)) return false;
	const std::string temporary = path + ".tmp";
	json_t* root = json_object();
	json_object_set_new(root, "schemaVersion", json_integer(kThemeSchemaVersion));
	json_object_set_new(root, "activePreset", json_string(document.activePreset.c_str()));
	json_object_set_new(root, "active", snapshotToJson(document.active));
	json_t* presets = json_array();
	for (const UserPresetSlot& slot : document.userPresets) {
		if (!slot.occupied) {
			json_array_append_new(presets, json_null());
			continue;
		}
		json_t* preset = snapshotToJson(slot.theme);
		json_object_set_new(preset, "name", json_string(slot.name.c_str()));
		json_array_append_new(presets, preset);
	}
	json_object_set_new(root, "userPresets", presets);

	FILE* file = std::fopen(temporary.c_str(), "wb");
	bool written = false;
	if (file) {
		written = json_dumpf(root, file, JSON_INDENT(2) | JSON_SORT_KEYS) == 0;
		written = std::fflush(file) == 0 && written;
		written = std::fclose(file) == 0 && written;
	}
	json_decref(root);
	if (!written || !system::rename(temporary, path)) {
		system::remove(temporary);
		return false;
	}
	return true;
}

void initializeFromUserStorage() {
	gDocument = defaultDocument();
	const LoadStatus status = loadDocument(userThemePath(), &gDocument);
	gMayOverwriteDocument = status != LoadStatus::FutureSchema;
	if (status == LoadStatus::Invalid && isDragonKingDebugEnabled())
		WARN("Leviathan Theme: invalid theme.json; using canonical defaults");
	if (status == LoadStatus::FutureSchema && isDragonKingDebugEnabled())
		WARN("Leviathan Theme: future theme.json schema preserved; using canonical defaults");
	initialize(gDocument.active);
}

void saveToUserStorage() {
	if (!gMayOverwriteDocument) return;
	const ThemeSnapshot current = read().snapshot;
	if (current != gDocument.active) gDocument.activePreset = "modified";
	gDocument.active = current;
	if (!saveDocumentAtomic(userThemePath(), gDocument) && isDragonKingDebugEnabled())
		WARN("Leviathan Theme: failed to save theme.json");
}

} // namespace persistence
} // namespace theme
} // namespace leviathan
