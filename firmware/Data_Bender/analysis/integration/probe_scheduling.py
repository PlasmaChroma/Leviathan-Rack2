#!/usr/bin/env python3
"""Original-instruction scheduling probes; no codec/ADC or hardware boot.

Extends the supplied small-buffer Engine. Its rand, powf and memset hooks
remain in place. Prepared states deliberately isolate channel/block coupling.
All frame indices in traces are zero-based; no physical timing is measured.
"""
from pathlib import Path
import hashlib
import json
import math
import struct
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'buffer_engine'))
from probe_engine import Engine, POOL, OBJ
from unicorn import UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_R8


def prepare(**kwargs):
    e = Engine(window=0., **kwargs)
    # Distinct address-identifiable data, identical on L/R, bounded audio.
    data = struct.pack('<32768f', *[math.sin(i * .071) for i in range(32768)])
    for base in (POOL, POOL + 32768 * 4):
        e.u.mem_write(base, data)
    return e


def run_partition(block, scenario, shared=1):
    e = prepare(N=1000, bend=.9 if scenario == 'macro' else 0.,
                brk=.8 if scenario == 'macro' else 0.,
                randoms=(5, 210, 400, 99, 254, 33, 180, 601, 75, 0, 160))
    e.wi(0x68, shared)
    e.wb(0x11a, 1)
    e.wb(0x11b, 1)
    if scenario == 'time':
        e.wf(0x54, 96028. / 2000.)
    if scenario == 'macro':
        for ch in (0, 1):
            e.wf(0x144 + ch * 4, 980.)
    output = []
    for _ in range(1920 // block):
        output.extend(e.process(n=block, kind='zero'))
    packed = struct.pack('<%df' % len(output), *output)
    return dict(block_frames=block, scenario=scenario, shared=shared,
                output_sha256=hashlib.sha256(packed).hexdigest(),
                stereo_max_difference=max(abs(output[i]-output[i+1]) for i in range(0,len(output),2)),
                first_stereo_difference=next((i//2 for i in range(0,len(output),2) if output[i]!=output[i+1]),None),
                states=[e.snapshot(ch) for ch in (0,1)], rng_calls=len(e.calls),
                rng_trace=e.calls, output=output)


def freeze_guard(block, guard, previous, buffer_value):
    e = prepare(N=64)
    data = struct.pack('<f', buffer_value) * 32768
    for base in (POOL, POOL + 32768 * 4):
        e.u.mem_write(base, data)
    e.wb(0x11a, 0)
    e.wb(0x11b, 1)
    e.wi(0x120, guard)
    for ch in (0,1):
        e.wb(0x234 + ch, 1)
        e.wf(0x1f0 + 4*ch, previous)
        e.wf(0x1f8 + 4*ch, previous)
    accepted = []
    def trace(u, address, size, user):
        ch = u.reg_read(UC_ARM_REG_R8)
        accepted.append(dict(channel=ch, frame=e.frame[ch],
                             prior_active=e.rb(0x11a), guard=e.ri(0x120)))
    e.u.hook_add(UC_HOOK_CODE, trace, begin=0x08005262, end=0x08005262)
    for _ in range(1920 // block):
        e.process(n=block, kind='constant', value=buffer_value)
    return dict(block_frames=block, guard_initial=guard, previous=previous,
                buffer_value=buffer_value, accepted=accepted,
                final=[e.snapshot(ch) for ch in (0,1)], guard_final=e.ri(0x120))


def main():
    partitions=[]
    for scenario, shared in [('steady',1),('time',1),('macro',1),('macro',0)]:
        rows=[run_partition(b,scenario,shared) for b in (1,16,48,96,192)]
        reference=rows[3]['output']
        for row in rows:
            samples=row.pop('output')
            row['max_difference_vs_96']=max(abs(a-b) for a,b in zip(samples,reference))
        partitions.extend(rows)
    freezes=[freeze_guard(b,g,p,v) for b in (1,96) for g in (0,100)
             for p,v in [(1.,1.),(-1.,1.),(0.,0.),(1.,0.),(0.,1.)]]
    # Metamorphic baseline: without changing shared state, partitioning is neutral.
    steady=[r for r in partitions if r['scenario']=='steady']
    assert len({r['output_sha256'] for r in steady}) == 1
    assert all(r['stereo_max_difference']==0 for r in steady)
    assert any(r['max_difference_vs_96']>0 for r in partitions if r['scenario']=='time')
    assert any(r['max_difference_vs_96']>0 for r in partitions if r['scenario']=='macro')
    for row in freezes:
        frames=[(v['channel'],v['frame']) for v in row['accepted']]
        if row['previous']==1. and row['buffer_value']==1.:
            expected=([(0,63),(1,63)] if row['guard_initial']==0 else
                      [(0,127),(1,127)] if row['block_frames']==1 else
                      [(1,127),(0,255)])
        else:
            expected=[(0,0),(1,0)]
        assert frames==expected, (row,expected)
    result=dict(method=__doc__, firmware_sha256=hashlib.sha256(
        (Path(__file__).resolve().parents[2]/'upload/Data_Bender_v1_4_7.bin').read_bytes()).hexdigest(),
        partition_cases=partitions, freeze_cases=freezes)
    dest=Path(__file__).with_name('scheduling_probes.json')
    dest.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(cases=len(partitions)+len(freezes),
        partitions=[{k:r[k] for k in ['scenario','shared','block_frames','max_difference_vs_96','stereo_max_difference']} for r in partitions],
        freezes=[{k:r[k] for k in ['block_frames','guard_initial','previous','buffer_value','accepted']} for r in freezes]),indent=2))

if __name__=='__main__':
    main()
