#pragma once

#include <rack.hpp>

namespace ui_expander {

template <typename Lookup>
inline rack::engine::Module* resolveById(const rack::engine::Module* owner, bool right, Lookup&& lookup) {
  if (!owner) return nullptr;
  const auto& expander = right ? owner->rightExpander : owner->leftExpander;
  if (expander.moduleId < 0) return nullptr;
  auto* adjacent = lookup(expander.moduleId);
  return adjacent && (right ? adjacent->leftExpander.moduleId : adjacent->rightExpander.moduleId) == owner->id
    ? adjacent : nullptr;
}

// UI thread only. Rack updates Expander::module on the engine thread; the UI
// owns adjacency IDs and resolves them through Engine's synchronized lookup.
// Module removal is serialized with this caller on the UI thread.
inline rack::engine::Module* neighbor(const rack::engine::Module* owner, bool right) {
  if (!owner) return nullptr;
  const auto& expander = right ? owner->rightExpander : owner->leftExpander;
  auto* context = rack::contextGet();
  if (!context || !context->engine) {
    // Detached native test fixtures have no Rack engine or concurrent topology.
    auto* adjacent = expander.module;
    return adjacent && (right ? adjacent->leftExpander.module : adjacent->rightExpander.module) == owner
      ? adjacent : nullptr;
  }
  return resolveById(owner, right, [context](int64_t id) { return context->engine->getModule(id); });
}

} // namespace ui_expander
