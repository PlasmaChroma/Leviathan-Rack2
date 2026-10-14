#!/usr/bin/env python3
"""Build literal preset/editor/patch data; inert, no manufacturer executable invoked."""
from pathlib import Path
import json,re,csv,struct,hashlib
from bs4 import BeautifulSoup
R=Path(__file__).resolve().parents[1];S=R/'extracted';T=R/'tables'
def parse_preset(path):
 text=path.read_text(errors='replace'); stripped=re.sub(r'/\*.*?\*/','',text,flags=re.S)
 vals={}
 for k,v in re.findall(r'PARAMETER\s*:\s*(\w+)\s*:\s*([^\r\n}]+)',stripped):
  tok=v.strip().split()
  try: nums=[float(t) if any(c in t for c in '.eE') else int(t) for t in tok];vals[k]=nums[0] if len(nums)==1 else nums
  except ValueError: vals[k]=v.strip()
 name=re.search(r'PRESET_NAME\s*:\s*"([^"]*)"',stripped)
 return {'source':str(path.relative_to(S)),'name':name[1] if name else None,'has_ARB_header':stripped.lstrip().startswith('#ARB'),'values':vals}
paths=sorted((S/'factoryPresets').glob('*_preset.txt'))+sorted((S/'_factoryPresets').glob('*.txt'))+[S/f for f in ['configurationDataInitFile.txt','loadPresetOnStartup.txt','classic_mode.txt','follow_mode.txt','stereo_mode.txt'] if (S/f).exists()]
presets=[parse_preset(p) for p in paths];(T/'presets.json').write_text(json.dumps(presets,indent=2))
order=['alpha','beta','gamma','delta','epsilon','zeta'];factory=sorted([x for x in presets if x['source'].startswith('factoryPresets/')],key=lambda x:order.index(Path(x['source']).stem.split('_')[0]))
keys=list(dict.fromkeys(k for x in presets for k in x['values']))
with (T/'preset_matrix.csv').open('w',newline='') as f:
 w=csv.writer(f);w.writerow(['parameter']+[x['name'] for x in factory]);
 for k in keys:w.writerow([k]+[json.dumps(x['values'].get(k)) for x in factory])
soup=BeautifulSoup((S/'arbhar_Preset_Editor.html').read_text(),'html.parser')
editor=[]
for sel in soup.find_all(['select','input']):
 e={'tag':sel.name,'id':sel.get('id'),'attributes':sel.attrs}
 if sel.name=='select':e['options']=[{'value':x.get('value'),'label':x.get_text(' ',strip=True),'selected_in_html':x.has_attr('selected')} for x in sel.find_all('option')]
 editor.append(e)
(T/'preset_editor_fields.json').write_text(json.dumps(editor,indent=2))
# HTML details are local evidence, not downloaded website summaries.
(S.parent/'evidence'/'preset_editor_text.txt').write_text(soup.get_text('\n',strip=True))
net=json.loads((T/'pd_netlists.json').read_text());invalid=[];shm=[]
for fn,cs in net.items():
 for c in cs:
  n=len(c['objects'])
  for edge in c['connections']:
   if edge[0]>=n or edge[2]>=n:invalid.append({'file':fn,'canvas':c['id'],'edge':edge,'objects':n})
  for o in c['objects']:
   for m in re.finditer(r'\b(memread|memset|memdump|memwrite)\s+(\d+)',o['record']):
    shm.append({'file':fn,'canvas':c['id'],'object':o['index'],'operation':m[1],'index':int(m[2]),'record':o['record']})
(T/'shmem_patch_references.json').write_text(json.dumps(shm,indent=2))
# Extract saved Pure Data arrays independently of graph numbering.
arrays=[]
for p in S.glob('*.pd'):
 cur=None
 for rec in re.split(r'(?<!\\);\s*(?:\n|$)',p.read_text(errors='replace')):
  rec=' '.join(rec.splitlines()).strip()
  m=re.match(r'#X array (\S+) (\d+) (\w+)',rec)
  if m:
   cur={'file':p.name,'name':m[1],'size':int(m[2]),'values':[None]*int(m[2])};arrays.append(cur)
  elif rec.startswith('#A ') and cur:
   tok=rec.split()
   try:
    off=int(tok[1]);vv=list(map(float,tok[2:]));cur['values'][off:off+len(vv)]=vv
   except ValueError:pass
(T/'pd_saved_arrays.json').write_text(json.dumps(arrays,indent=2))
report={'patches':len(net),'canvases':sum(map(len,net.values())),'objects':sum(len(c['objects']) for cs in net.values() for c in cs),'connections':sum(len(c['connections']) for cs in net.values() for c in cs),'invalid_endpoint_references':invalid,'presets':len(presets),'factory_presets':len(factory),'saved_arrays':len(arrays),'shmem_references':len(shm)}
(T/'structural_validation.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
print('Factory preset differences:')
for k in keys:
 v=[x['values'].get(k) for x in factory]
 if any(z!=v[0] for z in v):print(k,v)
