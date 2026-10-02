#!/usr/bin/env python3
"""Prepared reduced-buffer history wrap with Freeze active.
Original buffer instructions. Deliberately inject a retained bank offset that
can outlive a resize and a history cursor near capacity; not natural startup.
"""
from pathlib import Path
import sys,json,struct
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'buffer_engine'))
from probe_engine import Engine,POOL

def case(bank):
    e=Engine(N=1200,window=0.,speed=.125);e.track=False
    e.wb(0x11a,1);e.wb(0x11b,1)
    initial=struct.pack('<f',.2)*32768
    for ch in (0,1):
        e.u.mem_write(POOL+ch*32768*4,initial)
        e.wf(0xf8+ch*4,.125)
        e.wi(0x19c+ch*4,bank)
        e.wi(0x1cc+ch*0x14,32766)
    before=bytes(e.u.mem_read(POOL,4800*4))
    out=e.process(n=96,kind='constant',value=.8)
    after=bytes(e.u.mem_read(POOL,4800*4))
    changed=[i for i in range(4800) if before[i*4:i*4+4]!=after[i*4:i*4+4]]
    assert changed==list(range(2400,2494)),changed
    assert e.rb(0x11a)==1
    if bank==2400:assert max(out)>.79
    if bank==0:assert max(out)<.21
    return dict(read_bank=bank,frozen=e.rb(0x11a),changed_indices=[min(changed),max(changed)],
        changed_count=len(changed),first_12_output_frames=[out[i:i+2] for i in range(0,24,2)],
        peak=max(out),history_cursor=e.ri(0x1cc),final=e.snapshot(0))
if __name__=='__main__':
    r=dict(method=__doc__,cases=[case(0),case(2400)])
    Path(__file__).with_name('history_boundary_probes.json').write_text(json.dumps(r,indent=2)+'\n')
    print(json.dumps(r,indent=2))
