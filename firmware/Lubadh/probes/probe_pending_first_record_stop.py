#!/usr/bin/env python3
"""Original first-record move/retry/capacity decision slice with real stop consumer."""
import itertools
import json
from arm_byte_probe import ROOT, STACK
from probe_channel_set_state import fixture, LENGTH
from probe_speed_consumers import CHANNEL, LINK, PRESET
from probe_loop_regions import LENGTH_ADC, START_ADC, WORK


def setup(minimum, counter, pending):
    c = fixture(0, 0, 1., 0, 0, 0, 0, 128)
    state, table = CHANNEL + 592, 0x1A0000
    c.putu(state, table)
    c.putu(table, 0x3A6AC)
    c.putu(state + 4, 1)
    c.putu(state + 8, CHANNEL)
    c.putu(CHANNEL + 632, state)
    c.putu(CHANNEL + 212, PRESET)
    c.putu(PRESET + 48, minimum)
    for field, value in ((40, 49170), (128, 4), (24, 9000), (28, 5000)):
        c.putu(LINK + field, value)
    for field, value in ((76, 30000), (80, 32458), (128, 1000), (136, 128), (56, 1), (132, 512)):
        c.putu(CHANNEL + field, value)
    for field, value in ((0x4A4, 2048), (0x4A0, 1024), (0x4B4, 2048)):
        c.putu(WORK + field, value)
    for adc in (LENGTH_ADC, START_ADC):
        c.putu(adc + 4, 0)
        c.putu(adc + 8, 0)
        c.putu(adc + 12, 2)
    c.reg(0, CHANNEL + 740)
    c.reg(1, counter)
    c.call(0x4DF18)
    c.write(CHANNEL + 604, bytes([pending]))
    c.stop_address = None
    c.capacity_seen = False
    c.events = []
    c.observers[0x3F1C4] = lambda machine: c.events.append(machine.reg(1))
    def stop(machine, address):
        machine.stop_address = address
        machine.returned = True
        machine.cpu.emu_stop()
    c.observers[0x48F50] = lambda machine: setattr(machine, 'capacity_seen', True)
    for address in (0x48F1C, 0x48F30):
        c.observers[address] = lambda machine, address=address: stop(machine, address)
    c.coverage.clear()
    return c


def advance(c, frames):
    c.reg(4, CHANNEL)
    c.reg(7, 123)
    c.fp(16, frames)
    c.putu(STACK + 0x2C, frames)
    c.call(0x48C04)


def main():
    cases, checks, coverage, retries, capacity_cases = 0, 0, set(), 0, 0
    traces = []
    for minimum, frames, pending, delta in itertools.product((128, 1280), (1, 32, 128), range(2), range(6)):
        offset = (-frames-1, -frames, -frames+1, -1, 0, 1)[delta]
        counter = minimum * 4 + offset
        c = setup(minimum, counter, pending)
        advance(c, frames)
        after = counter + frames
        retry = bool(pending and after > minimum * 4)
        assert c.getu(CHANNEL + 744) == after
        assert c.events == ([0] if retry else [])
        assert c.stop_address == (0x48F30 if retry else 0x48F1C)
        assert c.read(CHANNEL + 604, 1)[0] == (0 if retry else pending)
        assert c.getu(CHANNEL + 632) == CHANNEL + (620 if retry else 592)
        assert c.getu(LENGTH) == (after + 12292 if retry else 123)
        cases += 1
        retries += int(retry)
        checks += 6
        coverage.update(c.coverage)
        if minimum == 1280 and frames == 128 and pending:
            traces.append(dict(before=counter, after=after, threshold=minimum * 4, retried=retry))
    for frames, pending, offset in itertools.product((1, 32, 128), range(2), (-1, 0, 1)):
        limit = 29502000 - 12292 - 2 * frames
        counter = limit + offset - frames
        c = setup(1280, counter, pending)
        advance(c, frames)
        capacity = offset >= 0
        retry = bool(capacity or pending)
        assert c.getu(CHANNEL + 744) == limit + offset
        assert c.capacity_seen == capacity
        assert c.stop_address == (0x48F30 if retry else 0x48F1C)
        assert c.events == ([0] if retry else [])
        assert c.read(CHANNEL + 604, 1)[0] == (0 if retry else pending)
        assert c.getu(CHANNEL + 632) == CHANNEL + (620 if retry else 592)
        assert c.getu(LENGTH) == (limit + offset + 12292 if retry else 123)
        checks += 7
        cases += 1
        capacity_cases += 1
        coverage.update(c.coverage)
    sequences = 0
    for minimum, frames in itertools.product((128, 1280), (1, 32, 128)):
        threshold = minimum * 4
        c = setup(minimum, threshold - 2 * frames, 0)
        c.reg(0, CHANNEL + 592)
        c.reg(1, 7)
        c.call(0x3A6AC)
        assert c.read(CHANNEL + 604, 1) == b'\1'
        for tick in range(3):
            advance(c, frames)
            count = threshold + (tick - 1) * frames
            complete = tick == 2
            assert c.getu(CHANNEL + 744) == count
            assert c.events == ([0] if complete else [])
            assert c.read(CHANNEL + 604, 1)[0] == int(not complete)
            assert c.getu(CHANNEL + 632) == CHANNEL + (620 if complete else 592)
            checks += 4
        sequences += 1
        checks += 1
        coverage.update(c.coverage)
    result = dict(status='PASS', decision_cases=cases, comparisons=checks, pending_retries=retries,
                  event_to_callback_sequences=sequences,
                  capacity_boundary_cases=capacity_cases, distinct_instruction_addresses=len(coverage), traces=traces,
                  limitations=['Original move, pending and capacity-handler slices from 0x48c04; stops before recording input.',
                               'Pending retry includes complete Channel event0, FirstRec stop, setState, region/head consumers with inert diagnostics.',
                               'Initialized active first-record Tap, supplied callback registers/control fields and separate Link; no whole callback or audio writes.'])
    (ROOT / 'probes/pending_first_record_stop_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: value for key, value in result.items() if key != 'traces'}, indent=2))


if __name__ == '__main__':
    main()
