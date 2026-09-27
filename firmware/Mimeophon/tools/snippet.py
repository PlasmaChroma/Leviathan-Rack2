import sys
from pathlib import Path
from llvm_thumb import ThumbDisassembler
b=(Path(__file__).resolve().parents[1]/'binaries/mimeophon_mp86.bin').read_bytes();d=ThumbDisassembler(b)
a=int(sys.argv[1],16);e=int(sys.argv[2],16)
if a<0x08020000:a+=0x08020000
if e<0x08020000:e+=0x08020000
for i in d.range(a,e):print(d.format(i))
