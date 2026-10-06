# Persisted transport and Time state at startup

Static continuation of Application::readAutosave, main ELF SHA-256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
No original ARM/Hjson execution is claimed. The audio reader is described in
AUTOSAVE_RESTORE_AUDIO_STATIC.md. These settings slices establish specific
producer/consumer joins; they are not the complete persistence schema.

## Keys and temporary values

Hjson::Value::at(char const*) is 0x609bc, to_int64() is 0x60d9c and
to_double() is 0x60c78, as identified by archived symbols. Integer stores below
retain r0's low word unless noted; conversion failures and bounds are not
established by their absence from this short sequence.

| Group / key | Temporary stack location | Conversion / evidence |
|---|---|---|
| root `blank` | +448 byte | Either word of int64 nonzero -> true; 0x2b7dc–0x2b7fc |
| root `length` | +452 word | int64 low word; 0x2b800–0x2b814 |
| `modes.monitoring` | +456 word | int64 low word; 0x2b838–0x2b84c |
| `modes.playLoop` | +460 word | int64 low word; 0x2b850–0x2b864 |
| `modes.recLoop` | +464 word | int64 low word; 0x2b868–0x2b87c |
| `time.active` | +468 word | int64 low word; 0x2b8a8–0x2b8bc |
| `time.clkDiv` | +472 word | int64 low word; 0x2b8c0–0x2b8d4 |
| `time.feedback` | +476 float | double -> float32; 0x2b8d8–0x2b8f0 |
| `time.xfade` | +480 word | int64 low word; 0x2b8f4–0x2b908 |
| `time.slew` | +484 float | double -> float32; 0x2b90c–0x2b924 |
| `time.tape` | +488 float | double -> float32; 0x2b928–0x2b940 |
| `time.file` | +492 word | int64 low word; 0x2b944–0x2b958 |
| `time.folder` | +496 word | int64 low word; 0x2b95c–0x2b970 |
| `time.quantiseFlag` | +504 byte | Either word of int64 nonzero -> true; 0x2b974–0x2b994 |
| `time.quantiseGrid` | +508 word | int64 low word; 0x2b998–0x2b9ac |

The nested modes/time values are copied from root lookups at 0x2b818–0x2b834
and 0x2b888–0x2b8a4. Seventeen key/group literals are now extracted directly
from file-backed main ELF bytes by verify_restore_tracker_bytes.ps1; all match
the table/group spellings. This proves literal provenance, not Hjson parsing
or that every malformed field is rejected.

## Blank versus nonblank activation and length ownership

At 0x2bf10–0x2bf20 the temporary blank byte is tested after C+645 is set to 1.
Blank follows a log path and joins common mode/Time application at 0x2bf5c.
Nonblank branches to 0x2c064, calls setState(3), then installs its stored
length and derived extents. Existing state reports identify 3 as Playback.
The raw read count does not feed this length: it comes from the settings key.

For settings length L, 0x2c070–0x2c0bc performs:

- `*C+88 = L` through the stored-length pointer;
- `C+76 = L-12292` logical period;
- `C+80 = L-9834` extended reverse destination;
- `C+132 = signed_min(L-12293,12292)` maximum fade span.

Subtraction is ARM word arithmetic and the min is signed. This slice supplies
no lower/capacity bound and does not compare L with the selected audio file's
read count. It matches import-completion extent arithmetic, but follows
persisted metadata. 0x2c07c reads C+260, computes unsigned `(mode-1)<=1` as
the first boolean argument to setLoopingParameters; the second argument is 1.
That call is at 0x2c0c4. These are exact local arguments, not a guarantee of
valid resulting regions for corrupt L.

## Common restored controls

At 0x2bf5c–0x2bf94 monitoring is stored at C+168. The monitoring-derived byte
at C+636 becomes 1 exactly when the stored low word equals 2, with old/new XOR
at C+637. The routine calls setPlaybackLoopMode with the persisted playLoop and
setRecordLoopMode with recLoop. Their internal gate/head side effects require
connected restore fixtures.

At 0x2bf98–0x2bfac the blank byte is tested again. When it is false and C+176
is zero, 0x2c164 calls doRetrig before common Time publication; otherwise that
call is skipped. Retrigger timing, delayed gates and resulting head lists have
not been executed for this restore history.

For the initial channel, the companion pointer in r4 at 0x2bfb0 is C+173252.
The stores at 0x2bfd0–0x2c014 give these joins:

| Persisted field | Destination |
|---|---|
| active | C+173264 |
| clkDiv | C+173272 |
| xfade | C+173236, the cached crossfade Time field |
| file / folder | C+173252 / C+173256 |
| feedback | selected LinkData+164 |
| slew | selected LinkData+172 |
| tape | selected LinkData+168 |
| quantiseFlag / quantiseGrid | selected LinkData+108 byte / +128 word |

C+648 is set to 1 during this publication. force_replace receives the parsed
temporary Preset at 0x2c018, and C+668 is cleared afterward. That call, preset
slot/table parsing, missing-field/default paths and subsequent control updates
must be traced before treating the table as the final settled state. In
particular, persisted `active` is not proof of a restored audio recording gate.

## Remaining specification gates

Test mismatched audio byte count versus L; L below the tail, zero, capacity and
over-capacity; blank with nonempty audio and nonblank with missing audio;
all loop/monitoring modes and preset replacement; first IO update after restore;
and both decks with differing settings/audio slots. Trace default preparation
at 0x2b508 before claiming blank-path behavior on failed settings loads.
Recover the autosave_data producer to confirm exactly when these fields are
captured and whether audio and settings generations align.

For Rack, persist explicitly versioned logical/stored/tail extents and declare
restored transport state. Validate extents against decoded assets before
publishing either deck, and preserve a previous complete generation on error.
These are proposed native improvements; implementation-ready bounds, defaults,
migrations and tests still need to be specified and verified.
