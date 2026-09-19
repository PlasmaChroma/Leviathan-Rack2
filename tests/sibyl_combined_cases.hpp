namespace {
void testCombinedComposition() {
  using namespace sibyl;
  std::ifstream file("doc/Sibyl_v3_Example_Composition.json");
  std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  auto parsed = parseCompositionJson(source, 1);
  if (!parsed.valid) {
    check(false, "P6 combined source compiles");
    return;
  }
  const std::string operations = R"([
    {"op":"upsert_track","id":"chord_low","track":{"id":"chord_low","channel":4}},
    {"op":"upsert_track","id":"chord_mid","track":{"id":"chord_mid","channel":5}},
    {"op":"upsert_track","id":"chord_high","track":{"id":"chord_high","channel":6}},
    {"op":"voice_progression","progression_id":"main_changes","scene_id":"chorus","resolution":"1/16",
     "voices":[{"track_id":"chord_low","pattern_id":"chorus_low","min":"C2","max":"C3"},
               {"track_id":"chord_mid","pattern_id":"chorus_mid","min":"C3","max":"C4"},
               {"track_id":"chord_high","pattern_id":"chorus_high","min":"C4","max":"C5"}]}])";
  auto decode = [](const std::string &text) {
    json_error_t e{};
    return json_loads(text.c_str(), 0, &e);
  };
  for (int rate : {44100, 48000, 96000}) {
    SibylModule module;
    module.acceptComposition(parsed.composition, ApplyAt::IMMEDIATE, PhasePolicy::RESTART_ALL);
    rack::engine::Module::ProcessArgs args{};
    args.sampleRate = float(rate);
    args.sampleTime = 1.f / rate;
    module.process(args);
    std::string response, error;
    module.handleSibylRequest(SibylControl::Operation::TRANSPORT, R"({"action":"play","apply_at":"immediate"})",
                              response, error);
    for (int i = 0; i < 100; ++i)
      module.process(args);
    json_t *before = module.dataToJson();
    auto *accepted = module.m_acceptedCompositionPtr;
    auto *pending = module.m_pendingAdoptionPtr.load();
    bool ok = module.handleSibylRequest(SibylControl::Operation::VALIDATE,
                                        "{\"expected_revision\":1,\"operations\":" + operations + "}", response, error);
    json_t *preview = decode(response);
    check(ok && json_is_true(json_object_get(preview, "valid")) && module.m_acceptedCompositionPtr == accepted &&
              module.m_pendingAdoptionPtr.load() == pending,
          "P6 combined voicing preview leaves accepted/active state untouched");
    ok = module.handleSibylRequest(
        SibylControl::Operation::EDIT,
        "{\"expected_revision\":1,\"apply_at\":\"nextBeat\",\"operations\":" + operations + "}", response, error);
    json_t *committed = decode(response);
    check(ok && module.m_acceptedRevision == 2 && module.m_activeRevision.load() == 1 &&
              json_equal(json_object_get(preview, "changes"), json_object_get(committed, "changes")),
          "P6 preview and nextBeat commit produce identical voicings and reports");
    json_decref(preview);
    json_decref(committed);
    if (!ok) {
      std::cerr << response << " " << error << "\n";
      json_decref(before);
      continue;
    }
    ok = module.handleSibylRequest(
        SibylControl::Operation::EDIT,
        R"({"expected_revision":2,"apply_at":"nextBeat","operations":[{"op":"set_meta","path":"title","value":"P6 combined accepted replacement"}]})",
        response, error);
    check(ok && module.m_acceptedRevision == 3 && module.m_activeRevision.load() == 1 &&
              module.m_acceptedCompositionPtr->patterns.count("chorus_low"),
          "P6 edit while pending retains generated patterns in replacement candidate");
    gAllocationCount = gDeallocationCount = 0;
    bool adopted = false, modCorrect = true, rootCorrect = true, accentCorrect = true, fillCorrect = true,
         generatedCorrect = true;
    int bassOnsets = 0, accents = 0, fills = 0, generated = 0, hatPlayed = 0, hatSkipped = 0;
    int64_t lastBass = -999, lastHat = -999, lastVoice = -999;
    for (int frame = 0; frame < rate * 24; ++frame) {
      args.frame = frame;
      gTrackAllocations = true;
      module.process(args);
      gTrackAllocations = false;
      adopted |= module.m_activeRevision.load() == 3;
      const int scene = module.m_sceneIndex, repeat = module.m_sceneRepeat;
      const auto &bass = module.m_trackStates[0];
      const auto &hats = module.m_trackStates[2];
      if (scene == 0) {
        double visit = repeat * 8. + module.m_scenePhase;
        modCorrect &= std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage(0) - visit * 5. / 32.) < 2e-4;
        double x = visit / 32.;
        double sweep = -2. + 4. * x * x * (3. - 2. * x);
        modCorrect &= std::abs(module.outputs[SibylModule::MOD_2_OUTPUT].getVoltage(3) - sweep) < 2e-4;
      }
      if (bass.activeNominalStep != lastBass) {
        lastBass = bass.activeNominalStep;
        if (bass.activeEventStep == 0 && bass.activeEventPlayed) {
          ++bassOnsets;
          double beat = scene == 0 ? repeat * 8. + module.m_scenePhase : 32. + module.m_scenePhase;
          const float roots[] = {-2.f, -2.25f, -31.f / 12.f, -29.f / 12.f};
          int chord = int(beat / 4.) % 4;
          rootCorrect &= std::abs(bass.targetPitch - (roots[chord] + (scene == 1 ? 1.f : 0.f))) < 1e-5;
        }
        if (scene == 0 && bass.activeEventStep == 14) {
          ++accents;
          accentCorrect &= bass.activeEventPlayed == (bass.condition.eventPass(bass.activeNominalStep, 16) % 4 == 0);
        }
      }
      if (hats.activeNominalStep != lastHat) {
        lastHat = hats.activeNominalStep;
        if (hats.activeEventStep == 14) {
          ++fills;
          fillCorrect &= hats.activeEventPlayed == (scene == 1 || repeat == 3);
        } else if (hats.activeEventStep >= 0) {
          if (hats.activeEventPlayed)
            ++hatPlayed;
          else
            ++hatSkipped;
        }
      }
      const auto &voice = module.m_trackStates[4];
      if (scene == 1 && voice.activeNominalStep != lastVoice) {
        lastVoice = voice.activeNominalStep;
        if (voice.activeEventStep >= 0) {
          ++generated;
          const auto &events = module.m_acceptedCompositionPtr->patterns.at("chorus_low").steps;
          for (const auto &event : events)
            if (event.step == voice.activeEventStep)
              generatedCorrect &= voice.activeEventPlayed && voice.targetPitch == event.compiledPitchV &&
                                  std::abs(voice.activeEventGate - 15.2f) < 1e-4;
        }
      }
    }
    check(adopted && gAllocationCount == 0 && gDeallocationCount == 0,
          "P6 combined nextBeat adoption and full arrangement allocate/free nothing at supported sample rates");
    check(modCorrect, "P6 combined 32-beat MOD and patternless smoothstep remain sample-correct");
    check(rootCorrect && bassOnsets >= 8, "P6 combined harmonic bass follows progression and chorus octave override");
    if (!(accentCorrect && accents >= 6 && fillCorrect && fills >= 10 && hatPlayed > 0 && hatSkipped > 0))
      std::cerr << "combined counters " << rate << " accent=" << accentCorrect << "/" << accents
                << " fill=" << fillCorrect << "/" << fills << " hats=" << hatPlayed << "/" << hatSkipped << "\n";
    check(accentCorrect && accents >= 6 && fillCorrect && fills >= 10 && hatPlayed > 0 && hatSkipped > 0,
          "P6 combined every-fourth accents, protected finale and varied hats survive repeats");
    check(generatedCorrect && generated >= 4,
          "P6 generated chorus pitches and sustained gates render alongside relative harmony");
    json_t *after = module.dataToJson();
    SibylModule restored;
    restored.dataFromJson(after);
    json_t *savedDoc = decode(serializeFullCompositionJson(*module.m_acceptedCompositionPtr));
    json_t *restoredDoc = restored.m_acceptedCompositionPtr
                              ? decode(serializeFullCompositionJson(*restored.m_acceptedCompositionPtr))
                              : nullptr;
    // Patch reload receives a new local revision; compare authored composition.
    check(restoredDoc &&
              json_equal(json_object_get(savedDoc, "composition"), json_object_get(restoredDoc, "composition")),
          "P6 combined Rack save/load preserves expressive fields and generated IDs");
    json_decref(savedDoc);
    json_decref(restoredDoc);
    module.dataFromJson(before);
    check(!module.m_acceptedCompositionPtr->patterns.count("chorus_low"),
          "P6 undo snapshot removes complete voicing transaction");
    module.dataFromJson(after);
    check(module.m_acceptedCompositionPtr->patterns.at("chorus_low").steps[0].id == "n1",
          "P6 redo snapshot restores generated IDs");
    json_decref(before);
    json_decref(after);
  }
}
} // namespace
