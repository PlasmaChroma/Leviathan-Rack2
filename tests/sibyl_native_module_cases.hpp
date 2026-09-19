#pragma once
#include "sibyl_native_fixture.hpp"
namespace {
void testNativeHarmonyPlayback() {
  using namespace sibyl;
  for (int n : {38, 53})
    for (int rate : {44100, 48000, 96000}) {
      auto parsed = parseCompositionJson(nativeHarmonyFixture(n), 1);
      check(parsed.valid, "P7C native harmony module fixture compiles");
      if (!parsed.valid)
        continue;
      SibylModule module;
      module.outputs[SibylModule::V_OCT_OUTPUT].channels = 1;
      module.outputs[SibylModule::GATE_OUTPUT].channels = 1;
      module.acceptComposition(parsed.composition, ApplyAt::IMMEDIATE, PhasePolicy::RESTART_ALL);
      rack::engine::Module::ProcessArgs args{};
      args.sampleRate = float(rate);
      args.sampleTime = 1.f / rate;
      for (int i = 0; i < 100; ++i) {
        args.frame = i;
        module.process(args);
      }
      check(std::abs(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage() - 17. / n) < 1e-6 &&
                module.outputs[SibylModule::GATE_OUTPUT].getVoltage() > 0,
            "P7C actual native third output at 44.1/48/96 kHz");
      json_t *undo = module.dataToJson();
      std::string response, error;
      const std::string operations =
          R"([{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"transposeSteps":1,"transposePeriods":1,"transposeCents":-1.5}}}])";
      const auto *accepted = module.m_acceptedCompositionPtr;
      check(module.handleSibylRequest(SibylControl::Operation::VALIDATE,
                                      "{\"expected_revision\":1,\"operations\":" + operations + "}", response,
                                      error) &&
                response.find("\"valid\":true") != std::string::npos &&
                module.m_acceptedCompositionPtr == accepted,
            "P7C preview leaves accepted native score unchanged");
      check(module.handleSibylRequest(SibylControl::Operation::EDIT,
                                      "{\"expected_revision\":1,\"apply_at\":\"nextBeat\",\"phase_policy\":"
                                      "\"restartAll\",\"operations\":" +
                                          operations + "}",
                                      response, error),
            "P7C native transform transaction accepted");
      gAllocationCount = gDeallocationCount = 0;
      gTrackAllocations = true;
      for (int i = 0; i < rate; ++i) {
        args.frame = 100 + i;
        module.process(args);
      }
      gTrackAllocations = false;
      check(gAllocationCount == 0 && gDeallocationCount == 0,
            "P7C native harmony playback and quantized adoption allocate/free nothing");
      check(module.m_activeRevision.load() == 2, "P7C native pitch edit adopts without user intervention");
      check(module.handleSibylRequest(SibylControl::Operation::TRANSPORT,
                                      R"({"action":"restart","target":"arrangement","apply_at":"immediate"})",
                                      response, error),
            "P7C restart native scene for post-adoption output check");
      for (int i = 0; i < 100; ++i) {
        args.frame = rate + 100 + i;
        module.process(args);
      }
      check(std::abs(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage() - (18. / n + 1. - 1.5 / 1200.)) <
                1e-6,
            "P7C actual output applies event/context/scene transforms exactly once after adoption");
      json_t *redo = module.dataToJson();
      SibylModule restored;
      restored.dataFromJson(redo);
      check(restored.m_acceptedCompositionPtr &&
                restored.m_acceptedCompositionPtr->progressions[0].chords[0].tones.size() == 3,
            "P7C patch reload retains native tones and roles");
      module.dataFromJson(undo);
      check(module.m_acceptedCompositionPtr->arrangement[0].tracks.at("v").overrides.pitchOffsets.fields == 0,
            "P7C undo-state restores original native offsets");
      module.dataFromJson(redo);
      check(module.m_acceptedCompositionPtr->arrangement[0].tracks.at("v").overrides.pitchOffsets.steps == 1,
            "P7C redo-state restores native transforms");
      check(module.handleSibylRequest(SibylControl::Operation::GET_COMPOSITION,
                                      R"({"view":"export_tuning_scl","id":"t"})", response, error) &&
                response.find("Scala") != std::string::npos,
            "P7D Scala export passes real module dispatch");
      check(module.handleSibylRequest(
                SibylControl::Operation::GET_COMPOSITION,
                R"({"view":"map_intervals","context_id":"c","intervals":[{"ratio":"3/2"}]})", response,
                error) &&
                response.find("realizedCents") != std::string::npos,
            "P7D interval mapping passes real module dispatch");
      json_decref(undo);
      json_decref(redo);
    }
}
} // namespace
