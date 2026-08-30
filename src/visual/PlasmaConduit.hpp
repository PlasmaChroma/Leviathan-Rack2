#pragma once

#include "../plugin.hpp"

#include <string>

namespace visual_assets {

// Standard panel convention: author one open centerline path per conduit in a
// group with this ID. Path direction controls the yellow-to-amber gradient.
extern const char* const kPlasmaConduitAnchorGroupId;

// Creates one cached native conduit layer from the standard anchor group in a
// split panel SVG. Returns nullptr when the panel has no valid centerlines.
widget::FramebufferWidget* createPlasmaConduitLayer(
	const std::string& panelSvgPath,
	Vec panelSizePx);

} // namespace visual_assets
