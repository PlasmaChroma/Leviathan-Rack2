#!/usr/bin/env python3
"""Seeded plate state, phase/ring wraps and original live reverb control updates."""
import hashlib
import itertools
import json
import math
import random
import struct

from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from probe_plate_process import initialized, parameters, SAMPLE
from probe_plate_initialization import CHANNEL, PLATE
from probe_plate_controls import LINK, PRESET, MIRROR
from plate_model import PlateModel, DESCRIPTORS, FILTERS, MODULATORS, DRY, WET, DECAY, FEEDBACK


def main():
    rows, coverage, comparisons, error_max, phase_wraps, index_wraps = [], set(), 0, 0., 0, 0
    for mode_base, phase_seed, depth_stress in itertools.product(range(4), (.249, .749, .999), (False, True)):
        cpu = initialized()
        params, modes, tables = parameters(cpu)
        modes = {o: (mode_base + i) % 4 for i, o in enumerate(FILTERS)}
        model = PlateModel(params, modes, tables)
        rng = random.Random(210 + mode_base)
        locations = {o: cpu.getu(cpu.getu(PLATE + o)) for o in DESCRIPTORS}
        for i, o in enumerate(DESCRIPTORS):
            seed = [f32(rng.uniform(-.025, .025)) for _ in model.buffers[o]]
            model.buffers[o] = seed
            model.indices[o] = len(seed) - 2
            cpu.write(locations[o], struct.pack('<' + 'f' * len(seed), *seed))
            cpu.putu(PLATE + o + 4, model.indices[o])
        for i, o in enumerate(FILTERS):
            model.modes[o] = modes[o]
            model.filter_state[o] = [f32(.013 + .001 * i), f32(-.005 + .002 * i)]
            cpu.putu(PLATE + o, modes[o])
            for j, value in enumerate(model.filter_state[o]):
                cpu.putf(PLATE + o + 8 + 4 * j, value)
        for i, o in enumerate(MODULATORS):
            model.phase[o] = f32(phase_seed)
            model.params[o + 24] = f32(.03125 if i == 0 else .125)
            if depth_stress:
                model.params[o + 16] = f32(700. if i == 0 else 500.)
            cpu.putf(PLATE + o + 28, model.phase[o])
            cpu.putf(PLATE + o + 24, model.params[o + 24])
            cpu.putf(PLATE + o + 16, model.params[o + 16])
        cpu.putu(CHANNEL + 0xE8, LINK)
        cpu.putu(LINK + 0xB0, PRESET)
        cpu.putu(CHANNEL + 171868, MIRROR)
        output, trajectory = [], []
        for n in range(256):
            if n % 32 == 0:
                amount = f32((0., .25, .45, .5, 1., .75, .125, 0.)[n // 32])
                cpu.putf(LINK + 168, amount)
                cpu.putf(PRESET + 16384 + 664, 1.)
                cpu.reg(4, CHANNEL)
                cpu.call(0x37CE8, stop_before=0x37D54)
                complement = f32(1. - amount)
                norm = f32(math.sqrt(libm.fmaf(amount, amount, f32(complement * complement))))
                model.params.update({DRY: f32(complement / norm), WET: f32(amount / norm), DECAY: min(f32(2. * amount), f32(.9))})
                trajectory.append(dict(sample=n, amount=amount, dry=model.params[DRY], wet=model.params[WET], decay=model.params[DECAY]))
            sample = f32(rng.uniform(-.4, .4))
            previous_phases, previous_indices = dict(model.phase), dict(model.indices)
            expected = model.process(sample)
            phase_wraps += sum(model.phase[o] < previous_phases[o] for o in MODULATORS)
            index_wraps += sum(model.indices[o] < previous_indices[o] for o in DESCRIPTORS)
            cpu.putf(SAMPLE, sample)
            cpu.reg(0, PLATE)
            cpu.reg(1, SAMPLE)
            cpu.call(0x496C0)
            actual = cpu.getf(SAMPLE)
            checks = [('output', expected, actual)]
            checks += [(f'parameter:{o:#x}', model.params[o], cpu.getf(PLATE + o)) for o in (DRY, WET, DECAY)]
            checks += [(f'filter:{o:#x}:{j}', value, cpu.getf(PLATE + o + 8 + 4 * j)) for o in FILTERS for j, value in enumerate(model.filter_state[o])]
            checks += [(f'phase:{o:#x}', model.phase[o], cpu.getf(PLATE + o + 28)) for o in MODULATORS]
            checks += [(f'feedback:{j}', value, cpu.getf(PLATE + o)) for j, (o, value) in enumerate(zip(FEEDBACK, model.feedback))]
            for o in DESCRIPTORS:
                assert model.indices[o] == cpu.getu(PLATE + o + 4)
                position = (model.indices[o] - 1) % len(model.buffers[o])
                checks.append((f'buffer:{o:#x}', model.buffers[o][position], cpu.getf(locations[o] + 4 * position)))
            for field, want, observed in checks:
                error = abs(want - observed)
                assert math.isfinite(observed) and error <= 5e-7, (mode_base, phase_seed, depth_stress, n, field, want, observed, error)
                error_max = max(error_max, error)
                comparisons += 1
            output.append(actual)
        assert cpu.floor_calls == 512
        coverage.update(cpu.coverage)
        rows.append(dict(filter_modes={hex(o): m for o, m in modes.items()}, phase_seed=phase_seed,
                         depth_stress=depth_stress, control_trajectory=trajectory, output=output))
    result = dict(status='PASS', sequences=len(rows), samples=256 * len(rows), comparisons=comparisons,
                  max_abs_error=error_max, phase_wraps=phase_wraps, ring_index_wraps=index_wraps,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(coverage), coverage_addresses=[hex(a) for a in sorted(coverage)], fixtures=rows,
                  limitations=['Seeded internal states, all four filter modes and accelerated modulation rates are branch fixtures, not normal production initial states.',
                               'Depth stress exceeds delay capacity to exercise the original single-subtraction branch; not a recovered user control.',
                               'Original Time reverb control sub-slice only; no complete event producer or linked-deck comparison.',
                               'No complete output coloration chain, hardware timing or analog comparison.'])
    (ROOT / 'probes/plate_state_transition_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'sequences', 'samples', 'comparisons', 'max_abs_error', 'phase_wraps', 'ring_index_wraps', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
