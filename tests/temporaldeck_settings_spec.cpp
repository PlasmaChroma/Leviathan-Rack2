#include "../src/TemporalDeckSettings.hpp"
#include <cstdlib>
#include <iostream>

int main() {
  json_t* root = json_object();
  bool pass = temporaldeck_settings::readScratchSmoothing(root);
  for (bool enabled : {false, true}) {
    temporaldeck_settings::writeScratchSmoothing(root, enabled);
    char* text = json_dumps(root, 0);
    json_error_t error;
    json_t* restored = json_loads(text, 0, &error);
    pass = pass && restored && temporaldeck_settings::readScratchSmoothing(restored) == enabled;
    json_decref(restored);
    std::free(text);
  }
  json_object_set_new(root, "scratchSmoothingEnabled", json_string("invalid"));
  pass = pass && temporaldeck_settings::readScratchSmoothing(root);
  json_object_del(root, "scratchSmoothingEnabled");
  pass = pass && temporaldeck_settings::readScratchSmoothing(root);
  json_decref(root);
  std::cout << (pass ? "PASS" : "FAIL") << ": smoothing JSON round-trip, legacy default and malformed value\n";
  return pass ? 0 : 1;
}
