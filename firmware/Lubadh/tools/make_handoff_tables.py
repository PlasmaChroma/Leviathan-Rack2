#!/usr/bin/env python3
"""Generate evidence-linked presets, constants and a *reconstructed* fade table."""
from pathlib import Path
import json,csv,struct,ctypes,math,hashlib
ROOT=Path(__file__).resolve().parents[1]
P=json.loads((ROOT/'tables/factory_presets.json').read_text())
def flat(d,p=''):
 out={}
 for k,v in d.items():
  q=p+k
  if isinstance(v,dict):out.update(flat(v,q+'.'))
  else:out[q]=v
 return out
D=flat(P['01-Tape-Looper'])
meta={
'RecordSpeed':('enum',{0:'Variable, signed recording speed follows transport',1:'Fixed +1 record speed; independent playback pitch'},'0x47c08, 0x47818','Do not implement all presets as fixed-speed loop recording.'),
'PlaybackSpeed.Looping':('enum',{0:'Follow live speed',1:'Hold speed at tap activation'},'0x3a6ac, 0x4eea8','Per-tap speed, independent of one-shot setting.'),
'PlaybackSpeed.Oneshot':('enum',{0:'Follow live speed',1:'Hold speed at tap activation'},'0x4eea8','Separate preset field.'),
'MultiTap':('enum',{0:'Single musical playback head with transition crossfades',1:'Multiple simultaneously active logical engines'},'0x4eea8, 0x4e358','Four engines, five transition Tap slots per engine; not twenty ordinary voices.'),
'SpeedControl':('enum',{0:'Notched',1:'Stepped',2:'Smooth',3:'V/oct'},'0x37510; fillSpeedTable symbols','Exact notched/stepped table boundaries remain unprobed.'),
'SpeedMarkers':('array',None,'factory preset comments; fillSpeedTable','Preserve order/data. Listed defaults are magnitude markers; signed mapping requires fillSpeedTable.'),
'TimePot':('enum',{0:'Crossfade duration',1:'Speed slew time',2:'Tape emulation amount'},'0x37a44','Selects the user-defined third Time function, not all Time UI states.'),
'RecordJack':('enum',{0:'Latching record',1:'Gated record',2:'External clock'},'0x284b0, 0x3eed8','Clock is a configurable RECORD input role; do not unconditionally clock RETRIG.'),
'ExternalClock.Average':('interval count',None,'factory comments; 0x3eed8','Average inter-pulse durations; exact outlier/first-pulse handling unverified.'),
'ExternalClock.Resolution':('pulses/full loop',None,'factory comments; 0x3eed8','Zero selects currently selected Clock Division. Not necessarily MIDI PPQ.'),
'ExternalClock.Timeout':('seconds',None,'factory comments; 0x3eed8','Zero disables timeout. Actual timeout interaction remains to be tested.'),
'ClockDivisions':('enum',{0:'All',1:'Even',2:'Odd',3:'Powers of two'},'0x3b320','See clock_division_sets.json.'),
'Quantisation':('enum',{0:'All division set',1:'Even division set',2:'Odd division set',3:'Powers-of-two division set'},'0x37a44, 0x376ac, factory comments','Comment also says 0 disables: contradictory. Enable state is separate; do not conflate it with list selector.'),
'EraseRecord':('enum',{0:'Punch-in overwrite',1:'Tap tempo'},'factory comments; 0x28734','Changes Erase+Record gesture.'),
'MinLength':('samples',None,'0x376ac','Default minimum selected region, not proven universal first-record stopping threshold. Seconds=samples/rate.'),
'CrossfadeDuration':('milliseconds',[0,250],'0x376ac','Actual selected fade spans have a 128-sample floor on inspected path; zero is not evidence of discontinuous hard cuts.'),
'RetrigDelay':('milliseconds',None,'0x49244, factory comments','Delays jack retriggers; block countdown, not proven universal button delay.'),
'MaxDubLevel':('normalized gain',[0,1],'0x37a44, 0x47818, 0x387fc','Maximum available dub level; transition envelope can vary actual old-buffer coefficient.'),
'WowFlutterDepth':('normalized',[0,1],'0x37a44, 0x502c4','Effective amount = clamp(Time tape amount * preset depth). Full modulation not numerically probed.'),
'CrinkleDepth':('normalized',[0,1],'0x37a44, 0x502c4','Effective amount = clamp(Time tape amount * preset depth). RNG/filtered irregularity.'),
'TapeAge':('normalized',[0,1],'0x37a44, 0x4f524','Mix amount for active static five-one-pole TapeFilter, not standalone TapeAge class.'),
'Hysterisis':('normalized',[0,1],'0x37a44, 0x4fce8','Original misspelling preserved. Mix amount for 68/159/251/375-sample signed delay diffuser.'),
'Wear':('normalized',[0,1],'0x37a44, 0x4fc60','Mix x toward x*abs(x); applies in input and playback chains.'),
'Reverb':('normalized',[0,1],'0x37a44, 0x496c0','u=clamp(Time tape amount*preset); normalized dry/wet; decay=min(2u,0.9). Full plate not ported.'),
'Knee':('normalized',[0,1],'0x37a44, 0x4ffe0, 0x4fea8','Not multiplied by Tape Amount. Knee <= double 0.01 selects rational mode.'),
'Compensation':('normalized',[0,1],'0x37a44, 0x4ffe0, 0x4fea8','Multiplied by Tape Amount; pre-gain in rational mode, post-gain in knee mode.'),
'LowCutFreq':('Hz',None,'preset comments; 0x4f474','Input resonant biquad; independent of TapeAge static bank. Coefficient law not fully verified.'),
'LowCutQ':('normalized',None,'preset comments; 0x4f474','Vendor comments call it 0..1; exact Q transform is not established.'),
'HighCutFreq':('Hz',None,'preset comments; 0x4f474','Input one-pole lowpass, independent of TapeAge wet bank.'),
'SpeedSlewTime':('milliseconds',None,'0x37510, 0x37408','Observed finite block ramp with nominal 2.7 divisor and integer conversions; not an exponential time constant.'),
'CapacativeTouchMode':('enum',{0:'Off',1:'Stall',2:'Dip'},'factory comments; 0x27c74','Original misspelling preserved. Physical touch enable/calibration is separate.')}
rows=[]
for key,value in D.items():
 unit,choices,src,note=meta[key]
 rows.append(dict(key=key,default=value,unit=unit,values_or_documented_range=choices,evidence=src,implementation_note=note,preset_values={name:flat(p)[key] for name,p in P.items()}))
(ROOT/'tables/parameter_bible.json').write_text(json.dumps({'scope':'Supplied 2.1.0 preset schema; ranges without full validation are not hardware measurements','fields':rows},indent=2)+'\n')
text=['# Parameter bible — supplied Lúbadh 2.1.0 update','',f'{len(rows)} flattened fields; exact vendor spellings preserved. Default = 01-Tape-Looper. Numeric ranges here are documented preset ranges, not measured voltage limits.','']
for r in rows:
 text += [f"## `{r['key']}`",'',f"**Default:** `{r['default']}` · **Unit:** {r['unit']}  ",f"**Evidence:** `{r['evidence']}`",'',r['implementation_note'],'']
 if r['values_or_documented_range'] is not None:text += ['Values / documented range: `'+json.dumps(r['values_or_documented_range'])+'`','']
(ROOT/'report/PARAMETER_BIBLE.md').write_text('\n'.join(text))
clocks={'all':[1,2,3,4,5,6,7,8,9,10,11,12,16,24,32,64],'even':[2,4,6,8,10,12,16,24,32,64],'odd':[1,3,5,7,9,11],'powers_of_two':[1,2,4,8,16,32,64]}
(ROOT/'tables/clock_division_sets.json').write_text(json.dumps({'source':'Uploaded preset comments, corroborated list-selection functions','clock':clocks,'start_length_quantisation':{k:[x for x in v if x!=1] for k,v in clocks.items()},'caveat':'Preset Quantisation=0 chooses All; source prose is contradictory about disabling, which is separate runtime state.'},indent=2)+'\n')
constants={
 'archive_sha256':(hashlib.sha256((ROOT.parent/'lubadh-v2.1.0.tar.gz').read_bytes()).hexdigest() if (ROOT.parent/'lubadh-v2.1.0.tar.gz').exists() else json.loads((ROOT/'tables/recovered_constants.json').read_text())['archive_sha256']),
 'main_sha256':hashlib.sha256((ROOT/'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
 'requested_jack':{'rate_hz':48000,'period_frames':128,'periods':3,'literal_command':'jackd -P70 -dalsa -r48000 -p128 -n3 &'},
 'dsp_rate':{'float_bits':'0x47401241','value':49170.25390625,'status':'literal arithmetic constant, not verified hardware sample clock'},
 'sox_file_rate_hz':49148,'deck_buffer_samples':29502000,'deck_buffer_bytes':118008000,
 'cubic_inv6':{'float_bits':'0x3e2aaaad','value':0.16666670143604279},
 'softclip':{'A':28.274333953857422,'A_bits':'0x41e231d6','B':9.42477798461914,'B_bits':'0x4116cbe4'},
 'tape_filter_onepole_coefficients':{'HP100':156.5137939453125,'HP150':104.34252166748047,'LP500':31.30275535583496,'LP1000':15.65137767791748,'LP3000':5.21712589263916},
 'diffuser_delays_samples':[68,159,251,375],'diffuser_wet_scale':0.17499999701976776,
 'input_hardware_gain_old':3.0999999046325684,'input_hardware_gain_new':1.0,
 'fade_table':{'base_bss_va':'0x93f98','length':256,'initialization_va':'0x19c6c','step_bits':'0x3b808081','pi_bits':'0x40490fdb','ideal_shape':'(t + sin(pi*t/2))/2','table_status':'Reconstructed using host libm cosf; not extracted runtime memory or original uClibc output'},
 'first_input_adc_thresholds':{'speed_change_strict_greater_than':9,'time_change_strict_greater_than':29},
 'tap_structure':{'logical_engines':4,'tap_slots_per_engine':5,'engine_stride_bytes':680,'tap_stride_bytes':132,'warning':'Transition storage is not equivalent to 20 musical voices'}}
(ROOT/'tables/recovered_constants.json').write_text(json.dumps(constants,indent=2)+'\n')
libm=ctypes.CDLL('libm.so.6');libm.cosf.argtypes=[ctypes.c_float];libm.cosf.restype=ctypes.c_float
libm.fmaf.argtypes=[ctypes.c_float]*3;libm.fmaf.restype=ctypes.c_float
f=lambda x:ctypes.c_float(x).value
step=struct.unpack('<f',struct.pack('<I',0x3b808081))[0];pi=struct.unpack('<f',struct.pack('<I',0x40490fdb))[0]
t=step;table=[0.];coords=[0.]
for i in range(1,256):
 cos=libm.cosf(f(t*pi));q=libm.fmaf(-cos,.5,.5)
 val=f(f(f(math.sqrt(q))+t)*.5);table.append(val);coords.append(t);t=f(t+step)
with (ROOT/'tables/reconstructed_xfade_table.csv').open('w',newline='') as fp:
 w=csv.writer(fp);w.writerow(['index','accumulated_float_t','reconstructed_value','float32_bits','ideal_at_i_over_255'])
 for i,(x,y) in enumerate(zip(coords,table)):w.writerow([i,x,y,hex(struct.unpack('<I',struct.pack('<f',y))[0]),.5*(i/255+math.sin(math.pi*i/510))])
(ROOT/'tables/reconstructed_xfade_table.f32le').write_bytes(struct.pack('<256f',*table))
print('parameter fields',len(rows),'fade endpoints',table[0],table[-1], 't last',coords[-1])
