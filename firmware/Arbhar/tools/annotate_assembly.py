#!/usr/bin/env python3
"""Annotate literal loads (interpretations, not guaranteed types), calls and nearby symbols."""
from extract_evidence import ELF32
import pathlib, re, struct, subprocess, json
R=pathlib.Path(__file__).resolve().parents[1]
for p in (R/'extracted').glob('*'):
 if not p.is_file() or p.read_bytes()[:4]!=b'\x7fELF':continue
 e=ELF32(p); sy={s['value']:s['name'] for s in e.symbols if s['value'] and s['section']}
 # In this ARM ELF linker layout, PLT0=20 bytes, each following stub=12 bytes.
 plts=[s for s in e.sections if s['name']=='.plt']; rels=[s for s in e.sections if s['name']=='.rel.plt']
 calls={}
 if plts and rels:
  rel=rels[0]; ds=e.sections[rel['link']]; st=e.secdata(e.sections[ds['link']])
  for i in range(rel['size']//8):
   ro,ri=struct.unpack_from('<II',e.data,rel['offset']+8*i); sn=struct.unpack_from('<I',e.data,ds['offset']+(ri>>8)*16)[0]
   calls[plts[0]['addr']+20+12*i]=e.cstr(st,sn)
 out=R/'evidence'/'elf'/p.name/'annotated';out.mkdir(exist_ok=True)
 for f in (out.parent/'functions').glob('*.asm'):
  lines=[]
  for l in f.read_text().splitlines():
   comments=[]
   m=re.search(r'\b(?:bl|b)\s+0x([0-9a-f]+)',l)
   if m and int(m[1],16) in calls:comments.append('CALL '+calls[int(m[1],16)])
   m=re.search(r'\b(vldr|ldr)\s+(\w+), \[pc, [^\]]+\].*@ 0x([0-9a-f]+)',l)
   if m:
    op,reg,v=m.groups();va=int(v,16)
    try:
     b=e.readva(va,8 if reg.startswith('d') else 4);u=struct.unpack_from('<I',b)[0]
     if reg.startswith('d'):comments.append(f'f64={struct.unpack("<d",b)[0]:.17g}')
     elif reg.startswith('s'):comments.append(f'f32={struct.unpack("<f",b)[0]:.9g}')
     else:comments.append(f'u32=0x{u:x}; f32?={struct.unpack("<f",b)[0]:.9g}')
    except Exception:pass
   lines.append(l+('  // '+'; '.join(comments) if comments else ''))
  (out/f.name).write_text('\n'.join(lines)+'\n')
 (out.parent/'plt_imports.json').write_text(json.dumps({hex(k):v for k,v in calls.items()},indent=2))
