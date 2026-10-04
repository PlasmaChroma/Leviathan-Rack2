"""Check six MP86 read kernels against exact symbolic basis weights.

Uses the supplied recursive instruction listing, verifies its bytes against the
canonical image, then interprets the selected floating arithmetic symbolically.
This is neither an independent disassembler nor ARM CPU/float32 emulation.
No third-party dependencies. Run from any directory with Python 3.10+.
"""
from pathlib import Path
from fractions import Fraction as F
import hashlib
import json

ROOT = Path(__file__).resolve().parents[1]
EXPECTED_SHA = '31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9'

# Sparse polynomials in (before, center, after, after2, t).
ZERO = (0, 0, 0, 0, 0)
def constant(value):
    return {ZERO: F(value)} if value else {}

def variable(i):
    powers = [0]*5
    powers[i] = 1
    return {tuple(powers): F(1)}

def plus(a, b, sign=1):
    result = dict(a)
    for key, value in b.items():
        result[key] = result.get(key, F(0)) + sign*value
    return {key: value for key, value in result.items() if value}

def times(a, b):
    result = {}
    for ka, va in a.items():
        for kb, vb in b.items():
            key = tuple(x+y for x, y in zip(ka, kb))
            result[key] = result.get(key, F(0)) + va*vb
    return {key: value for key, value in result.items() if value}

def require(condition, message):
    if not condition:
        raise AssertionError(message)

# Explicit boundary register values and addressed memory reads for each site.
# All address arithmetic lies outside the symbolic model; load mappings are
# analyst annotations checked against the accompanying assembly, not inferred.
CASES = [
    (0x2443a, 0x24478, 's14', 's2', 's13',
     {0x24444: 0, 0x2444c: 3, 0x24454: 1, 0x2445c: 2}),
    (0x2454a, 0x24588, 's15', 's5', 's14',
     {0x24554: 0, 0x2455c: 3, 0x24568: 1, 0x2456c: 2}),
    (0x25610, 0x25654, 's0', 's2', 's13',
     {0x2561c: 3, 0x25622: 0, 0x25630: 1, 0x2563c: 2}),
    (0x256c4, 0x2570c, 's2', 's5', 's1',
     {0x256d0: 3, 0x256d6: 0, 0x256e4: 1, 0x256f2: 2}),
    (0x257d2, 0x25816, 's0', 's2', 's14',
     {0x257de: 3, 0x257e4: 0, 0x257f2: 1, 0x257fe: 2}),
    (0x25886, 0x258ce, 's11', 's5', 's6',
     {0x25892: 3, 0x25898: 0, 0x258a6: 1, 0x258b2: 2}),
]

def main():
    firmware = (ROOT/'binaries/mimeophon_mp86.bin').read_bytes()
    require(hashlib.sha256(firmware).hexdigest() == EXPECTED_SHA, 'Wrong firmware')
    instructions = json.loads((ROOT/'analysis/reachable_instructions.json').read_text())
    index = {item['address']: item for item in instructions}
    a, b, c, d, t = [variable(i) for i in range(5)]
    outer_weight = times(constant(F(1, 2)), times(t, plus(t, constant(-1))))
    center_weight = plus(constant(1), times(constant(F(1, 2)), times(t, plus(t, constant(1)))), -1)
    after_weight = times(constant(F(1, 2)), times(t, plus(constant(3), t, -1)))
    expected = plus(plus(times(a, outer_weight), times(b, center_weight)),
                    plus(times(c, after_weight), times(d, outer_weight)))
    results = []
    for start, end, fraction, onehalf, output, loads in CASES:
        registers = {fraction: t, onehalf: constant(F(3, 2)), 's12': constant(F(1, 2))}
        address = 0x08000000 + start
        end_address = 0x08000000 + end
        verified = []
        seen_loads = set()
        while address <= end_address:
            ins = index[address]
            raw = bytes.fromhex(ins['bytes'])
            offset = address - 0x08020000
            require(firmware[offset:offset+len(raw)] == raw, f'Byte mismatch at {address:x}')
            op = ins['op']
            args = [x.strip() for x in ins['args'].split(',')]
            if op == 'vldr':
                relative = address-0x08000000
                registers[args[0]] = variable(loads[relative])
                seen_loads.add(relative)
            elif op in ('vadd.f32', 'vsub.f32', 'vmul.f32', 'vfma.f32', 'vfms.f32'):
                dest, left, right = args
                x, y = registers[left], registers[right]
                if op == 'vadd.f32': value = plus(x, y)
                elif op == 'vsub.f32': value = plus(x, y, -1)
                elif op == 'vmul.f32': value = times(x, y)
                else: value = plus(registers[dest], times(x, y), -1 if op == 'vfms.f32' else 1)
                registers[dest] = value
            else:
                require(op in ('add.w', 'adds', 'ands', 'ldr', 'str'), f'Unexpected instruction: {ins}')
            verified.append(f'0x{address:08x}')
            address += ins['size']
        require(seen_loads == set(loads), 'Missing annotated load')
        require(registers[output] == expected, f'Algebra mismatch at {start:x}')
        results.append({'start': f'0x{0x08000000+start:08x}',
                        'end_inclusive': f'0x{end_address:08x}',
                        'verified_instruction_count': len(verified), 'symbolic_match': True})
    result = {
        'firmware_sha256': EXPECTED_SHA,
        'scope': 'Binary-checked disassembly slices and exact rational algebra; not CPU execution or float32 equivalence',
        'load_mapping': 'Manually annotated memory inputs; address generation is not interpreted',
        'weights': ['t*(t-1)/2', '1-t*(t+1)/2', 't*(3-t)/2', 't*(t-1)/2'],
        'sites': results,
    }
    destination = ROOT/'analysis/delay_read_audit.json'
    destination.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(f'PASS: {len(results)} binary-checked read kernels match the four-tap quadratic exactly.')

if __name__ == '__main__':
    main()
