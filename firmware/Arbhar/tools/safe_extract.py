#!/usr/bin/env python3
"""Extract this supplied archive as inert regular files; never run an updater.
Usage: python tools/safe_extract.py INPUT.gz NEW_OUTPUT_DIRECTORY
The output directory must not already exist. Symlinks, links, special files,
absolute paths, traversal, duplicate filenames, and oversized input are rejected.
"""
from pathlib import Path, PurePosixPath
import argparse, hashlib, json, os, tarfile

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', type=Path)
    parser.add_argument('destination', type=Path)
    args = parser.parse_args()
    expected = '1414f35d8d0e3ca8d4cb0871a8c61f3957001caf3b106d30df0514ca9f404637'
    digest = hashlib.sha256(args.archive.read_bytes()).hexdigest()
    if digest != expected:
        raise ValueError('Archive does not match the analyzed input SHA-256')
    if args.destination.exists():
        raise FileExistsError('Choose a new, empty destination path')
    with tarfile.open(args.archive, 'r:gz') as archive:
        members = archive.getmembers()
        total = 0; names = set()
        for m in members:
            path = PurePosixPath(m.name)
            if path.is_absolute() or '..' in path.parts or '\\' in m.name:
                raise ValueError(f'Unsafe archive path: {m.name!r}')
            if not (m.isdir() or m.isfile()):
                raise ValueError(f'Nonregular archive member: {m.name!r}')
            if m.isfile():
                if str(path) in names: raise ValueError('Duplicate file member')
                names.add(str(path)); total += m.size
        if total > 100_000_000 or len(members) > 2000:
            raise ValueError('Archive exceeds evidence extraction limits')
        args.destination.mkdir(parents=True)
        rows = []
        for m in members:
            out = args.destination / m.name
            if m.isdir():
                out.mkdir(parents=True, exist_ok=True); continue
            out.parent.mkdir(parents=True, exist_ok=True)
            with archive.extractfile(m) as source:
                data = source.read()
            if len(data) != m.size: raise ValueError('Truncated archive member')
            out.write_bytes(data)
            os.chmod(out, 0o644)
            rows.append({'path':m.name,'size':m.size,'sha256':hashlib.sha256(data).hexdigest()})
    print(json.dumps({'archive_sha256':digest,'regular_files':len(rows),'total_bytes':total}, indent=2))
if __name__ == '__main__': main()
