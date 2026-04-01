#include "TemporalDeck.hpp"

const char *TemporalDeck::cartridgeLabelFor(int index) {
  switch (index) {
  case CARTRIDGE_M44_7:
    return "M44-7";
  case CARTRIDGE_ORTOFON_SCRATCH:
    return "C.MKII S";
  case CARTRIDGE_STANTON_680HP:
    return "680 HP";
  case CARTRIDGE_QBERT:
    return "Q.Bert";
  case CARTRIDGE_LOFI:
    return "Lo-Fi";
  case CARTRIDGE_CLEAN:
  default:
    return "Clean";
  }
}

CartridgeVisualStyle TemporalDeck::cartridgeVisualStyleFor(int index) {
  switch (index) {
  case CARTRIDGE_M44_7:
    return {nvgRGBA(26, 26, 26, 238), nvgRGBA(110, 110, 118, 190), nvgRGBA(252, 252, 252, 235)};
  case CARTRIDGE_ORTOFON_SCRATCH:
    return {nvgRGBA(242, 242, 242, 240), nvgRGBA(26, 26, 26, 210), nvgRGBA(18, 18, 18, 228)};
  case CARTRIDGE_STANTON_680HP:
    return {nvgRGBA(180, 186, 194, 238), nvgRGBA(120, 126, 134, 195), nvgRGBA(24, 24, 28, 230)};
  case CARTRIDGE_QBERT:
    return {nvgRGBA(34, 35, 40, 240), nvgRGBA(240, 242, 246, 210), nvgRGBA(248, 200, 58, 235)};
  case CARTRIDGE_LOFI:
    // Brand purple palette from HSV:
    // bright = overridden to RGB (87, 64, 191)
    // dim    = overridden to RGB (35, 28, 74)
    return {nvgRGBA(35, 28, 74, 238), nvgRGBA(87, 64, 191, 205), nvgRGBA(87, 64, 191, 224)};
  case CARTRIDGE_CLEAN:
  default:
    return {nvgRGBA(90, 178, 187, 236), nvgRGBA(12, 41, 45, 190), nvgRGBA(0, 0, 0, 235)};
  }
}

const char *TemporalDeck::scratchInterpolationLabelFor(int index) {
  switch (index) {
  case SCRATCH_INTERP_LAGRANGE6:
    return "6-point Lagrange";
  case SCRATCH_INTERP_SINC:
    return "Sinc (CPU heavy)";
  case SCRATCH_INTERP_CUBIC:
  default:
    return "Cubic";
  }
}

const char *TemporalDeck::slipReturnLabelFor(int index) {
  switch (index) {
  case SLIP_RETURN_SLOW:
    return "Slow";
  case SLIP_RETURN_INSTANT:
    return "Instant";
  case SLIP_RETURN_NORMAL:
  default:
    return "Normal";
  }
}

const char *TemporalDeck::bufferDurationLabelFor(int index) {
  switch (index) {
  case BUFFER_DURATION_20S:
    return "20 s";
  case BUFFER_DURATION_10M_STEREO:
    return "10 min stereo";
  case BUFFER_DURATION_10M_MONO:
    return "10 min mono";
  case BUFFER_DURATION_10S:
  default:
    return "10 s";
  }
}

const char *TemporalDeck::externalGatePosLabelFor(int index) {
  switch (index) {
  case EXTERNAL_GATE_POS_MODULE_SYNC:
    return "Module sync";
  case EXTERNAL_GATE_POS_GLIDE:
  default:
    return "Glide / inertia";
  }
}

const char *TemporalDeck::platterArtModeLabelFor(int index) {
  switch (index) {
  case PLATTER_ART_DRAGON_KING:
    return "Dragon King";
  case PLATTER_ART_BLANK:
    return "Blank";
  case PLATTER_ART_TEMPORAL_DECK:
    return "Temporal Deck";
  case PLATTER_ART_PROCEDURAL:
    return "Procedural";
  case PLATTER_ART_CUSTOM:
    return "Custom file";
  case PLATTER_ART_BUILTIN_SVG:
  default:
    return "Built-in SVG";
  }
}

const char *TemporalDeck::platterBrightnessLabelFor(int index) {
  switch (index) {
  case PLATTER_BRIGHTNESS_LOW:
    return "Low";
  case PLATTER_BRIGHTNESS_MEDIUM:
    return "Medium";
  case PLATTER_BRIGHTNESS_FULL:
  default:
    return "Full";
  }
}
