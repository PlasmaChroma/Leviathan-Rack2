# Startup audio selection and raw read

Static slices of Application::readAutosave, 0x2b238, from the hash-checked main
ELF. This report covers audio slot selection/read and its local error joins;
the large settings parser and complete startup activation are not yet recovered.
No ARM, stream or restore session was executed.

## Deck paths and tracker gate

The routine starts with Channel pointer r7=Application+72 (0x2b250), advances
it by 173368 at 0x2c03c–0x2c04c and loops at 0x2c054. Its sentinel is
Application+0x54ab8, so the ordinary outer loop visits left and right channels.
The companion raw-data pointer source advances with that loop.

At 0x2b27c the literal descriptor is 0x8fc34, autosaveAudioDir. The first
constructor uses descriptor-24=0x8fc1c, autosaveDir. The routine appends the
channel name and slash (0x2b294–0x2b2d4), then appends autosaveTrackerName
0x8fc94 through the helper at 0x2b314–0x2b324. This agrees with the writer's
per-deck directory and tracker layout in AUTOSAVE_AUDIO_STATIC_CONTRACT.md.

It opens tracker fstream with mode 12 at 0x2b328–0x2b334, then calls
istream::get(char&) into stack+35 at 0x2b34c–0x2b354. It loads that byte,
subtracts 49 and branches on unsigned difference>1
(0x2b358–0x2b364). Thus only ASCII '1' and '2' are accepted. The invalid branch
0x2d61c constructs invalid_argument with the audio-invalid-tracker message
and throws at 0x2d674. There is no attempt at the other slot on this local
invalid-byte path. Failed get() and its destination-byte behavior are not
established by these instructions; do not treat EOF as a known zero byte.

`tools/verify_restore_tracker_bytes.ps1` checks the relevant original words,
branch target and all 256 byte values against an independent acceptance
predicate. It passes as PASS_BYTES_AND_INDEPENDENT_PREDICATE_ONLY: two accepted,
254 rejected. The enumeration tests a byte already supplied, not file IO.

## Selected file check and read

The accepted tracker byte is appended to the base path at 0x2b388–0x2b3a8;
the extension descriptor 0x8fc64 is appended at 0x2b3ac–0x2b3c0. It selects
the matching `.dat` file rather than deriving a new slot from modification time.
Filesystem::status runs at 0x2b408. Its returned low type byte is transformed
with `(byte+1)&255` at 0x2b40c–0x2b41c; transformed values<=1 take the
runtime_error path 0x2d6d0 (branch at 0x2b448). This is an existence-style type
gate, not a decoded-audio checksum or read-size check. Exact library enum/error
semantics and pathological file types still require the matching runtime.

For the continuing path, a selected-file fstream opens with mode 12 at
0x2b4b4–0x2b4c0. The requested read capacity is 0x0708a8c0=118008000 bytes,
loaded into a stack local at 0x2b270–0x2b278. The reader passes it to
istream::read at 0x2b4c4–0x2b4d4. Its destination is the pointer at the
companion base-0x500; for the initial deck this is Application+172044,
equivalently Channel+171972. The next instruction overwrites r0 with the stream
object and calls its destructor. There is no gcount/returned-size calculation
between that read and destruction, nor a stored-extent update in this slice.

Unlike audio_load's cleared destination plus returned extent, this local slice
does not clear the array before reading. Constructor initialization may provide
that state; it must be traced separately. A short raw file cannot be assumed
to regenerate logical extent from bytes read here. The later settings fields
and any constructor defaults remain consequential to restoration correctness.

## Error joins and scope

The inspected audio exception catch region at 0x2d598–0x2d5ec obtains exception
text, logs at severity 2 and joins 0x2b508, where settings/default preparation
begins. It does not locally retry another audio slot. Exception table routing
and all runtime failures remain unexecuted, so this is a traced catch body,
not a guarantee that every failed stream operation reaches it.

Required evidence still includes missing/empty/invalid trackers; either selected
slot absent with the other valid; short/partial/over-capacity data; a valid
tracker pointing to unverified contents; independent audio/settings slot
generations; fresh constructor array state; restored extent validation; and
Playback/record/head activation after settings apply. Native restoration should
verify a complete generation before replacing live state and fall back to a
previous verified generation on corruption. That is an explicit improvement;
it must not be attributed to these firmware slices without further evidence.
