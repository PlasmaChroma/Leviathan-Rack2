#!/usr/bin/env python3
"""Execute selected routines in the limited PIC18 harness and export evidence.
No hardware, network, serial port, or flash programming is used. Tests validate
instruction/model consistency, NOT actual hardware timing or a complete clone.
"""
from pathlib import Path
import csv,json,random,math
from pic18_harness import initialized
from models import ratio_halfperiod, phase_offset, next_random, adc_state, decode_state
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'analysis'
def csvout(name,rows):
    with (OUT/name).open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
def main():
    summary={};m=initialized();(OUT/'SIMULATED_ram_after_c_runtime.bin').write_bytes(m.ram)
    summary['c_runtime_instructions']=m.steps
    assert m.ram[0x5ee:0x5f1]==bytes.fromhex('00022d')
    # All 64 EEPROM addressing cases, independent channel/state pattern.
    m.eeprom[:]=bytes((i*17+i//64)%256 for i in range(1024));rows=[]
    for s in range(64):
        m.ee_trace=[];m.run(0x87fa,w=s)
        expected=[64*c+s for c in range(6)]+[0x180+64*c+s for c in range(6)]+[0x300+s,0x340+s]
        assert sorted(x[1] for x in m.ee_trace if x[0]=='read')==sorted(expected)
        d=decode_state(m.eeprom,s)
        assert list(m.ram[0xc2c+6*s:0xc2c+6*s+6])==[r&255 for r in d['ratio_codes']]
        assert list(m.ram[0xdac+6*s:0xdac+6*s+6])==d['phase_codes']
        assert m.ram[0x840+s]==d['enable_mask'] and m.ram[0x800+s]==d['mod_mask']
        for a in expected:rows.append({'state':s,'eeprom_address':f'0x{a:03X}','value':m.eeprom[a]})
    csvout('state_eeprom_address_tests.csv',rows);summary['eeprom_state_cases']=64
    # Factory reconstruction begins with explicitly unobserved FF EEPROM bytes.
    m=initialized();m.run(0x6e56,w=0);mask=bytearray(1024)
    writes=[t for t in m.ee_trace if t[0]=='write']
    for _,a,v,pc in writes:mask[a]=255
    (ROOT/'firmware/SYNTHETIC_factory_eeprom.bin').write_bytes(m.eeprom)
    (ROOT/'firmware/SYNTHETIC_factory_eeprom_written_mask.bin').write_bytes(mask)
    csvout('factory_eeprom_write_trace.csv',[{'address':f'0x{a:03X}','value':v,'pc':f'0x{pc:06X}'} for _,a,v,pc in writes])
    states=[decode_state(m.eeprom,s) for s in range(64)]
    (OUT/'factory_states.json').write_text(json.dumps(states,indent=2)+'\n')
    csvout('factory_states.csv',[{'state_0_based':s['state'],'bank_1_based':s['bank']+1,'slot_1_based':s['slot']+1,
        **{f'ratio_ch{c+1}':s['ratio_codes'][c] for c in range(6)},
        **{f'phase_ch{c+1}':s['phase_codes'][c] for c in range(6)},
        'enable_mask':s['enable_mask'],'mod_mask':s['mod_mask']} for s in states])
    for s in states:
        for c in range(6):
            assert s['ratio_codes'][c]&255==(m.flash[0xfdad+6*s['state']+c] if s['state']<16 else 0)
            assert s['phase_codes'][c]==(m.flash[0xfd4d+6*s['state']+c] if s['state']<16 else 0)
    summary.update(factory_state_cases=64,factory_write_operations=len(writes),factory_unique_written_bytes=sum(bool(x) for x in mask))
    # Ratio model includes edge/saturation/wrapped signed arithmetic.
    rows=[]
    for h in [0,1,199,200,8000,100001,0xffffff]:
        for r in range(-124,125):
            m.put(0x60,r,2);m.put(0x62,h,4);m.run(0x7e54)
            got=m.u(0x60,4);want=ratio_halfperiod(r,h)
            assert got==want,(r,h,got,want)
            rows.append({'code':r,'master_halfperiod_ticks':h,'computed_halfperiod_ticks':got,'model_matches':True})
    csvout('ratio_test_vectors.csv',rows);summary['ratio_cases']=len(rows)
    # Phase calculations, complete 65DE routine with real helper arithmetic.
    phase_rows=[]
    for r in [-124,-8,-2,0,1,4,124]:
        for p in [0,1,3,7,127,255]:
            h=8000;m.put(0x45,h,4);m.ram[0x47f]=1
            for c in range(6):m.put(0x538+2*c,r,2);m.ram[0x570+c]=p
            m.run(0x65de)
            for c in range(6):
                got=m.u(0x520+4*c,4);want=phase_offset(r,p,h)&0xffffffff
                assert got==want,('phase',r,p,c,got,want)
            phase_rows.append({'code':r,'phase_byte':p,'master_halfperiod_ticks':h,'offset_ticks':got})
    csvout('phase_test_vectors.csv',phase_rows);summary['phase_six_lane_cases']=len(phase_rows)
    # Reduced ratio denominators and LCM alignment helper.
    alignment=[]
    for r in range(-124,125):
        m.put(0x1a,r);m.run(0x8bcc)
        want=(4-r)//math.gcd(4-r,4) if r<0 else 4//math.gcd(r+4,4)
        assert m.u(0x1a)==want
        alignment.append({'ratio_code':r,'master_cycles_to_alignment':want})
    csvout('ratio_alignment_factors.csv',alignment);summary['alignment_factor_cases']=len(alignment)
    pairs=[(1,1),(2,3),(3,4),(32,31),(5,7),(1,127),(16,32)]
    for a,b in pairs:
        m.put(0x1f,a);m.put(0x21,b);m.run(0x7f8e)
        assert m.u(0x1f)==math.lcm(a,b)
    summary['lcm_cases']=len(pairs)
    # Isolate the actual ISR output-drive prefix for all 64 logical patterns.
    for pattern in range(64):
        for c in range(6):m.ram[0x43c+c]=(pattern>>c)&1
        m.ram[0xf9e]=2;m.ram[0xf8a]=0xc0;m.run(0x808,stop=0x884)
        want=0xc0|sum(((pattern>>c)&1)<<bit for c,bit in enumerate([5,3,1,2,4,0]))
        assert m.ram[0xf8a]==want
    summary['gpio_output_patterns']=64
    # Deterministic 64-bit generator, including the surprising 8-bit return.
    rng=random.Random(71);rows=[]
    for seed in [0,1,2**64-1]+[rng.getrandbits(64) for _ in range(61)]:
        m.put(0x5b6,seed,8);m.run(0x8446);new,out=next_random(seed)
        assert m.u(0x5b6,8)==new and m.u(0x2a)==out
        rows.append({'seed':f'0x{seed:016X}','next_state':f'0x{new:016X}','returned_u16':out})
    csvout('random_test_vectors.csv',rows);summary['lcg_cases']=len(rows)
    # ADC fixture uses synthetic calibration and a hooked ADC-read wrapper.
    m=initialized();th=[32+64*j for j in range(16)]
    for j,t in enumerate(th):m.put(0x500+2*j,t)
    adc=[0];m.hooks[0x9318]=lambda q:q.put(0x12,adc[0])
    count=0
    for prev in range(16):
        for value in range(0,1024,4):
            adc[0]=value;m.put(0x4e6,prev);got=m.run(0x78fc);want=adc_state(value,prev,th)
            assert got==want,('adc',value,prev,got,want);count+=1
    summary['adc_hysteresis_cases']=count
    # Feed bytes into the actual parser through its RX ring. Tests of branches
    # that call EEPROM/UI functions hook those callees and record dispatch only.
    m=initialized();events=[];parsed=[]
    def feed(data):
        for b in data:
            i=m.u(0x44e);m.ram[0xa60+i]=b;m.put(0x44e,(i+1)%460);m.run(0x71aa)
            parsed.append({'byte':f'{b:02X}','status':f'{m.ram[0x45e]:02X}','index':m.ram[0x45d],
                           'program_request':m.ram[0x5fa],'mesh_hex':m.ram[0x544:0x54c].hex()})
    feed([0xc0,37]);assert m.ram[0x5fa]==37 and m.ram[0x45e]==0
    feed([0xc1,38]);assert m.ram[0x5fa]==37
    feed([0xf0,0,2,0x2d,1,7,1,63,0,7,0xf7]);assert m.ram[0x54b]==128 and m.ram[0x544]==0
    feed([0xf0,0,3,0x2d,1,9,0xf7]);assert m.ram[0x545]==0
    m.hooks[0x88d4]=lambda q:events.append({'callee':'0x0088D4','W':q.w,'flag_029':q.ram[0x29]})
    feed([0xf4,12,0xf4,64]);assert events==[{'callee':'0x0088D4','W':12,'flag_029':0},{'callee':'0x0088D4','W':0,'flag_029':1}]
    feed([0xc0,0xf8,22]);assert m.ram[0x5fa]==22
    csvout('select_bus_parser_trace.csv',parsed)
    summary['select_bus_scenarios']=7
    (OUT/'select_bus_dispatch_tests.json').write_text(json.dumps(events,indent=2)+'\n')
    summary['all_assertions_passed']=True
    summary['limitations']=['Custom decoder and harness share an instruction definition; not independent third-party validation.',
        'No cycle-accurate MCU, analog front end, asynchronous interrupts, or physical module was simulated.',
        'EEPROM operations complete instantly in the harness.',
        'ADC tests substitute the ADC wrapper result; calibration values are synthetic.',
        'Select Bus F4 tests verify dispatcher calls via hooks, not persistent-store timing.',
        'Factory EEPROM is reconstructed without interactive calibration; unobserved bytes are FF fill.']
    (OUT/'validation.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps(summary,indent=2))
if __name__=='__main__':main()
