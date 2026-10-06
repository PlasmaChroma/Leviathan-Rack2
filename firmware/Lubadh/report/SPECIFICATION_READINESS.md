# Specification readiness ledger

This is the current continuation checkpoint, superseding the original research
order where later probes have supplied evidence. The baseline unresolved-question
table and archived validation summary retain their original scope. A high number
of comparisons does not establish whole-instrument parity.

## Completion criterion

We can write an implementation-ready specification when every audible and
stateful feature has either a recovered, reproducibly checked contract or an
explicit native design decision with an acceptance test. A placeholder, an
unexplained firmware branch, or an arbitrary library substitution does not meet
that criterion. Hardware-exact voltage/analog claims additionally require device
measurements; the Rack adapter may instead specify deliberate voltage and timing
laws without presenting them as recovered hardware behavior.

**Current decision: not yet ready to claim a complete parity specification.**
The connected transport and output effects models are substantially stronger,
but speed-event, link, modulation, asset and routing contracts remain
consequential gaps.

## Coverage by contract

| Contract | Current evidence | Work required for specification closure |
|---|---|---|
| Signed split-coordinate motion, cubic playback and recording | Transport and persistent record probes execute original bytes; fractional, held, following, reverse and stalled cases | Join upstream speed/motion scheduling and recording boundary transitions; retain an independent full state model |
| Logical region calculation and physical/playback seams | Region, boundary-source, head-iteration and persistent boundary probes; independent seam states/audio/cleanup checked with available and four/five occupied slots, including replacement allocation failures | Production field publication, independent persistent reentry and callback-wide saturated iteration, simultaneous recording-manager boundaries; smallest legitimate regions, tail and capacity transitions |
| Tap pools and gain | Four engines/five slots, allocation exhaustion, reclamation and eight-call engine gain checked | User-reachable retrigger/one-shot/steal sequences and abrupt clear/preset changes |
| Tape write/playback ordering | Joined input/output/record probe checks 2,592 blocks on the same tape; first-record stop/tail slices check 5,296 blocks; real gate/state/write/expiry histories check 702 slices through both manager updates | Whole callback event/motion scheduling and both boundary managers; independently carry per-head fade generation; join full coloration to gate histories, first-record startup/tails and capacity-near writes |
| Literal input/output AntiAlias | Setter, recurrence and dispatch joined to persistent input history/recording and output playback; both input cutoff dispatch branches checked | Upstream RecordSpeed/speed/event producers, complete held/fixed head activation; decide literal versus improved sample-rate-aware option |
| Tape coloration and write clipping | Complete input/output DSP chains joined to playback/record; independent filter coefficient laws, persistent preset slices and Time/clipping producers checked | Full preset update, alternate Time modes, monitor/routing producers and reset/control transitions |
| MonoPlate | Constructor and complete independent thirteen-delay/filter/modulation recurrence checked; both tables, amount publication, seeded wraps and connected output chain pass | Full Time/preset event producers, reset/clear/preset transitions, long-tail characterization and deliberate Rack-rate policy |
| Pot/CV speed tables and slew | All table modes, calibrated V/oct law, quantizer/snap/tap-speed/slew, full pot deadband/deferred consumer and production ADC recurrence joined to asymmetric deck dispatch | Control-state/gate and physical CV producers, full speed/modulation-to-motion scheduling, actual calibration or deliberate Rack law; define cadence and native adaptation |
| Buttons, Record jack, clock and one-shot states | First-record bookkeeping, retrigger countdown/queue/cooldown/activation, V/oct reverse event, complete external-clock estimator, interleaved tap/clock/reset consumers, tap-span/division-limit slice and complete jack producer with joined GPIO edges | Consumer feedback in jack routing, physical first-edge/reset dispatch, between-pulse lost-clock behavior, latch/gate/arm, button chord priority, complete link transitions, connected audible activation and output pulses |
| Linked decks and auxiliary/normalled routing | Selected pointer publication, manual/static evidence and executed monitor copy/fade consumer, including endpoint state | Asymmetric two-deck forwarding matrix, unlink behavior, monitoring producer and connected mix, aux gain law and causal bounce schedule |
| Flutter, crinkle and touch | Scalar recurrence/release checked with test and original factory initialization; independent sine/seed/filter laws and forced uniform rounding endpoint checked | Actual noise-vector sizing, vector overload, modulation/touch producers, release and rate/cadence integration; explicit native entropy/libm policy |
| File load/save, tails and patch persistence | Scripts, preset parser, first-record tail bookkeeping and documented import policies | Actual sample/metadata fixtures for native/import tail policies, export/read/autosave and stereo pairing |
| Hardware adaptation and rates | Nominal 48 kHz documentation; distinct 49170.25390625 DSP and 49148 file constants | Driver evidence or measured timing; deliberate Rack rate/voltage policy, bounds and migration tests |
| Rack architecture and performance | Proposed handoff and repo conventions | Freeze IDs/schema, fixed internal scheduling, bounded allocation/event/storage policies, performance and context-lifecycle tests once implemented |

## Next sequence

1. Extend recording integration with the independently checked head-fade model,
   both boundary managers, linked aliases and full scheduling. Single-head gate
   activation/expiry/update and first-record tail writes are now joined. Input
   coloration/AntiAlias/history are separately joined to tape writes and final
   output; connect those effects to the event histories. Extend persistent
   fixtures with explicit execution boundaries and independent state checks.
2. Join the recovered speed tables and checked ADC/pot/slew/snap consumers to
   control-state/gate producers and motion scheduling. Event producers, reset
   transitions, callback cadence and sample-rate adaptation still need closure.
3. Execute control/clock/link event sequences against asymmetric decks and
   resolve the monitoring/routing producer and callback schedule.
4. Recover modulation and asset/tail semantics, then assemble the final native
   specification with traceable requirements, deliberate improvements and
   acceptance vectors. Physical measurements can refine the adapter separately.

## Evidence interpretation and corrections

Read the later `ENGINE_COUNT_GAIN_FINDINGS.md` for the approximately **0.81**
per-additional-engine multiplier. The 0.82 lead in `CONTINUATION_AUDIT.md` was
an earlier static interpretation and is superseded by executed evidence.

The persistent record reference reads original moved head/fade state at render
entry. Thus it independently checks arithmetic, same-tape ordering and stage
connection for those states, not all upstream event/transport generation.
Boundary-trigger laws and allocation have their own separate probes. All tests
remain offline instruction execution; no original appliance program is launched.

The plate model independently reconstructs both modulation tables and its state
recurrence, while using executed constructor coefficients/modes as fixture
parameters. The isolated output chain supplies mixed input; the connected record
suite joins actual tape rendering, output effects and writes. Agreement in these
slices does not prove whole-callback scheduling. Seeded plate
wrap tests use accelerated rates and some out-of-production depths for branch
coverage. See `PLATE_PROCESS_FINDINGS.md` and `OUTPUT_COLOR_PIPELINE.md` for exact
scope, provenance and reproduction commands. `CONNECTED_RECORD_OUTPUT_PIPELINE.md`
and `TIME_EFFECT_PUBLICATION.md` add connected and producer checks; recording
boundaries and full preset/control graphs remain open.

`INPUT_COLOR_PIPELINE.md` adds executed preset filter publication and an
independent input coloration model. `CONNECTED_INPUT_OUTPUT_RECORD_PIPELINE.md`
joins it to AntiAlias/history, tape writes/playback and output DSP. Its tanf
service is explicit host glibc, not original ARM libm. Coefficient-slice history
preservation does not prove the complete preset-update graph preserves state.
`MONITOR_MIX_FINDINGS.md` checks the isolated copy/fade consumer; its flags remain
disabled in the joined suite, so routing producers and integration remain open.

Do not sum counts from overlapping suites into an overall fidelity score. Counts
describe fixture coverage, and whole-tape comparisons include untouched cells.
Use per-feature evidence and explicit limitations when writing requirements.

`SPEED_TABLE_AND_SLEW_FINDINGS.md` closes all four table-generation laws with
53,248 checked values, the calibrated V/oct arithmetic with 20,480 values and
selected speed-state consumers with 3,149 comparisons. Actual calibration is
absent, `powf` is explicitly host glibc and the speed prefix stops before
flutter/touch. Speed-to-motion/event integration remains open.

`POT_SPEED_EVENT_PUBLICATION.md` adds complete pot-speed consumer sequences
and the production ADC recurrence/constructor slice joined to both deck speed
calls. Hardware reads use synthetic MCP samples. Numeric state/gate fields are
supplied; full linked-event consumption, physical CV and motion scheduling
remain open.

`ACTIVE_FADE_BOUNDARY_FINDINGS.md` checks 720 already-active retriggers: the
diagnostic branch proceeds to reinitialization rather than ignoring the request.
It also joins independent head/fade generation to original ordinary boundary
allocation, 240 raw head renders and manager cleanup over 160 cases. Fresh
single-event boundary audio no longer needs a fade-endpoint oracle. Persistent
reentry, physical/wrapped seams, saturation, simultaneous fades, both boundary
managers and full coloration/scheduling remain open. The shared boundary helper
now preserves held zero speed; nonzero existing fixtures are unchanged.

`RETRIGGER_EVENT_AND_QUEUE_FINDINGS.md` checks pending-event suppression,
callback-counted delay queues, complete cooldown/activation and V/oct reverse
consumers. Documented delay milliseconds conflict with the directly consumed
counter; parser-to-shared-preset integration and deliberate Rack timing policy
remain explicit requirements. Synthetic steady-clock timestamps and fresh
pools do not establish physical or full-callback timing.

`EXTERNAL_CLOCK_ESTIMATOR_FINDINGS.md` checks complete clock averaging, rounded
timeout rejection/recovery, constructor timestamp, fresh preset-window resize,
direction and slew over 64,800 supplied pulses. System-clock boundaries are
synthetic. Resolution-zero averaging and zero-filled warm-up are explicit
compatibility decisions; clock-role producers and connected motion remain open.

`TAP_CLOCK_INTERACTION_FINDINGS.md` adds 216 interleaved sequences and 120
tap-span/division-limit cases. Tap and clock share their timestamp; event 9
primes the next tap while retaining clock history and speed state. The span
slice receives supplied region/division/live speed. Physical gesture priority,
linked reset dispatch and complete callback scheduling remain open.

`JACK_EVENT_ROUTING_FINDINGS.md` adds 20,224 complete producer cases with
inert event recorders, including 4,096 joined six-input GPIO transitions.
It checks A-before-B ordering and asymmetric linked clock/record forwarding.
Consumer feedback is deliberately suppressed; the full connected state graph,
button priorities and link-switch publication remain open.

`RECORD_GATE_EVENT_FINDINGS.md` checks Channel gate publication and real
Empty/Playback/Overdub state-method forwarding, including overdub release fade
activation/reversal and 2,304 subsequent moves. `setState` remains an explicit
recorder boundary; FirstRec, callback expiry and connected tape writes remain
open. A negative fade coordinate alone does not deactivate `Fade::move`.

`CHANNEL_SET_STATE_FINDINGS.md` removes the setState recorder boundary for
882 transition fixtures and 36 real Playback/Overdub gate round trips. Original
managers/fades and erase request publication execute with inert diagnostics
and recorded semaphore posts. FirstRec stopping, pool exhaustion/history,
erase-worker completion and event-to-callback audio integration remain open.

`FIRST_RECORD_STOP_FINDINGS.md` joins the complete first-record stop method to
state/region/head consumers over 960 fixtures. Equality at four times MinLength
is accepted; shorter requests retain FirstRec with a pending flag. Tail metadata
publication is connected, but pending-stop callback handling, counter growth,
tail audio writes, linked aliases and capacity transitions remain open.

`PENDING_FIRST_RECORD_STOP_FINDINGS.md` checks 90 callback movement/decision
cases and six persistent event-to-callback sequences. Pending retry requires
strictly greater than four times MinLength; capacity equality triggers real
completion first. Execution stops before recordInput, so connected first-record
audio, trailing writes/reset and alias-aware callback integration remain open.

`FIRST_RECORD_TAIL_WRITE_FINDINGS.md` joins actual recordInput to completion,
input history, tail movement and reset over 5,296 slices. Tail equality writes;
exceeding stored extent resets and skips the block, leaving a frame-dependent
unwritten remainder. Full coloration/startup, linked aliases, erase histories
and capacity-near writes remain separate integrations.

`WRITE_FADE_EXPIRY_FINDINGS.md` checks 2,304 original movement/dispatch/expiry
cases with inert recordInput observers. Expiry occurs after record dispatch;
below-zero resets the record pool, above-254 preserves it. Actual crossing-block
envelope/tape arithmetic, event-to-write integration and manager updates remain
open despite the separately verified lifecycle.

`WRITE_FADE_CROSSING_FINDINGS.md` checks complete recordInput at both moving
fade bounds with 864 zero-discrepancy tape fixtures. Lower suffix preserves old
tape; upper suffix uses ordinary recording feedback. Callback expiry and
crossing arithmetic still need a joined event-to-write history; stationary
out-of-range, extreme/multiple-head and boundary interactions remain open.

`GATE_WRITE_HISTORY_FINDINGS.md` supersedes that single-head integration gap:
48 persistent real gate/state histories join input history, head motion, actual
crossing writes, asymmetric expiry and both manager updates over 702 slices.
Tape agreement is exact. The strengthened suite independently checks fresh-head
fade initialization and persistence, removing its original endpoint oracle. Whole
callback scheduling, aliases, active fade production in event histories, multiple
heads, boundaries and coloration remain open. Inactive channel fade coordinates
continue advancing after reset, but their flag disables audible envelope use.

`TAP_FADE_STATE_FINDINGS.md` supplies independently carried active fade states
for 960 complete trigger fixtures and 6,720 motion/update blocks, joined to
4,772 original gain renders with zero discrepancy. Integer-interval triggering,
the downward initialization bias, signed motion and asymmetric cleanup are
checked. Stationary out-of-table render reachability/policy, already-active
triggering, simultaneous fades and connected manager/boundary event histories
remain open.

`SEAM_FADE_AUDIO_FINDINGS.md` extends independent state-to-audio validation to
720 physical/wrapped/overlapping seam cases and 1,368 head renders through manager
cleanup. Inherited source fades advance again when the replacement head moves;
144 simultaneous-fade heads check the resulting gain products. Ordinary mode
was rerun successfully. Independent persistent reentry, saturation, multiple old
head iteration, both managers, full coloration and upstream scheduling remain
open; single-old-head seam states/audio are no longer an endpoint-oracle gap.

`SATURATED_SEAM_AUDIO_FINDINGS.md` adds 1,440 four/five occupied-slot cases and
7,080 head renders, including 696 failed replacement attempts. Outgoing fades
persist when allocation fails; physical replacement can exhaust the last slot
before the selected transition attempts allocation. Independent ordered fade
states and raw accumulated mix are checked through manager cleanup. Persistent
reentry and full iteration/movement of multiple old heads remain open; the
single-source occupied-slot audio contract no longer relies on a state oracle.

`FLUTTER_STATE_FINDINGS.md` checks 24 persistent scalar-generator sequences and
129,024 original MT19937 draws with independently carried integer/floating state.
Both-depths-zero returns exact unity while phases/RNG/filters continue evolving.
Frame argument controls periodic phases; supplied noise-vector length controls
random draws/filter evolution. Supplied reconstructed table and test coefficients
retain an explicit factory-initialization gap. Time/preset, touch, whole speed
and native rate/cadence integration remain open.

`FLUTTER_FACTORY_FINDINGS.md` closes factory arithmetic initialization for an
explicit entropy/libm boundary: all table entries, 624 seed words, filter
coefficients/default states and 384 persistent scalar calls are checked. Five
forced RNG endpoint cases execute the rare upper fallback, with exact filtered
output. Mode 1's literal numerator does not cancel DC; a conventional high-pass
substitution is unsupported. Actual vector sizing, depth/touch producers,
whole-speed/motion scheduling and native seed/rate policy remain open.

`SPEED_MODULATION_JOIN_FINDINGS.md` identifies the static two-deck buffer setter,
N-element noise publication, complete speed-call ordering and scalar touch
branches. The prepared independent joined probe has not run: WSL launch failed
automatic approval review because the app refresh token was revoked. These
static findings do not close executed sizing/touch/speed integration. Resume
with `probes/probe_speed_modulation_join.py` after authentication recovery;
driver cadence, upstream gestures/publication and motion remain open.

Static continuation identifies inline previous/current speed and factor aliases,
playback-then-record motion traversal, and the outer left-worker/right-calling-
thread schedule. Linked right reads select left speed/factor fields; the static
thread launch/join does not prove a consistent prior/current snapshot. The native
specification must define deterministic linked-deck publication and its tests.
The pending modulation probe now includes the unlinked inline alias as well as
the detached fixture; neither new variant has executed.

The same static trace corrects the touch boolean's initial interpretation:
Channel+452/+461 are the held state/rising edge of Button at +448, not recording
state. readButtons publishes this object before deck processing; checkCapTouch
is a separate pin-30 mode-disable path. The pending probe now executes original
Button construction/edge publication. Forced Time depth publication was already
checked separately; full preset transition and joined depth/DSP histories remain
open. See `SPEED_MODULATION_JOIN_FINDINGS.md` for the explicit evidence boundaries.

`TIME_SELECTOR_STATIC_CONTRACT.md` traces all six dispatch entries, the three
preset-selected third functions, and the unforced nonzero-mode diversion that
the existing probe stops before. Selector 0 and deferred division publication
index from cached Time data, not the new argument, and have no local length-1
clamp. Production endpoint/sentinel provenance needs execution and native bounds
policy. Ordinary cycling uses modulo 3; asset/menu paths temporarily install
other selectors and restore them. These new branches are static-only, and
complete event/load/cancel histories remain open.

Archived ELF data extraction now confirms the runtime clock vectors have leading
zero plus the documented musical values (17/11/7/8 entries). The default
quantisation vector has seven entries 0,2,4,8,16,32,64; its constructor allocates
exactly seven words, with no extra endpoint sentinel. Table artifact fields now
separate documented and runtime arrays. This narrows the table provenance gap;
the cached-raw endpoint and zero-value consumer behavior still require original
execution and native policy. Data extraction is not an ARM-probe pass.

All four preset quantisation arrays are now extracted as complete byte lists
including zero (16/11/6/7 entries), with their jump-table/copy extents traced.
Preset selector 0 installs All, while the constructor defaults to powers of two.
Zero selection clears the start flag, but the independent length flag remains.
Region code computes its grid before start-flag gating; existing region probes
exclude zero divisors/grids. An explicit native bounds/no-valid-grid fallback is
proposed with acceptance inputs in `TIME_SELECTOR_STATIC_CONTRACT.md`; full
original zero-entry/event execution remains required before parity claims.

The production Time ADC endpoint is now narrowed using previously executed
constructor/recurrence evidence: alpha=.5, integer truncation and constructor
zero preserve cached 0..4094 for valid 12-bit inputs. A new independent numerical
check passes 8,190 recurrence endpoint and 36,855 table-index checks; all nine
runtime lists are in bounds over that domain. Direct/altered-state 4095 still
selects one-past-end. This is numerical-domain evidence, not a new ARM run;
alternate writers/restoration, joined Time dispatch and zero-grid behavior remain
open. Native unsmoothed full scale needs an explicit safe indexing policy.

Production Time input provenance is now narrowed further: main ELF byte checks
verify constructor and initHardware pin blocks match, with Time pins A=7/B=2,
both in the valid MCP range. Static readMCP decoding masks arbitrary response
bytes to 0..4095; an independent check covers all 65,536 response pairs. The
Channel cached-Time zero initializer is also traced and its literal checked.
These checks pass as `PASS_DATA_AND_NUMERICAL_ONLY`, not ARM execution. Normal
read publication joins the cache offsets used by Time lookup; complete writer/
restoration analysis and dispatch execution are still open. Details and exact
addresses are in `TIME_SELECTOR_STATIC_CONTRACT.md`.

`TIME_MENU_LIFECYCLE.md` now traces file browsing's two-deck selector save/
restore and preset browsing's one-deck guarded save/restore. Both active
predicates exclude statuses 0/6; inMenu specifically means status 3. Exit
branches and their distinct semaphore posts are recorded, along with runIO's
Time-before-buttons order and the absence of local accepted-value restoration
or setter calls in those menu routines. Native transaction ownership, pickup,
idempotent reentry and stale-result rejection are proposed with acceptance
histories. These are static contracts and native design proposals, not executed
menu/cancellation coverage; worker order and full gesture priority remain open.

`FILE_LOAD_COMPLETION_STATIC.md` adds the main file consumer's status dispatch,
load-request tap shutdown before success, completion-time stereo target choice,
and separate L/L-12292/L-9834/min(L-12293,12292) extent publication. It traces
Playback, region and conditional retrigger activation and separates those
constants from soximport's 49148 Hz conversion. No new execution coverage is
claimed. Short-file bounds, worker status/data visibility, actual tail samples,
stereo target changes and failure/cancel histories remain open; transactional
replacement preserving old audio on failure is proposed as a native improvement.

`IMPORT_TAIL_STATIC_CONTRACT.md` connects audio_load's cleared destination and
maximum selected-deck read extent to its inlined tail handling and shared length
publication before status 6. It distinguishes no append, zero append and forward
in-place beginning copy, including capped capacity and standalone-versus-inline
invalid-policy differences. No new ARM execution is claimed. Short/empty files,
copy overlap/vector behavior, exact filename tags, unequal stereo files,
conversion failures and cross-process publication still require closure fixtures.

Import tag provenance is now extracted and traced: global setup installs
lowercase `{n}`, `{s}`, `{c}` in that order, selecting no append, silence and
beginning copy. Static substring search takes the first descriptor match;
no-match defaults to policy (folderIndex!=0). Hash-checked ELF data and nine
independent string-model cases pass, without original ARM execution. Folder 0's
directory-tree meaning, actual filename histories and full import fixture
closure remain open. See `IMPORT_TAIL_STATIC_CONTRACT.md` and
`evidence/import_tag_byte_checks.json` for exact scope.

Folder-0 provenance is now connected statically: mapFiles inserts the verified
`saved/` literal first and filters it out of the subsequent folder listing.
This ties the no-tag defaults to saved recordings versus other mapped folders.
`FILE_BROWSER_STATIC_CONTRACT.md` records reverse saved-file listing, ordinary
listing elsewhere and the twelve-filename local reader limit. Real directory,
changed-list, missing-folder and save/export round-trip fixtures remain open;
expanded limits and stable asset identity are explicit native design choices.

`EXPORT_ROUND_TRIP_STATIC_CONTRACT.md` joins FileSaver's selected stored-length
pointer to the local and linked workers' raw [0,L) write loops. Linked export
uses one extent for both deck arrays. Hash-checked ELF relocation inspection
resolves the write import and passes as static data coverage only. The existing
tail is retained in the requested raw range, explaining the no-append saved/
import default and L-12292 logical period under an unchanged-length successful
round trip. No end-to-end round trip is claimed. SoX gain/encoding, concurrent
tape snapshots, changed link state, invalid lengths, command failures and
durable completion remain open; bounded immutable snapshots and verified atomic
file publication are proposed native improvements.

Export completion is now narrowed further: both workers call the resolved
system import, overwrite its return register immediately and publish status 3
on the normal continuation. File open failure sets stream state; write/close
results do not gate that continuation in the traced slices. Main acknowledges
3 by clearing saving/status and handles 2 as an error, without proving that the
worker publishes 2 for these failures. Ten explicit native acceptance cases
cover marked tail round trips, channel ownership, immutable generation capture,
invalid extents, staged failures and stale completion. These are specification
gates, not executed native tests; runtime/library exceptions remain open.

`AUTOSAVE_AUDIO_STATIC_CONTRACT.md` establishes the audio writer's separate
deck mappings, startup semaphore gate, alternating 1/2 raw .dat slots and
post-close tracker byte. Unlike manual export, it rereads live stored extent
after every cell write and calls sched_yield between writes, so neither one
length nor one tape generation is captured by this loop. Seven hash-checked
ELF import resolutions and four literals pass as static data only. Reader
selection/fallback, autosave_data fields, startup activation, runtime failure
histories and matching audio/settings generations remain open. A verified
generation manifest is proposed as a native improvement; complete patch
persistence and bounded snapshot policy are still specification requirements.

`AUTOSAVE_RESTORE_AUDIO_STATIC.md` connects the main reader to the writer's
per-deck slots. Tracker byte acceptance is exactly '1'/'2'; hash-checked words
and all 256 independently modeled values pass. The selected file gets a type
gate followed by a 118008000-byte read; the local read slice does not derive
extent from gcount or clear the array. A traced catch body logs and continues
to settings preparation, with no local alternate-slot retry. Settings extent,
constructor defaults, error routing and startup activation still need closure;
this is static reader progress, not an executed restore fixture or parity claim.

`AUTOSAVE_RESTORE_STATE_STATIC.md` connects parsed settings length to nonblank
Playback and L-12292/L-9834/min(L-12293,12292) extent installation, independent
of raw read count. It traces fifteen root/nested values through mode and Time
publication, conditional retrigger and preset force_replace. Seventeen literal
keys/groups are hash-checked ELF data; no new ARM/Hjson execution is claimed.
Producer capture, malformed/missing defaults, actual asset/metadata bounds and
settled preset/control/head state remain open. Native restore requires explicit
versioned extents and verification against the asset before publication.

`AUTOSAVE_SETTINGS_WORKER_STATIC.md` establishes separate mapped blank/length/
mode reads during Hjson construction, a five-second requested interval between
passes and independent slot alternation. Settings-worker hash, three PLT/GOT
imports, six keys and the sleep literal pass static inspection; audio-worker
checks remain passing. No coherent settings producer snapshot or common audio/
settings generation has been demonstrated. Main publication, all remaining
serialized fields and tracker/failure histories remain open; native generation
capture and verified asset manifests require concrete implementation decisions.
