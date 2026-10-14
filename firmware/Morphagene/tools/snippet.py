"""Local linear sweep; use a known instruction boundary. Pools may decode as code."""
import sys
from pathlib import Path
from llvm_thumb import ThumbDisassembler
root = Path(__file__).resolve().parents[1]
d = ThumbDisassembler((root/'Morphagene_MG204_RE_bundle/mg204_flash_08020000.bin').read_bytes())
start, end = (int(a, 16) for a in sys.argv[1:3])
if start < d.base: start += d.base
if end < d.base: end += d.base
for i in d.range(start, end): print(d.format(i))
