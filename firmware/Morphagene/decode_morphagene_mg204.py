#!/usr/bin/env python3
import argparse, wave, numpy as np

PHASE_TO_DIBIT = {1:2, 3:0, 5:1, 7:3}  # 45,135,225,315 deg -> 2-bit symbol
SYNC = bytes.fromhex('99 99 99 99 cc cc cc cc')

def decode(wav_path):
    with wave.open(wav_path,'rb') as w:
        if (w.getnchannels(), w.getsampwidth(), w.getframerate()) != (1,2,48000):
            raise ValueError('expected mono 16-bit 48 kHz WAV')
        x=np.frombuffer(w.readframes(w.getnframes()),dtype='<i2').astype(np.float64)/32768.0
    start=48000  # one second initial silence
    N=8
    m=(len(x)-start)//N
    blocks=x[start:start+m*N].reshape(m,N)
    osc=np.exp(-1j*2*np.pi*6000*np.arange(N)/48000.0)
    z=blocks@osc
    phase=((np.round(((np.angle(z)%(2*np.pi))/(np.pi/4))))%8).astype(np.int8)
    valid=np.isin(phase,[1,3,5,7])
    if not valid.all():
        raise ValueError('unexpected phase state encountered')
    non=np.flatnonzero(phase!=3)
    first=int(non[0])
    last=int(non[-1])
    # Include idle phase-3 symbols only as needed to finish the final byte.
    end=last+1
    end += (- (end-first)) % 4
    s=phase[first:end]
    vals=np.fromiter((PHASE_TO_DIBIT[int(v)] for v in s), dtype=np.uint8, count=len(s))
    vals=vals[:len(vals)//4*4].reshape(-1,4)
    data=((vals[:,0]<<6)|(vals[:,1]<<4)|(vals[:,2]<<2)|vals[:,3]).astype(np.uint8).tobytes()
    if not data.startswith(SYNC):
        raise ValueError(f'sync mismatch: {data[:8].hex()}')
    return first, data

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('wav')
    ap.add_argument('--raw',default='mg204_qpsk_decoded.bin')
    ap.add_argument('--firmware',default='mg204_firmware_image.bin')
    args=ap.parse_args()
    first,data=decode(args.wav)
    open(args.raw,'wb').write(data)
    open(args.firmware,'wb').write(data[8:])
    print(f'first data symbol: {first} ({1+first/6000:.6f}s in WAV)')
    print(f'sync: {data[:8].hex(" ")}')
    print(f'wrote {len(data)} bytes to {args.raw}')
    print(f'wrote {len(data)-8} bytes to {args.firmware}')

if __name__=='__main__': main()
