#!/usr/bin/env python3
"""Independent trigger/move/update fade state joined to original gain rendering."""
import itertools
import json
import math
import struct
from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_splice_render import TABLE, fade_curve

TAP, HEADER, DATA = 0x150000, 0x160000, 0x170000
EPSILON = struct.unpack('<f', struct.pack('<I', 0xB58637BD))[0]


def reset():
    return [0, 0, 0., 0, 0., 1.]


def advance(state, amount):
    state[3:5] = state[1:3]
    part = libm.fmaf(state[5], f32(amount), state[2])
    # Binary32 unit normalization, also used by the firmware for large offsets.
    while part >= 1.:
        state[1] += 1
        part = f32(part-1.)
    while part < 0.:
        state[1] -= 1
        part = f32(part+1.)
    state[2] = part


def triggered(previous, pf, current, cf, target, duration, rising, forward):
    crossing = min(previous, current) <= target <= max(previous, current)
    step = f32(256./f32(duration))
    if rising != forward:
        step = -step
    offset = math.trunc(f32(f32(target-previous)*step))
    start = -offset if rising and crossing else 0 if rising else 255-offset if crossing else 255
    s = [1, start, 0., start, 0., step if rising else 1.]
    if not rising:
        advance(s, EPSILON)
        advance(s, 0.)
        s[5] = step
    if crossing:
        delta = f32(f32(f32(current)+cf)-f32(f32(previous)+pf))
        advance(s, delta)
    return s


class ModelView:
    """Expose only independent states to the existing interpolation reference."""
    def __init__(self, states):
        self.values = {}
        for kind, state in enumerate(states):
            for offset, value in zip((0, 4, 8, 12, 16, 20), state):
                self.values[TAP+28+kind*24+offset] = value

    def read(self, address, size):
        assert size == 1
        return bytes([self.values[address]])

    def gets(self, address):
        return self.values[address]

    def getf(self, address):
        return self.values[address]


def check(c, states):
    for kind, want in enumerate(states):
        b = TAP+28+kind*24
        got = [c.read(b, 1)[0], c.gets(b+4), c.getf(b+8), c.gets(b+12), c.getf(b+16), c.getf(b+20)]
        assert got == want, (kind, want, got)


def main():
    cases, blocks, checks, coverage, errors, lower, upper = 0, 0, 0, set(), 0., 0, 0
    renders, stationary_outside = 0, 0
    for kind, direction, fractions, offset, rising, forward, duration in itertools.product(
            range(4), (-1, 1), ((0., 0.), (.125, .75)), (-12, 0, 5, 10, 12),
            range(2), range(2), (32, 128, 1000)):
        c = ARMBytes()
        c.reg(0, TAP)
        c.call(0x4BB88)
        c.reg(0, TAP)
        c.reg(1, 1000)
        c.call(0x4DF18)
        previous, current = 1000, 1000+direction*10
        pf, cf = fractions
        target = 1000+direction*offset
        for address, value in ((TAP+4, current), (TAP+12, previous)):
            c.puts(address, value)
        c.putf(TAP+8, cf)
        c.putf(TAP+16, pf)
        c.reg(0, TAP)
        c.reg(1, kind)
        c.reg(2, target)
        c.reg(3, duration)
        c.putu(STACK, rising)
        c.putu(STACK+4, forward)
        c.call(0x4E46C)
        states = [reset() for _ in range(4)]
        states[kind] = triggered(previous, pf, current, cf, target, duration, rising, forward)
        check(c, states)
        checks += 24
        c.write(0x93F98, struct.pack('<256f', *TABLE))
        live = True
        head_position, head_fraction = current, cf
        for block, speed in enumerate((0., float(direction), -float(direction), .5*direction, 2.*direction, 0., float(direction))):
            before_position, before_fraction = head_position, head_fraction
            if live:
                c.reg(0, TAP)
                c.fp(0, speed)
                c.fp(1, 1.)
                c.fp(2, 32.)
                c.call(0x4BDE0)
                delta = f32(speed*32.)
                part = f32(head_fraction+delta)
                whole = math.floor(part)
                head_position += whole
                head_fraction = f32(part-whole)
                for state in states:
                    if state[0]:
                        advance(state, delta)
                check(c, states)
                assert c.gets(TAP+4) == head_position and c.getf(TAP+8) == head_fraction
                assert c.gets(TAP+12) == before_position and c.getf(TAP+16) == before_fraction
                outside = any(s[0] and abs(f32(f32(s[1]+s[2])-f32(s[3]+s[4]))) < f32(.1)
                              and not 0 <= s[1] <= 255 for s in states)
                if outside:
                    stationary_outside += 1
                else:
                    expected = fade_curve(ModelView(states), TAP, 32)
                    c.vector(HEADER, DATA, [1.]*32)
                    c.reg(0, TAP)
                    c.reg(1, HEADER)
                    c.reg(2, 32)
                    c.call(0x4BF7C)
                    for want, got in zip(expected, c.floats(DATA, 32)):
                        error = abs(want-got)
                        assert math.isfinite(got) and error <= 3e-7, (kind,direction,offset,rising,forward,duration,block,want,got)
                        errors = max(errors, error)
                    renders += 1
                    checks += 32
                checks += 28
            c.reg(0, TAP)
            c.call(0x4BFA0)
            if any(state[0] and state[1] < 0 for state in states):
                states = [reset() for _ in range(4)]
                live, head_position, head_fraction = False, 0, 0.
                lower += 1
            else:
                for k, state in enumerate(states):
                    if state[0] and state[1] > 254:
                        states[k] = reset()
                        upper += 1
            check(c, states)
            assert c.read(TAP, 1)[0] == int(live)
            checks += 25
            blocks += 1
        coverage.update(c.coverage)
        cases += 1
    result = dict(status='PASS', trigger_cases=cases, move_render_update_blocks=blocks,
                  gain_render_calls=renders, stationary_outside_render_omissions=stationary_outside,
                  comparisons=checks, max_abs_error=errors, lower_head_resets=lower,
                  upper_fade_resets=upper, distinct_instruction_addresses=len(coverage),
                  limitations=['Complete original inactive trigger_fade, Tap.move, get_xfade and Tap.update; independent state and interpolation models.',
                               'All four kinds, direction booleans, integer interval endpoints and fractional motion tested; supplied triggers/scheduling.',
                               'Stationary out-of-table render omitted explicitly; state movement/update still checked. No already-active trigger diagnostic path, zero duration, extreme coordinates, simultaneous fades, manager allocation or whole audio callback.'])
    (ROOT/'probes/tap_fade_state_probe_results.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
