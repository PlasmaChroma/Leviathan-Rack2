# Leviathan Chimera — Acceptance Tests

This document is normative with `Leviathan_Morphagene_Codex_Spec.md`. Test IDs are stable. The cases below are **requirements to implement and execute**, not a claim that an implementation already exists or has passed them.

## Test harness and oracles

Build a Rack-independent 48 kHz core harness and a separate host-rate/Rack integration harness. Inputs include stereo frames, connected flags, parameter values, timestamped edges, and prepared-store commands. Capture output audio, CV, natural boundary timestamps, record writes, stable marker IDs, requested/current selection, applied revisions, and ownership diagnostics. Support deterministic injection of latched duration and already-settled controls only in focused unit tests; end-to-end tests must exercise the actual control pipeline.

Use original generated fixtures: independent left/right impulses; tagged constant regions; sine tones at 100 Hz, 1 kHz, and 8 kHz; ramps; a deliberately discontinuous loop; lengths 1, 2, 3, 16, 257, 4,800, and the full 8,352,000 frames. Nonzero DC fixtures bypass input/output conditioning only in the explicitly named unit tests. Production code must not expose a user-facing test bypass accidentally.

Exact integer expectations apply to counts, addresses, IDs, revisions, PRNG states, and 48 kHz event times. Reference mapping/interpolation error is at most 1e-6 unless the main spec permits a 1e-5 optimized table error. Numerical DSP output tests must declare absolute/relative tolerances; never hide failures behind a global loose tolerance. Compare SRC output after its independently measured delay and transient region. Golden source recordings are not required.

`reference_vectors.json` and its standard-library generator provide mathematical anchors. They do not replace system-level tests or establish hardware fidelity. A test not executed is reported NOT RUN, not PASS.

## A. Registration and controls — MG-003, MG-009, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| CTL-001 | Build module, enumerate manifest/model and param/input/output/light IDs. | Exactly one new model; every ID/default matches section 3; existing modules unchanged. Capture append-only schema golden. |
| CTL-002 | Create null-module browser widget; create an actual empty module. | No full Reel allocation or eager disk worker in browser; empty module exposes all controls and live monitoring without sample content. |
| CTL-003 | S.O.S. knob 0.75, no cable; then patched 0, 4, 8, 16, and -4 V. | Settled mix 0.75; 0, 0.375, 0.75, 1, 0 respectively. Do not add knob value to patched CV. |
| CTL-004 | Apply 4 V to Gene/Slide with knob 0.25 and attenuverter +1/-1. | Effective normalized control 0.75/0; a zero attenuverter rejects CV. Morph and Organize use their distinct /5 law. |
| CTL-005 | NaN/Inf samples and controls; finite ±100 V control overload. | Audio non-finites become zero; controls retain last finite/default; raw control clamp ±24 V and final parameter clamp; no non-finite state. |
| CTL-006 | Evaluate classic speed at knob 0, 1/6, 1/2, 5/6, 1. | Rates -2, -1, 0, +1, +2; continuous approach to Stop; no invented 0.223 minimum rate. |
| CTL-007 | Classic speed with signed CV and attenuverter sweep. | Coordinate summing occurs before nonlinear mapping; crossing center reverses; hard cap ±2; table approximations meet error budget. |
| CTL-008 | `vsop=1`, unity knob, attenuverter +1, CV -2..+2 V. Repeat reverse unity. | Signed rates follow powers of two; reverse remains reverse; no semitone quantization. |
| CTL-009 | `vsop=2`, knobs 0, 0.75, 1; CV zero and ±1 V. | CCW Stop remains stopped under CV; unity at 0.75; forward-only output; cap and rate-limit telemetry correct. |
| CTL-010 | Step each smoothed control from 0 to 1 with known tau. | One-pole response matches formula; eventually lands exactly on unchanged target; gates and PM are not smoothed through this path. |
| CTL-011 | Sweep Gene control through 0.0001..0.0002 in both directions. | Hysteretic full/finite state; no chatter or dependence on approximate floating equality. |
| CTL-012 | Load a Reel while S.O.S.=0; set exact unity/Stop context actions in each speed mode. | Loading never changes the mix; context actions select the mode-appropriate knob coordinates. |
| CTL-013 | Initialize controls and change gain/routing during an existing fade. | First finite observation initializes smoothers; non-window fades use `(j+1)/K`, land on frame K, and restart from current value without stacked jobs. |

## B. Reader, windows, and scheduler — MG-001, MG-002, MG-004, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| DSP-001 | Interpolate known ramps and irregular sample arrays at integer/fractional coordinates. | Cubic outputs match reference vectors; identical coordinates but independent channel data; expected overshoot permitted. |
| DSP-002 | Read 1/2/3-frame regions at negative/large coordinates with PM and final rate ±512. | Every tap wraps within the voice region; no read from adjacent Splice or unused capacity; ASan/UBSan clean. |
| DSP-003 | Finite `Ng=480`, rates 0.5, 1, 2, -1, and a mid-Gene speed change. | Voice born b completes at b+480 in every case; source excursion changes, duration does not. |
| DSP-004 | Full-Splice `L=4800`, base rates 0.5, 1, 2. | Primary traversal times 9600, 4800, 2400 frames; source-travel remainder retained for noninteger rates. |
| DSP-005 | Evaluate finite Gene mapping for multiple Splice lengths, including L<16. | Logarithmic map and positive-rounding convention match vectors; bounded 1..L; no log(0) on empty state. |
| DSP-006 | Inspect windows N=1,2,3,16,480,4800 in both smoothing modes. | Discrete tiny-N rules; endpoint/plateau values match formulas; smooth edge longer where capacity permits; no default full Hann substitution. |
| DSP-007 | Constant unit source, centered pan, D=1, `gnsm=0`, bypass DC filters, after startup. | No periodic amplitude hole; steady plateau stays 1 within 1e-5. Boundary correction never stacks unbounded residuals. |
| DSP-008 | Repeat constant fixture with `gnsm=1`; inspect D<1. | Audible window dips intentionally remain in smooth mode; D<1 preserves actual scheduled no-voice gaps, not an always-on bypass voice. |
| DSP-009 | Discontinuous loop at D=1; capture boundary sample and next E samples. | Default unity boundary correction is causal, starts at previous emitted wet value, and reaches zero correction at E; `omod=1` bypasses it. |
| DSP-010 | Hold finite duration 480 and Morph anchors 0,1/6,1/2,5/6,1 for 10 seconds. | Onset counts match density/hop accumulator within one initialization event; fractional phase has no accumulating integer-hop drift. |
| DSP-011 | Set default seed and render repeatable onsets at low and maximum Morph. | PRNG uint32 states match vectors; exactly two draws per musical onset even below chord region; no wall-clock dependence. |
| DSP-012 | Maximum Morph, `mcr={2,-3,4}`, both base directions. | Slots 1..3 use signed configured ratios; multiplication of signs correct; slot 0 ratio remains 1; new ratios latch on new onsets. |
| DSP-013 | Distinct left/right signals; center and hard-side pan. | Center preserves both channels at gain 1; hard side uses sqrt(2)/0 law; no mono fold-down or channel swap. |
| DSP-014 | Four identical centered constant voices with overlapping plateau. | Common normalization produces unity, not 4×; denominator never divides by zero; transition tails do not add excess slot gain. |
| DSP-015 | Drastic duration reduction, repeated forced onsets, maximum Morph, PM, reversals. | At most four musical slots/eight reading cursors; extra replacement uses bounded scalar residual; no dynamic allocation or voice leak. |
| DSP-016 | Cross Vari-Speed through zero during playback and recording, PM disabled. | Wet fades to zero over 48 ticks, cursor is retained, writer/live path continue, full-Splice natural boundary does not fire at Stop. |
| DSP-017 | Repeat Stop with finite Genes and separately with active PM. | Finite timer behavior remains defined; PM can sound a stationary address; no divide-by-zero in full-Splice scheduler estimate. |
| DSP-018 | Queue a Splice change while high-ratio secondary full-Splice voices expire early. | Only the primary cycle governs queued selection/Play commitment; secondary completion still contributes EOSG events. |
| DSP-019 | Change gnsm mid-voice at D=1, then enter a D<1 gap near unity. | Existing voices retain latched window/unity policy; new voices use new policy; residual is zero whenever no musical voice contributes. |
| DSP-020 | Full-Splice L=1/2/3 with base rate 32 and ratios up to 16; then Stop with PM enabled. | Musical expiry counts once per voice; primary remainder stays modulo L with one boundary transition per frame; no catch-up onset loop or launches accumulating at Stop. |

## C. Position, selection, Play, and Clock — MG-007, MG-008, MG-009

| ID | Stimulus | Required result |
|---|---|---|
| TRN-001 | Slide 0→1 in L=1000 and L=8,352,000. | Target equals L-1; per-tick delta ≤64 source frames; current heads receive the delta once; long move is about 2.72 seconds. |
| TRN-002 | Hold Slide constant during finite free-running playback; then reverse rate. | Each onset revisits the same origin, while within-Gene cursor progresses; reverse onset begins at wrap(origin-1). |
| TRN-003 | Three unequal-length Splices; sweep Organize 0..1. | Equal index bins regardless of duration; u=1 selects final region, never out-of-range. |
| TRN-004 | Hold knob in bin 0; send successive Shift rises without moving knob. | Requested index advances and wraps; stationary knob does not snap selection back on the next sample. |
| TRN-005 | Dither Organize within 10% bin hysteresis; then large jump; combine a bin change and Shift at one timestamp. | No chatter; direct large selection; Organize resolves first, Shift increments that result. |
| TRN-006 | Request new Splice mid-primary-cycle in `omod=0`, then in `omod=1`. | Default waits precisely for next primary boundary; immediate commits on event frame and deliberately has no added selection fade. |
| TRN-007 | Pending selection plus PLAY rise, with Slide nonzero. | Pending selection commits before retrigger; new region uses its own Slide-mapped origin, not old absolute address. |
| TRN-008 | `pmod=0`: patched low at load, rising edge, later falling edge. | Low starts stopped; rise starts/retriggers; fall waits until primary boundary, then retires all musical activity coherently. |
| TRN-009 | `pmod=1`: rise and fall between boundaries. | Immediate start; immediate transport stop with ≤48-frame audio tail; tail does not emit EOSG. |
| TRN-010 | `pmod=2`: low/high sequences while playing. | Rise retriggers; low never requests stop. No implicit toggle interpretation. |
| TRN-011 | PLAY cable insertion/removal while previous logical value is high/low. | Normal-high applied once; high→high is not a spurious retrigger; low→normal-high is a single rise. |
| TRN-012 | First, second, and third Clock edges, with changed third interval. | Phase established first, period second; period updates directly thereafter; no half/double tempo guessing. |
| TRN-013 | Clock intervals 1,2,48000,2,880,000 ticks. | Unsupported too-fast interval diagnosed; accepted intervals remain bounded; no unbounded loop chasing missed edges. |
| TRN-014 | `ckop=1`, finite Ng=480, forward/reverse, known Slide. | Each edge moves origin ±480 frames modulo region and forces one onset; same-frame normal onset suppressed. |
| TRN-015 | `ckop=1`, full-Splice endpoint. | A step of L wraps to same region and retriggers it; Clock does not select a different Splice. |
| TRN-016 | `ckop=2`, Ng=480, P=2400, base rate 0.5 then 2. | Origin velocity is direction×0.2 source frame/core frame in both runs; within-Gene pitch changes independently. |
| TRN-017 | Stretch Clock tempo change and missing edges. | New edge reanchors newly born Genes; old voices finish; before two edges origin holds; timeout freezes trajectory and reports waiting. |
| TRN-018 | Hybrid density crosses 2±0.02; explicit `ckop=1/2`. | Hybrid hysteresis is stable; explicit modes ignore Morph-based selection. |
| TRN-019 | Disconnect stopped/running Clock while record state is armed or active. | Armed changes cancel without starting/stopping actual recording; playback returns to defined free behavior; no fabricated Clock edge. |
| TRN-020 | Natural primary completion and Shift/Play/REC/Clock events share a timestamp; repeat with `pmod=0` stopping on the due boundary. | Ordering matches section 13.2; due natural completion counted once, requested selection resolved before boundary commit, no double onset; the stop frame retains its due EOSG pulse and later stopped frames clear it. |
| TRN-021 | Pending Splice or pmod=0 boundary-stop followed by a Gene Shift Clock edge before natural expiry. | Clock resolves the explicit primary boundary first; commits/stops once, does not emit a fabricated completion, and does not restart stopped playback. |
| TRN-022 | Clock interval above 60 seconds, too-fast repeated edges, and no second edge after connection. | Overlong interval becomes new first edge; too-fast playback edges leave last accepted timestamp unchanged; record quantization remains independent; waiting state has a defined initial timeout. |

## D. Recording and marker editing — MG-001, MG-005, MG-006, MG-007

| ID | Stimulus | Required result |
|---|---|---|
| REC-001 | Prepared empty Reel; immediate Current start at frame 100, stop at 110. | Initial Append; exactly ten frames captured, covering 100..109; writer rate is fixed 48 kHz. |
| REC-002 | Existing Splice; start Current while the playhead is mid-Splice with Vari-Speed 0.5, -1, 2, and Stop. | First write uses the frame under the primary playhead; the writer then advances +1 per core tick within that Splice, independent of Vari-Speed. Play stop does not stop recording. |
| REC-003 | Aligned single-reader loop, bypass conditioning, constant old buffer B and input X, settled S.O.S.=s. | Each write equals `(1-s)*X+s*renderedOldBuffer`, read-before-write; no extra `+= oldDestination`. |
| REC-004 | Same as REC-003 but `inop=1`, then transition inop during record. | Stored frames equal live input after source transition; heard S.O.S. mix remains independent; 48-frame source crossfade is exact. |
| REC-005 | Reader and writer same address; impulse and one-frame-region fixtures. | All head reads precede that frame's write; no algebraic zero-delay feedback or dependence on voice processing order. |
| REC-006 | Arm Current, change selection, Clock start; then request another selection while recording. | Start uses the region and playhead address committed on its Clock edge. A pending selection leaves writes in that region; the frame that commits the next Splice writes at its primary playhead address, then wraps within the newly selected Splice. |
| REC-007 | Clock-connected REC toggle twice before start; repeat while ArmedStop. | First pair arms then cancels start; second pair arms then cancels stop; never immediate recording by accident. |
| REC-008 | REC start request and Clock at same timestamp; later stop request and Clock together. | Start frame included, stop frame excluded; output record length is exact, not one frame off. |
| REC-009 | Append to populated Reel while old region plays. | Old playback persists; validFrames grows exactly with writes; unwritten capacity never audible; finalization creates/request-selects new region. |
| REC-010 | Cancel zero-frame Append; append to final available frame; append at 300 Splices. | No zero-length region; last valid frame included then forced finalization; over-cap start rejected while Current/TLA still works. |
| REC-011 | Marker gate during Current, Append, and playback-only. | Capture respective pre-write cursor or primary pre-PM playback frame; endpoint marker during Append commits only when audio exists after it. |
| REC-012 | Simultaneous REC and SPLICE gates, then the alternate recording context command. | Gate inputs remain independent; the menu command invokes the alternate destination exactly once without requiring simultaneous buttons. |
| REC-013 | Insert duplicate/zero/end/out-of-bounds markers; fill marker table. | Preserve a single implicit start, reject/ignore per documented case, no zero-length regions, bounded 300 capacity. |
| REC-014 | Remove/move markers while voices retain old region; edit with stable selected ID. | Voice bounds remain safe until retirement; selection follows stable identity, not shifted array position. |
| REC-015 | Erase region, Delete Splice, and Clear Reel, then undo/redo. | Erase preserves length; Delete compacts and rebases markers; Clear is explicit; checkpoint-based undo restores exact audio/metadata or reports unavailable. |
| REC-016 | Full-volume/high-Morph feedback for extended run; inject nonfinite stored sample fixture. | Defined finite guards activate with telemetry; no NaN propagation, crash, or silent data pointer corruption. |
| REC-017 | Append at 298/299 regions; fill markers while armed/recording; save mid-Append; initial empty record with S.O.S. above zero. | Reserved start marker cannot be consumed by other insertions; prior playback bounds remain fixed; initial wet stays silent until finalize; mid-Append snapshot reloads captured frames/markers as valid Idle state. |
| REC-018 | Capture playback marker during overlapping/chord Genes, a density gap, and stopped playback. | Address follows the independent base-rate primary cursor, never slot 0 reuse or a secondary chord rate; Slide applies once and PM never enters marker address. |

## E. Buttons, options, and derived outputs — MG-009, MG-010, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| OPT-001 | Press/release REC, SHIFT, and SPLICE individually with a mouse; use each named menu action. | Single actions occur on release; every MVP action is reachable without simultaneous button presses. If a future combined action is added, its UI uses a visible latch. |
| OPT-002 | `rsop=0/1`; single REC versus alternate recording menu command. | Only default/alternate destination assignments swap; explicit semantic Current/Append commands remain explicit. |
| OPT-003 | Select each gain target -3,0,+6,+12 dB from the menu, including an interrupted fade. | Correct multipliers and 5 ms gain transition; no claim of ADC PGA simulation. |
| OPT-004 | Two Chimera instances with distinct Reels and Splice selections. | Each instance owns one independently saved Reel; Organize changes Splices only, with no bank or Reel Mode state. |
| OPT-005 | Set every ckop/vsop/inop/pmin/omod/gnsm/rsop/pmod/cvop and signed mcr limits; save/reload. | Accepted values persist; zero/out-of-range/nonfinite mcr rejected; each option has actual DSP/state behavior, not UI-only storage. |
| OPT-006 | Import options with whitespace/comments, duplicate keys, unknown keys, malformed values. | Whole-file validation; duplicate/malformed rejects without partial application; unknown safe keys warn/retain extras; no file execution. |
| OUT-001 | Identical unit-peak 1 kHz stereo sine; bypass conditioning; render 96,000 frames from zero and summarize frames 48,000..95,999. | Asymmetric energy follower matches reference mean/min/max (mean about 7.390832 V), within 1 mV; do not expect true-RMS `8/sqrt(2)` calibration. |
| OUT-002 | L-only/R-only energy and post-S.O.S. changes. | Follower measures specified stereo bus, not only L/input/pre-S.O.S.; routing and normalization tests remain distinct. |
| OUT-003 | `cvop=1`, finite primary Ng=480 and overlapping secondary voices. | One rising primary-cycle ramp, reset at primary boundary; secondary overlaps do not spuriously reset it; reverse still rises. |
| OUT-004 | Full-Splice ramp under varying rate and Stop. | Ramp follows primary traveled distance; Stop holds full-cycle phase, while an actual stopped transport reports the defined zero. |
| OUT-005 | Natural voice completions at slow/fast hops; forced steals/retriggers and record wraps. | EOSG 0/10 V; adaptive bounded pulse width; simultaneous events share pulse but telemetry counts all; artificial/record events do not emit. |
| OUT-006 | `pmin=1`, R connected, L below/above detector thresholds for controlled times. | 3-second absence entry and 16-tick presence exit use specified raw-L RMS; cable state alone is not the audio-presence detector. |
| OUT-007 | Active PM with ±1 V and ±20 V right input, stopped/moving heads. | 96 source frames/V, raw PM clamp ±10 V; common offset on every head; writer/marker/phase timing unchanged. |
| OUT-008 | PM activates/deactivates while R carries audio. | R leaves/returns to live record-monitor path with 5 ms transition; no unintended gain-normalization or L→R copying while PM active. |
| OUT-009 | Step stereo energy 0→1 from zero, then 1→0 from independently initialized unity state; opposite-polarity stereo sine. | Step samples match recurrence vectors within 1 mV; anti-phase stereo matches identical-phase energy; production detector uses internal units and clamps 0..8 V. |

## F. Sample-rate bridge and realtime behavior — MG-001, MG-009, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| RT-001 | Host 48 kHz. | Direct core path; no SRC buffering latency or converter allocation on each callback. |
| RT-002 | Host 44.1/48/88.2/96/176.4/192 kHz; capture known duration and pitch. | Reel stays 48 kHz and correct duration; pitch stable; conversions use calibrated input/output latency, not simply one core tick per host tick. |
| RT-003 | Host extremes 8 kHz and 768 kHz plus invalid rates. | Stress bounded FIFO/iteration work and defined error/silence recovery; no unbounded catch-up loop. Record whether the host actually sustains each extreme separately. Do not infer full-patch real-time playability from this test. |
| RT-004 | Host gate pulses shorter than one core period, multiple timestamps collapsing to one core tick. | Detected host edges preserved in bounded event queue and ordered; gating not low-pass resampled as audio. |
| RT-005 | Simultaneous audio impulse and PLAY/REC/Clock events through SRC. | Events act on matching delayed input frames; audio/CV/EOSG output alignment matches declared measured bridge latency. |
| RT-006 | Change host sample rate during playback, Current record, and ArmedStart. | Preserve Reel/core positions; pause/prepare/re-prime per spec; cancel armed transition; no old-rate writer drift or converter allocation/free on audio. |
| RT-007 | Allocation trap around process/onSampleRateChange-equivalent real-time paths under all feature combinations. | Zero heap allocation/free, filesystem, blocking locks, JSON work, thread joins, or final shared_ptr deletion. |
| RT-008 | Saturate command/completion/event queues while recording. | Busy/error visible, accepted jobs reach result, reserved retire capacity works; no silent dropped REC edge or leaked handle. |
| RT-009 | Four-voice maximum density + active PM + record + waveform updates; repeat during full snapshot. | Record hardware/compiler/settings, median/p95/p99 callback cost and UI timings; check target budgets separately from correctness. Do not mark PASS from estimates. |
| RT-010 | Process bypass on/off with REC held, all Play modes, live input present. | Bypass stops record/freezes core, supplies defined normalized dry path, zeroes CV/EOSG; unbypass does not synthesize a held-REC rise. |
| RT-011 | Host 768 kHz, five gates changing every host frame, connection changes, and maximum prepared SRC input delay. | All mapped events fit proven queue/history capacity and 256-event per-core budget; induced overflow stops writes/cancels arms visibly and requires fresh start. |

## G. Snapshot, assets, and lifecycle — MG-001, MG-011, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| IO-001 | Establish snapshot at an exact core cut while every page is subsequently overwritten. | Frozen file equals content at cut, including pages not incrementally scanned before first overwrite; metadata/audio revisions are coherent. |
| IO-002 | Snapshot while writer loops a one-page region thousands of times. | First protected-page write copies once for that lease; subsequent writes reuse private page; full reserve not repeatedly consumed. |
| IO-003 | Full 32,625-page Reel snapshot during recording. | At most 8 page references scanned per tick; capture completes in bounded core time; no missing recorded frames or audio maintenance ownership misses. |
| IO-004 | Finish encoding, release lease, request new export during reclamation. | Retire/reclaim ≤8 references per tick; second snapshot queues or reports busy until allowed; no reference reuse before worker releases. |
| IO-005 | Force impossible pool exhaustion in debug harness. | Safe recording stop/error, no allocator fallback or overwriting protected page; failure is surfaced rather than hidden. |
| IO-006 | Host not advancing, request save; race host restart with maintenance ownership. | Snapshot can complete without waiting for a nonexistent callback; atomic ownership prevents concurrent access; restart uses documented brief muted/re-prime behavior. |
| IO-007 | Save hook while live recording continues. | Hook returns only after coherent current SaveBundle/assets exist; no queue-and-return race with Rack archive; file I/O never holds audio ownership. |
| IO-008 | Disk full/permission failure/cancel during staged asset write. | Previous committed assets preserved; incomplete temp files not manifest targets; error visible; current in-memory Reel not silently replaced. |
| IO-009 | Save two Chimera instances with distinct full-length Reels, remove/move original import files, reload patch on clean install. | Each embedded Reel plays without source paths; instances have independent mutable stores and cache identities. |
| IO-010 | Duplicate module and edit/record in each; reuse saved preset/JSON without assets. | Full supported duplication yields independent mutable stores/cache IDs; missing-asset workflows report missing content, not borrow mutable pointers or invent silence silently. |
| IO-011 | Repeated add/remove, widget destruction, load cancellation, stale completion, plugin shutdown. | Generation checks retire results; no raw dead Module/Widget access; ASan/TSAN clean; plugin joins workers off audio. |
| IO-012 | Snapshot completion while doc metadata changes; concurrent save requests. | Save fence pins one exact audio+metadata revision bundle; coalescing only when revision compatible; no newest JSON over older samples. |
| IO-013 | Continuous record with autosave checkpoints; crash simulation before/after atomic commit. | Recover last completed checkpoint; never claim uncommitted latest samples survived; checkpoint cadence and durable-write assumptions reported. |
| IO-014 | Reset versus Clear Reel; absent asset on restore. | Reset preserves recorded content while resetting controls/options/transients; Clear requires explicit command; missing content error remains visible with live monitoring usable. |
| IO-015 | Empty unprepared module; first audio-input connection/record.prepare; REC before and after readiness. | Capacity prepared off audio; early start rejected visibly with no delayed auto-record; a fresh ready-state start has exact frame timing. |
| IO-016 | Save while host is stopped/bypassed, release lease, then save a newer cut without resuming audio. | Maintenance finishes prior capture/reclamation and makes progress on second save; no expired heartbeat assumption causes concurrent core/SPSC access. |
| IO-017 | Snapshot partial last page during Append, then append into new pages and overwrite retained pages during incremental reclamation. | Snapshot length/page count remain fixed at cut; partial page is protected, post-cut pages need no capture, reclaimed entries cannot clone/free recycled IDs. |
| IO-018 | Multiple consumers share one cut; rapid replacement loads with old snapshot encoder still active. | Lease releases only after last reader; retired payload remains charged; active + prepared + retired stores never exceed the 256 MiB payload ceiling. |
| IO-019 | Failed save with/without prior bundle; periodic serialization without onSave; normal save and shutdown paths on supported Rack runtime. | No unsafe hook exception; old coherent bundle or explicit missing-audio/saveFailure state; no newer markers over old audio; report actual host cancellation behavior and do not claim whole-patch atomicity. |

## H. WAV and state validation — MG-011, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| WAV-001 | Export and reimport canonical stereo float32 with distinct channels and markers. | 48 kHz, correct 8-byte frame stride, exact finite payload preservation, cue offsets in frames not channel-scalars; no automatic normalization. |
| WAV-002 | Inspect RIFF fmt/fact/cue/LIST/data byte lengths, padding, IDs, and start/end cues. | Matches chosen software interchange profile; no end sentinel becomes a zero-length Splice; do not label internal roundtrip a hardware validation. |
| WAV-003 | Strict mode mono/PCM/other-rate/overlength imports. | Reject with precise reason before changing active Reel. |
| WAV-004 | Convenience PCM16/24/32, float32/64, mono/stereo, several rates. | Worker conversion and mono duplication correct; marker round-rescale/sort/dedup correct; overlength rejects unless explicit truncation selected. |
| WAV-005 | Corrupt/truncated/overflow-sized chunks, huge labels, invalid counts, NaN/Inf frames, unsupported formats. | Parser remains bounded, handles warnings/errors as specified, never trusts extension or integer overflow; fuzz suite under sanitizers. |
| WAV-006 | Import and export one Reel using user-chosen filenames beside unrelated files. | Only the selected Reel file is read or written; unrelated files remain untouched and overwrite requires explicit permission. |
| STA-001 | Malformed/unknown/future schema fields; invalid paths including traversal/symlink escapes. | Validate before use; no directory escape; unsupported future schema yields explicit safe failure rather than destructive rewrite. |
| STA-002 | Save/reload with pending/recording/clock/gate/job state. | Only authored persistent state restored; transient recording/armed/jobs/transport position not serialized into an automatic destructive restart. |
| STA-003 | Change dspProfile in future test migration fixture. | Profile 1 keeps original curves and seed contract; schema and DSP profile are separate; no silent sound retuning. |

## I. Octavia and UI integration — MG-009, MG-010, MG-012

| ID | Stimulus | Required result |
|---|---|---|
| API-001 | Discover capabilities/document/status. | Existing Octavia interface used; correct capability/polyphony/limits; compact state contains no audio arrays or per-sample trace. |
| API-002 | Validate and apply options/marker transaction with expectedRevision. | Validate makes no mutation; edit is atomic, acknowledged only after actual adoption; stale revision rejects with current revision. |
| API-003 | Retry same idempotency key for record start, import, and destructive command. | Same result/job returned; no double toggle, duplicate import, or second destruction; conflicting payload with same key rejects. |
| API-004 | Explicit start Current/Append with quantization, repeated start, disconnected Clock, stop while armed/Idle, cancel arm, Play stop/retrigger. | `none` overrides cable; `clock` without cable rejects; same-destination start is no-op; stop cancels ArmedStart and is safe while Idle; semantic stop latch survives held Play until defined restart. |
| API-005 | Concurrent UI and semantic writes; unauthorized path, overwrite, destructive action, revision conflict. | One control dispatcher preserves SPSC ownership; existing authorization scope respected; every rejected/accepted action has truthful result. |
| API-006 | Long load/export accepted, then cancelled/module deleted; poll status. | Distinguish accepted/running/applied/failed/cancelled; no success before store swap/file completion; generation prevents stale adoption. |
| GUI-001 | Light/dark panels, module browser, normal/min/max zoom and high DPI. | All labels/jacks accessible, no overlap/cutoff, readable state independent of color, title-only baked raster convention respected. |
| GUI-002 | Full Reel, many markers, moving four heads while recording. | Waveform built off audio/UI hot path; bounded cached draw; no every-frame full-Reel scan or mutex held over NanoVG draw. |
| GUI-003 | Hide/offscreen module, destroy/reopen widget while sound continues. | No change to audio/record behavior; telemetry ownership remains valid; display cache and jobs safe across widget lifetime. |
| GUI-004 | With one mouse pointer and no simultaneous button presses: choose Current/Append, arm/start/stop REC, select/add Splices, change gain/options, load/export one Reel WAV, and save/reload the Rack patch; then scrub, reverse, overlap, clock stretch, and TLA. | Every MVP action has a visible panel or named menu path; status distinguishes requested/armed/active/applied and dirty/saved/error states. Loading a Reel asks before replacing occupied audio, export asks before overwrite, and the embedded patch reopens after the original WAV moves. Human listening notes remain separate from numerical/hardware claims. |
| GUI-005 | Regenerate assets from master; close/reopen DAW window and switch graphics contexts with cached display active. | Split assets/atlas match master; shared lifecycle helpers invalidate and rebuild safely; Debug Terminal preserves module-total Process/Step/Draw and debug gating. |

## Execution gates and reporting

Fast CI runs mapping/state/reader tests on small buffers. Rack-linked CI covers host hooks, JSON/patch assets, semantic control, and actual parameter wiring. Slow/nightly tests cover full capacity, parser fuzzing, prolonged feedback, multiple module instances, lifecycle stress, and sanitizer concurrency. A manual pass covers panel/readability/listening; a separate optional hardware worksheet covers the brief's unknown primitives.

Record test ID, pass/fail/not-run, build commit, operating system, compiler/flags, Rack version, fixture version, measured values/tolerance, and attached failure artifact path. For save-failure tests keep an untouched fixture copy and report whether the host changed its destination; module-owned asset consistency does not establish host archive atomicity. A complete release has no unexplained NOT RUN entries for mandatory acceptance cases. Physical-unit comparisons may remain pending, but then public documentation must not claim matched hardware behavior for the corresponding primitives.
