#!/usr/bin/env python3
"""Export immutable SP67 data and an analysis-only ELF; never executes firmware."""
from pathlib import Path
import csv, struct, hashlib, json, math, re, collections
ROOT=Path(__file__).resolve().parents[1]; B=(ROOT/'firmware/sp67.bin').read_bytes(); BASE=0x08020000
EXPECTED='b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9'
assert hashlib.sha256(B).hexdigest()==EXPECTED, 'Unexpected firmware: addresses are SP67-specific'
def csvout(path,rows):
    with path.open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
tables=[
 ('waveform_ramp_1024',0x08036674,1024,'Waveform; output-shape association requires further call-site tracing'),
 ('waveform_sine_1024',0x08037674,1024,'Full-period sine waveform'),
 ('waveform_triangle_1024',0x08038674,1024,'Triangle-like sampled waveform; preserve exact deviations'),
 ('waveform_shaped_1024',0x08039674,1024,'Strongly shaped periodic waveform; do not assign a panel label without tracing'),
 ('sine_8192',0x0803a674,8192,'Interpolated sine reference used by oscillator/analyzer'),
 ('noise_rate_256',0x08042674,256,'Geometric coefficients used in Noise path; 24 steps per octave'),
 ('focus_exponent_1024',0x08042a74,1024,'Linear Array Focus bit-domain power exponents'),
 ('exp2_2048',0x08043a74,2048,'One-octave exponential table'),
 ('factory_spectra_64x64',0x08045a98,4096,'Default spectral Array: 64 frames of 64 float32 coefficients')]
manifest=[]
for name,addr,n,note in tables:
    raw=B[addr-BASE:addr-BASE+4*n]; vals=struct.unpack('<%df'%n,raw)
    (ROOT/'tables'/f'{name}.bin').write_bytes(raw)
    rows=[{'index':i,'address':f'0x{addr+4*i:08x}','bits':f'0x{struct.unpack_from("<I",raw,4*i)[0]:08x}','value':format(v,'.9g')} for i,v in enumerate(vals)]
    csvout(ROOT/'tables'/f'{name}.csv',rows)
    info=dict(name=name,address=f'0x{addr:08x}',file_offset=f'0x{addr-BASE:x}',count=n,encoding='little-endian IEEE754 float32',bytes=len(raw),minimum=min(vals),maximum=max(vals),sha256=hashlib.sha256(raw).hexdigest(),note=note)
    if name=='sine_8192' or name=='waveform_sine_1024': info['max_abs_error_vs_sin_2pi_i_over_N']=max(abs(v-math.sin(2*math.pi*i/n)) for i,v in enumerate(vals))
    if name=='exp2_2048': info['max_abs_error_vs_exp2_i_over_2048']=max(abs(v-2**(i/2048)) for i,v in enumerate(vals))
    if name=='focus_exponent_1024': info['max_abs_error_vs_1p25_times_p4_pow_i_over_1023']=max(abs(v-1.25*.4**(i/1023)) for i,v in enumerate(vals))
    if name=='noise_rate_256': info['max_rel_error_vs_geometric_24_per_octave']=max(abs(v/(vals[0]*2**(i/24))-1) for i,v in enumerate(vals))
    if name=='factory_spectra_64x64':
        with (ROOT/'tables/factory_spectra_frames.csv').open('w',newline='') as f:
            w=csv.writer(f);w.writerow(['frame']+[f'coefficient_{i:02}' for i in range(64)])
            for i in range(64):w.writerow([i]+[format(v,'.9g') for v in vals[i*64:(i+1)*64]])
    manifest.append(info)
(ROOT/'tables/manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
strings=[{'offset':f'0x{m.start():x}','address':f'0x{BASE+m.start():08x}','length':len(m.group()),'text':m.group().decode()} for m in re.finditer(rb'[ -~]{5,}',B)]
csvout(ROOT/'analysis/strings.csv',strings)
(ROOT/'analysis/strings.txt').write_text('\n'.join(f"{r['address']} {r['text']}" for r in strings)+'\n')
def entropy(data):
    c=collections.Counter(data); n=len(data)
    return -sum((v/n)*math.log2(v/n) for v in c.values())
blocks=[]
for off in range(0,len(B),4096):
    raw=B[off:off+4096]; blocks.append({'offset':f'0x{off:x}','address':f'0x{BASE+off:08x}','length':len(raw),'entropy_bits_per_byte':entropy(raw),'zero_fraction':raw.count(0)/len(raw)})
csvout(ROOT/'analysis/entropy_4k.csv',blocks)
last=max(i for i,v in enumerate(B) if v)
identity={'filename':'sp67.dat','bytes':len(B),'sha256':EXPECTED,'application_base':'0x08020000','initial_sp':f'0x{struct.unpack_from("<I",B,0)[0]:08x}','reset_vector':f'0x{struct.unpack_from("<I",B,4)[0]:08x}','last_nonzero_file_offset':hex(last),'trailing_zero_bytes':len(B)-last-1,'whole_file_entropy':entropy(B),'scope':'Uploaded file only; no bootloader or hardware execution'}
(ROOT/'analysis/identity.json').write_text(json.dumps(identity,indent=2)+'\n')
# Analyst-assigned names, NOT recovered source/debug symbols. Confidence is per label.
S=[
(0x0802af74,'fatfs_like_open','high','File API inferred from arguments, call sites and object use'),
(0x0802b264,'fatfs_like_read','high','Buffer/length/bytes-read API'),
(0x0802b4cc,'fatfs_like_write','high','Buffer/length/bytes-written API'),
(0x0802b794,'fatfs_like_sync','medium','Filesystem finalization path'),
(0x0802b840,'fatfs_like_close','high','File lifetime end'),
(0x0802b944,'fatfs_like_seek','high','Offset positioning'),
(0x0802bef8,'fatfs_like_findfirst','high','Pattern enumeration'),
(0x0802bf88,'fatfs_like_truncate','medium','End-of-file finalization path'),
(0x0802c4e0,'application_state_init','high','Application state initialization'),
(0x0802cd0c,'array_linear_A','high','64-coefficient interpolation and Focus transform'),
(0x0802ce5c,'array_linear_B','high','Mirrored reader'),
(0x0802cfac,'array_planar_A','high','Four-frame bilinear interpolation'),
(0x0802d0f8,'array_planar_B','high','Mirrored reader'),
(0x0802d280,'ui_state_service_candidate','medium','Persistent UI state interactions; not fully translated'),
(0x0802d464,'control_acquisition_candidate','medium','Control/CV conversion path'),
(0x0802d900,'button_clock_state_service','medium','Button and clock state processing'),
(0x0802e750,'audio_block_process','high','64-frame audio callback target'),
(0x08032678,'audio_half_callback_offset128','high','Leaf wrapper passes word offset 128'),
(0x08032680,'audio_half_callback_offset0','high','Leaf wrapper passes word offset 0'),
(0x08032688,'gpio_clock_callback','high','GPIO 64/128 branch to two clock counters'),
(0x08033094,'riff_wave_parse','high','RIFF/WAVE/fmt/data parsing'),
(0x080332e4,'persistent_file_open_candidate','medium','Settings/file path; semantics incomplete'),
(0x080333c4,'restore_factory_array_A','high','Copies default spectral data'),
(0x080333fc,'restore_factory_array_B','high','Copies default spectral data'),
(0x08033434,'wave_decode_samples','high','PCM and float conversion branches'),
(0x080336f0,'wave_header_initialize','high','Actual entry includes instructions before push at +4'),
(0x080337cc,'wave_header_finalize','high','RIFF header size/format and finalization'),
(0x08033ab0,'wave_read_rewind_wrapper','high','Generic reader wrapper'),
(0x08033af8,'save_unsaved_array_slots','high','Per-side slot enumeration, normalization and 16-bit write'),
(0x08034060,'application_main','high','Reset-handler call target'),
(0x08035b40,'system_init','high','Processor/peripheral low-level initialization'),
(0x08036430,'reset_handler','high','Vector, data copying and BSS zero'),
(0x08036480,'default_handler','high','Unused interrupt vector target'),
(0x08036494,'libc_init_array_candidate','high','Startup constructors')]
rows=[dict(address=f'0x{a:08x}',name=n,kind='function',confidence=c,evidence=e) for a,n,c,e in S]
for name,addr,n,note in tables: rows.append(dict(address=f'0x{addr:08x}',name=name,kind='object',confidence='high',evidence=note))
csvout(ROOT/'analysis/symbols_curated.csv',rows)
# Build a synthetic ARM ELF32 wrapper. Single unsplit image segment; not original ELF.
def align(n,a):return (n+a-1)//a*a
strtab=bytearray(b'\0'); syms=bytearray(16)
for row in rows:
    no=len(strtab);strtab+=row['name'].encode()+b'\0';addr=int(row['address'],16)
    typ=2 if row['kind']=='function' else 1
    if typ==2:addr|=1
    syms+=struct.pack('<IIIBBH',no,addr,0,(1<<4)|typ,0,1)
shnames=['','.firmware','.symtab','.strtab','.shstrtab'];shstr=bytearray(b'\0');sno=[0]
for name in shnames[1:]:sno.append(len(shstr));shstr+=name.encode()+b'\0'
load_off=0x1000; out=bytearray(load_off);out+=B
symoff=align(len(out),4);out+=b'\0'*(symoff-len(out));out+=syms
stroff=len(out);out+=strtab
shstroff=len(out);out+=shstr
shoff=align(len(out),4);out+=b'\0'*(shoff-len(out));out+=bytes(40)
for tup in [(sno[1],1,6,BASE,load_off,len(B),0,0,4,0),(sno[2],2,0,0,symoff,len(syms),3,1,4,16),(sno[3],3,0,0,stroff,len(strtab),0,0,1,0),(sno[4],3,0,0,shstroff,len(shstr),0,0,1,0)]:out+=struct.pack('<10I',*tup)
ident=b'\x7fELF'+bytes([1,1,1,0])+bytes(8)
out[:52]=struct.pack('<16sHHIIIIIHHHHHH',ident,2,40,1,0x08036431,52,shoff,0x05000000,52,32,1,40,5,4)
out[52:84]=struct.pack('<8I',1,load_off,BASE,BASE,len(B),len(B),5,0x1000)
(ROOT/'firmware/sp67_analysis_mapped.elf').write_bytes(out)
print('Exported',len(manifest),'tables;',len(rows),'curated labels; ELF',len(out),'bytes')
