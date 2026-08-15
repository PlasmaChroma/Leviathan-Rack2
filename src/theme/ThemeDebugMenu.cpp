#include "ThemeDebugMenu.hpp"

#include "ThemePersistence.hpp"
#include "ThemeService.hpp"

namespace leviathan {
namespace theme {
namespace {

ThemeSnapshot factoryTheme(
	ThemeColor input,
	ThemeColor output,
	ThemeColor accent,
	float textureAmount) {
	ThemeSnapshot snapshot = canonicalDefault();
	snapshot.colors.input = input;
	snapshot.colors.output = output;
	snapshot.colors.accent = accent;
	snapshot.surface.textureAmount = textureAmount;
	return snapshot;
}

void addFactoryItem(ui::Menu* menu, const char* name, ThemeSnapshot snapshot) {
	menu->addChild(createCheckMenuItem(
		name,
		"",
		[snapshot]() { return read().snapshot == snapshot; },
		[snapshot]() {
			apply(snapshot);
			persistence::saveToUserStorage();
		}));
}

} // namespace

void appendDebugThemeMenu(ui::Menu* menu) {
	if (!menu || !isDragonKingDebugEnabled()) return;
	menu->addChild(new ui::MenuSeparator());
	menu->addChild(createSubmenuItem("Theme MVP", "", [](ui::Menu* submenu) {
		addFactoryItem(submenu, "Leviathan", canonicalDefault());
		addFactoryItem(submenu, "Abyssal", factoryTheme(
			ThemeColor(0x3f, 0x4c, 0x9a), ThemeColor(0x16, 0x7d, 0x8c),
			ThemeColor(0x2a, 0x33, 0x5f), 1.35f));
		addFactoryItem(submenu, "Monochrome", factoryTheme(
			ThemeColor(0xa7, 0xa9, 0xb0), ThemeColor(0xe1, 0xe3, 0xe8),
			ThemeColor(0x67, 0x6a, 0x73), 0.35f));
		addFactoryItem(submenu, "Ultraviolet", factoryTheme(
			ThemeColor(0xa4, 0x4d, 0xff), ThemeColor(0x35, 0xd8, 0xff),
			ThemeColor(0xff, 0x4d, 0xff), 1.20f));

		submenu->addChild(new ui::MenuSeparator());
		const float amounts[] = {0.f, 1.f, 2.f};
		const char* labels[] = {"Texture 0%", "Texture 100%", "Texture 200%"};
		for (int i = 0; i < 3; ++i) {
			const float amount = amounts[i];
			submenu->addChild(createCheckMenuItem(
				labels[i],
				"",
				[amount]() { return read().snapshot.surface.textureAmount == amount; },
				[amount]() {
					setTextureAmount(amount);
					persistence::saveToUserStorage();
				}));
		}
	}));
}

} // namespace theme
} // namespace leviathan
