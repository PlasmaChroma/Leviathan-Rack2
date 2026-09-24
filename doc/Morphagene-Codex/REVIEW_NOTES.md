# One-pass specification review — revision 2

Reviewed 2026-09-23 against checkout `ee5f8ff21d457b73b3752b303dd79bd22da56010`, local `AGENTS.md`, the supplied source brief, package artifacts, selected integration files, installed Rack SDK headers, and adjacent Rack source. Adjacent Rack source is evidence of an integration risk, not proof of the installed Rack Pro runtime's exact implementation. This review changes documentation and offline reference tooling only.

The shared-Reel architecture and feature scope are retained. Revision 2 resolves internal contradictions and makes previously implicit software choices explicit. DSP profile remains 1 because no implementation or released profile-1 patch compatibility was established. The source brief is byte-for-byte unchanged; its embedded research citation tokens are not independently retrievable citations from this review. Hardware claims remain attributed to that supplied brief, not newly verified here.

## Findings resolved

| Finding | Refinement and verification requirement |
|---|---|
| Asymmetric 5/80 ms follower cannot satisfy the true-RMS sine expectation. | Preserve the recurrence; add sine and closed-form energy-step vectors. A unit-peak 1 kHz stereo sine averages about 7.390832 V, not 5.657 V, with conditioning bypassed. OUT-001/009 cover calibration. |
| 64 events per core frame cannot cover the declared 768 kHz host rate. | Raise bounded processing to 256 including connection events; prove delayed-history queue capacity. Overflow stops recording visibly. RT-011. |
| Save failure was described as if the module could always abort the host archive. | Define coherent previous-bundle/missing-audio fallback with saveFailure diagnostics; require runtime tests before claiming safe cancellation. The local source invokes void onSave both during explicit save and destructor shutdown. IO-019. |
| Serialization without onSave could pair new metadata with old recorded audio. | Require explicit durable cuts and revision-specific manifests; cover autosave/history and no-prior-bundle failure. IO-012/019. |
| Stopped-host handling captured pages but did not finish old reclamation. | Maintenance handles capture, lease releases, and reclamation, including bypass, under exclusive ownership. IO-016. |
| Append could move the snapshot scan bound or mutate its partially filled last page. | Freeze snapshot length/page bound at a pre-write cut; protect the partial page, exclude later pages. IO-017. |
| Shared lease release and reclaimed-page protection were underspecified. | Release only after all consumers; explicit retained-reference state prevents reuse/double-free during reclamation. IO-017/018. |
| Memory budget omitted retired stores held by encoders. | Charge active, prepared and retired payload until actual off-thread destruction; backpressure prevents a third full store. IO-018. |
| Append reservation could be consumed by manual markers; growing playback bounds were unclear. | Reserve marker capacity across arming/recording, freeze existing playback regions, make initial appended audio playable only on finalization, and give mid-record snapshots valid provisional markers. REC-017. |
| Marker capture used an undefined primary playback address at high Morph. | Define a non-reading base-rate primary cursor independent of musical slot reuse, ratios and PM. REC-018. |
| Window-option adoption both latched and changed existing windows. | Latch the whole window/unity policy; prevent residuals filling scheduled gaps. DSP-019. |
| Tiny full-Splice loops at high ratios and Stop could imply unbounded completion/onset loops. | Define per-voice expiry, modulo primary remainder, bounded boundary resolution and suspended full-Splice launches at Stop. DSP-020. |
| Stop hysteresis, ramp endpoints, long Clock intervals and explicit recording commands were incomplete. | Align Stop with its specified deadband; define initialization, linear fades, estimator outliers, clock-required commands, repeated start/stop and terminal job status. CTL-013, TRN-022, API-004/006. |
| Source layout invited edits to generated SVGs and omitted current lifecycle/telemetry rules. | Add master SVG regeneration/atlas commands, shared graphics helpers, context reopen checks and module-total Process/Step/Draw. GUI-005. |
| Release plan treated the unfinished test-rack suite as universally required. | Align with AGENTS.md: native Windows plugin.dll and test-fast, focused integration tests, supported sanitizer configurations; no staging/committing. |
| Package validation statements and checksums described only the original assembly. | Add repeatable package validation, strict vector JSON type checks, refreshed vectors/checksums and this review ledger. |

Existing acceptance IDs remain stable; 13 new cases bring the matrix from 125 to 138. The new cases are requirements, not passed module tests.

## Remaining gates and choices

- **Phase 0 engineering proofs:** supported Rack runtime save/autosave/shutdown behavior; same-process duplication of newly recorded, not-yet-checkpointed audio; Speex linkage and prepared bridge latency; worst-rate event-history capacity; retirement memory admission. Do these before committing to the worker/persistence architecture.
- **Musical choices retained:** classic cubic (including its aliasing), four musical slots, density curve, 16-frame minimum Gene, 64-frame Slide slew, PM scale, clock stride and 28 HP layout are explicit software choices. Tune by listening before release if desired, updating vectors/tests and revision history together. They are not hardware measurements.
- **Public product identity:** Chimera is the user-selected Leviathan working title and slug as of document revision 3. Finalize attribution and original panel design before distribution.
- **First-record behavior clarified:** initial recording monitors only the live side of S.O.S. until finalized. There is no automatically growing wet feedback loop during initial capture. This is a deliberate software baseline, recorded here so it can be reconsidered explicitly before release.
- **Save limitation:** a failed module save can leave Rack with a clearly marked older coherent bundle. The module must not claim the current audio was saved, and documentation must not promise whole-patch atomicity without runtime evidence.

No user decision blocks reviewing or beginning Phase 0. No module implementation, plugin build, live Rack session, listening comparison or hardware validation was performed in this documentation pass.

## Revision 3 — Chimera working title

On 2026-09-23 the user selected **Chimera** for the Leviathan module. Updated document titles, proposed source/asset names, model registration, C++ namespace, module-relative asset paths, behavior menu label, semantic capability and reference-vector schema to match. The package directory and main specification filename retain their original names for continuity. Source-hardware references, the original technical brief, MG requirement IDs, hardware-oriented WAV filenames and all DSP formulas remain unchanged. No released module identity or saved-patch migration is implied by this pre-implementation rename.

## Revision 4 — Phase 0 bound and checkout audit

The native quality-5 Speex probe measured 640 host frames of input latency at 768 kHz. The 256-event per-core processing cap remains feasible if each jack emits at most one combined connection/edge record per host frame: `13*16=208` in one core interval. A 16,384-entry delayed jack history covers the measured `13*640=8,320` worst-case records with scheduling margin. The main specification now states this encoding and admission rule. `INTEGRATION_BASELINE.md` records the checkout audit; `IMPLEMENTATION_STATUS.md` tracks live work and is excluded from fixed package checksums.
