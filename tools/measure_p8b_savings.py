"""
Benchmark script to measure token savings of P8B Columnar Event Batches (insert_note_batch)
relative to baseline insert_notes.
"""

import json
import tiktoken

enc_cl100k = tiktoken.get_encoding("cl100k_base")
enc_o200k = tiktoken.get_encoding("o200k_base")


def count_tokens(text: str):
    return len(enc_cl100k.encode(text)), len(enc_o200k.encode(text))


# 1. Lead sequence (7 notes)
lead_steps_baseline = [
    {"step": 0, "note": "G3", "gate": 1.5, "velocity": 0.85},
    {"step": 2, "note": "Bb3", "gate": 0.9, "velocity": 0.8},
    {"step": 4, "note": "C4", "gate": 2.0, "velocity": 0.95},
    {"step": 8, "note": "Eb4", "gate": 1.2, "velocity": 0.85},
    {"step": 10, "note": "D4", "gate": 0.8, "velocity": 0.75},
    {"step": 12, "note": "Bb3", "gate": 1.8, "velocity": 0.8},
    {"step": 14, "note": "C4", "gate": 1.5, "velocity": 0.9}
]

op_lead_baseline = {
    "op": "insert_notes",
    "pattern_id": "p_lead",
    "notes": lead_steps_baseline
}

op_lead_columnar = {
    "op": "insert_note_batch",
    "pattern_id": "p_lead",
    "encoding": "columns_v1",
    "columns": ["step", "note", "gate", "velocity"],
    "rows": [
        [0, "G3", 1.5, 0.85],
        [2, "Bb3", 0.9, 0.8],
        [4, "C4", 2.0, 0.95],
        [8, "Eb4", 1.2, 0.85],
        [10, "D4", 0.8, 0.75],
        [12, "Bb3", 1.8, 0.8],
        [14, "C4", 1.5, 0.9]
    ]
}

# 2. Hats sequence (16 notes with accents)
hats_steps_baseline = [
    {"step": s, "note": "F#2", "gate": 0.3, "velocity": 0.6 + (0.2 if s % 4 == 2 else 0.0),
     "probability": 0.85}
    for s in range(16)
]

op_hats_baseline = {
    "op": "insert_notes",
    "pattern_id": "p_hats",
    "notes": hats_steps_baseline
}

op_hats_columnar = {
    "op": "insert_note_batch",
    "pattern_id": "p_hats",
    "encoding": "columns_v1",
    "columns": ["step", "velocity"],
    "defaults": {
        "note": "F#2",
        "gate": 0.3,
        "probability": 0.85
    },
    "rows": [
        [s, 0.6 + (0.2 if s % 4 == 2 else 0.0)]
        for s in range(16)
    ]
}

# 3. Microtonal 38-EDO sequence (16 notes)
micro_steps_baseline = [
    {"step": s, "tuned": {"context": "arch_38", "step": (s * 3) % 38}, "gate": 0.8, "velocity": 0.75}
    for s in range(16)
]

op_micro_baseline = {
    "op": "insert_notes",
    "pattern_id": "p_micro",
    "notes": micro_steps_baseline
}

op_micro_columnar = {
    "op": "insert_note_batch",
    "pattern_id": "p_micro",
    "encoding": "columns_v1",
    "columns": ["step", "tuned.step"],
    "defaults": {
        "tuned.context": "arch_38",
        "gate": 0.8,
        "velocity": 0.75
    },
    "rows": [
        [s, (s * 3) % 38]
        for s in range(16)
    ]
}

# 4. Bass harmonic sequence (6 notes)
bass_steps_baseline = [
    {"step": 0, "harmonic": {"kind": "tone", "role": "root", "octave": 0}, "gate": 1.2, "velocity": 0.9},
    {"step": 3, "harmonic": {"kind": "tone", "role": "third", "octave": 0}, "gate": 0.8, "velocity": 0.75},
    {"step": 6, "harmonic": {"kind": "tone", "role": "fifth", "octave": 0}, "gate": 0.8, "velocity": 0.8},
    {"step": 8, "harmonic": {"kind": "tone", "role": "root", "octave": 0}, "gate": 1.0, "velocity": 0.85},
    {"step": 11, "harmonic": {"kind": "tone", "role": "third", "octave": 1}, "gate": 0.7, "velocity": 0.7},
    {"step": 14, "harmonic": {"kind": "tone", "role": "fifth", "octave": 0}, "gate": 0.9, "velocity": 0.75}
]

op_bass_baseline = {
    "op": "insert_notes",
    "pattern_id": "p_bass",
    "notes": bass_steps_baseline
}

op_bass_columnar = {
    "op": "insert_note_batch",
    "pattern_id": "p_bass",
    "encoding": "columns_v1",
    "columns": ["step", "harmonic.role", "harmonic.octave", "gate", "velocity"],
    "defaults": {
        "harmonic.kind": "tone"
    },
    "rows": [
        [0, "root", 0, 1.2, 0.9],
        [3, "third", 0, 0.8, 0.75],
        [6, "fifth", 0, 0.8, 0.8],
        [8, "root", 0, 1.0, 0.85],
        [11, "third", 1, 0.7, 0.7],
        [14, "fifth", 0, 0.9, 0.75]
    ]
}


def evaluate(name: str, baseline_obj: dict, columnar_obj: dict):
    # Test compact JSON formatting (P8 standard)
    base_str = json.dumps(baseline_obj, separators=(",", ":"))
    col_str = json.dumps(columnar_obj, separators=(",", ":"))

    b_len = len(base_str)
    c_len = len(col_str)
    b_tok_cl, b_tok_o2 = count_tokens(base_str)
    c_tok_cl, c_tok_o2 = count_tokens(col_str)

    char_sav = (b_len - c_len) / b_len * 100
    tok_cl_sav = (b_tok_cl - c_tok_cl) / b_tok_cl * 100
    tok_o2_sav = (b_tok_o2 - c_tok_o2) / b_tok_o2 * 100

    print(f"=== {name} ===")
    print(f"  Baseline: {b_len:4d} chars | {b_tok_cl:3d} tokens (cl100k) | {b_tok_o2:3d} tokens (o200k)")
    print(f"  Columnar: {c_len:4d} chars | {c_tok_cl:3d} tokens (cl100k) | {c_tok_o2:3d} tokens (o200k)")
    print(f"  Savings:  {char_sav:5.1f}% chars | {tok_cl_sav:5.1f}% cl100k tokens | {tok_o2_sav:5.1f}% o200k tokens\n")
    return {
        "name": name,
        "base_chars": b_len,
        "col_chars": c_len,
        "base_tokens": b_tok_cl,
        "col_tokens": c_tok_cl,
        "char_saving_pct": char_sav,
        "token_saving_pct": tok_cl_sav
    }


def main():
    print("================================================================================")
    print("   SIBYL P8B: COLUMNAR EVENT BATCHES (insert_note_batch) SAVINGS MEASUREMENT")
    print("================================================================================\n")

    results = []
    results.append(evaluate("Lead Melodic Sequence (7 notes)", op_lead_baseline, op_lead_columnar))
    results.append(evaluate("Hi-Hat Rhythmic Grid (16 notes + defaults)", op_hats_baseline, op_hats_columnar))
    results.append(evaluate("38-EDO Microtonal Sequence (16 notes + tuned.step)", op_micro_baseline, op_micro_columnar))
    results.append(evaluate("Bass Harmonic Progression (6 notes + harmonic.role)", op_bass_baseline, op_bass_columnar))

    tot_base_chars = sum(r["base_chars"] for r in results)
    tot_col_chars = sum(r["col_chars"] for r in results)
    tot_base_toks = sum(r["base_tokens"] for r in results)
    tot_col_toks = sum(r["col_tokens"] for r in results)

    print("=== COMBINED ACROSS ALL FOUR SEQUENCES ===")
    print(f"  Total Baseline: {tot_base_chars} chars | {tot_base_toks} tokens")
    print(f"  Total Columnar: {tot_col_chars} chars | {tot_col_toks} tokens")
    print(f"  Total Savings:  {(tot_base_chars - tot_col_chars) / tot_base_chars * 100:.1f}% chars | {(tot_base_toks - tot_col_toks) / tot_base_toks * 100:.1f}% tokens")
    print("================================================================================")


if __name__ == "__main__":
    main()
