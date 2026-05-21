#!/usr/bin/env python3
"""SDT-SL Conductor v0 for cvOSCcv lane control in VCV Rack."""

from __future__ import annotations

import argparse
import json
import math
import random
import signal
import socket
import sys
import time
from dataclasses import dataclass
from typing import Callable


@dataclass(frozen=True)
class LaneSpec:
    index: int
    key: str
    label: str
    name: str
    kind: str
    default_range: str
    description: str
    suggested_patch_target: str


@dataclass
class SdtSlState:
    pressure: float = 0.0
    breath: float = 0.0
    identity: float = 0.0
    constraint: float = 0.0
    drift: float = 0.0
    resolution: float = 0.0
    threshold: float = 0.0
    phrase: float = 0.0


@dataclass(frozen=True)
class ProgramSpec:
    key: str
    title: str
    dialect: str
    description: str
    fn: Callable[[float, float, random.Random], SdtSlState]


LANES = [
    LaneSpec(1, "pressure", "P", "Pressure Density", "continuous", "0.0..1.0", "Intensity and accumulated force.", "VCA CV / filter drive"),
    LaneSpec(2, "breath", "B", "Breath Curve", "continuous", "0.0..1.0", "Body-like phrase envelope.", "Envelope amount / texture morph"),
    LaneSpec(3, "identity", "I", "Identity Persistence", "continuous", "0.0..1.0", "Stable motif anchor.", "Pitch center / cutoff center"),
    LaneSpec(4, "constraint", "C", "Constraint Field", "continuous", "0.0..1.0", "Restraint and blocked motion.", "Probability clamp / slew amount"),
    LaneSpec(5, "drift", "D", "Drift Instability", "continuous", "0.0..1.0", "Slippage and mutation.", "Detune / modulation depth"),
    LaneSpec(6, "resolution", "R", "Resolution Permission", "continuous", "0.0..1.0", "Permission to settle.", "Cadence enable / wet-dry open"),
    LaneSpec(7, "threshold", "T", "Threshold Gate", "gate", "0.0..1.0", "Boundary crossing event gate.", "Clock / trigger input"),
    LaneSpec(8, "phrase", "PH", "Phrase Gate", "gate", "0.0..1.0", "Phrase and section gate.", "Reset / section advance trigger"),
]

LANE_BY_KEY = {lane.key: lane for lane in LANES}

running = True


class OscSender:
    def __init__(self, host: str, port: int) -> None:
        self._host = host
        self._port = port
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def send_message(self, address: str, value: float) -> None:
        from pythonosc import osc_message_builder

        builder = osc_message_builder.OscMessageBuilder(address=address)
        builder.add_arg(float(value), arg_type="f")
        msg = builder.build()
        self._sock.sendto(msg.dgram, (self._host, self._port))

    def close(self) -> None:
        self._sock.close()


def clamp01(x: float) -> float:
    return max(0.0, min(1.0, x))


def smoothstep(x: float) -> float:
    x = clamp01(x)
    return x * x * (3.0 - 2.0 * x)


def triangle_phase(phase: float) -> float:
    """0..1 triangle LFO."""
    phase = phase % 1.0
    return 1.0 - abs(2.0 * phase - 1.0)


def handle_signal(_sig: int, _frame: object) -> None:
    global running
    running = False


def state_dict(state: SdtSlState) -> dict[str, float]:
    return {
        "pressure": state.pressure,
        "breath": state.breath,
        "identity": state.identity,
        "constraint": state.constraint,
        "drift": state.drift,
        "resolution": state.resolution,
        "threshold": state.threshold,
        "phrase": state.phrase,
    }


def scaled_values_by_lane(state: SdtSlState, scale: float) -> dict[str, float]:
    return {k: clamp01(v) * scale for k, v in state_dict(state).items()}


def monitor_line(values: dict[str, float]) -> str:
    return " ".join(f"{lane.label}:{values[lane.key]:>5.2f}" for lane in LANES)


def monitor_json(t: float, program: str, values: dict[str, float]) -> str:
    payload = {
        "t": round(t, 3),
        "program": program,
        "lanes": {lane.key: round(values[lane.key], 6) for lane in LANES},
    }
    return json.dumps(payload, separators=(",", ":"))


def normalize_prefix(prefix: str) -> str:
    if not prefix:
        raise ValueError("--address-prefix cannot be empty")
    p = prefix.strip()
    if not p:
        raise ValueError("--address-prefix cannot be blank")
    if not p.startswith("/"):
        p = "/" + p
    return "/" + "/".join(part for part in p.split("/") if part)


def normalize_namespace(namespace: str) -> str:
    ns = namespace.strip()
    if not ns:
        return ""
    ns = ns.strip("/")
    if not ns:
        return ""
    return "/" + "/".join(part for part in ns.split("/") if part)


def osc_address(lane: LaneSpec, args: argparse.Namespace) -> str:
    namespace = normalize_namespace(args.namespace)
    if args.address_mode == "numbered":
        body = f"{normalize_prefix(args.address_prefix)}/{lane.index}"
    else:
        body = f"/{lane.key}"
    return f"{namespace}{body}" if namespace else body


def validate_args(args: argparse.Namespace) -> None:
    if args.bpm <= 0.0:
        raise ValueError("--bpm must be > 0")
    if args.hz <= 0.0:
        raise ValueError("--hz must be > 0")
    if args.scale <= 0.0:
        raise ValueError("--scale must be > 0")
    if args.port <= 0 or args.port > 65535:
        raise ValueError("--port must be in range 1..65535")
    try:
        socket.getaddrinfo(args.host, args.port, proto=socket.IPPROTO_UDP)
    except socket.gaierror as exc:
        raise ValueError(f"Invalid --host '{args.host}': {exc}") from exc
    normalize_prefix(args.address_prefix)
    normalize_namespace(args.namespace)


def print_lanes() -> None:
    print("idx key        label kind        range    name")
    for lane in LANES:
        print(f"{lane.index:>3} {lane.key:<10} {lane.label:<5} {lane.kind:<10} {lane.default_range:<8} {lane.name}")
        print(f"    desc: {lane.description}")
        print(f"    patch: {lane.suggested_patch_target}")


def print_programs(programs: dict[str, ProgramSpec]) -> None:
    for key in sorted(programs.keys()):
        p = programs[key]
        print(f"{p.key:<12} {p.title:<20} {p.description}")


def print_addresses(args: argparse.Namespace) -> None:
    for lane in LANES:
        print(f"{lane.index:>2} {lane.key:<10} {osc_address(lane, args)}")


def send_state(client: object, state: SdtSlState, scale: float, args: argparse.Namespace) -> None:
    values = scaled_values_by_lane(state, scale)
    for lane in LANES:
        client.send_message(osc_address(lane, args), float(values[lane.key]))


def no_fin_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    beat_hz = bpm / 60.0
    beat = t * beat_hz
    phrase_phase = (beat / 16.0) % 1.0
    macro_phase = (beat / 64.0) % 1.0

    breath_raw = triangle_phase(phrase_phase)
    breath = smoothstep(breath_raw)
    pressure = 0.25 + 0.65 * smoothstep(macro_phase)
    identity_base = 0.35
    identity_pulse = 0.08 * math.sin(2.0 * math.pi * (beat / 8.0))
    identity_micro = 0.015 * math.sin(2.0 * math.pi * (beat * 1.618))
    identity = identity_base + identity_pulse + identity_micro
    constraint = 0.78 + 0.12 * (1.0 - breath)
    drift = 0.45 + 0.35 * smoothstep(macro_phase)
    drift += 0.05 * math.sin(2.0 * math.pi * (beat / 3.0))
    near_phrase_end = 1.0 if phrase_phase > 0.86 else 0.0
    false_cadence = near_phrase_end * (0.18 + 0.10 * math.sin(2.0 * math.pi * beat))
    resolution = 0.08 + false_cadence
    threshold = 1.0 if (phrase_phase < 0.035 and macro_phase > 0.50) else 0.0
    phrase = 1.0 if phrase_phase < 0.035 else 0.0

    return SdtSlState(
        pressure=clamp01(pressure),
        breath=clamp01(breath),
        identity=clamp01(identity),
        constraint=clamp01(constraint),
        drift=clamp01(drift),
        resolution=clamp01(resolution),
        threshold=clamp01(threshold),
        phrase=clamp01(phrase),
    )


def alethe_field_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    beat_hz = bpm / 60.0
    beat = t * beat_hz
    phrase_phase = (beat / 24.0) % 1.0

    breath = smoothstep(triangle_phase(phrase_phase))
    pressure = 0.20 + 0.25 * breath
    identity = 0.45 + 0.05 * math.sin(2.0 * math.pi * beat / 12.0)
    constraint = 0.25 + 0.10 * (1.0 - breath)
    drift = 0.18 + 0.03 * math.sin(2.0 * math.pi * beat / 5.0)
    resolution = 0.65 + 0.25 * breath
    threshold = 1.0 if phrase_phase < 0.025 else 0.0
    phrase = threshold

    return SdtSlState(
        pressure=clamp01(pressure),
        breath=clamp01(breath),
        identity=clamp01(identity),
        constraint=clamp01(constraint),
        drift=clamp01(drift),
        resolution=clamp01(resolution),
        threshold=clamp01(threshold),
        phrase=clamp01(phrase),
    )


def threshold_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    beat_hz = bpm / 60.0
    beat = t * beat_hz
    phrase_phase = (beat / 32.0) % 1.0
    pulse_phase = (beat / 4.0) % 1.0
    sub_phase = (beat / 2.0) % 1.0

    breath = 0.08 + 0.16 * smoothstep(triangle_phase(phrase_phase))
    pressure = 0.06 + 0.18 * smoothstep(triangle_phase((beat / 12.0) % 1.0))
    identity = 0.30 + 0.03 * math.sin(2.0 * math.pi * beat / 10.0)
    constraint = 0.60 + 0.20 * (1.0 - smoothstep(triangle_phase(phrase_phase)))
    drift = 0.10 + 0.04 * math.sin(2.0 * math.pi * beat / 6.0)
    resolution = 0.22 + 0.08 * smoothstep(triangle_phase((beat / 16.0) % 1.0))
    threshold = 1.0 if pulse_phase < 0.09 else 0.0
    phrase = 1.0 if sub_phase < 0.14 and phrase_phase < 0.25 else 0.0

    return SdtSlState(
        pressure=clamp01(pressure),
        breath=clamp01(breath),
        identity=clamp01(identity),
        constraint=clamp01(constraint),
        drift=clamp01(drift),
        resolution=clamp01(resolution),
        threshold=clamp01(threshold),
        phrase=clamp01(phrase),
    )


def edict_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    beat_hz = bpm / 60.0
    beat = t * beat_hz
    cycle16 = (beat / 16.0) % 1.0
    cycle64 = (beat / 64.0) % 1.0
    gate8 = (beat / 8.0) % 1.0
    phrase64 = (beat / 64.0) % 1.0

    pressure = 0.72 + 0.18 * smoothstep(triangle_phase(cycle64))
    breath = 0.10 + 0.10 * smoothstep(triangle_phase(cycle16))
    identity = 0.58 + 0.03 * math.sin(2.0 * math.pi * beat / 32.0)
    constraint = 0.82 + 0.12 * (1.0 - smoothstep(triangle_phase(cycle16)))
    drift = 0.07 + 0.02 * math.sin(2.0 * math.pi * beat / 12.0)
    resolution = 0.22 + 0.20 * smoothstep(triangle_phase((beat / 24.0) % 1.0))
    threshold = 1.0 if gate8 < 0.06 and cycle16 > 0.45 else 0.0
    phrase = 1.0 if phrase64 < 0.045 else 0.0

    return SdtSlState(
        pressure=clamp01(pressure),
        breath=clamp01(breath),
        identity=clamp01(identity),
        constraint=clamp01(constraint),
        drift=clamp01(drift),
        resolution=clamp01(resolution),
        threshold=clamp01(threshold),
        phrase=clamp01(phrase),
    )


def probe_ramp_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    cycle = 6.0
    lane_slot = int(t // cycle) % len(LANES)
    local_phase = (t % cycle) / cycle
    values = {lane.key: 0.0 for lane in LANES}
    values[LANES[lane_slot].key] = local_phase
    return SdtSlState(**values)


def probe_gates_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    beat = t * (bpm / 60.0)
    gate_a = 1.0 if (beat % 2.0) < 0.16 else 0.0
    gate_b = 1.0 if ((beat + 1.0) % 2.0) < 0.16 else 0.0
    return SdtSlState(
        pressure=0.2,
        breath=0.4,
        identity=0.6,
        constraint=0.3,
        drift=0.1,
        resolution=0.8,
        threshold=gate_a,
        phrase=gate_b,
    )


def probe_all_program(t: float, bpm: float, seed_rng: random.Random) -> SdtSlState:
    beat = t * (bpm / 60.0)
    threshold = 1.0 if (beat % 1.0) < 0.12 else 0.0
    phrase = 1.0 if ((beat + 0.5) % 2.0) < 0.18 else 0.0
    return SdtSlState(
        pressure=0.1,
        breath=0.25,
        identity=0.40,
        constraint=0.55,
        drift=0.70,
        resolution=0.85,
        threshold=threshold,
        phrase=phrase,
    )


def build_programs() -> dict[str, ProgramSpec]:
    return {
        "alethe": ProgramSpec("alethe", "Alethe Field", "Alethe", "gentle presence / high resolution", alethe_field_program),
        "no_fin": ProgramSpec("no_fin", "No Fin", "SDT-SL", "pressure rise / resolution denied", no_fin_program),
        "threshold": ProgramSpec("threshold", "Threshold", "SDT-SL", "sparse gates / low pressure", threshold_program),
        "edict": ProgramSpec("edict", "Edict Field", "SDT-SL", "high constraint / law-weight", edict_program),
        "probe_ramp": ProgramSpec("probe_ramp", "Probe Ramp", "Probe", "single-lane 0->1 scanning ramp", probe_ramp_program),
        "probe_gates": ProgramSpec("probe_gates", "Probe Gates", "Probe", "alternating threshold/phrase gates", probe_gates_program),
        "probe_all": ProgramSpec("probe_all", "Probe All", "Probe", "fixed continuous values with gate pulses", probe_all_program),
    }


def parse_args(program_keys: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=7001)
    parser.add_argument("--bpm", type=float, default=60.0)
    parser.add_argument("--hz", type=float, default=30.0, help="OSC update rate")
    parser.add_argument("--program", choices=program_keys, default="alethe")
    parser.add_argument("--scale", type=float, default=10.0)
    parser.add_argument("--seed", type=int, default=144)
    parser.add_argument("--dry-run", action="store_true", help="Print values without OSC send")
    parser.add_argument("--json-monitor", action="store_true")
    parser.add_argument("--list-programs", action="store_true")
    parser.add_argument("--list-lanes", action="store_true")
    parser.add_argument("--print-addresses", action="store_true")
    parser.add_argument("--address-mode", choices=["numbered", "named"], default="numbered")
    parser.add_argument("--address-prefix", default="/ch")
    parser.add_argument("--namespace", default="")
    return parser.parse_args()


def main() -> int:
    programs = build_programs()
    args = parse_args(sorted(programs.keys()))

    if args.list_programs:
        print_programs(programs)
        return 0
    if args.list_lanes:
        print_lanes()
        return 0

    validate_args(args)

    if args.print_addresses:
        print_addresses(args)
        return 0

    signal.signal(signal.SIGINT, handle_signal)
    signal.signal(signal.SIGTERM, handle_signal)

    client: OscSender | None = None
    if not args.dry_run:
        try:
            from pythonosc import osc_message_builder as _unused_builder  # noqa: F401
        except ModuleNotFoundError as exc:
            if exc.name == "pythonosc":
                print(
                    "Missing dependency: python-osc\n"
                    "Install with:\n"
                    "  py -m pip install python-osc\n"
                    "or:\n"
                    "  python -m pip install python-osc",
                    file=sys.stderr,
                )
                return 2
            raise
        try:
            client = OscSender(args.host, args.port)
        except PermissionError as exc:
            print(
                "Unable to open UDP socket for OSC send.\n"
                "Check local socket permissions/firewall policy and retry.\n"
                f"Details: {exc}",
                file=sys.stderr,
            )
            return 2

    rng = random.Random(args.seed)
    program = programs[args.program]

    mode = "dry-run" if args.dry_run else "osc-send"
    print(f"SDT-SL Conductor v0 mode={mode}")
    print(f"Target={args.host}:{args.port} Program={program.key} BPM={args.bpm} Hz={args.hz} Scale={args.scale}")
    print("Address mode:", args.address_mode, "namespace:", normalize_namespace(args.namespace) or "<none>")
    if args.address_mode == "numbered":
        print("Address prefix:", normalize_prefix(args.address_prefix))
    print("Monitor order: PRESSURE BREATH IDENTITY CONSTRAINT DRIFT RESOLUTION THRESHOLD PHRASE")
    print("Press Ctrl+C to stop.")

    start = time.perf_counter()
    interval = 1.0 / args.hz
    next_monitor_at = start

    try:
        while running:
            now = time.perf_counter()
            t = now - start
            state = program.fn(t, args.bpm, rng)
            values = scaled_values_by_lane(state, args.scale)

            if client is not None:
                send_state(client, state, args.scale, args)

            if now >= next_monitor_at:
                if args.json_monitor:
                    print(monitor_json(t, program.key, values))
                else:
                    print(monitor_line(values))
                next_monitor_at += 1.0

            time.sleep(interval)
    finally:
        if client is not None:
            send_state(client, SdtSlState(), args.scale, args)
            client.close()
        print("\nStopped. Sent zero state.")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ValueError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        raise SystemExit(2)
