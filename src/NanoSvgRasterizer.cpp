// Keep NanoSVG's rasterizer implementation in one neutral translation unit so
// visual components can share it without depending on another module's UI.
#include <cstdlib>
#include <cstring>

#include <nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>
