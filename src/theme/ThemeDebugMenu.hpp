#pragma once

#include "../plugin.hpp"

namespace leviathan {
namespace theme {

// Temporary Phase 0 audition surface. It is debug-gated by the caller and can
// be removed when the dedicated Theme module UI exists.
void appendDebugThemeMenu(ui::Menu* menu);

} // namespace theme
} // namespace leviathan
