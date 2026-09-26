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
    debounce.observe(original + 1, 1.0);
    need(debounce.pending() && !debounce.applyIfSettled(1.2), "wait during a drag");
    debounce.observe(original + 2, 1.2);
    need(!debounce.applyIfSettled(1.4) && debounce.applyIfSettled(1.6), "coalesce edits until settled");
    need(debounce.applied() == original + 2 && !debounce.pending(), "publish newest identity once");
    debounce.observe(original + 3, 2.0); debounce.observe(original + 2, 2.1);
    need(!debounce.pending() && !debounce.applyIfSettled(3), "reverted drag needs no refresh");
    std::puts("PASS: Leviathan-only theme fingerprint and refresh debounce");
}
