#!/usr/bin/env python3
"""Differential tests against recovered bytes, not a full firmware emulator.

Only 627C's ADC wrapper is hooked. Calibration thresholds are synthetic.
Outputs are written only after all assertions pass.
"""
from dataclasses import asdict
from pathlib import Path
import json
import random

from pic18_harness import initialized
from timing_models import LeadingState, leading_capture_service, tempo_control

ROOT = Path(__file__).resolve().parents[1]
FIELDS = dict(requested=(0x45,4), current=(0x41,4), countdown=(0x49,4),
              elapsed=(0x446,4), measuring=(0x481,1), accepted=(0x5a5,1),
              capture=(0x586,4), edge_remaining=(0x442,4),
              edge_level=(0x480,1), dirty=(0x47f,1))


def main():
    m = initialized()
    baseline = bytes(m.ram)
    summary = {}
    examples = []
    rng = random.Random(71)
    cases = [LeadingState(capture=c, edge_remaining=r, edge_level=l)
             for c in (0,1,398,399,400,401,15999,16000,16001,16002,32000,0x7fffffff)
             for r in (0,1,3999,4000,4001,7999,8000,12000) for l in (0,1)]
    cases += [LeadingState(elapsed=e, accepted=a, capture=c)
              for e in (119999,120000,120001,0xfffffe,0xffffff,0x1000000)
              for a in (0,1) for c in (0,398,16002)]
    cases += [LeadingState(requested=h, current=h, countdown=rng.randrange(1,h+1),
                           capture=rng.randrange(399,2*h+10000),
                           edge_remaining=rng.randrange(h+1), edge_level=rng.randrange(2),
                           dirty=rng.randrange(2), accepted=rng.randrange(2))
              for h in (rng.randrange(200,0x1000000) for _ in range(500))]
    for s in cases:
        m.ram[:] = baseline
        m.ram[0x4f1] |= 0x40  # Tap enabled: do not call ADC helper.
        m.ram[0x5a6] = m.ram[0x493]  # No Follow-mode transition.
        for k,(addr,n) in FIELDS.items():
            m.put(addr,getattr(s,k),n)
        m.run(0x4b16)
        got = {k:m.u(addr,n) for k,(addr,n) in FIELDS.items()}
        want = asdict(leading_capture_service(s))
        assert got == want, (asdict(s),got,want)
        if s.capture in (398,399,16000,16002,32000) and s.edge_remaining == 4000:
            examples.append(dict(input=asdict(s),output=got))
    summary['leading_capture_cases'] = len(cases)
    print('Leading capture model passed:', len(cases), flush=True)

    adc = [0]
    m.hooks[0x9318] = lambda q: q.put(0x12,adc[0])
    count = 0
    control_examples = []
    for th in ([32+64*i for i in range(16)], [20+3*i*i+15*i for i in range(16)]):
        for previous in (0,49,50,399,400,1023):
            for value in [*range(1024),0xffff]:
                m.ram[:] = baseline
                for i,t in enumerate(th):
                    m.put(0x500+2*i,t)
                m.put(0x44a,previous)
                m.put(0x45,8000,4)
                m.ram[0x5ad] = 0
                adc[0] = value
                m.run(0x627c)
                got = (m.u(0x21,4),m.u(0x44a),bool(m.ram[0x5ad]))
                want = tempo_control(value,previous,8000,th)
                assert got == want, (value,previous,th,got,want)
                count += 1
                if previous == 0 and value in (34,35,64,96,992,993,0xffff):
                    control_examples.append(dict(thresholds=th,adc=value,previous=previous,
                                                 period=got[0],retained_adc=got[1],moved=got[2]))
    summary['tempo_control_cases'] = count
    print('Tempo-control model passed:', count, flush=True)
    count = 0
    th = [32+64*i for i in range(16)]
    for value in (34,35,64,96,992,993):
        for measuring,capture in ((0,0),(1,0),(1,20000)):
            m.ram[:] = baseline
            for i,t in enumerate(th):
                m.put(0x500+2*i,t)
            m.put(0x44a,0)
            m.put(0x45,8000,4); m.put(0x41,8000,4); m.put(0x49,4000,4)
            m.ram[0x4f1] &= ~0x40
            m.ram[0x5a4] = 0x40
            m.ram[0x5a6] = m.ram[0x493]
            m.ram[0x481] = measuring
            m.put(0x586,capture,4)
            m.put(0x442,0,4)
            m.ram[0x480] = 0
            adc[0] = value
            m.run(0x4b16)
            candidate = tempo_control(value,0,8000,th)[0]
            want = 10000 if capture else (8000 if measuring else max(200,candidate//2))
            assert m.u(0x45,4) == want, (value,measuring,capture,m.u(0x45,4),want)
            count += 1
    summary['control_capture_arbitration_cases'] = count
    m.hooks.clear()

    # Execute the actual ISR capture prefix: first edge captures zero, next
    # edge captures elapsed; no peripheral clock simulation is implied.
    for level,previous in ((0,0),(0,1),(1,0),(1,1)):
        m.ram[:] = baseline
        m.ram[0xf9e] = 2
        m.ram[0xf80] = level << 4
        m.ram[0x486] = previous
        m.put(0x446,16000,4)
        m.put(0x49,1234,4)
        m.ram[0x47e] = 1
        m.run(0x808,stop=0x8c8)
        assert m.ram[0x486] == level
        if level and not previous:
            assert m.u(0x586,4) == 16000 and m.u(0x442,4) == 1234
            assert m.u(0x446,4) == 0 and m.ram[0x481] == 1 and m.ram[0x480] == 1
        else:
            assert m.u(0x446,4) == 16000
    summary['isr_capture_cases'] = 4

    # Reload ordering applies to master and all six unrolled channel timers.
    count = 0
    for lane,start,stop in ((-1,0x8c8,0x93c),(0,0xac2,0xb02),
                            (1,0xb72,0xbb2),(2,0xc26,0xc66),
                            (3,0xcda,0xd1a),(4,0xd8e,0xdce),(5,0xe42,0xe82)):
        for remaining in (-1,0,1,2,200):
            m.ram[:] = baseline
            m.ram[0xfe0] = 4
            addr = 0x41 if lane == -1 else 0x498+12*lane
            leveladdr = 0x47e if lane == -1 else 0x436+lane
            m.put(addr,333,4); m.put(addr+4,777,4); m.put(addr+8,remaining,4)
            m.ram[leveladdr] = 0
            m.put(0x5f7,100)
            m.run(start,stop=stop)
            expired = remaining <= 1
            assert m.u(addr+8,4) == (333 if expired else remaining-1), (lane,remaining)
            assert m.u(addr,4) == (777 if expired else 333), (lane,remaining)
            assert m.ram[leveladdr] == int(expired)
            count += 1
    summary['isr_reload_cases'] = count

    # Isolate the commit routine's numerical safety guards for each lane.
    count = 0
    for lane in range(6):
        for master in (-1,0,75,76,77,8000):
            for remaining in (-1,0,75,76,724,725,800,875,876):
                m.ram[:] = baseline
                m.ram[0xfe0] = 0
                m.ram[0xb8] = lane
                m.put(0x49,master,4)
                m.put(0x4a0+12*lane,remaining,4)
                m.put(0x49c+12*lane,800,4)
                m.pc = 0x2540
                for _ in range(500):
                    if m.pc in (0x32e8,0x26a6):
                        break
                    m.step()
                else:
                    raise AssertionError('guard fragment did not terminate')
                deferred = master < 76 or remaining < 76 or 725 <= remaining <= 875
                assert (m.pc == 0x32e8) == deferred, (lane,master,remaining,hex(m.pc))
                count += 1
    summary['commit_guard_cases'] = count
    summary['all_assertions_passed'] = True
    summary['limitations'] = [
        'Shared decoder/harness, not independent silicon validation.',
        'No asynchronous interrupts, analog calibration or physical timebase.',
        'Leading model tests Tap enabled with unchanged Follow; not all source arbitration.',
        'ADC wrapper hooked; calibration arrays synthetic, interpolation helpers execute real code.',
        'Commit tests cover numerical guard fragment only, not full history/phase scheduler.'
    ]
    result = dict(summary=summary,leading_examples=examples,tempo_control_examples=control_examples)
    (ROOT/'analysis/timing_validation.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(summary,indent=2))


if __name__ == '__main__':
    main()
