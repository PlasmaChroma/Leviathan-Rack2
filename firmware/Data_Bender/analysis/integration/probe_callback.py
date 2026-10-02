#!/usr/bin/env python3
"""Original callback clock scheduling and dispatcher RNG-consumption probes.
Hardware clock/gate/polling calls are stubbed; callback DSP is replaced by a
state recorder. Dispatcher tests separately bypass Buffer, Corrupt and Tone.
Prepared states test scheduling, not physical clocks or whole-device audio.
"""
from pathlib import Path
import hashlib
import json
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'corrupt_dsp'))
from firmware_harness import FirmwareMachine, FIRMWARE, bitsf32
from unicorn import UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_R5, UC_ARM_REG_R2, UC_ARM_REG_R3, UC_ARM_REG_S0
C=0x24000cf8
P=0x24000d68
IN=0x20020000
OUT=0x20030000


def callback_case(event_after_frames):
    m=FirmwareMachine()
    frequency=80. # within the normal internal Time range
    tau=m.f32(0x08003698)
    phase=tau-(event_after_frames-.25)*tau*frequency/96028.
    m.u32(C,0)
    m.u32(C+0x30,12500)
    for off,value in [(0xc,phase),(0x10,96028.),(0x34,frequency),
                      (0x38,.5),(0x40,.5),(0x44,.5),(0x48,1.),(0x68,12.5)]:
        m.f32(C+off,value)
    events=[]
    dispatched=[]
    def trace(uc,address,size,user):
        # r5 already advanced to the NEXT interleaved index here.
        events.append(uc.reg_read(UC_ARM_REG_R5)//2-1)
    m.uc.hook_add(UC_HOOK_CODE,trace,begin=0x080035be,end=0x080035be)
    for address in (0x080070e4,0x080023a0):
        m.hooks[address]=lambda x:x.return_u32(0)
    m.hooks[0x0800950c]=lambda x:x.return_u32(1000)
    m.hooks[0x08009510]=lambda x:x.return_u32(200000000)
    def dispatch(x):
        dispatched.append(dict(clock_pending=x.u8(P+0x74),
                               event_offset_field=x.u32(P+0x90),
                               time_guard=x.u8(P+0x94),
                               scalar_count=x.uc.reg_read(UC_ARM_REG_R3)))
        x.return_u32(0)
    m.hooks[0x08001e4c]=dispatch
    m.call(0x080032fc,args=(IN,OUT,192),max_instructions=200000)
    assert len(dispatched)==1
    assert events==[event_after_frames-1], events
    assert dispatched[0]['clock_pending']==1
    assert dispatched[0]['event_offset_field']==0
    return dict(prepared_event_after_frames=event_after_frames,
                callback_event_frames=events,dispatch=dispatched[0])


def dispatcher_case(mode,amount,pending):
    m=FirmwareMachine()
    m.f32(P,96028.)
    m.u32(P+4,0) # Macro
    m.u32(P+0x684,mode)
    m.f32(P+0x38,amount)
    m.u8(P+0x74,pending)
    m.u32(P+0x90,37) # synthetic tag to verify forwarding, not callback output
    draws=[]
    def rand(x):
        val=(12345,67890)[len(draws)]
        draws.append(val)
        x.return_u32(val)
    def fill(x):
        x.floats(x.uc.reg_read(UC_ARM_REG_R2),[0.]*x.uc.reg_read(UC_ARM_REG_R3))
        x.return_u32(0)
    m.hooks[0x08018958]=rand
    m.hooks[0x08004d58]=fill
    m.hooks[0x08001868]=fill
    m.hooks[0x08017aac]=lambda x:x.return_f32(bitsf32(x.uc.reg_read(UC_ARM_REG_S0)))
    m.call(0x08001e4c,args=(P,IN,OUT,192),max_instructions=200000)
    assert len(draws)==2*pending
    assert m.u8(P+0x74)==0
    return dict(mode=mode,amount=amount,clock_pending_initial=pending,
                rand_draws=draws,buffer_transition_request=m.u8(P+0x1b1),
                buffer_transition_guard=m.u32(P+0x1b8),
                corrupt_channel_choice=m.u32(P+0x2dc),
                corrupt_random_value=m.f32(P+0x2f0))


def main():
    callbacks=[callback_case(n) for n in (1,24,48,72,96)]
    dispatchers=[dispatcher_case(mode,amount,pending) for mode in (0,1,2,3,4,5)
                 for amount in (0.,.5,1.) for pending in (0,1)]
    dispatchers += [dispatcher_case(0,amount,1) for amount in (1./512.,1./256.,1./128.)]
    assert dispatchers[-3]['corrupt_random_value'] > 1.
    assert dispatchers[-2]['corrupt_random_value'] == 0.
    result=dict(method=__doc__,firmware_sha256=hashlib.sha256(FIRMWARE.read_bytes()).hexdigest(),
                callback_cases=callbacks,dispatcher_cases=dispatchers)
    Path(__file__).with_name('callback_probes.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(callbacks=callbacks,dispatcher_cases=len(dispatchers),
                         dispatcher_rng_check='two draws on every tested pending clock, zero without'),indent=2))

if __name__=='__main__':
    main()
