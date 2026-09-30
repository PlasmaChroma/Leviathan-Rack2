#!/usr/bin/env python3
"""Check added P examples/document structure; not a Tempi implementation test."""
from fractions import Fraction as F
import json
import math
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parent.parent


def require(ok, message):
    if not ok:
        raise ValueError(message)


def rate(code):
    return F(code + 4, 4) if code >= 0 else F(4, 4 - code)


def next_rise(h, code, offset, after):
    period = F(2 * h, 1) / rate(code)
    normalized = F(offset) % period
    k = math.floor((F(after) - normalized) / period) + 1
    return normalized + k * period


def check():
    cases = json.loads((ROOT / 'fixtures/review_policy_vectors.json').read_text(encoding='utf-8'))
    count = 0
    for c in cases['rationalEdges']:
        first = next_rise(c['h'], c['r'], c['offset'], c['after'])
        period = F(2*c['h'], 1) / rate(c['r'])
        actual = [math.ceil(first + i*period) for i in range(len(c['risingTicks']))]
        require(actual == c['risingTicks'], f'Rational edge example: {c}')
        count += 1
    for c in cases['safeCommit']:
        actual = math.ceil(next_rise(c['h'], c['r'], c['offset'], c['commit']))
        require(actual == c['nextRise'], f'Strict commit boundary: {c}')
        count += 1
    for c in cases['humanRatios']:
        step = {100: 4, 50: 2, 25: 1}[c['resolution']]
        codes = [r for r in range(-124, 125) if r % step == 0]
        chosen = min(codes, key=lambda r: (abs(F(c['interval']) - 1/rate(r)), abs(r), r))
        require(chosen == c['code'], f'Human period/tie example: {c}')
        count += 1
    for c in cases['humanPhases']:
        r, h = c['r'], c['h']
        period = 1 / rate(r)
        width = 4-r if r < 0 else 4
        step = {100: 4, 50: 2, 25: 1}[c['resolution']]
        nominal = min(0xffffff, max(200, math.floor(F(h, 1) / rate(r))))

        def distance(p):
            # These small, positive test inputs do not invoke signed32 wrap.
            offset = ((h if r < 0 else nominal) * p * 2) // 4
            delta = (F(c['masterTap']) - F(offset, 2*h)) % period
            return min(delta, period-delta), p

        require(min(range(0, width, step), key=distance) == c['phase'], f'Human phase tie: {c}')
        count += 1
    for c in cases['runDisplacement']:
        displacement = F(c['startMaster']) - F(c['oldBeta'])
        position = (F(c['nowMaster']) - F(c['newBeta']) - displacement) * F(c['newN'], c['newD'])
        nxt = F(c['newBeta']) + displacement + (math.floor(position)+1)*F(c['newD'], c['newN'])
        require(position == F(c['channelPosition']) and nxt == F(c['nextRiseMaster']),
                f'Destination displacement through Shift: {c}')
        count += 1
    for c in cases['samplePlacement']:
        sample = math.ceil(F(c['tick']*c['sampleRate'], 32000))
        require(sample == c['sample'], f'No early sample edge: {c}')
        count += 1

    spec = (ROOT / 'TEMPI_VCV_RACK_CODEX_SPEC.md').read_text(encoding='utf-8')
    # Verify inline navigation links using GitHub-style slugs used by this file.
    headings = re.findall(r'^#{1,6} (.+)$', spec, re.M)
    slugs = {re.sub(r'[^\w\- ]', '', h.lower()).replace(' ', '-') for h in headings}
    for target in re.findall(r'\]\(#([^)]*)\)', spec):
        require(target in slugs, f'Broken navigation anchor: {target}')
    json_blocks = re.findall(r'^```json\s*\n(.*?)^```', spec, re.M | re.S)
    for block in json_blocks:
        json.loads(block)
    ids = re.findall(r'^\| ([A-Z]+-\d+) \|', spec, re.M)
    require(len(ids) == len(set(ids)), 'Duplicate acceptance IDs')
    plan = (ROOT / 'IMPLEMENTATION_PLAN.md').read_text(encoding='utf-8')
    packets = re.findall(r'^\| (T\d+) /', plan, re.M)
    require(packets == [f'T{i:02}' for i in range(13)], 'Work packet sequence incomplete')
    return dict(status='PASS', scope='Draft specification examples and structure only',
                policy_cases=count, acceptance_ids=len(ids), json_blocks=len(json_blocks),
                work_packets=len(packets), navigation_links=len(re.findall(r'\]\(#', spec)))


if __name__ == '__main__':
    print(json.dumps(check(), indent=2))
