// Reuse the real GL lifecycle fixture and production DSP harness, not renderer mocks.
#define main shared_lifecycle_main
#include "gl_surface_lifecycle_spec.cpp"
#undef main
#define main bifurx_runtime_main
#include "bifurx_runtime_spec.cpp"
#undef main
#include "../src/BifurxGL.cpp"
bool isExtraGlValidationEnabled() { return false; }

int main() {
  check(glfwInit(), "GLFW initialized");
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  auto* native = glfwCreateWindow(256, 128, "Bifurx renderer", nullptr, nullptr);
  check(native, "hidden GL context"); glfwMakeContextCurrent(native);
  check(glewInit() == GLEW_OK, "GLEW initialized");
  while (glGetError() != GL_NO_ERROR) {}
  std::printf("OpenGL renderer: %s; version: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
  auto* vg = nvgCreateGL2(NVG_ANTIALIAS);
  rack::Context ctx; rack::widget::EventState events; rack::app::Scene scene; rack::window::Window window;
  window.win = native; window.vg = vg; ctx.scene = &scene; ctx.window = &window; ctx.event = &events; rack::contextSet(&ctx);
  {
    Bifurx module; BifurxSpectrumGLWidget widget;
    widget.module = &module; widget.box.size = Vec(256, 128);
    module.renderMode = Bifurx::RENDER_OPENGL;
    widget.state.hasOverlay = true; widget.state.hasCurve = true;
    widget.state.previewState.mode = widget.state.displayedPreviewState.mode = 10;
    for (int i = 0; i < kCurvePointCount; ++i) widget.state.overlayOutputDbfs[i] = -18.f + 12.f * float(i)/kCurvePointCount;
    GLuint hostBuffer = 0; glGenBuffers(1, &hostBuffer); glBindBuffer(GL_ARRAY_BUFFER, hostBuffer); glBufferData(GL_ARRAY_BUFFER, 4096, nullptr, GL_STATIC_DRAW); glBindBuffer(GL_ARRAY_BUFFER, 0);
    visual_assets::AdaptiveGlSurface surface; visual_assets::AdaptiveGlSurfacePolicy policy; policy.vertexAttributeCount = 4; policy.maxDensity = 1.f;
    struct Render { BifurxSpectrumGLWidget* widget; std::vector<unsigned char> pixels; } render{&widget, {}};
    auto capture = [](void* opaque, Vec size, int y) {
      auto& r = *static_cast<Render*>(opaque); r.widget->renderGlContent(size, y);
      r.pixels.resize(int(size.x)*int(size.y)*4);
      glReadPixels(0, y, int(size.x), int(size.y), GL_RGBA, GL_UNSIGNED_BYTE, r.pixels.data());
    };
    for (int mode : {0, 10}) for (int renderer : {0, 1, 2}) {
      widget.state.hasPreview = true; widget.state.hasCurve = false;
      widget.state.previewState.mode = mode; ++widget.state.lastPreviewSeq;
      widget.updateCurveCache();
      module.useGlShaderRenderer.store(renderer != 0);
      if (renderer == 2) { widget.textureShaderReady = false; widget.textureShaderInitAttempted = true; }
      glDisable(GL_STENCIL_TEST); glDisable(GL_ALPHA_TEST); glBlendEquation(GL_FUNC_ADD); glBindBuffer(GL_ARRAY_BUFFER, 0);
      surface.markDirty(); check(surface.renderIfNeeded(vg, widget.box.size, 1.f, 1.f, policy, true, capture, &render), "baseline render");
      const auto baseline = render.pixels; uint64_t alpha = 0;
      for (size_t i = 3; i < baseline.size(); i += 4) alpha += baseline[i];
      check(alpha > 10000, "spectrum is visible");
      bool premultiplied = true;
      for (size_t i=0;i<baseline.size();i+=4) for (int c=0;c<3;++c) premultiplied &= int(baseline[i+c]) <= int(baseline[i+3])+1;
      check(premultiplied, "surface uses premultiplied RGBA for NanoVG");
      glEnable(GL_STENCIL_TEST); glStencilFunc(GL_NEVER, 0, 255);
      glEnable(GL_ALPHA_TEST); glAlphaFunc(GL_NEVER, 1.f);
      glBlendEquation(GL_FUNC_REVERSE_SUBTRACT); glBindBuffer(GL_ARRAY_BUFFER, hostBuffer);
      glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D); glActiveTexture(GL_TEXTURE0);
      glClientActiveTexture(GL_TEXTURE1); glTexCoordPointer(2, GL_FLOAT, 0, nullptr); glEnableClientState(GL_TEXTURE_COORD_ARRAY); glClientActiveTexture(GL_TEXTURE0);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 77); glPixelStorei(GL_UNPACK_SKIP_ROWS, 3);
      surface.markDirty(); check(surface.renderIfNeeded(vg, widget.box.size, 1.f, 1.f, policy, true, capture, &render), "hostile-state render");
      check(render.pixels == baseline, "hostile GL state has identical pixels");
      check(glIsEnabled(GL_STENCIL_TEST) && glIsEnabled(GL_ALPHA_TEST), "host enables restored");
      check(integer(GL_BLEND_EQUATION_RGB) == GL_FUNC_REVERSE_SUBTRACT && integer(GL_ARRAY_BUFFER_BINDING) == int(hostBuffer), "blend and VBO restored");
      check(integer(GL_UNPACK_ROW_LENGTH) == 77 && integer(GL_UNPACK_SKIP_ROWS) == 3, "pixel unpack restored");
      glActiveTexture(GL_TEXTURE1); check(glIsEnabled(GL_TEXTURE_2D), "other texture-unit enable restored"); glDisable(GL_TEXTURE_2D); glActiveTexture(GL_TEXTURE0);
      glClientActiveTexture(GL_TEXTURE1); check(glIsEnabled(GL_TEXTURE_COORD_ARRAY), "other client texture array restored"); glDisableClientState(GL_TEXTURE_COORD_ARRAY); glClientActiveTexture(GL_TEXTURE0);
      check(glGetError() == GL_NO_ERROR, "no GL errors");
      glDisable(GL_STENCIL_TEST); glDisable(GL_ALPHA_TEST); glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); glPixelStorei(GL_UNPACK_SKIP_ROWS, 0); glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glDeleteBuffers(1, &hostBuffer);
  }
  std::printf("Bifurx GL renderer: %u checks passed\n", checks);
  // Fixture process teardown owns remaining context resources.
  return 0;
}
