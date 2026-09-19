"""Generate matched legacy/native pitch workloads without installing or opening Rack."""
import json
from pathlib import Path
import subprocess
import sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else "test-results/p7-benchmark")
root.mkdir(parents=True, exist_ok=True)
subprocess.run([sys.executable, "tools/sibyl_benchmark_fixtures.py", str(root)], check=True)
legacy = json.loads((root / "expressive-16.json").read_text())
for divisions in (38, 53):
    score = json.loads(json.dumps(legacy))
    score["schemaVersion"] = 4
    score["pitchSystems"] = {
        "tunings": {"t": {"kind": "equal", "divisions": divisions, "period": {"ratio": "2/1"}}},
        "contexts": {"c": {"tuning": "t", "anchor": {"note": "C4"}}}, "defaultContext": "c",
    }
    progression = score["harmony"]["progressions"]["p"]
    progression["pitchContext"] = "c"
    for chord in progression["chords"]:
        root_note = chord.pop("root")
        chord["rootPitch"] = {"tuned": {"step": -divisions if root_note == "C3" else round(-15 * divisions / 12)}}
        chord["tones"] = [
            {"id": role, "roles": [role], "interval": {"steps": round(interval * divisions / 12)}}
            for interval, role in zip(chord.pop("intervals"), ("root", "third", "fifth"))
        ]
    (root / f"native-{divisions}-16.json").write_text(json.dumps(score), encoding="utf-8")
