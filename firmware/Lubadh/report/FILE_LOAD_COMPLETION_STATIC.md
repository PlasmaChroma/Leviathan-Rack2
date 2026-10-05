# File-loader dispatch and completion publication

This report interprets static main-ELF instructions only, pinned to SHA-256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
No new ARM execution or file conversion was performed. The separate audio_load
worker and real asset bytes are not covered by these local branch contracts.
See `TIME_MENU_LIFECYCLE.md` for menu ownership, cancellation and native policy.

## Status dispatch

F denotes the FileLoader object, C its owning Channel pointer F+0. The shared
status pointer is F+8; F+40 is a remembered status. `process` reads status at
0x3b7e0–0x3b7ec and dispatches unsigned status-2 through five entries:

| Status | Branch | Local work |
|---:|---|---|
| 2 | 0x3bb38 | Logs the string at 0x72764, then common status remembrance |
| 3 | 0x3ba38 | Updates break-load flag, selected folder and file indices |
| 4 | 0x3b9bc | Only remembers the entry status in F+40 |
| 5 | 0x3b9cc | Logs at severity 2, writes C+656/+660=185/30, exits, rereads status |
| 6 | 0x3b81c | Publishes imported extents/state to selected Channel(s), exits, rereads status |
| Other | 0x3b9bc | Only remembers the entry status in F+40 |

The completion/error interpretations of 6/5 follow their observable work, not
an assumed complete enum recovered from debug symbols. Status 3 is independently
the inMenu predicate; statuses 0 and 6 are excluded by active(). The worker's
full status transition graph remains to be recovered and joined.

The common remembrance store is 0x3b9bc. Completion calls exit at 0x3b9b0,
then rereads the current shared status at 0x3b9b4–0x3b9b8 before storing it.
The status-5 path does likewise at 0x3ba1c–0x3ba28. Do not model these as
unconditionally retaining the entry values 6/5; exit normally publishes 0,
and an asynchronous writer could intervene.

In status 3, F+260 becomes (previous remembered status F+40 == 1)
(0x3ba40–0x3ba68). Folder/file selection uses a shared list at F+76,
the other deck's selector-4 raw slot W+0x4c8 for folder selection and the owner's
selector-3 raw slot W+0x4c4 for file selection (0x3ba8c–0x3bb28). These are
accepted menu values, distinct from cached ADC Time W+0x4a8. The positive-entry counting and
one-based folder adjustment precede file lookup. Complete list creation,
empty/short lists, changed lists, endpoints and lookup bounds are unverified;
no claim of safe file indexing follows from the Time clock-table domain proof.

## Load request precedes successful completion

`load` at 0x3adec logs the current F+108 folder and F+112 file selections.
It calls the owner's killAllTaps at 0x3af24 before checking stereo mode
APP+346812. Stereo mode additionally calls the other deck's killAllTaps at
0x3af80. It marks the owning deck's selection byte, sets the other deck's byte
from stereo mode, copies the two selected indices into the shared destination
F+120, then posts F+188 (0x3af40–0x3af70).

Thus the local request path stops existing tap activity before import success
is observed. A native design that keeps old playback until successful atomic
replacement is a deliberate improvement, not this request's literal law.
No local status guard in this routine establishes that the selected list indices
or worker destination are still valid at the time of use.

## Status-6 extent and activation laws

Completion builds a temporary vector containing C and, if stereo mode is
nonzero at completion, C's other deck pointer C+28
(0x3b85c–0x3b8c0). Stereo mode is reread here; it is not a saved request field
in this slice. Each selected Channel gets the same length L read through
F+220 at 0x3b8d0–0x3b8dc. The local stores are:

| Destination | Literal value | Instructions |
|---|---|---|
| Word pointed to by C+88 | L | 0x3b8e0/0x3b8f0 |
| C+76 | L-12292 | 0x3b8e4/0x3b8fc/0x3b908 |
| C+80 | L-9834 | 0x3b8e8/0x3b8ec/0x3b904 |
| C+132 | signed min(L-12293,12292) | 0x3b8f4–0x3b900/0x3b90c |

The subtraction and signed comparison are important. No local lower bound is
applied before these stores. For L<12293, the last expression can be negative;
for L<12292, C+76 can be negative under the signed interpretation. ARM word
arithmetic wraps at 32 bits. Native size_t subtraction must not silently inherit
an unchecked underflow. Worker length validation may prevent these cases in
production; this main routine does not prove it.

The 12292 subtraction matches the stored-length overhead already recovered in
first-record completion. C+76/C+80/C+132 remain separate stored fields; their
ultimate units and every consumer should stay traceable rather than collapsing
all into a single duration or claiming every field is the tail length.

For each selected Channel, completion then:

- Calls setState(3), the previously checked Playback label, at 0x3b910.
- Calls setLoopingParameters(false,true) at 0x3b920.
- Sets C+647=1, C+44=1.0, C+36=1.0, C+728/+732=1.0/0.0 and C+736=0
  (0x3b928–0x3b958; literal d8 at 0x3bb98).
- Clears C+236 if selected preset field +64 equals 3 (0x3b954–0x3b960).
- If selected LinkData+140 is not 1, clears C+568/+572 and calls doRetrig
  (0x3b964–0x3b974). This conditional must be joined to its actual loop-mode
  producer and retrigger consumer before naming it more broadly.
- Writes the imported extent from the C+88 pointer into metadata+4
  (0x3b97c–0x3b990).

It frees the temporary vector, exits the loader and remembers reread status.
There is no processing of imported audio samples in this completion slice:
the worker is responsible for publishing data before its status reaches this
consumer. Static order does not establish interprocess visibility or atomicity.

## Script conversion is a distinct layer

The archived `extracted/scripts/soximport` first copies the selected source to
its load directory, then requests raw mono float32 at 49148 Hz from SoX.
For a source detected as mono, Left is converted and copied to Right.
For a source detected as non-mono, stereo mode converts source channels 1 and 2
separately; non-stereo mode uses `remix 1,2` and duplicates Left to Right.
The script uses `-G`. Its exact gain/resampling law, dependency versions,
conversion failure behavior and handling of channel counts above two are not
established by executing or merely reading these commands.

The main completion block's 12292/9834 constants and the script's 49148 rate
must not be merged into a claimed host-time tail duration without checking
audio_load's addTail/loadFile, their length publication and real sample fixtures.

## Required closure fixtures and native improvements

Original-byte fixtures must cover status 0..6 and out-of-range statuses,
remembered-status transitions into browsing, stereo mode at both request and
completion, asymmetric deck/preset/loop mode state, and L around 0,9834,12292,
12293,24584,24585 and tape capacity. Check every untouched field as well as
the listed stores, downstream region/retrigger state and actual rendered audio.
Join script/import tail-policy fixtures using marked beginning/end samples and
distinct stereo channels, including conversion failure and cancellation.

The proposed native workflow validates decoded audio, capacity and metadata on
a worker before publishing a replacement. It records deck targets at request
time and tags results with transaction generation. Failure preserves the old
tape and musical state; successful replacement swaps tape/metadata together,
then executes the specified playback activation. Extents are named separately
and checked before signed-to-size conversion. Cancellation invalidates stale
results. These are proposed improvements with required failure/short-file/
stereo-toggle/race acceptance histories, not verified firmware behavior.
