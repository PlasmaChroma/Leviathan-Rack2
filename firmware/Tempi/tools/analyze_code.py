#!/usr/bin/env python3
"""Conservative PIC18 recursive-disassembly/xref exporter for the recovered image.
Byte-addressed program memory; labels are analyst names, not recovered symbols.
Does not emulate indirect PCL writes or resolve indirect RAM access.
"""
from pathlib import Path
from collections import defaultdict, deque, Counter
from dataclasses import asdict
import csv, json
from pic18 import decode, read_hex, SKIPS, CONDS, SFR
ROOT=Path(__file__).resolve().parents[1]
mem=read_hex(ROOT/'firmware/tempi71_recovered.hex')
STOP={'RETURN','RETFIE','RETLW','RESET','SLEEP','DW'}
roots=[0x800,0x808]
names={0x800:'application_reset_vector',0x808:'timer2_interrupt',0x11bc:'startup_trampoline',0x779a:'c_runtime_startup',0x8998:'main',0x81f8:'hardware_init',0x11c0:'button_ui_service',0x7336:'six_button_edge_service',0x78fc:'state_adc_quantizer',0x972a:'eeprom_read_byte',0x9576:'eeprom_write_byte',0x9710:'eeprom_write_base_offset',0x9318:'adc_read_wrapper',0x97ce:'adc_busy',0x97d4:'adc_start',0x9742:'led_serial_shift_one',0x9756:'led_serial_shift_zero'}
names.update({
0x71aa:'select_bus_parse_byte',0x7a5c:'state_selection_service',0x4b16:'leading_tempo_service',
0x340a:'human_programming_service',0x65de:'recompute_ratio_and_phase',0x234a:'clock_timing_commit_service',
0x4f66:'mod_routing_service',0x3f32:'led_ui_service',0x8540:'load_active_state_from_cache',
0x87fa:'load_one_eeprom_state_to_cache',0x88d4:'select_bus_copy_or_store',0x871a:'store_dirty_states',
0x8b16:'state_dirty_bitmap',0x976a:'load_all_states',0x964a:'reload_dirty_states',
0x9618:'write_ratio_byte',0x93b2:'write_phase_byte',0x96a6:'write_enable_mask',
0x7e54:'ratio_to_halfperiod',0x7f8e:'alignment_lcm_u16',0x8446:'lcg64_next_byte',
0x8e5a:'multiply_u64',0x8ff8:'multiply_u32',0x8a5a:'divide_s32',0x9176:'divide_u32',
0x9256:'remainder_u32',0x94fe:'remainder_u16',0x6e56:'factory_reset_states',
0x9678:'store_leading_tempo',0x5b66:'build_adc_calibration_thresholds',0x7d08:'mesh_bitmap',
0x8ee6:'initialize_current_state',0x7006:'copy_state_or_bank',0x6bfa:'paste_or_mutate',
0x953c:'led_shift_u16',0x979e:'led_latch',0x93fc:'recompute_alignment_length',
0x9368:'divide_u16',0x90fe:'divide_s16',0x8dc2:'store_global_settings',0x8bcc:'ratio_alignment_factor',0x7bba:'load_global_settings',0x693a:'calibration_ui_service'
})
def is_pcl_write(i):
    return (i.mnemonic=='MOVFF' and i.dst==0xff9) or (i.mnemonic not in {'MOVF','BTFSS','BTFSC','MULWF','CPFSEQ','CPFSGT','CPFSLT','TSTFSZ','LFSR'} and i.reg()==0xff9 and i.d!=0)
def succ(i, calls=True):
    n=i.address+i.size
    if i.mnemonic in STOP or is_pcl_write(i): return []
    if i.mnemonic in {'GOTO','BRA'}: return [i.target]
    if i.mnemonic in CONDS: return [n,i.target]
    if i.mnemonic in SKIPS:return [n,n+decode(mem,n).size]
    if i.mnemonic in {'CALL','RCALL'}:return [n,i.target] if calls else [n]
    return [n]
ins={};todo=roots[:];outside=set();dynamic=[]
while todo:
    pc=todo.pop()
    if pc in ins:continue
    if pc not in mem or pc+1 not in mem or not 0x800<=pc<0x10000:
        outside.add(pc);continue
    i=decode(mem,pc);ins[pc]=i
    if is_pcl_write(i):dynamic.append(pc)
    todo.extend(succ(i))
entries=set(roots+[0x11bc,0x779a,0x8998])|{i.target for i in ins.values() if i.mnemonic in {'CALL','RCALL'}}
# Per-entry intraprocedural BSR analysis. Calls are conservatively unknown.
# In state set: -1 means unknown. A literal MOVLB can recover precision.
all_banks=defaultdict(set);func=[];calls=[];owners=defaultdict(set)
for ent in sorted(entries):
    q=deque([(ent,frozenset({-1}))]);st={};pcs=set()
    while q:
        pc,banks=q.popleft()
        if pc not in ins:continue
        if pc!=ent and pc in entries:continue
        old=st.get(pc,frozenset());new=old|banks
        if old==new:continue
        st[pc]=new;pcs.add(pc);i=ins[pc]
        out=frozenset({i.literal}) if i.mnemonic=='MOVLB' else new
        # Explicit BSR writes also lose precision (rare/not expected).
        if (i.mnemonic=='MOVFF' and i.dst==0xfe0) or (i.mnemonic in {'MOVWF','CLRF','SETF'} and i.reg()==0xfe0):out=frozenset({-1})
        if i.mnemonic in {'CALL','RCALL'}:out=frozenset({-1})
        for nex in succ(i,False):q.append((nex,out))
    for pc,b in st.items():all_banks[pc].update(b);owners[pc].add(ent)
    local_calls=[i for p,i in ins.items() if p in pcs and i.mnemonic in {'CALL','RCALL'}]
    for i in local_calls:calls.append({'caller':f'0x{ent:06X}','site':f'0x{i.address:06X}','callee':f'0x{i.target:06X}','name':names.get(i.target,'')})
    func.append({'entry':f'0x{ent:06X}','analyst_name':names.get(ent,f'sub_{ent:06X}'),'instruction_count':len(pcs),'covered_bytes':sum(ins[p].size for p in pcs),'min_pc':f'0x{min(pcs):06X}' if pcs else '', 'max_pc':f'0x{max(pcs):06X}' if pcs else '', 'call_count':len(local_calls)})

def csvout(name,rows):
    if not rows:return
    with (ROOT/'analysis'/name).open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
csvout('functions.csv',func);csvout('calls.csv',calls)
xrefs=[];directs=[];out=[]
for pc,i in sorted(ins.items()):
    banks=all_banks[pc];bank=next(iter(banks)) if len(banks)==1 and -1 not in banks else None
    label=(names.get(pc,'') or (f'sub_{pc:06X}' if pc in entries else ''))
    if label:out+=['',label+':']
    raw=f'{i.word:04X}'+(f' {i.word2:04X}' if i.word2 is not None else '     ')
    out.append(f'{pc:06X}: {raw}  {i.text(bank)}')
    refs=[]
    if i.mnemonic=='MOVFF':refs=[('read',i.src),('write',i.dst)]
    elif i.mnemonic!='LFSR' and i.f is not None:
        r=i.reg(bank)
        if r is not None:
            if i.mnemonic in {'MOVWF','CLRF','SETF'}:mode='write'
            elif i.mnemonic in {'BTFSS','BTFSC','CPFSEQ','CPFSGT','CPFSLT','TSTFSZ','MULWF'} or i.d==0:mode='read'
            else:mode='read_write'
            refs=[(mode,r)]
    for mode,r in refs:
        row={'pc':f'0x{pc:06X}','register':f'0x{r:03X}','name':SFR.get(r,f'ram_{r:03X}'),'operation':mode,'bit':'' if i.bit is None else i.bit,'instruction':i.text(bank),'owners':';'.join(f'0x{x:06X}' for x in sorted(owners[pc]))}
        directs.append(row)
        if r in SFR:xrefs.append(row)
(ROOT/'analysis/disassembly_reachable.asm').write_text('; Recursive PIC18 disassembly. Program addresses are BYTE addresses.\n; Unresolved bank operands intentionally stay [BSR:xx].\n'+'\n'.join(out)+'\n')
csvout('sfr_xrefs.csv',xrefs);csvout('direct_data_xrefs.csv',directs)
covered={a for i in ins.values() for a in range(i.address,i.address+i.size)}
uncovered=sorted(a for a in mem if 0x800<=a<0x10000 and a not in covered)
ranges=[]
for a in uncovered:
    if ranges and a==ranges[-1][1]+1:ranges[-1][1]=a
    else:ranges.append([a,a])
summary={'roots':[f'0x{x:06X}' for x in roots],'reachable_instructions':len(ins),'reachable_bytes':len(covered),'function_entries':len(entries),'direct_call_sites':sum(i.mnemonic in {'CALL','RCALL'} for i in ins.values()),'instruction_histogram':dict(Counter(i.mnemonic for i in ins.values())),'unrecovered_targets':[hex(x) for x in sorted(outside)],'unresolved_pcl_writes':[hex(x) for x in sorted(dynamic)],'uncovered_flash_ranges':[{'start':f'0x{a:06X}','end':f'0x{b:06X}','length':b-a+1} for a,b in ranges],'cautions':['A reachable address is not proof every branch can execute.','Call entries are not source-level function boundaries.','Indirect control flow is not guessed.','SFR names assume PIC18(L)F46K22-family compatibility.']}
(ROOT/'analysis/code_summary.json').write_text(json.dumps(summary,indent=2)+'\n')
(ROOT/'analysis/instructions.json').write_text(json.dumps([asdict(i) for _,i in sorted(ins.items())],indent=2)+'\n')
lines=['digraph Tempi {','rankdir=LR;','node [shape=box];']
for f in func:lines.append(f'"{f["entry"]}" [label="{f["analyst_name"]}\\n{f["entry"]}"];')
for ca,ce in sorted({(r['caller'],r['callee']) for r in calls}):lines.append(f'"{ca}" -> "{ce}";')
lines.append('}');(ROOT/'analysis/callgraph.dot').write_text('\n'.join(lines)+'\n')
print(json.dumps(summary,indent=2))
