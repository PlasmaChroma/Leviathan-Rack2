#!/usr/bin/env python3
"""
Sibyl P8A MCP Token Reduction Measurement
Runs through MCP tools in Octavia_MCP with both compact (P8A) and legacy indented formatting,
measuring exact character, byte, and token reductions on the live Rack instance.
"""

import asyncio
import json
import os
import sys
import time
from pathlib import Path

# Add MCP to path
sys.path.insert(0, str(Path(__file__).parents[1] / "MCP"))
from mcp_server import Octavia_MCP as server

import tiktoken
cl100k = tiktoken.get_encoding("cl100k_base")
o200k = tiktoken.get_encoding("o200k_base")

RESULTS_DIR = Path("test-results/p8-baseline")
RESULTS_DIR.mkdir(parents=True, exist_ok=True)


async def run_benchmark():
    print("=" * 75)
    print("Sibyl P8A MCP Token & Context Reduction Benchmark")
    print("=" * 75)

    # 1. Discover module ID
    mods_raw = await server.vcv_list_modules()
    mods = json.loads(mods_raw)
    sibyl_id = None
    for m in mods:
        if m.get("plugin") == "Leviathan" and m.get("model") == "Sibyl":
            sibyl_id = m["id"]
            break
    if not sibyl_id:
        raise RuntimeError("No Sibyl module found in Rack!")
    print(f"Discovered Sibyl module ID: {sibyl_id}\n")

    # Benchmarks to run
    measurements = []

    async def measure_call(name: str, coro_factory):
        # Run in legacy indented mode
        os.environ["OCTAVIA_COMPACT_JSON"] = "0"
        legacy_res = await coro_factory()

        # Run in P8A compact mode
        os.environ["OCTAVIA_COMPACT_JSON"] = "1"
        p8a_res = await coro_factory()

        leg_chars = len(legacy_res)
        leg_bytes = len(legacy_res.encode("utf-8"))
        leg_tokens = len(cl100k.encode(legacy_res))

        p8a_chars = len(p8a_res)
        p8a_bytes = len(p8a_res.encode("utf-8"))
        p8a_tokens = len(cl100k.encode(p8a_res))

        saved_tokens = leg_tokens - p8a_tokens
        pct_tokens = 100.0 * saved_tokens / leg_tokens if leg_tokens else 0.0

        item = {
            "name": name,
            "legacy_chars": leg_chars,
            "p8a_chars": p8a_chars,
            "legacy_tokens": leg_tokens,
            "p8a_tokens": p8a_tokens,
            "saved_tokens": saved_tokens,
            "pct_tokens_saved": round(pct_tokens, 1)
        }
        measurements.append(item)
        print(f"{name:<42} | Legacy: {leg_tokens:>5} tok | P8A: {p8a_tokens:>5} tok | Saved: {saved_tokens:>5} ({pct_tokens:>4.1f}%)")
        return p8a_res

    # 1. Capabilities Read (Full vs Compact Manifest)
    async def get_caps_legacy():
        params = server.SibylCapabilitiesInput(module_id=sibyl_id, compact=False)
        return await server.vcv_sibyl_get_capabilities(params)

    async def get_caps_compact():
        params = server.SibylCapabilitiesInput(module_id=sibyl_id, compact=True)
        return await server.vcv_sibyl_get_capabilities(params)

    # Measure full formatting reduction on capabilities
    await measure_call("1. Capabilities Full (Whitespace Strip)", get_caps_legacy)
    
    # Measure Manifest vs Full Capabilities
    os.environ["OCTAVIA_COMPACT_JSON"] = "0"
    full_caps_str = await get_caps_legacy()
    os.environ["OCTAVIA_COMPACT_JSON"] = "1"
    manifest_caps_str = await get_caps_compact()
    tok_full = len(cl100k.encode(full_caps_str))
    tok_man = len(cl100k.encode(manifest_caps_str))
    saved = tok_full - tok_man
    pct = 100.0 * saved / tok_full
    print(f"{'1b. Manifest vs Full Capabilities':<42} | Legacy: {tok_full:>5} tok | P8A: {tok_man:>5} tok | Saved: {saved:>5} ({pct:>4.1f}%)")
    measurements.append({
        "name": "1b. Manifest vs Full Capabilities",
        "legacy_chars": len(full_caps_str),
        "p8a_chars": len(manifest_caps_str),
        "legacy_tokens": tok_full,
        "p8a_tokens": tok_man,
        "saved_tokens": saved,
        "pct_tokens_saved": round(pct, 1)
    })

    # 2. Composition Summary View
    async def get_summary():
        params = server.SibylCompositionInput(module_id=sibyl_id, view="summary")
        return await server.vcv_sibyl_get_composition(params)
    await measure_call("2. Composition Summary View", get_summary)

    # 3. Status Read
    async def get_stat():
        params = server.SibylModuleInput(module_id=sibyl_id)
        return await server.vcv_sibyl_get_status(params)
    await measure_call("3. Runtime Status", get_stat)

    # 4. Pattern Notes Readback (Full Fields)
    async def get_notes_full():
        params = server.SibylCompositionInput(module_id=sibyl_id, view="notes", pattern_id="p_expressive", fields="full")
        return await server.vcv_sibyl_get_composition(params)
    await measure_call("4. Expressive Notes Readback (Full)", get_notes_full)

    # 5. Scene Effective Expressions Projection
    async def get_scene_proj():
        params = server.SibylCompositionInput(module_id=sibyl_id, view="scene", id="chorus", fields=["effectiveExpressions"])
        return await server.vcv_sibyl_get_composition(params)
    await measure_call("5. Scene Effective Expressions", get_scene_proj)

    # 6. Edit Response (Full Changes vs Receipt Profile)
    status_raw = await server.vcv_sibyl_get_status(server.SibylModuleInput(module_id=sibyl_id))
    cur_rev = json.loads(status_raw).get("revision", 1)

    edit_ops = [{
        "op": "transpose_notes",
        "pattern_id": "p_lead",
        "selector": {"limit": 2},
        "semitones": 1
    }]

    async def do_edit_full():
        nonlocal cur_rev
        params = server.SibylEditInput(module_id=sibyl_id, expected_revision=cur_rev, operations=edit_ops, response_profile="full")
        res = await server.vcv_sibyl_edit(params)
        cur_rev = json.loads(res).get("revision", cur_rev + 1)
        return res

    async def do_edit_receipt():
        nonlocal cur_rev
        params = server.SibylEditInput(module_id=sibyl_id, expected_revision=cur_rev, operations=edit_ops, response_profile="receipt")
        res = await server.vcv_sibyl_edit(params)
        cur_rev = json.loads(res).get("revision", cur_rev + 1)
        return res

    os.environ["OCTAVIA_COMPACT_JSON"] = "0"
    edit_full_str = await do_edit_full()
    os.environ["OCTAVIA_COMPACT_JSON"] = "1"
    edit_receipt_str = await do_edit_receipt()
    tok_full_edit = len(cl100k.encode(edit_full_str))
    tok_rec_edit = len(cl100k.encode(edit_receipt_str))
    saved_edit = tok_full_edit - tok_rec_edit
    pct_edit = 100.0 * saved_edit / tok_full_edit
    print(f"{'6. Edit Response (Full vs Receipt)':<42} | Legacy: {tok_full_edit:>5} tok | P8A: {tok_rec_edit:>5} tok | Saved: {saved_edit:>5} ({pct_edit:>4.1f}%)")
    measurements.append({
        "name": "6. Edit Response (Full vs Receipt)",
        "legacy_chars": len(edit_full_str),
        "p8a_chars": len(edit_receipt_str),
        "legacy_tokens": tok_full_edit,
        "p8a_tokens": tok_rec_edit,
        "saved_tokens": saved_edit,
        "pct_tokens_saved": round(pct_edit, 1)
    })

    print("-" * 75)
    total_legacy_tokens = sum(m["legacy_tokens"] for m in measurements)
    total_p8a_tokens = sum(m["p8a_tokens"] for m in measurements)
    total_saved = total_legacy_tokens - total_p8a_tokens
    total_pct = 100.0 * total_saved / total_legacy_tokens
    print(f"{'TOTAL ACROSS TESTED ENDPOINTS':<42} | Legacy: {total_legacy_tokens:>5} tok | P8A: {total_p8a_tokens:>5} tok | Saved: {total_saved:>5} ({total_pct:>4.1f}%)")
    print("=" * 75)

    # Save results
    report = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "total_legacy_tokens": total_legacy_tokens,
        "total_p8a_tokens": total_p8a_tokens,
        "total_saved_tokens": total_saved,
        "total_pct_saved": round(total_pct, 1),
        "endpoints": measurements
    }
    (RESULTS_DIR / "p8a_savings_benchmark.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
    print(f"\nResults saved to: {RESULTS_DIR / 'p8a_savings_benchmark.json'}")


if __name__ == "__main__":
    asyncio.run(run_benchmark())
