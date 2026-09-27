"""Recover the supplied MG204 ZIP/WAV; preserve all 644 CRC-verified payloads.

Unlike the legacy decoder, packet boundaries do not depend on stripping trailing
zero bytes from a CRC. This is deliberately version-specific, not a universal modem.
"""
import argparse, io, struct, wave, zipfile, zlib
from pathlib import Path
import numpy as np
from common import ROOT, BASE, FLASH_HASH, WAV_HASH, ZIP_HASH, sha, put_json, put_csv
SYNC=bytes.fromhex('99999999cccccccc')

def demodulate(wav_bytes):
    with wave.open(io.BytesIO(wav_bytes),'rb') as w:
        if (w.getnchannels(),w.getsampwidth(),w.getframerate(),w.getcomptype())!=(1,2,48000,'NONE'):
            raise ValueError('Expected mono signed PCM16 at 48000 Hz')
        frames=w.getnframes()
        x=np.frombuffer(w.readframes(frames),dtype='<i2')
    nz=np.flatnonzero(x)
    if not len(nz): raise ValueError('No carrier')
    start=int(nz[0])
    if (len(x)-start)%8: raise ValueError('Partial symbol')
    blocks=x[start:].reshape(-1,8).astype(np.float64)
    phase=(np.angle(blocks@np.exp(-2j*np.pi*np.arange(8)/8),deg=True)+360)%360
    quadrants=np.rint((phase-45)/90).astype(np.int64)%4
    err=np.abs((phase-(quadrants*90+45)+180)%360-180)
    if err.max()>1: raise ValueError('Carrier phase error exceeds one degree')
    dibits=np.array([2,0,1,3],dtype=np.uint8)[quadrants]
    if len(dibits)%4: raise ValueError('Partial byte')
    d=dibits.reshape(-1,4)
    raw=((d[:,0]<<6)|(d[:,1]<<4)|(d[:,2]<<2)|d[:,3]).tobytes()
    return raw,dict(wav_frames=frames,wav_seconds=frames/48000,carrier_start_sample=start,
                    symbol_count=len(dibits),maximum_phase_error_degrees=float(err.max()))

def deframe(raw,start_sample=48000):
    sync=raw.find(SYNC)
    if sync!=1508 or raw[:sync]!=bytes(sync): raise ValueError('Unexpected preamble')
    p=sync+8;out=bytearray();rows=[]
    for n in range(644):
        data=raw[p:p+256]
        if len(data)!=256 or p+260>len(raw): raise ValueError(f'Truncated packet {n}')
        stored=struct.unpack_from('>I',raw,p+256)[0];computed=zlib.crc32(data)&0xffffffff
        if stored!=computed: raise ValueError(f'CRC mismatch in packet {n}')
        sample=start_sample+p*32
        rows.append((n,p,sample,sample/48000,n*256,f'0x{BASE+n*256:08x}',
                     f'{stored:08x}',f'{computed:08x}',sha(data)))
        out+=data;p+=260
        if n<643:
            pad=23 if n%4==3 else 8
            if raw[p:p+pad]!=bytes(pad):raise ValueError(f'Invalid padding after {n}')
            p+=pad
            if raw[p:p+8]!=SYNC:raise ValueError(f'Invalid sync after {n}')
            p+=8
    if len(raw)-p!=1515 or raw[p:]!=bytes(1515):raise ValueError('Unexpected trailer')
    return bytes(out),rows,dict(preamble_zero_bytes=sync,trailer_zero_bytes=len(raw)-p,
                              payload_stream_start=sync+8,payload_stream_end=p)

def run(source,out):
    data=source.read_bytes();archive_hash=None
    if zipfile.is_zipfile(source):
        archive_hash=sha(data)
        with zipfile.ZipFile(io.BytesIO(data)) as z:data=z.read('mg204.wav')
    if sha(data)!=WAV_HASH:raise ValueError('Not the audited original MG204 WAV')
    raw,meta=demodulate(data);b,rows,framing=deframe(raw,meta['carrier_start_sample'])
    if sha(b)!=FLASH_HASH:raise ValueError('CRC-valid image has unexpected version/hash')
    (out/'protocol').mkdir(parents=True,exist_ok=True);(out/'binaries').mkdir(exist_ok=True)
    (out/'protocol/demodulated_stream.bin').write_bytes(raw)
    (out/'binaries/morphagene_mg204.bin').write_bytes(b)
    put_csv(out/'protocol/packets.csv',['packet','stream_payload_offset','wav_payload_sample','wav_payload_seconds',
        'image_offset','flash_address','stored_crc32_be','computed_crc32','payload_sha256'],rows)
    meta.update(framing);meta.update(dict(wav_sha256=sha(data),archive_sha256=archive_hash,flash_sha256=sha(b),
        flash_base=hex(BASE),flash_bytes=len(b),packet_count=644,packet_payload_bytes=256,crc_valid_packets=len(rows),
        stream_bytes=len(raw),modulation='QPSK',samples_per_symbol=8,sample_rate=48000,carrier_hz=6000,
        phase_dibits={'45':'10','135':'00','225':'01','315':'11'},packing='MSB first',
        crc='IEEE CRC-32 / zlib.crc32, stored big-endian',
        missing=['resident bootloader','per-device calibration/settings','live RAM','SD card recordings']))
    put_json(out/'protocol/transport.json',meta)
    print(f'644/644 CRC-valid packets; {len(b)} firmware bytes; SHA-256 {sha(b)}')
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('source',type=Path,nargs='?',default=ROOT/'mg204_firmware.zip')
    p.add_argument('--out',type=Path,default=ROOT);a=p.parse_args();run(a.source,a.out)
