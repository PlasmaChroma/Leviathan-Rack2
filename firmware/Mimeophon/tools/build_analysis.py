from pathlib import Path
import json,struct,collections,csv,re
from llvm_thumb import ThumbDisassembler
from llvm_thumb import json_finite
P=Path(__file__).resolve().parents[1];B=(P/'binaries/mimeophon_mp86.bin').read_bytes();BASE=0x08020000;END=BASE+0x8c74
D=ThumbDisassembler(B)
vectors=struct.unpack_from('<114I',B)
roots={v&~1 for v in vectors[1:] if BASE+0x1c8<=v<END}
roots.update([BASE+0x1c8,BASE+0x2a0,BASE+0x2ec,BASE+0x338,BASE+0x697c,BASE+0x6ab4])
# Additional pointer candidates are retained as candidates, not silently trusted.
ptrs=collections.defaultdict(list)
for off in range(0,len(B)-3,4):
 v=struct.unpack_from('<I',B,off)[0]
 if v&1 and BASE+0x1c8<=v<END:ptrs[v&~1].append(BASE+off)
roots.update(ptrs)
queue=list(roots);ins={};calls=[];indirect=[];literals={}
while queue:
 a=queue.pop()
 while BASE+0x1c8<=a<END and a not in ins:
  d=D.decode(a)
  if not d:break
  ins[a]=d;op=d['op'];args=d['args'];n=a+d['size']
  if 'literal_address'in d:literals[d['literal_address']]=d.get('literal_u32')
  if 'target'in d:
   t=d['target']
   if BASE+0x1c8<=t<END:queue.append(t)
   if op in ('bl','blx'):
    roots.add(t);calls.append((a,t))
   if op in ('b','b.w'):break
  if op in ('bx','blx') and 'target'not in d:
   indirect.append(d)
   # Resolve immediate LDR Rn; BX Rn pattern at startup/fault trampolines.
   prev=ins.get(a-2) or ins.get(a-4)
   if prev and prev['op'].startswith('ldr') and 'literal_u32'in prev and prev['args'].split(',')[0]==args:
    t=prev['literal_u32']&~1
    if BASE+0x1c8<=t<END:queue.append(t);roots.add(t)
   if op=='bx':break
  if (op in ('pop','pop.w') and 'pc'in args) or (op.startswith('ldm') and 'pc'in args):break
  if op in ('tbb','tbh'):
   indirect.append(d)
   # Bounds confirmed from preceding CMP and unsigned range-check branches.
   count={BASE+0x2538:5, BASE+0x6eba:10, BASE+0x735a:4, BASE+0x0d9a:4}.get(a)
   if op=='tbb' and count:
    table=a+4
    for byte in B[table-BASE:table-BASE+count]:
     queue.append(table+2*byte)
   break
  a=n
# Build summaries using entry-to-next-entry bins; these are not compiler function boundaries.
starts=sorted(a for a in roots if BASE<=a<END and a in ins)
summary=[]
for start,nexta in zip(starts,starts[1:]+[END]):
 ii=[ins[a] for a in sorted(ins) if start<=a<nexta]
 summary.append({'address':f'0x{start:08x}','next_entry':f'0x{nexta:08x}','instruction_count':len(ii),'float_instructions':sum(d['op'].startswith('v') for d in ii),'calls':sorted(set(f"0x{d['target']:08x}" for d in ii if d['op']=='bl' and 'target'in d)),'entry_evidence':'vector_or_code_pointer_or_call'})
(P/'analysis/reachable_instructions.json').write_text(json.dumps(json_finite(list(ins.values())),indent=2,allow_nan=False))
(P/'analysis/function_candidates.json').write_text(json.dumps(summary,indent=2))
(P/'analysis/indirect_transfers.json').write_text(json.dumps(indirect,indent=2))
(P/'disassembly/reachable_thumb.asm').write_text('; Recursive traversal seeded by vectors, calls, and plausible flash pointers.\n; Not complete: unresolved indirect transfers are listed separately. Literal pools excluded where not reached.\n'+'\n'.join(D.format(ins[a]) for a in sorted(ins))+'\n')
with (P/'analysis/call_edges.csv').open('w',newline='') as f:
 w=csv.writer(f);w.writerow(['call_site','target']);w.writerows((f'0x{a:08x}',f'0x{t:08x}') for a,t in sorted(calls))
with (P/'analysis/literal_references.csv').open('w',newline='') as f:
 w=csv.writer(f);w.writerow(['instruction','literal_address','u32_hex','float32']);
 for a in sorted(ins):
  d=ins[a]
  if 'literal_u32'in d:w.writerow([f'0x{a:08x}',f"0x{d['literal_address']:08x}",f"0x{d['literal_u32']:08x}",d['literal_f32']])
print('reachable instructions',len(ins),'bytes',sum(d['size'] for d in ins.values()),'candidate funcs',len(starts),'roots',len(roots),'calls',len(calls),'unresolved/indirect',len(indirect))
print('TBB/TBH',[D.format(d) for d in indirect if d['op'] in ('tbb','tbh')])
# Save targeted listings; these are the main audited areas.
for name,a,e in [('dsp_core',0x39b4,0x697c),('audio_callbacks',0x697c,0x6bec),('startup',0x1c8,0x298),('main',0x7648,0x7c94),('initialization',0x32d0,0x39b4),('interrupt_handlers',0x81e8,0x82b0)]:
 (P/f'disassembly/{name}.asm').write_text('\n'.join(D.format(ins[k]) for k in sorted(ins) if BASE+a<=k<BASE+e)+'\n')
