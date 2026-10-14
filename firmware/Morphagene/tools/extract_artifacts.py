"""Export exact MG204 tables, synthetic address containers and evidence indexes."""
import math,re,struct
from common import *

# Counts distinguish physical blocks from the subset whose use is audited.
TABLES=[
 ('clock_reciprocal',0x0803ef50,6000,'f',None,6000,'Approximately 1/(32*(i+1)); clock-period lookup at 0x080275fa..0x08027614'),
 ('gene_size_exp',0x08044d10,1024,'f',None,1024,'Bit-exact float32(2**(i/341-3)); cubed at 0x08027944..0x08027952'),
 ('ratio_block_unassigned',0x08045d10,64,'f',None,None,'64-word bounded block, final zero; semantic role unproven, no reference in current traversal'),
 ('varispeed_positive',0x08045e10,296,'f',None,296,'vsop > 1 indexes 0..295 at 0x080287e0..0x08028816'),
 ('varispeed_magnitude',0x080462b0,200,'f',None,199,'vsop 0/1 magnitude table; maximum accessed index 198 in audited mapping'),
 ('signed_rate_block_unassigned',0x080465d0,128,'f',None,None,'Adjacent signed rate-shaped block; extent inferred from content, consumer unproven'),
 ('control_shape_a',0x08047264,64,'f',0x20000014,None,'Referenced at 0x08024b20; full semantics not traced'),
 ('control_shape_b',0x08047364,64,'f',0x20000114,None,'Referenced at 0x08024bc0; full semantics not traced'),
 ('control_shape_c',0x08047464,64,'f',0x20000214,None,'Referenced at 0x08024b1e; full semantics not traced'),
 ('clock_fraction_block',0x08047568,16,'f',0x20000318,None,'1,1/2,1/3,...1/256; consumer not established; not the Gene clock quantizer'),
 ('morph_density',0x080475a8,34,'f',0x20000358,22,'Nominal overlap density, selected at 0x08027a58; decimal approximations'),
 ('morph_launch',0x08047630,34,'f',0x200003e0,22,'Launch factor selected at 0x08027a50; decimal approximations'),
 ('morph_gain',0x080476b8,34,'f',0x20000468,34,'Envelope-sum-dependent gain target at 0x08027ade..0x08027b16; smoothed before playback scaling'),
 ('morph_cycle',0x08047740,34,'I',0x200004f0,22,'Integer scheduler cycle denominator selected at 0x08027a54')]

def ihex_record(address,kind,data):
    raw=bytes([len(data)])+address.to_bytes(2,'big')+bytes([kind])+data
    return ':'+(raw+bytes([-sum(raw)&255])).hex().upper()
def make_hex(b):
    lines=[];upper=None
    for off in range(0,len(b),16):
        a=BASE+off
        if a>>16!=upper:
            upper=a>>16;lines.append(ihex_record(0,4,upper.to_bytes(2,'big')))
        lines.append(ihex_record(a&65535,0,b[off:off+16]))
    lines.extend([ihex_record(0,5,b[4:8][::-1]),ihex_record(0,1,b'')])
    return '\n'.join(lines)+'\n'
def make_elf(b):
    # Minimal ELF32 ARM EABI5, sections and two load segments, no invented symbols.
    off=0x100;buf=bytearray(off)+b
    names=b'\0.flash\0.data\0.bss\0.shstrtab\0';namesoff=len(buf);buf+=names
    while len(buf)%4:buf+=b'\0'
    shoff=len(buf);buf+=bytes(40)
    def sh(n,t,flags,addr,pos,size,align=4):
        return struct.pack('<10I',names.index(n.encode()+b'\0'),t,flags,addr,pos,size,0,0,align,0)
    buf+=sh('.flash',1,6,BASE,off,len(b))
    buf+=sh('.data',1,3,0x20000000,off+0x27250,0xff0)
    buf+=sh('.bss',8,3,0x20000ff0,0,0x243c8-0xff0)
    buf+=sh('.shstrtab',3,0,0,namesoff,len(names),1)
    ident=b'\x7fELF'+bytes([1,1,1,0])+bytes(8)
    buf[:52]=struct.pack('<16sHHIIIIIHHHHHH',ident,2,40,1,0x08021415,52,shoff,0x05000000,52,32,2,40,5,4)
    buf[52:84]=struct.pack('<8I',1,off,BASE,BASE,len(b),len(b),5,4)
    buf[84:116]=struct.pack('<8I',1,off+0x27250,0x20000000,0x08047250,0xff0,0x243c8,6,4)
    return bytes(buf)

SYMBOLS=[
 ('Reset_Handler',0x08021414,'function','Copy RAM initializer, zero BSS, then system init/main'),
 ('system_initialize',0x0802145c,'function','Clock/peripheral register setup'),
 ('main',0x08022b68,'function','Main entry from reset'),
 ('audio_dma_configure',0x08025b04,'function','Receives TX, RX and halfword count'),
 ('audio_dma_irq',0x08025b9c,'function','Calls DSP for two halves of 32-halfword buffers'),
 ('cv_dma_irq',0x08025d58,'function','Converts 32-float half buffers to 12-bit CV words'),
 ('control_adc_irq',0x080263e4,'function','Six moving sums, 16/16/64/64/4/64 conversions'),
 ('dsp_initialize',0x08026730,'function','State, rate defaults and SDRAM initialization'),
 ('splice_insert_variant',0x08026c80,'function','Sorted splice-marker insertion; not generic audio initialization'),
 ('process_audio_block',0x08027518,'function','Large DSP entry; original odd entry 0x08027519 includes Thumb flag'),
 ('options',0x200012a8,'data','Parsed runtime options, outside startup initializer'),
 ('adc_sos',0x20021c68,'data','16-conversion average'),('adc_morph',0x20021c6c,'data','16-conversion average'),
 ('adc_organize',0x20021c70,'data','64-conversion average'),('adc_varispeed',0x20021c74,'data','64-conversion average'),
 ('adc_slide',0x20021c78,'data','4-conversion average'),('adc_gene_size',0x20021c7c,'data','64-conversion average'),
 ('whole_splice_flag',0x20021e14,'data','Distinct endpoint mode'),
 ('morph_launch_factor',0x20021f48,'data','Not the playback increment'),
 ('gene_duration',0x20021f90,'data','Output-time duration in ordinary mode'),
 ('morph_smoothed',0x20022084,'data','0.01 per-frame smoother'),
 ('rate_target',0x200220e8,'data','Base Vari-Speed target'),
 ('rng_state',0x200220f8,'data','32-bit LCG'),
 ('reel_left_pointer',0x200220fc,'data','Initialized to 0xd0000000'),
 ('reel_right_pointer',0x20022100,'data','Initialized to 0xd1000000'),
 ('morph_chord_ratios',0x20022124,'data','Three live ratios, not options struct'),
 ('splice_markers',0x2002215c,'data','Sorted sample offsets; full bounds/state policies still under analysis')]

def run():
    b=flash()
    for name in ['tables','binaries','reconstruction','analysis']:(ROOT/name).mkdir(exist_ok=True)
    (ROOT/'binaries/morphagene_mg204.bin').write_bytes(b)
    (ROOT/'binaries/morphagene_mg204.hex').write_text(make_hex(b))
    (ROOT/'binaries/morphagene_mg204_analysis_wrapper.elf').write_bytes(make_elf(b))
    (ROOT/'binaries/ram_initializer_20000000.bin').write_bytes(b[0x27250:0x28240])
    (ROOT/'binaries/preserved_tail.bin').write_bytes(b[0x28240:])
    manifests=[];header=['// Generated from the SHA-256-locked MG204 image. Do not regenerate formulas.',
       '// Exact table bits; roles and active extents are in tables/table_manifest.json.',
       '#pragma once','#include <array>','#include <cstdint>','namespace mg204 {']
    for name,a,n,fmt,ram,used,role in TABLES:
        data=b[a-BASE:a-BASE+n*4];assert len(data)==n*4
        vals=struct.unpack('<'+fmt*n,data);suffix='f32le' if fmt=='f' else 'u32le'
        (ROOT/f'tables/{name}.{suffix}.bin').write_bytes(data)
        put_csv(ROOT/f'tables/{name}.csv',['index','flash_address','ram_address','bits_hex','value'],
            [(i,hex(a+4*i),hex(ram+4*i) if ram else '',f'0x{struct.unpack_from("<I",data,4*i)[0]:08x}',repr(v)) for i,v in enumerate(vals)])
        manifests.append(dict(name=name,address=hex(a),image_offset=a-BASE,count=n,type=suffix,
            ram_address=hex(ram) if ram else None,audited_prefix_count=used,sha256=sha(data),interpretation=role,
            confidence='bytes exact; role unproven' if 'unassigned' in name or used is None else 'bytes exact; cited consumer traced'))
        header.append(f'inline constexpr std::array<{"float" if fmt=="f" else "uint32_t"}, {n}> {name} {{{{')
        for i in range(0,n,6):header.append('    '+', '.join(v.hex()+'f' if fmt=='f' else str(v)+'u' for v in vals[i:i+6])+',')
        header.append('}};')
    header.append('} // namespace mg204');(ROOT/'reconstruction/exact_tables.hpp').write_text('\n'.join(header)+'\n')
    put_json(ROOT/'tables/table_manifest.json',dict(flash_sha256=FLASH_HASH,tables=manifests))
    sym=[dict(name=n,address=hex(a),kind=k,evidence=e) for n,a,k,e in SYMBOLS]
    put_json(ROOT/'analysis/curated_symbols.json',sym)
    put_csv(ROOT/'analysis/curated_symbols.csv',['name','address','kind','evidence'],[(s['name'],s['address'],s['kind'],s['evidence']) for s in sym])
    put_json(ROOT/'analysis/memory_map.json',dict(flash_base=hex(BASE),flash_end_exclusive=hex(BASE+len(b)),
        stack_top='0x20030000',ram_initializer_flash='0x08047250',ram_initializer_destination='0x20000000',
        ram_initializer_bytes=0xff0,bss_start='0x20000ff0',bss_end_exclusive='0x200243c8',
        startup_evidence='0x08021414..0x08021442; literals 0x08021444..0x08021454',
        reel_planes=[dict(base=hex(a),sample_format='signed int16',sample_capacity=8388608,bytes=16777216) for a in (0xd0000000,0xd1000000)],
        reel_evidence='0x080268fc..0x0802692a and 0x080291ee..0x08029212',
        calibration_region='0x0800c000 (outside supplied image; reads at 0x08022d40 and following)',
        containers='HEX lossless; ELF synthetic and not an original build ELF; no live RAM is captured'))
    fits=[]
    for name,a,n,fn in [('gene_size_exp',0x08044d10,1024,lambda i:2**(i/341-3)),
                       ('clock_reciprocal',0x0803ef50,6000,lambda i:1/(32*(i+1)))]:
        vs=struct.unpack_from('<'+'f'*n,b,a-BASE)
        dif=[dict(index=i,stored_bits=hex(bits(v)),formula_bits=hex(bits(fn(i)))) for i,v in enumerate(vs) if bits(v)!=bits(fn(i))]
        fits.append(dict(name=name,entries=n,formula_binary32_mismatches=len(dif),differences=dif))
    put_json(ROOT/'analysis/formula_audit.json',fits)
    notes=[]
    for m in re.finditer(rb'[\x20-\x7e]{5,}',b):
        if 0x0803e340<=BASE+m.start()<0x0803e974:notes.append(f'0x{BASE+m.start():08x}  {m.group().decode()}')
    (ROOT/'analysis/embedded_options.txt').write_text('\n'.join(notes)+'\n')
    print(f'Exported {len(TABLES)} exact table blocks, HEX, synthetic ELF, startup bytes and evidence indexes')
if __name__=='__main__':run()
