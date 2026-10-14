#!/usr/bin/env python3
"""Validate MP86 deliverable integrity; no hardware or network access.

Runs exact decode, independent demodulator agreement, CRC-corruption rejection,
BIN/HEX/ELF consistency, raw/CSV table equivalence, and H8 algebra checks.
These tests do NOT establish complete DSP or hardware audio equivalence.
"""
from __future__ import annotations
import argparse, csv, hashlib, json, struct, tempfile, wave
from pathlib import Path
from decode_mp86 import decode, PREAMBLE

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1])
    args=parser.parse_args(); p=args.root
    result={'scope':'Offline extraction/artifact validation, not full DSP equivalence','checks':{}}
    checks=result['checks']
    fw,stream,packets,metadata=decode(p/'input/mp86.wav')
    assert fw==(p/'binaries/mimeophon_mp86.bin').read_bytes()
    assert metadata['firmware']['sha256']=='31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9'
    checks['decode_exact_payload_sha256']=True
    checks['crc_valid_packets']=len(packets)
    checks['dft_vs_time_domain_symbol_mismatches']=metadata['transport']['dft_vs_time_domain_symbol_mismatches']
    checks['demodulated_stream_matches_export']=stream==(p/'protocol/demodulated_stream.bin').read_bytes()
    assert checks['demodulated_stream_matches_export']
    records=list(csv.DictReader((p/'protocol/packets.csv').open()))
    assert len(records)==192 and all(r['stored_crc32']==r['computed_crc32'] for r in records)
    checks['packet_csv_rows']=len(records)
    # Negate one full carrier symbol in the FIRST PAYLOAD. Both demodulators
    # should still agree, but the checksum MUST reject the modified packet.
    with wave.open(str(p/'input/mp86.wav'),'rb') as w:
        params=w.getparams(); pcm=bytearray(w.readframes(w.getnframes()))
    sample=packets[0]['wav_sample_offset']+len(PREAMBLE)*32
    for k in range(8):
        val=struct.unpack_from('<h',pcm,2*(sample+k))[0]
        struct.pack_into('<h',pcm,2*(sample+k),-val)
    with tempfile.TemporaryDirectory() as td:
        altered=Path(td)/'corrupted.wav'
        with wave.open(str(altered),'wb') as w:w.setparams(params);w.writeframes(pcm)
        try: decode(altered)
        except ValueError as exc:
            assert 'CRC mismatch' in str(exc),str(exc)
            checks['deliberate_payload_corruption_rejected_by_crc']=True
        else:raise AssertionError('Corrupted packet was not rejected')
    # Rebuild the BIN independently from Intel HEX records.
    output=bytearray(len(fw)); populated=set(); upper=0
    for line in (p/'binaries/mimeophon_mp86.hex').read_text().splitlines():
        raw=bytes.fromhex(line[1:]); assert sum(raw)%256==0
        n=raw[0]; addr=int.from_bytes(raw[1:3],'big'); typ=raw[3]; body=raw[4:-1]
        assert len(body)==n
        if typ==4: upper=int.from_bytes(body,'big')<<16
        elif typ==0:
            off=upper+addr-0x08020000
            assert 0<=off<=len(fw)-n
            output[off:off+n]=body; populated.update(range(off,off+n))
        elif typ==5: assert int.from_bytes(body,'big')==0x08020299
        elif typ==1: assert n==0
        else: raise AssertionError('Unexpected HEX record type')
    assert bytes(output)==fw and len(populated)==len(fw)
    checks['intel_hex_exact_full_coverage_and_checksums']=True
    elf=(p/'binaries/mimeophon_mp86_analysis_wrapper.elf').read_bytes()
    assert elf[:7]==b'\x7fELF\x01\x01\x01'
    phoff=struct.unpack_from('<I',elf,28)[0]
    first=struct.unpack_from('<8I',elf,phoff)
    assert first[0]==1 and first[2]==0x08020000 and first[4]==len(fw)
    assert elf[first[1]:first[1]+first[4]]==fw
    second=struct.unpack_from('<8I',elf,phoff+32)
    assert second[2]==0x20000000 and second[3]==0x0802be08
    assert elf[second[1]:second[1]+second[4]]==fw[0xbe08:0xbf7c]
    checks['synthetic_elf_flash_and_initializer_exact']=True
    manifest=json.loads((p/'tables/table_manifest.json').read_text())
    for t in manifest:
        name=t['name']; off=int(t['offset'],16); n=t['count']
        raw=(p/f'tables/{name}.f32le.bin').read_bytes()
        assert raw==fw[off:off+4*n]
        assert hashlib.sha256(raw).hexdigest()==t['sha256']
        rows=list(csv.DictReader((p/f'tables/{name}.csv').open()))
        rebuilt=b''.join(struct.pack('<f',float(r['float32_exact_decimal'])) for r in rows)
        assert rebuilt==raw
    checks['flash_tables_raw_and_csv_bit_exact']=len(manifest)
    H=[[1 if (i&j).bit_count()%2==0 else -1 for j in range(8)] for i in range(8)]
    for i in range(8):
        for j in range(8):assert sum(H[i][k]*H[j][k] for k in range(8))==(8 if i==j else 0)
    checks['hadamard_H_times_transpose_equals_8I']=True
    result['not_tested']=['full ARM execution','complete DSP equivalence','physical module audio','Ghidra script runtime','official archive byte comparison']
    (p/'analysis/validation_results.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))

if __name__=='__main__':main()
