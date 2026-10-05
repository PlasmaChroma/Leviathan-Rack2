#!/usr/bin/env python3
"""Original wrapped-region/physical-seam callback slice versus explicit model."""
import hashlib
import itertools
import json
import math

import unicorn
from arm_byte_probe import ROOT
from arm_leaf_probe import f32
from probe_boundary_transitions import execute, MANAGER, SLOT_SIZE


def snapshot(observed, cpu, c):
    new_heads = []
    for slot in observed['active_slots']:
        if slot < c['occupied_slots']:
            continue
        p = MANAGER + slot * SLOT_SIZE
        new_heads.append(dict(slot=slot, position=cpu.gets(p + 4), fraction=cpu.getf(p + 8),
                              previous=cpu.gets(p + 12), previous_fraction=cpu.getf(p + 16),
                              held=cpu.read(p + 20, 1)[0], held_speed=cpu.getf(p + 24),
                              physical_fade=cpu.read(p + 28, 1)[0], loop_fade=cpu.read(p + 52, 1)[0]))
    return dict(current=observed['current'], fraction=observed['current_fraction'],
                previous_fraction=observed['previous_fraction'],
                source_physical_fade=cpu.read(MANAGER + 28, 1)[0],
                source_loop_fade=cpu.read(MANAGER + 52, 1)[0],
                active_slots=observed['active_slots'], fades=observed['fades'],
                allocations=observed['allocations'], new_heads=new_heads)


def model(c):
    delta = f32(f32(c['effective_speed'] * c['factor']) * c['frames'])
    total = f32(c['initial_fraction'] + delta)
    current = c['previous'] + math.floor(total)
    forward = c['effective_speed'] >= 0
    fades, allocations, new_heads = [], [], []
    flags = [int(c['physical_fading']), int(c['already_fading'])]
    next_slot = c['occupied_slots']

    def transition(kind, boundary, target, distance, advance, permission, duration):
        nonlocal next_slot
        fades.append([0, kind, boundary, duration, 0, int(forward)])
        flags[kind] = 1
        if not permission:
            return
        origin = target - distance
        allocations.append([origin, 0])
        if next_slot == 5:
            return
        incoming_flags = flags.copy()
        incoming_flags[kind] = 1
        fades.append([next_slot * SLOT_SIZE, kind, target, duration, 1, int(forward)])
        new_heads.append(dict(slot=next_slot, position=origin + math.floor(delta) if advance else origin,
                              fraction=delta-math.floor(delta) if advance else 0., previous=origin,
                              previous_fraction=0., held=int(c['held'] is not None),
                              held_speed=c['held'] if c['held'] is not None else 1., physical_fade=incoming_flags[0],
                              loop_fade=incoming_flags[1]))
        next_slot += 1

    if forward:
        if current >= c['period'] and not flags[0]:
            crossed = c['previous'] <= c['period']
            transition(0, c['period'], 1, c['period']-c['previous'] if crossed else 0,
                       crossed, True, 2458)
        if c['end'] <= current < c['start'] and not flags[1]:
            crossed = c['previous'] <= c['end']
            transition(1, c['end'], c['start'], c['end']-c['previous'] if crossed else 0,
                       crossed, c['replacement'], c['fade'])
    else:
        if current < c['physical_reverse'] and not flags[0]:
            crossed = c['previous'] >= c['physical_reverse']
            transition(0, c['physical_reverse'], c['physical_reverse_destination'],
                       c['physical_reverse']-c['previous'] if crossed else 0,
                       crossed, True, 2458)
        boundary = c['reverse_start_mod']
        if boundary == c['reverse_start']:
            outside = c['reverse_end'] <= current < boundary
            crossed = c['previous'] >= boundary
        else:
            outside = current < boundary or c['reverse_end'] <= current < c['start']
            crossed = current <= boundary <= c['previous']
        if outside and not flags[1]:
            transition(1, boundary, c['reverse_end'], boundary-c['previous'] if crossed else 0,
                       crossed, c['replacement'], c['fade'])
    return dict(current=current, fraction=total-math.floor(total), previous_fraction=c['initial_fraction'],
                source_physical_fade=flags[0], source_loop_fade=flags[1], active_slots=list(range(next_slot)),
                fades=fades, allocations=allocations, new_heads=new_heads)


def main():
    coverage, comparisons, examples = set(), 0, {}
    regions = [dict(start=700, end=300, reverse_start=732, reverse_end=332, reverse_start_mod=732),
               dict(start=1000, end=300, reverse_start=1032, reverse_end=332, reverse_start_mod=8),
               dict(start=300, end=300, reverse_start=332, reverse_end=332, reverse_start_mod=332)]
    targets = (0, 1, 31, 32, 33, 299, 300, 301, 331, 332, 333, 699, 700, 701, 731, 732, 733,
               999, 1000, 1001, 1023, 1024, 1025, 1031, 1032, 1033)
    for region, speed, held, fraction, current_target, permission, occupancy, loop_fading, physical_fading in itertools.product(
            regions, (-1., 0., 1.), (None, -1., 1.), (0., .375), targets,
            (False, True), (1, 4, 5), (False, True), (False, True)):
        effective = speed if held is None else held
        delta = f32(effective * .75 * 7.)
        c = dict(region, period=1024, physical_reverse=32, physical_reverse_destination=1282,
                 fade=32, speed=speed, held=held, effective_speed=effective, factor=.75, frames=7.,
                 initial_fraction=fraction, previous=current_target-math.floor(f32(fraction+delta)),
                 replacement=permission, occupied_slots=occupancy, saturated=occupancy == 5,
                 already_fading=loop_fading, physical_fading=physical_fading)
        expected = model(c)
        observed, cpu = execute(c, return_cpu=True)
        observed = snapshot(observed, cpu, c)
        assert observed == expected, (c, expected, observed)
        comparisons += len(observed) + sum(len(h) for h in observed['new_heads'])
        coverage.update(cpu.coverage)
        # Full generation is reproducible; store compact examples and traces for
        # each distinct branch signature rather than duplicating every fixture.
        signature = (region['start'], region['end'], effective >= 0, occupancy, permission,
                     loop_fading, physical_fading,
                     tuple((f[0], f[1]) for f in observed['fades']), len(observed['allocations']))
        examples.setdefault(signature, dict(config=c, output=observed))
    total_cases = len(regions)*3*3*2*len(targets)*2*3*2*2
    result = dict(status='PASS', unicorn_version=unicorn.__version__, cases=total_cases, comparisons=comparisons,
                  method='Original wrapped-region boundary iteration with nested fade/allocation/transfer/move execution',
                  main_sha256=hashlib.sha256((ROOT/'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  examples=list(examples.values()), distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=[
                      'LinkData-derived locals are independent fixtures; upstream publication is not executed.',
                      'Physical reverse threshold/destination are fixtures, not established factory coordinates.',
                      'Fade phase/sample rendering and whole callback scheduling remain open.',
                      'Existing fade flags are injected; their full histories are not reproduced.',
                      'One old-head iteration, not iteration over every newly allocated head in the callback.',
                  ])
    (ROOT/'probes/wrapped_boundary_probe_results.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({key:result[key] for key in ('status','cases','comparisons','distinct_instruction_addresses')},indent=2))


if __name__ == '__main__':
    main()
