#include "DeepcacheThemeIdentity.hpp"
#include <cstdio>
#include <cstdlib>

static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
int main() {
    using namespace deepcache;
    auto base = leviathan::theme::canonicalDefault();
    const auto original = themeVisualIdentity(base);
    need(usesLeviathanTheme("Leviathan") && !usesLeviathanTheme("Leviathan-Pro") &&
         !usesLeviathanTheme("LeviathanPro") && !usesLeviathanTheme("Other"), "exact plugin scope");
    for (unsigned role = 0; role < 6; ++role) {
        auto edited = base;
        switch (role) {
            case 0: ++edited.colors.input.r; break;
            case 1: ++edited.colors.output.g; break;
            case 2: --edited.colors.textInput.b; break;
            case 3: --edited.colors.textOutput.r; break;
            case 4: edited.colors.backgroundEnabled = true; break;
            case 5: edited.surface.textureAmount = 0.25f; break;
        }
        need(themeVisualIdentity(edited) != original, "every rendered setting changes cache identity");
    }
    auto hidden = base; ++hidden.colors.background.r;
    need(themeVisualIdentity(hidden) == original, "disabled background color does not redraw");
    hidden.colors.backgroundEnabled = base.colors.backgroundEnabled = true;
    need(themeVisualIdentity(hidden) != themeVisualIdentity(base), "enabled background hue changes identity");
    need(themeVisualIdentity(leviathan::theme::canonicalDefault()) == original, "identity survives restart/reset");
    ThemeRefreshDebounce debounce(original);
    // Match the picker's one-second publication cadence over a sustained drag.
    for (unsigned edit = 1; edit <= 6; ++edit) {
        debounce.observe(original + edit, double(edit));
        need(debounce.pending() && !debounce.applyIfSettled(double(edit) + 0.9),
             "no rebuild between one-second drag publications");
    }
    need(!debounce.applyIfSettled(8.99) && debounce.applyIfSettled(9.0),
         "refresh only after three seconds without another edit");
    need(debounce.applied() == original + 6 && !debounce.pending() &&
         !debounce.applyIfSettled(10.0), "publish newest identity exactly once");
    debounce.observe(original + 7, 11.0); debounce.observe(original + 6, 11.1);
    need(!debounce.pending() && !debounce.applyIfSettled(15.0), "reverted drag needs no refresh");
    std::puts("PASS: Leviathan-only theme fingerprint and refresh debounce");
}
