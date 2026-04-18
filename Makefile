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
	build/tests/bifurx_runtime_spec \
	build/tests/panel_svg_utils_spec \
	build/tests/crownstep_persistence_spec

RACK_TEST_WARN_FLAGS := -Wno-unused-parameter
RACK_TEST_OPT_FLAGS := -O1
CXX_MACHINE := $(shell $(CXX) -dumpmachine 2>/dev/null)
RACK_RUNTIME_DIR := $(abspath $(RACK_DIR))
# Candidate runtime locations for Rack-linked test binaries.
# `RACK_DIR` is primary, while `/tmp/Rack2` is used by some local Rack setups.
RACK_RUNTIME_DIRS := $(RACK_RUNTIME_DIR) /tmp/Rack2

define run_test_bin
	@if [ -x "$(1)" ]; then "$(1)"; \
	elif [ -x "$(1).exe" ]; then \
		if uname -s | grep -qi "linux" && command -v file >/dev/null 2>&1 && file "$(1).exe" | grep -qi "PE32"; then \
			echo "[SKIP] $(1).exe is a Windows test binary; cannot execute in this Linux shell."; \
		else \
			"$(1).exe"; \
		fi; \
	elif [ -f "$(1).exe" ]; then \
		if uname -s | grep -qi "linux" && command -v file >/dev/null 2>&1 && file "$(1).exe" | grep -qi "PE32"; then \
			echo "[SKIP] $(1).exe is a Windows test binary; cannot execute in this Linux shell."; \
		else \
			echo "[FAIL] Test binary exists but is not executable: $(1).exe"; exit 1; \
		fi; \
	else echo "[FAIL] Missing test binary: $(1)"; exit 1; fi
endef

define run_rack_test_bin
	@rack_path="$$PATH"; \
	rack_ld_path="$$LD_LIBRARY_PATH"; \
	for d in $(RACK_RUNTIME_DIRS); do \
		if [ -d "$$d" ]; then \
			rack_path="$$d:$$rack_path"; \
			rack_ld_path="$$d:$$rack_ld_path"; \
		fi; \
	done; \
	run_with_rack_env() { \
		PATH="$$rack_path" LD_LIBRARY_PATH="$$rack_ld_path" "$$1"; \
		rc=$$?; \
		if [ "$$rc" -eq 127 ]; then \
			echo "[FAIL] Rack-linked test could not start (exit 127). Runtime dirs checked: $(RACK_RUNTIME_DIRS)"; \
		fi; \
		return "$$rc"; \
	}; \
	if [ -x "$(1)" ]; then run_with_rack_env "$(1)"; \
	elif [ -x "$(1).exe" ]; then \
		if uname -s | grep -qi "linux" && command -v file >/dev/null 2>&1 && file "$(1).exe" | grep -qi "PE32"; then \
			echo "[SKIP] $(1).exe is a Windows Rack-linked test binary; cannot execute in this Linux shell."; \
		else \
			run_with_rack_env "$(1).exe"; \
		fi; \
	elif [ -f "$(1).exe" ]; then \
		if uname -s | grep -qi "linux" && command -v file >/dev/null 2>&1 && file "$(1).exe" | grep -qi "PE32"; then \
			echo "[SKIP] $(1).exe is a Windows Rack-linked test binary; cannot execute in this Linux shell."; \
		else \
			echo "[FAIL] Rack-linked test binary exists but is not executable: $(1).exe"; exit 1; \
		fi; \
	else echo "[FAIL] Missing Rack-linked test binary: $(1)"; exit 1; fi
endef

CROWNSTEP_MODULE_SOURCES := \
	src/Crownstep.cpp \
	src/CrownstepModule.cpp \
	src/CrownstepPlayback.cpp \
	src/CrownstepSerialization.cpp

.PHONY: test test-build test-odr
test-build: $(TEST_BINS)

test: test-build
	$(call run_test_bin,build/tests/temporaldeck_platter_spec_harness)
	$(call run_test_bin,build/tests/temporaldeck_arc_lights_spec)
	$(call run_test_bin,build/tests/temporaldeck_engine_spec)
	$(call run_test_bin,build/tests/temporaldeck_expander_preview_spec)
	$(call run_test_bin,build/tests/temporaldeck_menu_utils_spec)
	$(call run_test_bin,build/tests/temporaldeck_frame_input_spec)
	$(call run_test_bin,build/tests/temporaldeck_platter_input_spec)
	$(call run_test_bin,build/tests/temporaldeck_sample_prep_spec)
	$(call run_test_bin,build/tests/temporaldeck_virtual_integration_spec)
	$(call run_test_bin,build/tests/crownstep_spec)
	$(call run_test_bin,build/tests/bifurx_filter_spec)
	$(call run_rack_test_bin,build/tests/bifurx_runtime_spec)
	$(call run_rack_test_bin,build/tests/panel_svg_utils_spec)
	$(call run_rack_test_bin,build/tests/crownstep_persistence_spec)
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

build/tests/bifurx_runtime_spec: tests/bifurx_runtime_spec.cpp src/Bifurx.cpp src/PanelSvgUtils.cpp | build/tests
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra -Wno-subobject-linkage $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/bifurx_runtime_spec.cpp src/PanelSvgUtils.cpp -L$(RACK_DIR) -lRack -Wl,-rpath=/tmp/Rack2 -o $@

# Rack-linked tests are heavy C++ translation units under MSYS/MinGW. Chain
# them to avoid concurrent peak-memory spikes when users invoke `make -jN`.
build/tests/panel_svg_utils_spec: tests/panel_svg_utils_spec.cpp src/PanelSvgUtils.cpp | build/tests build/tests/bifurx_runtime_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath=/tmp/Rack2 -o $@

build/tests/crownstep_persistence_spec: tests/crownstep_persistence_spec.cpp $(CROWNSTEP_MODULE_SOURCES) | build/tests build/tests/panel_svg_utils_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath=/tmp/Rack2 -o $@
