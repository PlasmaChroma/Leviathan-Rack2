#!/usr/bin/env python3
"""Execute the actual mix/width dispatcher, substituting only buffer, Corrupt,
and output low-pass routines to isolate and measure the output routing.
No hardware boot is simulated. All substitutions are logged in the result.
"""
import json, math, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent / 'corrupt_dsp'))
from firmware_harness import FirmwareMachine, bitsf32, f32bits
from unicorn.arm_const import *

OBJ=0x20010000
IN=0x20020000
OUT=0x20030000

def probe(mix, width, dry=(1.,0.), wet=(0.,1.), freeze=False):
    m=FirmwareMachine()
    m.floats(IN,list(dry))
    m.u32(OBJ+4,1)  # Micro mode
    m.f32(OBJ,96028.)
    m.f32(OBJ+0xec,2.)
    m.f32(OBJ+0x30,.5)
    m.f32(OBJ+0x50,mix)
    m.f32(OBJ+0x6c,mix) # put mix smoother at target for static-curve probe
    m.f32(OBJ+0x6d0,width)
    m.u8(OBJ+0x6d4,0) # linear crossfade width curve
    m.u8(OBJ+0x11,int(freeze))
    def buffer(machine):
        machine.floats(machine.uc.reg_read(UC_ARM_REG_R2), list(wet))
        machine.return_u32(0)
    def corrupt(machine):
        n=machine.uc.reg_read(UC_ARM_REG_R3)
        data=machine.uc.mem_read(machine.uc.reg_read(UC_ARM_REG_R1),n*4)
        machine.uc.mem_write(machine.uc.reg_read(UC_ARM_REG_R2),bytes(data))
        machine.return_u32(0)
    def tone(machine):
        machine.return_f32(bitsf32(machine.uc.reg_read(UC_ARM_REG_S0)))
    m.hooks[0x08004d58]=buffer
    m.hooks[0x08001868]=corrupt
    m.hooks[0x08017aac]=tone
    m.call(0x08001e4c,args=(OBJ,IN,OUT,2))
    ms=m.f32(OBJ+0x6c)
    ld=math.sin((1-ms)*math.pi/2)*dry[0]+math.sin(ms*math.pi/2)*wet[0]
    rd=math.sin((1-ms)*math.pi/2)*dry[1]+math.sin(ms*math.pi/2)*wet[1]
    l=(1-width)*ld+width*rd
    r=(1-width)*rd+width*l
    actual=m.floats(OUT,2)
    return dict(mix=mix,width_crossfeed=width,freeze=freeze,dry=dry,wet=wet,
                target_mix=m.f32(OBJ+0x2c),smoothed_mix=ms,
                actual=actual,predicted=[l,r],max_error=max(abs(a-b) for a,b in zip(actual,[l,r])))

def probe_reader():
    rows=[]
    for phase,rate in [(0.,1.),(.25,1.),(2.5,.5),(6.25,-1.),(7.5,1.)]:
        m=FirmwareMachine();buf=0x20050000;flag=0x20060000
        m.floats(buf,[0.,10.,20.,30.,40.,50.,60.,70.])
        m.u32(OBJ+0xc,buf);m.u32(OBJ+0x10,8)
        m.f32(OBJ+0x13c,phase);m.u32(OBJ+0x19c,0)
        m.f32(OBJ+0xa4,rate);m.f32(OBJ+0xa8,1.)
        m.f32(OBJ+0xb8,-256.);m.f32(OBJ+0xbc,256.)
        m.f32(OBJ+0xdc,1.);m.f32(OBJ+0xe0,1.)
        m.f32(OBJ+0xf8,rate);m.u32(OBJ+0x15c,0);m.u32(OBJ+0x164,8)
        _,actual=m.call(0x0800493c,args=(OBJ,0,flag))
        i=int(phase);a=phase-i;samples=[0.,10.,20.,30.,40.,50.,60.,70.]
        expected=samples[i]+a*(samples[(i+1)%8]-samples[i])
        rows.append(dict(phase=phase,rate=rate,actual=actual,predicted=expected,
                         next_phase=m.f32(OBJ+0x13c),wrapped=m.u8(flag),error=abs(actual-expected)))
    return rows

if __name__=='__main__':
    rows=[probe(m,a) for m in [0.,.25,.5,.75,1.] for a in [0.,.25,.5]]
    rows += [probe(0.,0.,freeze=True)]
    result=dict(method='Original Cortex-M7 machine code; buffer wet return, Corrupt copy and output Tone bypass are explicit isolation hooks',
                dispatcher='0x08001e4c',width_function='0x08017488',reader='0x0800493c',
                mix_width=rows,reader_probes=probe_reader(),
                max_error=max(r['max_error'] for r in rows))
    p=Path(__file__).with_name('output_probe.json');p.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(file=str(p),max_error=result['max_error'],reader_errors=[r['error'] for r in result['reader_probes']],
                         width_extreme=rows[14],freeze_dry=rows[-1]),indent=2))
