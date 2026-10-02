#!/usr/bin/env python3
"""Long original-code Vinyl/model comparisons, including natural rollovers.
Original initializer, libm, noise generators and processing execute; no stubs.
Explicit zeroed RAM and seed=1, no hardware boot. 96-frame blocks, F_init=96028.
WAV playback rate 48000 is an audition assumption, not a board measurement.
"""
from pathlib import Path
import sys, json, struct, math, hashlib, wave
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'corrupt_dsp'))
from firmware_harness import FirmwareMachine, FIRMWARE, FLASH, RETURN_SENTINEL
from reference_models import Vinyl, F
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *

class FastMachine(FirmwareMachine):
    def __init__(self):
        self.uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
        self.uc.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
        b=FIRMWARE.read_bytes()
        self.uc.mem_map(FLASH,(len(b)+4095)&~4095); self.uc.mem_write(FLASH,b)
        for a in (0x20000000,0x24000000,0xe0000000):self.uc.mem_map(a,0x100000)
        self.u32(0xe000ed88,0x00f00000)
        self.uc.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000)
        self.uc.reg_write(UC_ARM_REG_FPEXC,0x40000000)
        self.uc.hook_add(UC_HOOK_CODE,lambda u,a,s,d:u.emu_stop(),begin=RETURN_SENTINEL,end=RETURN_SENTINEL)
        self.trace=[]

OBJ=0x20000000; IN=0x20010000; OUT=0x20020000; V=OBJ+0x240

def run_case(amount,pause=False):
    m=FastMachine();m.u32(0x24000004,1)
    m.u8(OBJ+0x28,16);m.u8(OBJ+0x50,16)
    m.call(0x08001384,args=(OBJ,),floats=(96028.,))
    ref=Vinyl(96028.)
    total=288096; worst=0.; mismatches=0; events=[0]*5; rollovers=[]; checkpoints=[]; audio=[]
    for start in range(0,total,96):
        u=.05 if pause and 47904<=start<48096 else amount
        ref.configure(F(u));m.u32(OBJ,5);m.f32(OBJ+0x10,u)
        # Silence makes generated texture directly inspectable.
        inputs=[0.]*192; expected=[]
        for i in range(96):
            expected.extend(ref.process(0.,0.))
            if ref.enabled:
                if ref.mod_counter==0:rollovers.append(dict(frame=start+i,modulation=ref.modulation))
                for j,d in enumerate(ref.dust):
                    if d.counter==0 and d.value!=0:events[j]+=1
        m.floats(IN,inputs)
        m.call(0x08001868,args=(OBJ,IN,OUT,192),max_instructions=1000000)
        observed=m.floats(OUT,192);audio.extend(observed)
        worst=max(worst,max(abs(a-b) for a,b in zip(expected,observed)))
        mismatches+=sum(struct.pack('<f',a)!=struct.pack('<f',b) for a,b in zip(expected,observed))
        assert m.u32(V+0x128)==ref.mod_counter
        assert m.u32(V+0x120)==ref.slow_counter
        assert m.f32(V+0x130)==ref.modulation
        assert m.f32(V+0x11c)==ref.slow_value
        if 47808<=start<=48192 or 95808<=start<=96480:
            checkpoints.append(dict(frame_end=start+96,amount=u,mod_counter=ref.mod_counter,
                modulation=ref.modulation,slow_value=ref.slow_value))
    assert worst<2e-5, worst
    assert len(rollovers)>=5
    if amount==1. and not pause:
        dest=Path(__file__).with_name('vinyl_silence_48k.wav')
        # Fixed attenuation, no normalization: preserve relative noise peaks.
        with wave.open(str(dest),'wb') as w:
            w.setnchannels(2);w.setsampwidth(2);w.setframerate(48000)
            w.writeframes(struct.pack('<%dh'%len(audio),*[round(max(-1.,min(1.,x*.5))*32767) for x in audio]))
    return dict(amount=amount,pause=pause,frames=total,max_abs_error=worst,
        unequal_float32_samples=mismatches,dust_event_counts=events,rollovers=rollovers,
        checkpoints=checkpoints,peak=max(map(abs,audio)),rms=math.sqrt(sum(x*x for x in audio)/len(audio)),
        audio_float32_sha256=hashlib.sha256(struct.pack('<%df'%len(audio),*audio)).hexdigest())

if __name__=='__main__':
    rows=[]
    for amount,pause in [(.25,False),(1.,False),(1.,True)]:
        row=run_case(amount,pause);rows.append(row)
        print(json.dumps({k:v for k,v in row.items() if k!='checkpoints'}),flush=True)
    result=dict(method=__doc__,firmware_sha256=hashlib.sha256(FIRMWARE.read_bytes()).hexdigest(),cases=rows)
    Path(__file__).with_name('vinyl_long_probes.json').write_text(json.dumps(result,indent=2)+'\n')
