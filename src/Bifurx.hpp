#pragma once

#include "plugin.hpp"
#include "DebugTerminalTransport.hpp"
#include "MathHelpers.hpp"
#include "BifurxInputStage.hpp"
#include "BifurxOutputStage.hpp"
#include "BifurxOversampling.hpp"
#include "BifurxTransitionSmoother.hpp"
#include "BifurxTypes.hpp"
#include "BifurxDsp.hpp"
#include "BifurxModule.hpp"
#include "BifurxDisplay.hpp"
#include "BifurxRenderClient.hpp"
#include "PanelSvgUtils.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <vector>

namespace bifurx {

// Forward declarations
struct Bifurx;
struct BifurxSpectrumGLWidget;
struct BifurxUiRenderPayload;
struct BifurxUiRenderSnapshot;
Widget* createGlSpectrumDisplay(Bifurx* module, math::Rect rectMm);

} // namespace bifurx
