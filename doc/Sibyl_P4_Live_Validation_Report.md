# Sibyl P4 live Rack validation

2026-09-18, disposable user-authorized testbed. Octavia 2.12.0 on port 34570,
48 kHz. Existing Sibyl module 4900188172136754 advertised automation version 1.
No source changes, installation, patch save, staging or commit were required.

Physically cabled Sibyl MOD1/MOD2/MOD3/gate to Octavia probes A/B/C/D. Four
float32 Rack-voltage recordings supplied sample-level evidence:

| Test | Result |
|---|---|
| Scene-visit ramp across four two-beat repeats | PASS; 0.625026, 1.875026, 3.125026 and 4.375026 V at the selected mid-repeat positions |
| Scene-repeat ramp | PASS; returns to the same 2.000083 V at each repeat midpoint |
| All notes rejected | PASS; gate remained exactly 0 V while curves moved |
| Patternless scene | PASS; MOD1 reached 3.000041 V at its midpoint, gate 0 V |
| Scene owner over arrangement owner | PASS; MOD3 switched from -2 V to 3 V for the patternless scene, then back on arrangement wrap |
| Live automation edit | PASS; MOD2 moved 4 to 8 V over exactly 48000 sample intervals; maximum linear-fit error 0.000000238 V |
| Sustained gate during edit | PASS; every gate sample remained 10 V throughout the 12-second recording |
| Add mode | PASS; raw event 1 + curve 2 + scene offset 0.5 = 3.5 V |
| Paused physical macro | PASS; 10 V Macro 1 stimulus added 0.25 V, giving exactly 3.75 V throughout the one-second capture |
| Curve removal | PASS; MOD2 returned from 8 to legacy 2 V over exactly 48000 sample intervals; gate remained 10 V throughout the eight-second recording |
| Undo | PASS; restored the deleted curve with its 1000 ms transition and 8 V authored value |
| Queued edit replacement | PASS; paused nextScene revision 6 remained pending over active revision 5 and 8 V output; immediate revision 7 superseded it and settled at 4 V, with no pending revision |
| MCP bounded curve preview | PASS; named coordinates returned 0, 1.25, 2.5, 3.75 and 5 V |
| Invalid preview | PASS; duplicate point time rejected with invalid_automation at the precise field; accepted revision unchanged |

The first recording uses a synchronized hardware reset request at frame offset
12000. Sibyl intentionally applies hardware reset at the next beat; the observed
reset was at offset 25409. The analysis initially assumed an immediate reset,
then was corrected after checking the existing hardware-reset implementation.
Measured musical positions are relative to the actual reset boundary. No playback
fix was needed. Normal cable/sample propagation accounts for the small residual
voltage offset at named positions.

## Evidence

Analysis script and machine-readable results are in `test-results/`:
`analyze_p4_live.py`, `p4-live-clocks.json`, `p4-live-handover.json`,
`p4-live-macro.json`, `p4-live-removal.json`. Reusable candidates are
`p4-live-clocks-fixture.json` and `p4-live-held-fixture.json`.

Raw WAVs and matching JSON frame metadata remain in
`C:/Users/Plasm/AppData/Local/Rack2/Leviathan/Octavia/Recordings/`:

- `octavia-1789753847791-1-sibyl-p4-clocks-silent-patternless.wav`
- `octavia-1789753902378-2-sibyl-p4-sustained-edit.wav`
- `octavia-1789754006203-3-sibyl-p4-paused-live-macro.wav`
- `octavia-1789754032986-4-sibyl-p4-remove-curve.wav`

Final state: accepted/active revision 8, looping two-scene clock fixture, no
pending edit. The four probe cables remain. Temporary Octavia Control A-to-reset
and Control B-to-macro cables were removed. The existing audio chain was untouched;
these were electrical CV/gate measurements, not musical listening tests.

This adds live Rack evidence to the P4 implementation report. It does not replace
the wider automated coverage or P6 integration/performance work. P5 is next.
