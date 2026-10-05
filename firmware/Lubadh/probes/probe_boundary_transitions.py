#!/usr/bin/env python3
"""Execute one original callback head-boundary iteration and nested operations.

The callback's upstream LinkData/stack production and downstream render are not
executed here. This probe covers ordinary selected regions; wrapped-region and
physical-seam branches need their own integrated fixtures.
"""
import hashlib
import itertools
import json
import math

import unicorn
from unicorn.arm_const import UC_ARM_REG_SP
from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32
from probe_tap_allocation import call

CHANNEL, LINK = 0x100000, 0x140000
MANAGER = CHANNEL + 872
SLOT_SIZE = 132


def execute(c, return_cpu=False):
    cpu = ARMBytes()
    cpu.putu(CHANNEL + 0xE8, LINK)
    call(cpu, 0x4CA68, MANAGER)
    source = call(cpu, 0x4EEA8, MANAGER, c['previous'], c['fade'], c['effective_speed'] >= 0)
    assert source == MANAGER
    occupancy = c.get('occupied_slots', 5 if c['saturated'] else 1)
    if occupancy > 1:
        for slot in range(1, occupancy):
            assert call(cpu, 0x4E460, MANAGER, 10000 + slot, 0) == MANAGER + SLOT_SIZE * slot
    cpu.putf(source + 8, c['initial_fraction'])
    cpu.write(source + 20, bytes([c['held'] is not None]))
    cpu.putf(source + 24, c['held'] if c['held'] is not None else 1.)
    cpu.reg(0, source)
    cpu.fp(0, c['speed'])
    cpu.fp(1, c['factor'])
    cpu.fp(2, c['frames'])
    cpu.call(0x4BDE0)
    current = cpu.gets(source + 4)
    before = cpu.read(source + 4, 16)
    if c['already_fading']:
        cpu.write(source + 52, b'\x01')
    if c.get('physical_fading', False):
        cpu.write(source + 28, b'\x01')
    # Stack values are the documented processChannel locals from LinkData.
    for offset, value in ((0xC, c['start']), (0x1C, c['end']),
                          (0x28, c['reverse_start']), (0x14, c['reverse_end']),
                          (0x18, c['fade']), (0x34, c['replacement']),
                          (0x38, c.get('period', 4096)), (0x40, c.get('physical_reverse', 1)),
                          (0x48, c.get('physical_reverse_destination', 4354)),
                          (0x24, c.get('reverse_start_mod', c['reverse_start']))):
        cpu.puts(STACK + offset, value)
    cpu.putu(STACK + 0x78, STACK + 128)
    cpu.reg(4, CHANNEL)
    cpu.reg(6, source)
    cpu.reg(8, STACK + 128)
    cpu.reg(11, MANAGER)
    cpu.fp(19, c['speed'])
    cpu.fp(17, c['factor'])
    cpu.fp(16, c['frames'])
    fades, allocations = [], []

    def fade_observer(machine):
        sp = machine.cpu.reg_read(UC_ARM_REG_SP)
        fades.append([machine.reg(0) - MANAGER, machine.reg(1),
                      machine.reg(2) if machine.reg(2) < 2**31 else machine.reg(2) - 2**32,
                      machine.reg(3), machine.getu(sp), machine.getu(sp + 4)])

    def allocation_observer(machine):
        position = machine.reg(1)
        allocations.append([position if position < 2**31 else position - 2**32, machine.reg(2)])

    cpu.observers[0x4E46C] = fade_observer
    cpu.observers[0x4E460] = allocation_observer
    cpu.call(0x47F44, stop_before=0x47F10)
    assert cpu.read(source + 4, 16) == before, 'Boundary iteration repositioned source head'
    active = [slot for slot in range(5) if cpu.read(MANAGER + slot * SLOT_SIZE, 1)[0]]
    new = None
    if len(active) == 2:
        pointer = MANAGER + SLOT_SIZE
        new = dict(position=cpu.gets(pointer + 4), fraction=cpu.getf(pointer + 8),
                   previous=cpu.gets(pointer + 12), previous_fraction=cpu.getf(pointer + 16),
                   held=cpu.read(pointer + 20, 1)[0], held_speed=cpu.getf(pointer + 24),
                   fade_active=cpu.read(pointer + 52, 1)[0])
    result = dict(current=current, current_fraction=cpu.getf(source + 8),
                previous_fraction=cpu.getf(source + 16), source_fade_active=cpu.read(source + 52, 1)[0],
                active_slots=active, fades=fades, allocations=allocations, replacement=new)
    return result, cpu if return_cpu else cpu.coverage


def model(c):
    delta = f32(f32(c['effective_speed'] * c['factor']) * c['frames'])
    total = f32(c['initial_fraction'] + delta)
    whole = math.floor(total)
    current = c['previous'] + whole
    forward = c['effective_speed'] >= 0
    outside = (current < c['start'] or current > c['end']) if forward else (
        current < c['reverse_start'] or current >= c['reverse_end'])
    triggered = outside and not c['already_fading']
    fades, allocations, new = [], [], None
    if triggered:
        boundary = c['end'] if forward else c['reverse_start']
        fades.append([0, 1, boundary, c['fade'], 0, int(forward)])
        if c['replacement']:
            crossed = (c['previous'] <= boundary <= current) if forward else (
                current <= boundary <= c['previous'])
            distance = boundary - c['previous'] if crossed else 0
            target = c['start'] if forward else c['reverse_end']
            position = target - distance
            allocations.append([position, 0])
            if not c['saturated']:
                fades.append([SLOT_SIZE, 1, target, c['fade'], 1, int(forward)])
                new = dict(position=position + math.floor(delta) if crossed else position,
                           fraction=delta - math.floor(delta) if crossed else 0., previous=position,
                           previous_fraction=0., held=int(c['held'] is not None),
                           held_speed=c['held'] if c['held'] is not None else 1., fade_active=1)
    return dict(current=current, current_fraction=total-whole, previous_fraction=c['initial_fraction'],
                source_fade_active=int(triggered or c['already_fading']),
                active_slots=list(range(5)) if c['saturated'] else [0, 1] if new else [0],
                fades=fades, allocations=allocations, replacement=new)


def main():
    coverage, cases, comparisons = set(), [], 0
    for speed, held, factor, frames, initial_fraction, current_target, replacement, saturated, fading in itertools.product(
            (-4., -1., -.5, 0., .5, 1., 4.), (None, -1., 1.), (1., .75),
            (7., 32.), (0., .375),
            (99, 100, 101, 131, 132, 133, 499, 500, 501, 531, 532, 533),
            (False, True), (False, True), (False, True)):
        effective = speed if held is None else held
        delta = f32(f32(effective * factor) * frames)
        previous = current_target - math.floor(f32(initial_fraction + delta))
        config = dict(start=100, end=500, reverse_start=132, reverse_end=532, fade=32,
                      speed=speed, held=held, effective_speed=effective, factor=factor,
                      frames=frames, initial_fraction=initial_fraction, previous=previous, replacement=replacement,
                      saturated=saturated, already_fading=fading)
        expected = model(config)
        observed, addresses = execute(config)
        assert observed == expected, (config, expected, observed)
        comparisons += len(observed) + (len(observed['replacement']) if observed['replacement'] else 0)
        coverage.update(addresses)
        # Preserve every configuration and original call trace for review.
        cases.append(dict(config=config, output=observed))
    result = dict(status='PASS', unicorn_version=unicorn.__version__,
                  method='Original processChannel single-head boundary slice 0x47f44 to before 0x47f10',
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  cases=len(cases), comparisons=comparisons, configurations=cases,
                  distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(address) for address in sorted(coverage)],
                  limitations=[
                      'Ordinary regions only; wrapped-region and physical-seam branches remain open.',
                      'Upstream LinkData and stack locals are explicit fixtures; no whole callback execution.',
                      'Nested fade, allocation, transfer and movement bytes execute; no audio render yet.',
                      'Fractions 0 and 0.375 and block sizes 7/32 are covered; no general scheduler claim.',
                      'Existing fade active flag is an explicit fixture, not its complete history.',
                  ])
    (ROOT / 'probes/boundary_transition_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in (
        'status', 'cases', 'comparisons', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
