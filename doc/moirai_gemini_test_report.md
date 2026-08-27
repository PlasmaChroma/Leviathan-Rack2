# Moirai Phase 6 Gemini Qualification Report

Date: 2026-08-26  
Environment: native Windows VCV Rack 2.10.0, Octavia bridge on `127.0.0.1:34570`  
Moirai: `7451012728073945`  
Sibyl: `6695610257002594`  
Octavia: `4050343691917319`

## Verdict

**NOT QUALIFIED.** R01 is restored and the native focused suite passed, but F03
lacks complete live modulation telemetry, F04 lacks the newly required physical
mode/reset/EOC measurements, F05 lacks a valid transient capture, F06 remains
blocked by observation-fixture timing/pool behavior, and G01/G02 plus G03–G05
require visual or manual evidence.

## Gate results

| Gate | Result | Evidence / disposition |
|---|---|---|
| A01 | PASS | Worktree was inspected; no source changes were made. Unrelated pre-existing untracked files were preserved. |
| A02 | PASS | Native Windows MINGW64 bridge was used; command documented in `doc/windows_build_from_wsl.md`. |
| A03 | PASS | Required Moirai docs, source, tests, panel assets, and Octavia references were present; `mTest3.md` was not present. |
| B01 | PASS | Native `make -j10 test-fast RACK_APP_RUNTIME_DIR='/c/Program Files/VCV/Rack2Pro'` completed with Moirai engine/module/adoption/compiler/edit/JSON summaries passed. |
| B02 | PASS | Native `make -j10 plugin.dll` completed (`plugin.dll` up to date). |
| B03 | PASS | Native `python3 tools/validate_plugin_json_tags.py` reported all module tags valid. |
| C01 | PASS | Live status: accepted revision 44, active revision 44, no pending revision. |
| C02 | PASS | Live module topology identified Octavia, Moirai, and Sibyl with expected ports. |
| C03 | PASS | Complete Moirai bank and relevant patch state captured before edits; initial undo stack was empty. |
| D01 | PASS | Live Moirai capability/status/full-bank reads succeeded; semantic view coverage is in `tests/moirai_module_spec.cpp`. |
| D02 | PASS | `tests/moirai_edit_spec.cpp` covers full-bank validation and rejection without state mutation. |
| D03 | PASS | `tests/moirai_edit_spec.cpp` reports revision-conflict rejection without commit. |
| D04 | PASS | `tests/moirai_edit_spec.cpp` reports rollback after later-operation failure. |
| D05 | PASS | `tests/moirai_edit_spec.cpp` reports one successful transaction/revision; live undo stack recorded semantic edits. |
| E01 | PASS | `tests/moirai_adoption_spec.cpp` covers nextTrigger adoption. |
| E02 | PASS | `tests/moirai_adoption_spec.cpp` covers allIdle adoption. |
| E03 | PASS | `tests/moirai_adoption_spec.cpp` covers nextClock adoption. |
| E04 | PASS | `tests/moirai_adoption_spec.cpp` covers finishCurrent/restartActive policies. |
| E05 | PASS | `tests/moirai_module_spec.cpp` covers command isolation and reset behavior. |
| F01 | PASS | Live cable list showed ordinary Sibyl→Moirai poly routing and Moirai/Sil physical monitor paths. |
| F02 | PASS | Live monitor discovery showed A/B/C connected at 48 kHz and rolling; snapshot 61 previously demonstrated the physical boundary. |
| F03 | BLOCKED-EVIDENCE | Existing live evidence remains: 16-channel input/output with concrete channel-0 and channel-15 telemetry. The native `tests/moirai_module_spec.cpp` and `src/Moirai.cpp` establish velocity neutral 10 V and M1/M2/M3 neutral 0 V plus one-channel broadcast code paths, but this pass did not obtain concrete live values for mono Velocity/M1/M2/M3 broadcast, matching-poly modulation, and missing-channel behavior. The required additional subtests therefore remain incomplete. |
| F04 | BLOCKED-EVIDENCE | Native tests cover reset/EOC/output modes, but this pass did not obtain the required physical-monitor measurements for reset/EOC or 0–10 V, 0–5 V, and bipolar ±5 V modes. |
| F05 | BLOCKED-EVIDENCE | A disposable staged one-shot was installed at Moirai revision 45 (20 ms attack, 80 ms decay to 25%, 350 ms release), then restored. Snapshot 65 covered frames `110874399..110910399` (12,000 pre/24,000 post at 48 kHz) and completed with A/B/C all `rms=0`, `peak=0`, `rmsDb=-140`, `issues=[silence]`. Snapshot 66 covered `112625439..112661439` with the same 12,000/24,000 frame request but expired (`allConnectedMask:0`). Neither proves rise/peak/release/downstream response. The prior constant 6.2 V snapshot remains physical-monitor/immutability evidence only. |
| F06 | BLOCKED-INFRA | One deterministic authored marker was attempted after transport stop, but the fast fixture advanced repeatedly through the pattern. Requests 1754–1817 had trigger frames `26185728..27697728`; 1754–1782 and 1790–1817 reported `snapshotId:null`, `snapshot_pool_busy`, while 1783–1789 allocated snapshot IDs 52–58. No single admissible retained completed marker snapshot was resolved and analyzed. An empty trigger list is not PASS; no `mTest3.md` artifact exists in this checkout. |
| G01 | BLOCKED-MANUAL | User explicitly confirmed browser discovery, exact 12 HP width, and complete artwork rendering. The user noted branding rework is desired and explicitly deferred control/anchor layout confirmation because some items will be repositioned later; port placement is therefore also unconfirmed. The actual context menu exposes no theme selector, so the theme subcheck is not applicable. No implementation/layout changes were made. |
| G02 | PASS | User confirmed channel and lane selection tracking and read the display status as `EXT 120 r44/44`, matching external clock at 120 BPM, accepted/active revision 44, and no pending revision. With GATE independently verified unpatched, manual trigger on selected channel 16 produced visible A/B activity, briefly showed `CH 16/16`, and returned to the idle `channels=1` state with revisions unchanged and no error. |
| G03 | PASS | From Moirai's actual context menu, the user selected `Factory preset for selected voice` → `AD Percussive` on lane B/channel 16. Semantic reads show one revision and one-bank-action delta: 44→45, active 45, no pending revision, creation/assignment of `preset_ad_percussive_b_ch16`, and no unrelated changes. One manual Undo exactly restored revision 44; one manual Redo exactly restored revision 45. A single Undo fully reverting the action and a single Redo fully restoring it establishes one Rack history entry. |
| G04 | BLOCKED-MANUAL | Save/reload remains a GUI/manual gate; no save was performed on the disposable patch. |
| G05 | PASS | User closed Rack, confirmed it was gone, reopened it, and explicitly confirmed the window was open before Octavia reconnection. After manually restarting Octavia, the bridge reconnected successfully and Moirai retained revision 45 plus the B/channel 16 preset assignment. The user visually confirmed the panel, labels, display, and controls all render correctly, then changed CHANNEL 16→15 and confirmed immediate clean display updating without flicker, freezing, or corruption. |
| R01 | PASS | Restored full Moirai bank exactly to the captured baseline: revision 44, same schema/clock/lanes/program/assignments/labels, accepted revision 44, active revision 44, pending revision null. Final Moirai parameter values were `[0,0,1,0,0,0]`. |
| R02 | PASS | Strict verdict rules applied: the incomplete required gates produce NOT QUALIFIED. |

## Fixture and restoration notes

## Interactive Phase 6 evidence — 2026-08-26

- G01 browser discovery: the user explicitly answered, "yeah, it's been visible for a while," confirming that Moirai appears in Rack's module browser.
- G01 panel width: the user explicitly answered, "exactly 12," confirming the live Moirai panel occupies 12 HP.
- G01 artwork: the user explicitly reported, "it's going to need some branding specific rework but it's all there." This confirms artwork/assets render completely while retaining branding rework as a non-blocking visual follow-up.
- G01 control/anchor placement: when asked to confirm the six controls against their printed/SVG positions, the user answered, "we'll have to reposition some stuff later, I'm not concerned about layout yet." This subcheck is explicitly deferred and is not recorded as PASS.
- G01 themes/context menu: the user reported, "all I got is factory preset for selected voice." No theme option is available, so the plan's "where available" theme check is N/A; this also identifies the live factory-preset menu used later by G03.
- G02 channel selection: the user explicitly confirmed that selecting channel 16 changes the display, adding that it shows `16/01`. `vcv_get_module` then reported parameter 4 (`Inspected channel`) as `15.0`, and `vcv_moirai_get_status` reported `channels: 1`; thus the display coherently presents selected channel 16 alongside one active/output channel.
- G02 lane selection: after being asked to press LANE once, the user explicitly confirmed that the display emphasis switched from lane A to lane B.
- G02 revision/clock presentation: the user read the display exactly as `EXT 120 r44/44`. Live semantic status independently reported `clockSource: external`, `estimatedBpm: 120.0`, `acceptedRevision: 44`, `activeRevision: 44`, and `pendingRevision: null`.
- G02 manual trigger/voice presentation: after all four duplicate Sibyl→Moirai GATE cables were manually removed, `vcv_get_module` independently verified input 0 as `connected: false`, `channels: 0`. The user held MANUAL TRIGGER on selected channel 16 and reported "there's a small blip, number next to a/b seem to go to 8000, CH shows 16/16 briefly." Post-trigger status returned to `channels: 1`, revisions `44/44`, pending null, and no error. This confirms visible lane activity/phase movement and temporary manual-trigger channel expansion with GATE unpatched.
- G03 preset application: immediately before the UI action, the full bank was revision 44 with only `factory_adsr`; channel 16 had no explicit A/B assignment. The user selected `AD Percussive` from Moirai's actual `Factory preset for selected voice` context submenu. Immediately afterward, the bank was revision 45 and active 45 with no pending revision; program `preset_ad_percussive_b_ch16` was created as a staged `oneShot` named `AD Percussive` (4 ms exponential attack to 1.0, 180 ms exponential decay to 0.0, restart retrigger), and only lane B channel 16 was assigned to it. All clock, lane A, output-mode, label, seed, and factory ADSR content remained unchanged.
- G03 Undo: the user performed exactly one Rack Undo and reported that it appeared correct. Fresh semantic reads proved exact restoration to the pre-action full bank: revision/active `44/44`, pending null, only `factory_adsr`, empty assignments on both lanes, and no B/channel 16 assignment. The entire clock, lane, seed, and factory program document matched the retained revision-44 baseline.
- G03 Redo: the user performed exactly one Rack Redo and reported that the preset returned. Fresh semantic reads proved exact restoration of the post-preset document: revision/active `45/45`, pending null, `preset_ad_percussive_b_ch16` present with byte-semantic field equality to the prior post-preset read, and only lane B channel 16 assigned to it. No unrelated authored-bank fields changed. Because one Undo fully removed the preset transaction and one Redo fully restored it, the context-menu action occupied exactly one Rack history entry.
- G05 close/reopen: the user explicitly confirmed Rack closed (`"ok, it's gone"`). No connection attempt was made while closed. The user then confirmed Rack was open but Octavia inactive; after the user pressed START and confirmed `"alright, live"`, the permitted reconnect reported `running: true`, port 34570, Rack 2.10.0. Moirai remained healthy at accepted/active `45/45`, pending null, no error, with B/channel 16 still assigned to `preset_ad_percussive_b_ch16`. The user visually confirmed, `"yeah, it's all good"`, for the reopened panel, printed labels, display, and controls, with no blank/stale/corrupt regions or repeated errors observed.
- G05 live update after reopen: the user changed CHANNEL from 16 to 15 and explicitly confirmed the display updated correctly without flicker, freezing, or corruption.

The initial disposable patch contained duplicate monitor cables, which caused
clipped 24.8 V captures. Those captures were excluded. The physical observation
boundary was then isolated to one Moirai A cable to Monitor A, one Moirai B cable
to Monitor B, and the Moirai A → Sil → Monitor C path before the prior constant
6.2 V capture. That capture is retained only for physical-boundary and
immutability evidence. The disposable patch was not saved or overwritten.

Moirai was restored with the captured full-bank document and parameter state.
Sibyl was restored semantically to its original empty composition; its revision
advanced transactionally, but its full content matched the baseline. No pending
Moirai revision remained. The final undo stack contains the expected reversible
actions from the qualification work; exact bank restoration was verified by
document comparison rather than by assuming an empty undo stack.

## Defects and files changed

No confirmed Moirai implementation defect was found. F03–F05 remain evidence
deficiencies, F06 is an observation-fixture/timing deficiency, and G01–G05 are
visual/manual evidence deficiencies.

Changed files:

- `doc/moirai_gemini_test_report.md`
- `doc/moirai_impl.md`

`doc/moirai_gemini_test_plan.md` and Moirai implementation sources were not
changed. No files were staged or committed.
