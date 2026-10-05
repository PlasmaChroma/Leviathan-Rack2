#!/usr/bin/env python3
"""Complete buffer publication and processSpeed, with independent persistent DSP."""
import json
import itertools
import struct
from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from flutter_factory import FactoryBytes, factory_table, factory_coefficients
from probe_flutter_state import MT, Filter, phase_step, OUT_BASE, OUT_SCALE

CHANNEL, LINK, PRESET = 0x100000, 0x140000, 0x150000
FLUTTER, TOUCH = CHANNEL+18960, CHANNEL+25128
VECTOR_SIZES = {172044:1,172056:1,172068:5,172080:5,172092:5,
                25116:1,6712:1,6724:1,6736:1,12868:1,12880:1,12892:1}


def literal(word):
    return struct.unpack('<f',struct.pack('<I',word))[0]


def touch_step(mode, value, state, frames, contact, trigger):
    # Scalar mode 1 has distinct >=1 return behavior; caller supplies input 1.
    if mode == 1:
        value = libm.fmaf(f32(frames),literal(0xb7a7c5ac if contact else 0x3951b717),value)
        return min(1.,max(0.,value)),state
    if mode != 2:
        return value,state
    delta = f32(frames)
    if state == 1:
        delta = f32(delta*literal(0xb8d1b717))
        if value <= f32(.3):
            state = 2
    elif state == 2:
        delta = f32(delta*literal(0x38d1b717))
        if value >= 1.:
            state = 0
    elif state == 0 and trigger:
        state,delta = 1,0.
    return min(1.,max(f32(.3),f32(delta+value))),state


def main():
    table,coverage = factory_table(),set()
    blocks,draws,comparisons,resizes,sequences = 0,0,0,0,0
    transitions = set()
    for seed in (5489,1337,0xdeadbeef):
        for mode,alias in itertools.product((0,1,2),(False,True)):
            c,rng = FactoryBytes(seed),MT(seed)
            link = CHANNEL+36 if alias else LINK
            c.reg(0,FLUTTER)
            c.call(0x4f1c0)
            c.reg(0,TOUCH)
            c.reg(1,CHANNEL)
            c.call(0x36b58)
            c.reg(0,CHANNEL+448)
            c.call(0x369f8)
            previous_contact = False
            c.putu(CHANNEL+232,link)
            c.putu(link+176,PRESET)
            c.putf(link+12,1.)
            c.putf(link+16,1.)
            c.putu(PRESET+60,mode)
            filters = [Filter(*coeff) for coeff in factory_coefficients()]
            phases = [[0,0.],[0,f32(.17)]]
            value,state = 1.,0
            ramp,counter,increment = f32(-.75),3,f32(.125)
            c.putf(CHANNEL+728,ramp)
            c.putf(CHANNEL+732,increment)
            c.putu(CHANNEL+736,counter)
            sizes = (1,7,32,128,128,32,7,1)
            previous_noise = []
            previous_count = 0
            for block in range(384):
                frames = sizes[block%len(sizes)]
                # Full original setter, empty sibling vectors initially; no resize stub.
                c.reg(0,CHANNEL)
                c.reg(1,frames)
                c.call(0x3b438)
                for offset,multiplier in VECTOR_SIZES.items():
                    begin,end = c.getu(CHANNEL+offset),c.getu(CHANNEL+offset+4)
                    assert end-begin == frames*multiplier*4,(offset,frames,end-begin)
                for offset in (172008,172028):
                    assert c.getu(CHANNEL+offset+4)-c.getu(CHANNEL+offset) == (frames+4)*4
                assert c.getu(CHANNEL+172004) == frames
                assert c.getu(CHANNEL+172024) == frames
                pointer = c.getu(FLUTTER+6156)
                retained = min(previous_count,frames)
                assert c.floats(pointer,retained) == previous_noise[:retained]
                if frames > previous_count:
                    assert c.floats(pointer+4*previous_count,frames-previous_count) == [0.]*(frames-previous_count)
                resizes += 1
                depth,crinkle = f32((0.,.25,1.,0.)[(block//32)%4]),f32((0.,1.,.25,0.)[(block//32)%4])
                c.putf(FLUTTER+24,depth)
                c.putf(FLUTTER+28,crinkle)
                linked = f32((-.5,0.,2.)[block%3])
                contact = block%96 < 48
                trigger = contact and not previous_contact
                released = previous_contact and not contact
                c.putf(link,linked)
                c.reg(0,CHANNEL+448)
                c.reg(1,int(contact))
                c.call(0x36640)
                assert c.getu(CHANNEL+452) == int(contact)
                assert c.getu(CHANNEL+456) == int(contact)
                assert c.read(CHANNEL+461,2) == bytes([trigger,released])
                previous_contact = contact
                if block%41 == 0:
                    counter,increment = 5,f32(-.0625 if block%82 else .0625)
                    c.putu(CHANNEL+736,counter)
                    c.putf(CHANNEL+732,increment)
                if counter > 0:
                    counter -= 1
                    ramp = f32(ramp+increment)
                noise = []
                for _ in range(frames):
                    u = f32(f32(rng.next())*f32(2.**-32))
                    if u >= 1.:
                        u = f32(1.-2.**-24)
                    x = f32(libm.fmaf(u,2.,-1.)-.5)
                    for flt in filters:
                        x = flt.process(x)
                    noise.append(x)
                phases = [list(phase_step(*p,f,frames)) for p,f in zip(phases,(f32(.6),5.))]
                waves = [libm.fmaf(pf,f32(table[pi+1]-table[pi]),table[pi]) for pi,pf in phases]
                periodic = libm.fmaf(depth,waves[0],f32(f32(depth*.5)*waves[1]))
                total = libm.fmaf(crinkle,noise[0],periodic)
                factor = libm.fmaf(f32(f32(total+2.5)/5.),OUT_SCALE,OUT_BASE)
                old_state = state
                value,state = touch_step(mode,value,state,frames,contact,trigger)
                transitions.add((mode,old_state,state))
                c.reg(0,CHANNEL)
                c.reg(1,frames)
                c.call(0x37408)
                assert [c.getf(CHANNEL+o) for o in (36,40,48,52,728)] == [ramp,linked,factor,value if mode else 1.,ramp]
                assert c.getu(CHANNEL+736) == counter
                assert c.getf(TOUCH+4) == value and c.getu(TOUCH+8) == state
                if alias:
                    assert [c.getf(link+o) for o in (0,4,12,16)] == [ramp,linked,factor,value if mode else 1.]
                assert c.floats(pointer,frames) == noise
                assert c.getu(FLUTTER+6144) == rng.index
                assert list(struct.unpack('<624I',c.read(FLUTTER+3648,2496))) == rng.values
                for offset,(index,fraction) in zip((0,12),phases):
                    assert c.getu(FLUTTER+offset+4) == index and c.getf(FLUTTER+offset+8) == fraction
                for i,flt in enumerate(filters):
                    assert c.floats(c.getu(FLUTTER+36+i*44+28),3) == flt.state
                previous_noise,previous_count = noise,frames
                comparisons += 663+2*frames+(4 if alias else 0)
                draws += frames
                blocks += 1
            coverage.update(c.coverage)
            sequences += 1
    assert {(2,0,1),(2,1,2),(2,2,0)} <= transitions
    result = dict(status='PASS',sequences=sequences,complete_speed_calls=blocks,
                  complete_buffer_resize_calls=resizes,rng_draws=draws,
                  comparisons=comparisons,max_abs_error=0.,
                  distinct_instruction_addresses=len(coverage),touch_transitions=sorted(transitions),
                  link_fixtures=['detached supplied LinkData','unlinked inline Channel+36 alias'],
                  limitations=['Original Flutter, CapTouch and Button constructors, Button.interpretGPIO, complete Channel.setBufferSize and processSpeed; explicit seed and host-libm services.',
                               'Channel shell and preset/mode/contact pin/depth/link/ramp producers supplied; no whole Channel constructor, hardware event scheduling, vector overload or head motion.',
                               'Frames 1/7/32/128 are deliberate fixtures, not evidence of driver callback size; positive frames only.'])
    (ROOT/'probes/speed_modulation_join_probe_results.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))


if __name__ == '__main__':
    main()
