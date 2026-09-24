#include "../src/plugin.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>

Plugin* pluginInstance = nullptr;
#include "../src/Chimera.cpp"

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}
int main() {
    need(modelChimera && modelChimera->slug == "Chimera", "registered model slug");
    Chimera module;
    need(module.getNumParams() == 12 && module.getNumInputs() == 13 &&
         module.getNumOutputs() == 4 && module.getNumLights() == 9,
         "Rack-facing schema counts");
    need(module.reel->capacityFrames() == chimera::kMaxReelFrames,
         "full off-audio Reel preparation");
    Module::ProcessArgs args{};
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f/48000.f;
    module.inputs[Chimera::AUDIO_L_INPUT].channels = 1;
    module.inputs[Chimera::AUDIO_L_INPUT].setVoltage(5.f);
    module.process(args);
    chimera::AudioCompletion adopted{};
    need(module.completions.tryPop(adopted) && adopted.kind == 1 && adopted.handle == 1,
         "audio accepted registry-owned handle through bounded command queue");
    need(std::fabs(module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage()-5.f) < 0.001f &&
         std::fabs(module.outputs[Chimera::AUDIO_R_OUTPUT].getVoltage()-5.f) < 0.001f,
         "L-only input monitors to both channels");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    module.params[Chimera::REC_PARAM].setValue(0.f);
    for (int i = 0; i < 3; ++i) module.process(args);
    need(module.reel->validFrames() == 4, "Rack process records four exact frames");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    need(module.reel->validFrames() == 4 && module.slice.recordState() == chimera::Slice::Idle,
         "stop edge excludes current frame");
    json_t* data = module.dataToJson();
    need(data && json_is_string(json_object_get(data, "audioStatus")),
         "development JSON explicitly declares unsaved audio");
    json_object_set_new(data, "inop", json_true());
    module.dataFromJson(data);
    need(module.inopSetting.load(), "writer source option persists through JSON");
    json_decref(data);
    args.sampleRate = 96000.f;
    args.sampleTime = 1.f/96000.f;
    module.process(args);
    need(module.reel->validFrames() == 4 && module.lights[Chimera::ERROR_LIGHT].getBrightness() == 1.f,
         "unsupported host rate never writes incorrect Reel time");
    std::puts("PASS: Chimera registered Rack module schema, monitoring, recording, and rate guard");
}
