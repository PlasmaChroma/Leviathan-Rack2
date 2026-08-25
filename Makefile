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
SOURCES += $(wildcard src/visual/*.cpp)
SOURCES += $(wildcard src/theme/*.cpp)
SOURCES += $(wildcard src/doom/*.c)


# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
RES_FILES := $(shell find res -type f ! -path 'res/icon/*')
RES_EXCLUDES := \
	res/Umi/panel_base@4x.png \
	res/flux.svg \
	res/proc.svg \
	res/deck.svg \
	res/undertow.svg \
	res/bifurx.svg \
	res/Deepcache.svg \
	$(shell find res/panels-source -type f 2>/dev/null)

DISTRIBUTABLES += res/icon
DISTRIBUTABLES += $(filter-out $(RES_EXCLUDES),$(RES_FILES))
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

# Rack SDK 2.5 adds this Clang-only option globally. GCC emits a note for it
# on every translation unit, so remove it after the SDK has assembled FLAGS.
FLAGS := $(filter-out -Wno-vla-extension,$(FLAGS))

# Chocolate Doom is vendored C89-era code. Keep the normal warning policy for
# Leviathan, while suppressing only its documented legacy warning classes.
DOOM_LEGACY_WARN_FLAGS := \
	-Wno-sign-compare \
	-Wno-implicit-fallthrough \
	-Wno-unused-but-set-parameter \
	-Wno-missing-field-initializers \
	-Wno-dangling-pointer \
	-Wno-stringop-truncation \
	-Wno-enum-conversion \
	-Wno-absolute-value

build/src/doom/%.c.o: CFLAGS += $(DOOM_LEGACY_WARN_FLAGS)

# Mandelwake's patch identity depends on exact float-to-fixed adapter rounding.
# Keep global audio optimizations elsewhere, but do not let fast-math rewrite
# its deterministic boundary calculations.
build/src/Mandelwake.cpp.o build/src/MandelwakeEngine.cpp.o: FLAGS += -fno-fast-math -fno-unsafe-math-optimizations

TEST_BINS_NON_RACK := \
	build/tests/octavia_analysis_spec \
	build/tests/octavia_measurement_spec \
	build/tests/octavia_observation_spec \
	build/tests/octavia_job_control_spec \
	build/tests/octavia_action_validation_spec \
	build/tests/octavia_cable_validation_spec \
	build/tests/octavia_console_mailbox_spec \
	build/tests/sibyl_adoption_spec \
	build/tests/sibyl_clock_estimator_spec \
	build/tests/sibyl_hardware_control_spec \
	build/tests/sibyl_edit_spec \
	build/tests/sibyl_json_spec \
	build/tests/sibyl_module_spec \
	build/tests/sibyl_timing_spec \
	build/tests/sibyl_transport_spec \
	build/tests/theme_service_spec \
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
	build/tests/mandelwake_engine_spec \
	build/tests/undertow_shape_spec \
	build/tests/math_helpers_spec \
	build/tests/puffy_engine_spec \
	build/tests/puffy_module_spec \
	build/tests/puffy_character_controller_spec \
	build/tests/cantor_culture_engine_spec \
	build/tests/cantor_module_spec \
	build/tests/wyrm_envelope_spec \
	build/tests/doorstop_engine_spec \
	build/tests/doorstop_reference_engine_spec \
	build/tests/bifurx_filter_spec \
	build/tests/sil_repair_spec \
	build/tests/bulkhead_geometry_spec \
	build/tests/umi_engine_spec \
	build/tests/aperture_light_transfer_spec \
	build/tests/iris_wavetable_spec \
	build/tests/nautiloid_location_code_spec \
	build/tests/wave_preview_simplification_spec \
	build/tests/deepcache_planner_spec \
	build/tests/deepcache_archive_spec \
	build/tests/chromatide_spec \
	build/tests/temporaldeck_longplay_spec


TEST_BINS_RACK := \
	build/tests/bifurx_runtime_spec \
	build/tests/panel_svg_utils_spec \
	build/tests/crownstep_persistence_spec \
	build/tests/doorstop_runtime_spec

RUN_CHRONOMAW_WIP_TESTS ?= 0
ifeq ($(RUN_CHRONOMAW_WIP_TESTS),1)
TEST_BINS_RACK += build/tests/chronomaw_serialization_spec
endif

TEST_BINS := $(TEST_BINS_NON_RACK) $(TEST_BINS_RACK)

RACK_TEST_WARN_FLAGS := -Wno-unused-parameter
RACK_TEST_OPT_FLAGS := -O1
CXX_MACHINE := $(shell $(CXX) -dumpmachine 2>/dev/null)
MINGW_TEST_CPPFLAGS :=
ifneq (,$(findstring mingw,$(CXX_MACHINE)))
MINGW_TEST_CPPFLAGS += -D_USE_MATH_DEFINES
endif

.PHONY: generate-panel-anchor-atlas generate-mandelwake-tables check-mandelwake-tables validate-plugin-json doorstop-reference-grid doorstop-corpus-audit doorstop-reference-evaluate doorstop-variant-grid doorstop-variant-evaluate doorstop-boing-audition
generate-panel-anchor-atlas:
	python3 tools/generate_panel_anchor_atlas.py

generate-mandelwake-tables:
	python3 tools/generate_mandelwake_tables.py

check-mandelwake-tables:
	python3 tools/generate_mandelwake_tables.py --check

validate-plugin-json:
	python3 tools/validate_plugin_json_tags.py plugin.json

build/tools/doorstop_reference_render: tools/doorstop_reference_render.cpp src/ReferenceSpringEngine.cpp src/ReferenceSpringEngine.hpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 -O2 -Wall -Wextra -DDOORSTOP_REFERENCE_ANALYSIS=1 tools/doorstop_reference_render.cpp src/ReferenceSpringEngine.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

DOORSTOP_REFERENCE_VELOCITIES ?= 0.5 0.75 1.0
DOORSTOP_REFERENCE_SEEDS ?= 1 77 7331 65537 104729 999983 2654435761 305419896 610839776 195948557 271828183 314159265 3735928559 324508639 4277009102 4294967291
DOORSTOP_REFERENCE_VARIANTS ?= current spring-only modes-only spring-forward spring-refined rack-v2 boing-refined
DOORSTOP_BOING_AUDITION_DIR ?= Samples/Doorstop/Auditions/reference-v2-vs-boing-refined

doorstop-reference-grid: build/tools/doorstop_reference_render
	mkdir -p build/doorstop-reference-renders
	@for velocity in $(DOORSTOP_REFERENCE_VELOCITIES); do \
		for seed in $(DOORSTOP_REFERENCE_SEEDS); do \
			name=build/doorstop-reference-renders/reference-v$${velocity}-seed$${seed}.wav; \
			build/tools/doorstop_reference_render $$name \
				--quiet --velocity $$velocity --seed $$seed; \
		done; \
	done

doorstop-corpus-audit:
	python3 tools/audit_doorstop_corpus.py

doorstop-reference-evaluate: doorstop-reference-grid
	python3 tools/audit_doorstop_corpus.py \
		--model-dir build/doorstop-reference-renders

doorstop-variant-grid: build/tools/doorstop_reference_render
	@for variant in $(DOORSTOP_REFERENCE_VARIANTS); do \
		mkdir -p build/doorstop-variant-renders/$$variant; \
		for velocity in $(DOORSTOP_REFERENCE_VELOCITIES); do \
			for seed in $(DOORSTOP_REFERENCE_SEEDS); do \
				name=build/doorstop-variant-renders/$$variant/$$variant-v$${velocity}-seed$${seed}.wav; \
				build/tools/doorstop_reference_render $$name \
					--quiet --variant $$variant --velocity $$velocity --seed $$seed; \
			done; \
		done; \
	done

doorstop-variant-evaluate: doorstop-variant-grid
	python3 tools/compare_doorstop_variants.py

# Keep the large population renders disposable, but preserve the compact,
# level-matched listening decision outside build/ so normal cleans do not
# erase it.
doorstop-boing-audition: build/tools/doorstop_reference_render
	$(MAKE) DOORSTOP_REFERENCE_VARIANTS="current rack-v2 boing-refined" doorstop-variant-grid
	python3 tools/compare_doorstop_variants.py \
		--variants current rack-v2 boing-refined \
		--output-dir $(DOORSTOP_BOING_AUDITION_DIR)

ifneq (,$(findstring mingw,$(CXX_MACHINE)))
LDFLAGS += -lws2_32
LDFLAGS += -lopengl32
endif

RACK_RUNTIME_DIR := $(abspath $(RACK_DIR))
# Optional extra runtime directory for libRack.dll (e.g. /c/Program Files/VCV/Rack2Pro).
# Keep this as a single directory path; pass it at invocation time if needed:
#   make test RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
RACK_APP_RUNTIME_DIR ?=
# Candidate runtime locations for Rack-linked test binaries.
# `RACK_DIR` is primary, while `/tmp/Rack2` is used by some local Rack setups.
# Include Rack dependency folders so MSYS2 can resolve transitive DLL/SO deps.
RACK_RUNTIME_DIRS := \
	$(RACK_RUNTIME_DIR) \
	$(RACK_RUNTIME_DIR)/dep/lib \
	$(RACK_RUNTIME_DIR)/dep/bin \
	/tmp/Rack2 \
	/tmp/Rack2/dep/lib \
	/tmp/Rack2/dep/bin \
	/mingw64/bin \
	/ucrt64/bin \
	/clang64/bin \
	/mingw32/bin

define run_test_bin
	@run_with_test_env() { \
		DYLD_LIBRARY_PATH="$(RACK_RUNTIME_DIR):$$DYLD_LIBRARY_PATH" "$$1"; \
	}; \
	if [ -x "$(1)" ]; then run_with_test_env "$(1)"; \
	elif [ -x "$(1).exe" ]; then \
		if uname -s | grep -qi "linux" && command -v file >/dev/null 2>&1 && file "$(1).exe" | grep -qi "PE32"; then \
			echo "[SKIP] $(1).exe is a Windows test binary; cannot execute in this Linux shell."; \
		else \
			run_with_test_env "$(1).exe"; \
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
	if [ -n "$(RACK_APP_RUNTIME_DIR)" ] && [ -d "$(RACK_APP_RUNTIME_DIR)" ]; then \
		rack_path="$(RACK_APP_RUNTIME_DIR):$$rack_path"; \
		rack_ld_path="$(RACK_APP_RUNTIME_DIR):$$rack_ld_path"; \
	fi; \
	run_with_rack_env() { \
		PATH="$$rack_path" LD_LIBRARY_PATH="$$rack_ld_path" "$$1"; \
		rc=$$?; \
		if [ "$$rc" -eq 127 ]; then \
			echo "[FAIL] Rack-linked test could not start (exit 127). Runtime dirs checked: $(RACK_RUNTIME_DIRS)"; \
			if command -v ldd >/dev/null 2>&1; then \
				echo "[INFO] ldd unresolved dependencies for $$1:"; \
				ldd "$$1" 2>/dev/null | grep -i "not found" || echo "[INFO] ldd found no unresolved dependencies (or could not inspect this binary)."; \
			fi; \
			if command -v cygcheck >/dev/null 2>&1; then \
				echo "[INFO] cygcheck unresolved dependencies for $$1:"; \
				cygcheck "$$1" 2>/dev/null | grep -i "not found" || echo "[INFO] cygcheck found no unresolved dependencies (or could not inspect this binary)."; \
			fi; \
			if command -v ntldd >/dev/null 2>&1; then \
				echo "[INFO] ntldd unresolved dependencies for $$1:"; \
				ntldd -R "$$1" 2>/dev/null | grep -i "not found" || echo "[INFO] ntldd found no unresolved dependencies (or could not inspect this binary)."; \
			fi; \
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
	src/CrownstepSerialization.cpp \
	src/DebugTerminalTransport.cpp

.PHONY: test test-fast test-rack test-build test-build-fast test-build-rack test-odr test-sibyl-tsan test-octavia-observation-tsan test-octavia-measurement-tsan
test-build: $(TEST_BINS)
test-build-fast: $(TEST_BINS_NON_RACK)
test-build-rack: $(TEST_BINS_RACK)

test-sibyl-tsan: build/tests/sibyl_module_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 \
	LD_LIBRARY_PATH="$(RACK_RUNTIME_DIR):$$LD_LIBRARY_PATH" \
	build/tests/sibyl_module_tsan_spec

test-octavia-observation-tsan: build/tests/octavia_observation_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 build/tests/octavia_observation_tsan_spec

test-octavia-measurement-tsan: build/tests/octavia_measurement_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 build/tests/octavia_measurement_tsan_spec

test-fast: test-build-fast
	$(call run_test_bin,build/tests/octavia_analysis_spec)
	$(call run_test_bin,build/tests/octavia_measurement_spec)
	$(call run_test_bin,build/tests/octavia_observation_spec)
	$(call run_test_bin,build/tests/octavia_job_control_spec)
	$(call run_test_bin,build/tests/octavia_action_validation_spec)
	$(call run_test_bin,build/tests/octavia_cable_validation_spec)
	$(call run_test_bin,build/tests/octavia_console_mailbox_spec)
	$(call run_test_bin,build/tests/sibyl_adoption_spec)
	$(call run_test_bin,build/tests/sibyl_clock_estimator_spec)
	$(call run_test_bin,build/tests/sibyl_hardware_control_spec)
	$(call run_rack_test_bin,build/tests/sibyl_edit_spec)
	$(call run_rack_test_bin,build/tests/sibyl_json_spec)
	$(call run_rack_test_bin,build/tests/sibyl_module_spec)
	$(call run_test_bin,build/tests/sibyl_timing_spec)
	$(call run_rack_test_bin,build/tests/sibyl_transport_spec)
	python3 tests/octavia_sibyl_contract_spec.py
	python3 tests/octavia_monitoring_panel_contract_spec.py
	python3 tests/split_svg_labels_spec.py
	python3 tools/generate_mandelwake_tables.py --check
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
	$(call run_test_bin,build/tests/mandelwake_engine_spec)
	$(call run_test_bin,build/tests/undertow_shape_spec)
	$(call run_test_bin,build/tests/math_helpers_spec)
	$(call run_test_bin,build/tests/puffy_engine_spec)
	$(call run_rack_test_bin,build/tests/puffy_module_spec)
	$(call run_rack_test_bin,build/tests/puffy_character_controller_spec)
	$(call run_test_bin,build/tests/cantor_culture_engine_spec)
	$(call run_rack_test_bin,build/tests/cantor_module_spec)
	$(call run_rack_test_bin,build/tests/wyrm_envelope_spec)
	$(call run_test_bin,build/tests/doorstop_engine_spec)
	$(call run_test_bin,build/tests/doorstop_reference_engine_spec)
	$(call run_test_bin,build/tests/bifurx_filter_spec)
	$(call run_test_bin,build/tests/sil_repair_spec)
	$(call run_test_bin,build/tests/bulkhead_geometry_spec)
	$(call run_test_bin,build/tests/umi_engine_spec)
	$(call run_test_bin,build/tests/aperture_light_transfer_spec)
	$(call run_test_bin,build/tests/iris_wavetable_spec)
	$(call run_test_bin,build/tests/nautiloid_location_code_spec)
	$(call run_test_bin,build/tests/wave_preview_simplification_spec)
	$(call run_test_bin,build/tests/deepcache_planner_spec)
	$(call run_test_bin,build/tests/deepcache_archive_spec)
	$(call run_rack_test_bin,build/tests/chromatide_spec)
	$(call run_rack_test_bin,build/tests/temporaldeck_longplay_spec)

test-rack: test-build-rack
	$(call run_rack_test_bin,build/tests/bifurx_runtime_spec)
ifeq ($(RUN_CHRONOMAW_WIP_TESTS),1)
	$(call run_rack_test_bin,build/tests/chronomaw_serialization_spec)
else
	@echo "[SKIP] chronomaw_serialization_spec (Chronomaw WIP; set RUN_CHRONOMAW_WIP_TESTS=1 to run)"
endif
	$(call run_rack_test_bin,build/tests/panel_svg_utils_spec)
	$(call run_rack_test_bin,build/tests/crownstep_persistence_spec)
	$(call run_rack_test_bin,build/tests/doorstop_runtime_spec)

test:
	@fast_rc=0; \
	rack_rc=0; \
	odr_rc=0; \
	$(MAKE) --no-print-directory test-fast || fast_rc=$$?; \
	$(MAKE) --no-print-directory test-rack || rack_rc=$$?; \
	$(MAKE) --no-print-directory test-odr || odr_rc=$$?; \
	failed=0; \
	if [ "$$fast_rc" -eq 0 ]; then fast_status="PASS"; else fast_status="FAIL"; failed=$$((failed + 1)); fi; \
	if [ "$$rack_rc" -eq 0 ]; then rack_status="PASS"; else rack_status="FAIL"; failed=$$((failed + 1)); fi; \
	if [ "$$odr_rc" -eq 0 ]; then odr_status="PASS"; else odr_status="FAIL"; failed=$$((failed + 1)); fi; \
	passed=$$((3 - failed)); \
	echo "--------------------------------"; \
	echo "[TEST SUMMARY] targets=3 passed=$$passed failed=$$failed"; \
	echo "[TEST SUMMARY] test-fast=$$fast_status"; \
	echo "[TEST SUMMARY] test-rack=$$rack_status"; \
	echo "[TEST SUMMARY] test-odr=$$odr_status"; \
	echo "--------------------------------"; \
	if [ "$$failed" -ne 0 ]; then exit 1; fi

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

build/tests/octavia_console_mailbox_spec: tests/octavia_console_mailbox_spec.cpp src/OctaviaConsoleMailbox.cpp src/OctaviaConsoleMailbox.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_console_mailbox_spec.cpp src/OctaviaConsoleMailbox.cpp -o $@

build/tests/octavia_cable_validation_spec: tests/octavia_cable_validation_spec.cpp src/OctaviaCableValidation.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/octavia_cable_validation_spec.cpp -o $@

build/tests/octavia_action_validation_spec: tests/octavia_action_validation_spec.cpp src/OctaviaActionValidation.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/octavia_action_validation_spec.cpp -o $@

build/tests/octavia_job_control_spec: tests/octavia_job_control_spec.cpp src/OctaviaJobControl.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_job_control_spec.cpp -o $@

build/tests/octavia_observation_spec: tests/octavia_observation_spec.cpp src/OctaviaObservation.cpp src/OctaviaObservation.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_observation_spec.cpp src/OctaviaObservation.cpp -o $@

build/tests/octavia_analysis_spec: tests/octavia_analysis_spec.cpp src/OctaviaAnalysis.cpp src/OctaviaAnalysis.hpp src/OctaviaObservation.cpp src/OctaviaObservation.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_analysis_spec.cpp src/OctaviaAnalysis.cpp src/OctaviaObservation.cpp -o $@

build/tests/octavia_measurement_spec: tests/octavia_measurement_spec.cpp src/OctaviaMeasurement.cpp src/OctaviaMeasurement.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_measurement_spec.cpp src/OctaviaMeasurement.cpp -o $@

build/tests/octavia_measurement_tsan_spec: tests/octavia_measurement_spec.cpp src/OctaviaMeasurement.cpp src/OctaviaMeasurement.hpp | build/tests
	$(CXX) -std=c++17 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer -Isrc tests/octavia_measurement_spec.cpp src/OctaviaMeasurement.cpp -o $@

build/tests/octavia_observation_tsan_spec: tests/octavia_observation_spec.cpp src/OctaviaObservation.cpp src/OctaviaObservation.hpp | build/tests
	$(CXX) -std=c++17 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer -Isrc tests/octavia_observation_spec.cpp src/OctaviaObservation.cpp -o $@

build/tests/temporaldeck_platter_spec_harness: tests/platter_spec_main.cpp tests/platter_spec_cases.cpp tests/platter_trace_replay.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/sibyl_json_spec: tests/sibyl_json_spec.cpp src/SibylJSON.cpp src/SibylJSON.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_json_spec.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_adoption_spec: tests/sibyl_adoption_spec.cpp src/SibylAdoption.cpp src/SibylAdoption.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_adoption_spec.cpp src/SibylAdoption.cpp -o $@

build/tests/sibyl_clock_estimator_spec: tests/sibyl_clock_estimator_spec.cpp src/SibylClockEstimator.cpp src/SibylClockEstimator.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_clock_estimator_spec.cpp src/SibylClockEstimator.cpp -o $@

build/tests/sibyl_hardware_control_spec: tests/sibyl_hardware_control_spec.cpp src/SibylHardwareControl.cpp src/SibylHardwareControl.hpp src/SibylAdoption.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_hardware_control_spec.cpp src/SibylHardwareControl.cpp -o $@

build/tests/sibyl_edit_spec: tests/sibyl_edit_spec.cpp src/SibylEdit.cpp src/SibylEdit.hpp src/SibylJSON.cpp src/SibylJSON.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_edit_spec.cpp src/SibylEdit.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_transport_spec: tests/sibyl_transport_spec.cpp src/SibylTransport.cpp src/SibylTransport.hpp src/SibylAdoption.cpp src/SibylAdoption.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_transport_spec.cpp src/SibylTransport.cpp src/SibylAdoption.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_timing_spec: tests/sibyl_timing_spec.cpp src/SibylTiming.cpp src/SibylTiming.hpp src/SibylTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_timing_spec.cpp src/SibylTiming.cpp -o $@

build/tests/sibyl_module_spec: tests/sibyl_module_spec.cpp src/Sibyl.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_module_spec.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_module_tsan_spec: tests/sibyl_module_spec.cpp src/Sibyl.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp | build/tests
	$(CXX) -std=c++17 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_module_spec.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

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

build/tests/sil_repair_spec: tests/sil_repair_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) $^ -o $@

build/tests/bulkhead_geometry_spec: tests/bulkhead_geometry_spec.cpp src/BulkheadGeometry.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/umi_engine_spec: tests/umi_engine_spec.cpp src/UmiEngine.cpp src/UmiLayout.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/aperture_light_transfer_spec: tests/aperture_light_transfer_spec.cpp src/visual/ApertureLightTransfer.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $< -o $@

build/tests/iris_wavetable_spec: tests/iris_wavetable_spec.cpp src/IrisWavetable.hpp src/IrisPolyphony.hpp src/IrisIO.cpp src/IrisIO.hpp src/IrisSourceField.cpp src/IrisSourceField.hpp | build/tests
	$(CXX) -std=c++11 -O3 -Wall -Wextra -I$(RACK_DIR)/dep/include tests/iris_wavetable_spec.cpp src/IrisIO.cpp src/IrisSourceField.cpp -o $@

build/tests/nautiloid_location_code_spec: tests/nautiloid_location_code_spec.cpp src/NautiloidLocationCode.cpp src/NautiloidLocationCode.hpp src/NautiloidFractal.hpp | build/tests
	$(CXX) -std=c++17 -O3 -Wall -Wextra tests/nautiloid_location_code_spec.cpp src/NautiloidLocationCode.cpp -o $@

build/tests/temporaldeck_virtual_integration_spec: tests/temporaldeck_virtual_integration_spec.cpp src/TemporalDeckPlatterInput.cpp src/TemporalDeckTransportControl.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/crownstep_spec: tests/crownstep_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/mandelwake_engine_spec: tests/mandelwake_engine_spec.cpp src/MandelwakeEngine.cpp src/MandelwakeEngine.hpp src/MandelwakeFixedPoint.hpp src/MandelwakeTables.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/mandelwake_engine_spec.cpp src/MandelwakeEngine.cpp -o $@

build/tests/undertow_shape_spec: tests/undertow_shape_spec.cpp src/UndertowShape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/undertow_shape_spec.cpp -o $@

build/tests/math_helpers_spec: tests/math_helpers_spec.cpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/math_helpers_spec.cpp src/MathHelpers.cpp -o $@

build/tests/puffy_engine_spec: tests/puffy_engine_spec.cpp src/PuffyEngine.cpp src/PuffyEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/puffy_engine_spec.cpp src/PuffyEngine.cpp src/MathHelpers.cpp -o $@

build/tests/puffy_module_spec: tests/puffy_module_spec.cpp src/Puffy.cpp src/Puffy.hpp src/PuffyEngine.cpp src/PuffyEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/puffy_module_spec.cpp src/Puffy.cpp src/PuffyEngine.cpp src/MathHelpers.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/puffy_character_controller_spec: tests/puffy_character_controller_spec.cpp src/PuffyCharacterController.cpp src/PuffyCharacterController.hpp src/PuffyPose.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/puffy_character_controller_spec.cpp src/PuffyCharacterController.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/cantor_culture_engine_spec: tests/cantor_culture_engine_spec.cpp src/CantorCultureEngine.cpp src/CantorCultureEngine.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/cantor_culture_engine_spec.cpp src/CantorCultureEngine.cpp -o $@

build/tests/theme_service_spec: tests/theme_service_spec.cpp src/theme/ThemeService.cpp src/theme/ThemeService.hpp src/theme/ThemeTypes.hpp src/theme/ThemePresets.cpp src/theme/ThemePresets.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/theme_service_spec.cpp src/theme/ThemeService.cpp src/theme/ThemePresets.cpp -o $@

build/tests/cantor_module_spec: tests/cantor_module_spec.cpp src/Cantor.cpp src/Cantor.hpp src/CantorCultureEngine.cpp src/CantorCultureEngine.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/cantor_module_spec.cpp src/Cantor.cpp src/CantorCultureEngine.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/wyrm_envelope_spec: tests/wyrm_envelope_spec.cpp src/Wyrm.cpp src/Wyrm.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/wyrm_envelope_spec.cpp src/Wyrm.cpp src/MathHelpers.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/temporaldeck_longplay_spec: tests/temporaldeck_longplay_spec.cpp src/LongPlayStreamEngine.cpp src/LongPlayStreamEngine.hpp src/codec.cpp src/codec.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/temporaldeck_longplay_spec.cpp src/LongPlayStreamEngine.cpp src/codec.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -pthread -o $@

build/tests/doorstop_engine_spec: tests/doorstop_engine_spec.cpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/doorstop_engine_spec.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

build/tests/doorstop_reference_engine_spec: tests/doorstop_reference_engine_spec.cpp src/ReferenceSpringEngine.cpp src/ReferenceSpringEngine.hpp src/DoorstopEngineRouter.cpp src/DoorstopEngineRouter.hpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/doorstop_reference_engine_spec.cpp src/ReferenceSpringEngine.cpp src/DoorstopEngineRouter.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

build/tests/bifurx_filter_spec: tests/bifurx_filter_spec.cpp tests/bifurx_filter_test_model.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $< -o $@

build/tests/wave_preview_simplification_spec: tests/wave_preview_simplification_spec.cpp src/WavePreviewSimplifier.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) tests/wave_preview_simplification_spec.cpp -o $@

build/tests/deepcache_planner_spec: tests/deepcache_planner_spec.cpp src/DeepcachePlanner.cpp src/DeepcachePlanner.hpp src/DeepcacheBrowserLogic.cpp src/DeepcacheBrowserLogic.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread tests/deepcache_planner_spec.cpp src/DeepcachePlanner.cpp src/DeepcacheBrowserLogic.cpp -o $@

build/tests/deepcache_archive_spec: tests/deepcache_archive_spec.cpp src/DeepcacheArchive.cpp src/DeepcacheArchive.hpp src/DeepcacheQoi.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/deepcache_archive_spec.cpp src/DeepcacheArchive.cpp src/DeepcacheQoi.cpp -o $@

build/tests/chromatide_spec: tests/chromatide_spec.cpp src/ChromatideCanvas.cpp src/Chromatide.cpp src/IrisSourceField.cpp src/DeepcacheQoi.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chromatide_spec.cpp src/ChromatideCanvas.cpp src/Chromatide.cpp src/IrisSourceField.cpp src/DeepcacheQoi.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/bifurx_runtime_spec: tests/bifurx_runtime_spec.cpp src/Bifurx.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp | build/tests
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra -Wno-subobject-linkage $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/bifurx_runtime_spec.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

build/tests/chronomaw_serialization_spec: tests/chronomaw_serialization_spec.cpp src/Chronomaw.cpp src/ChronomawEngine.cpp | build/tests
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chronomaw_serialization_spec.cpp src/Chronomaw.cpp src/ChronomawEngine.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

# Rack-linked tests are heavy C++ translation units under MSYS/MinGW. Chain
# them to avoid concurrent peak-memory spikes when users invoke `make -jN`.
build/tests/panel_svg_utils_spec: tests/panel_svg_utils_spec.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp | build/tests build/tests/bifurx_runtime_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

build/tests/crownstep_persistence_spec: tests/crownstep_persistence_spec.cpp $(CROWNSTEP_MODULE_SOURCES) | build/tests build/tests/panel_svg_utils_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

build/tests/doorstop_runtime_spec: tests/doorstop_runtime_spec.cpp src/Doorstop.cpp src/DoorstopEngine.cpp src/DoorstopEngineRouter.cpp src/ReferenceSpringEngine.cpp src/MathHelpers.cpp | build/tests build/tests/panel_svg_utils_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@
