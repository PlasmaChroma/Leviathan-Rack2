"""Summarize the two completed native full-stroke runs; standard library only."""
from pathlib import Path
import hashlib
import json
import re
import struct
import subprocess
import zlib

root=Path(__file__).resolve().parents[3]
folder=root/'doc/benchmarks/flux-full-stroke-20260910'
summary={'checkpoint':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),
         'scope':'Offline full-stroke CPU submission, excludes source geometry and Rack host work',
         'runs':{},'sha256':{}}
pattern=re.compile(r'full-stroke/([^/]+)/mode(\d)/(\d+\.\d+)x/(\d+)/CPU,n=(\d+),median_us=([\d.]+),p95_us=([\d.]+)')
for name in ('forward','reverse'):
    path=folder/(name+'.log')
    content=path.read_text()
    rows={}
    for mode,shape,scale,count,n,median,p95 in pattern.findall(content):
        assert int(n)==260
        rows[(shape,scale,count,mode)]={'median_us':float(median),'p95_us':float(p95)}
    assert len(rows)==60, 'incomplete timing scenarios'
    assert len(re.findall(r'/GPU,n=260,',content))==60, 'missing GPU results'
    assert content.count('gpu_missing=0')==60, 'missing GPU samples'
    final=re.search(r'validation_pairs=(\d+),baseline_exact=(\d+),changed_images=(\d+),max_mesh_error=([^,]+),max_relative_error=([^,]+),max_byte_error=(\d+)',content)
    assert final and int(final[1])==2900 and int(final[2])==2900, 'incomplete validation'
    comparisons=[]
    for shape in ('0','1'):
        for scale in ('1.00','1.19','2.00','4.00','8.00'):
            for count in ('1','16'):
                reference=rows[(shape,scale,count,'private-reference')]
                candidate=rows[(shape,scale,count,'candidate')]
                improvement=100*(1-candidate['median_us']/reference['median_us'])
                p95change=100*(candidate['p95_us']/reference['p95_us']-1)
                comparisons.append({'shape_mode':int(shape),'scale':float(scale),'count':int(count),
                  'reference':reference,'candidate':candidate,'median_improvement_percent':improvement,
                  'p95_change_percent':p95change,'cpu_screen_pass':improvement>=10 and p95change<=5})
    summary['runs'][name]={'environment':content.splitlines()[0], 'comparisons':comparisons,
      'image_pairs':int(final[1]),'baseline_exact_image_pairs':int(final[2]),'changed_candidate_images':int(final[3]),
      'maximum_mesh_error':float(final[4]),'maximum_relative_image_error':float(final[5]),'maximum_byte_error':int(final[6])}
    summary['sha256'][str(path.relative_to(root))]=hashlib.sha256(path.read_bytes()).hexdigest()
for relative in ('tools/experiments/lumin/prepare_round_stroke.py','tools/experiments/lumin/round_stroke_benchmark.cpp','tools/experiments/lumin/private_stroke_api.hpp','tools/experiments/lumin/analyze_round_stroke.py',
 'src/IntegralFluxWidget.cpp','src/IntegralFlux.cpp','build/tools/round_stroke/nanovg.c',
 'build/tools/round_stroke_benchmark.exe','work/nanovg/rack-2.6.6-nanovg-ref.json'):
    summary['sha256'][relative]=hashlib.sha256((root/relative).read_bytes()).hexdigest()
tail_content=(folder/'tails.log').read_text()
tail_rows=[]
for block in tail_content.split('tail_repeat=')[1:]:
    repeat=int(block.splitlines()[0])
    matches=pattern.findall(block)
    assert len(matches)==2 and all(int(m[4])==2000 for m in matches)
    values={m[0]:{'median_us':float(m[5]),'p95_us':float(m[6])} for m in matches}
    ref=values['private-reference'];candidate=values['candidate']
    improvement=100*(1-candidate['median_us']/ref['median_us'])
    p95change=100*(candidate['p95_us']/ref['p95_us']-1)
    tail_rows.append({'repeat':repeat,'shape_mode':int(matches[0][1]),'scale':float(matches[0][2]),'reference':ref,'candidate':candidate,
      'median_improvement_percent':improvement,'p95_change_percent':p95change,
      'cpu_screen_pass':improvement>=10 and p95change<=5})
assert len(tail_rows)==60
assert len(re.findall(r'/GPU,n=2000,',tail_content))==120
assert tail_content.count('gpu_missing=0')==120
summary['tail_followup']=tail_rows
summary['sha256']['tails.log']=hashlib.sha256((folder/'tails.log').read_bytes()).hexdigest()
summary['source_provenance']=json.loads((root/'build/tools/round_stroke/provenance.json').read_text())
(folder/'summary.json').write_text(json.dumps(summary,indent=2)+'\n')

# Convert the benchmark's exact RGB contact sheet without an imaging dependency.
ppm=(root/'build/tools/round-stroke-comparison.ppm').read_bytes()
header=re.match(rb'P6\n(\d+) (\d+)\n255\n',ppm)
assert header
w,h=map(int,header.groups())
rgb=ppm[header.end():]
assert len(rgb)==w*h*3
def chunk(name,data):
    return struct.pack('>I',len(data))+name+data+struct.pack('>I',zlib.crc32(name+data))
scan=b''.join(b'\0'+rgb[y*w*3:(y+1)*w*3] for y in range(h))
png=b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(scan))+chunk(b'IEND',b'')
(folder/'comparison.png').write_bytes(png)
all_rows=[r for run in summary['runs'].values() for r in run['comparisons']]
print('CPU screens passed:',sum(r['cpu_screen_pass'] for r in all_rows),'/',len(all_rows))
print('Longer tail screens passed:',sum(r['cpu_screen_pass'] for r in tail_rows),'/',len(tail_rows))
print('16-contour improvement range:',min(r['median_improvement_percent'] for r in all_rows if r['count']==16),
      max(r['median_improvement_percent'] for r in all_rows if r['count']==16))
