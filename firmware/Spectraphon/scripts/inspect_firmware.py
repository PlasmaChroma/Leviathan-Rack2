#!/usr/bin/env python3
"""Read-only SP67 binary analysis using LLVM's installed ARM disassembler."""
from pathlib import Path
import ctypes, struct, re, sys, json, math, collections, hashlib
ROOT=Path(__file__).resolve().parents[1]
B=(ROOT/'firmware/sp67.bin').read_bytes(); BASE=0x08020000
lib=ctypes.CDLL('/usr/lib/x86_64-linux-gnu/libLLVM-19.so')
for name in ('LLVMInitializeARMTargetInfo','LLVMInitializeARMTarget','LLVMInitializeARMTargetMC','LLVMInitializeARMDisassembler'):
 getattr(lib,name)()
lib.LLVMCreateDisasmCPUFeatures.restype=ctypes.c_void_p
lib.LLVMCreateDisasmCPUFeatures.argtypes=[ctypes.c_char_p,ctypes.c_char_p,ctypes.c_char_p,ctypes.c_void_p,ctypes.c_int,ctypes.c_void_p,ctypes.c_void_p]
D=lib.LLVMCreateDisasmCPUFeatures(b'thumbv7em-none-eabi',b'cortex-m7',b'+fp-armv8',None,0,None,None)
if not D: raise RuntimeError('LLVM ARM disassembler unavailable')
lib.LLVMDisasmInstruction.restype=ctypes.c_size_t
lib.LLVMDisasmInstruction.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_uint64,ctypes.c_uint64,ctypes.c_void_p,ctypes.c_size_t]
BUF=ctypes.create_string_buffer(B); OUT=ctypes.create_string_buffer(512)
def u32(a):
 if BASE<=a<=BASE+len(B)-4: return struct.unpack_from('<I',B,a-BASE)[0]
 return None
def decode(a):
 o=a-BASE
 if not 0<=o<len(B): return 0,''
 n=lib.LLVMDisasmInstruction(D,ctypes.byref(BUF,o),len(B)-o,a,OUT,512)
 return n,OUT.value.decode().strip() if n else '.hword 0x%04x'%struct.unpack_from('<H',B,o)[0]
def target(a,t):
 m=t.split()[0].removesuffix('.w')
 if m in {'b','beq','bne','bhs','blo','bhi','bls','bge','bgt','ble','blt','bmi','bpl','bvs','bvc','bcs','bcc','bl','blx','cbnz','cbz'}:
  nums=re.findall(r'#(-?\d+)',t)
  if nums: return a+4+int(nums[-1])
 return None
def literal(a,t):
 m=re.search(r'\[pc(?:, #(-?\d+))?\]',t)
 if m:
  at=((a+4)&~3)+int(m.group(1) or 0)
  v=u32(at)
  if v is not None: return at,v
 return None
def line(a,n,t):
 st=f'{a:08x}  {B[a-BASE:a-BASE+n].hex():<8s}  {t}'
 dst=target(a,t)
 if dst is not None: st+=f' ; -> 0x{dst:08x}'
 lit=literal(a,t)
 if lit:
  at,v=lit; fv=struct.unpack('<f',struct.pack('<I',v))[0]
  st+=f' ; [0x{at:08x}] = 0x{v:08x}'
  if re.match(r'vldr\s+d\d+',t) and BASE<=at<=BASE+len(B)-8:
   dv=struct.unpack_from('<d',B,at-BASE)[0]
   st+=f' / f64_bits_interpretation={dv:.17g}'
  elif math.isfinite(fv) and 1e-10<abs(fv)<1e10: st+=f' / f32_bits_interpretation={fv:.10g}'
 return st
def dump(start,end):
 a=start
 while a<end:
  n,t=decode(a); n=n or 2
  print(line(a,n,t)); a+=n
if __name__=='__main__':
 if len(sys.argv)>2: dump(int(sys.argv[1],0),int(sys.argv[2],0))
 else:
  print('bytes',len(B),'sha256',hashlib.sha256(B).hexdigest())
  for m in re.finditer(rb'[ -~]{5,}',B):
   s=m.group().decode()
   if len(s)>7 or any(x in s.lower() for x in ['.wav','wave','riff','fmt ']): print(hex(BASE+m.start()),repr(s))
