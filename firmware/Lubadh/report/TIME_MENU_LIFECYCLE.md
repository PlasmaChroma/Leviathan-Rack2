# Time selectors across asset and preset menus

Scope: static interpretation of the archived main ELF disassembly, SHA-256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
These are recovered local transition contracts and candidate native decisions,
not new executed histories. No hardware, script or original appliance process
was launched. See `TIME_SELECTOR_STATIC_CONTRACT.md` for setters and input bounds.

## State ownership

For each Channel C, W=C+172032. The current selector is W+0x4d0, cached ADC
Time is W+0x4a8, and the last accepted setter value is W+0x4ac. The other
Channel is the pointer at C+28; the selected LinkData pointer C+232 is a
different relationship. File-menu restoration follows C+28, not the LinkData
alias. Do not equate shared audio controls with shared menu ownership.

The file loader F is C+172528 (W+496). Its saved owner/other selectors are
F+252/+256. The preset loader P is C+172104 (W+72); its saved owner selector
is P+360. These are saved selector integers, not a saved bundle of DSP amounts,
accepted knob values or ADC states.

| Transition | Local selector writes | Saved state | Other observable local writes |
|---|---|---|---|
| Ordinary Time cycle, 0x3f67c–0x3f6d0 | Owner (selector+1)%3 | None | C+645/+648=1; selector reported at metadata+20 |
| FileLoader init, 0x39490–0x39548 | Owner=3, other=4 | Owner at F+252, other at F+256 | Posts F+44 semaphore before saving/installing selectors |
| FileLoader exit, 0x39564–0x3963c | Owner=F+252, other=F+256 | Reads both saved integers | F+260=1; status=0; C+645/+648=1 on both decks; serial indicator off |
| PresetLoader init, 0x39058–0x39134 | Owner=5 | Owner at P+360 | Inactive-only guard; clears two selection bytes, selects owning deck, posts P+224 semaphore |
| PresetLoader exit, 0x38e4c–0x38f28 | Owner=P+360 | Reads owner saved integer | status=0; C+645/+648=1 on owner; serial indicator off |
| modeReset, 0x393b0–0x39474 | Owner=1 | None | Resets additional mode/loop state, publishes metadata, sets C+645/+648=1 |

The ordinary-cycle formula applies to its reachable ordinary nonnegative
selector states. Do not infer general modulo semantics for corrupted negative
integers from the arithmetic slice.

File init clears selection bytes for both deck IDs, then marks its owner
(0x394dc–0x3950c). It posts its worker semaphore at 0x39510, then saves
selectors at 0x39528–0x39538 and installs 3/4 at 0x3953c–0x39540.
There is no local active guard in FileLoader::init. A direct repeated call can
overwrite the saved selectors with the temporary menu selectors; whether UI
priority prevents that call requires the complete gesture graph.

Preset init calls active() first (0x39064–0x3906c) and returns through the
already-active log path without replacing the saved selector. Its active()
predicate returns false for status 0 or 6, true for other values
(0x39038–0x39050). File active() uses the same status predicate
(0x39204–0x3921c). Both inMenu() predicates specifically mean status==3;
active and inMenu are not interchangeable.

Both exits restore selector integers before handling the status==3 branch.
For preset status 3, exit sets status to 0 and posts P+252; file status 3
sets status to 0 and posts F+188. Other statuses also become 0, without those
posts. Both subsequently reach their indicator/reporting writes. These
semaphore posts are cancellation-related synchronization evidence; they are
not proof of worker termination, joined cancellation or absence of later writes.

## Control publication and callback order

File-menu selector 3 publishes incoming saturated raw Time into W+0x4c4 and
metadata+44; selector 4 publishes W+0x4c8 and metadata+48. Preset selector 5
publishes W+0x4cc and metadata+52. All accepted setter branches also publish
that raw value to the shared per-Channel last-accepted W+0x4ac at 0x37d54.
They do not replace separate saved clock/dub/third-function raw slots.

The inspected init/exit/cycle routines do not locally call setTime, restore
W+0x4ac or alter Time's ADC alpha/current state. Setting C+645/+648 is a
reporting/update request, not locally the setTime force argument. Its complete
consumer graph must be traced before claiming a forced audible republish.

All four direct static calls to setTime in this ELF are the two-deck calls in
interpretADCs (0x2838c/0x283a0) and runIO (0x296a4/0x296b4). They pass
force=false. This direct-reference audit does not exclude indirect calls.
The setter itself only bypasses distance<=29 when force is nonzero
(0x37d60–0x37d68). For distance>29, ordinary Channel mode 0 supplies force
internally; other modes may take the separate deferred-division branch.
Therefore menu selector identity alone is insufficient to predict publication:
the Channel mode, last accepted value and any external force path also matter.

runIO reads buttons and ADCs, sets pot speed and regions, then calls both Time
setters before interpreting jacks, buttons and links (0x29628–0x296d0).
A menu entry/exit dispatched by interpretButtons later in that invocation
occurs after the Time publication stage. Its new selector can first affect a
subsequent Time stage, unless another verified path intervenes. This is a
static ordering fact, not a measured IO frequency or a complete worker schedule.

FileLoader::process also calls exit at 0x3ba1c, then rereads status into F+40
(0x3ba20–0x3ba28). Exiting is not exclusively a button path. Button paths
check inMenu before calls such as 0x2919c/0x29314; preset exit is called at
0x28a3c after an inMenu check. The surrounding chords and stereo selection
branches remain to be independently modeled and executed.

## Candidate native requirements and acceptance histories

These proposed native policies make the recovered ownership explicit and
resolve edge cases safely. They remain decisions for the full specification;
they are not certified firmware parity.

- Preserve independent musical Time amounts while menus temporarily use the
  knob. Menu exit restores selector ownership and leaves musical amounts
  unchanged. Use an explicit pickup state for the restored function; entering
  or leaving a menu must not apply its raw selection to an audible amount.
- File browsing owns a two-deck transaction with both saved selectors. Preset
  browsing owns one deck. Make repeated entry into the same live transaction
  idempotent, and reject conflicting file/preset ownership until it exits.
- Publish menu ownership and worker requests together in the native event
  order. Tag results with a transaction generation; accept only results whose
  generation still owns that request. Cancel restores once and prevents stale
  results from changing tape, preset or selector state.
- Keep worker IO outside the audio callback. Record and test selector state,
  audible state, menu state and worker completion as distinct state variables.

Required original-byte histories (currently unexecuted) and corresponding
native acceptance histories:

| History | Required observations |
|---|---|
| File entry from A and B with unequal saved selectors/amounts | Owner=3/other=4, correct saved slots, no unintended musical-value publication |
| File exit from each status 0..6 | Both saved selectors restored, exact status/post behavior, flags and break-load state |
| Preset entry from each ordinary selector and status 0..6 | Guard predicate, saved-selector preservation on active entry, only owner affected |
| Preset exit from each status 0..6 | Owner restoration, status/post behavior, other deck unchanged |
| Knob movement in menu then exit at unchanged raw, +/-29 and +/-30 | Accepted-value/deadband law, per-function amounts and subsequent pickup behavior |
| Entry/exit in the same IO tick as knob/jack/link events | Time-stage ordering, no assumed same-tick force publication |
| Repeated file entry and overlapping preset/file transactions | Firmware reachability/priority; native idempotence/conflict handling |
| Completion races cancellation and a new request | Worker synchronization contract; native stale-result rejection and exactly-once restore |
| modeReset during and after menus | Selector=1 local law, interaction with saved selectors and status ownership |

Byte execution of the local routines can close their branch laws, but cannot
alone prove concurrent worker order. The final module specification must join
these histories to file metadata/tail loading and complete event dispatch.
