#!/usr/bin/env python3
"""Force each Vinyl dust layer's rare nonzero branch with explicit RNG state.
Original instructions and libm, no function substitution. State injection is
not a natural event-rate measurement. Verify held pulse duration and routing.
"""
from pathlib import Path
import json,struct,sys,hashlib
from probe_vinyl_long import FastMachine,OBJ,IN,OUT,V,FIRMWARE
from reference_models import Vinyl,F

OFFSETS=[0,0x2c,0x58,0x84,0xb0]
ORDER=[0,1,3,2,4]

def case(layer):
    m=FastMachine();m.u32(0x24000004,1);m.u8(OBJ+0x28,16);m.u8(OBJ+0x50,16)
    m.call(0x08001384,args=(OBJ,),floats=(96028.,))
    ref=Vinyl(96028.);ref.configure(F(1.))
    # Make all five layers update on frame zero. Invert enough LCG steps so
    # exactly the target draw is zero (certain negative full-amplitude pulse).
    seed=0;inverse=pow(1103515245,-1,1<<31)
    for _ in range(ORDER.index(layer)+1):seed=((seed-12345)*inverse)&0x7fffffff
    m.u32(0x24000004,seed);ref.lcg=seed
    for i,d in enumerate(ref.dust):
        d.counter=d.period-1;m.u32(V+OFFSETS[i]+8,d.counter)
    m.u32(OBJ,5);m.f32(OBJ+0x10,1.)
    rows=[];worst=0.
    for frame in range(31):
        expected=ref.process(.2,-.1)
        m.floats(IN,[.2,-.1]);m.call(0x08001868,args=(OBJ,IN,OUT,2))
        actual=m.floats(OUT,2)
        worst=max(worst,max(abs(a-b) for a,b in zip(actual,expected)))
        dust=[m.f32(V+o+0x10) for o in OFFSETS]
        assert dust==[d.value for d in ref.dust]
        rows.append(dict(frame=frame,dust=dust,output=actual))
    assert worst<2e-5
    assert rows[0]['dust'][layer]<0
    period=ref.dust[layer].period
    assert all(r['dust'][layer]==rows[0]['dust'][layer] for r in rows[:period])
    return dict(layer=layer,injected_lcg_state=seed,hold_frames=period,max_error=worst,samples=rows)

if __name__=='__main__':
    rows=[case(i) for i in range(5)]
    result=dict(method=__doc__,firmware_sha256=hashlib.sha256(FIRMWARE.read_bytes()).hexdigest(),cases=rows)
    Path(__file__).with_name('vinyl_rare_probes.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps([{k:v for k,v in r.items() if k!='samples'} for r in rows],indent=2))
