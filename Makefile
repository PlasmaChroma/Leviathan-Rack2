# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../Rack-SDK

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS +=

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

.PHONY: test
test:
	@mkdir -p build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/platter_spec_main.cpp tests/platter_spec_cases.cpp tests/platter_trace_replay.cpp -o build/tests/platter_spec_harness
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/temporaldeck_arc_lights_spec.cpp src/TemporalDeckArcLights.cpp -o build/tests/temporaldeck_arc_lights_spec
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/temporaldeck_engine_spec.cpp -o build/tests/temporaldeck_engine_spec
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/temporaldeck_frame_input_spec.cpp src/TemporalDeckFrameInput.cpp -o build/tests/temporaldeck_frame_input_spec
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/temporaldeck_platter_input_spec.cpp src/TemporalDeckPlatterInput.cpp -o build/tests/temporaldeck_platter_input_spec
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/temporaldeck_sample_prep_spec.cpp src/TemporalDeckSamplePrep.cpp -o build/tests/temporaldeck_sample_prep_spec
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/temporaldeck_virtual_integration_spec.cpp src/TemporalDeckPlatterInput.cpp src/TemporalDeckTransportControl.cpp -o build/tests/temporaldeck_virtual_integration_spec
	@build/tests/platter_spec_harness
	@build/tests/temporaldeck_arc_lights_spec
	@build/tests/temporaldeck_engine_spec
	@build/tests/temporaldeck_frame_input_spec
	@build/tests/temporaldeck_platter_input_spec
	@build/tests/temporaldeck_sample_prep_spec
	@build/tests/temporaldeck_virtual_integration_spec
