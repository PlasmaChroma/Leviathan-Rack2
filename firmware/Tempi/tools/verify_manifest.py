#!/usr/bin/env python3
"""Verify package SHA-256 checksums without external dependencies."""
from pathlib import Path
import hashlib,sys
root=Path(__file__).resolve().parents[1]
failed=[];count=0
for line in (root/'SHA256SUMS.txt').read_text().splitlines():
    if not line.strip():continue
    expected,name=line.split('  ',1);path=root/name
    if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest()!=expected:failed.append(name)
    count+=1
print(f'{count-len(failed)}/{count} files verified')
if failed:print('FAILED:',*failed,sep='\n');sys.exit(1)
