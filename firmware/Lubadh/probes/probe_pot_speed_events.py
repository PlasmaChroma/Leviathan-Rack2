#!/usr/bin/env python3
"""Complete original setPotSpeed with independent persistent event-state model."""
import itertools
import json
import struct

from arm_byte_probe import ROOT
from arm_leaf_probe import f32
from probe_speed_consumers import fixture, slew_count, CHANNEL, LINK, PRESET, MARKERS
from speed_table_model import normalized_markers, speed_table
from probe_v_oct_curve import curve

APP, OTHER = 0x190000, 0x200000
WORK = CHANNEL + 172032
SEQUENCE = [-100, 0, 9, 10, 19, 20, 2047, 2048, 2056, 2057, 4095, 9000,
            4086, 4085, 2048, 2040, 2038]


def quantize(value, markers):
    for a, b in zip(markers, markers[1:]):
        if a <= value <= b:
            return b if value >= f32(f32(a + b) * .5) else a
    return markers[0] if value < markers[0] else markers[-1]


def model(s, raw, mode, state, reverse, linked, enable, table, markers):
    s = s.copy()
    raw = max(0, min(4095, raw))
    s['raw'] = raw
    def slew():
        s['target'] = s['candidate']
        s['remaining'] = slew_count(25.)
        s['increment'] = f32(f32(s['target'] - s['current']) / s['remaining'])
    if abs(raw - s['accepted']) > 9:
        s['accepted'] = raw
        s['candidate'] = table[raw]
        if state in (1, 2):
            if mode != 1:
                s['candidate'] = quantize(s['candidate'], markers)
            s['event'] = 1
            if linked:
                s['other_event'] = 1
            s['deferred'] = 1
        elif state == 0 and mode == 1:
            s['candidate'] = quantize(s['candidate'], markers)
        if mode == 3 and reverse == 1:
            s['candidate'] = -s['candidate']
        if state == 0:
            slew()
    if enable and s['deferred']:
        slew()
        s['deferred'] = 0
    return s


def observed(c, channel=CHANNEL, other=OTHER):
    work = channel + 172032
    return dict(raw=c.gets(work + 0x498), accepted=c.gets(work + 0x49C),
                candidate=c.getf(channel + 696), target=c.getf(channel + 44),
                current=c.getf(channel + 728), remaining=c.gets(channel + 736),
                increment=c.getf(channel + 732), event=c.getu(channel + 272),
                deferred=c.read(channel + 700, 1)[0], other_event=c.getu(other + 272))


def main():
    profiles = json.loads((ROOT / 'tables/factory_presets.json').read_text())
    profiles = {name: profiles[name] for name in ('01-Tape-Looper', '06-Octave-Delay', '07-SequencingMono')}
    profiles['fixture-smooth'] = dict(SpeedControl=2, SpeedMarkers=[0., .5, 1., 2., 4.])
    _, source = curve(0, 2457)
    cases, checks, coverage, traces = 0, 0, set(), []
    for name, p in profiles.items():
        mode = p['SpeedControl']
        table = speed_table(p['SpeedMarkers'], mode, source)
        markers = normalized_markers(p['SpeedMarkers'])
        for state, reverse, linked, enable, deferred in itertools.product(range(4), range(3), range(2), range(2), range(2)):
            c = fixture(mode=mode)
            c.write(PRESET + 548, struct.pack('<4096f', *table))
            c.vector(LINK + 180, MARKERS, markers)
            c.putu(CHANNEL + 32, APP)
            c.putu(CHANNEL + 28, OTHER)
            c.putu(CHANNEL + 260, state)
            c.putu(CHANNEL + 236, reverse)
            c.write(APP + 344064 + 0xABC, bytes([linked]))
            c.write(CHANNEL + 270, bytes([enable]))
            c.write(CHANNEL + 700, bytes([deferred]))
            c.puts(WORK + 0x49C, 2048)
            c.putf(CHANNEL + 696, -.125)
            c.putf(CHANNEL + 44, 1.25)
            c.putf(CHANNEL + 728, -.75)
            c.putf(CHANNEL + 732, f32(.24))
            c.puts(CHANNEL + 736, 7)
            s = dict(raw=0, accepted=2048, candidate=-.125, target=1.25, current=-.75,
                     increment=f32(.24), remaining=7, event=0, deferred=deferred, other_event=0)
            trace = []
            for raw in SEQUENCE:
                c.reg(0, CHANNEL)
                c.reg(1, raw)
                c.call(0x37510)
                s = model(s, raw, mode, state, reverse, linked, enable, table, markers)
                actual = observed(c)
                assert actual == s, (name, state, reverse, linked, enable, raw, actual, s)
                assert c.getu(CHANNEL + 236) == reverse
                cases += 1
                checks += len(s) + 1
                trace.append(dict(input=raw, **s))
            coverage.update(c.coverage)
            if reverse in (0, 1) and linked and enable and not deferred:
                traces.append(dict(profile=name, state=state, reverse=reverse, linked=linked,
                                   enable=enable, initial_deferred=deferred, steps=trace))
    result = dict(status='PASS', sequences=cases // len(SEQUENCE), calls=cases,
                  field_comparisons=checks, distinct_instruction_addresses=len(coverage),
                  input_sequence=SEQUENCE, traces=traces, coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Complete setPotSpeed only, no ADC/CV or gesture producer.',
                               'State and flag fields are supplied; their upstream meanings are not inferred from this test.',
                               'Linked peer receives pending event; full linked application scheduling not executed.',
                               'Speed tables from independent checked model; supplied V/oct calibration fixture.'])
    (ROOT / 'probes/pot_speed_event_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k not in ('traces', 'coverage_addresses')}, indent=2))


if __name__ == '__main__':
    main()
