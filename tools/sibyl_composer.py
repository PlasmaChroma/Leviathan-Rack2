"""
Sibyl Composer Toolkit (tools/sibyl_composer.py)
================================================
A high-level, deterministic Python composition engine for Leviathan Sibyl.
Lifts generative algorithmic rhythm, microtonal tuning systems (N-EDO),
P7 relative pitch/harmony progressions, automation envelopes, and multi-scene
arrangements into expressive, compact code while compiling into minimal-token
P8 atomic edit operations (insert_note_batch, clone_scene, clone_pattern).

Decoupled from FastMCP: suitable for standalone scratch scripting or macro tools.
"""

from __future__ import annotations

import json
import math
import urllib.error
import urllib.request
from typing import Any, Dict, List, Optional, Tuple, Union


# ══════════════════════════════════════════════════════════════════════════════
# 1. Algorithmic Rhythm: Bjorklund Euclidean Distribution
# ══════════════════════════════════════════════════════════════════════════════

def bjorklund(pulses: int, steps: int, rotation: int = 0) -> List[bool]:
    """
    Computes a maximal-evenness Euclidean rhythm of `pulses` distributed across `steps`.
    Uses Bjorklund's algorithm.
    """
    if steps <= 0:
        return []
    if pulses <= 0:
        return [False] * steps
    if pulses >= steps:
        return [True] * steps

    # Initial sequences: pulses of [1] and (steps - pulses) of [0]
    sequences = [[True] for _ in range(pulses)]
    remainders = [[False] for _ in range(steps - pulses)]

    while remainders:
        count = min(len(sequences), len(remainders))
        for i in range(count):
            sequences[i].extend(remainders.pop())
        if len(sequences) > count:
            remainders = sequences[count:]
            sequences = sequences[:count]

    result: List[bool] = []
    for seq in sequences:
        result.extend(seq)

    # Apply rotation
    if rotation:
        rot = rotation % steps
        result = result[-rot:] + result[:-rot]

    return result


# ══════════════════════════════════════════════════════════════════════════════
# 2. Microtonal Tuning & Interval Translation (N-EDO & P7 Pitch Systems)
# ══════════════════════════════════════════════════════════════════════════════

class EdoTuning:
    """
    Deterministic interval and pitch resolver for N-EDO (Equal Division of Octave)
    and P7 microtonal pitch systems.
    """

    INTERVAL_CENTS = {
        "P1": 0.0,
        "unison": 0.0,
        "m2": 100.0,
        "M2": 200.0,
        "subminor_third": 1200.0 * math.log2(7.0 / 6.0),  # ~266.87 cents
        "m3": 300.0,
        "neutral_third": 350.0,
        "M3": 400.0,
        "supermajor_third": 1200.0 * math.log2(9.0 / 7.0),  # ~435.08 cents
        "P4": 500.0,
        "tritone": 600.0,
        "P5": 700.0,
        "neutral_sixth": 850.0,
        "m6": 800.0,
        "M6": 900.0,
        "harmonic_seventh": 1200.0 * math.log2(7.0 / 4.0),  # ~968.83 cents
        "subminor_seventh": 1200.0 * math.log2(7.0 / 4.0),
        "m7": 1000.0,
        "M7": 1100.0,
        "P8": 1200.0,
        "octave": 1200.0
    }

    CHORD_TEMPLATES = {
        "maj": ["P1", "M3", "P5"],
        "min": ["P1", "m3", "P5"],
        "maj7": ["P1", "M3", "P5", "M7"],
        "min7": ["P1", "m3", "P5", "m7"],
        "dom7": ["P1", "M3", "P5", "m7"],
        "septimal_dom7": ["P1", "M3", "P5", "harmonic_seventh"],
        "septimal_min7": ["P1", "subminor_third", "P5", "harmonic_seventh"],
        "neutral_triad": ["P1", "neutral_third", "P5"],
        "sus4": ["P1", "P4", "P5"],
        "sus2": ["P1", "M2", "P5"],
        "dim": ["P1", "m3", "tritone"],
    }

    def __init__(self, divisions: int = 12):
        if divisions < 1 or divisions > 1024:
            raise ValueError(f"Divisions must be 1–1024, got {divisions}")
        self.divisions = divisions

    def step_from_cents(self, cents: float) -> int:
        """Finds closest integer step in this EDO for the given cents offset."""
        return round(self.divisions * cents / 1200.0)

    def step_from_ratio(self, numerator: int, denominator: int) -> int:
        """Finds closest integer step in this EDO for an exact ratio."""
        cents = 1200.0 * math.log2(numerator / denominator)
        return self.step_from_cents(cents)

    def step_from_interval(self, interval_name: str) -> int:
        """Resolves an interval name (e.g. 'M3', 'harmonic_seventh') to an EDO step."""
        cents = self.INTERVAL_CENTS.get(interval_name)
        if cents is None:
            raise ValueError(f"Unknown interval name: {interval_name}")
        return self.step_from_cents(cents)

    def chord_steps(self, chord_type: str, root_step: int = 0) -> List[int]:
        """Resolves a chord template (e.g. 'maj', 'septimal_dom7') to EDO steps."""
        intervals = self.CHORD_TEMPLATES.get(chord_type)
        if not intervals:
            raise ValueError(f"Unknown chord type: {chord_type}")
        return [root_step + self.step_from_interval(inv) for inv in intervals]


# ══════════════════════════════════════════════════════════════════════════════
# 3. Pattern & Note Sequence Builders (Columnar P8B Optimized)
# ══════════════════════════════════════════════════════════════════════════════

class PatternBuilder:
    """
    Fluent builder for musical patterns with algorithmic generation (Euclidean, arpeggio)
    and automatic compilation into minimal-token P8B columnar batches (insert_note_batch).
    """

    def __init__(self, length: int = 16, resolution: str = "1/16",
                 pitch_context: Optional[str] = None, evolution: Optional[dict] = None):
        self.length = length
        self.resolution = resolution
        self.pitch_context = pitch_context
        self.evolution = evolution
        self.events: Dict[int, dict] = {}  # step -> event dict

    def add_note(self, step: int, note: Optional[str] = None, degree: Optional[int] = None,
                 tuned_step: Optional[int] = None, tuned_ratio: Optional[str] = None,
                 harmonic_role: Optional[str] = None, octave: Optional[int] = None,
                 gate: Optional[float] = None, velocity: Optional[float] = None,
                 probability: Optional[float] = None, ratchets: Optional[int] = None,
                 glide_ms: Optional[float] = None, condition: Optional[dict] = None,
                 evolve: Optional[bool] = None) -> PatternBuilder:
        """Adds or updates a note event at a specific step."""
        if step < 0 or step >= self.length:
            raise ValueError(f"Step {step} outside pattern bounds [0, {self.length - 1}]")

        ev: Dict[str, Any] = {"step": step}
        if note is not None:
            ev["note"] = note
        if degree is not None:
            ev["degree"] = degree
        if octave is not None:
            ev["octave"] = octave
        if tuned_step is not None or tuned_ratio is not None or self.pitch_context:
            tuned: Dict[str, Any] = {}
            if self.pitch_context:
                tuned["context"] = self.pitch_context
            if tuned_step is not None:
                tuned["step"] = tuned_step
            if tuned_ratio is not None:
                tuned["ratio"] = tuned_ratio
            if tuned:
                ev["tuned"] = tuned
        if harmonic_role is not None:
            ev["harmonic"] = {"kind": "tone", "role": harmonic_role}
            if octave is not None:
                ev["harmonic"]["octave"] = octave

        if gate is not None:
            ev["gate"] = gate
        if velocity is not None:
            ev["velocity"] = velocity
        if probability is not None:
            ev["probability"] = probability
        if ratchets is not None and ratchets > 1:
            ev["ratchets"] = ratchets
        if glide_ms is not None and glide_ms > 0:
            ev["glideMs"] = glide_ms
        if condition is not None:
            ev["condition"] = condition
        if evolve is not None:
            ev["evolve"] = evolve

        self.events[step] = ev
        return self

    def euclidean(self, pulses: int, steps: Optional[int] = None,
                  note: Optional[str] = None, degree: Optional[int] = None,
                  tuned_step: Optional[int] = None, velocity: float = 0.8,
                  gate: float = 0.5, rotation: int = 0,
                  accent_interval: Optional[int] = None,
                  accent_velocity: float = 0.95,
                  ratchets: Optional[int] = None) -> PatternBuilder:
        """
        Populates pattern using a Bjorklund Euclidean rhythm distribution.
        """
        grid_steps = steps or self.length
        hits = bjorklund(pulses, grid_steps, rotation=rotation)

        pulse_idx = 0
        for s, hit in enumerate(hits):
            if hit:
                is_accent = accent_interval and (pulse_idx % accent_interval == 0)
                vel = accent_velocity if is_accent else velocity
                ratch = ratchets if (is_accent and ratchets) else None
                self.add_note(step=s, note=note, degree=degree, tuned_step=tuned_step,
                              velocity=vel, gate=gate, ratchets=ratch)
                pulse_idx += 1
        return self

    def arpeggiate(self, pitches: List[Union[str, int]],
                   step_interval: int = 2,
                   contour: str = "up",
                   octave_range: int = 1,
                   gate: float = 0.75,
                   velocity: float = 0.8) -> PatternBuilder:
        """
        Generates an arpeggiator line over a pitch list with configurable contour.
        `pitches` can be standard note names (e.g. ['C3', 'E3', 'G3']) or degrees/steps.
        """
        if not pitches:
            return self

        expanded: List[Union[str, int]] = []
        for oct_idx in range(octave_range):
            for p in pitches:
                if isinstance(p, int):
                    expanded.append(p + (oct_idx * 12))  # degree/step offset
                else:
                    expanded.append(p)

        sequence: List[Union[str, int]] = []
        if contour == "up":
            sequence = expanded
        elif contour == "down":
            sequence = list(reversed(expanded))
        elif contour == "pingpong":
            sequence = expanded + list(reversed(expanded[1:-1]))
        else:
            sequence = expanded

        step = 0
        seq_idx = 0
        while step < self.length:
            p = sequence[seq_idx % len(sequence)]
            if isinstance(p, int):
                self.add_note(step=step, degree=p, gate=gate, velocity=velocity)
            else:
                self.add_note(step=step, note=str(p), gate=gate, velocity=velocity)
            step += step_interval
            seq_idx += 1

        return self

    def to_upsert_op(self, pattern_id: str) -> dict:
        """Compiles pattern to a canonical upsert_pattern operation."""
        sorted_steps = [self.events[s] for s in sorted(self.events.keys())]
        pattern_dict: Dict[str, Any] = {
            "length": self.length,
            "resolution": self.resolution,
            "steps": sorted_steps
        }
        if self.evolution:
            pattern_dict["evolution"] = self.evolution
        if self.pitch_context:
            pattern_dict["pitchContext"] = self.pitch_context
        return {"op": "upsert_pattern", "id": pattern_id, "pattern": pattern_dict}

    def to_columnar_batch(self, pattern_id: str, collision: str = "replace") -> dict:
        """
        Compiles the pattern's notes into a hyper-compact P8B `insert_note_batch` payload,
        automatically factoring invariant attributes into `defaults`.
        """
        sorted_steps = [self.events[s] for s in sorted(self.events.keys())]
        if not sorted_steps:
            return {"op": "insert_note_batch", "pattern_id": pattern_id, "encoding": "columns_v1", "columns": ["step"], "rows": []}

        # Analyze column candidates and invariants
        keys_present = set()
        for ev in sorted_steps:
            for k, v in ev.items():
                if isinstance(v, dict):
                    for sub_k in v:
                        keys_present.add(f"{k}.{sub_k}")
                else:
                    keys_present.add(k)

        # Factor out defaults (attributes present on all events with the exact same value)
        defaults: Dict[str, Any] = {}
        for k in list(keys_present):
            if k == "step":
                continue
            values = []
            for ev in sorted_steps:
                if "." in k:
                    parent, child = k.split(".", 1)
                    val = ev.get(parent, {}).get(child)
                else:
                    val = ev.get(k)
                values.append(val)

            # If present and equal on all notes, move to defaults
            first = values[0]
            if first is not None and all(v == first for v in values):
                defaults[k] = first
                keys_present.remove(k)

        # Build column order: step first, then remaining dynamic fields
        columns = ["step"] + sorted(list(keys_present - {"step"}))

        rows = []
        for ev in sorted_steps:
            row = []
            for col in columns:
                if "." in col:
                    p, c = col.split(".", 1)
                    row.append(ev.get(p, {}).get(c))
                else:
                    row.append(ev.get(col))
            rows.append(row)

        op: Dict[str, Any] = {
            "op": "insert_note_batch",
            "pattern_id": pattern_id,
            "encoding": "columns_v1",
            "columns": columns,
            "rows": rows,
            "collision": collision
        }
        if defaults:
            op["defaults"] = defaults
        return op


# ══════════════════════════════════════════════════════════════════════════════
# 4. Progression & Harmony Builder (P7 Relative Pitch)
# ══════════════════════════════════════════════════════════════════════════════

class ProgressionBuilder:
    """
    Fluent builder for P7 relative pitch progressions and microtonal chord structures.
    """

    def __init__(self, pitch_context: Optional[str] = None, length_beats: float = 16.0):
        self.pitch_context = pitch_context
        self.length_beats = length_beats
        self.chords: List[dict] = []

    def chord(self, beat: float, chord_id: Optional[str] = None,
              root_note: Optional[str] = None, root_step: Optional[int] = None,
              root_ratio: Optional[str] = None, intervals: Optional[List[int]] = None,
              tones: Optional[List[dict]] = None) -> ProgressionBuilder:
        """Adds a chord anchor to the progression."""
        cid = chord_id or f"c_{len(self.chords) + 1}"
        c: Dict[str, Any] = {"id": cid, "beat": beat}

        if root_note is not None:
            c["root"] = root_note
        elif root_step is not None:
            c["rootPitch"] = {"tuned": {"step": root_step}}
        elif root_ratio is not None:
            c["rootPitch"] = {"tuned": {"ratio": root_ratio}}

        if intervals is not None:
            c["intervals"] = intervals
        if tones is not None:
            c["tones"] = tones

        self.chords.append(c)
        return self

    def to_upsert_op(self, progression_id: str) -> dict:
        prog: Dict[str, Any] = {
            "lengthBeats": self.length_beats,
            "chords": self.chords
        }
        if self.pitch_context:
            prog["pitchContext"] = self.pitch_context
        return {"op": "upsert_progression", "id": progression_id, "progression": prog}


# ══════════════════════════════════════════════════════════════════════════════
# 5. Automation Envelope Builder
# ══════════════════════════════════════════════════════════════════════════════

class AutomationBuilder:
    """
    Fluent builder for smooth parametric automation lanes (filter sweeps, resonance, mod).
    """

    def __init__(self, track_id: str, lane: str = "mod", scope: str = "arrangement"):
        self.track_id = track_id
        self.lane = lane
        self.scope = scope
        self.points: List[dict] = []

    def envelope(self, start_beat: float, end_beat: float,
                 start_v: float, peak_v: float, end_v: float,
                 peak_beat: Optional[float] = None,
                 shape: str = "smoothstep") -> AutomationBuilder:
        """Adds a multi-segment parametric swell/decay curve."""
        mid = peak_beat if peak_beat is not None else (start_beat + end_beat) / 2.0
        self.points.extend([
            {"beat": start_beat, "value": start_v, "shape": shape},
            {"beat": mid, "value": peak_v, "shape": shape},
            {"beat": end_beat, "value": end_v, "shape": shape}
        ])
        return self

    def add_point(self, beat: float, value: float, shape: str = "smoothstep") -> AutomationBuilder:
        self.points.append({"beat": beat, "value": value, "shape": shape})
        return self

    def to_upsert_op(self, automation_id: str) -> dict:
        sorted_points = sorted(self.points, key=lambda p: p["beat"])
        return {
            "op": "upsert_automation",
            "id": automation_id,
            "automation": {
                "target": {"track": self.track_id, "lane": self.lane},
                "scope": {"arrangement": True} if self.scope == "arrangement" else {"pattern": True},
                "clock": "arrangement",
                "mode": "replace",
                "points": sorted_points
            }
        }


# ══════════════════════════════════════════════════════════════════════════════
# 6. Arrangement & Scene Assembly
# ══════════════════════════════════════════════════════════════════════════════

class ArrangementBuilder:
    """
    Assembles multi-track scene progressions and exploits P8C clone_scene.
    """

    def __init__(self):
        self.tracks: List[dict] = []
        self.scenes: List[dict] = []
        self.operations: List[dict] = []

    def add_track(self, track_id: str, channel: int,
                  default_gate: float = 0.8, default_velocity: float = 0.8) -> ArrangementBuilder:
        self.operations.append({
            "op": "upsert_track",
            "id": track_id,
            "track": {"channel": channel, "defaultGate": default_gate, "defaultVelocity": default_velocity}
        })
        return self

    def add_scene(self, scene_id: str, length_beats: float = 16.0, repeats: int = 1,
                  tracks: Optional[Dict[str, str]] = None,
                  description: Optional[str] = None) -> ArrangementBuilder:
        scene: Dict[str, Any] = {
            "lengthBeats": length_beats,
            "repeats": repeats,
            "tracks": tracks or {}
        }
        if description:
            scene["description"] = description
        self.operations.append({"op": "upsert_scene", "id": scene_id, "scene": scene})
        return self

    def clone_scene(self, source_id: str, id: str,
                    track_overrides: Optional[dict] = None,
                    repeats: Optional[int] = None,
                    patterns: str = "share",
                    position: str = "after_source") -> ArrangementBuilder:
        """Emits P8C clone_scene operation."""
        op: Dict[str, Any] = {
            "op": "clone_scene",
            "source_id": source_id,
            "id": id,
            "patterns": patterns,
            "position": position
        }
        if track_overrides:
            op["track_overrides"] = track_overrides
        if repeats is not None:
            op["repeats"] = repeats
        self.operations.append(op)
        return self


# ══════════════════════════════════════════════════════════════════════════════
# 7. SibylClient & SibylScore Orchestrator
# ══════════════════════════════════════════════════════════════════════════════

class SibylClient:
    """
    Lightweight HTTP communication bridge directly to VCV Rack / Octavia / Sibyl.
    """

    def __init__(self, base_url: str = "http://127.0.0.1:34570"):
        self.base_url = base_url.rstrip("/")
        self._module_id: Optional[int] = None

    def get_module_id(self) -> int:
        """Finds the first active Sibyl module ID via /modules/summary."""
        if self._module_id is not None:
            return self._module_id
        req = urllib.request.Request(f"{self.base_url}/modules/summary")
        with urllib.request.urlopen(req, timeout=2.0) as resp:
            mods = json.loads(resp.read().decode("utf-8"))
            for m in mods:
                if m.get("plugin") == "Leviathan" and m.get("model") == "Sibyl":
                    self._module_id = m["id"]
                    return self._module_id
        raise RuntimeError("No active Sibyl module found in VCV Rack!")

    def get_status(self) -> dict:
        mid = self.get_module_id()
        req = urllib.request.Request(f"{self.base_url}/sibyl/{mid}/status")
        with urllib.request.urlopen(req, timeout=2.0) as resp:
            return json.loads(resp.read().decode("utf-8"))

    def edit(self, operations: List[dict], expected_revision: Optional[int] = None,
             response_profile: str = "receipt") -> dict:
        """Submits an atomic edit transaction."""
        mid = self.get_module_id()
        if expected_revision is None:
            st = self.get_status()
            expected_revision = st.get("revision", 0)

        payload = {
            "expected_revision": expected_revision,
            "operations": operations
        }
        data = json.dumps(payload, separators=(",", ":")).encode("utf-8")
        req = urllib.request.Request(
            f"{self.base_url}/sibyl/{mid}/edit",
            data=data,
            headers={"Content-Type": "application/json"},
            method="POST"
        )
        with urllib.request.urlopen(req, timeout=5.0) as resp:
            res = json.loads(resp.read().decode("utf-8"))
            if response_profile == "receipt" and res.get("ok"):
                return {
                    "ok": True,
                    "revision": res.get("revision"),
                    "activeRevision": res.get("activeRevision"),
                    "appliedOperations": len(operations),
                    "warnings": res.get("warnings", [])
                }
            return res


class SibylScore:
    """
    Top-level composition orchestrator. Bundles patterns, progressions,
    automation, and arrangement into a unified atomic transaction.
    """

    def __init__(self, title: str = "Piece", bpm: float = 120.0):
        self.title = title
        self.bpm = bpm
        self.operations: List[dict] = [
            {"op": "set_meta", "path": "title", "value": title},
            {"op": "set_meta", "path": "bpm", "value": bpm}
        ]

    def add_operation(self, op: dict) -> SibylScore:
        self.operations.append(op)
        return self

    def add_pattern(self, pattern_id: str, pattern: PatternBuilder,
                    use_columnar_batch: bool = True) -> SibylScore:
        """Adds a pattern. If use_columnar_batch is True, creates pattern then populates with P8B batch."""
        if use_columnar_batch:
            # Create empty pattern scaffold then insert notes via columnar batch
            self.operations.append({
                "op": "upsert_pattern",
                "id": pattern_id,
                "pattern": {
                    "length": pattern.length,
                    "resolution": pattern.resolution,
                    "steps": []
                }
            })
            if pattern.events:
                self.operations.append(pattern.to_columnar_batch(pattern_id))
        else:
            self.operations.append(pattern.to_upsert_op(pattern_id))
        return self

    def add_progression(self, progression_id: str, progression: ProgressionBuilder) -> SibylScore:
        self.operations.append(progression.to_upsert_op(progression_id))
        return self

    def add_automation(self, automation_id: str, automation: AutomationBuilder) -> SibylScore:
        self.operations.append(automation.to_upsert_op(automation_id))
        return self

    def add_arrangement(self, arrangement: ArrangementBuilder) -> SibylScore:
        self.operations.extend(arrangement.operations)
        return self

    def compile(self) -> List[dict]:
        """Returns the complete list of operations for atomic transaction submission."""
        return self.operations

    def commit(self, client: Optional[SibylClient] = None,
               expected_revision: Optional[int] = None) -> dict:
        """Directly executes the compiled score against VCV Rack."""
        c = client or SibylClient()
        return c.edit(self.compile(), expected_revision=expected_revision)
