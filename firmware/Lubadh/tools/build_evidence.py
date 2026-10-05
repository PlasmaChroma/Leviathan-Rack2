#!/usr/bin/env python3
from inspect_firmware import ELF32,ROOT
from pathlib import Path
import subprocess,re,json,struct,csv,shutil,os
LLVM=os.environ.get('LLVM_OBJDUMP') or shutil.which('llvm-objdump') or '/usr/local/swift/usr/bin/llvm-objdump'
for binary in (ROOT/'extracted/bin').iterdir():
 if not binary.is_file():continue
 e=ELF32(binary)
 dest=ROOT/'evidence'/f'{binary.name}_disassembly.txt'
 if binary.name=='lubadh_main':text=(ROOT/'evidence/main_disassembly.txt').read_text()
 else:text=subprocess.run([LLVM,'-d','--demangle',str(binary)],text=True,capture_output=True,check=True).stdout
 dest.write_text(text)
 sym={s['address']:s for s in e.symbols if s['type']==2 and s['section']!=0}
 strings=[]
 for sec in e.sections:
  if not (sec['flags']&2) or sec['type']==8:continue
  raw=e.section_data(sec)
  for m in re.finditer(rb'[\x20-\x7e]{4,}',raw):strings.append(dict(address=sec['addr']+m.start(),text=m.group().decode()))
 sd={x['address']:x['text'] for x in strings}
 (ROOT/'evidence'/f'{binary.name}_strings.json').write_text(json.dumps(strings,indent=2))
 annotated=[];regs={};calls=[];current=None
 for line in text.splitlines():
  m=re.match(r'\s*([0-9a-f]+):\s+[0-9a-f]{8}\s+(\S+)\s+(.*)',line)
  if m:
   addr=int(m[1],16);op=m[2];args=m[3].split('@')[0].split('<')[0].strip();extra=[]
   if addr in sym:current=sym[addr]
   mv=re.match(r'(r\d+), #(-?0x[0-9a-f]+|-?\d+)',args)
   if op in ('movw','movt') and mv:
    r,v=mv.groups();v=int(v,0)
    if op=='movw':regs[r]=v
    else:
     regs[r]=(regs.get(r,0)&65535)|(v<<16)
     if regs[r] in sd:extra.append(f'{r} -> '+repr(sd[regs[r]]))
   pc=re.search(r'@ 0x([0-9a-f]+)',line)
   if pc and op.startswith(('vldr','ldr')):
    try:
     p=int(pc[1],16);raw=e.read(p,4);v=struct.unpack('<I',raw)[0]
     extra.append(f'bits=0x{v:08x} f32={struct.unpack("<f",raw)[0]:.12g}')
     if v in sd:extra.append('ptr -> '+repr(sd[v]))
    except ValueError:pass
   if op=='bl':
    target=int(args,16)
    if current:calls.append(dict(caller_address=current['address'],caller=current['demangled'],call_address=addr,target_address=target,target=sym.get(target,{}).get('demangled','external/PLT')))
   line+=(' ; '+' ; '.join(extra)) if extra else ''
  annotated.append(line)
 (ROOT/'evidence'/f'{binary.name}_annotated.txt').write_text('\n'.join(annotated)+'\n')
 (ROOT/'evidence'/f'{binary.name}_direct_calls.json').write_text(json.dumps(calls,indent=2))
print('All eight binaries indexed, disassembled, strings and direct calls extracted.')
