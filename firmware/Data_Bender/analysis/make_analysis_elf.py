#!/usr/bin/env python3
"""Build an analyst-created ELF wrapper around the unmodified supplied image.

This is NOT the manufacturer's original ELF or original linker section layout.
It adds virtual addresses and provisional semantic symbols for analysis tools.
It is not a replacement flash/update file. The raw BIN remains authoritative.
"""
from pathlib import Path
import hashlib,json,struct,sys

ROOT=Path(__file__).resolve().parents[1]
EXPECTED='591eb538e2c6ce3024a1dfc1d85d8c7ef466ca5e13290191f2994aae2706f41d'
def align(n,a=4):return (n+a-1)&~(a-1)

def make(firmware,symbols,out):
    raw=firmware.read_bytes()
    if hashlib.sha256(raw).hexdigest()!=EXPECTED:raise ValueError('Image hash mismatch')
    names={}
    for line in symbols.read_text().splitlines()[1:]:
        addr,name=line.split('\t')[:2];names[int(addr,16)]=name
    payload=bytearray(0x1000);payload.extend(raw)
    def append(data):
        while len(payload)%4:payload.append(0)
        pos=len(payload);payload.extend(data);return pos,len(data)
    strtab=bytearray(b'\0');sym=[bytes(16)]
    for addr,name in sorted(names.items()):
        no=len(strtab);strtab.extend(name.encode()+b'\0')
        sym.append(struct.pack('<IIIBBH',no,addr|1,0,0x12,0,1))
    symoff,symsize=append(b''.join(sym));stroff,strsize=append(strtab)
    note={'kind':'synthetic analysis wrapper','original_sha256':EXPECTED,
          'original_file_size':len(raw),'symbols':'analysis-assigned; not original source symbols',
          'cpu':'STM32H750 / Cortex-M7, little-endian Thumb with FPv5',
          'image_bytes':'unchanged','section_layout':'synthetic; .firmware includes original literals and data image'}
    noteoff,notesize=append(json.dumps(note,indent=2).encode()+b'\0')
    sections=['','.firmware','.data','.bss','.active_sdram','.dtcm_stack','.symtab','.strtab','.analysis_note','.shstrtab']
    shstr=bytearray(b'\0');shnames={}
    for n in sections[1:]:shnames[n]=len(shstr);shstr.extend(n.encode()+b'\0')
    shoffnames,shsize=append(shstr)
    while len(payload)%4:payload.append(0)
    shoff=len(payload)
    # name,type,flags,address,file_offset,size,link,info,alignment,entry_size
    hdrs=[(0,)*10,
        (shnames['.firmware'],1,6,0x08000000,0x1000,len(raw),0,0,4,0),
        (shnames['.data'],1,3,0x24000000,0x1000+0x1a570,0x3e4,0,0,4,0),
        (shnames['.bss'],8,3,0x240003e4,0,0x47d4-0x3e4,0,0,4,0),
        (shnames['.active_sdram'],8,3,0xc0000000,0,28808400,0,0,4,0),
        (shnames['.dtcm_stack'],8,3,0x20000000,0,0x20000,0,0,8,0),
        (shnames['.symtab'],2,0,0,symoff,symsize,7,1,4,16),
        (shnames['.strtab'],3,0,0,stroff,strsize,0,0,1,0),
        (shnames['.analysis_note'],1,0,0,noteoff,notesize,0,0,1,0),
        (shnames['.shstrtab'],3,0,0,shoffnames,shsize,0,0,1,0)]
    payload.extend(b''.join(struct.pack('<10I',*h) for h in hdrs))
    ident=b'\x7fELF'+bytes([1,1,1,0,0])+bytes(7)
    header=struct.pack('<16sHHIIIIIHHHHHH',ident,2,40,1,0x08000a09,52,shoff,0x05000400,52,32,4,40,len(hdrs),9)
    # PT_LOAD, file offset, virtual address, physical/load address, file bytes,
    # memory bytes, flags and alignment. .data has flash LMA / RAM VMA.
    segments=[(1,0x1000,0x08000000,0x08000000,len(raw),len(raw),5,0x1000),
              (1,0x1000+0x1a570,0x24000000,0x0801a570,0x3e4,0x47d4,6,4),
              (1,0,0xc0000000,0xc0000000,0,28808400,6,4),
              (1,0,0x20000000,0x20000000,0,0x20000,6,8)]
    payload[:52]=header
    for i,p in enumerate(segments):payload[52+i*32:84+i*32]=struct.pack('<8I',*p)
    out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(payload)
    from elftools.elf.elffile import ELFFile
    with out.open('rb') as f:
        e=ELFFile(f)
        assert e.get_section_by_name('.firmware').data()==raw
        assert e.get_section_by_name('.data').data()==raw[0x1a570:]
        assert e.header.e_entry==0x08000a09
        assert e.get_section_by_name('.symtab').num_symbols()==len(names)+1
    return {'file':str(out),'bytes':len(payload),'symbols':len(names),'original_bytes_verified':True}

if __name__=='__main__':
    firmware=Path(sys.argv[1]) if len(sys.argv)>1 else ROOT/'upload/Data_Bender_v1_4_7.bin'
    symbols=Path(sys.argv[2]) if len(sys.argv)>2 else ROOT/'analysis/function_map.tsv'
    out=Path(sys.argv[3]) if len(sys.argv)>3 else ROOT/'analysis/Data_Bender_v1_4_7_ANALYSIS.elf'
    print(json.dumps(make(firmware,symbols,out),indent=2))
