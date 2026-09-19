# Sibyl P7A — Core microtonal pitch model

Implementation baseline: `4115c0d396f47fc3ab4e37719d47d6f8535559ab`.

## Implemented scope

- One equal-division engine for every integer division count 1–1024, including 38-EDO and 53-EDO, with arbitrary supported repeat periods.
- Unequal periodic cents/ratio tables, native scale subsets, pitch contexts, and explicit event → pattern → composition context resolution.
- Schema-4 static `tuned.step`, `tuned.degree`, `tuned.cents`, and `tuned.ratio` notes with signed periods. Exact ratios are reduced for serialization and never implicitly snapped to a grid.
- Signed degree/table wrapping and double-precision pitch compilation before the existing float output boundary.
- Schema-2/3 import, schema-4 serialization across full/pattern/note/scene views, patch state, and portable envelopes. Native fields in earlier schemas reject.
- Existing note insertion/update/projection preserves native data and pitch exclusivity. Existing semitone edits retain 100-cent units; degree edits support native degrees. Cross-pattern duplication pins inherited native context.
- Definition metadata and authored coordinates survive serialization; the audio thread consumes compiled voltages through the existing scheduling path.
- Pitch definition/event storage participates in the combined pitch, automation, and harmony budget.

`capabilities.sibyl.pitchSystems.stage` reports `P7A`. Native transform and harmony capability flags remain false. Unsupported native event transform fields fail explicitly rather than being silently ignored.

## Validation

Native Windows MINGW64 build and routine suite commands:

```sh
make -j10 plugin.dll test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
```

The final Windows `plugin.dll` build and full `test-fast` run completed successfully (exit 0). Results are recorded in `test-results/p7a-final-validation.log`. Focused codec coverage includes all 1–1024 divisions, negative indices, unequal tables, non-octave periods, explicit-context precedence, exact-ratio preservation, malformed/out-of-range coordinates, schema gates, and both acceptance tunings.

Module coverage exercises actual 38/53-EDO pitch and gate output, allocation-free processing, patch persistence, expression edits, and cross-pattern context preservation. All six legacy golden hashes remain unchanged at 44.1, 48, and 96 kHz in the final validation run, including the storage-accounting changes.

The example `Sibyl_P7A_53EDO_Example_Composition.json` is a self-contained stopped composition demonstrating native grid steps, a negative scale degree, exact ratios, and cents. No live Rack patch has been modified or saved by this milestone, and the built binary has not been installed into the running Rack process.

## Remaining P7 scope

P7 is not complete. P7B still needs definition-edit operations, typed event/scene transforms, retuning, explicit destination-context copy policy, native queries, and broader adoption tests. P7C adds native harmony and microtonal voicing. P7D adds preset discovery, interval mapping, Scala interchange, further capacity/performance coverage, and live integration.

Core static playback is a foundation for these phases, not evidence that their acceptance cases pass. In particular, existing conventional note/chord convenience forms retain their legacy 12-TET meanings.
