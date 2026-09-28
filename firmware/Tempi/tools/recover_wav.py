#!/usr/bin/env python3
"""Recover the exact digital Tempi update WAV supplied with this study.
Requires NumPy. Not an analog/noisy-recording demodulator. Run from any directory.
All Intel HEX lengths and checksums are checked; duplicate conflicting bytes fail.
The original PCM is reconstructed from payload bits + pause positions and compared.
"""
from pathlib import Path
import argparse, collections, csv, hashlib, json, wave
import numpy as np
ROOT=Path(__file__).resolve().parents[1]
def sha(b): return hashlib.sha256(b).hexdigest()
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('wav', nargs='?', type=Path, default=ROOT/'input/tempi71.wav')
    p.add_argument('--output', type=Path, default=ROOT)
    args=p.parse_args(); out=args.output
    for d in ['firmware','transport','analysis']: (out/d).mkdir(parents=True,exist_ok=True)
    with wave.open(str(args.wav),'rb') as w:
        props=(w.getnchannels(),w.getsampwidth(),w.getframerate(),w.getnframes())
        if props[:3]!=(1,2,40000): raise ValueError('Expected mono 16-bit 40000 Hz source')
        raw=w.readframes(w.getnframes())
    x=np.frombuffer(raw,'<i2'); nonzero=np.flatnonzero(x)
    if not len(nonzero): raise ValueError('Silent file')
    start=int(nonzero[0]); cuts=np.unique(np.r_[start,np.flatnonzero(x[1:]!=x[:-1])+1,len(x)])
    cuts=cuts[cuts>=start]; ds=np.diff(cuts); levels=x[cuts[:-1]]
    if not set(map(int,np.unique(ds)))<={3,10,350}: raise ValueError('Unexpected run lengths')
    if not np.all(np.abs(levels.astype(np.int32))==32767): raise ValueError('Unexpected run amplitude')
    if not np.all(levels[1:]==-levels[:-1]): raise ValueError('Expected alternating half-cycles')
    flags=ds!=350; bits=(ds[flags]==3).astype(np.uint8)
    if len(bits)%8: raise ValueError('Incomplete byte')
    payload=np.packbits(bits,bitorder='big').tobytes()
    text=payload.decode('ascii'); mem={}; base=0; records=[]; charoff=0
    samples=cuts[:-1][flags]
    for lineno,line in enumerate(text.splitlines(keepends=True),1):
        s=line.strip()
        if not s.startswith(':'): raise ValueError(f'Bad HEX prefix at line {lineno}')
        rec=bytes.fromhex(s[1:]); count=rec[0]; addr=int.from_bytes(rec[1:3],'big'); typ=rec[3]
        if len(rec)!=count+5 or sum(rec)&255: raise ValueError(f'Bad HEX length/checksum {lineno}')
        data=rec[4:-1]; effective=base+addr
        if typ==4:
            if count!=2: raise ValueError('Bad extended address record')
            base=int.from_bytes(data,'big')<<16
        elif typ==0:
            for j,v in enumerate(data):
                a=effective+j
                if a in mem and mem[a]!=v: raise ValueError(f'Conflicting data at {a:#x}')
                mem[a]=v
        elif typ!=1: raise ValueError(f'Unsupported record {typ}')
        records.append({'line':lineno,'type':typ,'address':f'0x{effective:06X}' if typ==0 else '',
                        'count':count,'checksum_ok':True,'payload_byte_offset':charoff,
                        'first_sample':int(samples[charoff*8]),
                        'first_time_seconds':float(samples[charoff*8]/props[2])})
        charoff+=len(line)
    if records[-1]['type']!=1: raise ValueError('Missing EOF record')
    (out/'firmware/tempi71_recovered.hex').write_bytes(payload)
    ranges=[]
    for a in sorted(mem):
        if ranges and a==ranges[-1][1]+1:ranges[-1][1]=a
        else:ranges.append([a,a])
    regions=[]
    for a,b in ranges:
        bb=bytes(mem[j] for j in range(a,b+1)); name=f'tempi71_{a:06x}_{b:06x}.bin'
        (out/'firmware'/name).write_bytes(bb)
        regions.append({'start':f'0x{a:06X}','end_inclusive':f'0x{b:06X}','length':len(bb),'file':name,'sha256':sha(bb)})
    image=bytearray([255])*65536; presence=bytearray(65536)
    for a,b in mem.items():
        if a<65536:image[a]=b;presence[a]=255
    (out/'firmware/tempi71_flash64k_FF_FILLED.bin').write_bytes(image)
    (out/'firmware/tempi71_flash64k_presence_mask.bin').write_bytes(presence)
    # Pause positions count preceding data bits. Lengths of data half-cycles are
    # obtained afresh from recovered payload, not copied from the original signal.
    pauses=[]; preceding=0
    for d in ds:
        if d==350:pauses.append(preceding)
        else:preceding+=1
    bit2=np.unpackbits(np.frombuffer(payload,dtype=np.uint8),bitorder='big')
    dl=np.where(bit2==1,3,10)
    rebuilt=np.empty(len(ds),dtype=np.int64); bi=pi=ri=0
    while ri<len(rebuilt):
        if pi<len(pauses) and pauses[pi]==bi:rebuilt[ri]=350;pi+=1
        else:rebuilt[ri]=dl[bi];bi+=1
        ri+=1
    lev=np.where(np.arange(len(rebuilt))%2==0,int(levels[0]),-int(levels[0])).astype('<i2')
    recreated=np.r_[np.zeros(start,dtype='<i2'),np.repeat(lev,rebuilt)].astype('<i2').tobytes()
    if recreated!=raw: raise ValueError('PCM reconstruction mismatch')
    starts=np.flatnonzero(flags & ~np.r_[False,flags[:-1]])
    ends=np.flatnonzero(flags & ~np.r_[flags[1:],False])+1
    bitcum=np.r_[0,np.cumsum(flags)]
    bursts=[{'burst':j,'first_sample':int(cuts[a]),'end_sample_exclusive':int(cuts[b]),
             'payload_bit_offset':int(bitcum[a]),'bits':int(b-a)} for j,(a,b) in enumerate(zip(starts,ends))]
    timing={'sample_rate':props[2],'leading_zero_samples':start,'first_level':int(levels[0]),
            'one_halfcycle_samples':3,'zero_halfcycle_samples':10,'pause_halfcycle_samples':350,
            'pause_after_data_bit_counts':pauses,'bit_order':'MSB first','pcm_round_trip_exact':True,
            'original_pcm_sha256':sha(raw),'rebuilt_pcm_sha256':sha(recreated)}
    (out/'transport/timing.json').write_text(json.dumps(timing,indent=2)+'\n')
    for name,rows in [('record_sample_map.csv',records),('data_bursts.csv',bursts)]:
        with (out/'transport'/name).open('w',newline='') as f:
            cw=csv.DictWriter(f,fieldnames=list(rows[0]));cw.writeheader();cw.writerows(rows)
    (out/'analysis/hex_records.json').write_text(json.dumps(records,indent=2)+'\n')
    info={'source':str(args.wav.name),'source_size':args.wav.stat().st_size,'source_sha256':sha(args.wav.read_bytes()),
          'channels':props[0],'sample_width_bytes':props[1],'sample_rate':props[2],'samples':props[3],
          'duration_seconds':props[3]/props[2],'halfcycle_length_counts':dict(collections.Counter(map(int,ds))),
          'payload_ascii_bytes':len(payload),'payload_sha256':sha(payload),'data_bits':len(bits),
          'records':len(records),'record_types':dict(collections.Counter(r['type'] for r in records)),
          'recovered_bytes':len(mem),'regions':regions,'data_bursts':len(bursts),
          'data_burst_bit_length_counts':dict(collections.Counter(b['bits'] for b in bursts)),
          'pcm_round_trip_exact':True,'missing_bootloader_range':'0x000000..0x0007FF',
          'warning':'FF-filled flash gaps are NOT recovered bytes. Presence mask is authoritative.'}
    (out/'analysis/recovery.json').write_text(json.dumps(info,indent=2)+'\n')
    print(json.dumps(info,indent=2))
if __name__=='__main__': main()
