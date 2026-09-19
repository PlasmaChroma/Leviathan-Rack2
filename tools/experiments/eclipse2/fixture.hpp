#include "plugin.hpp"
#include "preview_benchmark_utils.hpp"
static bool debugEnabled = true;
bool isDragonKingDebugEnabled() { return debugEnabled; }

#include "visual/VisualAssets.hpp"
#include "visual/Eclipse2Track.hpp"
#include "Eclipse2RingExperiment.hpp"

// Use Rack's real framebuffer implementation with a minimal offline window.
namespace rack {
Context::~Context() {}
namespace window {
Window::Window():internal(nullptr){}
Window::~Window(){}
double Window::getFrameTime(){return 1.;}
double Window::getFrameDurationRemaining(){return 1.;}
int& Window::fbCount(){static int count=0;return count;}
bool& Window::fbDirtyOnSubpixelChange(){static bool enabled=false;return enabled;}
}
}
