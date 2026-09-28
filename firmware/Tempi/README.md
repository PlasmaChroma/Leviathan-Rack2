# Tempi 71 — firmware recovery and engineering dossier

**Recovered successfully: PIC18 application code, not ARM.**

The supplied `tempi71(1).wav` contains an exact digital pulse-width transmission of ASCII Intel HEX. All 2,323 record checksums pass, and the recovered payload plus its pause manifest reconstructs the original PCM sample-for-sample.

Start with **`index.html`**, the self-contained dossier, or **`REPORT.md`** for its Markdown overview. This investigation concerns the supplied file; it is not an official Make Noise specification or a complete hardware emulator.

## Package contents

| Path | Contents |
|---|---|
| `index.html` | Complete readable dossier, including all technical appendices |
| `REPORT.md` | Executive engineering report and evidence summary |
| `docs/` | Transport, I/O, state storage, algorithms, Select Bus, validation, and porting notes |
| `input/tempi71.wav` | Exact copy of the supplied WAV, retained for reproducibility |
| `firmware/tempi71_recovered.hex` | Exact decoded ASCII Intel HEX payload |
| `firmware/tempi71_*.bin` | Raw recovered address segments, plus explicitly marked padded flash image and mask |
| `firmware/SYNTHETIC_*` | Factory EEPROM reconstruction and written-byte mask; **not a physical EEPROM dump** |
| `analysis/disassembly_reachable.asm` | Addressed PIC18 disassembly with analyst-assigned function labels |
| `analysis/functions.csv`, `calls.csv`, `callgraph.dot` | Reverse lookup and navigation indexes |
| `analysis/ram_symbols.csv`, `io_map.csv`, `sfr_xrefs.csv`, `direct_data_xrefs.csv` | RAM, GPIO, register, and instruction cross-references |
| `analysis/*test*`, `validation.json` | Routine test vectors, EEPROM traces, protocol trace, and validation results |
| `transport/` | Sample-to-record map, burst map, and timing manifest |
| `tools/` | Reproducible decoder, PIC18 disassembler, limited instruction harness, models, and extractors |
| `references/SOURCES.md` | Primary references, provenance, and negative lookup findings |
| `SHA256SUMS.txt` | Checksums for the package files, excluding this manifest itself |

## Reproduce

Use Python 3.10 or later. Only WAV recovery requires NumPy; the disassembler, harness, routine tests, and metadata exports use the standard library. From this directory:

```sh
python -m pip install -r requirements.txt
python tools/recover_wav.py
python tools/analyze_code.py
python tools/validate_and_extract.py
python tools/export_metadata.py
```

The decoder also accepts another input path and `--output DIRECTORY`. It is deliberately strict for this exact, clean, digitally generated waveform. It is **not** a general decoder for noisy recordings, resampled WAVs, or other Make Noise products.

To inspect a future EEPROM dump without modifying it:

```sh
python tools/inspect_eeprom.py firmware/SYNTHETIC_factory_eeprom.bin
```

To verify the delivered package:

```sh
python tools/verify_manifest.py
```

## Important boundaries

The first 2 KB of flash—the resident bootloader—are absent. Unrecovered flash bytes are not known to be erased, even where a convenience image contains `FF`. The update's configuration records are not a hardware fuse readback. Actual oscillator frequency, physical chip/package, board analog circuitry, and per-unit calibration have not been measured. Do not use this package as a blank-chip programming image.
