#!/usr/bin/env python3
"""Boundary allocation/trigger/render/update with independently generated fade states."""
import itertools
import argparse
import json
import math
import struct
from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32
from probe_boundary_transitions import execute, model, CHANNEL, MANAGER, SLOT_SIZE
from probe_splice_render import WORK, TAPE, OUTPUT, GAINS, TABLE, fade_curve, RECORD_MANAGER
from probe_tap_fade_state import triggered, reset, advance, ModelView, TAP
from probe_transport import cubic
from probe_wrapped_boundaries import model as seam_model, snapshot


def check(cpu, pointer, states):
    for kind, want in enumerate(states):
        b = pointer+28+kind*24
        got = [cpu.read(b,1)[0], cpu.gets(b+4), cpu.getf(b+8), cpu.gets(b+12), cpu.getf(b+16), cpu.getf(b+20)]
        assert got == want, (pointer,kind,want,got)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seams', action='store_true')
    parser.add_argument('--saturation', action='store_true')
    args = parser.parse_args()
    assert not args.saturation or args.seams, '--saturation requires --seams'
    cases, renders, comparisons, coverage, error_max = 0, 0, 0, set(), 0.
    double_fade_heads = 0
    failed_allocations, successful_allocations = 0, 0
    tape_length = 16384 if args.saturation else 4096
    tape = [f32(.3*math.sin(i*.137)+.1*math.cos(i*.071)) for i in range(tape_length)]
    for occupancy, family, effective, held, frames, fraction, duration, replacement in itertools.product(
            (4,5) if args.saturation else (1,),
            ('physical','wrapped','double') if args.seams else ('ordinary',),
            (-1., -.125, 0., .125, 1.), (False, True), (7,32,128) if args.seams else (7,32),
            (0.,.375), (32,128), (False,True)):
        speed = -effective if held else effective
        forward = effective >= 0
        delta = f32(f32(effective*.75)*frames)
        target = 501 if forward else 131
        previous = target-math.floor(f32(fraction+delta))
        config = dict(start=100,end=500,reverse_start=132,reverse_end=532,fade=duration,
                      speed=speed,held=effective if held else None,effective_speed=effective,factor=.75,
                      frames=frames,initial_fraction=fraction,previous=previous,replacement=replacement,
                      saturated=False,already_fading=False)
        if args.seams:
            config.update(start=700,end=300,reverse_start=732,reverse_end=332,reverse_start_mod=732,
                          period=1024,physical_reverse=32,physical_reverse_destination=1282,
                          physical_fading=False,occupied_slots=occupancy,saturated=occupancy==5)
            target = (1025 if forward else 31) if family=='physical' else (301 if forward else 731)
            if family=='double':
                config.update(start=1000,reverse_start=1032,reverse_start_mod=8)
                target = 1025 if forward else 7
            previous = target-math.floor(f32(fraction+delta))
            config['previous'] = previous
        expected = seam_model(config) if args.seams else model(config)
        observed, cpu = execute(config, return_cpu=True)
        if args.seams:
            observed = snapshot(observed,cpu,config)
        assert observed == expected, (config,expected,observed)
        new_count = len(expected['new_heads']) if args.seams else int(expected['replacement'] is not None)
        successful_allocations += new_count
        failed_allocations += len(expected['allocations'])-new_count
        coords = {0: (previous,fraction,expected['current'],expected['fraction' if args.seams else 'current_fraction'])}
        if args.seams:
            for new in expected['new_heads']:
                coords[new['slot']] = (new['previous'],new['previous_fraction'],new['position'],new['fraction'])
        elif expected['replacement']:
            new = expected['replacement']
            coords[1] = (new['previous'],new['previous_fraction'],new['position'],new['fraction'])
        states = {slot:[reset() for _ in range(4)] for slot in expected['active_slots']}
        chosen = {slot:effective for slot in coords}
        for slot in expected['active_slots']:
            if slot not in coords:
                coords[slot] = (10000+slot,0.,10000+slot,0.)
                chosen[slot] = speed
        for relative,kind,boundary,duration_arg,rising,direction in expected['fades']:
            slot = relative//SLOT_SIZE
            states[slot][kind] = triggered(*coords[slot],boundary,duration_arg,rising,direction)
            if slot:
                # The replacement inherits the other source fades before its
                # crossing motion; that motion advances inherited active fades.
                states[slot] = [s if k==kind else list(states[0][k]) for k,s in enumerate(states[slot])]
                pi,pf,ci,cf = coords[slot]
                if pi != ci or pf != cf:
                    for k,s in enumerate(states[slot]):
                        if k != kind and s[0]:
                            advance(s,delta)
        double_fade_heads += sum(sum(s[0] for s in fade_states)>1 for fade_states in states.values())
        for slot in states:
            check(cpu,MANAGER+slot*SLOT_SIZE,states[slot])
        cpu.vector(WORK+12,OUTPUT,[0.]*frames)
        cpu.vector(WORK+24,GAINS,[1.]*frames)
        cpu.reg(0,RECORD_MANAGER)
        cpu.call(0x4CA68)
        cpu.putu(STACK+0x30,RECORD_MANAGER)
        cpu.putu(STACK+12,WORK+24)
        cpu.putu(CHANNEL+171972,TAPE)
        cpu.write(TAPE,struct.pack('<%df' % tape_length,*tape))
        cpu.write(0x93F98,struct.pack('<256f',*TABLE))
        mix = [0.]*frames
        for slot in expected['active_slots']:
            pointer = MANAGER+slot*SLOT_SIZE
            gain = fade_curve(ModelView(states[slot]),TAP,frames)
            gain = [f32(g*min(1.,max(0.,4*abs(chosen[slot])))) for g in gain]
            pi,pf,ci,cf = coords[slot]
            increment = f32(f32(f32(f32(ci)+cf)-f32(f32(pi)+pf))/frames)
            anchor = pi+(2 if chosen[slot] >= 0 else -3)
            phase = pf
            for i in range(frames):
                index = max(1,min(29501997,anchor))-1
                assert 0 <= index <= len(tape)-4
                mix[i] += gain[i]*cubic(*tape[index:index+4],phase)
                part = f32(phase+increment)
                whole = math.floor(part)
                anchor,phase = anchor+whole,f32(part-whole)
            cpu.reg(4,CHANNEL)
            cpu.reg(5,WORK)
            cpu.reg(6,pointer)
            cpu.reg(7,29501997)
            for r,v in ((16,frames),(17,.75),(18,0.),(19,speed),(20,speed),(21,abs(speed)),
                        (22,abs(speed)),(24,4096.),(25,0.)):
                cpu.fp(r,v)
            cpu.call(0x48048,stop_before=0x48398)
            for field,want,actual in (('mix',mix,cpu.floats(OUTPUT,frames)),('gain',gain,cpu.floats(GAINS,frames))):
                for a,b in zip(want,actual):
                    error = abs(a-b)
                    assert math.isfinite(b) and error <= 8e-7,(config,slot,field,a,b)
                    error_max = max(error_max,error)
                    comparisons += 1
            renders += 1
        cpu.reg(0,MANAGER)
        cpu.call(0x4D138)
        for slot,fade_states in states.items():
            pointer = MANAGER+slot*SLOT_SIZE
            live = not any(s[0] and s[1]<0 for s in fade_states)
            if not live:
                fade_states = [reset() for _ in range(4)]
            else:
                fade_states = [reset() if s[0] and s[1]>254 else s for s in fade_states]
            check(cpu,pointer,fade_states)
            assert cpu.read(pointer,1)[0] == int(live)
            comparisons += 49
        cases += 1
        coverage.update(cpu.coverage)
    result = dict(status='PASS',boundary_cases=cases,head_renders=renders,comparisons=comparisons,
                  simultaneous_fade_heads=double_fade_heads,
                  successful_allocations=successful_allocations,failed_allocation_attempts=failed_allocations,
                  max_abs_error=error_max,distinct_instruction_addresses=len(coverage),
                  limitations=['Original boundary allocation, trigger, transfer, render and manager update; independent head coordinates/fade states/gains/mix.',
                               'Physical/wrapped/double seams' if args.seams else 'Ordinary selected-region boundaries',
                               'Four/five occupied slots in one engine' if args.saturation else 'Fresh source with available replacement slots',
                               'Supplied Link/stack scheduling and explicit head render order; no whole output effects, persistent reentry or iteration/movement of other old heads.'])
    filename = 'seam_fade_model_probe_results.json' if args.seams else 'boundary_fade_model_probe_results.json'
    if args.saturation:
        filename = 'saturated_seam_fade_model_probe_results.json'
    (ROOT/'probes'/filename).write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))


if __name__ == '__main__':
    main()
