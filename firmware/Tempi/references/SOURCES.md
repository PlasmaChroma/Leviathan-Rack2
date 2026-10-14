# Sources and provenance

## Primary analyzed material

**User-supplied WAV:** originally mounted as `tempi71(1).wav`, retained as `input/tempi71.wav`. SHA-256: `dfea58ec1694b63f645dc85af62d0971077942b281ae445c04ebaa2bebcbd09c`.

All instruction addresses, recovered data tables, exact transport rules, state-memory layouts, parser bytes, and executed-routine results originate from this file. Firmware bytes remain the manufacturer's material; the package does not assign an open-source license to them.

## Public primary references inspected on 27 September 2026

1. **Make Noise — Tempi product page**  
   https://www.makenoisemusic.com/modules/tempi/  
   Used for product identity, feature context, and the public firmware-71 link. It is not a source for the exact recovered GPIOs or algorithms.

2. **Make Noise — English Tempi manual PDF**  
   https://www.makenoise-manuals.com/tempi/tempi-manual.pdf  
   34 pages. The inspected manual identifies itself as firmware-60 documentation. Panel pages, factory-setting pages, state-CV range, nominal output mode descriptions, and Select Bus direction were used as cross-checks. Panel and factory-setting pages were also inspected visually. Version differences are not silently resolved in favor of the older manual.

3. **Microchip — PIC18(L)F2X/4XK22 data sheet, DS40001412G**  
   https://ww1.microchip.com/downloads/en/DeviceDoc/40001412G.pdf  
   Used for the candidate-family memory/peripheral model, legacy instruction encodings, timer/UART relationships, and device-family distinctions. The instruction table was inspected visually in addition to parsed text. The data sheet does not identify the chip fitted to a Tempi.

4. **gputils — p18f46k22.inc**  
   https://raw.githubusercontent.com/ilovezfs/gputils/master/gputils/header/p18f46k22.inc  
   Used as a primary open-source cross-check for register addresses and fuse-field interpretation. Symbol names in the custom decoder follow the candidate device family; this is not a chip readback.

The URLs are reference pointers; no third-party manual PDFs or repository snapshots are bundled. Internet access from the execution container was unavailable, so binaries from external URLs were not silently imported.

## Lookup results that did not establish a fact

The official update archive was linked by the product page but was not independently downloaded and hash-compared. The input's version label is therefore corroborated by naming and product context, not vendor-file identity verification.

Public searches for Tempi/PIC18 source and exact MCU identification did not locate a primary source confirming the installed part or supplying original source code. Printable-byte scanning did not produce an authenticated source-symbol or compiler-version table. The exact MCU/package and compiler remain unconfirmed.

No forum assertion, unrelated Select Bus implementation, or another Make Noise module's transport was substituted for evidence from this WAV.
