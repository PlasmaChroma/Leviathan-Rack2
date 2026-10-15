#include "../src/theme/ThemeService.hpp"
#include "../src/theme/ThemePresets.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

namespace {

int failures = 0;

void check(const std::string& name, bool pass) {
	std::cout << (pass ? "[PASS] " : "[FAIL] ") << name << '\n';
	if (!pass) ++failures;
}

} // namespace

int main() {
	using namespace leviathan::theme;

	initialize(canonicalDefault(), "factory:leviathan");
	std::size_t factoryCount = 0u;
	const FactoryPreset* factory = factoryPresets(&factoryCount);
	check("six stable factory presets are registered", factory && factoryCount == 6u);
	ThemeSnapshot leviathanFactory = canonicalDefault();
	leviathanFactory.colors.background = ThemeColor(0, 0, 0);
	leviathanFactory.colors.backgroundEnabled = true;
	check("Leviathan factory preset explicitly applies its black background",
		findFactoryPreset("factory:leviathan")
		&& findFactoryPreset("factory:leviathan")->snapshot == leviathanFactory);
	check("unknown factory preset is rejected", findFactoryPreset("factory:unknown") == nullptr);
	const ThemeState initial = read();
	check("canonical input color matches classic panel purple",
		initial.snapshot.colors.input == ThemeColor{0x57, 0x40, 0xbf});
	check("canonical output color", initial.snapshot.colors.output == ThemeColor{0x1c, 0xcc, 0xd9});
	check("canonical text color is white", initial.snapshot.colors.textInput == ThemeColor{0xff, 0xff, 0xff});
	const FactoryPreset* mono = findFactoryPreset("factory:monochrome");
	const FactoryPreset* allHallows = findFactoryPreset("factory:all-hallows");
	const FactoryPreset* saturnalia = findFactoryPreset("factory:saturnalia");
	check("Saturnalia uses wine red, evergreen, gold text, and a warm dark background",
		saturnalia && std::string(saturnalia->name) == "Saturnalia"
		&& saturnalia->snapshot.colors.input == ThemeColor(0xb8, 0x32, 0x48)
		&& saturnalia->snapshot.colors.output == ThemeColor(0x24, 0x80, 0x58)
		&& saturnalia->snapshot.colors.textInput == ThemeColor(0xff, 0xe4, 0xa0)
		&& saturnalia->snapshot.colors.textOutput == ThemeColor(0xff, 0xe4, 0xa0)
		&& saturnalia->snapshot.colors.background == ThemeColor(0x10, 0x0d, 0x08)
		&& saturnalia->snapshot.colors.backgroundEnabled
		&& std::fabs(saturnalia->snapshot.surface.textureAmount - 1.10f) < 1e-6f);
	check("All Hallows uses pumpkin orange, violet, warm bone text, and a midnight background",
		allHallows && std::string(allHallows->name) == "All Hallows"
		&& allHallows->snapshot.colors.input == ThemeColor(0xf0, 0x78, 0x18)
		&& allHallows->snapshot.colors.output == ThemeColor(0x7e, 0x3f, 0xc4)
		&& allHallows->snapshot.colors.textInput == ThemeColor(0xff, 0xf0, 0xd2)
		&& allHallows->snapshot.colors.textOutput == ThemeColor(0xff, 0xf0, 0xd2)
		&& allHallows->snapshot.colors.background == ThemeColor(0x10, 0x0b, 0x16)
		&& allHallows->snapshot.colors.backgroundEnabled
		&& std::fabs(allHallows->snapshot.surface.textureAmount - 1.25f) < 1e-6f);
	check("Mono preset uses its representative input/output contrast",
		mono && mono->snapshot.colors.input == ThemeColor{0xba, 0xba, 0xba}
		&& mono->snapshot.colors.output == ThemeColor{0x32, 0x32, 0x32}
		&& mono->snapshot.colors.textInput == ThemeColor{0xff, 0xff, 0xff}
		&& std::fabs(mono->snapshot.surface.textureAmount - 0.50f) < 1e-6f);

	check("equal apply is a no-op", apply(initial.snapshot) == ChangeNone);
	const ThemeState afterNoOp = read();
	check("equal apply preserves every generation",
		afterNoOp.generation == initial.generation
		&& afterNoOp.colorGeneration == initial.colorGeneration
		&& afterNoOp.surfaceGeneration == initial.surfaceGeneration);

	const ThemeColor red{0xff, 0x20, 0x30};
	const ThemeChange inputChange = setColor(ThemeRole::Input, red);
	check("input edit reports color and modified-preset changes",
		(std::uint32_t(inputChange) & ChangeColors) != 0u
		&& (std::uint32_t(inputChange) & ChangePresets) != 0u);
	const ThemeState afterColor = read();
	check("input edit advances color and active-preset generations",
		afterColor.snapshot.colors.input == red
		&& afterColor.generation == initial.generation + 1u
		&& afterColor.colorGeneration == initial.colorGeneration + 1u
		&& afterColor.surfaceGeneration == initial.surfaceGeneration
		&& afterColor.presetGeneration == initial.presetGeneration + 1u
		&& afterColor.activePreset == "modified");
	check("None role cannot mutate state", setColor(ThemeRole::None, red) == ChangeNone);

	check("texture edit reports surface change", setTextureAmount(3.f) == ChangeSurface);
	const ThemeState afterSurface = read();
	check("texture is clamped and isolated",
		afterSurface.snapshot.surface.textureAmount == 2.f
		&& afterSurface.colorGeneration == afterColor.colorGeneration
		&& afterSurface.surfaceGeneration == afterColor.surfaceGeneration + 1u);

	setTextureAmount(std::numeric_limits<float>::quiet_NaN());
	check("non-finite texture restores canonical amount",
		read().snapshot.surface.textureAmount == canonicalDefault().surface.textureAmount);

	ThemeSnapshot both = read().snapshot;
	both.colors.output = {0x10, 0x20, 0x30};
	both.surface.textureAmount = 0.25f;
	const ThemeState beforeBoth = read();
	const ThemeChange bothChange = apply(both);
	const ThemeState afterBoth = read();
	check("combined apply reports both domains",
		(std::uint32_t(bothChange) & ChangeColors) != 0u
		&& (std::uint32_t(bothChange) & ChangeSurface) != 0u);
	check("combined apply increments overall generation once",
		afterBoth.generation == beforeBoth.generation + 1u
		&& afterBoth.colorGeneration == beforeBoth.colorGeneration + 1u
		&& afterBoth.surfaceGeneration == beforeBoth.surfaceGeneration + 1u);
	check("atomic generation probes match coherent state",
		generation() == afterBoth.generation
		&& colorGeneration() == afterBoth.colorGeneration
		&& surfaceGeneration() == afterBoth.surfaceGeneration
		&& presetGeneration() == afterBoth.presetGeneration);

	const ThemeState beforePreset = read();
	check("preset identity change reports the preset domain",
		(std::uint32_t(applyPreset(beforePreset.snapshot, "factory:abyssal")) & ChangePresets) != 0u);
	const ThemeState afterPreset = read();
	check("equal snapshot with new preset identity advances only preset generations",
		afterPreset.generation == beforePreset.generation + 1u
		&& afterPreset.presetGeneration == beforePreset.presetGeneration + 1u
		&& afterPreset.colorGeneration == beforePreset.colorGeneration
		&& afterPreset.surfaceGeneration == beforePreset.surfaceGeneration
		&& afterPreset.activePreset == "factory:abyssal");
	check("reselecting the same preset identity is a no-op",
		applyPreset(afterPreset.snapshot, "factory:abyssal") == ChangeNone);

	resetToDefault();
	check("reset restores canonical snapshot", read().snapshot == canonicalDefault());
	const ThemeState beforeText = read();
	setColor(ThemeRole::TextInput, red);
	check("input text changes independently", color(ThemeRole::TextInput) == red
		&& color(ThemeRole::TextOutput) == beforeText.snapshot.colors.textOutput
		&& read().colorGeneration == beforeText.colorGeneration + 1u);
	const ThemeColor blue{0x10, 0x20, 0xff};
	setColor(ThemeRole::TextOutput, blue);
	check("output text changes independently", color(ThemeRole::TextInput) == red
		&& color(ThemeRole::TextOutput) == blue
		&& read().snapshot.colors.input == beforeText.snapshot.colors.input
		&& read().snapshot.colors.output == beforeText.snapshot.colors.output
		&& read().surfaceGeneration == beforeText.surfaceGeneration);
	const ThemeState beforeBackground = read();
	check("authored background is the default", !beforeBackground.snapshot.colors.backgroundEnabled);
	setColor(ThemeRole::Background, blue);
	check("background edit enables a custom color without changing text or texture",
		read().snapshot.colors.backgroundEnabled && color(ThemeRole::Background) == blue
		&& color(ThemeRole::TextInput) == red && color(ThemeRole::TextOutput) == blue
		&& read().colorGeneration == beforeBackground.colorGeneration + 1u
		&& read().surfaceGeneration == beforeBackground.surfaceGeneration);
	setBackgroundEnabled(false);
	check("original background restores independently and retains the custom color",
		!read().snapshot.colors.backgroundEnabled && color(ThemeRole::Background) == blue
		&& read().colorGeneration == beforeBackground.colorGeneration + 2u);
	check("restoring original again is a no-op", setBackgroundEnabled(false) == ChangeNone);
	setBackgroundEnabled(true);
	check("custom background can be re-enabled", read().snapshot.colors.backgroundEnabled);
	resetToDefault();
	check("reset restores the authored background", !read().snapshot.colors.backgroundEnabled);
	const ThemeColor expectedBackgrounds[] = {
		ThemeColor(0x00, 0x00, 0x00), ThemeColor(0x05, 0x0a, 0x18),
		ThemeColor(0x08, 0x08, 0x08), ThemeColor(0x10, 0x05, 0x1c),
		ThemeColor(0x10, 0x0b, 0x16), ThemeColor(0x10, 0x0d, 0x08)};
	for (std::size_t i = 0; i < factoryCount; ++i) {
		setColor(ThemeRole::Background, blue);
		setBackgroundEnabled(false);
		applyPreset(factory[i].snapshot, factory[i].id);
		check(std::string(factory[i].name) + " factory field replaces and enables the background",
			read().snapshot.colors.backgroundEnabled
			&& color(ThemeRole::Background) == expectedBackgrounds[i]
			&& read().snapshot == factory[i].snapshot
			&& read().activePreset == factory[i].id);
	}

	if (failures) {
		std::cerr << failures << " theme service test(s) failed\n";
		return 1;
	}
	std::cout << "Theme service spec passed\n";
	return 0;
}
