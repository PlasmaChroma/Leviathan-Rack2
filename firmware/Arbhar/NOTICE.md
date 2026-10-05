# Provenance and use notice

The `extracted/` directory contains files unpacked from the firmware archive supplied by the user. Their original authorship and notices remain applicable. The original package includes a Sebastian Lexer copyright notice. Inclusion in this research bundle does **not** grant a new right to distribute manufacturer firmware, source, tables, branding, sound assets, or other protected material in a product.

Generated disassembly, extracted numeric tables, and the exact float-literal reference header are derived from that supplied firmware. The narrative analysis, proposed native architecture, and scalar re-expressions are research work based on that evidence. This is not a claim of clean-room separation, official endorsement, or a release authorization.

The original scripts include privileged operating-system and storage operations. **Do not execute the files in `extracted/` on a development workstation.** The supplied analysis scripts inspect them as inert text or bytes. No manufacturer program was run to produce the reported results; only native research tests and a restricted instruction-text interpreter were executed.

The uploaded `.gz` archive itself is not duplicated in the deliverable ZIP. Its hash and all extracted regular-file hashes are recorded. Font files are not included.
