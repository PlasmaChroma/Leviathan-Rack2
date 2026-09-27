# Mimeophon MP86 extraction bundle

**The complete transported application was recovered: 49,152 bytes, 192/192 valid CRC-32 packets.**

Start with **REPORT.html** for the readable engineering report, or **REPORT.md** for an editor/agent-friendly version. **CODEX_HANDOFF.md** identifies what is already verified and what still needs tracing.

## Most useful files

| Path | Purpose |
|---|---|
| `binaries/mimeophon_mp86.bin` | Exact recovered firmware, mapped at `0x08020000` |
| `binaries/mimeophon_mp86.hex` | Lossless Intel HEX conversion with addresses |
| `binaries/mimeophon_mp86_analysis_wrapper.elf` | Synthetic analysis ELF with analyst labels; not the original build ELF |
| `protocol/packets.csv` | All 192 packets: WAV sample/time, payload offset, CRC and hash |
| `protocol/transport.json` | Audio format, modem parameters, checksums and image identity |
| `tables/table_manifest.json` | Exact table extents, hashes, interpretations and confidence |
| `tables/*.csv`, `tables/*.f32le.bin` | Exact float tables in readable and binary forms |
| `reconstruction/exact_tables.hpp` | Bit-exact extracted floats as C++17 hexadecimal constants |
| `reconstruction/audited_dsp_components.hpp` | Selected Hadamard/allpass/nonlinear equations; not a complete emulation |
| `disassembly/dsp_core.asm` | Main DSP region around `0x080239b4` |
| `disassembly/audio_callbacks.asm` | Four-frame stereo conversion/callback wrappers |
| `disassembly/reachable_thumb.asm` | Recursive static disassembly with literal references |
| `analysis/memory_map.json` | Flash, startup RAM, delay buffers, absent settings region |
| `analysis/curated_symbols.csv` | Useful named addresses; names supplied by the analyst |
| `analysis/codec_initialization_writes.csv` | Recovered two-byte codec-control writes |
| `analysis/validation_results.json` | Tests actually executed on the deliverables |
| `input/mp86.wav` | Preserved original upload |
| `MANIFEST.sha256` | Checksums for the bundle files |

## Reproduce the recovery

From this directory, with Python 3.10 or later:

```bash
python -m pip install -r tools/requirements.txt
python tools/decode_mp86.py input/mp86.wav --out reproduced
python tools/extract_artifacts.py --root reproduced
python tools/validate_bundle.py --root .
```

The scripts require NumPy for demodulation. The table/HEX/ELF exporter uses only the Python standard library. It checks the audited image hash before using version-specific offsets.

The optional disassembly tools need a system LLVM shared library built with ARM support; LLVM 19 was used here. They are not required to read the supplied disassembly or reproduce the binary extraction.

```bash
python tools/initial_disassembly.py
python tools/build_analysis.py
python tools/snippet.py 39b4 3a60
```

`snippet.py` performs a local linear sweep; begin on a known instruction boundary. Otherwise the first instruction can be decoded from the second half of a Thumb-2 instruction. Prefer the recursive listing for code navigation.

A C++17 sanity test can be compiled locally:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic tests/test_components.cpp -o test_components
./test_components
```

## Ghidra / other disassemblers

Import the raw BIN as little-endian ARM at image base `0x08020000`. Set Thumb context for the code, starting at `0x080201c8`; the reset trampoline's code address is `0x08020298`. The reset vector's low bit is a Thumb-state flag, not an additional byte of address.

Alternatively, import the synthetic ELF, which contains the full raw flash and a startup `.data`/`.bss` mapping. It is an analysis convenience, not evidence of original compiler sections or debug symbols. Review the supplied memory map before adding RAM regions.

`tools/annotate_ghidra.py` is an optional legacy-Jython label importer. It was syntax-reviewed but not executed in a Ghidra runtime here. It requests `analysis/curated_symbols.json` and skips labels in unmapped memory. Do not mistake a successful label import for a validated decompilation.

## Scope and cautions

The extraction is complete for the bytes present in this WAV. It does not include a resident bootloader, a full-device flash dump, recorded audio RAM, or a particular module's stored settings. No hardware execution, flashing, or analog audio measurements were performed.

The protocol is QPSK, not ordinary two-tone FSK. No descrambling was required. All bytes are preserved, including a 132-byte zero tail. The update WAV must not be judged by listening to it through loud speakers or headphones; listening is unnecessary for this analysis.

The original firmware is third-party material recovered from the user's upload. Analyst labels, synthetic containers, and selected mathematical transcriptions are not original source code or a grant of rights to distribute a derivative product. The bundle does not include copied third-party SDK sources.
