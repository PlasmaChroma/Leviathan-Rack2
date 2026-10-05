from inspect_firmware import ELF32,ROOT
import re,json,struct
E=ELF32(ROOT/'extracted/bin/lubadh_main')
sections={s['name']:s for s in E.sections}
ds=sections['.dynsym']; strings=E.section_data(sections['.dynstr']);dyn=[]
for i in range(0,ds['size'],16):
 vals=struct.unpack_from('<IIIBBH',E.section_data(ds),i);dyn.append(E.cstr(strings,vals[0]))
rel=E.section_data(sections['.rel.plt']);plt={}
for i in range(0,len(rel),8):
 r,info=struct.unpack_from('<II',rel,i);addr=sections['.plt']['addr']+20+12*(i//8);plt[addr]=dyn[info>>8]
(ROOT/'evidence/plt_map.json').write_text(json.dumps({hex(k):v for k,v in plt.items()},indent=2))
for p in (ROOT/'evidence/functions').glob('*.asm'):
 lines=[]
 for l in p.read_text().splitlines():
  l=re.sub(r' <.*?> @ imm = .*','',l)
  l=re.sub(r' <.*?>','',l)
  if 'bl\t' in l:
   m=re.search(r'bl\s+0x([\da-f]+)',l)
   if m and int(m[1],16) in plt:l+=' ; '+plt[int(m[1],16)]
  m=re.search(r'vldr\w*\s+([sd]\d+), \[pc.*?@ 0x([0-9a-f]+)',l)
  if m:
   a=int(m[2],16)
   try:
    f=struct.unpack('<'+('f' if m[1][0]=='s' else 'd'),E.read(a,4 if m[1][0]=='s' else 8))[0];l+=f' ; float {f:.12g}'
   except:pass
  lines.append(l)
 p.write_text('\n'.join(lines)+'\n')
