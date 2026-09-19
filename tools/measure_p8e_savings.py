#!/usr/bin/env python3
"""
Measurement script for P8E Prepared Transactions (Two-Phase Commit).
Compares standard two-round-trip flow (validate -> edit retransmitting operations)
against P8E prepared flow (validate with prepare:true -> edit with handle & receipt).
"""

import json
import sys
from pathlib import Path

# Add tools directory to path
sys.path.insert(0, str(Path(__file__).parent))
import sibyl_composer as sc


def count_tokens(text: str) -> int:
    try:
        import tiktoken
        enc = tiktoken.get_encoding("cl100k_base")
        return len(enc.encode(text))
    except Exception:
        return max(1, (len(text) + 3) // 4)


def run_benchmark():
    # Sequence 1: Dense 32-step melodic sequence
    steps = [
        {"step": i, "pitch": 60 + (i * 7) % 24, "gate": 0.8, "velocity": 0.9, "evolution": "evolve"}
        for i in range(32)
    ]
    columns = ["step", "pitch", "gate", "velocity", "evolution"]
    rows = [[s[k] for k in columns] for s in steps]
    dense_ops = [
        {
            "op": "insert_note_batch",
            "pattern_id": "lead",
            "format": "columns_v1",
            "columns": columns,
            "rows": rows
        }
    ]

    # Standard Flow
    std_val_req = {
        "expected_revision": 10,
        "return_changes": True,
        "operations": dense_ops
    }
    std_val_resp = {
        "ok": True,
        "revision": 10,
        "valid": True,
        "errors": [],
        "warnings": [],
        "changes": {
            "notesInserted": 32,
            "patternsUpserted": 0,
            "notes": [{"step": s["step"], "id": f"n{s['step']+1}", "action": "insert"} for s in steps]
        }
    }
    std_edit_req = {
        "expected_revision": 10,
        "phase_policy": "preserve",
        "operations": dense_ops
    }
    std_edit_resp = {
        "ok": True,
        "revision": 11,
        "activeRevision": 10,
        "pendingRevision": 11,
        "applyAt": "nextBeat",
        "phasePolicy": "preserve",
        "warnings": [],
        "changes": {
            "notesInserted": 32,
            "patternsUpserted": 0,
            "notes": [{"step": s["step"], "id": f"n{s['step']+1}", "action": "insert"} for s in steps]
        }
    }

    std_out1 = json.dumps(std_val_req, separators=(",", ":"))
    std_in1 = json.dumps(std_val_resp, separators=(",", ":"))
    std_out2 = json.dumps(std_edit_req, separators=(",", ":"))
    std_in2 = json.dumps(std_edit_resp, separators=(",", ":"))

    std_total_out = len(std_out1) + len(std_out2)
    std_total_in = len(std_in1) + len(std_in2)
    std_total = std_total_out + std_total_in

    std_tok_out = count_tokens(std_out1) + count_tokens(std_out2)
    std_tok_in = count_tokens(std_in1) + count_tokens(std_in2)
    std_tok_total = std_tok_out + std_tok_in

    # P8E Prepared Flow
    p8e_val_req = {
        "expected_revision": 10,
        "prepare": True,
        "return_changes": True,
        "operations": dense_ops
    }
    p8e_val_resp = {
        "ok": True,
        "revision": 10,
        "valid": True,
        "handle": "prep_rev10_1",
        "expiresInSeconds": 60,
        "errors": [],
        "warnings": [],
        "changes": {
            "notesInserted": 32,
            "patternsUpserted": 0,
            "notes": [{"step": s["step"], "id": f"n{s['step']+1}", "action": "insert"} for s in steps]
        }
    }
    p8e_edit_req = {
        "handle": "prep_rev10_1",
        "response_profile": "receipt"
    }
    p8e_edit_resp = {
        "ok": True,
        "revision": 11,
        "activeRevision": 10,
        "appliedOperations": 1,
        "changes": {"notesInserted": 32},
        "warnings": []
    }

    p8e_out1 = json.dumps(p8e_val_req, separators=(",", ":"))
    p8e_in1 = json.dumps(p8e_val_resp, separators=(",", ":"))
    p8e_out2 = json.dumps(p8e_edit_req, separators=(",", ":"))
    p8e_in2 = json.dumps(p8e_edit_resp, separators=(",", ":"))

    p8e_total_out = len(p8e_out1) + len(p8e_out2)
    p8e_total_in = len(p8e_in1) + len(p8e_in2)
    p8e_total = p8e_total_out + p8e_total_in

    p8e_tok_out = count_tokens(p8e_out1) + count_tokens(p8e_out2)
    p8e_tok_in = count_tokens(p8e_in1) + count_tokens(p8e_in2)
    p8e_tok_total = p8e_tok_out + p8e_tok_in

    print("==========================================================================")
    print("                SIBYL P8E PREPARED TRANSACTION BENCHMARK")
    print("==========================================================================")
    print(f"Phase 2 Commit Request Payload:")
    print(f"  Standard Edit (Retransmitting Ops): {len(std_out2)} chars ({count_tokens(std_out2)} tokens)")
    print(f"  P8E Edit (Opaque Handle Only):      {len(p8e_out2)} chars ({count_tokens(p8e_out2)} tokens)")
    print(f"  Phase 2 Outbound Reduction:        {(1 - len(p8e_out2)/len(std_out2))*100:.1f}% chars, {(1 - count_tokens(p8e_out2)/count_tokens(std_out2))*100:.1f}% tokens!")
    print("--------------------------------------------------------------------------")
    print(f"Phase 2 Commit Response Receipt:")
    print(f"  Standard Edit Response:             {len(std_in2)} chars ({count_tokens(std_in2)} tokens)")
    print(f"  P8E Receipt Response:               {len(p8e_in2)} chars ({count_tokens(p8e_in2)} tokens)")
    print(f"  Phase 2 Inbound Reduction:         {(1 - len(p8e_in2)/len(std_in2))*100:.1f}% chars, {(1 - count_tokens(p8e_in2)/count_tokens(std_in2))*100:.1f}% tokens!")
    print("--------------------------------------------------------------------------")
    print(f"Total Two-Phase Commit Round-Trip Summary:")
    print(f"  Standard Total Outbound:            {std_total_out} chars ({std_tok_out} tokens)")
    print(f"  P8E Total Outbound:                 {p8e_total_out} chars ({p8e_tok_out} tokens)")
    print(f"  Outbound Total Savings:            {(1 - p8e_total_out/std_total_out)*100:.1f}% chars, {(1 - p8e_tok_out/std_tok_out)*100:.1f}% tokens")
    print(f"  Standard Total Inbound:             {std_total_in} chars ({std_tok_in} tokens)")
    print(f"  P8E Total Inbound:                  {p8e_total_in} chars ({p8e_tok_in} tokens)")
    print(f"  Inbound Total Savings:             {(1 - p8e_total_in/std_total_in)*100:.1f}% chars, {(1 - p8e_tok_in/std_tok_in)*100:.1f}% tokens")
    print(f"  Total Round-Trip:                   {std_total} -> {p8e_total} chars ({(1 - p8e_total/std_total)*100:.1f}% reduction)")
    print(f"  Total Tokens:                       {std_tok_total} -> {p8e_tok_total} tokens ({(1 - p8e_tok_total/std_tok_total)*100:.1f}% reduction)")
    print("==========================================================================")


if __name__ == "__main__":
    run_benchmark()
