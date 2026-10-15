#pragma once
#include <jansson.h>

namespace temporaldeck_settings {
inline bool readScratchSmoothing(json_t* root) {
  json_t* value = json_object_get(root, "scratchSmoothingEnabled");
  return !json_is_boolean(value) || json_is_true(value);
}
inline void writeScratchSmoothing(json_t* root, bool enabled) {
  json_object_set_new(root, "scratchSmoothingEnabled", json_boolean(enabled));
}
} // namespace temporaldeck_settings
