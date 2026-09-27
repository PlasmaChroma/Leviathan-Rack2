"""Small ctypes adapter for LLVM's ARM disassembler; no capstone needed."""
import ctypes,ctypes.util,re,struct
class ThumbDisassembler:
 def __init__(self, data:bytes, base:int=0x08020000):
  self.data=data;self.base=base
  lib=ctypes.util.find_library('LLVM-19') or ctypes.util.find_library('LLVM')
  if not lib:raise RuntimeError('LLVM shared library not found (tested with libLLVM-19).')
  self.lib=ctypes.CDLL(lib)
  for suffix in ('TargetInfo','Target','TargetMC','Disassembler','AsmPrinter'):
   getattr(self.lib,'LLVMInitializeARM'+suffix)()
  self.lib.LLVMCreateDisasmCPUFeatures.argtypes=[ctypes.c_char_p,ctypes.c_char_p,ctypes.c_char_p,ctypes.c_void_p,ctypes.c_int,ctypes.c_void_p,ctypes.c_void_p]
  self.lib.LLVMCreateDisasmCPUFeatures.restype=ctypes.c_void_p
  self.ctx=self.lib.LLVMCreateDisasmCPUFeatures(b'thumbv7em-none-eabi',b'cortex-m7',b'+vfp4,+fp-armv8',None,0,None,None)
  if not self.ctx:raise RuntimeError('LLVM ARM context failed')
  self.lib.LLVMDisasmInstruction.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_uint64,ctypes.c_uint64,ctypes.c_void_p,ctypes.c_size_t]
  self.lib.LLVMDisasmInstruction.restype=ctypes.c_size_t
  self.buf=ctypes.create_string_buffer(data);self.text=ctypes.create_string_buffer(512)
 def decode(self,addr):
  off=addr-self.base
  if off<0 or off>=len(self.data)-1:return None
  n=self.lib.LLVMDisasmInstruction(self.ctx,ctypes.byref(self.buf,off),len(self.data)-off,addr,self.text,512)
  if not n:return None
  t=self.text.value.decode().strip();parts=t.split(None,1);op=parts[0];args=parts[1] if len(parts)>1 else ''
  d={'address':addr,'offset':off,'size':n,'bytes':self.data[off:off+n].hex(),'op':op,'args':args}
  m=re.search(r'\[pc(?:, #(-?\d+))?\]',args)
  if m:
   a=((addr+4)&~3)+int(m.group(1) or 0);d['literal_address']=a
   o=a-self.base
   if 0<=o<=len(self.data)-4:
    d['literal_u32']=struct.unpack_from('<I',self.data,o)[0]
    d['literal_f32']=struct.unpack_from('<f',self.data,o)[0]
  if op in ['bl','blx','b','b.w','beq','bne','bgt','bge','blt','ble','bhi','bhs','blo','bls','bcc','bcs','bmi','bpl','bvs','bvc'] or op.startswith(('beq.','bne.','bgt.','bge.','blt.','ble.','bhi.','bhs.','blo.','bls.','bcc.','bcs.','bmi.','bpl.','bvs.','bvc.','cbz','cbnz')):
   m=re.search(r'#(-?\d+)',args)
   if m:d['target']=addr+4+int(m.group(1))
  return d
 def format(self,d):
  s=f"{d['address']:08x}  {d['bytes']:<8}  {d['op']:<12} {d['args']}"
  if 'target'in d:s+=f"  ; -> 0x{d['target']:08x}"
  if 'literal_u32'in d:
   s+=f"  ; [0x{d['literal_address']:08x}] = 0x{d['literal_u32']:08x} (f32={d['literal_f32']:.9g})"
  return s
 def range(self,start,end):
  a=start
  while a<end:
   d=self.decode(a)
   if d:yield d;a+=d['size']
   else:a+=2


def json_finite(value):
    """JSON has no NaN/Infinity: retain raw literal_u32, emit null for that float view."""
    import math
    if isinstance(value, float) and not math.isfinite(value): return None
    if isinstance(value, dict): return {k: json_finite(v) for k,v in value.items()}
    if isinstance(value, (list, tuple)): return [json_finite(v) for v in value]
    return value
