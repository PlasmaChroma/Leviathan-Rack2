"""Verify exact artifacts, transport defenses and independently parsed containers.

Run from any directory. --manifest verifies the sealed bundle manifest as well.
The C++ component test is run separately; this tool never reports it as executed.
"""
import argparse,csv,json,re,struct,sys,zipfile
from common import *
from recover_transport import demodulate,deframe
def run(manifest=False):
    checks=[]
    def check(ok,name):
        if not ok:raise AssertionError(name)
        checks.append(name)
    b=flash();check(len(b)==164864,'Audited source image size and SHA-256')
    check((ROOT/'binaries/morphagene_mg204.bin').read_bytes()==b,'Canonical BIN equals supplied deframed BIN')
    with zipfile.ZipFile(ROOT/'mg204_firmware.zip') as z:wav=z.read('mg204.wav')
    check(sha((ROOT/'mg204_firmware.zip').read_bytes())==ZIP_HASH,'Original firmware ZIP hash')
    check(sha(wav)==WAV_HASH,'Original WAV hash')
    raw,meta=demodulate(wav);decoded,packets,framing=deframe(raw,meta['carrier_start_sample'])
    check(decoded==b and len(packets)==644,'Fresh WAV demodulation and all 644 packet CRCs')
    check(raw==(ROOT/'protocol/demodulated_stream.bin').read_bytes(),'Complete preserved symbol stream')
    check(raw[1508:framing['payload_stream_end']]==(ROOT/'mg204_qpsk_decoded.bin').read_bytes(),'Legacy top-level decoded BIN is sync plus transport, not flash')
    check(raw[1516:framing['payload_stream_end']]==(ROOT/'mg204_firmware_image.bin').read_bytes(),'Legacy firmware_image BIN still contains framing')
    for name,where in [('payload bit flip',1516+17),('CRC bit flip',1516+256),('padding bit flip',1516+260),('sync bit flip',1516+268),('trailer bit flip',len(raw)-1)]:
        bad=bytearray(raw);bad[where]^=1
        try:deframe(bytes(bad))
        except ValueError:pass
        else:raise AssertionError('Failed to reject '+name)
        check(True,'Reject '+name)
    with (ROOT/'protocol/packets.csv').open() as f:rows=list(csv.DictReader(f))
    check(len(rows)==644 and all(int(r['stream_payload_offset'])==p[1] and r['payload_sha256']==p[-1] and r['stored_crc32_be']==p[-3] for r,p in zip(rows,packets)), 'Packet CSV offsets, CRCs and hashes')
    tm=json.loads((ROOT/'tables/table_manifest.json').read_text())
    header=(ROOT/'reconstruction/exact_tables.hpp').read_text()
    for t in tm['tables']:
        data=(ROOT/f"tables/{t['name']}.{t['type']}.bin").read_bytes();off=t['image_offset']
        check(data==b[off:off+t['count']*4] and sha(data)==t['sha256'],'Exact table '+t['name'])
        with (ROOT/f"tables/{t['name']}.csv").open() as f:rows=list(csv.DictReader(f))
        fmt='<f' if t['type']=='f32le' else '<I'
        rebuilt=b''.join(struct.pack(fmt,float(r['value']) if fmt=='<f' else int(r['value'])) for r in rows)
        check(rebuilt==data,'CSV round trip '+t['name'])
        literals=re.search(r'\b'+t['name']+r'\s*\{\{(.*?)\}\};',header,re.S).group(1)
        values=[s.strip() for s in literals.split(',') if s.strip()]
        rebuilt=b''.join(struct.pack(fmt,float.fromhex(v[:-1]) if fmt=='<f' else int(v[:-1])) for v in values)
        check(rebuilt==data,'C++ hexadecimal table round trip '+t['name'])
    memory={};upper=0;entry=None;ended=False
    for line in (ROOT/'binaries/morphagene_mg204.hex').read_text().splitlines():
        v=bytes.fromhex(line[1:]);check(sum(v)%256==0,'HEX checksum')
        n=v[0];a=int.from_bytes(v[1:3],'big');kind=v[3];data=v[4:-1]
        if len(data)!=n:raise AssertionError('HEX record length')
        if kind==4:upper=int.from_bytes(data,'big')<<16
        elif kind==0:
            for i,x in enumerate(data):
                if upper+a+i in memory:raise AssertionError('Overlapping HEX data')
                memory[upper+a+i]=x
        elif kind==5:entry=int.from_bytes(data,'big')
        elif kind==1:ended=True
    check(ended and entry==0x08021415 and sorted(memory)==list(range(BASE,BASE+len(b))) and bytes(memory[a] for a in sorted(memory))==b,'Independent HEX reconstruction')
    elf=(ROOT/'binaries/morphagene_mg204_analysis_wrapper.elf').read_bytes()
    h=struct.unpack_from('<16sHHIIIIIHHHHHH',elf);check(h[0][:7]==b'\x7fELF\x01\x01\x01' and h[2]==40,'ELF32 little-endian ARM header')
    ph=[struct.unpack_from('<8I',elf,h[5]+i*h[9]) for i in range(h[10])]
    p=ph[0];check(p[2]==BASE and elf[p[1]:p[1]+p[4]]==b,'ELF flash LOAD preserves every byte')
    p=ph[1];check(p[2:6]==(0x20000000,0x08047250,0xff0,0x243c8) and elf[p[1]:p[1]+p[4]]==b[0x27250:0x28240],'ELF startup RAM LOAD')
    check((ROOT/'binaries/preserved_tail.bin').read_bytes()==b[0x28240:],'Preserved post-initializer tail')
    dis=json.loads((ROOT/'analysis/reachable_instructions.json').read_text());occupied=set()
    for d in dis:
        off=d['address']-BASE
        if bytes.fromhex(d['bytes'])!=b[off:off+d['size']]:raise AssertionError('Disassembly byte mismatch')
        for a in range(d['address'],d['address']+d['size']):
            if a in occupied:raise AssertionError('Overlapping decoded instructions')
            occupied.add(a)
    check(True,'All recursive disassembly bytes match image and do not overlap')
    if manifest:
        for line in (ROOT/'MANIFEST.sha256').read_text().splitlines():
            digest,path=line.split('  ',1)
            check(sha((ROOT/path).read_bytes())==digest,'Manifest '+path)
    # Avoid thousands of repetitive checksum entries in the report.
    summary=dict(status='pass',assertions=len(checks),checks=[s for s in checks if s!='HEX checksum'],
        python=sys.version,flash_sha256=FLASH_HASH,decoded_instructions=len(dis),
        scope='Artifact and finite component evidence only; no hardware or CPU-emulator execution')
    if not manifest:put_json(ROOT/'analysis/validation_results.json',summary)
    print(f'PASS: {len(checks)} artifact assertions; fresh 644-packet decode; 14 table and container round trips')
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--manifest',action='store_true');run(p.parse_args().manifest)
