from pathlib import Path
from llvm_thumb import ThumbDisassembler
from llvm_thumb import json_finite
import struct,json,collections
p=Path(__file__).resolve().parents[1];b=(p/'binaries/mimeophon_mp86.bin').read_bytes();d=ThumbDisassembler(b)
ins=list(d.range(0x080201c8,0x08028c7c))
(p/'disassembly/linear_sweep_thumb.asm').write_text('; LINEAR SWEEP: includes misinterpreted literal data; not a verified code-only listing.\n'+'\n'.join(d.format(i) for i in ins)+'\n')
(p/'analysis/linear_instructions.json').write_text(json.dumps(json_finite(ins),indent=2,allow_nan=False))
calls=collections.Counter(i['target'] for i in ins if i['op']=='bl' and 'target'in i)
print('TOP CALL TARGETS:',[(hex(k),v) for k,v in calls.most_common(40)])
for i in range(0,448,4):print(hex(i),hex(struct.unpack_from('<I',b,i)[0]))
print('RESET TARGET',hex(struct.unpack_from('<I',b,0x38c)[0]))
