# Package validation — document revision 4

Date: 2026-09-23. Scope: the reviewed documentation package and offline mathematical artifacts.

Run from the repository root (the native Python used for this review is shown):

```powershell
& C:/msys64/mingw64/bin/python.exe doc/Morphagene-Codex/generate_reference_vectors.py --check doc/Morphagene-Codex/reference_vectors.json
& C:/msys64/mingw64/bin/python.exe doc/Morphagene-Codex/validate_package.py
git diff --check
```

Checks executed for revision 4:

- Source brief unchanged, byte-for-byte equal to `doc/Morphagene_Tech_Brief.md`; SHA-256 `9cd32340a3781acd1a4d720214f0199e226121cf3c84aecd8754b0a3a3524407`.
- 138 unique acceptance IDs, contiguous within every family; original 125 IDs preserved.
- 11 implementation phases, numbered 0 through 10.
- Balanced code fences in authored Markdown (source brief intentionally preserved).
- Both JSON examples parse successfully.
- Both Python scripts parse; reference generator/checker and package validator execute with the standard library.
- Mathematical anchors regenerate and compare successfully, including the corrected asymmetric energy-follower sine/step expectations.
- Vector checker rejects boolean/integer type substitutions and non-finite float comparisons.
- Exact-byte SHA-256 manifest covers all 12 fixed package files other than the checksum manifest itself. The live `IMPLEMENTATION_STATUS.md` is intentionally excluded.
- Working diff passes whitespace validation.
- Chimera identity is consistent across proposed registration, source/assets, namespace, semantic capability and reference-vector schema; source-hardware references remain Morphagene.

`validate_package.py --refresh-checksums` refreshes the manifest after reviewed edits and performs the same structural/vector checks. Run the normal validator afterward to verify the saved manifest.

## Limits

These checks validate document structure and the supplied reference math, not a DSP implementation. None of the 138 module acceptance cases has run against an implemented module. No plugin build, live Rack session, sanitizer, GUI/performance measurement or physical-hardware comparison was performed in this documentation-only review.

The original assembly reported ZIP integrity and equality with a standalone specification copy. This review does not regenerate or validate an external ZIP or standalone copy; the files in this directory are the reviewed package. `REVIEW_NOTES.md` identifies the selected checkout/SDK/source inspection and the remaining runtime integration proofs. Memory/performance budgets and hardware hypotheses remain targets and design choices, not achieved measurements.
