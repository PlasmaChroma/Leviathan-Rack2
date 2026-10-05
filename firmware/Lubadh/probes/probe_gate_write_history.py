#!/usr/bin/env python3
"""Real gate/state/head/write/expiry/update histories with independent fresh-head state."""
import itertools
import json
import math
import struct
from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_channel_set_state import fixture, FADE
from probe_speed_consumers import CHANNEL, LINK, PRESET
from probe_splice_render import TABLE
from probe_first_record_tail_writes import (INPUT, INPUT_DATA, CONTRIBUTION, INDICES,
    FEEDBACK, RAW_HEADER, RAW_DATA, TAPE, TAPE_LENGTH)

HEAD = CHANNEL + 3688


def check_fresh_fades(c):
    for kind in range(4):
        b = HEAD + 28 + kind*24
        assert c.read(b, 1)[0] == 0
        assert [c.gets(b+4), c.getf(b+8), c.gets(b+12), c.getf(b+16), c.getf(b+20)] == [0, 0., 0, 0., 1.]


def envelope(active, previous, current, frames, feedback):
    if not active:
        return [1.] * frames, [feedback] * frames
    pi, pf = previous
    ci, cf = current
    delta = f32(f32(ci + cf) - f32(pi + pf))
    step = f32(delta / frames)
    prefix = frames
    if ci < 0 or ci > 254:
        distance = f32(-f32(ci + cf)) if ci < 0 else f32(f32(ci + cf) - 254.)
        removed = min(math.trunc(f32(distance / f32(abs(delta) / frames))) + 1, frames)
        prefix = frames - removed if removed >= 0 else frames
    incoming, retention = [], []
    for i in range(frames):
        if i < prefix:
            assert 0 <= pi <= 254
            incoming.append(libm.fmaf(pf, f32(TABLE[pi+1] - TABLE[pi]), TABLE[pi]))
            reverse = libm.fmaf(f32(1.-pf), f32(TABLE[254-pi] - TABLE[255-pi]), TABLE[255-pi])
            retention.append(libm.fmaf(f32(1.-feedback), reverse, feedback))
            part = f32(pf + step)
            whole = math.floor(part)
            pi, pf = pi + whole, f32(part - whole)
        else:
            incoming.append(0. if ci < 0 else 1.)
            retention.append(1. if ci < 0 else feedback)
    return incoming, retention


def main():
    sequences, blocks, checks, writes, coverage, max_error, traces = 0, 0, 0, 0, set(), 0., []
    for speed, duration, frames, early, feedback in itertools.product(
            (-1., 1.), (128, 1000), (32, 128), (False, True), (0., .5, 1.)):
        c = fixture(0, 0, speed, 0, 1, 0, 0, duration)
        for offset, method, label in ((608, 0x3A4EC, 2), (620, 0x3A5A0, 3)):
            table = 0x1A0000 + label * 16
            for address, value in ((CHANNEL+offset, table), (CHANNEL+offset+4, label),
                                   (CHANNEL+offset+8, CHANNEL), (table, method)):
                c.putu(address, value)
        c.putu(CHANNEL+632, CHANNEL+620)
        c.putu(PRESET+4, 1)
        c.putf(LINK+164, feedback)
        c.putu(CHANNEL+171972, TAPE)
        for header, data, integer in ((36, CONTRIBUTION, False), (48, INDICES, True), (60, FEEDBACK, False)):
            c.vector(CHANNEL+172032+header, data, [0]*frames, integers=integer)
        c.vector(INPUT+4, INPUT_DATA, [0.]*(frames+4))
        c.reg(0, CHANNEL+25160)
        c.fp(0, 1.)
        c.fp(1, 0.)
        c.call(0x4FFE0)
        c.write(0x93F98, struct.pack('<256f', *TABLE))
        tape = [f32(((i*11)%67-33)/256.) for i in range(TAPE_LENGTH)]
        c.write(TAPE, struct.pack('<%df' % TAPE_LENGTH, *tape))
        records = []
        def observe_record(m):
            check_fresh_fades(m)
            records.append(m.reg(2))
        c.observers[0x47818] = observe_record
        def stop(m):
            m.returned = True
            m.cpu.emu_stop()
        c.observers[0x48554] = stop
        c.reg(0, CHANNEL)
        c.reg(1, 4)
        c.call(0x3F1C4)
        assert c.getu(CHANNEL+632) == CHANNEL+608 and c.gets(HEAD+4) == 3456
        check_fresh_fades(c)
        pos, frac, phase_active, phase_step = 0, 0., True, f32(256./duration)
        head_position, buffer, live = 3456, [0.]*(frames+4), True
        release_block = 1 if early else math.ceil(duration/frames)+2
        trace = []
        for block in range(100):
            if block == release_block:
                c.reg(0, CHANNEL)
                c.reg(1, 5)
                c.call(0x3F1C4)
                assert c.getu(CHANNEL+632) == CHANNEL+620
                if not phase_active:
                    pos, frac = 254, 0.
                phase_active, phase_step = True, f32(-256./duration)
            samples = [f32(((block*frames+i)*7%101-50)/512.) for i in range(frames)]
            buffer = buffer[-4:] + samples
            c.vector(RAW_HEADER, RAW_DATA, samples)
            c.reg(0, INPUT)
            c.reg(1, RAW_HEADER)
            c.call(0x38A78)
            assert c.floats(INPUT_DATA, frames+4) == buffer
            previous = (pos, frac)
            part = libm.fmaf(phase_step, f32(frames), frac)
            whole = math.floor(part)
            pos, frac = pos+whole, f32(part-whole)
            gain, keep = envelope(phase_active, previous, (pos, frac), frames, feedback)
            start = head_position
            if live:
                c.reg(0, HEAD)
                c.fp(0, speed)
                c.fp(1, 1.)
                c.fp(2, frames)
                c.call(0x4BDE0)
                head_position += int(speed*frames)
                assert c.gets(HEAD+4) == head_position and c.gets(HEAD+12) == start
            records.clear()
            c.reg(4, CHANNEL)
            c.reg(6, CHANNEL+4096)
            c.fp(16, frames)
            c.fp(17, 1.)
            c.fp(21, 1.)
            for offset, value in ((0x20, CHANNEL+872), (0x30, HEAD), (0x3C, 0), (0x58, INPUT)):
                c.putu(STACK+offset, value)
            c.call(0x48C58)
            assert len(records) == int(live)
            if live:
                assert records[0] == HEAD
                indices = [start+int(speed)*i for i in range(frames)]
                assert [c.gets(INDICES+i*4) for i in range(frames)] == indices
                for i, index in enumerate(indices):
                    contribution = f32(buffer[i+1]*gain[i])
                    tape[index] = max(-1., min(1., libm.fmaf(tape[index], keep[i], contribution)))
                writes += frames
            low, high = phase_active and pos < 0, phase_active and pos > 254
            if low or high:
                phase_active, pos, frac, phase_step = False, 0, 0., 1.
            if low:
                live = False
            assert c.read(FADE, 1)[0] == int(phase_active)
            assert c.gets(FADE+4) == pos and c.getf(FADE+8) == frac, (block, c.gets(FADE+4), c.getf(FADE+8), pos, frac)
            assert c.read(HEAD, 1)[0] == int(live)
            check_fresh_fades(c)
            assert c.getf(INPUT+16) == 0.
            observed = c.floats(TAPE, TAPE_LENGTH)
            for a, b in zip(tape, observed):
                error = abs(a-b)
                assert math.isfinite(b) and error <= 3e-7, (speed,duration,frames,early,feedback,block,a,b)
                max_error = max(max_error, error)
            checks += TAPE_LENGTH+frames+32+(24 if records else 0)
            blocks += 1
            if low or high or block == release_block:
                trace.append(dict(block=block, released=block==release_block, lower_expiry=bool(low),
                                  upper_expiry=bool(high), record_head=live))
            # Verify the first callback after reset does not write.
            if not live and block > release_block and not records:
                break
        else:
            raise AssertionError('Gate release failed to finish')
        sequences += 1
        coverage.update(c.coverage)
        traces.append(dict(speed=speed,duration=duration,frames=frames,early=early,feedback=feedback,events=trace))
    result = dict(status='PASS', sequences=sequences, callback_slices=blocks, tape_writes=writes,
                  comparisons=checks, max_abs_error=max_error, distinct_instruction_addresses=len(coverage), traces=traces,
                  limitations=['Complete original gate events/setState, head motion/input history, callback fade/write/expiry and both manager updates; supplied scheduling.',
                               'Fresh-head fade flags, coordinates and steps are independently predicted and checked at activation, dispatch and post-update; no state oracle remains.',
                               'Single fresh record head, unit forward/reverse speed, looping preset, finite tape, transparent clipper; no GPIO/link aliases, effects, boundary retriggers or pool exhaustion.'])
    (ROOT/'probes/gate_write_history_probe_results.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='traces'}, indent=2))


if __name__ == '__main__':
    main()
