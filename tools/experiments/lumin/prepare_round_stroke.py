"""Generate a namespaced, offline-only NanoVG candidate and live Flux geometry.

Inputs: pinned VCVRack/nanovg 0bebdb314aff9cfa28fde4744bcb037a2b3fd756
in work/nanovg. Original copyright/license is retained in generated source.
No SDK, installed Rack, or production source is modified.
"""
from pathlib import Path
import re
import hashlib
import json

root = Path(__file__).resolve().parents[3]
vendor = root / 'work/nanovg'
out = root / 'build/tools/round_stroke'
out.mkdir(parents=True, exist_ok=True)
names = set()
hashes = {}
for name in ('nanovg.c', 'nanovg.h', 'nanovg_gl.h', 'fontstash.h', 'stb_truetype.h', 'stb_image.h'):
    data = (vendor / name).read_bytes()
    hashes[name] = hashlib.sha256(data).hexdigest()
    (out / name).write_bytes(data)
    if name.endswith('.h') and name.startswith('nanovg'):
        names.update(re.findall(r'\b(nvg\w+)\s*\(', data.decode()))
(out / 'rename.h').write_text('\n'.join(f'#define {n} offline_{n}' for n in sorted(names)))
source = (vendor / 'nanovg.c').read_text()
if hashes['nanovg.c'] != '88e8f8081073f77f41df51319900a0082518d8e374736bda74817db09184c044':
    raise ValueError('NanoVG source does not match the pinned experimental revision')
marker = 'static NVGvertex* nvg__roundJoin('
assert source.count(marker) == 1
source = source.replace(marker, 'static NVGvertex* nvg__roundJoinReference(', 1)
insert = '''
// ALTERED SOURCE: offline Lumen experiment, endpoint reuse for two-sample joins.
static int offlineCandidateEnabled = 0;
static NVGvertex* nvg__roundJoin(NVGvertex* dst, NVGpoint* p0, NVGpoint* p1,
 float lw, float rw, float lu, float ru, int ncap, float fringe, float threshold) {
 float dot=p0->dx*p1->dx+p0->dy*p1->dy;
 float cross=p0->dx*p1->dy-p0->dy*p1->dx;
 int left=(p1->flags & NVG_PT_LEFT)!=0;
 if (!offlineCandidateEnabled || ncap<2 || ncap>64 ||
     (left ? cross>=-1e-6f : cross<=1e-6f) ||
     fabsf(p0->dx*p0->dx+p0->dy*p0->dy-1.f)>=1e-6f ||
     fabsf(p1->dx*p1->dx+p1->dy*p1->dy-1.f)>=1e-6f ||
     (ncap!=2 && dot<=threshold))
  return nvg__roundJoinReference(dst,p0,p1,lw,rw,lu,ru,ncap,fringe);
 float x0,y0,x1,y1;
 if(left) {
  nvg__chooseBevel(p1->flags & NVG_PR_INNERBEVEL,p0,p1,lw,&x0,&y0,&x1,&y1);
  nvg__vset(dst++,x0,y0,lu,1);
  nvg__vset(dst++,p1->x-p0->dy*rw,p1->y+p0->dx*rw,ru,1);
  nvg__vset(dst++,p1->x,p1->y,.5f,1);
  nvg__vset(dst++,p1->x-p0->dy*rw,p1->y+p0->dx*rw,ru,1);
  nvg__vset(dst++,p1->x,p1->y,.5f,1);
  nvg__vset(dst++,p1->x-p1->dy*rw,p1->y+p1->dx*rw,ru,1);
  nvg__vset(dst++,x1,y1,lu,1);
  nvg__vset(dst++,p1->x-p1->dy*rw,p1->y+p1->dx*rw,ru,1);
 } else {
  nvg__chooseBevel(p1->flags & NVG_PR_INNERBEVEL,p0,p1,-rw,&x0,&y0,&x1,&y1);
  nvg__vset(dst++,p1->x+p0->dy*rw,p1->y-p0->dx*rw,lu,1);
  nvg__vset(dst++,x0,y0,ru,1);
  nvg__vset(dst++,p1->x+p0->dy*lw,p1->y-p0->dx*lw,lu,1);
  nvg__vset(dst++,p1->x,p1->y,.5f,1);
  nvg__vset(dst++,p1->x+p1->dy*lw,p1->y-p1->dx*lw,lu,1);
  nvg__vset(dst++,p1->x,p1->y,.5f,1);
  nvg__vset(dst++,p1->x+p1->dy*rw,p1->y-p1->dx*rw,lu,1);
  nvg__vset(dst++,x1,y1,ru,1);
 }
 return dst;
}
'''
source = source.replace('static NVGvertex* nvg__bevelJoin(', insert+'\nstatic NVGvertex* nvg__bevelJoin(', 1)
needle='int ncap = nvg__curveDivs(w, NVG_PI, ctx->tessTol);'
assert source.count(needle)==1
source=source.replace(needle,needle+'\n float threshold=offlineCandidateEnabled ? cosf(2.f*NVG_PI/ncap)+1e-5f : 0.f;')
needle='nvg__roundJoin(dst, p0, p1, w, w, u0, u1, ncap, aa)'
assert source.count(needle)==1
source=source.replace(needle,'nvg__roundJoin(dst, p0, p1, w, w, u0, u1, ncap, aa, threshold)')
(out/'nanovg.c').write_text(source)
(out/'private.cpp').write_text('''#include <GL/glew.h>
#include "rename.h"
#define STBTT_STATIC
#define NVG_NO_STB
#include "nanovg.c"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"
#include "private_stroke_api.hpp"
PrivateStrokeApi privateStrokeApi() {
 return {nvgCreateGL2,nvgDeleteGL2,nvgBeginFrame,nvgEndFrame,nvgBeginPath,
 nvgMoveTo,nvgLineTo,nvgStrokeWidth,nvgStrokeColor,nvgLineCap,nvgLineJoin,nvgStroke,nvgInternalParams};
}
void privateStrokeCandidate(bool enabled) { offlineCandidateEnabled=enabled; }
''')

# Extract the current methods rather than maintain a hand-copied Flux waveform.
widget=(root/'src/IntegralFluxWidget.cpp').read_text()
for declaration in ('POINT_COUNT = 128;', 'PREVIEW_LUT_SIZE = 512;', 'WAVE_LINE_WIDTH = 1.4f;', 'WAVE_EDGE_PAD = 1.0f;'):
    if declaration not in widget:
        raise ValueError('Flux geometry constants changed: refresh the offline model')
def method(signature):
    start=widget.index(signature)
    opening=widget.index('{',start)
    depth=1
    end=opening+1
    while depth:
        depth+=(widget[end]=='{')-(widget[end]=='}')
        end+=1
    return widget[start:end]
flux=(root/'src/IntegralFlux.cpp').read_text()
functions=flux[flux.index('float IntegralFlux::shapeSignedForMode'):flux.index('IntegralFlux::FunctionShapeMode IntegralFlux::functionShapeModeFromParam')]
fields='''
struct FluxGeometry {
 static constexpr int POINT_COUNT=128,PREVIEW_LUT_SIZE=512;
 static constexpr float WAVE_LINE_WIDTH=1.4f,WAVE_EDGE_PAD=1.f;
 rack::math::Rect box={rack::math::Vec(0,0),rack::math::Vec(106,48)};
 std::array<Vec,POINT_COUNT> points{};
 struct SimplifiedPreviewPath {std::array<Vec,POINT_COUNT> points{};int count=0;};
 SimplifiedPreviewPath simplifiedFullPath,simplifiedRisePath,simplifiedFallPath;
 std::array<float,PREVIEW_LUT_SIZE> cachedRiseLut{},cachedFallLut{};
 float cachedLutCurveSigned=0,contourRiseRatio=0;
 int cachedLutShapeMode=0,peakPointIndex=64;
 bool cachedLutsValid=false,pointsValid=false;
'''
methods=['void rebuildSimplifiedPath(', 'void rebuildSimplifiedPaths()', 'static void buildSegmentLut(',
 'static float sampleSegmentLut(', 'void ensureSegmentLuts(', 'void rebuildPoints(']
(out/'flux_geometry.hpp').write_text('#include "IntegralFlux.hpp"\n#include "WavePreviewSimplifier.hpp"\n'+functions+fields+'\n'.join(map(method,methods))+'\n};\n')
hashes['IntegralFluxWidget.cpp']=hashlib.sha256(widget.encode()).hexdigest()
(out/'provenance.json').write_text(json.dumps({'revision':'0bebdb314aff9cfa28fde4744bcb037a2b3fd756','input_sha256':hashes},indent=2))
