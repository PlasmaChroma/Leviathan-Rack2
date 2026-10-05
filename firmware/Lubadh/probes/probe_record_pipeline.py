#!/usr/bin/env python3
"""Persistent playback followed by actual tape writes and manager updates.

Original callback slices execute; --connected includes output coloration.
--input-chain connects original input coloration/AntiAlias/history; motion still
runs as explicit original calls. No appliance is booted.
"""
import hashlib
import argparse
import itertools
import json
import math
import struct

from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_antialias import coefficient, cutoff_for_speed, reference
from probe_engine_gain import BASE
from probe_playback_pipeline import AA, GAIN, MIX, MIX_HEADER, active, raw_model, rounded_cubic
from probe_splice_render import CHANNEL, MANAGER, WORK, RECORD_MANAGER, TAPE, OUTPUT, GAINS, TABLE, fade_curve
from probe_tap_allocation import call
from probe_boundary_transitions import LINK
from probe_plate_process import initialized, parameters
from probe_plate_initialization import PLATE
from probe_plate_controls import PRESET, MIRROR
from plate_model import PlateModel, FILTERS, MODULATORS, DRY, WET, DECAY
from output_color_model import TapeFilterModel, DiffuserModel, wear, clipper
from probe_input_color_pipeline import InputBytes
from probe_plate_process import PlateBytes
from input_color_model import InputFilterModel, preset_coefficients

INPUT, INPUT_DATA, BLOCK_HEADER, BLOCK_DATA = 0x160000, 0x170000, 0x210000, 0x220000
CONTRIBUTION, INDICES, FEEDBACK = 0x190000, 0x1A0000, 0x1B0000
LENGTH, CAPACITY = 8192, 1024


class InputPlateBytes(InputBytes, PlateBytes):
    """Explicit tanf and floorf services; all surrounding DSP remains original."""
    pass


def clip(value, knee, compensation):
    if knee == 0.:
        value = f32(value * f32(1. + compensation))
        square = f32(value * value)
        denominator = libm.fmaf(f32(9.42477798461914), square, f32(28.274333953857422))
        value = f32(f32(value * f32(square + f32(28.274333953857422))) / denominator)
        return max(-1., min(1., value))
    value = max(-1., min(1., value))
    delta = f32(abs(value) - knee)
    if delta <= 0.:
        return value
    ratio = f32(delta / f32(1. - knee))
    magnitude = f32(knee + f32(delta / libm.fmaf(ratio, ratio, 1.)))
    return math.copysign(magnitude, value)


def main():
    global INPUT
    parser = argparse.ArgumentParser()
    parser.add_argument('--connected', action='store_true')
    parser.add_argument('--cycles', type=int, default=1)
    parser.add_argument('--quick', action='store_true')
    parser.add_argument('--quick-profile', choices=('following', 'held_opposed', 'two_overlap', 'playback_splices'), default='two_overlap')
    parser.add_argument('--native-output-clipper', action='store_true')
    parser.add_argument('--input-chain', action='store_true')
    args = parser.parse_args()
    assert args.cycles > 0
    assert not args.native_output_clipper or args.connected
    assert not args.input_chain or args.connected
    if args.input_chain:
        INPUT = CHANNEL + 172004
        presets = list(json.loads((ROOT / 'tables/factory_presets.json').read_text()).items())
    rows, coverage, comparisons, assertions = [], set(), 0, 0
    errors = dict(raw=0., filtered=0., mix=0., tape=0., phase=0., filter_state=0.)
    if args.connected:
        errors.update(color_filter=0., wear=0., diffuser=0., plate=0., clipper=0., color_state=0.)
    if args.input_chain:
        errors.update(input_filter=0., input_age=0., input_wear=0., input_diffuser=0., input_antialias=0., input_state=0., input_coefficients=0.)
    write_calls, no_write_calls, overlap_blocks, order_sensitive_blocks = 0, 0, 0, 0

    def check(field, expected, actual):
        nonlocal comparisons
        assert len(expected) == len(actual)
        for want, observed in zip(expected, actual):
            error = abs(want - observed)
            assert math.isfinite(observed) and error <= 8e-7, (field, want, observed, error)
            errors[field] = max(errors[field], error)
            comparisons += 1

    fixtures = [(128, args.quick_profile, .5, (.3, 0.))] if args.quick else itertools.product(
        (7, 32, 128), ('following', 'held_opposed', 'two_overlap', 'playback_splices'),
        (0., .5, 1.), ((1., 0.), (.3, 0.), (0., .2)))
    for frames, profile, feedback, clipping in fixtures:
        if args.input_chain:
            cpu = InputPlateBytes()
            cpu.reg(4, CHANNEL)
            cpu.call(0x3CFF0, stop_before=0x3D618)
        else:
            cpu = initialized() if args.connected else ARMBytes()
        cpu.putu(CHANNEL + 0xE8, LINK)
        call(cpu, 0x4CA68, MANAGER)
        call(cpu, 0x4CA68, RECORD_MANAGER)
        cpu.reg(4, CHANNEL)
        cpu.call(0x3CF0C, stop_before=0x3CF34)
        call(cpu, 0x4B09C, AA)
        cpu.putu(CHANNEL + 171900, CHANNEL)
        cpu.putu(CHANNEL + 171972, TAPE)
        tape = [f32(.4 * math.sin(i * .137) + .15 * math.cos(i * .071)) for i in range(LENGTH)]
        cpu.write(TAPE, struct.pack('<8192f', *tape))
        cpu.write(0x93F98, struct.pack('<256f', *TABLE))
        for offset, pointer, integers in ((36, CONTRIBUTION, False), (48, INDICES, True), (60, FEEDBACK, False)):
            cpu.vector(WORK + offset, pointer, [0] * CAPACITY, integers=integers)
        knee, compensation = map(f32, clipping)
        cpu.reg(0, CHANNEL + 25160)
        cpu.fp(0, knee)
        cpu.fp(1, compensation)
        cpu.call(0x4FFE0)
        if args.connected:
            color_filter, color_wear, color_diffuser, color_clipper = (CHANNEL + o for o in (12804, 12904, 12908, 25172))
            call(cpu, 0x4F494, color_filter)
            call(cpu, 0x4FCA4, color_diffuser)
            params, modes, tables = parameters(cpu)
            plate = PlateModel(params, modes, tables)
            tape_filter = TapeFilterModel([cpu.getf(color_filter + 4 + i * 12) for i in range(5)])
            diffuser = DiffuserModel()
            output_knee, output_compensation = knee, compensation
            if args.native_output_clipper:
                cpu.putu(STACK + 0x24, color_clipper)
                cpu.reg(4, CHANNEL)
                cpu.reg(5, WORK)
                cpu.call(0x3D7E4, stop_before=0x3D844)
                output_knee, output_compensation = .5, .75
                assert cpu.getf(color_clipper) == .5 and cpu.getf(color_clipper + 4) == 1.25
                assert cpu.read(color_clipper + 8, 1)[0] == 0
            else:
                cpu.reg(0, color_clipper)
                cpu.fp(0, knee)
                cpu.fp(1, compensation)
                cpu.call(0x4FFE0)
            cpu.putu(LINK + 0xB0, PRESET)
            cpu.putu(CHANNEL + 171868, MIRROR)
            cpu.putu(WORK + 760, CHANNEL)
            cpu.putu(CHANNEL + 0x1C, 0x230000)
        play = call(cpu, 0x4EEA8, MANAGER, 3500, 32 if profile == 'playback_splices' else 4096, True)
        cpu.putf(play + 8, .375)
        for i in range(2 if profile == 'two_overlap' else 1):
            head = call(cpu, 0x4EEA8, RECORD_MANAGER, 3500 + i * 2, 4096, True)
            cpu.putf(head + 8, .375)
            if profile == 'held_opposed' or i:
                cpu.write(head + 20, b'\x01')
                cpu.putf(head + 24, -1. if profile == 'held_opposed' else .7)
        buffer, phase = [f32(-.03 + i * .01) for i in range(4)] + [0.] * frames, .125
        cpu.vector(INPUT + 4, INPUT_DATA, buffer)
        cpu.putf(INPUT + 16, phase)
        if args.input_chain:
            input_filter, input_age, input_wear, input_diffuser, input_aa = (CHANNEL + o for o in (6592, 6648, 6748, 6752, 6544))
            for address, obj in ((0x4F420, input_filter), (0x4F494, input_age), (0x4FCA4, input_diffuser), (0x4B09C, input_aa)):
                call(cpu, address, obj)
            input_model = InputFilterModel([1., 0., 0.], [1., 0., 0.], 1.)
            input_age_model = TapeFilterModel([cpu.getf(input_age + 4 + i * 12) for i in range(5)])
            input_diffuser_model = DiffuserModel()
            input_aa_state = [[0., 0.], [0., 0.]]
        state, gain, increment, remaining, cached = [[0., 0.], [0., 0.]], 0., 0., 0, 0
        previous_speed = f32(.3)
        history = []
        for block, speed in enumerate((.3, .7, 1.3, 0., -1.3, -.7, -.3, 0., 2., -2., .25, 1.) * args.cycles):
            speed = f32(speed)
            factor = (.5, 1., .75)[block % 3]
            previous_factor = (.5, 1., .75)[(block - 1) % 3] if block else factor
            cpu.putf(LINK, speed)
            cpu.putf(LINK + 164, feedback)
            if args.connected:
                age, wear_amount, hysteresis = map(f32, ((.25, .5, .7) if block % 8 < 4 else (.7, .125, .25)))
                amount = f32((.125, .5, .75, 1.)[(block // 3) % 4])
                cpu.putf(color_filter, age)
                cpu.putf(color_wear, wear_amount)
                cpu.putf(color_diffuser, hysteresis)
                cpu.putf(LINK + 168, amount)
                cpu.putf(PRESET + 16384 + 664, 1.)
                cpu.reg(4, CHANNEL)
                cpu.call(0x37CE8, stop_before=0x37D54)
                complement = f32(1. - amount)
                norm = f32(math.sqrt(libm.fmaf(amount, amount, f32(complement * complement))))
                plate.params.update({DRY: f32(complement / norm), WET: f32(amount / norm), DECAY: min(f32(2. * amount), f32(.9))})
                for i, address in enumerate((0x240000, 0x241000, 0x242000)):
                    cpu.vector(color_filter + 64 + i * 12, address, [0.] * frames)
            samples = [f32(.7 * math.sin((block * frames + i) * .071)) for i in range(frames)]
            cpu.vector(BLOCK_HEADER, BLOCK_DATA, samples)
            if args.input_chain:
                preset_name, preset = presets[(block // 3) % len(presets)]
                for offset, key in ((652, 'LowCutFreq'), (656, 'LowCutQ'), (660, 'HighCutFreq')):
                    cpu.putf(PRESET + 16384 + offset, preset[key])
                cpu.reg(3, PRESET)
                cpu.reg(4, CHANNEL + 4096)
                cpu.reg(6, CHANNEL)
                cpu.fp(14, 20000.)
                cpu.call(0x3E1E4, stop_before=0x3E2A4)
                a, b, low_c = preset_coefficients(preset['LowCutFreq'], preset['LowCutQ'], preset['HighCutFreq'])
                input_model.a, input_model.b, input_model.c = a, b, low_c
                check('input_coefficients', a + b + [low_c], cpu.floats(cpu.getu(input_filter + 4), 3) + cpu.floats(cpu.getu(input_filter + 16), 3) + [cpu.getf(input_filter + 44)])
                for obj, value in ((input_age, age), (input_wear, wear_amount), (input_diffuser, hysteresis)):
                    cpu.putf(obj, value)
                for i, address in enumerate((0x250000, 0x251000, 0x252000)):
                    cpu.vector(input_age + 64 + i * 12, address, [0.] * frames)
                fixed_input_cutoff = int(block % 6 >= 3)
                cpu.putu(LINK + 160, fixed_input_cutoff)
                cpu.putf(LINK + 4, previous_speed)
                cpu.putf(LINK + 12, factor)
                cpu.putf(LINK + 16, 1.)
                predicted_input = {key: [] for key in ('input_filter', 'input_age', 'input_wear', 'input_diffuser')}
                for sample in samples:
                    x = input_model.process(sample)
                    predicted_input['input_filter'].append(x)
                    x = input_age_model.process(x, age)
                    predicted_input['input_age'].append(x)
                    x = wear(x, wear_amount)
                    predicted_input['input_wear'].append(x)
                    x = input_diffuser_model.process(x, hysteresis)
                    predicted_input['input_diffuser'].append(x)
                input_cutoff = 20000. if fixed_input_cutoff else cutoff_for_speed(speed)
                check('input_state', input_aa_state[0] + input_aa_state[1], [cpu.getf(input_aa + o) for o in (4, 8, 16, 20)])
                predicted_input['input_antialias'] = reference(predicted_input['input_diffuser'], coefficient(input_cutoff), input_aa_state)
                input_observed = {}
                cpu.observers[0x47D44] = lambda machine: check('input_coefficients', [coefficient(input_cutoff)] * 2, [machine.getf(input_aa), machine.getf(input_aa + 12)])
                for address, key in ((0x47D50, 'input_filter'), (0x47D60, 'input_age'), (0x47D70, 'input_wear'), (0x47D80, 'input_diffuser'), (0x47DE0, 'input_antialias')):
                    cpu.observers[address] = lambda machine, name=key: input_observed.update({name: machine.floats(BLOCK_DATA, frames)})
                cpu.reg(4, CHANNEL)
                cpu.reg(6, BLOCK_HEADER)
                cpu.call(0x47CC0, stop_before=0x47DF4)
                for key in predicted_input:
                    check(key, predicted_input[key], input_observed[key])
                check('input_state', input_model.state + input_model.low_state +
                      [v for s in input_age_model.state for v in s] + input_aa_state[0] + input_aa_state[1],
                      cpu.floats(cpu.getu(input_filter + 28), 3) + cpu.floats(input_filter + 48, 2) +
                      [cpu.getf(input_age + 8 + i * 12 + j * 4) for i in range(5) for j in range(2)] +
                      [cpu.getf(input_aa + o) for o in (4, 8, 16, 20)])
                assert input_diffuser_model.indices == [cpu.getu(input_diffuser + 0x1774 + i * 4) for i in range(4)]
                buffer = buffer[-4:] + predicted_input['input_antialias']
            else:
                buffer = buffer[-4:] + samples
                cpu.reg(0, INPUT)
                cpu.reg(1, BLOCK_HEADER)
                cpu.call(0x38A78)
            assert cpu.floats(INPUT_DATA, frames + 4) == buffer
            assertions += 1
            heads, records = active(cpu, MANAGER), active(cpu, RECORD_MANAGER)
            for head in heads + records:
                cpu.reg(0, head)
                cpu.fp(0, speed)
                cpu.fp(1, factor)
                cpu.fp(2, frames)
                cpu.call(0x4BDE0)
            c = dict(frames=frames, speed=speed, previous_speed=previous_speed,
                     factor=factor, previous_factor=previous_factor)
            before_tape = list(tape)
            before_original = cpu.floats(TAPE, LENGTH)
            cutoff = cutoff_for_speed(speed)
            cpu.reg(0, AA)
            cpu.fp(0, cutoff)
            cpu.call(0x4B1C0)
            count = call(cpu, 0x4DDD0, MANAGER)
            target = 1.
            for _ in range(max(0, count - 1)):
                target = f32(target * BASE)
            if count != cached:
                increment, remaining, cached = f32(f32(target - gain) * .125), 8, count
            if remaining:
                gain, remaining = f32(gain + increment), remaining - 1
            prior = [f32(.1 * math.cos(i * .071 - block)) for i in range(frames)]
            cpu.vector(WORK + 12, OUTPUT, [0.] * frames)
            cpu.vector(WORK + 24, GAINS, [1.] * frames)
            cpu.vector(MIX_HEADER, MIX, prior)
            for offset, value in ((0x20, MANAGER), (0x30, RECORD_MANAGER), (0x54, AA),
                                  (0x5C, WORK + 12), (0x2C, frames), (0x58, INPUT), (0x3C, 0)):
                cpu.putu(STACK + offset, value)
            if profile == 'playback_splices':
                for offset, value in ((0x0C, 3430), (0x1C, 3550), (0x28, 3462),
                                      (0x14, 3582), (0x18, 32), (0x34, 1), (0x38, 49170),
                                      (0x40, 2459), (0x48, 51628), (0x24, 3462), (0x4C, 0), (0x08, 0)):
                    cpu.putu(STACK + offset, value)
                cpu.reg(0, MANAGER)
                cpu.reg(11, MANAGER)
                cpu.reg(8, STACK + 128)
            cpu.reg(4, CHANNEL)
            cpu.reg(5, WORK)
            cpu.reg(10, MIX_HEADER)
            for register, value in ((16, frames), (17, factor), (18, previous_factor),
                                     (19, speed), (20, previous_speed), (21, abs(speed)), (22, abs(previous_speed))):
                cpu.fp(register, value)
            observed = {}
            render_model = {}
            def at_render(machine):
                scratch = MANAGER + 2736
                render_heads = [machine.getu(scratch + i * 4) for i in range(20) if machine.getu(scratch + i * 4)]
                render_model['heads'] = render_heads
                render_model['raw'] = raw_model(machine, render_heads, records, c, tape)
            cpu.observers[0x48010] = at_render
            cpu.observers[0x483A4] = lambda machine: observed.update(raw=machine.floats(OUTPUT, frames))
            cpu.observers[0x483B0] = lambda machine: observed.update(filtered=machine.floats(OUTPUT, frames))
            cpu.call(0x47ED4 if profile == 'playback_splices' else 0x47FE4, stop_before=0x48454)
            heads = render_model['heads']
            expected_raw = render_model['raw']
            expected_filtered = reference(expected_raw, coefficient(cutoff), state)
            expected_mix = [libm.fmaf(a, gain, b) for a, b in zip(expected_filtered, prior)]
            check('raw', expected_raw, observed['raw'])
            check('filtered', expected_filtered, observed['filtered'])
            check('mix', expected_mix, cpu.floats(MIX, frames))
            check('filter_state', state[0] + state[1], [cpu.getf(AA + o) for o in (4, 8, 16, 20)])
            assert cpu.floats(TAPE, LENGTH) == before_original
            observed_mix = cpu.floats(MIX, frames)
            assertions += 1
            if args.connected:
                expected_color = {stage: [] for stage in ('color_filter', 'wear', 'diffuser', 'plate', 'clipper')}
                for sample in expected_mix:
                    x = tape_filter.process(sample, age)
                    expected_color['color_filter'].append(x)
                    x = wear(x, wear_amount)
                    expected_color['wear'].append(x)
                    x = diffuser.process(x, hysteresis)
                    expected_color['diffuser'].append(x)
                    x = plate.process(x)
                    expected_color['plate'].append(x)
                    x = clipper(x, output_knee, output_compensation)
                    expected_color['clipper'].append(x)
                for address, stage in ((0x48468, 'color_filter'), (0x48478, 'wear'), (0x48488, 'diffuser'), (0x484BC, 'plate'), (0x484D8, 'clipper')):
                    cpu.observers[address] = lambda machine, name=stage: observed.update({name: machine.floats(MIX, frames)})
            # Independently predict sequential writes. Each head gathers old
            # samples from the tape LEFT BY THE PREVIOUS HEAD, then scatters.
            written_sets, expected_phases = [], []
            for head in records:
                count = abs(cpu.gets(head + 4) - cpu.gets(head + 12))
                effective = f32(factor * (cpu.getf(head + 24) if cpu.read(head + 20, 1)[0] else speed))
                inverse = f32(1. / abs(effective)) if abs(effective) >= f32(1e-6) else 0.
                direction = 1 if effective > 0 else -1 if effective < 0 else 0
                indices = [cpu.gets(head + 12) + direction * i for i in range(count)]
                assert all(0 <= index < LENGTH for index in indices)
                fade = fade_curve(cpu, head, count) if count else []
                gathered = []
                for i, index in enumerate(indices):
                    source = max(0, min(len(buffer) - 4, math.trunc(phase)))
                    sample = rounded_cubic(*buffer[source:source + 4], f32(phase - source))
                    contribution = f32(sample * fade[i])
                    gathered.append(clip(libm.fmaf(tape[index], feedback, contribution), knee, compensation))
                    phase = f32(phase + inverse)
                if not count:
                    phase = f32(phase + inverse)
                    no_write_calls += 1
                else:
                    write_calls += 1
                phase = f32(phase - math.trunc(phase))
                for index, value in zip(indices, gathered):
                    tape[index] = value
                written_sets.append(set(indices))
                expected_phases.append(phase)
            if len(written_sets) == 2 and written_sets[0] & written_sets[1]:
                overlap_blocks += 1
            # Deliberately compute the counterfactual render after writes.
            after_raw = raw_model(cpu, heads, records, c, tape)
            if max(abs(a - b) for a, b in zip(after_raw, expected_raw)) > 8e-7:
                order_sensitive_blocks += 1
            visited, observed_phases = [], []
            cpu.observers[0x47818] = lambda machine: visited.append(machine.reg(2))
            cpu.observers[0x4851C] = lambda machine: observed_phases.append(machine.getf(INPUT + 16)) if len(observed_phases) < len(visited) else None
            # r6 points to Channel + 4096 at the original callback write-fade tests.
            cpu.reg(6, CHANNEL + 4096)
            cpu.call(0x48454 if args.connected else 0x484E4, stop_before=0x48554)
            assert visited == records
            assertions += 1
            check('phase', expected_phases, observed_phases)
            check('tape', tape, cpu.floats(TAPE, LENGTH))
            if args.connected:
                for stage in expected_color:
                    check(stage, expected_color[stage], observed[stage])
                check('color_state', [v for s in tape_filter.state for v in s] +
                      [v for o in FILTERS for v in plate.filter_state[o]] + [plate.phase[o] for o in MODULATORS],
                      [cpu.getf(color_filter + 8 + i * 12 + j * 4) for i in range(5) for j in range(2)] +
                      [cpu.getf(PLATE + o + 8 + j * 4) for o in FILTERS for j in range(2)] +
                      [cpu.getf(PLATE + o + 28) for o in MODULATORS])
                assert diffuser.indices == [cpu.getu(color_diffuser + 0x1774 + i * 4) for i in range(4)]
                assert cpu.floats(MIX, frames) == observed['clipper']
            else:
                assert cpu.floats(MIX, frames) == observed_mix
            assertions += 1
            history.append(dict(block=block, speed=speed, factor=factor, record_heads=len(records),
                                playback_heads=len(heads),
                                write_counts=[len(s) for s in written_sets], phase=phase,
                                changed_cells=sum(a != b for a, b in zip(before_tape, tape))))
            if args.input_chain:
                history[-1].update(input_preset=preset_name, fixed_input_cutoff=fixed_input_cutoff,
                                   input_cutoff=input_cutoff, input_age=age, input_wear=wear_amount,
                                   input_hysteresis=hysteresis)
            previous_speed = speed
        coverage.update(cpu.coverage)
        rows.append(dict(frames=frames, profile=profile, feedback=feedback,
                         knee=knee, compensation=compensation, blocks=history))
    if any(row['profile'] == 'two_overlap' for row in rows):
        assert overlap_blocks > 0
    assert order_sensitive_blocks > 0 and no_write_calls > 0
    result = dict(status='PASS', sequences=len(rows), blocks=sum(len(r['blocks']) for r in rows),
                  comparisons=comparisons, assertions=assertions, max_abs_errors=errors,
                  events=dict(write_calls=write_calls, no_write_calls=no_write_calls,
                              overlapping_record_blocks=overlap_blocks, order_sensitive_blocks=order_sensitive_blocks,
                              splice_profile_multihead_blocks=sum(b['playback_heads'] > 1 for r in rows if r['profile'] == 'playback_splices' for b in r['blocks'])),
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(coverage), coverage_addresses=[hex(a) for a in sorted(coverage)],
                  fixtures=rows, limitations=[
                      ('Playback slice through 0x48454 then connected coloration/write/update through 0x48554; observation stop at mix boundary, same CPU/tape/effect state.' if args.connected else 'Playback slice through 0x48454 then write/update slice 0x484e4..0x48554; intervening output coloration skipped.'),
                      'Original input-history and motion routines explicitly invoked, not full upstream callback.',
                      'Playback_splices profile executes one ordinary playback boundary manager with fixture region locals; recording manager boundaries are omitted.',
                      'No moving channel write fade, first-record tail scheduling or control events.',
                      'Initial tape/head/filter/clipper/control states are fixtures; shared reconstructed fade table.',
                      'Arithmetic models read original moved head/fade state, not independent full transport/event model.',
                      'No hardware, analog circuitry, concurrency or full Channel callback comparison.'])
    result['connected_output_coloration'] = args.connected
    result['cycles'] = args.cycles
    result['native_output_clipper'] = args.native_output_clipper
    result['connected_input_chain'] = args.input_chain
    if args.connected:
        result['limitations'] += ['Preview disabled on both decks; upstream coloration/preset producer fixtures; plate floor service explicit.',
                                  'Coloration receives independently predicted pre-write playback mix; record input history remains separate.']
    if args.input_chain:
        result['limitations'] = [s for s in result['limitations'] if ('input-history' not in s or 'motion' not in s) and 'record input history remains separate' not in s]
        result['limitations'] += ['Input slice 0x47cc0..0x47df4 joins cutoff dispatch, coloration, AntiAlias and history; routing/gating flags disabled and motion still explicitly called.',
                                  'Input preset filter publication executed; effect amounts and fixed-input-cutoff flag are fixtures, not complete preset/control producers.',
                                  'Explicit host tanf/floorf services, no original ARM libm equivalence.']
    if not args.quick or args.native_output_clipper:
        name = ('connected_input_output_long_record_pipeline_probe_results.json' if args.input_chain and args.quick else
                'connected_input_output_record_pipeline_probe_results.json' if args.input_chain else
                'connected_native_output_record_pipeline_probe_results.json' if args.native_output_clipper else
                'connected_record_pipeline_probe_results.json' if args.connected else 'record_pipeline_probe_results.json')
        (ROOT / 'probes' / name).write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'sequences', 'blocks', 'comparisons', 'assertions', 'max_abs_errors', 'events', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
