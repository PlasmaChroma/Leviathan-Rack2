// Headless-window integration test. Substitute only Rack's private Scene/Window
// construction and frame clock; use real Widget traversal, NanoVG and OpenGL.
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <app/Scene.hpp>
#include <window/Window.hpp>
#undef PRIVATE
#include "../src/visual/AdaptiveGlSurface.hpp"
#include "../src/GlResourceRetirement.hpp"
#define NANOVG_GL2
#include <nanovg_gl.h>
#include <cstdio>
#include <cstdlib>
#include <vector>

static double testFrame = 1.;
namespace rack {
// The fixture owns its stack members; the real Context destructor owns heap
// host services and assumes Rack logging/host initialization.
Context::~Context() {}
namespace app {
Scene::Scene() : internal(nullptr), rackScroll(nullptr), rack(nullptr), menuBar(nullptr), browser(nullptr) {}
Scene::~Scene() {}
}
namespace window {
Window::Window() : internal(nullptr) {}
Window::~Window() {}
double Window::getFrameTime() { return testFrame; }
}
}

static unsigned checks = 0;
static bool failAllocation = false;
static unsigned driverErrors = 0;
static void APIENTRY debugMessage(GLenum, GLenum type, GLuint, GLenum, GLsizei, const GLchar* message, const void*) {
  if (type == GL_DEBUG_TYPE_ERROR) {
    ++driverErrors;
    std::fprintf(stderr, "GL: %s\n", message);
  }
}

static void check(bool pass, const char* message) {
  ++checks;
  if (!pass) { std::fprintf(stderr, "FAIL: %s (GL error 0x%x)\n", message, glGetError()); std::exit(1); }
}
static GLint integer(GLenum key) { GLint v = 0; glGetIntegerv(key, &v); return v; }
static void service(rack::app::Scene& scene) {
  for (auto* child : scene.children) child->step();
}

struct DrawCapture {
  GLuint fbo = 0;
  GLuint texture = 0;
  int calls = 0;
  bool paddingClear = false;
};
static void render(void* user, rack::math::Vec size, int) {
  auto& capture = *static_cast<DrawCapture*>(user);
  ++capture.calls;
  check(glGetError() == GL_NO_ERROR, "allocation and guard entry have no GL errors");
  capture.fbo = GLuint(integer(GL_FRAMEBUFFER_BINDING));
  GLint texture = 0;
  glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);
  capture.texture = GLuint(texture);
  // Test full-capacity clear before the callback changes state/pixels.
  unsigned char pixel[4] = {255,255,255,255};
  glReadPixels(63, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
  capture.paddingClear = pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0 && pixel[3] == 0;
  glViewport(0, 0, int(size.x), int(size.y));
  glClearColor(1, 0, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glDisableVertexAttribArray(1);
  check(glGetError() == GL_NO_ERROR, "callback has no GL errors");
}

int main() {
  check(glfwInit(), "GLFW initialized");
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow* native = glfwCreateWindow(128, 128, "Leviathan GL lifecycle test", nullptr, nullptr);
  check(native != nullptr, "hidden GL window created");
  glfwMakeContextCurrent(native);
  check(glewInit() == GLEW_OK, "GLEW initialized");
  while (glGetError() != GL_NO_ERROR) {}
  if (glDebugMessageCallback) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessage, nullptr);
  }
  NVGcontext* vg = nvgCreateGL2(NVG_ANTIALIAS);
  check(vg != nullptr, "real NanoVG context created");
  rack::Context ctx;
  rack::widget::EventState events;
  ctx.event = &events;
  rack::app::Scene scene;
  rack::window::Window window;
  window.win = native;
  window.vg = vg;
  ctx.scene = &scene;
  ctx.window = &window;
  rack::contextSet(&ctx);

  auto lease = gl_lifecycle::acquireResourceContext(vg);
  check(lease && scene.children.size() == 1, "one scene service installed");
  check(lease == gl_lifecycle::acquireResourceContext(vg), "context service reused");
  NVGLUframebuffer* splitRead = nvgluCreateFramebuffer(vg, 8, 8, 0);
  check(splitRead != nullptr, "split-binding fixture allocated");
  GLint expectedRead = 0, expectedRenderbuffer = 0;
  GLuint texture0 = 0, texture3 = 0, buffer = 0, unpackBuffer = 0;
  glGenTextures(1, &texture0); glGenTextures(1, &texture3);
  glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture0);
  glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, texture3);
  glGenBuffers(1, &buffer); glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 256, nullptr, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 16, reinterpret_cast<void*>(4));
  glEnableVertexAttribArray(1);
  glGenBuffers(1, &unpackBuffer); glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpackBuffer);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, 16, nullptr, GL_STATIC_DRAW);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 7);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 2);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 3);
  glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);
  glEnable(GL_SCISSOR_TEST); glScissor(1, 1, 1, 1);
  visual_assets::AdaptiveGlSurfacePolicy policy;
  policy.vertexAttributeCount = 4;
  policy.maxDensity = 2;
  DrawCapture capture;
  auto* surface = new visual_assets::AdaptiveGlSurface;
  auto verifyState = [&]() {
    check(glGetError() == GL_NO_ERROR, "guard restoration has no GL errors");
    check(integer(GL_ACTIVE_TEXTURE) == GL_TEXTURE3, "active texture restored");
    check(GLuint(integer(GL_TEXTURE_BINDING_2D)) == texture3, "incoming texture binding restored");
    glActiveTexture(GL_TEXTURE0);
    check(GLuint(integer(GL_TEXTURE_BINDING_2D)) == texture0, "unit zero binding restored");
    glActiveTexture(GL_TEXTURE3);
    check(integer(GL_UNPACK_ALIGNMENT) == 8 && integer(GL_UNPACK_ROW_LENGTH) == 7
      && integer(GL_UNPACK_SKIP_ROWS) == 2 && integer(GL_UNPACK_SKIP_PIXELS) == 3, "unpack state restored");
    check(GLuint(integer(GL_PIXEL_UNPACK_BUFFER_BINDING)) == unpackBuffer, "unpack buffer restored");
    check(GLuint(integer(GL_ARRAY_BUFFER_BINDING)) == buffer, "array buffer restored");
    GLint enabled = 0, size = 0, attribBuffer = 0; void* pointer = nullptr;
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
    glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &attribBuffer);
    glGetVertexAttribPointerv(1, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
    check(enabled && size == 3 && GLuint(attribBuffer) == buffer && pointer == reinterpret_cast<void*>(4), "generic vertex input restored");
    GLboolean mask[4]; glGetBooleanv(GL_COLOR_WRITEMASK, mask);
    check(!mask[0] && mask[1] && !mask[2] && !mask[3], "incoming color mask restored");
    check(glIsEnabled(GL_SCISSOR_TEST), "scissor enable restored");
    check(integer(GL_DRAW_FRAMEBUFFER_BINDING) == 0, "draw framebuffer restored");
    check(integer(GL_READ_FRAMEBUFFER_BINDING) == expectedRead, "read framebuffer restored");
    check(integer(GL_RENDERBUFFER_BINDING) == expectedRenderbuffer, "renderbuffer restored");
    check(glGetError() == GL_NO_ERROR, "no GL errors after surface render");
  };
  check(glGetError() == GL_NO_ERROR, "setup has no GL errors");
  check(surface->renderIfNeeded(vg, {32,32}, 1.19f, 1, policy, true, render, &capture), "initial 119% render");
  check(capture.paddingClear, "all padding channels cleared despite restrictive mask");
  verifyState();
  check(!surface->renderIfNeeded(vg, {32,32}, 1.19f, 1, policy, true, render, &capture), "idle surface reuses image");
  for (float zoom : {1.9f, 0.5f, 1.19f}) {
    surface->markDirty();
    check(surface->renderIfNeeded(vg, {32,32}, zoom, 1, policy, true, render, &capture), "growth/shrink render");
    check(capture.paddingClear, "reused target fully cleared");
    verifyState();
  }
  expectedRead = GLint(splitRead->fbo); expectedRenderbuffer = GLint(splitRead->rbo);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, splitRead->fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, splitRead->rbo);
  surface->markDirty();
  check(surface->renderIfNeeded(vg, {32,32}, 1.19f, 1, policy, true, render, &capture), "render with split incoming framebuffer bindings");
  verifyState();
  const auto beforeFailure = surface->generation();
  const int callsBeforeFailure = capture.calls;
  failAllocation = true;
  surface->markDirty();
  check(!surface->renderIfNeeded(vg, {48,48}, 1.19f, 1, policy, true, render, &capture), "forced allocation failure returns cleanly");
  check(surface->generation() == beforeFailure && capture.calls == callsBeforeFailure,
    "failure preserves the last rendered front and skips callback");
  verifyState();
  check(surface->renderIfNeeded(vg, {32,32}, 1.19f, 1, policy, true, render, &capture), "render recovers after allocation failure");
  verifyState();
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  nvgBeginFrame(vg, 128, 128, 1);
  rack::widget::Widget::DrawArgs drawArgs; drawArgs.vg = vg;
  check(surface->draw(drawArgs, {32,32}), "surface queues a NanoVG presentation");
  const GLuint retiredFbo = capture.fbo, retiredTexture = capture.texture;
  delete surface;
  check(glIsFramebuffer(retiredFbo) && glIsTexture(retiredTexture), "destruction defers GPU deletion");
  service(scene);
  check(glIsFramebuffer(retiredFbo), "same-frame maintenance preserves queued presentations");
  nvgEndFrame(vg);
  ++testFrame;
  glfwMakeContextCurrent(nullptr);
  service(scene);
  glfwMakeContextCurrent(native);
  check(glIsFramebuffer(retiredFbo), "maintenance waits for owning current context");
  service(scene);
  check(!glIsFramebuffer(retiredFbo) && !glIsTexture(retiredTexture), "next-frame service deletes last module resources");

  check(gl_lifecycle::resourceRetirementStats().pendingFramebuffers == 0, "all retired framebuffer wrappers reclaimed");
  const auto beforeRemovalLoop = gl_lifecycle::resourceRetirementStats();
  for (int i = 0; i < 100; ++i) {
    auto* transient = new visual_assets::AdaptiveGlSurface;
    check(transient->renderIfNeeded(vg, {32,32}, 1.19f, 1, policy, true, render, &capture), "repeated instance renders");
    delete transient;
    ++testFrame;
    service(scene);
    check(!glIsFramebuffer(capture.fbo) && !glIsTexture(capture.texture), "repeated instance resources reclaimed");
  }
  const auto afterRemovalLoop = gl_lifecycle::resourceRetirementStats();
  check(afterRemovalLoop.pendingFramebuffers == 0
    && afterRemovalLoop.deletedFramebuffers - beforeRemovalLoop.deletedFramebuffers == 100,
    "100 removal cycles have zero outstanding wrappers");

  GLuint retiredBuffer = 0, retiredQuery = 0;
  const GLuint retiredProgram = glCreateProgram();
  const GLuint retiredShader = glCreateShader(GL_VERTEX_SHADER);
  glGenBuffers(1, &retiredBuffer); glBindBuffer(GL_ARRAY_BUFFER, retiredBuffer);
  glGenQueries(1, &retiredQuery); glBeginQuery(GL_SAMPLES_PASSED, retiredQuery); glEndQuery(GL_SAMPLES_PASSED);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  gl_lifecycle::retireObject(lease, gl_lifecycle::ObjectKind::Buffer, retiredBuffer);
  gl_lifecycle::retireObject(lease, gl_lifecycle::ObjectKind::Program, retiredProgram);
  gl_lifecycle::retireObject(lease, gl_lifecycle::ObjectKind::Shader, retiredShader);
  gl_lifecycle::retireObject(lease, gl_lifecycle::ObjectKind::Query, retiredQuery);
  check(glIsBuffer(retiredBuffer) && glIsProgram(retiredProgram) && glIsShader(retiredShader)
    && glIsQuery(retiredQuery), "raw renderer objects remain valid until maintenance");
  ++testFrame; service(scene);
  check(!glIsBuffer(retiredBuffer) && !glIsProgram(retiredProgram) && !glIsShader(retiredShader)
    && !glIsQuery(retiredQuery), "all raw renderer object kinds reclaimed");
  check(gl_lifecycle::resourceRetirementStats().pendingObjects == 0, "no raw objects remain queued");
  // A pending wrapper must also be freed if its context disappears, without
  // issuing GL deletion against a subsequent lifetime with reused names.
  NVGLUframebuffer* abandoned = nvgluCreateFramebuffer(vg, 8, 8, 0);
  check(abandoned != nullptr, "abandonment fixture allocated");
  const GLuint abandonedFbo = abandoned->fbo, abandonedRbo = abandoned->rbo;
  const int abandonedImage = abandoned->image;
  gl_lifecycle::retireFramebuffer(lease, abandoned);
  const auto beforeLoss = gl_lifecycle::resourceRetirementStats();
  GLuint disposable = 0; glGenTextures(1, &disposable); glBindTexture(GL_TEXTURE_2D, disposable);
  gl_lifecycle::retireObject(lease, gl_lifecycle::ObjectKind::Texture, disposable);
  rack::widget::Widget::ContextCreateEvent createEvent; createEvent.vg = vg;
  scene.onContextCreate(createEvent);
  const auto afterLoss = gl_lifecycle::resourceRetirementStats();
  check(afterLoss.pendingFramebuffers == 0
    && afterLoss.freedAbandonedFramebuffers == beforeLoss.freedAbandonedFramebuffers + 1,
    "context loss frees CPU wrappers independently of driver objects");
  check(glIsFramebuffer(abandonedFbo), "abandonment performs no GL deletion");
  // This test simulates loss on the same real context; explicitly clean up the
  // fixture's names, which a real host would reclaim by destroying the context.
  glDeleteFramebuffers(1, &abandonedFbo); glDeleteRenderbuffers(1, &abandonedRbo);
  nvgDeleteImage(vg, abandonedImage);
  check(!gl_lifecycle::resourceContextMatches(lease, vg), "same pointer cannot revive old context lease");
  ++testFrame; service(scene);
  check(glIsTexture(disposable), "abandoned context never deletes names in replacement lifetime");
  auto replacement = gl_lifecycle::acquireResourceContext(vg);
  check(replacement != lease, "replacement lifetime gets distinct lease");
  gl_lifecycle::retireObject(replacement, gl_lifecycle::ObjectKind::Texture, disposable);
  rack::widget::Widget::ContextDestroyEvent destroyEvent; destroyEvent.vg = vg;
  scene.onContextDestroy(destroyEvent);
  check(!glIsTexture(disposable), "context-destroy event drains pending resources");

  while (!scene.children.empty()) {
    auto* child = scene.children.front(); scene.removeChild(child); delete child;
  }
  glDisableVertexAttribArray(1); glBindBuffer(GL_ARRAY_BUFFER, 0);
  nvgluDeleteFramebuffer(splitRead);
  glDeleteBuffers(1, &unpackBuffer);
  glDeleteBuffers(1, &buffer); glDeleteTextures(1, &texture0); glDeleteTextures(1, &texture3);
  check(driverErrors == 0 && glGetError() == GL_NO_ERROR, "no driver errors across complete lifecycle test");
  nvgDeleteGL2(vg);
  ctx.scene = nullptr; ctx.window = nullptr; ctx.event = nullptr;
  rack::contextSet(nullptr);
  glfwDestroyWindow(native); glfwTerminate();
  std::printf("GL surface/lifecycle integration: %u checks passed\n", checks);
}

// Inject only the allocation boundary; all successful calls use real NVGLU.
static NVGLUframebuffer* createFramebufferProbe(NVGcontext* vg, int w, int h, int flags) {
  if (failAllocation) {
    failAllocation = false;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    return nullptr;
  }
  return nvgluCreateFramebuffer(vg, w, h, flags);
}
#define nvgluCreateFramebuffer createFramebufferProbe
#include "../src/visual/AdaptiveGlSurface.cpp"
#undef nvgluCreateFramebuffer
