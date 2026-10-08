#include "VTune.hpp"
#include "Vessel.hpp"
#include "vtune/BodyMapCore.hpp"
#include "vessel/PitchColorMap.hpp"
#include <cmath>

// Called from VTune::process(). Copy only UI telemetry from the neighboring
// Vessel on the engine side; the widget never dereferences expander pointers.
// This changes neither the TuneMessage layout nor Vessel's DSP/control model.
void VTune::updateBodyMapFrequency(float sampleTime) {
    bodyMapPublishElapsed += sampleTime;
    if (bodyMapPublishElapsed < .005f) return;
    const float elapsed = bodyMapPublishElapsed;
    bodyMapPublishElapsed = 0.f;
    float hz = 0.f;
    const Module* neighbor = leftExpander.module;
    if (neighbor && neighbor->model == modelVessel && neighbor->rightExpander.module == this) {
        const auto* vessel = static_cast<const Vessel*>(neighbor);
        hz = vessel->visualFrequency.load(std::memory_order_relaxed);
        if (!vtune_body::validFrequency(hz)) hz = 0.f;
    }
    // One atomic encodes both value and availability. No mixed linked/value
    // snapshot can occur, and a removed source is never cached in the widget.
    bodyFrequencyHz.store(hz, std::memory_order_relaxed);
    const auto weights = vessel_pitch_color::weights(hz);
    const float blend = std::min(1.f, 5.f * elapsed);
    for (int i = 0; i < vessel_pitch_color::count; ++i) {
        auto& light = lights[CHAKRA_ROOT_LIGHT + i];
        const float current = light.getBrightness();
        light.setBrightness(current + blend * (weights[i] - current));
    }
}

void VTune::onExpanderChange(const ExpanderChangeEvent& e) {
    Module::onExpanderChange(e);
    if (e.side == 0) {
        adoptedVessel = nullptr;
        bodyFrequencyHz.store(0.f, std::memory_order_relaxed);
        bodyMapPublishElapsed = 1.f;
        for (int i = CHAKRA_ROOT_LIGHT; i < LIGHTS_LEN; ++i) lights[i].setBrightness(0.f);
    }
}

void VTune::processBypass(const ProcessArgs&) {
    adoptVesselSettings();
    for (int i = CHAKRA_ROOT_LIGHT; i < LIGHTS_LEN; ++i) lights[i].setBrightness(0.f);
    bodyFrequencyHz.store(0.f, std::memory_order_relaxed);
    bodyMapPublishElapsed = 1.f;
    lights[VESSEL_LINK_LIGHT].setBrightness(0.f);
    lights[VESSEL_READY_LIGHT].setBrightness(0.f);
}

void VTune::onReset(const ResetEvent& e) {
    Module::onReset(e);
    for (int i = CHAKRA_ROOT_LIGHT; i < LIGHTS_LEN; ++i) lights[i].setBrightness(0.f);
    bodyFrequencyHz.store(0.f, std::memory_order_relaxed);
    bodyMapMode.store(int(vtune_body::Mode::Report), std::memory_order_relaxed);
    bodyMapOpacity.store(vtune_body::kDefaultOpacity, std::memory_order_relaxed);
    bodyMapPublishElapsed = 1.f;
}

json_t* VTune::dataToJson() {
    json_t* root = json_object();
    json_object_set_new(root, "bodyMapSchema", json_integer(1));
    json_object_set_new(root, "bodyMapMode", json_integer(int(vtune_body::sanitizeMode(bodyMapMode.load(std::memory_order_relaxed)))));
    json_object_set_new(root, "bodyMapOpacity", json_real(vtune_body::unit(bodyMapOpacity.load(std::memory_order_relaxed))));
    return root;
}

void VTune::dataFromJson(json_t* root) {
    // Runtime telemetry is never serialized. Old patches with no body-map
    // fields inherit defaults, while unknown schema versions fail closed.
    bodyFrequencyHz.store(0.f, std::memory_order_relaxed);
    // Leave the non-atomic timer owned by process()/engine events; JSON
    // restoration need not be synchronized with the audio callback. The next
    // normal publication arrives within approximately one 5 ms interval.
    bodyMapMode.store(int(vtune_body::Mode::Report), std::memory_order_relaxed);
    bodyMapOpacity.store(vtune_body::kDefaultOpacity, std::memory_order_relaxed);
    if (!json_is_object(root)) return;
    const json_t* schema = json_object_get(root, "bodyMapSchema");
    if (schema && (!json_is_integer(schema) || json_integer_value(schema) != 1)) {
        bodyMapMode.store(int(vtune_body::Mode::Off), std::memory_order_relaxed);
        return;
    }
    const json_t* mode = json_object_get(root, "bodyMapMode");
    if (json_is_integer(mode)) {
        const json_int_t value = json_integer_value(mode);
        if (value >= int(vtune_body::Mode::Report) && value <= int(vtune_body::Mode::Off))
            bodyMapMode.store(int(vtune_body::sanitizeMode(static_cast<int>(value))), std::memory_order_relaxed);
    }
    const json_t* opacity = json_object_get(root, "bodyMapOpacity");
    if (json_is_number(opacity)) {
        const double value = json_number_value(opacity);
        if (std::isfinite(value))
            bodyMapOpacity.store(static_cast<float>(std::max(0.0, std::min(1.0, value))), std::memory_order_relaxed);
    }
}
