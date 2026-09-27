# Morphagene MG204 — extended engineering bundle

Start with [REPORT.md](REPORT.md), or the formatted [REPORT.html](REPORT.html).
[CODEX_HANDOFF.md](CODEX_HANDOFF.md) identifies the reliable components and remaining work.

The original uploaded files are preserved. This extension independently recovers the
164,864-byte application from all **644 CRC-valid packets**, exports **14 table
blocks / 8,056 words**, supplies instruction evidence, and corrects several earlier
interpretations. It contains selected tested components, not a complete emulator.

**Use `binaries/morphagene_mg204.bin` for disassembly.** The older top-level
`mg204_firmware_image.bin` still includes CRCs, padding and sync markers. It is
180,128 bytes of transport records, despite its filename. The original nested
`Morphagene_MG204_RE_bundle/mg204_flash_08020000.bin` is the correct image too.

| File / directory | Purpose |
|---|---|
| `REPORT.md`, `REPORT.html` | Findings, corrections, address evidence and limitations |
| `protocol/transport.json`, `protocol/packets.csv` | WAV identity and all packet offsets, times, CRCs and hashes |
| `binaries/` | Exact BIN, lossless HEX, synthetic analysis ELF, startup RAM bytes |
| `tables/table_manifest.json` | Extents, hashes, consumers and confidence for each table |
| `tables/morph_active_stages.csv` | Exact stored values alongside nominal rational descriptions |
| `disassembly/reachable_thumb.asm` | Recursive Thumb listing with literal references |
| `disassembly/*read.asm`, `record_routing.asm` | Focused evidence for newly transcribed audio components |
| `analysis/control_curves.csv` | All 4,096 ADC cases, including corrected clock quantization |
| `analysis/curated_symbols.csv`, `memory_map.json` | Analyst names and audited memory layout |
| `analysis/validation_results.json` | Artifact checks actually executed |
| `analysis/component_test_results.json` | Native Windows and Linux arithmetic test results |
| `reconstruction/` | Exact C++17 tables and selected DSP/control components |
| `tests/` | Arithmetic fixtures and standalone C++ checks |
| `MANIFEST.sha256` | Bundle integrity; excludes itself and transient caches |
| `Morphagene_MG204_RE_bundle/` | Original report, decoder and unedited decompiler material |

## Reproduce

From this directory, Python 3.10+:

```sh
python -m pip install -r tools/requirements.txt
python tools/recover_transport.py
python tools/extract_artifacts.py
python tools/reference_models.py
python tools/validate_bundle.py
```

The preserved `mg204_firmware.zip` is the input. No new download is required.
`recover_transport.py --out reproduced` writes a separate transport/BIN recovery.
The other exporters write into this bundle. Export offsets are locked to the
audited flash SHA-256 and must be changed for another firmware revision.

Optional disassembly regeneration uses LLVM with ARM support. LLVM 15 under WSL
was used in this pass; set `LLVM_LIBRARY` to a shared-library path if autodetection
does not find it. The delivered assembly and JSON can be read without LLVM.

```sh
python tools/build_analysis.py
python tools/read_listing.py 2ab6c 2abda
```

Compile the selected-component checks with GCC/Clang and a correctly rounded
`std::fma(float,float,float)` implementation:

```sh
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffp-contract=off tests/test_components.cpp -o test_components
./test_components .
```

Native MINGW64 GCC 16.1 on the analysis machine required **`-mfma`** to pass the
bit checks: its default library FMA path differed by one ULP on a supplied kernel
case. Use that flag only on an FMA-capable x86 host. Linux GCC 11.4 passed without
it. Preserve the distinction between separate multiplies and explicit fused
operations; fast-math is unsuitable for these reference checks.

After regenerating content, rebuild the HTML and seal the manifest:

```sh
python tools/finalize_bundle.py
python tools/validate_bundle.py --manifest
```

`analysis/component_test_results.json` is an execution record, not a claim that
the exporter reruns a compiler. Update it only after actually rerunning the tests.

## Import parameters

Raw little-endian ARM Thumb image at `0x08020000`; reset code at `0x08021414`
(vector word `0x08021415` includes the Thumb flag). The Cortex-M4 target used by
LLVM decodes the observed Thumb-2/VFP instructions; this is not proof of an exact
MCU part number. The ELF is an analyst-created container, not the original linker
output. The full image, including 448 trailing zero bytes, is retained.
