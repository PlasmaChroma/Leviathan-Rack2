#!/usr/bin/env python3
"""Force original uniform endpoint rounding with inverse-tempered MT fixture words."""
import json
from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from flutter_factory import FactoryBytes, factory_coefficients
from probe_flutter_state import BASE, NOISE, Filter, OUT_BASE, OUT_SCALE


def undo_right(value, shift):
    result = value
    for _ in range(32):
        result = value^(result>>shift)
    return result


def undo_left(value, shift, mask):
    result = value
    for _ in range(32):
        result = value^((result<<shift)&mask)
    return result


def untemper(value):
    value = undo_right(value,18)
    value = undo_left(value,15,0xefc60000)
    value = undo_left(value,7,0x9d2c5680)
    return undo_right(value,11)


def main():
    cases, coverage = [], set()
    for desired in (0,0xffffff00,0xffffff7f,0xffffff80,0xffffffff):
        c = FactoryBytes(5489)
        c.reg(0,BASE)
        c.call(0x4F1C0)
        # Deliberate MT-state boundary fixture, not a claim about seed frequency.
        c.putu(BASE+3648,untemper(desired))
        c.putu(BASE+6144,0)
        c.putf(BASE+24,0.)
        c.putf(BASE+28,1.)
        c.vector(BASE+6156,NOISE,[0.])
        branch = []
        c.observers[0x50474] = lambda m: branch.append('upper fallback')
        u = f32(f32(desired)*f32(2.**-32))
        rounded_upper = u >= 1.
        if rounded_upper:
            u = f32(1.-2.**-24)
        raw = f32(libm.fmaf(u,2.,-1.)-.5)
        filtered = raw
        for a,b in factory_coefficients():
            filtered = Filter(a,b).process(filtered)
        expected = libm.fmaf(f32(f32(filtered+2.5)/5.),OUT_SCALE,OUT_BASE)
        c.reg(0,BASE)
        c.reg(1,32)
        c.fp(0,1.)
        c.call(0x502C4)
        assert c.fp(0) == expected and c.getf(NOISE) == filtered
        assert branch == (['upper fallback'] if rounded_upper else [])
        assert c.getu(BASE+6144) == 1
        coverage.update(c.coverage)
        cases.append(dict(random_word=hex(desired),upper_fallback=rounded_upper,raw=raw,filtered=filtered,factor=expected))
    result = dict(status='PASS',cases=cases,distinct_instruction_addresses=len(coverage),
                  limitations=['Complete original constructor/scalar process/MT tempering and uniform endpoint branch; host-libm and explicit entropy services.',
                               'Inverse-tempered state words deliberately supplied; tests branch arithmetic, not probability or entropy quality.'])
    (ROOT/'probes/flutter_endpoint_probe_results.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))


if __name__ == '__main__':
    main()
