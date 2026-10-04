#pragma once
// Test-only finite-value machine for the selected translated slices. Not ARM.
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <stdexcept>

static float from_bits(std::uint32_t value) {
    float out; std::memcpy(&out,&value,4); return out;
}
static std::uint32_t to_bits(float value) {
    std::uint32_t out; std::memcpy(&out,&value,4); return out;
}
struct Machine {
    std::array<float,32> s{};
    std::array<std::uint32_t,16> r{};
    std::map<std::uint32_t,std::uint32_t> memory;
    int comparison=0, float_comparison=0;
    std::uint32_t exit_pc=0;
    std::uint32_t load(std::uint32_t address) const { return memory.at(address); }
    float loadf(std::uint32_t address) const { return from_bits(load(address)); }
    void storef(std::uint32_t address,float value) { memory[address]=to_bits(value); }
    void compare(float a,float b) {
        assert(std::isfinite(a)&&std::isfinite(b));
        float_comparison=(a>b)-(a<b);
    }
    void compare_integer(std::int32_t a,std::int32_t b) { comparison=(a>b)-(a<b); }
};
