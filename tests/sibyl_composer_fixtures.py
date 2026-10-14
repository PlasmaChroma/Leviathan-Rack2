"""Generate actual toolkit output for the native canonical validator."""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.sibyl_composer import (PatternBuilder, ProgressionBuilder, SibylScore, EdoTuning,
                                  AutomationBuilder, VoicingBuilder, expand_note_batch)

for divisions in (38, 53):
    base = {
        "schemaVersion": 4,
        "pitchSystems": {
            "tunings": {"t": {"kind": "equal", "divisions": divisions, "period": {"ratio": "2/1"}}},
            "contexts": {"c": {"tuning": "t", "anchor": {"pitchV": 0}}}, "defaultContext": "c"},
        "tracks": [{"id": "v", "channel": 0}],
        "patterns": {"p": {"length": 16, "resolution": "1/16", "steps": []}},
        "arrangement": [{"id": "s", "lengthBeats": 4, "tracks": {"v": "p"}}]}
    edo = EdoTuning(divisions)
    progression = ProgressionBuilder("c", 4).chord(0, root_step=0, intervals=edo.chord_steps("septimal_dom7"))
    patterns = [
        PatternBuilder(pitch_context="c", evolution={"velocity": 0.2})
            .arpeggiate([0, edo.step_from_interval("M3")], period_steps=divisions, octave_range=2),
        PatternBuilder(pitch_context="c").add_note(0, tuned_ratio="7/4", ratchets=1, glide_ms=0,
            condition={"all": [{"scope": "patternPass", "every": 2, "offset": 1}]}).add_note(3, tuned_step=0)]
    for pattern in patterns:
        compact = SibylScore().add_pattern("p", pattern).add_progression("h", progression).compile()
        canonical = SibylScore().add_pattern("p", pattern, use_columnar_batch=False).add_progression("h", progression).compile()
        print(json.dumps({"base": base, "operations": compact, "canonical": canonical}, separators=(",", ":")))

# Exercise the non-octave builder and a seeded walk through the native parser.
setup = EdoTuning(13, "3/1").to_operations("tritave", "tritave_context")
walk = PatternBuilder(pitch_context="tritave_context").arpeggiate(
    [0, 4, 7], contour="random_walk", seed=314, period_steps=13, octave_range=2)
compact = setup + SibylScore().add_pattern("p", walk).compile()
canonical = setup + SibylScore().add_pattern("p", walk, use_columnar_batch=False).compile()
print(json.dumps({"base": base, "operations": compact, "canonical": canonical}, separators=(",", ":")))

# Compare native arithmetic/cyclic expansion against the actual client fallback.
batch = {"op": "insert_note_batch", "pattern_id": "p", "encoding": "columns_v1",
         "columns": ["tuned.step"], "rows": [[0], [17]],
         "defaults": {"tuned": {"context": "c"}, "velocity": 0.7},
         "onsets": {"start": 0, "spacing": 2, "count": 8},
         "overrides": {"7": {"tuned": {"context": "c", "ratio": "14/8"}, "gate": 0}}}
print(json.dumps({"base": base, "operations": [batch], "canonical": [expand_note_batch(batch)]}, separators=(",", ":")))

# Native voicing orchestration and scene automation must validate together.
progression = ProgressionBuilder("c", 4).chord(0, root_step=0, intervals=[0, 17, 31])
score = (SibylScore().add_progression("h", progression)
         .add_voicing(VoicingBuilder("h", "s", write_policy="replace").spread({"v": "p"}, -1, 1))
         .add_automation("swell", AutomationBuilder("v", scope="scene", scene_id="s").envelope(0, 4, 0, 5, 0)))
print(json.dumps({"base": base, "operations": score.compile(), "canonical": score.compile()}, separators=(",", ":")))
