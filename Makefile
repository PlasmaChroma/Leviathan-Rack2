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

TEST_BINS := \
	build/tests/platter_spec_harness \
	build/tests/temporaldeck_arc_lights_spec \
	build/tests/temporaldeck_engine_spec \
	build/tests/temporaldeck_expander_preview_spec \
	build/tests/temporaldeck_menu_utils_spec \
	build/tests/temporaldeck_frame_input_spec \
	build/tests/temporaldeck_platter_input_spec \
	build/tests/temporaldeck_sample_prep_spec \
	build/tests/temporaldeck_virtual_integration_spec

.PHONY: test test-build
test-build: $(TEST_BINS)

test: test-build
	@build/tests/platter_spec_harness
	@build/tests/temporaldeck_arc_lights_spec
	@build/tests/temporaldeck_engine_spec
	@build/tests/temporaldeck_expander_preview_spec
	@build/tests/temporaldeck_menu_utils_spec
	@build/tests/temporaldeck_frame_input_spec
	@build/tests/temporaldeck_platter_input_spec
	@build/tests/temporaldeck_sample_prep_spec
	@build/tests/temporaldeck_virtual_integration_spec

build/tests:
	@mkdir -p $@

build/tests/platter_spec_harness: tests/platter_spec_main.cpp tests/platter_spec_cases.cpp tests/platter_trace_replay.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_arc_lights_spec: tests/temporaldeck_arc_lights_spec.cpp src/TemporalDeckArcLights.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_engine_spec: tests/temporaldeck_engine_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_expander_preview_spec: tests/temporaldeck_expander_preview_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_menu_utils_spec: tests/temporaldeck_menu_utils_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_frame_input_spec: tests/temporaldeck_frame_input_spec.cpp src/TemporalDeckFrameInput.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_platter_input_spec: tests/temporaldeck_platter_input_spec.cpp src/TemporalDeckPlatterInput.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_sample_prep_spec: tests/temporaldeck_sample_prep_spec.cpp src/TemporalDeckSamplePrep.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/temporaldeck_virtual_integration_spec: tests/temporaldeck_virtual_integration_spec.cpp src/TemporalDeckPlatterInput.cpp src/TemporalDeckTransportControl.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@
