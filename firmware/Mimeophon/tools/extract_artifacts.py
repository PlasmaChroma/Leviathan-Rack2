#!/usr/bin/env python3
"""Export byte-exact MP86 tables, vectors, memory data and an analysis-only ELF.

Names and section boundaries are analyst supplied; this does NOT reconstruct an
original linker ELF, debug symbols, source code, bootloader or flash settings.
Python 3.10+, standard library only. Run after decode_mp86.py.
"""
from __future__ import annotations
import argparse, csv, hashlib, json, math, re, struct
from pathlib import Path
BASE = 0x08020000
EXPECTED = '31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9'

def csv_write(path, fields, rows):
    with path.open('w', newline='', encoding='utf-8') as f:
        w = csv.writer(f); w.writerow(fields); w.writerows(rows)

def ihex_record(address, kind, data):
    raw = bytes([len(data)]) + address.to_bytes(2, 'big') + bytes([kind]) + data
    return ':' + (raw + bytes([(-sum(raw)) & 255])).hex().upper()

def make_ihex(data):
    lines, upper = [], None
    for off in range(0, len(data), 16):
        a = BASE + off
        if a >> 16 != upper:
            upper = a >> 16; lines.append(ihex_record(0, 4, upper.to_bytes(2, 'big')))
        lines.append(ihex_record(a & 65535, 0, data[off:off+16]))
    lines.append(ihex_record(0, 5, struct.unpack_from('<I', data, 4)[0].to_bytes(4, 'big')))
    lines.append(ihex_record(0, 1, b''))
    return '\n'.join(lines) + '\n'

def make_elf(data, symbols):
    # Synthetic ARM EABI5 executable container, for disassemblers ONLY.
    # PT_LOAD #1 retains the complete exact raw flash image; #2 maps the startup
    # RAM initializer using the same source bytes and represents the zeroed BSS.
    flashoff = 0x100
    buf = bytearray(flashoff) + data
    name = bytearray(b'\0')
    sym = bytearray(16)  # mandatory null symbol
    for s in symbols:
        no = len(name); name += s['name'].encode() + b'\0'
        value = int(s['address'], 16)
        section = 1 if BASE <= value < BASE+len(data) else 2 if 0x20000000 <= value < 0x20000174 else 3
        info = 0x12 if s['kind'] == 'function' else 0x11
        sym += struct.pack('<IIIBBH', no, value, 0, info, 0, section)
    stroff = len(buf); buf += name
    while len(buf) % 4: buf += b'\0'
    symoff = len(buf); buf += sym
    shstrings = b'\0.flash\0.data\0.bss\0.strtab\0.symtab\0.shstrtab\0'
    shstroff = len(buf); buf += shstrings
    while len(buf) % 4: buf += b'\0'
    shoff = len(buf)
    def sh(n, typ, flags, addr, off, size, link=0, info=0, align=4, entsize=0):
        return struct.pack('<10I', shstrings.find(n.encode()+b'\0') if n else 0,
                           typ, flags, addr, off, size, link, info, align, entsize)
    buf += bytes(40)
    buf += sh('.flash',1,6,BASE,flashoff,len(data))
    buf += sh('.data',1,3,0x20000000,flashoff+0xbe08,0x174)
    buf += sh('.bss',8,3,0x20000174,0,0x5a80-0x174)
    buf += sh('.strtab',3,0,0,stroff,len(name),align=1)
    buf += sh('.symtab',2,0,0,symoff,len(sym),link=4,info=1,entsize=16)
    buf += sh('.shstrtab',3,0,0,shstroff,len(shstrings),align=1)
    ident = b'\x7fELF' + bytes([1,1,1,0]) + bytes(8)
    hdr = struct.pack('<16sHHIIIIIHHHHHH',ident,2,40,1,0x08020299,52,shoff,0x05000000,52,32,2,40,7,6)
    buf[:52] = hdr
    buf[52:84] = struct.pack('<8I',1,flashoff,BASE,BASE,len(data),len(data),5,4)
    buf[84:116] = struct.pack('<8I',1,flashoff+0xbe08,0x20000000,BASE+0xbe08,0x174,0x5a80,6,4)
    return bytes(buf)

TABLES = [
 ('exp2_fraction',0x8dc4,2048,'2^(i/2048), verified numerical fit; used by rate/exponential conversion','high'),
 ('halo_matrix_gain',0xadc4,128,'Scalar multiplying the unnormalized 8x8 Hadamard feedback matrix','high'),
 ('repeats_gain',0xafc4,128,'Repeats control gain target; not a measurement of complete closed-loop gain','high'),
 ('color_coefficient_a',0xb1c4,256,'Color-related coefficient table; unusual byte-index access also exists','medium'),
 ('color_allpass_coefficient',0xb5c4,256,'Four cascaded allpass-average filter stages; coefficient interpolated in time','high'),
 ('color_control_shape',0xb9c4,256,'Color-related shape/gain/control law; full transfer function not reconstructed','medium'),
 ('zone_minimum_samples',0xbe10,8,'Nominal zone minimum delay in samples','high'),
 ('zone_transition_scale',0xbe30,8,'Per-zone scale, likely transition/read-head window length; semantics partly inferred','medium'),
 ('zone_polynomial_c0',0xbe50,4,'First coefficient in amplitude-dependent polynomial gain','high'),
 ('zone_polynomial_c1',0xbe60,4,'Second coefficient in amplitude-dependent polynomial gain','high'),
 ('zone_polynomial_c2',0xbe70,4,'Third coefficient in amplitude-dependent polynomial gain','high'),
 ('zone_polynomial_c3',0xbe80,4,'Fourth coefficient in amplitude-dependent polynomial gain','high'),
 ('zone_slew_coefficient',0xbe90,8,'Per-zone coefficient selected into live state','high'),
 ('rate_aux_coefficients',0xbeb0,5,'Rate/time auxiliary coefficients; exact role not fully traced','medium'),
 ('zone_near_unity_coefficient',0xbeec,8,'Per-zone near-unity filter coefficient; precise filter role not fully traced','medium'),
]

SYMBOLS = [
 ('vector_table',BASE,'data','114 entries including initial SP'),
 ('runtime_start',BASE+0x1c8,'function','Startup memory initialization and call to main'),
 ('Reset_Handler_trampoline',BASE+0x298,'function','Vector reset target, Thumb bit removed'),
 ('tick_increment_candidate',BASE+0x420,'function','Support routine'),
 ('tick_read_candidate',BASE+0x430,'function','Support routine'),
 ('millisecond_delay_candidate',BASE+0x43c,'function','Support routine'),
 ('dsp_initialize',BASE+0x32d0,'function','Audio buffers, DSP state, tables, DMA startup'),
 ('clear_main_delay_buffers',BASE+0x3988,'function','Clears two 2^21-float rings'),
 ('process_stereo_block',BASE+0x39b4,'function','Main floating-point DSP, analyst label'),
 ('audio_callback_rx_first_half',BASE+0x697c,'function','4 stereo frames; outputs to other TX half'),
 ('audio_callback_rx_second_half',BASE+0x6ab4,'function','4 stereo frames; outputs to other TX half'),
 ('codec_setup_candidate',BASE+0x6bf0,'function','Hardware setup; codec identity not established'),
 ('zone_led_color_candidate',BASE+0x6eb0,'function','Table-branch-controlled display values'),
 ('clock_initialize_candidate',BASE+0x7548,'function','RCC/PLL configuration'),
 ('main',BASE+0x7648,'function','System/hardware main routine'),
 ('default_interrupt_handler',BASE+0x7c94,'function','Shared unused interrupt target'),
 ('delay_heads',0x20001a14,'data','Four 44-byte structures, two per main buffer'),
 ('audio_rx_words',0x20001adc,'data','16 signed 32-bit transfer words'),
 ('main_delay_mask',0x20001b20,'data','0x1fffff'),
 ('halo_aux_ring_mask',0x20001b4c,'data','32767'),
 ('halo_allpass_969',0x20001b6c,'data','969 float ring'),
 ('halo_aux_ring_pointer',0x20002a9c,'data','0xd0000000'),
 ('modulation_lcg_state',0x20002acc,'data','32-bit LCG state'),
 ('active_polynomial_coefficients',0x20002ad4,'data','Four zone-selected coefficients'),
 ('audio_tx_words',0x20002ae8,'data','16 signed 32-bit transfer words'),
 ('halo_allpass_803',0x20002b64,'data','803 float ring'),
 ('main_delay_length',0x200037f8,'data','2097152 samples'),
 ('tempo_delay_ratios',0x20003804,'data','13 initialized floats'),
 ('audio_float_output',0x200038a4,'data','8 floats = 4 stereo frames'),
 ('halo_allpass_1236',0x200038e8,'data','1236 float ring'),
 ('color_filter_state',0x20004c74,'data','Two observed 44-byte filter state groups'),
 ('halo_matrix_gain_state',0x20004d24,'data','Smoothed coefficient multiplying Hadamard outputs'),
 ('audio_float_input',0x20004d7c,'data','8 floats = 4 stereo frames'),
 ('adc_control_words',0x20004f98,'data','Seven observed 32-bit ADC/control slots'),
 ('firmware_revision_integer',0x200050fc,'data','Main stores integer 86'),
 ('halo_allpass_1511',0x20000278,'data','1511 float ring'),
]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1])
    a=p.parse_args(); root=a.root; data=(root/'binaries/mimeophon_mp86.bin').read_bytes()
    if hashlib.sha256(data).hexdigest()!=EXPECTED:
        raise ValueError('Not the audited MP86 payload. Offsets are version-specific; refusing to mislabel it.')
    for sub in ['analysis','binaries','tables','reconstruction']: (root/sub).mkdir(exist_ok=True,parents=True)
    symbols=[dict(name=n,address=f'0x{ad:08x}',kind=k,note=nt) for n,ad,k,nt in SYMBOLS]
    metadata=[]
    for name,off,count,meaning,confidence in TABLES:
        raw=data[off:off+4*count]; values=struct.unpack('<'+'f'*count,raw)
        (root/f'tables/{name}.f32le.bin').write_bytes(raw)
        csv_write(root/f'tables/{name}.csv',['index','flash_address','raw_u32_hex','float32_exact_decimal'],
                  ((i,f'0x{BASE+off+4*i:08x}',f'0x{struct.unpack_from("<I",raw,4*i)[0]:08x}',repr(v)) for i,v in enumerate(values)))
        m=dict(name=name,offset=f'0x{off:04x}',flash_address=f'0x{BASE+off:08x}',count=count,bytes=len(raw),
               min=min(values),max=max(values),sha256=hashlib.sha256(raw).hexdigest(),
               interpretation=meaning,interpretation_confidence=confidence)
        if name=='exp2_fraction':
            expected=[2**(i/2048) for i in range(count)]
            m['max_absolute_formula_error']=max(abs(v-e) for v,e in zip(values,expected))
            m['bit_exact_correctly_rounded_formula_entries']=sum(struct.pack('<f',v)==struct.pack('<f',e) for v,e in zip(values,expected))
        if name=='halo_matrix_gain':m['normalized_matrix_gain_range']=[min(values)*math.sqrt(8),max(values)*math.sqrt(8)]
        metadata.append(m)
        symbols.append(dict(name=name,address=f'0x{BASE+off:08x}',kind='data',note=meaning))
    (root/'tables/table_manifest.json').write_text(json.dumps(metadata,indent=2)+'\n')
    header = ['#pragma once', '// Bit-exact float32 values extracted from MP86. Analyst names, not recovered source.', '#include <array>', 'namespace mp86_tables {']
    for name, off, count, meaning, confidence in TABLES:
        values = struct.unpack_from('<' + str(count) + 'f', data, off)
        header.append(f'// Flash 0x{BASE+off:08x}; {meaning}')
        header.append(f'inline constexpr std::array<float, {count}> {name} = {{{{')
        for start in range(0, count, 4):
            header.append('    ' + ', '.join(v.hex()+'f' for v in values[start:start+4]) + ',')
        header.append('}};')
    header.append('} // namespace mp86_tables')
    (root/'reconstruction/exact_tables.hpp').write_text('\n'.join(header)+'\n')

    minima=struct.unpack_from('<8f',data,0xbe10)
    scales=[16,4,4,4,4,4,4,16]
    csv_write(root/'tables/zones_48000_nominal.csv',['zone','minimum_samples','maximum_samples_nominal','minimum_ms','maximum_ms_nominal','range_factor'],
              ((i,v,v*scales[i],v/48,v*scales[i]/48,scales[i]) for i,v in enumerate(minima)))
    # These are assembled from immediate stores, not a contiguous flash table.
    ratios=[.5,4/7,.6,2/3,.75,.8,1,1.25,4/3,1.5,5/3,1.75,2]
    ratios=[struct.unpack('<f',struct.pack('<f',v))[0] for v in ratios]
    csv_write(root/'tables/tempo_ratios.csv',['index','runtime_address','delay_ratio_float32','reciprocal_rate_ratio'],
              ((i,f'0x{0x20003804+4*i:08x}',v,1/v) for i,v in enumerate(ratios)))
    (root/'tables/tempo_ratios_reconstructed.f32le.bin').write_bytes(struct.pack('<13f',*ratios))
    H=[[1 if (i&j).bit_count()%2==0 else -1 for j in range(8)] for i in range(8)]
    csv_write(root/'tables/halo_hadamard8.csv',['output_row']+[f'tap_{i}' for i in range(8)],([i]+row for i,row in enumerate(H)))
    taps=[1838,2241,2662,3128,3688,4251,4861,5539]
    writes=[0,1839,2242,2663,3129,3689,4252,4862]
    csv_write(root/'tables/halo_aux_ring_taps.csv',['line','read_offset_samples','write_offset_samples','read_minus_write_samples','difference_ms_at_nominal_48k'],
              ((i,rd,wr,rd-wr,(rd-wr)/48) for i,(rd,wr) in enumerate(zip(taps,writes))))
    csv_write(root/'tables/halo_short_allpasses.csv',['runtime_base','samples','nominal_ms','allpass_coefficient','read_interpolation'],
              [('0x20001b6c',969,969/48,.7,'Linear interpolation with modulated fractional read'),
               ('0x20002b64',803,803/48,.7,'Linear interpolation with modulated fractional read'),
               ('0x200038e8',1236,1236/48,.4,'Integer ring read in audited path'),
               ('0x20000278',1511,1511/48,.4,'Integer ring read in audited path')])
    # Vectors: label only interrupts verified against the ST primary header.
    core=['initial_sp','Reset','NMI','HardFault','MemManage','BusFault','UsageFault','reserved','reserved','reserved','reserved','SVCall','DebugMonitor','reserved','PendSV','SysTick']
    irq={13:'DMA1_Stream2',28:'TIM2',47:'DMA1_Stream7',56:'DMA2_Stream0',57:'DMA2_Stream1',68:'DMA2_Stream5'}
    vec=struct.unpack_from('<114I',data)
    csv_write(root/'analysis/vectors.csv',['index','vector_address','irq_number','name','stored_u32','code_address','thumb_bit','default_handler'],
              ((i,f'0x{BASE+4*i:08x}',i-16 if i>=16 else '',core[i] if i<16 else irq.get(i-16,f'IRQ_{i-16}'),
                f'0x{v:08x}',f'0x{v&~1:08x}' if i and v else '',bool(v&1) if i else '',v==0x08027c95)
               for i,v in enumerate(vec)))
    strings=[]
    for m in re.finditer(rb'[\x20-\x7e]{6,}',data):
        strings.append([f'0x{m.start():04x}',f'0x{BASE+m.start():08x}',len(m.group()),m.group().decode('ascii'),
                        'diagnostic_region' if 0x8c7c<=m.start()<0x8dc4 else 'unclassified_may_be_instruction_bytes'])
    csv_write(root/'analysis/strings_all.csv',['file_offset','flash_address','length','text','classification'],strings)
    diagnostics=data[0x8c7c:0x8dc4].replace(b'\0',b'\n').decode('ascii',errors='backslashreplace')
    (root/'analysis/diagnostic_strings.txt').write_text(diagnostics)
    for name,start,end in [('vectors',0,0x1c8),('code_region_with_literal_pools',0x1c8,0x8c7c),('all_dsp_lookup_tables',0x8dc4,0xbdc4),('ram_initializer_20000000',0xbe08,0xbf7c),('preserved_zero_tail',0xbf7c,0xc000)]:
        (root/f'binaries/{name}.bin').write_bytes(data[start:end])
    (root/'binaries/mimeophon_mp86.hex').write_text(make_ihex(data))
    (root/'binaries/mimeophon_mp86_analysis_wrapper.elf').write_bytes(make_elf(data,symbols))
    (root/'analysis/curated_symbols.json').write_text(json.dumps(symbols,indent=2)+'\n')
    csv_write(root/'analysis/curated_symbols.csv',['name','address','kind','note'],([s[k] for k in ['name','address','kind','note']] for s in symbols))
    ranges=[
      ('application_flash',BASE,len(data),'RX','Recovered bytes, not whole device flash'),
      ('runtime_data',0x20000000,0x174,'RW','Copied from flash offset 0xbe08 by startup'),
      ('runtime_bss',0x20000174,0x5a80-0x174,'RW','Zeroed by startup'),
      ('halo_auxiliary_float_ring',0xd0000000,0x20000,'RW','32768 floats; 8-tap feedback network'),
      ('main_delay_ring_A',0xd0800000,0x800000,'RW','2097152 floats; four head structs share A/B'),
      ('main_delay_ring_B',0xd1000000,0x800000,'RW','2097152 floats; four head structs share A/B'),
      ('nonvolatile_settings_scan',0x080c0000,0x40000,'R?','Referenced by startup; contents NOT in this update'),
    ]
    (root/'analysis/memory_map.json').write_text(json.dumps([dict(name=n,base=f'0x{ad:08x}',bytes=sz,end_exclusive=f'0x{ad+sz:08x}',permissions=perm,note=note) for n,ad,sz,perm,note in ranges],indent=2)+'\n')
    print(f'Exported {len(TABLES)} exact flash tables, vectors, synthetic symbols, HEX and analysis ELF.')

if __name__=='__main__': main()
