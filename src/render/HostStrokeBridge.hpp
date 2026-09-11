#pragma once
#include "../plugin.hpp"

namespace lumin {
// Rack 2.6.6 bridge for open, round-join/butt-cap preview strokes.
// Uses the host's current paint/tint/scissor/composite/AA/width state. Consumes
// the current path, like the caller's ordinary path construction would.
class HostStrokeBridge {
 struct Impl;
 Impl* impl=nullptr;
public:
 HostStrokeBridge();
 ~HostStrokeBridge();
 HostStrokeBridge(const HostStrokeBridge&)=delete;
 HostStrokeBridge& operator=(const HostStrokeBridge&)=delete;
 bool stroke(NVGcontext* host,const rack::math::Vec* points,int count);
};
}
