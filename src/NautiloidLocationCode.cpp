#include "NautiloidLocationCode.hpp"

#include <array>
#include <cmath>
#include <limits>

namespace nautiloid_location {
namespace {

constexpr char kBase64UrlAlphabet[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

double clampFinite(double value, double fallback, double minimum, double maximum) {
  if (!std::isfinite(value)) return fallback;
  if (value < minimum) return minimum;
  if (value > maximum) return maximum;
  return value;
}

uint16_t encodeZoom(float value) {
  const double zoom = clampFinite(double(value), 0.0, double(kMinZoom), double(kMaxZoom));
  const double normalized = (zoom - double(kMinZoom)) / double(kMaxZoom - kMinZoom);
  return uint16_t(std::llround(normalized * double(std::numeric_limits<uint16_t>::max())));
}

float decodeZoom(uint16_t encoded) {
  return float(double(kMinZoom) +
    double(encoded) / double(std::numeric_limits<uint16_t>::max()) *
      double(kMaxZoom - kMinZoom));
}

uint32_t encodeCoordinate(double value) {
  const double coordinate = clampFinite(value, 0.0, kMinCoordinate, kMaxCoordinate);
  const double normalized =
    (coordinate - kMinCoordinate) / (kMaxCoordinate - kMinCoordinate);
  return uint32_t(std::llround(
    normalized * double(std::numeric_limits<uint32_t>::max())));
}

double decodeCoordinate(uint32_t encoded) {
  return kMinCoordinate +
    double(encoded) / double(std::numeric_limits<uint32_t>::max()) *
      (kMaxCoordinate - kMinCoordinate);
}

uint8_t crc8Atm(const uint8_t* data, size_t size) {
  uint8_t crc = 0u;
  for (size_t i = 0u; i < size; ++i) {
    crc ^= data[i];
    for (int bit = 0; bit < 8; ++bit) {
      crc = (crc & 0x80u) ? uint8_t((crc << 1u) ^ 0x07u) : uint8_t(crc << 1u);
    }
  }
  return crc;
}

void write16(std::array<uint8_t, kPayloadSize>* payload, size_t offset, uint16_t value) {
  (*payload)[offset] = uint8_t(value >> 8u);
  (*payload)[offset + 1u] = uint8_t(value);
}

void write32(std::array<uint8_t, kPayloadSize>* payload, size_t offset, uint32_t value) {
  (*payload)[offset] = uint8_t(value >> 24u);
  (*payload)[offset + 1u] = uint8_t(value >> 16u);
  (*payload)[offset + 2u] = uint8_t(value >> 8u);
  (*payload)[offset + 3u] = uint8_t(value);
}

uint16_t read16(const std::array<uint8_t, kPayloadSize>& payload, size_t offset) {
  return uint16_t((uint16_t(payload[offset]) << 8u) |
                  uint16_t(payload[offset + 1u]));
}

uint32_t read32(const std::array<uint8_t, kPayloadSize>& payload, size_t offset) {
  return (uint32_t(payload[offset]) << 24u) |
         (uint32_t(payload[offset + 1u]) << 16u) |
         (uint32_t(payload[offset + 2u]) << 8u) |
         uint32_t(payload[offset + 3u]);
}

int decodeBase64UrlChar(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a' + 26;
  if (c >= '0' && c <= '9') return c - '0' + 52;
  if (c == '-') return 62;
  if (c == '_') return 63;
  return -1;
}

std::string trimAsciiWhitespace(const std::string& text) {
  size_t begin = 0u;
  size_t end = text.size();
  auto whitespace = [](unsigned char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
  };
  while (begin < end && whitespace(uint8_t(text[begin]))) ++begin;
  while (end > begin && whitespace(uint8_t(text[end - 1u]))) --end;
  return text.substr(begin, end - begin);
}

std::string encodePayload(const std::array<uint8_t, kPayloadSize>& payload) {
  std::string code;
  code.reserve(kEncodedLength);
  for (size_t i = 0u; i < kPayloadSize; i += 3u) {
    const uint32_t bits = (uint32_t(payload[i]) << 16u) |
                          (uint32_t(payload[i + 1u]) << 8u) |
                          uint32_t(payload[i + 2u]);
    code.push_back(kBase64UrlAlphabet[(bits >> 18u) & 0x3fu]);
    code.push_back(kBase64UrlAlphabet[(bits >> 12u) & 0x3fu]);
    code.push_back(kBase64UrlAlphabet[(bits >> 6u) & 0x3fu]);
    code.push_back(kBase64UrlAlphabet[bits & 0x3fu]);
  }
  return code;
}

bool decodePayload(const std::string& code,
                   std::array<uint8_t, kPayloadSize>* payload,
                   std::string* error) {
  for (size_t i = 0u; i < kEncodedLength; i += 4u) {
    const int a = decodeBase64UrlChar(code[i]);
    const int b = decodeBase64UrlChar(code[i + 1u]);
    const int c = decodeBase64UrlChar(code[i + 2u]);
    const int d = decodeBase64UrlChar(code[i + 3u]);
    if (a < 0 || b < 0 || c < 0 || d < 0) {
      if (error) *error = "Invalid Base64URL character";
      return false;
    }
    const uint32_t bits = (uint32_t(a) << 18u) | (uint32_t(b) << 12u) |
                          (uint32_t(c) << 6u) | uint32_t(d);
    const size_t output = (i / 4u) * 3u;
    (*payload)[output] = uint8_t(bits >> 16u);
    (*payload)[output + 1u] = uint8_t(bits >> 8u);
    (*payload)[output + 2u] = uint8_t(bits);
  }
  return true;
}

} // namespace

State canonicalize(const State& requested) {
  State canonical;
  canonical.mode = iris::isBuiltinFractalMode(requested.mode)
    ? requested.mode : iris::FRACTAL_MANDELBROT;
  canonical.zoom = decodeZoom(encodeZoom(requested.zoom));
  canonical.centerX = decodeCoordinate(encodeCoordinate(requested.centerX));
  canonical.centerY = decodeCoordinate(encodeCoordinate(requested.centerY));
  return canonical;
}

std::string encode(const State& requested) {
  const State state = canonicalize(requested);
  std::array<uint8_t, kPayloadSize> payload {};
  payload[0] = uint8_t((kFormatVersion << 4u) | (state.mode & 0x0f));
  write16(&payload, 1u, encodeZoom(state.zoom));
  write32(&payload, 3u, encodeCoordinate(state.centerX));
  write32(&payload, 7u, encodeCoordinate(state.centerY));
  payload[11] = crc8Atm(payload.data(), 11u);
  return encodePayload(payload);
}

DecodeResult decode(const std::string& input) {
  DecodeResult result;
  const std::string text = trimAsciiWhitespace(input);
  if (text.size() != kEncodedLength) {
    result.error = "Location code must contain exactly 16 characters";
    return result;
  }

  std::array<uint8_t, kPayloadSize> payload {};
  if (!decodePayload(text, &payload, &result.error)) return result;

  const uint8_t version = uint8_t(payload[0] >> 4u);
  const int mode = int(payload[0] & 0x0fu);
  if (version == 0u) {
    result.error = "Location code version zero is invalid";
    return result;
  }
  if (version != kFormatVersion) {
    result.error = "Unsupported location code version";
    return result;
  }
  if (!iris::isBuiltinFractalMode(mode)) {
    result.error = "Invalid fractal mode";
    return result;
  }
  if (crc8Atm(payload.data(), 11u) != payload[11]) {
    result.error = "Location code checksum mismatch";
    return result;
  }

  result.state.mode = mode;
  result.state.zoom = decodeZoom(read16(payload, 1u));
  result.state.centerX = decodeCoordinate(read32(payload, 3u));
  result.state.centerY = decodeCoordinate(read32(payload, 7u));
  result.valid = true;
  return result;
}

bool isValid(const std::string& text) {
  return decode(text).valid;
}

} // namespace nautiloid_location
