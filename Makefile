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
SOURCES += src/render/HostStrokeBridge.cpp
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

# Premium validation uses Pro's bootstrap and identity with the CURRENT shared
# Bifurx sources. Nothing is installed or written into the Pro source checkout.
PREMIUM_PRO_DIR ?= ../Leviathan-Pro
PREMIUM_DRM_DIR ?= $(PREMIUM_PRO_DIR)/DRM
.PHONY: premium-prepare premium-build premium-dist test-premium
premium-prepare:
	python3 tools/prepare_bifurx_premium.py --pro-root "$(PREMIUM_PRO_DIR)" --drm-dir "$(PREMIUM_DRM_DIR)" --rack-dir "$(RACK_DIR)"

premium-build: premium-prepare
	$(MAKE) -C build/premium-validation RACK_DIR="$(abspath $(RACK_DIR))" DRM_DIR="$(abspath $(PREMIUM_DRM_DIR))" all

premium-dist: premium-prepare
	$(MAKE) -C build/premium-validation RACK_DIR="$(abspath $(RACK_DIR))" DRM_DIR="$(abspath $(PREMIUM_DRM_DIR))" dist

# Deliberately outside test-fast: normal development does not require DRM.
test-premium: premium-prepare
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) $(if $(ARCH_X64),-march=nehalem,) $(RACK_TEST_WARN_FLAGS) -Wno-subobject-linkage -DLEVIATHAN_PRO_DRM=1 -I"$(PREMIUM_DRM_DIR)" -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/bifurx_license_spec.cpp src/BifurxDisplay.cpp src/BifurxPreview.cpp src/BifurxState.cpp src/BifurxRenderClient.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o build/premium-validation/bifurx_license_spec
	$(call run_rack_test_bin,build/premium-validation/bifurx_license_spec)

# Rack SDK 2.5 adds this Clang-only option globally. GCC emits a note for it
# on every translation unit, so remove it after the SDK has assembled FLAGS.
FLAGS := $(filter-out -Wno-vla-extension,$(FLAGS))

# Mandelwake's patch identity depends on exact float-to-fixed adapter rounding.
# Keep global audio optimizations elsewhere, but do not let fast-math rewrite
# its deterministic boundary calculations.
build/src/Mandelwake.cpp.o build/src/MandelwakeEngine.cpp.o: FLAGS += -fno-fast-math -fno-unsafe-math-optimizations
build/src/Chimera.cpp.o: FLAGS += -fno-fast-math -fno-unsafe-math-optimizations

TEST_BINS_NON_RACK := \
	build/tests/debug_terminal_timing_spec \
	build/tests/review_state_handoff_spec \
	build/tests/halo_metrics_scope_spec \
	build/tests/chromatide_qoi_preflight_spec \
	build/tests/octavia_observation_bus_spec \
	build/tests/octavia_analysis_spec \
	build/tests/octavia_observation_spec \
	build/tests/octavia_recording_spec \
	build/tests/octavia_job_control_spec \
	build/tests/octavia_server_lifecycle_spec \
	build/tests/octavia_semantic_control_spec \
	build/tests/octavia_action_validation_spec \
	build/tests/octavia_cable_validation_spec \
	build/tests/octavia_presence_spec \
	build/tests/octavia_presence_routes_spec \
	build/tests/octavia_console_mailbox_spec \
	build/tests/sibyl_evolution_spec \
	build/tests/sibyl_codec_spec \
	build/tests/sibyl_note_edit_spec \
	build/tests/sibyl_legacy_golden_spec \
	build/tests/sibyl_adoption_spec \
	build/tests/sibyl_clock_estimator_spec \
	build/tests/sibyl_hardware_control_spec \
	build/tests/sibyl_edit_spec \
	build/tests/sibyl_composer_validation \
	build/tests/sibyl_json_spec \
	build/tests/sibyl_module_spec \
	build/tests/sibyl_timing_spec \
	build/tests/sibyl_transport_spec \
	build/tests/moirai_curves_spec \
	build/tests/moirai_adoption_spec \
	build/tests/moirai_compiler_spec \
	build/tests/moirai_edit_spec \
	build/tests/moirai_json_spec \
	build/tests/moirai_engine_spec \
	build/tests/moirai_module_spec \
	build/tests/theme_service_spec \
	build/tests/theme_persistence_spec \
	build/tests/temporaldeck_platter_spec_harness \
	build/tests/temporaldeck_arc_lights_spec \
	build/tests/temporaldeck_engine_spec \
	build/tests/temporaldeck_expander_preview_spec \
	build/tests/spsc_latest_snapshot_spec \
	build/tests/shared_svg_cache_spec \
	build/tests/nvg_graphics_lifecycle_spec \
	build/tests/temporaldeck_menu_utils_spec \
	build/tests/temporaldeck_frame_input_spec \
	build/tests/temporaldeck_platter_input_spec \
	build/tests/temporaldeck_sample_prep_spec \
	build/tests/temporaldeck_virtual_integration_spec \
	build/tests/crownstep_spec \
	build/tests/mandelwake_engine_spec \
	build/tests/undertow_shape_spec \
	build/tests/undertow_module_spec \
	build/tests/math_helpers_spec \
	build/tests/puffy_engine_spec \
	build/tests/puffy_module_spec \
	build/tests/puffy_character_controller_spec \
	build/tests/cantor_culture_engine_spec \
	build/tests/cantor_module_spec \
	build/tests/wyrm_envelope_spec \
	build/tests/doorstop_engine_spec \
	build/tests/doorstop_reference_engine_spec \
	build/tests/doorstop_helical_engine_spec \
	build/tests/$(ARCH_NAME)/bifurx_filter_spec$(if $(ARCH_WIN),.exe,) \
	build/tests/$(ARCH_NAME)/bifurx_runtime_spec$(if $(ARCH_WIN),.exe,) \
	build/tests/sil_repair_spec \
	build/tests/sil_limiter_peak_window_spec \
	build/tests/bulkhead_geometry_spec \
	build/tests/umi_engine_spec \
	build/tests/aperture_light_transfer_spec \
	build/tests/iris_wavetable_spec \
	build/tests/iris_worker_completion_spec \
	build/tests/iris_module_phase4_spec \
	build/tests/nautiloid_request_coordinator_spec \
	build/tests/nautiloid_location_code_spec \
	build/tests/nautiloid_gpu_precision_spec \
	build/tests/nautiloid_iris_restore_spec \
	build/tests/integral_flux_runtime_spec \
	build/tests/legacy_curve_preview_spec \
	build/tests/proc_runtime_spec \
	build/tests/wave_preview_simplification_spec \
	build/tests/deepcache_planner_spec \
	build/tests/deepcache_archive_spec \
	build/tests/deepcache_theme_classifier_spec \
	build/tests/chromatide_spec \
	build/tests/phonex_engine_spec \
	build/tests/temporaldeck_longplay_spec


TEST_BINS_RACK := \
	build/tests/panel_svg_utils_spec \
	build/tests/crownstep_persistence_spec \
	build/tests/doorstop_runtime_spec \
	build/tests/phonex_module_spec

RUN_CHRONOMAW_WIP_TESTS ?= 0
ifeq ($(RUN_CHRONOMAW_WIP_TESTS),1)
TEST_BINS_RACK += build/tests/chronomaw_serialization_spec
endif

TEST_BINS := $(TEST_BINS_NON_RACK) $(TEST_BINS_RACK)

RACK_TEST_WARN_FLAGS := -Wno-unused-parameter
RACK_TEST_OPT_FLAGS := -O1
INTEGRAL_FLUX_TEST_OPT_FLAGS := -O3 -funsafe-math-optimizations -fno-omit-frame-pointer
ifdef ARCH_X64
INTEGRAL_FLUX_TEST_OPT_FLAGS += -march=nehalem
endif
ifdef ARCH_ARM64
INTEGRAL_FLUX_TEST_OPT_FLAGS += -march=armv8-a+fp+simd
endif
CXX_MACHINE := $(shell $(CXX) -dumpmachine 2>/dev/null)
MINGW_TEST_CPPFLAGS :=
MINGW_TEST_STACK_FLAGS :=
ifneq (,$(findstring mingw,$(CXX_MACHINE)))
MINGW_TEST_CPPFLAGS += -D_USE_MATH_DEFINES
# The Phonex corpus contract deliberately compares several fixed-capacity
# sequences at once. Windows' default 2 MiB stack is too small for that
# test-only fixture set; the Rack module itself owns its sequences on the heap.
MINGW_TEST_STACK_FLAGS += -Wl,--stack,8388608
endif

.PHONY: generate-panel-anchor-atlas generate-mandelwake-tables check-mandelwake-tables validate-plugin-json phonex-quality-audit phonex-quality-compare phonex-phase2-audition phonex-phase4-audition phonex-phase8-audition doorstop-reference-grid doorstop-corpus-audit doorstop-reference-evaluate doorstop-variant-grid doorstop-variant-evaluate doorstop-boing-audition doorstop-v3-boing-audition doorstop-v2-phase-grid doorstop-v2-phase-evaluate
generate-panel-anchor-atlas:
	python3 tools/generate_panel_anchor_atlas.py

generate-mandelwake-tables:
	python3 tools/generate_mandelwake_tables.py

check-mandelwake-tables:
	python3 tools/generate_mandelwake_tables.py --check

validate-plugin-json:
	python3 tools/validate_plugin_json_tags.py plugin.json

PHONEX_QUALITY_TAG ?= working
PHONEX_QUALITY_VERIFY ?= 1
PHONEX_QUALITY_RENDERER_ARGS ?=
PHONEX_QUALITY_BASELINE ?= q0-complete
PHONEX_QUALITY_CANDIDATE ?= working
PHONEX_QUALITY_VERIFY_FLAG = $(if $(filter 1,$(PHONEX_QUALITY_VERIFY)),--verify-determinism,)

phonex-quality-audit: build/tools/phonex_render build/tools/phonex_corpus_report
	python3 tools/phonex_quality_audit.py \
		--renderer build/tools/phonex_render \
		--corpus-report build/tools/phonex_corpus_report \
		--tag "$(PHONEX_QUALITY_TAG)" $(PHONEX_QUALITY_VERIFY_FLAG) $(foreach arg,$(PHONEX_QUALITY_RENDERER_ARGS),--renderer-arg=$(arg))

phonex-quality-compare:
	python3 tools/phonex_quality_compare.py \
		"$(PHONEX_QUALITY_BASELINE)" "$(PHONEX_QUALITY_CANDIDATE)"

phonex-phase2-audition:
	$(MAKE) --no-print-directory phonex-quality-audit PHONEX_QUALITY_TAG=phase2-audition

phonex-phase4-audition:
	$(MAKE) --no-print-directory phonex-quality-audit PHONEX_QUALITY_TAG=phase4-audition

phonex-phase8-audition:
	$(MAKE) --no-print-directory phonex-quality-audit PHONEX_QUALITY_TAG=phase8-audition

build/tools/phonex_render: tools/phonex_render.cpp src/PhonexEngine.cpp src/PhonexEngine.hpp src/PhonexFixtures.cpp src/PhonexFixtures.hpp src/PhonexTypes.hpp src/PhonexRom.cpp src/PhonexRom.hpp src/PhonexRomData.inc src/PhonexSequenceCompiler.cpp src/PhonexSequenceCompiler.hpp src/PhonexPronunciation.cpp src/PhonexPronunciation.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tools/phonex_render.cpp src/PhonexEngine.cpp src/PhonexFixtures.cpp src/PhonexRom.cpp src/PhonexSequenceCompiler.cpp src/PhonexPronunciation.cpp -o $@

build/tools/phonex_corpus_report: tools/phonex_corpus_report.cpp src/PhonexRom.cpp src/PhonexRom.hpp src/PhonexRomData.inc src/PhonexTypes.hpp src/PhonexSequenceCompiler.cpp src/PhonexSequenceCompiler.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tools/phonex_corpus_report.cpp src/PhonexRom.cpp src/PhonexSequenceCompiler.cpp -o $@

build/tools/phonex_benchmark: tools/phonex_benchmark.cpp src/PhonexEngine.cpp src/PhonexEngine.hpp src/PhonexFixtures.cpp src/PhonexFixtures.hpp src/PhonexTypes.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tools/phonex_benchmark.cpp src/PhonexEngine.cpp src/PhonexFixtures.cpp -o $@

build/tools/doorstop_reference_render: tools/doorstop_reference_render.cpp src/ReferenceSpringEngine.cpp src/ReferenceSpringEngine.hpp src/HelicalContinuumEngine.cpp src/HelicalContinuumEngine.hpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 -O2 -Wall -Wextra -DDOORSTOP_REFERENCE_ANALYSIS=1 tools/doorstop_reference_render.cpp src/ReferenceSpringEngine.cpp src/HelicalContinuumEngine.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

DOORSTOP_REFERENCE_VELOCITIES ?= 0.5 0.75 1.0
DOORSTOP_REFERENCE_SEEDS ?= 1 77 7331 65537 104729 999983 2654435761 305419896 610839776 195948557 271828183 314159265 3735928559 324508639 4277009102 4294967291
DOORSTOP_REFERENCE_VARIANTS ?= current spring-only modes-only spring-forward spring-refined rack-v2 boing-refined v3-boing-probe v3-dark-boing v3-deep-swing
DOORSTOP_BOING_AUDITION_DIR ?= Samples/Doorstop/Auditions/reference-v2-vs-boing-refined
DOORSTOP_V2_PHASES ?= 0 15 30 45 60 75 90

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

doorstop-v3-boing-audition: build/tools/doorstop_reference_render
	$(MAKE) DOORSTOP_REFERENCE_VARIANTS="v3-boing-probe v3-dark-boing v3-deep-swing" doorstop-variant-grid
	python3 tools/compare_doorstop_variants.py \
		--variants v3-boing-probe v3-dark-boing v3-deep-swing \
		--variant-root build/doorstop-variant-renders \
		--baseline v3-boing-probe --blind \
		--output-dir build/doorstop-v3-boing-analysis

# Stage 0 phase probe. Each phase gets the bounded Rack output and the exact
# signal presented to tanh so saturation can be evaluated independently.
doorstop-v2-phase-grid: build/tools/doorstop_reference_render
	@for phase in $(DOORSTOP_V2_PHASES); do \
		phase_name=$$(printf "%03d" $$phase); \
		mkdir -p build/doorstop-v2-phase-renders/v2-phase-$$phase_name; \
		for velocity in $(DOORSTOP_REFERENCE_VELOCITIES); do \
			for seed in $(DOORSTOP_REFERENCE_SEEDS); do \
				base=build/doorstop-v2-phase-renders/v2-phase-$$phase_name/v2-phase-$$phase_name-v$${velocity}-seed$${seed}; \
				build/tools/doorstop_reference_render $$base-module.wav --quiet \
					--variant rack-v2 --radiation-phase $$phase --output-tap module \
					--velocity $$velocity --seed $$seed; \
				build/tools/doorstop_reference_render $$base-preconditioned.wav --quiet \
					--variant rack-v2 --radiation-phase $$phase --output-tap preconditioned \
					--velocity $$velocity --seed $$seed; \
			done; \
		done; \
	done

doorstop-v2-phase-evaluate: doorstop-v2-phase-grid
	python3 tools/compare_doorstop_variants.py \
		--variant-root build/doorstop-v2-phase-renders \
		--output-dir build/doorstop-v2-phase-analysis/module \
		--baseline v2-phase-090 --tap module --blind
	python3 tools/compare_doorstop_variants.py \
		--variant-root build/doorstop-v2-phase-renders \
		--output-dir build/doorstop-v2-phase-analysis/preconditioned \
		--baseline v2-phase-090 --tap preconditioned --blind

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

.PHONY: test test-fast test-rack test-build test-build-fast test-build-rack test-odr test-sibyl-tsan test-octavia-observation-tsan test-octavia-observation-bus-tsan test-octavia-measurement-tsan
.PHONY: test-spsc-snapshot-tsan
test-build: $(TEST_BINS)
test-build-fast: $(TEST_BINS_NON_RACK) build/tests/adaptive_visual_update_spec
test-build-rack: $(TEST_BINS_RACK)

# Chimera Phase 0 proves a native, Rack-independent C++17 harness before the
# engine types and reference-vector runner arrive in Phase 1.
.PHONY: test-chimera-phase0
test-chimera-phase0: | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/chimera_phase0_smoke.cpp -o build/tests/chimera_phase0_smoke$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_phase0_smoke$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/chimera_phase0_capacity_spec.cpp -o build/tests/chimera_phase0_capacity_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_phase0_capacity_spec$(if $(ARCH_WIN),.exe,)

# Test-only internal SDK headers allow a stack Engine without Rack's GUI host.
.PHONY: test-chimera-rack-contract
test-chimera-rack-contract: | build/tests
	$(CXX) -std=gnu++17 -O2 -Wall -Wextra -Wno-unused-parameter -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_rack_save_contract_spec.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_rack_save_contract_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_rack_save_contract_spec)

.PHONY: test-chimera-rack-save
test-chimera-rack-save: | build/tests
	$(CXX) -std=gnu++17 -O2 -Wall -Wextra -Wno-unused-parameter -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_rack_patch_save_spec.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_rack_patch_save_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_rack_patch_save_spec)

# Phase 1 math and deterministic control core have no Rack dependency.
.PHONY: test-chimera-phase1 test-chimera-phase1-fast test-chimera-phase1-sanitize
test-chimera-phase1: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pedantic -Isrc tests/chimera_phase1_spec.cpp -o build/tests/chimera_phase1_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_phase1_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pedantic -Isrc tests/chimera_profile_probe.cpp -o build/tests/chimera_profile_probe$(if $(ARCH_WIN),.exe,)
	python3 tools/chimera_phase1_vectors.py build/tests/chimera_profile_probe$(if $(ARCH_WIN),.exe,)

test-chimera-phase1-fast: | build/tests
	$(CXX) -std=c++11 -O3 $(if $(ARCH_X64),-march=nehalem,) -funsafe-math-optimizations -ffast-math -Wall -Wextra -Isrc tests/chimera_phase1_spec.cpp -o build/tests/chimera_phase1_fast_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_phase1_fast_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O3 $(if $(ARCH_X64),-march=nehalem,) -funsafe-math-optimizations -ffast-math -Wall -Wextra -Isrc tests/chimera_profile_probe.cpp -o build/tests/chimera_profile_fast_probe$(if $(ARCH_WIN),.exe,)
	python3 tools/chimera_phase1_vectors.py build/tests/chimera_profile_fast_probe$(if $(ARCH_WIN),.exe,)

test-chimera-phase1-sanitize: | build/tests
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Isrc tests/chimera_phase1_spec.cpp -o build/tests/chimera_phase1_sanitize_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_phase1_sanitize_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-phase2 test-chimera-phase2-sanitize test-chimera-phase3 test-chimera-phase3-sanitize test-chimera-module
test-chimera-module: | build/tests
	$(CXX) -std=c++11 -O1 -g $(if $(ARCH_X64),-march=nehalem,) -Wall -Wextra -Wno-unused-parameter -Wno-mismatched-new-delete -fno-fast-math -fno-unsafe-math-optimizations -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_module_spec.cpp src/ChimeraService.cpp src/ChimeraBundle.cpp src/ChimeraRecovery.cpp src/ChimeraWav.cpp src/ChimeraWavConvenience.cpp src/ChimeraEdit.cpp src/DebugTerminalTransport.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp src/visual/ApertureLight.cpp src/visual/RasterImageAssets.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32 -lws2_32,-lGL) -pthread -o build/tests/chimera_module_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_module_spec$(if $(ARCH_WIN),.exe,))

test-chimera-phase3: test-chimera-sos | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -fno-fast-math -fno-unsafe-math-optimizations -Isrc tests/chimera_slice_spec.cpp -o build/tests/chimera_slice_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_slice_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-phase4 test-chimera-gene-size
test-chimera-gene-size: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -fno-fast-math -pthread -Isrc tests/chimera_gene_size_spec.cpp -o build/tests/chimera_gene_size_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_gene_size_spec$(if $(ARCH_WIN),.exe,)

test-chimera-phase4: test-chimera-morph test-chimera-gene-size | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -fno-fast-math -fno-unsafe-math-optimizations -Isrc tests/chimera_grains_spec.cpp -o build/tests/chimera_grains_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_grains_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-clock
test-chimera-clock: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc tests/chimera_clock_spec.cpp -o build/tests/chimera_clock_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_clock_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-rate-bridge
test-chimera-rate-bridge: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_rate_bridge_spec.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_rate_bridge_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_rate_bridge_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-wav
test-chimera-wav: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/dep/include tests/chimera_wav_spec.cpp src/ChimeraWav.cpp src/ChimeraWavConvenience.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_wav_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_wav_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-edit
.PHONY: test-chimera-playback-reader test-chimera-checkpoints test-chimera-repairs
test-chimera-repairs: test-chimera-playback-reader test-chimera-checkpoints test-chimera-dispatch test-chimera-module test-chimera-patch test-chimera-marker-display

.PHONY: test-chimera-marker-display
test-chimera-marker-display: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc tests/chimera_marker_display_spec.cpp -o build/tests/chimera_marker_display_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_marker_display_spec$(if $(ARCH_WIN),.exe,)

test-chimera-playback-reader: | build/tests
	$(CXX) -std=c++11 -O3 $(if $(ARCH_X64),-march=nehalem,) -Wall -Wextra -fno-fast-math -Isrc tests/chimera_playback_reader_spec.cpp -o build/tests/chimera_playback_reader_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_playback_reader_spec$(if $(ARCH_WIN),.exe,)

test-chimera-edit: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc tests/chimera_edit_spec.cpp src/ChimeraEdit.cpp -o build/tests/chimera_edit_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_edit_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-bundle
test-chimera-bundle: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_bundle_spec.cpp src/ChimeraBundle.cpp src/ChimeraWav.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_bundle_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_bundle_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-recovery
test-chimera-checkpoints: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_checkpoint_session_spec.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_checkpoint_session_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_checkpoint_session_spec$(if $(ARCH_WIN),.exe,))

test-chimera-recovery: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_recovery_spec.cpp src/ChimeraRecovery.cpp src/ChimeraBundle.cpp src/ChimeraWav.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_recovery_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_recovery_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-waveform
test-chimera-waveform: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc tests/chimera_waveform_spec.cpp -o build/tests/chimera_waveform_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_waveform_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-panel
test-chimera-panel:
	python3 tests/chimera_panel_contract_spec.py

.PHONY: test-chimera-full-save
test-chimera-full-save: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_full_save_spec.cpp src/ChimeraBundle.cpp src/ChimeraWav.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_full_save_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_full_save_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-patch
test-chimera-patch: | build/tests
	$(CXX) -std=gnu++17 -O2 -g $(if $(ARCH_X64),-march=nehalem,) -Wall -Wextra -Wno-unused-parameter -fno-fast-math -fno-unsafe-math-optimizations -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_patch_spec.cpp src/ChimeraService.cpp src/ChimeraBundle.cpp src/ChimeraRecovery.cpp src/ChimeraWav.cpp src/ChimeraWavConvenience.cpp src/ChimeraEdit.cpp src/DebugTerminalTransport.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp src/visual/ApertureLight.cpp src/visual/RasterImageAssets.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32 -lws2_32,-lGL) -pthread -o build/tests/chimera_patch_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_patch_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-dispatch
test-chimera-dispatch: | build/tests
	$(CXX) -std=gnu++17 -O2 -g $(if $(ARCH_X64),-march=nehalem,) -Wall -Wextra -Wno-unused-parameter -fno-fast-math -fno-unsafe-math-optimizations -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_dispatch_spec.cpp src/ChimeraService.cpp src/ChimeraBundle.cpp src/ChimeraRecovery.cpp src/ChimeraWav.cpp src/ChimeraWavConvenience.cpp src/ChimeraEdit.cpp src/DebugTerminalTransport.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp src/visual/ApertureLight.cpp src/visual/RasterImageAssets.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32 -lws2_32,-lGL) -pthread -o build/tests/chimera_dispatch_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_dispatch_spec$(if $(ARCH_WIN),.exe,))

.PHONY: bench-chimera-phase6
bench-chimera-phase6: | build/tests
	$(CXX) -std=c++11 -O3 -DNDEBUG $(if $(ARCH_X64),-march=nehalem,) -Wno-unused-parameter -fno-fast-math -fno-unsafe-math-optimizations -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/chimera_phase6_module_bench.cpp src/ChimeraService.cpp src/ChimeraBundle.cpp src/ChimeraRecovery.cpp src/ChimeraWav.cpp src/ChimeraWavConvenience.cpp src/ChimeraEdit.cpp src/DebugTerminalTransport.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp src/visual/ApertureLight.cpp src/visual/RasterImageAssets.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32 -lws2_32,-lGL) -pthread -o build/tests/chimera_phase6_module_bench$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_phase6_module_bench$(if $(ARCH_WIN),.exe,))

test-chimera-phase3-sanitize: | build/tests
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-fast-math -pthread -Isrc tests/chimera_slice_spec.cpp -o build/tests/chimera_slice_sanitize_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_slice_sanitize_spec$(if $(ARCH_WIN),.exe,)

test-chimera-phase2: test-chimera-overlap | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Wno-mismatched-new-delete -pthread -Isrc tests/chimera_reel_spec.cpp -o build/tests/chimera_reel_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_reel_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc tests/chimera_reel_concurrency_spec.cpp -o build/tests/chimera_reel_concurrency_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_reel_concurrency_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc tests/chimera_jobs_spec.cpp -o build/tests/chimera_jobs_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_jobs_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_service_lifecycle_spec.cpp src/ChimeraService.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_service_lifecycle_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_service_lifecycle_spec$(if $(ARCH_WIN),.exe,))

.PHONY: test-chimera-overlap
test-chimera-overlap: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc tests/chimera_overlap_spec.cpp -o build/tests/chimera_overlap_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_overlap_spec$(if $(ARCH_WIN),.exe,)

test-chimera-phase2-sanitize: | build/tests
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -pthread -Isrc tests/chimera_reel_spec.cpp -o build/tests/chimera_reel_sanitize_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_reel_sanitize_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -pthread -Isrc tests/chimera_reel_concurrency_spec.cpp -o build/tests/chimera_reel_concurrency_sanitize_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_reel_concurrency_sanitize_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -pthread -Isrc tests/chimera_jobs_spec.cpp -o build/tests/chimera_jobs_sanitize_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_jobs_sanitize_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_service_lifecycle_spec.cpp src/ChimeraService.cpp -L$(RACK_DIR) -lRack -o build/tests/chimera_service_lifecycle_sanitize_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_service_lifecycle_sanitize_spec$(if $(ARCH_WIN),.exe,))

test-sibyl-tsan: build/tests/sibyl_module_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 \
	LD_LIBRARY_PATH="$(RACK_RUNTIME_DIR):$$LD_LIBRARY_PATH" \
	build/tests/sibyl_module_tsan_spec

test-octavia-observation-tsan: build/tests/octavia_observation_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 build/tests/octavia_observation_tsan_spec

test-octavia-observation-bus-tsan: build/tests/octavia_observation_bus_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 build/tests/octavia_observation_bus_tsan_spec

test-spsc-snapshot-tsan: build/tests/spsc_latest_snapshot_tsan_spec
	@TSAN_OPTIONS=halt_on_error=1 build/tests/spsc_latest_snapshot_tsan_spec

test-fast: test-build-fast
	$(call run_test_bin,build/tests/octavia_presence_spec)
	$(call run_rack_test_bin,build/tests/octavia_presence_routes_spec)
	$(call run_test_bin,build/tests/debug_terminal_timing_spec)
	$(call run_test_bin,build/tests/adaptive_visual_update_spec)
	$(call run_test_bin,build/tests/halo_metrics_scope_spec)
	$(call run_test_bin,build/tests/chromatide_qoi_preflight_spec)
	$(call run_rack_test_bin,build/tests/review_state_handoff_spec)
	$(call run_test_bin,build/tests/octavia_observation_bus_spec)
	$(call run_test_bin,build/tests/octavia_analysis_spec)
	$(call run_test_bin,build/tests/octavia_observation_spec)
	$(call run_test_bin,build/tests/octavia_recording_spec)
	$(call run_test_bin,build/tests/octavia_job_control_spec)
	$(call run_test_bin,build/tests/octavia_server_lifecycle_spec)
	$(call run_test_bin,build/tests/octavia_semantic_control_spec)
	$(call run_test_bin,build/tests/octavia_action_validation_spec)
	$(call run_test_bin,build/tests/octavia_cable_validation_spec)
	$(call run_test_bin,build/tests/octavia_console_mailbox_spec)
	$(call run_test_bin,build/tests/sibyl_evolution_spec)
	$(call run_rack_test_bin,build/tests/sibyl_codec_spec)
	$(call run_rack_test_bin,build/tests/sibyl_note_edit_spec)
	$(call run_rack_test_bin,build/tests/sibyl_legacy_golden_spec)
	$(call run_test_bin,build/tests/sibyl_adoption_spec)
	$(call run_test_bin,build/tests/sibyl_clock_estimator_spec)
	$(call run_test_bin,build/tests/sibyl_hardware_control_spec)
	$(call run_rack_test_bin,build/tests/sibyl_edit_spec)
	python3 -B tests/sibyl_composer_fixtures.py > build/tests/sibyl_composer_fixtures.jsonl
	$(call run_rack_test_bin,build/tests/sibyl_composer_validation) < build/tests/sibyl_composer_fixtures.jsonl
	python3 -B -m unittest discover -s MCP/tests -p test_sibyl_composer.py
	$(call run_rack_test_bin,build/tests/sibyl_json_spec)
	$(call run_rack_test_bin,build/tests/sibyl_module_spec)
	$(call run_test_bin,build/tests/sibyl_timing_spec)
	$(call run_rack_test_bin,build/tests/sibyl_transport_spec)
	$(call run_test_bin,build/tests/moirai_curves_spec)
	$(call run_test_bin,build/tests/moirai_adoption_spec)
	$(call run_test_bin,build/tests/moirai_compiler_spec)
	$(call run_rack_test_bin,build/tests/moirai_edit_spec)
	$(call run_rack_test_bin,build/tests/moirai_json_spec)
	$(call run_test_bin,build/tests/moirai_engine_spec)
	$(call run_rack_test_bin,build/tests/moirai_module_spec)
	$(call run_test_bin,build/tests/theme_service_spec)
	$(call run_rack_test_bin,build/tests/theme_persistence_spec)
	python3 tests/octavia_sibyl_contract_spec.py
	python3 tests/octavia_semantic_contract_spec.py
	python3 tests/octavia_monitoring_panel_contract_spec.py
	python3 tests/moirai_panel_contract_spec.py
	python3 tests/phonex_panel_contract_spec.py
	python3 tests/nautiloid_gl_lifecycle_contract_spec.py
	python3 tests/iris_nautiloid_nvg_phase5_contract_spec.py
	python3 tests/nautiloid_cache_phase6_contract_spec.py
	python3 tests/nautiloid_gpu_phase7_contract_spec.py
	python3 tests/nautiloid_gpu_phase8_contract_spec.py
	python3 tests/split_svg_labels_spec.py
	python3 tests/premium_staging_spec.py
	python3 tools/generate_mandelwake_tables.py --check
	$(call run_test_bin,build/tests/temporaldeck_platter_spec_harness)
	$(call run_test_bin,build/tests/temporaldeck_arc_lights_spec)
	$(call run_test_bin,build/tests/temporaldeck_engine_spec)
	$(call run_test_bin,build/tests/temporaldeck_expander_preview_spec)
	$(call run_test_bin,build/tests/spsc_latest_snapshot_spec)
	$(call run_test_bin,build/tests/shared_svg_cache_spec)
	$(call run_test_bin,build/tests/nvg_graphics_lifecycle_spec)
	$(call run_test_bin,build/tests/temporaldeck_menu_utils_spec)
	$(call run_test_bin,build/tests/temporaldeck_frame_input_spec)
	$(call run_test_bin,build/tests/temporaldeck_platter_input_spec)
	$(call run_test_bin,build/tests/temporaldeck_sample_prep_spec)
	$(call run_test_bin,build/tests/temporaldeck_virtual_integration_spec)
	$(call run_test_bin,build/tests/crownstep_spec)
	$(call run_test_bin,build/tests/mandelwake_engine_spec)
	$(call run_test_bin,build/tests/undertow_shape_spec)
	$(call run_rack_test_bin,build/tests/undertow_module_spec)
	$(call run_test_bin,build/tests/math_helpers_spec)
	$(call run_test_bin,build/tests/puffy_engine_spec)
	$(call run_rack_test_bin,build/tests/puffy_module_spec)
	$(call run_rack_test_bin,build/tests/puffy_character_controller_spec)
	$(call run_test_bin,build/tests/cantor_culture_engine_spec)
	$(call run_rack_test_bin,build/tests/cantor_module_spec)
	$(call run_rack_test_bin,build/tests/wyrm_envelope_spec)
	$(call run_test_bin,build/tests/doorstop_engine_spec)
	$(call run_test_bin,build/tests/doorstop_reference_engine_spec)
	$(call run_test_bin,build/tests/doorstop_helical_engine_spec)
	$(call run_rack_test_bin,build/tests/$(ARCH_NAME)/bifurx_runtime_spec$(if $(ARCH_WIN),.exe,))
	$(call run_test_bin,build/tests/$(ARCH_NAME)/bifurx_filter_spec$(if $(ARCH_WIN),.exe,))
	$(call run_test_bin,build/tests/sil_repair_spec)
	$(call run_test_bin,build/tests/sil_limiter_peak_window_spec)
	$(call run_test_bin,build/tests/bulkhead_geometry_spec)
	$(call run_test_bin,build/tests/umi_engine_spec)
	$(call run_test_bin,build/tests/aperture_light_transfer_spec)
	$(call run_test_bin,build/tests/iris_wavetable_spec)
	$(call run_test_bin,build/tests/iris_worker_completion_spec)
	$(call run_rack_test_bin,build/tests/iris_module_phase4_spec)
	$(call run_test_bin,build/tests/nautiloid_request_coordinator_spec)
	$(call run_test_bin,build/tests/nautiloid_location_code_spec)
	$(call run_test_bin,build/tests/nautiloid_gpu_precision_spec)
	$(call run_rack_test_bin,build/tests/nautiloid_iris_restore_spec)
	$(call run_rack_test_bin,build/tests/integral_flux_runtime_spec)
	$(call run_rack_test_bin,build/tests/legacy_curve_preview_spec)
	$(call run_rack_test_bin,build/tests/proc_runtime_spec)
	$(call run_test_bin,build/tests/wave_preview_simplification_spec)
	$(call run_test_bin,build/tests/deepcache_planner_spec)
	$(call run_test_bin,build/tests/deepcache_archive_spec)
	$(call run_test_bin,build/tests/deepcache_theme_classifier_spec)
	$(call run_rack_test_bin,build/tests/chromatide_spec)
	$(call run_rack_test_bin,build/tests/temporaldeck_longplay_spec)
	$(call run_test_bin,build/tests/phonex_engine_spec)
	python3 tools/generate_phonex_rom.py --check

test-rack: test-build-rack
ifeq ($(RUN_CHRONOMAW_WIP_TESTS),1)
	$(call run_rack_test_bin,build/tests/chronomaw_serialization_spec)
else
	@echo "[SKIP] chronomaw_serialization_spec (Chronomaw WIP; set RUN_CHRONOMAW_WIP_TESTS=1 to run)"
endif
	$(call run_rack_test_bin,build/tests/panel_svg_utils_spec)
	$(call run_rack_test_bin,build/tests/crownstep_persistence_spec)
	$(call run_rack_test_bin,build/tests/doorstop_runtime_spec)
	$(call run_rack_test_bin,build/tests/phonex_module_spec)

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

build/tests/octavia_recording_spec: tests/octavia_recording_spec.cpp src/OctaviaRecording.cpp src/OctaviaRecording.hpp src/OctaviaAnalysis.cpp src/OctaviaAnalysis.hpp src/OctaviaObservation.cpp src/OctaviaObservation.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_recording_spec.cpp src/OctaviaRecording.cpp src/OctaviaAnalysis.cpp src/OctaviaObservation.cpp -o $@

build/tests/octavia_analysis_spec: tests/octavia_analysis_spec.cpp src/OctaviaAnalysis.cpp src/OctaviaAnalysis.hpp src/OctaviaObservation.cpp src/OctaviaObservation.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_analysis_spec.cpp src/OctaviaAnalysis.cpp src/OctaviaObservation.cpp -o $@

build/tests/octavia_semantic_control_spec: tests/octavia_semantic_control_spec.cpp src/OctaviaSemanticControl.hpp src/SibylControl.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/octavia_semantic_control_spec.cpp -o $@

build/tests/octavia_observation_bus_spec: tests/octavia_observation_bus_spec.cpp src/OctaviaObservationBus.cpp src/OctaviaObservationBus.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/octavia_observation_bus_spec.cpp src/OctaviaObservationBus.cpp -o $@

build/tests/octavia_observation_bus_tsan_spec: tests/octavia_observation_bus_spec.cpp src/OctaviaObservationBus.cpp src/OctaviaObservationBus.hpp | build/tests
	$(CXX) -std=c++17 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer -Isrc tests/octavia_observation_bus_spec.cpp src/OctaviaObservationBus.cpp -o $@

build/tests/octavia_observation_tsan_spec: tests/octavia_observation_spec.cpp src/OctaviaObservation.cpp src/OctaviaObservation.hpp | build/tests
	$(CXX) -std=c++17 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer -Isrc tests/octavia_observation_spec.cpp src/OctaviaObservation.cpp -o $@

build/tests/temporaldeck_platter_spec_harness: tests/platter_spec_main.cpp tests/platter_spec_cases.cpp tests/platter_trace_replay.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/spsc_latest_snapshot_spec: tests/spsc_latest_snapshot_spec.cpp src/SpscLatestSnapshot.hpp src/TemporalDeckExpanderProtocol.hpp src/SilSpectrumSnapshot.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread $< -o $@

build/tests/spsc_latest_snapshot_tsan_spec: tests/spsc_latest_snapshot_spec.cpp src/SpscLatestSnapshot.hpp src/TemporalDeckExpanderProtocol.hpp src/SilSpectrumSnapshot.hpp | build/tests
	$(CXX) -std=c++11 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer $< -o $@

build/tests/shared_svg_cache_spec: tests/shared_svg_cache_spec.cpp src/visual/SharedSvgCacheState.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra $< -o $@

build/tests/nvg_graphics_lifecycle_spec: tests/nvg_graphics_lifecycle_spec.cpp src/NvgGraphicsLifecycle.cpp src/NvgGraphicsLifecycle.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -I$(RACK_DIR)/dep/include $(filter %.cpp,$^) -o $@

build/tests/sibyl_json_spec: tests/sibyl_json_spec.cpp src/SibylJSON.cpp src/SibylJSON.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_json_spec.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_adoption_spec: tests/sibyl_adoption_spec.cpp src/SibylAdoption.cpp src/SibylAdoption.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_adoption_spec.cpp src/SibylAdoption.cpp -o $@

build/tests/sibyl_clock_estimator_spec: tests/sibyl_clock_estimator_spec.cpp src/SibylClockEstimator.cpp src/SibylClockEstimator.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_clock_estimator_spec.cpp src/SibylClockEstimator.cpp -o $@

build/tests/sibyl_hardware_control_spec: tests/sibyl_hardware_control_spec.cpp src/SibylHardwareControl.cpp src/SibylHardwareControl.hpp src/SibylAdoption.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_hardware_control_spec.cpp src/SibylHardwareControl.cpp -o $@

build/tests/sibyl_edit_spec: tests/sibyl_edit_spec.cpp src/SibylEdit.cpp src/SibylAssignmentEdit.hpp src/SibylNoteEdit.cpp src/SibylNoteEdit.hpp src/SibylEdit.hpp src/SibylJSON.cpp src/SibylJSON.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_edit_spec.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_composer_validation: tests/sibyl_composer_validation.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylJSON.cpp $(wildcard src/Sibyl*.hpp) | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_composer_validation.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_transport_spec: tests/sibyl_transport_spec.cpp src/SibylTransport.cpp src/SibylTransport.hpp src/SibylAdoption.cpp src/SibylAdoption.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_transport_spec.cpp src/SibylTransport.cpp src/SibylAdoption.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/sibyl_timing_spec: tests/sibyl_timing_spec.cpp src/SibylTiming.cpp src/SibylTiming.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/sibyl_timing_spec.cpp src/SibylTiming.cpp -o $@

build/tests/sibyl_module_spec: tests/sibyl_module_spec.cpp tests/sibyl_condition_cases.hpp tests/sibyl_override_cases.hpp tests/sibyl_automation_cases.hpp tests/sibyl_harmony_cases.hpp tests/sibyl_voicing_cases.hpp tests/sibyl_combined_cases.hpp src/Sibyl.cpp src/SibylEvolution.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp src/SibylAssignmentEdit.hpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_module_spec.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

# Release-like process-only benchmark; run explicitly, outside the correctness suite.
SIBYL_BENCH_SOURCES := src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp
build/tests/sibyl_process_benchmark: tests/sibyl_process_benchmark.cpp src/Sibyl.cpp $(wildcard src/Sibyl*.hpp) $(SIBYL_BENCH_SOURCES) | build/tests
	$(CXX) -std=c++17 -O3 -funsafe-math-optimizations -march=nehalem -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_process_benchmark.cpp $(SIBYL_BENCH_SOURCES) -L$(RACK_DIR) -lRack -o $@

build/tests/sibyl_module_tsan_spec: tests/sibyl_module_spec.cpp tests/sibyl_condition_cases.hpp tests/sibyl_override_cases.hpp tests/sibyl_automation_cases.hpp tests/sibyl_harmony_cases.hpp tests/sibyl_voicing_cases.hpp tests/sibyl_combined_cases.hpp src/Sibyl.cpp src/SibylEvolution.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp src/SibylAssignmentEdit.hpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp | build/tests
	$(CXX) -std=c++17 -O1 -g -Wall -Wextra -pthread -fsanitize=thread -fno-omit-frame-pointer -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_module_spec.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/moirai_curves_spec: tests/moirai_curves_spec.cpp src/MoiraiCurves.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/moirai_curves_spec.cpp -o $@

build/tests/moirai_adoption_spec: tests/moirai_adoption_spec.cpp src/MoiraiEngine.cpp src/MoiraiEngine.hpp src/MoiraiCompiler.cpp src/MoiraiCompiler.hpp src/MoiraiPresets.cpp src/MoiraiPresets.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/moirai_adoption_spec.cpp src/MoiraiEngine.cpp src/MoiraiCompiler.cpp src/MoiraiPresets.cpp -o $@

build/tests/moirai_compiler_spec: tests/moirai_compiler_spec.cpp src/MoiraiCompiler.cpp src/MoiraiCompiler.hpp src/MoiraiCurves.hpp src/MoiraiPresets.cpp src/MoiraiPresets.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/moirai_compiler_spec.cpp src/MoiraiCompiler.cpp src/MoiraiPresets.cpp -o $@

build/tests/moirai_edit_spec: tests/moirai_edit_spec.cpp src/MoiraiEdit.cpp src/MoiraiEdit.hpp src/MoiraiJSON.cpp src/MoiraiJSON.hpp src/MoiraiCompiler.cpp src/MoiraiCompiler.hpp src/MoiraiPresets.cpp src/MoiraiPresets.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/moirai_edit_spec.cpp src/MoiraiEdit.cpp src/MoiraiJSON.cpp src/MoiraiCompiler.cpp src/MoiraiPresets.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/moirai_json_spec: tests/moirai_json_spec.cpp src/MoiraiJSON.cpp src/MoiraiJSON.hpp src/MoiraiCompiler.cpp src/MoiraiCompiler.hpp src/MoiraiPresets.cpp src/MoiraiPresets.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/moirai_json_spec.cpp src/MoiraiJSON.cpp src/MoiraiCompiler.cpp src/MoiraiPresets.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

build/tests/moirai_engine_spec: tests/moirai_engine_spec.cpp src/MoiraiEngine.cpp src/MoiraiEngine.hpp src/MoiraiCompiler.cpp src/MoiraiCompiler.hpp src/MoiraiCurves.hpp src/MoiraiPresets.cpp src/MoiraiPresets.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/moirai_engine_spec.cpp src/MoiraiEngine.cpp src/MoiraiCompiler.cpp src/MoiraiPresets.cpp -o $@

build/tests/moirai_module_spec: tests/moirai_module_spec.cpp src/Moirai.cpp src/Moirai.hpp src/MoiraiEdit.cpp src/MoiraiEdit.hpp src/MoiraiJSON.cpp src/MoiraiJSON.hpp src/MoiraiEngine.cpp src/MoiraiEngine.hpp src/MoiraiCompiler.cpp src/MoiraiCompiler.hpp src/MoiraiCurves.hpp src/MoiraiPresets.cpp src/MoiraiPresets.hpp src/MoiraiTypes.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/moirai_module_spec.cpp src/Moirai.cpp src/MoiraiEdit.cpp src/MoiraiJSON.cpp src/MoiraiEngine.cpp src/MoiraiCompiler.cpp src/MoiraiPresets.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

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

build/tests/sil_limiter_peak_window_spec: tests/sil_limiter_peak_window_spec.cpp src/SilLimiterPeakWindow.hpp | build/tests
	$(CXX) -std=c++11 -O3 -Wall -Wextra $< -o $@

build/tests/bulkhead_geometry_spec: tests/bulkhead_geometry_spec.cpp src/BulkheadGeometry.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/umi_engine_spec: tests/umi_engine_spec.cpp src/UmiEngine.cpp src/UmiLayout.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/aperture_light_transfer_spec: tests/aperture_light_transfer_spec.cpp src/visual/ApertureLightTransfer.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $< -o $@

build/tests/iris_wavetable_spec: tests/iris_wavetable_spec.cpp src/IrisWavetable.hpp src/IrisPolyphony.hpp src/IrisIO.cpp src/IrisIO.hpp src/IrisSourceField.cpp src/IrisSourceField.hpp | build/tests
	$(CXX) -std=c++11 -O3 -Wall -Wextra -I$(RACK_DIR)/dep/include tests/iris_wavetable_spec.cpp src/IrisIO.cpp src/IrisSourceField.cpp -o $@

build/tests/iris_worker_completion_spec: tests/iris_worker_completion_spec.cpp src/IrisWorkerCompletion.hpp | build/tests
	$(CXX) -std=c++11 -O3 -Wall -Wextra $< -o $@

build/tests/iris_module_phase4_spec: tests/iris_module_phase4_spec.cpp src/Iris.cpp src/Iris.hpp src/IrisIO.cpp src/IrisSourceField.cpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Wno-unused-parameter -DLEVIATHAN_IRIS_PHASE4_TEST=1 -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/iris_module_phase4_spec.cpp src/Iris.cpp src/IrisIO.cpp src/IrisSourceField.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/nautiloid_request_coordinator_spec: tests/nautiloid_request_coordinator_spec.cpp src/NautiloidRequestCoordinator.hpp src/NautiloidCachePolicy.hpp src/NautiloidFractal.hpp | build/tests
	$(CXX) -std=c++17 -O3 -Wall -Wextra tests/nautiloid_request_coordinator_spec.cpp -o $@

build/tests/nautiloid_location_code_spec: tests/nautiloid_location_code_spec.cpp src/NautiloidLocationCode.cpp src/NautiloidLocationCode.hpp src/NautiloidFractal.hpp | build/tests
	$(CXX) -std=c++17 -O3 -Wall -Wextra tests/nautiloid_location_code_spec.cpp src/NautiloidLocationCode.cpp -o $@

build/tests/nautiloid_gpu_precision_spec: tests/nautiloid_gpu_precision_spec.cpp src/NautiloidGpuPrecision.hpp | build/tests
	$(CXX) -std=c++17 -O3 -Wall -Wextra tests/nautiloid_gpu_precision_spec.cpp -o $@

build/tests/nautiloid_iris_restore_spec: tests/nautiloid_iris_restore_spec.cpp src/Nautiloid.cpp src/Nautiloid.hpp src/NautiloidLocationCode.cpp src/Iris.cpp src/Iris.hpp src/IrisIO.cpp src/IrisSourceField.cpp src/Chromatide.cpp src/ChromatideCanvas.cpp src/UiExpanderUtils.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $(filter %.cpp,$^) -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -pthread -o $@

build/tests/integral_flux_runtime_spec: tests/integral_flux_runtime_spec.cpp src/IntegralFlux.cpp src/IntegralFlux.hpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Wno-subobject-linkage -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/integral_flux_runtime_spec.cpp src/IntegralFlux.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/legacy_curve_preview_spec: tests/legacy_curve_preview_spec.cpp src/render/LegacyCurvePreview.hpp src/visual/SettledContourFramebuffer.hpp src/visual/SnapshotHistory.hpp src/WavePreviewTracer.hpp | build/tests
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $< -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tools/preview_invalidation_benchmark: tools/preview_invalidation_benchmark.cpp src/WavePreviewGeometryKey.hpp src/WavePreviewTracer.hpp src/visual/SettledContourFramebuffer.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/preview_invalidation_benchmark.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

# Explicit offline experiments only; not dependencies of the plugin or test-fast.
build/tests/adaptive_gl_batch_spec: tests/adaptive_gl_batch_spec.cpp src/visual/AdaptiveGlSurface.cpp src/visual/AdaptiveGlSurface.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/adaptive_gl_batch_spec.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/bounded_polyline_spec: tools/experiments/lumin/bounded_polyline_spec.cpp tools/experiments/lumin/BoundedPolyline.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra $< -o $@

build/tests/round_join_candidate_spec: tools/experiments/lumin/round_join_candidate_spec.cpp tools/experiments/lumin/round_join_candidate.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $< -o $@

build/tools/flux_shader/flux_geometry.hpp: tools/experiments/lumin/prepare_flux_shader_geometry.py src/IntegralFluxWidget.cpp src/IntegralFlux.cpp
	python3 tools/experiments/lumin/prepare_flux_shader_geometry.py

build/tests/flux_shader_surface_spec: tools/experiments/lumin/flux_shader_surface_spec.cpp tools/experiments/lumin/flux_shader_candidate.hpp tools/experiments/lumin/experimental_shader_stroke.hpp src/visual/AdaptiveGlSurface.cpp tools/experiments/lumin/PrivateContourEngine.cpp build/tools/flux_shader/flux_geometry.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/lumin/flux_shader_surface_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/round_stroke_surface_spec: tools/experiments/lumin/round_stroke_surface_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp tools/experiments/lumin/ExperimentalRoundStroke.hpp src/visual/AdaptiveGlSurface.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/lumin/round_stroke_surface_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

# Offline only: pinned NanoVG inputs must first be placed in work/nanovg.
build/tools/round_stroke/private.cpp: tools/experiments/lumin/prepare_round_stroke.py src/IntegralFluxWidget.cpp src/IntegralFlux.cpp work/nanovg/nanovg.c work/nanovg/nanovg.h work/nanovg/nanovg_gl.h work/nanovg/fontstash.h work/nanovg/stb_truetype.h work/nanovg/stb_image.h | build
	python3 tools/experiments/lumin/prepare_round_stroke.py

build/tools/round_stroke_benchmark: tools/experiments/lumin/round_stroke_benchmark.cpp tools/experiments/lumin/private_stroke_api.hpp build/tools/round_stroke/private.cpp | build
	$(CXX) -std=c++17 -O3 -march=nehalem -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Itools/experiments/lumin -Ibuild/tools/round_stroke -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/lumin/round_stroke_benchmark.cpp build/tools/round_stroke/private.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tools/callback_bridge_spec: tools/experiments/lumin/callback_bridge_spec.cpp tools/experiments/lumin/callback_bridge.cpp tools/experiments/lumin/callback_bridge.hpp build/tools/round_stroke/private.cpp | build
	$(CXX) -std=c++17 -O3 -march=nehalem -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Itools/experiments/lumin -Ibuild/tools/round_stroke -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/lumin/callback_bridge_spec.cpp tools/experiments/lumin/callback_bridge.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tools/proc_preview_render_benchmark: tools/proc_preview_render_benchmark.cpp tools/preview_benchmark_utils.hpp tools/experiments/lumin/round_join_candidate.hpp src/Proc.cpp src/ProcPreviewGeometry.hpp src/WavePreviewSimplifier.hpp tools/experiments/lumin/BoundedPolyline.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/proc_preview_render_benchmark.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tools/PhosphorPreviewBaseline.hpp: tools/prepare_phosphor_baseline.py | build
	python3 tools/prepare_phosphor_baseline.py

build/tools/proc_phosphor_benchmark: tools/proc_phosphor_benchmark.cpp tools/preview_benchmark_utils.hpp build/tools/PhosphorPreviewBaseline.hpp src/visual/PhosphorPreview.hpp src/GlLifecycleUtils.cpp src/GlLifecycleUtils.hpp src/Proc.cpp src/ProcPreviewGeometry.hpp | build
	mkdir -p build/tools
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -Ibuild/tools -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/proc_phosphor_benchmark.cpp src/GlLifecycleUtils.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/proc_runtime_spec: tests/proc_runtime_spec.cpp src/Proc.cpp src/ProcPreviewGeometry.hpp src/WavePreviewGeometryKey.hpp src/WavePreviewTracer.hpp src/visual/SettledContourFramebuffer.hpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Wno-subobject-linkage -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/proc_runtime_spec.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/temporaldeck_virtual_integration_spec: tests/temporaldeck_virtual_integration_spec.cpp src/TemporalDeckPlatterInput.cpp src/TemporalDeckTransportControl.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/crownstep_spec: tests/crownstep_spec.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $^ -o $@

build/tests/mandelwake_engine_spec: tests/mandelwake_engine_spec.cpp src/MandelwakeEngine.cpp src/MandelwakeEngine.hpp src/MandelwakeFixedPoint.hpp src/MandelwakeTables.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/mandelwake_engine_spec.cpp src/MandelwakeEngine.cpp -o $@

build/tests/phonex_engine_spec: tests/phonex_engine_spec.cpp src/PhonexEngine.cpp src/PhonexEngine.hpp src/PhonexFixtures.cpp src/PhonexFixtures.hpp src/PhonexTypes.hpp src/PhonexRom.cpp src/PhonexRom.hpp src/PhonexRomData.inc src/PhonexSequenceCompiler.cpp src/PhonexSequenceCompiler.hpp src/PhonexPronunciation.cpp src/PhonexPronunciation.hpp src/PhonexSequenceMailbox.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/phonex_engine_spec.cpp src/PhonexEngine.cpp src/PhonexFixtures.cpp src/PhonexRom.cpp src/PhonexSequenceCompiler.cpp src/PhonexPronunciation.cpp $(MINGW_TEST_STACK_FLAGS) -o $@

build/tests/undertow_shape_spec: tests/undertow_shape_spec.cpp src/UndertowShape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/undertow_shape_spec.cpp -o $@

build/tests/undertow_module_spec: tests/undertow_module_spec.cpp src/Undertow.cpp src/Undertow.hpp src/UndertowShape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/undertow_module_spec.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

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

build/tests/theme_persistence_spec: tests/theme_persistence_spec.cpp src/theme/ThemePersistence.cpp src/theme/ThemePersistence.hpp src/theme/ThemeService.cpp src/theme/ThemeService.hpp src/theme/ThemePresets.cpp src/theme/ThemePresets.hpp src/theme/ThemeTypes.hpp | build/tests
	# Match the plugin's SIMD target to avoid Rack SIMDe/native intrinsic alias conflicts.
	$(CXX) -std=c++17 -O2 -march=nehalem -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/theme_persistence_spec.cpp src/theme/ThemePersistence.cpp src/theme/ThemeService.cpp src/theme/ThemePresets.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/cantor_module_spec: tests/cantor_module_spec.cpp src/Cantor.cpp src/Cantor.hpp src/CantorCultureEngine.cpp src/CantorCultureEngine.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/cantor_module_spec.cpp src/Cantor.cpp src/CantorCultureEngine.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/wyrm_envelope_spec: tests/wyrm_envelope_spec.cpp src/Wyrm.cpp src/Wyrm.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/wyrm_envelope_spec.cpp src/Wyrm.cpp src/MathHelpers.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

build/tests/temporaldeck_longplay_spec: tests/temporaldeck_longplay_spec.cpp src/LongPlayStreamEngine.cpp src/LongPlayStreamEngine.hpp src/codec.cpp src/codec.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/temporaldeck_longplay_spec.cpp src/LongPlayStreamEngine.cpp src/codec.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -pthread -o $@

build/tests/octavia_server_lifecycle_spec: tests/octavia_server_lifecycle_spec.cpp src/OctaviaServerLifecycle.hpp src/third_party/httplib.h | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 $< -o $@ $(if $(filter win,$(ARCH_OS)),-lws2_32)

build/tests/doorstop_engine_spec: tests/doorstop_engine_spec.cpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/DoorstopVisualFeedback.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/doorstop_engine_spec.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

build/tests/doorstop_reference_engine_spec: tests/doorstop_reference_engine_spec.cpp src/ReferenceSpringEngine.cpp src/ReferenceSpringEngine.hpp src/HelicalContinuumEngine.cpp src/HelicalContinuumEngine.hpp src/DoorstopEngineRouter.cpp src/DoorstopEngineRouter.hpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -DDOORSTOP_REFERENCE_ANALYSIS=1 tests/doorstop_reference_engine_spec.cpp src/ReferenceSpringEngine.cpp src/HelicalContinuumEngine.cpp src/DoorstopEngineRouter.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

build/tests/doorstop_helical_engine_spec: tests/doorstop_helical_engine_spec.cpp src/HelicalContinuumEngine.cpp src/HelicalContinuumEngine.hpp src/ReferenceSpringEngine.cpp src/ReferenceSpringEngine.hpp src/DoorstopEngineRouter.cpp src/DoorstopEngineRouter.hpp src/DoorstopEngine.cpp src/DoorstopEngine.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/doorstop_helical_engine_spec.cpp src/HelicalContinuumEngine.cpp src/ReferenceSpringEngine.cpp src/DoorstopEngineRouter.cpp src/DoorstopEngine.cpp src/MathHelpers.cpp -o $@

build/tests/$(ARCH_NAME)/bifurx_filter_spec$(if $(ARCH_WIN),.exe,): tests/bifurx_filter_spec.cpp tests/bifurx_filter_test_model.hpp src/BifurxInputStage.hpp src/BifurxOutputStage.hpp src/MathHelpers.cpp src/MathHelpers.hpp | build/tests
	@mkdir -p $(dir $@)
	$(CXX) -std=c++17 -O2 -Wall -Wextra tests/bifurx_filter_spec.cpp src/MathHelpers.cpp -o $@

build/tests/wave_preview_simplification_spec: tests/wave_preview_simplification_spec.cpp src/WavePreviewSimplifier.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(MINGW_TEST_CPPFLAGS) tests/wave_preview_simplification_spec.cpp -o $@

build/tests/deepcache_planner_spec: tests/deepcache_planner_spec.cpp src/DeepcachePlanner.cpp src/DeepcachePlanner.hpp src/DeepcacheBrowserLogic.cpp src/DeepcacheBrowserLogic.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread tests/deepcache_planner_spec.cpp src/DeepcachePlanner.cpp src/DeepcacheBrowserLogic.cpp -o $@

build/tests/deepcache_archive_spec: tests/deepcache_archive_spec.cpp src/DeepcacheArchive.cpp src/DeepcacheArchive.hpp src/DeepcacheQoi.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc tests/deepcache_archive_spec.cpp src/DeepcacheArchive.cpp src/DeepcacheQoi.cpp -o $@

build/tests/deepcache_theme_classifier_spec: tests/deepcache_theme_classifier_spec.cpp src/DeepcacheThemeClassifier.cpp src/DeepcacheThemeClassifier.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc tests/deepcache_theme_classifier_spec.cpp src/DeepcacheThemeClassifier.cpp -o $@

build/tests/chromatide_spec: tests/chromatide_spec.cpp src/ChromatideCanvas.cpp src/Chromatide.cpp src/IrisSourceField.cpp src/DeepcacheQoi.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chromatide_spec.cpp src/ChromatideCanvas.cpp src/Chromatide.cpp src/IrisSourceField.cpp src/DeepcacheQoi.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

BIFURX_TEST_OPT_FLAGS ?= -O3 -funsafe-math-optimizations $(if $(ARCH_X64),-march=nehalem,)
build/tests/$(ARCH_NAME)/bifurx_runtime_spec$(if $(ARCH_WIN),.exe,): tests/bifurx_runtime_spec.cpp src/Bifurx.cpp src/Bifurx.hpp src/BifurxTypes.hpp src/BifurxDsp.hpp src/BifurxModule.hpp src/BifurxPreview.hpp src/BifurxDisplay.hpp src/BifurxDisplay.cpp src/BifurxRenderClient.hpp src/BifurxRenderClient.cpp src/BifurxInputStage.hpp src/BifurxOutputStage.hpp src/BifurxOversampling.hpp src/BifurxTransitionSmoother.hpp src/BifurxRenderData.hpp src/BifurxWorker.hpp src/BifurxWorker.cpp src/BifurxRenderPrep.hpp src/BifurxRenderPrep.cpp src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp | build/tests
	@mkdir -p $(dir $@)
	$(CXX) -std=c++17 $(BIFURX_TEST_OPT_FLAGS) -DBIFURX_WORKER_TEST_HOOKS=1 -DBIFURX_DISPLAY_TEST_HOOKS=1 -Wall -Wextra -Wno-subobject-linkage $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/bifurx_runtime_spec.cpp src/BifurxDisplay.cpp src/BifurxPreview.cpp src/BifurxState.cpp src/BifurxRenderClient.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(abspath $(RACK_DIR)) -o $@
	$(CXX) -std=c++17 $(BIFURX_TEST_OPT_FLAGS) -DBIFURX_WORKER_TEST_HOOKS=1 -DBIFURX_DISPLAY_TEST_HOOKS=1 -MM -MP -MT "$@" -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/bifurx_runtime_spec.cpp src/BifurxDisplay.cpp src/BifurxPreview.cpp src/BifurxState.cpp src/BifurxRenderClient.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp > $@.d

build/tests/chronomaw_serialization_spec: tests/chronomaw_serialization_spec.cpp src/Chronomaw.cpp src/ChronomawEngine.cpp | build/tests
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chronomaw_serialization_spec.cpp src/Chronomaw.cpp src/ChronomawEngine.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

# Rack-linked tests are heavy C++ translation units under MSYS/MinGW. Chain
# them to avoid concurrent peak-memory spikes when users invoke `make -jN`.
build/tests/panel_svg_utils_spec: tests/panel_svg_utils_spec.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp | build/tests build/tests/$(ARCH_NAME)/bifurx_runtime_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

build/tests/crownstep_persistence_spec: tests/crownstep_persistence_spec.cpp $(CROWNSTEP_MODULE_SOURCES) | build/tests build/tests/panel_svg_utils_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) $(if $(ARCH_X64),-march=nehalem) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack $(if $(findstring mingw,$(CXX_MACHINE)),-lws2_32) -Wl,-rpath,/tmp/Rack2 -o $@

build/tests/doorstop_runtime_spec: tests/doorstop_runtime_spec.cpp src/Doorstop.cpp src/DoorstopEngine.cpp src/DoorstopEngineRouter.cpp src/ReferenceSpringEngine.cpp src/HelicalContinuumEngine.cpp src/MathHelpers.cpp | build/tests build/tests/panel_svg_utils_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $^ -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

build/tests/phonex_module_spec: tests/phonex_module_spec.cpp src/Phonex.cpp src/PhonexSemantic.cpp src/Phonex.hpp src/PhonexEngine.cpp src/PhonexEngine.hpp src/PhonexRom.cpp src/PhonexRom.hpp src/PhonexRomData.inc src/PhonexSequenceCompiler.cpp src/PhonexSequenceCompiler.hpp src/PhonexPronunciation.cpp src/PhonexPronunciation.hpp src/PhonexSequenceMailbox.hpp src/PhonexTypes.hpp src/OctaviaSemanticControl.hpp | build/tests build/tests/doorstop_runtime_spec
	$(CXX) -std=c++17 $(RACK_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/phonex_module_spec.cpp src/Phonex.cpp src/PhonexSemantic.cpp src/PhonexEngine.cpp src/PhonexRom.cpp src/PhonexSequenceCompiler.cpp src/PhonexPronunciation.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,/tmp/Rack2 -o $@

build/tools/snapshot_history_benchmark: tools/snapshot_history_benchmark.cpp tools/preview_benchmark_utils.hpp src/visual/SnapshotHistory.hpp src/WavePreviewTracer.hpp src/Proc.cpp src/ProcPreviewGeometry.hpp | build
	@mkdir -p build/tools
	$(CXX) -std=c++17 $(INTEGRAL_FLUX_TEST_OPT_FLAGS) -Wall -Wextra $(RACK_TEST_WARN_FLAGS) -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/snapshot_history_benchmark.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@

# Real-driver lifecycle regression. Explicit opt-in: requires a window system,
# but creates only an invisible GLFW window (no live Rack patch required).
.PHONY: test-gl-lifecycle test-gl-batch test-render
test-render: test-gl-lifecycle test-gl-batch

test-gl-batch: build/tests/adaptive_gl_batch_spec
	$(call run_rack_test_bin,build/tests/adaptive_gl_batch_spec)

test-gl-lifecycle: build/tests/gl_surface_lifecycle_spec
	$(call run_rack_test_bin,build/tests/gl_surface_lifecycle_spec)

build/tests/gl_surface_lifecycle_spec: tests/gl_surface_lifecycle_spec.cpp src/visual/AdaptiveGlSurface.cpp src/visual/AdaptiveGlSurface.hpp src/GlResourceRetirement.cpp src/GlResourceRetirement.hpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) $(MINGW_TEST_CPPFLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/gl_surface_lifecycle_spec.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter win,$(ARCH_OS)),-lopengl32,-lGL) -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@


build/tests/debug_terminal_timing_spec: tests/debug_terminal_timing_spec.cpp src/DebugTerminalTransport.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc $< -pthread -o $@

build/tests/halo_metrics_scope_spec: tests/halo_metrics_scope_spec.cpp src/visual/HaloKnob2Metrics.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra $< -o $@

build/tests/chromatide_qoi_preflight_spec: tests/chromatide_qoi_preflight_spec.cpp src/ChromatideCanvas.cpp src/ChromatideCanvas.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -I$(RACK_DIR)/dep/include $(filter %.cpp,$^) -o $@

build/tests/review_state_handoff_spec: tests/review_state_handoff_spec.cpp src/Bulkhead.cpp src/BulkheadGeometry.cpp src/Chronomaw.cpp src/ChronomawEngine.cpp src/Bulkhead.hpp src/Chronomaw.hpp src/SpscLatestSnapshot.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra $(RACK_TEST_WARN_FLAGS) $(MINGW_TEST_CPPFLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $(filter %.cpp,$^) -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -pthread -o $@

# Explicit retained shader candidate; not enabled in live modules.
build/tests/lumin_polyline_spec: tests/lumin_polyline_spec.cpp src/render/PolylineStroke.hpp tools/experiments/lumin/retained_polyline_adapter.hpp tools/experiments/lumin/flux_shader_surface_spec.cpp build/tools/flux_shader/flux_geometry.hpp src/visual/AdaptiveGlSurface.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_polyline_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/lumin_function_curve_spec: tests/lumin_function_curve_spec.cpp tools/experiments/lumin/function_curve_candidate.hpp src/render/FunctionCurve.hpp tools/experiments/lumin/flux_shader_surface_spec.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp build/tools/flux_shader/flux_geometry.hpp build/tools/flux_shader/proc_shape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_function_curve_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tools/flux_shader/proc_shape.hpp: tools/experiments/lumin/prepare_proc_shape.py src/Proc.cpp src/ProcPreviewGeometry.hpp
	python3 tools/experiments/lumin/prepare_proc_shape.py

build/tests/lumin_function_history_spec: tests/lumin_function_history_spec.cpp tests/lumin_function_curve_spec.cpp tools/experiments/lumin/function_curve_candidate.hpp src/render/FunctionCurve.hpp tools/experiments/lumin/function_history.hpp src/visual/SnapshotHistory.hpp src/visual/AdaptiveGlSurface.cpp build/tools/flux_shader/flux_geometry.hpp build/tools/flux_shader/proc_shape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_function_history_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/lumin_shader_timing_spec: tests/lumin_shader_timing_spec.cpp tests/lumin_function_curve_spec.cpp tools/experiments/lumin/function_curve_candidate.hpp src/render/FunctionCurve.hpp src/visual/AdaptiveGlSurface.cpp build/tools/flux_shader/flux_geometry.hpp build/tools/flux_shader/proc_shape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_shader_timing_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/lumin_function_pilot_spec: tests/lumin_function_pilot_spec.cpp tests/lumin_function_curve_spec.cpp src/render/FunctionContourPilot.hpp src/render/FunctionCurve.hpp src/visual/AdaptiveGlSurface.cpp build/tools/flux_shader/flux_geometry.hpp build/tools/flux_shader/proc_shape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_function_pilot_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@


build/tests/adaptive_visual_update_spec: tests/adaptive_visual_update_spec.cpp src/render/AdaptiveVisualUpdate.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra $< -o $@

build/tests/lumin_host_stroke_bridge_spec: tests/lumin_host_stroke_bridge_spec.cpp src/UndertowShape.hpp src/WavePreviewSimplifier.hpp src/render/HostStrokeBridge.cpp src/render/HostStrokeBridge.hpp tools/experiments/lumin/callback_bridge.cpp build/tools/round_stroke/private.cpp | build/tests
	$(CXX) -std=c++17 -O3 -march=nehalem -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Itools/experiments/lumin -Ibuild/tools/round_stroke -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_host_stroke_bridge_spec.cpp src/render/HostStrokeBridge.cpp tools/experiments/lumin/callback_bridge.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/lumin_native_function_spec: tests/lumin_native_function_spec.cpp tests/lumin_function_curve_spec.cpp src/render/NativeFunctionContour.hpp src/render/FunctionCurve.hpp src/visual/AdaptiveGlSurface.cpp build/tools/flux_shader/flux_geometry.hpp build/tools/flux_shader/proc_shape.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Isrc -Itools -Ibuild/tools/flux_shader -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/lumin_native_function_spec.cpp tools/experiments/lumin/PrivateContourEngine.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/aperture_light_settling_spec: tests/aperture_light_settling_spec.cpp src/visual/ApertureLight.cpp src/visual/ApertureLight.hpp src/NvgGraphicsLifecycle.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Isrc -Itools -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/aperture_light_settling_spec.cpp src/visual/ApertureLight.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/aperture_light_layers_spec: tests/aperture_light_layers_spec.cpp src/visual/ApertureLight.cpp src/visual/ApertureLight.hpp src/NvgGraphicsLifecycle.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Isrc -Itools -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/aperture_light_layers_spec.cpp src/visual/ApertureLight.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

# Explicit offline aperture candidates; not part of plugin or test-fast.
build/tools/eclipse2/ring: tools/experiments/eclipse2/ring.cpp tools/experiments/eclipse2/fixture.hpp tools/experiments/eclipse2/main.inc build/tools/eclipse2/reference.inc tools/experiments/eclipse2/Eclipse2RingShader.hpp tools/experiments/eclipse2/Eclipse2RingExperiment.hpp src/visual/Eclipse2RingCache.hpp
	$(CXX) -std=c++17 -O2 -Isrc -Itools -Ibuild/tools/eclipse2 -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/eclipse2/ring.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp src/visual/Eclipse2RuntimeBake.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tools/eclipse2/reference.inc: tools/experiments/eclipse2/generate_reference.py src/visual/VisualAssets.cpp src/visual/Eclipse2Track.hpp
	python3 tools/experiments/eclipse2/generate_reference.py

build/tools/eclipse2/components: tools/experiments/eclipse2/components.cpp tools/experiments/eclipse2/fixture.hpp tools/experiments/eclipse2/main.inc build/tools/eclipse2/reference.inc src/visual/VisualAssets.hpp src/visual/SharedSvgCache.hpp src/visual/SharedSvgCache.cpp src/visual/Eclipse2RetainedCap.hpp src/NvgGraphicsLifecycle.cpp res/icon/Eclipse2Knob.svg res/icon/Eclipse2KnobShadow.svg
	$(CXX) -std=c++17 -O2 -Isrc -Itools -Ibuild/tools/eclipse2 -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/eclipse2/components.cpp src/visual/SharedSvgCache.cpp src/NvgGraphicsLifecycle.cpp src/visual/Eclipse2RuntimeBake.cpp src/visual/AdaptiveGlSurface.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/aperture_light_settling_spec build/tests/aperture_light_layers_spec build/tests/aperture_candidates: src/visual/ApertureBloomMasks.hpp

build/tests/aperture_bloom_pilot_spec: tests/aperture_bloom_pilot_spec.cpp tests/aperture_bloom_pilot_run.inc src/visual/ApertureLight.cpp src/visual/ApertureLight.hpp src/visual/ApertureBloomMasks.hpp src/NvgGraphicsLifecycle.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Isrc -Itools -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/aperture_bloom_pilot_spec.cpp src/visual/ApertureLight.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

build/tests/aperture_candidates: tools/experiments/aperture/aperture_candidates.cpp tools/experiments/aperture/candidates.inc tools/experiments/aperture/run.inc src/visual/ApertureLight.cpp src/visual/ApertureLight.hpp src/visual/ApertureLightTransfer.hpp src/NvgGraphicsLifecycle.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Isrc -Itools -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tools/experiments/aperture/aperture_candidates.cpp src/visual/ApertureLight.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(filter Windows_NT,$(OS)),-lopengl32,-lGL) -o $@

# Runtime baking is shared by both native Eclipse2 probes.
build/tools/eclipse2/components build/tools/eclipse2/ring: src/visual/Eclipse2RuntimeBake.cpp src/visual/Eclipse2RuntimeBake.hpp src/visual/Eclipse2Track.hpp src/visual/Eclipse2RetainedCap.hpp src/visual/Eclipse2RingCache.hpp tools/experiments/eclipse2/runtime_checks.inc

.PHONY: test-bifurx-gl
test-bifurx-gl: build/tests/$(ARCH_NAME)/bifurx_gl_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,$<)

build/tests/$(ARCH_NAME)/bifurx_gl_spec$(if $(ARCH_WIN),.exe,): tests/bifurx_gl_spec.cpp tests/bifurx_runtime_spec.cpp tests/gl_surface_lifecycle_spec.cpp $(wildcard src/Bifurx*) src/visual/AdaptiveGlSurface.cpp | build/tests
	@mkdir -p $(dir $@)
	$(CXX) -std=c++17 -O2 -Wno-subobject-linkage -DBIFURX_DISPLAY_TEST_HOOKS=1 $(MINGW_TEST_CPPFLAGS) -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include $< src/BifurxDisplay.cpp src/BifurxPreview.cpp src/BifurxState.cpp src/BifurxRenderClient.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp -L$(RACK_DIR) -lRack $(if $(ARCH_WIN),-lopengl32,-lGL) -Wl,-rpath,$(abspath $(RACK_DIR)) -o $@

	$(CXX) -std=c++17 -O2 -DBIFURX_DISPLAY_TEST_HOOKS=1 $(MINGW_TEST_CPPFLAGS) -MM -MP -MT "$@" -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/bifurx_gl_spec.cpp src/BifurxDisplay.cpp src/BifurxPreview.cpp src/BifurxState.cpp src/BifurxRenderClient.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp src/GlResourceRetirement.cpp src/GlLifecycleUtils.cpp src/NvgGraphicsLifecycle.cpp > $@.d
# Each translation unit contributes prerequisites to the same test target.
-include build/tests/$(ARCH_NAME)/bifurx_runtime_spec$(if $(ARCH_WIN),.exe,).d
-include build/tests/$(ARCH_NAME)/bifurx_gl_spec$(if $(ARCH_WIN),.exe,).d

build/tests/octavia_presence_spec: tests/octavia_presence_spec.cpp src/OctaviaPresence.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -pthread -Isrc $< -o $@

build/tests/octavia_presence_routes_spec: tests/octavia_presence_routes_spec.cpp src/OctaviaPresence.hpp src/OctaviaPresenceRoutes.hpp src/OctaviaServerLifecycle.hpp src/third_party/httplib.h | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -I$(RACK_DIR)/dep/include $< -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_RUNTIME_DIR) -o $@ $(if $(filter win,$(ARCH_OS)),-lws2_32)

build/tests/sibyl_evolution_spec: tests/sibyl_evolution_spec.cpp src/SibylEvolution.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Isrc $< -o $@

build/tests/sibyl_legacy_golden_spec: tests/sibyl_legacy_golden_spec.cpp src/Sibyl.cpp src/SibylEvolution.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp src/SibylAssignmentEdit.hpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -pthread -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_legacy_golden_spec.cpp src/SibylAdoption.cpp src/SibylClockEstimator.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylHardwareControl.cpp src/SibylJSON.cpp src/SibylTiming.cpp src/SibylTransport.cpp src/OctaviaObservationBus.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@


build/tests/sibyl_codec_spec: tests/sibyl_codec_spec.cpp src/SibylJSON.cpp src/SibylJSON.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_codec_spec.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@


build/tests/sibyl_note_edit_spec: tests/sibyl_note_edit_spec.cpp src/SibylEdit.cpp src/SibylAssignmentEdit.hpp src/SibylNoteEdit.cpp src/SibylNoteEdit.hpp src/SibylEdit.hpp src/SibylAdoption.cpp src/SibylJSON.cpp src/SibylJSON.hpp src/SibylTypes.hpp src/SibylCondition.hpp src/SibylOverrides.hpp src/SibylAutomation.hpp src/SibylAutomationJSON.hpp src/SibylHarmonyTypes.hpp src/SibylHarmony.hpp src/SibylHarmonyJSON.hpp src/SibylHarmonyView.hpp src/SibylHarmonyEdit.hpp src/SibylVoicing.hpp src/SibylVoicingEdit.hpp | build/tests
	$(CXX) -std=c++17 -O2 -Wall -Wextra -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/sibyl_note_edit_spec.cpp src/SibylEdit.cpp src/SibylNoteEdit.cpp src/SibylAdoption.cpp src/SibylJSON.cpp -L$(RACK_DIR) -lRack -Wl,-rpath,$(RACK_DIR) -o $@

# Pitch codec/math headers participate in incremental Sibyl regression builds.
build/tests/sibyl_codec_spec: tests/sibyl_tuning_cases.hpp src/SibylTuning.hpp src/SibylTuningJSON.hpp
build/tests/sibyl_module_spec build/tests/sibyl_legacy_golden_spec: src/SibylTuning.hpp src/SibylTuningJSON.hpp
build/tests/sibyl_module_spec: tests/sibyl_tuning_module_cases.hpp

# P7 control-side pitch editing and query contracts.
build/tests/sibyl_note_edit_spec build/tests/sibyl_module_spec build/tests/sibyl_legacy_golden_spec: src/SibylPitchEdit.hpp src/SibylRetune.hpp src/SibylPitchView.hpp src/SibylTuning.hpp src/SibylTuningJSON.hpp
build/tests/sibyl_note_edit_spec: tests/sibyl_pitch_edit_cases.hpp

build/tests/sibyl_codec_spec build/tests/sibyl_note_edit_spec build/tests/sibyl_module_spec build/tests/sibyl_legacy_golden_spec: $(wildcard src/Sibyl*.hpp) $(wildcard tests/sibyl_*cases.hpp) tests/sibyl_native_fixture.hpp

.PHONY: test-chimera-opus test-chimera-fast-math test-chimera-rate-preparation
test-chimera-opus: test-chimera-fast-math test-chimera-rate-preparation test-chimera-module test-chimera-patch test-chimera-wav test-chimera-bundle test-chimera-recovery test-chimera-checkpoints

test-chimera-fast-math: | build/tests
	$(CXX) -std=c++11 -O3 $(if $(ARCH_X64),-march=nehalem,) -Wall -Wextra -fno-fast-math -Isrc tests/chimera_fast_math_spec.cpp -o build/tests/chimera_fast_math_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_fast_math_spec$(if $(ARCH_WIN),.exe,)
	$(CXX) -std=c++11 -O3 $(if $(ARCH_X64),-march=nehalem,) -Wall -Wextra -ffast-math -Isrc tests/chimera_fast_math_spec.cpp -o build/tests/chimera_fast_math_fast_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_fast_math_fast_spec$(if $(ARCH_WIN),.exe,)

test-chimera-rate-preparation: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -DCHIMERA_RATE_SERVICE_TEST_HOOKS -Isrc -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/chimera_rate_preparation_spec.cpp src/ChimeraService.cpp -L$(RACK_DIR) -lRack -pthread -o build/tests/chimera_rate_preparation_spec$(if $(ARCH_WIN),.exe,)
	$(call run_rack_test_bin,build/tests/chimera_rate_preparation_spec)

.PHONY: test-chimera-morph
test-chimera-morph: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Wno-mismatched-new-delete -fno-fast-math -Isrc tests/chimera_morph_spec.cpp -o build/tests/chimera_morph_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_morph_spec$(if $(ARCH_WIN),.exe,)

.PHONY: test-chimera-sos
test-chimera-sos: | build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Wno-mismatched-new-delete -fno-fast-math -Isrc tests/chimera_sos_spec.cpp -o build/tests/chimera_sos_spec$(if $(ARCH_WIN),.exe,)
	build/tests/chimera_sos_spec$(if $(ARCH_WIN),.exe,)
