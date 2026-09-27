"""Conservative MG204 recursive Thumb listing. Not a decompiler or complete CFG.

Only vectors, explicitly audited entries and direct edges seed traversal.
Plausible words that look like code pointers are exported separately, not followed.
"""
import csv, hashlib, json, re, struct
from collections import defaultdict
from pathlib import Path
from llvm_thumb import ThumbDisassembler, json_finite

ROOT = Path(__file__).resolve().parents[1]
B = (ROOT/'Morphagene_MG204_RE_bundle/mg204_flash_08020000.bin').read_bytes()
if hashlib.sha256(B).hexdigest() != '44037eb2cf24b5fc411135e487a6fa226498a8478db981659bb53341cb967609':
    raise ValueError('Not the audited MG204 image')
BASE, START, END = 0x08020000, 0x080201c0, 0x0803e340
D = ThumbDisassembler(B)
def dump(name, obj):
    (ROOT/name).write_text(json.dumps(json_finite(obj), indent=2, allow_nan=False)+'\n', encoding='utf-8')
def csvout(name, fields, rows):
    with (ROOT/name).open('w', newline='', encoding='utf-8') as f:
        w=csv.writer(f); w.writerow(fields); w.writerows(rows)

vectors = struct.unpack_from('<107I', B)
roots = defaultdict(set)
for n, v in enumerate(vectors[1:], 1):
    if START <= (v & ~1) < END and v & 1: roots[v & ~1].add('vector_'+str(n))
for a in (0x08027518,): roots[a].add('prior_report_verified_prologue')
queue = list(roots)
ins, indirect, calls, overlaps = {}, [], [], []
occupied = {}
# Add only manually reviewed table bounds here, after examining each range check.
JUMP_COUNTS = {0x08024876:20, 0x0802286a:10, 0x08022892:10,
               0x080348d4:23, 0x080380aa:46}
while queue:
    a = queue.pop()
    history = []
    while START <= a < END and a not in ins:
        if a in occupied:
            overlaps.append({'target':hex(a),'instruction':hex(occupied[a])}); break
        d = D.decode(a)
        if not d: break
        ins[a] = d
        for byte in range(a, a+d['size']): occupied[byte] = a
        op, args, nxt = d['op'], d['args'], a+d['size']
        if 'target' in d:
            t = d['target']
            if START <= t < END: queue.append(t)
            if op in ('bl','blx'):
                calls.append((a,t)); roots[t].add('direct_call')
            if op in ('b','b.w'): break
        if op in ('bx','blx') and 'target' not in d:
            if args != 'lr':
                item=dict(d); item['resolved_target']=None
                if history:
                    prev=history[-1]
                    if prev['op'].startswith('ldr') and 'literal_u32' in prev and prev['args'].split(',')[0] == args:
                        t=prev['literal_u32']&~1
                        if START<=t<END:
                            queue.append(t); roots[t].add('literal_transfer'); item['resolved_target']=hex(t)
                indirect.append(item)
            if op=='bx': break
        if (op.startswith('pop') or op.startswith('ldm')) and re.search(r'\bpc\b',args):
            # A conditional return leaves a valid fallthrough path.
            if op.removesuffix('.w') in ('pop','ldm','ldmia','ldmdb'): break
        if op.startswith('ldr') and args.startswith('pc,'):
            indirect.append(dict(d, note='stack-return pattern' if args.startswith('pc, [sp]') else 'unresolved PC load'))
            if op in ('ldr','ldr.w'): break
        if op in ('tbb','tbh'):
            item=dict(d); item['preceding_instructions']=history[-8:]
            count=JUMP_COUNTS.get(a)
            item['audited_entry_count']=count
            if count:
                width=1 if op=='tbb' else 2
                offsets=struct.unpack_from('<'+('B' if width==1 else 'H')*count,B,a+4-BASE)
                item['targets']=[a+4+2*v for v in offsets];queue.extend(item['targets'])
            indirect.append(item);break
        history.append(d);a=nxt

ordered=[ins[k] for k in sorted(ins)]
dump('analysis/reachable_instructions.json',ordered)
dump('analysis/indirect_transfers.json',indirect)
dump('analysis/traversal_status.json',{'instruction_count':len(ins),'decoded_bytes':sum(i['size'] for i in ordered),
     'entry_count':len(roots),'call_edges':len(calls),'indirect_transfers':len(indirect),'overlapping_targets':overlaps,
     'scope':'vectors, verified DSP entry, direct edges; unresolved indirect targets omitted; not a complete CFG'})
dump('analysis/function_candidates.json',[{'address':hex(a),'evidence':sorted(e),'decoded':a in ins} for a,e in sorted(roots.items())])
ptrs=[]
for off in range(0,len(B)-3,4):
    v=struct.unpack_from('<I',B,off)[0]
    if v&1 and START<=v<END:ptrs.append((hex(BASE+off),hex(v&~1),(v&~1) in roots))
csvout('analysis/pointer_candidates.csv',['storage','target','already_a_root'],ptrs)
csvout('analysis/call_edges.csv',['instruction','target'],[(hex(a),hex(t)) for a,t in sorted(calls)])
csvout('analysis/literal_references.csv',['instruction','literal_address','u32_hex','float32'],
       [(hex(d['address']),hex(d['literal_address']),f"0x{d['literal_u32']:08x}",d['literal_f32']) for d in ordered if 'literal_u32' in d])
csvout('analysis/vectors.csv',['index','address','word','code_address'],
       [(n,hex(BASE+n*4),f'0x{v:08x}',hex(v&~1) if n and v&1 else '') for n,v in enumerate(vectors)])
csvout('analysis/strings_all.csv',['address','length','text'],
       [(hex(BASE+m.start()),len(m.group()),m.group().decode('ascii')) for m in re.finditer(rb'[\x20-\x7e]{5,}',B)])
header='; Recursive static traversal, not complete control flow.\n; See analysis/indirect_transfers.json for unresolved transfers.\n'
(ROOT/'disassembly/reachable_thumb.asm').write_text(header+'\n'.join(D.format(d) for d in ordered)+'\n')
for name,start,end in [('startup',0x21414,0x215a0),('main',0x22b68,0x24654),('control_scanner',0x263e4,0x26528),
                       ('initialization',0x26730,0x26c80),('splice_helpers',0x26c80,0x27518),
                       ('dsp_core',0x27518,0x2b600),('audio_interrupts',0x25700,0x263e4),('options_parser',0x22dc0,0x24654)]:
    (ROOT/f'disassembly/{name}.asm').write_text(header+'\n'.join(D.format(d) for d in ordered if 0x08000000+start<=d['address']<0x08000000+end)+'\n')
# Audit-sized snippets are generated from the recursive listing, not a sweep
# starting at an arbitrary halfword (some supplied decompilations did that).
for name,lo,hi in [('gene_and_morph_control',0x080278cc,0x08027b1e),('clock_gene_quantizer',0x0802ab6c,0x0802abda),
                   ('sparse_read',0x08028b5a,0x08028c5e),('dense_read',0x08029efe,0x08029f86),
                   ('record_routing',0x08028e00,0x08028ef4),('morph_launch_choice',0x0802842a,0x08028492)]:
    (ROOT/f'disassembly/{name}.asm').write_text(header+'\n'.join(D.format(d) for d in ordered if lo<=d['address']<hi)+'\n')
# A broad sweep is useful for finding missing roots, but deliberately NOT used as
# evidence of executable code, function extents or reference counts.
(ROOT/'disassembly/linear_sweep_thumb.asm').write_text(
    '; UNVALIDATED LINEAR SWEEP: includes literal data decoded as instructions.\n'+
    '\n'.join(D.format(d) for d in D.range(START,END))+'\n')
print(json.dumps(json.loads((ROOT/'analysis/traversal_status.json').read_text()),indent=2))
