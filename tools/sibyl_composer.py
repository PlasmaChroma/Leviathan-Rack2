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
import copy
import random
import os
from fractions import Fraction
import math
import urllib.error
import urllib.request
from typing import Any, Dict, List, Optional, Tuple, Union


_EVENT_FIELDS = set("id step pitchV note degree octave transposeSemitones transposeSteps transposePeriods transposeCents gate velocity probability mod mod2 mod3 glideMs microshift ratchets tie evolve observation condition harmonic tuned".split())
_NESTED_FIELDS = {
    "tuned": set("context step degree cents ratio periods scale".split()),
    "harmonic": set("role toneId periods reference range condition voicing retune kind".split()),
}


def expand_note_batch(operation: dict) -> dict:
    """Validate wire shape and expand columns/onsets/overrides without losing authored values."""
    if operation.get("encoding") != "columns_v1":
        raise ValueError("Expected columns_v1")
    allowed = {"op", "pattern_id", "encoding", "columns", "rows", "defaults", "onsets", "overrides", "collision", "expect_count", "report_id_limit"}
    if operation.keys() - allowed:
        raise ValueError("Unknown batch fields")
    columns, rows = operation.get("columns"), operation.get("rows")
    onsets = operation.get("onsets")
    if not isinstance(columns, list) or not (0 if onsets else 1) <= len(columns) <= 1024:
        raise ValueError("Expected bounded columns")
    if not all(isinstance(c, str) for c in columns) or len(set(columns)) != len(columns):
        raise ValueError("Duplicate or invalid columns")
    if not isinstance(rows, list) or not 1 <= len(rows) <= 1024 or any(not isinstance(row, list) or len(row) != len(columns) for row in rows):
        raise ValueError("Expected rectangular rows")
    def check_field(key):
        if not isinstance(key, str):
            raise ValueError("Invalid field name")
        if "." in key:
            parent, child = key.split(".", 1)
            if child not in _NESTED_FIELDS.get(parent, set()):
                raise ValueError("Unsupported nested field")
        elif key not in _EVENT_FIELDS:
            raise ValueError("Unknown event field")
    for key in columns:
        check_field(key)
        if "." in key and key.split(".")[0] in columns:
            raise ValueError("Conflicting columns")
    defaults = operation.get("defaults", {})
    if not isinstance(defaults, dict):
        raise ValueError("Expected defaults object")
    for key in defaults:
        check_field(key)
    count = len(rows)
    if onsets is not None:
        if not isinstance(onsets, dict) or set(onsets) != {"start", "spacing", "count"}:
            raise ValueError("Expected start, spacing and count")
        start, spacing, count = (onsets[k] for k in ("start", "spacing", "count"))
        if any(type(v) is not int for v in (start, spacing, count)) or not (0 <= start <= 1023 and 1 <= spacing <= 1024 and 1 <= count <= 1024):
            raise ValueError("Invalid onset bounds")
        if "step" in columns or len(rows) > count or start + spacing * (count - 1) > 1023:
            raise ValueError("Conflicting or out-of-range onsets")
    expected = operation.get("expect_count")
    if "expect_count" in operation and (type(expected) is not int or expected != count):
        raise ValueError("Batch count mismatch")
    overrides = operation.get("overrides", {})
    if not isinstance(overrides, dict):
        raise ValueError("Expected override map")
    for key, value in overrides.items():
        if not isinstance(key, str) or not key.isascii() or not key.isdigit() or len(key)>4 or str(int(key)) != key or int(key) >= count:
            raise ValueError("Invalid override row")
        if not isinstance(value, dict) or value.keys() - _EVENT_FIELDS:
            raise ValueError("Invalid override event")
    def assign(event, key, value):
        if "." in key:
            parent, child = key.split(".", 1)
            if not isinstance(event.get(parent), dict):
                event[parent] = {}
            event[parent][child] = copy.deepcopy(value)
        else:
            event[key] = copy.deepcopy(value)
    events = []
    for i in range(count):
        event = {}
        for key in sorted(defaults, key=lambda k: ("." in k, k)):
            assign(event, key, defaults[key])
        if onsets is not None:
            event["step"] = start + spacing * i
        for key, value in zip(columns, rows[i % len(rows)]):
            if value is not None:  # columns_v1 null is an omitted cell.
                assign(event, key, value)
        event.update(copy.deepcopy(overrides.get(str(i), {})))
        if "step" not in event:
            raise ValueError("Missing event step")
        events.append(event)
    return {"op": "insert_notes", "pattern_id": operation.get("pattern_id"), "notes": events,
            **{k: operation[k] for k in ("collision", "report_id_limit") if k in operation}}


def negotiate_operations(operations: List[dict], contract: dict) -> List[dict]:
    """Shape-check batches in both tiers; use canonical operations on legacy builds."""
    result = []
    for operation in operations:
        if operation.get("op") == "insert_note_batch":
            expanded = expand_note_batch(operation)
            modern = contract.get("p8", {}).get("version") == 1
            supported = "insert_note_batch" in contract.get("editOperations", [])
            result.append(operation if supported and (modern or not any(k in operation for k in ("onsets", "overrides"))) else expanded)
        else:
            result.append(operation)
    return result


def decode_note_columns(response: dict) -> List[dict]:
    """Reconstruct a columnar read, retaining missing versus explicit null values."""
    if response.get("encoding") != "columns_v1":
        return copy.deepcopy(response.get("notes", []))
    columns, rows, missing = response["columns"], response["rows"], response.get("missing", {})
    return [{key: copy.deepcopy(value) for col, (key, value) in enumerate(zip(columns, row))
             if col not in missing.get(str(i), [])} for i, row in enumerate(rows)]


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
        "m3": 1200.0 * math.log2(6.0 / 5.0),
        "neutral_third": 350.0,
        "M3": 1200.0 * math.log2(5.0 / 4.0),
        "supermajor_third": 1200.0 * math.log2(9.0 / 7.0),  # ~435.08 cents
        "P4": 500.0,
        "tritone": 600.0,
        "P5": 1200.0 * math.log2(3.0 / 2.0),
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

    def __init__(self, divisions: int = 12, period_ratio: str = "2/1"):
        if type(divisions) is not int or divisions < 1 or divisions > 1024:
            raise ValueError(f"Divisions must be 1–1024, got {divisions}")
        self.divisions = divisions
        if not isinstance(period_ratio, str):
            raise ValueError("Period ratio must be an exact string")
        period = Fraction(period_ratio)
        if period <= 1:
            raise ValueError("Period must exceed unison")
        self.period_cents = 1200.0 * math.log2(float(period))
        if not 1 <= self.period_cents <= 4800:
            raise ValueError("Period must be 1–4800 cents")
        self.period_ratio = period_ratio

    def step_from_cents(self, cents: float) -> int:
        """Finds closest integer step in this EDO for the given cents offset."""
        if not math.isfinite(cents):
            raise ValueError("Expected finite cents")
        return round(self.divisions * cents / self.period_cents)

    def to_operations(self, tuning_id: str, context_id: str, anchor_pitch_v: float = 0.0) -> List[dict]:
        """Define an equal division of an exact period and its anchored pitch context."""
        return [
            {"op": "upsert_tuning", "id": tuning_id, "tuning": {
                "kind": "equal", "divisions": self.divisions, "period": {"ratio": self.period_ratio}}},
            {"op": "upsert_pitch_context", "id": context_id, "context": {
                "tuning": tuning_id, "anchor": {"pitchV": anchor_pitch_v}}}]

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
        if sum(x is not None for x in (note, degree, tuned_step, tuned_ratio, harmonic_role)) != 1:
            raise ValueError("Specify exactly one pitch representation")
        if octave is not None and degree is None:
            raise ValueError("octave is only valid for legacy scale degrees")
        if tuned_step is not None or tuned_ratio is not None:
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
        if ratchets is not None:
            ev["ratchets"] = ratchets
        if glide_ms is not None:
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
                   velocity: float = 0.8,
                   period_steps: Optional[int] = None,
                   scale_degrees: Optional[int] = None,
                   seed: Optional[int] = None) -> PatternBuilder:
        """
        Generates an arpeggiator line over a pitch list with configurable contour.
        `pitches` can be standard note names (e.g. ['C3', 'E3', 'G3']) or degrees/steps.
        """
        if not pitches:
            return self
        if step_interval <= 0 or octave_range < 1 or (scale_degrees is not None and scale_degrees < 1):
            raise ValueError("Positive step interval, octave range and scale size required")
        if contour not in ("up", "down", "pingpong", "random_walk"):
            raise ValueError("Unsupported arpeggio contour")
        if contour == "random_walk" and type(seed) is not int:
            raise ValueError("random_walk requires an explicit integer seed")
        rng = random.Random(seed) if contour == "random_walk" else None
        if octave_range > 1 and any(isinstance(p, int) for p in pitches):
            span = period_steps if self.pitch_context else scale_degrees
            if span is None or span < 1:
                raise ValueError("Integer multi-period arpeggios require period_steps or scale_degrees")

        expanded: List[Union[str, int]] = []
        for oct_idx in range(octave_range):
            for p in pitches:
                if isinstance(p, int):
                    span = period_steps if self.pitch_context else scale_degrees
                    expanded.append(p + oct_idx * (span or 0))
                else:
                    import re
                    match = re.fullmatch(r"([A-Ga-g][#b]?)(-?\d+)", p)
                    if not match:
                        raise ValueError("Expected a note name with octave")
                    expanded.append(match[1] + str(int(match[2]) + oct_idx))

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
                self.add_note(step=step, tuned_step=p if self.pitch_context else None,
                              degree=None if self.pitch_context else p, gate=gate, velocity=velocity)
            else:
                self.add_note(step=step, note=str(p), gate=gate, velocity=velocity)
            step += step_interval
            if contour == "random_walk":
                # A private seeded generator; never touch process-wide randomness.
                seq_idx = min(len(sequence)-1, max(0, seq_idx + (-1 if rng.getrandbits(1) else 1)))
            else:
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
            raise ValueError("Cannot insert an empty batch; use an empty pattern scaffold")

        # Analyze column candidates and invariants
        keys_present = set()
        for ev in sorted_steps:
            for k, v in ev.items():
                if isinstance(v, dict) and k in ("tuned", "harmonic"):
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

        if sum(x is not None for x in (root_note, root_step, root_ratio)) != 1:
            raise ValueError("Specify exactly one chord root")
        if intervals is not None and tones is not None:
            raise ValueError("Specify intervals or tones, not both")
        if not self.pitch_context and (root_step is not None or root_ratio is not None or tones is not None):
            raise ValueError("Native chords require pitch_context")
        if root_note is not None:
            if self.pitch_context:
                c["rootPitch"] = {"note": root_note}
            else:
                c["root"] = root_note
        elif root_step is not None:
            c["rootPitch"] = {"tuned": {"step": root_step}}
        elif root_ratio is not None:
            c["rootPitch"] = {"tuned": {"ratio": root_ratio}}

        if intervals is not None:
            if self.pitch_context:
                if root_step is None:
                    raise ValueError("Native step intervals require a lattice root_step")
                roles = ["root", "third", "fifth", "seventh"]
                c["tones"] = [{"id": f"t{i}", "interval": {"steps": value},
                               "roles": [roles[i]] if i < len(roles) else (["extension"] if i == 4 else [])}
                              for i, value in enumerate(intervals)]
            else:
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

class VoicingBuilder:
    """Build native voice_progression requests; Sibyl's bounded solver owns the search."""
    def __init__(self, progression_id: str, scene_id: str, resolution: str = "1/4",
                 gate_ratio: float = 0.95, write_policy: str = "createOnly"):
        if write_policy not in ("createOnly", "replace") or not 0 <= gate_ratio <= 1:
            raise ValueError("Invalid voicing policy or gate ratio")
        self.operation = {"op": "voice_progression", "progression_id": progression_id,
                          "scene_id": scene_id, "resolution": resolution, "gate_ratio": gate_ratio,
                          "write_policy": write_policy, "voices": []}

    def voice(self, track_id: str, pattern_id: str, minimum: Union[str, dict],
              maximum: Union[str, dict]) -> VoicingBuilder:
        voices = self.operation["voices"]
        if len(voices) >= 8 or any(v["track_id"] == track_id or v["pattern_id"] == pattern_id for v in voices):
            raise ValueError("Voicing requires 1–8 distinct tracks and patterns")
        voices.append({"track_id": track_id, "pattern_id": pattern_id,
                       "min": copy.deepcopy(minimum), "max": copy.deepcopy(maximum)})
        return self

    def spread(self, track_patterns: Dict[str, str], minimum_v: float, maximum_v: float) -> VoicingBuilder:
        """Divide a register among voices in insertion order, from low to high."""
        if not 1 <= len(track_patterns) <= 8 or not -10 <= minimum_v < maximum_v <= 10:
            raise ValueError("Expected 1–8 voices and an ordered register within ±10V")
        width = (maximum_v - minimum_v) / len(track_patterns)
        for i, (track, pattern) in enumerate(track_patterns.items()):
            self.voice(track, pattern, {"pitchV": minimum_v + i * width},
                       {"pitchV": minimum_v + (i + 1) * width})
        return self

    def to_operation(self) -> dict:
        if not self.operation["voices"]:
            raise ValueError("Voicing requires at least one voice")
        return copy.deepcopy(self.operation)


class AutomationBuilder:
    """
    Fluent builder for smooth parametric automation lanes (filter sweeps, resonance, mod).
    """

    def __init__(self, track_id: str, lane: str = "mod", scope: str = "arrangement",
                 scene_id: Optional[str] = None, clock: Optional[str] = None):
        if lane not in ("mod", "mod2", "mod3") or scope not in ("arrangement", "scene"):
            raise ValueError("Unsupported automation lane or scope")
        if scope == "scene" and not scene_id:
            raise ValueError("Scene automation requires scene_id")
        self.clock = clock or ("arrangement" if scope == "arrangement" else "sceneRepeat")
        if self.clock not in (("arrangement",) if scope == "arrangement" else ("sceneRepeat", "sceneVisit")):
            raise ValueError("Automation clock does not match scope")
        self.scene_id = scene_id
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
        if not 0 <= start_beat < mid < end_beat:
            raise ValueError("Envelope requires ordered start, peak and end beats")
        self.add_point(start_beat, start_v, shape)
        self.add_point(mid, peak_v, shape)
        self.add_point(end_beat, end_v, "linear")
        return self

    def add_point(self, beat: float, value: float, shape: str = "smoothstep") -> AutomationBuilder:
        if not math.isfinite(beat) or beat < 0 or not -10 <= value <= 10 or shape not in ("step", "linear", "smoothstep"):
            raise ValueError("Invalid automation point")
        self.points.append({"beat": beat, "value": value, "shape": shape})
        return self

    def to_upsert_op(self, automation_id: str) -> dict:
        sorted_points = sorted(self.points, key=lambda p: p["beat"])
        return {
            "op": "upsert_automation",
            "id": automation_id,
            "automation": {
                "target": {"track": self.track_id, "lane": self.lane},
                "scope": {"arrangement": True} if self.scope == "arrangement" else {"scene": self.scene_id},
                "clock": self.clock,
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

def project_edit_response(response: dict, profile: str = "full", operation_count: Optional[int] = None) -> dict:
    """Project only successful receipts; never discard errors or material warnings."""
    if profile not in ("receipt", "summary", "full"):
        raise ValueError("Unknown response profile")
    if profile == "full" or not response.get("ok"):
        return response
    result = {key: response[key] for key in
              ("ok", "revision", "activeRevision", "pendingRevision", "applyAt", "phasePolicy",
               "appliedOperations", "affectedObjects", "operationSummaries", "warnings") if key in response}
    if "appliedOperations" not in result and operation_count is not None:
        result["appliedOperations"] = operation_count
    changes = response.get("changes", [])
    if isinstance(changes, list):
        totals = {}
        for change in changes:
            for key in ("matched", "inserted", "updated", "deleted", "clamped"):
                value = change.get(key)
                if isinstance(value, (int, float)) and value:
                    totals[key] = totals.get(key, 0) + value
        if totals:
            result["changes"] = totals
        if profile == "summary":
            # Keep per-operation reports without unbounded event details.
            result["operationSummaries"] = [
                {k: v for k, v in change.items() if k in
                 ("operationIndex", "patternId", "matched", "inserted", "updated", "deleted", "clamped")}
                for change in changes]
    elif isinstance(changes, dict):
        result["changes"] = {k: v for k, v in changes.items() if isinstance(v, (int, float)) and v}
    return result


class SibylClient:
    """HTTP client with negotiated formats and instance-bound capability caching."""

    def __init__(self, base_url: str = "http://127.0.0.1:34570", module_id: Optional[int] = None,
                 token: Optional[str] = None):
        self.base_url = base_url.rstrip("/")
        self._module_id = module_id
        self._token = os.environ.get("OCTAVIA_TOKEN", "") if token is None else token
        self._contract_key = None
        self._contract = None

    def _request(self, endpoint: str, payload: Optional[dict] = None):
        headers = {"Content-Type": "application/json"}
        if self._token:
            headers["X-Octavia-Token"] = self._token
        data = None if payload is None else json.dumps(payload, separators=(",", ":"), allow_nan=False).encode()
        request = urllib.request.Request(f"{self.base_url}/{endpoint}", data=data, headers=headers,
                                         method="GET" if payload is None else "POST")
        try:
            with urllib.request.urlopen(request, timeout=5.0) as response:
                return json.loads(response.read().decode("utf-8"))
        except urllib.error.HTTPError as error:
            self._contract_key = self._contract = None
            try:
                body = json.loads(error.read().decode("utf-8"))
            except (ValueError, UnicodeError):
                raise error
            if isinstance(body, dict) and "error" in body:
                return body
            raise error
        except Exception:
            self._contract_key = self._contract = None
            raise

    def get_module_id(self) -> int:
        if self._module_id is not None:
            return self._module_id
        modules = self._request("modules/summary")
        if not isinstance(modules, list):
            raise RuntimeError("Could not read module inventory")
        matches = [m for m in modules if m.get("plugin") == "Leviathan" and m.get("model") == "Sibyl"]
        if len(matches) != 1:
            raise RuntimeError("Expected exactly one Sibyl; supply module_id explicitly")
        self._module_id = matches[0]["id"]
        return self._module_id

    def capabilities(self) -> dict:
        mid = self.get_module_id()
        manifest = self._request(f"sibyl/{mid}/capabilities?format=manifest")
        if not isinstance(manifest, dict) or not manifest.get("ok"):
            self._contract_key = self._contract = None
            raise RuntimeError("Sibyl capability discovery failed")
        if "capabilities" in manifest:
            self._contract_key = self._contract = None
            return manifest["capabilities"]["sibyl"]
        if not manifest.get("instance") or not manifest.get("fingerprint"):
            raise RuntimeError("Missing Sibyl contract identity")
        key = (self.base_url, mid, manifest["instance"], manifest["fingerprint"])
        if key != self._contract_key:
            self._contract_key = self._contract = None
            full = self._request(f"sibyl/{mid}/capabilities")
            if (full.get("instance"), full.get("fingerprint")) != key[-2:]:
                raise RuntimeError("Sibyl instance changed during discovery")
            self._contract = full["capabilities"]["sibyl"]
            self._contract_key = key
        result = copy.deepcopy(self._contract)
        result["revision"] = manifest.get("revision")
        return result

    def get_status(self) -> dict:
        return self._request(f"sibyl/{self.get_module_id()}/status")

    def edit(self, operations: List[dict], expected_revision: Optional[int] = None,
             response_profile: str = "receipt") -> dict:
        project_edit_response({}, response_profile)  # Validate before any mutation.
        contract = self.capabilities()
        if expected_revision is None:
            expected_revision = contract["revision"]
        payload = {"expected_revision": expected_revision,
                   "operations": negotiate_operations(operations, contract)}
        if response_profile in contract.get("p8", {}).get("responseProfiles", []):
            payload["response_profile"] = response_profile
        response = self._request(f"sibyl/{self.get_module_id()}/edit", payload)
        return project_edit_response(response, response_profile, len(operations))

    def validate(self, operations: List[dict], expected_revision: Optional[int] = None,
                 prepare: bool = False, ttl_seconds: int = 60) -> dict:
        contract = self.capabilities()
        if expected_revision is None:
            expected_revision = contract["revision"]
        payload = {"expected_revision": expected_revision,
                   "operations": negotiate_operations(operations, contract), "return_changes": True}
        if prepare:
            if not contract.get("preparedTransactions"):
                raise ValueError("Server does not support prepared transactions")
            payload.update(prepare=True, ttl_seconds=ttl_seconds)
        return self._request(f"sibyl/{self.get_module_id()}/validate", payload)

    def commit_prepared(self, handle: str, expected_revision: Optional[int] = None,
                        response_profile: str = "receipt") -> dict:
        project_edit_response({}, response_profile)
        contract = self.capabilities()
        if not contract.get("preparedTransactions"):
            raise ValueError("Server does not support prepared transactions")
        payload = {"handle": handle}
        if expected_revision is not None:
            payload["expected_revision"] = expected_revision
        if response_profile in contract.get("p8", {}).get("responseProfiles", []):
            payload["response_profile"] = response_profile
        return project_edit_response(self._request(f"sibyl/{self.get_module_id()}/edit", payload), response_profile)

    def two_phase_commit(self, operations: List[dict], expected_revision: Optional[int] = None,
                         response_profile: str = "receipt") -> dict:
        val = self.validate(operations, expected_revision=expected_revision, prepare=True)
        if not val.get("valid"):
            return {"ok": False, "error": "validation_failed", "details": val}
        handle = val.get("handle")
        if not handle:
            return {"ok": False, "error": "no_handle_returned", "details": val}
        return self.commit_prepared(handle, expected_revision=val.get("revision"), response_profile=response_profile)


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

    def add_voicing(self, voicing: VoicingBuilder) -> SibylScore:
        self.operations.append(voicing.to_operation())
        return self

    def add_pattern(self, pattern_id: str, pattern: PatternBuilder,
                    use_columnar_batch: bool = True) -> SibylScore:
        """Adds a pattern. If use_columnar_batch is True, creates pattern then populates with P8B batch."""
        if use_columnar_batch:
            # Create empty pattern scaffold then insert notes via columnar batch
            scaffold = pattern.to_upsert_op(pattern_id)
            scaffold["pattern"]["steps"] = []
            self.operations.append(scaffold)
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

    def two_phase_commit(self, client: Optional[SibylClient] = None,
                         expected_revision: Optional[int] = None,
                         response_profile: str = "receipt") -> dict:
        """Two-phase commit: validates with prepare=True, then commits via opaque handle."""
        c = client or SibylClient()
        return c.two_phase_commit(self.compile(), expected_revision=expected_revision,
                                  response_profile=response_profile)
