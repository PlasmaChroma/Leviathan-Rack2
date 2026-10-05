#!/usr/bin/env python3
"""Original-byte engine/slot allocation, saturation and fade-reclamation probes."""
import hashlib
import json

import unicorn
from arm_byte_probe import ARMBytes, ROOT

MANAGER = 0x100000
ENGINE_SIZE, SLOT_SIZE = 680, 132


def call(cpu, address, *args):
    for register, value in enumerate(args):
        cpu.reg(register, value)
    cpu.call(address)
    return cpu.reg(0)


def slot_address(engine, slot):
    return MANAGER + engine * ENGINE_SIZE + slot * SLOT_SIZE


def main():
    comparisons, coverage, cases = 0, set(), []

    def check(expected, observed, label):
        nonlocal comparisons
        assert observed == expected, (label, expected, observed)
        comparisons += 1

    # Direct activation returns null on the sixth allocation in one engine;
    # previously active slots and the engine's last-activated pointer survive.
    for engine in range(4):
        cpu = ARMBytes()
        call(cpu, 0x4CA68, MANAGER)
        for slot in range(6):
            observed = call(cpu, 0x4E460, MANAGER, 1000 + slot, engine)
            check(slot_address(engine, slot) if slot < 5 else 0, observed, 'direct allocation')
            check(slot_address(engine, min(slot, 4)),
                  cpu.getu(MANAGER + engine * ENGINE_SIZE + 660), 'engine newest pointer')
        coverage.update(cpu.coverage)

    # With no motion/update between triggers, logical engines cycle by age and
    # transition slots accumulate. Saturation does not steal/reset an active slot.
    for direction in (False, True):
        for duration in (1, 128, 12292):
            cpu = ARMBytes()
            call(cpu, 0x4CA68, MANAGER)
            order, slots, positions, latest = [], [[] for _ in range(4)], {}, {}
            trace = []
            for trigger in range(32):
                empty = next((engine for engine in range(4) if not slots[engine]), None)
                engine = empty if empty is not None else order[-1]
                if engine in order:
                    order.remove(engine)
                order.insert(0, engine)
                allocated = len(slots[engine]) < 5
                position = trigger * 100 - 700
                if allocated:
                    slot = len(slots[engine])
                    slots[engine].append(slot)
                    latest[engine] = slot
                    positions[engine, slot] = position
                slot = latest[engine]
                pointer = call(cpu, 0x4EEA8, MANAGER, position, duration, direction)
                check(slot_address(engine, slot), pointer, 'manager result')
                check(positions[engine, slot], cpu.gets(pointer + 4), 'new or retained position')
                check(engine, call(cpu, 0x4DDF8, MANAGER), 'newest engine ID')
                check(order[-1], call(cpu, 0x4DE2C, MANAGER), 'oldest engine ID')
                check(len(order), call(cpu, 0x4DDD0, MANAGER), 'logical engine count')
                check(sum(map(len, slots)), call(cpu, 0x4DCF8, MANAGER), 'active slot count')
                check(int(len(order) < 4), call(cpu, 0x4DC80, MANAGER), 'logical engine availability')
                expected_order = [MANAGER + e * ENGINE_SIZE for e in order] + [0] * (4 - len(order))
                check(expected_order, [cpu.getu(MANAGER + 2720 + i * 4) for i in range(4)], 'engine age order')
                for e in range(4):
                    for s in range(5):
                        check(int(s in slots[e]), cpu.read(slot_address(e, s), 1)[0], 'slot active flag')
                trace.append(dict(trigger=trigger, engine=engine, slot=slot, allocated=allocated,
                                  requested_position=position, actual_position=cpu.gets(pointer + 4),
                                  active_slots=sum(map(len, slots))))
            cases.append(dict(direction=direction, fade_duration=duration, triggers=trace))
            coverage.update(cpu.coverage)

    # Advancing heads beyond 128-sample kill fades and executing the complete
    # manager update reclaims them. At 20 triggers each engine retains its newest
    # unfaded head; at 25 all four also received kill fades during saturation.
    reclamation = []
    for trigger_count in (20, 25):
        for speed, held, direction in ((-1., None, False), (1., None, True),
                                       (0., None, True), (0., 1., True), (0., -1., False)):
            cpu = ARMBytes()
            call(cpu, 0x4CA68, MANAGER)
            for trigger in range(trigger_count):
                call(cpu, 0x4EEA8, MANAGER, trigger * 100, 128, direction)
            for engine in range(4):
                for slot in range(5):
                    pointer = slot_address(engine, slot)
                    if held is not None:
                        cpu.write(pointer + 20, b'\x01')
                        cpu.putf(pointer + 24, held)
                    cpu.reg(0, pointer)
                    cpu.fp(0, speed)
                    cpu.fp(1, 1.)
                    cpu.fp(2, 256.)
                    cpu.call(0x4BDE0)
            call(cpu, 0x4D138, MANAGER)
            stalled = speed == 0 and held is None
            expected_count = 20 if stalled else 4 if trigger_count == 20 else 0
            check(expected_count, call(cpu, 0x4DCF8, MANAGER), 'reclaimed slot count')
            check(4 if expected_count else 0, call(cpu, 0x4DDD0, MANAGER), 'reclaimed engine count')
            active = [[e, s] for e in range(4) for s in range(5)
                      if cpu.read(slot_address(e, s), 1)[0]]
            expected_active = ([[e, s] for e in range(4) for s in range(5)] if stalled else
                               [[e, 4] for e in range(4)] if trigger_count == 20 else [])
            check(expected_active, active, 'reclamation survivors')
            reclamation.append(dict(triggers=trigger_count, speed=speed, held_speed=held, active_slots=active))
            coverage.update(cpu.coverage)

    result = dict(status='PASS', unicorn_version=unicorn.__version__,
                  method='Original TapManager constructor, activation, queries, motion and update bytes',
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  comparisons=comparisons, saturation_cases=cases, reclamation_cases=reclamation,
                  distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(address) for address in sorted(coverage)],
                  limitations=[
                      'No full audio/event callback; trigger burst intentionally omits intervening motion/update.',
                      'Saturation return semantics do not establish caller behavior after that return.',
                      'Held/stall reclamation uses explicit speed fixtures; control production and resync events remain open.',
                      'No complete loop splice/audio render or mixing gain validation.',
                  ])
    (ROOT / 'probes/tap_allocation_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in (
        'status', 'comparisons', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
