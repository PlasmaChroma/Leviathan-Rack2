#!/usr/bin/env python3
"""Complete Time setter in effect mode, with factory and clamp fixtures."""
import hashlib
import json
import math

from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_plate_process import initialized
from probe_plate_initialization import CHANNEL, PLATE
from probe_plate_controls import LINK, PRESET, MIRROR
from plate_model import DRY, WET, DECAY

WORK = CHANNEL + 172032
FIELDS = {'WowFlutterDepth': 624, 'CrinkleDepth': 628, 'Wear': 632,
          'TapeAge': 636, 'Hysterisis': 640, 'Knee': 644,
          'Compensation': 648, 'Reverb': 664}
DESTINATIONS = {'WowFlutterDepth': (18984,), 'CrinkleDepth': (18988,),
                'TapeAge': (6648, 12804), 'Wear': (6748, 12904),
                'Hysterisis': (6752, 12908)}


def main():
    cpu = initialized()
    cpu.putu(CHANNEL + 0xE8, LINK)
    cpu.putu(LINK + 0xB0, PRESET)
    cpu.putu(CHANNEL + 171868, MIRROR)
    cpu.putu(WORK + 0x4D0, 2)
    cpu.putu(PRESET + 44, 2)
    presets = json.loads((ROOT / 'tables/factory_presets.json').read_text())
    presets['clamp_low'] = {key: -.5 for key in FIELDS}
    presets['clamp_high'] = {key: 2. for key in FIELDS}
    rows, assertions = [], 0
    # Execute the constructor's fixed output clipper publication, including its
    # real stack-loaded pointer, without entering subsequent asset setup.
    cpu.putu(STACK + 0x24, CHANNEL + 25172)
    cpu.reg(4, CHANNEL)
    cpu.reg(5, WORK)
    cpu.call(0x3D7E4, stop_before=0x3D844)
    assert cpu.getf(CHANNEL + 25172) == .5
    assert cpu.getf(CHANNEL + 25176) == 1.25
    assert cpu.read(CHANNEL + 25180, 1)[0] == 0
    assertions += 3
    for name, preset in presets.items():
        for key, offset in FIELDS.items():
            cpu.putf(PRESET + 16384 + offset, preset[key])
        for raw in (-1000, -1, 0, 1, 28, 29, 30, 1024, 2047, 2048, 4094, 4095, 5000):
            value = max(0, min(4095, raw))
            amount = f32(value / 4095.)
            cpu.reg(0, CHANNEL)
            cpu.reg(1, raw)
            cpu.reg(2, 1)
            cpu.call(0x37A44)
            checks = [(LINK + 168, amount), (MIRROR + 40, amount)]
            for key, destinations in DESTINATIONS.items():
                published = max(0., min(1., f32(amount * f32(preset[key]))))
                checks += [(CHANNEL + offset, published) for offset in destinations]
            knee = max(0., min(1., f32(preset['Knee'])))
            compensation = max(0., min(1., f32(amount * f32(preset['Compensation']))))
            rational = knee <= .01
            gain = f32(1. + compensation) if rational else libm.fmaf(compensation, f32(f32(2. / f32(1. + knee)) - 1.), 1.)
            checks.append((CHANNEL + 25164, gain))
            if not rational:
                checks.append((CHANNEL + 25160, knee))
            u = max(0., min(1., f32(amount * f32(preset['Reverb']))))
            complement = f32(1. - u)
            norm = f32(math.sqrt(libm.fmaf(u, u, f32(complement * complement))))
            checks += [(PLATE + DRY, f32(complement / norm)), (PLATE + WET, f32(u / norm)),
                       (PLATE + DECAY, min(f32(2. * u), f32(.9)))]
            for address, expected in checks:
                assert cpu.getf(address) == expected, (name, raw, hex(address), expected, cpu.getf(address))
                assertions += 1
            assert cpu.read(CHANNEL + 25168, 1)[0] == int(rational)
            assert cpu.getu(WORK + 0x4AC) == value
            assert cpu.getu(WORK + 0x4BC) == value
            assertions += 3
            rows.append(dict(preset=name, raw=raw, saturated=value, amount=amount,
                             knee=knee, compensation=compensation, rational=rational,
                             write_clip_gain=gain, dry=f32(complement / norm), wet=f32(u / norm)))
    # An effect publication sentinel reveals whether the full setter runs.
    # Mode 0 forces larger movements. Other modes divert unforced larger
    # movements into a different control path, which this probe stops before.
    deadband = []
    for channel_mode in (0, 1, 2):
        cpu.putu(CHANNEL + 0x104, channel_mode)
        for forced in (0, 1):
            for delta in (-30, -29, -1, 0, 1, 29, 30):
                cpu.putu(WORK + 0x4AC, 2048)
                cpu.putf(LINK + 168, -123.)
                cpu.reg(0, CHANNEL)
                cpu.reg(1, 2048 + delta)
                cpu.reg(2, forced)
                cpu.call(0x37A44, stop_before=0x37D6C)
                expected = bool(forced or (channel_mode == 0 and abs(delta) > 29))
                # Tiny movement returns before the Channel mode check.
                # The <=29 branch accepts it only when forced.
                actual = cpu.getf(LINK + 168) != -123.
                assert actual == expected, (channel_mode, forced, delta, expected, actual)
                assertions += 1
                deadband.append(dict(channel_mode=channel_mode, forced=forced, delta=delta, published=actual))
    preset_clipping = []
    for name, preset in presets.items():
        cpu.putf(PRESET + 16384 + 644, preset['Knee'])
        cpu.putf(PRESET + 16384 + 648, preset['Compensation'])
        for amount in (0., .125, .5, .75, 1., 1.5):
            cpu.reg(2, PRESET + 16384)
            cpu.reg(5, CHANNEL + 167936)
            cpu.reg(6, CHANNEL)
            cpu.fp(1, amount)
            cpu.fp(12, 1.)
            cpu.fp(13, 0.)
            cpu.call(0x3E138, stop_before=0x3E1A4)
            knee = max(0., min(1., f32(preset['Knee'])))
            compensation = max(0., min(2., f32(f32(amount) * f32(preset['Compensation']))))
            rational = knee <= .01
            gain = f32(1. + compensation) if rational else libm.fmaf(compensation, f32(f32(2. / f32(1. + knee)) - 1.), 1.)
            assert cpu.getf(CHANNEL + 25164) == gain
            assert cpu.read(CHANNEL + 25168, 1)[0] == int(rational)
            assertions += 2
            if not rational:
                assert cpu.getf(CHANNEL + 25160) == knee
                assertions += 1
            preset_clipping.append(dict(preset=name, amount=amount, knee=knee,
                                       compensation=compensation, rational=rational, gain=gain))
    result = dict(status='PASS', publication_cases=len(rows), deadband_cases=len(deadband),
                  preset_clip_cases=len(preset_clipping), assertions=assertions,
                  fixtures=rows, deadband=deadband, preset_clipping=preset_clipping,
                  output_clipper_constructor=dict(knee=.5, compensation=.75, gain=1.25, rational=False),
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(cpu.coverage), coverage_addresses=[hex(a) for a in sorted(cpu.coverage)],
                  limitations=['Complete forced setTime routine in effect selector 2 with preset TimePot=2; no other selectors or linked UI event producer.',
                               'Deadband tests stop before the unforced nonzero Channel-mode diversion at 0x37d6c; that alternate path is not executed.',
                               'Factory preset scalar fields injected at recovered offsets; preset parsing/loading not executed.',
                               'Preset update clipper sub-slice checked separately; whole preset update not executed.',
                               'Only finite scalar inputs; no analog ADC/noise calibration.'])
    (ROOT / 'probes/time_effect_publication_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'publication_cases', 'deadband_cases', 'preset_clip_cases', 'assertions', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
