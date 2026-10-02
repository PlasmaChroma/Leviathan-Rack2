DATA BENDER v1.4.7 — REVERSE ENGINEERING FOR VCV RACK
Research and implementation handoff · 2 October 2026

START HERE
==========
1. Data_Bender_Reverse_Engineering.html is the complete readable report.
   It is self-contained and embeds the original supplied board photograph.
2. Data_Bender_Reverse_Engineering.txt contains the same substantive report
   as plain text, with complete source URLs for coding-assistant use.
3. Rack_Reconstruction_Handoff.txt gives a concrete implementation order,
   evidence navigation, state boundaries, and remaining verification targets.

This is a research dossier and executable reference harness. A finished VCV
Rack plugin or complete hardware emulator is not included.

THE AUTHORITATIVE IMAGE
=======================
upload/Data_Bender_v1_4_7.bin
Size: 108,884 bytes (0x1a954)
SHA-256: 591eb538e2c6ce3024a1dfc1d85d8c7ef466ca5e13290191f2994aae2706f41d
The supplied image matched the official Qu-Bit download byte for byte.
upload/DB-back.png is the unchanged supplied photograph.

Flash addresses begin at 0x08000000. Subtract that base for raw file offsets.
All addresses, table offsets, structure interpretations, and probes in this
bundle refer specifically to the image identified above.

EVIDENCE NAVIGATION
===================
analysis/function_map.tsv
  96 analysis-assigned semantic names and their entry addresses.
analysis/decompiled/functions.tsv
  Index of 497 extracted function candidates and decompilation status.
analysis/decompiled/0800xxxx.c
  Address-indexed Ghidra pseudocode. Not recovered original source.
analysis/decompiled/all_functions.c
  Combined pseudocode for convenient search.
analysis/decompiled/analyzed_instructions.tsv
  Instruction bytes, addresses, decoded instructions and assigned functions.
analysis/static/
  Image inventory, vectors, strings, exact table values and linear listings.
  The linear sweep may decode embedded data as code; prefer the analyzed
  instruction listing and original bytes for disputed locations.
analysis/platform/
  Hardware identification, physical pin map, clocks and audio-driver probes.
analysis/controls/
  Parameter curves, buttons, Shift gestures, gates, clock and persistence.
analysis/buffer_engine/
  Capacity, reader, bank transitions, Macro helpers, Window and Freeze.
analysis/corrupt_dsp/
  Five effects; four independent DSP models; original-code execution harness;
  exact tables, numerical comparison records and short golden vectors.
analysis/output_probe.json
  Isolated original output-path measurements and five reader probes.
research/
  Manufacturer cross-checks and a staged physical measurement plan.
manifest.json
  SHA-256 and size of every bundled payload file, excluding itself.
validation_results.json
  Packaging, source syntax, table, identity, HTML and recorded-result checks.

REPRODUCING THE ANALYSIS
========================
Use Python 3.11 or newer. Install the versions in requirements.txt in a local
virtual environment. Commands below assume the extracted bundle root.
Scripts use relative paths and do not require a network connection to run.
They write fresh JSON results beside the corresponding scripts.

  python analysis/reproduce_static.py
  python analysis/platform/extract_platform.py
  python analysis/platform/emulate_audio_config.py
  python analysis/controls/probe_controls.py
  python analysis/controls/probe_clock.py
  python analysis/buffer_engine/probe_init.py
  python analysis/buffer_engine/probe_helpers.py
  python analysis/buffer_engine/probe_engine.py
  python analysis/buffer_engine/probe_window_freeze.py
  python analysis/buffer_engine/probe_silence_limits.py
  python analysis/corrupt_dsp/verify_reconstructions.py
  python analysis/corrupt_dsp/emulate_corrupt.py
  python analysis/probe_output.py
  python analysis/make_analysis_elf.py

The four-effect differential test uses the original Corrupt initializer and
original libm code. It compares packed float32 bytes as well as numeric error.
Decimate, Destroy and Vinyl matched byte for byte in all 16 tested amounts
per mode; DJ Filter's maximum absolute difference was 1.1920928955078125e-7.
Each case uses 128 stereo frames and a fresh stated initialization.
This is short component coverage, not a claim of all-input or long-run proof.

Other probes state their isolation boundaries in code and result metadata.
Hardware, allocation, deterministic random values, host expf/powf equivalents,
or unrelated DSP stages may be substituted. Do not describe every probe as
unmodified whole-firmware execution. No physical board measurements were made.

GHIDRA AND THE ANALYSIS ELF
===========================
analysis/Data_Bender_v1_4_7_ANALYSIS.elf is a SYNTHETIC analysis wrapper.
It preserves the uploaded image bytes, maps flash/RAM, and adds semantic
symbols. It is not an original manufacturer ELF or a flashable update.

analysis/ghidra_project/DataBenderFpv5.gpr and DataBenderFpv5.rep belong together.
The project was created with Ghidra 11.0.3. Keep the .rep directory beside the
.gpr marker when opening the project. Ghidra binaries and Java are not bundled.

The Ghidra language selected was ARM:LE:32:v8T, a decoder superset used to
recognize this Cortex-M7 image's FPv5 instructions. The hardware remains
ARMv7E-M/Cortex-M7; this setting is not evidence of an ARMv8 CPU. The plain
Cortex decoder initially missed important floating-point instructions.
Even the improved decompilation can misrepresent conditional floating selects
or hard-float prototypes. Prefer instructions and executed observations when
the generated C disagrees with a probe.

analysis/tools/ contains the three Ghidra setup, naming and export scripts.
To repeat the import with Ghidra 11.0.3, import the BIN as raw binary at
0x08000000, choose the above language, run SetupDataBender.java before auto
analysis, then NameDataBender.java with analysis/function_map.tsv as its first
argument, followed by ExportDataBender.java with an output directory argument.
The analyzed project and static exports are already present for immediate use.

IMPORTANT IMPLEMENTATION DISTINCTIONS
=====================================
The DSP initialization constant is 96,028.0; the original audio driver selects
nominal 48,000 frames/s. The clock-register calculation predicts approximately
48,014.323 frames/s under the stated crystal assumption. This last value was
not physically measured. Keep host rate, renderer rate, F_init, block cadence
and clock event domains distinct until a hardware measurement settles them.

The active capacity is 3,601,050 float frames per channel, with two alternating
capture banks plus a history tail sharing each plane. It is not the larger
outer metadata count. Freeze protects current banks while history can continue.

The supplied models express the recovered digital algorithms. Board voltage
scaling, analog frequency response, natural startup random state, and complete
integrated gesture timing remain distinct verification targets.

SOURCE HANDLING
================
The report and sources/Source_Register.txt link official documentation and
upstream implementation comparisons. Exact retrieved DSP source URLs and
hashes are retained in analysis/corrupt_dsp/upstream_sources.json.
Manufacturer manuals and downloaded third-party implementation trees are not
duplicated here. The uploaded BIN and original photo are included unchanged.
