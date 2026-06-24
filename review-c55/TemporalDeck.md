# Temporal Deck Review

## 1. Executive Summary

Temporal Deck is a stereo live-buffer/turntable sampler with scratch, freeze, reverse, slip, file loading, platter customization, and TD.Scope expansion. Its behavior is unusually well tested. Live-buffer reconfiguration and prepared-sample installation now allocate on the worker, swap storage in constant time on audio, and retire displaced buffers on the worker. Live-to-sample conversion now retains the circular live storage and records a logical sample-start offset, eliminating its vector allocation and capture copy from `process()`. Its synchronous preview rebuild remains a callback-time spike to profile and move off audio.

Release readiness: 6/10

## 2. Module Inventory

- Source/UI: `TemporalDeck.cpp/.hpp`, 3,045-line engine header, UI, platter/frame/transport/sample lifecycle/prep, arc-light, menu/test and expander protocol files; codec and shared graphics helpers.
- Assets: `res/deck.svg`; `res/Vinyl/{Static.svg,Blank.png,TemporalDeck.png,inventory.json}`. Code also references absent fallback `DragonKingLeviathan.png`.
- Params (10): Buffer, Rate, Scratch sensitivity, Mix, Feedback, Freeze, Reverse, Slip, cartridge cycle, Add Scope. Inputs (7): position/rate CV, stereo audio, scratch/freeze/reverse gates. Outputs (4): stereo, scratch gate/position. Lights: transport/link/ready plus 62 arc lights.
- Menu/state: vinyl sync/art/brightness, interpolation, gate modes, buffer range, slip return, sample load/save/clear/info and debug exports/logs. JSON saves latches, modes, buffer, sample path, loop and art paths. Bidirectional right expander to TD.Scope.

## 3. DSP and Audio/CV Correctness

Engine, platter, transport, interpolation, preview, and sample preparation have strong deterministic tests. Sample-rate and buffer choices are explicit, stereo input normalizes from left, and async decode/preparation handles buffer construction. Reconfiguration/install use worker-prepared storage; live conversion is a zero-copy circular sample view with mapped playback, scope, preview and export access (`TemporalDeck.cpp::process`; `TemporalDeckEngine.hpp::convertLiveWindowToSample`). Non-finite external CV/audio sanitation is incomplete.

## 4. UI, Panel, and Interaction Review

The turntable metaphor, arc lights, tonearm and art system clearly communicate purpose and strongly fit the suite. The context menu is exceptionally long (roughly `TemporalDeckUI.cpp:3623-4046`); core sample/transport choices should remain prominent while signing/export/debug controls should be development-only. Missing Dragon King fallback art is stale.

## 5. Performance Review

- Severity: High — live-to-sample conversion no longer allocates or copies capture storage, but still rebuilds the full sample preview synchronously in `process()` (`TemporalDeckEngine.hpp::convertLiveWindowToSample`, `rebuildPreviewFromCurrentSample`).
- Severity: High — 10-minute stereo mode can retain large live, decoded and prepared buffers concurrently; logging records their capacities (`TemporalDeck.cpp:1377-1435`).
- Severity: Medium — audio process calls `system::getTime()` for drag/expander paths and publishes a large scope payload (`TemporalDeck.cpp:1660-2175`).
- Severity: Medium — vinyl download worker can detach at shutdown (`TemporalDeckUI.cpp:672-704`), relying on process-lifetime leaked state.

## 6. Stability and Rack Integration

Async sample lifecycle is carefully joined at module destruction and path restoration is serialized. `loadSampleFromPath()` always returns true before async decode and discards `errorOut`, so invalid files are only logged later (`TemporalDeck.cpp:2362-2399`). Absolute sample/art paths make patches non-portable. Expander protocol has magic/version/size checks. Preset reload with missing files needs explicit UX.

## 7. Code Quality and Maintainability

Behavior is split into testable helpers, a strength. Total surface area is very large, the engine is a 3,045-line header, and UI is 4,185 lines. Consolidate lifecycle/state transitions and move development-only vinyl signing tooling out of the production menu before adding features.

## 8. Musical Usefulness

Scratch/freeze/slip, live-to-sample conversion, cartridge characters and the scope interaction form a distinctive instrument. Defaults are usable. Buffer modes and sample persistence need clearer memory/portability explanations.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| TEMPORALDECK-001 | High | Audio RT | Allocation/copy portion resolved: reconfiguration/install are worker-built and live conversion retains circular storage. A full preview scan still runs on audio. | `TemporalDeckSampleLifecycle.cpp`; `TemporalDeckEngine.hpp::installPreparedSample`; `convertLiveWindowToSample` | Build or incrementally derive the converted-sample preview without an O(frames) callback scan; confirm with allocation/callback instrumentation. |
| TEMPORALDECK-002 | High | Memory | Long modes may hold live+decoded+prepared copies. | `TemporalDeckSampleLifecycle.cpp:112-129`; lifetime metrics | Set/enforce a memory budget and release obsolete copies before install. |
| TEMPORALDECK-003 | Medium | UX | Async load reports success immediately and ignores `errorOut`. | `TemporalDeck.cpp:2362-2399` | Publish completion/error state to UI. |
| TEMPORALDECK-004 | Medium | Assets | Dragon King mode maps to a missing packaged file/inventory entry. | `TemporalDeckUI.cpp:744-786`; `res/Vinyl/inventory.json` | Remove mode or package/sign the asset. |
| TEMPORALDECK-005 | Medium | Lifecycle | Download worker detaches during shutdown. | `TemporalDeckUI.cpp:681-704` | Add cancellation/timeout and guaranteed ownership. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Remove the synchronous live-conversion preview scan from audio and bound peak memory. Worker-based reconfiguration/install and zero-copy live conversion are complete.

### Should Fix Soon

Report async load failures, fix stale art, test shutdown/cancellation and portable missing-file behavior.

### Nice to Have

Split production sample controls from developer asset-signing tools.

## 11. Suggested Tests

Retain the existing 64 Temporal Deck tests; add real Rack 44.1-192 kHz changes during recording/sample playback, allocation tracing, 10-minute-mode failure injection, missing/corrupt file restore, duplicate during decode, rapid delete during workers/download, mono/stereo/poly cable behavior, expander hot-plug, 20-instance memory/CPU, and all zoom levels.

## 12. Final Verdict

Status: Experimental  
Primary blocker: live-to-sample conversion still performs an O(frames) preview scan on audio  
Best next action: make preview finalization incremental/off-thread and add allocation/callback-time instrumentation
