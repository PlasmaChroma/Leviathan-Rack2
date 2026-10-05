#!/usr/bin/env python3
"""Original callback boundary iteration and render-list refresh contracts."""
import hashlib
import itertools
import json

from arm_byte_probe import ARMBytes, ROOT, STACK
from probe_tap_allocation import call

CHANNEL = 0x100000
MANAGER = CHANNEL + 872
ENGINE_SIZE, SLOT_SIZE = 680, 132


def pointer(engine, slot):
    return MANAGER + ENGINE_SIZE * engine + SLOT_SIZE * slot


def list_values(cpu, address):
    return [cpu.getu(address + i * 4) for i in range(20)]


def main():
    rows, coverage, assertions = [], set(), 0
    patterns = [tuple([slots] * count) for count in range(1, 5) for slots in range(1, 6)]
    patterns += [(1, 4, 2, 5), (5, 1, 4, 2), (2, 3, 4, 1), (4, 2, 1, 3)]
    configurations = [(o, d, p, 'ordinary') for o, d, p in
                      itertools.product(patterns, (-1, 1), (False, True))]
    configurations += [(o, -1, p, 'reverse_double_seam') for o, p in
                       itertools.product(patterns, (False, True))]
    for occupancy, direction, permission, region in configurations:
        cpu = ARMBytes()
        call(cpu, 0x4CA68, MANAGER)
        previous = (499 if direction > 0 else 133) if region == 'ordinary' else 6
        for engine, count in enumerate(occupancy):
            assert call(cpu, 0x4EEA8, MANAGER, previous, 32, direction > 0) == pointer(engine, 0)
            for slot in range(1, count):
                assert call(cpu, 0x4E460, MANAGER, previous, engine) == pointer(engine, slot)
        original = [pointer(e, s) for e in reversed(range(len(occupancy)))
                    for s in reversed(range(occupancy[e]))]
        ages_before = cpu.read(MANAGER + 2720, 16)
        slot_ages_before = [cpu.read(MANAGER + e * ENGINE_SIZE + 660, 20)
                            for e in range(4)]
        scratch = call(cpu, 0x4D984, MANAGER)
        assert scratch == MANAGER + 2736
        assert list_values(cpu, scratch) == original + [0] * (20 - len(original))
        assert cpu.read(MANAGER + 2720, 16) == ages_before
        assert [cpu.read(MANAGER + e * ENGINE_SIZE + 660, 20) for e in range(4)] == slot_ages_before
        snapshot = cpu.read(scratch, 80)
        assertions += 4
        for head in original:
            cpu.reg(0, head)
            cpu.fp(0, direction)
            cpu.fp(1, .75)
            cpu.fp(2, 7.)
            cpu.call(0x4BDE0)
        for offset, value in ((0x0C, 100), (0x1C, 500), (0x28, 132), (0x14, 532),
                              (0x18, 32), (0x34, int(permission)), (0x38, 49170),
                              (0x40, 2459), (0x48, 51628), (0x24, 132)):
            cpu.puts(STACK + offset, value)
        if region == 'reverse_double_seam':
            for offset, value in ((0x0C, 1000), (0x1C, 300), (0x28, 1032),
                                  (0x14, 332), (0x38, 1024), (0x40, 32),
                                  (0x48, 1282), (0x24, 8)):
                cpu.puts(STACK + offset, value)
        cpu.reg(0, MANAGER)
        cpu.reg(4, CHANNEL)
        cpu.reg(8, STACK + 128)
        cpu.reg(11, MANAGER)
        cpu.fp(19, direction)
        cpu.fp(17, .75)
        cpu.fp(16, 7.)
        checked, allocations = [], []
        cpu.observers[0x47F44] = lambda machine: checked.append(machine.reg(6))
        cpu.observers[0x4E460] = lambda machine: allocations.append(machine.reg(2))
        cpu.call(0x47ED4, stop_before=0x47FD4)
        attempts_per_head = int(permission) if region == 'ordinary' else 1 + int(permission)
        expected_attempts = [e for e in reversed(range(len(occupancy)))
                             for _ in range(occupancy[e] * attempts_per_head)]
        assert checked == original
        assert allocations == expected_attempts
        assert cpu.read(scratch, 80) == snapshot
        assertions += 3
        counts = [min(5, (1 + attempts_per_head) * n) for n in occupancy]
        expected_new = [pointer(e, s) for e in range(len(occupancy))
                        for s in range(occupancy[e], counts[e])]
        active = [pointer(e, s) for e in range(4) for s in range(5)
                  if cpu.read(pointer(e, s), 1)[0]]
        assert set(active) == set(original + expected_new)
        # The callback's render phase refreshes the same scratch list.
        cpu.putu(STACK + 0x20, MANAGER)
        cpu.call(0x47FE4, stop_before=0x47FF4)
        refreshed = list_values(cpu, scratch)
        expected_order = [pointer(e, s) for e in reversed(range(len(occupancy)))
                          for s in reversed(range(counts[e]))]
        assert cpu.reg(0) == scratch
        assert refreshed == expected_order + [0] * (20 - len(expected_order))
        assert set(expected_new).isdisjoint(checked)
        assertions += 4
        rows.append(dict(occupancy=list(occupancy), direction=direction, permission=permission, region=region,
                         boundary_checked=[p - MANAGER for p in checked],
                         allocation_engine_attempts=allocations,
                         new_heads=[p - MANAGER for p in expected_new],
                         refreshed_render_list=[p - MANAGER for p in refreshed if p]))
        coverage.update(cpu.coverage)
    result = dict(status='PASS', cases=len(rows), assertions=assertions,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  fixtures=rows, distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Ordinary regions and one reduced reverse double-seam layout; not all wrapped cases.',
                               'Movement runs before the boundary loop as an explicit setup sequence.',
                               'Render-entry list refresh is executed, not downstream audio rendering.',
                               'No simultaneous recording manager, deactivation/update or full callback.',
                               'Only contiguous allocated slots and original allocation-produced age order.'])
    (ROOT / 'probes/head_iteration_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'cases', 'assertions',
                                            'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
