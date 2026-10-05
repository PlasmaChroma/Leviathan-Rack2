#!/usr/bin/env python3
"""Original readADCs and two-deck speed dispatch with isolated MCP stimuli."""
import itertools
import json
import math
import struct
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR, UC_ARM_REG_SP
from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_pot_speed_events import model, observed
from speed_table_model import normalized_markers, speed_table
from probe_v_oct_curve import curve

APP = 0x100000
CHANNELS = [APP + 72, APP + 173440]
# MCP pin field, ADC object, published integer; six slow slots follow two speed ADCs.
FIELDS = [(172032 + 0x4C8, 173376, 172032 + 0x4E0),
          (344064 + 0xA00, 346744, 344064 + 0xA18),
          (172032 + 0x4C9, 173392, 172032 + 0x4E8),
          (344064 + 0xA01, 346760, 344064 + 0xA20),
          (172032 + 0x4CB, 173408, 172032 + 0x4EC),
          (344064 + 0xA03, 346776, 344064 + 0xA24),
          (172032 + 0x4CA, 173424, 172032 + 0x4F0),
          (344064 + 0xA02, 346792, 344064 + 0xA28)]


class ADCBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.samples = {}
        self.reads = []

    def _code(self, cpu, address, size, user):
        if address == 0x6C064:
            pin = self.reg(1)
            assert pin in self.samples
            self.coverage.add(address)
            self.reads.append(pin)
            self.reg(0, self.samples[pin])
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, user)


def adc_step(current, raw, alpha):
    return math.trunc(libm.fmaf(f32(4095 - raw), alpha, f32(f32(current) * f32(1. - alpha))))


def main():
    presets = json.loads((ROOT / 'tables/factory_presets.json').read_text())
    profiles = [presets['01-Tape-Looper'], presets['07-SequencingMono']]
    _, source = curve(0, 2457)
    tables = [speed_table(p['SpeedMarkers'], p['SpeedControl'], source) for p in profiles]
    markers = [normalized_markers(p['SpeedMarkers']) for p in profiles]
    coverage, cases, comparisons, traces = set(), 0, 0, []
    for alpha_mode, state_a, state_b, linked, enable in itertools.product((.1, .5, 1., 'production'), range(4), range(4), range(2), range(2)):
        alphas = [f32(.9)] * 2 + [.5] * 6 if alpha_mode == 'production' else [f32(alpha_mode)] * 8
        c = ADCBytes()
        adc_current = [0] * 8
        for pin, (pin_field, adc, out) in enumerate(FIELDS):
            c.write(APP + pin_field, bytes([pin]))
            c.putf(APP + adc, alphas[pin])
        if alpha_mode == 'production':
            for channel in CHANNELS:
                c.reg(4, channel)
                c.reg(5, channel + 172032)
                c.call(0x3D730, stop_before=0x3D780)
            for pin, (_, adc, _) in enumerate(FIELDS):
                assert c.getf(APP + adc) == alphas[pin]
                assert c.gets(APP + adc + 4) == 0
                assert c.gets(APP + adc + 8) == 0
                assert c.gets(APP + adc + 12) == 2
                comparisons += 4
        states = []
        for deck, channel in enumerate(CHANNELS):
            link, preset, vector = 0x1A0000 + deck * 4096, 0x1B0000 + deck * 65536, 0x1D0000 + deck * 4096
            c.putu(channel + 232, link)
            c.putu(link + 176, preset)
            c.putf(link + 172, 25.)
            c.putu(preset + 64, profiles[deck]['SpeedControl'])
            c.write(preset + 548, struct.pack('<4096f', *tables[deck]))
            c.vector(link + 180, vector, markers[deck])
            c.putu(channel + 32, APP)
            c.putu(channel + 28, CHANNELS[1 - deck])
            c.putu(channel + 260, (state_a, state_b)[deck])
            c.putu(channel + 236, deck)
            c.write(channel + 270, bytes([enable]))
            c.puts(channel + 172032 + 0x49C, 2048)
            c.putf(channel + 696, -.125)
            c.putf(channel + 44, 1.25)
            c.putf(channel + 728, -.75)
            states.append(dict(raw=0, accepted=2048, candidate=-.125, target=1.25, current=-.75,
                               increment=0., remaining=0, event=0, deferred=0, other_event=0))
        c.write(APP + 344064 + 0xABC, bytes([linked]))
        trace = []
        for tick in range(24):
            raw = [(tick * 541 + pin * 337) % 4096 for pin in range(8)]
            if tick in (0, 1, 2):
                raw[:2] = [[4095, 0], [0, 4095], [2048, 2047]][tick]
            c.samples = dict(enumerate(raw))
            slot = tick % 8  # 6 and 7 also verify no slow-channel update.
            active = [0, 1] + ([2 + slot] if slot < 6 else [])
            c.reads = []
            c.reg(0, APP)
            c.reg(1, slot)
            c.cpu.reg_write(UC_ARM_REG_SP, STACK)
            c.call(0x28190)
            assert c.reads == active
            comparisons += 1
            for pin in active:
                adc_current[pin] = adc_step(adc_current[pin], raw[pin], alphas[pin])
            for pin, (_, adc, out) in enumerate(FIELDS):
                assert c.gets(APP + adc + 4) == adc_current[pin]
                assert c.gets(APP + out) == adc_current[pin]
                comparisons += 2
            # Execute both actual setPotSpeed calls and stop before loop/time consumers.
            c.reg(0, APP)
            c.cpu.reg_write(UC_ARM_REG_SP, STACK)
            c.call(0x2830C, stop_before=0x28340)
            for deck in range(2):
                states[deck] = model(states[deck], adc_current[deck], profiles[deck]['SpeedControl'],
                                     (state_a, state_b)[deck], deck, linked, enable, tables[deck], markers[deck])
                if linked:
                    states[1 - deck]['event'] = states[deck]['other_event']
                states[1 - deck]['other_event'] = states[deck]['event']
            for deck in range(2):
                actual = observed(c, CHANNELS[deck], CHANNELS[1 - deck])
                assert actual == states[deck], (alpha_mode, state_a, state_b, linked, enable, tick, deck, actual, states)
                comparisons += len(actual)
            trace.append(dict(tick=tick, slot=slot, speed_adc=adc_current[:2], deck_states=[s.copy() for s in states]))
            cases += 1
        coverage.update(c.coverage)
        if alpha_mode in (.5, 'production') and state_a == 0 and state_b == 1 and linked and enable:
            traces.append(dict(alpha_mode=alpha_mode, alphas=alphas, states=[state_a, state_b], linked=linked, enable=enable, steps=trace))
    result = dict(status='PASS', sequences=cases // 24, ticks=cases, field_comparisons=comparisons,
                  distinct_instruction_addresses=len(coverage), traces=traces,
                  limitations=['readMCP boundary supplies synthetic 12-bit samples; no hardware or SPI executed.',
                               'Production ADC constructors execute; additional alpha .1/.5/1 fixtures extend arithmetic coverage.',
                               'interpretADCs stops after both speed calls, before loop/time consumers.',
                               'Asymmetric mode/state matrix, not full link/gesture/audio callback schedule.',
                               'External speed CV is not separately digitized in this routine; physical front-end mapping remains unresolved.'])
    (ROOT / 'probes/adc_speed_publication_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'traces'}, indent=2))


if __name__ == '__main__':
    main()
