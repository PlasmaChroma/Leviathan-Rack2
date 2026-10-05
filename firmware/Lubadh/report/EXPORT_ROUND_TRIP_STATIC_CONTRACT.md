# Export extent and saved-folder round trip

Static evidence from `lubadh_main`, `audio_save` and the archived `soxexport`
script. Worker SHA-256:
`d51b574a71cca3a7d66a4b7803ad8f9e9305e69461d2217cc108c17bb506aa23`.
No appliance binary or script was launched. This connects request and worker
instructions to the import contracts; it does not claim executed round trips.

## Request length and selected source

`Channel::FileSaver::save` at 0x3932c sets owner C+576 to 1 and C+660 to 92.
It reads Application+346812 to choose its local SaveData at saver+4 or linked
SaveData at saver+104 (0x3934c–0x39360). It then reads the selected LinkData
pointer at C+232, loads its stored-length pointer at LinkData+52, dereferences
that pointer and writes the value through SaveData+32 (0x39364–0x39370).
This is the stored extent, rather than the logical period at C+76 or the
selected start/end region. See `BOUNDARY_SOURCE_FINDINGS.md` for the pointer
and inline aliases; `FILE_LOAD_COMPLETION_STATIC.md` traces imported extent
publication through C+88 separately from C+76=L-12292.

The constructor at 0x3bbac connects saver+32 to the named shared save-length
object (literal 0x3bdb8=0x94dd8, named sampleMemSaveLengthBase in main symbols).
The linked length object is saver+132. The corresponding SaveData+32 pointers
are saver+36 and saver+136. Worker global initialization installs
sampleMemSaveLengthBase at 0x2e290 using the `SampleSaveLength` literal
(0x128d0–0x128e0). The local worker constructs that shared object at stack+100
(0x12dd8–0x12df0), whose data pointer at stack+104 is read at 0x12f3c–0x12f4c.
The linked worker constructs its shared length at stack+168
(0x13638–0x13650), then reads stack+172 at 0x137cc–0x137e4.

The request posts its semaphore at 0x39374, then writes status 1 at
0x39378–0x39380. A worker also writes status 1 after waking. These instructions
do not establish a snapshot, lock, completion acknowledgment or release/acquire
ordering for concurrent audio edits. Link selection can change independently;
the current selected LinkData determines the requested extent.

## Raw sample range

The local worker tests signed length r5>0 at 0x13128–0x13130, initializes its
index to zero, and writes four bytes from audioData+4*index at
0x13134–0x13150 until index equals r5. The raw audio mapping was constructed at
stack+172 with capacity 118008000 bytes (0x12e40–0x12e58); its data pointer is
stack+176. There is no length clamp in this write loop.

The linked worker loads one common length into r7 before iterating deck IDs.
The two IDs are the words at 0x13b08 and 0x13b0c: 0 and 1, copied together to
stack+32 at 0x137e8. The outer loop consumes the words through stack+0 and stops
at stack+40 (0x13808–0x13818, 0x13b80–0x13b8c). Each deck uses its own shared
audio mapping, with descriptor stride 36. For each deck, 0x13a74–0x13aac tests
signed r7>0 and writes four bytes per index from zero through r7-1.
It does not derive a second extent from the other deck's stored-length word.

Both loops call PLT 0x12108. `tools/verify_export_bytes.ps1` hashes the ELF,
decodes that stub's address calculation and resolves its GOT relocation at
0x2e01c to `_ZNSo5writeEPKci` (ostream::write). The check passes as
`PASS_STATIC_ELF_DATA_ONLY`; it executes neither ARM nor the stream library.

Consequently the loop requests raw tape cells [0,L), including whatever tail
cells lie within stored extent L. The loop does not render playback heads,
apply playback speed or crop to the selected looping region. Preservation of
those bytes through stream writes and subsequent WAV conversion is still
unverified. Invalid lengths, concurrent tape mutations and unequal linked
extent histories require fixtures.

## Script conversion and round-trip implication

The archived `extracted/scripts/soxexport` requests float32 raw input at 49148
Hz. Left or Right uses `remix 1 1` to create dual mono. Linked converts each
raw deck into a separate stereo file with `remix 1 0` and `remix 0 1`, then
mixes them with `-m` into Linked.wav. Exact SoX mix gain, output encoding,
quantization and failure behavior need the matching dependency or marked
audio fixtures; the shell text alone does not prove bit-exact export.

After mounting, the script targets `/media/usb/samples/saved`. Its next filename
is derived using `ls | grep ".wav" | sort -nr | cut -f1 -d.` and arithmetic
LAST+1, then copies the WAV to `<number>.wav`. This is a shell listing algorithm,
not a validated file allocator. The script has no explicit `set -e` and does
not check each conversion/copy result. Worker status 3 stores are visible at
0x13274 and 0x13c1c, but this report does not equate status 3 with a verified
durable USB save. The normal command-return path is now resolved below;
exception/unwind and runtime failure histories still need execution coverage.

## Completion and error signaling

The expanded hash-checked ELF inspection resolves these imports directly from
their ARM PLT encodings and `R_ARM_JUMP_SLOT` relocations:

| PLT address | Imported operation | Relevant worker call |
|---|---|---|
| 0x121ec | basic_filebuf::open | 0x130e0 local; 0x13a1c linked |
| 0x123a8 | basic_ios::clear | 0x13100 local; 0x13a3c linked |
| 0x1245c | basic_filebuf::close | 0x13174 local; 0x13ad4 linked |
| 0x1233c | __basic_file destructor | 0x13180 local; 0x13ae0 linked |
| 0x1212c | system | 0x13240 local; 0x13be8 linked |
| 0x123f0 | basic_filebuf destructor | unwind paths 0x1346c / 0x13dd0 |

Open uses mode 52. A null open result takes 0x133cc–0x133d8 or
0x13cec–0x13cf8: OR 4 into the stream state and call clear, then join the
ordinary cleanup and write-loop path. A nonnull result clears state to zero.
Whether the library throws depends on its exception mask and implementation;
the static control flow does not establish that every open failure stops export.
Each write's returned stream reference is unused by the loop. Neither close's
returned pointer nor a later stream status test gates the subsequent command
on the inspected ordinary path. Both destructors run before that command.

The worker builds `sh scripts/soxexport ` followed by its channel name, passing
the command string to system. Local 0x13244 immediately reloads r0 from
stack+208; linked 0x13bec reloads r0 from stack+312. Both overwrite system's
return register without testing, storing or interpreting it. The data checker
verifies both original BL targets and the following reload words. After string
cleanup, status 3 is written at 0x13274 or 0x13c1c unconditionally on this
normal continuation. A failed system invocation or nonzero child exit therefore
does not itself select another status in these instructions. This finding
concerns normal returns; signals, C++ exceptions and process crashes have
different paths and remain unexecuted.

Main FileSaver::process at 0x39d4c reads both local and linked status objects.
For status 3, 0x39d80–0x39d90 / 0x39db4–0x39dc4 clear owner C+656, saving flag
C+576 and that status word. Status 2 takes the severity-2 log paths at 0x39ec4
or 0x39dd0, then clears saving and status and sets C+656/C+660 to 185/30
(0x39f80–0x39f98 / 0x39ea0–0x39eb8; literal pair at 0x3a010).
The consumer's status-2 handling does not establish that the export worker
publishes status 2 for shell, write or close failures. No such publication is
present in the ordinary slices just traced.

Thus the firmware completion indicator means the worker reached its normal
post-command continuation. A specification must not silently interpret it as
file existence, verified contents, successful copy, or durable persistence.
Native success-after-verification is a concrete improvement over this recovered
normal-path behavior. Failure injection must still verify the implementation.

`FILE_BROWSER_STATIC_CONTRACT.md` establishes saved/ as folder index zero.
`IMPORT_TAIL_STATIC_CONTRACT.md` establishes no-tag folder zero as no append.
Thus, for a successful untagged saved-file round trip with unchanged sample
count, L raw cells are reimported as extent L, and completion publishes logical
period L-12292. This matches the separation between logical loop and existing
tail. It is a static composition of contracts, not an end-to-end passing test.
Changing the same file to an ordinary folder selects silence append by default
and would instead give logical period L when capacity permits. Filename tags
can override either folder default. Short extents and capacity saturation have
the bounds described in the import reports.

## Native specification decisions and closure fixtures

A Rack implementation should explicitly separate an internal tape snapshot
with tail metadata from a user-facing region/audio export. Define whether
export saves full tape or the selected region, whether it preserves effects,
the sample rate/encoding and linked channel gain. An internal round trip can
retain the stored extent and metadata; a portable WAV export can declare its
tail policy. These are explicit native design choices, with the firmware's
full stored-range export retained as the compatibility behavior.

Use immutable worker snapshots, bounded extents, generation-tagged requests
and completion/error publication after verified file creation. Keep file IO
off the audio thread. Existing samples and a previous saved file should survive
failed export. These are proposed improvements, not recovered firmware guarantees.

Required marked fixtures: distinguish logical/tail cells and selected-region
cells; unequal linked extents and changed link state; mono dual-mono and linked
channel gain; save/reload in saved/ versus another folder and every tag; empty,
short and full-capacity lengths; active recording during save; failed stream,
conversion, missing USB and failed copy; semaphore/status races. Confidence in
these static joins does not close full instrument or Rack specification readiness.

### Concrete native acceptance cases

These are proposed specification gates, not passing tests. T=12292 and
K=29502000. Use finite, exactly representable float markers so sample ownership
can be checked independently of an audio listening judgment. Playback speed,
selected region and effect controls differ deliberately from the stored tape.

| Case | Fixture / action | Required native result and evidence |
|---|---|---|
| Full stored-range compatibility | Logical N=32768, extent L=N+T; cells [0,N) are +0.125, tail [N,L) is -0.25; selected region excludes both ends | Compatibility export decoded count L, original cell order and both marker runs; speed/effects do not alter this export mode |
| Internal saved-file round trip | Reload that export with explicit existing-tail metadata | Stored extent L, logical period N, tail markers retained; no second tail appended |
| External-file tail distinction | Same decoded L samples imported as external audio with silence-tail policy | Extent min(L+T,K), logical period extent-T; appended cells zero; no inference from incidental directory path in the native metadata path |
| Mono channel contract | Export local deck as float32 dual mono | Both channels equal the tape marker sequence; metadata declares rate and encoding |
| Linked channel contract | Left +0.125, right -0.25, same valid extent | Unity-gain left/right ownership with no crossfeed; this is a native target while firmware SoX mix gain remains unverified |
| Snapshot during recording | Pause export worker after accepting a generation; change tape cells, extent, link and region before resuming | Export contains one accepted generation's arrays and extents; no mixed generations; live recording continues within declared callback budget |
| Invalid extent | Request signed negative, above-capacity or missing-tail internal extent | Explicit error before any out-of-range access; previous file and live tape unchanged; no success indicator |
| Write / conversion / publication failure | Inject failure separately at each stage, including short writes and unavailable destination | Failed request identifies its stage; prior file survives; temporary output is not treated as saved audio; no success acknowledgment |
| Stale completion | Submit request A, supersede or cancel it, then deliver its worker completion | A cannot clear or report success for the current request; generation identity remains visible in state |
| Repeated save | Save twice to the same requested target with a failure during the second publication | First verified file remains readable; success follows complete replacement only; atomicity assumptions of the chosen storage API documented |

The native policy still needs a declared minimum valid internal extent, how
external empty/short files are represented, behavior for unequal linked lengths,
rate conversion, cancellation and snapshot memory bounds. Keep those decisions
explicit when drafting the final storage chapter; the table does not resolve
them through arbitrary firmware-default assumptions.
