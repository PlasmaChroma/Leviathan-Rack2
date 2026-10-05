#!/usr/bin/env python3
"""Original first-record move/stop/tail slice joined to input history and tape writes."""
import itertools
import json
import struct
from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32
from probe_pending_first_record_stop import setup
from probe_channel_set_state import LENGTH
from probe_speed_consumers import CHANNEL

INPUT, INPUT_DATA = 0x1B0000, 0x1C0000
CONTRIBUTION, INDICES, FEEDBACK = 0x1D0000, 0x1E0000, 0x1F0000
RAW_HEADER, RAW_DATA, TAPE = 0x200000, 0x201000, 0x210000
TAPE_LENGTH = 20000


def main():
    sequences, blocks, writes, comparisons, coverage, traces = 0, 0, 0, 0, set(), []
    for minimum, frames in itertools.product((128, 1280), (7, 31, 32, 128)):
        initial = minimum * 4 - 2 * frames
        c = setup(minimum, initial, 0)
        for address in (0x48F1C, 0x48F30):
            del c.observers[address]
        def stop(machine):
            machine.returned = True
            machine.cpu.emu_stop()
        c.observers[0x47DC0] = stop
        c.record_calls = []
        c.observers[0x47818] = lambda machine: c.record_calls.append([machine.reg(i) for i in range(4)])
        c.putu(CHANNEL + 171900 + 72, TAPE)
        for header, data, integer in ((0x24, CONTRIBUTION, False), (0x30, INDICES, True), (0x3C, FEEDBACK, False)):
            c.vector(CHANNEL + 172032 + header, data, [0] * frames, integers=integer)
        c.vector(INPUT + 4, INPUT_DATA, [0.] * (frames + 4))
        c.reg(0, CHANNEL + 25160)
        c.fp(0, 1.)
        c.fp(1, 0.)
        c.call(0x4FFE0)
        c.write(0x93F98, (ROOT / 'tables/reconstructed_xfade_table.f32le').read_bytes())
        c.reg(0, CHANNEL + 592)
        c.reg(1, 7)
        c.call(0x3A6AC)
        assert c.read(CHANNEL + 604, 1) == b'\1'
        expected_tape = [0.] * TAPE_LENGTH
        buffer, counter, stored, mode = [0.] * (frames + 4), initial, 123, 1
        trace = []
        for block in range(3000):
            samples = [f32(((block * frames + i) % 97 - 48) / 256.) for i in range(frames)]
            buffer = buffer[-4:] + samples
            c.vector(RAW_HEADER, RAW_DATA, samples)
            c.reg(0, INPUT)
            c.reg(1, RAW_HEADER)
            c.call(0x38A78)
            assert c.floats(INPUT_DATA, frames + 4) == buffer
            previous, counter = counter, counter + frames
            complete = mode == 1 and counter > minimum * 4
            if complete:
                mode, stored = 3, counter + 12292
            reset = mode != 1 and not complete and counter > stored
            c.record_calls.clear()
            c.reg(4, CHANNEL)
            c.reg(7, c.getu(LENGTH))
            c.fp(16, frames)
            c.putu(STACK + 0x2C, frames)
            c.putu(STACK + 0x10, INPUT)
            c.putu(STACK + 0x3C, 0)
            c.call(0x48C04)
            assert c.getu(CHANNEL + 632) == CHANNEL + (592 if mode == 1 else 620)
            assert c.getu(CHANNEL + 744) == (0 if reset else counter)
            assert c.read(CHANNEL + 740, 1)[0] == int(not reset)
            assert c.getu(LENGTH) == stored
            assert c.record_calls == ([] if reset else [[0, CHANNEL, CHANNEL + 740, INPUT]])
            if not reset:
                indices = list(range(previous, counter))
                assert [c.gets(INDICES + i * 4) for i in range(frames)] == indices
                for i, index in enumerate(indices):
                    expected_tape[index] = buffer[i + 1]
                assert c.getf(INPUT + 16) == 0.
                writes += frames
                comparisons += frames + 1
            observed = c.floats(TAPE, TAPE_LENGTH)
            assert observed == expected_tape, (minimum, frames, block,
                next((i for i, (a, b) in enumerate(zip(observed, expected_tape)) if a != b), None))
            comparisons += TAPE_LENGTH + frames + 9
            blocks += 1
            if complete or reset or counter in (minimum * 4, stored):
                trace.append(dict(block=block, moved_counter=counter, completion=complete, reset=reset,
                                  stored_length=stored, wrote=0 if reset else frames))
            if reset:
                traces.append(dict(minimum=minimum, frames=frames, initial_counter=initial,
                                   last_written_index=previous-1, unused_stored_samples=stored-previous, steps=trace))
                break
        else:
            raise AssertionError('Tail did not finish within fixture budget')
        sequences += 1
        coverage.update(c.coverage)
    result = dict(status='PASS', sequences=sequences, callback_slices=blocks, samples_written=writes,
                  comparisons=comparisons, distinct_instruction_addresses=len(coverage), traces=traces,
                  limitations=['Original callback slice from first-record move through real recordInput or tail reset; original input history published separately.',
                               'Supplied fixed unit speed, small tape, controls/Link pointers, zero feedback, transparent write clipper and reconstructed fade table.',
                               'No full input/output coloration, GPIO/link producers, startup samples before supplied counter, import/export or capacity-near tape writes.'])
    (ROOT / 'probes/first_record_tail_write_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: value for key, value in result.items() if key != 'traces'}, indent=2))


if __name__ == '__main__':
    main()
