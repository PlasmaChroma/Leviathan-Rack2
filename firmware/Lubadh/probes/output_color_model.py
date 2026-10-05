"""Independent rounded output TapeFilter/wear/diffuser/clipper equations."""
import math

from arm_leaf_probe import f32, libm


class TapeFilterModel:
    def __init__(self, coefficients):
        self.coefficients = list(coefficients)
        self.state = [[0., 0.] for _ in range(5)]

    def pole(self, i, sample, high=False):
        px, py = self.state[i]
        c = self.coefficients[i]
        low = f32(libm.fmaf(-py, f32(1. - c), f32(sample + px)) / f32(1. + c))
        self.state[i] = [sample, low]
        return f32(sample - low) if high else low

    def process(self, sample, amount):
        high_a, high_b = self.pole(0, sample, True), self.pole(1, sample, True)
        high = f32(libm.fma(high_a, .6, high_b * .6))
        low_a, low_b = self.pole(2, high), self.pole(3, high)
        low_c = self.pole(4, f32(sample * .2))
        wet = f32(libm.fma(low_a, .6, low_b * .6) + low_c)
        return libm.fmaf(amount, f32(wet - sample), sample)


class DiffuserModel:
    def __init__(self):
        self.buffers = [[0.] * n for n in (68, 159, 251, 375)]
        self.indices = [0] * 4

    def process(self, sample, amount):
        d = [b[p] for b, p in zip(self.buffers, self.indices)]
        summed = sample
        for value in d:
            summed = f32(summed + value)
        difference = libm.fmaf(summed, f32(.175), -sample)
        result = libm.fmaf(amount, difference, sample)
        writes = [sample, f32(sample - d[0]), f32(f32(sample + d[0]) - d[1]), f32(f32(f32(sample + d[0]) + d[1]) - d[2])]
        for i, value in enumerate(writes):
            self.buffers[i][self.indices[i]] = value
            self.indices[i] = (self.indices[i] + 1) % len(self.buffers[i])
        return result


def wear(sample, amount):
    shaped = math.copysign(f32(sample * sample), sample)
    return libm.fmaf(amount, f32(shaped - sample), sample)


def clipper(sample, knee, compensation):
    if knee <= .01:
        sample = f32(sample * f32(1. + compensation))
        square = f32(sample * sample)
        numerator = f32(f32(square + f32(28.274333953857422)) * sample)
        denominator = libm.fmaf(square, f32(9.42477798461914), f32(28.274333953857422))
        return max(-1., min(1., f32(numerator / denominator)))
    gain = libm.fmaf(compensation, f32(f32(2. / f32(1. + knee)) - 1.), 1.)
    sample = max(-1., min(1., sample))
    delta = f32(abs(sample) - knee)
    if delta > 0.:
        ratio = f32(delta / f32(1. - knee))
        sample = math.copysign(f32(knee + f32(delta / libm.fmaf(ratio, ratio, 1.))), sample)
    return f32(sample * gain)
