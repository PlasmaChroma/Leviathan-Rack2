#!/usr/bin/env python3
"""Check document structure, mathematical anchors and exact package hashes.

Run from any directory. --refresh-checksums updates hashes after intentional edits.
This does not run module acceptance tests or establish hardware equivalence.
"""
from __future__ import annotations

import argparse
import ast
import hashlib
import json
from pathlib import Path
import re
import sys

sys.dont_write_bytecode = True
from generate_reference_vectors import build_vectors, check_equivalent

ROOT = Path(__file__).resolve().parent
BRIEF_HASH = '9cd32340a3781acd1a4d720214f0199e226121cf3c84aecd8754b0a3a3524407'
FILES = (
    'ACCEPTANCE_TESTS.md', 'CODEX_START.md', 'IMPLEMENTATION_PLAN.md',
    'INTEGRATION_BASELINE.md', 'Leviathan_Morphagene_Codex_Spec.md', 'Morphagene_Tech_Brief.md',
    'PACKAGE_VALIDATION.md', 'README.md', 'REVIEW_NOTES.md',
    'generate_reference_vectors.py', 'reference_vectors.json', 'validate_package.py',
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def validate(refresh: bool) -> None:
    contents = {name: (ROOT / name).read_text(encoding='utf-8') for name in FILES}
    brief = (ROOT / 'Morphagene_Tech_Brief.md').read_bytes()
    require(hashlib.sha256(brief).hexdigest() == BRIEF_HASH, 'Source brief changed')
    original = ROOT.parent / 'Morphagene_Tech_Brief.md'
    if original.exists():
        require(original.read_bytes() == brief, 'Source brief differs from repository original')

    ids = re.findall(r'^\| ([A-Z]+-\d{3}) \|', contents['ACCEPTANCE_TESTS.md'], re.M)
    require(len(ids) == len(set(ids)), 'Duplicate acceptance IDs')
    families: dict[str, list[int]] = {}
    for case in ids:
        family, number = case.split('-')
        families.setdefault(family, []).append(int(number))
    for family, numbers in families.items():
        require(numbers == list(range(1, len(numbers) + 1)), f'Noncontiguous {family} IDs')
    require(f'{len(ids)} individually identified acceptance cases' in contents['README.md'],
            'README acceptance count is stale')
    phases = re.findall(r'^## Phase (\d+) ', contents['IMPLEMENTATION_PLAN.md'], re.M)
    require(phases == [str(i) for i in range(11)], 'Plan phases must be 0..10')

    json_count = 0
    for name, content in contents.items():
        if name.endswith('.py'):
            ast.parse(content, filename=name)
        if not name.endswith('.md') or name == 'Morphagene_Tech_Brief.md':
            continue
        opened = None
        for line in content.splitlines():
            fence = re.match(r'^\s*(`{3,}|~{3,})(.*)$', line)
            if fence:
                token = fence[1]
                if opened is None:
                    opened = token
                elif token[0] == opened[0] and len(token) >= len(opened) and not fence[2].strip():
                    opened = None
        require(opened is None, f'Unclosed Markdown fence: {name}')
        for example in re.findall(r'^```json\s*\n(.*?)^```\s*$', content, re.M | re.S):
            json.loads(example)
            json_count += 1

    check_equivalent(build_vectors(), json.loads(contents['reference_vectors.json']))
    # Ensure exact integer/boolean anchors cannot silently pass as other JSON types.
    for expected, invalid in [(1, True), (True, 1), (1, 1.0), (1.0, True), (1.0, float('nan'))]:
        try:
            check_equivalent(expected, invalid)
        except ValueError:
            pass
        else:
            raise ValueError(f'Checker accepted invalid type/value: {expected!r}, {invalid!r}')

    manifest = ''.join(f'{hashlib.sha256((ROOT / name).read_bytes()).hexdigest()}  {name}\n'
                       for name in sorted(FILES))
    checksum_path = ROOT / 'SHA256SUMS.txt'
    if refresh:
        checksum_path.write_text(manifest, encoding='utf-8', newline='\n')
    require(checksum_path.read_text(encoding='utf-8') == manifest, 'Package hashes differ; review edits before refreshing')
    print(f'PASS: {len(ids)} unique contiguous acceptance IDs, {len(phases)} phases, '
          f'{json_count} JSON examples, balanced authored fences, Python syntax, '
          f'vector anchors/type checks, unchanged source brief, {len(FILES)} file hashes.')
    print('No module implementation, Rack build, sanitizer, listening or hardware test was run.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--refresh-checksums', action='store_true')
    args = parser.parse_args()
    validate(args.refresh_checksums)
