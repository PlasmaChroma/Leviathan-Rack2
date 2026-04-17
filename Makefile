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
	build/tests/temporaldeck_platter_spec_harness \
	build/tests/temporaldeck_arc_lights_spec \
	build/tests/temporaldeck_engine_spec \
	build/tests/temporaldeck_expander_preview_spec \
	build/tests/temporaldeck_menu_utils_spec \
	build/tests/temporaldeck_frame_input_spec \
	build/tests/temporaldeck_platter_input_spec \
	build/tests/temporaldeck_sample_prep_spec \
	build/tests/temporaldeck_virtual_integration_spec \
	build/tests/crownstep_spec \
	build/tests/bifurx_filter_spec \
	build/tests/panel_svg_utils_spec \
	build/tests/crownstep_persistence_spec

CROWNSTEP_MODULE_SOURCES := \
	src/Crownstep.cpp \
	src/CrownstepModule.cpp \
	src/CrownstepPlayback.cpp \
	src/CrownstepSerialization.cpp

.PHONY: test test-build test-odr
test-build: $(TEST_BINS)

test: test-build
	@build/tests/temporaldeck_platter_spec_harness
	@build/tests/temporaldeck_arc_lights_spec
	@build/tests/temporaldeck_engine_spec
	@build/tests/temporaldeck_expander_preview_spec
	@build/tests/temporaldeck_menu_utils_spec
	@build/tests/temporaldeck_frame_input_spec
	@build/tests/temporaldeck_platter_input_spec
	@build/tests/temporaldeck_sample_prep_spec
	@build/tests/temporaldeck_virtual_integration_spec
	@build/tests/crownstep_spec
	@build/tests/bifurx_filter_spec
	@LD_LIBRARY_PATH=$(RACK_DIR):$$LD_LIBRARY_PATH build/tests/panel_svg_utils_spec
	@LD_LIBRARY_PATH=$(RACK_DIR):$$LD_LIBRARY_PATH build/tests/crownstep_persistence_spec
	@$(MAKE) --no-print-directory test-odr

test-odr: plugin.so
	@set -- $$(nm -C --defined-only plugin.so | awk '\
		/ modelIntegralFlux$$/ {mi++} \
		/ modelProc$$/ {mp++} \
		/ modelTemporalDeck$$/ {md++} \
		/ modelCrownstep$$/ {mc++} \
		/ modelTDScope$$/ {mt++} \
		/ T panel_svg::loadRectFromSvgMm\(/ {rh++} \
		/ T panel_svg::loadPointFromSvgMm\(/ {ph++} \
		/ T panel_svg::loadCircleFromSvg\(/ {ch++} \
		END {printf "%d %d %d %d %d %d %d %d", mi+0, mp+0, md+0, mc+0, mt+0, rh+0, ph+0, ch+0}'); \
	model_integralflux_count=$$1; \
	model_proc_count=$$2; \
	model_temporaldeck_count=$$3; \
	model_crownstep_count=$$4; \
	model_tdscope_count=$$5; \
	rect_helper_count=$$6; \
	point_helper_count=$$7; \
	circle_helper_count=$$8; \
	if [ "$$model_integralflux_count" -ne 1 ] || [ "$$model_proc_count" -ne 1 ] || [ "$$model_temporaldeck_count" -ne 1 ] || [ "$$model_crownstep_count" -ne 1 ] || [ "$$model_tdscope_count" -ne 1 ] || [ "$$rect_helper_count" -ne 1 ] || [ "$$point_helper_count" -ne 1 ] || [ "$$circle_helper_count" -ne 1 ]; then \
		echo "[FAIL] ODR/link symbol uniqueness check :: modelIntegralFlux=$$model_integralflux_count modelProc=$$model_proc_count modelTemporalDeck=$$model_temporaldeck_count modelCrownstep=$$model_crownstep_count modelTDScope=$$model_tdscope_count rectHelper=$$rect_helper_count pointHelper=$$point_helper_count circleHelper=$$circle_helper_count"; \
		exit 1; \
	fi; \
	echo "[PASS] ODR/link symbol uniqueness check :: modelIntegralFlux=$$model_integralflux_count modelProc=$$model_proc_count modelTemporalDeck=$$model_temporaldeck_count modelCrownstep=$$model_crownstep_count modelTDScope=$$model_tdscope_count rectHelper=$$rect_helper_count pointHelper=$$point_helper_count circleHelper=$$circle_helper_count"

build/tests:
	@mkdir -p $@

build/tests/temporaldeck_platter_spec_harness: tests/platter_spec_main.cpp tests/platter_spec_cases.cpp tests/platter_trace_replay.cpp | build/tests
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

build/tests/crownstep_spec: tests/crownstep_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/bifurx_filter_spec: tests/bifurx_filter_spec.cpp tests/bifurx_filter_test_model.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $< -o $@

build/tests/panel_svg_utils_spec: tests/panel_svg_utils_spec.cpp src/PanelSvgUtils.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath=/tmp/Rack2 -o $@

build/tests/crownstep_persistence_spec: tests/crownstep_persistence_spec.cpp $(CROWNSTEP_MODULE_SOURCES) | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath=/tmp/Rack2 -o $@
