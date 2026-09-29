# Memory layout, persistence, and factory reconstruction

## State format

A state is six signed ratio bytes, six phase bytes, one enable mask, and one MOD mask. Four groups of sixteen states are addressed as a flat state number `0..63`. Bits 0–5 of each mask correspond to the six channels. The factory enable mask is `FF`, not merely `3F`; the upper two bits exist in storage even though the channel loader only consumes the six relevant bits.

| Field | EEPROM address | RAM cache address |
|---|---|---|
| Ratio, channel c | `0x000 + 64*c + s` | `0xC2C + 6*s + c` |
| Phase, channel c | `0x180 + 64*c + s` | `0xDAC + 6*s + c` |
| Output-enable mask | `0x300 + s` | `0x840 + s` |
| MOD mask | `0x340 + s` | `0x800 + s` |

The active state loader at `0x8540` sign-extends ratios into RAM `0x538 + 2*c`, copies phase bytes to `0x570+c`, expands enable bits into `0x4E0+c`, and expands MOD bits into `0x5DC+c`. It marks timing dirty so the foreground arithmetic can rebuild channel parameters.

The 896-byte state payload is exactly `64 * 14`. EEPROM globals occupy the remainder of the 1 KB address space. The byte driver masks addresses with `0x3FF`; this is direct evidence for the EEPROM size assumed by the code.

## Persistence routines

| Entry, program byte address | Role |
|---|---|
| `0x972A` | Read one EEPROM byte |
| `0x9576` | Write one EEPROM byte |
| `0x9710` | Base-plus-offset EEPROM write helper |
| `0x87FA` | Load one state's EEPROM planes into RAM caches |
| `0x976A` | Load all 64 states, then activate selected state |
| `0x8540` | Activate a cached state |
| `0x8B16` | Test/set/clear dirty-state bitmap |
| `0x871A` | Store dirty states and associated persistent settings |
| `0x964A` | Reload/revert dirty state data and current settings |
| `0x88D4` | Select Bus copy/store dispatcher |
| `0x6E56` | Factory initialization |
| `0x8DC2` | Store global settings |
| `0x9678` | Store the leading-tempo value |

The dirty bitmap occupies RAM `0x6A0–0x6A7`. It permits an edited performance state to differ from its saved state without repeatedly writing EEPROM.

The EEPROM write routine disables global interrupts, performs the `55/AA` unlock sequence, starts the write, and waits for the hardware write bit to clear before restoring the previous interrupt-enable state. This is an observed code path. A clock disturbance during a real save is a plausible consequence, but no physical timing trace was taken and the harness completes writes instantly.

## Partial global map

A machine-readable partial map is supplied in `analysis/global_eeprom_partial_map.csv`. The most firmly anchored fields are the saved state index at `0x3FD`, follow flag at `0x3F5`, and three-byte leading-tempo value at `0x3F6–0x3F8`. Other fields relate to MOD mode, raw-versus-toggled MOD source selection, human resolution, and clock/tap configuration.

Addresses `0x3E3–0x3E9` participate in calibration-mode/endpoint logic; they are not supplied as a per-unit EEPROM image by this WAV. The factory routine's noninteractive path does not manufacture valid physical calibration readings.

An initially surprising write to `0x3F0` is preserved as observed. It comes from a base-plus-offset calculation and must not be silently relabeled as `0x3B0`. The partial map leaves uncertain semantics explicitly unresolved.

## Factory preset extraction

Flash `0xFD4D–0xFDAC` holds 16 × 6 phase bytes. Flash `0xFDAD–0xFE0C` holds 16 × 6 signed ratio bytes. The factory routine clears all 64 states, gives them enabled outputs and no MOD assignments, then overwrites the first sixteen with these tables.

| Bank A slot, 1-based | Recovered ratios, channels 1–6 |
|---|---|
| 1 | Unity on all channels |
| 2 | ÷1, ÷2, ÷4, ÷8, ÷16, ÷32 |
| 3 | ÷2, ÷3, ÷5, ÷7, ÷11, ÷13 |
| 4 | ÷1, ÷2, ÷3, ÷4, ÷5, ÷6 |
| 5 | ÷2, ÷4, ÷6, ÷8, ÷10, ÷12 |
| 6 | ÷3, ÷5, ÷7, ÷9, ÷11, ÷13 |
| 7 | ÷2, ÷3, ÷5, ÷8, ÷13, ÷21 |
| 8 | ×1, ×2, ×4, ×8, ×16, ×32 |
| 9 | ×2, ×3, ×5, ×7, ×11, ×13 |
| 10 | ×1, ×2, ×3, ×4, ×5, ×6 |
| 11 | ×2, ×4, ×6, ×8, ×10, ×12 |
| 12 | ×3, ×5, ×7, ×9, ×11, ×13 |
| 13 | ×2, ×3, ×5, ×8, ×13, ×21 |
| 14 | ×2, ×3, ×4, ÷2, ÷3, ÷4 |
| 15 | ÷1.5 on all channels; phase codes 0,1,2,3,4,5 |
| 16 | ÷2, ÷2.25, ÷2.5, ÷2.75, ÷3, ÷3.25 |

These numbers are derived from the extracted bytes and ratio routine, not copied as a substitute for binary analysis. Slot 15 is particularly useful: the byte tables establish both its exact noninteger ratio and its six explicit phase codes.

## Synthetic EEPROM boundaries

`SYNTHETIC_factory_eeprom.bin` starts with 1,024 bytes of `FF`, then applies only writes made by `0x6E56` with argument zero. That suppresses the interactive calibration branch. The write trace contains 1,101 operations and covers 908 unique addresses: all 896 state bytes plus 12 distinct global bytes.

Its companion mask contains `FF` for a byte written by that routine and `00` for a byte not established by this reconstruction. A mask-zero value is not known to be the real factory or current value of a physical module. This image is suitable for studying defaults, not for replacing a unit's EEPROM.

## RAM and constants

`ram_symbols.csv` lists the named arrays and fields with evidence addresses. `SIMULATED_ram_after_c_runtime.bin` is the harness's runtime-initialized RAM snapshot, not a JTAG/ICSP memory capture. It includes working-memory/SFR space in a simplified model.

The runtime copies a 16-entry integer table from flash `0x9442` to RAM `0x9AE`. Its values are:

```text
200000 180000 130000 80000 70000 40000 12000 10000
8500   6000   5000   3500  2500  1750  1000  250
```

The table is linked to the tempo-control path. Its exact interpolation and end-to-end voltage law are not presented as fully reconstructed here. Exposing the constants is useful without assigning an unsupported volts-to-BPM formula.
