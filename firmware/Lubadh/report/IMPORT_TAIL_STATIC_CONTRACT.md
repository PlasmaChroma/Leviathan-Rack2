# Imported audio tail and published length

Static evidence from `audio_load`, SHA-256
`2dd4523f1c846c14952b301aaacb509ed13125fbbd2e0e49c8a1634a1139301a`.
The archived executable was read and hashed, not launched. No new original-byte
execution or import fixture passes are claimed here. Main-application completion
is separately described in `FILE_LOAD_COMPLETION_STATIC.md`.

## Data loading and selected-deck extent

`loadFile` at 0x150b4 first clears the entire destination shared audio array:
0x15110–0x15120 passes zero and byte count 0x0708a8c0 (118008000), or
29,502,000 float32 cells, to the memory-clear import. The successful read path
passes that same byte capacity to its file read at 0x15184–0x15194, then returns
the read-length field shifted right two bits (0x15198–0x151a8). File open/read
imports, short-read interpretation and exception paths need execution coverage;
these instructions alone do not prove a fully validated float file format.

In the worker main loop, selected deck IDs are iterated through loadFile at
0x13484–0x1349c. r7 accumulates the unsigned maximum returned sample length,
starting at zero. Tail handling uses this common N for every selected deck,
rather than appending immediately after each deck's individual read extent.
Because loadFile clears each array first, a shorter selected deck has zeros
between its read extent and common N. How this interacts with mono duplication,
unequal raw files or failed conversion needs actual marked stereo fixtures.

The standalone addTail function at 0x14a10 is not called by a direct BL in the
archived text. Main contains equivalent policy branches inline. The two forms
must both remain identified; an isolated addTail probe alone will not establish
the production main-loop length/status publication.

## Append law and sample ownership

Let K=29,502,000, T=12,292 and E=min(N+T,K), for ordinary N within capacity
without 32-bit addition overflow. addTail computes this at 0x14a18–0x14a2c;
main computes it at 0x13630–0x13648. Beyond that stated domain, ARM unsigned
addition can wrap before the comparison; it is not a saturating addition.

| TailAdd integer | Standalone function | Production inline branch |
|---:|---|---|
| 0 | Return N; no writes (0x14a40–0x14a48) | Return/accumulate N for this deck (0x13710–0x13714) |
| 1 | Zero cells [N,E), return E (0x14a50–0x14a74) | Zero cells [N,E), use E (0x13868–0x13894) |
| 2 | Forward-copy source beginning into [N,E), return E (0x14a78–0x14b40) | Forward-copy source beginning into [N,E), use E (0x13718–0x137e8, scalar alternate 0x139f4) |
| Other | Return computed E without initialization | Inline main uses N, not E |

For copy mode, source and destination refer to the same array. This is a
forward in-place copy, not a snapshot copy from a separate immutable payload.
For nonzero N smaller than the appended count, scalar iteration can read cells
already written earlier in this append. The vector path reads four words before
writing four words and is gated away from the smallest overlapping offsets.
Short-file fixtures must independently compare both paths and remainders; do
not replace the observed operation with an unchecked overlapping memcpy.
For N=0, source and destination coincide and there is no original payload to
repeat; prior array clearing matters. Zero-length files still need an explicit
native policy and a check against main completion's length subtraction.

Policy 0 preserves the file extent. The main application subsequently subtracts
T from the published extent to establish C+76. Thus a file whose ending already
contains its intended tail can consume that ending as tail without adding new
samples. Policies 1/2 normally add T, so C+76 becomes N when E=N+T. At capacity,
E can be smaller than N+T; C+76 then becomes E-T, which can be shorter than the
original imported N. This capacity behavior is a recovered static consequence,
not proof of how the manual labels each filename tag.

## Filename selection and completion order

Main scans the selected filename against three string descriptors beginning at
0x36474, iterating descriptors at stride 24 (0x13540–0x135f4). Global setup
constructs them in order at 0x140d8–0x14104 from strings at
0x22f78/0x22f7c/0x22f80. Archived ELF extraction verifies the twelve bytes
`7b 6e 7d 00 7b 73 7d 00 7b 63 7d 00`: `{n}`, `{s}`, `{c}`.
The four-character string-dump threshold omitted these three-character tags;
their absence from audio_load_strings.json was not absence from the ELF.

| Scan index / policy | Tag | Effect |
|---:|---|---|
| 0 | `{n}` | No append; ending of file contributes to the published tail extent |
| 1 | `{s}` | Append silence |
| 2 | `{c}` | Forward-copy beginning into appended extent |

The search compares filename bytes directly (0x135bc–0x135c8); there is no
case-folding in this slice. Matching is substring search, not a required suffix
or extension. The first matching descriptor exits the scan through 0x13960 and
returns to 0x13610 at 0x139f0 with its scan index still in r10. Thus `{n}` wins
over `{s}` and `{c}`, even if it occurs later in the filename; `{s}` wins over
`{c}`. A filename containing uppercase `{C}` alone does not match `{c}`.

On no match, after all three descriptors, 0x135f8–0x1360c restores the selected
folder index from stack+24 and sets r10 to (folderIndex!=0). Consequently folder
index 0 defaults to no append, while any nonzero index defaults to silence.
The earlier index comes from the selected AudioFileLocation at
0x134a0–0x134bc/0x13524–0x13538. `mapFiles` inserts the `saved/` folder first,
then appends folders from a listing which excludes that name. Thus index 0
is the mapper's saved-recording folder on this normal construction path.
See `FILE_BROWSER_STATIC_CONTRACT.md` for exact insertion, ordering and limits;
real directory fixtures and failure histories remain unexecuted.

`tools/verify_import_tag_bytes.ps1` verifies the worker hash and file-backed
literal bytes and checks nine independent string-policy examples, including
reversed tag order, uppercase, no-tag folder defaults and embedded tags.
Its evidence artifact `evidence/import_tag_byte_checks.json` is explicitly
`PASS_DATA_AND_INDEPENDENT_POLICY_ONLY`. Those examples check the static-derived
model, not original ARM search execution or actual file import histories.

After applying the policy to each selected deck, r6 accumulates the resulting
maximum (0x137ec–0x137f8). Main writes r6 through the shared length pointer
at 0x13814, then iterates selected status pointers and stores 6
(0x13818–0x13830). This connects the tail result to the main application's
status-6 completion consumer statically. No memory-ordering barrier or complete
interprocess synchronization contract has been demonstrated by this slice.

The worker clear/read/append order and main application's completion stores are
now traceable as separate layers. The nominal file rate 49148 in soximport
does not turn this sample count into a verified physical timing law.

## Native requirements and closure fixtures

The candidate Rack import model needs explicit tail policy, original payload
extent, padded extent, logical period and capacity. Validate these on the worker
before replacement; preserve the old tape on failure. Capacity handling must
state whether it truncates the payload to reserve T or reduces the available
tail. Both are design choices requiring acceptance tests, not silent equivalence
to the firmware's capped E minus T publication.

Required fixtures include N=0..9, T-1/T/T+1, K-T-1/K-T/K-T+1, K-1/K,
all three policies, unmatched/overlapping and uppercase filename tags, folder
index 0/nonzero defaults, equal/unequal stereo
lengths, marked beginning/end samples, short reads and conversion failures.
Check untouched cells, scalar/vector/remainder copying, the shared length,
status sequence, main completion extents and actual playback seams. Native
acceptance must explicitly reject or define empty payloads, malformed byte
lengths, nonfinite samples and conflicting/stale asset results.
