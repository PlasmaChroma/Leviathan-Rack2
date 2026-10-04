# Spectraphon Array persistence and WAV interchange

**Continuation:** [RACK_RECONSTRUCTION.md](RACK_RECONSTRUCTION.md), section 9, defines the instruction-tested capture writer and integrated capture/read sequence. The continuation also checks complete save/header generation, save-to-load comparisons and write faults through explicit in-memory file-operation substitutions, alongside numerical and parser/loader boundaries. Capture-to-SAO gestures reach the reader through explicit persistence failure paths. Actual FatFs/media success and real module-generated WAV compatibility remain unverified.

## 1. Three different representations

Keep these distinct:

| Layer | Representation |
|---|---|
| Live/working spectrum | 64 float32 values in internal RAM |
| Stored Array in external RAM | Multiple 64-float spectra, with a separate four-word descriptor |
| Saved Array file | RIFF/WAVE container; the observed save path emits 16-bit mono coefficient samples |

A spectrum is not an FFT complex vector and does not store a phase value alongside each magnitude. The observed normal Array reader consumes one real float per coefficient. File “samples” are serialized coefficient values, not the original incoming audio samples.

Evidence: readers at `0x0802cd0c`, `0x0802ce5c`, `0x0802cfac`, `0x0802d0f8`; converter at `0x08033434`; saver at `0x08033af8`.

## 2. Slot descriptors and addresses

A four-word descriptor has the observed layout:

```text
offset +0:  float index offset relative to the side's external-memory base
offset +4:  dimension 1
offset +8:  dimension 2
offset +12: save-state marker; zero enters the save path, nonzero skips it
```

The last field must not be simplistically called a “present” flag. In the save routine it behaves like dirty/unsaved state. The continuation now checks all 16 first-unsaved slots on each side: the scanner skips nonzero markers and changes a zero marker to **slot index + 1 before calling file open**. Thus a nonzero marker alone cannot establish successful persistence. The exact lifecycle across every UI transition remains incomplete.

A-side descriptor base is `0x20000a20`; B-side descriptor base is `0x20000920`. Each contains 16 entries. Main's initializer uses 65,536-float spacing per slot. A offsets start at zero; B offsets include 1,048,576 floats, but relative to a separate B memory base.

Original-instruction execution now checks all 32 initial descriptors through
`0x0803418c–0x08034426`: A offset `slot*65536`, B offset
`1048576+slot*65536`, dimensions 8-by-8 and marker 1. The slice stops before
the next GPIO call; it does not run the complete boot sequence.

```text
A absolute spectrum address = *(0x20002f74) + 4*descriptorA.offset
B absolute spectrum address = *(0x20002f70) + 4*descriptorB.offset

A base pointer value = 0x60c01000
B base pointer value = 0x60001000
```

At initialization, the default coefficient block is copied from `0x08045a98` and dimensions are set to 8 × 8. This source block contains 4,096 floats: 64 frames of 64 coefficients.

Capacity arithmetic:

```text
1 RAM frame       = 64 * 4 = 256 bytes
1 maximum slot    = 1024 * 256 = 262144 bytes
1 PCM16 frame     = 64 * 2 = 128 bytes
maximum WAV data  = 1024 * 128 = 131072 bytes
factory WAV data  = 64 * 128 = 8192 bytes
```

The file adds RIFF headers/chunks to those payload sizes. Reconstruction section 9 now defines the checked capture cadence: policy 0 writes one 64-coefficient frame per eligible 64-sample callback, while policy 1 writes only at countdown 1. At the nominal 48 kHz rate, 1024 consecutive free-capture writes span about 1.365 seconds. UI/event alignment, disabled-audio stale captures and clock gating affect what is actually recorded; WAV duration is not capture duration.

For Rack playback, retain a full 1024-frame allocation per slot even when its
logical Array is shorter. The planar reader can use retained frames beyond the
logical length; 192 original-reader comparisons now check exact deltas against
two different tail histories. The bounded `planar_slot_deltas()` helper and
the physical-index proof are in `RACK_RECONSTRUCTION.md`, section 10. Capture
start/stop can also preserve a scan offset larger than the new length: 48 joined
sequences verify that transition, with 16 subsequent reads crossing the slot
boundary. `planar_storage_deltas()` supplies the required contiguous bank view;
neighboring slots and trailing guard memory must remain available. Section 10
defines the expanded allocation bound and its limitations. Logical
prefix-only WAV export cannot preserve that tail history; exact patch-state
restoration needs separate storage for it. Factory replacement and shorter
captures/loads should preserve any unwritten tail.

## 3. Names and discovery

Embedded wildcard strings are:

```text
spect*.wav
speca*.wav
specb*.wav
```

The observed per-side save names follow `speca000.wav` through `speca015.wav`, and the B equivalent. The enumeration path and a filename character walk fit a short-filename FatFs configuration. Exact mount options, all case handling and the meaning of every generic `spect*` path remain incompletely identified.

The inferred FatFs-like routines are labeled in the curated symbol CSV. Their names are analyst assignments, not symbols extracted from a linked library.

## 4. Saved WAV structure and the header anomaly

The save path selects internal sample-format code `0x73693136`, follows the signed-16 conversion path, and writes 128 payload bytes for every spectrum. The header builder/finalizer declares mono audio with nominal sample rate 48,000 and 16-bit samples.

The significant apparent inconsistency is:

| Header field | PCM16 save-path value inferred from instructions | Standard mono PCM16 value |
|---|---:|---:|
| Format tag | 1 | 1 |
| Channels | 1 | 1 |
| Sample rate | 48000 | 48000 |
| Byte rate | **192000** | **96000** |
| Block align | **4** | **2** |
| Bits per sample | 16 | 16 |

The common finalization sequence around `0x080338f4–0x0803390a` writes 192,000 and 4; the PCM16 branch around `0x08033998` selects 16 bits. The sequence then writes the header. The static translation therefore points to stale 32-bit byte-rate/alignment fields, not a 32-bit coefficient payload.

**Validation status:** no hardware-saved WAV was supplied. Fifteen original-instruction cases now confirm generated format bytes for PCM8/16/24/32 and float32 with input `fmt ` chunk lengths 16, 18 and 40. All five formats receive byte rate 192000 and block alignment 4; only PCM32 and float32 match standard mono fields. The format tag is 1 for integer PCM and 3 for float. The routine first requests a 22-byte chunk-header/format write, then a separate two-byte bit-depth write at chunk start +22. These tests stop before file calls and supply separate post-seek entry conditions; they prove field construction and write arguments, not a physical output file. A parser should report the inconsistency and use format/bit-depth to validate payload structure. A standards-correct writer should not reproduce the quirk by accident.

The optional `firmware_header_quirk=True` parameter in the reference encoder is deliberately explicit and experimental. It is not needed for ordinary coefficient inspection and is not a guarantee of hardware compatibility.

## 5. Quantization and save-time mutation

A per-side peak-like scalar controls conditional normalization:

```text
if 0 < trackedPeak < 1:
    gain = 1 / trackedPeak
else:
    gain = 1
```

Each coefficient is multiplied by this gain; the scaled float is written back to the Array RAM. Original-instruction tests now establish float32 rounding of the gain and product, followed by:

```text
integerSampleBits = trunc(double(scaledCoefficient) * 32767.0) & 0xffff
```

The continuation checks 28 frames across both sides and seven peak values, including non-power-of-two gain and coefficients outside the int16 range after scaling. The stored halfword wraps rather than saturates. Invalid/nonfinite or int32-overflowing conversions remain outside the test contract. The peak is a per-side scalar reused for the selected save slot, not a recomputed maximum. `array_save_frame()` in `sp67_extended.py` returns the replacement RAM floats and exact stored bytes for this bounded contract. The safe interchange writer **rejects** out-of-int16 values and preserves caller data.

All 28 generated frames are checked against the original signed-16 read conversion, entered after an explicitly supplied read buffer. It uses a factor of `1/32768`, not `1/32767`. For values whose saved integer fits int16, ignoring gain and float rounding, the round trip is approximately:

```text
loaded = trunc(savedFloat * 32767) / 32768
```

This is a real numerical distinction. The safe tool's encoder does not mutate the caller's data, while the firmware does update its stored floats. The API documentation makes that divergence explicit.

The tests stop before media calls and resume separate numerical slices with explicit inputs. They establish RAM mutation, PCM conversion and readback arithmetic, not a successful physical save/load operation. Playback from live RAM after saving retains the scaled floats; a later reload introduces the PCM quantization/scaling difference.

Some factory coefficients are larger than one. Do not destroy them during extraction merely to make a generic audio encoder happy. Raw float32 exports are the lossless representation of the extracted bank.

## 6. Input formats: parser recognition is not complete support

The generic WAV machinery recognizes RIFF/WAVE, `fmt `, `data`, and an additional `clm ` identifier, plus chunk walking/padding. It contains sample conversion branches for unsigned PCM8, signed PCM16/24/32 and IEEE float32. Other format tags seen in parsing are not enough to claim that compressed formats work as spectral Arrays.

### Instruction-checked parser boundaries

Four differential groups now check 576 cases inside parser `0x08033094`.
These start at explicit post-read or pre-I/O boundaries with supplied buffers,
statuses and pointers. They do not replace file calls, execute the complete
parser against a filesystem, or establish end-to-end malformed-file behavior.

* **72 signature cases:** after the initial read, only exact `RIFF` and `WAVE`
  enter chunk walking. A read error returns its status. With read status zero,
  a wrong signature also returns **zero**, rather than a distinct format error.
  The supplied byte-count field (0, 4 or 12) does not affect this decision when
  the same scratch bytes are supplied. Thus zero alone is not proof of a valid
  header, and stale scratch contents matter after an actual short read.
* **384 format cases:** the sample rate is copied without validation; channel
  count and format tag are read as signed 16-bit values. Channels 1, 2 and
  `0xffff` (stored as -1) produce the same classification. The executable helper
  `wav_format_classification()` describes the dispatch below. Unsupported tags
  return `0xffffffff`, after rate/channel writes but without replacing the
  depth or format code.
* **84 chunk cases:** unknown chunks and `data` advance by `8+size`, then round
  the next absolute position up to even. `data` stores size, payload start and
  the **unpadded** end. A completed `fmt ` advances by `8+size` **without padding**,
  even for an odd length. `clm ` requests the declared payload size into the
  scratch pointer `sp+64`; after the supplied successful read it advances and
  pads like an unknown chunk. This boundary does not cap the read length.
  Tested additions wrap as uint32, including near-end positions and size
  `0xffffffff`; these are arithmetic checks, not executed large file reads.
* **36 termination cases:** continue only while the file object's size is
  greater than the next chunk position and either `fmt ` or `data` has not yet
  been found. Otherwise return zero, including at EOF with one or both missing.
  This boundary uses the file-object size, not the RIFF length field.

| Signed format tag | Stored depth | Internal format code |
|---:|---:|---|
| 1 | Signed PCM depth read separately | 8: `0x75733038`; 16: `0x73693136`; 24: `0x73693234`; 32: `0x73693332` |
| 2 | 4 | `0x6d736164` |
| 3 | 32 | `0x666c3332` |
| 17 | 4 | `0x696d6164` |
| 257 | 8 | `0x6d753038` |
| 258 | 8 | `0x616c3038` |
| 259 | 4 | `0x69626164` |

For PCM, any other depth is still stored and the **previous format code remains
unchanged**; the parser continues. Tag 3 supplies depth 32 without reading the
file's bit-depth field on that branch. Recognition of the other table entries
does not establish corresponding sample-decoder support. Tags 6, 7 and
`0xfffe` are among the explicitly rejected cases. The strict interchange tool
below intentionally does not reproduce these permissive/error-path quirks.

### Bounded interchange and sample conversion

The included tool supports only mono PCM8/16/24/32 and mono IEEE float32. It intentionally rejects ADPCM, mu-law, WAVE_FORMAT_EXTENSIBLE, multiple data chunks, nonfinite float data and oversized/incomplete spectral payloads. This is a bounded inspection/interchange tool, not a claim to emulate every permissive firmware parser behavior.

The tool normalizes integer PCM using standard divisors. The firmware's 24-/32-bit conversion uses a nearby float32 constant (`0x2ffffff6`) and intermediate integer-to-float rounding, so the tool is not an instruction-exact replacement for those conversion branches. PCM16's divisor agrees exactly with the observed 1/32768 scaling.

The continuation's `array_decode_sample_buffer()` implements the instruction
schedule after a file read. A 240-case differential test covers five formats,
sample counts 0, 1, 2, 3, 4, 5, 7, 8, 9, 63, 64 and 65, and all four source
alignment offsets. It checks all output float bits, the reported sample count,
and an untouched destination suffix. Source alignment shifting is executed by
the firmware; the helper takes an already aligned payload.

| Format | Exact checked conversion |
|---|---|
| PCM8 | `(unsignedByte-128)/128` |
| PCM16 | `signed16/32768` |
| PCM24 | `float32(float32(signed24 << 8) * float32_from_bits(0x2ffffff6))` |
| PCM32 | `float32(float32(signed32) * float32_from_bits(0x2ffffff6))` |
| IEEE float32 | Copy the original 32-bit word; finite test values include negative zero |

The PCM24 loop has an additional quirk: it writes all complete samples but its
reported count is **4*floor(sampleCount/4)**. For example, five samples produce
five floats but report four; one to three samples produce floats but report
zero. Ordinary 64-value frames avoid this discrepancy. This is a tested
post-read conversion behavior, not proof of the complete parser or of support
for malformed/truncated WAVs. Nonfinite IEEE values and incomplete sample-byte
tails are outside these differential cases.

### Read request, alignment and short-read behavior

A further 140 cases execute the reader's entry through its pending read call,
then separately execute the post-read instructions through return. Each case
supplies the read status, byte count and scratch contents explicitly. Five
formats, all four payload alignments, successful counts 0/1/3/request-1/request,
and error statuses 1/9 are checked. This joins the reader's own setup and return
paths without claiming successful filesystem execution.

For the loader's request of 64 values, the reader asks for
`64*depth/8 + 4` bytes into `0x20003468`. On status zero, it subtracts four
from the reported byte count **only when the count equals the full request**.
It then shifts that many bytes left from source offset `dataStart & 3`, without
reducing the count for that offset. A short unaligned read can consequently
include old scratch bytes beyond the returned data. Status 1 or 9 returns zero
and leaves the conversion destination untouched in the checked cases.

The extra four bytes also create an overrun of the nominal 64-value output
when a successful read returns exactly one byte less than requested:

| Format | Requested bytes | Short return | Floats written | Count returned |
|---|---:|---:|---:|---:|
| PCM8 | 68 | 67 | 67 | 67 |
| PCM16 | 132 | 131 | 65 | 65 |
| PCM24 | 196 | 195 | 65 | 64 |
| PCM32 / float32 | 260 | 259 | 64 | 64 |

These are exact output comparisons with explicit scratch bytes, including the
untouched suffix after the actual write extent. They do not characterize all
possible file failures. A portable importer should bound reads and conversion
output by the intended payload/frame, and reject incomplete frames rather than
copying stale values or reproducing these overlong writes.

## 7. How file length becomes Array dimensions

Main's load path uses a special case for computed sampleCount 4,096 (scalar
values for mono; see the channel divisor below):

```text
if sampleCount == 4096:
    dim1 = 8
    dim2 = 8
else:
    dim1 = sampleCount >> 6
    dim2 = 1
```

This is visible around `0x08034fd6–0x080350c6`. The special case matches the 64-frame factory bank. No extra custom dimensional header is necessary to explain normal files in this path. The generic parser's handling of other chunks does not establish that every possible metadata field is ignored elsewhere.

Before that assignment, 864 cases now check all three loader paths' count and
initial seek arithmetic. For ordinary nonnegative sizes, positive dimensions
and a nonzero divisor, `array_load_count()` implements:

```text
bytesPerAudioFrame = floor(depth * channels / 8)
sampleCount = floor(dataChunkBytes / bytesPerAudioFrame)
initialSeek = dataStart & ~3
```

The tested depth values are 8/16/24/32, channels 1/2/3, payload lengths
0/127/128/129/8192/8193 and data starts 44..47. The instructions use signed
32-bit multiply/divide; overflow, negative fields and zero divisors are outside
this helper's contract. Neither sample rate nor the WAV byte-rate/alignment
fields participate in this arithmetic.

Despite the channel divisor, subsequent reader requests convert consecutive
**scalar samples**, without downmixing or deinterleaving. Three joined A-loader
sequences check mono, stereo and three-channel PCM16 with 128 audio frames.
Each computes count 128, assigns dimensions 2-by-1, and copies exactly the first
128 scalar values in two 64-value iterations. Stereo therefore retains the
first 64 interleaved L/R frames; three-channel retains the first 128 interleaved
values. These tests execute descriptor assignment, reader setup/conversion,
real memory copies and loop termination, supplying explicit seek/read results
at each I/O boundary. They stop before file close. Multichannel import is thus
not a conventional audio-channel conversion in this checked loader path.

An importer should validate that the payload contains a whole number of 64-value frames and at most 1,024 frames. It should not infer stereo Arrays simply because a generic WAV library can open stereo files. The tool requires one channel; the checked firmware multichannel path truncates scalar data rather than defining separate channel spectra.

The continuation checks the A-only, B-only and shared A/B load paths separately.
Twenty-four admission cases execute their post-seek count comparisons: a signed
count **>64** proceeds, while -1, 0, 1, 63 and 64 do not. Thus the strict
interchange tool's acceptance of a single complete 64-value frame is broader
than this firmware admission branch. The local comparison also admits 65537;
it supplies neither an upper bound nor a whole-frame check. This does not rule
out restrictions in upstream parsing, which these slices do not execute.

Seventy-two assignment cases cover eight counts across each of A, B and the
shared path, with different selected slots. Only exactly 4096 yields 8×8;
4095 gives 63×1 and 4097 gives 64×1. Descriptor offsets are preserved and
markers become 1 before payload loading. A/B paths compute their external-RAM
destination; the shared path records both word offsets and assigns both
descriptors. These checks stop before the next file seek.

Eighteen further cases enter immediately after the sample converter with return
counts 0, 1, 3, 4, 63 and 64. Each loader copies **256 bytes** from the scratch
buffer per destination and advances its memory cursor by 64 floats, while the
logical progress counter advances only by the returned count. Both sides of
the shared path receive the same scratch frame. A short or failed conversion
can therefore copy stale scratch values at this boundary. The test executes
the original copy routine and checks the untouched destination suffix; it
does not fabricate a successful file read or run a complete failing-media loop.

Rack import should explicitly reject incomplete frames, over-capacity payloads
and failed/short reads. Keep literal numeric conversion available separately
from these validation rules; unchecked memory access is not a portable import
contract.

## 8. Using the included tool

The connected continuation now executes the complete original WAV wrapper/parser
for 405 normal and 14 edge files, plus error injection at each file-operation
boundary. Another 120 sequences connect parsing to A/B Array loading across five
formats, channel counts 1/2/3, two payload alignments and slots 0/15. All comparisons
pass under an **explicit in-memory read/seek substitution**; original FatFs,
open/close and physical media are not executed in these tests. The substitution
and excluded instruction coverage are documented in
`RACK_RECONSTRUCTION.md` under “Complete file parsing and connected Array import.”

One connected short-header case makes the stale-scratch issue concrete: a
normal 16-byte fmt followed only by the four bytes `data` reuses the previous
chunk size 16 and returns start=44/end=60 despite EOF at byte 40. Missing chunks
can still return zero, duplicates replace prior metadata until both required
chunks are found, and advertised sample bytes need not exist. These results
reinforce the strict validation in the included inspection tool.

Run from the bundle root:

```bash
python reference/sp67_reference.py inspect-wav /path/to/speca000.wav
python reference/sp67_reference.py inspect-wav /path/to/speca000.wav --frames-json frames.json
python reference/sp67_reference.py extract-factory factory_frames.json
python tests/test_reference.py
```

The factory extraction command writes the coefficients already recovered from this firmware; it does not synthesize or guess their values. The inspect command reports format fields, inferred dimensions and header inconsistencies.

For programmatic use:

```python
from pathlib import Path
from reference.sp67_reference import decode_array_wav, encode_pcm16

info, frames = decode_array_wav(Path('speca000.wav').read_bytes())
print(info.frame_count, info.inferred_dimensions, info.warnings)
# Experimental output; not validated on a physical module.
encoded = encode_pcm16(frames)
Path('array_standard_header.wav').write_bytes(encoded)
```

Encoding can fail when coefficients exceed the representable int16 range. That failure is intentional: inspect or explicitly normalize the data rather than silently clipping it. Do not place experimental files on the only copy of a working SD card.

## 9. Independent checks that would most reduce uncertainty

The first comparison should use an untouched hardware-saved Array file, not a DAW-exported copy. Check the six format fields, actual data length, coefficient count and name. Compare the input file before and after a module load/save cycle to detect normalization or scaling.

For capture timing, use a controlled input tone and record a small Array at a known external clock interval. That can establish whether each capture tick stores one spectrum, how start/end events are handled, and whether the final frame count differs from the simple number of ticks. Those timing semantics should not be invented from the file format alone.
