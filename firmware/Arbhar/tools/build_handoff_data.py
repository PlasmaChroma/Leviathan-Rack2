#!/usr/bin/env python3
"""Build handoff schema from extracted configuration plus labelled analysis notes."""
from pathlib import Path
import csv, json
R=Path(__file__).resolve().parents[1]
def save(name, data): (R/name).write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n')
presets=json.loads((R/'tables/presets.json').read_text())
factory={p['source'].split('/')[-1].split('_')[0]:p for p in presets if p['source'].startswith('factoryPresets/')}
init=next(p for p in presets if p['source']=='configurationDataInitFile.txt')
fields=json.loads((R/'tables/preset_editor_fields.json').read_text())
byid={f['id']:f for f in fields}
keys=sorted(set().union(*(p['values'].keys() for p in presets)))
enums={
'LoadConfiguration':{0:'nothing',1:'preset',2:'layers',3:'scene'},
'InputMode':{0:'mono',1:'stereo'},'AnalogEmulation':{0:'disabled',1:'enabled'},
'PhaseSwitch':{0:'phase-inverted',1:'phase-corrected'},
'CaptureCVMode':{0:'latch',1:'momentary',2:'retrigger'},'CaptureButtonMode':{0:'latch',1:'momentary'},
'FollowMode':{0:'Scan',1:'Follow'},'FollowSpeedDirection':{0:'unidirectional',1:'bidirectional',2:'inverted-control unidirectional'},
'FollowPositionOffsetWithScanCV':{0:'knob and CV control speed/offset policy',1:'Scan knob only offset policy; see original configuration comments'},
'ModCV':{0:'none',1:'panning',2:'hold',3:'reverb',4:'delay'},
'ReverbReset':{0:'disabled',1:'Strike',2:'onset',3:'Strike and onset'},'EffectOrder':{0:'parallel',1:'series'},
'OnsetMode':{1:'alpha',2:'beta',3:'gamma',4:'delta',5:'epsilon',6:'zeta'}}
# Exact editor options supersede this concise semantic map where a matching field exists.
for key in keys:
 fid={'LoadConfiguration':'loadConfig'}.get(key,key[0].lower()+key[1:])
 if fid in byid and byid[fid].get('options'):
  opts=byid[fid]['options']
  if all(o['value'].lstrip('-').isdigit() for o in opts):
   enums[key]={int(o['value']):o['label'] for o in opts}
notes={
'StrikeCVDelay':'Milliseconds; Classic=0, other named factories=10, initialization fallback=10. Not a universal default.',
'QuantiseTable':'Ordered signed sequence; preserve order, duplicates, and pairing rather than sorting to pitch classes.',
'FollowSpeedDirection':'The compiled helper is numerically probed. Mode 2 reverses control orientation, not all playback signs.',
'EffectOrder':'Parallel/series are exact config labels. Full gain-normalized transition behavior remains unresolved.',
'ReverbReset':'The patch supports duck/restore. Tank-memory clearing is not established.',
'EnableClockedModeSwitch':'Zero in initialization fallback. Present experimental/optional feature, not an assumed stock default.',
'ClockedMode':'Zero in initialization fallback; disabled baseline.',
'LoadConfiguration':'Startup fallback is 3; named factories are 1; HTML selected option is not actual startup precedence.',
'WavetableCentreFrequency':'Hz; separate 0.976642 factor is applied downstream in the WT patch.',
'OnsetMode':'Six named profiles. Full per-profile transition/trigger behavior still needs a truth table.'}
config=[]
for key in keys:
 e={'key':key,'evidence':'F: preset/configuration text; enum labels from included editor when present',
    'initialization_fallback':init['values'].get(key),
    'factory_values':{k:factory[k]['values'].get(key) for k in ['alpha','beta','gamma','delta','epsilon','zeta']},
    'enum':enums.get(key), 'note':notes.get(key,'Consult original file comments; absent values are null, not inferred zero.')}
 config.append(e)
controls=[
 ('scan','Scan / Follow traversal',0,100,'S','Scan approximately 480000/4095 samples per calibrated 12-bit unit; Follow uses a separate speed helper.','Omega endpoint and capture-head boundary branches unresolved.'),
 ('length','Grain duration / wavetable entry',1,101,'S','u² * 100000 units, then *1.44 samples clamped 128..144000 in nominal-48k domain.','Mode-dependent entry thresholds and exact physical minimum require validation.'),
 ('pitch_v_oct','Pitch CV',2,None,'F/S/D','1 V/oct is the documented musical contract; raw hardware and WT paths require calibration.','Do not use raw ADC scaling as a Rack volts formula.'),
 ('pitch','Pitch knob',3,None,'F/S','Named pitch functions and exact PITCH_VALUES table recovered.','Full normal-mode center detent, quantisation, and MIDI merge not completely reconstructed.'),
 ('layer','Layer / Omega',5,30,'F/S','Six layers plus Omega; play and record layer may be separate.','Notes distinguish layer CV channel 4; switching/hysteresis and coupling need complete truth table.'),
 ('dry_wet','Dry / Wet',6,6,'S','Steady-state equal-power cosine/sine curve; raw control is inverted in the main patch.','Pd table approximation and smoothing not reproduced by ideal C++ sin/cos.'),
 ('hold','Hold',8,38,'F/S','Onset hold state; recorder converts shared milliseconds with factor 48.','Also affects Follow loop-length policy when configured; full panel law unresolved.'),
 ('spray','Spray',9,103,'S','u² * 100000 units, then *4.8 samples; Scan and Follow use different placement branches.','Not a proven uniform bipolar offset; MIDI slot 212 participates upstream.'),
 ('direction','Grain direction probability',10,None,'F/S','Per-grain direction and _dirPercent are present; reverse branch distinct.','Complete probability transfer and draw order unresolved.'),
 ('dub','Dub / recording blend',11,None,'F/S','Recorder has handover state and coefficient slew near 0.0078/sample.','Complete record equation and all capture fades unresolved.'),
 ('intensity','Continuous grain production',12,None,'F/S/D','Separate continuous and struck engines; timing and amplitude randomization are independent options.','Exact density law and duration coupling are high-priority open work.'),
 ('mod','Configurable modulation',13,None,'F/S','Enum selects none/pan/hold/reverb/delay; dedicated defaults also exist.','Do not make effects mutually exclusive in the 2.1-style feature set.'),
 ('texture','Grain window / texture',14,None,'F/S','101 x 515 reconstructed bank from exact templates.','Complete control-to-row and reverse phase stepping not proven.'),
 ('deviation','Pitch deviation',15,None,'F/S','Ordered quantisation sequences and setDeviation routine recovered.','Probability selection, continuous vs quantised behavior, and timing still incomplete.'),
 ('capture','Capture button/CV',None,None,'F/S','CV modes latch/momentary/retrigger; separate button policy.','Simultaneous events, accumulative behavior, and record destination locking need tests.'),
 ('strike','Strike button/CV',None,None,'F/S','Independent grain event source with configurable CV delay.','Pitch/control latch timing and scheduler order remain explicit fidelity work.'),
 ('shift','Shift / gesture layer',None,None,'F/S/D','Multiple mode/gesture handlers and staged parameter behavior.','Full hold/double-press timing and precedence not recovered.'),
 ('sense','Onset sensitivity / alternate input level',None,None,'F/D','Normal onset path is bonk~ after four hip~300 stages.','Analog control and stereo-mode level behavior not fully reconstructed.'),
 ('input_gain','Input level',None,None,'D/S','Separate input conditioning and analog preamp/limiter target.','No authentic analog transfer measurement supplied.'),
 ('output_gain','Output level',None,None,'D/S','Main output/phase/mute path separate from rational clipping.','Absolute hardware dB/V calibration not established.')]
cc=[dict(zip(['id','label','notes_adc_channel','shared_slot','evidence','recovered','open'],x)) for x in controls]
ports=[
 {'id':'audio_in','direction':'input','kind':'audio','role':'Capture input; left channel in stereo input mode','source':'D3 pp8/10/37'},
 {'id':'onset_right_in','direction':'input','kind':'audio','role':'Analysis input in mono mode; right channel in stereo input mode','source':'D3 pp8/10/37'},
 {'id':'out_1','direction':'output','kind':'audio','role':'First audio output; preserve cable-dependent mono policy as a separate compatibility setting','source':'D3 pp8/11'},
 {'id':'out_2','direction':'output','kind':'audio','role':'Second audio output; phase policy is configurable','source':'D3 pp8/11'},
 {'id':'pulse_out','direction':'output','kind':'trigger','role':'Event-derived pulse; source policy depends on mode/configuration','source':'D3 p8; GPIO symbols'},
]
for i in ['capture','strike']:
 ports.append({'id':i+'_in','direction':'input','kind':'gate/trigger','role':i+' event input','source':'D3 p8; GPIO handlers'})
for i in ['intensity','length','scan','pitch_v_oct']:
 ports.append({'id':i+'_cv','direction':'input','kind':'CV','role':i,'source':'D3 p8; control notes/binary'})
for i in ['spray','layer','direction','texture','deviation','dub','mod','dry_wet']:
 ports.append({'id':i+'_cv','direction':'input','kind':'CV','role':i,'location':'hardware CV expansion; native port may integrate','source':'D3 p9; included control notes'})
schema={'schema_version':1,'status':'reverse-engineering handoff; not a complete original DSP specification',
 'archive_sha256':'1414f35d8d0e3ca8d4cb0871a8c61f3957001caf3b106d30df0514ca9f404637',
 'rate_policy':{'requested_audio_hz':48000,'file_conversion_hz':49148,'actual_codec_hz':None,'wt_frequency_factor':0.976642},
 'storage':{'layers':6,'channels_per_layer':2,'frames_per_channel':624000,'nominal_scan_frames':480000,'storage_slots_per_player':82,'proven_max_simultaneously_audible_per_player':None},
 'hardware_controls':cc,'ports':ports,'configuration':config,
 'rack_recommendations':{'nominal_audio_volts_per_unit':5,'trigger_low_V':0.1,'trigger_high_V':1.0,'pulse_output_V':10,'pulse_duration_s':0.001,'input_polyphony':'single instrument; summed audio / first-channel CV policy must be explicit','microphone':'No automatic host microphone; optional user-patched source or explicit file import','clocked_mode_default':False}}
save('tables/parameter_bible.json',schema)
rows=['# Parameter and I/O bible','',
'**Status:** a mixed-evidence implementation map, not a claim that every transfer function is recovered. `F`, `S`, `D`, and `R` have the meanings defined in the main report. JSON companion: `tables/parameter_bible.json`.','',
'## 1. Inputs, outputs, and host adaptation','',
'Hardware has two audio input roles: **IN** and **ONSET**. In stereo mode these become left/right; in mono mode ONSET is the analysis input. The documented hardware normalization is microphone → ONSET → IN. A native plugin should not silently open a computer microphone. Provide an explicit optional source or leave unpatched input silent, and label that adaptation. [D3 pp10/37; R]','',
'| Port/group | Contract | Evidence |','|---|---|---|',
'| IN; ONSET / RIGHT | Two audio roles with mono/stereo reinterpretation | D3; `arbharRecorder.pd`, input externals |',
'| OUT 1; OUT 2 | Stereo audio; phase and cable-dependent summing need explicit policy | D3; `main_out_stereo~` |',
'| CAPTURE; STRIKE | Distinct gate/trigger event inputs | GPIO handlers and exact configuration enums |',
'| Pulse OUT | Mode-derived trigger output, not audio | `_setTriggerOut`; selected trigger-source config |',
'| Main CV | Intensity, Length, Scan, 1 V/oct | D3 panel; control path |',
'| Expansion CV | Spray, Layer, Direction, Texture, Deviation, Dub, Mod, Dry/Wet | D3 expansion panel; included control notes |','',
'Integrating all expansion controls into the Rack panel is a recommendation, not a claim about the original physical layout. Use ±5 V as a proposed nominal audio scaling, Schmitt triggering at 0.1/1 V, and 10 V/1 ms pulse output for a Rack-oriented interface. These choices follow the host conventions and are **not measured arbhar circuit thresholds**. [R; D5]','',
'## 2. Controls and recovered laws','',
'“Notes ADC channel” below is an archive analysis aid. It is **not** a complete hardware schematic or proof of jack-to-voltage scaling. Null/— means not established in this handoff.','',
'| Control | Notes ADC channel | Shared slot | Evidence | Recovered behavior and boundary |','|---|---:|---:|---|---|']
for c in cc:
 rows.append('| '+c['label']+' | '+str(c['notes_adc_channel'] if c['notes_adc_channel'] is not None else '—')+' | '+str(c['shared_slot'] if c['shared_slot'] is not None else '—')+' | '+c['evidence']+' | '+c['recovered']+' **Open:** '+c['open']+' |')
rows+=['','## 3. Exact configuration keys and defaults','',
'Values below come from parsed files, not from assumed GUI defaults. `—` means the key was absent in that source; do not convert it to zero. Columns are the initialization fallback, then alpha/beta/gamma/delta/epsilon/zeta. Array-valued fields are preserved in full in JSON and in the original files.','',
'| Key | Init | α | β | γ | δ | ε | ζ |','|---|---|---|---|---|---|---|']
def fmt(v):
 if v is None:return '—'
 if isinstance(v,list):return '['+', '.join(map(str,v))+']'
 return str(v)
for c in config:
 vals=[c['initialization_fallback']]+[c['factory_values'][k] for k in ['alpha','beta','gamma','delta','epsilon','zeta']]
 if any(isinstance(v,list) for v in vals):
  vals=[('array ('+str(len(v))+')') if isinstance(v,list) else v for v in vals]
 rows.append('| `'+c['key']+'` | '+' | '.join(fmt(v) for v in vals)+' |')
rows+=['','## 4. Enum semantics and specific cautions','']
for c in config:
 if c['enum']:
  rows+=['**`'+c['key']+'`** — '+'; '.join(str(k)+' = '+str(v) for k,v in c['enum'].items())+'.','']
for key,n in notes.items():rows+=['**`'+key+'`** — '+n,'']
rows+=['## 5. Shared-memory tracing','',
'The 250-value block at key 61019 mixes raw/processed controls, UI state, MIDI state, and engine communication. `tables/shmem_patch_references.json` contains 150 literal patch references, and the disassembly shows additional direct accesses. This is not a complete typed layout. Of particular importance, compiled Length writes slot 101 and Spray writes slot 103; an older note about 103 is not authoritative.','',
'Capture-related flags 105/106, onset marker 109, and record-position communication require ordering analysis before a native translation. Quantisation and MIDI ranges must not be treated as independent fixed-size application structs merely because an index is visible. The native engine should replace this mixed array with typed objects rather than recreating ad hoc magic indices. [S/R]','',
'## 6. Sources','',
'File evidence: `tables/presets.json`, `tables/preset_matrix.csv`, `tables/preset_editor_fields.json`, `extracted/Pin_Adc_Control_Info/`, compiled GPIO/player/recorder functions, and original Pd graphs. The main report defines D3 (official 2.0 manual, selected panel/I/O pages inspected) and D5 (official Rack voltage conventions).','']
(R/'PARAMETER_BIBLE.md').write_text('\n'.join(rows))
issues=[
 ('OPEN-01','P0','Grain allocation/retirement','82 storage slots vs 42-threshold logic and documented 44 sounding/engine','play_next 0x6c34; perform 0x4248','Trace every launch, kill, active count, MIDI and override branch; build oversubscription fixtures.'),
 ('OPEN-02','P0','Internal grain scheduler','Exact Intensity law, duration coupling, asynchronous jitter and minimum spacing','tickInternTrig 0x7928; GPIO timing functions; main/core patches','Trace clocks and snapshot time; test fixed duration/density grids with fixed random seed.'),
 ('OPEN-03','P0','Capture/Dub/boundaries','Full write equation, two-writer handover, 256-sample fades, end behavior','rec perform 0x4288; checkCaptureButton 0x4a14','Use ramps/impulses and event timeline fixtures; never assume perpetual ring recording.'),
 ('OPEN-04','P0','Delay macro','Creation multiplier differs from dynamic multiplier and descriptive ranges','arbhar_feedback.pd and upstream macro/MIDI wiring','Evaluate Pd message order; map actual incoming domain and render impulse traces.'),
 ('OPEN-05','P1','Pitch/quantisation','Detent fix, deviation probabilities, ordered sequence selection and V/oct latch','setDeviation 0x404c; _getChord 0x2f40; MIDI branches','Trace center departures and pair selection, preserve repeated values.'),
 ('OPEN-06','P1','Texture and reverse grains','Row indexing, window advance, boundary reads and reverse tails','makingWndArray 0x5b34; perform 0x4248','Probe signed-speed/texture endpoints and all buffer edges.'),
 ('OPEN-07','P1','Stereo output / gain / panning','Exact mixing, mono normalization, phase, activity gain and MIDI scaling','main graph; main_out_stereo~; play perform','Impulse both channels; compare cable states and preset phase policies.'),
 ('OPEN-08','P1','Onset profiles','Installed bonk~ implementation and six profile event truth tables','arbharRecorder.pd; _processOnsetData 0x4044; GPIO _setOnsetMode','Identify exact dependency, then test hits, gaps, hold, manual capture, and stereo transitions.'),
 ('OPEN-09','P1','Follow and Omega boundaries','Backward-looking spray, endpoint layer 6 guard, record-head exclusion','_getPositionRange 0x6868; _setPlayParameters 0x7c50','Trace guards including 0x6a20; test all six layer boundaries and offsets.'),
 ('OPEN-10','P1','Timebases/file length','48k request vs 49148 raw conversion vs WT factor','launch, conversion scripts, WT patch','Measure hardware or verified original process; test 624000 vs 638924 frame files.'),
 ('OPEN-11','P2','Auxiliary reverb','Nonzero activation not established','main object 336 and gains 337/338','Find normal initialization/control drivers before enabling.'),
 ('OPEN-12','P2','Clocked mode','Disabled fallback but beta source/editor present','configurationDataInitFile and clock scripts','Separate optional-feature compatibility from stock baseline.'),
 ('OPEN-13','P1','Analog input/output','No authentic gain/limiter/codec transfer measurement','stereo_in~; ALSA state; physical hardware','Measure physical transfer separately; avoid labelling a generic soft clip authentic.'),
 ('OPEN-14','P2','Source/build provenance','Source snapshot missing headers; DWARF is hardware-library data','arbhar_gpio~.c_changes?; Makefile; dwarf metadata','Do not use source sketches as authoritative compiled DSP definitions.')]
with (R/'tables/open_questions.csv').open('w',newline='') as f:
 w=csv.writer(f);w.writerow(['id','priority','area','question','entry_point','next_action']);w.writerows(issues)
save('tables/open_questions.json',[dict(zip(['id','priority','area','question','entry_point','next_action'],i)) for i in issues])
print('Wrote parameter bible:',len(config),'configuration keys,',len(cc),'controls,',len(ports),'ports;',len(issues),'open issues')
