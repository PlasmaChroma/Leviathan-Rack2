# Transport recovery and provenance

## Exact waveform format

The input is 16-bit signed mono PCM at 40 kHz. Its only values are zero and ±32767. The first 40,000 samples are zero. After that, the signal alternates polarity at every run boundary.

| Run length | Duration | Meaning | Count |
|---|---:|---|---:|
| 3 samples | 75 µs | Data bit 1 | 295,565 |
| 10 samples | 250 µs | Data bit 0 | 521,267 |
| 350 samples | 8.75 ms | Pause / synchronization half-cycle | 1,394 |

The data bits are assembled MSB-first into bytes. There are 816,832 data bits and no partial final byte. Repeated equal bits would form square waves at approximately 6.667 kHz or 2 kHz, but the decoding rule is **half-cycle duration**. Describing this simply as conventional fixed-baud UART FSK would lose the actual format.

There are 115 pause half-cycles after the one-second leading silence. The first data half-cycle begins at sample 80,250. The payload appears in 583 contiguous data bursts: 579 bursts of 1,408 bits, plus one each of 704, 352, 448, and 96 bits. A normal 1,408-bit burst contains 176 ASCII characters, corresponding to four ordinary 16-byte Intel HEX records with their line endings.

No encryption or compression layer is needed to recover this payload. No transport CRC32 is present in the decoded representation. Each Intel HEX record has its own additive checksum. The lack of another visible wrapper does not establish the missing bootloader's complete authentication or programming policy.

## Integrity checks

`tools/recover_wav.py` verifies sample format, amplitude, polarity alternation, allowed run lengths, complete bytes, ASCII decoding, record lengths, record checksums, supported record types, EOF, and absence of conflicting duplicate memory bytes.

It then reconstructs all data-run lengths **from the recovered ASCII bytes**, inserts pauses from `transport/timing.json`, restores the alternating signal and leading silence, and compares every PCM byte. This is stronger than recognizing a few plausible firmware strings.

| Object | SHA-256 |
|---|---|
| Original WAV | `dfea58ec1694b63f645dc85af62d0971077942b281ae445c04ebaa2bebcbd09c` |
| Recovered ASCII HEX | `fee378dcb92fa2f85bf4ef8bdb35f12b61920612576f9aca302ab46bcf00a5c8` |
| Main application segment | `bb758bd099e200cf1f806b1ba274aa47f2375f7874422a66ccb55d22418daf04` |
| High constant segment | `7bb18a582c1e84a28b8ad45f4e9f65068762b6c88672a72e88fcced18fee2410` |

All other digests are in `SHA256SUMS.txt` and `analysis/recovery.json`.

The first transmitted record is:

```text
:10080000DEEF08F0FF00FF004E82E1CF08F0E2CFFC
```

Its first four data bytes decode as a PIC18 GOTO to byte address `0x11BC`. Subsequent vector, runtime, and peripheral behavior strongly corroborate that interpretation.

## File semantics

The four recovered `.bin` segments contain only transmitted bytes. `tempi71_flash64k_FF_FILLED.bin` is a convenience mapping of the program address space; its mask contains `FF` at recovered positions and `00` elsewhere. A filled `FF` and a recovered `FF` are different evidence states.

The two files beginning `SYNTHETIC_` are derived by running a recovered initialization routine. They are not transport extractions and must not be confused with the raw segments. Their written-byte mask distinguishes code-defined writes from the initial fill.

`transport/record_sample_map.csv` connects each record to its first sample and time. `data_bursts.csv` indexes contiguous data intervals. `timing.json` retains the information needed for exact PCM reconstruction. These maps make an independent decoder comparison possible without searching through the entire waveform manually.

## Reproduction limits

The decoder intentionally rejects resampled or noisy data. It does not estimate zero crossings, compensate for DC offset, correct bit errors, or model an analog input comparator. A robust decoder for a recording played through an audio interface would be a separate task.

The official product page links an update called Tempi 71, but the vendor ZIP was not independently downloaded and compared byte-for-byte during this run. Provenance in this package is anchored to the **user-supplied WAV and its hash**, not an unperformed vendor-file comparison.
