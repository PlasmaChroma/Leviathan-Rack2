#include "../src/plugin.hpp"
#include "../src/theme/ThemePersistence.hpp"

#include <iostream>
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
	check("theme saves when its parent directory already exists",
		saveDocumentAtomic(path, written));

	ThemeDocument loaded;
	check("saved theme document reloads", loadDocument(path, &loaded) == LoadStatus::Loaded);
	check("saved active theme values survive reload",
		loaded.activePreset == "modified"
			&& loaded.active.colors.input == ThemeColor{0x12, 0x34, 0x56});

	system::removeRecursively(root);
	std::cout << "[SUMMARY] theme_persistence_spec: "
		<< (failures == 0 ? "passed" : "failed") << '\n';
	return failures == 0 ? 0 : 1;
}
