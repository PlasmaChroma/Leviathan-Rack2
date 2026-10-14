#!/usr/bin/env python3
"""Reproducible, inert evidence extraction. Never executes vendor code/scripts.
Usage: python tools/extract_evidence.py [bundle_root]
Requires GNU readelf and LLVM llvm-objdump; Python standard library only.
"""
import sys, pathlib, struct, subprocess, json, re, hashlib, csv
ROOT=pathlib.Path(sys.argv[1]) if len(sys.argv)>1 else pathlib.Path(__file__).resolve().parents[1]
SRC=ROOT/'extracted'; E=ROOT/'evidence'; T=ROOT/'tables'
E.mkdir(exist_ok=True);T.mkdir(exist_ok=True)
class ELF32:
 def __init__(self,path):
  self.data=pathlib.Path(path).read_bytes();self.path=path
  if self.data[:6]!=b'\x7fELF\x01\x01':raise ValueError('not little-endian ELF32')
  h=struct.unpack_from('<16sHHIIIIIHHHHHH',self.data);self.header=dict(zip(['ident','type','machine','version','entry','phoff','shoff','flags','ehsize','phentsize','phnum','shentsize','shnum','shstrndx'],h))
  self.sections=[]
  for i in range(h[12]):
   sh=struct.unpack_from('<10I',self.data,h[6]+i*h[11]);self.sections.append(dict(zip(['nameoff','type','flags','addr','offset','size','link','info','align','entsize'],sh)))
  ns=self.secdata(self.sections[h[13]])
  for s in self.sections:s['name']=self.cstr(ns,s['nameoff'])
  self.symbols=[]
  for sec in self.sections:
   if sec['type'] not in (2,11):continue
   st=self.secdata(self.sections[sec['link']])
   for j in range(0,sec['size'],sec['entsize']):
    sn,v,sz,info,other,idx=struct.unpack_from('<IIIBBH',self.data,sec['offset']+j)
    name=self.cstr(st,sn)
    if not name or name.startswith('$'):continue
    self.symbols.append(dict(name=name,value=v,size=sz,type=info&15,bind=info>>4,section=idx,table=sec['name']))
 def cstr(self,b,o):return b[o:b.find(b'\0',o)].decode('utf-8','replace')
 def secdata(self,s):return self.data[s['offset']:s['offset']+s['size']]
 def readva(self,v,n):
  for s in self.sections:
   if s['type']!=8 and s['addr']<=v and v+n<=s['addr']+s['size']:
    return self.data[s['offset']+v-s['addr']:s['offset']+v-s['addr']+n]
  raise ValueError(f'VA {v:#x} not backed by file bytes')

def decode_readelf_dwarf(text):
 dies={};stack=[]
 for line in text.splitlines():
  m=re.match(r'\s*<(\d+)><([0-9a-f]+)>: Abbrev Number: (\d+)(?: \(([^)]+)\))?',line)
  if m:
   depth,off,abbr,tag=m.groups();depth=int(depth)
   if abbr=='0':continue
   d={'offset':int(off,16),'depth':depth,'tag':tag,'attrs':{},'children':[]};dies[d['offset']]=d
   while stack and stack[-1]['depth']>=depth:stack.pop()
   if stack:stack[-1]['children'].append(d['offset'])
   stack.append(d);continue
  m=re.match(r'\s*<[0-9a-f]+>\s+(DW_AT_\w+)\s*:\s*(.*)',line)
  if m and stack:stack[-1]['attrs'][m[1][6:]]=m[2]
 def name(d):
  n=d.get('attrs',{}).get('name','')
  return n.split('): ',1)[-1] if '): ' in n else n
 def ref(v):
  m=re.search('<0x([0-9a-f]+)>',v);return int(m[1],16) if m else None
 def typ(off,seen=None):
  seen=set() if seen is None else seen
  if off in seen or off not in dies:return '?'
  d=dies[off];seen=seen|{off};a=d['attrs'];n=name(d);tag=d['tag']
  if tag=='DW_TAG_pointer_type':return typ(ref(a.get('type','')),seen)+'*'
  if tag in ('DW_TAG_const_type','DW_TAG_volatile_type'):return tag[7:-5]+' '+typ(ref(a.get('type','')),seen)
  if tag=='DW_TAG_array_type':
   bounds=[]
   for c in d['children']:
    ca=dies[c]['attrs'];v=ca.get('upper_bound');bounds.append(str(int(v)+1) if v and v.isdigit() else ca.get('count','?'))
   return typ(ref(a.get('type','')),seen)+''.join('['+b+']' for b in bounds)
  return n or tag
 out=[]
 for off,d in dies.items():
  if d['tag'] in ('DW_TAG_structure_type','DW_TAG_union_type','DW_TAG_enumeration_type'):
   members=[]
   for c in d['children']:
    ch=dies[c];a=ch['attrs'];members.append(dict(name=name(ch),type=typ(ref(a.get('type',''))),offset=a.get('data_member_location'),value=a.get('const_value'),source_line=a.get('decl_line')))
   out.append(dict(name=name(d),tag=d['tag'],offset=off,size=d['attrs'].get('byte_size'),members=members))
 return out,dies

def pdparse(path):
 # Pd file records can span multiple physical lines. Unescaped ; ends record.
 txt=path.read_text(errors='replace');records=re.split(r'(?<!\\);\s*(?:\n|$)',txt)
 stack=[];canvases=[]
 for rec in records:
  rec=' '.join(rec.splitlines()).strip()
  if rec.startswith('#N canvas'):
   c={'name':rec,'objects':[],'connections':[],'children':[],'id':len(canvases)};canvases.append(c)
   if stack:stack[-1]['children'].append(c['id'])
   stack.append(c)
  elif rec.startswith('#X restore'):
   child=stack.pop()
   if stack:stack[-1]['objects'].append({'index':len(stack[-1]['objects']),'record':rec,'subcanvas':child['id']})
  elif rec.startswith('#X connect'):
   nums=list(map(int,rec.split()[2:6]));stack[-1]['connections'].append(nums)
  elif rec.startswith('#X ') and not rec.startswith(('#X coords','#X declare','#X f ')):
   if stack:stack[-1]['objects'].append({'index':len(stack[-1]['objects']),'record':rec})
 return canvases

if __name__=='__main__':
 summary=[];alltypes={};selected_tables={};symrows=[]
 for p in sorted(SRC.rglob('*')):
  if not p.is_file() or p.read_bytes()[:4]!=b'\x7fELF':continue
  elf=ELF32(p);tag=str(p.relative_to(SRC)).replace('/','__');out=E/'elf'/tag;out.mkdir(parents=True,exist_ok=True)
  (out/'metadata.json').write_text(json.dumps({'header':{k:v for k,v in elf.header.items() if k!='ident'},'sections':elf.sections,'symbols':elf.symbols},indent=2))
  for opt,f in [(['-h','-A','-d','-n'],'readelf.txt'),(['--debug-dump=info'],'dwarf_info.txt'),(['--debug-dump=decodedline'],'dwarf_lines.txt')]:
   s=subprocess.run(['readelf']+opt+[str(p)],capture_output=True,text=True).stdout;(out/f).write_text(s)
   if f=='dwarf_info.txt' and s:
    ts,ds=decode_readelf_dwarf(s);alltypes[tag]=ts;(out/'dwarf_types.json').write_text(json.dumps(ts,indent=2))
  dis=subprocess.run(['llvm-objdump','-d','--demangle',str(p)],capture_output=True,text=True).stdout;(out/'disassembly.txt').write_text(dis)
  funcs={}
  for block in re.split(r'(?=^[0-9a-f]+ <)',dis,flags=re.M):
   m=re.match(r'([0-9a-f]+) <([^>]+)>:',block)
   if m:funcs[m[2]]=block
  fd=out/'functions';fd.mkdir(exist_ok=True)
  for n,b in funcs.items():
   if n.startswith(('bcm2835_','__')) or '@plt' in n:continue
   (fd/(re.sub(r'[^\w.~-]','_',n)+'.asm')).write_text(b)
  ss=[s for s in elf.symbols if s['table']=='.symtab'];
  for s in ss:symrows.append({'binary':str(p.relative_to(SRC)),**s})
  for s in ss:
   if s['name'] in ('SAW','GAUSS','SQUARE','PITCH_VALUES','VOL_REDUCTION'):
    data=elf.readva(s['value'],s['size']);vals=list(struct.unpack('<'+'f'*(len(data)//4),data))
    selected_tables[f'{tag}:{s["name"]}']={'binary':str(p.relative_to(SRC)),'symbol':s['name'],'virtual_address':hex(s['value']),'size_bytes':s['size'],'values':vals,'sha256':hashlib.sha256(data).hexdigest()}
  summary.append({'path':str(p.relative_to(SRC)),'sha256':hashlib.sha256(elf.data).hexdigest(),'size':len(elf.data),'defined_functions':sum(s['type']==2 and s['section']!=0 for s in ss),'debug_info':any(s['name']=='.debug_info' for s in elf.sections),'elf_type':elf.header['type'],'machine':elf.header['machine']})
 (T/'binary_inventory.json').write_text(json.dumps(summary,indent=2));(T/'dwarf_types.json').write_text(json.dumps(alltypes,indent=2));(T/'extracted_float_tables.json').write_text(json.dumps(selected_tables,indent=2))
 with (T/'symbols.csv').open('w',newline='') as f:
  w=csv.DictWriter(f,fieldnames=symrows[0].keys());w.writeheader();w.writerows(symrows)
 pd={p.name:pdparse(p) for p in SRC.glob('*.pd')};(T/'pd_netlists.json').write_text(json.dumps(pd,indent=2))
 with (E/'pd_netlists.txt').open('w') as f:
  for pn,cs in pd.items():
   f.write('\nFILE '+pn+'\n')
   for c in cs:
    f.write(f'\nCANVAS {c["id"]}: {c["name"]}\n')
    for o in c['objects']:f.write(f'{o["index"]:4d} {o["record"]}\n')
    for a,ao,b,bi in c['connections']:
     f.write(f'  {a}:{ao} -> {b}:{bi}\n')
 print('ELF binaries:',len(summary),'PD patches:',len(pd),'tables:',len(selected_tables),'symbols:',len(symrows))
