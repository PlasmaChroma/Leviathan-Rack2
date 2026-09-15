#include "../src/plugin.hpp"
#include "../src/theme/ThemePersistence.hpp"

#include <iostream>
#include <fstream>
#include <string>

Plugin* pluginInstance = nullptr;

bool isDragonKingDebugEnabled() {
	return false;
}

namespace {

int failures = 0;

void check(const char* name, bool passed) {
	std::cout << (passed ? "[PASS] " : "[FAIL] ") << name << '\n';
	if (!passed) ++failures;
}

} // namespace

int main() {
	using namespace leviathan::theme;
	using namespace leviathan::theme::persistence;

	const std::string root = system::join(
		system::getTempDirectory(), "leviathan-theme-persistence-spec");
	system::removeRecursively(root);
	check("fixture directory is created before saving", system::createDirectories(root));

	const std::string path = system::join(root, "theme.json");
	ThemeDocument written = defaultDocument();
	written.activePreset = "modified";
	written.active.colors.input = ThemeColor{0x12, 0x34, 0x56};
	written.active.colors.textInput = ThemeColor{0x11, 0x22, 0x33};
	written.active.colors.textOutput = ThemeColor{0xaa, 0xbb, 0xcc};
	written.active.colors.background = ThemeColor{0x33, 0x22, 0x11};
	written.active.colors.backgroundEnabled = true;
	written.userPresets[0].occupied = true;
	written.userPresets[0].name = "Two text colors";
	written.userPresets[0].theme = written.active;
	check("theme saves when its parent directory already exists",
		saveDocumentAtomic(path, written));

	ThemeDocument loaded;
	check("saved theme document reloads", loadDocument(path, &loaded) == LoadStatus::Loaded);
	check("saved active theme values survive reload",
		loaded.activePreset == "modified"
			&& loaded.active.colors.input == ThemeColor{0x12, 0x34, 0x56});
	check("active and preset colors plus background mode survive V3 round trip",
		loaded.schemaVersion == 3u && loaded.active == written.active
		&& loaded.userPresets[0].theme == written.active);
	{
		std::ofstream fixture(path);
		fixture << R"({"schemaVersion":1,"active":{"text":"#123456"},"userPresets":[{"name":"Legacy","text":"#ABCDEF"}]})";
	}
	check("V1 active and user presets migrate both text colors",
		loadDocument(path, &loaded) == LoadStatus::Loaded
		&& loaded.active.colors.textInput == ThemeColor{0x12, 0x34, 0x56}
		&& loaded.active.colors.textOutput == loaded.active.colors.textInput
		&& loaded.userPresets[0].theme.colors.textInput == ThemeColor{0xab, 0xcd, 0xef}
		&& loaded.userPresets[0].theme.colors.textOutput == loaded.userPresets[0].theme.colors.textInput);
	{
		std::ofstream fixture(path);
		fixture << R"({"schemaVersion":2,"active":{"text":"#123456","textInput":"#102030","textOutput":"invalid"}})";
	}
	check("explicit V2 colors override legacy fallback, malformed fields preserve it",
		loadDocument(path, &loaded) == LoadStatus::Loaded
		&& loaded.active.colors.textInput == ThemeColor{0x10, 0x20, 0x30}
		&& loaded.active.colors.textOutput == ThemeColor{0x12, 0x34, 0x56});
	check("V2 preserves original backgrounds", !loaded.active.colors.backgroundEnabled);
	{
		std::ofstream fixture(path);
		fixture << R"({"schemaVersion":3,"active":{"background":"#ABCDEF","backgroundEnabled":false}})";
	}
	check("disabled custom color survives loading",
		loadDocument(path, &loaded) == LoadStatus::Loaded
		&& !loaded.active.colors.backgroundEnabled
		&& loaded.active.colors.background == ThemeColor{0xab, 0xcd, 0xef});
	{
		std::ofstream fixture(path);
		fixture << R"({"schemaVersion":4})";
	}
	check("future schema is still rejected", loadDocument(path, &loaded) == LoadStatus::FutureSchema);

	system::removeRecursively(root);
	std::cout << "[SUMMARY] theme_persistence_spec: "
		<< (failures == 0 ? "passed" : "failed") << '\n';
	return failures == 0 ? 0 : 1;
}
