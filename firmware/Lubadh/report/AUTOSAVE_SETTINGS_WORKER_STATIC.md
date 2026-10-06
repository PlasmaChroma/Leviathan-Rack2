# Settings worker capture and cadence

Static evidence from autosave_data, SHA-256
`3b48ea8b2d279a73667dda4871827a613d432e4819fddaecf259ec744eb9af13`.
No ARM/Hjson/file worker execution is claimed. Main application's publication
into this shared structure remains untraced; the findings here describe its
consumer rather than proving a coherent producer snapshot.

## Cadence and shared structure

runChannel at 0x16540 opens/maps a shared settings object. Its mapped data
pointer is stack+224, repeatedly loaded by the serialization path. It is not
copied into a local structure before the first root-field reads:

| Serialized value | Mapped offset | Field read / Hjson construction |
|---|---|---|
| blank | +0 byte | 0x167e8–0x167f4, bool |
| length | +4 word | 0x16828–0x16834, int |
| modes.monitoring | +8 word | 0x16870–0x1687c, int |
| modes.playLoop | +12 word | 0x168b0–0x168bc, int |

Each value is read when its own Hjson entry is built. The first blank and
length read are separated by MapProxy/value construction and destruction, not
one aggregate load. The later mapped reads likewise occur throughout the
document-building path. This is evidence against treating the consumer as
one atomic whole-structure snapshot; it does not independently establish when
or how the producer mutates each field, or a measured torn-state failure.

The worker loads an eight-byte literal {5,0} at 0x1678c and stores it as the
sleep request at stack+256. 0x167cc–0x167d4 passes the same address for request
and remaining interval to nanosleep. A return of -1 checks errno through
0x167bc; errno==4 retries the remaining interval. Other normal continuations
proceed to document building. After a pass, 0x17898 branches back to 0x16790,
which resets the full five-second request. Thus the requested interval is
five seconds between passes, plus document/file work; it is not a five-second
audio/settings synchronization boundary or proof of an exact wall-clock rate.

`tools/verify_autosave_bytes.ps1 -SettingsWorker` verifies the worker hash,
resolves nanosleep and __errno_location through original PLT/GOT metadata,
checks the {5,0} file-backed literal and verifies six matching root/modes key
literals. It passes as PASS_STATIC_ELF_DATA_ONLY. The audio-worker variant
also still passes. Neither executes imported services or original instructions.

## Independent slots and tracker

The worker's slot byte is initialized to ASCII '1' at 0x16700–0x16704.
At the ordinary end of each pass, 0x1785c–0x17874 switches '1' to '2', otherwise
to '1'. Its tracker read path uses istream::get(char&) at 0x179c4–0x179cc,
as independently resolved by the data checker. Slot-file serialization,
tracker write results, startup selection and exception routing require further
decomposition; no crash-safe publication claim follows from this alternation.

The audio worker separately alternates its own slots while reading live tape
extent and yielding between individual writes (AUTOSAVE_AUDIO_STATIC_CONTRACT.md).
This settings pass has its own sleep, document construction and slot byte.
The examined consumer paths do not contain a shared audio/settings generation
identifier or per-pass barrier. The startup gate alone cannot establish
matching capture instants or synchronized slot numbers. Recover the producer
and both tracker reader/writer sequences before claiming what survives a
power interruption.

## Consequence for the native persistence specification

The Rack design should capture an explicitly identified generation containing
both decks' audio extents and settings, and publish a verified manifest naming
all of its assets. Five-second periodic persistence can be an optional policy,
but its completion must identify the captured generation and must not perform
filesystem work in the audio callback. Snapshot consistency, memory ownership
and maximum work per callback need concrete architecture decisions.

Acceptance histories must change blank/length/modes during settings encoding,
grow or shrink tape during an audio pass, interrupt either writer between data
and tracker publication, restore differing audio/settings slot selections, and
reject invalid lengths before activation. These are future gates, not passing
tests. Main publication, all remaining serialized fields, producer cadence and
complete restore behavior remain open.
