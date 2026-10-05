#!/usr/bin/env python3
"""Original input filter constructor, coefficients and connected color slice."""
import hashlib
import json
import math

from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR
from arm_byte_probe import ARMBytes, ROOT, _PLT_MAP
from arm_leaf_probe import f32, libm
from input_color_model import highpass_coefficients, preset_coefficients, InputFilterModel
from output_color_model import TapeFilterModel, DiffuserModel, wear

CHANNEL, PRESET, HEADER, DATA = 0x100000, 0x140000, 0x1E0000, 0x1F0000
INPUT, AGE, WEAR, DIFFUSER = (CHANNEL + o for o in (6592, 6648, 6748, 6752))
assert _PLT_MAP['0x159d8'] == 'tanf'


class InputBytes(ARMBytes):
    def _code(self, cpu, address, size, user):
        if address == 0x159D8:
            value = self.fp(0)
            assert math.isfinite(value)
            self.fp(0, libm.tanf(value))
            self.coverage.add(address)
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, user)


def main():
    cpu = InputBytes()
    for address, obj in ((0x4F420, INPUT), (0x4F494, AGE), (0x4FCA4, DIFFUSER)):
        cpu.reg(0, obj)
        cpu.call(address)
    a_ptr, b_ptr, state_ptr = (cpu.getu(INPUT + o) for o in (4, 16, 28))
    comparisons, error_max = 0, 0.

    def check(expected, actual, label):
        nonlocal comparisons, error_max
        assert len(expected) == len(actual)
        for want, observed in zip(expected, actual):
            error = abs(want - observed)
            assert math.isfinite(observed) and error <= 8e-7, (label, want, observed, error)
            comparisons += 1
            error_max = max(error_max, error)

    coefficient_cases = []
    for frequency in (-1., 0., 1e-6, .0004, .1, .25, .5, .999, 1., 2.):
        for quality in (-1., .2, .5, .8, 2.):
            cpu.reg(0, INPUT)
            cpu.fp(0, frequency)
            cpu.fp(1, quality)
            cpu.call(0x4A970)
            a, b = highpass_coefficients(frequency, quality)
            check(a + b, cpu.floats(a_ptr, 3) + cpu.floats(b_ptr, 3), 'coefficients')
            coefficient_cases.append(dict(frequency=frequency, quality=quality, a=a, b=b))
    presets = json.loads((ROOT / 'tables/factory_presets.json').read_text())
    presets['clamp_low'] = dict(LowCutFreq=-1., LowCutQ=-1., HighCutFreq=-1.)
    presets['clamp_high'] = dict(LowCutFreq=40000., LowCutQ=2., HighCutFreq=40000.)
    # Coefficient publication must retain histories through changing presets.
    model = InputFilterModel([1., 0., 0.], [1., 0., 0.], 1.)
    age_model = TapeFilterModel([cpu.getf(AGE + 4 + i * 12) for i in range(5)])
    diffuser = DiffuserModel()
    trajectories, absolute = [], 0
    for iteration in range(3):
        for name, preset in presets.items():
            for shift, key in ((652, 'LowCutFreq'), (656, 'LowCutQ'), (660, 'HighCutFreq')):
                cpu.putf(PRESET + 16384 + shift, preset[key])
            cpu.reg(3, PRESET)
            cpu.reg(4, CHANNEL + 4096)
            cpu.reg(6, CHANNEL)
            cpu.fp(14, 20000.)
            before = cpu.floats(state_ptr, 3) + cpu.floats(INPUT + 48, 2)
            cpu.call(0x3E1E4, stop_before=0x3E2A4)
            a, b, c = preset_coefficients(preset['LowCutFreq'], preset['LowCutQ'], preset['HighCutFreq'])
            check(a + b + [c], cpu.floats(a_ptr, 3) + cpu.floats(b_ptr, 3) + [cpu.getf(INPUT + 44)], 'preset_coefficients')
            assert cpu.floats(state_ptr, 3) + cpu.floats(INPUT + 48, 2) == before
            model.a, model.b, model.c = a, b, c
            for block in range(8):
                frames = (7, 32, 128)[(absolute + block) % 3]
                age, wear_amount, hysteresis = map(f32, (0., 0., 0.) if block < 2 else (.25, .5, .7))
                cpu.putf(AGE, age)
                cpu.putf(WEAR, wear_amount)
                cpu.putf(DIFFUSER, hysteresis)
                samples = [f32(.4 * math.sin((absolute + i) * .071) + .1) for i in range(frames)]
                cpu.vector(HEADER, DATA, samples)
                for i, address in enumerate((0x200000, 0x201000, 0x202000)):
                    cpu.vector(AGE + 64 + i * 12, address, [0.] * frames)
                expected = {key: [] for key in ('input_filter', 'age', 'wear', 'diffuser')}
                for sample in samples:
                    x = model.process(sample)
                    expected['input_filter'].append(x)
                    x = age_model.process(x, age)
                    expected['age'].append(x)
                    x = wear(x, wear_amount)
                    expected['wear'].append(x)
                    x = diffuser.process(x, hysteresis)
                    expected['diffuser'].append(x)
                observed = {}
                for address, stage in ((0x47D50, 'input_filter'), (0x47D60, 'age'), (0x47D70, 'wear')):
                    cpu.observers[address] = lambda machine, name=stage: observed.update({name: machine.floats(DATA, frames)})
                cpu.reg(4, CHANNEL)
                cpu.reg(6, HEADER)
                cpu.call(0x47D44, stop_before=0x47D80)
                observed['diffuser'] = cpu.floats(DATA, frames)
                for key in expected:
                    check(expected[key], observed[key], key)
                check(model.state + model.low_state, cpu.floats(state_ptr, 3) + cpu.floats(INPUT + 48, 2), 'input_state')
                check([v for s in age_model.state for v in s], [cpu.getf(AGE + 8 + i * 12 + j * 4) for i in range(5) for j in range(2)], 'age_state')
                assert diffuser.indices == [cpu.getu(DIFFUSER + 0x1774 + i * 4) for i in range(4)]
                trajectories.append(dict(preset=name, iteration=iteration, frames=frames, start_sample=absolute,
                                         age=age, wear=wear_amount, hysteresis=hysteresis,
                                         output=observed['diffuser'], model_output=expected['diffuser']))
                absolute += frames
    result = dict(status='PASS', coefficient_cases=coefficient_cases, preset_updates=36,
                  blocks=len(trajectories), samples=absolute, comparisons=comparisons, max_abs_error=error_max,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(cpu.coverage), coverage_addresses=[hex(a) for a in sorted(cpu.coverage)], fixtures=trajectories,
                  limitations=['Original constructor, high-pass coefficient setter, preset filter publication slice and input coloration slice only.',
                               'Explicit host glibc tanf hook at verified import; no original ARM libm or analog equivalence.',
                               'Full preset update, routing/gating, input AntiAlias, input history, transport and recording not connected here.',
                               'Quality/frequency edge fixtures include nonproduction normalized setter inputs; coefficient update preserves existing histories.'])
    (ROOT / 'probes/input_color_pipeline_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'preset_updates', 'blocks', 'samples', 'comparisons', 'max_abs_error', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
