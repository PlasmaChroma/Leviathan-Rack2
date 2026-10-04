#!/usr/bin/env python3
from pathlib import Path
import json
import numpy as np
from PIL import Image

R=Path(__file__).resolve().parents[1]
D=R/'res/VTune/body-map'
SOURCE=R/'doc/VTune_BodyMap/res/VTune/body-map'
data=json.loads((SOURCE/'definition.json').read_text(encoding='utf-8'))
original=np.array(Image.open(R/'doc/VTune_BodyMap/reference/VFOutline.png').convert('RGB')).astype(np.float32)
outline=np.array(Image.open(D/'body_outline.png').convert('RGBA')).astype(np.float32)
reconstructed=outline[:,:,:3]*(outline[:,:,3:4]/255)
error=np.abs(reconstructed-original).max()
assert error<=1, error
assert np.all(outline[:,:,3][original.max(axis=2)==0]==0)
body=np.array(Image.open(SOURCE/'silhouette_debug.png'))>0
w,h=data['mask_width'],data['mask_height']
# Allow one output-pixel antialias fringe at a boundary, but no halo outside it.
allowed=body.copy()
for _ in range(5):
 expanded=allowed.copy()
 expanded[1:,:] |= allowed[:-1,:]
 expanded[:-1,:] |= allowed[1:,:]
 expanded[:,1:] |= allowed[:,:-1]
 expanded[:,:-1] |= allowed[:,1:]
 allowed=expanded
assert {p.name for p in D.iterdir()} == {f'band_{i}.png' for i in range(7)} | {'body_backing.png', 'body_outline.png'}
for prefix in ['band']:
 for i in range(7):
  im=Image.open(D/f'{prefix}_{i}.png')
  assert im.mode=='RGBA' and im.size==(w,h)
  a=np.asarray(im)[:,:,3]
  for x,y in data['holes_seed_xy']+[[0,0],[780,10],[30,300]]:
   assert a[min(h-1,int(y*h/1002)),min(w-1,int(x*w/788))]==0
  full=np.asarray(Image.fromarray(a).resize((788,1002),Image.Resampling.NEAREST))
  assert full[~allowed].max()==0
print(f'PASS: 7 aligned RGBA body masks; runtime assets only; negative-space/background transparency; outline black-matte reconstruction error {error:.4f}/255.')
