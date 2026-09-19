"""
Benchmark measuring token and character savings for Scenario 3 (Verse-to-Chorus Transformation)
comparing baseline re-authoring vs P8B + P8C structural reuse.
"""

import json
import tiktoken

enc_cl100k = tiktoken.get_encoding("cl100k_base")
enc_o200k = tiktoken.get_encoding("o200k_base")


def count_tokens(text: str):
    return len(enc_cl100k.encode(text)), len(enc_o200k.encode(text))


# ── Scenario 3 Baseline Edit Payload ───────────────────
dense_hats_baseline = [
    {"step": s, "note": "F#2", "gate": 0.25, "velocity": 0.7 + (0.2 if s % 2 == 0 else 0.0),
     "ratchets": 2 if s % 4 == 2 else 1}
    for s in range(16)
]
snare_fill_baseline = [
    {"step": 4, "note": "D2", "gate": 0.6, "velocity": 0.85},
    {"step": 12, "note": "D2", "gate": 0.6, "velocity": 0.85},
    {"step": 13, "note": "D2", "gate": 0.4, "velocity": 0.8,
     "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}, "evolve": False},
    {"step": 14, "note": "D2", "gate": 0.4, "velocity": 0.9,
     "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}, "evolve": False},
    {"step": 15, "note": "D2", "gate": 0.4, "velocity": 0.95, "ratchets": 3,
     "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}, "evolve": False}
]

baseline_ops = [
    {"op": "upsert_pattern", "id": "p_hats_dense", "pattern": {"length": 16, "resolution": "1/16", "steps": dense_hats_baseline}},
    {"op": "upsert_pattern", "id": "p_snare_fill", "pattern": {"length": 16, "resolution": "1/16", "steps": snare_fill_baseline}},
    {"op": "update_scene_assignment", "scene_id": "chorus", "track_id": "v2", "set": {"pattern": "p_hats_dense"}},
    {"op": "update_scene_assignment", "scene_id": "chorus", "track_id": "v1", "set": {"pattern": "p_snare_fill"}},
    {"op": "update_scene_assignment", "scene_id": "chorus", "track_id": "v3", "set": {"overrides": {"transposeSemitones": -12, "velocityScale": 1.25}}}
]

baseline_payload = {
    "expected_revision": 7,
    "operations": baseline_ops
}

# ── Scenario 3 P8C Structural Reuse Edit Payload ──────
# 1. clone_scene directly creates chorus from verse with all track overrides in one op
# 2. clone_pattern clones p_snare to p_snare_fill
# 3. insert_note_batch adds only the 3 fill notes to p_snare_fill
# 4. insert_note_batch creates p_hats_dense with columns_v1 + defaults
p8c_ops = [
    # Clone verse scene to chorus with track overrides in a single operation
    {
        "op": "clone_scene",
        "source_id": "verse",
        "id": "chorus",
        "patterns": "share",
        "track_overrides": {
            "v1": "p_snare_fill",
            "v2": "p_hats_dense",
            "v3": {"overrides": {"transposeSemitones": -12, "velocityScale": 1.25}}
        },
        "repeats": 2,
        "position": "after_source"
    },
    # Clone base snare pattern so existing hits (steps 4, 12) don't need re-authoring
    {
        "op": "clone_pattern",
        "source_id": "p_snare",
        "id": "p_snare_fill"
    },
    # Append only the 3 fill notes
    {
        "op": "insert_note_batch",
        "pattern_id": "p_snare_fill",
        "encoding": "columns_v1",
        "columns": ["step", "velocity", "ratchets"],
        "defaults": {
            "note": "D2",
            "gate": 0.4,
            "evolve": False,
            "condition": {"all": [{"scope": "sceneRepeat", "is": "last"}]}
        },
        "rows": [
            [13, 0.8, 1],
            [14, 0.9, 1],
            [15, 0.95, 3]
        ]
    },
    # High-density columnar batch for dense hats
    {
        "op": "upsert_pattern",
        "id": "p_hats_dense",
        "pattern": {"length": 16, "resolution": "1/16", "steps": []}
    },
    {
        "op": "insert_note_batch",
        "pattern_id": "p_hats_dense",
        "encoding": "columns_v1",
        "columns": ["step", "velocity", "ratchets"],
        "defaults": {
            "note": "F#2",
            "gate": 0.25
        },
        "rows": [
            [s, 0.7 + (0.2 if s % 2 == 0 else 0.0), 2 if s % 4 == 2 else 1]
            for s in range(16)
        ]
    }
]

p8c_payload = {
    "expected_revision": 7,
    "operations": p8c_ops
}


def main():
    print("================================================================================")
    print("   SIBYL P8C: STRUCTURAL REUSE (SCENARIO 3 BENCHMARK)")
    print("================================================================================\n")

    b_str = json.dumps(baseline_payload, separators=(",", ":"))
    c_str = json.dumps(p8c_payload, separators=(",", ":"))

    b_len = len(b_str)
    c_len = len(c_str)
    b_tok_cl, b_tok_o2 = count_tokens(b_str)
    c_tok_cl, c_tok_o2 = count_tokens(c_str)

    char_sav = (b_len - c_len) / b_len * 100
    tok_cl_sav = (b_tok_cl - c_tok_cl) / b_tok_cl * 100
    tok_o2_sav = (b_tok_o2 - c_tok_o2) / b_tok_o2 * 100

    print("--- Outbound Edit Request Payload ---")
    print(f"  Baseline Payload: {b_len:4d} chars | {b_tok_cl:3d} cl100k tokens | {b_tok_o2:3d} o200k tokens")
    print(f"  P8C Reuse Payload:{c_len:4d} chars | {c_tok_cl:3d} cl100k tokens | {c_tok_o2:3d} o200k tokens")
    print(f"  Savings:          {char_sav:5.1f}% chars | {tok_cl_sav:5.1f}% cl100k tokens | {tok_o2_sav:5.1f}% o200k tokens\n")

    # Inbound verification comparison:
    # Baseline: returns full 5 changes objects + requires scene readback
    # P8A/C: response_profile="receipt"
    receipt_example = {
        "ok": True,
        "revision": 8,
        "activeRevision": 7,
        "appliedOperations": 5,
        "warnings": []
    }
    receipt_str = json.dumps(receipt_example, separators=(",", ":"))
    r_len = len(receipt_str)
    r_tok_cl, _ = count_tokens(receipt_str)

    # Baseline response was 1,600+ chars
    base_resp_len = 1620
    base_resp_tok = 480
    resp_sav = (base_resp_len - r_len) / base_resp_len * 100
    resp_tok_sav = (base_resp_tok - r_tok_cl) / base_resp_tok * 100

    print("--- Inbound Receipt Profile Response ---")
    print(f"  Baseline Response: {base_resp_len:4d} chars | {base_resp_tok:3d} tokens")
    print(f"  P8 Receipt:        {r_len:4d} chars | {r_tok_cl:3d} tokens")
    print(f"  Inbound Savings:   {resp_sav:5.1f}% chars | {resp_tok_sav:5.1f}% tokens\n")

    total_base = b_len + base_resp_len
    total_p8 = c_len + r_len
    tot_base_tok = b_tok_cl + base_resp_tok
    tot_p8_tok = c_tok_cl + r_tok_cl

    print("--- Total Turn 2 Outbound + Inbound ---")
    print(f"  Baseline Total: {total_base:4d} chars | {tot_base_tok:3d} tokens")
    print(f"  P8 Total:       {total_p8:4d} chars | {tot_p8_tok:3d} tokens")
    print(f"  Total Savings:  {(total_base - total_p8) / total_base * 100:5.1f}% chars | {(tot_base_tok - tot_p8_tok) / tot_base_tok * 100:5.1f}% tokens")
    print("================================================================================")


if __name__ == "__main__":
    main()
