#!/usr/bin/env python3
"""Reproduce byte-level Data Bender 1.4.7 evidence, without Ghidra.

Usage: python analysis/reproduce_static.py [firmware.bin] [output-directory]
Only the annotated disassembly requires capstone. No network calls are made.
Known table locations apply exclusively to the SHA-256 recorded below.
"""
from pathlib import Path
import csv, hashlib, json, math, re, struct, sys
from collections import Counter

ROOT=Path(__file__).resolve().parents[1]
EXPECTED='591eb538e2c6ce3024a1dfc1d85d8c7ef466ca5e13290191f2994aae2706f41d'
BASE=0x08000000

def analyze(firmware,out):
    d=firmware.read_bytes(); out.mkdir(parents=True,exist_ok=True)
    sha=hashlib.sha256(d).hexdigest()
    if sha!=EXPECTED:
        raise SystemExit('Hash differs from the analyzed image; refusing to apply version-specific offsets.')
    entropy=-sum(c/len(d)*math.log2(c/len(d)) for c in Counter(d).values())
    inv={'filename':firmware.name,'bytes':len(d),'sha256':sha,'entropy_bits_per_byte':entropy,
         'format':'raw, little-endian Thumb firmware; no transport decoding required',
         'load_base':hex(BASE),'initial_stack':hex(struct.unpack_from('<I',d)[0]),
         'reset_vector_thumb':hex(struct.unpack_from('<I',d,4)[0]),
         'data_copy':{'file_offset':'0x1a570','virtual_start':'0x24000000','bytes':0x3e4},
         'bss':{'start':'0x240003e4','end_exclusive':'0x240047d4','bytes':0x47d4-0x3e4},
         'active_buffer':{'left_base':'0xc0000000','right_base':'0xc0dbca68',
                          'frames_per_channel':3601050,'bytes_total':28808400,
                          'maximum_primary_window_frames':1800525},
         'caution':'The binary is the source of truth; memory regions, rates and durations require semantic analysis.'}
    (out/'firmware_inventory.json').write_text(json.dumps(inv,indent=2)+'\n')
    names=['initial_SP','Reset','NMI','HardFault','MemManage','BusFault','UsageFault',
           'reserved','reserved','reserved','reserved','SVCall','DebugMon','reserved','PendSV','SysTick']
    with (out/'vector_words.csv').open('w',newline='') as f:
        w=csv.writer(f);w.writerow(['index','role','file_offset','word','note'])
        for i in range(0x298//4):
            v=struct.unpack_from('<I',d,i*4)[0]
            w.writerow([i,names[i] if i<16 else f'IRQ_{i-16}',hex(i*4),f'0x{v:08x}',
                        'default handler' if v==0x080009fd else ''])
    tables={}
    for name,off,n in [('micro_octave_targets',0x19bf4,7),('external_clock_ratios',0x19d20,9),
                       ('decimate_bitcrush_table',0x19d44,17),('decimate_downsample_table',0x19d88,17),
                       ('macro_bend_rate_table',0x19dcc,10),('macro_break_silence_table',0x19df4,5),
                       ('driver_sample_rate_table',0x19ed4,5)]:
        tables[name]={'file_offset':hex(off),'virtual_address':hex(BASE+off),'encoding':'IEEE754 float32 little-endian',
                      'values':list(struct.unpack_from('<%df'%n,d,off)),
                      'raw_words':[f'0x{x:08x}' for x in struct.unpack_from('<%dI'%n,d,off)]}
    (out/'lookup_tables.json').write_text(json.dumps(tables,indent=2)+'\n')
    with (out/'strings.csv').open('w',newline='') as f:
        w=csv.writer(f);w.writerow(['file_offset','virtual_address','ascii','interpretation'])
        for m in re.finditer(rb'[\x20-\x7e]{7,}',d):
            w.writerow([hex(m.start()),hex(BASE+m.start()),m.group().decode(),
                        'candidate only; random instruction bytes also form ASCII'])
    try:
        from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB,CS_MODE_LITTLE_ENDIAN
        from capstone.arm import ARM_OP_IMM,ARM_OP_MEM,ARM_REG_PC
    except ImportError:
        return inv
    md=Cs(CS_ARCH_ARM,CS_MODE_THUMB|CS_MODE_LITTLE_ENDIAN);md.detail=True;md.skipdata=True
    calls=[];literals=[]
    with (out/'linear_thumb_listing.txt').open('w') as f:
        f.write('DIAGNOSTIC LINEAR SWEEP: embedded tables/literal pools may appear as instructions.\n'
                'Use the analyzed Ghidra listing and function evidence to distinguish code from data.\n\n')
        for ins in md.disasm(d[0x2a0:0x19bf4],BASE+0x2a0):
            line=f'{ins.address:08x}: {ins.bytes.hex():12} {ins.mnemonic:12} {ins.op_str}'
            if ins.id:
                if ins.mnemonic in ['bl','blx'] and ins.operands and ins.operands[0].type==ARM_OP_IMM:
                    calls.append({'from':hex(ins.address),'to':hex(ins.operands[0].imm)})
                if ins.mnemonic.startswith(('ldr','vldr')) and len(ins.operands)>1 and ins.operands[1].type==ARM_OP_MEM and ins.operands[1].mem.base==ARM_REG_PC:
                    a=((ins.address+4)&~3)+ins.operands[1].mem.disp
                    if BASE<=a<BASE+len(d)-3:
                        word=struct.unpack_from('<I',d,a-BASE)[0]
                        val=struct.unpack_from('<f',d,a-BASE)[0]
                        line+=f' ; literal @{a:08x}=0x{word:08x}, float={val:g}'
                        literals.append({'instruction':hex(ins.address),'literal':hex(a),'word':hex(word)})
            f.write(line+'\n')
    (out/'direct_calls_linear.json').write_text(json.dumps(calls,indent=2)+'\n')
    (out/'literal_references_linear.json').write_text(json.dumps(literals,indent=2)+'\n')
    return inv

if __name__=='__main__':
    firmware=Path(sys.argv[1]) if len(sys.argv)>1 else ROOT/'upload/Data_Bender_v1_4_7.bin'
    out=Path(sys.argv[2]) if len(sys.argv)>2 else ROOT/'analysis/static'
    print(json.dumps(analyze(firmware,out),indent=2))
