#!/usr/bin/env python3
"""Author the initial illustration definition. Coordinates/weights are design choices."""
from pathlib import Path
import json
ROOT = Path(__file__).resolve().parents[1]
# Lobe format: center-x, center-y, sigma-x, sigma-y, angle-degrees, strength.
# Coordinates are fractions of the 788 x 1002 source canvas. They do not encode
# anatomical precision, measured coupling, dose, efficacy, or biological bandwidth.
regions = {
 'chest': [.500,.448,.110,.074,0,1.0],
 'abdomen': [.500,.640,.100,.110,0,1.0],
 'core': [.500,.580,.080,.165,0,1.0],
 'spine': [.500,.530,.022,.200,0,1.0],
 'throat': [.500,.313,.034,.047,0,1.0],
 'head': [.500,.178,.068,.073,0,1.0],
 'upper_head': [.500,.125,.072,.050,0,1.0],
 'head_base': [.500,.247,.042,.040,0,1.0],
 'central_head': [.500,.194,.031,.032,0,1.0],
 'left_eye': [.458,.171,.015,.012,0,1.0],
 'right_eye': [.542,.171,.015,.012,0,1.0],
 'left_ear': [.396,.195,.021,.028,0,1.0],
 'right_ear': [.604,.195,.021,.028,0,1.0],
 'left_upper_arm': [.269,.506,.031,.101,5,1.0],
 'right_upper_arm': [.731,.506,.031,.101,-5,1.0],
 'left_forearm': [.298,.728,.033,.063,-32,1.0],
 'right_forearm': [.702,.728,.033,.063,32,1.0],
 'left_thigh': [.256,.842,.177,.040,18,1.0],
 'right_thigh': [.744,.842,.177,.040,-18,1.0],
 'lower_legs': [.515,.946,.202,.024,5,1.0],
 'pelvis': [.500,.787,.077,.040,0,1.0],
 'left_flank': [.432,.608,.031,.055,0,1.0],
 'right_flank': [.568,.608,.031,.055,0,1.0],
}
def lobes(*items):
    result=[]
    for key,weight in items:
        a=regions[key].copy(); a[5]*=weight
        result.append({'region':key,'gaussian':a})
    return result
bands=[
 {'id':'low_core','low_hz':20,'high_hz':30,'label':'20–30 Hz · chest, abdomen, eyes / ears',
  'source_label':'<30 Hz (infrasound/vibration)', 'source_lines':[11,11], 'color':'#E7AEC5',
  'lobes':lobes(('chest',.82),('abdomen',.85),('left_eye',.40),('right_eye',.40),('left_ear',.23),('right_ear',.23))},
 {'id':'musculoskeletal','low_hz':30,'high_hz':50,'label':'30–50 Hz · spine, core, legs',
  'source_label':'Musculoskeletal system (spine, legs, core)', 'source_lines':[13,13], 'color':'#EDC6A1',
  'lobes':lobes(('spine',.80),('core',.35),('left_thigh',.82),('right_thigh',.82),('lower_legs',.52))},
 {'id':'large_muscles','low_hz':50,'high_hz':100,'label':'50–100 Hz · larger muscle groups',
  'source_label':'Larger muscle groups and some neural rhythms', 'source_lines':[15,15], 'color':'#E9DDA4',
  'lobes':lobes(('left_upper_arm',.70),('right_upper_arm',.70),('left_forearm',.42),('right_forearm',.42),('left_thigh',.77),('right_thigh',.77),('core',.30))},
 {'id':'torso_throat','low_hz':100,'high_hz':300,'label':'100–300 Hz · throat, chest, abdomen / flanks',
  'source_label':'Organs & glands', 'source_lines':[17,17], 'color':'#A4DDC4',
  'lobes':lobes(('throat',.92),('chest',.48),('abdomen',.79),('left_flank',.30),('right_flank',.30))},
 {'id':'head_neck','low_hz':300,'high_hz':600,'label':'300–600 Hz · head and upper neck',
  'source_label':'Brainstem and nervous pathways', 'source_lines':[19,19], 'color':'#B1C0ED',
  'lobes':lobes(('head',.38),('head_base',.86),('throat',.34))},
 {'id':'upper_head','low_hz':600,'high_hz':1000,'label':'600–1000 Hz · upper / central head',
  'source_label':'Central nervous system and pineal', 'source_lines':[21,21], 'color':'#D5B6E9',
  'lobes':lobes(('upper_head',.80),('central_head',.58),('head',.23))},
 {'id':'auditory','low_hz':1000,'high_hz':2000,'label':'1000–2000 Hz · ears and head',
  'source_label':'Auditory & emotional centers', 'source_lines':[23,23], 'color':'#A4DCEB',
  'lobes':lobes(('left_ear',1.0),('right_ear',1.0),('head',.40))},
]
tones=[
 ('root',396,'Root', '#EDA8BA',.500,.804,.031,.025,7),
 ('sacral',417,'Sacral','#EFC0A1',.500,.704,.036,.030,7),
 ('solar_plexus',528,'Solar plexus','#EDDEA4',.500,.562,.038,.032,7),
 ('heart',639,'Heart','#A8E0C1',.500,.446,.038,.032,7),
 ('throat',741,'Throat','#A7DCEB',.500,.313,.031,.028,7),
 ('third_eye',852,'Third eye','#B4BEED',.500,.164,.028,.024,7),
 ('upper_head_symbol',963,'Upper head / unity motif','#D7BAED',.500,.097,.033,.026,21),
]
tone_data=[]
for id,hz,label,color,cx,cy,sx,sy,line in tones:
    tone_data.append(dict(id=id,frequency_hz=hz,label=label,color=color,source_lines=[line,line],
      placement_note=('Artistic upper-head placement for the report\'s pineal/unity motif; the report does not explicitly call 963 Hz the crown chakra.' if hz==963 else
                      'Chakra name and frequency are report-derived; the screen coordinate and extent are artistic.'),
      lobes=[dict(region=id,gaussian=[cx,cy,sx,sy,0,1.0])]))
data=dict(schema=1, title='V.Tune · Sound+Body illustrative mapping',
 source='reference/Sound+Body.md',
 semantics='Illustrates associations in the supplied report. Not a measured, validated, or clinical frequency-to-organ response model.',
 source_width=788,source_height=1002,mask_width=394,mask_height=501,
 report_range_hz=[20,2000],boundary_half_width_octaves=.08,tone_support_cents=70,ui_smoothing_seconds=.09,
 opacity_default=.60,combined_report_gain=.55,combined_symbolic_gain=.90,
 holes_seed_xy=[[260,650],[525,650]],
 bands=bands,tones=tone_data,
 omissions=[
 'The 20 Hz floor clips the report\'s <30 Hz band; no sub-20 Hz range is invented.',
 'No Aparmita mode: the report supplies only an incomplete selection of its chakra bands.',
 'No 174 Hz organ-specific hotspot: the report gives a pain-relief association, not a unique anatomical location.',
 'The report\'s 396/417 Hz pelvic anecdotes are represented in the symbolic mode, not spread over the whole 300–600 Hz band.',
 'For 528 Hz the explicit list in paragraph 3 (solar plexus) is used; the conclusion also says heart/solar plexus, so the source is not internally unambiguous.',
 'A symbolic 963 Hz upper-head motif is included; a crown-chakra label is not attributed to the source.',
 ],
 design_choices=['All colors, Gaussian locations, spatial extents, relative strengths, crossfade widths, tone tolerances, and smoothing times are authored visual choices, not results extracted from the report.',
 'Coarse zones are used instead of anatomically precise organ silhouettes because the report supplies no spatial registration or quantitative organ response data.'])
(ROOT/'res/VTune/body-map/definition.json').write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n')
