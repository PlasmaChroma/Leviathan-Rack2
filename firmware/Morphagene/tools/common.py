"""Shared, version-locked MG204 extraction helpers (Python standard library)."""
import csv, hashlib, json, struct
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
BASE = 0x08020000
FLASH_HASH = '44037eb2cf24b5fc411135e487a6fa226498a8478db981659bb53341cb967609'
WAV_HASH = 'dc6cb0731ef21725c83c6d24c7cd2e98ad7c6ffccc55071ec3bf0c8e318894a8'
ZIP_HASH = 'a653d8c67f8061fe6b2ffaf6a978584c02a8219eda2a0b690ca0f1272d380a36'
def sha(data): return hashlib.sha256(data).hexdigest()
def flash():
    b=(ROOT/'Morphagene_MG204_RE_bundle/mg204_flash_08020000.bin').read_bytes()
    if sha(b)!=FLASH_HASH: raise ValueError('Not the audited MG204 image; offsets are version-specific')
    return b
def put_json(path,data):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(json.dumps(data,indent=2,allow_nan=False)+'\n',encoding='utf-8')
def put_csv(path,fields,rows):
    path.parent.mkdir(parents=True,exist_ok=True)
    with path.open('w',newline='',encoding='utf-8') as f:
        w=csv.writer(f);w.writerow(fields);w.writerows(rows)
def f32(x): return struct.unpack('<f',struct.pack('<f',x))[0]
def bits(x): return struct.unpack('<I',struct.pack('<f',x))[0]
def frombits(x): return struct.unpack('<f',struct.pack('<I',x))[0]
