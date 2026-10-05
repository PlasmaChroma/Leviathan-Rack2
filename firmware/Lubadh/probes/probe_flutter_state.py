#!/usr/bin/env python3
"""Persistent original scalar flutter process with independent phase/RNG/filter model."""
import itertools
import argparse
import json
import math
import struct
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32, libm

BASE, NOISE = 0x150000, 0x170000
RATE = struct.unpack('<f', struct.pack('<I', 0x47401241))[0]
OUT_SCALE = struct.unpack('<f', struct.pack('<I', 0x3DAAAB00))[0]
OUT_BASE = struct.unpack('<f', struct.pack('<I', 0x3F755550))[0]


class MT:
    def __init__(self, seed):
        self.values = [seed]
        for i in range(1,624):
            v = self.values[-1]
            self.values.append((1812433253*(v^(v>>30))+i)&0xffffffff)
        self.index = 624

    def next(self):
        if self.index >= 624:
            for i in range(624):
                y = (self.values[i]&0x80000000)|(self.values[(i+1)%624]&0x7fffffff)
                self.values[i] = self.values[(i+397)%624]^(y>>1)^(0x9908b0df if y&1 else 0)
            self.index = 0
        y = self.values[self.index]
        self.index += 1
        y ^= y>>11
        y ^= (y<<7)&0x9d2c5680
        y ^= (y<<15)&0xefc60000
        return (y^(y>>18))&0xffffffff


def phase_step(index, fraction, frequency, frames):
    amount = f32(f32(f32(frequency)/RATE)*f32(frames))
    fraction = libm.fmaf(amount,255.,fraction)
    while fraction >= 1.:
        index += 1
        fraction = f32(fraction-1.)
    return index%255,fraction


class Filter:
    def __init__(self, a, b):
        self.a,self.b,self.state = list(map(f32,a)),list(map(f32,b)),[0.,0.,0.]

    def process(self, x):
        _,s1,s2 = self.state
        w = libm.fmaf(-self.a[1],s1,x)
        w = libm.fmaf(-self.a[2],s2,w)
        y = f32(s1*self.b[1])
        y = libm.fmaf(w,self.b[0],y)
        y = libm.fmaf(self.b[2],s2,y)
        self.state = [w,w,s1]
        return y


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--factory',action='store_true')
    args = parser.parse_args()
    cases,blocks,checks,coverage,error_max,draws = 0,0,0,set(),0.,0
    summaries, upper_rounding = [], 0
    table = [f32(math.sin(f32(f32(i/256.)*f32(2*math.pi)))) for i in range(256)]
    profiles = (([1.,0.,0.],[1.,0.,0.]),([1.,-.5,.125],[.15,.3,.15]))
    if args.factory:
        from flutter_factory import FactoryBytes, factory_table, factory_coefficients
        table = factory_table()
    for seed, profile, frequencies, initial in itertools.product(
            (5489,1337,0xdeadbeef),(0,) if args.factory else range(2),
            ((.6,5.),) if args.factory else ((.6,5.),(500.,1500.)),
            ((0,0.),) if args.factory else ((0,0.),(254,.875))):
        c = FactoryBytes(seed) if args.factory else ARMBytes()
        rng = MT(seed)
        if args.factory:
            c.reg(0,BASE)
            c.call(0x4F1C0)
            phases = [[0,0.],[0,f32(.17)]]
            frequencies = (f32(.6),5.)
            assert c.floats(BASE+124,256) == table
            assert list(struct.unpack('<624I',c.read(BASE+3648,2496))) == rng.values
            assert c.getu(BASE+6144) == 624
            assert c.getf(BASE+6148) == -1. and c.getf(BASE+6152) == 1.
            assert c.getf(BASE+24) == 0. and c.getf(BASE+28) == 0.
            assert c.getu(BASE+76) == 0 and c.getu(BASE+120) == 1
            assert c.getu(BASE+6156) == 0 and c.getu(BASE+6160) == 0
            service_counts = {name:sum(s['address']==name for s in c.services)
                              for name in ('0x162e4','0x160f8','0x159d8','0x16224','0x16368')}
            assert list(service_counts.values()) == [1,1,2,255,2]
            for offset,(index,fraction),frequency in zip((0,12),phases,frequencies):
                assert c.getf(BASE+offset) == frequency
                assert c.getu(BASE+offset+4) == index and c.getf(BASE+offset+8) == fraction
            filters = [Filter(*coeff) for coeff in factory_coefficients()]
        else:
            c.write(BASE+3648,struct.pack('<624I',*rng.values))
            c.putu(BASE+6144,624)
            c.putf(BASE+6148,-1.)
            c.putf(BASE+6152,1.)
            c.write(BASE+124,struct.pack('<256f',*table))
            phases = [list(initial),list(initial)]
            for offset,(index,fraction),frequency in zip((0,12),phases,frequencies):
                c.putf(BASE+offset,frequency)
                c.putu(BASE+offset+4,index)
                c.putf(BASE+offset+8,fraction)
            filters = [Filter(*profiles[profile]) for _ in range(2)]
        state_pointers = []
        for i,flt in enumerate(filters):
            p,d = BASE+36+i*44,0x180000+i*0x100
            if args.factory:
                assert c.floats(c.getu(p+4),3) == flt.a
                assert c.floats(c.getu(p+16),3) == flt.b
                assert c.floats(c.getu(p+28),3) == flt.state
                state_pointers.append(c.getu(p+28))
            else:
                c.vector(p+4,d,flt.a)
                c.vector(p+16,d+0x20,flt.b)
                c.vector(p+28,d+0x40,flt.state)
                state_pointers.append(d+0x40)
        raw = []
        returned = []
        for block in range(128):
            frames = (1,7,32,128,1024)[block%5]
            count = (1,7,32,128)[block%4]
            depth = f32((0.,.25,1.,0.)[(block//32)%4])
            crinkle = f32((0.,1.,.25,0.)[(block//32)%4])
            c.putf(BASE+24,depth)
            c.putf(BASE+28,crinkle)
            c.vector(BASE+6156,NOISE,[0.]*count)
            noise = []
            for _ in range(count):
                value = rng.next()
                u = f32(f32(value)*f32(2.**-32))
                if u >= 1.:
                    upper_rounding += 1
                    u = f32(1.-2.**-24)
                x = f32(libm.fmaf(u,2.,-1.)-.5)
                raw.append(x)
                for flt in filters:
                    x = flt.process(x)
                noise.append(x)
            for i,frequency in enumerate(frequencies):
                phases[i] = list(phase_step(*phases[i],frequency,frames))
            waves = [libm.fmaf(pf,f32(table[pi+1]-table[pi]),table[pi]) for pi,pf in phases]
            total = f32(f32(depth*.5)*waves[1])
            total = libm.fmaf(depth,waves[0],total)
            total = libm.fmaf(crinkle,noise[0],total)
            expected = libm.fmaf(f32(f32(total+2.5)/5.),OUT_SCALE,OUT_BASE)
            if depth == 0. and crinkle == 0.:
                assert expected == 1.
            c.reg(0,BASE)
            c.reg(1,frames)
            c.fp(0,(-10.,0.,1.,100.)[block%4])
            c.call(0x502C4)
            observed = c.fp(0)
            assert observed == expected,(seed,profile,frequencies,initial,block,expected,observed)
            assert c.floats(NOISE,count) == noise
            for offset,want in zip((0,12),phases):
                assert (c.getu(BASE+offset+4),c.getf(BASE+offset+8)) == tuple(want)
            assert c.getu(BASE+6144) == rng.index
            assert list(struct.unpack('<624I',c.read(BASE+3648,2496))) == rng.values
            for i,flt in enumerate(filters):
                assert c.floats(state_pointers[i],3) == flt.state
            error_max = max(error_max,abs(expected-observed))
            checks += 636+count
            draws += count
            returned.append(observed)
            blocks += 1
        cases += 1
        coverage.update(c.coverage)
        mean = sum(raw)/len(raw)
        summaries.append(dict(seed=seed,filter_profile=profile,frequencies=frequencies,initial=initial,
                              raw_noise_mean=mean,raw_noise_variance=sum((x-mean)**2 for x in raw)/len(raw),
                              factor_min=min(returned),factor_max=max(returned)))
    result = dict(status='PASS',sequences=cases,process_calls=blocks,rng_draws=draws,
                  comparisons=checks,max_abs_error=error_max,distinct_instruction_addresses=len(coverage),
                  upper_rounding_cases=upper_rounding,summaries=summaries,
                  limitations=['Complete original scalar TapeFlutter.process, MT19937 generation and two Biquad processes; persistent independent phases/RNG/filter/output.',
                               'Supplied sine table, seeded state, explicit noise-vector lengths and transparent/stable test coefficients; full constructor and factory coefficient generation not yet executed.',
                               'Depth/release controls supplied; no upstream Time/preset publication, touch, motion or vector-overload process integration.'])
    if args.factory:
        result['factory_initialization'] = dict(frequencies=[f32(.6),5.],phases=[[0,0.],[0,f32(.17)]],
                                                filters=factory_coefficients(),service_counts=service_counts,
                                                default_depths=[0.,0.],default_noise_vector_length=0)
        result['limitations'] = ['Complete original constructor and scalar process; explicit glibc math services and supplied entropy seed, no physical entropy device.',
                                'Independent factory table/coefficients/seed expansion and persistent phases/RNG/filters/output; noise-vector length and depth controls supplied.',
                                'Host-libm boundary does not prove appliance-libm identity; rare RNG endpoint covered separately, actual scheduling, touch and vector overload remain open.']
    filename = 'factory_flutter_state_probe_results.json' if args.factory else 'flutter_state_probe_results.json'
    (ROOT/'probes'/filename).write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='summaries'},indent=2))


if __name__ == '__main__':
    main()
