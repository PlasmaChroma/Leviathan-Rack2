#!/usr/bin/env python3
"""Persistent original output coloration callback slice and independent chain."""
import hashlib
import json
import math
import struct

from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from probe_plate_process import initialized, parameters
from probe_plate_initialization import CHANNEL, PLATE
from probe_plate_controls import LINK, PRESET, MIRROR
from plate_model import PlateModel, FILTERS, MODULATORS, DRY, WET, DECAY
from output_color_model import TapeFilterModel, DiffuserModel, wear, clipper

WORK = CHANNEL + 172032
FILTER, WEAR, DIFFUSER, CLIPPER = CHANNEL + 12804, CHANNEL + 12904, CHANNEL + 12908, CHANNEL + 25172
HEADER, DATA, OTHER = 0x1F0000, 0x1E0000, 0x230000
SCRATCH = (0x220000, 0x221000, 0x222000)


def main():
    rows, coverage, comparisons, error_max = [], set(), 0, 0.
    for profile in ('clean', 'colored_knee', 'colored_rational'):
        cpu = initialized()
        params, modes, tables = parameters(cpu)
        plate = PlateModel(params, modes, tables)
        for address, obj in ((0x4F494, FILTER), (0x4FCA4, DIFFUSER)):
            cpu.reg(0, obj)
            cpu.call(address)
        tape_filter = TapeFilterModel([cpu.getf(FILTER + 4 + i * 12) for i in range(5)])
        diffuser = DiffuserModel()
        knee, compensation = map(f32, ((.3, 0.) if profile == 'clean' else (.3, .2) if profile == 'colored_knee' else (0., .2)))
        cpu.reg(0, CLIPPER)
        cpu.fp(0, knee)
        cpu.fp(1, compensation)
        cpu.call(0x4FFE0)
        cpu.putu(CHANNEL + 0xE8, LINK)
        cpu.putu(LINK + 0xB0, PRESET)
        cpu.putu(CHANNEL + 171868, MIRROR)
        cpu.putu(WORK + 760, CHANNEL)
        cpu.putu(CHANNEL + 0x1C, OTHER)
        history, absolute = [], 0
        for block in range(160):
            frames = 128 if profile == 'colored_knee' else (7, 32, 128)[block % 3]
            age, wear_amount, hysteresis = map(f32, (0., 0., 0.) if profile == 'clean' else ((.25, .5, .7) if block % 40 < 20 else (.7, .125, .25)))
            amount = f32(0. if profile == 'clean' else (.125, .5, .75, 1.)[(block // 40) % 4])
            cpu.putf(FILTER, age)
            cpu.putf(WEAR, wear_amount)
            cpu.putf(DIFFUSER, hysteresis)
            cpu.putf(LINK + 168, amount)
            cpu.putf(PRESET + 16384 + 664, 1.)
            cpu.reg(4, CHANNEL)
            cpu.call(0x37CE8, stop_before=0x37D54)
            complement = f32(1. - amount)
            norm = f32(math.sqrt(libm.fmaf(amount, amount, f32(complement * complement))))
            plate.params.update({DRY: f32(complement / norm), WET: f32(amount / norm), DECAY: min(f32(2. * amount), f32(.9))})
            samples = [f32(.75 * math.sin((absolute + i) * .071) if absolute + i < 256 or 4000 <= absolute + i < 4200 else .8 if absolute + i in (2000, 6000) else 0.) for i in range(frames)]
            cpu.vector(HEADER, DATA, samples)
            for i, address in enumerate(SCRATCH):
                cpu.vector(FILTER + 64 + i * 12, address, [0.] * frames)
            expected = {stage: [] for stage in ('filter', 'wear', 'diffuser', 'plate', 'clipper')}
            for sample in samples:
                x = tape_filter.process(sample, age)
                expected['filter'].append(x)
                x = wear(x, wear_amount)
                expected['wear'].append(x)
                x = diffuser.process(x, hysteresis)
                expected['diffuser'].append(x)
                x = plate.process(x)
                expected['plate'].append(x)
                x = clipper(x, knee, compensation)
                expected['clipper'].append(x)
            observed = {}
            for address, stage in ((0x48468, 'filter'), (0x48478, 'wear'), (0x48488, 'diffuser'), (0x484BC, 'plate'), (0x484D8, 'clipper')):
                cpu.observers[address] = lambda machine, name=stage: observed.update({name: machine.floats(DATA, frames)})
            cpu.reg(4, CHANNEL)
            cpu.reg(5, WORK)
            cpu.reg(10, HEADER)
            # Stop observer at 0x484d8 is not called, since the harness stops
            # before executing that address. Capture its vector after return.
            cpu.call(0x48454, stop_before=0x484D8)
            observed['clipper'] = cpu.floats(DATA, frames)
            checks = [(stage, want, actual) for stage in expected for want, actual in zip(expected[stage], observed[stage])]
            checks += [(f'filter_state:{i}:{j}', value, cpu.getf(FILTER + 4 + i * 12 + 4 + j * 4)) for i in range(5) for j, value in enumerate(tape_filter.state[i])]
            checks += [(f'plate_state:{o:#x}:{j}', value, cpu.getf(PLATE + o + 8 + j * 4)) for o in FILTERS for j, value in enumerate(plate.filter_state[o])]
            checks += [(f'phase:{o:#x}', plate.phase[o], cpu.getf(PLATE + o + 28)) for o in MODULATORS]
            for stage, want, actual in checks:
                error = abs(want - actual)
                assert math.isfinite(actual) and error <= 8e-7, (profile, block, stage, want, actual, error)
                error_max = max(error_max, error)
                comparisons += 1
            assert diffuser.indices == [cpu.getu(DIFFUSER + 0x1774 + i * 4) for i in range(4)]
            history.append(dict(block=block, frames=frames, tape_age=age, wear=wear_amount, hysteresis=hysteresis, reverb_amount=amount,
                                output=observed['clipper'], model_output=expected['clipper']))
            absolute += frames
        rows.append(dict(profile=profile, samples=absolute, knee=knee, compensation=compensation, blocks=history))
        coverage.update(cpu.coverage)
    result = dict(status='PASS', sequences=len(rows), blocks=sum(len(r['blocks']) for r in rows),
                  samples=sum(r['samples'] for r in rows), comparisons=comparisons, max_abs_error=error_max,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(coverage), coverage_addresses=[hex(a) for a in sorted(coverage)], fixtures=rows,
                  limitations=['Original continuous output slice 0x48454..0x484d8 only; input is supplied mix vector, not rendered tape.',
                               'Preview disabled for both fixture decks; active file-preview branch unvalidated.',
                               'Original reverb control sub-slice; remaining effect amounts and clipper settings explicitly supplied.',
                               'No full Channel callback, input coloration, recording, control producer, analog or hardware equivalence.'])
    (ROOT / 'probes/output_color_pipeline_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'sequences', 'blocks', 'samples', 'comparisons', 'max_abs_error', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
