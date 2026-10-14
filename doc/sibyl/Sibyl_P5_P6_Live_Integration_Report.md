# Sibyl P5/P6 live integration

Date: 2026-09-18. Native Rack, Octavia API 2.12.0, 48 kHz.
Sibyl module: `4900188172136754`. Octavia: `3142362077727391`.

## Result

The expressive composition and generated chord voices pass live semantic and
physically recorded CV/gate checks. Integration also found an output reconnection
bug, now fixed and regression-tested in source. The initial recording session used the original P6 binary. The subsequent refreshed-build reconnection test passes, as recorded below.

## Live semantic checks

- Capabilities advertise schema 3, harmony, conditions, overrides, automation and
  `voice_progression_v1`, including the documented search budgets.
- The complete companion composition validates without warnings.
- Three generated chorus voices preview and commit with identical reports:
  295 partial assignments, 1039 DP transitions, zero missing pitch classes,
  maximum leap 2 semitones, total movement 9, squared movement 17.
- `createOnly` collision is rejected. A preceding metadata edit in the same failed
  transaction is rolled back; accepted revision and title remain unchanged.
- While playing, revisions 12 and 13 replace the pending `nextScene` candidate
  while active revision stays 11. Revision 13 subsequently becomes active.
- A separate playing `nextBeat` edit returns accepted 14/active 13, then active 14
  with no pending revision. Paused edits adopted immediately, as the existing
  transport contract allows; they were not counted as a pending-replacement test.
- Contextual reads show the bass's chorus octave/velocity/probability overrides;
  automation reads return 0, 1.25, 2.5, 3.75 and 5 V at beats 0/8/16/24/32.
- Complete module state was exported through `vcv_get_module_state` and restored
  through `vcv_set_module_state`. Authored composition, stable note IDs and
  allocator values compare exactly after sorting JSON object keys.
- Octavia semantic undo restores an edited composition exactly, advancing the
  local accepted revision. This is **Octavia undo**, not Rack's native UI undo.

## Physical playback checks

Two temporary VCV Split modules exposed Sibyl's individual pitch/gate channels.
All observed signals were physically cabled to Octavia. Master monitor inputs
were temporarily used for CV; no musical audio listening claim is made.

The first capture exposed the cable bug below and ended 0.373 seconds before the
arrangement's end because two control calls consumed its initial window. It was
not counted as complete-arrangement acceptance. The final full capture used one
play call from a stopped score and retained 26.256 seconds of playback.

After forcing the old binary's output cache to refresh by temporarily changing
the highest track channel and restoring it, recordings verified:

- all 48 beats of the build and chorus;
- relative bass pitches through C/A/F/G harmony and the chorus octave shift,
  maximum observed pitch error approximately `1.59e-7 V`;
- bass onset counts `[7,7,7,8,7,7,7,8]` over the eight build pattern passes;
- protected two-pulse hat fills only on the final build repeat, at beats
  27.5/27.625 and 31.5/31.625;
- changing hat trigger patterns across repeated passes;
- the uninterrupted 32-beat MOD rise, sampled error below `6.68e-6 V`;
- all three generated voices' four pitches, two-second onset spacing, and
  1.9-second gates (at most one sample short in the observed gate durations).

Generated pitch voltages, in chord order:

| Voice | C | A minor | F | G |
|---|---:|---:|---:|---:|
| Low | -2 | -2 | -2 | -1.83333337 |
| Mid | -0.66666669 | -0.66666669 | -0.58333331 | -0.41666666 |
| High | 0.58333331 | 0.75 | 0.75 | 0.91666669 |

Evidence in `test-results/`: `p6-live-semantic.json`,
`p6-live-module-state.json`, `p6-live-waveform-final.json`,
`p6-live-three-voices.json` and `analyze_p6_live.py`. The waveform result files
include the exact archived WAV paths and per-check measurements. Recordings are
IEEE float32 raw Rack volts, not normalized listening audio.

## Reconnection defect and fix

When a seven-channel score was already loaded, attaching the previously unused
pitch output produced one channel. The pre-existing gate cable still carried
seven. Rack's SDK `Output::setChannels()` does nothing while disconnected; a new
connection starts mono. Sibyl cached only the desired score channel count, so it
did not reapply that count after a new cable connection.

`SibylModule::onPortChange()` now marks output channel routing dirty through an
atomic flag. The audio thread reapplies channel counts on a dirty cache and keeps
the ordinary fast path to a relaxed flag read. The callback does not mutate the
audio-owned cache directly. No allocation, lock, or output scan was added to the
ordinary processing path.

Regression tests simulate the SDK connection state and port callback for all six
polyphonic outputs, checking both first connection and reconnection at seven
channels. All pass with zero tracked audio-thread allocations/deallocations.
Authoritative Windows plugin build, full Sibyl module suite and all six legacy
golden traces pass after the fix. Logs: `p6-live-fix-build.log`,
`p6-live-fix-module.log`, `p6-live-fix-golden.log`.

Updated `plugin.dll` SHA-256:
`197DEE488EC8437D2891C92D98C2D01BCD420D604B3B7CA41EB3568C389B8F42`.
The preceding P6 implementation report's hash refers to the pre-fix binary.
Post-fix benchmark observations are retained in
`test-results/p6-live-fix-performance.json`.

## Handoff

Both temporary splitters were removed. All six original Octavia monitor routes
were restored; the original audio path was untouched. The expressive composition
with three generated chorus voices remains loaded, stopped at the beginning of
`build`, accepted/active revision 23, no pending edit or reported error. Its saved
transport setting is also stopped. No Rack patch file was saved, and no code was
staged or committed.

At the initial handoff, refreshed-build reconnection verification remained pending; it is now complete below. Actual Rack UI undo/redo, patch-file
save/reopen and musical listening were not performed. State serialization and
Octavia undo were verified through their available bridge routes instead.

Alternate tuning remains a separate prospective phase. This integration does
not extend the current 12-TET convenience/harmony model.


## Refreshed-build reconnection acceptance

After the user loaded the rebuilt plugin, the same seven-track composition was
still stopped at build beat zero, accepted/active revision 23. Pitch and velocity
outputs were initially unconnected. Connected pitch, velocity and MOD2 to Octavia
A/B/C: all three source ports and all three receiving ports reported seven channels.
Disconnected all three, confirmed zero source channels, and reconnected: all six
port readbacks again reported seven channels. MOD2 channel 4 retained the expected
-2 V patternless-curve value. No composition edit, channel-count workaround,
transport restart, or additional module was needed.

The bridge's batched cable additions are asynchronous: an immediate read initially
preceded the third cable's application. Once the cable inventory confirmed all
three connections, every width was correct. This was distinguished from the old
persistent mono-output defect.

Original cable topology was restored and compared by module/port endpoints;
pitch/velocity returned to unconnected and the four original connected Sibyl
outputs retained seven channels. Transport and revision remained unchanged.
No patch was saved. Evidence: `test-results/p6-live-reconnection-verified.json`.

The reconnection defect is now closed by both native regression tests and live
Rack verification. Native Rack UI undo/redo, patch-file save/reopen and musical
listening remain separate acceptance items; this follow-up did not claim them.
