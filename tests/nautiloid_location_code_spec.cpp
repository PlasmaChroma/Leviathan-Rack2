#include "../src/NautiloidLocationCode.hpp"
#include "../src/NautiloidFractal.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

int checks = 0;
int failures = 0;

void check(const std::string& name, bool condition) {
  ++checks;
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
  if (!condition) ++failures;
}

bool equalState(const nautiloid_location::State& a,
                const nautiloid_location::State& b) {
  return a.mode == b.mode && a.zoom == b.zoom &&
         a.centerX == b.centerX && a.centerY == b.centerY;
}

bool base64UrlOnly(const std::string& text) {
  for (char c : text) {
    const bool accepted = (c >= 'A' && c <= 'Z') ||
      (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_';
    if (!accepted) return false;
  }
  return true;
}

uint8_t crc8(const uint8_t* data, size_t size) {
  uint8_t crc = 0u;
  for (size_t i = 0u; i < size; ++i) {
    crc ^= data[i];
    for (int bit = 0; bit < 8; ++bit) {
      crc = (crc & 0x80u) ? uint8_t((crc << 1u) ^ 0x07u) : uint8_t(crc << 1u);
    }
  }
  return crc;
}

std::string encodePayload(std::array<uint8_t, 12> payload) {
  static const char alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  payload[11] = crc8(payload.data(), 11u);
  std::string code;
  for (size_t i = 0u; i < payload.size(); i += 3u) {
    const uint32_t bits = (uint32_t(payload[i]) << 16u) |
                          (uint32_t(payload[i + 1u]) << 8u) |
                          uint32_t(payload[i + 2u]);
    code.push_back(alphabet[(bits >> 18u) & 63u]);
    code.push_back(alphabet[(bits >> 12u) & 63u]);
    code.push_back(alphabet[(bits >> 6u) & 63u]);
    code.push_back(alphabet[bits & 63u]);
  }
  return code;
}

void testFixedVectors() {
  struct Vector {
    const char* name;
    nautiloid_location::State state;
    const char* code;
  };
  const Vector vectors[] = {
    {"Mandelbrot origin", {iris::FRACTAL_MANDELBROT, 0.f, 0.0, 0.0},
     "EQAAgAAAAIAAAABm"},
    {"maximum zoom at negative corner", {iris::FRACTAL_MANDELBROT, 4.f, -2.0, -2.0},
     "Ef__AAAAAAAAAADK"},
    {"maximum zoom at positive corner", {iris::FRACTAL_MANDELBROT, 4.f, 2.0, 2.0},
     "Ef____________8d"},
    {"Julia representative", {iris::FRACTAL_JULIA, 1.f, 0.5, -0.75},
     "FEAAn____1AAAACK"},
    {"arbitrary Tricorn viewport", {iris::FRACTAL_TRICORN, 2.345678f, -1.234567, 0.87654321},
     "GpYfMPzasbgZSLDv"},
  };
  for (const Vector& vector : vectors) {
    const std::string encoded = nautiloid_location::encode(vector.state);
    check(std::string("fixed vector: ") + vector.name, encoded == vector.code);
    const nautiloid_location::DecodeResult decoded = nautiloid_location::decode(vector.code);
    check(std::string("fixed vector decodes: ") + vector.name,
          decoded.valid && equalState(decoded.state, nautiloid_location::canonicalize(vector.state)));
  }
}

void testRoundTrips() {
  uint64_t random = 0x4e415554494c4f49ull;
  auto next = [&]() {
    random ^= random >> 12u;
    random ^= random << 25u;
    random ^= random >> 27u;
    return random * 2685821657736338717ull;
  };
  const int modes[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13};
  bool all = true;
  for (int i = 0; i < 5000; ++i) {
    nautiloid_location::State state;
    state.mode = modes[next() % (sizeof(modes) / sizeof(modes[0]))];
    state.zoom = float(double(next() & 0xffffffu) / double(0xffffffu) * 8.0 - 2.0);
    state.centerX = double(next() & 0xffffffffu) / double(0xffffffffu) * 8.0 - 4.0;
    state.centerY = double(next() & 0xffffffffu) / double(0xffffffffu) * 8.0 - 4.0;
    const std::string code = nautiloid_location::encode(state);
    const nautiloid_location::DecodeResult decoded = nautiloid_location::decode(code);
    all = all && code.size() == nautiloid_location::kEncodedLength &&
      base64UrlOnly(code) && decoded.valid &&
      equalState(decoded.state, nautiloid_location::canonicalize(state)) &&
      nautiloid_location::encode(decoded.state) == code;
  }
  check("5000 deterministic round trips are canonical", all);
}

void testMalformedInput() {
  check("empty input rejected", !nautiloid_location::isValid(""));
  check("15 characters rejected", !nautiloid_location::isValid("EQAAgAAAAIAAAAB"));
  check("17 characters rejected", !nautiloid_location::isValid("EQAAgAAAAIAAAABmx"));
  check("embedded whitespace rejected", !nautiloid_location::isValid("EQAAgAAA AIAAABm"));
  check("plus rejected", !nautiloid_location::isValid("+QAAgAAAAIAAAABm"));
  check("slash rejected", !nautiloid_location::isValid("/QAAgAAAAIAAAABm"));
  check("padding rejected", !nautiloid_location::isValid("EQAAgAAAAIAAAAB="));
  check("arbitrary text rejected", !nautiloid_location::isValid("not-a-location!!"));
  check("surrounding ASCII whitespace accepted",
        nautiloid_location::isValid(" \tEQAAgAAAAIAAAABm\r\n"));

  std::string corrupted = "EQAAgAAAAIAAAABm";
  corrupted[8] = corrupted[8] == 'A' ? 'B' : 'A';
  check("checksum corruption rejected", !nautiloid_location::isValid(corrupted));

  std::array<uint8_t, 12> payload {};
  check("all-zero payload rejected", !nautiloid_location::isValid(encodePayload(payload)));
  payload[0] = uint8_t(2u << 4u) | uint8_t(iris::FRACTAL_MANDELBROT);
  check("unsupported version rejected", !nautiloid_location::isValid(encodePayload(payload)));
  payload[0] = uint8_t(1u << 4u) | 15u;
  check("invalid fractal mode rejected", !nautiloid_location::isValid(encodePayload(payload)));
}

void testCanonicalization() {
  const double infinity = std::numeric_limits<double>::infinity();
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const nautiloid_location::State inputs[] = {
    {iris::FRACTAL_MANDELBROT, -3.f, -9.0, 9.0},
    {iris::FRACTAL_NOVA, 9.f, 9.0, -9.0},
    {-100, nan, infinity, -infinity},
  };
  bool idempotent = true;
  for (const auto& input : inputs) {
    const auto once = nautiloid_location::canonicalize(input);
    const auto twice = nautiloid_location::canonicalize(once);
    idempotent = idempotent && equalState(once, twice);
  }
  check("canonicalization is exactly idempotent", idempotent);
  const auto fallback = nautiloid_location::canonicalize(inputs[2]);
  check("non-finite and invalid values use deterministic defaults",
        fallback.mode == iris::FRACTAL_MANDELBROT && fallback.zoom == 0.f &&
        std::fabs(fallback.centerX) < 1e-9 && std::fabs(fallback.centerY) < 1e-9);
}

void testIrisPixelEquivalence() {
  const int modes[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13};
  bool all = true;
  for (int mode : modes) {
    nautiloid_location::State requested {mode, 1.375f, -0.3125, 0.21875};
    const auto canonical = nautiloid_location::canonicalize(requested);
    const auto decoded = nautiloid_location::decode(nautiloid_location::encode(canonical));
    iris::NautiloidFractalSourceParams aParams;
    aParams.mode = canonical.mode;
    aParams.zoom = canonical.zoom;
    aParams.centerX = canonical.centerX;
    aParams.centerY = canonical.centerY;
    iris::NautiloidFractalSourceParams bParams;
    bParams.mode = decoded.state.mode;
    bParams.zoom = decoded.state.zoom;
    bParams.centerX = decoded.state.centerX;
    bParams.centerY = decoded.state.centerY;
    iris::SourceField a;
    iris::SourceField b;
    std::string error;
    all = all && decoded.valid &&
      iris::makeNautiloidIrisSource(aParams, &a, &error) &&
      iris::makeNautiloidIrisSource(bParams, &b, &error) &&
      a.width == b.width && a.height == b.height &&
      a.channels == b.channels && a.bitDepth == b.bitDepth && a.rgb8 == b.rgb8;
  }
  check("location round trips reproduce every built-in Iris source byte-for-byte", all);
}

} // namespace

int main() {
  testFixedVectors();
  testRoundTrips();
  testMalformedInput();
  testCanonicalization();
  testIrisPixelEquivalence();
  std::cout << "[SUMMARY] checks=" << checks << " failures=" << failures << "\n";
  return failures == 0 ? 0 : 1;
}
