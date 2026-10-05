#!/usr/bin/env python3
"""Verify supplied-archive evidence without executing any manufacturer code."""
from pathlib import Path
import argparse, hashlib, json, sys

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--archive', type=Path, help='Optional original .gz file to verify too')
    args = p.parse_args()
    root = Path(__file__).resolve().parents[1]
    manifest = json.loads((root/'evidence/manifest.json').read_text())
    errors = []
    for entry in manifest['files']:
        target = root/'extracted'/entry['path']
        if not target.is_file() or target.is_symlink():
            errors.append(f"Missing/nonregular: {entry['path']}"); continue
        data = target.read_bytes()
        if len(data) != entry['size'] or hashlib.sha256(data).hexdigest() != entry['sha256']:
            errors.append(f"Hash/size mismatch: {entry['path']}")
    if args.archive:
        if hashlib.sha256(args.archive.read_bytes()).hexdigest() != manifest['sha256']:
            errors.append('Original archive hash mismatch')
    print(json.dumps({'checked_files':len(manifest['files']), 'errors':errors}, indent=2))
    return int(bool(errors))
if __name__ == '__main__':
    sys.exit(main())
