"""Generate identical legacy workloads and a maximum-channel expressive workload.

Run from the repository root; optional argument is the output directory.
See doc/Sibyl_P6_Implementation_Report.md for matching native build commands.
"""
import json
from pathlib import Path
import sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else "test-results/p6-benchmark")
root.mkdir(parents=True, exist_ok=True)
for count in (1, 16):
    composition = {
        "meta": {"bpm": 120},
        "transport": {"running": True, "loop": True},
        "tracks": [{"id": f"v{i}", "channel": i} for i in range(count)],
        "patterns": {"p": {"length": 1024, "resolution": "1/64", "steps": [
            {"step": i, "pitchV": (i % 12) / 12, "gate": 0.8} for i in range(1024)
        ]}},
        "arrangement": [{"id": "s", "lengthBeats": 64,
                         "tracks": {f"v{i}": "p" for i in range(count)}}],
    }
    (root / f"legacy-{count}.json").write_text(json.dumps(composition), encoding="utf-8")
    if count != 16:
        continue
    composition["schemaVersion"] = 3
    composition["harmony"] = {
        "progressions": {"p": {"lengthBeats": 64, "chords": [
            {"id": str(i), "beat": i / 2, "root": "C3" if i % 2 == 0 else "A2",
             "intervals": [0, 4, 7] if i % 2 == 0 else [0, 3, 7]} for i in range(128)
        ]}},
        "default": {"progression": "p", "clock": "arrangement", "loop": True},
    }
    for event in composition["patterns"]["p"]["steps"]:
        del event["pitchV"]
        event["harmonic"] = {"kind": "tone", "index": event["step"] % 3}
        event["condition"] = {"all": [{"scope": "patternPass", "every": 2, "offset": 1}]}
        event["probability"] = 0.8
    composition["patterns"]["p"]["evolution"] = {"probability": 0.2}
    composition["automation"] = {
        f"v{channel}_{lane}": {
            "target": {"track": f"v{channel}", "lane": lane},
            "scope": {"arrangement": True}, "clock": "arrangement", "mode": "replace",
            "points": [{"beat": i / 4, "value": (i % 21) / 2 - 5, "shape": "smoothstep"}
                       for i in range(256)],
        } for channel in range(16) for lane in ("mod", "mod2", "mod3")
    }
    (root / "expressive-16.json").write_text(json.dumps(composition), encoding="utf-8")
    # Simultaneously reach maximum event slots, driven lanes, per-curve points,
    # total curve points and ratchets, at the finest supported triplet grid.
    composition["patterns"]["p"]["resolution"] = "1/64t"
    for event in composition["patterns"]["p"]["steps"]:
        event["ratchets"] = 16
    for index, curve in enumerate(composition["automation"].values()):
        count_points = 1024 if index < 15 else 32 if index == 15 else 31
        curve["points"] = [
            {"beat": i * 64 / (count_points - 1), "value": (i % 21) / 2 - 5, "shape": "smoothstep"}
            for i in range(count_points)
        ]
    assert sum(len(c["points"]) for c in composition["automation"].values()) == 16384
    (root / "expressive-max.json").write_text(json.dumps(composition), encoding="utf-8")
