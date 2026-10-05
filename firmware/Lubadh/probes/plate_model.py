"""Independently expressed MonoPlate recurrence for offline comparison.

Numeric coefficients/tables are supplied as explicit fixture parameters. No
instruction dispatch or original output/state is consulted during processing.
"""
import math

from arm_leaf_probe import f32, libm
from probe_playback_pipeline import rounded_cubic

DESCRIPTORS = (0xF58, 0x1330, 0x1604, 0x1FD8, 0x270C, 0x3054,
               0x9ED4, 0xCD34, 0x11DDC, 0x12410, 0x198A4, 0x1DD28, 0x23CD0)
LENGTHS = (980, 233, 175, 623, 455, 588, 6933, 2959, 5156, 392, 7322, 4376, 6116)
FILTERS = (0xF74, 0xF64, 0x9EE0, 0x198B0)
MODULATORS = (0x3054, 0x12410)
DRY, WET, DECAY = 0x23CE8, 0x23CEC, 0x23CDC
FEEDBACK = (0x23CF0, 0x23CF4)


def modulation_table():
    table = []
    for i in range(128):
        x = f32(f32(i) / 127.)
        table.append(f32(f32(x - .75) * 2.) if x >= .75 else libm.fmaf(-abs(f32(x - .25)), 2., 1.))
    return table


class PlateModel:
    def __init__(self, parameters, modes, tables):
        self.params = dict(parameters)
        self.modes = dict(modes)
        self.tables = {o: list(t) for o, t in tables.items()}
        self.buffers = {o: [0.] * n for o, n in zip(DESCRIPTORS, LENGTHS)}
        self.indices = {o: 0 for o in DESCRIPTORS}
        self.filter_state = {o: [0., 0.] for o in FILTERS}
        self.phase = {o: 0. for o in MODULATORS}
        self.feedback = [0., 0.]

    def filter(self, offset, sample):
        mode = self.modes[offset]
        if mode not in (0, 1, 2):
            return sample
        coefficient = self.params[offset + 4]
        px, py = self.filter_state[offset]
        low = f32(libm.fmaf(-py, f32(1. - coefficient), f32(sample + px)) / f32(1. + coefficient))
        self.filter_state[offset] = [sample, low]
        if mode == 0:
            return low
        high = f32(sample - low)
        return high if mode == 1 else f32(low - high)

    def peek(self, offset, delay=0):
        return self.buffers[offset][(self.indices[offset] - delay) % len(self.buffers[offset])]

    def push(self, offset, value):
        self.buffers[offset][self.indices[offset]] = value
        self.indices[offset] = (self.indices[offset] + 1) % len(self.buffers[offset])

    def allpass(self, offset, sample):
        delayed = self.peek(offset)
        coefficient = self.params[offset + 12]
        written = libm.fmaf(coefficient, delayed, sample)
        self.push(offset, written)
        return libm.fmaf(-coefficient, written, delayed)

    def modulated_allpass(self, offset, sample):
        phase = f32(self.phase[offset] + self.params[offset + 24])
        phase = f32(phase - math.floor(phase))
        self.phase[offset] = phase
        table_position = f32(phase * 127.)
        table_index = math.trunc(table_position)
        fraction = f32(table_position - table_index)
        table = self.tables[offset]
        amount = libm.fmaf(fraction, f32(table[table_index + 1] - table[table_index]), table[table_index])
        amount = f32(amount * self.params[offset + 16])
        buffer = self.buffers[offset]
        if amount > len(buffer) - 1:
            amount = f32(amount - f32(len(buffer) - 1))
        delay = f32(f32(len(buffer)) - amount)
        position = f32(f32(self.indices[offset]) - delay)
        integer = math.trunc(position)
        fraction = f32(position - integer)
        delayed = rounded_cubic(*(buffer[(integer + i) % len(buffer)] for i in (-1, 0, 1, 2)), fraction)
        coefficient = self.params[offset + 12]
        written = libm.fmaf(coefficient, delayed, sample)
        self.push(offset, written)
        return libm.fmaf(-coefficient, written, delayed)

    def process(self, sample):
        sample = self.filter(0xF74, sample)
        taps = [self.peek(0x9ED4, 437), f32(self.peek(0x9ED4, 4889) * .125),
                f32(self.peek(0xCD34, 2655) * f32(.3)), f32(self.peek(0x11DDC, 3282) * f32(.2)),
                f32(self.peek(0x198A4, 3272) * .25), f32(self.peek(0x1DD28, 307) * f32(.8752999901771545)),
                f32(self.peek(0x23CD0, 1753) * .5)]
        delayed = self.filter(0xF64, self.peek(0xF58))
        self.push(0xF58, sample)
        for offset in (0x1330, 0x1604, 0x1FD8, 0x270C):
            delayed = self.allpass(offset, delayed)
        decay = self.params[DECAY]
        self.feedback = [f32(self.peek(0x11DDC) * decay), f32(self.peek(0x23CD0) * decay)]
        tank_a = self.allpass(0xCD34, self.filter(0x9EE0, self.peek(0x9ED4)))
        self.push(0x11DDC, tank_a)
        tank_b = self.allpass(0x1DD28, self.filter(0x198B0, self.peek(0x198A4)))
        self.push(0x23CD0, tank_b)
        branch_a = self.modulated_allpass(0x3054, f32(delayed + self.feedback[1]))
        self.push(0x9ED4, branch_a)
        branch_b = self.modulated_allpass(0x12410, f32(delayed + self.feedback[0]))
        self.push(0x198A4, branch_b)
        summed = f32(taps[1] + taps[0])
        for sign, tap in zip((-1, 1, -1, -1, -1), taps[2:]):
            summed = f32(summed + sign * tap)
        mixed = libm.fmaf(self.params[DRY], sample, f32(summed * self.params[WET]))
        square = f32(mixed * mixed)
        numerator = f32(f32(square + f32(28.274333953857422)) * mixed)
        denominator = libm.fmaf(square, f32(9.42477798461914), f32(28.274333953857422))
        return max(-1., min(1., f32(numerator / denominator)))
