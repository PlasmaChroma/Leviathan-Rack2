#!/usr/bin/env python3
import json,sys,pathlib
r=pathlib.Path(__file__).resolve().parents[1]
j=json.loads((r/'tables/pd_netlists.json').read_text());p=sys.argv[1];ci=int(sys.argv[2]) if len(sys.argv)>2 else 0;c=j[p][ci]
for o in c['objects']: print(f"{o['index']:3}: {o['record']}")
if '--edges' in sys.argv:
 print('\nEDGES:')
 for a,ao,b,bi in c['connections']:
  print(f"{a}:{ao} {c['objects'][a]['record']} -> {b}:{bi} {c['objects'][b]['record']}")
