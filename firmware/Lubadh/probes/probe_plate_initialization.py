#!/usr/bin/env python3
"""Execute the inlined MonoPlate constructor, retaining its state map.

This does not yet execute or validate the complete plate recurrence.
"""
import hashlib
import json

from arm_byte_probe import ARMBytes, ROOT

CHANNEL = 0x100000
PLATE = CHANNEL + 25184
SIZE = 146688


def main():
    cpu = ARMBytes()
    cpu.reg(4, CHANNEL)
    cpu.call(0x3CFF0, stop_before=0x3D618)
    # Process code dereferences these data-object pointers, whose first word
    # points to their sample storage and second word carries allocated length.
    descriptors = (0xF58, 0x1330, 0x1604, 0x1FD8, 0x270C, 0x3054,
                   0x9ED4, 0xCD34, 0x11DDC, 0x12410, 0x198A4, 0x1DD28, 0x23CD0)
    rows = []
    for offset in descriptors:
        pointer = cpu.getu(PLATE + offset)
        assert PLATE <= pointer < PLATE + SIZE, (hex(offset), hex(pointer))
        data, capacity = cpu.getu(pointer), cpu.getu(pointer + 4)
        assert PLATE <= data < PLATE + SIZE and 0 < capacity < 20000, (hex(offset), hex(data), capacity)
        assert data + capacity * 4 <= PLATE + SIZE
        assert cpu.read(data, capacity * 4) == bytes(capacity * 4)
        rows.append(dict(field_offset=hex(offset), descriptor_offset=pointer - PLATE,
                         data_offset=data - PLATE, capacity=capacity,
                         write_index=cpu.getu(PLATE + offset + 4),
                         processing_span=cpu.getu(PLATE + offset + 8)))
    # Retain only nonzero constructor words; inline audio regions are zero.
    words = [dict(offset=hex(offset), bits=hex(cpu.getu(PLATE + offset)))
             for offset in range(0, SIZE, 4) if cpu.getu(PLATE + offset)]
    result = dict(status='PASS', constructor_slice=['0x3cff0', '0x3d618'],
                  plate_channel_offset=25184, inspected_bytes=SIZE, descriptors=rows,
                  nonzero_words=words, distinct_instruction_addresses=len(cpu.coverage),
                  coverage_addresses=[hex(a) for a in sorted(cpu.coverage)],
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  limitations=['Inlined plate initialization only; no process, impulse, control-update or output comparison.',
                               'Descriptor labels are process-load addresses, not final semantic names.',
                               'Inspected span ends before unrelated Channel asset constructors.'])
    (ROOT / 'probes/plate_initialization_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'descriptors', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
