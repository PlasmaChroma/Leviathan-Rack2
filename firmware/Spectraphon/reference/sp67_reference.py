#!/usr/bin/env python3
"""Executable probes for recovered SP67 equations, NOT a complete Spectraphon emulator.

No original firmware execution, hardware I/O or flashing. Python 3.10+; stdlib only.
Numerical helpers use float32 where indicated; the polynomial/analyzer probes are
mathematical translations, not instruction-scheduled Cortex-M7 implementations.
"""
from __future__ import annotations
import argparse, ctypes, ctypes.util, hashlib, json, math, struct, functools
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import Sequence
ROOT=Path(__file__).resolve().parents[1]

def f32(x: float) -> float:
    return struct.unpack('<f',struct.pack('<f',float(x)))[0]
def bits(x: float) -> int:
    return struct.unpack('<I',struct.pack('<f',float(x)))[0]
def from_bits(x: int) -> float:
    return struct.unpack('<f',struct.pack('<I',x & 0xffffffff))[0]
def signed32(x: int) -> int:
    x &= 0xffffffff
    return x if x < 0x80000000 else x-0x100000000

# Native fmaf avoids the double-rounding ambiguity of f32(a*b+c).
_fmaf=None
for libname in (ctypes.util.find_library('m'), None):
    try:
        lib=ctypes.CDLL(libname); fun=lib.fmaf
        fun.argtypes=(ctypes.c_float,ctypes.c_float,ctypes.c_float);fun.restype=ctypes.c_float
        _fmaf=fun;break
    except (OSError,AttributeError): pass

def fma32(a: float,b: float,c: float) -> float:
    if _fmaf is None:
        raise RuntimeError('A C library providing fmaf is required for this exact float32 helper')
    return float(_fmaf(f32(a),f32(b),f32(c)))

@functools.lru_cache(maxsize=16)
def table(name: str) -> tuple[float,...]:
    manifest=json.loads((ROOT/'tables/manifest.json').read_text())
    info=next((x for x in manifest if x['name']==name),None)
    if info is None: raise ValueError('Unknown extracted table: '+name)
    raw=(ROOT/'tables'/f'{name}.bin').read_bytes()
    if hashlib.sha256(raw).hexdigest()!=info['sha256']:raise ValueError('Table hash mismatch: '+name)
    return struct.unpack('<%df'%info['count'],raw)

def fast_sqrt_bits(energy: float) -> float:
    """Literal unsigned transform. Zero/subnormals deliberately NOT repaired."""
    if not math.isfinite(energy) or energy<0:raise ValueError('Expected finite nonnegative energy')
    return from_bits((((bits(energy)-0x00800000)&0xffffffff)>>1)+0x20000000)

def focus_transform(amplitude: float,focus: float) -> float:
    """Linear-reader bit-domain power transform, including its exact bypass."""
    if not math.isfinite(amplitude) or not 0<=focus<=1:raise ValueError('Invalid amplitude or Focus')
    a=f32(amplitude);f=f32(focus); k=f32(f32(.75)*f)
    if a<=0 or k==f32(.25):return a
    idx=min(math.trunc(f*1024),1023)
    expo=table('focus_exponent_1024')[idx]
    t=signed32(bits(a)+0xc0876c0b)
    y=fma32(expo,f32(t),f32(1064866816))
    if not -2147483648<=y<2147483648:raise ValueError('Bit power conversion outside signed32 range')
    approx=from_bits(math.trunc(y));gain=f32(f32(1.25)-k)
    return f32(gain*approx)

def sine_lookup(phase_cycles: float) -> float:
    """Positive wrapped phase, 8192-point interpolation; not a full phase engine."""
    if not math.isfinite(phase_cycles):raise ValueError('Nonfinite phase')
    vals=table('sine_8192');p=f32((phase_cycles%1)*8192)
    i=math.trunc(p); frac=f32(p-i)
    return fma32(frac,f32(vals[(i+1)&8191]-vals[i&8191]),vals[i&8191])

def analysis_parameters(slide: float,focus: float) -> tuple[float,float]:
    """Return analyzer reference increment (cycles/sample) and two-pole coefficient."""
    if not 0<=slide<=1 or not 0<=focus<=1:raise ValueError('Slide and Focus must be normalized')
    q=math.trunc(f32(f32(slide)*f32(8191)))
    inc=f32(table('exp2_2048')[q&2047]*(1<<(q>>11))/2400)
    v=f32(1-f32(f32(.98)*f32(focus)))
    pole=min(f32(1+float(inc)*(-3.14)*float(v)*float(v)),from_bits(0x3f7ffeb0))
    # Operation scheduling above is explanatory; use assembly for bit-exact reproduction.
    return inc,pole

def active_terms(increment: float) -> int:
    """Recovered four-term loop admission/ceiling for nonnegative increments."""
    inc=f32(increment)
    if not math.isfinite(inc) or inc<0:raise ValueError('Use a nonnegative finite increment')
    if inc>=from_bits(0x3de66666):return 0
    n=4
    while n<60 and f32((n+4)*inc)<from_bits(0x3ee66666):n+=4
    return n

def partials_radius(partials: float) -> float:
    if not math.isfinite(partials) or partials<0:raise ValueError('Partials must be finite and nonnegative')
    p=f32(partials)
    return min(f32(float(f32(f32(.65)*p))+.35*float(p)*float(p)),1.0)

def compensation(r: float) -> float:
    g=from_bits(0x4118234e)-from_bits(0x41de117d)*r+from_bits(0x4202b291)*r*r-from_bits(0x41577473)*r*r*r
    return g*(25*r if r<=from_bits(0x3d23d70a) else 1)

def soft_clip(x: float) -> float:
    x=max(-1.5,min(1.5,x));return x*(1-from_bits(0x3e17b426)*x*x)

def polynomial_synthesis(coefficients: Sequence[float],partials: float,
                         odd_phase: float,even_phase: float,terms: int=60) -> tuple[float,float]:
    """Mathematical reconstruction of the standard branch's paired recurrences.

    Independent cycle phases are explicit because FM/phase/jack routing is not
    completely reconstructed. Returns compensated but NOT output-clipped sums.
    No spectral ramping, clock logic, Noise/Chaos, ADC calibration or analog model.
    """
    if len(coefficients)!=64 or terms not in range(0,61,4):raise ValueError('Need 64 coefficients and 0..60 terms in groups of four')
    if not all(math.isfinite(v) for v in coefficients):raise ValueError('Nonfinite coefficient')
    r=partials_radius(partials)
    x=r*math.cos(2*math.pi*odd_phase)
    y=r*math.sin(2*math.pi*even_phase); z=r*math.cos(2*math.pi*even_phase)
    C=[r*r,x];E=[0.0,y]
    for n in range(2,terms+1):C.append(2*x*C[-1]-C[-2]);E.append(2*z*E[-1]-E[-2])
    odd=sum((1 if i%4==0 else -1)*coefficients[i]*C[i+1] for i in range(0,terms,2))
    even=sum((-1 if i%4==1 else 1)*coefficients[i]*E[i+1] for i in range(1,terms,2))
    g=compensation(r);return odd*g,even*g

@dataclass
class QuadratureDetector:
    """Two cascaded one-poles per quadrature; mathematical, not bit-exact DSP."""
    i1: float=0; i2: float=0; q1: float=0; q2: float=0
    def step(self,sample: float,cosine: float,sine: float,pole: float) -> float:
        if not 0<=pole<1:raise ValueError('Pole must be in [0,1)')
        i=sample*cosine;q=sample*sine
        self.i1=i+pole*(self.i1-i);self.i2=self.i1+pole*(self.i2-self.i1)
        self.q1=q+pole*(self.q1-q);self.q2=self.q1+pole*(self.q2-self.q1)
        return fast_sqrt_bits(self.i2*self.i2+self.q2*self.q2)

def _frames(frames: Sequence[Sequence[float]]) -> list[list[float]]:
    if not 1<=len(frames)<=1024:raise ValueError('Need 1..1024 spectral frames')
    result=[list(map(float,row)) for row in frames]
    if any(len(row)!=64 for row in result):raise ValueError('Each frame must contain exactly 64 coefficients')
    if any(not math.isfinite(v) for row in result for v in row):raise ValueError('Nonfinite coefficient')
    return result

def _wrap_once(index: int,n: int) -> int:
    index=index-n if index>=n else index
    if not 0<=index<n:raise ValueError('Recovered single-wrap addressing would leave the valid Array')
    return index

def linear_read(frames: Sequence[Sequence[float]],slide: float,focus: float,
                clock_offset: int=0,apply_focus: bool=True) -> list[float]:
    f=_frames(frames);n=len(f)
    if not 0<=slide<=1 or not 0<=focus<=1:raise ValueError('Controls must be in [0,1]')
    pos=fma32(f32(n-1),f32(slide),f32(clock_offset));i=math.trunc(pos);t=f32(pos-i)
    a=f[_wrap_once(i,n)];b=f[_wrap_once(i+1,n)]
    result=[fma32(t,f32(y-x),f32(x)) for x,y in zip(a,b)]
    return [focus_transform(v,focus) for v in result] if apply_focus else result

def planar_read(frames: Sequence[Sequence[float]],slide: float,focus: float,
                clock_offset: int=0) -> list[float]:
    """Preserves the unusual one-subtraction edge rule; rejects unsafe addresses."""
    f=_frames(frames);n=len(f)
    if not 0<=slide<=1 or not 0<=focus<=1:raise ValueError('Controls must be in [0,1]')
    g=math.ceil(math.sqrt(n));h=g-1
    x=fma32(f32(focus),f32(h),f32(clock_offset));y=f32(f32(slide)*f32(h))
    xi=math.trunc(x);yi=math.trunc(y);tx=f32(x-xi);ty=f32(y-yi)
    row0=yi-h if yi>=g else yi;row1=yi+1-h if yi>=h else yi+1
    indices=[row0*g+xi,row0*g+xi+1,row1*g+xi,row1*g+xi+1]
    a,b,c,d=[f[_wrap_once(i,n)] for i in indices]
    out=[]
    for aa,bb,cc,dd in zip(a,b,c,d):
        low=fma32(tx,f32(bb-aa),f32(aa)); high=fma32(tx,f32(dd-cc),f32(cc))
        out.append(fma32(ty,f32(high-low),low))
    return out

def ramp_delta(current: Sequence[float],target: Sequence[float]) -> list[float]:
    if len(current)!=64 or len(target)!=64:raise ValueError('Need two 64-value vectors')
    return [f32(f32(b-a)*f32(1/64)) for a,b in zip(current,target)]

@dataclass
class WaveInfo:
    format_tag: int; channels: int; sample_rate: int; byte_rate: int
    block_align: int; bits_per_sample: int; sample_count: int; frame_count: int
    inferred_dimensions: tuple[int,int]; warnings: list[str]

def decode_array_wav(raw: bytes) -> tuple[WaveInfo,list[list[float]]]:
    """Strict bounded RIFF reader; standardized sample normalization, not exact MCU rounding.

    Only mono PCM8/16/24/32 and IEEE float32, no ADPCM, mu-law or WAVE_EXTENSIBLE.
    Accepts but reports byte-rate/block-alignment inconsistencies. 8 MiB file cap.
    """
    if len(raw)>8*1024*1024:raise ValueError('File exceeds inspection size limit')
    if len(raw)<12 or raw[:4]!=b'RIFF' or raw[8:12]!=b'WAVE':raise ValueError('Not RIFF/WAVE')
    end=struct.unpack_from('<I',raw,4)[0]+8
    if end<12 or end>len(raw):raise ValueError('Truncated or invalid RIFF size')
    pos=12;fmt=None;data=None;warnings=[]
    if end<len(raw):warnings.append('Trailing bytes outside declared RIFF ignored')
    while pos<end:
        if pos+8>end:raise ValueError('Truncated chunk header')
        tag=raw[pos:pos+4];size=struct.unpack_from('<I',raw,pos+4)[0];start=pos+8;stop=start+size
        if stop>end:raise ValueError('Chunk extends past RIFF bounds')
        if tag==b'fmt ':
            if fmt is not None:raise ValueError('Multiple fmt chunks')
            if size<16:raise ValueError('Short fmt chunk')
            fmt=struct.unpack_from('<HHIIHH',raw,start)
        elif tag==b'data':
            if data is not None:raise ValueError('Multiple data chunks not supported')
            data=raw[start:stop]
        pos=stop+(size&1)
        if pos>end:raise ValueError('Missing odd-chunk pad byte')
    if fmt is None or data is None:raise ValueError('Need fmt and data chunks')
    tag,ch,rate,br,ba,bps=fmt
    if ch!=1:raise ValueError('Only mono spectral Arrays supported by this tool')
    if tag==1 and bps in (8,16,24,32):width=bps//8
    elif tag==3 and bps==32:width=4
    else:raise ValueError(f'Unsupported format tag {tag}, bit depth {bps}')
    if rate<=0:raise ValueError('Invalid sample rate')
    if ba!=width:warnings.append(f'blockAlign={ba}, expected {width} for declared mono format')
    if br!=rate*width:warnings.append(f'byteRate={br}, expected {rate*width} for declared mono format')
    if len(data)%width:raise ValueError('Incomplete final sample')
    count=len(data)//width
    if not 64<=count<=65536 or count%64:raise ValueError('Payload must contain 1..1024 complete 64-value frames')
    vals=[]
    for o in range(0,len(data),width):
        if tag==3:v=struct.unpack_from('<f',data,o)[0]
        elif bps==8:v=(data[o]-128)/128
        else:v=int.from_bytes(data[o:o+width],'little',signed=True)/(1<<(bps-1))
        if not math.isfinite(v):raise ValueError('Nonfinite floating coefficient')
        vals.append(v)
    n=count//64;dims=(8,8) if count==4096 else (n,1)
    return WaveInfo(tag,ch,rate,br,ba,bps,count,n,dims,warnings),[vals[i:i+64] for i in range(0,count,64)]

def encode_pcm16(frames: Sequence[Sequence[float]],peak: float|None=None,
                 firmware_header_quirk: bool=False) -> bytes:
    """Experimental interchange writer, NOT hardware-tested.

    Mirrors 32767/truncation and optional recovered peak normalization. Rejects
    out-of-range values instead of reproducing firmware integer wrap. Does not
    mutate caller memory. The firmware does mutate stored amplitudes when saving.
    """
    f=_frames(frames)
    if peak is not None and (not math.isfinite(peak) or peak<0):raise ValueError('Invalid recorded peak')
    gain=1/peak if peak is not None and 0<peak<1 else 1
    ints=[math.trunc(float(v)*gain*32767.0) for row in f for v in row]
    if any(not -32768<=i<=32767 for i in ints):raise ValueError('Coefficient would exceed int16; choose explicit normalization')
    payload=struct.pack('<%dh'%len(ints),*ints)
    br,ba=(192000,4) if firmware_header_quirk else (96000,2)
    fmt=struct.pack('<HHIIHH',1,1,48000,br,ba,16)
    chunks=b'fmt '+struct.pack('<I',16)+fmt+b'data'+struct.pack('<I',len(payload))+payload
    return b'RIFF'+struct.pack('<I',4+len(chunks))+b'WAVE'+chunks

def main() -> None:
    parser=argparse.ArgumentParser(description=__doc__)
    sub=parser.add_subparsers(dest='command',required=True)
    p=sub.add_parser('inspect-wav');p.add_argument('path',type=Path);p.add_argument('--frames-json',type=Path)
    p=sub.add_parser('extract-factory');p.add_argument('output_json',type=Path)
    args=parser.parse_args()
    if args.command=='inspect-wav':
        info,frames=decode_array_wav(args.path.read_bytes());print(json.dumps(asdict(info),indent=2))
        if args.frames_json:args.frames_json.write_text(json.dumps(frames)+'\n')
    else:
        vals=table('factory_spectra_64x64');frames=[vals[i:i+64] for i in range(0,len(vals),64)]
        args.output_json.write_text(json.dumps(frames)+'\n');print('Wrote 64 frames; these are extracted factory data, not a generated recording.')
if __name__=='__main__':
    try:main()
    except (ValueError,OSError,RuntimeError) as exc:raise SystemExit(f'Error: {exc}')
