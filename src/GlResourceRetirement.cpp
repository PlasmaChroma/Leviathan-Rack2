#include "GlResourceRetirement.hpp"
#include <nanovg_gl.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vector>

namespace gl_lifecycle {
namespace { thread_local ResourceRetirementStats stats; }
ResourceRetirementStats resourceRetirementStats() { return stats; }
struct ResourceContext {
  struct Entry {
    ObjectKind kind;
    GLuint name;
    NVGLUframebuffer* framebuffer;
    double frame;
  };
  app::Scene* scene = nullptr;
  NVGcontext* vg = nullptr;
  GLFWwindow* native = nullptr;
  bool alive = true;
  std::vector<Entry> pending;

  // No driver/NanoVG calls after context loss or from destructors. The CPU
  // wrapper is malloc-owned independently of the context-owned GL objects.
  void abandon() {
    alive = false;
    for (const Entry& entry : pending) {
      if (entry.framebuffer) {
        std::free(entry.framebuffer);
        --stats.pendingFramebuffers;
        ++stats.freedAbandonedFramebuffers;
      } else {
        --stats.pendingObjects;
      }
    }
    pending.clear();
  }
  ~ResourceContext() { abandon(); }

  void drain(double frame, bool all = false) {
    size_t keep = 0;
    for (const Entry& entry : pending) {
      if (!all && !(entry.frame < frame)) {
        pending[keep++] = entry;
        continue;
      }
      if (entry.framebuffer) {
        nvgluDeleteFramebuffer(entry.framebuffer);
        --stats.pendingFramebuffers;
        ++stats.deletedFramebuffers;
      } else {
        --stats.pendingObjects;
        ++stats.deletedObjects;
        switch (entry.kind) {
          case ObjectKind::Buffer: glDeleteBuffers(1, &entry.name); break;
          case ObjectKind::Program: glDeleteProgram(entry.name); break;
          case ObjectKind::Shader: glDeleteShader(entry.name); break;
          case ObjectKind::Texture: glDeleteTextures(1, &entry.name); break;
          case ObjectKind::Query: glDeleteQueries(1, &entry.name); break;
        }
      }
    }
    pending.resize(keep);
  }
};

namespace {
// Weak registry cannot keep either a removed scene or its resources alive.
thread_local std::vector<std::weak_ptr<ResourceContext>> contexts;
struct RetirementService : widget::Widget {
  ContextLease lease;
  explicit RetirementService(ContextLease lease) : lease(std::move(lease)) {}
  ~RetirementService() override { lease->abandon(); }
  void step() override {
    if (!lease->alive) { requestDelete(); return; }
    if (!APP || !APP->window) return;
    if (APP->window->win != lease->native
        || (APP->window->vg != lease->vg && APP->window->fbVg != lease->vg)) {
      lease->abandon();
      return;
    }
    // Rack makes the window context current before Scene::step(). A strictly
    // later frame ensures queued NanoVG presentations have reached EndFrame.
    if (resourceContextIsCurrent(lease))
      lease->drain(APP->window->getFrameTime());
  }
  void onContextDestroy(const ContextDestroyEvent& e) override {
    if (resourceContextIsCurrent(lease)
        && (lease->vg == e.vg || lease->vg == APP->window->fbVg)) {
      lease->drain(0., true);
    }
    lease->abandon();
  }
  void onContextCreate(const ContextCreateEvent&) override {
    // Even a reused NVG pointer must not revive names from the old editor.
    lease->abandon();
  }
};

double retirementFrame() {
  const double frame = APP && APP->window ? APP->window->getFrameTime() : 0.;
  return std::isfinite(frame) ? frame : -std::numeric_limits<double>::infinity();
}
}

ContextLease acquireResourceContext(NVGcontext* vg) {
  if (!vg || !APP || !APP->scene || !APP->window) return {};
  for (auto it = contexts.begin(); it != contexts.end();) {
    auto lease = it->lock();
    if (!lease) { it = contexts.erase(it); continue; }
    if (lease->alive && lease->scene == APP->scene) {
      if (lease->native != APP->window->win
          || (lease->vg != APP->window->vg && lease->vg != APP->window->fbVg))
        lease->abandon();
      else if (lease->vg == vg)
        return lease;
    }
    ++it;
  }
  auto lease = std::make_shared<ResourceContext>();
  lease->scene = APP->scene;
  lease->vg = vg;
  lease->native = APP->window->win;
  contexts.push_back(lease);
  APP->scene->addChild(new RetirementService(lease));
  return lease;
}

bool resourceContextMatches(const ContextLease& lease, NVGcontext* vg) {
  return lease && lease->alive && lease->vg == vg && APP && APP->window
    && APP->scene == lease->scene && APP->window->win == lease->native
    && (APP->window->vg == vg || APP->window->fbVg == vg);
}

bool resourceContextIsCurrent(const ContextLease& lease) {
  return lease && lease->alive && lease->native && APP && APP->window
    && APP->window->win == lease->native && glfwGetCurrentContext() == lease->native
    && (APP->window->vg == lease->vg || APP->window->fbVg == lease->vg);
}

void retireObject(const ContextLease& lease, ObjectKind kind, GLuint name) {
  if (name && lease && lease->alive) {
    lease->pending.push_back({kind, name, nullptr, retirementFrame()});
    ++stats.pendingObjects;
  }
}

void retireFramebuffer(const ContextLease& lease, NVGLUframebuffer* framebuffer) {
  if (!framebuffer) return;
  if (lease && lease->alive) {
    lease->pending.push_back({ObjectKind::Texture, 0, framebuffer, retirementFrame()});
    ++stats.pendingFramebuffers;
  } else {
    std::free(framebuffer);
    ++stats.freedAbandonedFramebuffers;
  }
}
} // namespace gl_lifecycle
