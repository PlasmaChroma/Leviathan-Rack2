#!/usr/bin/env python3
"""Offline, dependency-free ELF32/symbol/disassembly manifest extraction."""
from pathlib import Path
import struct, subprocess, json, hashlib, re, sys
ROOT=Path(__file__).resolve().parents[1]
class ELF32:
 def __init__(self,p):
  self.data=Path(p).read_bytes(); d=self.data
  assert d[:6]==b'\x7fELF\x01\x01'
  h=struct.unpack_from('<16sHHIIIIIHHHHHH',d)
  self.entry=h[4]; shoff=h[6]; shentsize,shnum,shstrndx=h[11:14]
  self.sections=[]
  for i in range(shnum):
   v=struct.unpack_from('<10I',d,shoff+i*shentsize)
   self.sections.append(dict(zip(['name_idx','type','flags','addr','offset','size','link','info','align','entsize'],v)))
  strings=self.section_data(self.sections[shstrndx])
  for s in self.sections: s['name']=self.cstr(strings,s['name_idx'])
  self.symbols=[]
  for sec in self.sections:
   if sec['type']!=2:continue
   strings=self.section_data(self.sections[sec['link']])
   for off in range(sec['offset'],sec['offset']+sec['size'],sec['entsize']):
    name,val,size,info,other,ndx=struct.unpack_from('<IIIBBH',d,off)
    self.symbols.append(dict(name=self.cstr(strings,name),address=val,size=size,type=info&15,bind=info>>4,section=ndx))
  p=subprocess.run(['c++filt'],input='\n'.join(s['name'] for s in self.symbols),text=True,capture_output=True)
  for s,n in zip(self.symbols,p.stdout.splitlines()):s['demangled']=n
 def cstr(self,d,i):return d[i:d.find(b'\0',i)].decode('utf-8','replace')
 def section_data(self,s):return self.data[s['offset']:s['offset']+s['size']]
 def read(self,addr,size):
  for s in self.sections:
   if s['addr']<=addr and addr+size<=s['addr']+s['size'] and s['type']!=8:
    a=s['offset']+addr-s['addr'];return self.data[a:a+size]
  raise ValueError(hex(addr))
 def floats(self,addr,n):return struct.unpack('<'+'f'*n,self.read(addr,4*n))
 def ints(self,addr,n):return struct.unpack('<'+'I'*n,self.read(addr,4*n))
if __name__=='__main__':
 manifests={}
 for p in sorted((ROOT/'extracted').rglob('*')):
  if not p.is_file():continue
  key=str(p.relative_to(ROOT/'extracted'))
  m=dict(size=p.stat().st_size,sha256=hashlib.sha256(p.read_bytes()).hexdigest())
  if p.parent.name=='bin':
   e=ELF32(p);m['entry']=hex(e.entry);m['symbols']=len(e.symbols);m['functions']=sum(s['type']==2 and s['section']!=0 for s in e.symbols)
   (ROOT/'evidence'/f'{p.name}_symbols.json').write_text(json.dumps(e.symbols,indent=2))
   (ROOT/'evidence'/f'{p.name}_readelf.txt').write_text(subprocess.run(['readelf','-h','-A','-d','-l',str(p)],capture_output=True,text=True).stdout)
  manifests[key]=m
 (ROOT/'evidence'/'manifest.json').write_text(json.dumps(manifests,indent=2))
 e=ELF32(ROOT/'extracted/bin/lubadh_main')
 lines=(ROOT/'evidence/main_disassembly.txt').read_text().splitlines()
 funcs=[]
 for s in e.symbols:
  if s['type']!=2 or not s['size'] or s['section']==0:continue
  n=s['demangled']
  if n.startswith(('std::','shm::','Hjson::','__gnu')):continue
  if not any(k in n for k in ['lubadh::','idsp::','main','Clock::']):continue
  funcs.append(s)
 unique={s['address']:s for s in funcs}
 dest=ROOT/'evidence/functions';dest.mkdir(exist_ok=True)
 for addr,s in sorted(unique.items()):
  name=f"{addr:08x}_"+re.sub('[^a-zA-Z0-9._-]+','_',s['demangled'])[:155]+'.asm'
  text=[f"; {s['demangled']}\n; VA {addr:#x} size {s['size']}\n"]
  for line in lines:
   m=re.match(r'^\s*([0-9a-f]+):',line)
   if m and addr<=int(m[1],16)<addr+s['size']:text.append(line)
  (dest/name).write_text('\n'.join(text)+'\n')
 (ROOT/'evidence/engine_function_index.json').write_text(json.dumps(list(unique.values()),indent=2))
 print('unique selected functions',len(unique))
 print('main symbols/functions',len(e.symbols),sum(s['type']==2 and s['section']!=0 for s in e.symbols))
