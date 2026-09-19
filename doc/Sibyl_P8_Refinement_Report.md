# Sibyl P8 refinement

This pass addresses the correctness findings in the P8 review and implements the
remaining protocol/toolkit refinements. Live baseline and audible acceptance gates
remain unmeasured; implementation coverage does not imply those gates passed.

## Correctness

- Prepared commits always compare the candidate's base revision with the accepted
  revision, even when the caller supplies a newer `expected_revision`.
- Receipt replay reports success without recording another Octavia undo entry,
  through either the legacy or generic semantic route. The original receipt is
  returned unchanged.
- Nested columnar defaults are independently owned by each expanded event.
- Scene copies reject existing destination IDs, malformed mappings, and mappings
  that merge distinct source patterns. Repeated references to one source share
  only the new copy created within that operation.
- Native progression helpers emit typed `tones`, not legacy `intervals` alongside
  `rootPitch`. Acoustic major/minor thirds and fifths derive from ratios; the
  53-EDO septimal dominant is `[0,17,31,43]`.
- Compact pattern scaffolds retain evolution and pitch context. Condition objects
  remain objects; explicit ratchet and glide values survive compilation.
- Native integer arpeggios use native steps. Multi-period integer arpeggios require
  explicit `period_steps` or legacy `scale_degrees`. Named notes transpose their
  octave labels. Invalid contours and nonpositive spacing reject locally.

## Interface

- `vcv_sibyl_resolve` returns one module ID without returning the patch inventory
  to the model. Multiple Sibyl modules require explicit selection. The standalone
  Python client also refuses to choose the first module in an ambiguous patch.
- Compact capabilities include a revision-independent contract fingerprint and
  omit operation/view inventories. Optional topic queries return one contract.
- `view="arrangement"` returns scene assignments, overrides, referenced pattern
  counts/resolutions/contexts/evolution, and automation IDs without note arrays.
  Pages default to eight scenes (maximum 32); cursors are revision-bound. Bounded
  automation lists explicitly report totals and omissions.
- Accepted edit responses include the actual operation count and bounded authored
  object paths (`affectedObjects.ids`, `total`, `omitted`). Parent paths represent
  changes to definition collections such as harmony progressions.
- Python/MCP receipts preserve pending revision, apply boundary, phase policy,
  warnings, and affected objects. Prepared operation counts come from the server;
  older responses without that count do not invent one. Summary responses add
  per-operation counts; full responses retain detailed reports.
- Native edits negotiate `response_profile` (`receipt`, `summary`, `full`), retaining
  the existing full default. Invalid profiles reject before mutation. Warnings are
  preserved, so warning-heavy receipts may exceed the normal byte target.
- Native capability manifests include a module-instance identity and a contract
  fingerprint independent of score revision. Python clients probe this small
  manifest and cache full contracts only for that identity/fingerprint. Legacy
  builds remain usable without trusting a stale cached contract.
- `OCTAVIA_SIBYL_TOOLSET=compact` opts into three Sibyl tools: resolve, capabilities,
  and typed action dispatch. Detailed action schemas are available through the
  `toolSchemas` capability topic. The default keeps all existing specialized tools.
- Arithmetic onset series and sparse overrides expand before canonical validation.
  Python validates batch shape and expands to `insert_notes` for legacy builds.
- Columnar note reads retain explicit null versus absent fields with a separate
  missing-cell map; cursors bind the requested encoding.
- Toolkit additions cover non-octave equal temperaments, explicitly seeded random
  walks, native voicing orchestration, and corrected scene automation scopes.

## Validation

Regression coverage includes stale prepared candidates, replay mutation reporting,
nested default isolation, invalid scene-copy mappings, arrangement summaries and
stale cursors, ambiguity-safe resolution, compact fingerprints, and native macro
structures.

Verified on 2026-09-19:

- Native MINGW64 `make -j10 test-fast` with the installed Rack2Pro runtime: passed.
- Native `plugin.dll` compilation and link: passed.
- Python/MCP unittest discovery: 52 tests passed.
- Real toolkit/C++ validator checks: all seven fixtures passed.
- Native standard receipt fixture: 233 bytes (500-byte acceptance target).
- Native compact manifest fixture: below 300 bytes.
- Diff whitespace check (with Windows CRLF recognized): passed.

`test-fast` now generates seven fixtures using the actual Python toolkit and feeds
them to a Rack-linked C++ validator. Both 38- and 53-EDO compact output must compile
and produce the same canonical score as event-object authoring, including ratio
pitches, conditions and evolution. Further cases exercise a non-octave seeded walk,
native arithmetic/sparse expansion against Python fallback, and native voicing with
scene automation. This complements mocked adapter tests.

## Remaining acceptance work

Live end-to-end remeasurement of the seven baseline scenarios requires loading the
updated plugin in Rack. Context-recovery size, whole-workflow traffic reductions,
and audible adoption are not established by the isolated regression fixtures.
No live patch was changed in this pass. See [P8 wire usage](Sibyl_P8_Wire_Usage.md)
for the new request formats and compatibility behavior.

The persistent score remains schema version 4. All new work is on the control or
Python side; no new DSP parsing or expansion was introduced.
