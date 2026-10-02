#!/usr/bin/env python3
"""Original buffer processor under controlled Time/Freeze trajectories.
Uses existing Engine rand/powf/memset hooks, reduced 32768-frame/channel store.
96-frame channel-major blocks. Outer guard is a documented float32 model of
callback period smoothing, not a full callback/ADC simulation. Clock requests
are supplied by a fractional phase model, delivered as a block pending flag.
Prepared address-coded audio distinguishes retained memory from fresh input.
"""
from pathlib import Path
import sys,json,struct,math,hashlib
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'buffer_engine'))
from probe_engine import Engine,POOL,OBJ
from firmware_input import firmware_path
from unicorn import UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_R1
FS=96028.; INITIAL=2400; CAP=32768

def F(x):return struct.unpack('<f',struct.pack('<f',x))[0]

def run_case(speed,frozen,bank,clock_requests=True):
    e=Engine(N=INITIAL,speed=speed,window=0.);e.track=False
    for ch in (0,1):
        e.wf(0xf8+4*ch,speed)
        e.wi(0x19c+4*ch,bank*INITIAL)
    e.wb(0x11a,int(frozen));e.wb(0x11b,int(frozen))
    initial=struct.pack('<%df'%CAP,*[(-1 if (i//32)%2 else 1)*(.1+.5*i/CAP) for i in range(CAP)])
    for base in (POOL,POOL+CAP*4):e.u.mem_write(base,initial)
    stage_reads=[{},{}]
    addresses=[set(),set()]
    def reader(u,a,size,data):
        ch=u.reg_read(UC_ARM_REG_R1)
        pos=e.rf(0x13c+4*ch)+e.ri(0x19c+4*ch)
        index=max(0,int(pos))%CAP
        value=struct.unpack('<f',u.mem_read(POOL+ch*CAP*4+index*4,4))[0]
        key=('fresh' if abs(value)>.69 else 'initial')+('_bank0' if index<INITIAL else '_bank1' if index<2*INITIAL else '_tail')
        stage_reads[ch][key]=stage_reads[ch].get(key,0)+1
        addresses[ch].add(index)
    e.u.hook_add(UC_HOOK_CODE,reader,begin=0x0800493c,end=0x0800493c)
    smooth_ms=F(1000*INITIAL/FS);phase=0.;frame=0;stages=[]
    for name,n,blocks,marker in [('baseline',INITIAL,32,.71),('expand',4800,256,.75),
                               ('shrink',1200,256,.8),('restore',INITIAL,256,.85)]:
        stage_reads[:]=[{},{}];addresses[:]=[set(),set()]
        before=bytes(e.u.mem_read(POOL,2*INITIAL*4));states=[];prev=None;guard_blocks=0
        out_peak=0.;largest_jump=0.;last_output=None
        for b in range(blocks):
            target=F(FS/n);period_us=int(1e6/target);period_ms=F(period_us*F(.001))
            # Callback clocks/guard run before DSP over the complete block.
            pending=False
            for _ in range(96):
                smooth_ms=F(F(smooth_ms*F(.999869167804718))+period_ms*F(.00013083219528198242))
                guard=abs(F(period_ms-smooth_ms))>5.
                phase+=target/FS
                if phase>=1.:
                    phase-=1.
                    if not guard:pending=True
            e.wb(0x110,int(guard));guard_blocks+=int(guard)
            e.wf(0x54,F(1./F(period_us*F(.000001))))
            if pending and clock_requests:e.wb(0x119,1);e.wi(0x120,0)
            output=e.process(n=96,kind='constant',value=marker if (b//4)%2==0 else -marker)
            out_peak=max(out_peak,max(map(abs,output)))
            for sample in output[::2]:
                if last_output is not None:largest_jump=max(largest_jump,abs(sample-last_output))
                last_output=sample
            frame+=96
            snap=[e.snapshot(ch) for ch in (0,1)]
            sig=(guard,)+tuple((s['bankR'],s['bankW'],s['frozen'],s['timeResize']) for s in snap)
            if sig!=prev or b in (0,blocks-1):
                states.append(dict(frame=frame,guard=guard,channels=snap));prev=sig
        after=bytes(e.u.mem_read(POOL,2*INITIAL*4))
        changed=[i for i in range(2*INITIAL) if before[4*i:4*i+4]!=after[4*i:4*i+4]]
        stages.append(dict(name=name,target_N=n,frames=blocks*96,guard_blocks=guard_blocks,
            reads=stage_reads.copy(),address_ranges=[([min(a),max(a)] if a else []) for a in addresses],
            initial_region_changed_count=len(changed),changed_index_range=[min(changed),max(changed)] if changed else [],
            original_bank0_matches=after[:INITIAL*4]==initial[:INITIAL*4],
            original_bank1_matches=after[INITIAL*4:]==initial[INITIAL*4:2*INITIAL*4],
            peak=out_peak,max_adjacent_left_jump=largest_jump,states=states))
    assert all(math.isfinite(s['peak']) for s in stages)
    if frozen:assert stages[0]['initial_region_changed_count']==0
    return dict(speed=speed,frozen=frozen,initial_read_bank=bank,clock_requests=clock_requests,stages=stages)

if __name__=='__main__':
    rows=[]
    for speed,frozen,bank in [(s,f,b) for s in (1.,-1.,.125) for f,b in [(True,0),(True,1),(False,0)]]:
        row=run_case(speed,frozen,bank);rows.append(row)
        print(json.dumps(dict(speed=speed,frozen=frozen,bank=bank,stages=[{k:s[k] for k in ['name','reads','address_ranges','initial_region_changed_count','changed_index_range','original_bank0_matches','original_bank1_matches']} for s in row['stages']])),flush=True)
    rows.append(run_case(.125,True,1,clock_requests=False))
    result=dict(method=__doc__,firmware_sha256=hashlib.sha256(firmware_path().read_bytes()).hexdigest(),cases=rows)
    Path(__file__).with_name('time_memory_probes.json').write_text(json.dumps(result,indent=2)+'\n')
