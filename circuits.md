# Bifurx Circuit Findings

## LL dropout root-cause findings

- The current `LL` issue in the non-`SVF` circuits does not look like a simple voicing mismatch.
- The stronger evidence is that the shared mode mixer assumes every circuit exports comparable `lp` / `bp` / `hp` / `notch` signals, but only the `SVF` actually has that contract cleanly.
- `LL` is the most sensitive mode because it uses a true cascade path:
  - stage A processes the input
  - stage B processes `a.lp`
  - the mode output is built primarily from `b.lp`
- That means any mismatch in what a circuit means by `lp` becomes much more obvious in `LL` than in the mixed parallel modes.

## Why the recent LL tuning looks suspicious

- The current working-tree changes add circuit-specific compensation in the shared combiner rather than fixing the circuit cores themselves.
- In particular, `LL` now adds large per-circuit support taps from `lpA` and `lpB`, especially for `MS2` and `PRD`.
- That is a strong sign that the cascade output is not trustworthy on its own for those circuits.
- Put differently: the patch is saying "the exported lowpass outputs are not lining up across circuits, so blend in extra signals until the result stops collapsing."

## Actual seam where the problem likely lives

- `combineModeResponse()` is shared across all circuits.
- The nonlinear cores (`DFM`, `MS2`, `PRD`) return hand-shaped output proxies, not a normalized common basis.
- Those proxies are scaled differently and likely do not represent the same effective pole outputs as the `SVF`.
- If the shared mixer is written as though all circuits expose equivalent band outputs, then mode-dependent failures are expected.
- `LL` just exposes that mismatch first because it is the most cascade-dependent mode.

## Important implication

- This should be treated as a contract problem between the circuit cores and the mode combiner.
- It should not be treated as an `LL`-only taste problem unless measurement proves otherwise.
- Tuning the shared `LL` coefficients before fixing that contract risks baking the mismatch deeper into the code.

## Preview limitation

- The preview model is still based on idealized display biquads, not the nonlinear runtime cores.
- That means preview agreement cannot be used as proof that the runtime circuit semantics are correct.
- A preview-guided retune can easily hide the real mismatch instead of solving it.

## Findings on the circuit realignment "solver"

- The current realignment code is not actually solving measured peak placement.
- It does not inspect:
  - runtime spectral peaks
  - preview local extrema
  - analysis FFT data
  - any circuit output error signal
- Instead, it computes a target from the existing static cutoff mapping and then eases toward that target over several samples.
- In practice, the target is just the ratio needed to cancel `circuitCutoffScale()`.
- That means the solver is functionally a smoothed correction layer over the hardcoded cutoff scale, not a true measurement-driven alignment system.

## Why that is a problem

- If the core mismatch is really in the meaning of exported `lp` / `bp` / `hp` signals, then aligning nominal cutoff frequencies cannot fix it.
- It can only make the markers and nominal center frequencies look more similar while the actual mode behavior remains inconsistent.
- In the worst case, it can hide the real defect:
  - the selected circuit appears "aligned"
  - but the cascade and mode mixer are still operating on incompatible signals
- That makes debugging harder because the cutoff bookkeeping looks intentional even when the spectral behavior is still wrong.

## Current judgment on the solver

- I do not trust the current solver as evidence that cross-circuit peak behavior is truly aligned.
- Right now it appears to be mostly bookkeeping around the pre-existing cutoff scale offsets.
- Unless it is upgraded to use actual measured behavior, it should be treated as a UI/transition smoothing device, not a true corrective solver.

## Recommended next pass

1. Use the normalized-export implementation as the current baseline.
2. Capture `LL` telemetry for `SVF`, `DFM`, `MS2`, and `PRD` at matching settings.
3. Compare:
   - stage-A lowpass output
   - stage-B lowpass output
   - relative loss through the `A.lp -> B -> B.lp` path
   - final output gain in the failing low-frequency window
4. Decide from the measurements whether the semantic export layer is sufficient or needs a different per-circuit adapter shape.
5. Do not add new shared combiner compensation before those measurements are in hand.

## Current plan

- Immediate implementation target:
  - validate the stage-aware export adapter shape for `MS2` first, then `PRD`
- Keep fixed:
  - `combineModeResponse()`
  - shared mode coefficients
  - solver-free baseline
  - no support taps or combiner glue
- Validation target:
  - re-run the Rack `LL` capture after the adapter revision
  - judge success by whether `ll_stage_b_over_a_db` and `ll_output_over_input_db` move materially toward the `SVF` envelope
- Stop and escalate if:
  - adapter refinement starts requiring per-mode hacks
  - the cleanest next move becomes per-circuit extraction rather than semantic export normalization

## Current best judgment

- The best path is still to root-cause the core/output contract first.
- Between the candidate approaches, the next thing to do is measurement on the normalized-export baseline, not more tuning.
- After that:
  - if the normalized exports hold across circuits, that is probably the simplest long-term design
  - if they cannot, then the shared combiner is the wrong abstraction and per-circuit mode extraction is the more honest solution

## Session state

- The circuit realignment solver path has now been fully pruned from `src/Bifurx.cpp`.
- Circuit selection now switches directly to the requested circuit mode with no alignment solve and no retained alignment state.
- The `LL` workaround is removed:
  - the stage-A support tap is gone
  - `LL` now returns the raw cascade path again
- Temporary `LL` telemetry has been added to the existing Bifurx curve-debug path.
- `make test-fast` passes in the current shell environment, including `bifurx_filter_spec`.
- This is now the correct baseline for root-cause investigation.

## Immediate next step from this state

- Gather one more normalized-baseline `LL` capture set in Rack if you want final runtime confirmation.
- Compare the new numbers against the earlier raw-baseline telemetry:
  - stage-B over stage-A should be materially closer to `SVF`
  - final output over input should also compress toward the `SVF` envelope
- If the normalized baseline still leaves large gaps, adjust the adapter shape or escalate to a different export contract.
- Use `Log Curve Debug` and `scripts/bifurx_curve_debug_summary.sh <csv...>` for that check; at this point, measurement is for validating the implemented contract, not for re-proving the original mismatch.

## Rack run checklist

1. Enable `Log Curve Debug`.
2. Set mode to `LL` (`Low + Low`).
3. Keep input steady and use the same fixed knob positions across all circuits.
4. Run one circuit at a time for 8-10 seconds.
5. Capture separate runs for:
   - `SVF`
   - `DFM`
   - `MS2`
   - `PRD`
6. Run `scripts/bifurx_curve_debug_summary.sh <csv...>` on the resulting CSV files.
7. Read both summaries:
   - circuit-level rollup
   - circuit-plus-mode breakdown
8. Compare:
   - `ll_stage_a_lp_rms`
   - `ll_stage_b_lp_rms`
   - `ll_stage_b_over_a_db`
   - `ll_output_over_input_db`
9. Treat the pass as successful if the non-`SVF` circuits now sit in the same rough envelope as `SVF` instead of showing a large structural gap.

## Measured LL telemetry results

- That telemetry pass has now been done at least twice:
  - one moderate-level pass
  - one lower-level pass to reduce clipping/saturation as a confound
- The lower-level pass is the more important one because it reduces the chance that the differences are just drive artifacts.

### Lower-level pass summary

- `SVF`:
  - `ll_stage_b_over_a_db ~= 0 dB`
  - `ll_output_over_input_db ~= -0.22 dB`
- `DFM`:
  - `ll_stage_b_over_a_db ~= +5.5 dB`
  - `ll_output_over_input_db ~= +10.4 dB`
- `MS2`:
  - `ll_stage_b_over_a_db ~= +5.75 dB`
  - `ll_output_over_input_db ~= +5.0 dB`
- `PRD`:
  - `ll_stage_b_over_a_db ~= +4.85 dB`
  - `ll_output_over_input_db ~= +7.2 dB`

### What those measurements mean

- `SVF` behaves like a coherent baseline contract.
- The non-`SVF` circuits are not merely "voiced differently" in `LL`.
- Their exported lowpass/cascade behavior is structurally hotter than `SVF`, even in the lower-level pass.
- That makes the contract mismatch hypothesis much stronger than the "shared LL coefficients need tuning" hypothesis.
- The ambiguity here is now much lower than it was before telemetry existed.

## Updated current judgment

- The evidence now strongly supports a core output-contract mismatch.
- Shared combiner retuning should remain frozen until the contract decision is made.
- At this point, the question is no longer "is there a mismatch?"
- The real question is which architecture should own the fix:
  - normalize the circuit core outputs to a common contract, or
  - introduce per-circuit mode extraction/mixing

## Current implementation status

- The semantic export normalization pass is now implemented.
- The shared `combineModeResponse()` abstraction is still in place.
- `DFM`, `MS2`, and `PRD` now go through a normalized semantic export layer before the shared combiner sees `lp` / `bp` / `hp` / `notch`.
- The implementation now uses a stage-aware per-circuit soft-saturation adapter rather than a new combiner-side compensation layer.
- This preserves the shared mode algebra while moving the contract boundary to the circuit export side.
- The test model mirrors the same normalized-export behavior so the fast spec exercises the same contract as runtime and preview.

## What not to do next

- Do not retune shared combiner coefficients as the primary fix.
- Do not add circuit-specific support taps or compensating glue in `combineModeResponse()`.
- Do not skip directly to fully per-circuit mode extraction unless the normalized-export approach has been tried or ruled out for a concrete reason.

## Present status

- Measurement-first investigation has succeeded.
- The raw `LL` telemetry is no longer hypothetical; it clearly separated `SVF` from `DFM`, `MS2`, and `PRD`.
- The normalized-export implementation is now in place in both runtime and the fast-test model.
- `make test-fast` passes with the semantic export layer enabled.
- The source build for `src/Bifurx.cpp` now compiles in this WSL shell.
- The repo is at a validation point for the stage-aware adapter revision.
- `GPT-5.4` weighed in on the abstraction boundary:
  - prefer normalized per-circuit semantic exports first
  - keep shared mode combination unless that approach fails
- If follow-up work is needed, it should start from this stage-aware baseline instead of returning to raw proxy tuning.

## Handoff readiness

- The repo is ready for a constrained follow-on handoff.
- The correct handoff scope is:
  - validate the stage-aware adapter against Rack telemetry
  - keep the shared `combineModeResponse()` path in place while evaluating that baseline
- The handoff is not:
  - freeform tuning
  - shared-combiner retuning
  - direct introduction of per-circuit mode extraction without first exhausting the normalized-export baseline
- Stop and escalate if:
  - the export contract cannot be kept coherent without per-mode hacks
  - normalization starts requiring circuit-specific glue inside `combineModeResponse()`
  - the work starts collapsing into ad hoc compensation rather than a clean semantic contract

## Instructions for GPT-5.3

- Treat this as a measurement-first task, not a tuning task.
- Do not reintroduce:
  - `LL` support taps
  - circuit-specific combiner glue
  - alignment or solve layers
  - preview-guided retuning presented as proof of runtime correctness
- The immediate objective is to determine where the loss actually occurs:
  - if `ll_stage_b_over_a_db` diverges strongly by circuit, the core lowpass outputs are not on a common contract
  - if stage-B looks acceptable but `ll_output_over_input_db` still diverges strongly, inspect mode extraction/combiner semantics next
- That first condition has now been satisfied by measurement.
- Do not spend more time trying to prove that a cross-circuit lowpass contract mismatch exists; treat it as established until contradictory evidence appears.
- The desired outcome of the next pass is not "make LL sound better".
- The desired outcome is one of these explicit conclusions, backed by measurement:
  - the nonlinear cores can be normalized to a common output contract, or
  - the shared combiner is the wrong abstraction and per-circuit mode extraction is required
- Do not add corrective DSP until one of those conclusions is justified by the telemetry.
- The current planning default is:
  - attempt normalized per-circuit semantic exports first
  - only move to per-circuit mode extraction if that fails cleanly

## When to ask GPT-5.4 to weigh in

- Ask `GPT-5.4` to weigh in when the task shifts from measurement to abstraction choice.
- Specifically, escalate when one of these is true:
  - the telemetry is ambiguous and more than one interpretation fits
  - there is a design choice between normalizing core outputs versus introducing per-circuit mode extraction
  - a proposed fix changes the abstraction boundary between the circuit cores and `combineModeResponse()`
  - a model is about to add compensating glue because it cannot clearly explain the contract mismatch
- Do not spend `GPT-5.4` budget on routine implementation, logging, or test-running once the design direction is already clear.

## Environment note

- In this WSL shell, `make test-fast` works and should be used for local validation.
- Do not treat full Rack/plugin linking in this shell as authoritative.
- Final Rack/runtime validation still belongs in the Windows/MSYS2 toolchain.
