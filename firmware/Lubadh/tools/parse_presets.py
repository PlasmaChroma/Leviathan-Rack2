#!/usr/bin/env python3
"""Parse the deliberately limited Hjson subset used by supplied factory presets.
Does not execute JavaScript or any update scripts. Preserves vendor key spelling.
"""
from pathlib import Path
import re,json,csv
ROOT=Path(__file__).resolve().parents[1]
TOKEN=re.compile(r'\s*(?:(-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)|([A-Za-z_][A-Za-z_0-9-]*)|([{}\[\]:,]))')
def parse(text):
 text=re.sub(r'/\*.*?\*/','',text,flags=re.S)
 text=re.sub(r'//[^\n]*','',text)
 tokens=[];p=0
 while p<len(text):
  m=TOKEN.match(text,p)
  if not m:
   if not text[p:].strip():break
   raise ValueError('Unexpected Hjson near '+repr(text[p:p+80]))
  tokens.append(m.group(1) or m.group(2) or m.group(3));p=m.end()
 i=0
 def value():
  nonlocal i
  tok=tokens[i];i+=1
  if tok=='{':
   obj={}
   while tokens[i]!='}':
    key=tokens[i];i+=1
    assert tokens[i]==':';i+=1;obj[key]=value()
    if tokens[i]==',':i+=1
   i+=1;return obj
  if tok=='[':
   arr=[]
   while tokens[i]!=']':
    arr.append(value())
    if tokens[i]==',':i+=1
   i+=1;return arr
  if re.fullmatch(r'-?\d+',tok):return int(tok)
  return float(tok)
 out=value();assert i==len(tokens);return out

def flatten(obj,prefix=''):
 for k,v in obj.items():
  key=f'{prefix}.{k}' if prefix else k
  if isinstance(v,dict):yield from flatten(v,key)
  else:yield key,v
if __name__=='__main__':
 records={p.stem:parse(p.read_text()) for p in sorted((ROOT/'extracted/presets/new_factory').glob('*.txt'))}
 (ROOT/'tables/factory_presets.json').write_text(json.dumps(records,indent=2)+'\n')
 flat={k:dict(flatten(v)) for k,v in records.items()};keys=list(next(iter(flat.values())))
 assert all(set(v)==set(keys) for v in flat.values())
 with (ROOT/'tables/factory_preset_matrix.csv').open('w',newline='') as f:
  w=csv.writer(f);w.writerow(['parameter',*flat])
  for key in keys:w.writerow([key,*[json.dumps(v[key]) for v in flat.values()]])
 (ROOT/'tables/factory_preset_matrix.md').write_text('| Parameter | '+' | '.join(flat)+' |\n|'+'---|'*(len(flat)+1)+'\n'+''.join('| '+key+' | '+' | '.join(str(v[key]) for v in flat.values())+' |\n' for key in keys))
 print(json.dumps(records,indent=2))
