# Autosave audio writer and tracker publication

Static inspection of `autosave_audio`, SHA-256
`5449c24265304400012bb37d0250ca7dae0cd38c7c5f0e848a3bdc4dcd873622`.
`tools/verify_autosave_bytes.ps1` verifies its hash, file-backed literals and
seven ARM PLT/GOT import relationships. Result: `PASS_STATIC_ELF_DATA_ONLY`.
No original ARM execution, worker session or file operation is claimed.

## Startup gate and deck independence

Main waits on a semaphore at 0x1302c before launching deck 0 through a thread
state carrying argument 0 and runChannel=0x14a98 (0x13074–0x130b8), then calls
runChannel(1) directly at 0x130d4–0x130d8. Global setup names AutosaveStart at
0x14070–0x14078. This is a startup gate; the runChannel write loop traced below
does not wait on that semaphore before each pass. Complete shutdown/signal
behavior and startup restore ordering remain separate tasks.

runChannel maps a 118008000-byte tape array at stack+240
(0x14ba4–0x14bb4) and a four-byte stored-extent object at stack+276
(0x14c60–0x14c70). Their data pointers are stack+244 and stack+280. Each deck
gets its own mappings from the channel-name construction. The extent name base
is the descriptor at 0x2f2d8 (literal 0x159b8), used at 0x14bd4–0x14be0;
global-name provenance and the data literal `EoD_` are in the archived evidence.
The worker is not passed a linked pair snapshot or one common export length.

## Slot selection and file ownership

Global autosaveDir at 0x2f308 is initialized from `autosave/`
(0x13f74–0x13f80). runChannel concatenates this descriptor with the deck name
at 0x14c8c–0x14cb4, appends slash at 0x14cb8–0x14cd0 and retains the base
directory string at stack+72. Initial slot byte stack+31 is ASCII '1'
(0x14d0c–0x14d10). The two filesystem status branches at 0x14d28–0x14d94
need runtime fixtures; this report does not claim automatic directory repair
always succeeds.

For each pass, the worker copies the directory string, appends the slot byte
at 0x14e04–0x14e14, and appends autosaveAudioFileExt at 0x2f320
(0x14e18–0x14e28). The verified extension is `.dat`, giving paths of the form
`autosave/Left/1.dat` and `autosave/Right/1.dat`, then their slot-2 counterparts.
These are raw sample writes rather than the SoX WAV export path.
After tracker handling, 0x15278–0x152a0 changes '1' to '2', otherwise to '1',
and branches back into the next pass at 0x152ac. No timed sleep is present in
this ordinary loop; pass time depends on sample count, IO and scheduler behavior.

## Live extent and sample reads

At 0x14f90–0x14fa4 the worker dereferences the mapped extent word and, if its
signed value is positive, starts index r7 at zero. Each iteration
(0x14fb0–0x14fc0) writes four bytes from tapeData+4*index through ostream::write
(PLT 0x124e0). It then calls sched_yield (PLT 0x1263c at 0x14fc4), increments
index and **rereads the same live extent word** at 0x14fc8–0x14fd0. It continues
while that newly read signed extent exceeds index (0x14fd4–0x14fd8).

This is not manual export's one loaded length for its entire raw write loop.
With a growing extent the pass can grow, and with a shrinking extent it can stop
after cells already written. Each source cell is read during its individual
write, so the instructions do not capture all samples at one instant either.
The scheduler yield is not a fixed delay or an audio-tick synchronization barrier.
No conclusion about a coherent tape generation follows from it.

r8 is initialized to 0x01c22a30=29502000 at 0x14dc4–0x14dc8. Before subsequent
cell writes 0x14fa8–0x14fac tests index==r8 and branches to 0x15670. That branch
uses an exception-related construction path; do not model it as normal clipping
or successful capacity truncation. Runtime error consequences remain unverified.
Nonpositive initial extents bypass writes and still reach closing/tracker code.

## Close, tracker byte and alternating passes

The sample stream closes at 0x15000 (basic_filebuf::close, PLT 0x128c4),
followed by destruction/cleanup. Its returned pointer is not tested before the
ordinary tracker sequence. Open null-return branches set stream state through
basic_ios::clear (0x15358 for samples, 0x15348 for tracker); original library
exception-mask behavior remains unexecuted.

The specialized concatenation at 0x149c0 adds autosaveTrackerName at 0x2f278
(0x14d04–0x14d14). Its literal is `tracker.txt`, naming a file within that deck's
base directory. After closing the sample file, the worker constructs/open this
tracker with mode 52 (0x15198–0x151c0). It passes the current ASCII slot byte to
ostream::put at 0x151d8–0x151e0, then closes the tracker at 0x15208. The put
result and close result do not gate slot alternation on this ordinary path.

Alternating two sample files and publishing one tracker byte supplies a
recoverable-slot mechanism, but does not alone prove crash safety, durable IO,
sample validity or consistency with the separately saved settings. There is no
observed checksum, generation identifier or atomic rename in this local write
sequence. The reader's choice of slot, missing/truncated tracker behavior and
fallback rules still need decomposition. Neither this report nor the tracker
byte proves which playback/tap state survives startup.

## Specification consequences and required evidence

Native persistence must declare a snapshot generation for both tape arrays,
stored/logical/tail extents, preset/control state and linked mode. Use bounded
background work and publish a manifest only after its referenced files are
complete and verified. Preserve the previous valid generation when saving or
restoring fails. These are proposed improvements, not recovered guarantees.

Closure fixtures must cover extent growth/shrink during a pass, live sample
mutation, zero/negative/over-capacity extent, different deck lengths, interrupted
sample write and interrupted tracker publication, missing/corrupt slots,
audio/settings generation mismatch, and startup restore while IO is active.
The final Rack specification additionally needs a patch asset destination,
path relocation/missing-asset policy, schema migration, cancellation and a
measured worker/audio memory budget. Startup readers, autosave_data fields and
main's restore consumer remain open; this report establishes the writer only.
