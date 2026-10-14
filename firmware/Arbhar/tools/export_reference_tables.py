#!/usr/bin/env python3
"""Export exact uploaded float tables to a C++17 header. Tables are vendor-derived evidence."""
import json,pathlib
R=pathlib.Path(__file__).resolve().parents[1];t=json.loads((R/'tables/extracted_float_tables.json').read_text())
lines=['#pragma once','#include <array>','// Exact table bytes decoded from uploaded arbhar_play~.pd_linux. See provenance in JSON.','// This file is research data, not a grant of redistribution rights.','namespace arbhar_re {']
for orig,n in [('PITCH_VALUES','kPitchValues'),('VOL_REDUCTION','kVolumeReduction'),('SAW','kSaw'),('GAUSS','kGaussian'),('SQUARE','kSquare')]:
 d=t['arbhar_play~.pd_linux:'+orig];v=d['values'];lines+=['// '+orig+' VA '+d['virtual_address']+' sha256 '+d['sha256'],f'inline constexpr std::array<float,{len(v)}> {n} = {{']
 for i in range(0,len(v),5):lines.append('  '+', '.join(float(x).hex()+'f' for x in v[i:i+5])+',')
 lines+=['};']
lines+=['} // namespace arbhar_re'];(R/'reference/recovered_tables.hpp').write_text('\n'.join(lines)+'\n')
