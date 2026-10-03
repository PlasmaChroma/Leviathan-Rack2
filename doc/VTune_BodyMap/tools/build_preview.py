#!/usr/bin/env python3
"""Build an offline browser preview using the same baked layers and definitions."""
from pathlib import Path
import base64,json
ROOT=Path(__file__).resolve().parents[1]
D=ROOT/'res/VTune/body-map'
def data_uri(name):
 return 'data:image/png;base64,'+base64.b64encode((D/name).read_bytes()).decode()
d=json.loads((D/'definition.json').read_text())
assets={name:data_uri(name) for name in ['body_backing.png','body_outline.png']+
        [f'band_{i}.png' for i in range(7)]+[f'tone_{i}.png' for i in range(7)]}
html='''<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>V.Tune · Body Map Preview</title>
<style>
:root{color-scheme:dark;font-family:system-ui,sans-serif;background:#11151b;color:#e7edf5}
*{box-sizing:border-box}body{max-width:1150px;margin:0 auto;padding:34px 26px 60px}
h1{font-size:30px;letter-spacing:-.8px;margin:3px 0 10px}h2{font-size:17px;font-weight:600}
p{line-height:1.55;color:#aebbc9}header{max-width:900px}.eyebrow{font-size:12px;letter-spacing:2px;color:#afdacb}
.grid{display:grid;grid-template-columns:minmax(320px,1.1fr) minmax(320px,1fr);gap:24px;margin-top:26px}
.card{border:1px solid #303a45;border-radius:18px;padding:22px;background:#19202a}
.visuals{display:flex;flex-wrap:wrap;align-items:center;justify-content:space-around;gap:14px;min-height:380px;overflow:hidden}
.figure{display:flex;flex-direction:column;align-items:center;gap:12px}.figure small{color:#9aa9bb;font-size:11px}
.stage{background:#0a0c10;border-radius:10px;padding:12px;transition:background .2s}
.stage.light{background:#dce2e8}canvas{display:block}.number{font-size:30px;font-weight:650;font-variant-numeric:tabular-nums;color:#e6f5ef;letter-spacing:-.5px}
label{display:block;font-size:12px;color:#aebbc9;margin:18px 0 8px}input[type=range]{width:100%;accent-color:#aed7c4}input[type=number],select{background:#0e141d;border:1px solid #465465;color:#e6edf5;border-radius:8px;padding:10px;font-size:14px;width:100%}
.row{display:flex;gap:12px;align-items:center}.row>div{flex:1}.toggles{display:flex;gap:18px;flex-wrap:wrap;margin-top:18px}.toggles label{margin:0;font-size:13px}.toggles input{accent-color:#aed7c4}
.pills{display:flex;gap:7px;flex-wrap:wrap;margin-top:18px}button{cursor:pointer;border-radius:6px;border:1px solid #47586a;color:#d5e6f1;background:#25303e;padding:7px 10px;font-size:12px}button:hover{background:#374656}
.readout{margin-top:20px;min-height:78px}.line{font-size:12px;display:grid;grid-template-columns:12px 1fr 44px;gap:8px;align-items:center;margin:9px 0}.dot{height:9px;width:9px;border-radius:50%}.pct{font-variant-numeric:tabular-nums;text-align:right;color:#a7bacb}
.note{font-size:12px;color:#b3b7bf}.tag{font-size:11px;border:1px solid #485461;padding:4px 8px;border-radius:5px;display:inline-block;color:#c6d8e1}
details{margin-top:24px;border-top:1px solid #33414f;padding-top:18px}summary{cursor:pointer;font-size:14px;color:#d2dfe8}table{width:100%;border-collapse:collapse;margin-top:16px;font-size:12px;text-align:left}th,td{padding:10px;border-bottom:1px solid #303c49;line-height:1.5;vertical-align:top}th{color:#b3c7d5}footer{font-size:12px;color:#8e9cae;margin-top:24px;line-height:1.7}
@media(max-width:800px){.grid{grid-template-columns:1fr}.visuals{min-height:370px}body{padding:20px 14px}.card{padding:16px}}
</style></head><body>
<header><div class="eyebrow">LEVIATHAN / V.TUNE</div><h1>A frequency, gently mapped.</h1>
<p>Interactive rendering reference for the supplied <i>Sound+Body</i> report. Broad anatomical associations and symbolic solfeggio points remain separate. This is an illustration of the report—not a physiological simulation, therapeutic guide, or measurement of organ activity.</p></header>
<div class="grid"><section class="card"><div class="row"><h2 style="flex:1">Body display</h2><span class="tag">Same aligned textures as C++</span></div>
<div class="visuals"><div class="figure"><div class="stage"><canvas id="body"></canvas></div><small>260 px wide · inspection scale</small></div>
<div class="figure"><div class="stage"><canvas id="small"></canvas></div><small>100 px wide · small-panel check</small></div></div>
<div class="toggles"><label><input id="light" type="checkbox"> Light surround</label><label><input id="backing" type="checkbox" checked> Black body backing</label></div>
<p class="note">Soft zones are clipped to the body, including the negative spaces under the arms. The original outline is composited last. Browser preview—not a Rack screenshot.</p></section>
<section class="card"><div class="number" id="frequencyLabel">174.00 Hz</div><p class="note" id="domain">Vessel target-center frequency · 20–2000 Hz</p>
<label for="logfreq">Frequency (logarithmic)</label><input id="logfreq" type="range" min="0" max="1" step="0.0001">
<div class="row"><div><label for="frequency">Exact frequency, Hz</label><input id="frequency" type="number" step=".01" value="174"></div><div><label for="opacity">Overlay intensity</label><input id="opacity" type="range" min="0" max="1" step=".01" value=".6"></div></div>
<label for="mode">Mapping layer</label><select id="mode"><option value="0">Report regions · illustrative</option><option value="1">Solfeggio · symbolic</option><option value="2">Combined · illustrative + symbolic</option><option value="3">Off · outline only</option></select>
<div class="toggles"><label><input id="connected" type="checkbox" checked> Linked frequency</label><label><input id="sweep" type="checkbox"> Sweep</label></div>
<div class="pills" id="presets"></div><div class="readout" id="readout"></div>
<p class="note">Band edges crossfade over 0.16 octaves; visual states ease over 90 ms. A frequency jump crossfades the endpoint regions rather than sweeping through intermediate anatomy.</p>
</section></div>
<details><summary>Mapping definitions, source limits, and implementation choices</summary>
<p class="note">The seven broad bands come from the report’s “Synthesis: Frequency ↦ Body-Part Map.” Color, spatial position, Gaussian size, relative weights, transition width, and timing are authored design choices. Highlights are intentionally coarse rather than anatomically precise gland or brain-region silhouettes.</p>
<table><thead><tr><th>Report band</th><th>Screen regions</th><th>Pastel</th></tr></thead><tbody id="definitionRows"></tbody></table>
<p class="note">Solfeggio points use a ±70-cent visual tolerance, not a biological resonance width. The explicit frequency list places 528 Hz at the solar plexus; the report’s conclusion is less consistent. The 963 Hz upper-head motif is an artistic placement based on the report’s pineal/unity association; the report does not explicitly name it the crown chakra. The incomplete Aparmita chart is not expanded or guessed. A 174 Hz organ-specific highlight is not invented.</p>
</details>
<footer>This file is self-contained and makes no network requests. The preview uses the same source-canvas registration, assets, band equations, and smoothing constant as the C++ implementation. NanoVG compositing, mipmap filtering, Rack brightness, and host scaling still require in-Rack visual validation.</footer>
<script>
const DEF = __DEFINITION__;
const ASSET_DATA = __ASSETS__;
const $=id=>document.getElementById(id);
const clamp=x=>Math.max(0,Math.min(1,Number.isFinite(x)?x:0));
const smooth=x=>{const t=clamp(x);return t*t*(3-2*t)};
function evaluate(hz,mode){
 const out=Array(14).fill(0);
 if(!Number.isFinite(hz)||hz<20||hz>2000||mode===3)return out;
 if(mode===0||mode===2){let prev=1;const gain=mode===2?DEF.combined_report_gain:1;
  for(let i=0;i<6;i++){const b=smooth((Math.log2(hz/DEF.bands[i].high_hz)+DEF.boundary_half_width_octaves)/(2*DEF.boundary_half_width_octaves));out[i]=gain*Math.max(0,prev-b);prev=b}out[6]=gain*prev;}
 if(mode===1||mode===2){const gain=mode===2?DEF.combined_symbolic_gain:1;
  DEF.tones.forEach((tone,i)=>{const cents=Math.abs(1200*Math.log2(hz/tone.frequency_hz));out[7+i]=gain*smooth(1-cents/DEF.tone_support_cents)});}
 return out;
}
window.bodyMapEvaluate=evaluate;
let hz=174,current=Array(14).fill(0),last=null,images={},ready=false,sweepPhase=0;
const names=[...DEF.bands.map(b=>b.label),...DEF.tones.map(t=>t.frequency_hz+' Hz · '+t.label)];
const colors=[...DEF.bands.map(b=>b.color),...DEF.tones.map(t=>t.color)];
function setHz(value){hz=value;$('frequency').value=Number.isFinite(hz)?Math.round(hz*100)/100:'';$('logfreq').value=clamp(Math.log2(Math.max(hz,20)/20)/Math.log2(100));}
$('logfreq').addEventListener('input',()=>{setHz(20*Math.pow(100,Number($('logfreq').value)));$('sweep').checked=false});
$('frequency').addEventListener('input',()=>{hz=Number($('frequency').value);$('logfreq').value=clamp(Math.log2(Math.max(hz,20)/20)/Math.log2(100));$('sweep').checked=false});
$('light').addEventListener('change',()=>document.querySelectorAll('.stage').forEach(e=>e.classList.toggle('light',$('light').checked)));
for(const f of [25,45,75,174,396,417,528,639,852,963,1500]){const b=document.createElement('button');b.textContent=f+' Hz';b.onclick=()=>{setHz(f);$('sweep').checked=false};$('presets').append(b)}
DEF.bands.forEach(b=>{const tr=document.createElement('tr');[b.source_label,b.label,b.color].forEach(v=>{const td=document.createElement('td');td.textContent=v;tr.append(td)});$('definitionRows').append(tr)});
setHz(174);
Promise.all(Object.entries(ASSET_DATA).map(([name,src])=>new Promise((resolve,reject)=>{const im=new Image();im.onload=()=>{images[name]=im;resolve()};im.onerror=reject;im.src=src}))).then(()=>{ready=true;window.bodyMapReady=true}).catch(e=>{$('domain').textContent='Image load error';console.error(e)});
function draw(canvas,width){
 const height=width*DEF.source_height/DEF.source_width,dpr=window.devicePixelRatio||1;
 const iw=Math.round(width*dpr),ih=Math.round(height*dpr);
 if(canvas.width!==iw||canvas.height!==ih){canvas.width=iw;canvas.height=ih;canvas.style.width=width+'px';canvas.style.height=height+'px'}
 const ctx=canvas.getContext('2d');ctx.setTransform(dpr,0,0,dpr,0,0);ctx.clearRect(0,0,width,height);if(!ready)return;
 ctx.imageSmoothingEnabled=true;ctx.globalAlpha=1;
 if($('backing').checked)ctx.drawImage(images['body_backing.png'],0,0,width,height);
 for(let i=0;i<14;i++){const a=current[i]*Number($('opacity').value);if(a<=1/1024)continue;ctx.globalAlpha=a;ctx.drawImage(images[(i<7?'band_'+i:'tone_'+(i-7))+'.png'],0,0,width,height)}
 ctx.globalAlpha=1;ctx.drawImage(images['body_outline.png'],0,0,width,height);
}
function frame(t){const dt=last===null?1/60:Math.min(.25,(t-last)/1000);last=t;
 if($('sweep').checked){sweepPhase+=dt*.08;setHz(20*Math.pow(100,(1-Math.cos(sweepPhase*2*Math.PI))*.5))}
 const connected=$('connected').checked,mode=Number($('mode').value),effective=connected?hz:0,target=evaluate(effective,mode),a=-Math.expm1(-dt/DEF.ui_smoothing_seconds);
 current=current.map((v,i)=>{let r=clamp(v+a*(target[i]-v));return r<1e-6&&target[i]===0?0:r});
 $('frequencyLabel').textContent=Number.isFinite(hz)?hz.toFixed(2)+' Hz':'No frequency';
 $('domain').textContent=!connected?'Disconnected · highlights fade out':hz<20||hz>2000?'Outside report range · no mapped highlight':'Vessel target-center frequency · 20–2000 Hz';
 draw($('body'),260);draw($('small'),100);
 const r=$('readout');r.replaceChildren();
 current.forEach((v,i)=>{if(v<.025)return;const row=document.createElement('div');row.className='line';const dot=document.createElement('span');dot.className='dot';dot.style.background=colors[i];const label=document.createElement('span');label.textContent=names[i];const pct=document.createElement('span');pct.className='pct';pct.textContent=Math.round(v*100)+'%';row.append(dot,label,pct);r.append(row)});
 if(!r.children.length){const p=document.createElement('p');p.className='note';p.textContent=mode===1?'No nearby symbolic tone at this frequency.':'Outline only.';r.append(p)}
 window.bodyMapWeights=current;requestAnimationFrame(frame);
}
requestAnimationFrame(frame);
</script></body></html>'''
html=html.replace('__DEFINITION__',json.dumps(d,ensure_ascii=False)).replace('__ASSETS__',json.dumps(assets))
(ROOT/'VTune_BodyMap_Preview.html').write_text(html)
print('Wrote preview:',(ROOT/'VTune_BodyMap_Preview.html').stat().st_size,'bytes')
