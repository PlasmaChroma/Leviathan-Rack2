#!/usr/bin/env python3
"""Persistent boundary -> render/filter/gain and subsequent original update."""
import hashlib
import itertools
import json
import math
import struct

from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_antialias import coefficient, cutoff_for_speed, reference
from probe_engine_gain import BASE
from probe_playback_pipeline import (AA, GAIN, MIX, MIX_HEADER, active, raw_model)
from probe_splice_render import (CHANNEL, MANAGER, WORK, RECORD_MANAGER, TAPE,
                                 OUTPUT, GAINS, TABLE)
from probe_tap_allocation import call


def main():
    tape = [f32(.3 * math.sin(i * .137) + .1 * math.cos(i * .071)) for i in range(8192)]
    regions = {
        'ordinary_forward': (1, 100, 500, 132, 532, 49170, 2459, 51628, 132, 501),
        'ordinary_reverse': (-1, 100, 500, 132, 532, 49170, 2459, 51628, 132, 131),
        'physical_forward': (1, 700, 300, 732, 332, 1024, 32, 1282, 732, 1025),
        'physical_reverse': (-1, 700, 300, 732, 332, 1024, 32, 1282, 732, 31),
        'reverse_double': (-1, 1000, 300, 1032, 332, 1024, 32, 1282, 8, 1),
    }
    rows, coverage, comparisons, snapshot_assertions = [], set(), 0, 0
    errors = dict(raw=0., filtered=0., mix=0., filter_state=0.)
    for family, frames, engines, occupancy, permission in itertools.product(
            regions, (7, 128), (1, 4), (1, 5), (False, True)):
        speed, start, end, ra, rb, period, pr, destination, reduced, target_position = regions[family]
        factor, fraction = .75, .375
        previous = target_position - math.floor(f32(fraction + f32(speed * factor * frames)))
        cpu = ARMBytes()
        call(cpu, 0x4CA68, MANAGER)
        call(cpu, 0x4CA68, RECORD_MANAGER)
        cpu.reg(4, CHANNEL)
        cpu.call(0x3CF0C, stop_before=0x3CF34)
        call(cpu, 0x4B09C, AA)
        cutoff = cutoff_for_speed(speed)
        cpu.reg(0, AA)
        cpu.fp(0, cutoff)
        cpu.call(0x4B1C0)
        coeff = coefficient(cutoff)
        cpu.putu(CHANNEL + 171972, TAPE)
        cpu.write(TAPE, struct.pack('<8192f', *tape))
        cpu.write(0x93F98, struct.pack('<256f', *TABLE))
        for engine in range(engines):
            head = call(cpu, 0x4EEA8, MANAGER, previous, 32, speed > 0)
            cpu.putf(head + 8, fraction)
            for slot in range(1, occupancy):
                head = call(cpu, 0x4E460, MANAGER, previous, engine)
                cpu.putf(head + 8, fraction)
        state = [[0., 0.], [0., 0.]]
        gain, increment, remaining, cached = 0., 0., 0, 0
        history = []
        for block in range(12):
            heads = active(cpu, MANAGER)
            for head in heads:
                cpu.reg(0, head)
                cpu.fp(0, speed)
                cpu.fp(1, factor)
                cpu.fp(2, frames)
                cpu.call(0x4BDE0)
            positions_before = {head: cpu.read(head + 4, 16) for head in heads}
            prior = [f32(.1 * math.cos(i * .071 - block)) for i in range(frames)]
            cpu.vector(WORK + 12, OUTPUT, [0.] * frames)
            cpu.vector(WORK + 24, GAINS, [1.] * frames)
            cpu.vector(MIX_HEADER, MIX, prior)
            for offset, value in ((0x0C, start), (0x1C, end), (0x28, ra), (0x14, rb),
                                  (0x18, 32), (0x34, int(permission)), (0x38, period),
                                  (0x40, pr), (0x48, destination), (0x24, reduced),
                                  (0x20, MANAGER), (0x30, RECORD_MANAGER), (0x54, AA),
                                  (0x5C, WORK + 12), (0x2C, frames), (0x08, 0), (0x4C, 0)):
                cpu.putu(STACK + offset, value)
            cpu.reg(0, MANAGER)
            cpu.reg(4, CHANNEL)
            cpu.reg(5, WORK)
            cpu.reg(8, STACK + 128)
            cpu.reg(10, MIX_HEADER)
            cpu.reg(11, MANAGER)
            for register, value in ((16, frames), (17, factor), (18, factor),
                                     (19, speed), (20, speed), (21, abs(speed)), (22, abs(speed))):
                cpu.fp(register, value)
            checked, rendered, attempts, model, observed = [], [], [], {}, {}
            c = dict(frames=frames, speed=speed, previous_speed=speed,
                     factor=factor, previous_factor=factor)

            def render_entry(machine):
                nonlocal gain, increment, remaining, cached
                scratch = MANAGER + 2736
                render_heads = [machine.getu(scratch + i * 4) for i in range(20)
                                if machine.getu(scratch + i * 4)]
                model['heads'] = render_heads
                model['raw'] = raw_model(machine, render_heads, [], c, tape)
                model['filtered'] = reference(model['raw'], coeff, state)
                count = sum(bool(machine.getu(MANAGER + 2720 + i * 4)) for i in range(4))
                target = 1.
                for _ in range(max(0, count - 1)):
                    target = f32(target * BASE)
                if count != cached:
                    increment, remaining, cached = f32(f32(target - gain) * .125), 8, count
                if remaining:
                    gain, remaining = f32(gain + increment), remaining - 1
                model['mix'] = [libm.fmaf(a, gain, b) for a, b in zip(model['filtered'], prior)]

            cpu.observers[0x47F44] = lambda machine: checked.append(machine.reg(6))
            cpu.observers[0x4E460] = lambda machine: attempts.append(machine.reg(2))
            cpu.observers[0x48010] = render_entry
            cpu.observers[0x48048] = lambda machine: rendered.append(machine.reg(6))
            cpu.observers[0x483A4] = lambda machine: observed.update(raw=machine.floats(OUTPUT, frames))
            cpu.observers[0x483B0] = lambda machine: observed.update(filtered=machine.floats(OUTPUT, frames))
            cpu.call(0x47ED4, stop_before=0x48454)
            assert checked == heads
            assert rendered == model['heads']
            assert all(cpu.read(head + 4, 16) == value for head, value in positions_before.items())
            snapshot_assertions += 3
            if block == 0:
                attempts_per_head = (int(permission) if family.startswith('ordinary') else
                                     1 + int(permission) if family == 'reverse_double' else 1)
                assert len(attempts) == engines * occupancy * attempts_per_head
                assert len(rendered) == engines * min(5, occupancy * (1 + attempts_per_head))
                snapshot_assertions += 2
            observed['mix'] = cpu.floats(MIX, frames)
            observed['filter_state'] = [cpu.getf(AA + o) for o in (4, 8, 16, 20)]
            model['filter_state'] = state[0] + state[1]
            for field in errors:
                for want, actual in zip(model[field], observed[field]):
                    error = abs(want - actual)
                    assert math.isfinite(actual) and error <= 8e-7, (family, frames, engines, occupancy, permission, block, field, want, actual, error)
                    errors[field] = max(errors[field], error)
                    comparisons += 1
            assert [cpu.getf(GAIN), cpu.getf(GAIN + 4), cpu.gets(GAIN + 8), cpu.gets(GAIN + 12)] == [gain, increment, remaining, cached]
            comparisons += 4
            # Execute original end-of-callback manager updates, skipping the
            # intervening uninitialized coloration and recording chain.
            cpu.reg(4, CHANNEL)
            cpu.call(0x48544, stop_before=0x48554)
            survivors = active(cpu, MANAGER)
            history.append(dict(block=block, before=[h - MANAGER for h in heads],
                                allocation_engine_attempts=attempts,
                                rendered=[h - MANAGER for h in rendered],
                                survivors=[h - MANAGER for h in survivors], gain=gain,
                                output=observed['mix'], model_output=model['mix']))
        rows.append(dict(region=family, frames=frames, engines=engines, occupancy=occupancy,
                         permission=permission, blocks=history))
        coverage.update(cpu.coverage)
    blocks = [b for r in rows for b in r['blocks']]
    events = dict(allocation_attempts=sum(len(b['allocation_engine_attempts']) for b in blocks),
                  blocks_with_new_heads=sum(bool(set(b['rendered']) - set(b['before'])) for b in blocks),
                  blocks_with_reclamation=sum(bool(set(b['rendered']) - set(b['survivors'])) for b in blocks),
                  blocks_empty_after_update=sum(not b['survivors'] for b in blocks))
    result = dict(status='PASS', sequences=len(rows), blocks=len(blocks), events=events,
                  comparisons=comparisons, snapshot_assertions=snapshot_assertions,
                  max_abs_errors=errors,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  fixtures=rows, distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['One playback boundary manager with an empty recording manager.',
                               'Upstream boundary locals, initial coordinates and speed are fixtures.',
                               'Render model reads original post-boundary head/fade state; no independent full trigger model.',
                               'Motion runs as explicit original calls before the continuous boundary/render slice.',
                               'Original final update slice executes separately after skipping coloration and tape writes.',
                               'Reduced physical coordinates are branch fixtures, not a normal recorded loop.',
                               'No full callback, output coloration, tape writes, hardware or concurrency comparison.'])
    (ROOT / 'probes/boundary_pipeline_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'sequences', 'blocks', 'comparisons',
                                            'snapshot_assertions', 'max_abs_errors', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
