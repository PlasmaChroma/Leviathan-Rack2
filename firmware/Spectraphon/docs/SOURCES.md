# Sources and provenance

## Primary evidence: uploaded firmware

The user's `sp67.dat` is the primary source for the recovered addresses, constants, tables, memory organization and translated equations. Its SHA-256 is `b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9`. Every flash address in this bundle uses base `0x08020000`. The raw input is preserved unchanged as `firmware/sp67.bin`.

The byte-level evidence is not supplied by the external pages below. Public documentation is used to constrain semantics and check architecture, not to manufacture exact DSP equations that the manual does not state.

## External sources consulted

### S1 — Make Noise product page

https://www.makenoisemusic.com/modules/spectraphon/

Manufacturer source for module identity, dual SAM/SAO architecture, interface counts, and relationships between FM, Sine/Sub and Follow/Sync. Accessed 28 September 2026 and rechecked 4 October 2026. No claim about current retail price is used.

### S2 — Make Noise firmware listing

https://www.makenoisemusic.com/firmware/

Manufacturer listing identifies SP67 as a Spectraphon firmware download at access time. This listing is not a hash verification of the uploaded image. The upload's identity is verified internally, not by claiming a downloaded vendor file was compared byte-for-byte.

### S3 — Make Noise Spectraphon cheat sheet

https://www.makenoise-manuals.com/spectraphon/spectraphon-cheat-sheet.pdf

Manufacturer's three-page reference for mode-dependent control roles and UI gestures. The mode matrix on page 3 was visually checked in the initial pass and its extracted text rechecked 4 October 2026: Noise Slide is low-pass and Focus high-pass; Chaos Slide is feedback and Focus ratio. This is a concise semantic cross-check, not an engineering specification. The full current manual PDF could not be reliably retrieved through the available web path; this bundle does not claim to have exhaustively checked it.

### S4 — STMicroelectronics STM32H743/753 family description

https://www.st.com/en/microcontrollers-microprocessors/stm32h743-753.html

Primary source for Cortex-M7 family features, including double-precision floating point. It supports the architecture-family interpretation, not the exact chip marking on the user's module.

### S5 — STMicroelectronics official Cortex-M7 device startup source

https://raw.githubusercontent.com/STMicroelectronics/cmsis-device-h7/master/Source/Templates/gcc/startup_stm32h743xx.s

Primary device source used to compare vector ordering and handler names. A current startup template need not have identical initialization ordering to this compiled firmware. No specific compiler/HAL version is inferred from superficial startup similarity.

### S6 — ARMv7-M Architecture Reference Manual, DDI 0403E.b

ARM-authored document, hosted by an academic mirror:

https://www.profdong.com/elc4438_spring2016/DDI0403E_B_armv7m_arm.pdf

Section A7.7.231 (printed pages A7-517 onward) supplies fused multiply-add/subtract sign conventions relevant to the recurrence. In particular, VFNMS computes product minus the old destination. The document is used for instruction semantics, not to assert that the STM32H7 only implements the older minimum feature set described in that manual.

## Additional manufacturer documentation

### S7 — Make Noise manual: Buttons and Display

https://www.makenoise-manuals.com/spectraphon/spectraphon-manual-buttons.html

Manufacturer-linked HTML manual, accessed 4 October 2026. Supplies the off,
steady Follow and flashing Sync indicator semantics used to name the raw
states after checking the original LED routine. It does not supply GPIO pins,
counter timing or the compiled state machine.

### S8 — Make Noise manual: Frequency and Partials Controls

https://makenoise-manuals.com/spectraphon/spectraphon-manual-frequency.html

Manufacturer-linked HTML manual, accessed 4 October 2026. Cross-checks Follow's
linked B pitch and Sync's additional spectral synchronization, and supplies
tuning-beacon color semantics. Exact ratio windows, retained pin states and
DSP formulas remain firmware-derived. The product page links to this manual
site; these HTML sections are accessible even though the earlier full-PDF
retrieval was unreliable.

## Negative provenance statements

No vendor source code, debug ELF, bootloader dump, board schematic, physical chip marking, hardware-generated Array file, hardware recording, second firmware revision or original compiler map was available for this pass. No successful Ghidra/IDA decompilation is represented. The 4 October continuation does execute bounded original ARM instruction paths using Unicorn 2.1.4; exact inputs, address coverage and limitations are in the differential suite and reconstruction guide. This is not whole-board emulation or a hardware-audio comparison.

The factory spectra and lookup tables are directly extracted vendor data from the user-provided firmware. The analysis scripts, reports and reference probes are newly authored reconstruction work. Inclusion in a research bundle is not an assertion of redistribution permission for vendor assets.
