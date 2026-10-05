#!/usr/bin/env python3
"""Complete original MonoPlate recurrence versus an independent state model."""
import argparse
import hashlib
import json
import math
import random
import struct

from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR
from arm_byte_probe import ARMBytes, ROOT, _PLT_MAP
from arm_leaf_probe import f32
from probe_plate_initialization import CHANNEL, PLATE
from plate_model import PlateModel, DESCRIPTORS, FILTERS, MODULATORS, DRY, WET, DECAY, FEEDBACK, modulation_table

SAMPLE = 0x200000
if _PLT_MAP.get('0x163a4') != 'floorf':
    raise RuntimeError('Plate math import does not match archived evidence')


class PlateBytes(ARMBytes):
    """Only this probe opts into an explicit finite float32 floor service."""
    def __init__(self):
        self.floor_calls = 0
        super().__init__()

    def _code(self, cpu, address, size, user):
        if address == 0x163A4:
            value = self.fp(0)
            if not math.isfinite(value):
                raise RuntimeError('Nonfinite phase at explicit floorf service')
            self.fp(0, math.floor(value))
            self.floor_calls += 1
            self.coverage.add(address)
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, user)


def initialized():
    cpu = PlateBytes()
    cpu.reg(4, CHANNEL)
    cpu.call(0x3CFF0, stop_before=0x3D618)
    return cpu


def parameters(cpu):
    floats = [o + 4 for o in FILTERS] + [o + 12 for o in DESCRIPTORS[1:5]]
    floats += [0xCD40, 0x1DD34, DRY, WET, DECAY]
    floats += [o + shift for o in MODULATORS for shift in (12, 16, 24)]
    return ({o: cpu.getf(PLATE + o) for o in floats},
            {o: cpu.getu(PLATE + o) for o in FILTERS},
            {o: modulation_table() for o in MODULATORS})


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--samples', type=int, default=16384)
    parser.add_argument('--quick', action='store_true')
    args = parser.parse_args()
    profiles = ['default'] if args.quick else ['default', 'wet', 'mixed', 'allpass_filters', 'bypassed_filters']
    results, coverage, comparisons, error_max = [], set(), 0, 0.
    for profile in profiles:
        cpu = initialized()
        params, modes, tables = parameters(cpu)
        if profile != 'default':
            params.update({DRY: 0. if profile == 'wet' else .5, WET: 1. if profile == 'wet' else .5, DECAY: .7})
        if profile == 'allpass_filters':
            modes = {o: 2 for o in FILTERS}
        if profile == 'bypassed_filters':
            modes = {o: 3 for o in FILTERS}
        for o, value in params.items():
            cpu.putf(PLATE + o, value)
            params[o] = f32(value)
        for o, mode in modes.items():
            cpu.putu(PLATE + o, mode)
        model = PlateModel(params, modes, tables)
        data_locations = {o: cpu.getu(cpu.getu(PLATE + o)) for o in DESCRIPTORS}
        rng = random.Random(210)
        output, expected_output, peak = [], [], 0.
        for sample_index in range(args.samples):
            sample = f32(.4 if sample_index == 0 else -.2 if sample_index == 997 else rng.uniform(-.1, .1) if sample_index < 128 else 0.)
            cpu.putf(SAMPLE, sample)
            cpu.reg(0, PLATE)
            cpu.reg(1, SAMPLE)
            cpu.call(0x496C0)
            expected = model.process(sample)
            actual = cpu.getf(SAMPLE)
            checks = [('output', expected, actual)]
            for o in FILTERS:
                checks += [(f'filter:{o:#x}:{i}', v, cpu.getf(PLATE + o + 8 + 4 * i)) for i, v in enumerate(model.filter_state[o])]
            for o in MODULATORS:
                checks.append((f'phase:{o:#x}', model.phase[o], cpu.getf(PLATE + o + 28)))
            checks += [(f'feedback:{i}', v, cpu.getf(PLATE + o)) for i, (o, v) in enumerate(zip(FEEDBACK, model.feedback))]
            for o in DESCRIPTORS:
                assert model.indices[o] == cpu.getu(PLATE + o + 4), (profile, sample_index, 'index', hex(o))
                written_index = (model.indices[o] - 1) % len(model.buffers[o])
                checks.append((f'buffer:{o:#x}', model.buffers[o][written_index], cpu.getf(data_locations[o] + 4 * written_index)))
            for field, want, observed in checks:
                error = abs(want - observed)
                assert math.isfinite(observed) and error <= 5e-7, (profile, sample_index, field, want, observed, error)
                comparisons += 1
                error_max = max(error_max, error)
            output.append(actual)
            expected_output.append(expected)
            peak = max(peak, abs(actual))
        assert cpu.floor_calls == 2 * args.samples
        coverage.update(cpu.coverage)
        results.append(dict(profile=profile, samples=args.samples, peak=peak,
                            first_nonzero=next((i for i, x in enumerate(output) if x != 0.), None),
                            output_sha256=hashlib.sha256(struct.pack('<' + 'f' * len(output), *output)).hexdigest(),
                            parameters={hex(o): v for o, v in params.items()}, filter_modes={hex(o): v for o, v in modes.items()},
                            output=output, reference_output=expected_output))
    result = dict(status='PASS', profiles=results, comparisons=comparisons, max_abs_error=error_max,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(coverage), coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Complete original MonoPlate process only, not full channel or hardware.',
                               'Explicit host finite floorf service; other DSP instructions original.',
                               'Coefficients/modes seeded from original constructor or explicit fixture settings; modulation tables independently reconstructed.',
                               'Control publication and table construction checked separately by probe_plate_controls.py; no full Time event or sample-rate adapter comparison.',
                               'Model and byte execution share float32/FMA semantics, not independent physical ARM hardware.'])
    if not args.quick:
        (ROOT / 'probes/plate_process_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(status=result['status'], profiles=[{k: r[k] for k in ('profile', 'samples', 'peak', 'first_nonzero')} for r in results],
                         comparisons=comparisons, max_abs_error=error_max, distinct_instruction_addresses=len(coverage)), indent=2))


if __name__ == '__main__':
    main()
