#include "Nautiloid.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"

#include <nanovg_gl.h>

#include <fstream>

namespace {

constexpr float kNautiloidWidthMm = 101.6f;
constexpr float kNautiloidHeightMm = 128.5f;
constexpr float kNautiloidMaxFractalZoom = nautiloid_location::kMaxZoom;
constexpr float kNautiloidWheelZoomStep = 0.015f;

#define NAUTILOID_GLSL_STRINGIFY_DETAIL(x) #x
#define NAUTILOID_GLSL_STRINGIFY(x) NAUTILOID_GLSL_STRINGIFY_DETAIL(x)

std::string nautiloidUserRootPath() {
  return system::join(asset::user(), "Leviathan/Nautiloid");
}

std::string nautiloidDebugLogPath() {
  return system::join(nautiloidUserRootPath(), "fractal-pipeline.csv");
}

Vec nautiloidFractalViewportHalfSpan(int mode) {
  switch (mode) {
    case iris::FRACTAL_MANDELBROT:
      return Vec(1.62f, 0.86f);
    case iris::FRACTAL_MULTIBROT:
      return Vec(1.45f, 0.90f);
    case iris::FRACTAL_MULTIJULIA:
      return Vec(1.45f, 0.84f);
    case iris::FRACTAL_JULIA:
      return Vec(1.58f, 0.72f);
    case iris::FRACTAL_PHOENIX_JULIA:
      return Vec(1.62f, 0.74f);
    case iris::FRACTAL_MANOWAR:
      return Vec(1.65f, 0.95f);
    case iris::FRACTAL_BURNING_SHIP:
      return Vec(0.42f, 0.145f);
    case iris::FRACTAL_CELTIC:
      return Vec(1.62f, 0.88f);
    case iris::FRACTAL_SPIDER:
      return Vec(1.56f, 0.84f);
    case iris::FRACTAL_NOVA:
      return Vec(2.0f, 0.86f);
    case iris::FRACTAL_NEWTON:
      return Vec(2.45f, 0.98f);
    case iris::FRACTAL_TRICORN:
    default:
      return Vec(1.68f, 0.90f);
  }
}

bool nautiloidRequestDue(double* lastRequestTime, double minIntervalSec) {
  const double now = system::getTime();
  if (!std::isfinite(*lastRequestTime) || now - *lastRequestTime >= minIntervalSec) {
    *lastRequestTime = now;
    return true;
  }
  return false;
}

bool nautiloidVisibleViewOutgrowsCache(Nautiloid* module, float threshold) {
  if (!module) return false;
  Nautiloid::DisplayTileCacheSnapshot snapshot;
  module->displayTileCacheSnapshot(&snapshot);
  if (snapshot.cacheMode != module->fractalMode || snapshot.cacheZoom < 0.f || snapshot.cacheScale <= 1.f) {
    return true;
  }
  const float viewZoomScale = std::pow(0.05f, clamp(module->fractalZoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec viewHalfSpan = nautiloidFractalViewportHalfSpan(module->fractalMode).mult(viewZoomScale);
  const float cacheZoomScale = std::pow(0.05f, clamp(snapshot.cacheZoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec cacheHalfSpan = nautiloidFractalViewportHalfSpan(snapshot.cacheMode).mult(cacheZoomScale).mult(snapshot.cacheScale);
  if (cacheHalfSpan.x <= 0.f || cacheHalfSpan.y <= 0.f) return true;
  const float dx = float(std::fabs(module->fractalCenterX - snapshot.cacheCenterX));
  const float dy = float(std::fabs(module->fractalCenterY - snapshot.cacheCenterY));
  const float useX = (dx + viewHalfSpan.x) / cacheHalfSpan.x;
  const float useY = (dy + viewHalfSpan.y) / cacheHalfSpan.y;
  return std::max(useX, useY) >= threshold;
}

bool nautiloidGpuPreviewEnabled(Nautiloid* module) {
  if (!module ||
      !iris::isBuiltinFractalMode(module->fractalMode) ||
      !module->debugGpuPreviewEnabled.load(std::memory_order_relaxed)) {
    return false;
  }
  return true;
}

bool nautiloidGpuPreviewActive(Nautiloid* module) {
  return nautiloidGpuPreviewEnabled(module) &&
    module->debugGpuPreviewAvailable.load(std::memory_order_relaxed);
}

double nautiloidClampDouble(double value, double minValue, double maxValue) {
  return std::max(minValue, std::min(value, maxValue));
}

int nautiloidColorMode(const Nautiloid* module) {
  if (!module) return Nautiloid::COLOR_PRISM;
  return nautiloid_color::normalize(module->fractalColorMode);
}

const char* nautiloidColorModeName(int mode) {
  return nautiloid_color::name(mode);
}

void nautiloidApplyDisplayPalette(int mode, uint8_t r, uint8_t g, uint8_t b,
                                  uint8_t* outR, uint8_t* outG, uint8_t* outB) {
  nautiloid_color::applyPixel(mode, r, g, b, outR, outG, outB);
}

struct NautiloidGlPreview final : widget::OpenGlWidget {
  struct ModeProgram {
    GLuint program = 0;
    GLuint fragmentShader = 0;
    GLint uniformCenter = -1;
    GLint uniformHalfSpan = -1;
    GLint uniformColorMode = -1;
    bool initAttempted = false;
    bool ready = false;
  };

  Nautiloid* module = nullptr;
  GLuint vertexShader = 0;
  bool vertexShaderInitAttempted = false;
  std::array<ModeProgram, size_t(iris::kLastBuiltinFractalMode) + 1u> modePrograms {};
  bool lastEffectiveActive = false;
  int lastMode = -1;
  float lastZoom = NAN;
  double lastCenterX = NAN;
  double lastCenterY = NAN;
  int lastColorMode = -1;

  explicit NautiloidGlPreview(Nautiloid* module) : module(module) {}

  ~NautiloidGlPreview() override {
    if (module) {
      module->debugGpuPreviewAvailable.store(false, std::memory_order_relaxed);
    }
    releaseGlResources(false);
  }

  void onContextDestroy(const ContextDestroyEvent& e) override {
    OpenGlWidget::onContextDestroy(e);
    releaseGlResources(true);
    if (module) {
      module->debugGpuPreviewAvailable.store(false, std::memory_order_relaxed);
    }
  }

  void releaseGlResources(bool deleteGlObjects) {
    for (ModeProgram& modeProgram : modePrograms) {
      if (deleteGlObjects && modeProgram.program) {
        glDeleteProgram(modeProgram.program);
      }
      if (deleteGlObjects && modeProgram.fragmentShader) {
        glDeleteShader(modeProgram.fragmentShader);
      }
      modeProgram = ModeProgram();
    }
    if (deleteGlObjects && vertexShader) {
      glDeleteShader(vertexShader);
    }
    vertexShader = 0;
    vertexShaderInitAttempted = false;
  }

  static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    if (!shader) return 0;
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) {
      GLint logLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
      std::vector<char> logBuf(size_t(std::max(logLen, 1)));
      GLsizei written = 0;
      glGetShaderInfoLog(shader, GLsizei(logBuf.size()), &written, logBuf.data());
      WARN("Nautiloid GPU preview shader compile failed: %s", logBuf.data());
      glDeleteShader(shader);
      return 0;
    }
    return shader;
  }

  bool ensureShaderReady(int mode) {
    if (!iris::isBuiltinFractalMode(mode) || mode < 0 || size_t(mode) >= modePrograms.size()) {
      return false;
    }
    ModeProgram& modeProgram = modePrograms[size_t(mode)];
    if (modeProgram.initAttempted) return modeProgram.ready;
    modeProgram.initAttempted = true;

    static const char* const kVertexShaderSrc = R"GLSL(
      #version 120
      varying vec2 vUv;
      void main() {
        gl_Position = ftransform();
        vUv = gl_MultiTexCoord0.xy;
      }
    )GLSL";

    static const char* const kFragmentShaderBody = R"GLSL(
      varying vec2 vUv;
      uniform vec2 uCenter;
      uniform vec2 uHalfSpan;
      uniform int uColorMode;

      vec3 applyColorMode(vec3 rgb) {
        if (uColorMode == 0) {
          return rgb;
        }
        float value = max(rgb.r, max(rgb.g, rgb.b));
        vec3 low;
        vec3 middle;
        vec3 high;
        if (uColorMode == 1) {
          low = vec3(2.0, 7.0, 18.0) / 255.0;
          middle = vec3(5.0, 100.0, 172.0) / 255.0;
          high = vec3(144.0, 247.0, 255.0) / 255.0;
        } else if (uColorMode == 2) {
          low = vec3(18.0, 3.0, 2.0) / 255.0;
          middle = vec3(202.0, 42.0, 8.0) / 255.0;
          high = vec3(255.0, 231.0, 104.0) / 255.0;
        } else {
          low = vec3(11.0, 3.0, 22.0) / 255.0;
          middle = vec3(126.0, 25.0, 194.0) / 255.0;
          high = vec3(255.0, 171.0, 246.0) / 255.0;
        }
        const float split = 0.52;
        return value < split
          ? mix(low, middle, value / split)
          : mix(middle, high, (value - split) / (1.0 - split));
      }

      vec3 hsvToRgb(float h, float s, float v) {
        h = fract(h);
        s = clamp(s, 0.0, 1.0);
        v = clamp(v, 0.0, 1.0);
        float c = v * s;
        float hp = h * 6.0;
        float x = c * (1.0 - abs(mod(hp, 2.0) - 1.0));
        vec3 rgb = vec3(0.0);
        if (hp < 1.0) {
          rgb = vec3(c, x, 0.0);
        } else if (hp < 2.0) {
          rgb = vec3(x, c, 0.0);
        } else if (hp < 3.0) {
          rgb = vec3(0.0, c, x);
        } else if (hp < 4.0) {
          rgb = vec3(0.0, x, c);
        } else if (hp < 5.0) {
          rgb = vec3(x, 0.0, c);
        } else {
          rgb = vec3(c, 0.0, x);
        }
        return rgb + vec3(v - c);
      }

      bool mandelbrotMainInterior(vec2 c) {
        float q = (c.x - 0.25) * (c.x - 0.25) + c.y * c.y;
        if (q * (q + (c.x - 0.25)) < 0.25 * c.y * c.y) {
          return true;
        }
        return (c.x + 1.0) * (c.x + 1.0) + c.y * c.y < 0.0625;
      }

      vec3 escapeColor(int iter, int maxIter, float mag2, float minOrbit) {
        float smooth = float(iter) + 1.0 - log(log(sqrt(max(mag2, 1.000001)))) / log(2.0);
        float t = clamp(smooth / float(maxIter), 0.0, 1.0);
        float orbit = 1.0 - clamp(sqrt(min(minOrbit, 4.0)) * 0.5, 0.0, 1.0);
        return hsvToRgb(0.64 + 1.35 * t + 0.08 * orbit,
                        0.72 + 0.20 * orbit,
                        0.18 + 0.82 * sqrt(t));
      }

      vec3 rootColor(vec2 z, int iter, int maxIter, float phase) {
        float d0 = (z.x - 1.0) * (z.x - 1.0) + z.y * z.y;
        float d1 = (z.x + 0.5) * (z.x + 0.5) + (z.y - 0.8660254) * (z.y - 0.8660254);
        float d2 = (z.x + 0.5) * (z.x + 0.5) + (z.y + 0.8660254) * (z.y + 0.8660254);
        int root = d0 < d1 && d0 < d2 ? 0 : (d1 < d2 ? 1 : 2);
        float t = 1.0 - float(iter) / float(maxIter);
        float v = 0.20 + 0.80 * sqrt(clamp(t, 0.0, 1.0));
        return vec3((root == 0 ? 0.95 : 0.20 + phase) * v,
                    (root == 1 ? 0.90 : 0.30 + phase) * v,
                    (root == 2 ? 1.00 : 0.46 + phase) * v);
      }

      void main() {
        vec2 p = uCenter + (vUv * 2.0 - 1.0) * uHalfSpan;
        if (NAUTILOID_MODE == 12 || NAUTILOID_MODE == 13) {
          vec2 z = p;
          const int maxRootIter = NAUTILOID_ROOT_MAX_ITER;
          int iter = 0;
          for (int i = 0; i < maxRootIter; ++i) {
            float zr2 = z.x * z.x;
            float zi2 = z.y * z.y;
            float denom = 3.0 * ((zr2 - zi2) * (zr2 - zi2) + 4.0 * zr2 * zi2);
            if (denom < 1.0e-14) {
              iter = i;
              break;
            }
            vec2 nextZ = vec2((2.0 * z.x * (zr2 + zi2) + (zr2 - zi2)) / denom,
                              (2.0 * z.y * (zr2 + zi2) - 2.0 * z.x * z.y) / denom);
            if (NAUTILOID_MODE == 13) {
              nextZ += vec2(-0.52, 0.38);
            }
            vec2 delta = nextZ - z;
            z = nextZ;
            iter = i;
            if (dot(delta, delta) < 1.0e-12 || dot(z, z) > 64.0) {
              break;
            }
          }
          gl_FragColor = vec4(applyColorMode(rootColor(
            z, iter, maxRootIter, NAUTILOID_MODE == 13 ? 0.18 : 0.0)), 1.0);
          return;
        }

        vec2 c = vec2(0.0);
        vec2 z = vec2(0.0);
        vec2 prev = vec2(0.0);
        if (NAUTILOID_MODE == 1) {
          c = vec2(-0.75, 0.0) + p;
          if (mandelbrotMainInterior(c)) {
            gl_FragColor = vec4(applyColorMode(vec3(7.0, 4.0, 18.0) / 255.0), 1.0);
            return;
          }
        } else if (NAUTILOID_MODE == 2) {
          c = p;
        } else if (NAUTILOID_MODE == 3) {
          // Keep this in sync with kNautiloidMultijuliaReal/Imag.
          c = vec2(-0.18, 0.76);
          z = p;
        } else if (NAUTILOID_MODE == 4) {
          c = vec2(-0.74543, 0.11301);
          z = p;
        } else if (NAUTILOID_MODE == 5) {
          c = vec2(-0.42, 0.08);
          z = p;
        } else if (NAUTILOID_MODE == 6) {
          c = vec2(-0.50, 0.0) + p;
        } else if (NAUTILOID_MODE == 7) {
          c = vec2(-1.76, -0.045) + p;
        } else if (NAUTILOID_MODE == 8) {
          c = vec2(-0.25, 0.02) + p;
        } else if (NAUTILOID_MODE == 11) {
          c = vec2(-0.52, 0.0) + p;
        } else {
          c = vec2(-0.12, 0.0) + p;
        }

        float minOrbit = 1.0e9;
        float mag2 = 0.0;
        int iter = 0;
        const int maxIter = NAUTILOID_ESCAPE_MAX_ITER;
        for (int i = 0; i < maxIter; ++i) {
          if (NAUTILOID_MODE == 7) {
            z = abs(z);
          }
          float zr2 = z.x * z.x;
          float zi2 = z.y * z.y;
          minOrbit = min(minOrbit, zr2 + zi2);
          if (NAUTILOID_MODE == 2 || NAUTILOID_MODE == 3) {
            z = vec2(z.x * (zr2 - 3.0 * zi2) + c.x,
                     z.y * (3.0 * zr2 - zi2) + c.y);
          } else if (NAUTILOID_MODE == 5 || NAUTILOID_MODE == 6) {
            float feedback = NAUTILOID_MODE == 5 ? 0.48 : 1.0;
            vec2 nextZ = vec2(zr2 - zi2 + c.x + feedback * prev.x,
                              2.0 * z.x * z.y + c.y + feedback * prev.y);
            prev = z;
            z = nextZ;
          } else if (NAUTILOID_MODE == 10) {
            z = vec2(zr2 - zi2 + c.x, -2.0 * z.x * z.y + c.y);
          } else if (NAUTILOID_MODE == 8) {
            z = vec2(abs(zr2 - zi2) + c.x, 2.0 * z.x * z.y + c.y);
          } else if (NAUTILOID_MODE == 11) {
            vec2 nextZ = vec2(zr2 - zi2 + c.x, 2.0 * z.x * z.y + c.y);
            c = 0.5 * c + nextZ;
            z = nextZ;
          } else {
            z = vec2(zr2 - zi2 + c.x, 2.0 * z.x * z.y + c.y);
          }
          mag2 = dot(z, z);
          iter = i;
          if (mag2 > 16.0) {
            break;
          }
        }
        if (mag2 <= 16.0) {
          gl_FragColor = vec4(applyColorMode(vec3(7.0, 4.0, 18.0) / 255.0), 1.0);
          return;
        }
        gl_FragColor = vec4(applyColorMode(escapeColor(iter, maxIter, mag2, minOrbit)), 1.0);
      }
    )GLSL";

    if (!vertexShader) {
      if (vertexShaderInitAttempted) return false;
      vertexShaderInitAttempted = true;
      vertexShader = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
    }
    if (!vertexShader) {
      return false;
    }

    const std::string fragmentSource =
      std::string("#version 120\n") +
      "#define NAUTILOID_ESCAPE_MAX_ITER " NAUTILOID_GLSL_STRINGIFY(LEVIATHAN_NAUTILOID_ESCAPE_FRACTAL_MAX_ITER) "\n" +
      "#define NAUTILOID_ROOT_MAX_ITER " NAUTILOID_GLSL_STRINGIFY(LEVIATHAN_NAUTILOID_ROOT_FRACTAL_MAX_ITER) "\n" +
      "#define NAUTILOID_MODE " + std::to_string(mode) + "\n" +
      kFragmentShaderBody;
    modeProgram.fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());
    if (!modeProgram.fragmentShader) {
      return false;
    }
    modeProgram.program = glCreateProgram();
    if (!modeProgram.program) {
      glDeleteShader(modeProgram.fragmentShader);
      modeProgram.fragmentShader = 0;
      return false;
    }
    glAttachShader(modeProgram.program, vertexShader);
    glAttachShader(modeProgram.program, modeProgram.fragmentShader);
    glLinkProgram(modeProgram.program);
    GLint linkOk = GL_FALSE;
    glGetProgramiv(modeProgram.program, GL_LINK_STATUS, &linkOk);
    if (linkOk != GL_TRUE) {
      GLint logLen = 0;
      glGetProgramiv(modeProgram.program, GL_INFO_LOG_LENGTH, &logLen);
      std::vector<char> logBuf(size_t(std::max(logLen, 1)));
      GLsizei written = 0;
      glGetProgramInfoLog(modeProgram.program, GLsizei(logBuf.size()), &written, logBuf.data());
      WARN("Nautiloid GPU preview shader link failed for mode %d: %s", mode, logBuf.data());
      glDeleteProgram(modeProgram.program);
      glDeleteShader(modeProgram.fragmentShader);
      modeProgram.program = 0;
      modeProgram.fragmentShader = 0;
      return false;
    }
    modeProgram.uniformCenter = glGetUniformLocation(modeProgram.program, "uCenter");
    modeProgram.uniformHalfSpan = glGetUniformLocation(modeProgram.program, "uHalfSpan");
    modeProgram.uniformColorMode = glGetUniformLocation(modeProgram.program, "uColorMode");
    modeProgram.ready = modeProgram.uniformCenter >= 0 && modeProgram.uniformHalfSpan >= 0 &&
      modeProgram.uniformColorMode >= 0;
    if (!modeProgram.ready) {
      glDeleteProgram(modeProgram.program);
      glDeleteShader(modeProgram.fragmentShader);
      modeProgram.program = 0;
      modeProgram.fragmentShader = 0;
      return false;
    }
    return true;
  }

  void step() override {
    FramebufferWidget::step();
    const bool effectiveActive = nautiloidGpuPreviewEnabled(module);
    bool currentModeReady = false;
    if (module && iris::isBuiltinFractalMode(module->fractalMode) &&
        module->fractalMode >= 0 && size_t(module->fractalMode) < modePrograms.size()) {
      currentModeReady = modePrograms[size_t(module->fractalMode)].ready;
    }
    if (module) {
      module->debugGpuPreviewAvailable.store(effectiveActive && currentModeReady, std::memory_order_relaxed);
    }
    bool dirty = false;
    if (effectiveActive != lastEffectiveActive) {
      lastEffectiveActive = effectiveActive;
      dirty = true;
    }
    if (module) {
      if (module->fractalMode != lastMode ||
          module->fractalZoom != lastZoom ||
          module->fractalCenterX != lastCenterX ||
          module->fractalCenterY != lastCenterY ||
          nautiloidColorMode(module) != lastColorMode) {
        lastMode = module->fractalMode;
        lastZoom = module->fractalZoom;
        lastCenterX = module->fractalCenterX;
        lastCenterY = module->fractalCenterY;
        lastColorMode = nautiloidColorMode(module);
        dirty = true;
      }
    }
    if (dirty) {
      setDirty();
    }
  }

  void drawFramebuffer() override {
    Vec fbSize = getFramebufferSize();
    glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    const int mode = module ? int(module->fractalMode) : iris::FRACTAL_NONE;
    if (!nautiloidGpuPreviewEnabled(module) || !ensureShaderReady(mode)) {
      if (module) {
        module->debugGpuPreviewAvailable.store(false, std::memory_order_relaxed);
      }
      return;
    }
    if (module) {
      module->debugGpuPreviewAvailable.store(true, std::memory_order_relaxed);
    }

    ModeProgram& modeProgram = modePrograms[size_t(mode)];
    const float zoomScale = std::pow(0.05f, clamp(module->fractalZoom, 0.f, kNautiloidMaxFractalZoom));
    const Vec halfSpan = nautiloidFractalViewportHalfSpan(module->fractalMode).mult(zoomScale);
    const float w = std::max(box.size.x, 1.f);
    const float h = std::max(box.size.y, 1.f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, double(w), double(h), 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);

    glUseProgram(modeProgram.program);
    glUniform2f(modeProgram.uniformCenter, float(module->fractalCenterX), float(module->fractalCenterY));
    glUniform2f(modeProgram.uniformHalfSpan, halfSpan.x, halfSpan.y);
    glUniform1i(modeProgram.uniformColorMode, nautiloidColorMode(module));
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(w, 0.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, h);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(w, h);
    glEnd();
    glUseProgram(0);
  }
};

struct NautiloidDisplay final : OpaqueWidget {
  Nautiloid* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  NVGcontext* imageContext = nullptr;
  int imageHandle = -1;
  int uploadedWidth = 0;
  int uploadedHeight = 0;
  std::vector<uint8_t> rgba;
  bool panActive = false;
  bool lastGpuPreviewActive = false;
  int uploadedColorMode = -1;
  Vec lastPanLocal;
  double lastPanRequestTime = -INFINITY;

  explicit NautiloidDisplay(Nautiloid* module) : module(module) {}

  bool applyWheelZoom(const event::HoverScroll& e) {
    if (!module || box.size.x <= 1.f || box.size.y <= 1.f) {
      return false;
    }

    const float wheel = e.scrollDelta.y;
    if (std::fabs(wheel) < 1e-4f) {
      return false;
    }

    Nautiloid::FractalState state = module->fractalStateSnapshot();
    const float previousZoom = clamp(state.zoom, 0.f, kNautiloidMaxFractalZoom);
    const float nextZoom = clamp(previousZoom + clamp(wheel, -4.f, 4.f) * kNautiloidWheelZoomStep,
      0.f, kNautiloidMaxFractalZoom);
    if (std::fabs(nextZoom - previousZoom) < 1e-5f) {
      return true;
    }

    // Preserve the fractal coordinate under the cursor while changing scale.
    const Vec cursorNorm(
      (e.pos.x / box.size.x - 0.5f) * 2.f,
      (e.pos.y / box.size.y - 0.5f) * 2.f);
    const Vec previousHalfSpan = nautiloidFractalViewportHalfSpan(state.mode).mult(
      std::pow(0.05f, previousZoom));
    const Vec nextHalfSpan = nautiloidFractalViewportHalfSpan(state.mode).mult(
      std::pow(0.05f, nextZoom));
    const double focusX = state.centerX + double(cursorNorm.x * previousHalfSpan.x);
    const double focusY = state.centerY + double(cursorNorm.y * previousHalfSpan.y);

    state.zoom = nextZoom;
    state.centerX = nautiloidClampDouble(focusX - double(cursorNorm.x * nextHalfSpan.x), -2.0, 2.0);
    state.centerY = nautiloidClampDouble(focusY - double(cursorNorm.y * nextHalfSpan.y), -2.0, 2.0);
    module->setFractalState(state);

    const bool recenterCache = nextZoom < previousZoom && nautiloidVisibleViewOutgrowsCache(module, 0.78f);
    module->requestInteractiveZoomPreview(module->fractalCenterX, module->fractalCenterY, recenterCache);
    module->requestRenderWithCacheCenter(module->fractalCenterX, module->fractalCenterY, recenterCache);
    return true;
  }

  ~NautiloidDisplay() override {
    if (APP && APP->window && APP->window->vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, APP->window->vg, true);
      return;
    }
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
  }

  Vec currentLocalMousePos() const {
    if (!parent || !APP || !APP->scene || !APP->scene->rack) {
      return Vec();
    }
    return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
  }

  void onButton(const event::Button& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
      panActive = true;
      lastPanLocal = currentLocalMousePos();
      e.consume(this);
      return;
    }
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE) {
      panActive = false;
    }
    OpaqueWidget::onButton(e);
  }

  void onHoverScroll(const event::HoverScroll& e) override {
    const int mods = (APP && APP->window) ? APP->window->getMods() : 0;
    if ((mods & RACK_MOD_CTRL) != 0) {
      Widget::onHoverScroll(e);
      return;
    }
    if (applyWheelZoom(e)) {
      e.consume(this);
      return;
    }
    Widget::onHoverScroll(e);
  }

  void onDragStart(const event::DragStart& e) override {
    if (module && e.button == GLFW_MOUSE_BUTTON_LEFT) {
      panActive = true;
      lastPanLocal = currentLocalMousePos();
      e.consume(this);
      return;
    }
    OpaqueWidget::onDragStart(e);
  }

  void onDragMove(const event::DragMove& e) override {
    if (module && panActive && e.button == GLFW_MOUSE_BUTTON_LEFT) {
      const Vec current = currentLocalMousePos();
      const Vec delta = current.minus(lastPanLocal);
      lastPanLocal = current;
      if (box.size.x > 1.f && box.size.y > 1.f && (std::fabs(delta.x) > 0.f || std::fabs(delta.y) > 0.f)) {
        Nautiloid::FractalState state = module->fractalStateSnapshot();
        const float zoomScale = std::pow(0.05f, clamp(state.zoom, 0.f, kNautiloidMaxFractalZoom));
        const Vec halfSpan = nautiloidFractalViewportHalfSpan(state.mode).mult(zoomScale);
        const Vec centerDelta(
          -delta.x / box.size.x * 2.f * halfSpan.x,
          -delta.y / box.size.y * 2.f * halfSpan.y);
        state.centerX = nautiloidClampDouble(state.centerX + double(centerDelta.x), -2.0, 2.0);
        state.centerY = nautiloidClampDouble(state.centerY + double(centerDelta.y), -2.0, 2.0);
        module->setFractalState(state);
        if (nautiloidRequestDue(&lastPanRequestTime, 0.05)) {
          const float cacheLead = 3.f;
          const float maxLeadX = 0.35f * halfSpan.x;
          const float maxLeadY = 0.35f * halfSpan.y;
          const double cacheCenterX =
            nautiloidClampDouble(
              state.centerX + double(clamp(centerDelta.x * cacheLead, -maxLeadX, maxLeadX)), -2.0, 2.0);
          const double cacheCenterY =
            nautiloidClampDouble(
              state.centerY + double(clamp(centerDelta.y * cacheLead, -maxLeadY, maxLeadY)), -2.0, 2.0);
          module->requestRenderWithCacheCenter(cacheCenterX, cacheCenterY);
        }
      }
      e.consume(this);
      return;
    }
    OpaqueWidget::onDragMove(e);
  }

  void onDragEnd(const event::DragEnd& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && panActive) {
      panActive = false;
      if (module) {
        module->requestRenderWithCenteredCache();
      }
      e.consume(this);
      return;
    }
    OpaqueWidget::onDragEnd(e);
  }

  void step() override {
    const uint64_t currentGeneration =
      module ? module->previewGeneration.load(std::memory_order_acquire) : 0u;
    const bool gpuPreviewActive = nautiloidGpuPreviewActive(module);
    const int colorMode = nautiloidColorMode(module);
    if ((generation != currentGeneration || gpuPreviewActive != lastGpuPreviewActive ||
         colorMode != uploadedColorMode) && framebuffer) {
      framebuffer->setDirty();
    }
    lastGpuPreviewActive = gpuPreviewActive;
    OpaqueWidget::step();
  }

  void draw(const DrawArgs& args) override {
    if (nautiloidGpuPreviewActive(module)) {
      return;
    }

    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGB(4, 7, 10));
    nvgFill(args.vg);

    const uint64_t currentGeneration =
      module ? module->previewGeneration.load(std::memory_order_acquire) : 0u;
    const int colorMode = nautiloidColorMode(module);
    if (imageContext != args.vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
      imageContext = args.vg;
      generation = uint64_t(-1);
    }
    if (generation != currentGeneration || uploadedColorMode != colorMode || imageHandle < 0 ||
        !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, imageHandle, uploadedWidth, uploadedHeight)) {
      std::vector<uint8_t> rgb;
      int width = 0;
      int height = 0;
      if (module) {
        module->previewSnapshot(&rgb, &width, &height);
      }
      rgba.resize(rgb.size() / 3u * 4u);
      for (size_t i = 0; i + 2u < rgb.size(); i += 3u) {
        const size_t out = (i / 3u) * 4u;
        nautiloidApplyDisplayPalette(colorMode, rgb[i + 0u], rgb[i + 1u], rgb[i + 2u],
          &rgba[out + 0u], &rgba[out + 1u], &rgba[out + 2u]);
        rgba[out + 3u] = 255u;
      }
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, imageContext == args.vg);
      imageContext = args.vg;
      if (width > 0 && height > 0 && !rgba.empty()) {
        imageHandle = nvgCreateImageRGBA(args.vg, width, height, NVG_IMAGE_PREMULTIPLIED, rgba.data());
        uploadedWidth = width;
        uploadedHeight = height;
      }
      generation = currentGeneration;
      uploadedColorMode = colorMode;
    }

    if (imageHandle >= 0) {
      NVGpaint paint =
        nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillPaint(args.vg, paint);
      nvgFill(args.vg);
    }
    if (module && module->loading.load(std::memory_order_acquire)) {
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 34));
      nvgFill(args.vg);
    }
  }
};

struct NautiloidZoomSpeedQuantity final : Quantity {
  float position = 0.5f;

  void setValue(float value) override {
    position = clamp(value, 0.f, 1.f);
  }

  float getValue() override {
    return position;
  }

  float getDefaultValue() override {
    return 0.5f;
  }

  float getDisplayValue() override {
    return (getValue() - 0.5f) * 200.f;
  }

  void setDisplayValue(float displayValue) override {
    setValue(displayValue / 200.f + 0.5f);
  }

  std::string getLabel() override {
    return "Zoom";
  }

  std::string getUnit() override {
    return "%";
  }

  std::string getDisplayValueString() override {
    return string::f("%+.0f", getDisplayValue());
  }
};

struct NautiloidIrisMiniDisplay final : OpaqueWidget {
  Nautiloid* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  NVGcontext* imageContext = nullptr;
  int imageHandle = -1;
  int uploadedWidth = 0;
  int uploadedHeight = 0;
  std::vector<uint8_t> rgba;

  explicit NautiloidIrisMiniDisplay(Nautiloid* module) : module(module) {}

  ~NautiloidIrisMiniDisplay() override {
    if (APP && APP->window && APP->window->vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, APP->window->vg, true);
      return;
    }
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
  }

  void step() override {
    const uint64_t currentGeneration =
      module ? module->irisPreviewGeneration.load(std::memory_order_acquire) : 0u;
    if (generation != currentGeneration && framebuffer) {
      framebuffer->setDirty();
    }
    OpaqueWidget::step();
  }

  void draw(const DrawArgs& args) override {
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGB(3, 5, 7));
    nvgFill(args.vg);

    const uint64_t currentGeneration =
      module ? module->irisPreviewGeneration.load(std::memory_order_acquire) : 0u;
    if (imageContext != args.vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
      imageContext = args.vg;
      generation = uint64_t(-1);
    }
    if (generation != currentGeneration || imageHandle < 0 ||
        !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, imageHandle, uploadedWidth, uploadedHeight)) {
      std::vector<uint8_t> rgb;
      int width = 0;
      int height = 0;
      if (module) {
        module->irisPreviewSnapshot(&rgb, &width, &height);
      }
      rgba.resize(rgb.size() / 3u * 4u);
      for (size_t i = 0; i + 2u < rgb.size(); i += 3u) {
        const size_t out = (i / 3u) * 4u;
        rgba[out + 0u] = rgb[i + 0u];
        rgba[out + 1u] = rgb[i + 1u];
        rgba[out + 2u] = rgb[i + 2u];
        rgba[out + 3u] = 255u;
      }
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, imageContext == args.vg);
      imageContext = args.vg;
      if (width > 0 && height > 0 && !rgba.empty()) {
        imageHandle = nvgCreateImageRGBA(args.vg, width, height, NVG_IMAGE_PREMULTIPLIED, rgba.data());
        uploadedWidth = width;
        uploadedHeight = height;
      }
      generation = currentGeneration;
    }

    if (imageHandle >= 0) {
      NVGpaint paint =
        nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillPaint(args.vg, paint);
      nvgFill(args.vg);
    }
  }
};

struct NautiloidTileCacheGrid final : TransparentWidget {
  Nautiloid* module = nullptr;

  explicit NautiloidTileCacheGrid(Nautiloid* module) : module(module) {}

  void draw(const DrawArgs& args) override {
    if (nautiloidGpuPreviewActive(module)) {
      return;
    }

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 3.f);
    nvgFillColor(args.vg, nvgRGB(4, 7, 10));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(88, 65, 191, 150));
    nvgStroke(args.vg);

    if (!module) return;

    Nautiloid::DisplayTileCacheSnapshot snapshot;
    module->displayTileCacheSnapshot(&snapshot);
    if (snapshot.columns <= 0 || snapshot.rows <= 0 ||
        snapshot.tileCurrent.size() != size_t(snapshot.columns) * size_t(snapshot.rows)) {
      return;
    }

    constexpr float pad = 3.2f;
    const float gap = 1.05f;
    const float cellW = (box.size.x - 2.f * pad - gap * float(snapshot.columns - 1)) / float(snapshot.columns);
    const float cellH = (box.size.y - 2.f * pad - gap * float(snapshot.rows - 1)) / float(snapshot.rows);
    const float cell = std::max(1.f, std::min(cellW, cellH));
    const float gridW = float(snapshot.columns) * cell + float(snapshot.columns - 1) * gap;
    const float gridH = float(snapshot.rows) * cell + float(snapshot.rows - 1) * gap;
    const float x0 = 0.5f * (box.size.x - gridW);
    const float y0 = 0.5f * (box.size.y - gridH);

    const bool compatibleCache = snapshot.cacheMode == module->fractalMode && snapshot.cacheZoom >= 0.f;
    const NVGcolor staleColor = snapshot.current ? nvgRGBA(33, 40, 56, 205) : nvgRGBA(45, 30, 70, 145);
    const NVGcolor currentColor = nvgRGB(28, 204, 217);
    const NVGcolor edgeColor = nvgRGBA(226, 232, 240, 58);
    for (int row = 0; row < snapshot.rows; ++row) {
      for (int column = 0; column < snapshot.columns; ++column) {
        const size_t index = size_t(row) * size_t(snapshot.columns) + size_t(column);
        const bool current = snapshot.tileCurrent[index] != 0u;
        const float x = x0 + float(column) * (cell + gap);
        const float y = y0 + float(row) * (cell + gap);
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, x, y, cell, cell, std::min(1.8f, cell * 0.25f));
        nvgFillColor(args.vg, current ? currentColor : staleColor);
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 0.65f);
        nvgStrokeColor(args.vg, current ? nvgRGBA(220, 255, 255, 90) : edgeColor);
        nvgStroke(args.vg);
      }
    }

    if (compatibleCache) {
      const float viewZoomScale = std::pow(0.05f, clamp(module->fractalZoom, 0.f, kNautiloidMaxFractalZoom));
      const Vec viewHalfSpan = nautiloidFractalViewportHalfSpan(module->fractalMode).mult(viewZoomScale);
      const float cacheZoomScale = std::pow(0.05f, clamp(snapshot.cacheZoom, 0.f, kNautiloidMaxFractalZoom));
      const Vec cacheBaseHalfSpan = nautiloidFractalViewportHalfSpan(snapshot.cacheMode).mult(cacheZoomScale);
      const float cacheScale = std::max(1.f, snapshot.cacheScale);
      const float cacheHalfX = cacheBaseHalfSpan.x * cacheScale;
      const float cacheHalfY = cacheBaseHalfSpan.y * cacheScale;
      if (cacheHalfX > 0.f && cacheHalfY > 0.f) {
        const float dx = float(module->fractalCenterX - snapshot.cacheCenterX);
        const float dy = float(module->fractalCenterY - snapshot.cacheCenterY);
        const float centerX = x0 + (0.5f + dx / (2.f * cacheHalfX)) * gridW;
        const float centerY = y0 + (0.5f + dy / (2.f * cacheHalfY)) * gridH;
        const float viewW = gridW * viewHalfSpan.x / cacheHalfX;
        const float viewH = gridH * viewHalfSpan.y / cacheHalfY;
        const bool outgrown =
          centerX - 0.5f * viewW < x0 ||
          centerY - 0.5f * viewH < y0 ||
          centerX + 0.5f * viewW > x0 + gridW ||
          centerY + 0.5f * viewH > y0 + gridH;
        nvgBeginPath(args.vg);
        nvgRect(args.vg, centerX - 0.5f * viewW, centerY - 0.5f * viewH, viewW, viewH);
        nvgStrokeWidth(args.vg, outgrown ? 1.55f : 1.2f);
        nvgStrokeColor(args.vg, outgrown ? nvgRGBA(255, 177, 66, 230) : nvgRGBA(236, 240, 255, 190));
        nvgStroke(args.vg);
      }
    }
  }
};

struct NautiloidDebugCounters final : TransparentWidget {
  Nautiloid* module = nullptr;
  double lastLogTime = -INFINITY;
  uint64_t lastLoggedRequests = uint64_t(-1);
  uint64_t lastLoggedIrisGeneration = uint64_t(-1);

  explicit NautiloidDebugCounters(Nautiloid* module) : module(module) {}

  void step() override {
    if (module && module->debugFileLoggingEnabled.load(std::memory_order_relaxed)) {
      const double now = system::getTime();
      const uint64_t requests = module->renderRequestsSubmitted.load(std::memory_order_relaxed);
      const uint64_t irisGeneration = module->irisPreviewGeneration.load(std::memory_order_relaxed);
      if ((!std::isfinite(lastLogTime) || now - lastLogTime >= 0.25) &&
          (requests != lastLoggedRequests || irisGeneration != lastLoggedIrisGeneration)) {
        lastLogTime = now;
        lastLoggedRequests = requests;
        lastLoggedIrisGeneration = irisGeneration;
        appendLog(now);
      }
    }
    TransparentWidget::step();
  }

  void appendLog(double now) {
    const std::string dir = nautiloidUserRootPath();
    system::createDirectories(dir);
    const std::string path = nautiloidDebugLogPath();
    const bool needsHeader = !system::exists(path) || system::getFileSize(path) == 0u;
    std::ofstream log(path, std::ios::app);
    if (!log) return;
    if (needsHeader) {
      log << "time,zoom,center_x,center_y,loading,req,display_gen,iris_gen,display_done,"
             "display_stale,display_reprojections,display_reprojection_zoom_ahead_hits,"
             "cache_hits,cache_partial_hits,cache_misses,cache_submitted,cache_dequeued,"
             "cache_done,cache_composite_publishes,cache_tiles_current,cache_tiles_full,"
             "cache_tiles_rendered,cache_tile_aborts,cache_resets,cache_shifts,"
             "zoom_ahead_tiles_rendered,zoom_ahead_l0_tiles,zoom_ahead_l1_tiles,zoom_ahead_l2_tiles,"
             "zoom_ahead_l0_full,zoom_ahead_l1_full,zoom_ahead_l2_full,"
             "iris_done,iris_stale,iris_expander_publishes\n";
    }
    Nautiloid::DisplayTileCacheSnapshot tileSnapshot;
    module->displayTileCacheSnapshot(&tileSnapshot);
    Nautiloid::ZoomAheadCacheSnapshot zoomAheadSnapshot;
    module->zoomAheadCacheSnapshot(&zoomAheadSnapshot);
    log
      << now << ','
      << module->fractalZoom << ','
      << module->fractalCenterX << ','
      << module->fractalCenterY << ','
      << (module->loading.load(std::memory_order_relaxed) ? 1 : 0) << ','
      << module->renderRequestsSubmitted.load(std::memory_order_relaxed) << ','
      << module->previewGeneration.load(std::memory_order_relaxed) << ','
      << module->irisPreviewGeneration.load(std::memory_order_relaxed) << ','
      << module->displayRendersCompleted.load(std::memory_order_relaxed) << ','
      << module->displayRendersDroppedStale.load(std::memory_order_relaxed) << ','
      << module->displayReprojectionPublishes.load(std::memory_order_relaxed) << ','
      << module->displayReprojectionZoomAheadHits.load(std::memory_order_relaxed) << ','
      << module->displayCacheHits.load(std::memory_order_relaxed) << ','
      << module->displayCachePartialHits.load(std::memory_order_relaxed) << ','
      << module->displayCacheMisses.load(std::memory_order_relaxed) << ','
      << module->cacheRequestsSubmitted.load(std::memory_order_relaxed) << ','
      << module->cacheRequestsDequeued.load(std::memory_order_relaxed) << ','
      << module->displayCacheRendersCompleted.load(std::memory_order_relaxed) << ','
      << module->displayCacheCompositePublishes.load(std::memory_order_relaxed) << ','
      << tileSnapshot.currentTileCount << ','
      << tileSnapshot.fullTileCount << ','
      << module->displayCacheTilesRendered.load(std::memory_order_relaxed) << ','
      << module->displayCacheTileAborts.load(std::memory_order_relaxed) << ','
      << module->displayTileCacheResets.load(std::memory_order_relaxed) << ','
      << module->displayTileCacheShifts.load(std::memory_order_relaxed) << ','
      << module->zoomAheadTilesRendered.load(std::memory_order_relaxed) << ','
      << zoomAheadSnapshot.currentTileCount[0] << ','
      << zoomAheadSnapshot.currentTileCount[1] << ','
      << zoomAheadSnapshot.currentTileCount[2] << ','
      << zoomAheadSnapshot.fullTileCount[0] << ','
      << zoomAheadSnapshot.fullTileCount[1] << ','
      << zoomAheadSnapshot.fullTileCount[2] << ','
      << module->irisRendersCompleted.load(std::memory_order_relaxed) << ','
      << module->irisRendersDroppedStale.load(std::memory_order_relaxed)
      << ','
      << module->irisExpanderPublishes.load(std::memory_order_relaxed)
      << '\n';
  }
};

// Match the indicator embedded in Bifurx's LuminSlider. The slider control
// owns positioning; this widget only supplies the LED and its glass overlay.
struct NautiloidZoomHandleLight final : VCVSliderLight<LeviathanCyanPurpleLight> {
  void step() override {
    widget::TransparentWidget::step();
  }
};

struct NautiloidZoomSlider final : ui::Slider {
  Nautiloid* module = nullptr;
  NautiloidZoomSpeedQuantity* zoomSpeed = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  NautiloidZoomHandleLight* handleLight = nullptr;
  Vec handleLightNaturalSize;
  Vec lightOverlayOffset;
  std::shared_ptr<window::Svg> handleSvg;
  bool zoomActive = false;
  double lastStepTime = -INFINITY;
  double lastPreviewRequestTime = -INFINITY;
  double lastRenderRequestTime = -INFINITY;
  float lastDrawValue = -1.f;
  float lastDrawSpeed = 100.f;
  float lastDrawZoomAmount = -1.f;
  Vec lastDrawSize;

  NautiloidZoomSlider() {
    handleSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/LeviathanSliderHandle.svg"));
    handleLight = new NautiloidZoomHandleLight;
    handleLightNaturalSize = handleLight->box.size;
    if (handleLight->fb) {
      handleLight->fb->dirtyOnSubpixelChange = false;
    }
  }

  void onButton(const event::Button& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      if (e.action == GLFW_PRESS) {
        zoomActive = true;
        if (module) {
          module->zoomInteractionActive.store(true, std::memory_order_relaxed);
        }
        lastStepTime = system::getTime();
      } else if (e.action == GLFW_RELEASE) {
        stopZoom();
      }
    }
    ui::Slider::onButton(e);
  }

  void onDragStart(const event::DragStart& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      zoomActive = true;
      if (module) {
        module->zoomInteractionActive.store(true, std::memory_order_relaxed);
      }
      lastStepTime = system::getTime();
    }
    ui::Slider::onDragStart(e);
  }

  void onDragEnd(const event::DragEnd& e) override {
    ui::Slider::onDragEnd(e);
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      stopZoom();
    }
  }

  void step() override {
    const double now = system::getTime();
    const bool cvConnected =
      module && module->zoomRateCvConnected.load(std::memory_order_relaxed);
    const float cvSpeed =
      cvConnected ? module->zoomRateCvNorm.load(std::memory_order_relaxed) : 0.f;
    bool xVelocityConnected = false;
    bool yVelocityConnected = false;
    float xVelocity = 0.f;
    float yVelocity = 0.f;
    if (module) {
      xVelocityConnected = module->xVelocityCvConnected.load(std::memory_order_relaxed);
      yVelocityConnected = module->yVelocityCvConnected.load(std::memory_order_relaxed);
      if (xVelocityConnected) {
        xVelocity = module->xVelocityCvNorm.load(std::memory_order_relaxed);
      }
      if (yVelocityConnected) {
        yVelocity = module->yVelocityCvNorm.load(std::memory_order_relaxed);
      }
    }
    const float manualSpeed =
      (zoomActive && zoomSpeed) ? (zoomSpeed->getValue() - 0.5f) * 2.f : 0.f;
    const float speed = clamp(manualSpeed + cvSpeed, -1.f, 1.f);
    const bool speedActive = std::fabs(speed) > 0.015f;
    const bool xVelocityActive = std::fabs(xVelocity) > 0.015f;
    const bool yVelocityActive = std::fabs(yVelocity) > 0.015f;
    const bool velocityActive = xVelocityActive || yVelocityActive;
    const bool interactionActive = zoomActive || (cvConnected && speedActive) || velocityActive;
    if (module) {
      module->zoomInteractionActive.store(interactionActive, std::memory_order_relaxed);
    }
    if (module && interactionActive) {
      if (!std::isfinite(lastStepTime)) {
        lastStepTime = now;
      }
      const double dt = std::max(0.0, std::min(now - lastStepTime, 0.05));
      lastStepTime = now;
      if ((speedActive || velocityActive) && dt > 0.0) {
        Nautiloid::FractalState state = module->fractalStateSnapshot();
        const float previousZoom = state.zoom;
        const double previousX = state.centerX;
        const double previousY = state.centerY;
        if (speedActive) {
          const float shapedSpeed = speed * std::fabs(speed);
          state.zoom = clamp(state.zoom + shapedSpeed * float(dt) * 0.85f, 0.f, kNautiloidMaxFractalZoom);
        }
        if (velocityActive) {
          const float zoomScale = std::pow(0.05f, clamp(state.zoom, 0.f, kNautiloidMaxFractalZoom));
          const Vec halfSpan = nautiloidFractalViewportHalfSpan(state.mode).mult(zoomScale);
          state.centerX = nautiloidClampDouble(
            state.centerX + double(2.f * halfSpan.x * xVelocity * std::fabs(xVelocity) * float(dt) * 0.85f), -2.0, 2.0);
          state.centerY = nautiloidClampDouble(
            state.centerY + double(2.f * halfSpan.y * yVelocity * std::fabs(yVelocity) * float(dt) * 0.85f), -2.0, 2.0);
        }
        // Deep zoom makes a screen-relative pan step extremely small in world
        // coordinates. Preserve every representable step so it can accumulate.
        if (previousZoom != state.zoom ||
            previousX != state.centerX ||
            previousY != state.centerY) {
          const bool zoomingOut = state.zoom < previousZoom;
          module->setFractalState(state);
          const bool recenterCache = (zoomingOut || velocityActive) &&
            nautiloidVisibleViewOutgrowsCache(module, 0.78f);
          if (nautiloidRequestDue(&lastPreviewRequestTime, 0.04)) {
            module->requestInteractiveZoomPreview(module->fractalCenterX, module->fractalCenterY, recenterCache);
          }
          if (!module->displayRenderBusy.load(std::memory_order_acquire) &&
              nautiloidRequestDue(&lastRenderRequestTime, 0.12)) {
            module->requestRenderWithCacheCenter(module->fractalCenterX, module->fractalCenterY, recenterCache);
          }
        }
      }
    } else {
      lastStepTime = -INFINITY;
    }
    ui::Slider::step();

    const float drawSpeed = clamp(manualSpeed + cvSpeed, -1.f, 1.f);
    const float drawValue = (zoomActive || cvConnected)
      ? 0.5f + 0.5f * drawSpeed
      : (zoomSpeed ? clamp(zoomSpeed->getValue(), 0.f, 1.f) : 0.5f);
    const float drawZoomAmount =
      module ? clamp(module->fractalZoom / kNautiloidMaxFractalZoom, 0.f, 1.f) : 0.f;
    if (framebuffer &&
        (std::fabs(drawValue - lastDrawValue) > 1e-4f ||
         std::fabs(drawSpeed - lastDrawSpeed) > 1e-4f ||
         std::fabs(drawZoomAmount - lastDrawZoomAmount) > 1e-4f ||
         std::fabs(box.size.x - lastDrawSize.x) > 1e-4f ||
         std::fabs(box.size.y - lastDrawSize.y) > 1e-4f)) {
      lastDrawValue = drawValue;
      lastDrawSpeed = drawSpeed;
      lastDrawZoomAmount = drawZoomAmount;
      lastDrawSize = box.size;
      framebuffer->setDirty();
    }
  }

  void setHandleLightState(float displaySpeed, Vec center) {
    if (!handleLight) return;
    const float positive = std::max(displaySpeed, 0.f);
    const float negative = std::max(-displaySpeed, 0.f);
    handleLight->visible = true;
    const Vec lightSize = (handleLightNaturalSize.x > 0.f && handleLightNaturalSize.y > 0.f)
      ? handleLightNaturalSize
      : handleLight->box.size;
    handleLight->box.size = lightSize;
    handleLight->box.pos = lightOverlayOffset.plus(center).minus(lightSize.div(2.f));
    handleLight->setBrightnesses({
      positive,
      negative
    });
  }

  void draw(const DrawArgs& args) override {
    const bool cvConnected =
      module && module->zoomRateCvConnected.load(std::memory_order_relaxed);
    const float cvSpeed =
      cvConnected ? module->zoomRateCvNorm.load(std::memory_order_relaxed) : 0.f;
    const float manualSpeed =
      (zoomActive && zoomSpeed) ? (zoomSpeed->getValue() - 0.5f) * 2.f : 0.f;
    const float displaySpeed = clamp(manualSpeed + cvSpeed, -1.f, 1.f);
    const float value = (zoomActive || cvConnected)
      ? 0.5f + 0.5f * displaySpeed
      : (zoomSpeed ? clamp(zoomSpeed->getValue(), 0.f, 1.f) : 0.5f);
    const float zoomAmount =
      module ? clamp(module->fractalZoom / kNautiloidMaxFractalZoom, 0.f, 1.f) : 0.f;
    const float insetX = std::max(2.f, box.size.y * 0.14f);
    const float insetY = std::max(1.f, box.size.y * 0.08f);
    const float contentW = std::max(1.f, box.size.x - 2.f * insetX);
    const float centerX = insetX + 0.5f * contentW;
    const float handleX = insetX + value * contentW;
    const float trackH = std::max(4.f, std::min(8.f, box.size.y * 0.32f));
    const float progressH = std::max(4.f, std::min(8.f, box.size.y * 0.32f));
    const float gap = std::max(3.f, box.size.y * 0.16f);
    const float progressY = std::max(
      insetY + trackH + gap,
      box.size.y - insetY - progressH - std::max(1.f, box.size.y * 0.03f));
    const float trackY = std::max(insetY, progressY - gap - trackH);
    const float progressRadius = 0.5f * progressH;

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, insetX, trackY, contentW, trackH, 1.5f);
    nvgFillColor(args.vg, nvgRGB(12, 16, 22));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(174, 132, 255, 96));
    nvgStroke(args.vg);

    const float fillLeft = std::min(centerX, handleX);
    const float fillW = std::fabs(handleX - centerX);
    if (fillW > 0.75f) {
      const bool zoomIn = handleX > centerX;
      nvgSave(args.vg);
      nvgIntersectScissor(args.vg, fillLeft, trackY - 1.f, fillW, trackH + 2.f);
      nvgBeginPath(args.vg);
      nvgRoundedRect(args.vg, insetX, trackY, contentW, trackH, 1.5f);
      const NVGcolor centerColor = nvgRGB(226, 232, 240);
      const NVGcolor edgeColor = zoomIn ? nvgRGB(28, 204, 217) : nvgRGB(122, 92, 255);
      NVGpaint speedPaint = nvgLinearGradient(args.vg, centerX, trackY, handleX, trackY, centerColor, edgeColor);
      nvgFillPaint(args.vg, speedPaint);
      nvgFill(args.vg);
      nvgRestore(args.vg);
    }

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, centerX, trackY - 3.f);
    nvgLineTo(args.vg, centerX, trackY + trackH + 3.f);
    nvgStrokeWidth(args.vg, 1.25f);
    nvgStrokeColor(args.vg, nvgRGBA(216, 194, 255, 132));
    nvgStroke(args.vg);

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, insetX, progressY, contentW, progressH, progressRadius);
    nvgFillColor(args.vg, nvgRGB(8, 11, 16));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(174, 132, 255, 88));
    nvgStroke(args.vg);

    const float progressW = zoomAmount * contentW;
    if (progressW > 0.5f) {
      nvgSave(args.vg);
      nvgIntersectScissor(args.vg, insetX, progressY - 1.f, progressW, progressH + 2.f);
      nvgBeginPath(args.vg);
      nvgRoundedRect(args.vg, insetX, progressY, contentW, progressH, progressRadius);
      NVGpaint progressPaint = nvgLinearGradient(
        args.vg, insetX, progressY, insetX + contentW, progressY, nvgRGB(122, 92, 255), nvgRGB(28, 204, 217));
      nvgFillPaint(args.vg, progressPaint);
      nvgFill(args.vg);
      nvgRestore(args.vg);
    }

    if (handleSvg) {
      const Vec svgSize = handleSvg->getSize();
      if (svgSize.x > 1.f && svgSize.y > 1.f) {
        const float sliderLaneTop = std::max(0.f, trackY - 3.f);
        const float sliderLaneBottom = std::max(sliderLaneTop + trackH, progressY - 1.f);
        const float sliderLaneH = sliderLaneBottom - sliderLaneTop;
        const float handleW = svgSize.x;
        const float handleDrawX =
          clamp(handleX - 0.5f * handleW, 1.f, std::max(1.f, box.size.x - handleW - 1.f));
        const float handleCenterX = handleDrawX + 0.5f * handleW;
        const float handleCenterY = sliderLaneTop + 0.5f * sliderLaneH;
        setHandleLightState(displaySpeed, Vec(handleCenterX, handleCenterY));

        nvgSave(args.vg);
        nvgTranslate(args.vg, handleCenterX, handleCenterY);
        nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
        handleSvg->draw(args.vg);
        nvgRestore(args.vg);
      }
    }
  }

  void stopZoom() {
    zoomActive = false;
    lastStepTime = -INFINITY;
    lastPreviewRequestTime = -INFINITY;
    lastRenderRequestTime = -INFINITY;
    if (module) {
      module->zoomInteractionActive.store(false, std::memory_order_relaxed);
    }
    if (zoomSpeed) {
      zoomSpeed->setValue(0.5f);
    }
    if (module) {
      module->requestRenderWithCenteredCache();
    }
  }
};

struct NautiloidSourceButton final : TL1105 {
  Nautiloid* module = nullptr;

  void onButton(const event::Button& e) override {
    if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
      TL1105::onButton(e);
      return;
    }
    ui::Menu* menu = createMenu();
    menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
    menu->addChild(createMenuLabel("Fractals"));
    const std::array<int, 12> modes = {{
      iris::FRACTAL_MANDELBROT,
      iris::FRACTAL_MULTIBROT,
      iris::FRACTAL_JULIA,
      iris::FRACTAL_MULTIJULIA,
      iris::FRACTAL_PHOENIX_JULIA,
      iris::FRACTAL_MANOWAR,
      iris::FRACTAL_BURNING_SHIP,
      iris::FRACTAL_CELTIC,
      iris::FRACTAL_TRICORN,
      iris::FRACTAL_SPIDER,
      iris::FRACTAL_NEWTON,
      iris::FRACTAL_NOVA,
    }};
    for (int mode : modes) {
      menu->addChild(createCheckMenuItem(
        iris::builtinFractalName(mode), "",
        [this, mode]() { return module->fractalMode == mode; },
        [this, mode]() { module->requestFractal(mode); }));
    }
    e.consume(this);
  }

  void draw(const DrawArgs& args) override {
    TL1105::draw(args);
    const float cx = 0.5f * box.size.x;
    const float cy = 0.5f * box.size.y;
    const float dy = std::max(1.6f, 0.16f * box.size.y);
    const float halfW = std::max(1.9f, 0.22f * box.size.x);
    const float y0 = cy - dy;
    for (int i = 0; i < 3; ++i) {
      const float y = y0 + dy * float(i);
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, cx - halfW, y);
      nvgLineTo(args.vg, cx + halfW, y);
      nvgStrokeWidth(args.vg, 1.2f);
      nvgStrokeColor(args.vg, nvgRGBA(225, 232, 240, 244));
      nvgStroke(args.vg);
    }
  }
};

struct NautiloidZoomReadout final : TransparentWidget {
  static constexpr float LABEL_FONT_SIZE = 11.5f;
  Nautiloid* module = nullptr;

  explicit NautiloidZoomReadout(Nautiloid* module) : module(module) {}

  void draw(const DrawArgs& args) override {
    if (!module) return;
    const float pct = 100.f * clamp(module->fractalZoom / kNautiloidMaxFractalZoom, 0.f, 1.f);
    const std::string text = string::f("Zoom: %.2f%%", pct);

    nvgFontSize(args.vg, LABEL_FONT_SIZE);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgTextLetterSpacing(args.vg, 0.f);
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, text.c_str(), nullptr);
  }
};

struct NautiloidLocationCodeDisplay final : Widget {
  Nautiloid* module = nullptr;

  void draw(const DrawArgs& args) override {
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 3.f);
    nvgFillColor(args.vg, nvgRGB(5, 10, 17));
    nvgFill(args.vg);
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.75f, 0.75f, box.size.x - 1.5f, box.size.y - 1.5f, 3.f);
    nvgStrokeWidth(args.vg, 1.5f);
    const bool valid = !module || module->locationCodeInputValid.load(std::memory_order_relaxed);
    nvgStrokeColor(args.vg, valid
      ? nvgRGBA(64, 196, 222, 180)
      : nvgRGBA(160, 74, 118, 210));
    nvgStroke(args.vg);
  }

  void drawLayer(const DrawArgs& args, int layer) override {
    if (layer == 1) {
      std::shared_ptr<window::Font> font = APP->window->loadFont(
        asset::system("res/fonts/ShareTechMono-Regular.ttf"));
      if (font && font->handle >= 0) {
        const std::string code = module
          ? module->locationCodeSnapshot()
          : nautiloid_location::encode(Nautiloid::FractalState {});
        nvgFontFaceId(args.vg, font->handle);
        nvgFontSize(args.vg, 12.f);
        nvgTextLetterSpacing(args.vg, 0.f);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg, nvgRGB(215, 245, 255));
        nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, code.c_str(), nullptr);
      }
    }
    Widget::drawLayer(args, layer);
  }
};

struct NautiloidClipboardButton final : LeviathanIconButton {
  std::string tooltipText;
  ui::Tooltip* tooltip = nullptr;

  ~NautiloidClipboardButton() override {
    destroyTooltip();
  }

  void createTooltip() {
    if (!settings::tooltips || tooltip || !APP || !APP->scene)
      return;
    tooltip = new ui::Tooltip();
    tooltip->text = tooltipText;
    APP->scene->addChild(tooltip);
  }

  void destroyTooltip() {
    if (!tooltip)
      return;
    if (APP && APP->scene)
      APP->scene->removeChild(tooltip);
    delete tooltip;
    tooltip = nullptr;
  }

  void onEnter(const event::Enter& e) override {
    LeviathanIconButton::onEnter(e);
    createTooltip();
  }

  void onLeave(const event::Leave& e) override {
    LeviathanIconButton::onLeave(e);
    destroyTooltip();
  }
};

struct NautiloidColorModeButton final : LeviathanIconButton {
  Nautiloid* module = nullptr;
  ui::Tooltip* tooltip = nullptr;

  ~NautiloidColorModeButton() override {
    destroyTooltip();
  }

  void createTooltip() {
    if (!settings::tooltips || tooltip || !APP || !APP->scene) return;
    tooltip = new ui::Tooltip();
    updateTooltip();
    APP->scene->addChild(tooltip);
  }

  void destroyTooltip() {
    if (!tooltip) return;
    if (APP && APP->scene) APP->scene->removeChild(tooltip);
    delete tooltip;
    tooltip = nullptr;
  }

  void updateTooltip() {
    if (tooltip) {
      tooltip->text = std::string("Fractal color: ") +
        nautiloidColorModeName(nautiloidColorMode(module)) + "\nClick to cycle";
    }
  }

  void onEnter(const event::Enter& e) override {
    LeviathanIconButton::onEnter(e);
    createTooltip();
  }

  void onLeave(const event::Leave& e) override {
    LeviathanIconButton::onLeave(e);
    destroyTooltip();
  }

  void step() override {
    updateTooltip();
    LeviathanIconButton::step();
  }

  void draw(const DrawArgs& args) override {
    LeviathanIconButton::draw(args);
    const int mode = nautiloidColorMode(module);
    const float radius = std::min(box.size.x, box.size.y) * 0.105f;
    const float gap = radius * 1.55f;
    const float centerX = 0.5f * box.size.x;
    const float centerY = 0.5f * box.size.y;
    for (int i = 0; i < 3; ++i) {
      NVGcolor color;
      if (mode == Nautiloid::COLOR_PRISM) {
        static const NVGcolor prism[3] = {
          nvgRGB(88, 65, 191), nvgRGB(28, 204, 217), nvgRGB(232, 82, 202)
        };
        color = prism[i];
      } else {
        const nautiloid_color::Rgb stop = nautiloid_color::stop(mode, i);
        color = nvgRGB(uint8_t(stop.r), uint8_t(stop.g), uint8_t(stop.b));
      }
      nvgBeginPath(args.vg);
      nvgCircle(args.vg, centerX + float(i - 1) * gap, centerY, radius);
      nvgFillColor(args.vg, color);
      nvgFill(args.vg);
      nvgStrokeWidth(args.vg, 0.65f);
      nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 150));
      nvgStroke(args.vg);
    }
  }
};

struct NautiloidLocationValidLight final : SmallAperture<GreenApertureLight> {
  void step() override {
    if (module) {
      SmallAperture<GreenApertureLight>::step();
    } else {
      setBrightnesses({1.f});
    }
  }
};

} // namespace

struct NautiloidWidget final : ModuleWidget {
  explicit NautiloidWidget(Nautiloid* module) {
    setModule(module);
    visual_assets::SplitPanelRenderer splitPanel(this, "res/nautiloid.panel.svg");
    const std::string& panelPath = splitPanel.panelPath();
    splitPanel.addLabels("res/nautiloid.labels.svg");
    visual_assets::addFractalGlassOverlay(this, panelPath);
    addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<CyanOrbScrew>(
      Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    auto rectMm = [&](const char* id, math::Rect fallback) {
      math::Rect rect = fallback;
      panel_svg::loadRectFromSvgMm(panelPath, id, &rect);
      return rect;
    };
    auto pointMm = [&](const char* id, Vec fallback) {
      Vec point = fallback;
      panel_svg::loadPointFromSvgMm(panelPath, id, &point);
      return point;
    };

    const math::Rect displayRectMm = rectMm("DISPLAY", math::Rect(Vec(1.8f, 6.5f), Vec(98.f, 65.27f)));
    addChild(visual_assets::createPreviewFrameEnhancementWidget(
      displayRectMm, visual_assets::PreviewFrameTint::Cyan));
    NautiloidGlPreview* glPreview = new NautiloidGlPreview(module);
    glPreview->box.pos = mm2px(displayRectMm.pos.plus(Vec(0.4f, 0.4f)));
    glPreview->box.size = mm2px(displayRectMm.size.minus(Vec(0.8f, 0.8f)));
    addChild(glPreview);
    widget::FramebufferWidget* displayFb = new widget::FramebufferWidget();
    displayFb->box.pos = mm2px(displayRectMm.pos.plus(Vec(0.4f, 0.4f)));
    displayFb->box.size = mm2px(displayRectMm.size.minus(Vec(0.8f, 0.8f)));
    displayFb->dirtyOnSubpixelChange = false;
    NautiloidDisplay* display = new NautiloidDisplay(module);
    display->framebuffer = displayFb;
    display->box.size = displayFb->box.size;
    displayFb->addChild(display);
    addChild(displayFb);

    NautiloidZoomReadout* zoomReadout = new NautiloidZoomReadout(module);
    const math::Rect zoomReadoutRectMm = rectMm("ZOOM_READOUT", math::Rect(Vec(24.f, 72.2f), Vec(60.f, 5.2f)));
    zoomReadout->box.pos = mm2px(zoomReadoutRectMm.pos);
    zoomReadout->box.size = mm2px(zoomReadoutRectMm.size);
    addChild(zoomReadout);

    const math::Rect zoomBarRectMm = rectMm("ZOOM_BAR", math::Rect(Vec(5.f, 79.f), Vec(91.6f, 9.f)));
    addChild(visual_assets::createPreviewFrameEnhancementWidget(
      zoomBarRectMm, visual_assets::PreviewFrameTint::Purple));
    widget::FramebufferWidget* zoomFb = new widget::FramebufferWidget();
    zoomFb->box.pos = mm2px(zoomBarRectMm.pos);
    zoomFb->box.size = mm2px(zoomBarRectMm.size);
    zoomFb->dirtyOnSubpixelChange = false;
    NautiloidZoomSlider* zoomSlider = new NautiloidZoomSlider();
    zoomSlider->module = module;
    zoomSlider->framebuffer = zoomFb;
    zoomSlider->lightOverlayOffset = zoomFb->box.pos;
    zoomSlider->box.size = zoomFb->box.size;
    NautiloidZoomSpeedQuantity* zoomSpeed = new NautiloidZoomSpeedQuantity();
    zoomSlider->zoomSpeed = zoomSpeed;
    zoomSlider->quantity = zoomSpeed;
    zoomFb->addChild(zoomSlider);
    addChild(zoomFb);
    // Keep the LED out of the zoom artwork framebuffer, like LuminSlider.
    // Its glass SVG and emissive rectangle then composite live over the handle.
    addChild(zoomSlider->handleLight);

    NautiloidSourceButton* sourceButton =
      createParamCentered<NautiloidSourceButton>(
        mm2px(pointMm("SOURCE_MENU_PARAM", Vec(7.8f, 75.4f))), module, Nautiloid::SOURCE_MENU_PARAM);
    sourceButton->module = module;
    addParam(sourceButton);

    LeviathanResetButton* resetButton =
      createParamCentered<LeviathanResetButton>(
        mm2px(pointMm("RESET_VIEW_PARAM", Vec(19.2f, 75.4f))), module, Nautiloid::RESET_VIEW_PARAM);
    if (module) {
      resetButton->buttonAction = [module]() { module->resetView(); };
    }
    addParam(resetButton);

    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(pointMm("ZOOM_RATE_INPUT", Vec(88.6f, 75.4f))), module, Nautiloid::ZOOM_RATE_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(pointMm("X_VELOCITY_INPUT", Vec(22.f, 98.36f))), module, Nautiloid::X_VELOCITY_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(pointMm("Y_VELOCITY_INPUT", Vec(35.f, 98.36f))), module, Nautiloid::Y_VELOCITY_INPUT));
    auto* colorModeButton = new NautiloidColorModeButton();
    colorModeButton->module = module;
    colorModeButton->box.pos = mm2px(
      pointMm("COLOR_MODE_BUTTON", Vec(48.5f, 98.36f))).minus(
        colorModeButton->box.size.mult(0.5f));
    if (module) {
      colorModeButton->buttonAction = [module]() {
        const int next = (nautiloidColorMode(module) + 1) % Nautiloid::FRACTAL_COLOR_MODES_LEN;
        module->setFractalColorMode(next);
      };
    }
    addChild(colorModeButton);
    NautiloidLocationCodeDisplay* locationCodeDisplay = new NautiloidLocationCodeDisplay();
    locationCodeDisplay->module = module;
    const math::Rect locationCodeRectMm = rectMm(
      "LOCATION_CODE_FIELD", math::Rect(Vec(30.7f, 109.f), Vec(36.f, 7.f)));
    locationCodeDisplay->box.pos = mm2px(locationCodeRectMm.pos);
    locationCodeDisplay->box.size = mm2px(locationCodeRectMm.size);
    addChild(locationCodeDisplay);
    addChild(createLightCentered<NautiloidLocationValidLight>(
      mm2px(pointMm("LOCATION_CODE_VALID_LIGHT", Vec(69.3f, 112.5f))),
      module, Nautiloid::LOCATION_CODE_VALID_LIGHT));
    auto* copyLocationButton = new NautiloidClipboardButton();
    copyLocationButton->tooltipText = "Copy location code";
    copyLocationButton->iconSvg = visual_assets::loadPluginSvgCached("res/icon/nautiloid-copy.svg");
    copyLocationButton->iconScale = 0.52f;
    copyLocationButton->box.pos = mm2px(
      pointMm("COPY_LOCATION_BUTTON", Vec(77.f, 112.5f))).minus(
        copyLocationButton->box.size.mult(0.5f));
    if (module) {
      copyLocationButton->buttonAction = [module]() {
        if (!APP || !APP->window || !APP->window->win)
          return;
        const std::string code = module->locationCodeSnapshot();
        glfwSetClipboardString(APP->window->win, code.c_str());
        module->locationCodeInputValid.store(true, std::memory_order_relaxed);
      };
    }
    addChild(copyLocationButton);
    auto* pasteLocationButton = new NautiloidClipboardButton();
    pasteLocationButton->tooltipText = "Paste location code";
    pasteLocationButton->iconSvg = visual_assets::loadPluginSvgCached("res/icon/nautiloid-paste.svg");
    pasteLocationButton->iconScale = 0.52f;
    pasteLocationButton->box.pos = mm2px(
      pointMm("PASTE_LOCATION_BUTTON", Vec(87.f, 112.5f))).minus(
        pasteLocationButton->box.size.mult(0.5f));
    if (module) {
      pasteLocationButton->buttonAction = [module]() {
        if (!APP || !APP->window || !APP->window->win)
          return;
        const char* clipboard = glfwGetClipboardString(APP->window->win);
        const bool valid = clipboard && module->loadLocationCode(clipboard);
        module->locationCodeInputValid.store(valid, std::memory_order_relaxed);
      };
    }
    addChild(pasteLocationButton);
    addChild(createLightCentered<SmallAperture<AmberGreenVioletApertureLight>>(
      mm2px(pointMm("IRIS_EXPANDER_LIGHT", Vec(98.4f, 5.8f))), module, Nautiloid::IRIS_LINK_LIGHT));

    NautiloidDebugCounters* counters = new NautiloidDebugCounters(module);
    const math::Rect countersRectMm = rectMm("DEBUG_COUNTERS", math::Rect(Vec(48.0f, 93.8f), Vec(50.5f, 10.8f)));
    counters->box.pos = mm2px(countersRectMm.pos);
    counters->box.size = mm2px(countersRectMm.size);
    addChild(counters);

  }

  void appendContextMenu(Menu* menu) override {
    ModuleWidget::appendContextMenu(menu);
    Nautiloid* naut = dynamic_cast<Nautiloid*>(module);
    if (!naut || !isDragonKingDebugEnabled()) return;

    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Nautiloid Debug"));
    menu->addChild(createCheckMenuItem(
      "Log fractal pipeline to file", "",
      [naut]() {
        return naut->debugFileLoggingEnabled.load(std::memory_order_relaxed);
      },
      [naut]() {
        const bool current = naut->debugFileLoggingEnabled.load(std::memory_order_relaxed);
        naut->debugFileLoggingEnabled.store(!current, std::memory_order_relaxed);
      }));
    menu->addChild(createCheckMenuItem(
      "GPU preview for built-in fractals", "",
      [naut]() {
        return naut->debugGpuPreviewEnabled.load(std::memory_order_relaxed);
      },
      [naut]() {
        const bool current = naut->debugGpuPreviewEnabled.load(std::memory_order_relaxed);
        naut->debugGpuPreviewEnabled.store(!current, std::memory_order_relaxed);
        naut->debugGpuPreviewAvailable.store(false, std::memory_order_relaxed);
      }));
    menu->addChild(createMenuLabel(nautiloidDebugLogPath()));
  }
};

Model* modelNautiloid = createModel<Nautiloid, NautiloidWidget>("Nautiloid");
