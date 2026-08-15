#include "../src/theme/ThemeService.hpp"

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

	initialize(canonicalDefault());
	const ThemeState initial = read();
	check("canonical input color", initial.snapshot.colors.input == ThemeColor{0x7a, 0x5c, 0xff});
	check("canonical output color", initial.snapshot.colors.output == ThemeColor{0x1c, 0xcc, 0xd9});
	check("canonical accent color", initial.snapshot.colors.accent == ThemeColor{0x57, 0x40, 0xbf});

	check("equal apply is a no-op", apply(initial.snapshot) == ChangeNone);
	const ThemeState afterNoOp = read();
	check("equal apply preserves every generation",
		afterNoOp.generation == initial.generation
		&& afterNoOp.colorGeneration == initial.colorGeneration
		&& afterNoOp.surfaceGeneration == initial.surfaceGeneration);

	const ThemeColor red{0xff, 0x20, 0x30};
	check("input edit reports color change", setColor(ThemeRole::Input, red) == ChangeColors);
	const ThemeState afterColor = read();
	check("input edit changes only color generations",
		afterColor.snapshot.colors.input == red
		&& afterColor.generation == initial.generation + 1u
		&& afterColor.colorGeneration == initial.colorGeneration + 1u
		&& afterColor.surfaceGeneration == initial.surfaceGeneration);
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

	resetToDefault();
	check("reset restores canonical snapshot", read().snapshot == canonicalDefault());

	if (failures) {
		std::cerr << failures << " theme service test(s) failed\n";
		return 1;
	}
	std::cout << "Theme service spec passed\n";
	return 0;
}
