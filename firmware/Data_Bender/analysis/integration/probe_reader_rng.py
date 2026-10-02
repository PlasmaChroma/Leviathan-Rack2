#!/usr/bin/env python3
"""Execute original fractional reader and libc PRNG against independent models.
Prepared state; no peripheral emulation, random stub, or libm substitution.
PRNG seeds are explicit test seeds, not a claim about natural hardware boot.
"""
from pathlib import Path
import hashlib
import json
import struct
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'corrupt_dsp'))
from firmware_harness import FirmwareMachine, FIRMWARE

OBJ=0x20010000
BUF=0x20050000
FLAG=0x20060000
MASK=(1<<64)-1
MULTIPLIER=6364136223846793005

def reader_case(length, speed):
    m=FirmwareMachine()
    m.floats(BUF,[float(i) for i in range(length)])
    for off,val in [(0xc,BUF),(0x10,length),(0x15c,0),(0x164,length)]:
        m.u32(OBJ+off,val)
    phase=0. if speed>0 else float(length-1)
    for off,val in [(0x13c,phase),(0xa4,speed),(0xa8,1.),(0xb8,-8.),(0xbc,8.),
                    (0xdc,1.),(0xe0,1.),(0xf8,speed)]:
        m.f32(OBJ+off,val)
    m.u8(OBJ+0x11a,1)
    wraps=[]
    samples=[]
    for frame in range(160):
        m.u8(FLAG,0)
        _,sample=m.call(0x0800493c,args=(OBJ,0,FLAG))
        assert sample==phase, (length,speed,frame,sample,phase)
        phase+=speed
        wrap=phase<0 or phase>length-1
        if wrap:
            phase=float(length-1) if speed<0 else 0.
            wraps.append(frame)
        assert m.f32(OBJ+0x13c)==phase
        assert m.u8(FLAG)==int(wrap)
        samples.append(sample)
    return dict(length=length,rate=speed,wrap_frames=wraps[:12],
                first_samples=samples[:16], distinct_samples=len(set(samples)),
                wrap_period=wraps[1]-wraps[0] if len(wraps)>1 else None)


def rng_case(seed):
    m=FirmwareMachine()
    # Follow the original literal pointer; supply only the libc reentrant storage.
    pointer_slot=m.u32(0x080189b4)
    reent=0x20070000
    state=0x20071000
    m.u32(pointer_slot,reent)
    m.u32(reent+0x38,state)
    m.uc.mem_write(state+0x10,struct.pack('<Q',seed))
    expected=seed
    results=[]
    for _ in range(256):
        actual,_=m.call(0x08018958)
        expected=(expected*MULTIPLIER+1)&MASK
        assert actual==(expected>>32)&0x7fffffff
        assert struct.unpack('<Q',m.uc.mem_read(state+0x10,8))[0]==expected
        results.append(actual)
    return dict(seed=seed,draws_checked=256,first_12=results[:12],
                first_12_mod255=[v%255 for v in results[:12]],final_state=hex(expected))


def main():
    readers=[reader_case(n,s) for n in (2,4,8,16,64,101)
             for s in (-8.,-2.,-.5,.5,1.,1.5,2.,8.)]
    rng=[rng_case(seed) for seed in (0,1,0x123456789abcdef0,0xffffffffffffffff)]
    result=dict(method=__doc__,firmware_sha256=hashlib.sha256(FIRMWARE.read_bytes()).hexdigest(),
                reader_cases=readers, rng=dict(multiplier=MULTIPLIER,increment=1,
                output='(state >> 32) & 0x7fffffff',cases=rng))
    Path(__file__).with_name('reader_rng_probes.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(reader_cases=len(readers),reader_frames=len(readers)*160,
        rng_draws=1024,rng=rng,examples=[r for r in readers if r['length']==8]),indent=2))

if __name__=='__main__':
    main()
