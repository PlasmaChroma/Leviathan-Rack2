# Physical and wrapped seam fade audio

Original seam transitions now connect to independently predicted head coordinates,
all four fade states, raw audio rendering and manager cleanup. All 720 cases /
1,368 head renders pass, with 219,336 comparisons and maximum gain/mix discrepancy
3.0401959949521995e-8. There are 144 heads with simultaneous active fade kinds;
coverage is 1,652 instruction addresses. Counts describe these fixtures, not
whole-instrument parity.

## Reproduction

```sh
python firmware/Lubadh/probes/probe_boundary_fade_model.py --seams
```

Results: `probes/seam_fade_model_probe_results.json`. The shared harness pins
main ELF SHA256 to
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
The ordinary mode was rerun after this extension: all 160 cases / 240 renders
still pass with maximum discrepancy 2.510718088988284e-8.

The original boundary iteration, nested allocation/transfer/trigger/movement,
raw render slice and complete TapManager::update execute. Expected boundary
calls and replacement head coordinates come from the prior wrapped-region model,
not from original snapshots. Those predictions drive the independent trigger
state equations and interpolation reference. All fade fields are compared before
rendering and after cleanup; each head's gains and accumulated raw mix are
independently predicted. No executed head/fade endpoint is a reference input.

## Fixtures

Three families cover physical, wrapped selected-region and overlapping seams.
Each uses effective speeds -1/-0.125/0/+0.125/+1, following or held speed, fractions
0/0.375, 7/32/128 frames, selected fade duration 32/128, and selected replacement
permission enabled/disabled. Held speed is opposed to the live speed for nonzero
cases; held zero remains zero. The wrapped model's former `held or 1` fixture
expression was corrected accordingly.

The physical fixture uses supplied period 1024, reverse threshold 32 and reverse
destination 1282. Forward cases reach 1025; reverse cases reach 31. The selected
wrapped fixture has start/end 700/300 and reverse start/end 732/332; targets
301 forward and 731 reverse enter its excluded interval. The overlapping fixture
uses start 1000, reverse start 1032 and reverse start modulo 8. Reverse target 7
triggers both physical and selected-region transitions. Forward target 1025
checks the corresponding physical path.

The physical transition supplies fade duration 2458 independently of the selected
fade duration. Selected replacement permission does not suppress the physical
replacement. With both reverse transitions and selected permission enabled,
the source plus two replacement heads are rendered and checked. The finite tape
has 4096 deterministic cells; raw cubic sampling uses the original clamp policy.
These supplied physical coordinates are not asserted to be factory loop length
or a Rack memory limit.

## Inherited fades move again on the replacement

The overlapping case closes an ordering gap in the reference. A new replacement
preserves its own selected fade kind while inheriting the other source fade
states. When crossing motion advances that new head, it also advances the
inherited active fades. Copying the source's final states without that additional
movement predicts incorrect current phases and splice audio.

The model applies the copied fades' movement with the signed supplied block
displacement, while the replacement's own fade uses its independently predicted
trigger initialization. This matches both endpoint fields and products of the
simultaneous fade gains, including fractional and 128-frame movement. Cleanup
then resets the whole head for any active lower-bound crossing, or just the
individual fade for an upper crossing, as in the separate lifecycle findings.

## Remaining boundary

These are complete single-old-head seam iterations with available replacement
slots. Persistent reentry, saturated allocation, iteration over multiple old
heads, simultaneous recording-manager transitions, upstream Link/stack
publication, full output coloration and callback scheduling remain separate
integrations. Earlier head-iteration and persistent-render suites provide related
evidence, but their fade-state oracles are not silently promoted to independent
state proof by this result. Stationary out-of-table production reachability also
remains unresolved.
