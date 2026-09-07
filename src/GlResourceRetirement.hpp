#pragma once

#include "plugin.hpp"
#include <memory>

namespace gl_lifecycle {
// Cold-path counters for lifecycle diagnostics/tests; no GL queries required.
struct ResourceRetirementStats {
  uint64_t pendingFramebuffers = 0;
  uint64_t pendingObjects = 0;
  uint64_t deletedFramebuffers = 0;
  uint64_t freedAbandonedFramebuffers = 0;
  uint64_t deletedObjects = 0;
};
ResourceRetirementStats resourceRetirementStats();
struct ResourceContext;
using ContextLease = std::shared_ptr<ResourceContext>;

// UI/graphics thread only. Acquire before creating persistent GL objects.
// The scene owns the maintenance service; removing the last module does not
// remove the consumer. Each editor context lifetime gets a distinct lease.
ContextLease acquireResourceContext(NVGcontext* vg);
bool resourceContextMatches(const ContextLease& lease, NVGcontext* vg);
bool resourceContextIsCurrent(const ContextLease& lease);
enum class ObjectKind { Buffer, Program, Shader, Texture, Query };
void retireObject(const ContextLease& lease, ObjectKind kind, GLuint name);
void retireFramebuffer(const ContextLease& lease, NVGLUframebuffer* framebuffer);
} // namespace gl_lifecycle
