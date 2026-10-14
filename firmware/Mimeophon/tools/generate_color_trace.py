"""Generate test-only C++ execution of bounded DSP and event instruction slices.

Checks every used instruction and literal against MP86 before emitting code.
This trusts the supplied decoder and uses host IEEE float/FMA; it is NOT an ARM
emulator. Memory addressing, conditional branches, and state writes within the
selected slices are executed, rather than replacing them with a Color equation.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re

ROOT = Path(__file__).resolve().parents[1]
SHA = '31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9'
BASE = 0x08000000
SLICES = {
    'left': (0x246fe, 0x248e0, [(0x246fe,0x248e0),(0x253b4,0x253bc),
                              (0x253dc,0x253e8),(0x25384,0x25390)]),
    'right': (0x2494a,0x24b58,[(0x2494a,0x249a6),(0x24a04,0x24b58),
                              (0x25364,0x25384)]),
    'halo_tail': (0x24f10,0x25158,[(0x24f10,0x25158),(0x2535e,0x25364)]),
    'mod_init': (0x2385c,0x2388c,[(0x2385c,0x2388c)]),
    'mod_random_a': (0x25b5e,0x23f9e,[(0x25b5e,0x25baa),(0x25df6,0x25e0e),(0x2686a,0x26872)]),
    'mod_random_b': (0x25acc,0x23fa6,[(0x25acc,0x25b18),(0x25dde,0x25df6),(0x26862,0x2686a)]),
    'mod_positions': (0x24e2c,0x24f10,[(0x24e2c,0x24e9c),(0x24efc,0x24f10)]),
    'mod_counters': (0x23f64,0x23f8e,[(0x23f64,0x23f8e),(0x253bc,0x253c2)]),
    'expiry_events': (0x23f64,0x23fa6,[(0x23f64,0x23fa6),(0x253bc,0x253c2),
        (0x25a9e,0x25baa),(0x25dda,0x25e0e),(0x25fca,0x25fe0),
        (0x263e0,0x263f8),(0x263fc,0x26416),(0x266e2,0x26706),
        (0x267ae,0x267b6),(0x26862,0x26872)]),
    'consume_requests': (0x24288,0x24298,[(0x24288,0x24298),
        (0x2599e,0x259ea),(0x25eda,0x25ee4),(0x25eec,0x25ef8)]),
    'reverse_windows': (0x25c80,0x243bc,[(0x25c80,0x25d44),
        (0x25e1e,0x25e3c),(0x25ea8,0x25ebc)]),
    'hold_transport': (0x241a4,0x241c8,[(0x241a4,0x241c8),(0x253fa,0x2543e),(0x25d84,0x25d94)]),
    'hold_retarget': (0x259ea,0x24288,[(0x259ea,0x25a9e),(0x25e74,0x25ea8),
        (0x25ebc,0x25eda),(0x25ee4,0x25eec),(0x25f18,0x25f1c)]),
    'hold_input': (0x27004,0x27042,[(0x27004,0x27042),(0x2712a,0x27130),(0x27156,0x27162)]),
    'clock_prefix': (0x23af8,(0x23b20,0x25fb6,0x264e8),
        [(0x23af8,0x23b20),(0x25f9a,0x25fb6)]),
}

def reg(name):
    if name == 'sp': return 'm.r[13]'
    if name == 'lr': return 'm.r[14]'
    if re.fullmatch(r'[sr]\d+',name): return f'm.{name[0]}[{name[1:]}]'
    raise ValueError(f'Unknown register {name}')

def operand(value):
    return value[1:] if value.startswith('#') else reg(value)

def memory(args):
    match = re.fullmatch(r'\[(r\d+|sp|lr)(?:, #(-?\d+))?\]', args)
    if not match: raise ValueError(f'Unsupported address {args}')
    return f'({reg(match[1])} + ({match[2] or 0}))'

def translate(ins, firmware):
    op, args = ins['op'], ins['args']
    if op in ('it','ite'): return '// Predication is encoded in each following mnemonic'
    predicates = {'moveq':('mov','==0'),'movne':('mov','!=0'),
                  'movle':('mov','<=0'),'subgt.w':('sub','>0'),'subwgt':('sub','>0')}
    if op in predicates:
        bare,condition = predicates[op]
        return f'if (m.comparison{condition}) {{ '+translate({**ins,'op':bare},firmware)+' }'
    fields = [x.strip() for x in args.split(',')]
    destination = reg(fields[0]) if fields[0].startswith(('s','r')) or fields[0]=='lr' else None
    if op in ('ldr','ldr.w','str','str.w','vldr','vstr'):
        first, address = args.split(', ',1)
        if 'literal_address' in ins:
            offset = ins['literal_address']-0x08020000
            value = int.from_bytes(firmware[offset:offset+4],'little')
            if value != ins['literal_u32']: raise ValueError('Literal mismatch')
            expression = f'0x{value:08x}u'
            if op == 'vldr': expression = f'from_bits({expression})'
            return f'{reg(first)} = {expression};'
        pointer = memory(address)
        if op == 'vstr': return f'm.storef({pointer},{reg(first)});'
        if op in ('str','str.w'): return f'm.memory[{pointer}] = {reg(first)};'
        return f'{reg(first)} = m.{"loadf" if op=="vldr" else "load"}({pointer});'
    if op in ('vadd.f32','vsub.f32','vmul.f32','vfma.f32','vfms.f32'):
        x,y = reg(fields[1]),reg(fields[2])
        if op in ('vfma.f32','vfms.f32'):
            return f'{destination} = std::fma({"-" if op=="vfms.f32" else ""}{x},{y},{destination});'
        symbol = {'vadd.f32':'+','vsub.f32':'-','vmul.f32':'*'}[op]
        return f'{destination} = {x}{symbol}{y};'
    if op in ('vminnm.f32','vmaxnm.f32'):
        return f'{destination} = std::{"min" if op=="vminnm.f32" else "max"}({reg(fields[1])},{reg(fields[2])});'
    if op in ('vabs.f32','vneg.f32'):
        return f'{destination} = '+(f'std::abs({reg(fields[1])});' if op=='vabs.f32' else f'-{reg(fields[1])};')
    if op in ('vcmp.f32','vcmpe.f32'):
        return f'm.compare({reg(fields[0])},{operand(fields[1])});'
    if op=='vmrs': return 'm.comparison = m.float_comparison;'
    if op=='vmov.f32': return f'{destination} = {operand(fields[1])};'
    if op=='vmov':
        if fields[0].startswith('r'): return f'{destination} = to_bits({reg(fields[1])});'
        return f'{destination} = from_bits({reg(fields[1])});'
    if op=='vcvt.s32.f32':
        return f'{destination} = from_bits(static_cast<std::uint32_t>(static_cast<std::int32_t>({reg(fields[1])})));'
    if op=='vcvt.f32.s32':
        return f'{destination} = float(static_cast<std::int32_t>(to_bits({reg(fields[1])})));'
    if op=='vcvt.f32.u32':
        return f'{destination} = float(to_bits({reg(fields[1])}));'
    if op in ('add','add.w','adds','addw'):
        if len(fields)==4 and fields[3]=='lsl #2':
            return f'{destination} = {reg(fields[1])} + ({reg(fields[2])} << 2);'
        if len(fields)==3: return f'{destination} = {reg(fields[1])} + {operand(fields[2])};'
        if len(fields)==2: return f'{destination} += {operand(fields[1])};'
    if op in ('mov','movw','mov.w'): return f'{destination} = {operand(fields[1])};'
    if op=='movs':
        return f'{destination} = {operand(fields[1])}; m.compare_integer({destination},0);'
    if op=='mul': return f'{destination} = {reg(fields[1])} * {reg(fields[2])};'
    if op=='mla': return f'{destination} = {reg(fields[1])} * {reg(fields[2])} + {reg(fields[3])};'
    if op=='mvn': return f'{destination} = ~std::uint32_t({operand(fields[1])});'
    if op=='sub': return f'{destination} = {reg(fields[1])} - {operand(fields[2])};'
    if op=='subs':
        # Only used by the bounded decrement slice: overwritten by VMRS/CMP
        # before any branch. This represents N/Z, not general ARM C/V flags.
        left,right=(destination,operand(fields[1])) if len(fields)==2 else (reg(fields[1]),operand(fields[2]))
        return f'{destination} = {left} - {right}; m.compare_integer({destination},0);'
    if op=='and.w': return f'{destination} = {reg(fields[1])} & {reg(fields[2])};'
    if op=='ands':
        left,right=(destination,operand(fields[1])) if len(fields)==2 else (reg(fields[1]),operand(fields[2]))
        return f'{destination} = {left} & {right}; m.compare_integer({destination},0);'
    if op=='tst.w': return f'm.compare_integer({reg(fields[0])} & {operand(fields[1])},0);'
    if op=='lsls': return f'{destination} = {reg(fields[1])} << {operand(fields[2])}; m.compare_integer({destination},0);'
    if op in ('cmp','cmp.w'):
        return f'm.compare_integer({reg(fields[0])},{operand(fields[1])});'
    if op in ('cbz','cbnz'):
        return f'if ({reg(fields[0])}{"==" if op=="cbz" else "!="}0) pc = 0x{ins["target"]:08x}u;'
    if op in ('b','b.w','bge.w','ble.w','ble','bpl.w','bpl','bgt','bgt.w','beq.w','beq','bne.w','bne','blt.w','bmi.w','bmi'):
        target = f'0x{ins["target"]:08x}u'
        if op in ('b','b.w'): return f'pc = {target};'
        condition = {'ble.w':'<=0','ble':'<=0','bgt':'>0','bgt.w':'>0','beq.w':'==0','beq':'==0',
                     'bne.w':'!=0','bne':'!=0','blt.w':'<0','bmi.w':'<0','bmi':'<0'}.get(op,'>=0')
        return f'if (m.comparison{condition}) pc = {target};'
    raise ValueError(f'Unsupported instruction: {ins}')

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    args = parser.parse_args()
    firmware = (ROOT/'binaries/mimeophon_mp86.bin').read_bytes()
    if hashlib.sha256(firmware).hexdigest()!=SHA: raise ValueError('Wrong firmware')
    index = {x['address']:x for x in json.loads((ROOT/'analysis/reachable_instructions.json').read_text())}
    output = ['// Generated test-only bounded instruction execution. Do not edit.']
    summary = {}
    for name,(start,end,ranges) in SLICES.items():
        instructions = []
        for lo,hi in ranges:
            pc = BASE+lo
            while pc<BASE+hi:
                ins = index[pc]
                raw = bytes.fromhex(ins['bytes'])
                off = pc-0x08020000
                if firmware[off:off+len(raw)]!=raw: raise ValueError(f'Byte mismatch at {pc:x}')
                instructions.append(ins)
                pc += ins['size']
            if pc!=BASE+hi: raise ValueError('End not on instruction boundary')
        exits = (end,) if isinstance(end,int) else end
        allowed = {x['address'] for x in instructions}|{BASE+x for x in exits}
        for ins in instructions:
            if 'target' in ins and ins['target'] not in allowed: raise ValueError('Branch escapes slice')
        exit_condition = ' || '.join(f'pc==0x{BASE+x:08x}u' for x in exits)
        output += [f'inline void trace_{name}(Machine& m) {{',f'  std::uint32_t pc=0x{BASE+start:08x}u;',
                   '  for (int steps=0;steps<1000;++steps) {',f'    if ({exit_condition}) {{ m.exit_pc=pc; return; }}',
                   '    switch(pc) {']
        for ins in instructions:
            output += [f'    case 0x{ins["address"]:08x}u: // {ins["op"]} {ins["args"]}',
                       f'      pc=0x{ins["address"]+ins["size"]:08x}u;',
                       '      '+translate(ins,firmware),'      break;']
        output += ['    default: throw std::runtime_error("PC escaped bounded slice");',
                   '    }','  }','  throw std::runtime_error("Bounded slice step budget exhausted");','}']
        summary[name] = {'instruction_count':len(instructions),
                         'exits':[f'0x{BASE+x:08x}' for x in exits],
                         'ranges_half_open':[[f'0x{BASE+a:08x}',f'0x{BASE+b:08x}'] for a,b in ranges]}
    args.output.write_text('\n'.join(output)+'\n',encoding='utf-8')
    summary = {'scope':'Binary-checked bounded instruction translation; finite host float/FMA, not ARM execution',
               'firmware_sha256':SHA,'slices':summary}
    (ROOT/'analysis/color_trace_audit.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    print(f'PASS: generated {len(SLICES)} DSP/event slices, all binary-checked.')

if __name__=='__main__': main()
