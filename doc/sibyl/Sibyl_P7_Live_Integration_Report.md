# Sibyl P7 live integration — 2026-09-19

The refreshed test rack passed the P7 semantic and physically monitored CV/gate
checks below. No implementation defect was found in this run.

## Live environment and observation

- Octavia reported running, bridge version 2.12.0, at port 34570.
- Sibyl capabilities advertised schema 4, stage P7D, native harmony, Scala
  interchange, and `voice_progression_micro_v1`. The refreshed MCP exposed the
  catalog, mapping, and Scala export arguments.
- Sample rate: 48 kHz. Sibyl ID: `4900188172136754`; Octavia ID:
  `3142362077727391`. No external clock/run cables were connected.
- The existing MOD1-to-A cable was replaced with Sibyl pitch output 0 to monitor
  A. Existing MOD2-to-B, MOD3-to-C, and gate-to-D cables were retained. Monitor
  discovery and capture connection masks confirmed physical connections.
- Captures observe the first polyphonic channel. No oscillator/listening claim
  is made; the measured signals are raw Rack volts, not normalized audio.

## Semantic checks

1. Validated and loaded a four-event 53-EDO score: static root, harmonic third,
   tone-ID fifth, and nearest-tone selection across the octave boundary.
2. Catalog discovery returned the ordinary 53-EDO definition. Interval mapping
   returned steps 0/17/31 for 1/1, 5/4, and 3/2; the fifth error was
   -0.0682084126 cents relative to the pure ratio.
3. Previewed a scene transform without changing accepted revision 24. Committed
   one step + one period - 1.5 cents at `nextBeat`; observed pending revision 25
   and then active revision 25. Recorded its actual output.
4. Previewed and committed native two-voice materialization. Preview and commit
   reported the same root bass at -1 V and upper third at 17/53 V. Notes were
   fixed `pitchV` values; the upper voice was physically recorded.
5. Used Octavia's real undo operation to undo the voicing transaction. Revision
   27 restored pattern `p` and its scene transforms. This tests bridge undo,
   not manual interaction with Rack's undo/redo menu.
6. Exported the authored 53-EDO tuning to Scala, imported it as `scala53`, and
   exported it again. Text was identical, including implicit-unison/count and
   terminal-period handling.
7. An intentionally unsupported negative-cent Scala edit returned
   `unsupported_tuning_shape`. Accepted and active revision stayed 29, and
   playback remained running with no last error.
8. Validated and physically recorded 38-EDO with steps 0/12/22 and nearest root
   selection across the period boundary.
9. Validated and physically recorded an unequal table with positions 0/200/700
   cents and a 3/1 period. Checked a repeated root, static table coordinate,
   harmonic lattice shift, and repeated fifth.
10. Captured the live full module preset, changed its tuning, then restored the
    preset through `vcv_set_module_state`. Readback at revision 32 retained the
    unequal table, exact 3/1 period, native roots, tone IDs and roles. A second
    physical capture confirmed the restored pitches.

## Physical capture results

All six captures had connection mask 60 throughout (A–D connected). In total,
960,000 four-channel frames were recorded over 20 seconds. Analysis checked
522,871 gate-high frames, excluding the first eight frames after each gate
onset to avoid interpreting inter-module scheduling transients as held pitch.
All expected event classes appeared. MOD2/MOD3 carried opposite-signed event
markers and were also checked on the four-event fixtures.

| Capture | Duration | Gate rises | Maximum held-pitch error |
|---|---:|---:|---:|
| 53-EDO baseline | 4 s | 8 | 2.362e-8 V |
| 53-EDO scene transforms | 4 s | 8 | 5.335e-8 V |
| Materialized upper voice | 2 s | 1 | 1.013e-8 V |
| 38-EDO | 4 s | 8 | 1.255e-8 V |
| Unequal tritave | 4 s | 8 | 2.624e-8 V |
| Restored unequal tritave | 2 s | 4 | 2.624e-8 V |

**Zero mismatching checked frames.** The largest error is approximately
0.0000641 cents, consistent with float output representation.

Retained evidence:

- `test-results/p7-live-evidence.json`: capabilities, fixtures, preview,
  mapping/export, rejected edit, undo/restoration, and final status.
- `test-results/p7-live-recordings-final.json`: exact recording paths, Rack-frame
  intervals, sample rates and connection masks.
- `test-results/p7-live-analysis.json` and `test-results/p7_live_analyze.py`:
  reproducible float32 WAV checks and counts.
- `test-results/p7-live-original-state.json`: original live Sibyl preset.

WAVs and sidecars are at the paths returned by Octavia under the Rack user
directory's `Leviathan/Octavia/Recordings` folder.

## Final state and remaining boundaries

The test rack contains the 53-EDO fixture, stopped at beat zero, accepted/active
revision 33, no pending operation, gate mask zero, no errors or warnings. Monitor
A remains the pitch probe; B/C are MOD2/MOD3 and D is gate. Presence was released
to automatic. No Rack patch file was saved or overwritten; no files were staged
or committed.

Live semantic operation, bridge undo, preset-state restoration and physical
pitch/gate acceptance passed. Listening through a synthesizer, manual Rack UI
redo, and patch-file save/close/reopen were not performed. The earlier native
Windows build, full fast suite and 44.1/48/96 kHz module tests remain documented
in [the P7C/D implementation report](./Sibyl_P7CD_Implementation_Report.md).
