"""Independent rounded high-pass biquad and input low-pass reference."""
import ctypes
import math

from arm_leaf_probe import f32, libm

libm.tanf.argtypes = [ctypes.c_float]
libm.tanf.restype = ctypes.c_float
RATE = f32(49170.25390625)


def highpass_coefficients(frequency, quality):
    frequency, quality = f32(frequency), max(f32(.2), min(f32(.8), f32(quality)))
    k = 0. if frequency <= 0. else f32(8.742277657347586e-8) if frequency >= 1. else libm.tanf(f32(frequency * math.pi))
    ratio = f32(k / quality)
    norm = f32(1. / libm.fmaf(k, k, f32(ratio + 1.)))
    a1 = f32(f32(libm.fmaf(k, k, -1.) * 2.) * norm)
    a2 = f32(libm.fmaf(k, k, f32(1. - ratio)) * norm)
    return [1., a1, a2], [norm, f32(norm * -2.), norm]


class InputFilterModel:
    def __init__(self, a, b, low_coefficient):
        self.a, self.b, self.c = list(a), list(b), low_coefficient
        self.state = [0., 0., 0.]
        self.low_state = [0., 0.]

    def process(self, sample):
        _, v1, v2 = self.state
        v0 = libm.fmaf(-self.a[1], v1, sample)
        v0 = libm.fmaf(-self.a[2], v2, v0)
        out = libm.fmaf(v0, self.b[0], f32(v1 * self.b[1]))
        out = libm.fmaf(self.b[2], v2, out)
        self.state = [v0, v0, v1]
        px, py = self.low_state
        low = f32(libm.fmaf(-py, f32(1. - self.c), f32(out + px)) / f32(1. + self.c))
        self.low_state = [out, low]
        return low


def preset_coefficients(low, quality, high):
    low = max(20., min(20000., f32(low)))
    high = max(20., min(20000., f32(high)))
    a, b = highpass_coefficients(f32(low / RATE), quality)
    c = f32(1. / (math.pi * f32(high / RATE)))
    return a, b, c
