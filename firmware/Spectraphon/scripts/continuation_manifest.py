"""Generate/check the current bundle manifest; preserve the initial manifest."""
import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
MANIFEST=ROOT/'SHA256SUMS.continuation.txt'


def contents():
    lines=[]
    for path in sorted(ROOT.rglob('*')):
        if not path.is_file() or path==MANIFEST or '__pycache__' in path.parts or path.suffix=='.pyc':
            continue
        lines.append(hashlib.sha256(path.read_bytes()).hexdigest()+'  '+path.relative_to(ROOT).as_posix())
    return '\n'.join(lines)+'\n'


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--check',action='store_true')
    args=parser.parse_args();expected=contents()
    if args.check:
        if not MANIFEST.exists() or MANIFEST.read_text()!=expected:raise SystemExit('Manifest differs; inspect changes before regenerating')
        print('Continuation manifest verified')
    else:
        MANIFEST.write_text(expected,encoding='utf-8',newline='\n')
        print(f'Wrote {len(expected.splitlines())} file hashes')
