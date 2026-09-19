#!/usr/bin/env python3
"""
Sibyl P8 Baseline Measurement Harness
Implements the 7 required scenarios from doc/Sibyl_P8_Token_Lean_Composition_Codex_Spec.md Section 4
against the live Rack bridge at port 34570.
"""

import json
import os
import sys
import time
import urllib.request
import urllib.error
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

BRIDGE_PORT = int(os.environ.get("OCTAVIA_PORT", "34570"))
BRIDGE_URL = f"http://127.0.0.1:{BRIDGE_PORT}"
RESULTS_DIR = Path("test-results/p8-baseline")
RESULTS_DIR.mkdir(parents=True, exist_ok=True)


class TelemetryTracker:
    def __init__(self):
        self.events: List[Dict[str, Any]] = []
        self.scenario_name: str = ""

    def start_scenario(self, name: str):
        self.scenario_name = name

    def record_call(self, endpoint: str, method: str, req_body: Optional[Any],
                    resp_status: int, resp_body: Optional[Any], latency_ms: float, is_error: bool = False):
        req_text = json.dumps(req_body) if req_body is not None else ""
        resp_text = json.dumps(resp_body) if resp_body is not None else ""
        
        event = {
            "scenario": self.scenario_name,
            "endpoint": endpoint,
            "method": method,
            "latency_ms": round(latency_ms, 2),
            "is_error": is_error,
            "request_chars": len(req_text),
            "request_bytes": len(req_text.encode("utf-8")),
            "response_chars": len(resp_text),
            "response_bytes": len(resp_text.encode("utf-8")),
            "status_code": resp_status,
        }
        self.events.append(event)
        return event


tracker = TelemetryTracker()


def http_call(endpoint: str, method: str = "GET", payload: Optional[Any] = None) -> Tuple[int, Any]:
    url = f"{BRIDGE_URL}/{endpoint.lstrip('/')}"
    data_bytes = None
    headers = {"Content-Type": "application/json"}
    if payload is not None:
        data_bytes = json.dumps(payload).encode("utf-8")

    req = urllib.request.Request(url, data=data_bytes, headers=headers, method=method)
    t0 = time.perf_counter()
    status_code = 200
    try:
        with urllib.request.urlopen(req, timeout=10.0) as resp:
            status_code = resp.status
            raw = resp.read().decode("utf-8")
            latency_ms = (time.perf_counter() - t0) * 1000.0
            data = json.loads(raw) if raw else {}
            tracker.record_call(endpoint, method, payload, status_code, data, latency_ms, is_error=False)
            return status_code, data
    except urllib.error.HTTPError as e:
        status_code = e.code
        raw = e.read().decode("utf-8")
        latency_ms = (time.perf_counter() - t0) * 1000.0
        try:
            data = json.loads(raw)
        except Exception:
            data = {"error": raw}
        tracker.record_call(endpoint, method, payload, status_code, data, latency_ms, is_error=True)
        return status_code, data
    except Exception as e:
        latency_ms = (time.perf_counter() - t0) * 1000.0
        tracker.record_call(endpoint, method, payload, 0, {"error": str(e)}, latency_ms, is_error=True)
        raise


def get_sibyl_module_id() -> int:
    status, modules = http_call("modules")
    if status != 200:
        raise RuntimeError(f"Failed to query modules: {modules}")
    for m in modules:
        if m.get("plugin") == "Leviathan" and m.get("model") == "Sibyl":
            return m["id"]
    raise RuntimeError("No Leviathan Sibyl module found in Rack!")


def get_status(sibyl_id: int) -> Dict[str, Any]:
    _, res = http_call(f"sibyl/{sibyl_id}/status")
    return res


def get_composition(sibyl_id: int, **kwargs) -> Dict[str, Any]:
    from urllib.parse import urlencode
    query_parts = {}
    for k, v in kwargs.items():
        if v is not None:
            query_parts[k] = json.dumps(v) if isinstance(v, (list, dict)) else v
    qs = f"?{urlencode(query_parts)}" if query_parts else ""
    _, res = http_call(f"sibyl/{sibyl_id}/composition{qs}")
    return res


def edit_sibyl(sibyl_id: int, expected_revision: int, operations: List[Dict[str, Any]],
               apply_at: Optional[str] = None, phase_policy: str = "preserve") -> Tuple[int, Dict[str, Any]]:
    payload = {
        "expected_revision": expected_revision,
        "phase_policy": phase_policy,
        "operations": operations
    }
    if apply_at:
        payload["apply_at"] = apply_at
    return http_call(f"sibyl/{sibyl_id}/edit", method="POST", payload=payload)


def validate_sibyl(sibyl_id: int, expected_revision: int, operations: List[Dict[str, Any]],
                   return_changes: bool = True) -> Tuple[int, Dict[str, Any]]:
    payload = {
        "expected_revision": expected_revision,
        "return_changes": return_changes,
        "operations": operations
    }
    return http_call(f"sibyl/{sibyl_id}/validate", method="POST", payload=payload)


# ── Scenario Implementations ──────────────────────────────────────────────────

def run_scenario_1(sibyl_id: int):
    """
    Scenario 1: Compose an eight-track, multi-section piece using reused patterns,
    independent automation, probability/evolution, and relative harmony.
    """
    tracker.start_scenario("Scenario 1: Eight-track multi-section composition")
    
    # 1. Discover capabilities and initial orientation
    http_call(f"sibyl/{sibyl_id}/capabilities")
    comp_summary = get_composition(sibyl_id, view="summary")
    status = get_status(sibyl_id)
    rev = status.get("revision", 0)

    # 2. Upsert 8 tracks
    tracks_ops = [
        {"op": "upsert_track", "id": f"v{i}", "track": {"channel": i, "defaultGate": 0.8, "defaultVelocity": 0.8}}
        for i in range(8)
    ]

    # 3. Relative harmony progression (4 chords: Cm7, Abmaj7, Ebmaj7, Bb7)
    prog_op = {
        "op": "upsert_progression",
        "id": "changes_prog",
        "progression": {
            "lengthBeats": 16,
            "chords": [
                {"id": "cm7", "beat": 0, "root": "C3", "intervals": [0, 3, 7, 10]},
                {"id": "abmaj7", "beat": 4, "root": "Ab2", "intervals": [0, 4, 7, 11]},
                {"id": "ebmaj7", "beat": 8, "root": "Eb3", "intervals": [0, 4, 7, 11]},
                {"id": "bb7", "beat": 12, "root": "Bb2", "intervals": [0, 4, 7, 10]}
            ]
        }
    }
    def_harm_op = {
        "op": "set_default_harmony",
        "binding": {"progression": "changes_prog", "clock": "arrangement", "loop": True}
    }

    # 4. Patterns
    # Drum patterns
    kick_steps = [{"step": s, "note": "C2", "gate": 0.5, "velocity": 0.9} for s in (0, 4, 8, 12)]
    snare_steps = [{"step": s, "note": "D2", "gate": 0.6, "velocity": 0.85} for s in (4, 12)]
    hats_steps = [
        {"step": s, "note": "F#2", "gate": 0.3, "velocity": 0.6 + (0.2 if s % 4 == 2 else 0.0),
         "probability": 0.85,
         "condition": {"all": [{"scope": "patternPass", "every": 2, "offset": 2}]} if s == 14 else None}
        for s in range(16)
    ]
    # Remove None condition
    for h in hats_steps:
        if h["condition"] is None:
            del h["condition"]

    # Bass pattern using relative harmony
    bass_steps = [
        {"step": 0, "harmonic": {"kind": "tone", "role": "root", "octave": 0}, "gate": 1.2, "velocity": 0.9},
        {"step": 3, "harmonic": {"kind": "tone", "role": "third", "octave": 0}, "gate": 0.8, "velocity": 0.75},
        {"step": 6, "harmonic": {"kind": "tone", "role": "fifth", "octave": 0}, "gate": 0.8, "velocity": 0.8},
        {"step": 8, "harmonic": {"kind": "tone", "role": "root", "octave": 0}, "gate": 1.0, "velocity": 0.85},
        {"step": 11, "harmonic": {"kind": "tone", "role": "seventh" if False else "third", "octave": 1}, "gate": 0.7, "velocity": 0.7},
        {"step": 14, "harmonic": {"kind": "tone", "role": "fifth", "octave": 0}, "gate": 0.9, "velocity": 0.75}
    ]

    # Melodic lead
    lead_steps = [
        {"step": 0, "note": "G3", "gate": 1.5, "velocity": 0.85},
        {"step": 2, "note": "Bb3", "gate": 0.9, "velocity": 0.8},
        {"step": 4, "note": "C4", "gate": 2.0, "velocity": 0.95},
        {"step": 8, "note": "Eb4", "gate": 1.2, "velocity": 0.85},
        {"step": 10, "note": "D4", "gate": 0.8, "velocity": 0.75},
        {"step": 12, "note": "Bb3", "gate": 1.8, "velocity": 0.8},
        {"step": 14, "note": "C4", "gate": 1.5, "velocity": 0.9}
    ]

    pattern_ops = [
        {"op": "upsert_pattern", "id": "p_kick", "pattern": {"length": 16, "resolution": "1/16", "steps": kick_steps}},
        {"op": "upsert_pattern", "id": "p_snare", "pattern": {"length": 16, "resolution": "1/16", "steps": snare_steps}},
        {"op": "upsert_pattern", "id": "p_hats", "pattern": {"length": 16, "resolution": "1/16", "steps": hats_steps}},
        {"op": "upsert_pattern", "id": "p_bass", "pattern": {
            "length": 16, "resolution": "1/16",
            "evolution": {"probability": 0.15, "velocity": 0.1, "gate": 0.1},
            "steps": bass_steps
        }},
        {"op": "upsert_pattern", "id": "p_lead", "pattern": {"length": 16, "resolution": "1/16", "steps": lead_steps}}
    ]

    # 5. Automation
    auto_op = {
        "op": "upsert_automation",
        "id": "filter_sweep",
        "automation": {
            "target": {"track": "v3", "lane": "mod"},
            "scope": {"arrangement": True},
            "clock": "arrangement",
            "mode": "replace",
            "points": [
                {"beat": 0, "value": -3.0, "shape": "smoothstep"},
                {"beat": 16, "value": 2.0, "shape": "smoothstep"},
                {"beat": 32, "value": 4.5, "shape": "linear"},
                {"beat": 48, "value": -1.0, "shape": "smoothstep"}
            ]
        }
    }

    # 6. Multi-section arrangement: Intro, Verse, Chorus, Outro
    scenes_ops = [
        {"op": "upsert_scene", "id": "intro", "scene": {
            "lengthBeats": 16, "repeats": 1,
            "tracks": {"v0": "p_kick", "v2": "p_hats"}
        }},
        {"op": "upsert_scene", "id": "verse", "scene": {
            "lengthBeats": 16, "repeats": 2,
            "tracks": {"v0": "p_kick", "v1": "p_snare", "v2": "p_hats", "v3": "p_bass", "v6": "p_lead"}
        }},
        {"op": "upsert_scene", "id": "chorus", "scene": {
            "lengthBeats": 16, "repeats": 2,
            "tracks": {
                "v0": "p_kick", "v1": "p_snare", "v2": "p_hats",
                "v3": {"pattern": "p_bass", "overrides": {"transposeSemitones": 12, "velocityScale": 1.2}},
                "v6": "p_lead"
            }
        }},
        {"op": "upsert_scene", "id": "outro", "scene": {
            "lengthBeats": 16, "repeats": 1,
            "tracks": {"v3": "p_bass", "v6": "p_lead"}
        }}
    ]

    # Combine into atomic composition transaction
    all_ops = tracks_ops + [prog_op, def_harm_op] + pattern_ops + [auto_op] + scenes_ops
    code, res = edit_sibyl(sibyl_id, expected_revision=rev, operations=all_ops)
    if code != 200 or not res.get("ok"):
        raise RuntimeError(f"Scenario 1 edit failed: {res}")
    
    # Readback verification
    get_composition(sibyl_id, view="summary")
    get_status(sibyl_id)


def run_scenario_2(sibyl_id: int):
    """
    Scenario 2: Change the final four notes of a phrase, including a conflict/recovery case.
    """
    tracker.start_scenario("Scenario 2: Phrase revision & conflict recovery")
    
    # Read existing notes of p_lead
    notes_res = get_composition(sibyl_id, view="notes", pattern_id="p_lead", fields=["id", "step", "note", "velocity"])
    notes = notes_res.get("notes", [])
    if len(notes) < 4:
        raise RuntimeError(f"Expected at least 4 notes in p_lead, got {len(notes)}")
    
    status = get_status(sibyl_id)
    cur_rev = status.get("revision", 1)

    # Pick the final 4 notes
    final_4 = sorted(notes, key=lambda n: n["step"])[-4:]
    final_4_ids = [n["id"] for n in final_4]

    # Intentionally trigger a conflict with stale revision (cur_rev - 1)
    stale_rev = cur_rev - 1
    conflict_op = [{
        "op": "update_notes",
        "pattern_id": "p_lead",
        "selector": {"ids": final_4_ids},
        "adjust": {"velocity": {"multiply": 1.2}},
        "expect_count": 4
    }]
    code, conflict_res = edit_sibyl(sibyl_id, expected_revision=stale_rev, operations=conflict_op)
    # Verify it was rejected
    assert code != 200 or not conflict_res.get("ok"), "Conflict did not trigger on stale revision!"

    # Recovery: Re-fetch status to get the accepted revision
    fresh_status = get_status(sibyl_id)
    valid_rev = fresh_status.get("revision")

    # Apply note changes to the final 4 notes: transpose them up 2 semitones and boost velocity
    recovery_ops = [
        {
            "op": "transpose_notes",
            "pattern_id": "p_lead",
            "selector": {"ids": final_4_ids},
            "semitones": 2,
            "expect_count": 4
        },
        {
            "op": "update_notes",
            "pattern_id": "p_lead",
            "selector": {"ids": final_4_ids},
            "set": {"velocity": 0.95},
            "expect_count": 4
        }
    ]
    code, rec_res = edit_sibyl(sibyl_id, expected_revision=valid_rev, operations=recovery_ops)
    if code != 200 or not rec_res.get("ok"):
        raise RuntimeError(f"Recovery edit failed: {rec_res}")

    # Focused verification of modified notes
    get_composition(sibyl_id, view="notes", pattern_id="p_lead", selector={"ids": final_4_ids}, fields=["id", "step", "note", "velocity"])


def run_scenario_3(sibyl_id: int):
    """
    Scenario 3: Build a chorus from a verse with a changed bass register,
    denser percussion, and a final-repeat fill.
    """
    tracker.start_scenario("Scenario 3: Verse to Chorus transformation")
    status = get_status(sibyl_id)
    rev = status.get("revision")

    # 1. Read verse assignment
    verse_view = get_composition(sibyl_id, view="scene", id="verse")

    # 2. Add dense percussion pattern (p_hats_dense) and fill pattern (p_snare_fill)
    dense_hats = [
        {"step": s, "note": "F#2", "gate": 0.25, "velocity": 0.7 + (0.2 if s % 2 == 0 else 0.0), "ratchets": 2 if s % 4 == 2 else 1}
        for s in range(16)
    ]
    snare_fill_steps = [
        {"step": 4, "note": "D2", "gate": 0.6, "velocity": 0.85},
        {"step": 12, "note": "D2", "gate": 0.6, "velocity": 0.85},
        # Fill on final repeat only
        {"step": 13, "note": "D2", "gate": 0.4, "velocity": 0.8, "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}, "evolve": False},
        {"step": 14, "note": "D2", "gate": 0.4, "velocity": 0.9, "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}, "evolve": False},
        {"step": 15, "note": "D2", "gate": 0.4, "velocity": 0.95, "ratchets": 3, "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}, "evolve": False}
    ]

    ops = [
        {"op": "upsert_pattern", "id": "p_hats_dense", "pattern": {"length": 16, "resolution": "1/16", "steps": dense_hats}},
        {"op": "upsert_pattern", "id": "p_snare_fill", "pattern": {"length": 16, "resolution": "1/16", "steps": snare_fill_steps}},
        # Update chorus scene assignments: dense hats on v2, snare fill on v1, lowered bass register (-12) on v3
        {"op": "update_scene_assignment", "scene_id": "chorus", "track_id": "v2", "set": {"pattern": "p_hats_dense"}},
        {"op": "update_scene_assignment", "scene_id": "chorus", "track_id": "v1", "set": {"pattern": "p_snare_fill"}},
        {"op": "update_scene_assignment", "scene_id": "chorus", "track_id": "v3", "set": {"overrides": {"transposeSemitones": -12, "velocityScale": 1.25}}}
    ]

    code, res = edit_sibyl(sibyl_id, expected_revision=rev, operations=ops)
    if code != 200 or not res.get("ok"):
        raise RuntimeError(f"Scenario 3 edit failed: {res}")

    # Verify with scene projection
    get_composition(sibyl_id, view="scene", id="chorus", fields=["effectiveExpressions"])


def run_scenario_4(sibyl_id: int):
    """
    Scenario 4: Create and revise both 38-EDO and 53-EDO material through P7,
    including exact-ratio exceptions and native voicing.
    """
    tracker.start_scenario("Scenario 4: P7 38-EDO / 53-EDO microtonal composition")
    status = get_status(sibyl_id)
    rev = status.get("revision")

    # 1. Define 38-EDO tuning, scale, and context
    t38_op = {"op": "upsert_tuning", "id": "t38", "tuning": {"kind": "equal", "divisions": 38, "period": {"ratio": "2/1"}}}
    s38_op = {"op": "upsert_pitch_scale", "id": "s38", "scale": {"tuning": "t38", "steps": [0, 6, 13, 19, 25, 31]}}
    c38_op = {"op": "upsert_pitch_context", "id": "c38", "context": {"tuning": "t38", "scale": "s38", "anchor": {"note": "C4"}}}

    # 2. Native progression in 38-EDO with exact-ratio exception (ratio 7/4 harmonic 7th)
    prog38_op = {
        "op": "upsert_progression",
        "id": "prog38",
        "progression": {
            "pitchContext": "c38",
            "lengthBeats": 8,
            "chords": [
                {
                    "id": "I_38", "beat": 0, "rootPitch": {"tuned": {"step": 0}},
                    "tones": [
                        {"id": "r", "interval": {"steps": 0}, "roles": ["root"]},
                        {"id": "m", "interval": {"steps": 13}, "roles": ["third"]},
                        {"id": "f", "interval": {"steps": 22}, "roles": ["fifth"]}
                    ]
                },
                {
                    "id": "IV_sub", "beat": 4, "rootPitch": {"tuned": {"step": 19}},
                    "tones": [
                        {"id": "r", "interval": {"steps": 0}, "roles": ["root"]},
                        {"id": "h7", "interval": {"ratio": "7/4"}, "roles": ["seventh"]}
                    ]
                }
            ]
        }
    }

    # 3. Pattern using 38-EDO tuned steps and exact ratio
    micro_steps = [
        {"step": 0, "tuned": {"step": 0}, "gate": 1.0, "velocity": 0.8},
        {"step": 4, "tuned": {"step": 13}, "gate": 1.0, "velocity": 0.85},
        {"step": 8, "tuned": {"step": 22}, "gate": 1.0, "velocity": 0.8},
        {"step": 12, "tuned": {"ratio": "7/4"}, "gate": 1.5, "velocity": 0.9}
    ]
    p_micro_op = {
        "op": "upsert_pattern",
        "id": "p_micro",
        "pattern": {"length": 16, "resolution": "1/16", "pitchContext": "c38", "steps": micro_steps}
    }

    code, res = edit_sibyl(sibyl_id, expected_revision=rev, operations=[t38_op, s38_op, c38_op, prog38_op, p_micro_op])
    if code != 200 or not res.get("ok"):
        raise RuntimeError(f"Scenario 4 (38-EDO) setup failed: {res}")

    # Read pitch context definition & pitch details
    get_composition(sibyl_id, view="pitch_context", id="c38")
    get_composition(sibyl_id, view="notes", pattern_id="p_micro", fields=["id", "step", "pitchDetails", "effectivePitchV"])

    # 4. Revise to 53-EDO: introduce 53-EDO tuning and retune pattern
    status = get_status(sibyl_id)
    rev = status.get("revision")

    t53_op = {"op": "upsert_tuning", "id": "t53", "tuning": {"kind": "equal", "divisions": 53, "period": {"ratio": "2/1"}}}
    c53_op = {"op": "upsert_pitch_context", "id": "c53", "context": {"tuning": "t53", "anchor": {"note": "C4"}}}
    retune_op = {
        "op": "retune_notes",
        "pattern_id": "p_micro",
        "selector": {"all": True},
        "target_context": "c53",
        "mode": "nearest",
        "target": "tuning",
        "expect_count": 4
    }

    code, res2 = edit_sibyl(sibyl_id, expected_revision=rev, operations=[t53_op, c53_op, retune_op])
    if code != 200 or not res2.get("ok"):
        raise RuntimeError(f"Scenario 4 (53-EDO revision) failed: {res2}")

    # Readback pitch systems
    get_composition(sibyl_id, view="pitch_systems", id="tunings")


def run_scenario_5(sibyl_id: int):
    """
    Scenario 5: Resume an unfamiliar existing composition after context loss
    and make one focused edit.
    """
    tracker.start_scenario("Scenario 5: Context loss resume and focused edit")
    
    # 1. Orientation packet after memory loss: capabilities, summary, and status
    http_call(f"sibyl/{sibyl_id}/capabilities")
    summary = get_composition(sibyl_id, view="summary")
    status = get_status(sibyl_id)
    rev = status.get("revision")

    # 2. Agent decides to inspect 'p_lead' notes with field projection
    lead_notes = get_composition(sibyl_id, view="notes", pattern_id="p_lead", page_size=16, fields=["id", "step", "note", "velocity"])
    notes = lead_notes.get("notes", [])
    assert len(notes) > 0, "No notes found in p_lead during resume"
    first_note_id = notes[0]["id"]

    # 3. Make one focused edit on first_note_id (accentuate velocity)
    edit_op = [{
        "op": "update_notes",
        "pattern_id": "p_lead",
        "selector": {"ids": [first_note_id]},
        "set": {"velocity": 1.0, "gate": 2.5},
        "expect_count": 1
    }]

    code, res = edit_sibyl(sibyl_id, expected_revision=rev, operations=edit_op)
    if code != 200 or not res.get("ok"):
        raise RuntimeError(f"Scenario 5 edit failed: {res}")

    # Focused verification of updated note
    get_composition(sibyl_id, view="notes", pattern_id="p_lead", selector={"ids": [first_note_id]}, fields=["id", "velocity", "gate"])


def run_scenario_6(sibyl_id: int):
    """
    Scenario 6: Diagnose a sounding mismatch without fetching every note
    or dumping raw audio/monitor data.
    """
    tracker.start_scenario("Scenario 6: Sounding mismatch diagnosis")
    
    # 1. Inspect runtime status (gateMask, activeRevision, sceneId, beat, conditionPasses)
    status = get_status(sibyl_id)

    # 2. Inspect effective context at current/targeted beat
    beat = status.get("beat", 0.0)
    get_composition(sibyl_id, view="effective_context", scene_id="chorus", scene_repeat=0, beat=beat)

    # 3. Inspect module output voltages across the patch to verify signal levels
    http_call("modules/voltages")


def run_scenario_7(sibyl_id: int):
    """
    Scenario 7: Author an irregular, highly expressive passage with little repetition;
    compact mode must not constrain it.
    """
    tracker.start_scenario("Scenario 7: Irregular expressive passage")
    status = get_status(sibyl_id)
    rev = status.get("revision")

    # 16 highly bespoke events with custom microtiming, distinct velocities, glides, ratchets, MOD lanes
    expressive_steps = [
        {"step": 0, "note": "C3", "gate": 0.75, "velocity": 0.88, "glideMs": 15, "mod": 1.2, "mod2": -0.5, "mod3": 0.0},
        {"step": 1, "note": "D#3", "gate": 0.4, "velocity": 0.45, "probability": 0.7, "ratchets": 2, "mod": 0.5, "mod2": 1.0},
        {"step": 3, "note": "F3", "gate": 1.8, "velocity": 0.95, "glideMs": 45, "mod": -2.0, "mod2": 3.0, "mod3": 1.5},
        {"step": 4, "note": "F#3", "gate": 0.3, "velocity": 0.6, "probability": 0.8, "mod": 0.0, "mod2": 0.0},
        {"step": 5, "note": "G3", "gate": 0.9, "velocity": 0.82, "ratchets": 3, "mod": 2.5, "mod2": -1.5},
        {"step": 7, "note": "A#3", "gate": 0.5, "velocity": 0.7, "probability": 0.9, "mod": 1.0},
        {"step": 8, "note": "C4", "gate": 2.2, "velocity": 0.99, "glideMs": 60, "mod": 4.0, "mod2": -3.0, "mod3": 2.0},
        {"step": 10, "note": "D#4", "gate": 0.6, "velocity": 0.65, "probability": 0.6, "mod": -1.0},
        {"step": 11, "note": "D4", "gate": 0.45, "velocity": 0.75, "ratchets": 2, "mod": 0.8, "mod2": 0.4},
        {"step": 12, "note": "C4", "gate": 1.4, "velocity": 0.85, "glideMs": 25, "mod": 1.5, "mod2": -0.5},
        {"step": 13, "note": "G#3", "gate": 0.5, "velocity": 0.55, "probability": 0.75, "mod": -0.5},
        {"step": 14, "note": "G3", "gate": 0.8, "velocity": 0.78, "ratchets": 4, "mod": 3.0, "mod2": 1.2, "mod3": -1.0},
        {"step": 15, "note": "F3", "gate": 1.1, "velocity": 0.82, "glideMs": 30, "mod": 0.2, "mod2": 0.0}
    ]

    op = {
        "op": "upsert_pattern",
        "id": "p_expressive",
        "pattern": {"length": 16, "resolution": "1/16", "steps": expressive_steps}
    }

    code, res = edit_sibyl(sibyl_id, expected_revision=rev, operations=[op])
    if code != 200 or not res.get("ok"):
        raise RuntimeError(f"Scenario 7 edit failed: {res}")

    # Full pattern readback to verify all expressive fields survived exactly
    get_composition(sibyl_id, view="notes", pattern_id="p_expressive", fields=["full"])


def main():
    print("=" * 70)
    print("Sibyl P8 Baseline Measurement Suite")
    print("Connecting to live Rack bridge...")
    
    sibyl_id = get_sibyl_module_id()
    print(f"Discovered Sibyl module ID: {sibyl_id}")
    
    initial_status = get_status(sibyl_id)
    print(f"Initial Status: revision={initial_status.get('revision')}, active={initial_status.get('activeRevision')}")

    # Execute the 7 scenarios sequentially
    scenarios = [
        ("Scenario 1: 8-track multi-section composition", run_scenario_1),
        ("Scenario 2: Phrase revision & conflict recovery", run_scenario_2),
        ("Scenario 3: Verse to Chorus transformation", run_scenario_3),
        ("Scenario 4: P7 38-EDO / 53-EDO microtonality", run_scenario_4),
        ("Scenario 5: Context loss resume & focused edit", run_scenario_5),
        ("Scenario 6: Sounding mismatch diagnosis", run_scenario_6),
        ("Scenario 7: Irregular expressive passage", run_scenario_7),
    ]

    for title, func in scenarios:
        print(f"\n>>> Running {title}...")
        t_start = time.perf_counter()
        func(sibyl_id)
        duration = (time.perf_counter() - t_start) * 1000.0
        print(f"    Completed in {duration:.1f}ms")

    # Read final full composition for score size accounting
    final_comp = get_composition(sibyl_id, view="full")
    final_comp_json = json.dumps(final_comp, indent=2)
    (RESULTS_DIR / "final_composition.json").write_text(final_comp_json, encoding="utf-8")
    
    # Save raw telemetry events
    telemetry_json = json.dumps(tracker.events, indent=2)
    (RESULTS_DIR / "baseline_telemetry.json").write_text(telemetry_json, encoding="utf-8")

    # Aggregate metrics per scenario
    summary_report = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "bridge_url": BRIDGE_URL,
        "sibyl_module_id": sibyl_id,
        "total_calls": len(tracker.events),
        "scenarios": {}
    }

    print("\n" + "=" * 70)
    print("BASELINE MEASUREMENT RESULTS SUMMARY")
    print("=" * 70)
    header = f"{'Scenario':<42} | {'Calls':<5} | {'Out Chars':<9} | {'In Chars':<9} | {'Latency(ms)':<11}"
    print(header)
    print("-" * len(header))

    scenario_groups = {}
    for ev in tracker.events:
        scenario_groups.setdefault(ev["scenario"], []).append(ev)

    for sc_name, events in scenario_groups.items():
        call_count = len(events)
        out_chars = sum(e["request_chars"] for e in events)
        out_bytes = sum(e["request_bytes"] for e in events)
        in_chars = sum(e["response_chars"] for e in events)
        in_bytes = sum(e["response_bytes"] for e in events)
        total_latency = sum(e["latency_ms"] for e in events)
        
        summary_report["scenarios"][sc_name] = {
            "call_count": call_count,
            "outbound_chars": out_chars,
            "outbound_bytes": out_bytes,
            "inbound_chars": in_chars,
            "inbound_bytes": in_bytes,
            "total_latency_ms": round(total_latency, 2),
            "approx_outbound_tokens": round(out_chars / 4),
            "approx_inbound_tokens": round(in_chars / 4)
        }
        print(f"{sc_name[:42]:<42} | {call_count:<5} | {out_chars:<9} | {in_chars:<9} | {total_latency:<11.1f}")

    total_out_chars = sum(e["request_chars"] for e in tracker.events)
    total_in_chars = sum(e["response_chars"] for e in tracker.events)
    print("-" * len(header))
    print(f"{'TOTAL':<42} | {len(tracker.events):<5} | {total_out_chars:<9} | {total_in_chars:<9} |")
    print("=" * 70)
    print(f"Final composition saved to: {RESULTS_DIR / 'final_composition.json'}")
    print(f"Score size: {len(final_comp_json)} chars ({len(final_comp_json.encode('utf-8'))} bytes)")

    summary_json = json.dumps(summary_report, indent=2)
    (RESULTS_DIR / "baseline_summary.json").write_text(summary_json, encoding="utf-8")
    print(f"Summary report saved to: {RESULTS_DIR / 'baseline_summary.json'}")


if __name__ == "__main__":
    main()
