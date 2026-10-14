#!/usr/bin/env python3
"""Reproducible conservative CFG and symbol-candidate inventory; no firmware execution."""
from inspect_firmware import *
import csv, bisect
CODE_BEGIN=BASE+0x298
CODE_END=0x080365e4
BR={'b','b.w','beq','bne','bhs','blo','bhi','bls','bge','bgt','ble','blt','bmi','bpl','bvs','bvc','bcs','bcc'}
def get_target(a,t):
 mnem=t.split()[0].removesuffix('.w')
 if mnem in BR or mnem in {'bl','blx','cbnz','cbz'}:
  nums=re.findall(r'#(-?\d+)',t)
  if nums: return a+4+int(nums[-1])
 return None
def lin(start,end):
 a=start
 while a<end:
  n,t=decode(a); n=n or 2
  yield a,n,t; a+=n
L={a:(n,t) for a,n,t in lin(CODE_BEGIN,CODE_END)}
starts=collections.defaultdict(set)
vectors=[]
for i in range(166):
 word=u32(BASE+4*i)
 vectors.append({'index':i,'irq':i-16,'word':f'0x{word:08x}','target':f'0x{word&~1:08x}' if i and word else '', 'kind':'initial_sp' if not i else ('reserved' if not word else 'default' if word==0x08036481 else 'handler')})
 if i and CODE_BEGIN<=word&~1<CODE_END: starts[word&~1].add('vector')
starts[0x08034060].add('startup_call')
for a,(n,t) in L.items():
 dst=get_target(a,t)
 if t.split()[0] in {'bl','blx'} and dst is not None and CODE_BEGIN<=dst<CODE_END: starts[dst].add('direct_call_candidate')
# Callback address candidates only when aligned word points to plausible function prologue.
for o in range(0,len(B)-3,4):
 p=struct.unpack_from('<I',B,o)[0]
 if p&1 and CODE_BEGIN<=p&~1<CODE_END:
  n,t=decode(p&~1)
  if t.startswith('push') or p&~1 in starts: starts[p&~1].add('flash_pointer_candidate')
# Include otherwise unreachable prologue candidates, explicitly flagged as heuristic.
for a,(n,t) in L.items():
 if t.startswith('push') and 'lr' in t and a%4==0: starts[a].add('prologue_heuristic')
starts.pop(0x080336f4,None)  # prologue is four bytes after the actual called entry
for a in (0x08032678,0x08032680,0x08032688): starts[a].add('manually_confirmed_callback')
starts[0x0802e750].add('confirmed_audio_callback_tail_target')
funcs={}; allcode={}; edges=[]; literals=[]; issues=[]
queue=list(sorted(starts)); processed=set()
while queue:
 s=queue.pop(0)
 if s in processed: continue
 processed.add(s); todo=[s]; seen=set(); ins=[]; fcalls=[]; problems=[]
 while todo:
  a=todo.pop()
  while CODE_BEGIN<=a<CODE_END and a not in seen:
   if a!=s and a in starts:
    fcalls.append((a,'tail_or_function_boundary')); break
   n,t=decode(a)
   if not n: problems.append(f'decode_failure_0x{a:08x}'); break
   seen.add(a); ins.append((a,n,t)); allcode[a]=(n,t)
   lit=literal(a,t)
   if lit: literals.append({'function':f'0x{s:08x}','instruction':f'0x{a:08x}','pool_address':f'0x{lit[0]:08x}','value':f'0x{lit[1]:08x}','assembly':t})
   m=t.split()[0]; mn=m.removesuffix('.w'); dst=get_target(a,t)
   if mn in {'bl','blx'}:
    if dst is not None and CODE_BEGIN<=dst<CODE_END:
     fcalls.append((dst,'direct_call'))
     if dst not in starts: starts[dst].add('reachable_direct_call'); queue.append(dst)
    elif dst is None: problems.append(f'indirect_call_0x{a:08x}')
   elif mn in BR or mn in {'cbz','cbnz'}:
    if dst is not None and CODE_BEGIN<=dst<CODE_END: todo.append(dst)
    if mn=='b': break
   if (mn=='bx' and 'lr' in t) or ((mn in {'pop','ldm','ldmia'}) and 'pc' in t) or (mn=='ldr' and t.startswith('ldr\tpc')): break
   if mn=='bx': problems.append(f'indirect_branch_0x{a:08x}'); break
   if mn in {'tbb','tbh'}:
    # Recover maximum from nearby cmp immediate (switch tables emitted by GCC).
    before=sorted([x for x in ins if a-24<=x[0]<a])
    maximum=None
    for aa,nn,tt in reversed(before):
     mm=re.search(r'^cmp(?:\.w)?\s+r\d+, #(\d+)',tt)
     if mm: maximum=int(mm.group(1)); break
    if maximum is not None and maximum<128 and '[pc,' in t:
     width=1 if mn=='tbb' else 2
     for j in range(maximum+1):
      oo=a+4-BASE+j*width
      val=B[oo] if width==1 else struct.unpack_from('<H',B,oo)[0]
      dst2=a+4+2*val
      if CODE_BEGIN<=dst2<CODE_END: todo.append(dst2)
    else: problems.append(f'unresolved_switch_0x{a:08x}')
    break
   a+=n
 ins.sort(); funcs[s]={'instructions':ins,'calls':fcalls,'issues':problems}
 for dst,kind in fcalls: edges.append({'caller':f'0x{s:08x}','callee':f'0x{dst:08x}','kind':kind})
 issues.extend([{'function':f'0x{s:08x}','issue':x} for x in problems])
# Save artifacts.
def csvwrite(path,rows,fields=None):
 with path.open('w',newline='') as out:
  w=csv.DictWriter(out,fieldnames=fields or list(rows[0])); w.writeheader(); w.writerows(rows)
csvwrite(ROOT/'analysis/vector_table.csv',vectors)
csvwrite(ROOT/'analysis/call_graph.csv',edges)
csvwrite(ROOT/'analysis/literal_xrefs.csv',literals)
csvwrite(ROOT/'analysis/cfg_issues.csv',issues,['function','issue'])
rows=[]
for s,f in sorted(funcs.items()):
 ins=f['instructions']; fp=sum(t.startswith('v') for _,_,t in ins)
 rows.append({'address':f'0x{s:08x}','end_exclusive':f'0x{max([a+n for a,n,t in ins],default=s):08x}','instruction_count':len(ins),'decoded_bytes':sum(n for _,n,_ in ins),'fp_instructions':fp,'calls':len(f['calls']),'entry_evidence':';'.join(sorted(starts[s])),'issues':';'.join(f['issues'])})
 (ROOT/f'disassembly/fn_{s:08x}.asm').write_text('; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol\n; Entry evidence: '+','.join(sorted(starts[s]))+'\n'+'\n'.join(line(a,n,t)+(f' ; branch_target=0x{get_target(a,t):08x}' if get_target(a,t) is not None else '') for a,n,t in ins)+'\n')
csvwrite(ROOT/'analysis/function_candidates.csv',rows)
(ROOT/'analysis/decoded_instructions.json').write_text(json.dumps({f'{a:08x}':{'size':n,'text':t} for a,(n,t) in sorted(allcode.items())}))
with (ROOT/'disassembly/reachable_and_candidate_code.asm').open('w') as out:
 for a,(n,t) in sorted(allcode.items()): out.write(line(a,n,t)+'\n')
with (ROOT/'disassembly/linear_sweep_UNVERIFIED.asm').open('w') as out:
 out.write('; LINEAR SWEEP: includes literal pools and data misinterpreted as instructions.\n')
 for a,(n,t) in sorted(L.items()): out.write(line(a,n,t)+'\n')
print('Functions candidates',len(rows),'decoded ins',len(allcode),'bytes',sum(n for n,t in allcode.values()),'issues',len(issues))
for r in sorted(rows,key=lambda x:x['fp_instructions'],reverse=True)[:38]: print(r)
