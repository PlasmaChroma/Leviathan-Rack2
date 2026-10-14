# Ghidra legacy Jython script. Analyst annotation aid, NOT an emulator.
#@category MP86
# Import the BIN at 0x08020000 as little-endian ARM/Thumb before running.
# Select analysis/curated_symbols.json when prompted.
# This integration script was syntax-reviewed but not run in Ghidra here.
import json
from java.math import BigInteger
from ghidra.program.model.symbol import SourceType

source = askFile('Select MP86 analysis/curated_symbols.json', 'Open')
with open(source.getAbsolutePath(), 'r') as handle:
    labels = json.load(handle)
mem = currentProgram.getMemory()
base = toAddr(0x08020000)
if mem.getBlock(base) is None:
    raise RuntimeError('Import raw firmware at 0x08020000, or import the analysis ELF, first.')
ctx = currentProgram.getProgramContext()
tmode = currentProgram.getRegister('TMode')
if tmode is not None:
    ctx.setValue(tmode, toAddr(0x080201c8), toAddr(0x08028c73), BigInteger.ONE)
count = 0
for item in labels:
    addr = toAddr(long(item['address'], 16))
    if mem.getBlock(addr) is None:
        # Do not invent initialized RAM, recorded delay contents or device settings.
        continue
    createLabel(addr, item['name'], True, SourceType.USER_DEFINED)
    setPlateComment(addr, 'MP86 analyst label: ' + item['note'])
    if item['kind'] == 'function':
        disassemble(addr)
        if getFunctionAt(addr) is None:
            createFunction(addr, item['name'])
    count += 1
print('Added %d MP86 analyst labels. Review function bounds and indirect branches manually.' % count)
