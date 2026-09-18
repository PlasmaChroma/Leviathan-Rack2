namespace {
void testHarmony() {
  using namespace sibyl;
  const char *fixture =
      R"({"schemaVersion":3,"meta":{"bpm":60},"tracks":[{"id":"v","channel":0},{"id":"drums","channel":1}],
      "patterns":{"p":{"length":4,"resolution":"1/4","steps":[{"id":"root","step":0,"harmonic":{"kind":"tone","index":0},"gate":4},{"id":"third","step":1,"harmonic":{"kind":"tone","role":"third"},"gate":3}]},
      "d":{"length":4,"steps":[{"step":0,"note":"C2"}]}},
      "arrangement":[{"id":"s","lengthBeats":4,"repeats":2,"tracks":{"v":"p","drums":"d"}},{"id":"t","lengthBeats":4,"tracks":{"v":"p","drums":"d"}}],
      "harmony":{"progressions":{"changes":{"lengthBeats":4,"chords":[{"id":"c","beat":0,"root":"C3","intervals":[0,4,7]},{"id":"a","beat":1,"root":"A2","intervals":[0,3,7]}]}},"default":{"progression":"changes","clock":"arrangement","loop":true}}})";
  auto decode = [](const std::string &s) {
    json_error_t e{};
    return json_loads(s.c_str(), 0, &e);
  };
  auto compile = [&](json_t *root, int revision = 1) {
    return parseCompositionJson(harmony_json::dump(root), revision);
  };
  auto base = parseCompositionJson(fixture, 1);
  check(base.valid, "P5 harmony fixture compiles");
  if (!base.valid) {
    for (const auto &e : base.errors)
      std::cerr << e.path << ": " << e.message << "\n";
    return;
  }
  auto &p = base.composition->patterns.at("p");
  check(contextualPitch(*base.composition, p.steps[0], 0, 0, 0) == -1.f &&
            contextualPitch(*base.composition, p.steps[0], 0, 0, 1) == -1.25f,
        "H01 root follows C3 to A2");
  check(std::abs(contextualPitch(*base.composition, p.steps[1], 0, 0, 0) - (-8.f / 12)) < 1e-6 &&
            contextualPitch(*base.composition, p.steps[1], 0, 0, 1) == -1.f,
        "H02 third resolves major and minor chord roles");
  auto round = parseCompositionJson(serializeFullCompositionJson(*base.composition), 2);
  check(round.valid && round.composition->patterns.at("p").steps[1].harmonic == p.steps[1].harmonic &&
            round.composition->harmonicPitchEntries == 4,
        "P5 authored round trip and interned expression/chord entries");
  auto edit = [&](const Composition &comp, const std::string &text, int rev = 2) {
    json_t *ops = decode(text);
    auto r = applyCompositionEdit(comp, ops, rev);
    json_decref(ops);
    return r;
  };
  auto chordAt = [](json_t *root, int index) {
    return json_array_get(
        json_object_get(json_object_get(json_object_get(json_object_get(root, "harmony"), "progressions"), "changes"),
                        "chords"),
        index);
  };
  {
    json_t *root = decode(fixture);
    json_object_set_new(chordAt(root, 0), "intervals", decode("[0,2,7]"));
    check(!compile(root).valid, "H03 suspended chord does not invent third");
    json_object_set_new(chordAt(root, 0), "intervals", decode("[0,3,4,7]"));
    check(!compile(root).valid, "P5 ambiguous third rejected");
    json_object_set_new(chordAt(root, 0), "intervals", decode("[0,4,7,12]"));
    check(!compile(root).valid, "P5 duplicate pitch class rejected");
    json_decref(root);
  }
  for (const char *kind : {"lower", "higher"}) {
    json_t *root = decode(fixture);
    json_t *steps = json_object_get(json_object_get(json_object_get(root, "patterns"), "p"), "steps");
    json_array_clear(steps);
    json_t *e = decode(
        R"({"id":"near","step":0,"harmonic":{"kind":"nearest","reference":{"note":"D3"},"range":{"min":"C3","max":"C5"}}})");
    json_object_set_new(json_object_get(e, "harmonic"), "tieBreak", json_string(kind));
    json_array_append_new(steps, e);
    auto r = compile(root);
    check(r.valid, "P5 nearest expression compiles");
    if (r.valid)
      check(std::abs(contextualPitch(*r.composition, r.composition->patterns.at("p").steps[0], 0, 0, 0) -
                     (std::string(kind) == "lower" ? -1.f : -8.f / 12)) < 1e-6,
            "H04/H05 nearest tie direction is deterministic");
    json_object_set_new(json_object_get(json_object_get(e, "harmonic"), "range"), "min", json_string("C#3"));
    json_object_set_new(json_object_get(json_object_get(e, "harmonic"), "range"), "max", json_string("C#3"));
    check(!compile(root).valid, "P5 empty nearest register rejected");
    json_decref(root);
  }
  {
    json_t *root = decode(fixture);
    json_object_del(root, "harmony");
    check(!compile(root).valid, "H08 assigned harmonic pattern requires harmony");
    json_array_clear(json_object_get(root, "arrangement"));
    auto unbound = compile(root);
    check(unbound.valid && !unbound.warnings.empty() && unbound.warnings[0].code == "unbound_harmony_pattern",
          "H08 unassigned harmonic bank pattern retained with warning");
    json_decref(root);
    auto disabled = edit(*base.composition, R"([{"op":"set_scene_harmony","scene_id":"s","binding":null}])");
    check(!disabled.valid, "H09 explicit disabled binding rejects assigned relative notes");
    auto inherited = edit(
        *base.composition,
        R"([{"op":"set_scene_harmony","scene_id":"s","binding":null},{"op":"inherit_scene_harmony","scene_id":"s"}])");
    check(inherited.valid && !inherited.composition->arrangement[0].harmony.present,
          "H09 removing override restores inheritance atomically");
    auto held = edit(
        *base.composition,
        R"([{"op":"set_scene_harmony","scene_id":"s","binding":{"progression":"changes","clock":"sceneVisit","loop":false}}])");
    check(held.valid &&
              contextualPitch(*held.composition, held.composition->patterns.at("p").steps[0], 0, 1, 0) == -1.25f,
          "P5 nonloop binding holds last chord after progression end");
  }
  {
    auto inUse = edit(*base.composition, R"([{"op":"delete_progression","id":"changes"}])");
    check(!inUse.valid && inUse.errorCode == "object_in_use", "P5 referenced progression deletion rejected");
    auto converted = edit(
        *base.composition,
        R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"note":"C3"}},{"op":"set_default_harmony","binding":null},{"op":"delete_progression","id":"changes"}])");
    check(converted.valid && converted.composition->progressions.empty(),
          "P5 static pitch replacement and detached progression deletion atomic");
    auto transposed = edit(*base.composition,
                           R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"semitones":12}])");
    check(transposed.valid && contextualPitch(*transposed.composition,
                                              transposed.composition->patterns.at("p").steps[0], 0, 0, 0) == 0.f,
          "P5 note transposition applies after harmonic selection");
    auto degree =
        edit(*base.composition, R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"degrees":1}])");
    check(!degree.valid && degree.errorCode == "unsupported_pitch_transform",
          "P5 degree transpose rejects harmonic selection");
  }
  {
    json_t *root = decode(fixture);
    json_object_set_new(chordAt(root, 0), "root", json_string("D3"));
    auto changed = compile(root, 2);
    json_decref(root);
    check(changed.valid && changedTrackChannelMask(*base.composition, *changed.composition) == 1,
          "H10 harmony dependency changes melodic track without marking fixed percussion");
    auto future = edit(
        *base.composition,
        R"([{"op":"set_scene_harmony","scene_id":"t","binding":{"progression":"changes","clock":"sceneRepeat","loop":false}}])");
    check(future.valid && changedTrackChannelMask(*base.composition, *future.composition) == 1,
          "P5 future-scene harmonic dependency participates in adoption");
    json_t *reordered = decode(serializeFullCompositionJson(*base.composition));
    auto sorted = compile(reordered);
    json_decref(reordered);
    check(sorted.valid && changedTrackChannelMask(*base.composition, *sorted.composition) == 0,
          "H11 canonical object order preserves harmonic dependencies");
  }
  {
    json_t *q = decode(R"({"view":"effective_context","scene_id":"s","scene_repeat":0,"beat":1})");
    json_t *response = decode(serializeHarmonyView(*base.composition, q));
    check(json_is_true(json_object_get(response, "ok")) &&
              std::string(json_string_value(
                  json_object_get(json_object_get(json_object_get(response, "derived"), "chord"), "id"))) == "a",
          "P5 context view selects destination chord at exact boundary");
    json_decref(response);
    json_object_set_new(q, "beat", json_integer(4));
    response = decode(serializeHarmonyView(*base.composition, q));
    check(json_is_false(json_object_get(response, "ok")), "P5 context view rejects repeat endpoint");
    json_decref(response);
    json_decref(q);
  }
  {
    // Deliberately overshoot the .995-beat marker: the .99 scheduled onset
    // still belongs to C, while the exactly 1.0 onset belongs to A.
    json_t *root = decode(fixture);
    json_object_set_new(chordAt(root, 1), "beat", json_real(.995));
    json_t *steps = json_object_get(json_object_get(json_object_get(root, "patterns"), "p"), "steps");
    json_array_clear(steps);
    json_array_append_new(
        steps,
        decode(R"({"id":"anticipated","step":1,"harmonic":{"kind":"tone","index":0},"microshift":-0.01,"gate":3})"));
    auto early = compile(root);
    check(early.valid, "P5 microshift harmony fixture compiles");
    auto tick = [](SibylModule &module, int frames) {
      rack::engine::Module::ProcessArgs args{};
      args.sampleRate = 100;
      args.sampleTime = .01f;
      for (int i = 0; i < frames; ++i) {
        args.frame = i;
        gTrackAllocations = true;
        module.process(args);
        gTrackAllocations = false;
      }
    };
    if (early.valid) {
      SibylModule module;
      module.acceptComposition(early.composition, ApplyAt::IMMEDIATE, PhasePolicy::RESTART_ALL);
      tick(module, 103);
      check(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage() == -1.f,
            "H07 delayed boundary sample uses anticipated scheduled onset chord");
      tick(module, 80);
      check(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage() == -1.f,
            "H06 chord change does not retune held pitch");
    }
    json_object_set_new(json_array_get(steps, 0), "microshift", json_real(0.));
    json_object_set_new(chordAt(root, 1), "beat", json_real(1.));
    auto exact = compile(root);
    json_decref(root);
    if (exact.valid) {
      SibylModule module;
      module.acceptComposition(exact.composition, ApplyAt::IMMEDIATE, PhasePolicy::RESTART_ALL);
      tick(module, 103);
      check(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage() == -1.25f,
            "H07 onset exactly at chord marker selects new chord");
      auto popup = module.readVoicingSnapshot();
      check(!popup.rows.empty() && popup.rows[0].pitches[0] == -1.25f,
            "P5 voicing popup uses current scene/time harmony");
    }
    check(gAllocationCount == 0 && gDeallocationCount == 0,
          "P5 harmonic playback/onset/chord boundary has zero audio-thread allocation/free");
  }
  {
    // Scene assignments restrict the compiled contexts, rather than expanding
    // every bank expression against every chord in every progression.
    json_t *root = decode(fixture);
    json_array_remove(json_object_get(root, "arrangement"), 1);
    json_t *scene = json_array_get(json_object_get(root, "arrangement"), 0);
    json_object_set_new(scene, "lengthBeats", json_real(.5));
    json_object_set_new(scene, "repeats", json_integer(1));
    json_object_set_new(chordAt(root, 1), "intervals", decode("[0,2,7]"));
    auto reachable = compile(root);
    check(reachable.valid && reachable.composition->harmonicPitchEntries == 2,
          "P5 unreachable suspended chord does not expand or invalidate table");
    json_decref(root);
  }
  {
    const char *invalidExpressions[] = {
        R"({"kind":"tone","index":0,"role":"root"})", R"({"kind":"tone","index":8})",
        R"({"kind":"tone","index":0,"octave":1.5})",
        R"({"kind":"nearest","reference":{"harmonic":{"kind":"tone","index":0}},"range":{"min":"C3","max":"C4"}})",
        R"({"kind":"nearest","reference":{"note":"C3","pitchV":-1},"range":{"min":"C3","max":"C4"}})"};
    for (const char *expression : invalidExpressions) {
      json_t *root = decode(fixture);
      json_t *event =
          json_array_get(json_object_get(json_object_get(json_object_get(root, "patterns"), "p"), "steps"), 0);
      json_object_set_new(event, "harmonic", decode(expression));
      check(!compile(root).valid, "P5 malformed harmonic expression rejected");
      json_decref(root);
    }
    json_t *root = decode(fixture);
    json_t *definitions = json_object_get(json_object_get(root, "harmony"), "progressions");
    json_t *prototype = json_object_get(definitions, "changes");
    for (int i = 0; i < 128; ++i)
      json_object_set(definitions, ("unused" + std::to_string(i)).c_str(), prototype);
    auto capacity = compile(root);
    check(!capacity.valid && capacity.errors[0].code == "capacity_exceeded",
          "P5 progression count bounded before table compilation");
    json_decref(root);
  }
  {
    auto source = std::make_shared<Composition>(*base.composition);
    source->patterns["p"].steps.resize(1);
    source->patterns["p"].eventIndexByStep[1] = -1;
    source->patterns["p"].steps[0].ratchets = 4;
    source->patterns["p"].steps[0].gate = 1;
    source->progressions[0].chords[1].beat = .2;
    bool rates = true;
    for (int rate : {44100, 48000, 96000}) {
      SibylModule module;
      module.acceptComposition(source, ApplyAt::IMMEDIATE, PhasePolicy::RESTART_ALL);
      rack::engine::Module::ProcessArgs args{};
      args.sampleRate = float(rate);
      args.sampleTime = 1.f / rate;
      for (int i = 0; i < rate; ++i) {
        args.frame = i;
        gTrackAllocations = true;
        module.process(args);
        gTrackAllocations = false;
        rates &= module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage() == -1.f;
      }
    }
    check(rates, "H06 ratchets retain onset pitch across chord boundary at 44.1/48/96 kHz");
    SibylModule module;
    module.acceptComposition(base.composition, ApplyAt::IMMEDIATE, PhasePolicy::RESTART_ALL);
    processOneSample(module);
    auto first = edit(
        *base.composition,
        R"([{"op":"set_scene_harmony","scene_id":"s","binding":{"progression":"changes","clock":"sceneVisit","loop":false}}])",
        2);
    module.acceptComposition(first.composition, ApplyAt::NEXT_SCENE, PhasePolicy::PRESERVE);
    auto second = edit(*first.composition, R"([{"op":"inherit_scene_harmony","scene_id":"s"}])", 3);
    module.acceptComposition(second.composition, ApplyAt::IMMEDIATE, PhasePolicy::PRESERVE);
    check(module.m_pendingAdoptionPtr.load()->restartChannelMask == 0,
          "P5 pending replacement compares harmonic dependency against sounding revision");
    gTrackAllocations = true;
    processOneSample(module);
    gTrackAllocations = false;
    check(module.m_activeRevision.load() == 3 && module.outputs[SibylModule::GATE_OUTPUT].getVoltage() == 10.f,
          "P5 equivalent superseding harmony edit preserves sounding gate");
    check(gAllocationCount == 0 && gDeallocationCount == 0,
          "P5 cross-rate playback and replacement adoption allocate/free nothing");
  }
  {
    json_t *root = decode(fixture);
    json_t *pattern = json_object_get(json_object_get(root, "patterns"), "p");
    json_t *steps = json_array();
    json_object_set_new(pattern, "length", json_integer(1024));
    for (int i = 0; i < 1024; ++i) {
      json_t *event = decode(
          R"({"step":0,"harmonic":{"kind":"nearest","reference":{"pitchV":0},"range":{"min":"C3","max":"C5"}}})");
      json_object_set_new(event, "step", json_integer(i));
      json_object_set_new(json_object_get(json_object_get(event, "harmonic"), "reference"), "pitchV",
                          json_real(double(i) / 1024.));
      json_array_append_new(steps, event);
    }
    json_object_set_new(pattern, "steps", steps);
    json_t *progressions = json_object();
    json_t *scenes = json_array();
    const char *names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    for (int pi = 0; pi < 9; ++pi) {
      std::string id = "p" + std::to_string(pi);
      json_t *chords = json_array();
      for (int ci = 0; ci < 128; ++ci) {
        int index = pi * 128 + ci, rootNote = index % 96, family = index / 96;
        std::string note = std::string(names[rootNote % 12]) + std::to_string(rootNote / 12);
        json_t *intervals = family < 11 ? json_pack("[i,i]", 0, family + 1) : json_pack("[i,i,i]", 0, 1, 2);
        json_array_append_new(chords, json_pack("{s:s,s:i,s:s,s:o}", "id", ("c" + std::to_string(ci)).c_str(), "beat",
                                                ci, "root", note.c_str(), "intervals", intervals));
      }
      json_object_set_new(progressions, id.c_str(), json_pack("{s:i,s:o}", "lengthBeats", 128, "chords", chords));
      json_array_append_new(scenes, json_pack("{s:s,s:i,s:{s:s},s:{s:s,s:s,s:b}}", "id", id.c_str(), "lengthBeats", 128,
                                              "tracks", "v", "p", "harmony", "progression", id.c_str(), "clock",
                                              "sceneRepeat", "loop", 1));
    }
    json_object_set_new(root, "arrangement", scenes);
    json_object_set_new(root, "harmony", json_pack("{s:o}", "progressions", progressions));
    auto exhausted = compile(root);
    json_decref(root);
    check(!exhausted.valid && !exhausted.errors.empty() && exhausted.errors.back().code == "capacity_exceeded" &&
              exhausted.errors.back().message.find("1048576") != std::string::npos,
          "P5 expression/chord budget rejects expansion beyond 1048576 entries");
  }
  {
    std::ifstream file("doc/Sibyl_v3_Example_Composition.json");
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto example = parseCompositionJson(text, 1);
    check(example.valid, "P5 complete companion expressive-composition fixture compiles");
    if (!example.valid)
      for (const auto &e : example.errors)
        std::cerr << e.path << ": " << e.message << "\n";
    if(example.valid) {
      SibylModule module;module.acceptComposition(example.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);
      std::string response,error;
      const std::string query="{\"view\":\"effective_context\",\"scene_id\":\""+example.composition->arrangement[0].id+"\",\"scene_repeat\":0,\"beat\":0}";
      check(module.handleSibylRequest(SibylControl::Operation::GET_COMPOSITION,query,response,error),"P5 module exposes effective context through standard GET route");
      json_t* context=decode(response);json_t* derived=json_object_get(context,"derived");
      check(json_is_array(json_object_get(derived,"notes"))&&json_is_array(json_object_get(derived,"automation")),"P5 combined context separates note expressions and independent automation");json_decref(context);
      json_t* state=module.dataToJson();SibylModule restored;restored.dataFromJson(state);json_decref(state);
      check(restored.m_acceptedCompositionPtr&&restored.m_acceptedCompositionPtr->harmonicPitchEntries==example.composition->harmonicPitchEntries,
            "P5 Rack state round trip retains and recompiles harmonic tables");
    }
  }
}
} // namespace
