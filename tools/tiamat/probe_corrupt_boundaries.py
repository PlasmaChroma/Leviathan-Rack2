#!/usr/bin/env python3
"""Optional original-instruction Dropout boundary regression, Unicorn 2.1.4.

Only rand() is substituted. No DSP/math hooks, no board startup. Routine tests
use the two captured decisions directly and do not need firmware or Unicorn.
"""
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'firmware/Data_Bender/analysis/corrupt_dsp'))
from emulate_corrupt import run, FW

cases = []
for word, draw, expected in [(1028865070, 1, True), (1038887682, 5, False)]:
    amount = struct.unpack('<f', struct.pack('<I', word))[0]
    case = run(amount=amount, gate=True, randoms=(draw,))
    assert 'error' not in case and case['final_gate'] == expected
    assert case['calls'] == [['rand', draw]]
    cases.append(case)
result = {'firmware_sha256': hashlib.sha256(FW.read_bytes()).hexdigest(),
          'method': __doc__, 'cases': cases}
dest = ROOT / 'build/tiamat-dropout-boundaries.json'
dest.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print('Both original Dropout boundary decisions match; evidence:', dest)
