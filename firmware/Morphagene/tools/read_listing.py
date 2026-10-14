"""Print already decoded instructions in an address range, without LLVM."""
import json,sys
from pathlib import Path
from llvm_thumb import ThumbDisassembler
root=Path(__file__).resolve().parents[1]
lo,hi=(int(v,16) for v in sys.argv[1:3])
if lo<0x08000000: lo+=0x08000000
if hi<0x08000000: hi+=0x08000000
for d in json.loads((root/'analysis/reachable_instructions.json').read_text()):
    if lo<=d['address']<hi: print(ThumbDisassembler.format(None,d))
