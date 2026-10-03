# Append after the Rack SDK include in the plugin Makefile.
# The two new implementation .cpp files live in src/, which is already globbed.
# Preserve finite guards even when the main plugin enables fast math.
build/src/VTune.cpp.o build/src/VTuneBodyMapModule.cpp.o build/src/VTuneBodyMapWidget.cpp.o: FLAGS += -fno-fast-math -fno-unsafe-math-optimizations

# Optional standalone core test; requires no Rack SDK for the compile itself.
.PHONY: test-vtune-body-map
test-vtune-body-map:
	mkdir -p build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic -Isrc tests/body_map_core_test.cpp -o build/tests/body_map_core_test$(if $(ARCH_WIN),.exe,)
	./build/tests/body_map_core_test$(if $(ARCH_WIN),.exe,)
