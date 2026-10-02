#!/usr/bin/env python3
"""Differential comparisons against the uploaded ARM firmware via Unicorn."""
from pathlib import Path
import json, math, random, struct
from firmware_harness import FirmwareMachine
from reference_models import Decimator,Destroy,DjFilter,Vinyl,F

OBJ=0x20000000;INPUT=0x20010000;OUTPUT=0x20020000
FS=96028.0 # firmware initialization constant; hardware wire rate is a separate question

def init():
    m=FirmwareMachine()
    m.u32(0x24000004,1)
    m.u8(OBJ+0x28,16);m.u8(OBJ+0x50,16)
    m.call(0x08001384,args=(OBJ,),floats=(FS,))
    return m

def run(m,mode,amount,samples):
    m.u32(OBJ,mode);m.f32(OBJ+0x10,amount)
    m.floats(INPUT,samples)
    m.call(0x08001868,args=(OBJ,INPUT,OUTPUT,len(samples)),max_instructions=2000000)
    return m.floats(OUTPUT,len(samples))

if __name__=='__main__':
    rng=random.Random(147)
    samples=[F(rng.uniform(-.8,.8)) for _ in range(256)]
    amounts=[0.,.019,.02,.03,.05,.051,.1234,.25,.499,.5,.501,.505,.65,.75,.9,1.]
    cases=[];vectors=[]
    for mode in [1,3,4,5]:
      for amount in amounts:
        m=init()
        expected=[]
        if mode==1:
          ref=[Decimator(),Decimator()]
          for c in ref:c.configure(F(amount))
          expected=[ref[i%2].process(x) for i,x in enumerate(samples)]
        else:
          ref={3:Destroy,4:lambda:DjFilter(FS),5:lambda:Vinyl(FS)}[mode]()
          ref.configure(F(amount))
          for i in range(0,len(samples),2):expected.extend(ref.process(samples[i],samples[i+1]))
        observed=run(m,mode,amount,samples)
        errors=[abs(a-b) for a,b in zip(observed,expected)]
        largest=max(errors)
        bit_exact = struct.pack('<%df' % len(observed), *observed) == struct.pack('<%df' % len(expected), *expected)
        cases.append(dict(mode=mode,amount=amount,samples=len(samples),max_abs_error=largest,bit_exact=bit_exact,pass_tolerance=largest<2e-5))
        if amount in [.0,.5,1.]:vectors.append(dict(mode=mode,amount=amount,inputs=samples[:32],outputs=observed[:32],reference_outputs=expected[:32]))
        print(mode,amount,largest,bit_exact)
    result={'method':'Original Cortex-M7 machine code, including libm, invoked after original Corrupt initializer. Fresh explicit zero object plus maxbit ctor16, shared LCG seed1. Reference models independently express recovered equations. No math hooks. bit_exact compares packed little-endian IEEE 754 float32 bytes, distinguishing positive and negative zero; tolerance uses numeric absolute error.','config_sample_rate':FS,'cases':cases,'all_pass':all(x['pass_tolerance'] for x in cases),'vectors':vectors}
    out=Path(__file__).with_name('differential_verification.json');out.write_text(json.dumps(result,indent=2)+'\n')
    print('all_pass',result['all_pass'])
