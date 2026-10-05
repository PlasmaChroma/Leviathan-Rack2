"""Independent rounded normalized marker and speed-table construction."""
import math
import struct
from arm_leaf_probe import f32, libm


def normalized_markers(markers):
    return sorted(set([0.] + [f32(sign * m) for m in markers for sign in (-1., 1.)]))


def speed_table(markers, mode, source):
    if mode == 3:
        return list(source)
    if mode in (1, 2):
        step = struct.unpack('<f', struct.pack('<I', 0x3B000801))[0]
        value, output = -4., []
        for _ in range(4096):
            output.append(value)
            value = f32(value + step)
        return output
    assert mode == 0
    notches = [[-4., 0, 0, 0]]
    for marker in normalized_markers(markers):
        position = libm.fmaf(f32(f32(marker + 4.) * .125), 4095., 0.)
        center = math.trunc(position)
        notches.append([marker, max(0, min(4095, center - 50)), center, max(0, min(4095, center + 50))])
    notches.append([4., 4096, 4096, 4096])
    notches.sort(key=lambda n: n[0])
    for a, b in zip(notches, notches[1:]):
        if a[3] > b[1]:
            half = math.trunc((b[2] - a[2]) / 2)
            a[3], b[1] = a[2] + half, b[2] - half
    output = [0.] * 4096
    for a, b in zip(notches, notches[1:]):
        for i in range(a[1], a[3]):
            output[i] = a[0]
        length = b[1] - a[3]
        for j in range(length):
            output[a[3] + j] = libm.fmaf(f32(j / length), f32(b[0] - a[0]), a[0])
    return output
