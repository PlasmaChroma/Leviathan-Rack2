# Spectraphon Array persistence and WAV interchange

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

The last field must not be simplistically called a “present” flag. In the save routine it behaves like dirty/unsaved state; the exact lifecycle across every UI transition is not fully reconstructed.

A-side descriptor base is `0x20000a20`; B-side descriptor base is `0x20000920`. Each contains 16 entries. Main's initializer uses 65,536-float spacing per slot. A offsets start at zero; B offsets include 1,048,576 floats, but relative to a separate B memory base.

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

The file adds RIFF headers/chunks to those payload sizes. Storage capacity does not establish capture duration; frame cadence depends on capture/clock state, which is incompletely translated.

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

**Validation status:** no hardware-saved WAV was supplied. This is an instruction-derived finding awaiting file confirmation. A parser should report the inconsistency and use the actual format/bit-depth fields to validate payload structure, rather than blindly trust block alignment. A standards-correct writer should not reproduce the quirk by accident.

The optional `firmware_header_quirk=True` parameter in the reference encoder is deliberately explicit and experimental. It is not needed for ordinary coefficient inspection and is not a guarantee of hardware compatibility.

## 5. Quantization and save-time mutation

A per-side peak-like scalar controls conditional normalization:

```text
if 0 < trackedPeak < 1:
    gain = 1 / trackedPeak
else:
    gain = 1
```

Each coefficient is multiplied by this gain; the scaled float is written back to the Array RAM. The integer save conversion is approximately:

```text
integerSample = trunc(double(scaledCoefficient) * 32767.0)
```

The complete floating conversion/narrowing behavior for invalid or overflowing inputs is not asserted here. No explicit saturating clamp is visible in the traced store sequence. The reference writer **rejects** out-of-int16 values rather than reproducing unchecked narrowing.

The observed signed-16 read conversion uses a factor of `1/32768`, not `1/32767`. Thus, ignoring gain and float rounding, the round trip is approximately:

```text
loaded = trunc(savedFloat * 32767) / 32768
```

This is a real numerical distinction. The safe tool's encoder does not mutate the caller's data, while the firmware does update its stored floats. The API documentation makes that divergence explicit.

Some factory coefficients are larger than one. Do not destroy them during extraction merely to make a generic audio encoder happy. Raw float32 exports are the lossless representation of the extracted bank.

## 6. Input formats: parser recognition is not complete support

The generic WAV machinery recognizes RIFF/WAVE, `fmt `, `data`, and an additional `clm ` identifier, plus chunk walking/padding. It contains sample conversion branches for unsigned PCM8, signed PCM16/24/32 and IEEE float32. Other format tags seen in parsing are not enough to claim that compressed formats work as spectral Arrays.

The included tool supports only mono PCM8/16/24/32 and mono IEEE float32. It intentionally rejects ADPCM, mu-law, WAVE_FORMAT_EXTENSIBLE, multiple data chunks, nonfinite float data and oversized/incomplete spectral payloads. This is a bounded inspection/interchange tool, not a claim to emulate every permissive firmware parser behavior.

The tool normalizes integer PCM using standard divisors. The firmware's 24-/32-bit conversion uses a nearby float32 constant (`0x2ffffff6`) and intermediate integer-to-float rounding, so the tool is not an instruction-exact replacement for those conversion branches. PCM16's divisor agrees exactly with the observed 1/32768 scaling.

## 7. How file length becomes Array dimensions

Main's load path uses a special case for 4,096 scalar values:

```text
if sampleCount == 4096:
    dim1 = 8
    dim2 = 8
else:
    dim1 = sampleCount >> 6
    dim2 = 1
```

This is visible around `0x08034fd6–0x080350c6`. The special case matches the 64-frame factory bank. No extra custom dimensional header is necessary to explain normal files in this path. The generic parser's handling of other chunks does not establish that every possible metadata field is ignored elsewhere.

An importer should validate that the payload contains a whole number of 64-value frames and at most 1,024 frames. It should not infer stereo Arrays simply because a generic WAV library can open stereo files. The tool requires one channel because arbitrary multichannel Array semantics were not established.

## 8. Using the included tool

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
