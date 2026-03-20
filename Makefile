# If RACK_DIR is not defined when calling the Makefile, default to the platform-appropriate SDK
LINUX_RACK_DIR ?= ../Rack-SDK-linux
WINDOWS_RACK_DIR ?= ../Rack-SDK
LINUX_BUILD_DIR ?= build-linux
WINDOWS_BUILD_DIR ?= build-default
STALE_BUILD_DIR ?= build-stale
UNAME_S := $(shell uname -s 2>/dev/null)

ifdef MSYSTEM
  DEFAULT_RACK_DIR := $(WINDOWS_RACK_DIR)
  DEFAULT_BUILD_DIR := $(WINDOWS_BUILD_DIR)
else ifeq ($(OS),Windows_NT)
  DEFAULT_RACK_DIR := $(WINDOWS_RACK_DIR)
  DEFAULT_BUILD_DIR := $(WINDOWS_BUILD_DIR)
else
  DEFAULT_RACK_DIR := $(LINUX_RACK_DIR)
  DEFAULT_BUILD_DIR := $(LINUX_BUILD_DIR)
endif

RACK_DIR ?= $(DEFAULT_RACK_DIR)
ACTIVE_BUILD_DIR ?= $(DEFAULT_BUILD_DIR)

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

.PHONY: prepare-build-link linux windows native clean-linux clean-windows

prepare-build-link:
	@if [ -e build ] && [ ! -L build ]; then \
		if [ -z "$$(find build -mindepth 1 -print -quit 2>/dev/null)" ]; then \
			rmdir build; \
		elif [ ! -e "$(WINDOWS_BUILD_DIR)" ] && [ ! -e "$(LINUX_BUILD_DIR)" ]; then \
			mv build "$(WINDOWS_BUILD_DIR)"; \
			echo "Moved existing build directory to $(WINDOWS_BUILD_DIR)"; \
		else \
			rm -rf "$(STALE_BUILD_DIR)"; \
			mv build "$(STALE_BUILD_DIR)"; \
			echo "Moved stale build directory to $(STALE_BUILD_DIR)"; \
		fi; \
	fi
	@if [ -L build ] || [ -f build ]; then rm -f build; fi
	@mkdir -p "$(ACTIVE_BUILD_DIR)"
	@ln -s "$(ACTIVE_BUILD_DIR)" build

$(TARGET): | prepare-build-link

linux:
	$(MAKE) RACK_DIR="$(LINUX_RACK_DIR)" ACTIVE_BUILD_DIR="$(LINUX_BUILD_DIR)" all

windows native:
	$(MAKE) RACK_DIR="$(WINDOWS_RACK_DIR)" ACTIVE_BUILD_DIR="$(WINDOWS_BUILD_DIR)" all

clean-linux:
	rm -rf "$(LINUX_BUILD_DIR)"

clean-windows:
	rm -rf "$(WINDOWS_BUILD_DIR)"
